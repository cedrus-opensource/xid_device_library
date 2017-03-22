#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "Interface_Connection.h"
#include "DeviceConfig.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


namespace {

/*
XIDDeviceScanner::DetectXIDDevices has optional parameters:
    reportFunction is for reporting errors during the scanning process
    progressFunction is for reporting progress on a 0-100 scale.

Wrappers for both versions - with and without parameters are needed
to expose this member function to Python.
*/

/*
Wrap XIDDeviceScanner::DetectXIDDevices( boost::function< void(std::string) >,
                                                   boost::function< bool(unsigned int) > )
allowing pass python function as parameters
*/
int DetectXIDDevices( cedrus::XIDDeviceScanner& self,
                              boost::python::object reportFunction,
                              boost::python::object progressFunction )
{
    return self.DetectXIDDevices( reportFunction, progressFunction );
}

/*
Wrap XIDDeviceScanner::DetectXIDDevices() without paramters 
*/
int DetectXIDDevicesDef( cedrus::XIDDeviceScanner& self )
{
    return self.DetectXIDDevices();
}

} // anonymous namespace

/*
boost::get_pointer() specialization for cedrus::XIDDevice const volatile *
to avoid VC 14.0 link error.
*/
namespace boost {

template <>
cedrus::XIDDevice const volatile* get_pointer(class cedrus::XIDDevice const volatile* bd)
{
    return bd;
}

} // namespace boost

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( XIDDevice_Overloads, RaiseLines, 1, 2 )


BOOST_PYTHON_MODULE( xid )
{
    #if defined(PY_MEMBER_FUNCTION) || defined(class_name) || defined(mem_fun_name) 
    #error "PY_MEMBER_FUNCTION macro redefinition"
    #endif

    #define PY_MEMBER_FUNCTION(class_name, mem_fun_name) #mem_fun_name, &class_name::mem_fun_name

    namespace py = boost::python;
    using namespace cedrus;

    // Expose std::vector<int> for DevicePort::keyMap.
    py::class_<std::vector<int> >( "KeyVec" )
        .def( py::vector_indexing_suite<std::vector<int> >() )
    ;

    // Expose struct DevicePort.
    py::class_<DevicePort>( "DevicePort" )
        .def( py::init<>() )
        .def( py::init<DevicePort>() )
        .def_readwrite( PY_MEMBER_FUNCTION(DevicePort, portName) )
        .def_readwrite( PY_MEMBER_FUNCTION(DevicePort, portNumber) )
        .def_readwrite( PY_MEMBER_FUNCTION(DevicePort, numberOfLines) )
        .def_readwrite( PY_MEMBER_FUNCTION(DevicePort, keyMap) )
        .def_readwrite( PY_MEMBER_FUNCTION(DevicePort, isResponsePort) )
    ; 

    py::register_ptr_to_python<DevicePort*>();

    // Expose std::vector<DevicePort> for return value of DeviceConfig::GetVectorOfPorts()
    py::class_<std::vector<DevicePort> >( "DevicePortVec" )
        .def(py::vector_indexing_suite<std::vector<DevicePort> >())
    ;

    py::register_ptr_to_python< std::vector<DevicePort>* >();

    // Expose struct response.
    py::class_<response>( "response" )
        .def(py::init<>())
        .def(py::init<response>())
        .add_property( PY_MEMBER_FUNCTION( response, port) )
        .add_property( PY_MEMBER_FUNCTION( response, key) )
        .add_property( PY_MEMBER_FUNCTION( response, wasPressed) )
        .add_property( PY_MEMBER_FUNCTION( response, reactionTime) )
    ;

    // Expose Interface_Connection.
    py::class_<Interface_Connection, boost::noncopyable>( "Interface_Connection", py::no_init )
        .def( "Read", &Interface_Connection::Read, py::args( "inBuffer, bytesToRead, bytesRead" ))
    ;

    // Expose class ResponseManager.
    py::class_<ResponseManager, boost::noncopyable>( "ResponseManager",
                                                  py::init<int , boost::shared_ptr<const DeviceConfig> >() )
        .def( PY_MEMBER_FUNCTION(ResponseManager, CheckForKeypress), py::args( "portConnection, devConfig" ) )
        .def( PY_MEMBER_FUNCTION(ResponseManager, HasQueuedResponses) )
        .def( PY_MEMBER_FUNCTION(ResponseManager, GetNextResponse) )
        .def( PY_MEMBER_FUNCTION(ResponseManager, GetNumberOfKeysDown) )
        .def( PY_MEMBER_FUNCTION(ResponseManager, ClearResponseQueue) )
    ;
    py::scope().attr("INVALID_PORT_BITS") = static_cast<int>(ResponseManager::INVALID_PORT_BITS);

    // Expose interface XIDDeviceReadOnly.
    py::class_<XIDDeviceReadOnly, boost::noncopyable>( "XIDDeviceReadOnly", py::no_init )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetHardwareGeneration) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetLightSensorMode) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetLightSensorThreshold) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetPulseDuration) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetAccessoryConnectorMode) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetAccessoryConnectorDevice) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetOutpostModel) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetDeviceConfig) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetBaudRate) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetProductAndModelID), py::args( "productID, modelID" ) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetMajorFirmwareVersion) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetMinorFirmwareVersion) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceReadOnly, GetInternalProductName) )
    ;

    // Expose struct XIDDevice::product_and_model_id_t.
    py::class_<XIDDevice::ProductAndModelID>("ProductAndModelID")
        .add_property("productID", &XIDDevice::ProductAndModelID::productID)
        .add_property("modelID", &XIDDevice::ProductAndModelID::modelID)
    ;

    // Expose class XIDDevice.

    // Define pointer to member function XIDDevice::*GetProductAndModelID which returns 
    // product and model IDs to expose it  to Python.
    // See XIDDevice::GetProductAndModelID() exposing below.
    XIDDevice::ProductAndModelID (XIDDevice::*GetProductAndModelID)() = &XIDDevice::GetProductAndModelID;

    py::class_<XIDDevice, py::bases<XIDDeviceReadOnly>, boost::shared_ptr<XIDDevice>, boost::noncopyable >( "XIDDevice", py::no_init )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetOutpostModel) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetHardwareGeneration) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetLightSensorMode) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetLightSensorMode), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetLightSensorThreshold), py::arg( "threshold" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetLightSensorThreshold) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetScannerTriggerFilter), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, ResetRtTimer) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, ResetBaseTimer) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, QueryBaseTimer) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetPulseDuration) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetPulseDuration), py::arg( "duration" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, PollForResponse) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, HasQueuedResponses) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetNumberOfKeysDown) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, ClearResponseQueue) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetNextResponse) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, ClearResponses) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetAccessoryConnectorMode) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetAccessoryConnectorDevice) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetOutputLogic) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetOutputLogic), py::arg( "mode" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetAccessoryConnectorMode), py::args( "mode" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetVKDropDelay) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetVKDropDelay), py::arg( "delay" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetTriggerDefault) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetTriggerDefault), py::arg( "defaultOn" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetTriggerDebounceTime) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetTriggerDebounceTime), py::arg( "time" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetButtonDebounceTime) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetButtonDebounceTime), py::arg( "time" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetProtocol ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetProtocol), py::arg( "protocol" ) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetDeviceConfig) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, OpenConnection) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, CloseConnection) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, HasLostConnection) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetBaudRate) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, SetBaudRate), py::arg( "rate" ) )
        .def( "GetProductAndModelID", GetProductAndModelID)
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetMajorFirmwareVersion) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetMinorFirmwareVersion) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, GetInternalProductName) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, RaiseLines), XIDDevice_Overloads() )
        .def( PY_MEMBER_FUNCTION(XIDDevice, ClearLines) )
        .def( PY_MEMBER_FUNCTION(XIDDevice, RestoreFactoryDefaults) )
    ;

    // Expose class DeviceConfig.
    py::class_<DeviceConfig, boost::shared_ptr<DeviceConfig>,
               boost::noncopyable >( "DeviceConfig", py::no_init )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetMappedKey), py::args("port", "key") )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetDeviceName) )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetProductID) )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetModelID) )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetMajorVersion) )
        .def( PY_MEMBER_FUNCTION(DeviceConfig, GetVectorOfPorts),
                                 py::return_value_policy<py::reference_existing_object>() )

        .def("get_port_by_index", &DeviceConfig::GetPortPtrByIndex,
                                   py::arg( "portNum" ),
                                   py::return_value_policy<py::reference_existing_object>() )

        .def( PY_MEMBER_FUNCTION(DeviceConfig, DoesConfigMatchDevice),
                                 py::args( "deviceId, modelID, majorFirmwareVer ") )
    ;

    py::register_ptr_to_python< boost::shared_ptr<const DeviceConfig> >();

    // Expose class XIDDeviceScanner.
    py::class_<XIDDeviceScanner>( "XIDDeviceScanner", py::init<std::string const &>() )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, CloseAllConnections) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, OpenAllConnections) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DropEveryConnection) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DropConnectionByPtr), py::arg( "device" ) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, CheckConnectionsDropDeadOnes) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, ReadInDevconfigs), py::arg( "configFileLocation" ) )
        .def( "DetectXIDDevices", &DetectXIDDevices, py::args( "reportFunction", "progressFunction" ) )
        .def( "DetectXIDDevices", &DetectXIDDevicesDef)
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DeviceConnectionAtIndex), py::arg( "index" ) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DeviceCount) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DevconfigAtIndex), py::arg( "index" ) )
        .def( PY_MEMBER_FUNCTION(XIDDeviceScanner, DevconfigCount) )
    ;

    #undef PY_MEMBER_FUNCTION

} // BOOST_PYTHON_MODULE
