#!/usr/bin/python
"""
You can see a full list of all XID2 commands on Cedrus' website here:
https://www.cedrus.com/support/xid/commands.htm

m-pod specific commands can be found on a separate page here:
https://www.cedrus.com/support/xid/mpod_commands.htm

For an exhaustive list of all functions in the library, see
/xid_device_library/xid_device_driver/py_binding.cpp
"""

from sys import platform as _platform

if _platform != "darwin" and _platform != "win32":
    print("Unsupported platform: ", _platform)
    quit()

import time
import pyxid2

"""
The library contains a host of so-called device configs that let it know which XID device any
given serial device is. For instance, an RB-540 has the Product ID of 2, a Model ID of 1 and
a major firmware version of 2, while an RB-530 has the same Product and Model IDs, but a major
version of 1. This information is exposed for a variety of reasons,such as setting up a GUI
based on which device you're working with. There are also helpers for all XID device categories
and sub-categories(up to a point), such as IsRBx30(), IsXID1() and IsStimTracker2Quad().
"""
def print_dev_info(devCon):
    devCfg = devCon.GetDeviceConfig()
    print "###########################"
    print "#### Basic device info ####"
    print "----- From devconfig"
    print '%-20s%-20s' % ("GetDeviceName(): ", devCfg.GetDeviceName())
    print '%-20s%-20s' % ("GetProductID(): ", unichr(devCfg.GetProductID()))
    print '%-20s%-20s' % ("GetModelID(): ", unichr(devCfg.GetModelID()))
    print '%-20s%-20s' % ("GetMajorVersion(): ", devCfg.GetMajorVersion())
    print
    
    """
    The distinction between these two sections is that pulling information from the config doesn't
    require the library to query the device itself, which is beneficial for speed reasons, and also
    because sending queries interferes with response collection.

    Moreover, the information in the config should always match the query result, so these commands
    exists largely as a way to sanity-check device identification errors.
    """

    print "----- From device itself"
    productID = devCon.GetProductID()
    print '%-20s%-20s' % ("GetProductID(): ", unichr(productID))
    modelID = devCon.GetModelID()
    print '%-20s%-20s' % ("GetModelID(): ", unichr(modelID))
    print '%-20s%-20s' % ("GetMajorFirmwareVersion(): ", devCon.GetMajorFirmwareVersion())
    print '%-20s%-20s' % ("GetMinorFirmwareVersion(): ", devCon.GetMinorFirmwareVersion())
    print "GetInternalProductName():"
    internalName = devCon.GetInternalProductName()
    print internalName
    if devCfg.IsXID2():
        print '%-20s%-20s' % ("GetOutpostModel() : ", devCon.GetOutpostModel())
        print '%-20s%-20s' % ("GetHardwareGeneration(): ", devCon.GetHardwareGeneration())
        print "###########################"
        print

"""
Most devices will generate output on the serial port, and those devices have a map of their own
"ports" to help you make sense of what comes out of them. Each port object contains a string
for its name, its "number"(can also be a letter) and the number of lines it has. Aside from the
educational value, these things can help you set up a GUI. Some ports also contain a keyMap that
lets the library interpet what comes out of the device better. On some RBs it might not be
immediately clear which order the buttons go in, and the actual key presses reported make about
as much sense. That one's for internal use primarily, but it's exposed just in case. A lack of a
keyMap means that the library reports keys as they are given to it by the device.
"""
def print_ports_info(devCon):
    devCfg = devCon.GetDeviceConfig()
    ports = devCfg.GetMapOfPorts()

    print "###########################"
    print "#### Device Port info #####"

    for port in ports:
        print '%-20s%-20s' % ("portName: ", port.data().portName)
        print '%-20s%-20s' % ("portNumber: ", port.data().portNumber)
        print '%-20s%-20s' % ("numberOfLines: ", port.data().numberOfLines)
        print "keyMap: ",
        for k in port.data().keyMap:
            print k,
        print
    print "###########################"
    print

"""
The contents of a response object are mostly self-explanatory. Port number is going to match to one
of the ports that the device has, and the key is going be some value, in some case seemingly arbitrary
(a Light Sensor response from an RB-x40 is on Port 2 and its Key is 3. You'll never see a Key 0 or 1 on
that port). wasPressed just indicates whether the key was pressed or released(or equivalent) and the
reaction time is a timestamp from the device itself.
"""
def print_response(response):
    print "---Fetched Response---"
    print '%-20s%-20s' % ("port: ", response.port)
    print '%-20s%-20s' % ("key: ", response.key)
    print '%-20s%-20s' % ("wasPressed: ", response.wasPressed)
    print '%-20s%-20s' % ("reactionTime: ", response.reactionTime)

"""
Gathering responses from the pad has a few moving parts. You start by calling PollForResponse(), which
will pull some bytes from the buffer and attempt to parse them as a single response packet. If it
succeeds, it will store a processed response, and HasQueuedResponses() will now return true. You can then
retrieve a response from the queue at any time by calling GetNextResponse(). If you want to know whether
multiple keys are held down at once, GetNumberOfKeysDown() has you covered.

You can flush both the buffer of the device of itself, and the queue of processed responses via
ClearResponsesFromBuffer() and ClearResponseQueue() respectively.
"""
def run_button_tests(devCon):
    devCon.ClearResponseQueue()
    devCon.ClearResponsesFromBuffer()

    print "Testing basic response gathering... Press a button!"
    while (devCon.HasQueuedResponses() == False):
        devCon.PollForResponse()
        time.sleep(.01)

    print_response(devCon.GetNextResponse())
    print "The response queue should be empty now, as we've popped the only response it should have had."
    print '%-20s%-20s' % ("HasQueuedResponses(): ", devCon.HasQueuedResponses())
    print

    devCfg = devCon.GetDeviceConfig()
    # You physically can't hold down two buttons on the SV-1
    if not devCfg.IsSV1():
        devCon.ClearResponsesFromBuffer()
        print "Testing simultaneous keypresses... Hold down two buttons!"
        while (devCon.GetNumberOfKeysDown() < 2):
            devCon.PollForResponse()
            time.sleep(.01)
        print "Two keys down. Good job!"

        print "Clearing respons queue and buffer..."
        devCon.ClearResponseQueue()
        devCon.ClearResponsesFromBuffer()

    print "Testing buffer flushing... You have 5 seconds to press some buttons!"
    time.sleep (5)
    print "Calling ClearResponsesFromBuffer()"
    devCon.ClearResponsesFromBuffer()
    print "Attempting to retrieve a response from the buffer..."
    devCon.PollForResponse()
    print "Checking if we have responses in the queue (we shouldn't)"
    print '%-20s%-20s' % ("HasQueuedResponses(): ", devCon.HasQueuedResponses())
    print

"""
There is rarely a reason to connect to an m-pod directly. It does have settings that can be tweaked, but
navigating those can be a little complicated. We recommend that you use Xidon 2, a utility we provide
separately, in order to change those settings.
"""
def test_mpod_commands(devCon):
    print "Checking if an m-pod is present via GetMpodModel()..."
    podModel = unichr(devCon.GetMpodModel(1))
    print "m-pod 1 model (- means no connection): ", podModel

    if podModel == '-':
        return

    devCon.ConnectToMpod(1,1)
    print_dev_info(devCon)

    print '%-20s%-20s' % ("GetTranslationTable(): ", devCon.GetTranslationTable())

    mappedSignals = devCon.GetMappedSignals(0)
    print '%-20s%-20s' % ("GetMappedSignals(): ", mappedSignals)
    devCon.MapSignals(0, 536870912)
    print '%-20s%-20s' % ("GetMappedSignals(): ", devCon.GetMappedSignals(0))
    devCon.MapSignals(0, mappedSignals)
    print '%-20s%-20s' % ("GetMappedSignals(): ", devCon.GetMappedSignals(0))

    print '%-20s%-20s' % ("GetMpodPulseDuration(): ", devCon.GetMpodPulseDuration())
    #devCon.SetMpodPulseDuration()
    #devCon.ResetMappedLinesToDefault() // atX

    devCon.ConnectToMpod(1,0)

scanner = pyxid2.XIDDeviceScanner.GetDeviceScanner()

print ("Detecting XID devices, stand by.")
"""
Detecting devices will open all of the respective serial ports and keep them open for as long as the scanner persists.
Typically, this is the way to use the library. If you need to do something trickier with the devices, you can use these
functions to manipulate them.

CloseAllConnections() -- Closes all connections, but the devices remain in the scanner. You can open them back up without
re-scanning.
OpenAllConnections()  -- Opens all connection back up.
DropEveryConnection() -- In addition to closing the connections, the devices will be removed from the scanner and will
require a fresh scan to work with them again.
DropConnectionByPtr(device) -- Drops a specific device from the scanner.
CheckConnectionsDropDeadOnes() -- Fairly self-explanatory. Live devices will remain intact and their connections open.
"""
scanner.DetectXIDDevices()

devCount = scanner.DeviceCount()
if (devCount == 0):
    print("No devices found, returning...")
    quit()

print "Device count = ", devCount

# Test officially starting here
print("Found the following devices:")
print "---------"
for d in xrange (0, scanner.DeviceCount()):
    # This is the main way of accessing devices found by the scanner.
    devCon = scanner.DeviceConnectionAtIndex(d)
    print devCon.GetDeviceConfig().GetDeviceName()
print "---------"

for device_index in xrange (0,  scanner.DeviceCount()):
    print "========================================================"
    print "++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
    print "========================================================"
    devCon = scanner.DeviceConnectionAtIndex(device_index)
    devCfg = devCon.GetDeviceConfig()
    isStimTrackerOrCpod = devCfg.IsStimTracker1() or devCfg.IsCPod()

    # Printing device and port information
    print_dev_info(devCon)
    print_ports_info(devCon)

    # Resetting an LP-400 makes it output a trigger, just like sending
    # it f4 does. We're going to wait a little and then flush the buffer to
    # prevent it from messing any tests up.
    if devCfg.IsLuminaLP400():
        time.sleep(2)
        devCon.ClearResponsesFromBuffer()

    # Some slight gymnastics to revert the baud rate later.
    baudRate = devCon.GetBaudRate()
    baudToSet = 0 #9600
    if baudRate == 19200:
        baudToSet = 1
    elif baudRate == 38400:
        baudToSet = 2
    elif baudRate == 57600:
        baudToSet = 3
    elif baudRate == 115200:
        baudToSet = 4

    print "### Testing baud changing commands..."
    print '%-20s%-20s' % ("GetBaudRate(): ", devCon.GetBaudRate())
    print "Calling SetBaudRate(1) aka 19200"
    devCon.SetBaudRate(1)

    print '%-20s%-20s' % ("GetBaudRate(): ", devCon.GetBaudRate())
    print "Calling SetBaudRate(" + str(baudToSet) + ") to set it back to " + str(baudRate)
    devCon.SetBaudRate(baudToSet)

    print "### Done!"
    print
    time.sleep(1)

    # Non-StimTracker XID1 devices will actually record and store pulse duration, but
    # have no way of taking advantage of it.
    if devCfg.IsXID2() or devCfg.IsStimTracker1():
        print '%-20s%-20s' % ("GetPulseDuration(): ", devCon.GetPulseDuration())
        print "Calling SetPulseDuration(500)"
        devCon.SetPulseDuration(500)
        print '%-20s%-20s' % ("GetPulseDuration(): ", devCon.GetPulseDuration())
        print

    """
    There are several commands for controlling and filtering input from the device. A lot of these only ever need
    to be set once and then committed to flash with SaveSettingsToFlash(). We're avoiding using Set commands in
    this sample to ensure that the device state isn't changed. You can read more about these commands on the XID
    commands page.

    There are many different input selectors one can pass to these commands, varying from device to device. Light
    Sensor 1 ('A') is the one that all XID2 devices have.
    """
    if devCfg.IsXID2():
        print "Testing timer commands..."
        print '%-20s%-20s' % ("QueryRtTimer(): ", devCon.QueryRtTimer())
        print "Calling devCon.ResetRtTimer()"
        devCon.ResetRtTimer()
        print '%-20s%-20s' % ("QueryRtTimer(): ", devCon.QueryRtTimer())
        print

        print '%-20s%-20s' % ("GetEnableDigitalOutput(): ", devCon.GetEnableDigitalOutput(ord('A')))
        # This is what the Set equivalent of this command looks like.
        #devCon.GetEnableDigitalOutput(ord('A'), 2)

        print '%-20s%-20s' % ("GetEnableUSBOutput(): ", devCon.GetEnableUSBOutput(ord('A')))
        #devCon.GetEnableUSBOutput(ord('A'), 1)

        print '%-20s%-20s' % ("GetTimerResetOnOnsetMode(): ", devCon.GetTimerResetOnOnsetMode(ord('A')))
        #devCon.SetTimerResetOnOnsetMode(ord('A'), 2)

        print '%-20s%-20s' % ("GetAnalogInputThreshold(): ", devCon.GetAnalogInputThreshold(ord('A')))
        #devCon.SetAnalogInputThreshold(ord('A'), 66)

        singleShot = devCon.GetSingleShotMode(ord('A'))
        print "GetSingleShotMode() :"
        print 'enabled: %s delay: %s' % (singleShot.enabled, singleShot.delay)

        signalFilter = devCon.GetSignalFilter(ord('A'))
        print "GetSignalFilter() :"
        print 'holdOn: %s holdOff: %s' % (signalFilter.holdOn, signalFilter.holdOff)

    print "Calling ResetRtTimer() prior to the input test..."
    devCon.ResetRtTimer()
    print
    #run_button_tests(devCon)
    print "TESTING MPOD COMMANDS"
    test_mpod_commands(devCon)

    print "Performing line output test..."

    devCon.SetPulseDuration(300)
    linesBitmask = 1
    sleep_flash = .3
    print str(sleep_flash*1000) + "ms between line changes"
    for bm in xrange (0, 16):
        print "raise lines bitmask: ", linesBitmask
        scanner.DeviceConnectionAtIndex(device_index).RaiseLines(linesBitmask)
        linesBitmask = linesBitmask << 1
        if linesBitmask > 255:
            linesBitmask = 1
        time.sleep(sleep_flash)

print("Clearing lines on all devices...")
for d in xrange (0, scanner.DeviceCount()):
    devCon = scanner.DeviceConnectionAtIndex(d)
    devCon.ClearLines()
