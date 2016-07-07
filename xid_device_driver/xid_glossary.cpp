
#include "xid_glossary.h"

#include "xid_con_t.h"
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

unsigned int cedrus::xid_glossary::adjust_endianness_chars_to_uint
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

int cedrus::xid_glossary_pst_proof::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_major_firmware_version(xid_con, true);
}

std::string cedrus::xid_glossary_pst_proof::get_device_protocol( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_device_protocol( xid_con, true );
}

void cedrus::xid_glossary_pst_proof::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id )
{
    return cedrus::xid_glossary::get_product_and_model_id( xid_con, product_id, model_id, true );
}

void cedrus::xid_glossary::reset_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    DWORD bytes_written;
    xid_con->write((unsigned char*)"e1", 2, &bytes_written);
}

int cedrus::xid_glossary::query_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[6];
    int read = xid_con->send_xid_command(
        "e3",
        return_info,
        sizeof(return_info));

    bool valid_response = (read == 6);

    if(valid_response)
    {
        return adjust_endianness_chars_to_uint
            ( return_info[2],
              return_info[3],
              return_info[4],
              return_info[5] );
    }

    return XID_GENERAL_ERROR;
}

void cedrus::xid_glossary::reset_rt_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    DWORD bytes_written;
    xid_con->write((unsigned char*)"e5", 2, &bytes_written);
}

std::string cedrus::xid_glossary::get_internal_product_name( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[100];
    xid_con->send_xid_command_slow(
        "_d1",
        return_info,
        sizeof(return_info));

    std::string return_name((char*)return_info);

    return return_name.empty() ? std::string("Error Retrieving Name") : return_name;
}

void cedrus::xid_glossary::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id, bool pst_proof )
{
    unsigned char product_id_return[1]; // we rely on send_xid_command to zero-initialize this buffer

    int bytes_stored;

    if ( pst_proof )
    {
        xid_con->flush_read_from_device_buffer();

        bytes_stored = xid_con->send_xid_command_pst_proof(
            "_d2",
            product_id_return,
            sizeof(product_id_return));
    }
    else
    {
        bytes_stored = xid_con->send_xid_command(
            "_d2",
            product_id_return,
            sizeof(product_id_return));
    }

    CEDRUS_ASSERT( bytes_stored >= 1 || product_id_return[0] == 0,
        "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our product_id_return buffer" );

    // the preceding assertion makes sure we don't read GARBAGE from product_id_return[0] here:
    *product_id = (int)(product_id_return[0]);

    CEDRUS_ASSERT( *product_id >= 48 && *product_id <= 50 || *product_id == 83, "_d2 command's result value must be between '0' and '2', or be 'S'" );

    // Model IDs are meaningless for non-RB devices
    unsigned char model_id_return[1]; // we rely on send_xid_command to zero-initialize this buffer
    int bytes_count;

    if ( pst_proof )
    {
        bytes_count = xid_con->send_xid_command_pst_proof(
            "_d3",
            model_id_return,
            sizeof(model_id_return));
    }
    else
    {
        bytes_count = xid_con->send_xid_command(
            "_d3",
            model_id_return,
            sizeof(model_id_return));
    }

    CEDRUS_ASSERT( bytes_count >= 1 || model_id_return[0] == 0,
        "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
        it will at least zero our buffer" );

    *model_id = (int)(model_id_return[0]);

    CEDRUS_ASSERT( *model_id >= 49 && *model_id <= 69, "_d3 command's result value must be between '1' and 'E'" );
}

int cedrus::xid_glossary::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_major_firmware_version(xid_con, false);
}

int cedrus::xid_glossary::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof )
{
    unsigned char major_return[1];

    if( pst_proof )
    {
        xid_con->flush_read_from_device_buffer();

        xid_con->send_xid_command_pst_proof(
            "_d4",
            major_return,
            sizeof(major_return));
    }
    else
    {
        xid_con->send_xid_command_slow(
            "_d4",
            major_return,
            sizeof(major_return));
    }

    bool return_valid = (major_return[0] >= 48 && major_return[0] <= 50);

    CEDRUS_ASSERT( return_valid, "_d4 command's result value must be between '0' and '2'" );

    return return_valid ? major_return[0]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::xid_glossary::get_minor_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char minor_return[1];

    xid_con->send_xid_command_slow(
        "_d5",
        minor_return,
        sizeof(minor_return));

    bool return_valid = minor_return[0] >= 48;

    CEDRUS_ASSERT( return_valid, "_d5 command's result value must be '0' or greater" );

    return return_valid ? minor_return[0]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::xid_glossary::get_outpost_model( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char outpost_return[1];

    xid_con->send_xid_command(
        "_d6",
        outpost_return,
        sizeof(outpost_return));

    bool return_valid = (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x';

    CEDRUS_ASSERT( return_valid, "_d6 command's result value must be between '0' and '4' or be 'x'" );

    return return_valid ? (int)(outpost_return[0]) : INVALID_RETURN_VALUE;
}

int cedrus::xid_glossary::get_hardware_generation( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char gen_return[1];

    xid_con->send_xid_command(
        "_d7",
        gen_return,
        sizeof(gen_return));

    return gen_return[0]-'0';
}

int cedrus::xid_glossary::get_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    const DWORD bytes_count = xid_con->send_xid_command(
        "_lr",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid_lr = boost::starts_with( return_info , "_lr" );
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT( return_valid_lr, "get_light_sensor_mode xid query result must start with _lr" );
    CEDRUS_ASSERT( return_valid_val, "get_light_sensor_mode's return value must be between '0' and '3'" );

    return (return_valid_lr && return_valid_val) ? return_info[3]-'0' : INVALID_RETURN_VALUE;
}

void cedrus::xid_glossary::set_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char change_mode_cmd[3];
    change_mode_cmd[0] = 'l';
    change_mode_cmd[1] = 'r';
    change_mode_cmd[2] = mode+'0';

    xid_con->write(change_mode_cmd, 3, &bytes_written);
}

int cedrus::xid_glossary::get_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_lt",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_lt" );

    CEDRUS_ASSERT( return_valid, "get_light_sensor_threshold xid query result must start with _lt" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::xid_glossary::set_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con, unsigned char threshold )
{
    DWORD bytes_written;
    unsigned char change_threshold_cmd[3];
    change_threshold_cmd[0] = 'l';
    change_threshold_cmd[1] = 't';
    change_threshold_cmd[2] = threshold;

    xid_con->write(change_threshold_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_scanner_trigger_filter( boost::shared_ptr<xid_con_t> xid_con, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_trigger_slice_filter_cmd[3];
    set_trigger_slice_filter_cmd[0] = 't';
    set_trigger_slice_filter_cmd[1] = 's';
    set_trigger_slice_filter_cmd[2] = mode;

    xid_con->write(set_trigger_slice_filter_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_digital_output_lines_xid ( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines )
{
    char set_lines_cmd[3];

    set_lines_cmd[0] = XID_COMMAND_PREFIX;
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = lines & 0x000000ff;

    DWORD bytes_written;
    xid_con->write((unsigned char*)set_lines_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_digital_output_lines_st ( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines )
{
    unsigned int mask = lines;
    char set_lines_cmd[4];

    set_lines_cmd[0] = ST_COMMAND_PREFIX;
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = mask & 0x000000ff;
    mask >>= 8;
    set_lines_cmd[3] = mask & 0x000000ff;

    DWORD bytes_written;
    xid_con->write((unsigned char*)set_lines_cmd, 4, &bytes_written);
}

std::string cedrus::xid_glossary::get_device_protocol( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof )
{
    // There's a possibility that the device is in E-Prime mode. Right
    // now is the only time the library cares about it, and we need to
    // do some PST-proofing. To start, flush everything to remove the
    // potential spew of zeroes.
    unsigned char return_info[5]; // we rely on send_xid_command to zero-initialize this buffer
    int bytes_count;

    if ( pst_proof )
    {
        xid_con->flush_read_from_device_buffer();

        bytes_count = xid_con->send_xid_command_pst_proof("_c1",
            return_info,
            sizeof(return_info));
    }
    else
    {
        bytes_count = xid_con->send_xid_command("_c1",
            return_info,
            sizeof(return_info));
    }

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[4] == 0 ),
                   "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid = boost::starts_with( return_info , "_xid" );

    // It's okay for this to sometimes return nothing at all. That just
    // means we queried an incorrect baud and there's nothing wrong with that.
    CEDRUS_ASSERT( return_valid || return_info[0] == 0,
        "get_device_protocol's return value must start with _xid" );

    return return_valid ? std::string((char*)return_info, sizeof(return_info)) : std::string("Invalid Protocol");
}

std::string cedrus::xid_glossary::get_device_protocol( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_device_protocol(xid_con, false );
}

void cedrus::xid_glossary::set_device_protocol( boost::shared_ptr<xid_con_t> xid_con, unsigned char protocol )
{
    DWORD bytes_written;
    unsigned char set_device_protocol_cmd[3];
    set_device_protocol_cmd[0] = 'c';
    set_device_protocol_cmd[1] = '1';
    set_device_protocol_cmd[2] = protocol;

    xid_con->write(set_device_protocol_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_device_baud_rate( boost::shared_ptr<xid_con_t> xid_con, unsigned char rate )
{
    DWORD bytes_written;
    unsigned char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    xid_con->write(change_baud_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id )
{
    return cedrus::xid_glossary::get_product_and_model_id(xid_con, product_id, model_id, true );
}

unsigned int cedrus::xid_glossary::get_pulse_duration( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[7];
    xid_con->send_xid_command(
        "_mp",
        return_info,
        sizeof(return_info));

    unsigned int dur = adjust_endianness_chars_to_uint
        ( return_info[3],
          return_info[4],
          return_info[5],
          return_info[6] );

    return dur;
}

void cedrus::xid_glossary::set_pulse_duration( boost::shared_ptr<xid_con_t> xid_con, unsigned int duration )
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
    xid_con->write(
        command,
        6,
        &written);
}

int cedrus::xid_glossary::get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    xid_con->send_xid_command(
        "_a1",
        return_info,
        sizeof(return_info));

    bool return_valid_a1 = boost::starts_with( return_info , "_a1" );
    bool return_valid_val = return_info[3] >= 48 && return_info[3] <= 51;

    CEDRUS_ASSERT( return_valid_a1, "get_accessory_connector_mode's return value must start with _a1" );
    CEDRUS_ASSERT( return_valid_val, "get_accessory_connector_mode's return value must be between '0' and '3'" );

    return (return_valid_a1 && return_valid_val) ? return_info[3]-'0' : INVALID_RETURN_VALUE;
}

int cedrus::xid_glossary::get_accessory_connector_device( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    const DWORD bytes_count = xid_con->send_xid_command_slow(
        "_aa",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    bool return_valid_aa = boost::starts_with( return_info , "_aa" );
    bool return_valid_val = return_info[3] >= 0 && return_info[3] <= 64;

    CEDRUS_ASSERT( return_valid_aa, "get_accessory_connector_device's xid query result must start with _aa" );
    CEDRUS_ASSERT( return_valid_val, "get_accessory_connector_device's return value must be between 0 and 64" );

    return (return_valid_aa && return_valid_val) ?
        static_cast<int>( return_info[3] )
        : static_cast<int>( INVALID_RETURN_VALUE );
}

void cedrus::xid_glossary::set_output_logic( boost::shared_ptr<xid_con_t> xid_con, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_output_logic_cmd[3];
    set_output_logic_cmd[0] = 'a';
    set_output_logic_cmd[1] = '0';
    set_output_logic_cmd[2] = mode+'0';

    xid_con->write(set_output_logic_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, unsigned char mode )
{
    DWORD bytes_written;
    unsigned char set_accessory_connector_mode_cmd[3];
    set_accessory_connector_mode_cmd[0] = 'a';
    set_accessory_connector_mode_cmd[1] = '1';
    set_accessory_connector_mode_cmd[2] = mode + '0';

    xid_con->write(set_accessory_connector_mode_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_vk_drop_delay( boost::shared_ptr<xid_con_t> xid_con, unsigned char delay )
{
    DWORD bytes_written;
    unsigned char set_vk_drop_delay_cmd[3];
    set_vk_drop_delay_cmd[0] = 'b';
    set_vk_drop_delay_cmd[1] = '3';
    set_vk_drop_delay_cmd[2] = delay;

    xid_con->write(set_vk_drop_delay_cmd, 3, &bytes_written);
}

// This function's intended return is essentially a boolean. However, that prevents us from
// verifying that the query succeeded, which is why it's an int instead.
int cedrus::xid_glossary::get_trigger_default( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char default_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command_slow(
        "_f4",
        default_return,
        sizeof(default_return));

    bool return_valid_f4 = boost::starts_with( default_return , "_f4" );
    bool return_valid_val = default_return[3] == '0' || default_return[3] == '1';

    CEDRUS_ASSERT( return_valid_f4, "get_trigger_default's xid query result must start with _f4" );
    CEDRUS_ASSERT( return_valid_val, "get_trigger_default's value must be either '0' or '1'" );

    return (return_valid_f4 && return_valid_val) ?
        static_cast<int>(default_return[3] == '1')
        : static_cast<int>( INVALID_RETURN_VALUE );
}

void cedrus::xid_glossary::set_trigger_default( boost::shared_ptr<xid_con_t> xid_con, bool default_on )
{
    DWORD bytes_written;
    unsigned char set_trigger_default_cmd[3];
    set_trigger_default_cmd[0] = 'f';
    set_trigger_default_cmd[1] = '4';
    set_trigger_default_cmd[2] = (unsigned char)default_on + '0';

    xid_con->write(set_trigger_default_cmd, 3, &bytes_written);
}

int cedrus::xid_glossary::get_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_f5",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_f5" );

    CEDRUS_ASSERT( return_valid, "get_trigger_debounce_time's xid query result must start with _f5" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::xid_glossary::set_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '5';
    set_debouncing_time_cmd[2] = time;

    xid_con->write(set_debouncing_time_cmd, 3, &bytes_written);
}

int cedrus::xid_glossary::get_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_f6",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_f6" );

    CEDRUS_ASSERT( return_valid, "get_button_debounce_time's xid query result must start with _f6" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::xid_glossary::set_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xid_con->write(set_debouncing_time_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::restore_factory_defaults( boost::shared_ptr<xid_con_t> xid_con )
{
    DWORD bytes_written;
    xid_con->write((unsigned char*)"f7", 2, &bytes_written);
}

int cedrus::xid_glossary::get_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con )
{
    unsigned char threshold_return[4];

    xid_con->send_xid_command(
        "_a6",
        threshold_return,
        sizeof(threshold_return));

    bool return_valid = boost::starts_with( threshold_return , "_a6" );

    CEDRUS_ASSERT( return_valid, "get_ac_debouncing_time's xid query result must start with _a6" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return return_valid ? (int)(return_val) : INVALID_RETURN_VALUE;
}

void cedrus::xid_glossary::set_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con, unsigned char time )
{
    DWORD bytes_written;
    unsigned char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'a';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xid_con->write(set_debouncing_time_cmd, 3, &bytes_written);
}
