#pragma once

#include <boost/preprocessor/control/if.hpp>

#define CEDRUS_XID_MAKEDLL 1

#ifdef __APPLE__
#    define CEDEXP __attribute__ ((visibility("default")))
#    define CEDIMP __attribute__ ((visibility("default")))
#elif defined(_WIN32)
#    define CEDEXP __declspec(dllexport)
#    define CEDIMP __declspec(dllimport)
#endif

#define CEDRUS_XIDDRIVER_IMPORTEXPORT   BOOST_PP_IIF( CEDRUS_XID_MAKEDLL, CEDEXP, CEDIMP )
