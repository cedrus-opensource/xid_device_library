#ifndef xid_device_scanner_helper_win_H
#define xid_device_scanner_helper_win_H

#include <vector>
#include <string>
#include <sstream>

#include "xid_con_t.h"
#include "constants.h"

namespace cedrus
{
    void load_com_ports_platform_specific( std::vector<std::string> * available_com_ports )
    {
        available_com_ports->clear();

        for(int i=1; i < MAX_PORTS; ++i)
        {
            std::string port_name;
            std::ostringstream s;
            s << "COM" << i;
            port_name = s.str().c_str();

            cedrus::xid_con_t conn(port_name);

            if(conn.open() == XID_NO_ERR)
            {
                conn.close();
                available_com_ports->push_back(port_name);
            }
        }
    }
} // namespace cedrus

#endif // xid_device_scanner_helper_win_H
