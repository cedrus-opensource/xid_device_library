

#ifndef XID_GLOSSARY_H
#define XID_GLOSSARY_H

#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class xid_con_t;

    class xid_glossary_pst_proof
    {
    public:
        static int get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con );

        static std::string get_device_protocol( boost::shared_ptr<xid_con_t> xid_con );

        static void get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id );
    };

    class xid_glossary
    {
    public:
        /**
        * Resets the device's base timer
        *
        * This should be called when the device is initialized or an experiment
        * starts
        */
        // e1
        static void reset_base_timer(boost::shared_ptr<xid_con_t> xid_con);

        // e3
        static int query_base_timer(boost::shared_ptr<xid_con_t> xid_con);

        /**
        * Resets the internal device reaction time timer.
        *
        * This should be called when a stimulus is presented
        */
        // e5
        static void reset_rt_timer(boost::shared_ptr<xid_con_t> xid_con);

        static unsigned int adjust_endianness_chars_to_uint
        ( const char c1,
          const char c2,
          const char c3,
          const char c4 );

        // _d1
        static std::string get_internal_product_name(boost::shared_ptr<xid_con_t> xid_con);

        // _d2 and _d3
        static void get_product_and_model_id( boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id );

        // _d4
        static int get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con );

        // _d5
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

        // ts XID 2.0 only
        static void set_scanner_trigger_filter( boost::shared_ptr<xid_con_t> xid_con, int mode );

        // ah
        static void set_digital_output_lines_xid( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines );

        // mh
        static void set_digital_output_lines_st( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines );

        // _c1
        static std::string get_device_protocol( boost::shared_ptr<xid_con_t> xid_con );

        // c1
        static void set_device_protocol( boost::shared_ptr<xid_con_t> xid_con, int protocol );

        // f1
        static void set_device_baud_rate( boost::shared_ptr<xid_con_t> xid_con, int rate );

        // _mp
        static unsigned int get_pulse_duration( boost::shared_ptr<xid_con_t> xid_con );

        // mp
        static void set_pulse_duration( boost::shared_ptr<xid_con_t> xid_con, unsigned int duration );

        // _a1 deprecated in XID 2.0
        static int get_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con );

        // _aa XID 2.0 only.
        static int get_accessory_connector_device( boost::shared_ptr<xid_con_t> xid_con );

        // a0 deprecated in XID 2.0
        static void set_output_logic( boost::shared_ptr<xid_con_t> xid_con, int mode );

        // a1 deprecated in XID 2.0
        static void set_accessory_connector_mode( boost::shared_ptr<xid_con_t> xid_con, int mode );

        // _f4
        static int get_trigger_default( boost::shared_ptr<xid_con_t> xid_con );

        // f4
        static void set_trigger_default( boost::shared_ptr<xid_con_t> xid_con, bool default_on );

        // _f5
        static int get_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con );

        // f5
        static void set_trigger_debounce_time( boost::shared_ptr<xid_con_t> xid_con, int time );

        // _f6
        static int get_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con );

        // f6
        static void set_button_debounce_time( boost::shared_ptr<xid_con_t> xid_con, int time );

        // f7
        static void restore_factory_defaults( boost::shared_ptr<xid_con_t> xid_con );

        // _a6
        static int get_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con );

        // a6
        static void set_ac_debouncing_time( boost::shared_ptr<xid_con_t> xid_con, int time );

private:
        // ah or mh
        static void set_digital_output_lines( boost::shared_ptr<xid_con_t> xid_con, unsigned int lines, char product_specific_prefix );

        // _d4
        static int get_major_firmware_version( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof );

        // _c1
        static std::string get_device_protocol( boost::shared_ptr<xid_con_t> xid_con, bool pst_proof );

        // _d2 and _d3
        static void get_product_and_model_id(boost::shared_ptr<xid_con_t> xid_con, int *product_id, int *model_id, bool pst_proof );

        // A small subset of commands differs slightly between "true" xid devices and StimTracker
        static const char XID_COMMAND_PREFIX = 'a';
        static const char ST_COMMAND_PREFIX = 'm';

        // For commands that have pst-proof implementations.
        friend class xid_glossary_pst_proof;
    };
}
#endif // XID_GLOSSARY_H
