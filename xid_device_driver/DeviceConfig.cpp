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

#include "DeviceConfig.h"

#include "DeviceConfigRepository.h"

#include "constants.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sstream>

#include "CedrusAssert.h"

void Cedrus::DeviceConfig::PopulateConfigList(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
{
    CreateRB530Config(listOfAllConfigs);
    CreateRB540Config(listOfAllConfigs);
    CreateRB730Config(listOfAllConfigs);
    CreateRB740Config(listOfAllConfigs);
    CreateRB830Config(listOfAllConfigs);
    CreateRB840Config(listOfAllConfigs);
    CreateRB834Config(listOfAllConfigs);
    CreateRB844Config(listOfAllConfigs);

    CreateLuminaLP400Config(listOfAllConfigs);
    CreateLumina3GConfig(listOfAllConfigs);

    CreateSV1Config(listOfAllConfigs);

    CreateST100Config(listOfAllConfigs);
    CreateST2DuoConfig(listOfAllConfigs);
    CreateST2QuadConfig(listOfAllConfigs);

    CreateAllmpodConfigs(listOfAllConfigs);

    CreateAllcpodConfigs(listOfAllConfigs);
}

void Cedrus::DeviceConfig::CreateInvalidConfig(std::shared_ptr<Cedrus::DeviceConfig> & invalidCfgPtr)
{
    CreateEmptyConfig(invalidCfgPtr);
}

Cedrus::DeviceConfig::DeviceConfig(std::string deviceName,
    int productID,
    int modelID,
    int majorFirmwareVer,
    unsigned int outputLines,
    std::map<unsigned int, DevicePort> devicePorts)
    :
    m_DeviceName(deviceName),
    m_ProductID(productID),
    m_ModelID(modelID),
    m_MajorFirmwareVer(majorFirmwareVer),
    m_requiresDelay(false),
    m_outputLines(outputLines),
    m_DevicePorts(devicePorts)
{
    m_requiresDelay = (IsRB() && IsXID1()) || IsSV1();
}

Cedrus::DeviceConfig::DeviceConfig(std::string deviceName,
    int productID,
    int modelID,
    int majorFirmwareVer,
    unsigned int outputLines)
    :
    m_DeviceName(deviceName),
    m_ProductID(productID),
    m_ModelID(modelID),
    m_MajorFirmwareVer(majorFirmwareVer),
    m_requiresDelay(false),
    m_outputLines(outputLines)
{
    m_requiresDelay = (IsRB() && IsXID1()) || IsSV1();
}

Cedrus::DeviceConfig::~DeviceConfig(void)
{
}

int Cedrus::DeviceConfig::GetMappedKey(int port, int key) const
{
    int mapped_key = key;

    try
    {
        const Cedrus::DevicePort & key_port = m_DevicePorts.at(port);
        if (!key_port.keyMap.empty())
            mapped_key = key_port.keyMap[key];
    }
    catch (std::out_of_range e){}

    return mapped_key;
}

const std::map<unsigned int, Cedrus::DevicePort> * Cedrus::DeviceConfig::GetMapOfPorts() const
{
    return &m_DevicePorts;
}

const Cedrus::DevicePort * Cedrus::DeviceConfig::GetPortPtrByNumber(unsigned int portNum) const
{
    const Cedrus::DevicePort * port_ptr = nullptr;

    try
    {
        port_ptr = &(m_DevicePorts.at(portNum));
    }
    catch (std::out_of_range e)
    {
        return nullptr;
    }

    return port_ptr;
}

std::string Cedrus::DeviceConfig::GetDeviceName() const
{
    return m_DeviceName;
}

int Cedrus::DeviceConfig::GetProductID() const
{
    return m_ProductID;
}

int Cedrus::DeviceConfig::GetModelID() const
{
    return m_ModelID;
}

int Cedrus::DeviceConfig::GetMajorVersion() const
{
    return m_MajorFirmwareVer;
}

unsigned int Cedrus::DeviceConfig::GetNumberOfOutputLines() const
{
    return m_outputLines;
}

bool Cedrus::DeviceConfig::DoesConfigMatchDevice( int deviceID, int modelID, int majorFirmwareVer ) const
{
    bool does_match = false;
    if ( m_ProductID == deviceID && m_MajorFirmwareVer == majorFirmwareVer &&
        (!ModelIDMatters() || (m_ModelID == modelID)) )
    {
        does_match = true;
    }

    return does_match;
}
