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

#ifndef RESPONSE_MGR_H
#define RESPONSE_MGR_H

#include "constants.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <queue>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    class interface_xid_con;
    class xid_device_config_t;

    struct response
    {
        response():
            port(-1),
            key(-1),
            was_pressed(false),
            reaction_time(-1){}

        //port the response came from. Usualy 0
        int port;

        //button pressed. This is a 0 based index
        int key;

        //pressed or released?
        bool was_pressed;

        int reaction_time;
    };

    class response_mgr : private boost::noncopyable
    {
    public:
        enum
        {
            INVALID_PORT_BITS = 0x08
        };

        // This is exported purely for testing purposes! The response manager isn't meant to be used on its own!
        CEDRUS_XIDDRIVER_IMPORTEXPORT response_mgr(int minor_firmware_ver, boost::shared_ptr<const xid_device_config_t> dev_config);

        // This is exported purely for testing purposes! The response manager isn't meant to be used on its own!
        CEDRUS_XIDDRIVER_IMPORTEXPORT ~response_mgr();

        /**
         * Checks the device to see if an event response has been sent.
         *
         * @returns key_state if no event was found, NO_KEY_DETECTED is returned.
         * Otherwise, it responds with FOUND_KEY_UP, or FOUND_KEY_DOWN.
         */
        // This is exported purely for testing purposes! The response manager isn't meant to be used on its own!
        void CEDRUS_XIDDRIVER_IMPORTEXPORT check_for_keypress(boost::shared_ptr<interface_xid_con> port_connection, boost::shared_ptr<const xid_device_config_t> dev_config);

        // This is exported purely for testing purposes! The response manager isn't meant to be used on its own!
        bool CEDRUS_XIDDRIVER_IMPORTEXPORT has_queued_responses() const;

        // This is exported purely for testing purposes! The response manager isn't meant to be used on its own!
        response CEDRUS_XIDDRIVER_IMPORTEXPORT get_next_response();

        // Even though the number of keys down should never be negative, this
        // returns a signed int as a way to check for errors. The count going
        // negative means that at some point we lost a key press, and that's
        // serious.
        int get_number_of_keys_down() const;

        void clear_response_queue();

    private:
        enum { OS_FILE_ERROR = -1 };

        void adjust_buffer_for_packet_recovery();
        key_state xid_input_found( response &res );
        key_state xid_input_found_lumina3g_21( response &res );

        enum {XID_PACKET_SIZE = 6};
        enum {INVALID_PACKET_INDEX = -1};
        enum {KEY_RELEASE_BITMASK = 0x10};

        // Used by the response parsing logic
        int m_bytes_in_buffer;
        unsigned char m_input_buffer[XID_PACKET_SIZE];
        int m_xid_packet_index;

        int m_numKeysDown;
        std::queue<response> m_responseQueue;
        boost::function< cedrus::key_state (response&) > m_response_parsing_function;
    };
} // namespace cedrus

#endif // RESPONSE_MGR_H
