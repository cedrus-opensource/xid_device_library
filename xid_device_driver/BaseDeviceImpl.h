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

#ifndef BASE_DEVICE_IMPL_H
#define BASE_DEVICE_IMPL_H

#include "base_device_t.h"

#include <boost/utility.hpp>

namespace cedrus
{
    class xid_con_t;
    class xid_device_config_t;

    class BaseDeviceImpl : public base_device_t
    {
    public:
        BaseDeviceImpl(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<const xid_device_config_t> dev_config);

        virtual ~BaseDeviceImpl();

        virtual int get_outpost_model();

        virtual int get_hardware_generation();

        virtual int get_light_sensor_mode();

        virtual void set_light_sensor_mode( unsigned char mode );

        virtual void set_light_sensor_threshold( unsigned char threshold );

        virtual int get_light_sensor_threshold();

        virtual void set_scanner_trigger_filter( unsigned char mode );

        virtual void reset_rt_timer();

        virtual void reset_base_timer();

        virtual unsigned int query_base_timer();

        virtual unsigned int get_pulse_duration();

        virtual void set_pulse_duration( unsigned int duration );

        virtual int get_number_of_lines();

        virtual void set_number_of_lines(unsigned int lines);

        virtual void poll_for_response();

        virtual bool has_queued_responses();

        virtual int get_number_of_keys_down() const;

        virtual void clear_response_queue();

        virtual cedrus::response get_next_response();

        virtual void clear_responses();

        virtual int get_accessory_connector_mode();

        virtual int get_accessory_connector_device();

        virtual int get_output_logic();

        virtual void set_output_logic( int mode );

        virtual void set_accessory_connector_mode( int mode );

        virtual int get_vk_drop_delay();

        virtual void set_vk_drop_delay( unsigned char delay );

        virtual void reprogram_flash();

        virtual int get_trigger_default();

        virtual void set_trigger_default( bool default_on );

        virtual int get_trigger_debounce_time();

        virtual void set_trigger_debounce_time( unsigned char time );

        virtual int  get_button_debounce_time();

        virtual void set_button_debounce_time( unsigned char time );

        virtual void set_device_protocol( unsigned char protocol );

        virtual void connect_to_mpod(unsigned int mpod, unsigned int action);

        // Every device needs these.
        virtual const boost::shared_ptr<const xid_device_config_t> get_device_config() const;
        virtual int open_connection();
        virtual int close_connection();
        virtual bool has_lost_connection();
        virtual int get_baud_rate();
        virtual void set_baud_rate( unsigned char rate );
        virtual std::string get_device_protocol();
        virtual void get_product_and_model_id( int *product_id, int *model_id );
        virtual void set_model_id( unsigned char model );
        virtual int get_major_firmware_version();
        virtual int get_minor_firmware_version();
        virtual std::string get_internal_product_name();
        virtual void restore_factory_defaults();

        // Implementations of these will differ
        virtual void raise_lines(unsigned int lines_bitmask, bool leave_remaining_lines = false)=0;
        virtual void lower_lines(unsigned int lines_bitmask, bool leave_remaining_lines = false)=0;
        virtual void clear_lines()=0;

        virtual product_and_model_id get_product_and_model_id();

    protected:
        unsigned int m_linesState;
        boost::shared_ptr<xid_con_t> m_xidCon;
        const boost::shared_ptr<const cedrus::xid_device_config_t> m_config;
        const boost::shared_ptr<cedrus::response_mgr> m_response_mgr;
    };

} // namespace cedrus

#endif //BASE_DEVICE_IMPL_H
