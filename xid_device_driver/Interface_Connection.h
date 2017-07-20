#ifndef INTERFACE_XID_CON_T_H
#define INTERFACE_XID_CON_T_H

#include "ftd2xx.h"

namespace Cedrus
{
    class Interface_Connection
    {
    public:
        virtual ~Interface_Connection()
        {}

        virtual bool Read( unsigned char *inBuffer, DWORD bytesToRead, LPDWORD bytesRead) = 0;
    };
} // namespace Cedrus

#endif // INTERFACE_XID_CON_T_H
