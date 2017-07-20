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

#include "XIDDeviceImpl.h"

#include "constants.h"
#include "ResponseManager.h"
#include "Connection.h"
#include "DeviceConfig.h"

#include "CedrusAssert.h"
#include <string.h>
#include <sstream>

#include <boost/algorithm/string/predicate.hpp>

Cedrus::XIDDeviceImpl::XIDDeviceImpl(
            boost::shared_ptr<Connection> xidCon,
            boost::shared_ptr<const DeviceConfig> devConfig)
    : m_xidCon(xidCon),
    m_config(devConfig),
    m_ResponseMgr((devConfig->IsRB() || devConfig->IsSV1() || devConfig->IsLumina()) ?
        new ResponseManager(GetMinorFirmwareVersion(), devConfig) : nullptr)
{
}

Cedrus::XIDDeviceImpl::~XIDDeviceImpl()
{

}

int Cedrus::XIDDeviceImpl::GetOutputLogic() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char output_logic[4];

    m_xidCon->SendXIDCommand(
        "_a0",
        output_logic,
        sizeof(output_logic),
        m_config->NeedsDelay());

    bool return_valid_a0 = boost::starts_with(output_logic, "_a0");

    return return_valid_a0 ? output_logic[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetOutputLogic(unsigned char mode)
{
    if (!m_config->IsXID1InputDevice())
        return;

    DWORD bytes_written;
    unsigned char set_output_logic_cmd[3];
    set_output_logic_cmd[0] = 'a';
    set_output_logic_cmd[1] = '0';
    set_output_logic_cmd[2] = mode + '0';

    m_xidCon->Write(set_output_logic_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetAccessoryConnectorMode() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    m_xidCon->SendXIDCommand(
        "_a1",
        return_info,
        sizeof(return_info),
        m_config->NeedsDelay());

    bool return_valid_a1 = boost::starts_with(return_info, "_a1");
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT(return_valid_a1, "GetAccessoryConnectorMode's return value must start with _a1");
    CEDRUS_ASSERT(return_valid_val, "GetAccessoryConnectorMode's return value must be between '0' and '3'");

    return (return_valid_a1 && return_valid_val) ? return_info[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetAccessoryConnectorMode(unsigned char mode)
{
    if (!m_config->IsXID1InputDevice())
        return;

    DWORD bytes_written;
    unsigned char set_accessory_connector_mode_cmd[3];
    set_accessory_connector_mode_cmd[0] = 'a';
    set_accessory_connector_mode_cmd[1] = '1';
    set_accessory_connector_mode_cmd[2] = mode + '0';

    m_xidCon->Write(set_accessory_connector_mode_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetACDebouncingTime() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char threshold_return[4];

    m_xidCon->SendXIDCommand(
        "_a6",
        threshold_return,
        sizeof(threshold_return),
        m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_a6");

    CEDRUS_ASSERT(return_valid, "GetACDebouncingTime's xid query result must start with _a6");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetACDebouncingTime(unsigned char time)
{
    if (!m_config->IsXID1InputDevice())
        return;

    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'a';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetMpodModel(unsigned int mpod) const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    char get_mpod_model_cmd[4];
    get_mpod_model_cmd[0] = '_';
    get_mpod_model_cmd[1] = 'a';
    get_mpod_model_cmd[2] = 'q';
    get_mpod_model_cmd[3] = '0' + mpod;

    unsigned char mpod_model_return[5];
    m_xidCon->SendXIDCommand(
        get_mpod_model_cmd,
        mpod_model_return,
        sizeof(mpod_model_return),
        m_config->NeedsDelay());

    bool return_valid = boost::starts_with(mpod_model_return, "_aq");

    unsigned char return_val = (unsigned char)(mpod_model_return[4]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::ConnectToMpod(unsigned int mpod, unsigned int action)
{
    if (!m_config->IsXID2())
        return;

    DWORD bytes_written;
    unsigned char connect_to_mpod_cmd[4];
    connect_to_mpod_cmd[0] = 'a';
    connect_to_mpod_cmd[1] = 'q';
    connect_to_mpod_cmd[2] = '0' + mpod;
    connect_to_mpod_cmd[3] = '0' + action;

    m_xidCon->Write(connect_to_mpod_cmd, 4, &bytes_written, m_config->NeedsDelay());

    SLEEP_FUNC(200 * SLEEP_INC);
}

int Cedrus::XIDDeviceImpl::GetVKDropDelay() const
{
    if (!m_config->IsXID1InputDevice())
        return INVALID_RETURN_VALUE;

    unsigned char vk_drop_delay[4];

    m_xidCon->SendXIDCommand(
        "_b3",
        vk_drop_delay,
        sizeof(vk_drop_delay),
        m_config->NeedsDelay());

    return vk_drop_delay[3];
}

void Cedrus::XIDDeviceImpl::SetVKDropDelay(unsigned char delay)
{
    if (!m_config->IsXID1InputDevice())
        return;

    DWORD bytes_written;
    unsigned char set_vk_drop_delay_cmd[3];
    set_vk_drop_delay_cmd[0] = 'b';
    set_vk_drop_delay_cmd[1] = '3';
    set_vk_drop_delay_cmd[2] = delay;

    m_xidCon->Write(set_vk_drop_delay_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

std::string Cedrus::XIDDeviceImpl::GetProtocol() const
{
    return GetProtocol(m_xidCon);
}

/*static*/ std::string Cedrus::XIDDeviceImpl::GetProtocol(boost::shared_ptr<Connection> xidCon)
{
    // There's a possibility that the device is in E-Prime mode. Right
    // now is the only time the library cares about it, and we need to
    // do some PST-proofing. To start, flush everything to remove the
    // potential spew of zeroes.
    unsigned char return_info[5]; // we rely on SendXIDCommand to zero-initialize this buffer
    int bytes_count;

    xidCon->FlushReadFromDeviceBuffer();

    bytes_count = xidCon->SendXIDCommand_PST_Proof("_c1",
        return_info,
        sizeof(return_info),
        true);

    CEDRUS_ASSERT(bytes_count >= 1 || (return_info[0] == 0 && return_info[4] == 0),
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer");

    bool return_valid = boost::starts_with(return_info, "_xid");

    // It's okay for this to sometimes return nothing at all. That just
    // means we queried an incorrect baud and there's nothing wrong with that.
    CEDRUS_ASSERT(return_valid || return_info[0] == 0,
        "GetProtocol's return value must start with _xid");

    return return_valid ? std::string((char*)return_info, sizeof(return_info)) : std::string("Invalid Protocol");
}

void Cedrus::XIDDeviceImpl::SetProtocol(unsigned char protocol)
{
    return SetProtocol(m_xidCon, protocol);
}

void Cedrus::XIDDeviceImpl::SetProtocol(boost::shared_ptr<Connection> xidCon, unsigned char protocol)
{
    DWORD bytes_written;
    unsigned char set_device_protocol_cmd[3];
    set_device_protocol_cmd[0] = 'c';
    set_device_protocol_cmd[1] = '1';
    set_device_protocol_cmd[2] = protocol + '0';

    xidCon->Write(set_device_protocol_cmd, 3, &bytes_written, true);
}

std::string Cedrus::XIDDeviceImpl::GetInternalProductName() const
{
    unsigned char return_info[100];
    m_xidCon->SendXIDCommand(
        "_d1",
        return_info,
        sizeof(return_info),
        m_config->NeedsDelay());

    std::string return_name((char*)return_info);

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

int Cedrus::XIDDeviceImpl::GetProductID() const
{
    return XIDDeviceImpl::GetProductID(m_xidCon);
}

int Cedrus::XIDDeviceImpl::GetModelID() const
{
    return XIDDeviceImpl::GetModelID(m_xidCon);
}

/*static*/ int Cedrus::XIDDeviceImpl::GetProductID(boost::shared_ptr<Connection> xidCon)
{
    unsigned char product_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->FlushReadFromDeviceBuffer();

    xidCon->SendXIDCommand_PST_Proof(
        "_d2",
        product_id_return,
        sizeof(product_id_return),
        true);

    int product_id = (int)(product_id_return[0]);
    bool return_valid = product_id >= 48 && product_id <= 52 || product_id == 83;

    CEDRUS_ASSERT(return_valid, "_d2 command's result value must be between '0' and '4', or be 'S'");

    return return_valid ? product_id : INVALID_RETURN_VALUE;
}

/*static*/ int Cedrus::XIDDeviceImpl::GetModelID(boost::shared_ptr<Connection> xidCon)
{
    unsigned char model_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->FlushReadFromDeviceBuffer();

    xidCon->SendXIDCommand_PST_Proof(
        "_d3",
        model_id_return,
        sizeof(model_id_return),
        true);

    int model_id = (int)(model_id_return[0]);
    bool return_valid = model_id >= 48 && model_id <= 103;

    CEDRUS_ASSERT(return_valid, "_d3 command's result value must be between '0' and 'g'");

    return return_valid ? model_id : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetModelID(unsigned char model)
{
    char set_model_cmd[3];

    set_model_cmd[0] = 'd';
    set_model_cmd[1] = '3';
    set_model_cmd[2] = model;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_model_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetMajorFirmwareVersion() const
{
    return GetMajorFirmwareVersion(m_xidCon);
}

/*static*/ int Cedrus::XIDDeviceImpl::GetMajorFirmwareVersion(boost::shared_ptr<Connection> xidCon)
{
    unsigned char major_return[1];

    xidCon->FlushReadFromDeviceBuffer();

    xidCon->SendXIDCommand_PST_Proof(
        "_d4",
        major_return,
        sizeof(major_return),
        true);

    bool return_valid = (major_return[0] >= 48 && major_return[0] <= 50);

    CEDRUS_ASSERT(return_valid, "_d4 command's result value must be between '0' and '2'");

    return return_valid ? major_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDeviceImpl::GetMinorFirmwareVersion() const
{
    unsigned char minor_return[1];

    m_xidCon->SendXIDCommand(
        "_d5",
        minor_return,
        sizeof(minor_return),
        m_config->NeedsDelay());

    bool return_valid = minor_return[0] >= 48;

    CEDRUS_ASSERT(return_valid, "_d5 command's result value must be '0' or greater");

    return return_valid ? minor_return[0] - '0' : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDeviceImpl::GetOutpostModel() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char outpost_return[1];

    m_xidCon->SendXIDCommand(
        "_d6",
        outpost_return,
        sizeof(outpost_return),
        m_config->NeedsDelay());

    bool return_valid = (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x';

    return return_valid ? (int)(outpost_return[0]) : INVALID_RETURN_VALUE;
}

int Cedrus::XIDDeviceImpl::GetHardwareGeneration() const
{
    if (!m_config->IsXID2())
        return INVALID_RETURN_VALUE;

    unsigned char gen_return[1];

    m_xidCon->SendXIDCommand(
        "_d7",
        gen_return,
        sizeof(gen_return),
        m_config->NeedsDelay());

    return gen_return[0] - '0';
}

void Cedrus::XIDDeviceImpl::ResetBaseTimer()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"e1", 2, &bytes_written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDeviceImpl::QueryBaseTimer()
{
    if (!m_config->IsXID1())
        return 0;

    unsigned int base_timer = 0;
    unsigned char return_info[6];
    int read = m_xidCon->SendXIDCommand(
        "e3",
        return_info,
        sizeof(return_info),
        m_config->NeedsDelay());

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

void Cedrus::XIDDeviceImpl::ResetRtTimer()
{
    if (!m_config->IsXID1InputDevice())
        return;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"e5", 2, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::SetBaudRate(unsigned char rate)
{
    if (m_config->IsMPod())
        return;

    DWORD bytes_written;
    unsigned char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    m_xidCon->Write(change_baud_cmd, 3, &bytes_written, m_config->NeedsDelay());

    m_xidCon->SetBaudRate(rate);
    CloseConnection();
    OpenConnection();
}

void Cedrus::XIDDeviceImpl::GetLockingLevel()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"_f2", 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::SetLockingLevel(unsigned char level)
{
    if (!m_config->IsXID1())
        return;

    DWORD bytes_written;
    unsigned char set_level_cmd[3];
    set_level_cmd[0] = 'f';
    set_level_cmd[1] = '2';
    set_level_cmd[2] = level;

    m_xidCon->Write(set_level_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::ReprogramFlash()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f3", 2, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetTriggerDefault() const
{
    unsigned char default_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand(
        "_f4",
        default_return,
        sizeof(default_return),
        m_config->NeedsDelay());

    bool return_valid_f4 = boost::starts_with(default_return, "_f4");
    bool return_valid_val = default_return[3] == '0' || default_return[3] == '1';

    CEDRUS_ASSERT(return_valid_f4, "GetTriggerDefault's xid query result must start with _f4");
    CEDRUS_ASSERT(return_valid_val, "GetTriggerDefault's value must be either '0' or '1'");

    return (return_valid_f4 && return_valid_val) ?
        static_cast<int>(default_return[3] == '1')
        : static_cast<int>(INVALID_RETURN_VALUE);
}

void Cedrus::XIDDeviceImpl::SetTriggerDefault(bool defaultOn)
{
    DWORD bytes_written;
    unsigned char set_trigger_default_cmd[3];
    set_trigger_default_cmd[0] = 'f';
    set_trigger_default_cmd[1] = '4';
    set_trigger_default_cmd[2] = (unsigned char)defaultOn + '0';

    m_xidCon->Write(set_trigger_default_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetTriggerDebounceTime() const
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand(
        "_f5",
        threshold_return,
        sizeof(threshold_return),
        m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_f5");

    CEDRUS_ASSERT(return_valid, "GetTriggerDebounceTime's xid query result must start with _f5");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetTriggerDebounceTime(unsigned char time)
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '5';
    set_debouncing_time_cmd[2] = time;

    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetButtonDebounceTime() const
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand(
        "_f6",
        threshold_return,
        sizeof(threshold_return),
        m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_f6");

    CEDRUS_ASSERT(return_valid, "GetButtonDebounceTime's xid query result must start with _f6");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetButtonDebounceTime(unsigned char time)
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    m_xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::RestoreFactoryDefaults()
{
    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)"f7", 2, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetNumberOfLines() const
{
    if (!(m_config->IsCPod() || m_config->IsMPod()))
        return INVALID_RETURN_VALUE;

    unsigned char gen_return[4];

    m_xidCon->SendXIDCommand(
        "_ml",
        gen_return,
        sizeof(gen_return),
        m_config->NeedsDelay());

    return gen_return[3];
}

void Cedrus::XIDDeviceImpl::SetNumberOfLines(unsigned int lines)
{
    unsigned char set_number_of_lines_cmd[3];

    set_number_of_lines_cmd[0] = 'm';
    set_number_of_lines_cmd[1] = 'l';
    set_number_of_lines_cmd[2] = lines;

    DWORD bytes_written;
    m_xidCon->Write((unsigned char*)set_number_of_lines_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

unsigned int Cedrus::XIDDeviceImpl::GetPulseDuration() const
{
    if ((m_config->IsXID1InputDevice() || m_config->IsMPod()))
        return 0;

    unsigned char return_info[7];
    m_xidCon->SendXIDCommand(
        "_mp",
        return_info,
        sizeof(return_info), 
        m_config->NeedsDelay());

    CEDRUS_ASSERT(boost::starts_with(return_info, "_mp"), "GetPulseDuration's return value must start with _mp");

    unsigned int dur = AdjustEndiannessCharsToUint(
        return_info[3],
        return_info[4],
        return_info[5],
        return_info[6]);

    return dur;
}

void Cedrus::XIDDeviceImpl::SetPulseDuration(unsigned int duration)
{
    if ((m_config->IsXID1InputDevice() || m_config->IsMPod()))
        return;

    unsigned char command[6];
    command[0] = 'm';
    command[1] = 'p';

    AdjustEndiannessUintToChars(
        duration,
        &(command[2]),
        &(command[3]),
        &(command[4]),
        &(command[5]));

    DWORD written = 0;
    m_xidCon->Write(command, 6, &written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetLightSensorMode() const
{
    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    const DWORD bytes_count = m_xidCon->SendXIDCommand(
        "_lr",
        return_info,
        sizeof(return_info),
        m_config->NeedsDelay());

    CEDRUS_ASSERT(bytes_count >= 1 || (return_info[0] == 0 && return_info[3] == 0),
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer");

    bool return_valid_lr = boost::starts_with(return_info, "_lr");
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT(return_valid_lr, "GetLightSensorMode xid query result must start with _lr");
    CEDRUS_ASSERT(return_valid_val, "GetLightSensorMode's return value must be between '0' and '3'");

    return (return_valid_lr && return_valid_val) ? return_info[3] - '0' : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::SetLightSensorMode(unsigned char mode)
{
    DWORD bytes_written;
    unsigned char change_mode_cmd[3];
    change_mode_cmd[0] = 'l';
    change_mode_cmd[1] = 'r';
    change_mode_cmd[2] = mode + '0';

    m_xidCon->Write(change_mode_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::SetLightSensorThreshold(unsigned char threshold)
{
    DWORD bytes_written;
    unsigned char change_threshold_cmd[3];
    change_threshold_cmd[0] = 'l';
    change_threshold_cmd[1] = 't';
    change_threshold_cmd[2] = threshold;

    m_xidCon->Write(change_threshold_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

int Cedrus::XIDDeviceImpl::GetLightSensorThreshold() const
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    m_xidCon->SendXIDCommand(
        "_lt",
        threshold_return,
        sizeof(threshold_return),
        m_config->NeedsDelay());

    bool return_valid = boost::starts_with(threshold_return, "_lt");

    CEDRUS_ASSERT(return_valid, "GetLightSensorThreshold xid query result must start with _lt");

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void Cedrus::XIDDeviceImpl::RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines)
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

void Cedrus::XIDDeviceImpl::LowerLines(unsigned int linesBitmask, bool leaveRemainingLines)
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

void Cedrus::XIDDeviceImpl::ClearLines()
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

int Cedrus::XIDDeviceImpl::GetBaudRate() const
{
    return m_xidCon->GetBaudRate();
}

const boost::shared_ptr<const Cedrus::DeviceConfig> Cedrus::XIDDeviceImpl::GetDeviceConfig() const
{
    return m_config;
}

int Cedrus::XIDDeviceImpl::OpenConnection() const
{
    return m_xidCon->Open();
}

int Cedrus::XIDDeviceImpl::CloseConnection() const
{
    return m_xidCon->Close();
}

bool Cedrus::XIDDeviceImpl::HasLostConnection() const
{
    return m_xidCon->HasLostConnection();
}

void Cedrus::XIDDeviceImpl::PollForResponse() const
{
    m_ResponseMgr->CheckForKeypress(m_xidCon, m_config);
}

bool Cedrus::XIDDeviceImpl::HasQueuedResponses() const
{
    return m_ResponseMgr->HasQueuedResponses();
}

int Cedrus::XIDDeviceImpl::GetNumberOfKeysDown() const
{
    return m_ResponseMgr->GetNumberOfKeysDown();
}

Cedrus::Response Cedrus::XIDDeviceImpl::GetNextResponse() const
{
    return m_ResponseMgr->GetNextResponse();
    // OR return Cedrus::Response();
}

void Cedrus::XIDDeviceImpl::ClearResponseQueue()
{
    m_ResponseMgr->ClearResponseQueue();
}

void Cedrus::XIDDeviceImpl::ClearResponsesFromBuffer()
{
    m_xidCon->FlushReadFromDeviceBuffer();
}

void Cedrus::XIDDeviceImpl::SetDigitalOutputLines_RB(boost::shared_ptr<Connection> xidCon, unsigned int lines)
{
    char set_lines_cmd[3];

    set_lines_cmd[0] = 'a';
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = lines & 0x000000ff;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 3, &bytes_written, m_config->NeedsDelay());
}

void Cedrus::XIDDeviceImpl::SetDigitalOutputLines_ST(boost::shared_ptr<Connection> xidCon, unsigned int lines)
{
    unsigned int mask = lines;
    char set_lines_cmd[4];

    set_lines_cmd[0] = 'm';
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written, m_config->NeedsDelay());
}
