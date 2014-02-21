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
    boost::shared_ptr<xid_device_config_t> dev_config)
    : base_device_t(xid_con, dev_config),
      button_count_(8),
      input_name_prefix_("Button")
{
    init_response_device();
}

cedrus::xid_device_t::~xid_device_t(void)
{
}

int cedrus::xid_device_t::get_button_count() const
{
    if(config_)
        return config_->number_of_lines();
    else
        return 8;
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

std::string cedrus::xid_device_t::input_name_prefix() const
{
    return input_name_prefix_;
}

void cedrus::xid_device_t::init_response_device()
{
    switch(get_product_id())
    {
    case 1:
        {
            input_name_prefix_ = "Voice Response";
            break;
        }
    default:
        {
            input_name_prefix_ = "Button";
            break;
        }
    }
}
