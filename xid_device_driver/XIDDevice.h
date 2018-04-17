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

#include <boost/utility.hpp>

namespace Cedrus
{
    class Connection;
    class DeviceConfig;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT XIDDevice
    {
    public:
        XIDDevice(std::shared_ptr<Connection> xidCon, std::shared_ptr<const DeviceConfig> devConfig);

        ~XIDDevice();

        int GetOutputLogic() const; // _a0
        void SetOutputLogic(unsigned char mode); // a0
        int GetAccessoryConnectorMode() const; // _a1
        void SetAccessoryConnectorMode(unsigned char mode); // a1
        int GetACDebouncingTime() const; // _a6
        void SetACDebouncingTime(unsigned char time); // a6
        int GetMpodModel(unsigned int mpod) const; // _aq
        void ConnectToMpod(unsigned int mpod, unsigned int action); // aq
        unsigned int GetMappedSignals(unsigned int line); // _at
        void MapSignals(unsigned int line, unsigned int map); // at
        void ResetMappedLinesToDefault(); // atX
        void CommitLineMappingToFlash(); // af

        int GetVKDropDelay() const; // _b3
        void SetVKDropDelay(unsigned char delay); // b3

        std::string GetProtocol() const; // _c1
        static std::string GetProtocol(std::shared_ptr<Connection> xidCon); // _c1 used during device detection
        void SetProtocol(unsigned char protocol); // c1
        static void SetProtocol(std::shared_ptr<Connection> xidCon, unsigned char protocol); // c1
        std::string GetCombinedInfo() const; // _d0
        std::string GetInternalProductName() const; // _d1
        int GetProductID() const; // _d2
        int GetModelID() const; // _d3
        static int GetProductID(std::shared_ptr<Connection> xidCon); // _d2 used during device detection
        static int GetModelID(std::shared_ptr<Connection> xidCon); // _d3 used during device detection
        void SetModelID(unsigned char model); // d3
        int GetMajorFirmwareVersion() const; // _d4
        static int GetMajorFirmwareVersion(std::shared_ptr<Connection> xidCon); // _d4 used during device detection
        int GetMinorFirmwareVersion() const; // _d5
        int GetOutpostModel() const; // _d6
        int GetHardwareGeneration() const; // _d7

        void ResetBaseTimer(); // e1
        unsigned int QueryBaseTimer(); // e3
        void ResetRtTimer(); // e5

        void SetBaudRate(unsigned char rate); // f1
        void GetLockingLevel(); // _f2
        void SetLockingLevel(unsigned char level); // f2
        void ReprogramFlash(); // f3
        int GetTriggerDefault() const; // _f4
        void SetTriggerDefault(bool defaultOn); // f4
        int GetTriggerDebounceTime() const; // _f5
        void SetTriggerDebounceTime(unsigned char time); // f5
        int GetButtonDebounceTime() const; // _f6
        void SetButtonDebounceTime(unsigned char time); // f6
        void RestoreFactoryDefaults(); // f7
        void SaveSettingsToFlash(); //f9

        int GetTimerResetOnOnsetMode(unsigned char selector) const; // _ir
        void SetTimerResetOnOnsetMode(unsigned char selector, unsigned char mode); // ir
        bool GetGenerateTimestampedOutput(unsigned char selector) const; // _io
        void SetGenerateTimestampedOutput(unsigned char selector, unsigned char mode); // io
        int GetAnalogInputThreshold(unsigned char selector) const; // _it
        void SetAnalogInputThreshold(unsigned char selector, unsigned char threshold); // it
        int GetMpodOutputMode(unsigned char selector) const; // _im
        void SetMpodOutputMode(unsigned char selector, unsigned char mode); // im
        int GetMixedInputMode() const; // _iv
        void SetMixedInputMode(unsigned char mode); // iv

        unsigned int GetNumberOfLines() const; // _ml
        void SetNumberOfLines(unsigned int lines); // ml
        unsigned int GetPulseDuration() const; // _mp
        void SetPulseDuration(unsigned int duration); // mp

        // The following two blocks of commands do not query the device directly
        int GetBaudRate() const;
        std::shared_ptr<const DeviceConfig> GetDeviceConfig() const;
        int OpenConnection() const;
        int CloseConnection() const;
        bool HasLostConnection() const;

        // These are for getting button input from an RB
        void PollForResponse() const;
        bool HasQueuedResponses() const;
        unsigned int GetNumberOfKeysDown() const;
        Cedrus::Response GetNextResponse() const;
        void ClearResponseQueue(); // Clear processed responses
        void ClearResponsesFromBuffer(); // Clear characters from the physical buffer

         // mh or ah
        void RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines = false);
        void LowerLines(unsigned int linesBitmask, bool leaveRemainingLines = false);
        void ClearLines();

    private:
        void SetDigitalOutputLines_RB(std::shared_ptr<Connection> xidCon, unsigned int lines);
        void SetDigitalOutputLines_ST(std::shared_ptr<Connection> xidCon, unsigned int lines);
        void MatchConfigToModel(char model);

        unsigned int m_linesState;
        std::shared_ptr<Connection> m_xidCon;
        std::shared_ptr<const DeviceConfig> m_config;
        const std::shared_ptr<ResponseManager> m_ResponseMgr;
    };

} // namespace Cedrus
