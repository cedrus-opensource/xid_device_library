#include "Connection_TestOnly.h"

Cedrus::Connection_TestOnly::Connection_TestOnly( char * inputBuffer, int size )
{
   InsertMoreDataIntoBuffer(inputBuffer, size);
}

bool Cedrus::Connection_TestOnly::Read(
    unsigned char *inBuffer,
    DWORD bytesToRead,
    LPDWORD bytesRead)
{
    m_InputBuffer.read((char*)inBuffer, bytesToRead);
    
    *bytesRead = (int)m_InputBuffer.gcount();

    return true;
}

void Cedrus::Connection_TestOnly::InsertMoreDataIntoBuffer( char * inputBuffer, int size )
{
    // The write will fail if ios::good() isn't true. This usually happens when
    // we exhaust the buffer, seting eofbit on it. Clear everything.
    m_InputBuffer.clear (m_InputBuffer.goodbit);
    m_InputBuffer.write(inputBuffer, size);
}
