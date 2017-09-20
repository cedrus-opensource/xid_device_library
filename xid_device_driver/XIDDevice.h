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
#include "ResponseManager.h"

namespace Cedrus
{
    class DeviceConfig;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT XIDDevice : public boost::noncopyable
    {
    public:
        virtual ~XIDDevice()
        {
        }

        // _a0
        virtual int GetOutputLogic() const = 0;
        // a0
        virtual void SetOutputLogic(unsigned char mode) = 0;
        // _a1
        virtual int GetAccessoryConnectorMode() const = 0;
        // a1
        virtual void SetAccessoryConnectorMode(unsigned char mode) = 0;
        // _a6
        virtual int GetACDebouncingTime() const = 0;
        // a6
        virtual void SetACDebouncingTime(unsigned char time) = 0;
        // _aq
        virtual int GetMpodModel(unsigned int mpod) const = 0;
        // aq
        virtual void ConnectToMpod(unsigned int mpod, unsigned int action) = 0;

        // _b3
        virtual int GetVKDropDelay() const = 0;
        // b3
        virtual void SetVKDropDelay(unsigned char delay) = 0;

        // _c1
        virtual std::string GetProtocol() const = 0;
        // c1
        virtual void SetProtocol(unsigned char protocol) = 0;
        // _d1
        virtual std::string GetInternalProductName() const = 0;
        // _d2
        virtual int GetProductID() const = 0;
        // _d3
        virtual int GetModelID() const = 0;
        // d3
        virtual void SetModelID(unsigned char model) = 0;
        // _d4
        virtual int GetMajorFirmwareVersion() const = 0;
        // _d5
        virtual int GetMinorFirmwareVersion() const = 0;
        // _d6
        virtual int GetOutpostModel() const = 0;
        // _d7
        virtual int GetHardwareGeneration() const = 0;

        // e1
        virtual void ResetBaseTimer() = 0;
        // e3
        virtual unsigned int QueryBaseTimer() = 0;
        // e5
        virtual void ResetRtTimer() = 0;

        // f1
        virtual void SetBaudRate(unsigned char rate) = 0;
        // _f2
        virtual void GetLockingLevel() = 0;
        // f2
        virtual void SetLockingLevel(unsigned char level) = 0;
        // f3
        virtual void ReprogramFlash() = 0;
        // _f4
        virtual int GetTriggerDefault() const = 0;
        // f4
        virtual void SetTriggerDefault(bool defaultOn) = 0;
        // _f5
        virtual int GetTriggerDebounceTime() const = 0;
        // f5
        virtual void SetTriggerDebounceTime(unsigned char time) = 0;
        // _f6
        virtual int GetButtonDebounceTime() const = 0;
        // f6
        virtual void SetButtonDebounceTime(unsigned char time) = 0;
        // f7
        virtual void RestoreFactoryDefaults() = 0;

        // _ml
        virtual int GetNumberOfLines() const = 0;
        //ml
        virtual void SetNumberOfLines(unsigned int lines) = 0;
        // _mp
        virtual unsigned int GetPulseDuration() const = 0;
        // mp
        virtual void SetPulseDuration(unsigned int duration) = 0;

        // These are getting replaced with ir, im, it, iv and such commands
        // _lr
        virtual int GetLightSensorMode() const = 0;
        // lr
        virtual void SetLightSensorMode(unsigned char mode) = 0;
        // _lt
        virtual int GetLightSensorThreshold() const = 0;
        // lt
        virtual void SetLightSensorThreshold(unsigned char threshold) = 0;

        // The following two blocks of commands do not query the device directly
        virtual int GetBaudRate() const = 0;
        virtual std::shared_ptr<const DeviceConfig> GetDeviceConfig() const = 0;
        virtual int OpenConnection() const = 0;
        virtual int CloseConnection() const = 0;
        virtual bool HasLostConnection() const = 0;

        // These are for getting button input from an RB
        virtual void PollForResponse() const = 0;
        virtual bool HasQueuedResponses() const = 0;
        virtual int GetNumberOfKeysDown() const = 0;
        virtual Cedrus::Response GetNextResponse() const = 0;
        virtual void ClearResponseQueue() = 0; // Clear processed responses
        virtual void ClearResponsesFromBuffer() = 0; // Clear characters from the physical buffer

        // ah or mh
        virtual void RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void LowerLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void ClearLines() = 0;
    };

} // namespace Cedrus
