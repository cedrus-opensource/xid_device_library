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

#include "XidDriverImpExpDefs.h"

#include "response_mgr.h"
#include "read_only_device.h"

#include <boost/utility.hpp>

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT base_device_t : public read_only_device, boost::noncopyable
    {
    public:
        struct product_and_model_id
        {
            product_and_model_id() : product_id(-1), model_id(-1) { }
            int product_id;
            int model_id;
        };

        virtual ~base_device_t()
        {
        }

        virtual int get_outpost_model()=0;

        virtual int get_hardware_generation()=0;

        virtual int get_light_sensor_mode()=0;

        virtual void set_light_sensor_mode( unsigned char mode )=0;

        virtual void set_light_sensor_threshold( unsigned char threshold )=0;

        virtual int get_light_sensor_threshold()=0;

        virtual void set_scanner_trigger_filter( unsigned char mode )=0;

        virtual void reset_rt_timer()=0;

        virtual void reset_base_timer()=0;

        virtual unsigned int query_base_timer()=0;

        virtual unsigned int get_pulse_duration()=0;

        virtual void set_pulse_duration( unsigned int duration )=0;

        virtual int get_number_of_lines()=0;

        virtual void set_number_of_lines(unsigned int lines)=0;

        virtual void poll_for_response()=0;

        virtual bool has_queued_responses()=0;

        virtual int get_number_of_keys_down() const=0;

        virtual void clear_response_queue()=0;

        virtual cedrus::response get_next_response()=0;

        virtual void clear_responses()=0;

        virtual int get_accessory_connector_mode()=0;

        virtual int get_accessory_connector_device()=0;

        virtual int get_output_logic()=0;

        virtual void set_output_logic( int mode )=0;

        virtual void set_accessory_connector_mode( int mode )=0;

        virtual int get_vk_drop_delay()=0;

        virtual void set_vk_drop_delay( unsigned char delay )=0;

        virtual void reprogram_flash()=0;

        virtual int get_trigger_default()=0;

        virtual void set_trigger_default( bool default_on )=0;

        virtual int get_trigger_debounce_time()=0;

        virtual void set_trigger_debounce_time( unsigned char time )=0;

        virtual int  get_button_debounce_time()=0;

        virtual void set_button_debounce_time( unsigned char time )=0;

        virtual void set_device_protocol( unsigned char protocol )=0;

        // Every device needs these.
        virtual const boost::shared_ptr<const xid_device_config_t> get_device_config() const = 0;
        virtual int open_connection() = 0;
        virtual int close_connection() = 0;
        virtual bool has_lost_connection() = 0;
        virtual int get_baud_rate() = 0;
        virtual void set_baud_rate( unsigned char rate ) = 0;
        virtual std::string get_device_protocol()=0;
        virtual void get_product_and_model_id( int *product_id, int *model_id ) = 0;
        virtual void set_model_id( unsigned char model ) = 0;
        virtual product_and_model_id get_product_and_model_id()=0;
        virtual int get_major_firmware_version() = 0;
        virtual int get_minor_firmware_version() = 0;
        virtual std::string get_internal_product_name() = 0;
        virtual void raise_lines(unsigned int lines_bitmask, bool leave_remaining_lines = false) = 0;
        virtual void lower_lines(unsigned int lines_bitmask, bool leave_remaining_lines = false) = 0;
        virtual void clear_lines() = 0;
        virtual void restore_factory_defaults() = 0;
    };

} // namespace cedrus

#endif
