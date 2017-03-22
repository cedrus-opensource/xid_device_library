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

#include "XIDDeviceScanner.h"

#include "CommandGlossary.h"
#include "DeviceConfig.h"
#include "Connection.h"
#include "XIDDevice.h"
#include "DeviceFactory.h"

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

cedrus::XIDDeviceScanner::XIDDeviceScanner( const std::string &configFileLocation )
{
    ReadInDevconfigs(configFileLocation);
}

bool cedrus::XIDDeviceScanner::ReadInDevconfigs ( const std::string &configFileLocation )
{
    try {
        boost::filesystem::path target_dir(configFileLocation);
        boost::filesystem::directory_iterator it(target_dir), eod;
    }
    catch ( boost::filesystem::filesystem_error ) {
        return false; // BAIL BAIL BAIL! Error retrieving devconfigs!
    }

    boost::filesystem::path target_dir(configFileLocation);
    boost::filesystem::directory_iterator it(target_dir), eod;

    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
    {
        if( is_regular_file(p) && p.extension() == ".devconfig" && p.filename().string().find("XID") != std::string::npos )
        {
            boost::property_tree::ptree pt;
            try
            {
                boost::property_tree::ini_parser::read_ini(p.string(), pt);
                m_MasterConfigList.push_back(cedrus::DeviceConfig::ConfigForDevice(&pt));
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

void cedrus::XIDDeviceScanner::LoadCOMPorts( std::vector<DWORD> * availableCOMPorts)
{
    availableCOMPorts->clear();

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
                availableCOMPorts->push_back(dev_info[i].LocId);
            }
        }

        free(dev_info);
    }
}

void cedrus::XIDDeviceScanner::CloseAllConnections()
{
    for (unsigned int i = 0; i < m_Devices.size(); i++)
        m_Devices[i]->CloseConnection();
}

// This may seem like an odd function to have, but it can be used to short-
// circuit logic by preventing existing devices from being picked up during
// a scan.
void cedrus::XIDDeviceScanner::OpenAllConnections()
{
    for (unsigned int i = 0; i < m_Devices.size(); i++)
        m_Devices[i]->OpenConnection();
}

void cedrus::XIDDeviceScanner::DropEveryConnection()
{
    CloseAllConnections();

    m_Devices.clear();
}

void cedrus::XIDDeviceScanner::DropConnectionByPtr( boost::shared_ptr<cedrus::XIDDevice> device )
{
    for( std::vector< boost::shared_ptr<cedrus::XIDDevice> >::iterator iter = m_Devices.begin();
            iter != m_Devices.end(); )
     {
        if ( device == *iter )
        {
            (*iter)->CloseConnection();
            iter = m_Devices.erase(iter);
        }
        else
            ++iter;
    }
}

void cedrus::XIDDeviceScanner::CheckConnectionsDropDeadOnes()
{
    CloseAllConnections();
    std::vector< boost::shared_ptr<cedrus::XIDDevice> >::iterator iter = m_Devices.begin();
    while( iter != m_Devices.end() )
    {
        bool drop_connection = (*iter)->OpenConnection() != XID_NO_ERR;

        // If the connection held up, we still need to verify the device is what
        // it used to be. XID devices are largely interchangeable as far as the library
        // is concerned, but sending commands to the wrong one can make things
        // very confusing.
        if ( !drop_connection )
        {
            int product_id, model_id;
            (*iter)->GetProductAndModelID(&product_id, &model_id);

            int major_firmware_version = (*iter)->GetMajorFirmwareVersion();

            if ( (*iter)->GetDeviceConfig()->DoesConfigMatchDevice(product_id, model_id, major_firmware_version) )
            {
                ++iter;
            }
            else
                drop_connection = true; // The device is not what we thought it was.
        }

        if( drop_connection )
        {
            (*iter)->CloseConnection();
            iter = m_Devices.erase(iter);
        }
    }

    CloseAllConnections();
}


int cedrus::XIDDeviceScanner::DetectXIDDevices
(
 boost::function< void ( std::string ) > reportFunction,
 boost::function< bool ( unsigned int ) > progressFunction
)
{
    CheckConnectionsDropDeadOnes();
    OpenAllConnections();

    unsigned int current_prog = 0;

    if ( progressFunction )
        progressFunction(current_prog);

    std::vector<DWORD> available_com_ports;
    LoadCOMPorts( &available_com_ports );

    unsigned int prog_increment = 100/((available_com_ports.size()*5)+1); // 5 is the number of possible xid bauds
    bool scanning_canceled = false;

    for(std::vector<DWORD>::iterator iter = available_com_ports.begin(),
        end = available_com_ports.end();
        iter != end && !scanning_canceled; ++iter)
    {
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
                    DropEveryConnection();
                    scanning_canceled = true;
                    break;
                }
            }

            boost::shared_ptr<cedrus::Connection> xid_con(new Connection(*iter, baud_rate[i]));

            if(xid_con->Open() == XID_NO_ERR)
            {
                // This may seem like a good place to flush, but Open() has taken care of that by now.

                // NOTE THE USAGE OF XIDGlossaryPSTProof IN THIS CODE. IT'S IMPORTANT!
                std::string info = XIDGlossaryPSTProof::GetProtocol(xid_con);

                if( boost::starts_with( info, "_xid" ) )
                {
                    device_found = true;
                    bool mode_changed = false;

                    if(strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // Force the device into XID mode if it isn't. This is an XID library.
                        CommandGlossary::SetProtocol(xid_con, 0);

                        mode_changed = true;
                    }

                    int product_id, model_id;
                    int major_firmware_version = XIDGlossaryPSTProof::GetMajorFirmwareVersion(xid_con);

                    //What device is it? Get product/model ID, find the corresponding config
                    XIDGlossaryPSTProof::GetProductAndModelID(xid_con, &product_id, &model_id);

                    // call "CreateDevice" (which resides in DeviceFactory.cpp)
                    boost::shared_ptr<cedrus::XIDDevice> matched_dev =
                        CreateDevice( product_id,
                                       model_id,
                                       major_firmware_version,
                                       m_MasterConfigList,
                                       xid_con );

                    if ( matched_dev )
                    {
                        m_Devices.push_back( matched_dev );

                        if ( mode_changed && reportFunction )
                            reportFunction( matched_dev->GetDeviceConfig()->GetDeviceName() );

                        break;
                    }
                }
            }
        }
    }

    if ( progressFunction )
        progressFunction(100);

    return m_Devices.size();
}

boost::shared_ptr<cedrus::XIDDevice>
cedrus::XIDDeviceScanner::DeviceConnectionAtIndex(unsigned int i) const
{
    if(i >= m_Devices.size())
        return boost::shared_ptr<XIDDevice>();

    return m_Devices[i];
}

unsigned int cedrus::XIDDeviceScanner::DeviceCount() const
{
    return m_Devices.size();
}

const boost::shared_ptr<const cedrus::DeviceConfig>
cedrus::XIDDeviceScanner::DevconfigAtIndex(unsigned int i) const
{
    if(i >= m_MasterConfigList.size())
        return boost::shared_ptr<const DeviceConfig>();

    return m_MasterConfigList[i];
}

unsigned int cedrus::XIDDeviceScanner::DevconfigCount() const
{
    return m_MasterConfigList.size();
}
