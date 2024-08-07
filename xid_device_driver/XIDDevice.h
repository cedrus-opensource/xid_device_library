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

#include <string>

namespace Cedrus
{
    struct SignalFilter
    {
        unsigned int holdOn = 0;
        unsigned int holdOff = 0;
    };

    struct SingleShotMode
    {
        bool enabled = 0;
        unsigned int delay = 0;
    };

    class Connection;
    class DeviceConfig;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT XIDDevice
    {
    public:
        enum { IS_ANALOG_POD = 'V' };
        enum { AM_FIXED_DELTA = 1, AM_BINARY = 2 };   // AM = analog mode

        enum RipondaLEDFunction { LED_OFF = '0', LED_FOR_LIGHT_SENSOR = '1', LED_FOR_VOICE_KEY = '2' };

        XIDDevice(std::shared_ptr<Connection> xidCon, std::shared_ptr<const DeviceConfig> devConfig);

        ~XIDDevice();

        int GetOutputLogic() const; // _a0
        void SetOutputLogic(unsigned char mode); // a0
        int GetAccessoryConnectorMode() const; // _a1
        void SetAccessoryConnectorMode(unsigned char mode); // a1
        int GetACDebouncingTime() const; // _a6
        void SetACDebouncingTime(unsigned char time); // a6
        unsigned int GetFlashBackupCRC() const; // _ab (v2.2.2)
        void BackupFlashData(); // ab (v2.2.2)
        unsigned int GetTranslationTableCRC() const; // _ac (v2.2.2)
        bool IsMpodOutputEnabled() const; // _ae
        void EnableMpodOutput(bool enable); // ae
        unsigned char GetMpodOutputMode() const; // _am
        void SetMpodOutputMode(unsigned char mode); // am
        int GetMpodModel(unsigned char mpod) const; // _aq
        void ConnectToMpod(unsigned char mpod, unsigned char action); // aq
        unsigned char GetTranslationTable() const; // _as
        void SetTranslationTable(unsigned char table); // as
        unsigned int GetMappedSignals(unsigned int line); // _at
        void MapSignals(unsigned int line, unsigned int map); // at
        void ResetMappedLinesToDefault(); // atX
        void CommitLineMappingToFlash(); // af
        bool IsPodLocked() const; // _au (v2.2.2)
        unsigned int GetPodUnlockCRC() const; // _au (v2.2.2)
        void LockPod(bool lock); // au (v2.2.2)
        unsigned char GetMpodPulseDuration() const; // _aw
        void SetMpodPulseDuration(unsigned char duration); // aw
        char GetPodOutputLogic() const; // _al
        void SetPodOutputLogic(char logic); // al

        int GetVKDropDelay() const; // _b3
        void SetVKDropDelay(unsigned char delay); // b3

        std::string GetProtocol() const; // _c1
        static std::string GetProtocol_Scan(std::shared_ptr<Connection> xidCon); // _c1 used during device detection
        void SetProtocol(unsigned char protocol); // c1
        static void SetProtocol_Scan(std::shared_ptr<Connection> xidCon, unsigned char protocol); // c1
        unsigned int GetKBModeProtocol() const; // _c2
        void SetKBModeProtocol(unsigned char mode); // c2
        void SwitchToKeyboardMode(); // c3
        unsigned char GetCPodInputMode() const; // _c4
        void SetCPodInputMode(unsigned char mode); // c4
        std::string GetCombinedInfo() const; // _d0
        std::string GetInternalProductName() const; // _d1
        int GetProductID() const; // _d2
        int GetModelID() const; // _d3
        static int GetProductID_Scan(std::shared_ptr<Connection> xidCon); // _d2 used during device detection
        static int GetModelID_Scan(std::shared_ptr<Connection> xidCon); // _d3 used during device detection
        void SetModelID(unsigned char model); // d3
        int GetMajorFirmwareVersion() const; // _d4
        static int GetMajorFirmwareVersion_Scan(std::shared_ptr<Connection> xidCon); // _d4 used during device detection
        int GetMinorFirmwareVersion() const; // _d5
        int GetOutpostModel() const; // _d6
        int GetHardwareGeneration() const; // _d7

        void ResetBaseTimer(); // e1 (XID 1 Only)
        unsigned int QueryBaseTimer(); // e3 (XID 1 Only)
        unsigned int QueryRtTimer(); // _e5
        void ResetRtTimer(); // e5

        void SetBaudRate(unsigned char rate); // f1
        void GetLockingLevel(); // _f2
        void SetLockingLevel(unsigned char level); // f2
        void ReprogramFlash(); // f3
        bool GetTriggerDefault() const; // _f4
        void SetTriggerDefault(bool defaultOn); // f4
        int GetTriggerDebounceTime() const; // _f5
        void SetTriggerDebounceTime(unsigned char time); // f5
        int GetButtonDebounceTime() const; // _f6
        void SetButtonDebounceTime(unsigned char time); // f6
        void RestoreFactoryDefaults(); // f7
        void SaveSettingsToFlash(); //f9
        bool IsOpticalIsolationSwitchOn() const; //_fo

        SingleShotMode GetSingleShotMode (unsigned char selector) const; // _ia
        void SetSingleShotMode(unsigned char selector, bool enable, unsigned int delay); // ia
        unsigned char GetCPodInputLines() const; // _ic
        SignalFilter GetSignalFilter(unsigned char selector) const; // _if
        void SetSignalFilter(unsigned char selector, unsigned int holdOn, unsigned int holdOff); // if
        bool IsKbAutorepeatOn() const; // _ig (v2.2.1)
        void EnableKbAutorepeat(bool pause); // ig (v2.2.1)
        bool IsRBx40LEDEnabled() const; // _il
        void EnableRBx40LED(bool enable); // il
        unsigned int GetRipondaLEDFunction() const; // _il again
        void SetRipondaLEDFunction ( unsigned int nFunction ); // il again
        bool GetEnableDigitalOutput(unsigned char selector) const; // _io
        void SetEnableDigitalOutput(unsigned char selector, bool mode); // io
        bool IsOutputPaused() const; // _ip (v2.2.1)
        void PauseAllOutput(bool pause); // ip (v2.2.1)
        int GetTimerResetOnOnsetMode(unsigned char selector) const; // _ir
        void SetTimerResetOnOnsetMode(unsigned char selector, unsigned char mode); // ir
        bool GetEnableUSBOutput(unsigned char selector) const; // _iu
        void SetEnableUSBOutput(unsigned char selector, bool mode); // iu
        int GetAnalogInputThreshold(unsigned char selector) const; // _it
        void SetAnalogInputThreshold(unsigned char selector, unsigned char threshold); // it
        int GetMixedInputMode() const; // _iv
        void SetMixedInputMode(unsigned char mode); // iv

        unsigned int GetRaisedLines() const; // _mh / _ah
        unsigned int GetNumberOfLines() const; // _ml
        void SetNumberOfLines(unsigned int lines); // ml
        unsigned int GetPulseDuration() const; // _mp
        void SetPulseDuration(unsigned int duration); // mp
        unsigned int GetPulseTableBitMask(); // _mk
        void SetPulseTableBitMask(unsigned int lines); // mk
        void ClearPulseTable(); // mc
        bool IsPulseTableRunning() const; // _mr
        void RunPulseTable(); // mr
        void StopPulseTable(); // ms
        void AddPulseTableEntry(unsigned int time, unsigned int lines); // mt
        void ResetOutputLines(); // mz

        void SetVoltageRange ( unsigned int nMinimum, unsigned int nMaximum ); // vr
        unsigned int GetMaxVoltageRange() const; // _vr
        void SetVoltageRangeForTesting ( unsigned int nMinimum, unsigned int nMaximum ); // vt
        unsigned int GetMaxVoltageRangeForTesting() const; // _vt
        void SetAnalogOutputMode ( unsigned int mode ); // vm
        unsigned int GetAnalogOutputMode() const; // _vm
        void SetNumberOfAnalogOutputLevels ( unsigned int numLevels ); // vl
        unsigned int GetNumberOfAnalogOutputLevels() const; // _vl

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
        void SendPulse(unsigned int duration, unsigned int lines, unsigned int pulses, unsigned int ipi); // mx
        bool ArePulsesBeingSent() const; // _mx

    private:
        void SetDigitalOutputLines_RB(std::shared_ptr<Connection> xidCon, unsigned int lines);
        void SetDigitalOutputLines_ST(std::shared_ptr<Connection> xidCon, unsigned int lines);
        void MatchConfigToModel(char model);
        void MatchConfigToModel_MPod(char model);

        void SetMPodLineMapping_Neuroscan16bit();
        void SetMPodLineMapping_NeuroscanGrael();
        void SetCPodLineMapping_NeuroscanGrael();

        unsigned int m_linesState;
        std::shared_ptr<Connection> m_xidCon;
        std::shared_ptr<const DeviceConfig> m_config;
        std::shared_ptr<const DeviceConfig> m_podHostConfig;
        std::shared_ptr<ResponseManager> m_ResponseMgr;
        int m_baudRatePriorToMpod;
        unsigned int m_curMinorFwVer;
    };

} // namespace Cedrus
