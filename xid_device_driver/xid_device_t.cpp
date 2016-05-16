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

#include "xid_device_t.h"

#include "xid_con_t.h"
#include "xid_glossary.h"

#include "CedrusAssert.h"

cedrus::xid_device_t::xid_device_t(
    boost::shared_ptr<xid_con_t> xid_con,
    boost::shared_ptr<const xid_device_config_t> dev_config)
    : m_xidCon(xid_con),
      m_config(dev_config),
      m_response_mgr(new response_mgr(get_minor_firmware_version(), dev_config))
{
    clear_lines();
    reset_rt_timer();
    reset_base_timer();
}

cedrus::xid_device_t::~xid_device_t(void)
{
}

void cedrus::xid_device_t::reset_rt_timer( void )
{
    xid_glossary::reset_rt_timer(m_xidCon);
}

void cedrus::xid_device_t::poll_for_response()
{
    m_response_mgr->check_for_keypress(m_xidCon, m_config);
}

bool cedrus::xid_device_t::has_queued_responses()
{
    return m_response_mgr->has_queued_responses();
}

int cedrus::xid_device_t::get_number_of_keys_down() const
{
    return m_response_mgr->get_number_of_keys_down();;
}

void cedrus::xid_device_t::clear_response_queue()
{
    m_response_mgr->clear_response_queue();
}

cedrus::response cedrus::xid_device_t::get_next_response()
{
    return m_response_mgr->get_next_response();
}

void cedrus::xid_device_t::clear_responses()
{
    m_xidCon->flush_read_from_device_buffer();
}

int cedrus::xid_device_t::get_accessory_connector_mode( void )
{
    return xid_glossary::get_accessory_connector_mode(m_xidCon);
}

void cedrus::xid_device_t::set_output_logic( int mode )
{
    xid_glossary::set_output_logic(m_xidCon, mode);
}

void cedrus::xid_device_t::set_accessory_connector_mode( int mode )
{
    xid_glossary::set_accessory_connector_mode(m_xidCon, mode);
}

void cedrus::xid_device_t::set_vk_drop_delay( unsigned int delay )
{
    xid_glossary::set_vk_drop_delay(m_xidCon, delay);
}

void cedrus::xid_device_t::set_device_protocol( int protocol )
{
    xid_glossary::set_device_protocol(m_xidCon, protocol);
}

const boost::shared_ptr<const cedrus::xid_device_config_t> cedrus::xid_device_t::get_device_config( void ) const
{
    return m_config;
}

int cedrus::xid_device_t::close_connection( void )
{
    return m_xidCon->close();
}

int cedrus::xid_device_t::open_connection( void )
{
    return m_xidCon->open();
}

bool cedrus::xid_device_t::has_lost_connection( void )
{
    return m_xidCon->has_lost_connection();
}

int cedrus::xid_device_t::get_baud_rate( void )
{
    return m_xidCon->get_baud_rate();
}

void cedrus::xid_device_t::set_baud_rate( unsigned char rate )
{
    //CEDRUS_ASSERT( 1 == 2, "here is set_baud_rate" ); // <--- JUST A SAMPLE FOR SYNTAX

    m_xidCon->set_baud_rate(rate);
    xid_glossary::set_device_baud_rate(m_xidCon, rate);
}

void cedrus::xid_device_t::get_product_and_model_id( int *product_id, int *model_id )
{
    xid_glossary::get_product_and_model_id(m_xidCon, product_id, model_id);
}

int cedrus::xid_device_t::get_major_firmware_version( void )
{
    return xid_glossary::get_major_firmware_version(m_xidCon);
}

int cedrus::xid_device_t::get_minor_firmware_version( void )
{
    return xid_glossary::get_minor_firmware_version(m_xidCon);
}

std::string cedrus::xid_device_t::get_internal_product_name( void )
{
    return xid_glossary::get_internal_product_name(m_xidCon);
}

void cedrus::xid_device_t::raise_lines(unsigned int lines_bitmask, bool leave_remaining_lines)
{
    unsigned int output_lines = lines_bitmask;

    if(leave_remaining_lines)
        output_lines |= m_linesState;

    xid_glossary::set_digital_output_lines_xid(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::xid_device_t::clear_lines( void )
{
    xid_glossary::set_digital_output_lines_xid(m_xidCon, 0);
    m_linesState = 0;
}

void cedrus::xid_device_t::reset_base_timer()
{
    xid_glossary::reset_base_timer(m_xidCon);
}

int cedrus::xid_device_t::query_base_timer()
{
    return xid_glossary::query_base_timer( m_xidCon );
}

int cedrus::xid_device_t::get_trigger_default( void )
{
    return xid_glossary::get_trigger_default(m_xidCon);
}

void cedrus::xid_device_t::set_trigger_default( bool default_on )
{
    xid_glossary::set_trigger_default(m_xidCon, default_on);
}

int cedrus::xid_device_t::get_trigger_debounce_time( void )
{
    return xid_glossary::get_trigger_debounce_time(m_xidCon);
}

void cedrus::xid_device_t::set_trigger_debounce_time( unsigned char time )
{
    xid_glossary::set_trigger_debounce_time(m_xidCon, time);
}

int cedrus::xid_device_t::get_button_debounce_time( void )
{
    return xid_glossary::get_button_debounce_time(m_xidCon);
}

void cedrus::xid_device_t::set_button_debounce_time( unsigned char time )
{
    xid_glossary::set_button_debounce_time(m_xidCon, time);
}

void cedrus::xid_device_t::restore_factory_defaults( void )
{
    xid_glossary::restore_factory_defaults(m_xidCon);
}
