"""
This sample gives a simple example of how to utilize the Pulse Table feature of XID 2.

You can see an overview of it on https://www.cedrus.com/support/xid/commands.htm under
"Digital Output Commands - A Sequence of Pulses"
"""
from sys import platform

if platform != "darwin" and platform != "win32":
    print("Unsupported platform: ", platform)
    quit()

import time
import pyxid2

scanner = pyxid2.XIDDeviceScanner.GetDeviceScanner()

print "Detecting XID devices, stand by..."
scanner.DetectXIDDevices()

devCount = scanner.DeviceCount()
if (devCount == 0):
    print "No devices found."
    quit()

devCon = scanner.DeviceConnectionAtIndex(0)
print devCon.GetCombinedInfo()

devCon.ClearPulseTable()
devCon.AddPulseTableEntry(0, 0x01)
devCon.AddPulseTableEntry(500, 0x02)
devCon.AddPulseTableEntry(1000, 0x04)
devCon.AddPulseTableEntry(1500, 0x08)
devCon.AddPulseTableEntry(2000, 0x01)
devCon.AddPulseTableEntry(2500, 0x02)
devCon.AddPulseTableEntry(3000, 0x04)
devCon.AddPulseTableEntry(3500, 0x08)
devCon.AddPulseTableEntry(4000, 0x00)
devCon.AddPulseTableEntry(0, 0x00)
devCon.RunPulseTable()
print "Waiting 5s for the pulse table to finish, as the bit mask cannot be cleared while it's running."
# You could also end the program here and let the table run, but the sample is trying to avoid making lingering changes to the device.
time.sleep(2)
devCon.RaiseLines(0xFF) #This is supposed to flash all lines, but you'll find that only the second 4 flash, because the first 4 were reserved by the table we set up
time.sleep(3)
devCon.ClearLines()
devCon.ClearPulseTable()
