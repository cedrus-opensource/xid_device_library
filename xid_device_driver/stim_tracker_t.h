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

#ifndef STIM_TRACKER_T_H
#define STIM_TRACKER_T_H

#include "XidDriverImpExpDefs.h"
#include "base_device_t.h"
#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class stim_tracker_t : public base_device_t
    {
    public:
        /**
         * stim_tracker_t constructor
         *
         * @param[in] xid_con xid_con_t object to use
         * @param[in] devconfig_path path to devconfig files.  Defaults to an
         * empty string. If no path is used, very conservative default values
         * are used and reults may not be what you expect.  It's highly
         * recommended to use the devconfig files.
         */
        stim_tracker_t(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<xid_device_config_t> dev_config);
        virtual ~stim_tracker_t(void);

        /**
         * Set the pulse duration when raise_lines() is called.  This is how 
         * long the line will be active.
         * 
         * @param[in] duration Length of time in miliseconds
         */
        void set_pulse_duration(unsigned int duration);

        virtual xid_device_config_t get_device_config();
        virtual int open_connection();
        virtual int close_connection();
        virtual int get_baud_rate();
        virtual void set_baud_rate( int rate );
        virtual void get_product_and_model_id( int &product_id, int &model_id );
        virtual int get_major_firmware_version();
        virtual int get_minor_firmware_version();
        virtual std::string get_internal_product_name();
        virtual void raise_lines(unsigned int lines_bitmask,
            bool leave_remaining_lines = false);
        virtual void clear_lines();

    private:
        char lines_state_;

    protected:
        boost::shared_ptr<xid_con_t> xid_con_;
        boost::shared_ptr<cedrus::xid_device_config_t> config_;
    };
} // namespace cedrus

#endif
