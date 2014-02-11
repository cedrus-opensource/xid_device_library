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

#include "xid_con_t.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <boost/timer.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

cedrus::key_state cedrus::xid_con_t::xid_input_found()
{
    key_state input_found = NO_KEY_DETECTED;

    if(bytes_in_buffer_ >= 6)
    {
        const int last_byte_index = bytes_in_buffer_ - XID_PACKET_SIZE;

        for(int i = 0; i <= last_byte_index; ++i)
        {
            if(input_buffer_[i] == 'k' &&
               (input_buffer_[i+1] & INVALID_PORT_BITS) == 0 &&
               input_buffer_[i+5] == '\0')
            {
                // found a valid XID packet
                first_valid_xid_packet_ = i;
                last_resp_pressed_ = (
                    input_buffer_[first_valid_xid_packet_+1] & KEY_RELEASE_BITMASK) ==
                    KEY_RELEASE_BITMASK;
                last_resp_port_ = 
                    input_buffer_[first_valid_xid_packet_+1] & 0x0F;

                last_resp_key_ = (
                    input_buffer_[first_valid_xid_packet_+1] & 0xE0) >> 5;

                if(last_resp_key_ == 0)
                    last_resp_key_ = 8;
                
                // get reaction time
                union {
                    int as_int;
                    char as_char[4];
                } rt;

                for(int n = 0; n < 4; ++n)
                {
                    rt.as_char[n] = input_buffer_[first_valid_xid_packet_+2+n];
                }
                
                last_resp_rt_ = rt.as_int;

                input_found = static_cast<key_state>(
                    FOUND_KEY_DOWN + !last_resp_pressed_);

                if(input_found == FOUND_KEY_DOWN)
                    num_keys_down_++;
                else
                    num_keys_down_--;
            }
        }
    }
    
    return input_found;
}

cedrus::key_state cedrus::xid_con_t::check_for_keypress()
{
    int bytes_read = 0;
    int status = NO_ERR; 
    key_state response_found = NO_KEY_DETECTED;

    status = read(&input_buffer_[bytes_in_buffer_], 6, bytes_read);

    if(bytes_read > 0)
    {
        bytes_in_buffer_ += bytes_read;
        response_found = xid_input_found();
    }

    return response_found;
}

void cedrus::xid_con_t::remove_current_response()
{
    if(first_valid_xid_packet_ != INVALID_PACKET_INDEX)
    {
        unsigned char *dest_char = input_buffer_;
        unsigned char *src_char  = &input_buffer_[first_valid_xid_packet_];
        int num_bytes = bytes_in_buffer_ - first_valid_xid_packet_;

        for(int i = 0; i < num_bytes; ++i, ++dest_char, ++src_char)
        {
            dest_char = src_char;
        }

        bytes_in_buffer_ -= num_bytes;
        first_valid_xid_packet_ = INVALID_PACKET_INDEX;
    }
}

void cedrus::xid_con_t::get_current_response(
    int &port, int &key, bool &was_pressed, int &reaction_time)
{
    if(first_valid_xid_packet_ != INVALID_PACKET_INDEX)
    {
        port = last_resp_port_;
        key  = last_resp_key_;
        was_pressed = last_resp_pressed_;
        reaction_time = last_resp_rt_;

        remove_current_response();
    }
    else
    {
        port = -1;
        key  = -1;
        was_pressed = false;
        reaction_time = -1;
    }
}

int cedrus::xid_con_t::get_number_of_keys_down()
{
    return num_keys_down_;
}

unsigned long cedrus::xid_con_t::GetTickCount()
{
    boost::posix_time::ptime time_microseconds = boost::posix_time::microsec_clock::local_time();
    unsigned long milliseconds = time_microseconds.time_of_day().total_milliseconds();

    return milliseconds;
}

void cedrus::xid_con_t::set_needs_interbyte_delay(bool needs_delay)
{
    needs_interbyte_delay_ = needs_delay;
}

void cedrus::xid_con_t::set_digital_out_prefix(char prefix)
{
    set_lines_cmd_[0] = prefix;
}

void cedrus::xid_con_t::set_digital_output_lines(
    unsigned int lines,
    bool leave_remaining_lines)
{
    if(lines > 255)
        return;

    int bytes_written;

    if(leave_remaining_lines)
        lines |= lines_state_;

    if(set_lines_cmd_[0] == 'a')
    {
        set_lines_cmd_[2] = ~lines;
    }
    else
    {
        set_lines_cmd_[2] = lines;
    }

    write((unsigned char*)set_lines_cmd_, 4, bytes_written);
    lines_state_ = lines;
}

void cedrus::xid_con_t::clear_digital_output_lines(
    unsigned int lines,
    bool leave_remaining_lines)
{
    if(lines > 255)
        return;

    int bytes_written;
    lines = ~lines;

    if(leave_remaining_lines)
        lines = lines & lines_state_;

    if(set_lines_cmd_[0] == 'a')
    {
        set_lines_cmd_[2] = ~lines;
    }
    else
    {
        set_lines_cmd_[2] = lines;
    }

    write((unsigned char*)set_lines_cmd_, 4, bytes_written);
    lines_state_ = lines;
}
