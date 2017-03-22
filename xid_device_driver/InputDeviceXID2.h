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

#ifndef XID2_INPUT_DEVICE_H
#define XID2_INPUT_DEVICE_H

#include "InputDevice.h"

#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class InputDeviceXID2 : public InputDevice
    {
    public:
        InputDeviceXID2(
            boost::shared_ptr<Connection> xidCon,
            boost::shared_ptr<const DeviceConfig> devConfig
            );
        virtual ~InputDeviceXID2(void);

    virtual int GetOutpostModel();
    virtual int GetHardwareGeneration();
    virtual int GetLightSensorMode();
    virtual void SetLightSensorMode( unsigned char mode );
    virtual void SetLightSensorThreshold( unsigned char threshold );
    virtual int GetLightSensorThreshold();
    virtual void SetScannerTriggerFilter( unsigned char mode );
    virtual int GetAccessoryConnectorMode();
    virtual int GetAccessoryConnectorDevice();
    virtual void ConnectToMpod(unsigned int mpod, unsigned int action);
    };
} // namespace cedrus

#endif // XID2_INPUT_DEVICE_H
