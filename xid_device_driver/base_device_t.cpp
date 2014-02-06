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


cedrus::base_device_t::base_device_t(
    boost::shared_ptr<xid_con_t> xid_con,
    const std::string &devconfig_path)
    : xid_con_(xid_con),
      product_id_(-1),
      model_id_(0)
{
    init_device(devconfig_path);
}

cedrus::base_device_t::~base_device_t()
{}


void cedrus::base_device_t::reset_rt_timer()
{
    char return_info[200];
    xid_con_->send_xid_command(
        "e5", // reset rt timer Xid command
        2,
        return_info,
        sizeof(return_info),
        0);
}

void cedrus::base_device_t::reset_base_timer()
{
    char return_info[200];
    xid_con_->send_xid_command(
        "e1", // reset base timer Xid command
        2,
        return_info,
        sizeof(return_info),
        0);
}

int cedrus::base_device_t::query_base_timer()
{
    char return_info[200];
    int read = xid_con_->send_xid_command(
        "e3",
        2,
        return_info,
        sizeof(return_info),
        6);

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

void cedrus::base_device_t::init_device(const std::string &devconfig_path)
{
    product_id_ = get_inquiry("_d2",1,100,100);

    switch(product_id_)
    {
    case 0:
        device_name_ = "Cedrus Lumina LP-400 Response Pad System";
        break;
    case 1:
        device_name_ = "Cedrus SV-1 Voice Key";
        break;
    case 2:
        {
            model_id_ = get_inquiry("_d3", 1, 100, 100);
            switch(model_id_)
            {
            case 1:
                device_name_ = "Cedrus RB-530";
                break;
            case 2:
                device_name_ = "Cedrus RB-730";
                break;
            case 3:
                device_name_ = "Cedrus RB-830";
                break;
            case 4:
                device_name_ = "Cedrus RB-834";
                break;
            case -99:
                device_name_ = "Invalid XID Device";
                break;
            default:
                device_name_ = "Unknown Cedrus RB Series Device";
            }
            break;
        }
    case 83:
        device_name_ = "Cedrus StimTracker";
        break;
    case -99:
        device_name_ = "Invalid XID Device";
        break;
    default:
        device_name_ = "Unknown XID device";
        break;
    }

    if(!devconfig_path.empty())
    {
        config_ = xid_device_config_t::config_for_device(
          product_id_, model_id_, devconfig_path);

        xid_con_->set_needs_interbyte_delay(
            config_->needs_interbyte_delay());

        xid_con_->set_digital_out_prefix(
            config_->digital_out_prefix());
    }
    else
    {
        xid_con_->set_needs_interbyte_delay(true);
        if(product_id_ == 83)
            xid_con_->set_digital_out_prefix('m');
    }
}

int cedrus::base_device_t::get_inquiry(
    const char in_command[],
    int expected_bytes_rec,
    int timeout,
    int delay)
{
    int return_value = INVALID_RETURN_VALUE;
    char return_info[20];
    int last_byte = expected_bytes_rec -1;

    int bytes_returned = xid_con_->send_xid_command(
        in_command,
        0,
        return_info,
        sizeof(return_info),
        expected_bytes_rec,
        timeout,
        delay);

    if(bytes_returned > 0)
    {
        return_value = atoi(return_info);

        if(return_value == 0 && return_info[0] >= 'A')
        {
            return_value = return_info[last_byte];
        }
    }

    return return_value;
}

std::string cedrus::base_device_t::get_device_name()
{
    return device_name_;
}

int cedrus::base_device_t::get_product_id() const
{
    return product_id_;
}

int cedrus::base_device_t::get_model_id() const
{
    return model_id_;
}

