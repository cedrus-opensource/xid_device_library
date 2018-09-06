//---------------------------------------------------------------------------
#ifndef exceptionH
#define exceptionH
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------

namespace public_nbs {

class Exception 
{
public:
	Exception( const std::wstring& message ) : message_( message ) {}

	const wchar_t* message() const { return message_.c_str(); }
private:
	std::wstring message_;
};

} // namespace public_nbs

//---------------------------------------------------------------------------
#endif