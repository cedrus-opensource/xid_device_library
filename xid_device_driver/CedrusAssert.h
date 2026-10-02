#pragma once

// Debug-only "soft" assertions for the XID driver.
//
//   CEDRUS_ASSERT(cond, msg)   CEDRUS_FAIL(msg)
//
// On failure the condition, message and source location are written to stderr (and, on
// Windows, to the debugger's output window). If a debugger is attached it stops there;
// either way execution then CONTINUES.
//
// Compiled out entirely when NDEBUG is defined (CMake defines it for every non-Debug
// configuration). msg must be a const char*: use str.c_str() or qstr.toUtf8().constData().

#ifndef NDEBUG

#include <cstdio>
#include <cstdlib>

#if defined(_WIN32)
#   include <windows.h>
#elif defined(__APPLE__)
#   include <signal.h>
#   include <sys/sysctl.h>
#   include <unistd.h>
#endif

namespace Cedrus
{
    inline bool IsDebuggerAttached()
    {
    #if defined(_WIN32)
        return ::IsDebuggerPresent() != 0;
    #elif defined(__APPLE__)
        // Apple Technical Q&A QA1361
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
        struct kinfo_proc info = {};
        size_t size = sizeof(info);
        if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
            return false;
        return (info.kp_proc.p_flag & P_TRACED) != 0;
    #else
        return false;
    #endif
    }

    inline void ReportAssertFailure(const char* cond, const char* msg,
                                    const char* file, int line, const char* func)
    {
        if (std::getenv("CEDRUS_SUPALL_ASRT"))
            return;

        char text[1024];
        std::snprintf(text, sizeof(text), "%s(%d): %s in %s: %s%s%s\n",
                      file, line, cond ? "CEDRUS_ASSERT" : "CEDRUS_FAIL", func,
                      cond ? cond : "", cond ? " -- " : "", msg ? msg : "");
        std::fputs(text, stderr);

    #if defined(_WIN32)
        ::OutputDebugStringA(text);
        if (IsDebuggerAttached())
            __debugbreak();
    #elif defined(__APPLE__)
        if (IsDebuggerAttached())
            raise(SIGTRAP);
    #endif
    }

    inline void Suppress_All_Assertions()
    {
    #if defined(_WIN32)
        _putenv_s("CEDRUS_SUPALL_ASRT", "1");
    #else
        setenv("CEDRUS_SUPALL_ASRT", "1", 1);
    #endif
    }

    inline void UnSuppress_All_Assertions()
    {
    #if defined(_WIN32)
        _putenv_s("CEDRUS_SUPALL_ASRT", "");   // an empty value removes the variable on Windows
    #else
        unsetenv("CEDRUS_SUPALL_ASRT");
    #endif
    }
}

#define CEDRUS_ASSERT(cond, msg) \
    do { if (!(cond)) ::Cedrus::ReportAssertFailure(#cond, (msg), __FILE__, __LINE__, __func__); } while (0)

#define CEDRUS_FAIL(msg) \
    ::Cedrus::ReportAssertFailure(nullptr, (msg), __FILE__, __LINE__, __func__)

#else // NDEBUG

namespace Cedrus
{
    inline void Suppress_All_Assertions() {}
    inline void UnSuppress_All_Assertions() {}
}

// sizeof keeps variables that exist only for an assertion "used" (no warnings) without evaluating anything.
#define CEDRUS_ASSERT(cond, msg) ((void)sizeof(!(cond)), (void)sizeof(msg))
#define CEDRUS_FAIL(msg)         ((void)sizeof(msg))

#endif // NDEBUG