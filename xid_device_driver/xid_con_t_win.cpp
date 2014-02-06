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

cedrus::xid_con_t::xid_con_t(
    const std::wstring &port_name,
    int port_speed,
    int delay_ms,
    port_settings_t::bytesize byte_size,
    port_settings_t::bitparity bit_parity,
    port_settings_t::stopbits stop_bits
    )
    : in_buffer_size_(2048),
      out_buffer_size_(2048),
      model_id_(0),
      device_id_(NULL),
      delay_(delay_ms),
      bytes_in_buffer_(0),
      invalid_port_bits_(0x0C),
      timer_rate_(1),
      first_valid_xid_packet_(INVALID_PACKET_INDEX),
      num_keys_down_(0),
      last_resp_port_(-1),
      last_resp_key_(-1),
      last_resp_pressed_(false),
      last_resp_rt_(-1),
      lines_state_(0),
      needs_interbyte_delay_(true)

{
    wsprintf(port_name_, L"\\\\.\\%s", port_name.c_str());
    port_settings_ = port_settings_t(port_name_, 
                                     port_speed,
                                     byte_size,
                                     bit_parity,
                                     stop_bits);

    for(int i = 0; i < INPUT_BUFFER_SIZE; ++i)
    {
        input_buffer_[i] = '\0';
    }

    set_lines_cmd_[0] = 'a';
    set_lines_cmd_[1] = 'h';
    set_lines_cmd_[2] = '\0';
    set_lines_cmd_[3] = '\0';
}


cedrus::xid_con_t::~xid_con_t(void)
{
    if(device_id_ != 0)
        close();
}

int cedrus::xid_con_t::close()
{
    if(CloseHandle(device_id_) == 0)
        return ERROR_CLOSING_PORT;

    device_id_ = 0;
    return NO_ERR;
}

int cedrus::xid_con_t::flush_input()
{
    int status = NO_ERR;

    if(PurgeComm(device_id_, PURGE_RXABORT|PURGE_RXCLEAR) == 0)
    {
        status = ERROR_FLUSHING_PORT;
    }

    return status;
}

int cedrus::xid_con_t::flush_output()
{
    int status = NO_ERR;

    if(PurgeComm(device_id_, PURGE_TXABORT|PURGE_TXCLEAR) == 0)
    {
        status = ERROR_FLUSHING_PORT;
    }

    return status;
}

int cedrus::xid_con_t::open()
{
    int status = NO_ERR;

    device_id_ = CreateFile(
        port_name_,
        GENERIC_READ|GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        0);

    if(device_id_ == INVALID_HANDLE_VALUE)
    {
        device_id_ = 0;
        status = PORT_NOT_AVAILABLE;
    }
    else
    {
        status = setup_com_port();
        PurgeComm(device_id_,
            PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);
    }

    return status;
}

int cedrus::xid_con_t::setup_com_port()
{
    DCB dcb;
    int status = NO_ERR;

    if(SetupComm(device_id_, in_buffer_size_, out_buffer_size_) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    if(GetCommState(device_id_, &dcb) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    setup_dcb(dcb);

    if(SetCommState(device_id_, &dcb) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    COMMTIMEOUTS ct;
    if(GetCommTimeouts(device_id_, &ct) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    setup_timeouts(ct);
    
    if(SetCommTimeouts(device_id_, &ct) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    status = flush_input();
    if(status == NO_ERR)
        status = flush_output();

    return status;
}

void cedrus::xid_con_t::setup_dcb(DCB &dcb) const
{
    dcb.BaudRate = port_settings_.baud_rate();
    dcb.ByteSize = static_cast<BYTE>(port_settings_.byte_size());
    dcb.Parity   = static_cast<BYTE>(port_settings_.bit_parity());
    dcb.StopBits = static_cast<BYTE>(port_settings_.stop_bits());
    dcb.fBinary  = 1;
}

void cedrus::xid_con_t::setup_timeouts(COMMTIMEOUTS &ct) const
{
    ct.ReadIntervalTimeout         = MAXDWORD;
    ct.ReadTotalTimeoutConstant    = 0;
    ct.ReadTotalTimeoutMultiplier  = 0;
    ct.WriteTotalTimeoutConstant   = 0;
    ct.WriteTotalTimeoutMultiplier = 500;
}

int cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int &bytes_read)
{
    DWORD read = 0;
        
    int status = NO_ERR;

    if(ReadFile(device_id_, in_buffer, bytes_to_read, &read, NULL) == 0)
    {
        status = ERROR_READING_PORT;
    }
    else
    {
        bytes_read = read;
    }
    
    return status;
}

int cedrus::xid_con_t::write(
    unsigned char * const in_buffer,
    int bytes_to_write,
    int &bytes_written)
{
    unsigned char *p = in_buffer;
    int status = NO_ERR;
    DWORD written = 0;
    
    if(needs_interbyte_delay_)
    {
        for(int i = 0; i < bytes_to_write && status == NO_ERR; ++i)
        {
            DWORD  byte_count;
            if(WriteFile(device_id_, p, 1, &byte_count, NULL) == 0)
            {
                status = ERROR_WRITING_TO_PORT;
                break;
            }
            
            written += byte_count;

            if(written == bytes_to_write)
                break;

            Sleep(delay_);

            ++p;
        }
        bytes_written = written;
    }
    else
    {
        if(WriteFile(device_id_, p, bytes_to_write, &written, NULL) == 0)
        {
            status = ERROR_WRITING_TO_PORT;
        }
        else
        {
            bytes_written = written;
        }
    }

    return status;
}

cedrus::key_state cedrus::xid_con_t::xid_input_found()
{
    key_state input_found = NO_KEY_DETECTED;

    if(bytes_in_buffer_ >= 6)
    {
        const int last_byte_index = bytes_in_buffer_ - XID_PACKET_SIZE;

        for(int i = 0; i <= last_byte_index; ++i)
        {
            if(input_buffer_[i] == 'k' &&
               (input_buffer_[i+1] & invalid_port_bits_) == 0 &&
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

int cedrus::xid_con_t::send_xid_command(
    const char in_command[],
    int num_bytes,
    char out_response[],
    int max_out_response_size,
    int expected_bytes_rec,
    int timeout,
    int command_delay)
{
    if(out_response != NULL)
    {
        memset(out_response, 0x00, max_out_response_size);
    }

    if(num_bytes == 0)
    {
        num_bytes = strlen(in_command);
    }

    int bytes_written = 0;
    write((unsigned char*)in_command, num_bytes, bytes_written);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    int bytes_read = 0;
    int bytes_stored = 0;

    // sometimes sending a command needs a delay because the 4MHz processors
    // in the response pads need a little time to process the command and 
    // send a response.
    if(command_delay > 0)
        Sleep(command_delay);

    DWORD current_time;
    DWORD start_time = GetTickCount();
    DWORD end_time = start_time + timeout;

    int status = 0;
    do
    {
        if(needs_interbyte_delay_)
            Sleep(delay_);

        status = read(
            in_buff, sizeof(in_buff), bytes_read);

        if(status != NO_ERR)
            break;

        if(bytes_read >= 1)
        {
            for(int i = 0; (i<bytes_read) && (bytes_stored < max_out_response_size);
                ++i)
            {
                out_response[bytes_stored] = in_buff[i];
                bytes_stored++;
            }
        }
        current_time = GetTickCount();
    } while (current_time < end_time &&
             bytes_stored < max_out_response_size &&
             bytes_stored < expected_bytes_rec);

    if(out_response != NULL)
        out_response[bytes_stored] = '\0';

    return bytes_stored;
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
    if(lines < 0 || lines > 255)
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
    if(lines < 0 || lines > 255)
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