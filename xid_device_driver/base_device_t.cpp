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

#include "base_device_t.h"
#include "xid_device_config_t.h"
#include "xid_con_t.h"
#include "constants.h"
#include <sstream>

cedrus::base_device_t::base_device_t(
    boost::shared_ptr<xid_con_t> xid_con,
    boost::shared_ptr<xid_device_config_t> dev_config)
    : xid_con_(xid_con),
      config_(dev_config)
{
    xid_con_->set_needs_interbyte_delay(config_->needs_interbyte_delay());
    xid_con_->set_digital_out_prefix(config_->digital_out_prefix());
}

cedrus::base_device_t::~base_device_t()
{}

void cedrus::base_device_t::reset_rt_timer()
{
    int bytes_written;
    xid_con_->write((unsigned char*)"e5", 2, bytes_written);
}

void cedrus::base_device_t::reset_base_timer()
{
    int bytes_written;
    xid_con_->write((unsigned char*)"e1", 2, bytes_written);
}

int cedrus::base_device_t::query_base_timer()
{
    char return_info[200];
    int read = xid_con_->send_xid_command(
        "e3",
        return_info,
        sizeof(return_info));

    bool valid_response = (read == 6);

    if(valid_response)
    {
        union {
            int as_int;
            char as_char[4];
        } rt;

        for(int i = 0; i < 4; ++i)
        {
            rt.as_char[i] = return_info[2+i];
        }
        return rt.as_int;
    }

    return GENERAL_ERROR;
}

std::string cedrus::base_device_t::get_internal_product_name()
{
    char return_info[200];
    xid_con_->send_xid_command(
        "_d1",
        return_info,
        sizeof(return_info));
    return std::string(return_info);
}

int cedrus::base_device_t::get_major_firmware_version( void )
{
    char major_return[2];

    xid_con_->send_xid_command(
        "_d4",
        major_return,
        sizeof(major_return));

    return major_return[0]-'0';
}

int cedrus::base_device_t::get_minor_firmware_version( void )
{
    char minor_return[2];

    xid_con_->send_xid_command(
        "_d5",
        minor_return,
        sizeof(minor_return));

    return minor_return[0]-'0';
}

std::string cedrus::base_device_t::get_device_name()
{
    return config_->get_device_name();
}

int cedrus::base_device_t::get_product_id() const
{
    return config_->get_product_id();
}

int cedrus::base_device_t::get_model_id() const
{
    return config_->get_model_id();
}

int cedrus::base_device_t::number_of_lines() const
{
    return config_->number_of_lines();
}

int cedrus::base_device_t::close_connection()
{
    return xid_con_->close();
}

int cedrus::base_device_t::open_connection()
{
    return xid_con_->open();
}

void cedrus::base_device_t::raise_lines( unsigned int lines_bitmask, bool leave_remaining_lines)
{
    xid_con_->set_digital_output_lines(lines_bitmask, leave_remaining_lines);
}

void cedrus::base_device_t::clear_lines(unsigned int lines_bitmask, bool leave_remaining_lines)
{
    xid_con_->clear_digital_output_lines(lines_bitmask, leave_remaining_lines);
}

int cedrus::base_device_t::get_baud_rate ( void ) const
{
    return xid_con_->get_baud_rate();
}
