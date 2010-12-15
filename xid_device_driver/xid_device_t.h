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

#ifndef XID_DEVICE_T_H
#define XID_DEVICE_T_H

#include <Windows.h>
#include <queue>
#include <string>
#include "port_settings_t.h"
#include "constants.h"
#include <boost/shared_ptr.hpp>
#include "base_device_t.h"

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;
    
    /**
     * @class response
     * 
     * @brief struct to encapsulate a response from an XID device
     */
    struct response
    {
        /** 
         * port the response came from. Usualy 0 
         */
        int port; 

        /** 
         *button pressed. This is a 0 based index 
         */
        int button;

        /** 
         * whether or not a button was pressed 
         */
        bool pressed;

        /** 
         * reaction time 
         */
        int reaction_time; 
        
        /** 
         * key state. NO_KEY_DETECTED, FOUND_KEY_UP, or FOUND_KEY_DOWN 
         */
        cedrus::key_state key_state;
    };


    /**
     * @class xid_device_t xid_device_t.h "xid_device_driver/xid_device_t.h"
     *
     * @brief abstraction of an XID device
     */
    class xid_device_t : public base_device_t
    {
    public:
        /**
         * xid_device_t constructor
         *
         * @param[in] xid_con xid_con_t object to use
         * @param[in] devconfig_path path to devconfig files.  Defaults to an
         * empty string.  If no path is used, very conservative default values
         * are used and results may not be what you expect.  It's highly
         * reccommended to use the devconfig files.
         */
        xid_device_t(
            boost::shared_ptr<xid_con_t> xid_con,
            const std::wstring &devconfig_path = L""
            );
        virtual ~xid_device_t(void);
        


        /**
         * Poll the COM interface for a button press event.
         *
         * If there is a button press event, it is placed into an internal
         * response queue.  
         */
        void poll_for_response();

        /**
         * Number of responses in the response queue.
         *
         * @returns number of responses pending processing.
         */
        std::size_t response_queue_size() const;
        
        /**
         * @returns true if there are queued responses
         */
        bool has_queued_responses() const;

        /**
         * Clears the response queue
         */
        void clear_response_queue();

        /**
         * Returns the next response in the queue waiting for processing
         */
        response get_next_response();

        /**
         * Number of buttons the device has
         *
         * @returns number of buttons
         */
        int get_button_count() const;

        /**
         * Prefix used for the device for GUI display purposes
         *
         * @returns "Button" for Lumina, RB-series pads, and unknown devices.
         * "Voice Response" for SV-1 Voice Key systems.
         */
        std::wstring input_name_prefix() const;

    private:
        void init_response_device();
        std::queue<response> response_queue_;

        int button_count_;
        std::wstring input_name_prefix_;
    };
} // namespace cedrus

#endif // XID_DEVICE_T_H