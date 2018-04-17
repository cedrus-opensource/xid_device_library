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

int Cedrus::XIDDevice::GetMpodModel(unsigned int mpod) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    static char gmm_cmd[4] = { '_','a','q' };
    gmm_cmd[3] = '0' + mpod;

    unsigned char mpod_model_return[5];
    m_xidCon->SendXIDCommand(gmm_cmd, 4, mpod_model_return, sizeof(mpod_model_return), m_config->NeedsDelay());

    //SLEEP_FUNC(100 * SLEEP_INC);

    bool return_valid = boost::starts_with(mpod_model_return, "_aq");

    unsigned char return_val = (unsigned char)(mpod_model_return[4]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::ConnectToMpod(unsigned int mpod, unsigned int action)
{
    if (!m_config->IsXID2())
        return;

    // m-pods operate at 19200 only
    SetBaudRate(1);

    static unsigned char ctm_cmd[4] = {'a','q'};
    ctm_cmd[2] = '0' + mpod;
    ctm_cmd[3] = '0' + action;

    DWORD bytes_written;
    m_xidCon->Write(ctm_cmd, 4, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(100 * SLEEP_INC);

    MatchConfigToModel(-1);
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
}

void Cedrus::XIDDevice::CommitLineMappingToFlash()
{
    if (!m_config->IsXID2())
        return;

    static unsigned char commit_map_cmd[2] = { 'a','f' };

    DWORD bytes_written;
    m_xidCon->Write(commit_map_cmd, 2, &bytes_written, m_config->NeedsDelay());
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
    return GetProtocol(m_xidCon);
}

/*static*/ std::string Cedrus::XIDDevice::GetProtocol(std::shared_ptr<Connection> xidCon)
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
    return SetProtocol(m_xidCon, protocol);
}

void Cedrus::XIDDevice::SetProtocol(std::shared_ptr<Connection> xidCon, unsigned char protocol)
{
    static unsigned char sdp_cmd[3] = { 'c', '1' };
    sdp_cmd[2] = protocol + '0';

    DWORD bytes_written;
    xidCon->Write(sdp_cmd, 3, &bytes_written, true);
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
    return XIDDevice::GetProductID(m_xidCon);
}

int Cedrus::XIDDevice::GetModelID() const
{
    return XIDDevice::GetModelID(m_xidCon);
}

/*static*/ int Cedrus::XIDDevice::GetProductID(std::shared_ptr<Connection> xidCon)
{
    unsigned char product_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand("_d2", 3, product_id_return, sizeof(product_id_return), true);

    int product_id = (int)(product_id_return[0]);
    bool return_valid = product_id >= 48 && product_id <= 52 || product_id == 83;

    CEDRUS_ASSERT(return_valid, "_d2 command's result value must be between '0' and '4', or be 'S'");

    return return_valid ? product_id : INVALID_RETURN_VALUE;
}

/*static*/ int Cedrus::XIDDevice::GetModelID(std::shared_ptr<Connection> xidCon)
{
    unsigned char model_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand("_d3", 3, model_id_return, sizeof(model_id_return), true);

    int model_id = (int)(model_id_return[0]);
    bool return_valid = model_id >= 48 && model_id <= 105;

    CEDRUS_ASSERT(return_valid, "_d3 command's result value must be between '0' and 'g'");

    return return_valid ? model_id : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetModelID(unsigned char model)
{
    static char set_model_cmd[3] = { 'd', '3' };
    set_model_cmd[2] = model;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_model_cmd, 3, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(100 * SLEEP_INC);

    MatchConfigToModel(model);
}

int Cedrus::XIDDevice::GetMajorFirmwareVersion() const
{
    return GetMajorFirmwareVersion(m_xidCon);
}

/*static*/ int Cedrus::XIDDevice::GetMajorFirmwareVersion(std::shared_ptr<Connection> xidCon)
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
    set_level_cmd[2] = level;

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
}

void Cedrus::XIDDevice::SaveSettingsToFlash()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f9", 2, &bytes_written, m_config->NeedsDelay());
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

bool Cedrus::XIDDevice::GetGenerateTimestampedOutput(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return false;

    unsigned char return_info[5];

    static char gtso_command[3] = { '_','i','o' };
    gtso_command[3] = selector;

    m_xidCon->SendXIDCommand(gtso_command, 3, return_info, sizeof(return_info), m_config->NeedsDelay());

    return boost::starts_with(return_info, "_io") ? return_info[4] - '0' : false;
}

void Cedrus::XIDDevice::SetGenerateTimestampedOutput(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsStimTracker2())
        return;

    static unsigned char stso_command[4] = { 'i','o' };
    stso_command[2] = selector;
    stso_command[3] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(stso_command, 4, &bytes_written, m_config->NeedsDelay());
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

int Cedrus::XIDDevice::GetMpodOutputMode(unsigned char selector) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char cmd_return[5];
    static char gmom_command[4] = { '_','i','m' };
    gmom_command[3] = selector;

    m_xidCon->SendXIDCommand(gmom_command, 4, cmd_return, sizeof(cmd_return), m_config->NeedsDelay());

    return boost::starts_with(cmd_return, "_im") ? (int)(cmd_return[4]) - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDevice::SetMpodOutputMode(unsigned char selector, unsigned char mode)
{
    if (!m_config->IsXID2())
        return;

    static unsigned char smom_command[4] = {'i','m'};
    smom_command[2] = selector;
    smom_command[3] = mode + '0';

    DWORD bytes_written;
    m_xidCon->Write(smom_command, 4, &bytes_written, m_config->NeedsDelay());
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
