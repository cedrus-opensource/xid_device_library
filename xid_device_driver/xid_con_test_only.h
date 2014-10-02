#ifndef XID_CON_TEST_ONLY_H
#define XID_CON_TEST_ONLY_H

#include "interface_xid_con.h"
#include "XidDriverImpExpDefs.h"

#include <sstream>

namespace cedrus
{
    class CEDRUS_XIDDRIVER_IMPORTEXPORT xid_con_test_only : public interface_xid_con
    {
    public:
        xid_con_test_only( char * input_buffer, int size );

        bool read( unsigned char *in_buffer, int bytes_to_read, int *bytes_read);

    private:
        std::stringstream m_input_buffer;
    };
} // namespace cedrus

#endif // XID_CON_TEST_ONLY_H
