
#include "CommandGlossary.h"

#include "Connection.h"
#include "constants.h"
#include "CedrusAssert.h"

#include <string.h>
#include <sstream>

#include <boost/algorithm/string/predicate.hpp>

namespace
{
    // if we decide that other classes can use this, then i will re-declare it in the header
    void adjust_endianness_uint_to_chars
    (
     const unsigned int the_int,
     unsigned char* c1,
     unsigned char* c2,
     unsigned char* c3,
     unsigned char* c4
    )
    {
        unsigned int num = the_int;

        *c1 = num & 0x000000ff;
        num >>= 8;
        *c2 = num & 0x000000ff;
        num >>= 8;
        *c3 = num & 0x000000ff;
        num >>= 8;
        *c4 = num & 0x000000ff;
    }
}

unsigned int cedrus::CommandGlossary::AdjustEndiannessCharsToUint
(
 const char c1,
 const char c2,
 const char c3,
 const char c4
)
{
    unsigned int result = 0;

    result |= (c4 & 0x000000ff );
    result <<= 8;
    result |= (c3 & 0x000000ff );
    result <<= 8;
    result |= (c2 & 0x000000ff );
    result <<= 8;
    result |= (c1 & 0x000000ff );

    return result;
}

int cedrus::XIDGlossaryPSTProof::GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon )
{
    return cedrus::CommandGlossary::GetMajorFirmwareVersion(xidCon, true);
}

std::string cedrus::XIDGlossaryPSTProof::GetProtocol( boost::shared_ptr<Connection> xidCon )
{
    return cedrus::CommandGlossary::GetProtocol( xidCon, true );
}

void cedrus::XIDGlossaryPSTProof::GetProductAndModelID(boost::shared_ptr<Connection> xidCon, int *productID, int *modelID )
{
    return cedrus::CommandGlossary::GetProductAndModelID( xidCon, productID, modelID, true );
}

void cedrus::CommandGlossary::ResetBaseTimer( boost::shared_ptr<Connection> xidCon )
{
    DWORD bytes_written;
    xidCon->Write((unsigned char*)"e1", 2, &bytes_written);
}

unsigned int cedrus::CommandGlossary::QueryBaseTimer( boost::shared_ptr<Connection> xidCon )
{
    unsigned int base_timer = 0;
    unsigned char return_info[6];
    int read = xidCon->SendXIDCommand(
        "e3",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( boost::starts_with( return_info , "e3" ), "QueryBaseTimer xid query result must start with e3" );
    bool valid_response = (read == 6);

    if(valid_response)
    {
        base_timer = AdjustEndiannessCharsToUint
            ( return_info[2],
              return_info[3],
              return_info[4],
              return_info[5] );
    }

    return base_timer;
}

void cedrus::CommandGlossary::ResetRtTimer( boost::shared_ptr<Connection> xidCon )
{
    DWORD bytes_written;
    xidCon->Write((unsigned char*)"e5", 2, &bytes_written);
}

std::string cedrus::CommandGlossary::GetInternalProductName( boost::shared_ptr<Connection> xidCon )
{
    unsigned char return_info[100];
    xidCon->SendXIDCommand_Slow(
        "_d1",
        return_info,
        sizeof(return_info));

    std::string return_name((char*)return_info);

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

void cedrus::CommandGlossary::GetProductAndModelID(boost::shared_ptr<Connection> xidCon, int *productID, int *modelID, bool pstProof )
{
    unsigned char product_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer

    int bytes_stored;

    if ( pstProof )
    {
        xidCon->FlushReadFromDeviceBuffer();

        bytes_stored = xidCon->SendXIDCommand_PST_Proof(
            "_d2",
            product_id_return,
            sizeof(product_id_return));
    }
    else
    {
        bytes_stored = xidCon->SendXIDCommand(
            "_d2",
            product_id_return,
            sizeof(product_id_return));
    }

    CEDRUS_ASSERT( bytes_stored >= 1 || product_id_return[0] == 0,
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our product_id_return buffer" );

    // the preceding assertion makes sure we don't read GARBAGE from product_id_return[0] here:
    *productID = (int)(product_id_return[0]);

    CEDRUS_ASSERT( *productID >= 48 && *productID <= 52 || *productID == 83, "_d2 command's result value must be between '0' and '4', or be 'S'" );

    // Model IDs are meaningless for non-RB devices
    unsigned char model_id_return[1]; // we rely on SendXIDCommand to zero-initialize this buffer
    int bytes_count;

    if ( pstProof )
    {
        bytes_count = xidCon->SendXIDCommand_PST_Proof(
            "_d3",
            model_id_return,
            sizeof(model_id_return));
    }
    else
    {
        bytes_count = xidCon->SendXIDCommand(
            "_d3",
            model_id_return,
            sizeof(model_id_return));
    }

    CEDRUS_ASSERT( bytes_count >= 1 || model_id_return[0] == 0,
        "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    *modelID = (int)(model_id_return[0]);

    CEDRUS_ASSERT( *modelID >= 48 && *modelID <= 103, "_d3 command's result value must be between '0' and 'g'" );
}

void cedrus::CommandGlossary::SetModelID(boost::shared_ptr<Connection> xidCon, unsigned char model)
{
    char set_model_cmd[3];

    set_model_cmd[0] = 'd';
    set_model_cmd[1] = '3';
    set_model_cmd[2] = model;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_model_cmd, 3, &bytes_written);
}

int cedrus::CommandGlossary::GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon )
{
    return cedrus::CommandGlossary::GetMajorFirmwareVersion(xidCon, false);
}

int cedrus::CommandGlossary::GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon, bool pstProof )
{
    unsigned char major_return[1];

    if( pstProof )
    {
        xidCon->FlushReadFromDeviceBuffer();

        xidCon->SendXIDCommand_PST_Proof(
            "_d4",
            major_return,
            sizeof(major_return));
    }
    else
    {
        xidCon->SendXIDCommand_Slow(
            "_d4",
            major_return,
            sizeof(major_return));
    }

    bool return_valid = (major_return[0] >= 48 && major_return[0] <= 50);

    CEDRUS_ASSERT( return_valid, "_d4 command's result value must be between '0' and '2'" );

    return return_valid ? major_return[0]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::CommandGlossary::GetMinorFirmwareVersion( boost::shared_ptr<Connection> xidCon )
{
    unsigned char minor_return[1];

    xidCon->SendXIDCommand_Slow(
        "_d5",
        minor_return,
        sizeof(minor_return));

    bool return_valid = minor_return[0] >= 48;

    CEDRUS_ASSERT( return_valid, "_d5 command's result value must be '0' or greater" );

    return return_valid ? minor_return[0]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::CommandGlossary::GetOutpostModel( boost::shared_ptr<Connection> xidCon )
{
    unsigned char outpost_return[1];

    xidCon->SendXIDCommand(
        "_d6",
        outpost_return,
        sizeof(outpost_return));

    bool return_valid = (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x';

    CEDRUS_ASSERT( return_valid, "_d6 command's result value must be between '0' and '4' or be 'x'" );

    return return_valid ? (int)(outpost_return[0]) : INVALID_RETURN_VALUE;
}

int cedrus::CommandGlossary::GetHardwareGeneration( boost::shared_ptr<Connection> xidCon )
{
    unsigned char gen_return[1];

    xidCon->SendXIDCommand(
        "_d7",
        gen_return,
        sizeof(gen_return));

    return gen_return[0]-'0';
}

int cedrus::CommandGlossary::GetLightSensorMode( boost::shared_ptr<Connection> xidCon )
{
    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    const DWORD bytes_count = xidCon->SendXIDCommand(
        "_lr",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid_lr = boost::starts_with( return_info , "_lr" );
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT( return_valid_lr, "GetLightSensorMode xid query result must start with _lr" );
    CEDRUS_ASSERT( return_valid_val, "GetLightSensorMode's return value must be between '0' and '3'" );

    return (return_valid_lr && return_valid_val) ? return_info[3]-'0' : INVALID_RETURN_VALUE;
}

void cedrus::CommandGlossary::SetLightSensorMode( boost::shared_ptr<Connection> xidCon, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char change_mode_cmd[3];
    change_mode_cmd[0] = 'l';
    change_mode_cmd[1] = 'r';
    change_mode_cmd[2] = mode+'0';

    xidCon->Write(change_mode_cmd, 3, &bytes_written);
}

int cedrus::CommandGlossary::GetLightSensorThreshold( boost::shared_ptr<Connection> xidCon )
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand(
        "_lt",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_lt" );

    CEDRUS_ASSERT( return_valid, "GetLightSensorThreshold xid query result must start with _lt" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::CommandGlossary::SetLightSensorThreshold( boost::shared_ptr<Connection> xidCon, unsigned char threshold )
{
    DWORD bytes_written;
    unsigned char change_threshold_cmd[3];
    change_threshold_cmd[0] = 'l';
    change_threshold_cmd[1] = 't';
    change_threshold_cmd[2] = threshold;

    xidCon->Write(change_threshold_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::SetScannerTriggerFilter( boost::shared_ptr<Connection> xidCon, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_trigger_slice_filter_cmd[3];
    set_trigger_slice_filter_cmd[0] = 't';
    set_trigger_slice_filter_cmd[1] = 's';
    set_trigger_slice_filter_cmd[2] = mode;

    xidCon->Write(set_trigger_slice_filter_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::SetDigitalOutputLines_RB ( boost::shared_ptr<Connection> xidCon, unsigned int lines )
{
    char set_lines_cmd[3];

    set_lines_cmd[0] = XID_COMMAND_PREFIX;
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = lines & 0x000000ff;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::SetDigitalOutputLines_ST ( boost::shared_ptr<Connection> xidCon, unsigned int lines )
{
    unsigned int mask = lines;
    char set_lines_cmd[4];

    set_lines_cmd[0] = ST_COMMAND_PREFIX;
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_lines_cmd, 4, &bytes_written);
}

int cedrus::CommandGlossary::GetNumberOfLines(boost::shared_ptr<Connection> xidCon)
{
    unsigned char gen_return[4];

    xidCon->SendXIDCommand(
        "_ml",
        gen_return,
        sizeof(gen_return));

    return gen_return[3];
}

void cedrus::CommandGlossary::SetNumberOfLines(boost::shared_ptr<Connection> xidCon, unsigned int lines)
{
    unsigned char set_number_of_lines_cmd[3];

    set_number_of_lines_cmd[0] = 'm';
    set_number_of_lines_cmd[1] = 'l';
    set_number_of_lines_cmd[2] = lines;

    DWORD bytes_written;
    xidCon->Write((unsigned char*)set_number_of_lines_cmd, 3, &bytes_written);
}

std::string cedrus::CommandGlossary::GetProtocol( boost::shared_ptr<Connection> xidCon, bool pstProof )
{
    // There's a possibility that the device is in E-Prime mode. Right
    // now is the only time the library cares about it, and we need to
    // do some PST-proofing. To start, flush everything to remove the
    // potential spew of zeroes.
    unsigned char return_info[5]; // we rely on SendXIDCommand to zero-initialize this buffer
    int bytes_count;

    if ( pstProof )
    {
        xidCon->FlushReadFromDeviceBuffer();

        bytes_count = xidCon->SendXIDCommand_PST_Proof("_c1",
            return_info,
            sizeof(return_info));
    }
    else
    {
        bytes_count = xidCon->SendXIDCommand("_c1",
            return_info,
            sizeof(return_info));
    }

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[4] == 0 ),
                   "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid = boost::starts_with( return_info , "_xid" );

    // It's okay for this to sometimes return nothing at all. That just
    // means we queried an incorrect baud and there's nothing wrong with that.
    CEDRUS_ASSERT( return_valid || return_info[0] == 0,
        "GetProtocol's return value must start with _xid" );

    return return_valid ? std::string((char*)return_info, sizeof(return_info)) : std::string("Invalid Protocol");
}

std::string cedrus::CommandGlossary::GetProtocol( boost::shared_ptr<Connection> xidCon )
{
    return cedrus::CommandGlossary::GetProtocol(xidCon, false );
}

void cedrus::CommandGlossary::SetProtocol( boost::shared_ptr<Connection> xidCon, unsigned char protocol )
{
    DWORD bytes_written;
    unsigned char set_device_protocol_cmd[3];
    set_device_protocol_cmd[0] = 'c';
    set_device_protocol_cmd[1] = '1';
    set_device_protocol_cmd[2] = protocol+'0';

    xidCon->Write(set_device_protocol_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::SetBaudRate( boost::shared_ptr<Connection> xidCon, unsigned char rate )
{
    DWORD bytes_written;
    unsigned char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    xidCon->Write(change_baud_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::GetProductAndModelID(boost::shared_ptr<Connection> xidCon, int *productID, int *modelID )
{
    return cedrus::CommandGlossary::GetProductAndModelID(xidCon, productID, modelID, true );
}

unsigned int cedrus::CommandGlossary::GetPulseDuration( boost::shared_ptr<Connection> xidCon )
{
    unsigned char return_info[7];
    xidCon->SendXIDCommand(
        "_mp",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( boost::starts_with( return_info , "_mp" ), "GetPulseDuration's return value must start with _mp" );

    unsigned int dur = AdjustEndiannessCharsToUint
        ( return_info[3],
          return_info[4],
          return_info[5],
          return_info[6] );

    return dur;
}

void cedrus::CommandGlossary::SetPulseDuration( boost::shared_ptr<Connection> xidCon, unsigned int duration )
{
    unsigned char command[6];
    command[0] = 'm';
    command[1] = 'p';

    adjust_endianness_uint_to_chars
        ( duration,
          &(command[2]),
          &(command[3]),
          &(command[4]),
          &(command[5]) );

    DWORD written = 0;
    xidCon->Write(
        command,
        6,
        &written);
}

int cedrus::CommandGlossary::GetAccessoryConnectorMode( boost::shared_ptr<Connection> xidCon )
{
    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    xidCon->SendXIDCommand(
        "_a1",
        return_info,
        sizeof(return_info));

    bool return_valid_a1 = boost::starts_with( return_info , "_a1" );
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT( return_valid_a1, "GetAccessoryConnectorMode's return value must start with _a1" );
    CEDRUS_ASSERT( return_valid_val, "GetAccessoryConnectorMode's return value must be between '0' and '3'" );

    return (return_valid_a1 && return_valid_val) ? return_info[3]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::CommandGlossary::GetAccessoryConnectorDevice( boost::shared_ptr<Connection> xidCon )
{
    unsigned char return_info[4]; // we rely on SendXIDCommand to zero-initialize this buffer
    const DWORD bytes_count = xidCon->SendXIDCommand_Slow(
        "_aa",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where SendXIDCommand neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid_aa = boost::starts_with( return_info , "_aa" );
    bool return_valid_val = return_info[3] >= 0 && return_info[3] <= 127;

    CEDRUS_ASSERT( return_valid_aa, "GetAccessoryConnectorDevice's xid query result must start with _aa" );
    CEDRUS_ASSERT( return_valid_val, "GetAccessoryConnectorDevice's return value must be between 0 and 64" );

    return (return_valid_aa && return_valid_val) ?
        static_cast<int>( return_info[3] )
        : static_cast<int>( INVALID_RETURN_VALUE );
}

int cedrus::CommandGlossary::GetOutputLogic( boost::shared_ptr<Connection> xidCon )
{
    unsigned char output_logic[4];

    xidCon->SendXIDCommand(
        "_a0",
        output_logic,
        sizeof(output_logic));

    return output_logic[3]-'0';
}

void cedrus::CommandGlossary::SetOutputLogic( boost::shared_ptr<Connection> xidCon, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_output_logic_cmd[3];
    set_output_logic_cmd[0] = 'a';
    set_output_logic_cmd[1] = '0';
    set_output_logic_cmd[2] = mode+'0';

    xidCon->Write(set_output_logic_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::SetAccessoryConnectorMode( boost::shared_ptr<Connection> xidCon, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_accessory_connector_mode_cmd[3];
    set_accessory_connector_mode_cmd[0] = 'a';
    set_accessory_connector_mode_cmd[1] = '1';
    set_accessory_connector_mode_cmd[2] = mode + '0';

    xidCon->Write(set_accessory_connector_mode_cmd, 3, &bytes_written);
}

int cedrus::CommandGlossary::GetVKDropDelay( boost::shared_ptr<Connection> xidCon )
{
    unsigned char vk_drop_delay[4];

    xidCon->SendXIDCommand(
        "_b3",
        vk_drop_delay,
        sizeof(vk_drop_delay));

    return vk_drop_delay[3];
}

void cedrus::CommandGlossary::SetVKDropDelay( boost::shared_ptr<Connection> xidCon, unsigned char delay )
{
    DWORD bytes_written;
    unsigned char set_vk_drop_delay_cmd[3];
    set_vk_drop_delay_cmd[0] = 'b';
    set_vk_drop_delay_cmd[1] = '3';
    set_vk_drop_delay_cmd[2] = delay;

    xidCon->Write(set_vk_drop_delay_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::ReprogramFlash(boost::shared_ptr<Connection> xidCon)
{
    DWORD bytes_written;
    xidCon->Write((unsigned char*)"f3", 2, &bytes_written);
}

// This function's intended return is essentially a boolean. However, that prevents us from
// verifying that the query succeeded, which is why it's an int instead.
int cedrus::CommandGlossary::GetTriggerDefault( boost::shared_ptr<Connection> xidCon )
{
    unsigned char default_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand_Slow(
        "_f4",
        default_return,
        sizeof(default_return));

    bool return_valid_f4 = boost::starts_with( default_return , "_f4" );
    bool return_valid_val = default_return[3] == '0' || default_return[3] == '1';

    CEDRUS_ASSERT( return_valid_f4, "GetTriggerDefault's xid query result must start with _f4" );
    CEDRUS_ASSERT( return_valid_val, "GetTriggerDefault's value must be either '0' or '1'" );

    return (return_valid_f4 && return_valid_val) ?
        static_cast<int>(default_return[3] == '1')
        : static_cast<int>( INVALID_RETURN_VALUE );
}

void cedrus::CommandGlossary::SetTriggerDefault( boost::shared_ptr<Connection> xidCon, bool defaultOn )
{
    DWORD bytes_written;
    unsigned char set_trigger_default_cmd[3];
    set_trigger_default_cmd[0] = 'f';
    set_trigger_default_cmd[1] = '4';
    set_trigger_default_cmd[2] = (unsigned char)defaultOn + '0';

    xidCon->Write(set_trigger_default_cmd, 3, &bytes_written);
}

int cedrus::CommandGlossary::GetTriggerDebounceTime( boost::shared_ptr<Connection> xidCon )
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand(
        "_f5",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_f5" );

    CEDRUS_ASSERT( return_valid, "GetTriggerDebounceTime's xid query result must start with _f5" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::CommandGlossary::SetTriggerDebounceTime( boost::shared_ptr<Connection> xidCon, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '5';
    set_debouncing_time_cmd[2] = time;

    xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written);
}

int cedrus::CommandGlossary::GetButtonDebounceTime( boost::shared_ptr<Connection> xidCon )
{
    unsigned char threshold_return[4]; // we rely on SendXIDCommand to zero-initialize this buffer

    xidCon->SendXIDCommand(
        "_f6",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_f6" );

    CEDRUS_ASSERT( return_valid, "GetButtonDebounceTime's xid query result must start with _f6" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::CommandGlossary::SetButtonDebounceTime( boost::shared_ptr<Connection> xidCon, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::RestoreFactoryDefaults( boost::shared_ptr<Connection> xidCon )
{
    DWORD bytes_written;
    xidCon->Write((unsigned char*)"f7", 2, &bytes_written);
}

int cedrus::CommandGlossary::GetACDebouncingTime( boost::shared_ptr<Connection> xidCon )
{
    unsigned char threshold_return[4];

    xidCon->SendXIDCommand(
        "_a6",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_a6" );

    CEDRUS_ASSERT( return_valid, "GetACDebouncingTime's xid query result must start with _a6" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::CommandGlossary::SetACDebouncingTime( boost::shared_ptr<Connection> xidCon, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'a';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xidCon->Write(set_debouncing_time_cmd, 3, &bytes_written);
}

void cedrus::CommandGlossary::ConnectToMpod(boost::shared_ptr<Connection> xidCon, unsigned int mpod, unsigned int action)
{
    DWORD bytes_written;
    unsigned char connect_to_mpod_cmd[4];
    connect_to_mpod_cmd[0] = 'a';
    connect_to_mpod_cmd[1] = 'q';
    connect_to_mpod_cmd[2] = '0' + mpod;
    connect_to_mpod_cmd[3] = '0' + action;

    xidCon->Write(connect_to_mpod_cmd, 4, &bytes_written);
}
