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

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;

    class base_device_t
    {
    public:
        /**
          * @class base_device_t base_device_t.h "xid_device_driver/base_device_t.h"
          *
          * @brief base class for Cedrus devices.
          */
        base_device_t(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<xid_device_config_t> dev_config);
        virtual ~base_device_t();

        /**
         * Returns the name of the device
         *
         * @returns name of the device
         */
        std::string CEDRUS_XIDDRIVER_IMPORTEXPORT get_device_name();

        std::string CEDRUS_XIDDRIVER_IMPORTEXPORT get_internal_product_name ();

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_major_firmware_version();

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_minor_firmware_version();

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_outpost_model() const;

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_hardware_generation() const;

        void CEDRUS_XIDDRIVER_IMPORTEXPORT set_light_sensor_threshold( int threshold ) const;

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_light_sensor_threshold() const;

        /**
         * product id of the device.
         * 
         * @returns product id
         * 0: Lumina LP-400 response pad system
         * 1: SV-1 voice key system
         * 2: RB series response pad.
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_product_id() const;
        
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
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_model_id() const;

        int CEDRUS_XIDDRIVER_IMPORTEXPORT number_of_lines() const;

        /**
         * Resets the internal device reaction time timer.
         * 
         * This should be called when a stimulus is presented
         */
        void CEDRUS_XIDDRIVER_IMPORTEXPORT reset_rt_timer();

        /**
         * Resets the device's base timer
         *
         * This should be called when the device is initialized or an experiment
         * starts
         */
        void CEDRUS_XIDDRIVER_IMPORTEXPORT reset_base_timer();

        /**
         * Returns the time elapsed since the base timer was reset
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT query_base_timer();

        int CEDRUS_XIDDRIVER_IMPORTEXPORT close_connection();
        int CEDRUS_XIDDRIVER_IMPORTEXPORT open_connection();

        /**
         * Raise digital output lines on the StimTracker device.
         * 
         * @param[in] lines_bitmask This is a bitmask used to specify the lines
         * to be raised on the device. Each of the 8 bits in the integer
         * specifies a line.  If bits 0 and 7 are 1, lines 1 and 8 are raised.
         * @param[in] leave_remaining_lines boolean value of whether or not to
         * keep the current line state when applying the new bitmask.
         */
        void CEDRUS_XIDDRIVER_IMPORTEXPORT raise_lines(unsigned int lines_bitmask,
            bool leave_remaining_lines = false);

        /**
         * Clear digital output lines on the StimTracker device.
         *
         * @param[in] lines_bitmask This is a bitmask used to specify the lines
         * to be raised on the device. Each of the 8 bits in the integer
         * specifies a line.  If bits 0 and 7 are 1, lines 1 and 8 are raised.
         * @param[in] leave_remaining_lines boolean value of whether or not to
         * keep the current line state when applying the new bitmask.
         */ 
        void CEDRUS_XIDDRIVER_IMPORTEXPORT clear_lines(unsigned int lines_bitmask,
            bool leave_remaining_lines = false);

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_baud_rate () const;

    protected:
        boost::shared_ptr<xid_con_t> xid_con_;
        boost::shared_ptr<cedrus::xid_device_config_t> config_;

    private:
        void init_device();
    };

} // namespace cedrus

#endif
