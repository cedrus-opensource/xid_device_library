#!/usr/bin/python

from sys import platform as _platform

if _platform != "darwin" and _platform != "win32":
    print("Unsupported platform: ", _platform)
    quit()

import time
import xid

# Edit this to match your devconfig location!
xds = xid.XIDDeviceScanner("C:/Users/Eugene Matsak/source/Xidon/xid_device_library/devconfig")

print ("Detecting XID devices, stand by.")
xds.DetectXIDDevices()

devCfgCount = xds.DevconfigCount()
if (devCfgCount == 0):
    print("No device configs found, returning...")
    quit()

print "Devconfig count = ", devCfgCount

devCount = xds.DeviceCount()
if (devCount == 0):
    print("No devices found, returning...")
    quit()

print "Device count = ", devCount

def connect_to_devices():
    for d in xrange (0, xds.DeviceCount()):
        devCon = xds.DeviceConnectionAtIndex(d)
        devCfg = devCon.GetDeviceConfig()
        print 'Open %-20s:%-20s' % (devCfg.GetDeviceName(), devCon.OpenConnection())

def print_dev_names():
    print "---------"
    for d in xrange (0, xds.DeviceCount()):
        devCon = xds.DeviceConnectionAtIndex(d)
        devCfg = devCon.GetDeviceConfig()
        print devCfg.GetDeviceName()
    print "---------"

def test_mpod_commands(devCon):
    print "Calling SetBaudRate(1) aka 19200 in order to properly interface with m-pod."
    devCon.SetBaudRate(1)

    devCon.ConnectToMpod(1,1)
    print_dev_info(devCon)

    devCon.ConnectToMpod(1,0)

def print_dev_info(devCon):
    devCfg = devCon.GetDeviceConfig()
    print "###########################"
    print "#### Basic device info ####"
    print "----- From devconfig"
    print '%-20s%-20s' % ("GetDeviceName(): ", devCfg.GetDeviceName())
    print '%-20s%-20s' % ("GetProductID(): ", devCfg.GetProductID())
    print '%-20s%-20s' % ("GetModelID(): ", devCfg.GetModelID())
    print '%-20s%-20s' % ("GetMajorVersion(): ", devCfg.GetMajorVersion())
    print

    print "----- From device itself"
    productID = devCon.GetProductID()
    print '%-20s%-20s' % ("productID: ", productID)
    modelID = devCon.GetModelID()
    print '%-20s%-20s' % ("modelID: ", modelID)
    # This value differentiates old and new devices
    print '%-20s%-20s' % ("GetMajorFirmwareVersion(): ", devCon.GetMajorFirmwareVersion())
    print '%-20s%-20s' % ("GetMinorFirmwareVersion(): ", devCon.GetMinorFirmwareVersion())
    print "GetInternalProductName():"
    internalName = devCon.GetInternalProductName()
    print internalName
    print "These two commands only work with XID2.0 devices:"
    print '%-20s%-20s' % ("GetOutpostModel() : ", devCon.GetOutpostModel())
    print '%-20s%-20s' % ("GetHardwareGeneration(): ", devCon.GetHardwareGeneration())
    print "###########################"
    print

def print_ports_info(devCon):
    devCfg = devCon.GetDeviceConfig()
    ports = devCfg.GetVectorOfPorts()

    print "###########################"
    print "#### Device Port info #####"

    for p in xrange (0, len(ports)):
        DevicePort = devCfg.get_port_by_index(p)
        print 'Port #%d'  % (p)
        print '%-20s%-3s%-20s' % ("portName: ", p, DevicePort.portName)
        print '%-20s%-3s%-20s' % ("portNumber: ", p, DevicePort.portNumber)
        print '%-20s%-3s%-20s' % ("numberOfLines: ", p, DevicePort.numberOfLines)
        print '%-20s%-3s%-20s' % ("isResponsePort: ", p, DevicePort.isResponsePort)
        print "keyMap: ",
        for k in DevicePort.keyMap:
            print k,
        print
    print "###########################"
    print

def print_response(response):
    print "---Fetched Response---"
    print '%-20s%-20s' % ("port: ", response.port)
    print '%-20s%-20s' % ("key: ", response.key)
    print '%-20s%-20s' % ("wasPressed: ", response.wasPressed)
    print '%-20s%-20s' % ("reactionTime: ", response.reactionTime)

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
    devCon.ClearResponsesFromBuffer()
    print

    devCfg = devCon.GetDeviceConfig()
    # You physically can't hold down two buttons on the SV-1
    if devCfg.GetProductID() != 49:
        print "Testing simultaneous keypresses... Hold down two buttons!"
        while (devCon.GetNumberOfKeysDown() < 2):
            devCon.PollForResponse()
            time.sleep(.01)
        print "Two keys down. Good job!"

        print "Clearing response queue..."
        devCon.ClearResponseQueue()
        devCon.ClearResponsesFromBuffer()

        print "Checking if we have queued responses (we should not, we just cleared them)"
        print '%-20s%-20s' % ("HasQueuedResponses(): ", devCon.HasQueuedResponses())
        print

    print "Testing response flushing... You have 5 seconds to press some buttons!"
    time.sleep (5)
    print "Calling ClearResponsesFromBuffer()"
    devCon.ClearResponsesFromBuffer()
    print "Attempting to retrieve a response from the buffer and put it into the queue..."
    devCon.PollForResponse()
    print "Checking if we have responses in the queue (we shouldn't due to the ClearResponsesFromBuffer() call)"
    print '%-20s%-20s' % ("HasQueuedResponses(): ", devCon.HasQueuedResponses())
    print

# Test officially starting here
print("Found the following devices:")
print_dev_names()

for device_index in xrange (0,  xds.DeviceCount()):
    print "========================================================"
    print "++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
    print "========================================================"
    devCon = xds.DeviceConnectionAtIndex(device_index)
    devCfg = devCon.GetDeviceConfig()
    isStimTrackerOrCpod = devCfg.GetProductID() == 83 or devCfg.GetProductID() == 52

    # Printing device and port information
    print_dev_info(devCon)
    print_ports_info(devCon)

    print "Calling RestoreFactoryDefaults()"
    print
    devCon.RestoreFactoryDefaults()

    # Resetting an LP-400 makes it output a trigger, just like sending
    # it f4 does. We're going to wait a little and then flush the buffer to
    # prevent it from messing any tests up.
    if devCfg.GetProductID() == 48 and devCfg.GetMajorVersion() == 1:
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

    # These calls do nothing for StimTracker! They're not implemented in the library
    # and actually sending these bytes to it doesn't result in anything noteworthy.
    if not isStimTrackerOrCpod:
        print "Testing protocol commands..."
        print '%-20s%-20s' % ("GetProtocol(): ", devCon.GetProtocol())
        print "Calling SetProtocol(3)"
        devCon.SetProtocol(3)
        print '%-20s%-20s' % ("GetProtocol(): ", devCon.GetProtocol())
        print "Calling SetProtocol(0)"
        devCon.SetProtocol(0)
        print

    # StimTracker will actually accept this command and adjust its internal state,
    # but nothing can possibly come of it, because of what StimTracker is.
    if devCfg.GetProductID() == 49:
        print '%-20s%-20s' % ("GetVKDropDelay(): ", devCon.GetVKDropDelay())
        print "Calling SetVKDropDelay(123)"
        devCon.SetVKDropDelay(123)
        time.sleep(.5)
        print '%-20s%-20s' % ("GetVKDropDelay(): ", devCon.GetVKDropDelay())
        print

    # Non-StimTracker devices will actually record and store pulse duration, but
    # have no way of taking advantage of it.
    if isStimTrackerOrCpod:
        print '%-20s%-20s' % ("GetPulseDuration(): ", devCon.GetPulseDuration())
        print "Calling SetPulseDuration(500)"
        devCon.SetPulseDuration(500)
        print '%-20s%-20s' % ("GetPulseDuration(): ", devCon.GetPulseDuration())
        print

    # This only does anything for Lumina LP-400
    if devCfg.GetProductID() == 48:
        trigger_default = devCon.GetTriggerDefault()
        print '%-20s%-20s' % ("GetTriggerDefault(): ", trigger_default)
        print "Inverting trigger default"
        devCon.SetTriggerDefault(not trigger_default)
        print '%-20s%-20s' % ("GetTriggerDefault(): ", devCon.GetTriggerDefault())
        print "Inverting trigger default back"
        devCon.SetTriggerDefault(trigger_default)
        print
        # Changing trigger default makes a Lumina LP-400 output a trigger. We're going to wait
        # a second and then flush the buffer.
        time.sleep(1)
        devCon.ClearResponsesFromBuffer()

    if devCfg.GetMajorVersion() > 1:
        print '%-20s%-20s' % ("QueryBaseTimer(): ", devCon.QueryBaseTimer())
        print "Calling devCon.ResetBaseTimer()"
        devCon.ResetBaseTimer()
        print '%-20s%-20s' % ("QueryBaseTimer(): ", devCon.QueryBaseTimer())
        print

    # These commands are XID 2 only.
    if devCfg.GetMajorVersion() > 1:
        print '%-20s%-20s' % ("GetLightSensorMode(): ", devCon.GetLightSensorMode())
        print "Calling SetLightSensorMode(2)"
        devCon.SetLightSensorMode(2)
        print '%-20s%-20s' % ("GetLightSensorMode(): ", devCon.GetLightSensorMode())
        print

        print '%-20s%-20s' % ("GetLightSensorThreshold(): ", devCon.GetLightSensorThreshold())
        print "Calling SetLightSensorThreshold(200)"
        devCon.SetLightSensorThreshold(200)
        print '%-20s%-20s' % ("GetLightSensorThreshold(): ", devCon.GetLightSensorThreshold())
        print

        # There is currently no matching get command for this
        #print "Calling SetScannerTriggerFilter(2)"
        #devCon.SetScannerTriggerFilter(2)

        print '%-20s%-20s' % ("GetMpodModel(1): ", devCon.GetMpodModel(1))

    # These commands are deprecated in XID 2
    else:
        # These are some more of those commands StimTracker will actually take, store
        # and then do nothing with. May as well not bother.
        if not isStimTrackerOrCpod:
            acc_conn_mode = devCon.GetAccessoryConnectorMode();
            print '%-20s%-20s' % ("GetAccessoryConnectorMode(): ", acc_conn_mode)
            print "Inverting accessory connector mode with SetAccessoryConnectorMode()"
            devCon.SetAccessoryConnectorMode(not acc_conn_mode)
            print '%-20s%-20s' % ("GetAccessoryConnectorMode(): ", devCon.GetAccessoryConnectorMode())
            print "Inverting accessory connector mode back with SetAccessoryConnectorMode()"
            devCon.SetAccessoryConnectorMode(acc_conn_mode)
            print

            print '%-20s%-20s' % ("GetTriggerDebounceTime(): ", devCon.GetTriggerDebounceTime())
            print "Calling SetTriggerDebounceTime(10)"
            devCon.SetTriggerDebounceTime(10)
            print '%-20s%-20s' % ("GetTriggerDebounceTime(): ", devCon.GetTriggerDebounceTime())
            print

            print '%-20s%-20s' % ("GetButtonDebounceTime(): ", devCon.GetButtonDebounceTime())
            print "call SetButtonDebounceTime(150)"
            devCon.SetButtonDebounceTime(150)
            print '%-20s%-20s' % ("GetButtonDebounceTime(): ", devCon.GetButtonDebounceTime())
            print

            output_logic = devCon.GetOutputLogic()
            print '%-20s%-20s' % ("GetOutputLogic(): ", output_logic)
            print "Inverting output logic with SetOutputLogic()"
            devCon.SetOutputLogic(not output_logic)
            print '%-20s%-20s' % ("GetOutputLogic(): ", devCon.GetOutputLogic())
            print "Inverting output logic back with SetOutputLogic()"
            devCon.SetOutputLogic(output_logic)
    print

    # StimTracker ST-100 has no buttons to press and cannot return responses.
    if not isStimTrackerOrCpod:
        print "Calling ResetRtTimer() You should be able to see the effects momentarily."
        devCon.ResetRtTimer()
        print
        run_button_tests(devCon)
    print("Performing line output test...")

    devCon.SetPulseDuration(300)
    linesBitmask = 1
    sleep_flash = .3
    print str(sleep_flash*1000) + "ms between line changes"
    for bm in xrange (0, 16):
        print "raise lines bitmask: ", linesBitmask
        xds.DeviceConnectionAtIndex(device_index).RaiseLines(linesBitmask)
        linesBitmask = linesBitmask << 1
        if linesBitmask > 255:
            linesBitmask = 1
        time.sleep(sleep_flash)

print("Clearing lines on all devices...")
print "Calling RestoreFactoryDefaults() on all devices..."
print("Closing device connections...")
for d in xrange (0, xds.DeviceCount()):
    devCon = xds.DeviceConnectionAtIndex(d)
    devCon.ClearLines()
    devCon.RestoreFactoryDefaults()
    devCon.CloseConnection()
    devCfg = devCon.GetDeviceConfig()
    print(devCfg.GetDeviceName() + " has lost connection at any point: " + str(devCon.HasLostConnection()))
