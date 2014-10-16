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

    void setup_dcb(DCB &dcb,
        int baud_rate,
        bytesize byte_size,
        bitparity bit_parity,
        stopbits stop_bits) const
    {
        dcb.BaudRate = baud_rate;
        dcb.ByteSize = static_cast<BYTE>(byte_size);
        dcb.Parity   = static_cast<BYTE>(bit_parity);
        dcb.StopBits = static_cast<BYTE>(stop_bits);
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
    bytesize byte_size,
    bitparity bit_parity,
    stopbits stop_bits
    )
    : baud_rate_(port_speed),
      byte_size_(byte_size),
      bit_parity_(bit_parity),
      stop_bits_(stop_bits),
      handshaking_(HANDSHAKE_NONE),
      port_name_(port_name),
      m_connection_dead (false),
      m_winPimpl( new WindowsConnPimpl )
{
    std::ostringstream s;
    s << "\\\\.\\" << port_name;
    port_name_ = s.str().c_str();
}


cedrus::xid_con_t::~xid_con_t(void)
{
    if(m_winPimpl->device_id_ != 0)
        close();
}

bool cedrus::xid_con_t::close()
{
    bool status = ( CloseHandle(m_winPimpl->device_id_) != 0 );
    m_winPimpl->device_id_ = 0;
    return status;
}

bool cedrus::xid_con_t::flush_write_to_device_buffer()
{
    return (PurgeComm(m_winPimpl->device_id_, PURGE_RXABORT|PURGE_RXCLEAR) != 0);
}

bool cedrus::xid_con_t::flush_read_from_device_buffer()
{
    return (PurgeComm(m_winPimpl->device_id_, PURGE_TXABORT|PURGE_TXCLEAR) != 0);
}

int cedrus::xid_con_t::open()
{
    int status = XID_NO_ERR;

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
        status = XID_PORT_NOT_AVAILABLE;
    }
    else
    {
        if ( !setup_com_port() )
            status = XID_ERROR_SETTING_UP_PORT;

        PurgeComm(m_winPimpl->device_id_,
            PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);
    }

    return status;
}

bool cedrus::xid_con_t::setup_com_port()
{
    DCB dcb;
    bool status = false;

    if( SetupComm(m_winPimpl->device_id_, IN_BUFFER_SIZE, OUT_BUFFER_SIZE) == 0 )
        return status;

    if( GetCommState(m_winPimpl->device_id_, &dcb) == 0 )
        return status;

    m_winPimpl->setup_dcb(dcb, baud_rate_, byte_size_, bit_parity_, stop_bits_);

    if( SetCommState(m_winPimpl->device_id_, &dcb) == 0 )
        return status;

    COMMTIMEOUTS ct;
    if( GetCommTimeouts(m_winPimpl->device_id_, &ct) == 0 )
        return status;

    m_winPimpl->setup_timeouts(ct);

    if( SetCommTimeouts(m_winPimpl->device_id_, &ct) == 0 )
        return status;

    status = flush_write_to_device_buffer();
    if(status)
        status = flush_read_from_device_buffer();

    return status;
}

bool cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int *bytes_read)
{
    DWORD read = 0;
    bool status = (ReadFile(m_winPimpl->device_id_, in_buffer, bytes_to_read, &read, NULL) != 0);

    if ( status )
        *bytes_read = read;
    else
    {
        if ( GetLastError() == ERROR_ACCESS_DENIED )
            m_connection_dead = true;
    }

    return status;
}

bool cedrus::xid_con_t::write(
    unsigned char * const in_buffer,
    int bytes_to_write,
    int *bytes_written)
{
    unsigned char *p = in_buffer;
    bool status = true;
    DWORD written = 0;

    for(int i = 0; i < bytes_to_write && status; ++i)
    {
        DWORD  byte_count;
        status = (WriteFile(m_winPimpl->device_id_, p, 1, &byte_count, NULL) != 0);
        if( !status  )
        {
            if ( GetLastError() == ERROR_ACCESS_DENIED )
                m_connection_dead = true;
            break;
        }

        written += byte_count;

        /* 
        This used to be governed by a devconfig flag on a per-device basis.
        However, the entire thing wasn't documented, and different versions
        of devconfigs had conflicting information on the subject; with some
        devices apparently needing it, contrary to the claim, and vice versa.

        It's safer to just do this for every XID device, as the only time-
        sensitive use for the library is reading responses, and no writing
        takes place at that time.
        */
        Sleep(INTERBYTE_DELAY);

        if(written == bytes_to_write)
            break;

        ++p;
    }

    *bytes_written = written;

    return status;
}
