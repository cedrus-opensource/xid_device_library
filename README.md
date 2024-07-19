# Cedrus XID Device and StimTracker Library for Windows #

## Supported Hardware ##

* Cedrus RB-530
* Cedrus RB-730
* Cedrus RB-830
* Cedrus RB-834
* Cedrus RB-540
* Cedrus RB-740
* Cedrus RB-840
* Cedrus RB-844
* Cedrus Riponda
* Cedrus SV-1 Voice Key
* Cedrus Lumina LP-400 fMRI Response Pad System
* Cedrus Lumina 3G
* Cedrus StimTracker
* Cedrus StimTracker 2 Duo
* Cedrus StimTracker 2 Quad
* Cedrus c-pod
* Cedrus m-pod

More information on this hardware can be found on [cedrus.com](http://www.cedrus.com "cedrus.com")

## Documentation ##

There is currently no proper documentation for the library itself.

However, you can view the full list of XID2 commands here:
https://www.cedrus.com/support/xid/commands.htm

There are some m-pod specific commands on a separate page:
https://www.cedrus.com/support/xid/mpod_commands.htm

There is some additional helpful information for signal flow here:
https://www.cedrus.com/support/xid/signal_flow.htm

## Examples ##

### Presentation Response Extension ###

The Visual Studio 2010 solution comes with our response extension for
[Presentation](http://www.neurobs.com/ "Presentation"). The code that
uses the xid_device_driver library to interact with Presentation is in
the CedrusXidDriver subfolder in the repository.

### C++ Code Example for Response Collection ###

    #include "XIDDeviceScanner.h"
    #include "XIDDevice.h"

    #include <iostream>

    int main()
    {
        Cedrus::XIDDeviceScanner & scanner = Cedrus::XIDDeviceScanner::GetDeviceScanner();

        scanner.DetectXIDDevices();

        std::shared_ptr<Cedrus::XIDDevice> device = scanner.DeviceConnectionAtIndex(0);

        std::cout << "Device found:\n" << device->GetInternalProductName() << std::endl;
        std::cout << "This program will close as soon as you press a button on the device.";

        while (true)
        {
            device->PollForResponse();

            if (device->HasQueuedResponses())
            {
                Cedrus::Response resp = device->GetNextResponse();
                // process response
                break;
            }
        }

        return 0;
    }

## License and Copyright ##

Code in the PresentationSDK subfolder is copyrighted and licensed by
Neurobehavioral Systems.

Code in the CedrusXidDriver and xid_device_driver subfolders are
copyrited by Cedrus Corporation and licensed for use and distribution
under the modified BSD license (below).

### BSD License ###

Copyright (c) 2024, Cedrus Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  

Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

Neither the name of Cedrus Corporation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
