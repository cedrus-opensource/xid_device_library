#!/usr/bin/python

from sys import platform as _platform

if _platform != "darwin" and _platform != "win32":
    print("Unsupported platform: ", _platform)
    quit()

import time
import xid

# Edit this to match your devconfig location!
xds = xid.xid_device_scanner("c:/Users/cedrusdev/source/xid_device_library/devconfig")

print ("Detecting XID devices, stand by.")
xds.detect_valid_xid_devices()

dev_cfg_count = xds.devconfig_count()
if (dev_cfg_count == 0):
    print("No device configs found, returning...")
    quit()

print "Devconfig count = ", dev_cfg_count

dev_count = xds.device_count()
if (dev_count == 0):
    print("No devices found, returning...")
    quit()

print "Device count = ", dev_count

def connect_to_devices():
    for d in xrange (0, xds.device_count()):
        dev_con = xds.device_connection_at_index(d)
        dev_cfg = dev_con.get_device_config()
        print 'Open %-20s:%-20s' % (dev_cfg.get_device_name(), dev_con.open_connection())

def print_dev_names():
    print "---------"
    for d in xrange (0, xds.device_count()):
        dev_con = xds.device_connection_at_index(d)
        dev_cfg = dev_con.get_device_config()
        print dev_cfg.get_device_name()
    print "---------"

def print_dev_info(dev_con):
    dev_cfg = dev_con.get_device_config()
    print "###########################"
    print "#### Basic device info ####"
    print "----- From devconfig"
    print '%-20s%-20s' % ("get_device_name(): ", dev_cfg.get_device_name())
    print '%-20s%-20s' % ("get_product_id(): ", dev_cfg.get_product_id())
    print '%-20s%-20s' % ("get_model_id(): ", dev_cfg.get_model_id())
    print '%-20s%-20s' % ("get_major_version(): ", dev_cfg.get_major_version())
    print

    print "----- From device itself"
    product_model_id = dev_con.get_product_and_model_id()
    print '%-20s%-20s' % ("product_id: ", product_model_id.product_id)
    print '%-20s%-20s' % ("model_id: ", product_model_id.model_id)
    # This value differentiates old and new devices
    print '%-20s%-20s' % ("get_major_firmware_version(): ", dev_con.get_major_firmware_version())
    print '%-20s%-20s' % ("get_minor_firmware_version(): ", dev_con.get_minor_firmware_version())
    print "get_internal_product_name():"
    internal_name = dev_con.get_internal_product_name()
    print internal_name
    print "These two commands only work with XID2.0 devices:"
    print '%-20s%-20s' % ("get_outpost_model() : ", dev_con.get_outpost_model())
    print '%-20s%-20s' % ("get_hardware_generation(): ", dev_con.get_hardware_generation())
    print "###########################"
    print

def print_ports_info(dev_con):
    dev_cfg = dev_con.get_device_config()
    ports = dev_cfg.get_vector_of_ports()
    ports_num = 0

    print "###########################"
    print "#### Device Port info #####"

    for p in xrange (0, len(ports)):
        device_port = dev_cfg.get_port_by_index(p)
        print 'Port #%d'  % (p)
        print '%-20s%-3s%-20s' % ("port_name: ", p, device_port.port_name)
        print '%-20s%-3s%-20s' % ("port_number: ", p, device_port.port_number)
        print '%-20s%-3s%-20s' % ("number_of_lines: ", p, device_port.number_of_lines)
        print '%-20s%-3s%-20s' % ("is_response_port: ", p, device_port.is_response_port)
        print "key_map: ",
        for k in device_port.key_map:
            print k,
        print
    print "###########################"
    print

def print_response(response):
    print "---Fetched Response---"
    print '%-20s%-20s' % ("port: ", response.port)
    print '%-20s%-20s' % ("key: ", response.key)
    print '%-20s%-20s' % ("was_pressed: ", response.was_pressed)
    print '%-20s%-20s' % ("reaction_time: ", response.reaction_time)

def run_button_tests(dev_con):
    dev_con.clear_response_queue()
    dev_con.clear_responses()

    print "Testing basic response gathering... Press a button!"
    while (dev_con.has_queued_responses() == False):
        dev_con.poll_for_response()
        time.sleep(.01)

    print_response(dev_con.get_next_response())
    print "The response queue should be empty now, as we've popped the only response it should have had."
    print '%-20s%-20s' % ("has_queued_responses(): ", dev_con.has_queued_responses())
    dev_con.clear_responses()
    print

    dev_cfg = dev_con.get_device_config()
    # You physically can't hold down two buttons on the SV-1
    if dev_cfg.get_product_id() != 49:
        print "Testing simultaneous keypresses... Hold down two buttons!"
        while (dev_con.get_number_of_keys_down() < 2):
            dev_con.poll_for_response()
            time.sleep(.01)
        print "Two keys down. Good job!"

        print "Clearing response queue..."
        dev_con.clear_response_queue()
        dev_con.clear_responses()

        print "Checking if we have queued responses (we should not, we just cleared them)"
        print '%-20s%-20s' % ("has_queued_responses(): ", dev_con.has_queued_responses())
        print

    print "Testing response flushing... You have 5 seconds to press some buttons!"
    time.sleep (5)
    print "Calling clear_responses()"
    dev_con.clear_responses()
    print "Attempting to retrieve a response from the buffer and put it into the queue..."
    dev_con.poll_for_response()
    print "Checking if we have responses in the queue (we shouldn't due to the clear_responses() call)"
    print '%-20s%-20s' % ("has_queued_responses(): ", dev_con.has_queued_responses())
    print

# Test officially starting here
print("Found the following devices:")
print_dev_names()

for device_index in xrange (0,  xds.device_count()):
    print "========================================================"
    print "++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
    print "========================================================"
    dev_con = xds.device_connection_at_index(device_index)
    dev_cfg = dev_con.get_device_config()
    is_StimTracker_or_C_Pod = dev_cfg.get_product_id() == 83 or dev_cfg.get_product_id() == 52

    # Printing device and port information
    print_dev_info(dev_con)
    print_ports_info(dev_con)

    print "Calling restore_factory_defaults()"
    print
    dev_con.restore_factory_defaults()

    # Resetting an LP-400 makes it output a trigger, just like sending
    # it f4 does. We're going to wait a little and then flush the buffer to
    # prevent it from messing any tests up.
    if dev_cfg.get_product_id() == 48 and dev_cfg.get_major_version() == 1:
        time.sleep(2)
        dev_con.clear_responses()

    # Some slight gymnastics to revert the baud rate later.
    baud_rate = dev_con.get_baud_rate()
    baud_to_set = 0 #9600
    if baud_rate == 19200:
        baud_to_set = 1
    elif baud_rate == 38400:
        baud_to_set = 2
    elif baud_rate == 57600:
        baud_to_set = 3
    elif baud_rate == 115200:
        baud_to_set = 4

    print "### Testing baud changing commands..."
    print '%-20s%-20s' % ("get_baud_rate(): ", dev_con.get_baud_rate())
    print "Calling set_baud_rate(2) aka 38400"
    dev_con.set_baud_rate(2)

    # Needed after changing baud rate.
    dev_con.close_connection();
    dev_con.open_connection();

    print '%-20s%-20s' % ("get_baud_rate(): ", dev_con.get_baud_rate())
    print "Calling set_baud_rate(" + str(baud_to_set) + ") to set it back to " + str(baud_rate)
    dev_con.set_baud_rate(baud_to_set)

    # Needed after changing baud rate.
    dev_con.close_connection();
    dev_con.open_connection();
    print "### Done!"
    print
    time.sleep(1)

    # These calls do nothing for StimTracker! They're not implemented in the library
    # and actually sending these bytes to it doesn't result in anything noteworthy.
    if not is_StimTracker_or_C_Pod:
        print "Testing protocol commands..."
        print '%-20s%-20s' % ("get_device_protocol(): ", dev_con.get_device_protocol())
        print "Calling set_device_protocol(3)"
        dev_con.set_device_protocol(3)
        print '%-20s%-20s' % ("get_device_protocol(): ", dev_con.get_device_protocol())
        print "Calling set_device_protocol(0)"
        dev_con.set_device_protocol(0)
        print

    # StimTracker will actually accept this command and adjust its internal state,
    # but nothing can possibly come of it, because of what StimTracker is.
    if dev_cfg.get_product_id() == 49:
        print '%-20s%-20s' % ("get_vk_drop_delay(): ", dev_con.get_vk_drop_delay())
        print "Calling set_vk_drop_delay(123)"
        dev_con.set_vk_drop_delay(123)
        time.sleep(.5)
        print '%-20s%-20s' % ("get_vk_drop_delay(): ", dev_con.get_vk_drop_delay())
        print

    # Non-StimTracker devices will actually record and store pulse duration, but
    # have no way of taking advantage of it.
    if is_StimTracker_or_C_Pod:
        print '%-20s%-20s' % ("get_pulse_duration(): ", dev_con.get_pulse_duration())
        print "Calling set_pulse_duration(500)"
        dev_con.set_pulse_duration(500)
        print '%-20s%-20s' % ("get_pulse_duration(): ", dev_con.get_pulse_duration())
        print

    # This only does anything for Lumina LP-400
    if dev_cfg.get_product_id() == 48:
        trigger_default = dev_con.get_trigger_default()
        print '%-20s%-20s' % ("get_trigger_default(): ", trigger_default)
        print "Inverting trigger default"
        dev_con.set_trigger_default(not trigger_default)
        print '%-20s%-20s' % ("get_trigger_default(): ", dev_con.get_trigger_default())
        print "Inverting trigger default back"
        dev_con.set_trigger_default(trigger_default)
        print
        # Changing trigger default makes a Lumina LP-400 output a trigger. We're going to wait
        # a second and then flush the buffer.
        time.sleep(1)
        dev_con.clear_responses()

    print '%-20s%-20s' % ("query_base_timer(): ", dev_con.query_base_timer())
    print "Calling dev_con.reset_base_timer()"
    dev_con.reset_base_timer()
    print '%-20s%-20s' % ("query_base_timer(): ", dev_con.query_base_timer())
    print

    # These commands are XID 2 only.
    if dev_cfg.get_major_version() > 1:
        print '%-20s%-20s' % ("get_light_sensor_mode(): ", dev_con.get_light_sensor_mode())
        print "Calling set_light_sensor_mode(2)"
        dev_con.set_light_sensor_mode(2)
        print '%-20s%-20s' % ("get_light_sensor_mode(): ", dev_con.get_light_sensor_mode())
        print

        print '%-20s%-20s' % ("get_light_sensor_threshold(): ", dev_con.get_light_sensor_threshold())
        print "Calling set_light_sensor_threshold(200)"
        dev_con.set_light_sensor_threshold(200)
        print '%-20s%-20s' % ("get_light_sensor_threshold(): ", dev_con.get_light_sensor_threshold())
        print

        # There is currently no matching get command for this
        #print "Calling set_scanner_trigger_filter(2)"
        #dev_con.set_scanner_trigger_filter(2)

        print '%-20s%-20s' % ("get_accessory_connector_device(): ", dev_con.get_accessory_connector_device())
    # These commands are deprecated in XID 2
    else:
        # These are some more of those commands StimTracker will actually take, store
        # and then do nothing with. May as well not bother.
        if not is_StimTracker_or_C_Pod:
            acc_conn_mode = dev_con.get_accessory_connector_mode();
            print '%-20s%-20s' % ("get_accessory_connector_mode(): ", acc_conn_mode)
            print "Inverting accessory connector mode with set_accessory_connector_mode()"
            dev_con.set_accessory_connector_mode(not acc_conn_mode)
            print '%-20s%-20s' % ("get_accessory_connector_mode(): ", dev_con.get_accessory_connector_mode())
            print "Inverting accessory connector mode back with set_accessory_connector_mode()"
            dev_con.set_accessory_connector_mode(acc_conn_mode)
            print

            print '%-20s%-20s' % ("get_trigger_debounce_time(): ", dev_con.get_trigger_debounce_time())
            print "Calling set_trigger_debounce_time(10)"
            dev_con.set_trigger_debounce_time(10)
            print '%-20s%-20s' % ("get_trigger_debounce_time(): ", dev_con.get_trigger_debounce_time())
            print

            print '%-20s%-20s' % ("get_button_debounce_time(): ", dev_con.get_button_debounce_time())
            print "call set_button_debounce_time(150)"
            dev_con.set_button_debounce_time(150)
            print '%-20s%-20s' % ("get_button_debounce_time(): ", dev_con.get_button_debounce_time())
            print

            output_logic = dev_con.get_output_logic()
            print '%-20s%-20s' % ("get_output_logic(): ", output_logic)
            print "Inverting output logic with set_output_logic()"
            dev_con.set_output_logic(not output_logic)
            print '%-20s%-20s' % ("get_output_logic(): ", dev_con.get_output_logic())
            print "Inverting output logic back with set_output_logic()"
            dev_con.set_output_logic(output_logic)
    print

    # StimTracker ST-100 has no buttons to press and cannot return responses.
    if not is_StimTracker_or_C_Pod:
        print "Calling reset_rt_timer() You should be able to see the effects momentarily."
        dev_con.reset_rt_timer()
        print
        run_button_tests(dev_con)
    print("Performing line output test...")

    dev_con.set_pulse_duration(300)
    lines_bitmask = 1
    sleep_flash = .3
    print str(sleep_flash*1000) + "ms between line changes"
    for bm in xrange (0, 16):
        print "raise lines bitmask: ", lines_bitmask
        xds.device_connection_at_index(device_index).raise_lines(lines_bitmask)
        lines_bitmask = lines_bitmask << 1
        if lines_bitmask > 255:
            lines_bitmask = 1
        time.sleep(sleep_flash)

print("Clearing lines on all devices...")
print "Calling restore_factory_defaults() on all devices..."
print("Closing device connections...")
for d in xrange (0, xds.device_count()):
    dev_con = xds.device_connection_at_index(d)
    dev_con.clear_lines()
    dev_con.restore_factory_defaults()
    dev_con.close_connection()
    dev_cfg = dev_con.get_device_config()
    print(dev_cfg.get_device_name() + " has lost connection at any point: " + str(dev_con.has_lost_connection()))
