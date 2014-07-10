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

int cedrus::xid_con_t::get_baud_rate ( void ) const
{
    return baud_rate_;
}

void cedrus::xid_con_t::set_baud_rate ( int rate )
{
    switch ( rate )
    {
        case 0:
            baud_rate_ = 9600;
            break;
        case 1:
            baud_rate_ = 19200;
            break;
        case 2:
            baud_rate_ = 38400;
            break;
        case 3:
            baud_rate_ = 57600;
            break;
        case 4:
            baud_rate_ = 115200;
            break;
        default:
            break;
    }
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
