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

cedrus::xid_device_scanner_t::xid_device_scanner_t( const std::string &config_file_location )
{
    read_in_devconfigs(config_file_location);
}

cedrus::xid_device_scanner_t::~xid_device_scanner_t(void)
{
}

bool cedrus::xid_device_scanner_t::read_in_devconfigs ( const std::string &config_file_location )
{
    try {
        boost::filesystem::path targetDir(config_file_location);
        boost::filesystem::directory_iterator it(targetDir), eod;
    }
    catch ( boost::filesystem::filesystem_error ) {
        return false; // BAIL BAIL BAIL! Error retrieving devconfigs!
    }

    boost::filesystem::path targetDir(config_file_location);
    boost::filesystem::directory_iterator it(targetDir), eod;

    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
    {
        if( is_regular_file(p) && p.extension() == ".devconfig" && p.filename().string().find("XID") != std::string::npos )
        {
            boost::property_tree::ptree pt;
            try
            {
                boost::property_tree::ini_parser::read_ini(p.string(), pt);
                m_masterConfigList.push_back(cedrus::xid_device_config_t::config_for_device(&pt));
            }
            catch ( boost::property_tree::ini_parser::ini_parser_error err )
            {
                // TODO: do something with the actual error message
                //err.filename(); err.line(); err.message();
            }
        }
    }

    return true;
}

void cedrus::xid_device_scanner_t::load_com_ports( std::vector<DWORD> * available_com_ports )
{
    available_com_ports->clear();

    FT_STATUS status;
    DWORD num_devs;

    // create the device information list
    status = FT_CreateDeviceInfoList(&num_devs);

    if (num_devs > 0)
    {
        // allocate storage for list based on numDevs
        FT_DEVICE_LIST_INFO_NODE * dev_info = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*num_devs);
        // get the device information list
        status = FT_GetDeviceInfoList(dev_info, &num_devs);

        if ( status == FT_OK )
        {
            for ( unsigned int i = 0; i < num_devs; i++ )
            {
                cedrus::xid_con_t conn(dev_info[i].LocId);

                if(conn.open() == XID_NO_ERR)
                {
                    conn.close();
                    available_com_ports->push_back(dev_info[i].LocId);
                }
            }
        }

        free(dev_info);
    }
}

void cedrus::xid_device_scanner_t::close_all_connections()
{
    for (unsigned int i = 0; i < devices_.size(); i++)
        devices_[i]->close_connection();
}

// This may seem like an odd function to have, but it can be used to short-
// circuit logic by preventing existing devices from being picked up during
// a scan.
void cedrus::xid_device_scanner_t::open_all_connections()
{
    for (unsigned int i = 0; i < devices_.size(); i++)
        devices_[i]->open_connection();
}

void cedrus::xid_device_scanner_t::drop_every_connection()
{
    close_all_connections();

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

void cedrus::xid_device_scanner_t::check_connections_drop_dead_ones()
{
    close_all_connections();
    std::vector< boost::shared_ptr<cedrus::base_device_t> >::iterator iter = devices_.begin();
    while( iter != devices_.end() )
    {
        if( (*iter)->open_connection() != XID_NO_ERR )
        {
            (*iter)->close_connection();
            iter = devices_.erase(iter);
        }
        else
            ++iter;
    }

    close_all_connections();
}


int cedrus::xid_device_scanner_t::detect_valid_xid_devices
(
 boost::function< void ( std::string ) > reportFunction,
 boost::function< bool ( unsigned int ) > progressFunction
)
{
    check_connections_drop_dead_ones();
    open_all_connections();

    unsigned int current_prog = 0;

    if ( progressFunction )
        progressFunction(current_prog);

    std::vector<DWORD> available_com_ports;
    load_com_ports( &available_com_ports );

    unsigned int prog_increment = 100/((available_com_ports.size()*5)+1); // 5 is the number of possible xid bauds
    bool scanning_canceled = false;

    for(std::vector<DWORD>::iterator iter = available_com_ports.begin(),
        end = available_com_ports.end();
        iter != end && !scanning_canceled; ++iter)
    {
        std::vector<boost::shared_ptr< cedrus::xid_device_config_t> > config_candidates;

        // If a config is for a device that can potentially be on this port, it's a candidate. This step
        // is important because "dry" port scans are very time consuming!
        BOOST_FOREACH(boost::shared_ptr<cedrus::xid_device_config_t> const config, m_masterConfigList)
        {
            // This may possibly be no longer needed with the FTDI library.
            //if ( !config->is_port_on_ignore_list( *iter ) )
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
            // Update progress
            current_prog += prog_increment;
            if ( progressFunction )
            {
                if ( progressFunction(current_prog) )
                {
                    drop_every_connection();
                    scanning_canceled = true;
                    break;
                }
            }

            boost::shared_ptr<cedrus::xid_con_t> xid_con(new xid_con_t(*iter, baud_rate[i]));

            if(xid_con->open() == XID_NO_ERR)
            {
                // This may seem like a good place to flush, but open() has taken care of that by now.

                // NOTE THE USAGE OF xid_glossary_pst_proof IN THIS CODE. IT'S IMPORTANT!
                std::string info = xid_glossary_pst_proof::get_device_protocol(xid_con);

                if( boost::starts_with( info, "_xid" ) )
                {
                    device_found = true;
                    bool mode_changed = false;

                    if(strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // Force the device into XID mode if it isn't. This is an XID library.
                        xid_glossary::set_device_protocol(xid_con, 0);

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

                        if ( mode_changed && reportFunction )
                            reportFunction( matched_dev->get_device_config()->get_device_name() );

                        break;
                    }
                }
            }

            xid_con->close();
        }
    }

    if ( progressFunction )
        progressFunction(100);

    close_all_connections();

    return devices_.size();
}

boost::shared_ptr<cedrus::base_device_t>
cedrus::xid_device_scanner_t::device_connection_at_index(unsigned int i) const
{
    if(i >= devices_.size())
        return boost::shared_ptr<base_device_t>();

    return devices_[i];
}

unsigned int cedrus::xid_device_scanner_t::device_count() const
{
    return devices_.size();
}

const boost::shared_ptr<const cedrus::xid_device_config_t>
cedrus::xid_device_scanner_t::devconfig_at_index(unsigned int i) const
{
    if(i >= m_masterConfigList.size())
        return boost::shared_ptr<const xid_device_config_t>();

    return m_masterConfigList[i];
}

unsigned int cedrus::xid_device_scanner_t::devconfig_count() const
{
    return m_masterConfigList.size();
}
