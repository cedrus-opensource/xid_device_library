

#ifndef XID_GLOSSARY_H
#define XID_GLOSSARY_H

#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class xid_con_t;

    class xid_glossary
    {
    public:
        /**
        * Resets the internal device reaction time timer.
        *
        * This should be called when a stimulus is presented
        */
        static void reset_rt_timer(boost::shared_ptr<xid_con_t> xid_con);

        /**
        * Resets the device's base timer
        *
        * This should be called when the device is initialized or an experiment
        * starts
        */
        static void reset_base_timer(boost::shared_ptr<xid_con_t> xid_con);

        static int query_base_timer(boost::shared_ptr<xid_con_t> xid_con);

        static std::string get_internal_product_name(boost::shared_ptr<xid_con_t> xid_con);

        static int get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con );

        static int get_minor_firmware_version( boost::shared_ptr<xid_con_t> xid_con );

        // _d6 XID 2.0 only.
        static int get_outpost_model( boost::shared_ptr<xid_con_t> xid_con );

        // _d7 XID 2.0 only
        static int get_hardware_generation( boost::shared_ptr<xid_con_t> xid_con );

        // _lr XID 2.0 only
        static int get_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con );

        // lr XID 2.0 only
        static void set_light_sensor_mode( boost::shared_ptr<xid_con_t> xid_con, int mode );

        // _lt XID 2.0 only
        static int get_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con );

        // lt XID 2.0 only
        static void set_light_sensor_threshold( boost::shared_ptr<xid_con_t> xid_con, int threshold );

        static void set_scanner_trigger_filter( boost::shared_ptr<xid_con_t> xid_con, int mode );

        static void set_digital_output_lines_xid( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines );

        static void set_digital_output_lines_st( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines );

        static void set_device_mode( boost::shared_ptr<xid_con_t> xid_con, int protocol );

        static void set_device_baud_rate( boost::shared_ptr<xid_con_t> xid_con, int rate );

        static void get_product_and_model_id( boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id );

        static unsigned int get_pulse_duration( boost::shared_ptr<xid_con_t> xid_con );

        static void set_pulse_duration( boost::shared_ptr<xid_con_t> xid_con, unsigned int duration );

        // _a1 deprecated in XID 2.0
        static int get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con );

        // _aa XID 2.0 only.
        static int get_accessory_connector_device( boost::shared_ptr<xid_con_t> xid_con );

        // a1 deprecated in XID 2.0
        static void set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, int mode );

        static int get_debouncing_time( boost::shared_ptr<xid_con_t> xid_con );

        static void set_debouncing_time( boost::shared_ptr<xid_con_t> xid_con, int time );

    private:
        /**
        * Raise digital output lines on the StimTracker device.
        *
        * @param[in] lines_bitmask This is a bitmask used to specify the lines
        * to be raised on the device. Each of the 8 bits in the integer
        * specifies a line.  If bits 0 and 7 are 1, lines 1 and 8 are raised.
        * @param[in] leave_remaining_lines boolean value of whether or not to
        * keep the current line state when applying the new bitmask.
        */
        static void set_digital_output_lines( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines, char product_specific_prefix );

        // A small subset of commands differs slightly between "true" xid devices and StimTracker
        static const char XID_COMMAND_PREFIX = 'a';
        static const char ST_COMMAND_PREFIX = 'm';
    };
}
#endif // XID_GLOSSARY_H
