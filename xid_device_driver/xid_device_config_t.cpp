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
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
#include <iostream>

boost::shared_ptr<cedrus::xid_device_config_t>
    cedrus::xid_device_config_t::config_for_device(
        int product_id,
        int model_id,
        const std::string &devconfig_location)
{
    boost::shared_ptr<xid_device_config_t> devconfig;
    
    if(product_id == -99 || model_id == -99)
    {
        // invalid product or model id
        return devconfig;
    }

    if(devconfig_location.empty())
    {
        // no path to devconfig files.  Won't be able to load them so return
        // an invalid
        return devconfig;
    }

    devconfig.reset(new xid_device_config_t(devconfig_location));
    
    devconfig->load_devconfig(product_id, model_id);

    return devconfig;
}

cedrus::xid_device_config_t::xid_device_config_t(
    const std::string &config_file_location)
    : config_file_location_(config_file_location),
      needs_interbyte_delay_(true),
      number_of_lines_(8),
      digital_out_prefix_('a')
{
}


cedrus::xid_device_config_t::~xid_device_config_t(void)
{
}

void cedrus::xid_device_config_t::load_devconfig(int product_id, int model_id)
{
    //std::string search_mask = config_file_location_ + L"*.devconfig";

    boost::filesystem::path targetDir(config_file_location_);
	boost::filesystem::directory_iterator it(targetDir), eod;
	std::vector< std::string > devconfigs;

	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{ 
	    if( is_regular_file(p) && p.extension() == ".devconfig" )
			devconfigs.push_back( p.string() );
	}

    BOOST_FOREACH(std::string const config, devconfigs)
    {
	    boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(config, pt);

        std::string device_name;
        long xid_product_id = -1;
        long xid_model_id = -1;
        std::string digital_output_command;
        std::string ignored_ports;

		xid_product_id = pt.get<long>("DeviceInfo.XidProductID");
		xid_model_id = pt.get<long>("DeviceInfo.XidModelID");

		if(xid_product_id == product_id)
        {
            // if this is an RB device, make sure the model IDs match
            if(product_id == 2 && xid_model_id != model_id)
                continue;

            // does this device need an interbyte delay?
            std::string byte_delay = pt.get("DeviceOptions.XidNeedsInterByteDelay", "not_found");
            
            if(byte_delay.compare("No") == 0)
                needs_interbyte_delay_ = false;

            // get the digital output prefix
            std::string std_response = pt.get("DeviceOptions.XidDigitalOutputCommand", "not_found");
            if(std_response != "not_found")
                digital_out_prefix_ = std_response[0];
            
            char port_str[255];
            
            // xid devices support up to 255 ports.  In reality, usually only
            // 2 or 3 are used
            for(int i = 0; i <= 255; ++i)
            {
                snprintf(port_str, sizeof(port_str), "Port%d", i);

                try {
                    pt.get_child(port_str);
                }
                catch ( boost::property_tree::ptree_error ) {
                    continue; // The device doesn't support this port
                }

                std::map<std::string, std::string> res_map;
                BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child(port_str) )
                {
	                res_map.insert(std::make_pair(v.first.data(),v.second.data()));
                }

                std::map<std::string,std::string>::iterator found = 
                    res_map.find("PortName");

                if(found == res_map.end())
                {
                    // no port name found. Try the next one
                    continue;
                }

                if(found->second.compare("Keys") == 0 ||
                   found->second.compare("Voice Key") == 0 ||
                   found->second.compare("Event Marker") == 0)
                {
                    // found a RB-Series response pad, Lumina system,
                    // SV-1 Voice Key, or StimTracker.
                    found = res_map.find("NumberOfLines");
                    if(found != res_map.end())
                    {
                        number_of_lines_ = atoi(found->second.c_str());
                    }
                
                    // devconfig files have up to 8 key mappings
                    for(int i = 1; i <=8; ++i)
                    {
                        char key_name[100];
                        snprintf(key_name, sizeof(key_name), "XidDeviceKeyMap%d", i);
                        
                        found = res_map.find(std::string(key_name));
                        if(found != res_map.end())
                        {
                            int key_num = atoi(found->second.c_str());
                            key_map_.insert(std::make_pair(i, key_num));
                        }
                    }
                }
                else
                {
                    // no valid port found. Try the next one.
                    continue;
                }
            }
        }
	}
}

int cedrus::xid_device_config_t::get_mapped_key(int key) const
{
    if(key_map_.empty())
        return key;

    std::map<int,int>::const_iterator found = 
        key_map_.find(key);

    if(found == key_map_.end())
        return -1;

    return found->second;
}

int cedrus::xid_device_config_t::number_of_lines() const
{
    return number_of_lines_;
}

bool cedrus::xid_device_config_t::needs_interbyte_delay() const
{
    return needs_interbyte_delay_;
}

char cedrus::xid_device_config_t::digital_out_prefix() const
{
    return digital_out_prefix_;
}
