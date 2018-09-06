#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "DeviceConfig.h"
#include "constants.h"

#include <iostream>
#include <chrono>

int main()
{
    // There is a single static scanner.
    Cedrus::XIDDeviceScanner & scanner = Cedrus::XIDDeviceScanner::GetDeviceScanner();

    // Run detection.
    scanner.DetectXIDDevices();

    // Loop over the list of found XID devices.
    for (unsigned int i = 0; i < scanner.DeviceCount(); ++i)
    {
        // Grab a device, pull up its config for some information.
        std::shared_ptr<Cedrus::XIDDevice> device = scanner.DeviceConnectionAtIndex(i);
        std::shared_ptr<const Cedrus::DeviceConfig> dev_config = device->GetDeviceConfig();

        std::cout << "Device found:\n" << device->GetInternalProductName() << std::endl;

        // The Keyboard vs Standard mode only applies to RB-x40s and the Lumina 3G. There is no
        // harm in calling this command on anything else, but there is no reason to either.
        if (dev_config->IsRBx40() || dev_config->IsLumina3G())
        {
            std::cout << "Switching device to keyboard mode..." << std::endl;
            device->SwitchToKeyboardMode(); // The command itself.
            std::cout << "Done!" << std::endl << std::endl;
        }
    }

    std::cout << "All valid devices switched to keyboard mode." << std::endl;

    return 0;
}
