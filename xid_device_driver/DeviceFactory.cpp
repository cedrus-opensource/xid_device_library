#include "DeviceFactory.h"

#include "InputDevice.h"
#include "InputDeviceXID2.h"
#include "StimTracker.h"
#include "StimTrackerXID2.h"
#include "DeviceConfig.h"

#include <boost/foreach.hpp>

boost::shared_ptr<cedrus::XIDDevice> cedrus::CreateDevice
(
    const int productID, // d2_value
    const int modelID,   // d3 value
    const int majorFirmwareVersion, // d4 value
    const std::vector< boost::shared_ptr<cedrus::DeviceConfig> > &configCandidates,
    boost::shared_ptr<Connection> xidCon
)
{
    boost::shared_ptr<cedrus::XIDDevice> result;

    // match the product/model id and firmware version to a devconfig
    BOOST_FOREACH(boost::shared_ptr<cedrus::DeviceConfig> const config, configCandidates)
    {
        if (config->DoesConfigMatchDevice(productID, modelID, majorFirmwareVersion))
        {
            // StimTracker or a c-pod
            if (productID == 'S' || static_cast<char>(productID) == '4')
            {
                if (majorFirmwareVersion == 1)
                    result.reset(new StimTracker(xidCon, config));
                else
                    result.reset(new StimTrackerXID2(xidCon, config));
            }
            else
            {
                if (majorFirmwareVersion == 1)
                {
                    result.reset(new InputDevice(xidCon, config));
                }
                else
                {
                    result.reset(new InputDeviceXID2(xidCon, config));
                }
            }

            break;
        }
    }

    return result;
}
