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

#include "XIDDeviceImpl.h"

#include "constants.h"
#include "ResponseManager.h"
#include "Connection.h"
#include "CommandGlossary.h"

cedrus::XIDDeviceImpl::XIDDeviceImpl(
            boost::shared_ptr<Connection> xidCon,
            boost::shared_ptr<const DeviceConfig> devConfig)
    : m_xidCon(xidCon),
      m_config(devConfig)
{
}

cedrus::XIDDeviceImpl::~XIDDeviceImpl()
{

}

const boost::shared_ptr<const cedrus::DeviceConfig> cedrus::XIDDeviceImpl::GetDeviceConfig() const
{
    return m_config;
}

int cedrus::XIDDeviceImpl::CloseConnection()
{
    return m_xidCon->Close();
}

int cedrus::XIDDeviceImpl::OpenConnection()
{
    return m_xidCon->Open();
}

bool cedrus::XIDDeviceImpl::HasLostConnection()
{
    return m_xidCon->HasLostConnection();
}

int cedrus::XIDDeviceImpl::GetBaudRate()
{
    return m_xidCon->GetBaudRate();
}

void cedrus::XIDDeviceImpl::SetBaudRate( unsigned char rate )
{
    m_xidCon->SetBaudRate(rate);
    CommandGlossary::SetBaudRate(m_xidCon, rate);
}

std::string cedrus::XIDDeviceImpl::GetProtocol()
{
    return CommandGlossary::GetProtocol(m_xidCon);
}

void cedrus::XIDDeviceImpl::GetProductAndModelID( int *productID, int *modelID)
{
    CommandGlossary::GetProductAndModelID(m_xidCon, productID, modelID);
}

int cedrus::XIDDeviceImpl::GetMajorFirmwareVersion()
{
    return CommandGlossary::GetMajorFirmwareVersion(m_xidCon);
}

int cedrus::XIDDeviceImpl::GetMinorFirmwareVersion()
{
    return CommandGlossary::GetMinorFirmwareVersion(m_xidCon);
}

std::string cedrus::XIDDeviceImpl::GetInternalProductName()
{
    return CommandGlossary::GetInternalProductName(m_xidCon);
}

void cedrus::XIDDeviceImpl::RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines)
{
    unsigned int output_lines = linesBitmask;

    if(leaveRemainingLines)
        output_lines |= m_linesState;

    CommandGlossary::SetDigitalOutputLines_ST(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::XIDDeviceImpl::LowerLines( unsigned int linesBitmask, bool leaveRemainingLines )
{
    unsigned int output_lines = ~linesBitmask;

    if(leaveRemainingLines)
        output_lines &= m_linesState;

    CommandGlossary::SetDigitalOutputLines_ST(m_xidCon, output_lines);

    m_linesState = output_lines;
}

void cedrus::XIDDeviceImpl::ResetBaseTimer()
{
    return CommandGlossary::ResetBaseTimer(m_xidCon);
}

unsigned int cedrus::XIDDeviceImpl::QueryBaseTimer()
{
    return CommandGlossary::QueryBaseTimer(m_xidCon);
}

void cedrus::XIDDeviceImpl::RestoreFactoryDefaults()
{
    CommandGlossary::RestoreFactoryDefaults(m_xidCon);
}

int cedrus::XIDDeviceImpl::GetOutpostModel()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::XIDDeviceImpl::GetHardwareGeneration()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::XIDDeviceImpl::GetLightSensorMode()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetLightSensorMode( unsigned char /*mode*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::SetLightSensorThreshold( unsigned char /*threshold*/ )
{
    // nothing
}

int cedrus::XIDDeviceImpl::GetLightSensorThreshold()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetScannerTriggerFilter( unsigned char /*mode*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::ResetRtTimer()
{
    // nothing
}

unsigned int cedrus::XIDDeviceImpl::GetPulseDuration()
{
    return 0;
}

void cedrus::XIDDeviceImpl::SetPulseDuration(unsigned int /*duration*/)
{
    // nothing
}

int cedrus::XIDDeviceImpl::GetNumberOfLines()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetNumberOfLines(unsigned int /*lines*/)
{
    // nothing
}

void cedrus::XIDDeviceImpl::PollForResponse()
{
    // nothing
}

bool cedrus::XIDDeviceImpl::HasQueuedResponses()
{
    return false;
}

int cedrus::XIDDeviceImpl::GetNumberOfKeysDown() const
{
    return 0;
}

void cedrus::XIDDeviceImpl::ClearResponseQueue()
{
    // nothing
}

cedrus::response cedrus::XIDDeviceImpl::GetNextResponse()
{
    return cedrus::response();
}

void cedrus::XIDDeviceImpl::ClearResponses()
{
    // nothing
}

int cedrus::XIDDeviceImpl::GetAccessoryConnectorMode()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::XIDDeviceImpl::GetAccessoryConnectorDevice()
{
    return INVALID_RETURN_VALUE;
}

int cedrus::XIDDeviceImpl::GetOutputLogic()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetOutputLogic( int /*mode*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::SetAccessoryConnectorMode( int /*mode*/ )
{
    // nothing
}
int cedrus::XIDDeviceImpl::GetVKDropDelay()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetVKDropDelay( unsigned char /*delay*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::ReprogramFlash()
{
    CommandGlossary::ReprogramFlash(m_xidCon);
}

int cedrus::XIDDeviceImpl::GetTriggerDefault()
{
    return false;
}

void cedrus::XIDDeviceImpl::SetTriggerDefault( bool /*defaultOn*/ )
{
    // nothing
}

int cedrus::XIDDeviceImpl::GetTriggerDebounceTime()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetTriggerDebounceTime( unsigned char /*time*/ )
{
    // nothing
}

int cedrus::XIDDeviceImpl::GetButtonDebounceTime()
{
    return INVALID_RETURN_VALUE;
}

void cedrus::XIDDeviceImpl::SetButtonDebounceTime( unsigned char /*time*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::SetProtocol( unsigned char /*protocol*/ )
{
    // nothing
}

void cedrus::XIDDeviceImpl::ConnectToMpod(unsigned int mpod, unsigned int action)
{
    CommandGlossary::ConnectToMpod(m_xidCon, mpod, action);
}

cedrus::XIDDeviceImpl::ProductAndModelID cedrus::XIDDeviceImpl::GetProductAndModelID()
{
    ProductAndModelID pm_id;
    GetProductAndModelID(&pm_id.productID, &pm_id.modelID);
    return pm_id;
}

void cedrus::XIDDeviceImpl::SetModelID( unsigned char model )
{
    CommandGlossary::SetModelID(m_xidCon, model);
}
