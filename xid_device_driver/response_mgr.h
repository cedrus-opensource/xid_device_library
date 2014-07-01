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
#include "XidDriverImpExpDefs.h"

#include <boost/shared_ptr.hpp>
#include <queue>  

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;

    struct response
    {
        response():
            port(-1),
            key(-1),
            was_pressed(false),
            reaction_time(-1),
            key_state(NO_KEY_DETECTED){}

        //port the response came from. Usualy 0 
        int port; 

        //button pressed. This is a 0 based index 
        int key;

        //pressed or released?
        bool was_pressed;

        int reaction_time; 
        
        //key state. NO_KEY_DETECTED, FOUND_KEY_UP, or FOUND_KEY_DOWN 
        cedrus::key_state key_state;
    };

    /**
     * @class xid_con_t xid_con_t.h "xid_device_driver/xid_con_t.h"
     * 
     * @brief Implementation of communication with an XID device
     *
     * This class implements the nuts and bolts of communicating with an
     * XID device.  Inter-byte delays are optionally added if it's
     * communicating with a 4 MHz XID device.  It also handles the parsing
     * of the XID protocol 6-byte packats.
     */
    class response_mgr
    {
    public:
        enum
        {
	    INVALID_PORT_BITS = 0x0C
        };

        /**
         * xid_con_t constructor
         *
         * @param[in] port_name COM port name the device is connected to.
         * @param[in] port_speed speed of the port. This defaults to 115200bps
         * @param[in] delay_ms Inter-byte delay time in miliseconds.  Defaults to 1ms.
         * @param[in] byte_size Byte size used by the device
         * @param[in] bit_parity bit parity of the device.  Defaults to none.
         * @param[in] stop_bits stop bits of the device. Defaults to 0
         */
        response_mgr();
        
        ~response_mgr();

        /**
         * Checks the device to see if an event response has been sent.
         * 
         * @returns key_state if no event was found, NO_KEY_DETECTED is returned.
         * Otherwise, it responds with FOUND_KEY_UP, or FOUND_KEY_DOWN.
         */
        void check_for_keypress(boost::shared_ptr<xid_con_t> port_connection, boost::shared_ptr<xid_device_config_t> dev_config);

        int get_number_of_keys_down();

        bool has_queued_responses() const;

        response get_next_response();

    private:
    	enum { OS_FILE_ERROR = -1 };

        void adjust_buffer_for_packet_recovery();
        key_state xid_input_found( response &res );
      
        int delay_;

        enum {XID_PACKET_SIZE = 6};
        enum {INVALID_PACKET_INDEX = -1};
        enum {KEY_RELEASE_BITMASK = 0x10};
        
        int m_bytes_in_buffer;
        unsigned char m_input_buffer[XID_PACKET_SIZE];

        int m_xid_packet_index;
        int num_keys_down_;

        std::queue<response> response_queue_;
    };
} // namespace cedrus

#endif // RESPONSE_MGR_H