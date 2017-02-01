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

#include "BaseDeviceImpl.h"

#include "constants.h"
#include "response_mgr.h"
#include "xid_con_t.h"
#include "xid_glossary.h"

cedrus::BaseDeviceImpl::BaseDeviceImpl(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<const xid_device_config_t> dev_config)
    : m_xidCon(xid_con),
      m_config(dev_config)
{
}

cedrus::BaseDeviceImpl::~BaseDeviceImpl()
{

}

const boost::shared_ptr<const cedrus::xid_device_config_t> cedrus::BaseDeviceImpl::get_device_config() const
{
    return m_config;
}

int cedrus::BaseDeviceImpl::close_connection( void )
{
    return m_xidCon->close();
}

int cedrus::BaseDeviceImpl::open_connection( void )
{
    return m_xidCon->open();
}

bool cedrus::BaseDeviceImpl::has_lost_connection( void )
{
    return m_xidCon->has_lost_connection();
}

int cedrus::BaseDeviceImpl::get_baud_rate( void )
{
    return m_xidCon->get_baud_rate();
}

void cedrus::BaseDeviceImpl::set_baud_rate( unsigned char rate )
{
    m_xidCon->set_baud_rate(rate);
    xid_glossary::set_device_baud_rate(m_xidCon, rate);
}

std::string cedrus::BaseDeviceImpl::get_device_protocol()
{
    return xid_glossary::get_device_protocol(m_xidCon);
}

void cedrus::BaseDeviceImpl::get_product_and_model_id( int *product_id, int *model_id )
{
    xid_glossary::get_product_and_model_id(m_xidCon, product_id, model_id);
}

int cedrus::BaseDeviceImpl::get_major_firmware_version( void )
{
    return xid_glossary::get_major_firmware_version(m_xidCon);
}

int cedrus::BaseDeviceImpl::get_minor_firmware_version( void )
{
    return xid_glossary::get_minor_firmware_version(m_xidCon);
}

std::string cedrus::BaseDeviceImpl::get_internal_product_name( void )
{
    return xid_glossary::get_internal_product_name(m_xidCon);
}

void cedrus::BaseDeviceImpl::raise_lines(unsigned int lines_bitmask, bool leave_remaining_lines)
{
    unsigned int output_lines = lines_bitmask;

    if(leave_remaining_lines)
        output_lines |= m_linesState;

    xid_glossary::set_digital_output_lines_st(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::BaseDeviceImpl::lower_lines( unsigned int lines_bitmask, bool leave_remaining_lines )
{
    unsigned int output_lines = ~lines_bitmask;

    if(leave_remaining_lines)
        output_lines &= m_linesState;

    xid_glossary::set_digital_output_lines_st(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::BaseDeviceImpl::reset_base_timer()
{
    return xid_glossary::reset_base_timer(m_xidCon);
}

unsigned int cedrus::BaseDeviceImpl::query_base_timer()
{
    return xid_glossary::query_base_timer(m_xidCon);
}

void cedrus::BaseDeviceImpl::restore_factory_defaults( void )
{
    xid_glossary::restore_factory_defaults(m_xidCon);
}

int cedrus::BaseDeviceImpl::get_outpost_model()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::BaseDeviceImpl::get_hardware_generation()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::BaseDeviceImpl::get_light_sensor_mode()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_light_sensor_mode( unsigned char /*mode*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::set_light_sensor_threshold( unsigned char /*threshold*/ )
{
    // nothing
}

int cedrus::BaseDeviceImpl::get_light_sensor_threshold()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_scanner_trigger_filter( unsigned char /*mode*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::reset_rt_timer()
{
    // nothing
}

unsigned int cedrus::BaseDeviceImpl::get_pulse_duration()
{
    return 0;
}

void cedrus::BaseDeviceImpl::set_pulse_duration(unsigned int /*duration*/)
{
    // nothing
}

int cedrus::BaseDeviceImpl::get_number_of_lines()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_number_of_lines(unsigned int /*lines*/)
{
    // nothing
}

void cedrus::BaseDeviceImpl::poll_for_response()
{
    // nothing
}

bool cedrus::BaseDeviceImpl::has_queued_responses()
{
    return false;
}

int cedrus::BaseDeviceImpl::get_number_of_keys_down() const
{
    return 0;
}

void cedrus::BaseDeviceImpl::clear_response_queue()
{
    // nothing
}

cedrus::response cedrus::BaseDeviceImpl::get_next_response()
{
    return cedrus::response();
}

void cedrus::BaseDeviceImpl::clear_responses()
{
    // nothing
}

int cedrus::BaseDeviceImpl::get_accessory_connector_mode()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::BaseDeviceImpl::get_accessory_connector_device()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::BaseDeviceImpl::get_output_logic()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_output_logic( int /*mode*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::set_accessory_connector_mode( int /*mode*/ )
{
    // nothing
}
int cedrus::BaseDeviceImpl::get_vk_drop_delay()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_vk_drop_delay( unsigned char /*delay*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::reprogram_flash()
{
    xid_glossary::reprogram_flash(m_xidCon);
}

int cedrus::BaseDeviceImpl::get_trigger_default()
{
    return false;
}

void cedrus::BaseDeviceImpl::set_trigger_default( bool /*default_on*/ )
{
    // nothing
}

int cedrus::BaseDeviceImpl::get_trigger_debounce_time()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_trigger_debounce_time( unsigned char /*time*/ )
{
    // nothing
}

int cedrus::BaseDeviceImpl::get_button_debounce_time()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::BaseDeviceImpl::set_button_debounce_time( unsigned char /*time*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::set_device_protocol( unsigned char /*protocol*/ )
{
    // nothing
}

void cedrus::BaseDeviceImpl::connect_to_mpod(unsigned int mpod, unsigned int action)
{
    xid_glossary::connect_to_mpod(m_xidCon, mpod, action);
}

cedrus::BaseDeviceImpl::product_and_model_id cedrus::BaseDeviceImpl::get_product_and_model_id()
{
    product_and_model_id pm_id;
    get_product_and_model_id(&pm_id.product_id, &pm_id.model_id);
    return pm_id;
}

void cedrus::BaseDeviceImpl::set_model_id( unsigned char model )
{
    xid_glossary::set_model_id(m_xidCon, model);
}
