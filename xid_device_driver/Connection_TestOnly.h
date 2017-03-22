#ifndef XID_CON_TEST_ONLY_H
#define XID_CON_TEST_ONLY_H

#include "Interface_Connection.h"
#include "XidDriverImpExpDefs.h"

#include <sstream>
#include <boost/noncopyable.hpp>

namespace cedrus
{
    class  Connection_TestOnly : public Interface_Connection, private boost::noncopyable
    {
    public:
        CEDRUS_XIDDRIVER_IMPORTEXPORT Connection_TestOnly( char * inputBuffer, int size );

        CEDRUS_XIDDRIVER_IMPORTEXPORT bool Read( unsigned char *inBuffer, DWORD bytesToRead, LPDWORD bytesRead);
        CEDRUS_XIDDRIVER_IMPORTEXPORT void InsertMoreDataIntoBuffer( char * inputBuffer, int size );

    private:
        std::stringstream m_InputBuffer;
    };
} // namespace cedrus

#endif // XID_CON_TEST_ONLY_H
