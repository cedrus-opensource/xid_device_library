#ifndef xid_device_scanner_helper_win_H
#define xid_device_scanner_helper_win_H

#include <vector>
#include <string>
#include <sstream>

#include "xid_con_t.h"
#include "constants.h"

#include "ftd2xx.h"

namespace cedrus
{

void load_com_ports_platform_specific( std::vector<DWORD> * available_com_ports )
{
    available_com_ports->clear();

    FT_STATUS status;
    FT_DEVICE_LIST_INFO_NODE * dev_info;
    DWORD num_devs;

    // create the device information list
    status = FT_CreateDeviceInfoList(&num_devs);


    if (num_devs > 0)
    {
        // allocate storage for list based on numDevs
        dev_info = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*num_devs);
        // get the device information list
        status = FT_GetDeviceInfoList(dev_info, &num_devs);
    }

    if ( status == FT_OK )
    {
        for ( unsigned int i = 0; i < num_devs; i++ )
        {
            cedrus::xid_con_t conn(dev_info[i].LocId);

            if(conn.open() == XID_NO_ERR)
            {
                conn.close();
                available_com_ports->push_back(dev_info[i].LocId);
            }
        }
    }

    if ( num_devs > 0 )
        free(dev_info);
}

} // namespace cedrus

#endif // xid_device_scanner_helper_win_H
