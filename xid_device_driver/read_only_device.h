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

#ifndef READ_ONLY_DEVICE_H
#define READ_ONLY_DEVICE_H

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    class read_only_device
    {
    public:
        virtual ~read_only_device()
        {
        };

        virtual int get_hardware_generation() = 0;

        virtual int get_light_sensor_mode() = 0;

        virtual int get_light_sensor_threshold() = 0;

        virtual unsigned int get_pulse_duration() = 0;

        virtual int get_accessory_connector_mode( void ) = 0;

        virtual int get_accessory_connector_device() = 0;
        
        virtual int get_outpost_model() = 0;

        // Every device needs these.
        virtual const boost::shared_ptr<const xid_device_config_t> get_device_config() const = 0;
        virtual int get_baud_rate() = 0;
        virtual void get_product_and_model_id( int *product_id, int *model_id ) = 0;
        virtual int get_major_firmware_version() = 0;
        virtual int get_minor_firmware_version() = 0;
        virtual std::string get_internal_product_name() = 0;
    };

} // namespace cedrus

#endif
