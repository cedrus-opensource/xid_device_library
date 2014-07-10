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

#ifndef XID2_DEVICE_H
#define XID2_DEVICE_H

#include "constants.h"
#include "xid_device_t.h"
#include "response_mgr.h"

#include "XidDriverImpExpDefs.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <queue>

namespace cedrus
{
    class xid2_device : public xid_device_t
    {
    public:
        xid2_device(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<xid_device_config_t> dev_config
            );
        virtual ~xid2_device(void);

    virtual int get_outpost_model();
    virtual int get_light_sensor_mode();
    virtual void set_light_sensor_mode( int mode );
    virtual void set_light_sensor_threshold( int threshold );
    virtual int get_light_sensor_threshold();
    virtual void set_scanner_trigger_filter( int mode );
    virtual int get_accessory_connector_mode( void );
    virtual int get_accessory_connector_device( void );
    virtual int get_debouncing_time();
    virtual void set_debouncing_time( int time );

    };
} // namespace cedrus

#endif // XID2_DEVICE_H
