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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sstream>

#include <windows.h>

#include "constants.h"

struct cedrus::xid_con_t::WindowsConnPimpl
{
    WindowsConnPimpl()
        : device_id_ ( NULL )
    {}

    void setup_dcb(DCB &dcb, port_settings_t port_settings) const
    {
        dcb.BaudRate = port_settings.baud_rate();
        dcb.ByteSize = static_cast<BYTE>(port_settings.byte_size());
        dcb.Parity   = static_cast<BYTE>(port_settings.bit_parity());
        dcb.StopBits = static_cast<BYTE>(port_settings.stop_bits());
        dcb.fBinary  = 1;
    }

    void setup_timeouts(COMMTIMEOUTS &ct) const
    {
        ct.ReadIntervalTimeout         = MAXDWORD;
        ct.ReadTotalTimeoutConstant    = 0;
        ct.ReadTotalTimeoutMultiplier  = 0;
        ct.WriteTotalTimeoutConstant   = 0;
        ct.WriteTotalTimeoutMultiplier = 500;
    }

    HANDLE device_id_;
};

cedrus::xid_con_t::xid_con_t(
    const std::string &port_name,
    int port_speed,
    int delay_ms,
    port_settings_t::bytesize byte_size,
    port_settings_t::bitparity bit_parity,
    port_settings_t::stopbits stop_bits
    )
    : delay_(delay_ms),
      bytes_in_buffer_(0),
      first_valid_xid_packet_(INVALID_PACKET_INDEX),
      num_keys_down_(0),
      last_resp_port_(-1),
      last_resp_key_(-1),
      last_resp_pressed_(false),
      last_resp_rt_(-1),
      lines_state_(0),
      needs_interbyte_delay_(true),
      m_winPimpl( new WindowsConnPimpl )
{
    std::ostringstream s;
    s << "\\\\.\\" << port_name;
    port_name_ = s.str().c_str();

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
    if(m_winPimpl->device_id_ != 0)
        close();
}

int cedrus::xid_con_t::close()
{
    if(CloseHandle(m_winPimpl->device_id_) == 0)
        return ERROR_CLOSING_PORT;

    m_winPimpl->device_id_ = 0;
    return NO_ERR;
}

int cedrus::xid_con_t::flush_input()
{
    int status = NO_ERR;

    if(PurgeComm(m_winPimpl->device_id_, PURGE_RXABORT|PURGE_RXCLEAR) == 0)
    {
        status = ERROR_FLUSHING_PORT;
    }

    return status;
}

int cedrus::xid_con_t::flush_output()
{
    int status = NO_ERR;

    if(PurgeComm(m_winPimpl->device_id_, PURGE_TXABORT|PURGE_TXCLEAR) == 0)
    {
        status = ERROR_FLUSHING_PORT;
    }

    return status;
}

int cedrus::xid_con_t::open()
{
    int status = NO_ERR;

    std::wstring name( port_name_.begin(), port_name_.end() );
    const wchar_t* wchar_name = name.c_str();

    m_winPimpl->device_id_ = CreateFile(
        wchar_name,
        GENERIC_READ|GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        0);

    if(m_winPimpl->device_id_ == INVALID_HANDLE_VALUE)
    {
        m_winPimpl->device_id_ = 0;
        status = PORT_NOT_AVAILABLE;
    }
    else
    {
        status = setup_com_port();
        PurgeComm(m_winPimpl->device_id_,
            PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);
    }

    return status;
}

int cedrus::xid_con_t::setup_com_port()
{
    DCB dcb;
    int status = NO_ERR;

    if(SetupComm(m_winPimpl->device_id_, IN_BUFFER_SIZE, OUT_BUFFER_SIZE) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    if(GetCommState(m_winPimpl->device_id_, &dcb) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    m_winPimpl->setup_dcb(dcb, port_settings_);

    if(SetCommState(m_winPimpl->device_id_, &dcb) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    COMMTIMEOUTS ct;
    if(GetCommTimeouts(m_winPimpl->device_id_, &ct) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    m_winPimpl->setup_timeouts(ct);

    if(SetCommTimeouts(m_winPimpl->device_id_, &ct) == 0)
    {
        status = ERROR_SETTING_UP_PORT;
        return status;
    }

    status = flush_input();
    if(status == NO_ERR)
        status = flush_output();

    return status;
}

int cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int &bytes_read)
{
    DWORD read = 0;

    int status = NO_ERR;

    if(ReadFile(m_winPimpl->device_id_, in_buffer, bytes_to_read, &read, NULL) == 0)
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
            if(WriteFile(m_winPimpl->device_id_, p, 1, &byte_count, NULL) == 0)
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
        if(WriteFile(m_winPimpl->device_id_, p, bytes_to_write, &written, NULL) == 0)
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
