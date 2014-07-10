
#ifndef device_factory_H
#define device_factory_H

#include <boost/shared_ptr.hpp>
#include <vector>

namespace cedrus
{
    class base_device_t;
    class xid_device_config_t;
    class xid_con_t;

    boost::shared_ptr<cedrus::base_device_t>
    create_device( int product_id,
                   int model_id,
                   int major_firmware_version,
                   const std::vector< boost::shared_ptr<cedrus::xid_device_config_t> > &config_candidates,
                   boost::shared_ptr<xid_con_t> xid_con );
}

#endif // #define device_factory_H
