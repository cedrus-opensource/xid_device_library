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

#include "XIDDevice.h"

#include "constants.h"
#include "ResponseManager.h"
#include "Connection.h"
#include "DeviceConfig.h"
#include "XIDDeviceScanner.h"

#include "CedrusAssert.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <locale>


Cedrus::XIDDevice::XIDDevice(std::shared_ptr<Connection> xidCon, std::shared_ptr<const DeviceConfig> devConfig)
  : m_linesState(0),
    m_xidCon(xidCon),
    m_config(devConfig),
    m_podHostConfig(),
    m_ResponseMgr(devConfig->IsInputDevice() ? new ResponseManager(m_config) : nullptr),
    m_baudRatePriorToMpod(115200),
    m_curMinorFwVer(GetMinorFirmwareVersion())
{
}

Cedrus::XIDDevice::~XIDDevice()
{
}

int Cedrus::XIDDevice::GetOutputLogic() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char output_logic[4];

    m_xidCon->SendXIDCommand("_a0", 3, output_logic, sizeof(output_logic));

    bool return_valid_a0 = strncmp((char*)output_logic, "_a0", 3) == 0;

    return return_valid_a0 ? output_logic[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetOutputLogic(unsigned char mode)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char sol_cmd[3] = { 'a', '0' };
    sol_cmd[2] = mode + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(sol_cmd, 3, &bytes_written);
}

int Cedrus::XIDDevice::GetAccessoryConnectorMode() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    m_xidCon->SendXIDCommand("_a1", 3, return_info, sizeof(return_info));

    bool return_valid_a1 = strncmp((char*)return_info, "_a1", 3) == 0;
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT(return_valid_a1, "GetAccessoryConnectorMode's return value must start with _a1");
    CEDRUS_ASSERT(return_valid_val, "GetAccessoryConnectorMode's return value must be between '0' and '3'");

    return (return_valid_a1 && return_valid_val) ? return_info[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetAccessoryConnectorMode(unsigned char mode)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char sacm_cmd[3] = { 'a','1' };
    sacm_cmd[2] = mode + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(sacm_cmd, 3, &bytes_written);
}

int Cedrus::XIDDevice::GetACDebouncingTime() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char threshold_return[4];

    m_xidCon->SendXIDCommand("_a6", 3, threshold_return, sizeof(threshold_return));

    bool return_valid = strncmp((char*)threshold_return, "_a6", 3) == 0;

    CEDRUS_ASSERT(return_valid, "GetACDebouncingTime's xid query result must start with _a6");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetACDebouncingTime(unsigned char time)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char sacdt_command[3] = {'a','6'};
    sacdt_command[2] = time;

    DWORD bytes_written = 0;
    m_xidCon->Write(sacdt_command, 3, &bytes_written);
}

unsigned int Cedrus::XIDDevice::GetFlashBackupCRC() const
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return 0;

    unsigned char crc_return[7];
    m_xidCon->SendXIDCommand("_ab", 3, crc_return, sizeof(crc_return));

    unsigned int crc = AdjustEndiannessCharsToUint(
        crc_return[3],
        crc_return[4],
        crc_return[5],
        crc_return[6]);

    return crc;
}

void Cedrus::XIDDevice::BackupFlashData()
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return;

    // Get the CRC to unlock the m-pod
    unsigned char crc_return[7];
    m_xidCon->SendXIDCommand("_ab", 3, crc_return, sizeof(crc_return));

    static unsigned char spdtfb_cmd[6] = { 'a','b' };
    spdtfb_cmd[2] = crc_return[3];
    spdtfb_cmd[3] = crc_return[4];
    spdtfb_cmd[4] = crc_return[5];
    spdtfb_cmd[5] = crc_return[6];

    DWORD bytes_written = 0;
    m_xidCon->Write ( spdtfb_cmd, 6, &bytes_written, SAVES_TO_FLASH );
}

unsigned int Cedrus::XIDDevice::GetTranslationTableCRC() const
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return 0;

    unsigned char crc_return[7];
    m_xidCon->SendXIDCommand("_ac", 3, crc_return, sizeof(crc_return));

    unsigned int crc = AdjustEndiannessCharsToUint(
        crc_return[3],
        crc_return[4],
        crc_return[5],
        crc_return[6]);

    return crc;
}

bool Cedrus::XIDDevice::IsMpodOutputEnabled() const
{
    if (!m_config->IsXID2InputDevice())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_ae", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::EnableMpodOutput(bool enable)
{
    if (!m_config->IsXID2InputDevice())
        return;

    static unsigned char emo_command[3] = { 'a','e' };
    emo_command[2] = enable ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(emo_command, 3, &bytes_written);
}

unsigned char Cedrus::XIDDevice::GetMpodOutputMode() const
{
    if (!m_config->IsMPod())
        return '0';

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_am", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] - '0';
}

void Cedrus::XIDDevice::SetMpodOutputMode(unsigned char mode)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char smom_command[3] = { 'a','m' };
    smom_command[2] = mode + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write ( smom_command, 3, &bytes_written, SAVES_TO_FLASH );

    SLEEP_FUNC(50 * SLEEP_INC);
}

bool Cedrus::XIDDevice::IsPodLocked() const
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return false;

    unsigned char locked_return[8];
    m_xidCon->SendXIDCommand("_au", 3, locked_return, sizeof(locked_return));

    return locked_return[3] == '1' ? false : true;
}

unsigned int Cedrus::XIDDevice::GetPodUnlockCRC() const
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return 0;

    unsigned char crc_return[7];
    m_xidCon->SendXIDCommand("_au", 3, crc_return, sizeof(crc_return));

    unsigned int crc = AdjustEndiannessCharsToUint(
        crc_return[3],
        crc_return[4],
        crc_return[5],
        crc_return[6]);

    return crc;
}

void Cedrus::XIDDevice::LockPod(bool lock)
{
    if (!(m_config->IsMPod() || m_config->IsCPod()) || m_curMinorFwVer < 22)
        return;

    // Get the CRC to unlock the m-pod
    unsigned char crc_return[8];

    if (!lock)
        m_xidCon->SendXIDCommand("_au", 3, crc_return, sizeof(crc_return));
    else
        memset(crc_return, 0x00, 8);

    static unsigned char lock_pod_cmd[7] = { 'a','u' };
    lock_pod_cmd[2] = lock ? '0' : '1';
    lock_pod_cmd[3] = crc_return[4];
    lock_pod_cmd[4] = crc_return[5];
    lock_pod_cmd[5] = crc_return[6];
    lock_pod_cmd[6] = crc_return[7];

    DWORD bytes_written = 0;
    m_xidCon->Write(lock_pod_cmd, 7, &bytes_written);
}

unsigned char Cedrus::XIDDevice::GetMpodPulseDuration() const
{
    if (!m_config->IsMPod())
        return '0';

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_aw", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3];
}

void Cedrus::XIDDevice::SetMpodPulseDuration(unsigned char duration)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char smpd_command[3] = { 'a','w' };
    smpd_command[2] = duration;

    DWORD bytes_written = 0;
    m_xidCon->Write(smpd_command, 3, &bytes_written);

    SLEEP_FUNC(50 * SLEEP_INC);
}

char Cedrus::XIDDevice::GetPodOutputLogic() const
{
    if (!m_config->IsPod())
        return '0';

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_al", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3];
}

void Cedrus::XIDDevice::SetPodOutputLogic(char logic)
{
    if (!m_config->IsPod())
        return;

    static unsigned char spol_command[3] = { 'a','l' };
    spol_command[2] = logic;

    DWORD bytes_written = 0;
    m_xidCon->Write ( spol_command, 3, &bytes_written, SAVES_TO_FLASH );
}

int Cedrus::XIDDevice::GetMpodModel(unsigned char mpod) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    static char gmm_cmd[4] = { '_','a','q' };
    gmm_cmd[3] = '0' + mpod;

    unsigned char mpod_model_return[5];
    m_xidCon->SendXIDCommand(gmm_cmd, 4, mpod_model_return, sizeof(mpod_model_return));

    bool return_valid = strncmp((char*)mpod_model_return, "_aq", 3) == 0;

    unsigned char return_val = (unsigned char)(mpod_model_return[4]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::ConnectToMpod(unsigned char mpod, unsigned char action)
{
    if (!m_config->IsXID2())
        return;

    int rate = 4;

    if (action > 0)
    {
        m_baudRatePriorToMpod = GetBaudRate();
        // m-pods operate at 19200 only
        SetBaudRate(1);
    }
    else
    {
        switch (m_baudRatePriorToMpod)
        {
        case 9600:
            rate = 0;
            break;
        case 19200:
            rate = 1;
            break;
        case 38400:
            rate = 2;
            break;
        case 57600:
            rate = 3;
            break;
        case 115200:
            rate = 4;
            break;
        default:
            break;
        }
    }

    static unsigned char ctm_cmd[4] = {'a','q'};
    ctm_cmd[2] = '0' + mpod;
    ctm_cmd[3] = '0' + action;

    DWORD bytes_written = 0;
    m_xidCon->Write(ctm_cmd, 4, &bytes_written);

    SLEEP_FUNC(150 * SLEEP_INC);

    m_podHostConfig = (action == 0 ? nullptr : m_config);
    MatchConfigToModel_MPod(-1);

    m_curMinorFwVer = GetMinorFirmwareVersion();

    if (action == 0)
        SetBaudRate ( static_cast<unsigned char> (rate) );
}

unsigned char Cedrus::XIDDevice::GetTranslationTable() const
{
    if (!m_config->IsMPod())
        return 0;

    static char gtt_command[3] = { '_','a','s' };

    unsigned char return_info[4];
    m_xidCon->SendXIDCommand(gtt_command, 3, return_info, sizeof(return_info));

    return return_info[3] - '0';
}

void Cedrus::XIDDevice::SetTranslationTable(unsigned char table)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char stt_cmd[3] = { 'a','s' };
    stt_cmd[2] = table + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(stt_cmd, 3, &bytes_written);

    SLEEP_FUNC(50 * SLEEP_INC);
}

unsigned int Cedrus::XIDDevice::GetMappedSignals(unsigned int line)
{
    if (!m_config->IsXID2())
        return 0;

    static char gms_cmd[4] = { '_','a','t' };
    char        line_as_char = static_cast<char> (line);
    gms_cmd[3] = line_as_char > 9 ? '7' + line_as_char : '0' + line_as_char;

    unsigned char mapped_signals_return[12];
    m_xidCon->SendXIDCommand(gms_cmd, 4, mapped_signals_return, sizeof(mapped_signals_return));

    //bool return_valid = strncmp((char*)mapped_signals_return, "_at", 3) == 0;

    char mask[9];
    mask[8] = '\0';

    memcpy(mask, mapped_signals_return + 4, 8 * sizeof(unsigned char));
    std::string mapped_signals = mask;

    unsigned int signals = 0;
    if (!mapped_signals.empty())
        signals = std::stoul(mapped_signals, nullptr, 16);

    return signals;
}

void Cedrus::XIDDevice::MapSignals(unsigned int line, unsigned int map)
{
    if (!m_config->IsXID2())
        return;

    std::stringstream stream;
    stream << std::setfill('0') << std::setw(sizeof(unsigned int) * 2)
        << std::hex << map << std::endl;
    std::string string_mask(stream.str());
    for (auto & c : string_mask) c = toupper(c);

    static unsigned char map_signals_cmd[11] = { 'a','t' };
    unsigned char        line_as_char = static_cast<unsigned char> (line);

    map_signals_cmd[2] = line_as_char > 9 ? '7' + line_as_char : '0' + line_as_char;
    map_signals_cmd[3] = string_mask[0];
    map_signals_cmd[4] = string_mask[1];
    map_signals_cmd[5] = string_mask[2];
    map_signals_cmd[6] = string_mask[3];
    map_signals_cmd[7] = string_mask[4];
    map_signals_cmd[8] = string_mask[5];
    map_signals_cmd[9] = string_mask[6];
    map_signals_cmd[10] = string_mask[7];

    DWORD bytes_written = 0;
    m_xidCon->Write(map_signals_cmd, 11, &bytes_written);
}

void Cedrus::XIDDevice::ResetMappedLinesToDefault()
{
    if (!m_config->IsXID2())
        return;

    static unsigned char rmltd_cmd[3] = { 'a','t','X' };

    DWORD bytes_written = 0;
    m_xidCon->Write(rmltd_cmd, 3, &bytes_written);

    if (m_config->IsMPod())
    {
        if (m_config->GetModelID() == 72) // Neuroscan 16-bit
        {
            SetMPodLineMapping_Neuroscan16bit();
        }
        else if (m_config->GetModelID() == 104) // Neuroscan Grael
        {
            SetMPodLineMapping_NeuroscanGrael();
        }
    }
    else if (m_config->IsCPod() && m_config->GetModelID() == 104)
    {
        SetCPodLineMapping_NeuroscanGrael(); // Neuroscan Grael
    }
}

void Cedrus::XIDDevice::CommitLineMappingToFlash()
{
    if (!m_config->IsXID2())
        return;

    static unsigned char commit_map_cmd[2] = { 'a','f' };

    DWORD bytes_written = 0;
    m_xidCon->Write ( commit_map_cmd, 2, &bytes_written, SAVES_TO_FLASH );

    SLEEP_FUNC(50 * SLEEP_INC);
}

int Cedrus::XIDDevice::GetVKDropDelay() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char vk_drop_delay[4];

    m_xidCon->SendXIDCommand("_b3", 4, vk_drop_delay, sizeof(vk_drop_delay));

    return vk_drop_delay[3];
}

void Cedrus::XIDDevice::SetVKDropDelay(unsigned char delay)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char svkdd_cmd[3] = { 'b','3' };
    svkdd_cmd[2] = delay;

    DWORD bytes_written = 0;
    m_xidCon->Write(svkdd_cmd, 3, &bytes_written);
}

std::string Cedrus::XIDDevice::GetProtocol() const
{
    return GetProtocol_Scan(m_xidCon);
}

/*static*/ std::string Cedrus::XIDDevice::GetProtocol_Scan(std::shared_ptr<Connection> xidCon)
{
    // There's a possibility that the device is in E-Prime mode. Right
    // now is the only time the library cares about it, and we need to
    // do some PST-proofing. To start, flush everything to remove the
    // potential spew of zeroes.
    unsigned char return_info[5]; // we rely on SendXIDCommand to zero-initialize this buffer

    int bytes_count = xidCon->SendXIDCommand_PST_Proof("_c1", 3, return_info, sizeof(return_info));

    CEDRUS_ASSERT(bytes_count >= 1 || (return_info[0] == 0 && return_info[4] == 0),
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer");

    bool return_valid = strncmp((char*)return_info, "_xid", 4) == 0;

    // It's okay for this to sometimes return nothing at all. That just
    // means we queried an incorrect baud and there's nothing wrong with that.
    CEDRUS_ASSERT(return_valid || return_info[0] == 0, "GetProtocol's return value must start with _xid");

    return return_valid ? std::string((char*)return_info, sizeof(return_info)) : std::string("Invalid Protocol");
}

void Cedrus::XIDDevice::SetProtocol(unsigned char protocol)
{
    return SetProtocol_Scan(m_xidCon, protocol);
}

void Cedrus::XIDDevice::SetProtocol_Scan(std::shared_ptr<Connection> xidCon, unsigned char protocol)
{
    static unsigned char sdp_cmd[3] = { 'c', '1' };
    sdp_cmd[2] = protocol + '0';

    DWORD bytes_written = 0;
    xidCon->Write(sdp_cmd, 3, &bytes_written);
}

unsigned int Cedrus::XIDDevice::GetKBModeProtocol() const
{
    if (!(m_config->IsRBx40() || m_config->IsLumina3G()))
        return static_cast<unsigned int> (INVALID_RETURN_VALUE);

    unsigned char return_info[4];
    m_xidCon->SendXIDCommand("_c2", 3, return_info, sizeof(return_info));


    return return_info[3] - '0';
}

void Cedrus::XIDDevice::SetKBModeProtocol(unsigned char mode)
{
    if (!(m_config->IsRBx40() || m_config->IsLumina3G()))
        return;

    static unsigned char sacm_cmd[3] = { 'c','2' };
    sacm_cmd[2] = mode + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(sacm_cmd, 3, &bytes_written);
}

void Cedrus::XIDDevice::SwitchToKeyboardMode()
{
    if (!(m_config->IsLumina3G() || m_config->IsRBx40()))
        return;

    static unsigned char stkm_cmd[2] = { 'c', '3' };

    DWORD bytes_written = 0;
    m_xidCon->Write(stkm_cmd, 2, &bytes_written);
}

unsigned char Cedrus::XIDDevice::GetCPodInputMode() const
{
    if (!m_config->IsCPodWithInput())
        return '0';

    unsigned char return_info[4];
    m_xidCon->SendXIDCommand("_c4", 3, return_info, sizeof(return_info));


    return return_info[3];
}

void Cedrus::XIDDevice::SetCPodInputMode(unsigned char mode)
{
    if (!m_config->IsCPodWithInput())
        return;

    static unsigned char scpim_cmd[3] = { 'c','4' };
    scpim_cmd[2] = mode; // 'p' for polled, 't' for timestamped

    DWORD bytes_written = 0;
    m_xidCon->Write(scpim_cmd, 3, &bytes_written);
}

std::string Cedrus::XIDDevice::GetCombinedInfo() const
{
    m_xidCon->SetReadTimeout(100);

    unsigned char return_info[1000];
    m_xidCon->SendXIDCommand("_d0", 3, return_info, sizeof(return_info));

    m_xidCon->SetReadTimeout(50);

    std::string return_name((char*)return_info);

    std::replace(return_name.begin(), return_name.end(), '\r', '\n');

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

std::string Cedrus::XIDDevice::GetInternalProductName() const
{
    m_xidCon->SetReadTimeout(100);

    unsigned char return_info[100];
    m_xidCon->SendXIDCommand("_d1", 3, return_info, sizeof(return_info));

    m_xidCon->SetReadTimeout(50);

    std::string return_name((char*)return_info);

    std::replace(return_name.begin(), return_name.end(), '\r', '\n');

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

int Cedrus::XIDDevice::GetProductID() const
{
    return XIDDevice::GetProductID_Scan(m_xidCon);
}

int Cedrus::XIDDevice::GetModelID() const
{
    return XIDDevice::GetModelID_Scan(m_xidCon);
}

/*static*/ int Cedrus::XIDDevice::GetProductID_Scan(std::shared_ptr<Connection> xidCon)
{
    unsigned char product_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand("_d2", 3, product_id_return, sizeof(product_id_return));

    int product_id = (int)(product_id_return[0]);

    return product_id;
}

/*static*/ int Cedrus::XIDDevice::GetModelID_Scan(std::shared_ptr<Connection> xidCon)
{
    unsigned char model_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand("_d3", 3, model_id_return, sizeof(model_id_return));

    int model_id = (int)(model_id_return[0]);

    return model_id;
}

void Cedrus::XIDDevice::SetModelID(unsigned char model)
{
    static char set_model_cmd[3] = { 'd', '3' };
    set_model_cmd[2] = model;

    DWORD bytes_written = 0;
    m_xidCon->Write ( (unsigned char*)set_model_cmd, 3, &bytes_written, SAVES_TO_FLASH );

    SLEEP_FUNC(250 * SLEEP_INC);

    if (!m_config->IsMPod())
        MatchConfigToModel(model);
    else
        MatchConfigToModel_MPod(model);

    if (m_config->IsMPod())
    {
        if (model == 72) // Neuroscan 16-bit
        {
            SetMPodLineMapping_Neuroscan16bit();
        }
        else if (model == 104) // Neuroscan Grael
        {
            SetMPodLineMapping_NeuroscanGrael();
        }
        else
        {
            ResetMappedLinesToDefault();
            CommitLineMappingToFlash();
            SetMpodOutputMode(0);
        }
    }
    else if (m_config->IsCPod())
    {
        if (model == 104)
            SetCPodLineMapping_NeuroscanGrael(); // Neuroscan Grael
        else
        {
            ResetMappedLinesToDefault();
            CommitLineMappingToFlash();
        }
    }

    if ( m_config->IsPod()  && ( model == 79 || model == 111 ) )    // Magstim EGI
    {
        SetPodOutputLogic('n');
    }
}

int Cedrus::XIDDevice::GetMajorFirmwareVersion() const
{
    return GetMajorFirmwareVersion_Scan(m_xidCon);
}

/*static*/ int Cedrus::XIDDevice::GetMajorFirmwareVersion_Scan(std::shared_ptr<Connection> xidCon)
{
    unsigned char major_return[1];

    xidCon->SendXIDCommand("_d4", 3, major_return, sizeof(major_return));

    bool return_valid = (major_return[0] >= 48 && major_return[0] <= 50);

    return return_valid ? major_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetMinorFirmwareVersion() const
{
    unsigned char minor_return[1];

    m_xidCon->SendXIDCommand("_d5", 3, minor_return, sizeof(minor_return));

    bool return_valid = minor_return[0] >= 48;

    return return_valid ? minor_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetOutpostModel() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char outpost_return[1];

    m_xidCon->SendXIDCommand("_d6", 3, outpost_return, sizeof(outpost_return));

    bool return_valid = (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x';

    return return_valid ? (int)(outpost_return[0]) : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetHardwareGeneration() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char gen_return[1];

    m_xidCon->SendXIDCommand("_d7", 3, gen_return, sizeof(gen_return));

    return gen_return[0] - '0';
}

void Cedrus::XIDDevice::ResetBaseTimer()
{
    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"e1", 2, &bytes_written);
}

unsigned int Cedrus::XIDDevice::QueryBaseTimer()
{
    if (!m_config->IsXID1())
        return 0;

    unsigned int base_timer = 0;
    unsigned char return_info[6];
    int read = m_xidCon->SendXIDCommand("e3", 2, return_info, sizeof(return_info));

    CEDRUS_ASSERT(strncmp((char*)return_info, "e3", 2) == 0, "QueryBaseTimer xid query result must start with e3");
    bool valid_response = (read == 6);

    if (valid_response)
    {
        base_timer = AdjustEndiannessCharsToUint
        (return_info[2],
            return_info[3],
            return_info[4],
            return_info[5]);
    }

    return base_timer;
}

unsigned int Cedrus::XIDDevice::QueryRtTimer()
{
    if (!m_config->IsXID2())
        return 0;

    static char qrt_command[3] = { '_', 'e','5' };

    unsigned char return_info[7];
    m_xidCon->SendXIDCommand(qrt_command, 3, return_info, sizeof(return_info));

    unsigned int timer = AdjustEndiannessCharsToUint(
        return_info[3],
        return_info[4],
        return_info[5],
        return_info[6]);

    return timer;
}

void Cedrus::XIDDevice::ResetRtTimer()
{
    if (m_config->IsStimTracker1())
        return;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"e5", 2, &bytes_written);
}

void Cedrus::XIDDevice::SetBaudRate(unsigned char rate)
{
    if (m_config->IsMPod())
        return;

    static unsigned char change_baud_cmd[3] = { 'f', '1' };
    change_baud_cmd[2] = rate;

    DWORD bytes_written = 0;
    m_xidCon->Write(change_baud_cmd, 3, &bytes_written);

    m_xidCon->SetBaudRate(rate);
    CloseConnection();
    OpenConnection();
}

void Cedrus::XIDDevice::GetLockingLevel()
{
    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"_f2", 3, &bytes_written);
}

void Cedrus::XIDDevice::SetLockingLevel(unsigned char level)
{
    if (!m_config->IsXID1())
        return;

    static unsigned char set_level_cmd[3] = { 'f', '2' };
    set_level_cmd[2] = level + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(set_level_cmd, 3, &bytes_written);
}

void Cedrus::XIDDevice::ReprogramFlash()
{
    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"f3", 2, &bytes_written);
}

bool Cedrus::XIDDevice::GetTriggerDefault() const
{
    if (!m_config->IsLumina())
        return false;

    unsigned char default_return[4];

    m_xidCon->SendXIDCommand("_f4", 3, default_return, sizeof(default_return));

    return default_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::SetTriggerDefault(bool defaultOn)
{
    if (!m_config->IsLumina())
        return;

    static unsigned char set_trigger_default_cmd[3] = { 'f', '4' };
    set_trigger_default_cmd[2] = (unsigned char)defaultOn + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(set_trigger_default_cmd, 3, &bytes_written);
}

int Cedrus::XIDDevice::GetTriggerDebounceTime() const
{
    if (!m_config->IsLumina())
        return 0;

    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand("_f5", 3, threshold_return, sizeof(threshold_return));

    bool return_valid = strncmp((char*)threshold_return, "_f5", 3) == 0;

    CEDRUS_ASSERT(return_valid, "GetTriggerDebounceTime's xid query result must start with _f5");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetTriggerDebounceTime(unsigned char time)
{
    if (!m_config->IsLumina())
        return;

    static unsigned char set_debouncing_time_cmd[3] = { 'f', '5' };
    set_debouncing_time_cmd[2] = time;

    DWORD bytes_written = 0;
    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written);
}

int Cedrus::XIDDevice::GetButtonDebounceTime() const
{
    if (!m_config->IsLumina() && !m_config->IsRB())
        return INVALID_RETURN_VALUE;

    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand("_f6", 3, threshold_return, sizeof(threshold_return));

    bool return_valid = strncmp((char*)threshold_return, "_f6", 3) == 0;

    CEDRUS_ASSERT(return_valid, "GetButtonDebounceTime's xid query result must start with _f6");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetButtonDebounceTime(unsigned char time)
{
    if (!m_config->IsLumina() && !m_config->IsRB())
        return;

    static unsigned char set_debouncing_time_cmd[3] = { 'f', '6' };
    set_debouncing_time_cmd[2] = time;

    DWORD bytes_written = 0;
    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written);
}

void Cedrus::XIDDevice::RestoreFactoryDefaults()
{
    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"f7", 2, &bytes_written);

    SLEEP_FUNC(100 * SLEEP_INC);

    if (m_config->IsMPod())
    {
        SetTranslationTable(m_podHostConfig->IsStimTracker2() ? 1 : 0);
        if (m_config->GetModelID() == 72) // Neuroscan 16-bit
        {
            SetMPodLineMapping_Neuroscan16bit();
        }
        else if (m_config->GetModelID() == 104) // Neuroscan Grael
        {
            SetMPodLineMapping_NeuroscanGrael();
        }
    }
    else if (m_config->IsCPod())
    {
        if (m_config->GetModelID()== 104)
            SetCPodLineMapping_NeuroscanGrael(); // Neuroscan Grael
    }
}

void Cedrus::XIDDevice::SaveSettingsToFlash()
{
    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"f9", 2, &bytes_written);
    SLEEP_FUNC(50 * SLEEP_INC);
}

unsigned int Cedrus::XIDDevice::GetTimeSinceLastOscillatorTest ()
{
    if ( !m_config->IsCPod () )
        return 0;

    static char time_since_osc_cmd[3] = { '_', 'f','a' };

    unsigned char return_info[7];
    m_xidCon->SendXIDCommand ( time_since_osc_cmd, 3, return_info, sizeof ( return_info ) );

    unsigned int timer = AdjustEndiannessCharsToUint (
        return_info[3],
        return_info[4],
        return_info[5],
        return_info[6] );

    return timer;
}

void Cedrus::XIDDevice::StartOscillatorTest (bool start)
{
    if ( !m_config->IsCPod () )
        return;

    static unsigned char start_osc_test_cmd[3] = { 'f','a' };
    start_osc_test_cmd[2] = start ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write ( start_osc_test_cmd, 3, &bytes_written );
}

void Cedrus::XIDDevice::SetAdjustmentValue ( char adj )
{
    if ( !m_config->IsCPod () )
        return;

    static unsigned char set_adj_val_cmd[3] = { 'f','b' };
    set_adj_val_cmd[2] = adj;

    DWORD bytes_written = 0;
    m_xidCon->Write ( set_adj_val_cmd, 3, &bytes_written, SAVES_TO_FLASH );
}

void Cedrus::XIDDevice::SetAdjustmentFlag (bool testConducted)
{
    if (!m_config->IsCPod ())
        return;

    static unsigned char set_adj_flag_cmd[3] = { 'f','c' };
    set_adj_flag_cmd[2] = testConducted ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write ( set_adj_flag_cmd, 3, &bytes_written, SAVES_TO_FLASH );
}

bool Cedrus::XIDDevice::IsOpticalIsolationSwitchOn() const
{
    if (!m_config->IsLumina3G())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_fo", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

Cedrus::SingleShotMode Cedrus::XIDDevice::GetSingleShotMode(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return SingleShotMode();

    static char gssm_command[4] = { '_','i','a' };

    gssm_command[3] = selector;

    unsigned char return_info[9];
    m_xidCon->SendXIDCommand(gssm_command, 4, return_info, sizeof(return_info));

    SingleShotMode ss_mode;
    ss_mode.enabled = return_info[4] == '1';

    ss_mode.delay = AdjustEndiannessCharsToUint(
        return_info[5],
        return_info[6],
        return_info[7],
        return_info[8]);

    return ss_mode;
}

void Cedrus::XIDDevice::SetSingleShotMode(unsigned char selector, bool enable, unsigned int delay)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char sssm_cmd[8] = { 'i','a' };
    sssm_cmd[2] = selector;
    sssm_cmd[3] = enable? '1' : '0';

    AdjustEndiannessUintToChars(
        delay,
        &(sssm_cmd[4]),
        &(sssm_cmd[5]),
        &(sssm_cmd[6]),
        &(sssm_cmd[7]));

    DWORD bytes_written = 0;
    m_xidCon->Write(sssm_cmd, 8, &bytes_written);
}

unsigned char Cedrus::XIDDevice::GetCPodInputLines() const
{
    if (!m_config->IsCPodWithInput())
        return '0';

    unsigned char return_info[4];
    m_xidCon->SendXIDCommand("_ic0", 4, return_info, sizeof(return_info));

    return return_info[3];
}

Cedrus::SignalFilter Cedrus::XIDDevice::GetSignalFilter(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return SignalFilter();

    static char gsf_command[4] = { '_','i','f' };

    gsf_command[3] = selector;

    unsigned char return_info[12];
    m_xidCon->SendXIDCommand(gsf_command, 4, return_info, sizeof(return_info));

    SignalFilter filter;
    filter.holdOn = AdjustEndiannessCharsToUint(
        return_info[4],
        return_info[5],
        return_info[6],
        return_info[7]);

    filter.holdOff = AdjustEndiannessCharsToUint(
        return_info[8],
        return_info[9],
        return_info[10],
        return_info[11]);

    return filter;
}

void Cedrus::XIDDevice::SetSignalFilter(unsigned char selector, unsigned int holdOn, unsigned int holdOff)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char ssf_cmd[11] = { 'i','f' };
    ssf_cmd[2] = selector;

    AdjustEndiannessUintToChars(
        holdOn,
        &(ssf_cmd[3]),
        &(ssf_cmd[4]),
        &(ssf_cmd[5]),
        &(ssf_cmd[6]));

    AdjustEndiannessUintToChars(
        holdOff,
        &(ssf_cmd[7]),
        &(ssf_cmd[8]),
        &(ssf_cmd[9]),
        &(ssf_cmd[10]));

    DWORD bytes_written = 0;
    m_xidCon->Write(ssf_cmd, 11, &bytes_written);
}

bool Cedrus::XIDDevice::IsKbAutorepeatOn() const
{
    if (!(m_config->IsRBx40() || m_config->IsLumina3G()) || m_curMinorFwVer < 21)
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_ig", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::EnableKbAutorepeat(bool pause)
{
    if (!(m_config->IsRBx40() || m_config->IsLumina3G()) || m_curMinorFwVer < 21)
        return;

    static unsigned char enable_kb_autorepeat_cmd[3] = { 'i','g' };
    enable_kb_autorepeat_cmd[2] = pause ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write ( enable_kb_autorepeat_cmd, 3, &bytes_written, SAVES_TO_FLASH );
}

bool Cedrus::XIDDevice::IsRBx40LEDEnabled() const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_il", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::EnableRBx40LED(bool enable)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char enable_rb_led_cmd[3] = { 'i','l' };
    enable_rb_led_cmd[2] = enable ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write ( enable_rb_led_cmd, 3, &bytes_written );

    SLEEP_FUNC(50 * SLEEP_INC);
}


unsigned int Cedrus::XIDDevice::GetRipondaLEDFunction() const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_il", 3, cmd_return, sizeof(cmd_return));

    return static_cast<unsigned int> ( cmd_return[3] );
}


void Cedrus::XIDDevice::SetRipondaLEDFunction ( unsigned int nFunction )
{
    CEDRUS_ASSERT ( nFunction >= LED_OFF && nFunction <= LED_FOR_VOICE_KEY, "Invalid Riponda LED parameter!" );

    if (!m_config->IsXID2())
        return;
    
    static unsigned char enable_rb_led_cmd[3] = { 'i','l' };
    enable_rb_led_cmd[2] = static_cast<unsigned char> ( nFunction );

    DWORD bytes_written = 0;
    m_xidCon->Write ( enable_rb_led_cmd, 3, &bytes_written, SAVES_TO_FLASH );

    SLEEP_FUNC(50 * SLEEP_INC);
}


bool Cedrus::XIDDevice::GetEnableDigitalOutput(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char return_info[5];

    static char gtso_command[4] = { '_','i','o' };
    gtso_command[3] = selector;

    m_xidCon->SendXIDCommand(gtso_command, 4, return_info, sizeof(return_info));

    return return_info[4] == '1' ? true : false;
}

void Cedrus::XIDDevice::SetEnableDigitalOutput(unsigned char selector, bool mode)
{
    if (!m_config->IsStimTracker2())
        return;

    static unsigned char stso_command[4] = { 'i','o' };
    stso_command[2] = selector;
    stso_command[3] = mode ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(stso_command, 4, &bytes_written);
}

bool Cedrus::XIDDevice::IsOutputPaused() const
{
    if (!m_config->IsXID2InputDevice() || m_curMinorFwVer < 21)
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_ip", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::PauseAllOutput(bool pause)
{
    if (!m_config->IsXID2InputDevice() || m_curMinorFwVer < 21)
        return;

    static unsigned char pause_output_cmd[3] = { 'i','p' };
    pause_output_cmd[2] = !pause ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(pause_output_cmd, 3, &bytes_written);
}

int Cedrus::XIDDevice::GetTimerResetOnOnsetMode(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    static char gtrom_command[4] = {'_','i','r'};
    gtrom_command[3] = selector;

    unsigned char return_info[5];
    m_xidCon->SendXIDCommand(gtrom_command, 4, return_info, sizeof(return_info));

    return strncmp((char*)return_info, "_ir", 3) == 0 ? return_info[4] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetTimerResetOnOnsetMode(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_mode_cmd[4] = { 'i','r' };
    change_mode_cmd[2] = selector;
    change_mode_cmd[3] = mode + '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(change_mode_cmd, 4, &bytes_written);
}

bool Cedrus::XIDDevice::GetEnableUSBOutput(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return false;

    // c-pod don't respond to this command, which is generally fine, as they
    // don't produce usb input. However, it's different for MindWare and
    // Universal c-pods, which have their own sort-of equivalent of this
    // command in '_c4'. This returns true rather than false for the benefit
    // of existing software, to indicate that if input is disabled it's not
    // due to this command. There isn't a particularly strong case either way.
    if (m_config->IsCPodWithInput())
        return true;

    unsigned char return_info[5];

    static char geuo_command[4] = { '_','i','u' };
    geuo_command[3] = selector;

    m_xidCon->SendXIDCommand(geuo_command, 4, return_info, sizeof(return_info));

    return return_info[4] == '1' ? true : false;
}

void Cedrus::XIDDevice::SetEnableUSBOutput(unsigned char selector, bool mode)
{
    if (!m_config->IsStimTracker2())
        return;

    static unsigned char seuo_command[4] = { 'i','u' };
    seuo_command[2] = selector;
    seuo_command[3] = mode ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(seuo_command, 4, &bytes_written);
}

int Cedrus::XIDDevice::GetAnalogInputThreshold(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char cmd_return[5];
    static char gait_command[4] = { '_','i','t' };
    gait_command[3] = selector;

    m_xidCon->SendXIDCommand(gait_command, 4, cmd_return, sizeof(cmd_return));

    return strncmp((char*)cmd_return, "_it", 3) == 0 ? (int)(cmd_return[4]) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetAnalogInputThreshold(unsigned char selector, unsigned char threshold)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_threshold_cmd[4] = { 'i','t' };
    change_threshold_cmd[2] = selector;
    change_threshold_cmd[3] = threshold;

    DWORD bytes_written = 0;
    m_xidCon->Write(change_threshold_cmd, 4, &bytes_written);
}

int Cedrus::XIDDevice::GetMixedInputMode() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_iv", 3, cmd_return, sizeof(cmd_return));

    return strncmp((char*)cmd_return, "_iv", 3) == 0 ? (int)(cmd_return[3]) - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetMixedInputMode(unsigned char mode)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_threshold_cmd[3] = { 'i','v' };
    change_threshold_cmd[2] = mode ? '1' : '0';

    DWORD bytes_written = 0;
    m_xidCon->Write(change_threshold_cmd, 3, &bytes_written);
}

unsigned int Cedrus::XIDDevice::GetRaisedLines() const
{
    unsigned char return_info[5];

    if (m_config->IsXID2() || m_config->IsStimTracker1())
        m_xidCon->SendXIDCommand("_mh", 3, return_info, sizeof(return_info));
    else
        m_xidCon->SendXIDCommand("_ah", 3, return_info, sizeof(return_info));

    unsigned int mask = AdjustEndiannessCharsToUint(0, 0,
        return_info[3],
        return_info[4]);

    return mask;
}

unsigned int Cedrus::XIDDevice::GetNumberOfLines() const
{
    if (!m_config->IsXID2())
        return 0;

    unsigned char gen_return[4];

    m_xidCon->SendXIDCommand("_ml", 3, gen_return, sizeof(gen_return));

    return gen_return[3];
}

void Cedrus::XIDDevice::SetNumberOfLines(unsigned int lines)
{
    static unsigned char set_number_of_lines_cmd[3] = { 'm','l' };
    set_number_of_lines_cmd[2] = static_cast<unsigned char> (lines);

    DWORD bytes_written = 0;
    m_xidCon->Write ( (unsigned char*)set_number_of_lines_cmd, 3, &bytes_written, SAVES_TO_FLASH );
}

unsigned int Cedrus::XIDDevice::GetPulseDuration() const
{
    if (m_config->IsXID1InputDevice() || m_config->IsMPod())
        return 0;

    unsigned char return_info[7];
    m_xidCon->SendXIDCommand("_mp", 3, return_info, sizeof(return_info));

    CEDRUS_ASSERT(strncmp((char*)return_info, "_mp", 3) == 0, "GetPulseDuration's return value must start with _mp");

    unsigned int dur = AdjustEndiannessCharsToUint(
        return_info[3],
        return_info[4],
        return_info[5],
        return_info[6]);

    return dur;
}

void Cedrus::XIDDevice::SetPulseDuration(unsigned int duration)
{
    if ((m_config->IsXID1InputDevice() || m_config->IsMPod()))
        return;

    static unsigned char spd_command[6] = { 'm','p' };

    AdjustEndiannessUintToChars(
        duration,
        &(spd_command[2]),
        &(spd_command[3]),
        &(spd_command[4]),
        &(spd_command[5]));

    DWORD written = 0;
    m_xidCon->Write ( spd_command, 6, &written, SAVES_TO_FLASH );
}

unsigned int Cedrus::XIDDevice::GetPulseTableBitMask()
{
    if (!m_config->IsXID2())
        return 0;

    unsigned char return_info[5];
    m_xidCon->SendXIDCommand("_mk", 5, return_info, sizeof(return_info));

    unsigned int mask = AdjustEndiannessCharsToUint(0,0,
        return_info[3],
        return_info[4]);

    return mask;
}

void Cedrus::XIDDevice::SetPulseTableBitMask(unsigned int lines)
{
    if (!m_config->IsXID2())
        return;

    unsigned int mask = lines;

    static char set_lines_cmd[4] = { 'm','k' };
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written);
}

void Cedrus::XIDDevice::ClearPulseTable()
{
    if (!m_config->IsXID2())
        return;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"mc", 2, &bytes_written);
}

bool Cedrus::XIDDevice::IsPulseTableRunning() const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_mr", 3, cmd_return, sizeof(cmd_return));

    return cmd_return[3] == '1' ? true : false;
}

void Cedrus::XIDDevice::RunPulseTable()
{
    if (!m_config->IsXID2())
        return;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"mr", 2, &bytes_written);
}

void Cedrus::XIDDevice::StopPulseTable()
{
    if (!m_config->IsXID2())
        return;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"ms", 2, &bytes_written);
}

void Cedrus::XIDDevice::AddPulseTableEntry(unsigned int time, unsigned int lines)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char apte_cmd[8] = { 'm','t' };

    AdjustEndiannessUintToChars(
        time,
        &(apte_cmd[2]),
        &(apte_cmd[3]),
        &(apte_cmd[4]),
        &(apte_cmd[5]));

    AdjustEndiannessUintToChars(
        lines,
        &(apte_cmd[6]),
        &(apte_cmd[7]));

    DWORD bytes_written = 0;
    m_xidCon->Write(apte_cmd, 8, &bytes_written);
}

void Cedrus::XIDDevice::ResetOutputLines()
{
    if (!m_config->IsXID2())
        return;

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)"mz", 2, &bytes_written);
}


void Cedrus::XIDDevice::SetVoltageRange ( unsigned int /* nMinimum */, unsigned int nMaximum )
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return;

    static unsigned char set_voltage_range_cmd[4] = { 'v','r' };
    set_voltage_range_cmd[2] = 0;   // Minimum voltage, always 0V for now
    set_voltage_range_cmd[3] = static_cast<unsigned char> (nMaximum);

    DWORD bytes_written = 0;
    m_xidCon->Write ( (unsigned char*)set_voltage_range_cmd, sizeof(set_voltage_range_cmd), &bytes_written, SAVES_TO_FLASH );
}


unsigned int Cedrus::XIDDevice::GetMaxVoltageRange() const
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return 0;

    unsigned char gen_return[5];

    m_xidCon->SendXIDCommand("_vr", 3, gen_return, sizeof(gen_return));

    return gen_return[4];
}


void Cedrus::XIDDevice::SetVoltageRangeForTesting ( unsigned int /* nMinimum */, unsigned int nMaximum )
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return;

    static unsigned char set_voltage_range_cmd[4] = { 'v','t' };
    set_voltage_range_cmd[2] = 0;   // Minimum voltage, always 0V for now
    set_voltage_range_cmd[3] = static_cast<unsigned char> (nMaximum);

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)set_voltage_range_cmd, 4, &bytes_written);
}


unsigned int Cedrus::XIDDevice::GetMaxVoltageRangeForTesting() const
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return 0;

    unsigned char gen_return[5];

    m_xidCon->SendXIDCommand("_vt", 3, gen_return, sizeof(gen_return));

    return gen_return[4];
}


void Cedrus::XIDDevice::SetAnalogOutputMode ( unsigned int mode )
{
    CEDRUS_ASSERT ( mode == AM_FIXED_DELTA  ||  mode == AM_BINARY, "Invalid parameter sent to SetAnalogOutputMode() !" );

    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return;

    static unsigned char set_voltage_range_cmd[3] = { 'v','m' };
    set_voltage_range_cmd[2] = static_cast<unsigned char> (mode);

    DWORD bytes_written = 0;
    m_xidCon->Write ( (unsigned char*)set_voltage_range_cmd, sizeof(set_voltage_range_cmd), &bytes_written, SAVES_TO_FLASH );
}


unsigned int Cedrus::XIDDevice::GetAnalogOutputMode() const
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return 0;

    unsigned char gen_return[4];

    m_xidCon->SendXIDCommand("_vm", 3, gen_return, sizeof(gen_return));

    return gen_return[3];
}


void Cedrus::XIDDevice::SetNumberOfAnalogOutputLevels ( unsigned int numLevels )
{
    CEDRUS_ASSERT ( numLevels == 8  ||  numLevels == 16, "Number of analog output levels can be ONLY 8 or 16." );

    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return;

    static unsigned char set_voltage_range_cmd[3] = { 'v','l' };
    set_voltage_range_cmd[2] = static_cast<unsigned char> (numLevels);

    DWORD bytes_written = 0;
    m_xidCon->Write ( (unsigned char*)set_voltage_range_cmd, sizeof(set_voltage_range_cmd), &bytes_written, SAVES_TO_FLASH );
}


unsigned int Cedrus::XIDDevice::GetNumberOfAnalogOutputLevels() const
{
    if ( !m_config->IsXID2()  ||  GetModelID() != 'V' )
        return 0;

    unsigned char gen_return[4];

    m_xidCon->SendXIDCommand("_vl", 3, gen_return, sizeof(gen_return));

    return gen_return[3];
}


void Cedrus::XIDDevice::RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines)
{
    unsigned int output_lines = linesBitmask;

    if (leaveRemainingLines)
        output_lines |= m_linesState;

    if (m_config->IsXID1InputDevice())
    {
        SetDigitalOutputLines_RB(m_xidCon, output_lines);
    }
    else
    {
        SetDigitalOutputLines_ST(m_xidCon, output_lines);
    }

    m_linesState = output_lines;
}

void Cedrus::XIDDevice::LowerLines(unsigned int linesBitmask, bool leaveRemainingLines)
{
    unsigned int output_lines = ~linesBitmask;

    if (leaveRemainingLines)
        output_lines &= m_linesState;

    if (((m_config->IsRB() || m_config->IsLumina()) && m_config->IsXID1()) || m_config->IsSV1())
    {
        SetDigitalOutputLines_RB(m_xidCon, output_lines);
    }
    else
    {
        SetDigitalOutputLines_ST(m_xidCon, output_lines);
    }

    m_linesState = output_lines;
}

void Cedrus::XIDDevice::ClearLines()
{
    if (((m_config->IsRB() || m_config->IsLumina()) && m_config->IsXID1()) || m_config->IsSV1())
    {
        SetDigitalOutputLines_RB(m_xidCon, 0);
    }
    else
    {
        SetDigitalOutputLines_ST(m_xidCon, 0);
    }

    m_linesState = 0;
}

void Cedrus::XIDDevice::SendPulse(unsigned int duration, unsigned int lines, unsigned int pulses, unsigned int ipi)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char send_pulse_cmd[9] = { 'm','x' };

    AdjustEndiannessUintToChars(
        duration,
        &(send_pulse_cmd[2]),
        &(send_pulse_cmd[3]));

    AdjustEndiannessUintToChars(
        lines,
        &(send_pulse_cmd[4]),
        &(send_pulse_cmd[5]));

    send_pulse_cmd[6] = pulses & 0x000000FF;

    AdjustEndiannessUintToChars(
        ipi,
        &(send_pulse_cmd[7]),
        &(send_pulse_cmd[8]));

    DWORD bytes_written = 0;
    m_xidCon->Write((unsigned char*)send_pulse_cmd, 9, &bytes_written);
}

bool Cedrus::XIDDevice::ArePulsesBeingSent() const
{
    if (!m_config->IsXID2())
        return 0;

    unsigned char get_pulse_return[4];

    m_xidCon->SendXIDCommand("_mx", 3, get_pulse_return, sizeof(get_pulse_return));

    return get_pulse_return[3] == '1' ? true : false;
}

int Cedrus::XIDDevice::GetBaudRate() const
{
    return m_xidCon->GetBaudRate();
}

std::shared_ptr<const Cedrus::DeviceConfig> Cedrus::XIDDevice::GetDeviceConfig() const
{
    return m_config;
}

int Cedrus::XIDDevice::OpenConnection() const
{
    return m_xidCon->Open();
}

int Cedrus::XIDDevice::CloseConnection() const
{
    return m_xidCon->Close();
}

bool Cedrus::XIDDevice::HasLostConnection() const
{
    return m_xidCon->HasLostConnection();
}

void Cedrus::XIDDevice::PollForResponse() const
{
    if (m_ResponseMgr)
        m_ResponseMgr->CheckForKeypress(m_xidCon);
}

bool Cedrus::XIDDevice::HasQueuedResponses() const
{
    if (m_ResponseMgr)
        return m_ResponseMgr->HasQueuedResponses();
    else
        return false;
}

unsigned int Cedrus::XIDDevice::GetNumberOfKeysDown() const
{
    if (m_ResponseMgr)
        return m_ResponseMgr->GetNumberOfKeysDown();
    else
        return 0;
}

Cedrus::Response Cedrus::XIDDevice::GetNextResponse() const
{
    if (m_ResponseMgr)
        return m_ResponseMgr->GetNextResponse();
    else
        return Response();
}

void Cedrus::XIDDevice::ClearResponseQueue()
{
    if (m_ResponseMgr)
        m_ResponseMgr->ClearResponseQueue();
}

void Cedrus::XIDDevice::ClearResponsesFromBuffer()
{
    m_xidCon->FlushReadFromDeviceBuffer();
}

void Cedrus::XIDDevice::SetDigitalOutputLines_RB(std::shared_ptr<Connection> xidCon, unsigned int lines)
{
    static char set_lines_cmd[3] = { 'a','h' };
    set_lines_cmd[2] = lines & 0x000000ff;

    DWORD bytes_written = 0;
    xidCon->Write((unsigned char*)set_lines_cmd, 3, &bytes_written);
}

void Cedrus::XIDDevice::SetDigitalOutputLines_ST(std::shared_ptr<Connection> xidCon, unsigned int lines)
{
    unsigned int mask = lines;

    static char set_lines_cmd[4] = { 'm','h' };
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written = 0;
    xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written);
}

void Cedrus::XIDDevice::MatchConfigToModel(char model)
{
    m_config = XIDDeviceScanner::GetDeviceScanner().GetConfigForGivenDevice(GetProductID(), model != -1 ? model : GetModelID(), m_config->GetMajorVersion());
    if (model != -1)
        m_ResponseMgr.reset(m_config->IsInputDevice() ? new ResponseManager(m_config) : nullptr);
}

void Cedrus::XIDDevice::MatchConfigToModel_MPod(char model)
{
    m_config = XIDDeviceScanner::GetDeviceScanner().GetConfigForGivenDevice(GetProductID(), model != -1 ? model : GetModelID(), m_config->GetMajorVersion());
}

void Cedrus::XIDDevice::SetMPodLineMapping_Neuroscan16bit()
{
    unsigned char initial_table = GetTranslationTable();

    SetTranslationTable(0);
    MapSignals(0, 524288);
    MapSignals(1, 0);
    MapSignals(2, 0);
    MapSignals(3, 0);
    MapSignals(4, 256);
    MapSignals(5, 512);
    MapSignals(6, 1024);
    MapSignals(7, 2048);
    MapSignals(8, 16777216);
    MapSignals(9, 33554432);
    MapSignals(10, 67108864);
    MapSignals(11, 134217728);
    MapSignals(12, 268435456);
    MapSignals(13, 536870912);
    MapSignals(14, 1073741824);
    MapSignals(15, 2147483648);
    CommitLineMappingToFlash();

    SetTranslationTable(1);
    MapSignals(0, 65536);
    MapSignals(1, 131072);
    MapSignals(2, 1048576);
    MapSignals(3, 12582912);
    MapSignals(4, 134217728);
    MapSignals(5, 67108864);
    MapSignals(6, 33554432);
    MapSignals(7, 16777216);
    MapSignals(8, 1);
    MapSignals(9, 2);
    MapSignals(10, 4);
    MapSignals(11, 8);
    MapSignals(12, 16);
    MapSignals(13, 32);
    MapSignals(14, 64);
    MapSignals(15, 128);
    CommitLineMappingToFlash();

    SetTranslationTable(initial_table);
    SetMpodOutputMode(0);
}

void Cedrus::XIDDevice::SetMPodLineMapping_NeuroscanGrael()
{
    unsigned char initial_table = GetTranslationTable();

    SetTranslationTable(0);
    MapSignals(0, 0);
    MapSignals(1, 0);
    MapSignals(2, 0);
    MapSignals(3, 0);
    MapSignals(4, 256);
    MapSignals(5, 512);
    MapSignals(6, 1024);
    MapSignals(7, 2048);
    MapSignals(8, 16777216);
    MapSignals(9, 33554432);
    MapSignals(10, 67108864);
    MapSignals(11, 134217728);
    MapSignals(12, 268435456);
    MapSignals(13, 536870912);
    MapSignals(14, 0);
    MapSignals(15, 524288);
    CommitLineMappingToFlash();

    SetTranslationTable(1);
    MapSignals(0, 0);
    MapSignals(1, 0);
    MapSignals(2, 0);
    MapSignals(3, 0);
    MapSignals(4, 0);
    MapSignals(5, 0);
    MapSignals(6, 0);
    MapSignals(7, 0);
    MapSignals(8, 1);
    MapSignals(9, 2);
    MapSignals(10, 4);
    MapSignals(11, 8);
    MapSignals(12, 16);
    MapSignals(13, 32);
    MapSignals(14, 12582912);
    MapSignals(15, 65536);
    CommitLineMappingToFlash();

    SetTranslationTable(initial_table);
    SetMpodOutputMode(1);
    SetMpodPulseDuration(5);
}

void Cedrus::XIDDevice::SetCPodLineMapping_NeuroscanGrael()
{
    MapSignals(0, 0);
    MapSignals(1, 0);
    MapSignals(2, 0);
    MapSignals(3, 0);
    MapSignals(4, 0);
    MapSignals(5, 0);
    MapSignals(6, 0);
    MapSignals(7, 0);
    MapSignals(8, 1);
    MapSignals(9, 2);
    MapSignals(10, 4);
    MapSignals(11, 8);
    MapSignals(12, 16);
    MapSignals(13, 32);
    MapSignals(14, 64);
    MapSignals(15, 128);
    CommitLineMappingToFlash();
}
