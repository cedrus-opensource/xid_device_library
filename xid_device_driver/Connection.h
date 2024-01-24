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

#pragma once

#include "ftd2xx.h"

#ifdef __APPLE__
#   define SLEEP_FUNC usleep
#   define SLEEP_INC 1000
#elif defined(_WIN32)
#   include <windows.h>
#   define SLEEP_FUNC Sleep
#   define SLEEP_INC 1
#endif

#include <chrono>

namespace Cedrus
{
    enum { SAVES_TO_FLASH = true };

    class Connection
    {
    public:
        Connection(
            const DWORD location,
            DWORD port_speed = 115200,
            BYTE byte_size = FT_BITS_8,
            BYTE bit_parity = FT_PARITY_NONE,
            BYTE stop_bits = FT_STOP_BITS_1
        );

        ~Connection();

        bool Close();

        bool FlushWriteToDeviceBuffer();

        bool FlushReadFromDeviceBuffer();

        int Open();

        bool Read(unsigned char *inBuffer, DWORD bytesToRead, LPDWORD bytesRead);

        bool Write(
            unsigned char * const inBuffer,
            DWORD bytesToWrite,
            LPDWORD bytesWritten,
            bool savesToFlash = false );

        DWORD SendXIDCommand(
            const char inCommand[],
            DWORD commandSize,
            unsigned char outResponse[],
            unsigned int maxOutResponseSize);

        DWORD SendXIDCommand_PST_Proof(
            const char inCommand[],
            DWORD commandSize,
            unsigned char outResponse[],
            unsigned int maxOutResponseSize);

        int GetBaudRate() const;

        void SetBaudRate(unsigned char rate);

        bool HasLostConnection();

        void SetCmdThroughputLimit(bool isXid2device);

        void SetReadTimeout(DWORD readTimeout);

    private:
        bool SetupCOMPort();

        DWORD m_BaudRate;
        BYTE m_ByteSize;
        BYTE m_BitParity;
        BYTE m_StopBits;
        DWORD m_Location;

        bool m_ConnectionDead;
        unsigned int m_cmdThroughputLimit;

        FT_HANDLE m_DeviceHandle;

        std::chrono::high_resolution_clock::time_point m_timestamp;
    };
} // namespace Cedrus
