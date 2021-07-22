#pragma once

#ifdef __APPLE__
#    define CEDEXP __attribute__ ((visibility("default")))
#    define CEDIMP __attribute__ ((visibility("default")))
#elif defined(_WIN32)
#    define CEDEXP __declspec(dllexport)
#    define CEDIMP __declspec(dllimport)
#endif

#define CEDRUS_XIDDRIVER_IMPORTEXPORT CEDEXP
