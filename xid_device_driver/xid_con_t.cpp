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
#include <boost/date_time/posix_time/posix_time.hpp>

cedrus::xid_con_t::xid_con_t(
    const DWORD location,
    DWORD port_speed,
    BYTE byte_size,
    BYTE bit_parity,
    BYTE stop_bits
    )
    : m_baudRate(port_speed),
      m_byteSize(byte_size),
      m_bitParity(bit_parity),
      m_stopBits(stop_bits),
      m_location(location),
      m_connection_dead (false),
      m_deviceHandle(nullptr)
{
}

cedrus::xid_con_t::~xid_con_t(void)
{
    if(m_deviceHandle != nullptr)
        close();
}

bool cedrus::xid_con_t::close()
{
    DWORD close_status = FT_OK;

    // Don't bother if the handle is already null
    if ( m_deviceHandle != nullptr )
    {
        close_status = FT_Close(m_deviceHandle);
        m_deviceHandle = nullptr;
    }

    return close_status == FT_OK;
}

bool cedrus::xid_con_t::flush_write_to_device_buffer()
{
    return (FT_Purge(m_deviceHandle, FT_PURGE_TX) == FT_OK);
}

bool cedrus::xid_con_t::flush_read_from_device_buffer()
{
    return (FT_Purge(m_deviceHandle, FT_PURGE_RX) == FT_OK);
}

int cedrus::xid_con_t::open()
{
    int status = XID_NO_ERR;

    // Erring on the side of caution in case we already have a handle.
    close();

    DWORD open_success = FT_OpenEx(
              (PVOID)m_location,
              FT_OPEN_BY_LOCATION,
              &m_deviceHandle);

    if( open_success != FT_OK )
    {
        status = XID_PORT_NOT_AVAILABLE;
    }
    else
    {
        m_connection_dead = false;
        if ( !setup_com_port() )
            status = XID_ERROR_SETTING_UP_PORT;

        FT_Purge(m_deviceHandle, FT_PURGE_RX|FT_PURGE_TX);
    }

    return status;
}

bool cedrus::xid_con_t::setup_com_port()
{
    bool status = false;

    FT_SetBaudRate(m_deviceHandle, m_baudRate);
    FT_SetDataCharacteristics(m_deviceHandle, m_byteSize, m_stopBits, m_bitParity);

    FT_SetTimeouts(m_deviceHandle, 1, 500);
    FT_SetUSBParameters(m_deviceHandle, 64, 64);
    FT_SetLatencyTimer(m_deviceHandle, 2);

    status = flush_write_to_device_buffer();
    if(status)
        status = flush_read_from_device_buffer();

    return status;
}

bool cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    DWORD bytes_to_read,
    LPDWORD bytes_read)
{
    DWORD read_status = FT_OK;

    read_status = FT_Read(m_deviceHandle, in_buffer, bytes_to_read, bytes_read);

    if ( read_status != FT_OK )
    {
        // We used to check for specific error codes here, but I'm not certain why.
        // I don't know that any of them are errors you can recover from, so let's
        // err on the side of caution here.
        m_connection_dead = true;
    }

    return read_status == FT_OK;
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
    status = (FT_GetStatus(m_deviceHandle, &rx_bytes, &tx_bytes, &event_dword) == FT_OK);

    unsigned int bytes_to_actually_read = 0;

    if ( rx_bytes < bytes_to_read )
        bytes_to_actually_read = rx_bytes;
    else
        bytes_to_actually_read = bytes_to_read;

    if ( bytes_to_actually_read > 0 )
        status = (FT_Read(m_deviceHandle, in_buffer, bytes_to_actually_read, &read) == FT_OK);

    if ( status )
        *bytes_read = read;
    else
    {
        // We used to check for specific error codes here, but I'm not certain why.
        // I don't know that any of them are errors you can recover from, so let's
        // err on the side of caution here.
        m_connection_dead = true;
    }

    return status;
}
*/

bool cedrus::xid_con_t::write(
    unsigned char * const in_buffer,
    DWORD bytes_to_write,
    LPDWORD bytes_written)
{
    unsigned char *p = in_buffer;
    DWORD write_status = FT_OK;
    DWORD written = 0;

    for(unsigned int i = 0; i < bytes_to_write; ++i)
    {
        DWORD byte_count;
        write_status = FT_Write(m_deviceHandle, p, 1, &byte_count);
        if( write_status != FT_OK )
        {
            // We used to check for specific error codes here, but I'm not certain why.
            // I don't know that any of them are errors you can recover from, so let's
            // err on the side of caution here.
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
        SLEEP_FUNC(INTERBYTE_DELAY*SLEEP_INC);

        if(written == bytes_to_write)
            break;

        ++p;
    }

    *bytes_written = written;

    return write_status == FT_OK;
}

unsigned long cedrus::xid_con_t::GetTickCount() const
{
    boost::posix_time::ptime time_microseconds = boost::posix_time::microsec_clock::local_time();
    unsigned long milliseconds = static_cast<unsigned long>(time_microseconds.time_of_day().total_milliseconds());

    return milliseconds;
}

int cedrus::xid_con_t::get_baud_rate ( void ) const
{
    return m_baudRate;
}

void cedrus::xid_con_t::set_baud_rate ( unsigned char rate )
{
    switch ( rate )
    {
        case 0:
            m_baudRate = 9600;
            break;
        case 1:
            m_baudRate = 19200;
            break;
        case 2:
            m_baudRate = 38400;
            break;
        case 3:
            m_baudRate = 57600;
            break;
        case 4:
            m_baudRate = 115200;
            break;
        default:
            break;
    }
}

bool cedrus::xid_con_t::has_lost_connection( void )
{
    return m_connection_dead;
}

DWORD cedrus::xid_con_t::send_xid_command(
    const char in_command[],
    unsigned char out_response[],
    unsigned int max_out_response_size)
{
    // 15 has been determined to be enough for most commands
    return send_xid_command(in_command, out_response, max_out_response_size, 100);
}

DWORD cedrus::xid_con_t::send_xid_command_slow(
    const char in_command[],
    unsigned char out_response[],
    unsigned int max_out_response_size)
{
    // Some commands, like _aa apparently need some extra leeway. 50 has been
    // enough thus far.
    return send_xid_command(in_command, out_response, max_out_response_size, 200);
}


DWORD cedrus::xid_con_t::send_xid_command(
    const char in_command[],
    unsigned char out_response[],
    unsigned int max_out_response_size,
    unsigned int num_retries)
{
    if(out_response != NULL)
        memset(out_response, 0x00, max_out_response_size);

    DWORD bytes_written = 0;
    write((unsigned char*)in_command, strlen(in_command), &bytes_written);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    DWORD bytes_read = 0;
    DWORD bytes_stored = 0;

    unsigned int i = 0;
    do
    {
        SLEEP_FUNC(INTERBYTE_DELAY*SLEEP_INC);

        if( !read(in_buff, sizeof(in_buff), &bytes_read) )
            break;

        if(bytes_read >= 1)
        {
            for(unsigned int j = 0; (j < bytes_read) && (bytes_stored < max_out_response_size); ++j)
            {
                out_response[bytes_stored] = in_buff[j];
                bytes_stored++;
            }
        }

        ++i;
    } while (i < num_retries && bytes_stored < max_out_response_size);

    return bytes_stored;
}

DWORD cedrus::xid_con_t::send_xid_command_pst_proof(
    const char in_command[],
    unsigned char out_response[],
    unsigned int max_out_response_size)
{
    if(out_response != NULL)
        memset(out_response, 0x00, max_out_response_size);

    DWORD bytes_written = 0;
    write((unsigned char*)in_command, strlen(in_command), &bytes_written);

    unsigned char in_buff[64];
    memset(in_buff, 0x00, sizeof(in_buff));
    DWORD bytes_read = 0;
    DWORD bytes_stored = 0;

    // sometimes sending a command needs a delay because the 4MHz processors
    // in the response pads need a little time to process the command and
    // send a response.
    SLEEP_FUNC(100*SLEEP_INC);

    unsigned int num_retries = 0;
    do
    {
        // This isn't actually related to the interbyte delay, but the wait is the same.
        SLEEP_FUNC(INTERBYTE_DELAY*SLEEP_INC);

        // We're reading from the buffer in chunks of 64 because of all the potential zeroes.
        if( !read(in_buff, sizeof(in_buff), &bytes_read) )
            break;

        if(bytes_read >= 1)
        {
            for(unsigned int i = 0; (i < bytes_read) && (bytes_stored < max_out_response_size); ++i)
            {
                // Clean out the potential zeroes in the buffer.
                // NOTE: THIS ONLY WORKS AS LONG AS NOTHING CAN RETURN NULL
                // AS A VALID RESPONSE.
                if(in_buff[i] != 0)
                {
                    out_response[bytes_stored] = in_buff[i];
                    bytes_stored++;
                }
            }
        }

        ++num_retries;
    } while (num_retries < 50 && bytes_stored < max_out_response_size);

    return bytes_stored;
}
