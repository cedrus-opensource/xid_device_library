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

    return outpost_return[0]-'0';
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

void cedrus::xid_glossary::set_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con, int threshold )
{
    int bytes_written;
    char change_threshold_cmd[3];
    change_threshold_cmd[0] = 'l';
    change_threshold_cmd[1] = 't';
    change_threshold_cmd[2] = threshold;

    xid_con->write((unsigned char*)change_threshold_cmd, 3, bytes_written);
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

void cedrus::xid_glossary::set_pulse_duration( boost::shared_ptr<xid_con_t> xid_con, unsigned int duration )
{
    union {
        unsigned int as_int;
        unsigned char as_char[8];
    } dur;

    dur.as_int = duration;

    unsigned char command[10];
    command[0] = 'm';
    command[1] = 'p';
    
    for(int i = 2; i < 10; ++i)
    {
        command[i] = dur.as_char[i-2];
    }

    int written = 0;
    xid_con->write(
        command,
        10,
        written);
}

int cedrus::xid_glossary::get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con )
{
    char return_info[4];
    xid_con->send_xid_command(
        "_a1",
        return_info,
        sizeof(return_info));

    return return_info[3]-'0';
}

void cedrus::xid_glossary::set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, int mode )
{
    int bytes_written;
    std::ostringstream s;
    s << "a1" << mode;

    xid_con->write((unsigned char*)s.str().c_str(), s.str().length(), bytes_written);
}
