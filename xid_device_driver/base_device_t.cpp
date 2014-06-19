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

#include "base_device_t.h"

#include "xid_con_t.h"
#include "constants.h"
#include <sstream>


cedrus::base_device_t::~base_device_t()
{}

int cedrus::base_device_t::get_outpost_model( void )
{
    return INVALID_RETURN_VALUE;
}

int cedrus::base_device_t::get_hardware_generation( void )
{
    return INVALID_RETURN_VALUE;
}

int cedrus::base_device_t::get_light_sensor_mode( void )
{
    return INVALID_RETURN_VALUE;
}

void cedrus::base_device_t::set_light_sensor_mode(  int mode )
{
    // nothing
}

void cedrus::base_device_t::set_light_sensor_threshold( int threshold )
{
    // nothing
}

int cedrus::base_device_t::get_light_sensor_threshold()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::base_device_t::set_scanner_trigger_filter( int mode )
{
    // nothing
}

void cedrus::base_device_t::reset_rt_timer( void )
{
    // nothing
}

void cedrus::base_device_t::reset_base_timer( void )
{
    // nothing
}

int cedrus::base_device_t::query_base_timer()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::base_device_t::set_pulse_duration(unsigned int duration)
{
    // nothing
}

void cedrus::base_device_t::poll_for_response()
{
    // nothing
}

bool cedrus::base_device_t::has_queued_responses()
{
    return false;
}

cedrus::response cedrus::base_device_t::get_next_response()
{
    return cedrus::response();
}

int cedrus::base_device_t::get_accessory_connector_mode( void )
{
    return INVALID_RETURN_VALUE;
}

void cedrus::base_device_t::set_accessory_connector_mode( int mode )
{
    // nothing
}

int cedrus::base_device_t::get_debouncing_time( void )
{
    return INVALID_RETURN_VALUE;
}

void cedrus::base_device_t::set_debouncing_time( int time )
{
    // nothing
}

void cedrus::base_device_t::set_device_mode( int protocol )
{
    // nothing
}
