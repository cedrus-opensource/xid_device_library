#include "xid_con_test_only.h"

cedrus::xid_con_test_only::xid_con_test_only( char * input_buffer, int size )
{
    m_input_buffer.write(input_buffer, size);
}

bool cedrus::xid_con_test_only::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int *bytes_read)
{
    m_input_buffer.read((char*)in_buffer, bytes_to_read);
    
    *bytes_read = (int)m_input_buffer.gcount();

    return true;
}
