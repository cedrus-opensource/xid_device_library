/*
This sample outputs 8 event markers using an XID device on lines 1 through 8
at 300ms intervals, producing what we call "marching lights".
*/
#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "DeviceConfig.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main()
{
    // Get a list of all attached XID devices.
    Cedrus::XIDDeviceScanner & scanner = Cedrus::XIDDeviceScanner::GetDeviceScanner();
    scanner.DetectXIDDevices();

    if (scanner.DeviceCount() == 0)
    {
        std::cout << "No XID devices detected" << std::endl;
        return 0;
    }

    for (unsigned int i = 0; i < scanner.DeviceCount(); ++i)
        std::cout << "Device found: " << scanner.DevconfigAtIndex(i)->GetDeviceName() << std::endl;

    std::shared_ptr<Cedrus::XIDDevice> dev = scanner.DeviceConnectionAtIndex(0); // get the first device to use

    std::cout << "Using device: " << dev->GetDeviceConfig()->GetDeviceName() << std::endl;

    // SendPulse() takes the pulse duration (ms), the lines bitmask, the number of
    // pulses and the inter-pulse interval (ms).
    // Note: SendPulse() requires an XID 2 device and does nothing on older ones.
    for (unsigned int line = 0; line < 8; ++line)
    {
        unsigned int mask = 1u << line;
        std::cout << "raising line " << mask << std::endl;
        dev->SendPulse(300, mask, 1, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    /*
    Older alternative: set a global pulse duration once, then raise lines.

    // Setting the pulse duration to 0 makes the lines stay activated until lowered
    // manually with LowerLines() or ClearLines().
    dev->SetPulseDuration(300);

    for (unsigned int line = 0; line < 8; ++line)
    {
        unsigned int mask = 1u << line;
        std::cout << "raising line " << mask << std::endl;
        dev->RaiseLines(mask);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    dev->ClearLines();
    */

    return 0;
}