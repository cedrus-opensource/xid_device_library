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
#include <string.h>
#include <sstream>
#include <iomanip>
#include <locale>

#include <boost/algorithm/string/predicate.hpp>

Cedrus::XIDDevice::XIDDevice(
    std::shared_ptr<Connection> xidCon,
    std::shared_ptr<const DeviceConfig> devConfig)
    : m_linesState(0),
    m_xidCon(xidCon),
    m_config(devConfig),
    m_podHostConfig(),
    m_ResponseMgr((devConfig->IsRB() || devConfig->IsSV1() || devConfig->IsLumina() || devConfig->IsStimTracker2()) ?
        new ResponseManager(GetMinorFirmwareVersion(), devConfig) : nullptr)
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

    m_xidCon->SendXIDCommand("_a0", 3, output_logic, sizeof(output_logic), m_config->NeedsDelay());

    bool return_valid_a0 = boost::starts_with(output_logic, "_a0");

    return return_valid_a0 ? output_logic[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetOutputLogic(unsigned char mode)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char sol_cmd[3] = { 'a', '0' };
    sol_cmd[2] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(sol_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetAccessoryConnectorMode() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    m_xidCon->SendXIDCommand("_a1", 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    bool return_valid_a1 = boost::starts_with(return_info, "_a1");
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

    DWORD bytes_written;
    m_xidCon->Write(sacm_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetACDebouncingTime() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char threshold_return[4];

    m_xidCon->SendXIDCommand("_a6", 3, threshold_return, sizeof(threshold_return), m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_a6");

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

    DWORD bytes_written;
    m_xidCon->Write(sacdt_command, 3, &bytes_written, m_config->NeedsDelay());
}

unsigned char Cedrus::XIDDevice::GetMpodOutputMode() const
{
    if (!m_config->IsMPod())
        return '0';

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_am", 3, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return cmd_return[3] - '0';
}

void Cedrus::XIDDevice::SetMpodOutputMode(unsigned char mode)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char smom_command[3] = { 'a','m' };
    smom_command[2] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(smom_command, 3, &bytes_written, m_config->NeedsDelay());
}

unsigned char Cedrus::XIDDevice::GetMpodPulseDuration() const
{
    if (!m_config->IsMPod())
        return '0';

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_aw", 3, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return cmd_return[3];
}

void Cedrus::XIDDevice::SetMpodPulseDuration(unsigned char duration)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char smpd_command[3] = { 'a','w' };
    smpd_command[2] = duration;

    DWORD bytes_written;
    m_xidCon->Write(smpd_command, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetMpodModel(unsigned char mpod) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    static char gmm_cmd[4] = { '_','a','q' };
    gmm_cmd[3] = '0' + mpod;

    unsigned char mpod_model_return[5];
    m_xidCon->SendXIDCommand(gmm_cmd, 4, mpod_model_return, sizeof(mpod_model_return), m_config->NeedsDelay());

    bool return_valid = boost::starts_with(mpod_model_return, "_aq");

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

    DWORD bytes_written;
    m_xidCon->Write(ctm_cmd, 4, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(300 * SLEEP_INC);

    m_podHostConfig = (action == 0 ? nullptr : m_config);
    MatchConfigToModel(-1);

    if (action == 0)
        SetBaudRate(rate);
}

unsigned char Cedrus::XIDDevice::GetTranslationTable() const
{
    if (!m_config->IsMPod())
        return '0';

    static char gtt_command[3] = { '_','a','s' };

    unsigned char return_info[4];
    m_xidCon->SendXIDCommand(gtt_command, 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    return return_info[3] + '0';
}

void Cedrus::XIDDevice::SetTranslationTable(unsigned char table)
{
    if (!m_config->IsMPod())
        return;

    static unsigned char stt_cmd[3] = { 'a','s' };
    stt_cmd[2] = table + '0';

    DWORD bytes_written;
    m_xidCon->Write(stt_cmd, 3, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(100 * SLEEP_INC);
}

unsigned int Cedrus::XIDDevice::GetMappedSignals(unsigned int line)
{
    if (!m_config->IsXID2())
        return 0;

    static char gms_cmd[4] = { '_','a','t' };
    gms_cmd[3] = line > 9 ? '7' + line : '0' + line;

    unsigned char mapped_signals_return[12];
    m_xidCon->SendXIDCommand(gms_cmd, 4, mapped_signals_return, sizeof(mapped_signals_return), m_config->NeedsDelay());

    bool return_valid = boost::starts_with(mapped_signals_return, "_at");

    char mask[9];
    mask[8] = '\0';

    memcpy(mask, mapped_signals_return + 4, 8 * sizeof(unsigned char));
    std::string mapped_signals = mask;

    unsigned int signals = 0;
    if (!mapped_signals.empty())
        signals = std::stoul(mapped_signals, nullptr, 16);

    //SLEEP_FUNC(100 * SLEEP_INC);

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
    map_signals_cmd[2] = line > 9 ? '7' + line : '0' + line;
    map_signals_cmd[3] = string_mask[0];
    map_signals_cmd[4] = string_mask[1];
    map_signals_cmd[5] = string_mask[2];
    map_signals_cmd[6] = string_mask[3];
    map_signals_cmd[7] = string_mask[4];
    map_signals_cmd[8] = string_mask[5];
    map_signals_cmd[9] = string_mask[6];
    map_signals_cmd[10] = string_mask[7];

    DWORD bytes_written;
    m_xidCon->Write(map_signals_cmd, 11, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::ResetMappedLinesToDefault()
{
    if (!m_config->IsXID2())
        return;

    static unsigned char rmltd_cmd[3] = { 'a','t','X' };

    DWORD bytes_written;
    m_xidCon->Write(rmltd_cmd, 3, &bytes_written, m_config->NeedsDelay());

    if (m_config->IsMPod())
    {
        if (m_config->GetModelID() == 72) // Neuroscan 16-bit
        {
            SetPodLineMapping_Neuroscan16bit();
        }
        else if (m_config->GetModelID() == 104) // Neuroscan Grael
        {
            SetPodLineMapping_NeuroscanGrael();
        }
    }
}

void Cedrus::XIDDevice::CommitLineMappingToFlash()
{
    if (!m_config->IsXID2())
        return;

    static unsigned char commit_map_cmd[2] = { 'a','f' };

    DWORD bytes_written;
    m_xidCon->Write(commit_map_cmd, 2, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(100*SLEEP_INC);
}

int Cedrus::XIDDevice::GetVKDropDelay() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char vk_drop_delay[4];

    m_xidCon->SendXIDCommand("_b3", 4, vk_drop_delay, sizeof(vk_drop_delay), m_config->NeedsDelay());

    return vk_drop_delay[3];
}

void Cedrus::XIDDevice::SetVKDropDelay(unsigned char delay)
{
    if (!m_config->IsXID1InputDevice())
        return;

    static unsigned char svkdd_cmd[3] = { 'b','3' };
    svkdd_cmd[2] = delay;

    DWORD bytes_written;
    m_xidCon->Write(svkdd_cmd, 3, &bytes_written, m_config->NeedsDelay());
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

    int bytes_count = xidCon->SendXIDCommand_PST_Proof("_c1", 3, return_info, sizeof(return_info), true);

    CEDRUS_ASSERT(bytes_count >= 1 || (return_info[0] == 0 && return_info[4] == 0),
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer");

    bool return_valid = boost::starts_with(return_info, "_xid");

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

    DWORD bytes_written;
    xidCon->Write(sdp_cmd, 3, &bytes_written, true);
}

void Cedrus::XIDDevice::SwitchToKeyboardMode()
{
    if (!(m_config->IsLumina3G() || m_config->IsRBx40()))
        return;

    static unsigned char stkm_cmd[2] = { 'c', '3' };

    DWORD bytes_written;
    m_xidCon->Write(stkm_cmd, 2, &bytes_written, m_config->NeedsDelay());
}

std::string Cedrus::XIDDevice::GetCombinedInfo() const
{
    unsigned char return_info[1000];
    m_xidCon->SendXIDCommand("_d0", 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    std::string return_name((char*)return_info);

    std::replace(return_name.begin(), return_name.end(), '\r', '\n');

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

std::string Cedrus::XIDDevice::GetInternalProductName() const
{
    unsigned char return_info[100];
    m_xidCon->SendXIDCommand("_d1", 3, return_info, sizeof(return_info), m_config->NeedsDelay());

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

    xidCon->SendXIDCommand("_d2", 3, product_id_return, sizeof(product_id_return), true);

    int product_id = (int)(product_id_return[0]);

    return product_id;
}

/*static*/ int Cedrus::XIDDevice::GetModelID_Scan(std::shared_ptr<Connection> xidCon)
{
    unsigned char model_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand("_d3", 3, model_id_return, sizeof(model_id_return), true);

    int model_id = (int)(model_id_return[0]);

    return model_id;
}

void Cedrus::XIDDevice::SetModelID(unsigned char model)
{
    static char set_model_cmd[3] = { 'd', '3' };
    set_model_cmd[2] = model;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_model_cmd, 3, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(300 * SLEEP_INC);

    MatchConfigToModel(model);

    if (m_config->IsMPod())
    {
        if (model == 72) // Neuroscan 16-bit
        {
            SetPodLineMapping_Neuroscan16bit();
        }
        else if (model == 104) // Neuroscan Grael
        {
            SetPodLineMapping_NeuroscanGrael();
        }
        else
        {
            ResetMappedLinesToDefault();
            CommitLineMappingToFlash();
        }
    }
}

int Cedrus::XIDDevice::GetMajorFirmwareVersion() const
{
    return GetMajorFirmwareVersion_Scan(m_xidCon);
}

/*static*/ int Cedrus::XIDDevice::GetMajorFirmwareVersion_Scan(std::shared_ptr<Connection> xidCon)
{
    unsigned char major_return[1];

    xidCon->SendXIDCommand("_d4", 3, major_return, sizeof(major_return), true);

    bool return_valid = (major_return[0] >= 48 && major_return[0] <= 50);

    CEDRUS_ASSERT(return_valid, "_d4 command's result value must be between '0' and '2'");

    return return_valid ? major_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetMinorFirmwareVersion() const
{
    unsigned char minor_return[1];

    m_xidCon->SendXIDCommand("_d5", 3, minor_return, sizeof(minor_return), m_config->NeedsDelay());

    bool return_valid = minor_return[0] >= 48;

    CEDRUS_ASSERT(return_valid, "_d5 command's result value must be '0' or greater");

    return return_valid ? minor_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetOutpostModel() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char outpost_return[1];

    m_xidCon->SendXIDCommand("_d6", 3, outpost_return, sizeof(outpost_return), m_config->NeedsDelay());

    bool return_valid = (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x';

    return return_valid ? (int)(outpost_return[0]) : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDevice::GetHardwareGeneration() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char gen_return[1];

    m_xidCon->SendXIDCommand("_d7", 3, gen_return, sizeof(gen_return), m_config->NeedsDelay());

    return gen_return[0] - '0';
}

void Cedrus::XIDDevice::ResetBaseTimer()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"e1", 2, &bytes_written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDevice::QueryBaseTimer()
{
    if (!m_config->IsXID1())
        return 0;

    unsigned int base_timer = 0;
    unsigned char return_info[6];
    int read = m_xidCon->SendXIDCommand("e3", 2, return_info, sizeof(return_info), m_config->NeedsDelay());

    CEDRUS_ASSERT(boost::starts_with(return_info, "e3"), "QueryBaseTimer xid query result must start with e3");
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
    m_xidCon->SendXIDCommand(qrt_command, 3, return_info, sizeof(return_info), m_config->NeedsDelay());

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

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"e5", 2, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::SetBaudRate(unsigned char rate)
{
    if (m_config->IsMPod())
        return;

    static unsigned char change_baud_cmd[3] = { 'f', '1' };
    change_baud_cmd[2] = rate;

    DWORD bytes_written;
    m_xidCon->Write(change_baud_cmd, 3, &bytes_written, m_config->NeedsDelay());

    m_xidCon->SetBaudRate(rate);
    CloseConnection();
    OpenConnection();
}

void Cedrus::XIDDevice::GetLockingLevel()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"_f2", 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::SetLockingLevel(unsigned char level)
{
    if (!m_config->IsXID1())
        return;

    static unsigned char set_level_cmd[3] = { 'f', '2' };
    set_level_cmd[2] = level + '0';

    DWORD bytes_written;
    m_xidCon->Write(set_level_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::ReprogramFlash()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f3", 2, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetTriggerDefault() const
{
    unsigned char default_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand("_f4", 3, default_return, sizeof(default_return), m_config->NeedsDelay());

    bool return_valid_f4 = boost::starts_with(default_return, "_f4");
    bool return_valid_val = default_return[3] == '0' || default_return[3] == '1';

    CEDRUS_ASSERT(return_valid_f4, "GetTriggerDefault's xid query result must start with _f4");
    CEDRUS_ASSERT(return_valid_val, "GetTriggerDefault's value must be either '0' or '1'");

    return (return_valid_f4 && return_valid_val) ?
        static_cast<int>(default_return[3] == '1')
        : static_cast<int>(INVALID_RETURN_VALUE);
}

void Cedrus::XIDDevice::SetTriggerDefault(bool defaultOn)
{
    static unsigned char set_trigger_default_cmd[3] = { 'f', '4' };
    set_trigger_default_cmd[2] = (unsigned char)defaultOn + '0';

    DWORD bytes_written;
    m_xidCon->Write(set_trigger_default_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetTriggerDebounceTime() const
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand("_f5", 3, threshold_return, sizeof(threshold_return), m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_f5");

    CEDRUS_ASSERT(return_valid, "GetTriggerDebounceTime's xid query result must start with _f5");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetTriggerDebounceTime(unsigned char time)
{
    static unsigned char set_debouncing_time_cmd[3] = { 'f', '5' };
    set_debouncing_time_cmd[2] = time;

    DWORD bytes_written;
    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetButtonDebounceTime() const
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand("_f6", 3, threshold_return, sizeof(threshold_return), m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_f6");

    CEDRUS_ASSERT(return_valid, "GetButtonDebounceTime's xid query result must start with _f6");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetButtonDebounceTime(unsigned char time)
{
    static unsigned char set_debouncing_time_cmd[3] = { 'f', '6' };
    set_debouncing_time_cmd[2] = time;

    DWORD bytes_written;
    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::RestoreFactoryDefaults()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f7", 2, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(200 * SLEEP_INC);

    if (m_config->IsMPod())
    {
        SetTranslationTable(m_podHostConfig->IsStimTracker2() ? 1 : 0);
        if (m_config->GetModelID() == 72) // Neuroscan 16-bit
        {
            SetPodLineMapping_Neuroscan16bit();
        }
        else if (m_config->GetModelID() == 104) // Neuroscan Grael
        {
            SetPodLineMapping_NeuroscanGrael();
        }
    }
}

void Cedrus::XIDDevice::SaveSettingsToFlash()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f9", 2, &bytes_written, m_config->NeedsDelay());
}

Cedrus::SingleShotMode Cedrus::XIDDevice::GetSingleShotMode(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return SingleShotMode();

    static char gssm_command[4] = { '_','i','a' };

    gssm_command[3] = selector;

    unsigned char return_info[9];
    m_xidCon->SendXIDCommand(gssm_command, 4, return_info, sizeof(return_info), m_config->NeedsDelay());

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

    DWORD bytes_written;
    m_xidCon->Write(sssm_cmd, 8, &bytes_written, m_config->NeedsDelay());
}

Cedrus::SignalFilter Cedrus::XIDDevice::GetSignalFilter(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return SignalFilter();

    static char gsf_command[4] = { '_','i','f' };

    gsf_command[3] = selector;

    unsigned char return_info[12];
    m_xidCon->SendXIDCommand(gsf_command, 4, return_info, sizeof(return_info), m_config->NeedsDelay());

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

    DWORD bytes_written;
    m_xidCon->Write(ssf_cmd, 11, &bytes_written, m_config->NeedsDelay());
}

bool Cedrus::XIDDevice::IsRBx40LEDEnabled() const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_il", 3, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return (bool)(cmd_return[3] - '0');
}

void Cedrus::XIDDevice::EnableRBx40LED(bool enable)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_threshold_cmd[3] = { 'i','l' };
    change_threshold_cmd[2] = enable ? '1' : '0';

    DWORD bytes_written;
    m_xidCon->Write(change_threshold_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

bool Cedrus::XIDDevice::GetEnableDigitalOutput(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char return_info[5];

    static char gtso_command[4] = { '_','i','o' };
    gtso_command[3] = selector;

    m_xidCon->SendXIDCommand(gtso_command, 4, return_info, sizeof(return_info), m_config->NeedsDelay());

    return boost::starts_with(return_info, "_io") ? return_info[4] - '0' : false;
}

void Cedrus::XIDDevice::SetEnableDigitalOutput(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsStimTracker2())
        return;

    static unsigned char stso_command[4] = { 'i','o' };
    stso_command[2] = selector;
    stso_command[3] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(stso_command, 4, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetTimerResetOnOnsetMode(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    static char gtrom_command[4] = {'_','i','r'};
    gtrom_command[3] = selector;

    unsigned char return_info[5];
    m_xidCon->SendXIDCommand(gtrom_command, 4, return_info, sizeof(return_info), m_config->NeedsDelay());

    return boost::starts_with(return_info, "_ir") ? return_info[4] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetTimerResetOnOnsetMode(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_mode_cmd[4] = { 'i','r' };
    change_mode_cmd[2] = selector;
    change_mode_cmd[3] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(change_mode_cmd, 4, &bytes_written, m_config->NeedsDelay());
}

bool Cedrus::XIDDevice::GetEnableUSBOutput(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char return_info[5];

    static char geuo_command[4] = { '_','i','u' };
    geuo_command[3] = selector;

    m_xidCon->SendXIDCommand(geuo_command, 4, return_info, sizeof(return_info), m_config->NeedsDelay());

    return boost::starts_with(return_info, "_iu") ? return_info[4] - '0' : false;
}

void Cedrus::XIDDevice::SetEnableUSBOutput(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsStimTracker2())
        return;

    static unsigned char seuo_command[4] = { 'i','u' };
    seuo_command[2] = selector;
    seuo_command[3] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(seuo_command, 4, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetAnalogInputThreshold(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char cmd_return[5];
    static char gait_command[4] = { '_','i','t' };
    gait_command[3] = selector;

    m_xidCon->SendXIDCommand(gait_command, 4, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return boost::starts_with(cmd_return, "_it") ? (int)(cmd_return[4]) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetAnalogInputThreshold(unsigned char selector, unsigned char threshold)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_threshold_cmd[4] = { 'i','t' };
    change_threshold_cmd[2] = selector;
    change_threshold_cmd[3] = threshold;

    DWORD bytes_written;
    m_xidCon->Write(change_threshold_cmd, 4, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDevice::GetMixedInputMode() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_iv", 3, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return boost::starts_with(cmd_return, "_iv") ? (int)(cmd_return[3]) - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetMixedInputMode(unsigned char mode)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char change_threshold_cmd[3] = { 'i','v' };
    change_threshold_cmd[2] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(change_threshold_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDevice::GetNumberOfLines() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char gen_return[4];

    m_xidCon->SendXIDCommand("_ml", 3, gen_return, sizeof(gen_return), m_config->NeedsDelay());

    return gen_return[3];
}

void Cedrus::XIDDevice::SetNumberOfLines(unsigned int lines)
{
    static unsigned char set_number_of_lines_cmd[3] = { 'm','l' };
    set_number_of_lines_cmd[2] = lines;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_number_of_lines_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDevice::GetPulseDuration() const
{
    if (m_config->IsXID1InputDevice() || m_config->IsMPod())
        return 0;

    unsigned char return_info[7];
    m_xidCon->SendXIDCommand("_mp", 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    CEDRUS_ASSERT(boost::starts_with(return_info, "_mp"), "GetPulseDuration's return value must start with _mp");

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
    m_xidCon->Write(spd_command, 6, &written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDevice::GetPulseTableBitMask()
{
    if (!m_config->IsXID2())
        return 0;

    unsigned char return_info[5];
    m_xidCon->SendXIDCommand("_mk", 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    unsigned int dur = AdjustEndiannessCharsToUint(0,0,
        return_info[3],
        return_info[4]);

    return dur;
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

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::ClearPulseTable()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"mc", 2, &bytes_written, m_config->NeedsDelay());
}

bool Cedrus::XIDDevice::IsPulseTableRunning() const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char cmd_return[4];

    m_xidCon->SendXIDCommand("_mr", 3, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return boost::starts_with(cmd_return, "_mr") ? (int)(cmd_return[3]) - '0' : false;
}

void Cedrus::XIDDevice::RunPulseTable()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"mr", 2, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::StopPulseTable()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"ms", 2, &bytes_written, m_config->NeedsDelay());
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

    DWORD bytes_written;
    m_xidCon->Write(apte_cmd, 8, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::ResetOutputLines()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"mz", 2, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines)
{
    unsigned int output_lines = linesBitmask;

    if (leaveRemainingLines)
        output_lines |= m_linesState;

    if (((m_config->IsRB() || m_config->IsLumina()) && m_config->IsXID2()) || m_config->IsSV1())
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

    if (((m_config->IsRB() || m_config->IsLumina()) && m_config->IsXID2()) || m_config->IsSV1())
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
    if (((m_config->IsRB() || m_config->IsLumina()) && m_config->IsXID2()) || m_config->IsSV1())
    {
        SetDigitalOutputLines_RB(m_xidCon, 0);
    }
    else
    {
        SetDigitalOutputLines_ST(m_xidCon, 0);
    }

    m_linesState = 0;
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

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::SetDigitalOutputLines_ST(std::shared_ptr<Connection> xidCon, unsigned int lines)
{
    unsigned int mask = lines;

    static char set_lines_cmd[4] = { 'm','h' };
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDevice::MatchConfigToModel(char model)
{
    m_config = XIDDeviceScanner::GetDeviceScanner().GetConfigForGivenDevice(GetProductID(), model != -1 ? model : GetModelID(), m_config->GetMajorVersion());
}

void Cedrus::XIDDevice::SetPodLineMapping_Neuroscan16bit()
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

void Cedrus::XIDDevice::SetPodLineMapping_NeuroscanGrael()
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
}