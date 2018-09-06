//---------------------------------------------------------------------------
#ifndef xptrH
#define xptrH
//---------------------------------------------------------------------------

namespace public_nbs {

// an auto_ptr for arrays
template<class X>
class Array_auto_ptr
{
public:
   Array_auto_ptr() : ptr( 0 ) {}
   Array_auto_ptr( X* p ) : ptr( p ) {}
   Array_auto_ptr( Array_auto_ptr<X>& r ) : ptr( r.release() ) {}
   ~Array_auto_ptr() { if (ptr) delete[] ptr; }
   void operator= ( Array_auto_ptr<X>& r) {
      if (ptr) delete[] ptr;
      ptr = r.release();
   }
   X& operator[]( unsigned int index ) {
      return *(ptr + index);
   }
   const X& operator[]( unsigned int index ) const  {
      return *(ptr + index);
   }
   X* get() const { return ptr; }
   X* release() { X* temp = ptr; ptr = 0; return temp; }
   void reset( X* p ) {
      if (ptr) delete[] ptr;
      ptr = p;
   }
private:
   X* ptr;
};

} // namespace public_nbs

//---------------------------------------------------------------------------
#endif