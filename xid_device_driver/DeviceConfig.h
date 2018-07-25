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

#include "constants.h"
#include "XidDriverImpExpDefs.h"

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace Cedrus
{
    struct DevicePort
    {
        DevicePort():
            portName(""),
            portNumber(-1),
            numberOfLines(-1),
            keyMap(8, -1)
        {
        }

        DevicePort(std::string portName,
            int portNumber,
            int numberOfLines,
            std::vector<int> keyMap)
            :
            portName(portName),
            portNumber(portNumber),
            numberOfLines(numberOfLines),
            keyMap(keyMap)
        {
        }

        DevicePort(std::string portName, int portNumber, int numberOfLines)
            :
            portName(portName),
            portNumber(portNumber),
            numberOfLines(numberOfLines),
            keyMap()
        {
        }

        std::string portName;
        int portNumber;
        int numberOfLines;
        std::vector<int> keyMap;

        bool operator==( const DevicePort& other ) const
        {
            return ( portName == other.portName &&
                     portNumber == other.portNumber &&
                     numberOfLines == other.numberOfLines &&
                     keyMap == other.keyMap );
        }
    };

    class CEDRUS_XIDDRIVER_IMPORTEXPORT DeviceConfig
    {
    public:
        DeviceConfig(std::string deviceName,
            int productID,
            int modelID,
            int majorFirmwareVer,
            unsigned int outputLines,
            std::map<unsigned int, DevicePort> devicePorts);

        ~DeviceConfig(void);

        static void PopulateConfigList(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs);
        static void CreateInvalidConfig(std::shared_ptr<Cedrus::DeviceConfig> & invalidCfgPtr);

        int GetMappedKey(int port, int key) const;

        std::string GetDeviceName() const;

        int GetProductID() const;

        int GetModelID() const;

        int GetMajorVersion() const;

        unsigned int GetNumberOfOutputLines() const;

        const std::map<unsigned int, DevicePort> * GetMapOfPorts() const;

        const Cedrus::DevicePort * GetPortPtrByNumber(unsigned int portNum) const;

        bool DoesConfigMatchDevice( int deviceID, int modelID, int majorFirmwareVer ) const;

        bool NeedsDelay() const;

        bool IsLumina() const
        {
            return m_ProductID == PRODUCT_ID_LUMINA;
        }

        bool IsLuminaLP400() const
        {
            return m_ProductID == PRODUCT_ID_LUMINA && IsXID1();
        }

        bool IsLumina3G() const
        {
            return m_ProductID == PRODUCT_ID_LUMINA && IsXID2();
        }

        bool IsSV1() const
        {
            return m_ProductID == PRODUCT_ID_SV1;
        }

        bool IsRB() const
        {
            return m_ProductID == PRODUCT_ID_RB;
        }

        bool IsRBx40() const
        {
            return m_ProductID == PRODUCT_ID_RB && IsXID2();
        }

        bool IsMPod() const
        {
            return m_ProductID == PRODUCT_ID_MPOD;
        }

        bool IsCPod() const
        {
            return m_ProductID == PRODUCT_ID_CPOD;
        }

        bool IsStimTracker() const
        {
            return m_ProductID == PRODUCT_ID_STIMTRACKER;
        }

        bool IsStimTracker1() const
        {
            return IsStimTracker() && IsXID1();
        }

        bool IsStimTracker2() const
        {
            return IsStimTracker() && IsXID2();
        }

        bool IsXID1() const
        {
            return m_MajorFirmwareVer == 1;
        }

        bool IsXID2() const
        {
            return m_MajorFirmwareVer == 2;
        }

        bool IsXID1InputDevice() const
        {
            return IsXID1() && !IsStimTracker();
        }

        bool ModelIDMatters() const
        {
            return IsRB() || IsStimTracker() || IsCPod() || IsMPod();
        }

    private:
        std::string m_DeviceName;
        int m_ProductID;
        int m_ModelID;
        int m_MajorFirmwareVer;
        bool m_requiresDelay;
        unsigned int m_outputLines;

        std::map<unsigned int, DevicePort> m_DevicePorts;
    };
} // namespace Cedrus
