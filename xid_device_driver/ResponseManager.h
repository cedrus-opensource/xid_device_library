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

#include "constants.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <queue>

#include "XidDriverImpExpDefs.h"

namespace Cedrus
{
    class Connection;
    class DeviceConfig;

    struct Response
    {
        Response():
            port(-1),
            key(-1),
            wasPressed(false),
            reactionTime(-1){}

        // Port the response came from, usually 0.
        int port;
        // Button pressed. This is a 0 based index.
        int key;
        bool wasPressed;
        int reactionTime;
    };

    class ResponseManager : private boost::noncopyable
    {
    public:
        enum
        {
            INVALID_PORT_BITS = 0x08
        };

         ResponseManager(int minorFirmwareVer, std::shared_ptr<const DeviceConfig> devConfig);

        ~ResponseManager();

        void CheckForKeypress(std::shared_ptr<Connection> portConnection);

        bool HasQueuedResponses() const;

        Response GetNextResponse();

        // Even though the number of keys down should never be negative, this
        // returns a signed int as a way to check for errors. The count going
        // negative means that at some point we lost a key press, and that's
        // serious.
        unsigned int GetNumberOfKeysDown() const;

        void ClearResponseQueue();

    private:
        enum { OS_FILE_ERROR = -1 };

        void AdjustBufferForPacketRecovery();
        bool XidInputFound(Response &res);
        bool ST2InputFound(Response &res);
        bool XIDInputFoundLumina3G_21(Response &res);

        enum { XID_PACKET_SIZE = 6 };
        enum { ST2_PACKET_SIZE = 9 };
        enum { INVALID_PACKET_INDEX = -1 };
        enum { KEY_RELEASE_BITMASK = 0x10 };

        // Used by the response parsing logic
        int m_BytesInBuffer;
        unsigned char m_InputBuffer[ST2_PACKET_SIZE];
        int m_XIDPacketIndex;
        unsigned int m_packetSize;

        unsigned int m_numKeysDown;
        std::queue<Response> m_responseQueue;
        boost::function< bool(Response&) > m_ResponseParsingFunction;
        const std::shared_ptr<const DeviceConfig> m_respDevConfig;
    };
} // namespace Cedrus

