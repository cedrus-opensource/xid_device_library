//---------------------------------------------------------------------------
#include "string_utilities.h"
using namespace public_nbs;
#pragma warning( disable : 4996 )
//---------------------------------------------------------------------------

std::string public_nbs::to_str( int i )
{
	char buffer[34];
	return itoa( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( int i )
{
	wchar_t buffer[34];
	return _itow( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::string public_nbs::to_str( unsigned long int i )
{
	char buffer[34];
	return ultoa( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( unsigned long int i )
{
	wchar_t buffer[34];
	return _ultow( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::string public_nbs::to_str( unsigned int i )
{
	return to_str( (unsigned long int) i);
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( unsigned int i )
{
	return to_wstr( (unsigned long int) i);
}

//---------------------------------------------------------------------------

std::string public_nbs::to_str( sint64 i )
{
	char buffer[65];
	return _i64toa( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( sint64 i )
{
	wchar_t buffer[65];
	return _i64tow( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::string public_nbs::to_str( uint64 i )
{
	char buffer[65];
	return _ui64toa( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( uint64 i )
{
	wchar_t buffer[65];
	return _ui64tow( i, buffer, 10 );
}

//---------------------------------------------------------------------------

std::string public_nbs::to_str( double d )
{
	char buffer[100];
//	sprintf( buffer, "%.12g", d );
//	return std::string( buffer );
	gcvt( d, 15, buffer );
	size_t len = strlen( buffer );
	if( len > 1 && buffer[len-1] == '.' && buffer[len] == 0 ) {
		buffer[len-1] = 0;
	}
	return buffer;
}

//---------------------------------------------------------------------------

std::wstring public_nbs::to_wstr( double d )
{
	return ascii_to_wstring( to_str( d ) );
}

//---------------------------------------------------------------------------

std::wstring public_nbs::ascii_to_wstring( const std::string& ascii_string )
{
	std::wstring rval( ascii_string.size(), ' ' );
	std::copy( ascii_string.begin(), ascii_string.end(), rval.begin() );
	return rval;
}

//---------------------------------------------------------------------------

std::wstring public_nbs::hex_wstr( uint32 i, unsigned int width )
{
	std::wstring rval;
#ifdef __BORLANDC__
	if (width == 0) width = 16;
	rval = to_unicode( AnsiString::IntToHex( i, width ).c_str() );
#else
	const unsigned int SIZE = 100;
	wchar_t buf[SIZE + 1];
	buf[SIZE] = 0;
	_snwprintf( buf, SIZE, L"%0*I32X", width, i );
	rval = buf;
#endif
	return rval;
}

//---------------------------------------------------------------------------

#undef min
#undef max
bool public_nbs::trunc_str( const std::wstring& source, std::string& dest )
{
	bool res = true;
	dest.resize( source.size(), ' ' );

	std::string::iterator b = dest.begin();
	std::wstring::const_iterator i = source.begin();
	std::wstring::const_iterator e = source.end();
	for( ; i != e; ++i, ++b )
	{
		if( *i < std::numeric_limits<std::string::value_type>::min() || 
			*i > std::numeric_limits<std::string::value_type>::max() ) 
		{
			res = false;
		} 
		else 
		{
			*b = (std::string::value_type)*i;
		}
	}
	return res;
}

//---------------------------------------------------------------------------
