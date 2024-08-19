#pragma once

#include <cassert>
#include <iostream> // for std::cerr
#include <signal.h> // for raise
#include <stdlib.h> // for getenv

#if defined(_WIN32)
#    include <Windows.h>
#endif // #if defined(_WIN32)

#if defined(__APPLE__)
#    include <CoreFoundation/CoreFoundation.h>
#    define _putenv putenv
#endif // defined(__APPLE__)

namespace Cedrus
{
    // break into the debugger
    inline void TrapDebug()
    {
        #if defined(_WIN32)
            DebugBreak();
        #elif defined(__APPLE__)
            raise(SIGTRAP);
        #else
            // TODO
        #endif // Win/Apple
    }

    inline void OptionToContinue
    (
     const char* title,
     const char* message,
     const char* filename,
     const int line,
     const char* funcname
    )
    {
        #if defined(_WIN32)

        // suppress warnings about unused parameters on win32:
        (void) title;
        (void) message;
        (void) filename;
        (void) line;
        (void) funcname;

        #elif defined(__APPLE__)

        std::cerr << title << ":\n";
        std::cerr << funcname << "\n";
        std::cerr << filename << ":" << line << "\n";
        std::cerr << message << "\n";

        CFStringRef headerRef =  CFStringCreateWithCString( NULL, title,    kCFStringEncodingUTF8 );
        CFStringRef messageRef = CFStringCreateWithCString( NULL, message,  kCFStringEncodingUTF8 );

        CFStringRef button1 =    CFStringCreateWithCString( NULL, "Break",  kCFStringEncodingUTF8 ); // defaultButtonTitle
        CFStringRef button2 =    CFStringCreateWithCString( NULL, "Continue", kCFStringEncodingUTF8 ); // alternateButtonTitle

        CFOptionFlags response;

        CFUserNotificationDisplayAlert
            ( 0, // timeout. (apparently in seconds) The amount of time to wait for the user to dismiss
                 // the notification dialog before the dialog dismisses
                 // itself. Pass 0 to have the dialog never time out.
              kCFUserNotificationCautionAlertLevel,
              NULL, // iconURL
              NULL, // soundURL
              NULL, // localizationURL
              headerRef,
              messageRef,
              button1, // defaultButtonTitle
              button2, // alternateButtonTitle
              NULL, // otherButtonTitle
              &response);

        CFRelease(headerRef);
        CFRelease(messageRef);
        CFRelease(button1);
        CFRelease(button2);

        if ( response == kCFUserNotificationDefaultResponse )
        {
            // choice was "Break"
            TrapDebug();
        }
        else if ( response == kCFUserNotificationAlternateResponse )
        {
            // choice was "Continue"
            // do nothing
        }
        else // if you add a 3rd button, this would be kCFUserNotificationOtherResponse
        {
            // either the notification timed out by itself (no user interaction),
            // or else the user hit the ESCAPE key

            std::cerr << "ignoring opportunity to debug the FAIL (either due to inaction or ESC key)\n";
        }
        #else
            // TODO
        #endif // Win/Apple
    }

#if defined(__APPLE__)

    inline void Ced_Asrt_Mac
    ( const char* message,
      const char* filename,
      const int line,
      const char* funcname )
    {
        if ( getenv("CEDRUS_SUPALL_ASRT") )
        {
            return; // BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!
        }

        OptionToContinue
            ( "CEDRUS_ASSERT",
              message,
              filename,
              line,
              funcname );
    }

    inline void Ced_Fail_Mac
    ( const char* message,
      const char* filename,
      const int line,
      const char* funcname )
    {
        if ( getenv("CEDRUS_SUPALL_ASRT") )
        {
            return; // BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!  BAILING OUT!!
        }

        OptionToContinue
            ( "CEDRUS_FAIL",
              message,
              filename,
              line,
              funcname );
    }

#endif //#if defined(__APPLE__)

    inline void Suppress_All_Assertions()
    {
    // if asserts have been BUILD-TIME compiled away, then do NOT bother calling putenv
    #ifndef CEDRUS_DISABLE_ASSERT

        static const char* setting = "CEDRUS_SUPALL_ASRT=1";
        // should return 0 for SUCCESS:
        /*const int ret =*/ _putenv( (char*) setting );

    #endif // CEDRUS_DISABLE_ASSERT
    }

    inline void UnSuppress_All_Assertions()
    {
    // if asserts have been BUILD-TIME compiled away, then do NOT bother calling putenv
    #ifndef CEDRUS_DISABLE_ASSERT

        static const char* negation = "CEDRUS_SUPALL_ASRT=";
        // should return 0 for SUCCESS:
        /*const int ret =*/ _putenv( (char*) negation ); // on win32, this is enough to nullify the var.

        #if ! defined(_WIN32)
        /*const int ret =*/ unsetenv( "CEDRUS_SUPALL_ASRT" );// on posix, the above set it to "", and HERE we nullify it
        #endif //#if ! defined(_WIN32)

    #endif // CEDRUS_DISABLE_ASSERT
    }

    inline bool GetEnv_WinOnly( const char* name )
    {
    #if !defined(_WIN32)

        // suppress warnings about unused parameters on mac:
        (void) name;
        return false;

    #else
#ifdef _UNICODE
        size_t converted = 0;
        wchar_t wtext[500];
        mbstowcs_s(&converted, wtext, 500, name, 480);
        LPWSTR ptr = wtext;

        // get the size of the buffer
        DWORD dwRet = ::GetEnvironmentVariable(ptr, NULL, 0);
#else
        // get the size of the buffer
        DWORD dwRet = ::GetEnvironmentVariable(name, NULL, 0);
#endif _UNICODE

        bool rslt = false;

        if ( dwRet )
        {
            rslt = true;
        }

        return rslt;

    #endif // #if defined(_WIN32)
    }

}

/**
   PLEASE READ THIS INTERESTING ITEM ABOUT BAD INTERACTIONS OF IF/ELSE
   STRUCTURES AND PREPROCESSOR MACROS:

    http://stackoverflow.com/questions/154136/do-while-and-if-else-statements-in-c-c-macros
*/

/*
  If you have a Qt QString, then you need to pass it to CEDRUS_ASSERT
  by doing something like this:

       QString message;

       CEDRUS_ASSERT( 1 == 2, message.toUtf8() );

  If you are using std::string, then you will want to pass it in the following
  way:

       std::string message;

       CEDRUS_ASSERT( 1 == 2, message.c_str() );

*/
#ifdef CEDRUS_DISABLE_ASSERT

         // define it as "nothing." they will be "compiled out"
#        define CEDRUS_ASSERT(cond, msg)

#        define CEDRUS_FAIL(msg)


#else // the ELSE is when we _ENABLE_ our assertions

#    if defined(_WIN32)

         // when assertions are enabled on Win:
#        define CEDRUS_ASSERT(cond, msg)         \
             __pragma(warning(push))         \
             __pragma(warning(disable:4127)) \
             do { if ( ! Cedrus::GetEnv_WinOnly("CEDRUS_SUPALL_ASRT") ) assert( ( cond ) && ( msg ) );  } while ( 0 ) \
             __pragma(warning(pop))

         // when assertions are enabled on Win:
#        define CEDRUS_FAIL(msg)                 \
             __pragma(warning(push))         \
             __pragma(warning(disable:4127)) \
             do { if ( ! Cedrus::GetEnv_WinOnly("CEDRUS_SUPALL_ASRT") ) assert( ! msg );  } while ( 0 ) \
             __pragma(warning(pop))

#    elif defined(__APPLE__)

         // when assertions are enabled on Mac:
#        define CEDRUS_ASSERT(cond, msg)         \
            if ( ( cond ) )                      \
            {                                    \
            }                                    \
            else                                 \
                Cedrus::Ced_Asrt_Mac( msg, __FILE__, __LINE__,  __func__ )

// when assertions are enabled on Mac:
#        define CEDRUS_FAIL(msg)                 \
            Cedrus::Ced_Fail_Mac( msg, __FILE__, __LINE__,  __func__ )


#else
        // TODO
#endif // Win/Apple

#endif // #ifdef CEDRUS_DISABLE_ASSERT

#ifdef NDEBUG_WAS_SET_CEDRUS_HEADER
// Restoring NDEBUG if it was enabled originally
#    define NDEBUG
#endif

