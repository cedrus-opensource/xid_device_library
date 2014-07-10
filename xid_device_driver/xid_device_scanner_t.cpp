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

#include "xid_device_scanner_t.h"

#include "xid_glossary.h"
#include "xid_device_config_t.h"
#include "xid_con_t.h"
#include "xid_device_t.h"
#include "xid2_device.h"
#include "stim_tracker_t.h"

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

#ifdef __APPLE__
#   include "xid_device_scanner_helper_mac.h"
#elif defined(_WIN32)
#   include "xid_device_scanner_helper_win.h"
#endif

enum { OS_FILE_ERROR = -1,
    NO_FILE_SELECTED = 0};

cedrus::xid_device_scanner_t::xid_device_scanner_t(void)
{
}

cedrus::xid_device_scanner_t::~xid_device_scanner_t(void)
{
}

void cedrus::xid_device_scanner_t::load_available_com_ports()
{
    load_com_ports_platform_specific( &available_com_ports_ );
}

void cedrus::xid_device_scanner_t::drop_every_connection()
{
    for (unsigned int i = 0; i < devices_.size(); i++)
        devices_[i]->close_connection();

    devices_.clear();
}

int cedrus::xid_device_scanner_t::detect_valid_xid_devices(const std::string &config_file_location, boost::function< void ( std::string ) > reportFunction)
{
    int devices = 0; //the return value

    try {
        boost::filesystem::path targetDir(config_file_location);
        boost::filesystem::directory_iterator it(targetDir), eod;
    }
    catch ( boost::filesystem::filesystem_error ) {
        return -1; // Error retrieving devconfigs
    }

    boost::filesystem::path targetDir(config_file_location);
    boost::filesystem::directory_iterator it(targetDir), eod;

    // This will contain every devconfig we can find.
    std::vector< boost::shared_ptr<cedrus::xid_device_config_t> > master_config_list;

    load_available_com_ports();
    devices_.clear();

	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
	    if( is_regular_file(p) && p.extension() == ".devconfig" )
        {
            boost::property_tree::ptree pt;
            boost::property_tree::ini_parser::read_ini(p.string(), pt);
            master_config_list.push_back(cedrus::xid_device_config_t::config_for_device(&pt));
        }
	}

    for(std::vector<std::string>::iterator iter = available_com_ports_.begin(),
        end = available_com_ports_.end();
        iter != end; ++iter)
    {
        std::vector<boost::shared_ptr< cedrus::xid_device_config_t> > config_candidates;

        // If a config is for a device that can potentially be on this port, it's a candidate. This step
        // is important because "dry" port scans are very time consuming!
        BOOST_FOREACH(boost::shared_ptr<cedrus::xid_device_config_t> const config, master_config_list)
        {
            if ( !config->is_port_on_ignore_list( *iter ) )
                config_candidates.push_back(config);
        }

        if ( config_candidates.empty() )
            continue;

        bool device_found = false;
        const int baud_rate[] = { 115200, 19200, 9600, 57600, 38400 };
        const int num_bauds   = sizeof(baud_rate)/sizeof(int);

        // Here we're going to actually connect to a port and send it some signals. Our aim here is to
        // get an XID device's product/device and model IDs.
        for(int i = 0; i < num_bauds && !device_found; ++i)
        {
            boost::shared_ptr<cedrus::xid_con_t> xid_con(new xid_con_t(*iter, baud_rate[i]));

            if(xid_con->open() == NO_ERR)
            {
                char return_info[200];
                xid_con->flush_input();
                xid_con->flush_output();

                xid_con->send_xid_command("_c1",
                                          return_info,
                                          sizeof(return_info),
                                          1000,
                                          100);

                std::string info;
                if(return_info[0] == 0)
                {
                    // there's a possibility that the device is in E-Prime mode.
                    // Go through the buffer and discard NULL characters, and
                    // only keep the non NULL characters. Also, flush everything.
                    for(size_t j = 0; j < sizeof(return_info); ++j)
                    {
                        if(return_info[j] != 0)
                        {
                            info.append(&return_info[j], 1);
                        }
                    }
                    xid_con->flush_input();
                    xid_con->flush_output();
                }
                else
                    info = std::string(return_info);

                if( strstr(info.c_str(), "_xid") )
                {
                    device_found = true;
                    bool mode_changed = false;

                    if(strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // Force the device into XID mode if it isn't. This is an XID library.
                        xid_glossary::set_device_mode(xid_con, 0);

                        xid_con->flush_input();
                        xid_con->flush_output();
                        mode_changed = true;
                    }

                    int product_id;
                    int model_id;
                    char major_return[2];

                    xid_con->send_xid_command(
                            "_d4",
                            major_return,
                            sizeof(major_return));

                    int major_firmware_version = major_return[0]-'0';

                    //What device is it? Get product/model ID, find the corresponding config
                    xid_glossary::get_product_and_model_id(xid_con, &product_id, &model_id);

                    BOOST_FOREACH(boost::shared_ptr<cedrus::xid_device_config_t> const config, config_candidates)
                    {
                        if ( config->does_config_match_device(product_id, model_id, major_firmware_version) )
                        {
                            ++devices;
                            if ( mode_changed )
                                reportFunction( config->get_device_name() );

                            char dev_type[10];
                            xid_con->send_xid_command("_d2", dev_type, sizeof(dev_type), 1000);

                            if(dev_type[0] == 'S')
                            {
                                boost::shared_ptr<cedrus::base_device_t> stim_tracker (new stim_tracker_t(xid_con, config));
                                devices_.push_back(stim_tracker);
                            }
                            else
                            {
                                boost::shared_ptr<cedrus::base_device_t> rb_device;
                                if ( major_firmware_version == 1 )
                                    rb_device.reset(new xid_device_t(xid_con, config));
                                else
                                    rb_device.reset(new xid2_device(xid_con, config));

                                devices_.push_back(rb_device);
                            }
                        }
                    }
                }
            }

            xid_con->close();
        }
    }
    return devices;
}

boost::shared_ptr<cedrus::base_device_t>
cedrus::xid_device_scanner_t::device_connection_at_index(unsigned int i)
{
    if(i >= devices_.size())
        return boost::shared_ptr<base_device_t>();

    return devices_[i];
}

int cedrus::xid_device_scanner_t::device_count() const
{
    return devices_.size();
}
