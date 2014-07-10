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

#include "xid_device_config_t.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <iostream>

#include <sstream>

boost::shared_ptr<cedrus::xid_device_config_t> cedrus::xid_device_config_t::config_for_device(
        boost::property_tree::ptree * pt )
{
    boost::shared_ptr<xid_device_config_t> devconfig;

    devconfig.reset(new xid_device_config_t(pt));

    return devconfig;
}

cedrus::xid_device_config_t::xid_device_config_t( boost::property_tree::ptree * pt )
    : needs_interbyte_delay_(true)
{
    std::string digital_output_command;

    major_firmware_ver_ = pt->get<long>("DeviceInfo.MajorFirmwareVersion");
    device_name_ = pt->get<std::string>("DeviceInfo.DeviceName");
    product_id_ = pt->get<long>("DeviceInfo.XidProductID");
    model_id_ = pt->get<long>("DeviceInfo.XidModelID");

    std::string regex_string(",");
    std::string ports_string = pt->get("DeviceOptions.XidIgnoreSerialPorts", "not_found");

    boost::split(ports_to_ignore_,ports_string,boost::is_any_of(regex_string));

    // does this device need an interbyte delay?
    std::string byte_delay = pt->get("DeviceOptions.XidNeedsInterByteDelay", "not_found");

    if(byte_delay.compare("No") == 0)
        needs_interbyte_delay_ = false;

    // xid devices support up to 255 ports.  In reality, usually only
    // 2 or 3 are used
    for(int i = 0; i <= 255; ++i)
    {
        std::string port_str;
        std::ostringstream s;
        s << "Port" << i;
        port_str = s.str().c_str();

        try {
            pt->get_child(port_str);
        }
        catch ( boost::property_tree::ptree_error ) {
            continue; // The device doesn't support this port
        }

        device_port port;
        port.port_name = pt->get(port_str+".PortName", "not_found");
        port.port_number = i;
        port.number_of_lines = pt->get(port_str+".NumberOfLines", -1);

        if ( pt->get(port_str+".UseableAsResponse", "not_found") == "Yes" )
        {
            // devconfig files have up to 8 key mappings
            for( int j = 0; j < 8; ++j )
            {
                std::ostringstream s2;
                s2 << ".XidDeviceKeyMap" << j;
                std::string key_name = s2.str().c_str();

                int key_num = pt->get(port_str+key_name, -1);
                port.key_map[j] = key_num;
            }
        }

        m_device_ports.insert(std::make_pair(i, port));
    }
}

cedrus::xid_device_config_t::~xid_device_config_t(void)
{
}

int cedrus::xid_device_config_t::get_mapped_key(int port, int key) const
{
    std::map<int,device_port>::const_iterator port_found = m_device_ports.find(port);

    // The device doesn't have this port, should never happen.
    if( port_found == m_device_ports.end() )
        return -1;

    // Anything that wasn't mapped during the devconfig parsing process will default to -1
    return port_found->second.key_map[key];
}

int cedrus::xid_device_config_t::get_num_lines_on_port(int port) const
{
    int num_lines = -1;
    std::map<int,device_port>::const_iterator port_found = m_device_ports.find(port);

    if( port_found != m_device_ports.end() )
        num_lines = port_found->second.number_of_lines;

    return num_lines;
}

std::vector<cedrus::device_port> cedrus::xid_device_config_t::get_vector_of_ports() const
{
    std::vector<device_port> vector_of_ports;

    std::map<int,device_port>::const_iterator port = m_device_ports.begin();

    while( port != m_device_ports.end() )
    {
        vector_of_ports.push_back(port->second);
        port++;
    }

    return vector_of_ports;
}

bool cedrus::xid_device_config_t::needs_interbyte_delay() const
{
    return needs_interbyte_delay_;
}

bool cedrus::xid_device_config_t::is_port_on_ignore_list( std::string port_name) const
{
    return std::find(ports_to_ignore_.begin(), ports_to_ignore_.end(), port_name) != ports_to_ignore_.end();
}

std::string cedrus::xid_device_config_t::get_device_name()
{
    return device_name_;
}

int cedrus::xid_device_config_t::get_product_id() const
{
    return product_id_;
}

int cedrus::xid_device_config_t::get_model_id() const
{
    return model_id_;
}

bool cedrus::xid_device_config_t::does_config_match_device( int device_id, int model_id, int major_firmware_ver ) const
{
    bool doesMatch = false;
    if ( product_id_ == device_id && major_firmware_ver_ == major_firmware_ver)
    {
        //per XID spec, '0' is Lumina, '1' is SV1, '2' is an RB
        if ( static_cast<char>(product_id_) == '2' )
        {
            if ( model_id_ == model_id )
                doesMatch = true;
        }
        else
            doesMatch = true;
    }

    return doesMatch;
}
