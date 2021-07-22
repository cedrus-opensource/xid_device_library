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

#pragma once

#include "XidDriverImpExpDefs.h"

#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace Cedrus
{
    class Connection;
    class XIDDevice;
    class StimTracker;
    class DeviceConfig;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT XIDDeviceScanner
    {
    private:
        XIDDeviceScanner();
    public:
        static XIDDeviceScanner& GetDeviceScanner();

        void CloseAllConnections();

        void OpenAllConnections();

        void DropEveryConnection();

        void DropConnectionByPtr(std::shared_ptr<XIDDevice> device);

        void CheckConnectionsDropDeadOnes();

        // This does a clean scan for devices.
        //  reportFunction is for reporting errors during the scanning process
        //  progressFunction is for reporting progress on a 0-100 scale and the
        // return value is to signal that we need to cancel the scanning process.
        // true for stop, false for don't
        int DetectXIDDevices(
            std::function< void(std::string) > reportFunction = NULL,
            std::function< bool(unsigned int) > progressFunction = NULL);

        std::shared_ptr<XIDDevice> DeviceConnectionAtIndex(unsigned int i) const;

        unsigned int DeviceCount() const;

        std::shared_ptr<const DeviceConfig> DevconfigAtIndex(unsigned int i) const;

        unsigned int DevconfigCount() const;

        std::shared_ptr<const DeviceConfig> GetConfigForGivenDevice(int deviceID, int modelID, int majorFirmwareVer) const;

    private:
        std::vector<std::shared_ptr<XIDDevice> > m_Devices;
        std::vector<std::shared_ptr<DeviceConfig> > m_MasterConfigList;
        std::shared_ptr<DeviceConfig> m_emptyConfig;
    };
} // namespace Cedrus
