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

#include "DeviceConfig.h"
#include "Connection.h"

#include "XIDDevice.h"

std::shared_ptr<Cedrus::XIDDevice> CreateDevice
(
    const int productID, // d2 value
    const int modelID,   // d3 value
    const int majorFirmwareVersion, // d4 value
    const std::vector<std::shared_ptr<Cedrus::DeviceConfig> > &configCandidates,
    std::shared_ptr<Cedrus::Connection> xidCon
)
{
    std::shared_ptr<Cedrus::XIDDevice> result;

    // match the product/model id and firmware version to a devconfig
    for (unsigned int i = 0; i < configCandidates.size(); ++i)
    {
        if (configCandidates[i]->DoesConfigMatchDevice(productID, modelID, majorFirmwareVersion))
        {
            xidCon->SetCmdThroughputLimit(configCandidates[i]->IsXID2());
            result.reset(new Cedrus::XIDDevice(xidCon, configCandidates[i]));
            break;
        }
    }

    return result;
}

void LoadCOMPorts(std::vector<DWORD> * availableCOMPorts)
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

        if (status == FT_OK)
        {
            for (unsigned int i = 0; i < num_devs; i++)
            {
                availableCOMPorts->push_back(dev_info[i].LocId);
            }
        }

        free(dev_info);
    }
}

Cedrus::XIDDeviceScanner::XIDDeviceScanner()
{
    DeviceConfig::PopulateConfigList(m_MasterConfigList);
    DeviceConfig::CreateInvalidConfig(m_emptyConfig);
}

Cedrus::XIDDeviceScanner& Cedrus::XIDDeviceScanner::GetDeviceScanner()
{
    static XIDDeviceScanner deviceScanner = XIDDeviceScanner();

    return deviceScanner;
}

void Cedrus::XIDDeviceScanner::CloseAllConnections()
{
    for (unsigned int i = 0; i < m_Devices.size(); i++)
        m_Devices[i]->CloseConnection();
}

// This may seem like an odd function to have, but it can be used to short-circuit logic
// by preventing existing devices from being picked up during a scan.
void Cedrus::XIDDeviceScanner::OpenAllConnections()
{
    for (unsigned int i = 0; i < m_Devices.size(); i++)
        m_Devices[i]->OpenConnection();
}

void Cedrus::XIDDeviceScanner::DropEveryConnection()
{
    CloseAllConnections();

    m_Devices.clear();
}

void Cedrus::XIDDeviceScanner::DropConnectionByPtr(std::shared_ptr<Cedrus::XIDDevice> device)
{
    for (std::vector< std::shared_ptr<Cedrus::XIDDevice> >::iterator iter = m_Devices.begin();
        iter != m_Devices.end(); )
    {
        if (device == *iter)
        {
            (*iter)->CloseConnection();
            iter = m_Devices.erase(iter);
        }
        else
            ++iter;
    }
}

void Cedrus::XIDDeviceScanner::CheckConnectionsDropDeadOnes()
{
    CloseAllConnections();
    std::vector< std::shared_ptr<Cedrus::XIDDevice> >::iterator iter = m_Devices.begin();
    while (iter != m_Devices.end())
    {
        bool drop_connection = (*iter)->OpenConnection() != XID_NO_ERR;

        // If the connection held up, we still need to verify the device is what
        // it used to be. XID devices are largely interchangeable as far as the library
        // is concerned, but sending commands to the wrong one can make things
        // very confusing.
        if (!drop_connection)
        {
            int product_id = (*iter)->GetProductID();
            int model_id = (*iter)->GetModelID();

            int major_firmware_version = (*iter)->GetMajorFirmwareVersion();

            if ((*iter)->GetDeviceConfig()->DoesConfigMatchDevice(product_id, model_id, major_firmware_version))
            {
                ++iter;
            }
            else
                drop_connection = true; // The device is not what we thought it was.
        }

        if (drop_connection)
        {
            (*iter)->CloseConnection();
            iter = m_Devices.erase(iter);
        }
    }

    CloseAllConnections();
}

int Cedrus::XIDDeviceScanner::DetectXIDDevices
(
    std::function< void(std::string) > reportFunction,
    std::function< bool(unsigned int) > progressFunction
)
{
    CheckConnectionsDropDeadOnes();
    OpenAllConnections();

    unsigned int current_prog = 0;

    if (progressFunction)
        progressFunction(current_prog);

    std::vector<DWORD> available_com_ports;
    LoadCOMPorts(&available_com_ports);

    unsigned int prog_increment = 100 / ((available_com_ports.size() * 5) + 1); // 5 is the number of possible xid bauds
    bool scanning_canceled = false;

    for (std::vector<DWORD>::iterator iter = available_com_ports.begin(),
        end = available_com_ports.end();
        iter != end && !scanning_canceled; ++iter)
    {
        bool device_found = false;
        const int baud_rate[] = { 115200, 19200, 9600, 57600, 38400 };
        const int num_bauds = sizeof(baud_rate) / sizeof(int);

        // Here we're going to actually connect to a port and send it some signals. Our aim here is to
        // get an XID device's product/device and model IDs.
        for (int i = 0; i < num_bauds && !device_found; ++i)
        {
            // Update progress
            current_prog += prog_increment;
            if (progressFunction)
            {
                if (progressFunction(current_prog))
                {
                    DropEveryConnection();
                    scanning_canceled = true;
                    break;
                }
            }

            std::shared_ptr<Cedrus::Connection> xid_con(new Connection(*iter, baud_rate[i]));

            if (xid_con->Open() == XID_NO_ERR)
            {
                // This may seem like a good place to flush, but Open() has taken care of that by now.

                // NOTE THE USAGE OF XIDGlossaryPSTProof IN THIS CODE. IT'S IMPORTANT!
                std::string info = XIDDevice::GetProtocol_Scan(xid_con);

                if (info.rfind("_xid", 0) == 0)
                {
                    device_found = true;
                    bool mode_changed = false;

                    if (strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // Force the device into XID mode if it isn't. This is an XID library.
                        XIDDevice::SetProtocol_Scan(xid_con, 0);

                        mode_changed = true;
                    }

                    int product_id = XIDDevice::GetProductID_Scan(xid_con);
                    int model_id = XIDDevice::GetModelID_Scan(xid_con);
                    int major_firmware_version = XIDDevice::GetMajorFirmwareVersion_Scan(xid_con);

                    std::shared_ptr<Cedrus::XIDDevice> matched_dev =
                        CreateDevice(product_id,
                            model_id,
                            major_firmware_version,
                            m_MasterConfigList,
                            xid_con);

                    if (matched_dev)
                    {
                        m_Devices.push_back(matched_dev);

                        if (mode_changed && reportFunction)
                            reportFunction(matched_dev->GetDeviceConfig()->GetDeviceName());

                        break;
                    }
                }
            }
        }
    }

    if (progressFunction)
        progressFunction(100);

    return m_Devices.size();
}

std::shared_ptr<Cedrus::XIDDevice> Cedrus::XIDDeviceScanner::DeviceConnectionAtIndex(unsigned int i) const
{
    if (i >= m_Devices.size())
        return std::shared_ptr<XIDDevice>();

    return m_Devices[i];
}

std::shared_ptr<Cedrus::XIDDevice> Cedrus::XIDDeviceScanner::GetDeviceOfGivenProductID(Cedrus::XidProductID devID) const
{
    if (devID == Cedrus::XidProductID::UNDEFINED)
        return m_Devices[0];

    for (auto i : m_Devices)
    {
        if (i->GetDeviceConfig()->GetProductID() == devID)
            return i;
    }

    return nullptr;
}

unsigned int Cedrus::XIDDeviceScanner::DeviceCount() const
{
    return m_Devices.size();
}

std::shared_ptr<const Cedrus::DeviceConfig> Cedrus::XIDDeviceScanner::DevconfigAtIndex(unsigned int i) const
{
    if (i >= m_MasterConfigList.size())
        return std::shared_ptr<const DeviceConfig>();

    return m_MasterConfigList[i];
}

unsigned int Cedrus::XIDDeviceScanner::DevconfigCount() const
{
    return m_MasterConfigList.size();
}

std::shared_ptr<const Cedrus::DeviceConfig> Cedrus::XIDDeviceScanner::GetConfigForGivenDevice(int deviceID, int modelID, int majorFirmwareVer) const
{
    std::shared_ptr<const Cedrus::DeviceConfig> config;

    for (unsigned int i = 0; i < m_MasterConfigList.size(); ++i)
    {
        if (m_MasterConfigList[i]->DoesConfigMatchDevice(deviceID, modelID, majorFirmwareVer))
        {
            config = m_MasterConfigList[i];
            break;
        }
    }

    // The model value didn't match any known pod configs. Give the "no model set" config instead.
    if (!config)
    {
        if (deviceID == XidProductID::MPOD || deviceID == XidProductID::CPOD)
            config = XIDDeviceScanner::GetDeviceScanner().GetConfigForGivenDevice(deviceID, '0', majorFirmwareVer);
        else
            config = m_emptyConfig;
    }

    return config;
}
