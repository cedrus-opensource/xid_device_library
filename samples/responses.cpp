/*
This sample shows how to collect responses from an XID device. Note that you
have to poll the device for responses manually (done in a while loop here).

When a physical key is pressed on the device, a set of bytes describing it go
into the serial buffer. This also occurs when the physical key is released.
Calling PollForResponse() makes the library check the serial buffer for bytes
constituting a response packet, and put a Response object in its internal
response queue. It does so once per PollForResponse() call. Calling
GetNextResponse() pops a single response from the response queue. If you want
to avoid seeing more responses than necessary, you can use
ClearResponsesFromBuffer() to prevent more responses from being added to the
queue by PollForResponse(), and you can clear already processed responses with
ClearResponseQueue().
*/
#include "XIDDeviceScanner.h"
#include "XIDDevice.h"
#include "DeviceConfig.h"

#include <iostream>
#include <memory>

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

    dev->ResetRtTimer();

    std::cout << "Press a key!" << std::endl;
    while (!dev->HasQueuedResponses())
        dev->PollForResponse();

    Cedrus::Response response = dev->GetNextResponse();
    // You can filter out key releases by simply ignoring them
    if (response.wasPressed)
    {
        // Process response as desired
        std::cout << "\nResponse detected!"
                  // This lets you know what type of response this was. You can see in DeviceConfigRepository what
                  // kinds of input any given device has. An RB-x40 has its keys (0) and its light sensor (2)
                  << "\nPort: " << response.port
                  // This is the index of the key according to their order on the pad. Note that the order may
                  // not be immediately obvious, depending on the specific model. This value is actually different
                  // from what the pad actually returns and is translated in accordance to the key map.
                  << "\nKey: " << response.key
                  // This one is pretty self-explanatory. A response is generated for both a press and a release,
                  // and this lets you know which one it was.
                  << "\nPressed: " << response.wasPressed
                  // The response time is measured in ms since the last timer reset.
                  << "\nReaction Time: " << response.reactionTime << std::endl;
    }

    dev->ClearResponsesFromBuffer();
    dev->ClearResponseQueue();

    return 0;
}
