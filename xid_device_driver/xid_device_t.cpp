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
#include "xid_device_config_t.h"
#include "xid_con_t.h"
#include "constants.h"

cedrus::xid_device_t::xid_device_t(
    boost::shared_ptr<xid_con_t> xid_con,
    const std::wstring &devconfig_path)
    : xid_con_(xid_con),
      product_id_(-1),
      model_id_(0),
      button_count_(8),
      input_name_prefix_(L"Button")
{
    init_device(devconfig_path);
}

cedrus::xid_device_t::~xid_device_t(void)
{
}

int cedrus::xid_device_t::get_xid_inquiry(
    const char in_command[],
    int expected_bytes_rec,
    int timeout,
    int delay)
{
    int return_value = INVALID_RETURN_VALUE;
    char return_info[20];

    int bytes_returned = xid_con_->send_xid_command(
        in_command,
        0,
        return_info,
        sizeof(return_info),
        expected_bytes_rec,
        timeout,
        delay);

    if(bytes_returned > 0)
        return_value = atoi(return_info);

    return return_value;
}

void cedrus::xid_device_t::reset_rt_timer()
{
    char return_info[200];
    xid_con_->send_xid_command(
        "e5", // reset rt timer Xid command
        2,
        return_info,
        sizeof(return_info),
        0);
}

void cedrus::xid_device_t::reset_base_timer()
{
    char return_info[200];
    xid_con_->send_xid_command(
        "e1", // reset base timer Xid command
        2,
        return_info,
        sizeof(return_info),
        0);
}

int cedrus::xid_device_t::query_base_timer()
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

void cedrus::xid_device_t::poll_for_response()
{
    cedrus::key_state state = xid_con_->check_for_keypress();

    if(state != cedrus::NO_KEY_DETECTED)
    {
        int port = -1;
        int key  = -1;
        bool was_pressed = false;
        int response_time = -1;

        xid_con_->get_current_response(port, key, was_pressed, response_time);
        
        if(config_)
        {
            key = config_->get_mapped_key(key);
        }
        else
        {
            // RB series response pads respond with a 1 based button index.  
            // Users of the API should expect a 0 based index.  Subtract 1 
            // from the key to give the user the correct button number.
            key -= 1;
        }

        response res;
        res.port = port;
        res.button = key;
        res.pressed = was_pressed;
        res.reaction_time = response_time;
        res.key_state = state;
 
        response_queue_.push(res);
    }
}

std::size_t cedrus::xid_device_t::response_queue_size() const
{
    return response_queue_.size();
}

bool cedrus::xid_device_t::has_queued_responses() const
{
    return !response_queue_.empty();
}

void cedrus::xid_device_t::clear_response_queue()
{
    while(response_queue_.size() > 0)
    {
        response_queue_.pop();
    }
}

cedrus::response cedrus::xid_device_t::get_next_response()
{
    response res = response_queue_.front();
    response_queue_.pop();
    return res;
}

std::string cedrus::xid_device_t::get_device_name()
{
    return device_name_;
}

int cedrus::xid_device_t::get_product_id() const
{
    return product_id_;
}

int cedrus::xid_device_t::get_model_id() const
{
    return model_id_;
}

int cedrus::xid_device_t::get_button_count() const
{
    if(config_)
        return config_->number_of_lines();
    else
        return 8;
}

void cedrus::xid_device_t::init_device(const std::wstring &devconfig_path)
{
    product_id_ = get_xid_inquiry("_d2",1,100,100);

    switch(product_id_)
    {
    case 0:
        device_name_ = "Cedrus Lumina LP-400 Response Pad System";
        input_name_prefix_ = L"Button";
        break;
    case 1:
        device_name_ = "Cedrus SV-1 Voice Key";
        input_name_prefix_ = L"Voice Response";
        break;
    case 2:
        {
            model_id_ = get_xid_inquiry("_d3", 1, 100, 100);
            input_name_prefix_ = L"Button";
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
    case -99:
        device_name_ = "Invalid XID Device";
        input_name_prefix_ = L"Button";
        break;
    default:
        device_name_ = "Unknown XID device";
        input_name_prefix_ = L"Button";
        break;
    }

    if(!devconfig_path.empty())
    {
        config_ = xid_device_config_t::config_for_device(
          product_id_, model_id_, devconfig_path);

        xid_con_->set_needs_interbyte_delay(
            config_->needs_interbyte_delay());
    }
    else
    {
        xid_con_->set_needs_interbyte_delay(true);
    }
}

std::wstring cedrus::xid_device_t::input_name_prefix() const
{
    return input_name_prefix_;
}