#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "DeviceConfig.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

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
int DetectXIDDevices(Cedrus::XIDDeviceScanner& self,
                     boost::python::object reportFunction,
                     boost::python::object progressFunction)
{
    return self.DetectXIDDevices(reportFunction, progressFunction);
}

/*
Wrap XIDDeviceScanner::DetectXIDDevices() without paramters 
*/
int DetectXIDDevicesDef(Cedrus::XIDDeviceScanner& self)
{
    return self.DetectXIDDevices();
}

} // anonymous namespace

/*
boost::get_pointer() specialization for Cedrus::XIDDevice const volatile *
to avoid VC 14.0 link error.
*/
namespace boost {

template <>
Cedrus::XIDDevice const volatile* get_pointer(class Cedrus::XIDDevice const volatile* bd)
{
    return bd;
}

} // namespace boost

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(XIDDevice_Overloads, RaiseLines, 1, 2)

BOOST_PYTHON_MODULE( pyxid )
{
    #if defined(PY_MEMBER_FUNCTION) || defined(class_name) || defined(mem_fun_name) 
    #error "PY_MEMBER_FUNCTION macro redefinition"
    #endif

    #define PY_MEMBER_FUNCTION(class_name, mem_fun_name) #mem_fun_name, &class_name::mem_fun_name

    namespace py = boost::python;
    using namespace Cedrus;

    // Expose std::vector<int> for DevicePort::keyMap.
    py::class_<std::vector<int> >("KeyVec")
        .def(py::vector_indexing_suite<std::vector<int> >())
    ;

    // Expose struct DevicePort.
    py::class_<DevicePort>("DevicePort")
        .def(py::init<>())
        .def(py::init<DevicePort>())
        .def_readwrite(PY_MEMBER_FUNCTION(DevicePort, portName))
        .def_readwrite(PY_MEMBER_FUNCTION(DevicePort, portNumber))
        .def_readwrite(PY_MEMBER_FUNCTION(DevicePort, numberOfLines))
        .def_readwrite(PY_MEMBER_FUNCTION(DevicePort, keyMap))
    ; 

    py::register_ptr_to_python<DevicePort*>();

    // Expose std::map<unsigned int, DevicePort> for return value of DeviceConfig::GetMapOfPorts()
    py::class_<std::map<unsigned int, DevicePort> >("DevicePortMap")
        .def(py::map_indexing_suite<std::map<unsigned int, DevicePort> >())
    ;

    py::register_ptr_to_python< std::map<unsigned int, DevicePort>* >();

    // Expose struct Response.
    py::class_<Response>("Response")
        .def(py::init<>())
        .def(py::init<Response>())
        .add_property(PY_MEMBER_FUNCTION(Response, port))
        .add_property(PY_MEMBER_FUNCTION(Response, key))
        .add_property(PY_MEMBER_FUNCTION(Response, wasPressed))
        .add_property(PY_MEMBER_FUNCTION(Response, reactionTime))
        ;

    // Expose struct SignalFilter.
    py::class_<SignalFilter>("SignalFilter")
        .def(py::init<>())
        .def(py::init<SignalFilter>())
        .add_property(PY_MEMBER_FUNCTION(SignalFilter, holdOn))
        .add_property(PY_MEMBER_FUNCTION(SignalFilter, holdOff))
        ;

    // Expose struct SingleShotMode.
    py::class_<SingleShotMode>("SingleShotMode")
        .def(py::init<>())
        .def(py::init<SingleShotMode>())
        .add_property(PY_MEMBER_FUNCTION(SingleShotMode, enabled))
        .add_property(PY_MEMBER_FUNCTION(SingleShotMode, delay))
        ;

    // Expose class ResponseManager.
    py::class_<ResponseManager, boost::noncopyable>("ResponseManager",
                                                    py::init<int, std::shared_ptr<const DeviceConfig> >())
        .def(PY_MEMBER_FUNCTION(ResponseManager, CheckForKeypress), py::args("portConnection"))
        .def(PY_MEMBER_FUNCTION(ResponseManager, HasQueuedResponses))
        .def(PY_MEMBER_FUNCTION(ResponseManager, GetNextResponse))
        .def(PY_MEMBER_FUNCTION(ResponseManager, GetNumberOfKeysDown))
        .def(PY_MEMBER_FUNCTION(ResponseManager, ClearResponseQueue))
    ;
    py::scope().attr("INVALID_PORT_BITS") = static_cast<int>(ResponseManager::INVALID_PORT_BITS);

    // Expose class DeviceConfig.
    py::class_<DeviceConfig, std::shared_ptr<DeviceConfig>, boost::noncopyable >("DeviceConfig", py::no_init)
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetMappedKey), py::args("port", "key"))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetDeviceName))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetProductID))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetModelID))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetMajorVersion))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, GetMapOfPorts),
            py::return_value_policy<py::reference_existing_object>())
        .def(PY_MEMBER_FUNCTION(DeviceConfig, IsStimTracker2))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, IsRBx40))
        .def(PY_MEMBER_FUNCTION(DeviceConfig, IsXID2))

        .def("GetPortPtrByNumber", &DeviceConfig::GetPortPtrByNumber,
            py::arg("portNum"),
            py::return_value_policy<py::reference_existing_object>())

        .def(PY_MEMBER_FUNCTION(DeviceConfig, DoesConfigMatchDevice),
            py::args("deviceId, modelID, majorFirmwareVer "))
        ;

    py::register_ptr_to_python< std::shared_ptr<const DeviceConfig> >();

    // Expose class XIDDevice.
    py::class_<XIDDevice, std::shared_ptr<XIDDevice>, boost::noncopyable >("XIDDevice", py::no_init)
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetOutputLogic)) // _a0
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetOutputLogic), py::arg("mode")) // a0
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetAccessoryConnectorMode)) // _a1
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetAccessoryConnectorMode), py::args("mode")) // a1
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetACDebouncingTime)) // _a6
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetACDebouncingTime), py::args("time")) // a6
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetMpodModel), py::arg("mpod")) // _aq
        .def(PY_MEMBER_FUNCTION(XIDDevice, ConnectToMpod), py::args("mpod, action")) // aq
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetVKDropDelay)) // _b3
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetVKDropDelay), py::arg("delay")) // b3
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetProtocol)) // _c1
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetProtocol), py::arg("protocol"))
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetCombinedInfo)) // _d0
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetInternalProductName)) // _d1
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetProductID)) // _d2
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetModelID)) // _d3
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetModelID), py::arg("model")) // d3
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetMajorFirmwareVersion)) // _d4
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetMinorFirmwareVersion))// _d5
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetOutpostModel)) // _d6
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetHardwareGeneration)) // _d7
        .def(PY_MEMBER_FUNCTION(XIDDevice, ResetBaseTimer)) // e1
        .def(PY_MEMBER_FUNCTION(XIDDevice, QueryBaseTimer)) // e3
        // _e5
        .def(PY_MEMBER_FUNCTION(XIDDevice, ResetRtTimer)) // e5
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetBaudRate))
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetBaudRate), py::arg("rate")) // f1
        .def(PY_MEMBER_FUNCTION(XIDDevice, ReprogramFlash)) // f3
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetTriggerDefault)) // _f4
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetTriggerDefault), py::arg("defaultOn")) // f4
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetTriggerDebounceTime)) // _f5
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetTriggerDebounceTime), py::arg("time")) // f5
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetButtonDebounceTime)) // _f6
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetButtonDebounceTime), py::arg("time")) // f6
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetSingleShotMode), py::arg("selector")) // _ia
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetSingleShotMode), py::args("selector", "mode", "delay")) // ia
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetSignalFilter), py::arg("selector")) // _if
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetSignalFilter), py::args("selector", "mode", "delay")) // if
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetTimerResetOnOnsetMode), py::arg("selector")) // _ir
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetTimerResetOnOnsetMode), py::args("selector", "mode")) // ir
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetEnableUSBOutput), py::arg("selector")) // _iu
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetEnableUSBOutput), py::args("selector", "mode")) // iu
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetAnalogInputThreshold), py::arg("selector")) // _it
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetAnalogInputThreshold), py::args("selector", "threshold")) // it
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetDeviceConfig))
        .def(PY_MEMBER_FUNCTION(XIDDevice, OpenConnection))
        .def(PY_MEMBER_FUNCTION(XIDDevice, CloseConnection))
        .def(PY_MEMBER_FUNCTION(XIDDevice, HasLostConnection))
        .def(PY_MEMBER_FUNCTION(XIDDevice, RaiseLines), XIDDevice_Overloads()) // mh (ah/mh for XID 1)
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetPulseDuration)) // _mp
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetPulseDuration), py::arg("duration")) // mp
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetNumberOfLines)) // _ml
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetNumberOfLines), py::arg("lines")) // ml
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetPulseTableBitMask)) // _mk
        .def(PY_MEMBER_FUNCTION(XIDDevice, SetPulseTableBitMask), py::arg("lines")) // mk
        .def(PY_MEMBER_FUNCTION(XIDDevice, ClearPulseTable)) // mc
        .def(PY_MEMBER_FUNCTION(XIDDevice, IsPulseTableRunning)) // _mr
        .def(PY_MEMBER_FUNCTION(XIDDevice, RunPulseTable)) // mr
        .def(PY_MEMBER_FUNCTION(XIDDevice, StopPulseTable)) // ms
        .def(PY_MEMBER_FUNCTION(XIDDevice, AddPulseTableEntry), py::args("time", "lines")) // mt
        .def(PY_MEMBER_FUNCTION(XIDDevice, ResetOutputLines)) // mz
        .def(PY_MEMBER_FUNCTION(XIDDevice, ClearLines))
        .def(PY_MEMBER_FUNCTION(XIDDevice, RestoreFactoryDefaults)) // f7
        .def(PY_MEMBER_FUNCTION(XIDDevice, PollForResponse))
        .def(PY_MEMBER_FUNCTION(XIDDevice, HasQueuedResponses))
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetNumberOfKeysDown))
        .def(PY_MEMBER_FUNCTION(XIDDevice, ClearResponseQueue))
        .def(PY_MEMBER_FUNCTION(XIDDevice, GetNextResponse))
        .def(PY_MEMBER_FUNCTION(XIDDevice, ClearResponsesFromBuffer))
        ;

    // Expose class XIDDeviceScanner.
    py::class_<XIDDeviceScanner>("XIDDeviceScanner", py::no_init)
        //.def(PY_MEMBER_FUNCTION(XIDDeviceScanner, GetDeviceScanner))
        .def("GetDeviceScanner", &XIDDeviceScanner::GetDeviceScanner, py::return_value_policy<py::reference_existing_object>())
        .staticmethod("GetDeviceScanner")
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, CloseAllConnections))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, OpenAllConnections))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DropEveryConnection))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DropConnectionByPtr), py::arg("device"))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, CheckConnectionsDropDeadOnes))
        .def("DetectXIDDevices", &DetectXIDDevices, py::args("reportFunction", "progressFunction"))
        .def("DetectXIDDevices", &DetectXIDDevicesDef)
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DeviceConnectionAtIndex), py::arg("index"))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DeviceCount))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DevconfigAtIndex), py::arg("index"))
        .def(PY_MEMBER_FUNCTION(XIDDeviceScanner, DevconfigCount))
    ;

    #undef PY_MEMBER_FUNCTION

} // BOOST_PYTHON_MODULE
