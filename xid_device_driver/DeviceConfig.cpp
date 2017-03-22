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

#include "constants.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>

#include "CedrusAssert.h"

boost::shared_ptr<cedrus::DeviceConfig> cedrus::DeviceConfig::ConfigForDevice(
        boost::property_tree::ptree * pt )
{
    boost::shared_ptr<DeviceConfig> devconfig;

    devconfig.reset(new DeviceConfig(pt));

    return devconfig;
}

cedrus::DeviceConfig::DeviceConfig( boost::property_tree::ptree * pt )
{
    std::string digital_output_command;

    m_MajorFirmwareVer = pt->get<int>("DeviceInfo.MajorFirmwareVersion", (int) INVALID_RETURN_VALUE );
    m_DeviceName = pt->get<std::string>("DeviceInfo.DeviceName", "" );
    m_ProductID = pt->get<int>("DeviceInfo.XidProductID", (int) INVALID_RETURN_VALUE );
    m_ModelID = pt->get<int>("DeviceInfo.XidModelID", (int) INVALID_RETURN_VALUE );

    bool missing_port = false;

    // The XID protocol can report responses from a maximum of 8 ports,
    // but usually only 2-3 ports are actually defined and only one
    // of them is for input.
    for(unsigned int i = 0; i < 8; ++i)
    {
        std::string port_str;
        std::ostringstream s;
        s << "Port" << i;
        port_str = s.str().c_str();

        try {
            pt->get_child(port_str);
        }
        catch ( boost::property_tree::ptree_bad_path err ) {
            missing_port = true;
            continue; // The device doesn't support this port
        }

        if ( missing_port )
            CEDRUS_FAIL("Devconfigs should include a fake port block for the ports they're missing!");

        DevicePort port;
        port.portName = pt->get(port_str+".PortName", "not_found");
        port.portNumber = i;
        port.numberOfLines = pt->get(port_str+".NumberOfLines", -1);
        port.isResponsePort = pt->get(port_str+".UseableAsResponse", "not_found") == "Yes";

        if ( port.isResponsePort )
        {
            // devconfig files have up to 8 key mappings per port
            for( unsigned int j = 0; j < 8; ++j )
            {
                std::ostringstream s2;
                s2 << ".XidDeviceKeyMap" << j;
                std::string key_name = s2.str().c_str();

                int key_num = pt->get(port_str+key_name, -1);
                port.keyMap[j] = key_num;
            }
        }

        m_DevicePorts.push_back(port);
    }
}

cedrus::DeviceConfig::~DeviceConfig(void)
{
}

int cedrus::DeviceConfig::GetMappedKey(int port, int key) const
{
    int mapped_key = -1;

    if ( port < m_DevicePorts.size() )
    {
        mapped_key = m_DevicePorts[port].keyMap[key];
    }

    // Anything that wasn't mapped during the devconfig parsing process will default to -1
    return mapped_key;
}

const std::vector<cedrus::DevicePort> * cedrus::DeviceConfig::GetVectorOfPorts() const
{
    return &m_DevicePorts;
}

const cedrus::DevicePort * cedrus::DeviceConfig::GetPortPtrByIndex(unsigned int portNum) const
{
    const cedrus::DevicePort * port_ptr = nullptr;
    if ( portNum < m_DevicePorts.size() )
        port_ptr = &(m_DevicePorts[portNum]);
    else
        CEDRUS_FAIL("Requested port number doesn't exist!");

    return port_ptr;
}

std::string cedrus::DeviceConfig::GetDeviceName() const
{
    return m_DeviceName;
}

int cedrus::DeviceConfig::GetProductID() const
{
    return m_ProductID;
}

int cedrus::DeviceConfig::GetModelID() const
{
    return m_ModelID;
}

int cedrus::DeviceConfig::GetMajorVersion() const
{
    return m_MajorFirmwareVer;
}

bool cedrus::DeviceConfig::DoesConfigMatchDevice( int deviceID, int modelID, int majorFirmwareVer ) const
{
    bool does_match = false;
    if ( m_ProductID == deviceID && m_ModelID == modelID && m_MajorFirmwareVer == majorFirmwareVer)
    {
        does_match = true;
    }

    return does_match;
}
