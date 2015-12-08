#include "xid_con_test_only.h"

cedrus::xid_con_test_only::xid_con_test_only( char * input_buffer, int size )
{
   insert_more_data_into_buffer(input_buffer, size);
}

bool cedrus::xid_con_test_only::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    DWORD *bytes_read)
{
    m_input_buffer.read((char*)in_buffer, bytes_to_read);
    
    *bytes_read = (int)m_input_buffer.gcount();

    return true;
}

void cedrus::xid_con_test_only::insert_more_data_into_buffer( char * input_buffer, int size )
{
    // The write will fail if ios::good() isn't true. This usually happens when
    // we exhaust the buffer, seting eofbit on it. Clear everything.
    m_input_buffer.clear (m_input_buffer.goodbit);
    m_input_buffer.write(input_buffer, size);
}
