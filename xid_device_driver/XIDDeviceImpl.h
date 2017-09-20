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

#include "XIDDevice.h"

#include <boost/utility.hpp>

namespace Cedrus
{
    class Connection;
    class DeviceConfig;

    class XIDDeviceImpl : public XIDDevice
    {
    public:
        XIDDeviceImpl(
            std::shared_ptr<Connection> xidCon,
            std::shared_ptr<const DeviceConfig> devConfig);

        virtual ~XIDDeviceImpl();

        virtual int GetOutputLogic() const;
        virtual void SetOutputLogic(unsigned char mode);
        virtual int GetAccessoryConnectorMode() const;
        virtual void SetAccessoryConnectorMode(unsigned char mode);
        virtual int GetACDebouncingTime() const;
        virtual void SetACDebouncingTime(unsigned char time);
        virtual int GetMpodModel(unsigned int mpod) const;
        virtual void ConnectToMpod(unsigned int mpod, unsigned int action);

        virtual int GetVKDropDelay() const;
        virtual void SetVKDropDelay(unsigned char delay);

        virtual std::string GetProtocol() const;
        static std::string GetProtocol(std::shared_ptr<Connection> xidCon);
        virtual void SetProtocol(unsigned char protocol);
        static void SetProtocol(std::shared_ptr<Connection> xidCon, unsigned char protocol);
        std::string GetInternalProductName() const;
        virtual int GetProductID() const;
        virtual int GetModelID() const;
        static int GetProductID(std::shared_ptr<Connection> xidCon); // used during device detection
        static int GetModelID(std::shared_ptr<Connection> xidCon); // used during device detection
        virtual void SetModelID(unsigned char model);
        virtual int GetMajorFirmwareVersion() const;
        static int GetMajorFirmwareVersion(std::shared_ptr<Connection> xidCon); // used during device detection
        virtual int GetMinorFirmwareVersion() const;
        virtual int GetOutpostModel() const;
        virtual int GetHardwareGeneration() const;

        virtual void ResetBaseTimer();
        virtual unsigned int QueryBaseTimer();
        virtual void ResetRtTimer();

        virtual void SetBaudRate(unsigned char rate);
        virtual void GetLockingLevel();
        virtual void SetLockingLevel(unsigned char level);
        virtual void ReprogramFlash();
        virtual int GetTriggerDefault() const;
        virtual void SetTriggerDefault(bool defaultOn);
        virtual int GetTriggerDebounceTime() const;
        virtual void SetTriggerDebounceTime(unsigned char time);
        virtual int  GetButtonDebounceTime() const;
        virtual void SetButtonDebounceTime(unsigned char time);
        virtual void RestoreFactoryDefaults();

        virtual int GetNumberOfLines() const;
        virtual void SetNumberOfLines(unsigned int lines);
        virtual unsigned int GetPulseDuration() const;
        virtual void SetPulseDuration(unsigned int duration);

        // These are getting replaced with ir, im, it, iv and such commands
        virtual int GetLightSensorMode() const;
        virtual void SetLightSensorMode(unsigned char mode);
        virtual int GetLightSensorThreshold() const;
        virtual void SetLightSensorThreshold(unsigned char threshold);

        // The following two blocks of commands do not query the device directly
        virtual int GetBaudRate() const;
        virtual std::shared_ptr<const DeviceConfig> GetDeviceConfig() const;
        virtual int OpenConnection() const;
        virtual int CloseConnection() const;
        virtual bool HasLostConnection() const;

        // These are for getting button input from an RB
        virtual void PollForResponse() const;
        virtual bool HasQueuedResponses() const;
        virtual int GetNumberOfKeysDown() const;
        virtual Cedrus::Response GetNextResponse() const;
        virtual void ClearResponseQueue(); // Clear processed responses
        virtual void ClearResponsesFromBuffer(); // Clear characters from the physical buffer

        virtual void RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines = false);
        virtual void LowerLines(unsigned int linesBitmask, bool leaveRemainingLines = false);
        virtual void ClearLines();

    private:
        void SetDigitalOutputLines_RB(std::shared_ptr<Connection> xidCon, unsigned int lines);
        void SetDigitalOutputLines_ST(std::shared_ptr<Connection> xidCon, unsigned int lines);

        unsigned int m_linesState;
        std::shared_ptr<Connection> m_xidCon;
        std::shared_ptr<const Cedrus::DeviceConfig> m_config;
        const std::shared_ptr<Cedrus::ResponseManager> m_ResponseMgr;
    };

} // namespace Cedrus
