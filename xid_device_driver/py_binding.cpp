#include "xid_device_scanner_t.h"
#include "base_device_t.h"
#include "interface_xid_con.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


namespace {

/*
xid_device_scanner::detect_valid_xid_devices has optional parameters:
    reportFunction is for reporting errors during the scanning process
    progressFunction is for reporting progress on a 0-100 scale.

Wrappers for both versions - with and without parameters are needed
to expose this member function to Python.
*/

/*
Wrap xid_device_scanner::detect_valid_xid_devices( boost::function< void(std::string) >,
                                                   boost::function< bool(unsigned int) > )
allowing pass python function as parameters
*/
int detect_valid_xid_devices( cedrus::xid_device_scanner_t& self,
                              boost::python::object reportFunction,
                              boost::python::object progressFunction )
{
    return self.detect_valid_xid_devices( reportFunction, progressFunction );
}

/*
Wrap xid_device_scanner::detect_valid_xid_devices() without paramters 
*/
int detect_valid_xid_devices_def( cedrus::xid_device_scanner_t& self )
{
    return self.detect_valid_xid_devices();
}

} // anonymous namespace

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( base_device_t_overloads, raise_lines, 1, 2 )


BOOST_PYTHON_MODULE( xid )
{
    #if defined(PY_MEMBER_FUNCTION) || defined(class_name) || defined(mem_fun_name) 
    #error "PY_MEMBER_FUNCTION macro redefinition"
    #endif

    #define PY_MEMBER_FUNCTION(class_name, mem_fun_name) #mem_fun_name, &class_name::mem_fun_name

    namespace py = boost::python;
    using namespace cedrus;

    // Expose std::vector<int> for device_port::key_map.
    py::class_<std::vector<int> >( "key_vec" )
        .def( py::vector_indexing_suite<std::vector<int> >() )
    ;

    // Expose struct device_port.
    py::class_<device_port>( "device_port" )
        .def( py::init<>() )
        .def( py::init<device_port>() )
        .def_readwrite( PY_MEMBER_FUNCTION(device_port, port_name) )
        .def_readwrite( PY_MEMBER_FUNCTION(device_port, port_number) )
        .def_readwrite( PY_MEMBER_FUNCTION(device_port, number_of_lines) )
        .def_readwrite( PY_MEMBER_FUNCTION(device_port, key_map) )
        .def_readwrite( PY_MEMBER_FUNCTION(device_port, is_response_port) )
    ; 

    py::register_ptr_to_python<device_port*>();

    // Expose std::vector<device_port> for return value of xid_device_config_t::get_vector_of_ports()
    py::class_<std::vector<device_port> >( "device_port_vec" )
        .def(py::vector_indexing_suite<std::vector<device_port> >())
    ;

    py::register_ptr_to_python< std::vector<device_port>* >();

    // Expose struct response.
    py::class_<response>( "response" )
        .def(py::init<>())
        .def(py::init<response>())
        .add_property( PY_MEMBER_FUNCTION( response, port) )
        .add_property( PY_MEMBER_FUNCTION( response, key) )
        .add_property( PY_MEMBER_FUNCTION( response, was_pressed) )
        .add_property( PY_MEMBER_FUNCTION( response, reaction_time) )
    ;

    // Expose interface_xid_con.
    py::class_<interface_xid_con, boost::noncopyable>( "interface_xid_con", py::no_init )
        .def( "read", &interface_xid_con::read, py::args( "in_buffer, bytes_to_read, bytes_read" ))
    ;

    // Expose class response_mgr.
    py::class_<response_mgr, boost::noncopyable>( "response_mgr",
                                                  py::init<int , boost::shared_ptr<const xid_device_config_t> >() )
        .def( PY_MEMBER_FUNCTION(response_mgr, check_for_keypress), py::args( "port_connection, dev_config" ) )
        .def( PY_MEMBER_FUNCTION(response_mgr, has_queued_responses) )
        .def( PY_MEMBER_FUNCTION(response_mgr, get_next_response) )
        .def( PY_MEMBER_FUNCTION(response_mgr, get_number_of_keys_down) )
        .def( PY_MEMBER_FUNCTION(response_mgr, clear_response_queue) )
    ;
    py::scope().attr("INVALID_PORT_BITS") = static_cast<int>(response_mgr::INVALID_PORT_BITS);

    // Expose interface read_only_device.
    py::class_<read_only_device, boost::noncopyable>( "read_only_device", py::no_init )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_hardware_generation) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_light_sensor_mode) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_light_sensor_threshold) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_pulse_duration) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_accessory_connector_mode) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_accessory_connector_device) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_outpost_model) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_device_config) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_baud_rate) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_product_and_model_id), py::args( "product_id, model_id" ) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_major_firmware_version) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_minor_firmware_version) )
        .def( PY_MEMBER_FUNCTION(read_only_device, get_internal_product_name) )
    ;

    // Expose struct base_device_t::product_and_model_id_t.
    py::class_<base_device_t::product_and_model_id>("product_and_model_id")
        .add_property("product_id", &base_device_t::product_and_model_id::product_id)
        .add_property("model_id", &base_device_t::product_and_model_id::model_id)
    ;

    // Expose class base_device_t.

    // Define pointer to member function base_device_t::*get_product_and_model_id which returns 
    // product and model IDs to expose it  to Python.
    // See base_device_t::get_product_and_model_id() exposing below.
    base_device_t::product_and_model_id (base_device_t::*get_product_and_model_id)() = &base_device_t::get_product_and_model_id;

    py::class_<base_device_t, py::bases<read_only_device>, boost::shared_ptr<base_device_t>, boost::noncopyable >( "base_device_t", py::no_init )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_outpost_model) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_hardware_generation) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_light_sensor_mode) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_light_sensor_mode), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_light_sensor_threshold), py::arg( "threshold" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_light_sensor_threshold) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_scanner_trigger_filter), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, reset_rt_timer) )
        .def( PY_MEMBER_FUNCTION(base_device_t, reset_base_timer) )
        .def( PY_MEMBER_FUNCTION(base_device_t, query_base_timer) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_pulse_duration) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_pulse_duration), py::arg( "duration" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, poll_for_response) )
        .def( PY_MEMBER_FUNCTION(base_device_t, has_queued_responses) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_number_of_keys_down) )
        .def( PY_MEMBER_FUNCTION(base_device_t, clear_response_queue) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_next_response) )
        .def( PY_MEMBER_FUNCTION(base_device_t, clear_responses) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_accessory_connector_mode) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_accessory_connector_device) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_output_logic), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_accessory_connector_mode), py::args( "mode" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_vk_drop_delay), py::arg( "delay" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_trigger_default) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_trigger_default), py::arg( "default_on" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_trigger_debounce_time) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_trigger_debounce_time), py::arg( "time" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_button_debounce_time) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_button_debounce_time), py::arg( "time" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_device_protocol), py::arg( "protocol" ) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_device_config) )
        .def( PY_MEMBER_FUNCTION(base_device_t, open_connection) )
        .def( PY_MEMBER_FUNCTION(base_device_t, close_connection) )
        .def( PY_MEMBER_FUNCTION(base_device_t, has_lost_connection) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_baud_rate) )
        .def( PY_MEMBER_FUNCTION(base_device_t, set_baud_rate), py::arg( "rate" ) )
        .def( "get_product_and_model_id", get_product_and_model_id)
        .def( PY_MEMBER_FUNCTION(base_device_t, get_major_firmware_version) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_minor_firmware_version) )
        .def( PY_MEMBER_FUNCTION(base_device_t, get_internal_product_name) )
        .def( PY_MEMBER_FUNCTION(base_device_t, raise_lines), base_device_t_overloads() )
        .def( PY_MEMBER_FUNCTION(base_device_t, clear_lines) )
        .def( PY_MEMBER_FUNCTION(base_device_t, restore_factory_defaults) )
    ;

    // Expose class xid_device_config_t.
    py::class_<xid_device_config_t, boost::shared_ptr<xid_device_config_t>,
               boost::noncopyable >( "xid_device_config", py::no_init )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_mapped_key), py::args("port", "key") )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_device_name) )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_product_id) )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_model_id) )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_num_lines_on_port), py::arg( "port" ) )
        .def( PY_MEMBER_FUNCTION(xid_device_config_t, get_vector_of_ports),
                                 py::return_value_policy<py::reference_existing_object>() )

        .def("get_port_by_index", &xid_device_config_t::get_port_ptr_by_index,
                                   py::arg( "port_num" ),
                                   py::return_value_policy<py::reference_existing_object>() )

        .def( PY_MEMBER_FUNCTION(xid_device_config_t, does_config_match_device),
                                 py::args( "device_id, model_id, major_firmware_ver ") )
    ;

    py::register_ptr_to_python< boost::shared_ptr<const xid_device_config_t> >();

    // Expose class xid_device_scanner_t.
    py::class_<xid_device_scanner_t>( "xid_device_scanner", py::init<std::string const &>() )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, close_all_connections) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, open_all_connections) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, drop_every_connection) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, drop_connection_by_ptr), py::arg( "device" ) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, check_connections_drop_dead_ones) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, read_in_devconfigs), py::arg( "config_file_location" ) )
        .def( "detect_valid_xid_devices", &detect_valid_xid_devices, py::args( "report_function", "progress_function" ) )
        .def( "detect_valid_xid_devices", &detect_valid_xid_devices_def)
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, device_connection_at_index), py::arg( "index" ) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, device_count) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, devconfig_at_index), py::arg( "index" ) )
        .def( PY_MEMBER_FUNCTION(xid_device_scanner_t, devconfig_count) )
    ;

    #undef PY_MEMBER_FUNCTION

} // BOOST_PYTHON_MODULE
