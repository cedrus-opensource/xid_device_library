#ifndef INTERFACE_XID_CON_T_H
#define INTERFACE_XID_CON_T_H

namespace cedrus
{
    class interface_xid_con
    {
    public:
        virtual bool read( unsigned char *in_buffer, int bytes_to_read, int *bytes_read) = 0;

    };
} // namespace cedrus

#endif // INTERFACE_XID_CON_T_H
