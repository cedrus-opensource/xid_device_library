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

#include "xid_glossary.h"
#include "constants.h"
#include "xid_con_t.h"
#include "xid_device_config_t.h"
#include "response_mgr.h"

#include "XidDriverImpExpDefs.h"
#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class xid_con_t;

    class base_device_t
    {
    public:
        virtual ~base_device_t();

        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_outpost_model();

        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_hardware_generation();

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT set_light_sensor_threshold( int threshold );

        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_light_sensor_threshold();

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT reset_rt_timer();

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT reset_base_timer();

        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT query_base_timer();

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT set_pulse_duration(unsigned int duration);

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT poll_for_response();

        virtual bool CEDRUS_XIDDRIVER_IMPORTEXPORT has_queued_responses();

        virtual cedrus::response CEDRUS_XIDDRIVER_IMPORTEXPORT get_next_response();

        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_accessory_connector_mode( void );

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT set_accessory_connector_mode( int mode );

        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT set_device_mode( int protocol );

        // Every device needs these.
        virtual xid_device_config_t CEDRUS_XIDDRIVER_IMPORTEXPORT get_device_config() = 0;
        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT open_connection() = 0;
        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT close_connection() = 0;
        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_baud_rate() = 0;
        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT set_baud_rate( int rate ) = 0;
        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT get_product_and_model_id( int &product_id, int &model_id ) = 0;
        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_major_firmware_version() = 0;
        virtual int CEDRUS_XIDDRIVER_IMPORTEXPORT get_minor_firmware_version() = 0;
        virtual std::string CEDRUS_XIDDRIVER_IMPORTEXPORT get_internal_product_name() = 0;
        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT raise_lines(unsigned int lines_bitmask,
            bool leave_remaining_lines = false) = 0;
        virtual void CEDRUS_XIDDRIVER_IMPORTEXPORT clear_lines() = 0;
    };

} // namespace cedrus

#endif
