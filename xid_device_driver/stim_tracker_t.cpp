#include "stim_tracker_t.h"
#include "xid_con_t.h"

cedrus::stim_tracker_t::stim_tracker_t(
            boost::shared_ptr<xid_con_t> xid_con,
            const std::string &devconfig_path)
    : base_device_t(xid_con, devconfig_path)
{
    clear_lines(255);
}


cedrus::stim_tracker_t::~stim_tracker_t(void)
{
}

void cedrus::stim_tracker_t::raise_lines(
    unsigned int lines_bitmask,
    bool leave_remaining_lines)
{
    xid_con_->set_digital_output_lines(
        lines_bitmask, leave_remaining_lines);
}

void cedrus::stim_tracker_t::clear_lines(
    unsigned int lines_bitmask,
    bool leave_remaining_lines)
{
    xid_con_->clear_digital_output_lines(
        lines_bitmask, leave_remaining_lines);
}

void cedrus::stim_tracker_t::set_pulse_duration(unsigned int duration)
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
    xid_con_->write(
        command,
        10,
        written);
}
