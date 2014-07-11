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

#include "xid2_device.h"

#include "xid_glossary.h"

cedrus::xid2_device::xid2_device(
    boost::shared_ptr<xid_con_t> xid_con,
    boost::shared_ptr<const xid_device_config_t> dev_config)
    : xid_device_t(xid_con, dev_config)
{
}

cedrus::xid2_device::~xid2_device(void)
{
}

int cedrus::xid2_device::get_outpost_model( void )
{
    return xid_glossary::get_outpost_model(xid_con_);
}

int cedrus::xid2_device::get_light_sensor_mode( void )
{
    return xid_glossary::get_light_sensor_mode(xid_con_);
}

void cedrus::xid2_device::set_light_sensor_mode( int mode )
{
    xid_glossary::set_light_sensor_mode(xid_con_, mode);
}

void cedrus::xid2_device::set_light_sensor_threshold( int threshold )
{
    xid_glossary::set_light_sensor_threshold(xid_con_, threshold);
}

int cedrus::xid2_device::get_light_sensor_threshold( void )
{
    return xid_glossary::get_light_sensor_threshold(xid_con_);
}

void cedrus::xid2_device::set_scanner_trigger_filter( int mode )
{
    xid_glossary::set_scanner_trigger_filter(xid_con_, mode);
}

int cedrus::xid2_device::get_accessory_connector_mode( void )
{
    return INVALID_RETURN_VALUE;
}

int cedrus::xid2_device::get_accessory_connector_device( void )
{
    return xid_glossary::get_accessory_connector_device(xid_con_);
}

int cedrus::xid2_device::get_debouncing_time( void )
{
    return xid_glossary::get_debouncing_time(xid_con_);
}

void cedrus::xid2_device::set_debouncing_time( int time )
{
    xid_glossary::set_debouncing_time(xid_con_, time);
}
