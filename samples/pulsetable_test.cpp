/*
This is a short sample of the Pulse Table feature of XID. Under the vast majority
of circumstances it's not necessary, and users are better served sending separate
event markers (see event_markers.cpp).
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

    //dev->GetPulseTableBitMask();
    //dev->IsPulseTableRunning();

    dev->SetPulseDuration(0);
    dev->RaiseLines(0xFFFF); // This is supposed to flash all lines, and will do so for 2 seconds before we set up the pulse table
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Setting up the pulse table will reserve the lines used for pulse table use, so you will see those lines go low.
    dev->ClearPulseTable();
    dev->AddPulseTableEntry(0, 0x0101);
    dev->AddPulseTableEntry(500, 0x0202);
    dev->AddPulseTableEntry(1000, 0x0404);
    dev->AddPulseTableEntry(1500, 0x0808);
    dev->AddPulseTableEntry(2000, 0x0110);
    dev->AddPulseTableEntry(2500, 0x0220);
    dev->AddPulseTableEntry(3000, 0x0440);
    dev->AddPulseTableEntry(3500, 0x0880);
    dev->AddPulseTableEntry(4000, 0x0000);
    dev->AddPulseTableEntry(0, 0x0000);
    dev->RunPulseTable();
    std::cout << "Waiting 5s for the pulse table to finish, as the bit mask cannot be cleared while it's running." << std::endl;
    // You could also end the program here and let the table run, but the sample is trying to avoid making lingering changes to the device.

    std::this_thread::sleep_for(std::chrono::seconds(5));
    dev->RaiseLines(0x00);
    dev->ClearPulseTable();

    return 0;
}
