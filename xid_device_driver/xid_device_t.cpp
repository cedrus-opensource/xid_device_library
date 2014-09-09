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
    : xid_con_(xid_con),
      config_(dev_config),
      m_response_mgr(new response_mgr())
{
    clear_lines();
    xid_con_->set_needs_interbyte_delay(config_->needs_interbyte_delay());
    reset_rt_timer();
    reset_base_timer();
}

cedrus::xid_device_t::~xid_device_t(void)
{
}

void cedrus::xid_device_t::reset_rt_timer( void )
{
    xid_glossary::reset_rt_timer(xid_con_);
}

void cedrus::xid_device_t::poll_for_response()
{
    m_response_mgr->check_for_keypress(xid_con_, config_);
}

bool cedrus::xid_device_t::has_queued_responses()
{
    return m_response_mgr->has_queued_responses();
}

cedrus::response cedrus::xid_device_t::get_next_response()
{
    return m_response_mgr->get_next_response();
}

int cedrus::xid_device_t::get_accessory_connector_mode( void )
{
    return xid_glossary::get_accessory_connector_mode(xid_con_);
}

void cedrus::xid_device_t::set_accessory_connector_mode( int mode )
{
    xid_glossary::set_accessory_connector_mode(xid_con_, mode);
}

void cedrus::xid_device_t::set_device_protocol( int protocol )
{
    xid_glossary::set_device_protocol(xid_con_, protocol);
}

cedrus::xid_device_config_t cedrus::xid_device_t::get_device_config( void )
{
    return *config_;
}

int cedrus::xid_device_t::close_connection( void )
{
    return xid_con_->close();
}

int cedrus::xid_device_t::open_connection( void )
{
    return xid_con_->open();
}

bool cedrus::xid_device_t::has_lost_connection( void )
{
    return xid_con_->has_lost_connection();
}

int cedrus::xid_device_t::get_baud_rate( void )
{
    return xid_con_->get_baud_rate();
}

void cedrus::xid_device_t::set_baud_rate( int rate )
{
    //CEDRUS_ASSERT( 1 == 2, "here is set_baud_rate" ); // <--- JUST A SAMPLE FOR SYNTAX

    xid_con_->set_baud_rate(rate);
    xid_glossary::set_device_baud_rate(xid_con_, rate);
}

void cedrus::xid_device_t::get_product_and_model_id( int *product_id, int *model_id )
{
    xid_glossary::get_product_and_model_id(xid_con_, product_id, model_id);
}

int cedrus::xid_device_t::get_major_firmware_version( void )
{
    return xid_glossary::get_major_firmware_version(xid_con_);
}

int cedrus::xid_device_t::get_minor_firmware_version( void )
{
    return xid_glossary::get_minor_firmware_version(xid_con_);
}

std::string cedrus::xid_device_t::get_internal_product_name( void )
{
    return xid_glossary::get_internal_product_name(xid_con_);
}

void cedrus::xid_device_t::raise_lines(unsigned int lines_bitmask, bool leave_remaining_lines)
{
    unsigned int output_lines = lines_bitmask;

    if(leave_remaining_lines)
        output_lines |= lines_state_;

    xid_glossary::set_digital_output_lines_xid(xid_con_, output_lines);

    lines_state_ = output_lines;
}

void cedrus::xid_device_t::clear_lines( void )
{
    xid_glossary::set_digital_output_lines_xid(xid_con_, 0);
    lines_state_ = 0;
}

void cedrus::xid_device_t::reset_base_timer()
{
    xid_glossary::reset_base_timer(xid_con_);
}

int cedrus::xid_device_t::query_base_timer()
{
    return xid_glossary::query_base_timer( xid_con_ );
}

int cedrus::xid_device_t::get_debouncing_time( void )
{
    return xid_glossary::get_debouncing_time(xid_con_);
}

void cedrus::xid_device_t::set_debouncing_time( int time )
{
    xid_glossary::set_debouncing_time(xid_con_, time);
}
