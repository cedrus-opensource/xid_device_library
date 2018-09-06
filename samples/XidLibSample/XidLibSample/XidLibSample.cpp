#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "DeviceConfig.h"
#include "constants.h"

#include <iostream>
#include <chrono>

int main()
{
    // There is a single static scanner
    Cedrus::XIDDeviceScanner & scanner = Cedrus::XIDDeviceScanner::GetDeviceScanner();

    scanner.DetectXIDDevices();

    if (scanner.DeviceCount() > 0)
    {
        std::shared_ptr<Cedrus::XIDDevice> device = scanner.DeviceConnectionAtIndex(0);

        std::cout << "Device found:\n" << device->GetInternalProductName() << std::endl;

        int ls_threshold = device->GetAnalogInputThreshold('A');
        std::cout << "Light Sensor Threshold: " << ls_threshold << std::endl;

        std::cout << "Setting Light Sensor Threshold to itself for purely educational purposes\n"
            << "Do some input with your device! Generate a total of 6 responses through keys/light sensor/scanner trigger/voice key input.\n"
            << "Remember that a key press and a key release are two separate responses!" << std::endl;

        device->SetAnalogInputThreshold('A', ls_threshold);

        device->ClearResponseQueue(); // Clears response collection in the library
        device->ClearResponsesFromBuffer(); // Clears out the port buffers responses are drawn from
        device->ResetRtTimer();

        unsigned int responses = 0;

        // A function that isn't used in this example, but can be helpful for sanity-checking is GetNumberOfKeysDown()
        while(true)
        {
            device->PollForResponse(); // Will parse a single response in the buffer

            if (device->HasQueuedResponses())
            {
                Cedrus::Response resp = device->GetNextResponse();

                std::cout << "\nWe have a response!"
                    // This lets you know what type of response this was. You can see in DeviceConfigRepository what
                    // kinds of input any given device has. An RB-x40 has its keys (0) and its light sensor (2)
                    << "\nPort: " << resp.port 
                    // This is the index of the key according to their order on the pad. Note that the order may
                    // not be immediately obvious, depending on the specific model. This value is actually different
                    // from what the pad actually returns and is translated in accordance to the key map.
                    << "\nKey:" << resp.key
                    // This one is pretty self-explanatory. A response is generated for both a press and a release,
                    // and this lets you know which one it was.
                    << "\nPressed: " << resp.wasPressed
                    // The response time is measured in ms since the last timer reset.
                    << "\nReaction Time: " << resp.reactionTime << std::endl;

                ++responses;
            }

            if (responses > 5)
            {
                device->ClearResponseQueue();
                device->ClearResponsesFromBuffer();
                break;
            }
        }

        std::cout << "You should see some flashing lights now!" << std::endl;

        device->ClearLines(); // Just in case.

        // This is fairly self-explanatory. A value of 0 means lines will stay up until changed.
        device->SetPulseDuration(300);
        unsigned int lines_bitmask = 1;

        // This will just rotate through the first 8 output pins twice.
        for (unsigned int i = 0; i < 16; ++i)
        {
            device->RaiseLines(lines_bitmask);
            lines_bitmask = lines_bitmask << 1;
            std::chrono::duration<int, std::milli> timespan(200);

            if (lines_bitmask > 255)
                lines_bitmask = 1;
        }

        // m-pods communicate on 19200 baud only, so you need to set the RB to it
        device->SetBaudRate(1); // 1 means 19200
        // The first argument is the m-pod you want to connect to (some devices can have multiple),
        // Second is what you want to do. '0' goes back to the RB, '1' connects to the m-pod, '2'
        // is for updating firmware
        device->ConnectToMpod(1, 1);

        std::cout << "\nNow connected to:\n" << device->GetInternalProductName() << std::endl;

        device->ConnectToMpod(1, 0);
        std::cout << "And we're back!" << std::endl;
    }
    else
        std::cout << "No devices found" << std::endl;

    std::cout << "We're done!" << std::endl;
    int nothing;
    std::cin >> nothing;

    return 0;
}
