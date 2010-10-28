# Cedrus XID Device Library for Windows #

## Build Requirements ##

* Visual Studio 2010
* Boost >= 1.23 (for smart_ptr<>)

## Examples ##

### Presentation Response Extension ###

The Visual Studio 2010 solution comes with our response extension for
[Presentation](http://www.neurobs.com/ "Presentation"). The code that
uses the xid_device_driver library to interact with Presentation is in
the CedrusXidDriver subfolder in the repository.

### C++ Code Example ###

Here's a quick, mile high view on using an XID device in your C++
code:

    #include "xid_device_scanner_t.h"
    #include "xid_con_t.h"
    #include "xid_device_t.h"

    using namespace cedrus;

    // create a device scanner object to ease creation of new devices
    // and connections.  
    xid_device_scanner_t scanner;

    // get the number of detected XID devices.
    int num_response_pads = scanner.detect_valid_xid_devices();
    
     // get the first detected connection
    boost::shared_ptr<xid_con_t> xid_con =
        scanner.connection_at_index(0);

    // create the device and load its configuration
    boost::shared_ptr<xid_device_t> xid_device(
        new xid_device_t(xid_con,L"C:\Path\To\DevConfig\Files");
    
    while(true)
    {
        xid_device->poll_for_response();

        while(xid_device->has_queued_response())
        {
            response = xid_device->get_next_response();
            
            // do something with the response
        }
    }

## License and Copyright ##

Code in the PresentationSDK subfolder is copyrighted and licensed by
Neurobehavioral Systems.

Code in the CedrusXidDriver and xid_device_driver subfolders are
copyrited by Cedrus Corporation and licensed for use and distribution
under the modified BSD license (below).

### BSD License ###

Copyright (c) 2010, Cedrus Corporation
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
