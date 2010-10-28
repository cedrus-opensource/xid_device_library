//---------------------------------------------------------------------------
#ifndef floating_point_controlH
#define floating_point_controlH
//---------------------------------------------------------------------------
#include <float.h>
//---------------------------------------------------------------------------

namespace public_nbs
{

class Floating_point_control
{
public:
	Floating_point_control( unsigned int new_value, unsigned int mask )
		: previous_value_( _control87( 0, 0 ) ), mask_( mask )
	{
		_control87( new_value, mask );
	}

	~Floating_point_control()
	{
		_clear87();
		_control87( previous_value_, mask_ );
	}

private:
	Floating_point_control( const Floating_point_control& );
	void operator=( const Floating_point_control& );
	unsigned int previous_value_;
	unsigned int mask_;
};

} // namespace public_nbs

//---------------------------------------------------------------------------
#endif