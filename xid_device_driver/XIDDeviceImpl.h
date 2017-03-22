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

#ifndef XID_DEVICE_IMPL_H
#define XID_DEVICE_IMPL_H

#include "XIDDevice.h"

#include <boost/utility.hpp>

namespace cedrus
{
    class Connection;
    class DeviceConfig;

    class XIDDeviceImpl : public XIDDevice
    {
    public:
        XIDDeviceImpl(
            boost::shared_ptr<Connection> xidCon,
            boost::shared_ptr<const DeviceConfig> devConfig);

        virtual ~XIDDeviceImpl();

        virtual int GetOutpostModel();

        virtual int GetHardwareGeneration();

        virtual int GetLightSensorMode();

        virtual void SetLightSensorMode(unsigned char mode);

        virtual void SetLightSensorThreshold(unsigned char threshold);

        virtual int GetLightSensorThreshold();

        virtual void SetScannerTriggerFilter(unsigned char mode);

        virtual void ResetRtTimer();

        virtual void ResetBaseTimer();

        virtual unsigned int QueryBaseTimer();

        virtual unsigned int GetPulseDuration();

        virtual void SetPulseDuration(unsigned int duration);

        virtual int GetNumberOfLines();

        virtual void SetNumberOfLines(unsigned int lines);

        virtual void PollForResponse();

        virtual bool HasQueuedResponses();

        virtual int GetNumberOfKeysDown() const;

        virtual void ClearResponseQueue();

        virtual cedrus::response GetNextResponse();

        virtual void ClearResponses();

        virtual int GetAccessoryConnectorMode();

        virtual int GetAccessoryConnectorDevice();

        virtual int GetOutputLogic();

        virtual void SetOutputLogic(int mode);

        virtual void SetAccessoryConnectorMode(int mode);

        virtual int GetVKDropDelay();

        virtual void SetVKDropDelay(unsigned char delay);

        virtual void ReprogramFlash();

        virtual int GetTriggerDefault();

        virtual void SetTriggerDefault(bool defaultOn);

        virtual int GetTriggerDebounceTime();

        virtual void SetTriggerDebounceTime(unsigned char time);

        virtual int  GetButtonDebounceTime();

        virtual void SetButtonDebounceTime(unsigned char time);

        virtual void SetProtocol(unsigned char protocol);

        virtual void ConnectToMpod(unsigned int mpod, unsigned int action);

        // Every device needs these.
        virtual const boost::shared_ptr<const DeviceConfig> GetDeviceConfig() const;
        virtual int OpenConnection();
        virtual int CloseConnection();
        virtual bool HasLostConnection();
        virtual int GetBaudRate();
        virtual void SetBaudRate(unsigned char rate);
        virtual std::string GetProtocol();
        virtual void GetProductAndModelID(int *productID, int *modelID);
        virtual void SetModelID(unsigned char model);
        virtual int GetMajorFirmwareVersion();
        virtual int GetMinorFirmwareVersion();
        virtual std::string GetInternalProductName();
        virtual void RestoreFactoryDefaults();

        // Implementations of these will differ
        virtual void RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void LowerLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void ClearLines() = 0;

        virtual ProductAndModelID GetProductAndModelID();

    protected:
        unsigned int m_linesState;
        boost::shared_ptr<Connection> m_xidCon;
        const boost::shared_ptr<const cedrus::DeviceConfig> m_config;
        const boost::shared_ptr<cedrus::ResponseManager> m_ResponseMgr;
    };

} // namespace cedrus

#endif // XID_DEVICE_IMPL_H
