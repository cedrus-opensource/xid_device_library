//--------------------------------------------------------------------------- 
#include "COM_initializer.h"
#include "objbase.h"
using namespace public_nbs;
//---------------------------------------------------------------------------   
//
// class COM_initializer
//
//---------------------------------------------------------------------------

COM_initializer& COM_initializer::singlet()
{
   static COM_initializer instance;
   return instance;
}

//---------------------------------------------------------------------------

COM_initializer::~COM_initializer()
{
   CoUninitialize();
}

//---------------------------------------------------------------------------

COM_initializer::COM_initializer()
{
   HRESULT res = CoInitialize( NULL );
   initialized = (res == S_OK) || (res == S_FALSE);
}

//---------------------------------------------------------------------------
