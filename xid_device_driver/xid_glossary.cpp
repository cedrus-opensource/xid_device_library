#include "xid_glossary.h"
#include "xid_con_t.h"
#include "constants.h"

#include <string.h>
#include <sstream>

void cedrus::xid_glossary::reset_rt_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    int bytes_written;
    xid_con->write((unsigned char*)"e5", 2, bytes_written);
}

void cedrus::xid_glossary::reset_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    int bytes_written;
    xid_con->write((unsigned char*)"e1", 2, bytes_written);
}

int cedrus::xid_glossary::query_base_timer( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[200];
    int read = xid_con->send_xid_command(
        "e3",
        return_info,
        sizeof(return_info));

    bool valid_response = (read == 6);

    if(valid_response)
    {
        union {
            int as_int;
            char as_char[4];
        } rt;

        for(int i = 0; i < 4; ++i)
        {
            rt.as_char[i] = return_info[2+i];
        }
        return rt.as_int;
    }

    return GENERAL_ERROR;
}

std::string cedrus::xid_glossary::get_internal_product_name( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[200];
    xid_con->send_xid_command(
        "_d1",
        return_info,
        sizeof(return_info));
    return std::string(return_info);
}

int cedrus::xid_glossary::get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    char major_return[2];

    xid_con->send_xid_command(
        "_d4",
        major_return,
        sizeof(major_return));

    return major_return[0]-'0';
}

int cedrus::xid_glossary::get_minor_firmware_version( boost::shared_ptr<xid_con_t> xid_con )
{
    char minor_return[2];

    xid_con->send_xid_command(
        "_d5",
        minor_return,
        sizeof(minor_return));

    return minor_return[0]-'0';
}

int cedrus::xid_glossary::get_outpost_model( boost::shared_ptr<xid_con_t> xid_con )
{
    char outpost_return[2];

    xid_con->send_xid_command(
        "_d6",
        outpost_return,
        sizeof(outpost_return));

    return (int)(outpost_return[0]);
}

int cedrus::xid_glossary::get_hardware_generation( boost::shared_ptr<xid_con_t> xid_con )
{
    char gen_return[2];

    xid_con->send_xid_command(
        "_d7",
        gen_return,
        sizeof(gen_return));

    return gen_return[0]-'0';
}

int cedrus::xid_glossary::get_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[5];
    xid_con->send_xid_command(
        "_lr",
        return_info,
        sizeof(return_info));

    return return_info[3]-'0';
}

void cedrus::xid_glossary::set_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    char change_mode_cmd[3];
    change_mode_cmd[0] = 'l';
    change_mode_cmd[1] = 'r';
    change_mode_cmd[2] = mode+'0';

    xid_con->write((unsigned char*)change_mode_cmd, 3, bytes_written);
}

int cedrus::xid_glossary::get_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[5];

    xid_con->send_xid_command(
        "_lt",
        threshold_return,
        sizeof(threshold_return));

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

    xid_con->write((unsigned char*)change_threshold_cmd, 3, bytes_written);
}

void cedrus::xid_glossary::set_scanner_trigger_filter( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    char set_trigger_slice_filter_cmd[3];
    set_trigger_slice_filter_cmd[0] = 't';
    set_trigger_slice_filter_cmd[1] = 's';
    set_trigger_slice_filter_cmd[2] = mode;

    xid_con->write((unsigned char*)set_trigger_slice_filter_cmd, 3, bytes_written);
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
    xid_con->write((unsigned char*)set_lines_cmd, 4, bytes_written);
}

void cedrus::xid_glossary::set_device_mode( boost::shared_ptr<xid_con_t> xid_con, int protocol )
{
    std::ostringstream s;
    s << "c1" << protocol;
    int bytes_written;

    xid_con->write((unsigned char*)s.str().c_str(), s.str().length(), bytes_written);
}

void cedrus::xid_glossary::set_device_baud_rate( boost::shared_ptr<xid_con_t> xid_con, int rate )
{
    int bytes_written;
    char change_baud_cmd[3];
    change_baud_cmd[0] = 'f';
    change_baud_cmd[1] = '1';
    change_baud_cmd[2] = rate;

    xid_con->write((unsigned char*)change_baud_cmd, 3, bytes_written);
}

void cedrus::xid_glossary::get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int &product_id, int &model_id )
{
    char product_id_return[2];
    char model_id_return[2];

    xid_con->send_xid_command(
        "_d2",
        product_id_return,
        sizeof(product_id_return));

    xid_con->send_xid_command(
        "_d3",
        model_id_return,
        sizeof(model_id_return));

    product_id = (int)(product_id_return[0]);
    model_id = (int)(model_id_return[0]);
}

unsigned int cedrus::xid_glossary::get_pulse_duration( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[8];
    xid_con->send_xid_command(
        "_mp",
        return_info,
        sizeof(return_info));

    unsigned int dur = 0;

    dur |= (return_info[6] & 0x000000ff );
    dur <<= 8;
    dur |= (return_info[5] & 0x000000ff );
    dur <<= 8;
    dur |= (return_info[4] & 0x000000ff );
    dur <<= 8;
    dur |= (return_info[3] & 0x000000ff );

    return dur;
}

void cedrus::xid_glossary::set_pulse_duration( boost::shared_ptr<xid_con_t> xid_con, unsigned int duration )
{
    unsigned char command[6];
    command[0] = 'm';
    command[1] = 'p';
    
    command[2] = duration & 0x000000ff;
    duration >>= 8;
    command[3] = duration & 0x000000ff;
    duration >>= 8;
    command[4] = duration & 0x000000ff;
    duration >>= 8;
    command[5] = duration & 0x000000ff;
    duration >>= 8;

    int written = 0;
    xid_con->write(
        command,
        6,
        written);
}

int cedrus::xid_glossary::get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[5];
    xid_con->send_xid_command(
        "_a1",
        return_info,
        sizeof(return_info));

    return return_info[3]-'0';
}

int cedrus::xid_glossary::get_accessory_connector_device( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[5];
    xid_con->send_xid_command(
        "_aa",
        return_info,
        sizeof(return_info));

    return return_info[3];
}

void cedrus::xid_glossary::set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    std::ostringstream s;
    s << "a1" << mode;

    xid_con->write((unsigned char*)s.str().c_str(), s.str().length(), bytes_written);
}

int cedrus::xid_glossary::get_debouncing_time( boost::shared_ptr<xid_con_t> xid_con )
{
    char threshold_return[5];

    xid_con->send_xid_command(
        "_a6",
        threshold_return,
        sizeof(threshold_return));

    unsigned char return_val = (unsigned char)(threshold_return[3]);
    return (int)(return_val);
}

void cedrus::xid_glossary::set_debouncing_time( boost::shared_ptr<xid_con_t> xid_con, int time )
{
    int bytes_written;
    char set_debouncing_time_cmd[3];
    set_debouncing_time_cmd[0] = 't';
    set_debouncing_time_cmd[1] = 's';
    set_debouncing_time_cmd[2] = time;

    xid_con->write((unsigned char*)set_debouncing_time_cmd, 3, bytes_written);
}
