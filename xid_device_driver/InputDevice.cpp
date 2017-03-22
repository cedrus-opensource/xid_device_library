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

#include "InputDevice.h"

#include "Connection.h"
#include "CommandGlossary.h"

#include "CedrusAssert.h"

cedrus::InputDevice::InputDevice(
    boost::shared_ptr<Connection> xidCon,
    boost::shared_ptr<const DeviceConfig> devConfig)
    : XIDDeviceImpl(xidCon, devConfig),
    m_ResponseMgr(new ResponseManager(GetMinorFirmwareVersion(), devConfig))
{
    ClearLines();
    ResetRtTimer();
    ResetBaseTimer();
}

cedrus::InputDevice::~InputDevice()
{
}

void cedrus::InputDevice::ResetRtTimer()
{
    CommandGlossary::ResetRtTimer(m_xidCon);
}

void cedrus::InputDevice::PollForResponse()
{
    m_ResponseMgr->CheckForKeypress(m_xidCon, m_config);
}

bool cedrus::InputDevice::HasQueuedResponses()
{
    return m_ResponseMgr->HasQueuedResponses();
}

int cedrus::InputDevice::GetNumberOfKeysDown() const
{
    return m_ResponseMgr->GetNumberOfKeysDown();;
}

void cedrus::InputDevice::ClearResponseQueue()
{
    m_ResponseMgr->ClearResponseQueue();
}

cedrus::response cedrus::InputDevice::GetNextResponse()
{
    return m_ResponseMgr->GetNextResponse();
}

void cedrus::InputDevice::ClearResponses()
{
    m_xidCon->FlushReadFromDeviceBuffer();
}

int cedrus::InputDevice::GetAccessoryConnectorMode()
{
    return CommandGlossary::GetAccessoryConnectorMode(m_xidCon);
}

int cedrus::InputDevice::GetOutputLogic()
{
    return CommandGlossary::GetOutputLogic(m_xidCon);
}

void cedrus::InputDevice::SetOutputLogic( int mode )
{
    CommandGlossary::SetOutputLogic(m_xidCon, mode);
}

void cedrus::InputDevice::SetAccessoryConnectorMode( int mode )
{
    CommandGlossary::SetAccessoryConnectorMode(m_xidCon, mode);
}

int cedrus::InputDevice::GetVKDropDelay()
{
    return CommandGlossary::GetVKDropDelay(m_xidCon);
}

void cedrus::InputDevice::SetVKDropDelay( unsigned char delay )
{
    CommandGlossary::SetVKDropDelay(m_xidCon, delay);
}

void cedrus::InputDevice::RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines)
{
    unsigned int output_lines = linesBitmask;

    if(leaveRemainingLines)
        output_lines |= m_linesState;

    CommandGlossary::SetDigitalOutputLines_RB(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::InputDevice::LowerLines( unsigned int linesBitmask, bool leaveRemainingLines )
{
    unsigned int output_lines = ~linesBitmask;

    if(leaveRemainingLines)
        output_lines &= m_linesState;

    CommandGlossary::SetDigitalOutputLines_RB(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::InputDevice::ClearLines()
{
    CommandGlossary::SetDigitalOutputLines_RB(m_xidCon, 0);
    m_linesState = 0;
}

int cedrus::InputDevice::GetTriggerDefault()
{
    return CommandGlossary::GetTriggerDefault(m_xidCon);
}

void cedrus::InputDevice::SetTriggerDefault( bool defaultOn )
{
    CommandGlossary::SetTriggerDefault(m_xidCon, defaultOn);
}

int cedrus::InputDevice::GetTriggerDebounceTime()
{
    return CommandGlossary::GetTriggerDebounceTime(m_xidCon);
}

void cedrus::InputDevice::SetTriggerDebounceTime( unsigned char time )
{
    CommandGlossary::SetTriggerDebounceTime(m_xidCon, time);
}

int cedrus::InputDevice::GetButtonDebounceTime()
{
    return CommandGlossary::GetButtonDebounceTime(m_xidCon);
}

void cedrus::InputDevice::SetButtonDebounceTime( unsigned char time )
{
    CommandGlossary::SetButtonDebounceTime(m_xidCon, time);
}
