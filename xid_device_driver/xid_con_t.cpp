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

#ifdef __APPLE__
#   define SLEEP_FUNC usleep
#   define SLEEP_INC 1000
#   define OS_DEPENDENT_LONG unsigned long
#elif defined(_WIN32)
#   include <windows.h>
#   define SLEEP_FUNC Sleep
#   define SLEEP_INC 1
#   define OS_DEPENDENT_LONG DWORD
#endif

cedrus::key_state cedrus::xid_con_t::xid_input_found()
{
    // Wisdom from Hisham, circa Aug 13, 2005. Point C is especially relevant.
    //
    // First, we try to determine if we have a valid packet. Our options
    // are limited; here what we look for:
    //
    // a. The first byte must be the letter 'k'
    //
    // b. Bits 0-3 of the second byte indicate the port number.  Lumina
    //    and RB-x30 models use only bits 0 and 1; SV-1 uses only bits
    //    1 and 2.  We check that the two remaining bits are zero.
    //
    // c. The remaining four bytes provide the reaction time.  Here, we'll
    //    assume that the RT will never exceed 4.66 hours :-) and verify
    //    that the last byte is set to 0.

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
                    char as_char[4];// what if it were a 64 bit int? would this still work?
                } rt;

                for(int n = 0; n < 4; ++n)
                {
                    // would this work on PPC? due to flipped endianness?
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

unsigned long cedrus::xid_con_t::GetTickCount() const
{
    boost::posix_time::ptime time_microseconds = boost::posix_time::microsec_clock::local_time();
    unsigned long milliseconds = static_cast<unsigned long>(time_microseconds.time_of_day().total_milliseconds());

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

void cedrus::xid_con_t::set_device_mode( int protocol )
{
    std::ostringstream s;
    s << "c1" << protocol;
    int bytes_written;

    write((unsigned char*)s.str().c_str(), s.str().length(), bytes_written);
}

void cedrus::xid_con_t::set_device_baud_rate( int rate )
{
    int bytes_written;
    char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    write((unsigned char*)change_baud_cmd, 3, bytes_written);

    switch ( rate )
    {
        case 0:
            set_baud_rate(9600);
            break;
        case 1:
            set_baud_rate(19200);
            break;
        case 2:
            set_baud_rate(38400);
            break;
        case 3:
            set_baud_rate(57600);
            break;
        case 4:
            set_baud_rate(115200);
            break;
        default:
            break;
    }
}

void cedrus::xid_con_t::get_product_and_model_id( int &product_id, int &model_id ) const
{
    char product_id_return[2];
    char model_id_return[2];

    send_xid_command(
        "_d2",
        product_id_return,
        sizeof(product_id_return));

    send_xid_command(
        "_d3",
        model_id_return,
        sizeof(model_id_return));

    product_id = (int)(product_id_return[0]);
    model_id = (int)(model_id_return[0]);
}

int cedrus::xid_con_t::get_inquiry(const char in_command[],
                                                  int expected_bytes_rec,
                                                  int timeout,
                                                  int delay)
{
    int return_value = INVALID_RETURN_VALUE;
    char return_info[20];
    int last_byte = expected_bytes_rec -1;

    int bytes_returned = send_xid_command(in_command,
                                          return_info,
                                          sizeof(return_info),
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

int cedrus::xid_con_t::get_baud_rate ( void ) const
{
    return port_settings_.baud_rate();
}

void cedrus::xid_con_t::set_baud_rate ( int rate )
{
    port_settings_.baud_rate(rate);
}

int cedrus::xid_con_t::send_xid_command(
    const char in_command[],
    char out_response[],
    int max_out_response_size,
    int timeout,
    int command_delay) const
{
    if(out_response != NULL)
    {
        memset(out_response, 0x00, max_out_response_size);
    }

    int bytes_written = 0;
    write((unsigned char*)in_command, strlen(in_command), bytes_written);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    int bytes_read = 0;
    int bytes_stored = 0;

    // sometimes sending a command needs a delay because the 4MHz processors
    // in the response pads need a little time to process the command and 
    // send a response.
    if(command_delay > 0)
        SLEEP_FUNC(command_delay*SLEEP_INC);

    OS_DEPENDENT_LONG current_time;
    OS_DEPENDENT_LONG start_time = GetTickCount();
    OS_DEPENDENT_LONG end_time = start_time + timeout;

    int status = 0;
    do
    {
        if(needs_interbyte_delay_)
            SLEEP_FUNC(delay_*SLEEP_INC);

        status = read(in_buff, sizeof(in_buff), bytes_read);

        if(status != NO_ERR)
            break;

        if(bytes_read >= 1)
        {
            for(int i = 0; (i<bytes_read) && (bytes_stored < max_out_response_size-1); ++i)
            {
                out_response[bytes_stored] = in_buff[i];
                bytes_stored++;
            }
        }
        current_time = GetTickCount();
    } while (current_time < end_time &&
             bytes_stored < max_out_response_size);

    if(out_response != NULL)
    {
        if (bytes_stored >= max_out_response_size )
        {
            assert(!"refusing to write a zero into a location one past the end of char array");
        }
        else
        {
            out_response[bytes_stored] = '\0';
        }
    }

    return bytes_stored;
}
