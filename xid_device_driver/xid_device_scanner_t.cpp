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
#include "base_device_t.h"
#include "device_factory.h"

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

void cedrus::xid_device_scanner_t::drop_every_connection()
{
    for (unsigned int i = 0; i < devices_.size(); i++)
        devices_[i]->close_connection();

    devices_.clear();
}

void cedrus::xid_device_scanner_t::drop_connection_by_ptr( boost::shared_ptr<cedrus::base_device_t> device )
{
    for( std::vector< boost::shared_ptr<cedrus::base_device_t> >::iterator iter = devices_.begin();
            iter != devices_.end(); )
     {
        if ( device == *iter )
        {
            (*iter)->close_connection();
            iter = devices_.erase(iter);
        }
        else
            ++iter;
    }
}

int cedrus::xid_device_scanner_t::detect_valid_xid_devices
(
 const std::string &config_file_location,
 boost::function< void ( std::string ) > reportFunction,
 boost::function< void ( int ) > progressFunction
)
{
    devices_.clear();
    progressFunction(0);

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

    std::vector<std::string> available_com_ports;
    load_com_ports_platform_specific( &available_com_ports );

    progressFunction(5);
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
	    if( is_regular_file(p) && p.extension() == ".devconfig" )
        {
            boost::property_tree::ptree pt;
            try
            {
                boost::property_tree::ini_parser::read_ini(p.string(), pt);
                master_config_list.push_back(cedrus::xid_device_config_t::config_for_device(&pt));
            }
            catch ( boost::property_tree::ini_parser::ini_parser_error err )
            {
                // TODO: do something with the actual error message
                //err.filename(); err.line(); err.message();
            }
        }
	}

    progressFunction(40);
    for(std::vector<std::string>::iterator iter = available_com_ports.begin(),
        end = available_com_ports.end();
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

            if(xid_con->open() == XID_NO_ERR)
            {
                // NOTE THE USAGE OF xid_glossary_pst_proof IN THIS CODE. IT'S IMPORTANT!
                std::string info = xid_glossary_pst_proof::get_device_protocol(xid_con);

                if( boost::starts_with( info, "_xid" ) )
                {
                    progressFunction(70);
                    device_found = true;
                    bool mode_changed = false;

                    if(strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // Force the device into XID mode if it isn't. This is an XID library.
                        xid_glossary::set_device_protocol(xid_con, 0);

                        xid_con->flush_read_from_device_buffer();
                        mode_changed = true;
                    }

                    int product_id;
                    int model_id;

                    int major_firmware_version = xid_glossary_pst_proof::get_major_firmware_version(xid_con);

                    //What device is it? Get product/model ID, find the corresponding config
                    xid_glossary_pst_proof::get_product_and_model_id(xid_con, &product_id, &model_id);

                    // call "create_device" (which resides in device_factory.cpp)
                    boost::shared_ptr<cedrus::base_device_t> matched_dev =
                        create_device( product_id,
                                       model_id,
                                       major_firmware_version,
                                       config_candidates,
                                       xid_con );

                    if ( matched_dev )
                    {
                        devices_.push_back( matched_dev );

                        if ( mode_changed )
                            reportFunction( matched_dev-> get_device_config().get_device_name() );
                    }
                }
            }

            xid_con->close();
        }
    }
    progressFunction(100);
    return devices_.size();
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
