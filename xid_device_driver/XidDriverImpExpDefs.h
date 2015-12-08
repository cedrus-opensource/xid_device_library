#ifndef  CEDRUS_XID_DRIVER_IMP_EXP_DLL_DEF
#define  CEDRUS_XID_DRIVER_IMP_EXP_DLL_DEF

#include <boost/preprocessor/control/if.hpp>

#ifdef CEDRUS_XID_MAKEDLL
#    undef CEDRUS_XID_MAKEDLL

#    define CEDRUS_XID_MAKEDLL 1
#else
#    define CEDRUS_XID_MAKEDLL 0
#endif

#ifdef __APPLE__
#    define CEDEXP __attribute__ ((visibility("default")))
#    define CEDIMP __attribute__ ((visibility("default")))
#elif defined(_WIN32)
#    define CEDEXP __declspec(dllexport)
#    define CEDIMP __declspec(dllimport)
#endif

#define CEDRUS_XIDDRIVER_IMPORTEXPORT   BOOST_PP_IIF( CEDRUS_XID_MAKEDLL, CEDEXP, CEDIMP )

#endif  // CEDRUS_XID_DRIVER_IMP_EXP_DLL_DEF
