#ifndef xid_device_scanner_helper_mac_H
#define xid_device_scanner_helper_mac_H

#include <vector>
#include <string>

#include <termios.h>
#include <IOKit/IOTypes.h> // for io_iterator_t
#include <CoreFoundation/CFBase.h> // for CFIndex
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>

#import <CoreServices/CoreServices.h>
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
#include <IOKit/serial/ioss.h>
#endif


namespace cedrus
{
    /// code is from http://developer.apple.com/library/mac/samplecode/SerialPortSample/index.html
    void FindAllSerialPorts( io_iterator_t & rIOIterator )
    {
        CFMutableDictionaryRef classesToMatch;

        // Serial devices are instances of class IOSerialBSDClient.
        classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);

        if (classesToMatch != NULL)
        {
            CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDRS232Type));

            IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &rIOIterator);
        }
    }


    /// code is from http://developer.apple.com/library/mac/samplecode/SerialPortSample/index.html
    bool FindNextSerialPort( io_iterator_t & rIOIterator, char *deviceFilePath, CFIndex maxPathSize )
    {
        io_object_t     serialService;
        kern_return_t 	kernResult = KERN_FAILURE;
        bool		    serialFound = false;

        // Initialize the returned path
        *deviceFilePath = '\0';

        // Iterate across all modems found. In this example, we exit after
        // finding the first modem.
        while (!serialFound && (serialService = IOIteratorNext(rIOIterator)) ) // assignment intentional
        {
            CFTypeRef   deviceFilePathAsCFString;
            // Get the callout device's path (/dev/cu.xxxxx).
            // The callout device should almost always be
            // used. You would use the dialin device (/dev/tty.xxxxx) when
            // monitoring a serial port for
            // incoming calls, for example, a fax listener.

            deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(serialService,
                                                                       CFSTR(kIOCalloutDeviceKey),
                                                                       kCFAllocatorDefault,
                                                                       0);
            if (deviceFilePathAsCFString)
            {
                Boolean result;

                result = CFStringGetCString(static_cast<CFStringRef>(deviceFilePathAsCFString),
                                            deviceFilePath,
                                            maxPathSize,
                                            kCFStringEncodingASCII);

                CFRelease(deviceFilePathAsCFString);

                if (result)
                {
                    serialFound = true;
                    kernResult = KERN_SUCCESS;
                }
            }

            IOObjectRelease(serialService);
        }

        return serialFound;
    }

    void load_com_ports_platform_specific( std::vector<std::string> * available_com_ports )
    {
        available_com_ports->clear();

        io_iterator_t rIOIterator;
        FindAllSerialPorts( rIOIterator );

        char deviceFilePath[50];
        std::string temp;

        while( (FindNextSerialPort(rIOIterator, deviceFilePath, sizeof(deviceFilePath))) && (available_com_ports->size() < MAX_PORTS) )
        {
            temp = deviceFilePath;
            available_com_ports->push_back(temp);
        }
    }
} // namespace cedrus

#endif // xid_device_scanner_helper_mac_H
