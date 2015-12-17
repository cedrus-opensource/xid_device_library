#ifndef INTERFACE_XID_CON_T_H
#define INTERFACE_XID_CON_T_H

#include "ftd2xx.h"

namespace cedrus
{
    class interface_xid_con
    {
    public:
        virtual ~interface_xid_con()
        {}

        virtual bool read( unsigned char *in_buffer, DWORD bytes_to_read, LPDWORD bytes_read ) = 0;
    };
} // namespace cedrus

#endif // INTERFACE_XID_CON_T_H
