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

#ifndef BASE_DEVICE_T_H
#define BASE_DEVICE_T_H

#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;

    class base_device_t
    {
    public:
        /**
          * @class base_device_t xid_device_t.h "xid_device_driver/xid_device_t.h"
          *
          * @brief base class for Cedrus devices.
          */
        base_device_t(
            boost::shared_ptr<xid_con_t> xid_con,
            const std::wstring &devconfig_path = L"");
        virtual ~base_device_t();

        /**
         * Returns the name of the device
         *
         * @returns name of the device
         */
        std::string get_device_name();

        /**
         * product id of the device.
         * 
         * @returns product id
         * 0: Lumina LP-400 response pad system
         * 1: SV-1 voice key system
         * 2: RB series response pad.
         */
        int get_product_id() const;
        
        /**
         * model ID of the device.
         * 
         * This is generally only valid on RB series response pads.
         *
         * @returns model id of the device
         * 1: RB-530
         * 2: RB-730
         * 3: RB-830
         * 4: RB-834
         */
        int get_model_id() const;

        /**
         * Resets the internal device reaction time timer.
         * 
         * This should be called when a stimulus is presented
         */
        void reset_rt_timer();

        /**
         * Resets the device's base timer
         *
         * This should be called when the device is initialized or an experiment
         * starts
         */
        void reset_base_timer();

        /**
         * Returns the time elapsed since the base timer was reset
         */
        int query_base_timer();

        /**
         * Send a command to the device
         *
         * @param[in] in_command command to send to the device.  Commands are
         * detailed at http://www.cedrus.com/xid/
         * @param[in] expected_bytes_rec expected number of bytes to receive
         * @param[in] timeout time in miliseconds the device should respond in
         * @param[in] delay some devices need an additional delay between
         * receiving a command and issuing a response. Defaults to 0
         * 
         * @returns an integer value of the response
         */
        int get_inquiry(
            const char in_command[],
            int expected_bytes_rec = 1,
            int timeout = 100,
            int delay = 0);

    protected:
        boost::shared_ptr<xid_con_t> xid_con_;
        boost::shared_ptr<cedrus::xid_device_config_t> config_;

    private:
        void init_device(const std::wstring &devconfig_path);
        std::string device_name_;
        int product_id_;
        int model_id_;

    };

} // namespace cedrus

#endif
