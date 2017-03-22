#ifndef DEVICE_FACTORY_H
#define DEVICE_FACTORY_H

#include <boost/shared_ptr.hpp>
#include <vector>

namespace cedrus
{
    class XIDDevice;
    class DeviceConfig;
    class Connection;

    boost::shared_ptr<cedrus::XIDDevice> CreateDevice(
        int productID,
        int modelID,
        int major_firmware_version,
        const std::vector< boost::shared_ptr<cedrus::DeviceConfig> > &configCandidates,
        boost::shared_ptr<Connection> xidCon);
}

#endif // #define DEVICE_FACTORY_H
