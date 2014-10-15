The README is basically a sample heavily commented devconfig:

[DeviceInfo]
# This is the only way to tell an RB-x30 from an RB-x40 by querying the device. Other than their major firmware version they will appear identical.
MajorFirmwareVersion = 1
# Device name is here because all RBs respond with "RB-x30/x40 Response Pad" when queried, and have to be told apart by their model IDs
DeviceName = RB-830
# Product ID is an ASCII value, because StimTracker 1G returns 'S' for it.
XidProductID = 50
# Same goes for model IDs. IMPORTANT NOTE: Model IDs returned by Lumina and SV-1 are to be ignored! They may be used in the future.
XidModelID = 51

[DeviceOptions]
# This is primarily a MacOS feature. Checking all bauds on a port can be time-consuming, and there will never be an XID device on these common ports.
# Add more ports you don't want to check, if you know of any.
XidIgnoreSerialPorts = /dev/cu.Bluetooth-PDA-Sync,/dev/cu.KeySerial1,/dev/cu.KeySerial2,/dev/cu.modem,/dev/cu.MotorolaPhone-Dial-UpNe-1,/dev/cu.MotorolaPhone-Dial-UpNe-2
# This is uhhh... ????
XidNeedsInterByteDelay = Yes

# Ports are labelled 0-7, since we have 3 bits to relay it as per the XID protocol. Some devices have more than others
[Port0]
# The port name isn't used by the library itself, but may be of great value to its clients as a way to differentiate the ports.
PortName = Keys
# This line is of greater value to non-response ports and is pretty self-explanatory.
NumberOfLines = 8
# Determines whether or not this port value is capable of producing responses for the library to consume.
UseableAsResponse = Yes
# This is an important part. Devices will return a 0-7 value corresponding to the key that was pressed. In the overwhelming majority of the cases,
# it will not make a lot of sense when compared to the physical button pressed on the device, and some devices have fewer than 8 buttons.
# That's where these mappings come in, and they exist almost entirely for the convenience of the users.
XidDeviceKeyMap0 = 7
XidDeviceKeyMap1 = 3
XidDeviceKeyMap2 = 4
XidDeviceKeyMap3 = 1
XidDeviceKeyMap4 = 2
XidDeviceKeyMap5 = 5
XidDeviceKeyMap6 = 6
XidDeviceKeyMap7 = 0

[Port1]
PortName = Accessory Connector
NumberOfLines = 6
UseableAsResponse = No
