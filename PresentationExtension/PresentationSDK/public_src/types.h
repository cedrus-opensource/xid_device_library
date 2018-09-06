//---------------------------------------------------------------------------
#ifndef typesH
#define typesH
//---------------------------------------------------------------------------
#include "loki\static_check.h"
#include <limits>
//---------------------------------------------------------------------------

namespace public_nbs {

// Quick conversion chart:
// BYTE = uint8
// WORD = uint16
// DWORD = uint32
// LONGLONG = sint64
// ULONGLONG = uint64
// LARGE_INTEGER = Large_int

typedef unsigned char      uint8;    // 8 -bit unsigned
typedef char               sint8;   // 8 -bit   signed

typedef unsigned short int uint16;    // 16-bit unsigned
typedef short int          sint16;   // 16-bit   signed

typedef unsigned long int  uint32;   // 32-bit unsigned
typedef long int           sint32;  // 32-bit   signed

typedef unsigned __int64   uint64;   // 64-bit   unsigned
typedef __int64            sint64;   // 64-bit   signed

typedef double				sfloat64; // 64-bit signed

typedef union _Large_int {
    struct {
        uint32 LowPart;
        sint32 HighPart;
    };
    struct {
        uint32 LowPart;
        sint32 HighPart;
    } u;
    sint64 QuadPart;
} Large_int;

STATIC_CHECK( sizeof(uint8) == 1, uint8_wrong_size )
STATIC_CHECK( !std::numeric_limits<uint8>::is_signed, uint8_wrong_sign )

STATIC_CHECK( sizeof(sint8) == 1, sint8_wrong_size )
STATIC_CHECK( std::numeric_limits<sint8>::is_signed, sint8_wrong_sign )

STATIC_CHECK( sizeof(uint16) == 2, uint16_wrong_size )
STATIC_CHECK( !std::numeric_limits<uint16>::is_signed, uint16_wrong_sign )

STATIC_CHECK( sizeof(sint16) == 2, sint16_wrong_size )
STATIC_CHECK( std::numeric_limits<sint16>::is_signed, sint16_wrong_sign )

STATIC_CHECK( sizeof(uint32) == 4, uint32_wrong_size )
STATIC_CHECK( !std::numeric_limits<uint32>::is_signed, uint32_wrong_sign )

STATIC_CHECK( sizeof(sint32) == 4, sint32_wrong_size )
STATIC_CHECK( std::numeric_limits<sint32>::is_signed, sint32_wrong_sign )

//STATIC_CHECK( std::numeric_limits<Int64>::is_signed, Int64_wrong_sign )  
STATIC_CHECK( sizeof(sint64) == 8, Int64_wrong_size )

} // end namespace public_nbs

//---------------------------------------------------------------------------
#endif