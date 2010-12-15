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
#include <Windows.h>
#include <iostream>

#include "string_tokenizer.hpp"

boost::shared_ptr<cedrus::xid_device_config_t>
    cedrus::xid_device_config_t::config_for_device(
        int product_id,
        int model_id,
        const std::wstring &devconfig_location)
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
    const std::wstring &config_file_location)
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
    std::wstring search_mask = config_file_location_ + L"*.devconfig";
    WIN32_FIND_DATA file_info;
    HANDLE find_handle = FindFirstFile(
        search_mask.c_str(),
        &file_info);

    while(find_handle)
    {
        long device_id = -1;
        std::string device_name;
        long xid_product_id = -1;
        long xid_model_id = -1;
        std::string digital_output_command;
        std::wstring ignored_ports;

        wchar_t result[1024];

        std::wstring full_file_path = config_file_location_ +
            file_info.cFileName;

        xid_product_id = GetPrivateProfileInt(
            L"DeviceInfo",
            L"XidProductID",
            0,
            full_file_path.c_str());

        xid_model_id = GetPrivateProfileInt(
            L"DeviceInfo",
            L"XidModelID",
            0,
            full_file_path.c_str());

        if(xid_product_id == product_id)
        {
            // if this is an RB device, make sure the model IDs match
            if(product_id == 2 && xid_model_id != model_id)
            {
                if(!FindNextFile(find_handle, &file_info))
                    break;
                continue;
            }
            
            // we've found the configuration for this device

            // does this device need an interbyte delay?
            if(GetPrivateProfileString(
                L"DeviceOptions",
                L"XidNeedsInterByteDelay",
                L"No",
                result,
                sizeof(result),
                full_file_path.c_str()) != 0)
            {
                std::wstring byte_delay(result);
                
                if(byte_delay.compare(L"Yes") == 0)
                {
                    needs_interbyte_delay_ = true;
                }
            }

            // get the digital output prefix
            if(GetPrivateProfileString(
                L"DeviceOptions",
                L"XidDigitalOutputCommand",
                L"a",
                result,
                sizeof(result),
                full_file_path.c_str()) != 0)
            {
                std::wstring response(result);
                std::string std_response;
                std_response.assign(response.begin(), response.end());
                digital_out_prefix_ = std_response[0];
            }
            
            wchar_t port_str[255];
            
            // xid devices support up to 255 ports.  In reality, usually only
            // 2 or 3 are used
            for(int i = 0; i <= 255; ++i)
            {
                wsprintf(port_str, L"Port%d", i);

                int return_size = GetPrivateProfileSection(
                    port_str,
                    result,
                    sizeof(result),
                    full_file_path.c_str());
                if(return_size == 0)
                {
                    // section not found.  This should be impossible to get to.
                    continue;
                }

                // The result of GetPrivateProfileSection() is a an array of
                // NULL delimited strings.  Here we create a response std::string
                // with the contents of the result[] array with nulls replaced
                // by commas for easier tokenizing.
                std::wstring res_str;
                res_str.reserve(return_size);
                for(int j = 0; j < return_size; ++j)
                {
                    if(result[j] != 0)
                    {
                        res_str.append(1, result[j]);
                    }
                    else
                    {
                        if(j != (return_size-1))
                            res_str.append(1, L',');
                    }
                }

                // Tokenize the result into Key=Value strings.
                std::vector<std::wstring> key_value_pairs =
                    cedrus::tokenize<std::wstring>(res_str,L",");
                
                // turn the Key=Value strings into a std::map
                std::map<std::wstring, std::wstring> res_map;
                for(std::vector<std::wstring>::iterator iter = key_value_pairs.begin();
                    iter != key_value_pairs.end(); ++iter)
                {
                    std::vector<std::wstring> key_value = 
                        cedrus::tokenize<std::wstring>(*iter, L"=");

                    res_map.insert(std::make_pair(key_value[0],key_value[1]));
                }

                std::map<std::wstring,std::wstring>::iterator found = 
                    res_map.find(L"PortName");

                if(found == res_map.end())
                {
                    // no port name found.  Try the next one
                    continue;
                }

                if(found->second.compare(L"Keys") == 0 ||
                   found->second.compare(L"Voice Key") == 0 ||
                   found->second.compare(L"Event Marker") == 0)
                {
                    // found a RB-Series response pad, Lumina system,
                    // SV-1 Voice Key, or StimTracker.
                    found = res_map.find(L"NumberOfLines");
                    if(found != res_map.end())
                    {
                        number_of_lines_ = _wtoi(found->second.c_str());
                    }
                
                    // devconfig files have up to 8 key mappings
                    for(int i = 1; i <=8; ++i)
                    {
                        wchar_t key_name[100];
                        memset(key_name, 0, sizeof(key_name));
                        wsprintf(key_name,L"XidDeviceKeyMap%d", i);
                        
                        found = res_map.find(std::wstring(key_name));
                        if(found != res_map.end())
                        {
                            int key_num = _wtoi(found->second.c_str());
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

        if(!FindNextFile(find_handle, &file_info))
            break;
    }

    FindClose(find_handle);
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