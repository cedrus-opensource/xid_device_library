//---------------------------------------------------------------------------
#ifndef com_ptrH
#define com_ptrH
//---------------------------------------------------------------------------
#include "guiddef.h"
#include "unknwn.h"
#include "com_initializer.h"
#include "string_utilities.h"
#include "exception.h"
//---------------------------------------------------------------------------

namespace public_nbs {

// smart pointer for COM interfaces
// NOTE that constructor with pointer argument and reset do not call AddRef
template<class X>
class COM_ptr
{
public:
    COM_ptr() : ptr( 0 ) {}
    COM_ptr( X* p ) : ptr( p ) {}
    COM_ptr( const COM_ptr<X>& r ) : ptr( r.get() ) { if (ptr) ptr->AddRef(); }
    ~COM_ptr() { if (ptr) { ptr->Release(); } }
    void operator= (const COM_ptr<X>& r) {
        if (ptr) ptr->Release();
        ptr = r.get();
        if (ptr) ptr->AddRef();
    }
    X& operator* () const { return *ptr; }
    X* operator-> () const { return ptr; }
    X* get() const { return ptr; }
    void reset( X* p ) {
        if (ptr) ptr->Release();
        ptr = p;
    }                           
    X** assignee() {
        if (ptr) ptr->Release();
        ptr = 0;
        return &ptr;
    }
    bool operator<( const COM_ptr<X>& p ) const { return ptr < p.ptr; }
    bool operator==( const COM_ptr<X>& p ) const { return ptr == p.ptr; }
    bool operator!=( const COM_ptr<X>& p ) const { return ptr != p.ptr; }
    bool operator!() const { return !ptr; }

    static COM_ptr<X> create( REFCLSID class_id, REFIID interface_id,
        LPUNKNOWN aggregate = NULL, DWORD context = CLSCTX_INPROC_SERVER );

private:
    X* ptr;
};
    
template<class X>
COM_ptr<X> COM_ptr<X>::create( REFCLSID class_id, REFIID interface_id,
    LPUNKNOWN aggregate, DWORD context )
{
    X* temp_ptr = 0;
    public_nbs::COM_initializer::singlet();
    HRESULT res = CoCreateInstance( class_id, aggregate, context, interface_id, (void**)&temp_ptr );

    if (res != S_OK)
    {
        std::wstring error;
        switch (res) {
            case REGDB_E_CLASSNOTREG: error = L"Class not registered"; break;
            case E_OUTOFMEMORY: error = L"Out of memory"; break;
            case E_INVALIDARG: error = L"Invalid argument"; break;
            case CLASS_E_NOAGGREGATION: error = L"No aggregation"; break;
            default: error = L"Error code " + to_wstr( res );
        }
        throw Exception( error );
    }

    return COM_ptr<X>( temp_ptr );
}

} // namespace public_nbs

//---------------------------------------------------------------------------
#endif
 