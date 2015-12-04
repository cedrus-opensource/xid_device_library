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

#include "constants.h"

// Not defined on mac, probably temporary
#define MAXDWORD 0xffffffff

struct cedrus::xid_con_t::WindowsConnPimpl
{
    WindowsConnPimpl()
        : m_deviceId ( NULL )
    {}

    FT_HANDLE m_deviceId;
};

cedrus::xid_con_t::xid_con_t(
    const DWORD location,
    DWORD port_speed,
    BYTE byte_size,
    BYTE bit_parity,
    BYTE stop_bits
    )
    : baud_rate_(port_speed),
      byte_size_(byte_size),
      bit_parity_(bit_parity),
      stop_bits_(stop_bits),
      m_location(location),
      m_connection_dead (false),
      m_winPimpl( new WindowsConnPimpl )
{
}

cedrus::xid_con_t::~xid_con_t(void)
{
    if(m_winPimpl->m_deviceId != 0)
        close();
}

bool cedrus::xid_con_t::close()
{
    bool status = ( FT_Close(m_winPimpl->m_deviceId) == FT_OK );
    m_winPimpl->m_deviceId = 0;
    return status;
}

bool cedrus::xid_con_t::flush_write_to_device_buffer()
{
    return (FT_Purge(m_winPimpl->m_deviceId, FT_PURGE_RX) == FT_OK);
}

bool cedrus::xid_con_t::flush_read_from_device_buffer()
{
    return (FT_Purge(m_winPimpl->m_deviceId, FT_PURGE_TX) == FT_OK);
}

int cedrus::xid_con_t::open()
{
    int status = XID_NO_ERR;

    DWORD open_success = FT_OpenEx(
              (PVOID)m_location,
              FT_OPEN_BY_LOCATION,
              &m_winPimpl->m_deviceId);

    if( open_success != FT_OK )
    {
        m_winPimpl->m_deviceId = 0;
        status = XID_PORT_NOT_AVAILABLE;
    }
    else
    {
        if ( !setup_com_port() )
            status = XID_ERROR_SETTING_UP_PORT;

        FT_Purge(m_winPimpl->m_deviceId, FT_PURGE_RX|FT_PURGE_TX);
    }

    return status;
}

bool cedrus::xid_con_t::setup_com_port()
{
    bool status = false;

    FT_SetBaudRate(m_winPimpl->m_deviceId, baud_rate_);
    FT_SetDataCharacteristics(m_winPimpl->m_deviceId, byte_size_, stop_bits_, bit_parity_);

    FT_SetTimeouts(m_winPimpl->m_deviceId, 1, 500);
    FT_SetUSBParameters(m_winPimpl->m_deviceId, 64, 64);

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
    bool status = false;
    DWORD read = 0;

    status = (FT_Read(m_winPimpl->m_deviceId, in_buffer, bytes_to_read, &read) == FT_OK);

    if ( status )
        *bytes_read = read;
    else
    {
        // The call will no longer be needed: use the status return instead.
        //int error_code = FT_W32_GetLastError(m_winPimpl->m_deviceId);
        //if ( error_code == FT_INVALID_HANDLE )
        //    m_connection_dead = true;
    }

    return status;
}

// We can avoid having to do an actual read by checking if there's anything to be read first.
// I don't know if that's actually faster or not, according to the docs this is primarily for
// threading purposes.
/*
bool cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int *bytes_read)
{
    bool status = false;
    DWORD read = 0;

    DWORD event_dword;
    DWORD rx_bytes;
    DWORD tx_bytes;
    status = (FT_GetStatus(m_winPimpl->m_deviceId, &rx_bytes, &tx_bytes, &event_dword) == FT_OK);

    unsigned int bytes_to_actually_read = 0;

    if ( rx_bytes < bytes_to_read )
        bytes_to_actually_read = rx_bytes;
    else
        bytes_to_actually_read = bytes_to_read;

    if ( bytes_to_actually_read > 0 )
        status = (FT_Read(m_winPimpl->m_deviceId, in_buffer, bytes_to_actually_read, &read) == FT_OK);

    if ( status )
        *bytes_read = read;
    else
    {
        // The call will no longer be needed: use the status return instead.
        //int error_code = FT_W32_GetLastError(m_winPimpl->m_deviceId);
        //if ( error_code == FT_INVALID_HANDLE )
        //    m_connection_dead = true;
    }

    return status;
}
*/

bool cedrus::xid_con_t::write(
    unsigned char * const in_buffer,
    int bytes_to_write,
    int *bytes_written)
{
    unsigned char *p = in_buffer;
    bool status = false;
    DWORD written = 0;

    for(int i = 0; i < bytes_to_write; ++i)
    {
        DWORD byte_count;
        status = (FT_Write(m_winPimpl->m_deviceId, p, 1, &byte_count) == FT_OK);
        if( !status )
        {
            //int error_code = FT_W32_GetLastError(m_winPimpl->m_deviceId);
            //if ( /*error_code == ERROR_ACCESS_DENIED ||*/ error_code == FT_INVALID_HANDLE )
            //    m_connection_dead = true;
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
        SLEEP_FUNC(INTERBYTE_DELAY);

        if(written == bytes_to_write)
            break;

        ++p;
    }

    *bytes_written = written;

    return status;
}
