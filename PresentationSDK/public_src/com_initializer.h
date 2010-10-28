//---------------------------------------------------------------------------
#ifndef COM_initializerH
#define COM_initializerH
//---------------------------------------------------------------------------

namespace public_nbs {

// used by clients to make sure COM is initialized
class COM_initializer
{
public:
	bool is_initialized() { return initialized; }
	static COM_initializer& singlet();
	~COM_initializer();
private:
	COM_initializer();
	COM_initializer( const COM_initializer& );
	COM_initializer& operator=( const COM_initializer& );
	bool initialized;
};

} // namespace public_nbs

//---------------------------------------------------------------------------
#endif
