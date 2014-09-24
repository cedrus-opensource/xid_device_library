
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

void cedrus::xid_glossary::reset_rt_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    int bytes_written;
    xid_con->write((unsigned char*)"e5", 2, &bytes_written);
}

void cedrus::xid_glossary::reset_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    int bytes_written;
    xid_con->write((unsigned char*)"e1", 2, &bytes_written);
}

int cedrus::xid_glossary::query_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[6];
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

std::string cedrus::xid_glossary::get_internal_product_name( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[100];
    xid_con->send_xid_command_slow(
        "_d1",
        return_info,
        sizeof(return_info));
    return std::string(return_info);
}

void cedrus::xid_glossary::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id, bool pst_proof )
{
    char product_id_return[1]; // we rely on send_xid_command to zero-initialize this buffer

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
    if ( *product_id == PRODUCT_ID_RB )
    {
        char model_id_return[1]; // we rely on send_xid_command to zero-initialize this buffer
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

        CEDRUS_ASSERT( *model_id >= 49 && *model_id <= 52, "_d3 command's result value must be between '1' and '4'" );
    }
    else
        *model_id = 0;
}

int cedrus::xid_glossary::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_major_firmware_version(xid_con, false);
}

int cedrus::xid_glossary::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof )
{
    char major_return[1];

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

    CEDRUS_ASSERT( major_return[0] >= 48 && major_return[0] <= 50, "_d4 command's result value must be between '0' and '2'" );

    return major_return[0]-'0';
}

int cedrus::xid_glossary::get_minor_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    char minor_return[1];

    xid_con->send_xid_command_slow(
        "_d5",
        minor_return,
        sizeof(minor_return));

    CEDRUS_ASSERT( minor_return[0] >= 48, "_d5 command's result value must be '0' or greater" );

    return minor_return[0]-'0';
}

int cedrus::xid_glossary::get_outpost_model( boost::shared_ptr<xid_con_t> xid_con )
{
    char outpost_return[1];

    xid_con->send_xid_command(
        "_d6",
        outpost_return,
        sizeof(outpost_return));

    CEDRUS_ASSERT( (outpost_return[0] >= 48 && outpost_return[0] <= 52) || outpost_return[0] == 'x', "_d6 command's result value must be between '0' and '4' or be 'x'" );

    return (int)(outpost_return[0]);
}

int cedrus::xid_glossary::get_hardware_generation( boost::shared_ptr<xid_con_t> xid_con )
{
    char gen_return[1];

    xid_con->send_xid_command(
        "_d7",
        gen_return,
        sizeof(gen_return));

    return gen_return[0]-'0';
}

int cedrus::xid_glossary::get_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    const int bytes_count = xid_con->send_xid_command(
        "_lr",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    CEDRUS_ASSERT( boost::starts_with( return_info , "_lr" ), "get_light_sensor_mode xid query result must start with _lr" );
    CEDRUS_ASSERT( return_info[3] >= 48 && return_info[3] <= 51, "get_light_sensor_mode's return value must be between '0' and '3'" );

    return return_info[3]-'0';
}

void cedrus::xid_glossary::set_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    char change_mode_cmd[3];
    change_mode_cmd[0] = 'l';
    change_mode_cmd[1] = 'r';
    change_mode_cmd[2] = mode+'0';

    xid_con->write((unsigned char*)change_mode_cmd, 3, &bytes_written);
}

int cedrus::xid_glossary::get_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_lt",
        threshold_return,
        sizeof(threshold_return));

    CEDRUS_ASSERT( boost::starts_with( threshold_return , "_lt" ), "get_light_sensor_threshold xid query result must start with _lt" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return (int)(return_val);
}

void cedrus::xid_glossary::set_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con, int threshold )
{
    int bytes_written;
    char change_threshold_cmd[3];
    change_threshold_cmd[0] = 'l';
    change_threshold_cmd[1] = 't';
    change_threshold_cmd[2] = threshold;

    xid_con->write((unsigned char*)change_threshold_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_scanner_trigger_filter( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    char set_trigger_slice_filter_cmd[3];
    set_trigger_slice_filter_cmd[0] = 't';
    set_trigger_slice_filter_cmd[1] = 's';
    set_trigger_slice_filter_cmd[2] = mode;

    xid_con->write((unsigned char*)set_trigger_slice_filter_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::set_digital_output_lines_xid (
    boost::shared_ptr<xid_con_t> xid_con,
    unsigned int lines)
{
    set_digital_output_lines(xid_con, lines, XID_COMMAND_PREFIX);
}

void cedrus::xid_glossary::set_digital_output_lines_st (
    boost::shared_ptr<xid_con_t> xid_con,
    unsigned int lines)
{
    set_digital_output_lines(xid_con, lines, ST_COMMAND_PREFIX);
}

void cedrus::xid_glossary::set_digital_output_lines(
    boost::shared_ptr<xid_con_t> xid_con,
    unsigned int lines,
    char product_specific_char)
{
    if(lines > 255)
        return;

    char set_lines_cmd[4];

    set_lines_cmd[0] = product_specific_char;
    set_lines_cmd[1] = 'h';
    set_lines_cmd[2] = lines;
    set_lines_cmd[3] = '\0';

    int bytes_written;
    xid_con->write((unsigned char*)set_lines_cmd, 4, &bytes_written);
}

std::string cedrus::xid_glossary::get_device_protocol( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof )
{
    // There's a possibility that the device is in E-Prime mode. Right
    // now is the only time the library cares about it, and we need to
    // do some PST-proofing. To start, flush everything to remove the
    // potential spew of zeroes.
    char return_info[5]; // we rely on send_xid_command to zero-initialize this buffer
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

    // It's okay for this to sometimes return nothing at all. That just
    // means we queried an incorrect baud and there's nothing wrong with that.
    CEDRUS_ASSERT( boost::starts_with( return_info , "_xid" ) || return_info[0] == 0,
        "get_device_protocol's return value must start with _xid" );

    return std::string(return_info, sizeof(return_info));
}

std::string cedrus::xid_glossary::get_device_protocol( boost::shared_ptr<xid_con_t> xid_con )
{
    return cedrus::xid_glossary::get_device_protocol(xid_con, false );
}

void cedrus::xid_glossary::set_device_protocol( boost::shared_ptr<xid_con_t> xid_con, int protocol )
{
    std::ostringstream s;
    s << "c1" << protocol;
    int bytes_written;

    xid_con->write((unsigned char*)s.str().c_str(), s.str().length(), &bytes_written);
}

void cedrus::xid_glossary::set_device_baud_rate( boost::shared_ptr<xid_con_t> xid_con, int rate )
{
    int bytes_written;
    char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    xid_con->write((unsigned char*)change_baud_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id )
{
    return cedrus::xid_glossary::get_product_and_model_id(xid_con, product_id, model_id, true );
}

unsigned int cedrus::xid_glossary::get_pulse_duration( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[7];
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

    int written = 0;
    xid_con->write(
        command,
        6,
        &written);
}

int cedrus::xid_glossary::get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    xid_con->send_xid_command(
        "_a1",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( boost::starts_with( return_info , "_a1" ), "get_accessory_connector_mode's return value must start with _a1" );
    CEDRUS_ASSERT( return_info[3] >= 48 && return_info[3] <= 51, "get_accessory_connector_device's return value must be between '0' and '3'" );

    return return_info[3]-'0';
}

int cedrus::xid_glossary::get_accessory_connector_device( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[4]; // we rely on send_xid_command to zero-initialize this buffer
    const int bytes_count = xid_con->send_xid_command_slow(
        "_aa",
        return_info,
        sizeof(return_info));

    CEDRUS_ASSERT( bytes_count >= 1 || ( return_info[0] == 0 && return_info[3] == 0 ),
                   "in the case where send_xid_command neglected to store ANY BYTES, we are relying on a GUARANTEE that \
it will at least zero our buffer" );

    CEDRUS_ASSERT( boost::starts_with( return_info , "_aa" ), "get_accessory_connector_device's xid query result must start with _aa" );
    CEDRUS_ASSERT( return_info[3] >= 0 && return_info[3] <= 64, "get_accessory_connector_device's return value must be between 0 and 64" );

    return return_info[3];
}

void cedrus::xid_glossary::set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    std::ostringstream s;
    s << "a1" << mode;

    xid_con->write((unsigned char*)s.str().c_str(), s.str().length(), &bytes_written);
}

int cedrus::xid_glossary::get_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_f5",
        threshold_return,
        sizeof(threshold_return));

    CEDRUS_ASSERT( boost::starts_with( threshold_return , "_f5" ), "get_trigger_debounce_time's xid query result must start with _f5" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return (int)(return_val);
}

void cedrus::xid_glossary::set_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con, int time )
{
    int bytes_written;
    char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '5';
    set_debouncing_time_cmd[2] = time;

    xid_con->write((unsigned char*)set_debouncing_time_cmd, 3, &bytes_written);
}

int cedrus::xid_glossary::get_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[4]; // we rely on send_xid_command to zero-initialize this buffer

    xid_con->send_xid_command(
        "_f6",
        threshold_return,
        sizeof(threshold_return));

    CEDRUS_ASSERT( boost::starts_with( threshold_return , "_f6" ), "get_button_debounce_time's xid query result must start with _f6" );

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return (int)(return_val);
}

void cedrus::xid_glossary::set_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con, int time )
{
    int bytes_written;
    char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'f';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xid_con->write((unsigned char*)set_debouncing_time_cmd, 3, &bytes_written);
}

void cedrus::xid_glossary::restore_factory_defaults( boost::shared_ptr<xid_con_t> xid_con )
{
    int bytes_written;
    xid_con->write((unsigned char*)"f7", 2, &bytes_written);
}

int cedrus::xid_glossary::get_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[4];

    xid_con->send_xid_command(
        "_a6",
        threshold_return,
        sizeof(threshold_return));

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return (int)(return_val);
}

void cedrus::xid_glossary::set_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con, int time )
{
    int bytes_written;
    char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 'a';
    set_debouncing_time_cmd[1] = '6';
    set_debouncing_time_cmd[2] = time;

    xid_con->write((unsigned char*)set_debouncing_time_cmd, 3, &bytes_written);
}
