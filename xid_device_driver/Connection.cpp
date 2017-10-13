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
#include <boost/date_time/posix_time/posix_time.hpp>

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
    m_DeviceHandle(nullptr)
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

    return status;
}

bool Cedrus::Connection::SetupCOMPort()
{
    bool status = false;

    FT_SetBaudRate(m_DeviceHandle, m_BaudRate);
    FT_SetDataCharacteristics(m_DeviceHandle, m_ByteSize, m_StopBits, m_BitParity);

    FT_SetTimeouts(m_DeviceHandle, 1, 500);
    FT_SetUSBParameters(m_DeviceHandle, 64, 64);
    FT_SetLatencyTimer(m_DeviceHandle, 2);

    status = FlushWriteToDeviceBuffer();
    if (status)
        status = FlushReadFromDeviceBuffer();

    return status;
}

bool Cedrus::Connection::Read(
    unsigned char *inBuffer,
    DWORD bytesToRead,
    LPDWORD bytesRead)
{
    DWORD read_status = FT_OK;

    read_status = FT_Read(m_DeviceHandle, inBuffer, bytesToRead, bytesRead);

    if (read_status != FT_OK)
    {
        // We used to check for specific error codes here, but I'm not certain why.
        // I don't know that any of them are errors you can recover from, so let's
        // err on the side of caution here.
        m_ConnectionDead = true;
    }

    return read_status == FT_OK;
}

bool Cedrus::Connection::Write(
    unsigned char * const inBuffer,
    DWORD bytesToWrite,
    LPDWORD bytesWritten,
    bool requiresDelay)
{
    DWORD write_status = FT_OK;

    if (!requiresDelay)
    {
        write_status = FT_Write(m_DeviceHandle, inBuffer, bytesToWrite, bytesWritten);
    }
    else
    {
        unsigned char *p = inBuffer;
        for (unsigned int i = 0; i < bytesToWrite; ++i)
        {
            DWORD byte_count;
            write_status = FT_Write(m_DeviceHandle, p, 1, &byte_count);
            if ((write_status != FT_OK) || (++(*bytesWritten) == bytesToWrite))
            {
                break;
            }

            SLEEP_FUNC(2 * SLEEP_INC);
            ++p;
        }
    }

    m_ConnectionDead = (write_status != FT_OK);

    return m_ConnectionDead;
}

unsigned long Cedrus::Connection::GetTickCount() const
{
    boost::posix_time::ptime time_microseconds = boost::posix_time::microsec_clock::local_time();
    unsigned long milliseconds = static_cast<unsigned long>(time_microseconds.time_of_day().total_milliseconds());

    return milliseconds;
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

bool Cedrus::Connection::HasLostConnection()
{
    return m_ConnectionDead;
}

DWORD Cedrus::Connection::SendXIDCommand(
    const char inCommand[],
    unsigned char outResponse[],
    unsigned int maxOutResponseSize,
    bool requiresDelay)
{
    if (outResponse != NULL)
        memset(outResponse, 0x00, maxOutResponseSize);

    DWORD bytes_written = 0;
    Write((unsigned char*)inCommand, strlen(inCommand), &bytes_written, requiresDelay);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    DWORD bytes_read = 0;
    DWORD bytes_stored = 0;

    unsigned int i = 0;
    do
    {
        Read(in_buff, sizeof(in_buff), &bytes_read);

        if (bytes_read > 0)
        {
            for (unsigned int j = 0; (j < bytes_read) && (bytes_stored < maxOutResponseSize); ++j)
            {
                outResponse[bytes_stored] = in_buff[j];
                bytes_stored++;
            }
        }

        ++i;
    } while (i < 50 && bytes_stored < maxOutResponseSize);

    return bytes_stored;
}

DWORD Cedrus::Connection::SendXIDCommand_PST_Proof(
    const char inCommand[],
    unsigned char outResponse[],
    unsigned int maxOutResponseSize,
    bool requiresDelay)
{
    if (outResponse != NULL)
        memset(outResponse, 0x00, maxOutResponseSize);

    DWORD bytes_written = 0;
    Write((unsigned char*)inCommand, strlen(inCommand), &bytes_written, requiresDelay);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    DWORD bytes_read = 0;
    DWORD bytes_stored = 0;

    unsigned int numRetries = 0;
    do
    {
        // We're reading from the buffer in chunks of 64 because of all the potential zeroes.
        Read(in_buff, sizeof(in_buff), &bytes_read);

        if (bytes_read > 0)
        {
            for (unsigned int i = 0; (i < bytes_read) && (bytes_stored < maxOutResponseSize); ++i)
            {
                // Ignore potential zeroes in the buffer.
                if (in_buff[i] != 0)
                {
                    outResponse[bytes_stored] = in_buff[i];
                    bytes_stored++;
                }
            }
        }

        ++numRetries;
    } while (numRetries < 100 && bytes_stored < maxOutResponseSize);

    return bytes_stored;
}
