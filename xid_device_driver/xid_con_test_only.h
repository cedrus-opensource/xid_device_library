#ifndef XID_CON_TEST_ONLY_H
#define XID_CON_TEST_ONLY_H

#include "interface_xid_con.h"
#include "XidDriverImpExpDefs.h"

#include <sstream>

namespace cedrus
{
    class  xid_con_test_only : public interface_xid_con
    {
    public:
        CEDRUS_XIDDRIVER_IMPORTEXPORT xid_con_test_only( char * input_buffer, int size );

        CEDRUS_XIDDRIVER_IMPORTEXPORT bool read( unsigned char *in_buffer, int bytes_to_read, int *bytes_read);
        CEDRUS_XIDDRIVER_IMPORTEXPORT void insert_more_data_into_buffer( char * input_buffer, int size );

    private:
        std::stringstream m_input_buffer;
    };
} // namespace cedrus

#endif // XID_CON_TEST_ONLY_H
