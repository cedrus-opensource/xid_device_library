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

#include "InputDeviceXID2.h"

#include "CommandGlossary.h"

cedrus::InputDeviceXID2::InputDeviceXID2(
    boost::shared_ptr<Connection> xidCon,
    boost::shared_ptr<const DeviceConfig> devConfig)
    : InputDevice(xidCon, devConfig)
{
}

cedrus::InputDeviceXID2::~InputDeviceXID2(void)
{
}

int cedrus::InputDeviceXID2::GetOutpostModel()
{
    return CommandGlossary::GetOutpostModel(m_xidCon);
}

int cedrus::InputDeviceXID2::GetHardwareGeneration()
{
    return CommandGlossary::GetHardwareGeneration(m_xidCon);
}

int cedrus::InputDeviceXID2::GetLightSensorMode()
{
    return CommandGlossary::GetLightSensorMode(m_xidCon);
}

void cedrus::InputDeviceXID2::SetLightSensorMode( unsigned char mode )
{
    CommandGlossary::SetLightSensorMode(m_xidCon, mode);
}

void cedrus::InputDeviceXID2::SetLightSensorThreshold( unsigned char threshold )
{
    CommandGlossary::SetLightSensorThreshold(m_xidCon, threshold);
}

int cedrus::InputDeviceXID2::GetLightSensorThreshold()
{
    return CommandGlossary::GetLightSensorThreshold(m_xidCon);
}

void cedrus::InputDeviceXID2::SetScannerTriggerFilter( unsigned char mode )
{
    CommandGlossary::SetScannerTriggerFilter(m_xidCon, mode);
}

int cedrus::InputDeviceXID2::GetAccessoryConnectorMode()
{
    return CommandGlossary::GetAccessoryConnectorMode(m_xidCon);
}

int cedrus::InputDeviceXID2::GetAccessoryConnectorDevice()
{
    return CommandGlossary::GetAccessoryConnectorDevice(m_xidCon);
}

void cedrus::InputDeviceXID2::ConnectToMpod(unsigned int mpod, unsigned int action)
{
    CommandGlossary::ConnectToMpod(m_xidCon, mpod, action);
}
