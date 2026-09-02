/* Copyright (c) 2010, Cedrus Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Cedrus Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Connection.h"

#include "CedrusAssert.h"

#include "constants.h"

Cedrus::Connection::Connection(
    const DWORD location,
    DWORD port_speed,
    BYTE byte_size,
    BYTE bit_parity,
    BYTE stop_bits
)
    : m_BaudRate(port_speed),
    m_ByteSize(byte_size),
    m_BitParity(bit_parity),
    m_StopBits(stop_bits),
    m_Location(location),
    m_ConnectionDead(false),
    m_cmdThroughputLimit(3),
    m_DeviceHandle(nullptr),
    m_ReadTimeout ( 50 ),
    m_WriteTimeout ( 50 )
{
}

Cedrus::Connection::~Connection(void)
{
    if (m_DeviceHandle != nullptr)
        Close();
}

bool Cedrus::Connection::Close()
{
    DWORD close_status = FT_OK;

    // Don't bother if the handle is already null
    if (m_DeviceHandle != nullptr)
    {
        close_status = FT_Close(m_DeviceHandle);
        m_DeviceHandle = nullptr;
    }

    return close_status == FT_OK;
}

bool Cedrus::Connection::FlushWriteToDeviceBuffer()
{
    return (FT_Purge(m_DeviceHandle, FT_PURGE_TX) == FT_OK);
}

bool Cedrus::Connection::FlushReadFromDeviceBuffer()
{
    return (FT_Purge(m_DeviceHandle, FT_PURGE_RX) == FT_OK);
}

int Cedrus::Connection::Open()
{
    int status = XID_NO_ERR;

    // Erring on the side of caution in case we already have a handle.
    Close();

    DWORD open_success = FT_OpenEx((PVOID)m_Location, FT_OPEN_BY_LOCATION, &m_DeviceHandle);

    if (open_success != FT_OK)
    {
        status = XID_PORT_NOT_AVAILABLE;
    }
    else
    {
        m_ConnectionDead = false;
        if (!SetupCOMPort())
            status = XID_ERROR_SETTING_UP_PORT;

        FT_Purge(m_DeviceHandle, FT_PURGE_RX | FT_PURGE_TX);
    }

    m_timestamp = std::chrono::high_resolution_clock::now();

    return status;
}

bool Cedrus::Connection::SetupCOMPort()
{
    if (m_DeviceHandle == nullptr)
        return false;

    if (FT_SetBaudRate(m_DeviceHandle, m_BaudRate) != FT_OK)
        return false;

    if (FT_SetDataCharacteristics(m_DeviceHandle, m_ByteSize, m_StopBits, m_BitParity) != FT_OK)
        return false;

    if (FT_SetTimeouts(m_DeviceHandle, m_ReadTimeout, m_WriteTimeout) != FT_OK)
        return false;

    if (FT_SetUSBParameters(m_DeviceHandle, 64, 64) != FT_OK)
        return false;

    if (FT_SetLatencyTimer(m_DeviceHandle, 10) != FT_OK)
        return false;

    return FlushWriteToDeviceBuffer() && FlushReadFromDeviceBuffer();
}

void Cedrus::Connection::SetReadTimeout(DWORD readTimeout)
{
    m_ReadTimeout = readTimeout;
    FT_SetTimeouts(m_DeviceHandle, readTimeout, m_WriteTimeout );
}

void Cedrus::Connection::SetWriteTimeout ( DWORD writeTimeout )
{
    m_WriteTimeout = writeTimeout;
    FT_SetTimeouts ( m_DeviceHandle, m_ReadTimeout, writeTimeout );
}

DWORD Cedrus::Connection::GetBytesAvailable()
{
    DWORD bytes_in_queue = 0;

    if ( FT_GetQueueStatus ( m_DeviceHandle, &bytes_in_queue ) != FT_OK )
    {
        m_ConnectionDead = true;
        bytes_in_queue = 0;
    }

    return bytes_in_queue;
}

bool Cedrus::Connection::Read (
    unsigned char* inBuffer,
    DWORD bytesToRead,
    LPDWORD bytesRead )
{
    if ( bytesRead == nullptr || inBuffer == nullptr )
        return false;

    *bytesRead = 0;

    if ( m_DeviceHandle == nullptr )
    {
        m_ConnectionDead = true;
        return false;
    }

    const DWORD readStatus = FT_Read ( m_DeviceHandle, inBuffer, bytesToRead, bytesRead );

    if ( readStatus != FT_OK )
    {
        m_ConnectionDead = true;
        return false;
    }

    return true;
}

bool Cedrus::Connection::Write(
    unsigned char * const inBuffer,
    DWORD bytesToWrite,
    LPDWORD bytesWritten,
    bool savesToFlash)
{
    if (bytesWritten == nullptr || inBuffer == nullptr)
        return false;

    if (m_DeviceHandle == nullptr || !FlushWriteToDeviceBuffer() )
    {
        m_ConnectionDead = true;
        return false;
    }

    *bytesWritten = 0;

    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - m_timestamp).count() < m_cmdThroughputLimit)
    {
        SLEEP_FUNC(1 * SLEEP_INC);
    }

    m_timestamp = std::chrono::high_resolution_clock::now();

    for (; *bytesWritten < bytesToWrite; ++(*bytesWritten))
    {
        DWORD byteCount = 0;
        const DWORD writeStatus = FT_Write(
            m_DeviceHandle,
            inBuffer + *bytesWritten,
            1,
            &byteCount);

        if ( writeStatus != FT_OK )
        {
            m_ConnectionDead = true;
            return false;
        }

        if ( byteCount != 1 )
            return false;

        if (*bytesWritten + 1 < bytesToWrite)
            SLEEP_FUNC(1 * SLEEP_INC);
    }

    if (savesToFlash)
        SLEEP_FUNC(100 * SLEEP_INC);

    return true;
}

bool Cedrus::Connection::WriteLarge (
    unsigned char * const inBuffer,
    DWORD bytesToWrite,
    LPDWORD bytesWritten,
    bool savesToFlash)
{
    if (bytesWritten == nullptr || inBuffer == nullptr)
        return false;

    if (m_DeviceHandle == nullptr || !FlushWriteToDeviceBuffer())
    {
        m_ConnectionDead = true;
        return false;
    }

    *bytesWritten = 0;

    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - m_timestamp).count() < m_cmdThroughputLimit)
    {
        SLEEP_FUNC(1 * SLEEP_INC);
    }

    m_timestamp = std::chrono::high_resolution_clock::now();

    const DWORD writeStatus = FT_Write(
        m_DeviceHandle,
        inBuffer,
        bytesToWrite,
        bytesWritten);

    if (writeStatus != FT_OK)
    {
        m_ConnectionDead = true;
        return false;
    }

    if (*bytesWritten != bytesToWrite)
        return false;

    if (savesToFlash)
        SLEEP_FUNC(100 * SLEEP_INC);

    return true;
}

int Cedrus::Connection::GetBaudRate() const
{
    return m_BaudRate;
}

void Cedrus::Connection::SetBaudRate(unsigned char rate)
{
    switch (rate)
    {
    case 0:
        m_BaudRate = 9600;
        break;
    case 1:
        m_BaudRate = 19200;
        break;
    case 2:
        m_BaudRate = 38400;
        break;
    case 3:
        m_BaudRate = 57600;
        break;
    case 4:
        m_BaudRate = 115200;
        break;
    default:
        break;
    }
}

bool Cedrus::Connection::HasLostConnection() const
{
    return m_ConnectionDead;
}

void Cedrus::Connection::SetCmdThroughputLimit(bool isXid2device)
{
    m_cmdThroughputLimit = isXid2device ? 3 : 10;
}

DWORD Cedrus::Connection::SendXIDCommand(
    const char inCommand[],
    DWORD commandSize,
    unsigned char outResponse[],
    unsigned int maxOutResponseSize)
{
    CEDRUS_ASSERT ( outResponse != NULL, "outResponse should be non-null. If you don't care about the response, use Write!" );

    memset(outResponse, 0x00, maxOutResponseSize);

    if ( !FlushReadFromDeviceBuffer () )
    {
        m_ConnectionDead = true;
        return 0;
    }

    DWORD bytes_written = 0;
    if ( !Write ( (unsigned char*)inCommand, commandSize, &bytes_written ) )
        return 0;

    DWORD bytes_stored = 0;

    // A long response is allowed to take as many reads as it needs,
    // so long as the device keeps feeding us bytes. We only give up
    // after the queue has stayed empty for this long.
    const auto idle_budget = std::chrono::milliseconds(150);
    auto last_progress = std::chrono::high_resolution_clock::now();

    while ( bytes_stored < maxOutResponseSize && !m_ConnectionDead )
    {
        if ( std::chrono::high_resolution_clock::now () - last_progress >= idle_budget )
            break;

        const DWORD available = GetBytesAvailable();

        if ( available == 0 )
        {
            SLEEP_FUNC ( 1 * SLEEP_INC );
            continue;
        }

        const DWORD remaining = maxOutResponseSize - bytes_stored;
        const DWORD to_read = ( available < remaining ) ? available : remaining;

        DWORD bytes_read = 0;
        if ( !Read ( outResponse + bytes_stored, to_read, &bytes_read ) )
            break;

        if ( bytes_read == 0 )
        {
            SLEEP_FUNC ( 1 * SLEEP_INC );
            continue;
        }

        bytes_stored += bytes_read;
        last_progress = std::chrono::high_resolution_clock::now ();
    }

    return bytes_stored;
}

DWORD Cedrus::Connection::SendXIDCommand_PST_Proof(
    const char inCommand[],
    DWORD commandSize,
    unsigned char outResponse[],
    unsigned int maxOutResponseSize)
{
    CEDRUS_ASSERT(outResponse != nullptr, "outResponse should be non-null. If you don't care about the response, use Write!");

    if (inCommand == nullptr || outResponse == nullptr)
        return 0;

    memset(outResponse, 0x00, maxOutResponseSize);

    if (!FlushReadFromDeviceBuffer())
    {
        m_ConnectionDead = true;
        return 0;
    }

    DWORD bytesWritten = 0;
    if (!Write((unsigned char*)inCommand, commandSize, &bytesWritten))
        return 0;

    unsigned char inBuffer[64];
    DWORD bytesStored = 0;
    unsigned int numRetries = 0;

    // We're reading from the buffer in chunks of 64 because of all the potential zeroes.
    while (bytesStored < maxOutResponseSize && numRetries < 3 && !m_ConnectionDead)
    {
        DWORD bytesRead = 0;
        if (!Read(inBuffer, sizeof(inBuffer), &bytesRead))
            break;

        for (DWORD i = 0; i < bytesRead && bytesStored < maxOutResponseSize; ++i)
        {
            // Ignore potential zeroes in the buffer.
            if (inBuffer[i] != 0)
                outResponse[bytesStored++] = inBuffer[i];
        }

        ++numRetries;
    }

    return bytesStored;
}
