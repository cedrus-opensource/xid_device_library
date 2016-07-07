#!/usr/bin/python

from sys import platform as _platform

if _platform != "darwin" and _platform != "win32":
	print("Unsupported platform: ", _platform)
	quit()

import time
import xid

xds = xid.xid_device_scanner("c:/Users/cedrusdev/source/py_xid/xid_device_library/devconfig")

print ("Detecting XID devices, stand by.")
xds.detect_valid_xid_devices()

dev_cfg_count = xds.devconfig_count()
if (dev_cfg_count == 0):
	print("No device configurations found, returning...")
	quit()

print "Device configurations count = ", dev_cfg_count

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
	for d in xrange (0, xds.device_count()):
		dev_con = xds.device_connection_at_index(d)
		dev_cfg = dev_con.get_device_config()
		print dev_cfg.get_device_name()

def print_dev_info(dev_con):
	dev_cfg = dev_con.get_device_config()
	print dev_cfg.get_device_name()
	print '%-20s%-20s' % ("device_name: ", dev_cfg.get_device_name())
	print '%-20s%-20s' % ("cfg_get_product_id: ", dev_cfg.get_product_id())
	print '%-20s%-20s' % ("cfg_get_model_id: ", dev_cfg.get_model_id())

	product_model_id = dev_con.get_product_and_model_id()
	print '%-20s%-20s' % ("dev_product_id: ", product_model_id.product_id)
	print '%-20s%-20s' % ("dev_model_id: ", product_model_id.model_id)

	print '%-20s%-20s' % ("hardware_generation: ", dev_con.get_hardware_generation())
	time.sleep(1)
	print '%-20s%-20s' % ("major_firmware_version: ", dev_con.get_major_firmware_version())
	time.sleep(1)
	print '%-20s%-20s' % ("minor_firmware_version: ", dev_con.get_minor_firmware_version())
	print '%-20s%-20s' % ("internal_product_name: ", dev_con.get_internal_product_name())
	print '%-20s%-20s' % ("outpost_model: ", dev_con.get_outpost_model())

def print_ports_info(dev_con):
	dev_cfg = dev_con.get_device_config()
	ports_vec = dev_cfg.get_vector_of_ports()
	ports_num = 0

	for dp in ports_vec:
		print '%-20s%-3s%-20s' % ("get_num_lines_on_port: ", ports_num, dev_cfg.get_num_lines_on_port(ports_num))
		ports_num =ports_num + 1

	for p in xrange (0, ports_num):
		device_port = dev_cfg.get_port_by_index(p)
		print '%-20s%-3s%-20s' % ("port_name: ", p, device_port.port_name)
		print '%-20s%-3s%-20s' % ("port_number: ", p, device_port.port_number)
		print '%-20s%-3s%-20s' % ("number_of_lines: ", p, device_port.number_of_lines)
		print '%-20s%-3s%-20s' % ("is_response_port: ", p, device_port.is_response_port)
		print "key_map: ",
		for k in device_port.key_map:
			print k,
		print

def raise_lines(lines_bitmask):
	for d in xrange (0, xds.device_count()):
		dev_con = xds.device_connection_at_index(d)
		dev_con.raise_lines(lines_bitmask)

def clear_lines():
	for d in xrange (0, xds.device_count()):
		dev_con = xds.device_connection_at_index(d)
		dev_con.clear_lines()

def print_response(response):
	print '%-20s%-20s' % ("port: ", response.port)
	print '%-20s%-20s' % ("key: ", response.key)
	print '%-20s%-20s' % ("was_pressed: ", response.was_pressed)
	print '%-20s%-20s' % ("reaction_time: ", response.reaction_time)

#Start tests
print("Found the following devices:")
print_dev_names()

print("Connect to the devices:")
connect_to_devices()

print("Device info:")
for d in xrange (0, xds.device_count()):
	dev_con = xds.device_connection_at_index(d)
	print_dev_info(dev_con)

print("Raising all lines:")
raise_lines(255)

sleep_sec = 1
time.sleep(sleep_sec)

print("Clearing lines")
clear_lines()

time.sleep(sleep_sec)

print("Test devices")
for d in xrange (0,  xds.device_count()):
	dev_con = xds.device_connection_at_index(d)
	dev_cfg = dev_con.get_device_config()

	print "######"
	print "Device: " + dev_cfg.get_device_name()

	print '%-20s%-20s' % ("baud_rate: ", dev_con.get_baud_rate())
	print "Call dev_con.set_baud_rate(2)"

	#TODO: test dev_con.set_baud_rate()

	print '%-20s%-20s' % ("baud_rate: ", dev_con.get_baud_rate())

	#TODO: test dev_con.set_device_protocol(1)

	print '%-20s%-20s' % ("light_sensor_mode: ", dev_con.get_light_sensor_mode())
	print "call dev_con.set_light_sensor_mode(2)"
	dev_con.set_light_sensor_mode(2)
	print '%-20s%-20s' % ("light_sensor_mode: ", dev_con.get_light_sensor_mode())

	print '%-20s%-20s' % ("light_sensor_threshold: ", dev_con.get_light_sensor_threshold())
	print "call dev_con.set_light_sensor_threshold(2)"
	dev_con.set_light_sensor_threshold(2)
	print '%-20s%-20s' % ("light_sensor_threshold: ", dev_con.get_light_sensor_threshold())

	print '%-20s%-20s' % ("pulse_duration: ", dev_con.get_pulse_duration())
	print "call dev_con.set_pulse_duration(1)"
	dev_con.set_pulse_duration(1)
	print '%-20s%-20s' % ("pulse_duration: ", dev_con.get_pulse_duration())

	print "call dev_con.set_scanner_trigger_filter(2)"
	dev_con.set_scanner_trigger_filter(2)

	print '%-20s%-20s' % ("query_base_timer: ", dev_con.query_base_timer())
	print "call dev_con.reset_rt_timer()"
	dev_con.reset_rt_timer()
	print "call dev_con.reset_base_timer()"
	dev_con.reset_base_timer()
	print '%-20s%-20s' % ("query_base_timer: ", dev_con.query_base_timer())

	print '%-20s%-20s' % ("accessory_connector_mode: ", dev_con.get_accessory_connector_mode())
	print "call dev_con.set_accessory_connector_mode(0)"
	dev_con.set_accessory_connector_mode(0)
	print '%-20s%-20s' % ("accessory_connector_mode: ", dev_con.get_accessory_connector_mode())

	print '%-20s%-20s' % ("accessory_connector_device: ", dev_con.get_accessory_connector_device())

	print "call dev_con.set_output_logic(2)"
	dev_con.set_output_logic(2)

	#TODO: set_vk_drop_delay does not work properly
	#print "call dev_con.set_vk_drop_delay(2)"
	#dev_con.set_vk_drop_delay(0)

	time.sleep(0.1)

	trigger_default = dev_con.get_trigger_default()
	print '%-20s%-20s' % ("trigger_default: ", trigger_default)
	print "invert trigger_default"
	dev_con.set_trigger_default(not trigger_default)
	print '%-20s%-20s' % ("trigger_default: ", dev_con.get_trigger_default())

	trigger_default = dev_con.get_trigger_default()
	print '%-20s%-20s' % ("trigger_default: ", trigger_default)
	print "invert trigger_default"
	dev_con.set_trigger_default(not trigger_default)
	print '%-20s%-20s' % ("trigger_default: ", dev_con.get_trigger_default())

	print '%-20s%-20s' % ("trigger_debounce_time: ", dev_con.get_trigger_debounce_time())
	print "call dev_con.set_trigger_debounce_time(10)"
	dev_con.set_trigger_debounce_time(10)
	print '%-20s%-20s' % ("trigger_debounce_time: ", dev_con.get_trigger_debounce_time())

	print '%-20s%-20s' % ("button_debounce_time: ", dev_con.get_button_debounce_time())
	print "call dev_con.set_button_debounce_time(150)"
	dev_con.set_button_debounce_time(150)
	print '%-20s%-20s' % ("button_debounce_time: ", dev_con.get_button_debounce_time())

	dev_con.clear_response_queue()
	dev_con.clear_responses()

	print "Press a button"
	while (dev_con.has_queued_responses() == False):
		dev_con.poll_for_response()
		time.sleep(.01)

	print "Response found:"
	print_response(dev_con.get_next_response())

	dev_con.clear_responses()
	print '%-20s%-20s' % ("has_queued_responses: (should be False due to get_next_response(): ", dev_con.has_queued_responses())

	print "Hold down two buttons"
	while (dev_con.get_number_of_keys_down() < 2):
		dev_con.poll_for_response()
		time.sleep(.01)
	print "Two keys down. Good job!"

	dev_con.clear_response_queue()
	dev_con.clear_responses()

	print "You have 5 seconds to press buttons"
	time.sleep (5)

	print "Call clear_responses()"
	dev_con.clear_responses()
	dev_con.poll_for_response()
	print '%-20s%-20s' % ("has_queued_responses (should be False, due to clear_responses() call): ", dev_con.has_queued_responses())
	print "Response found:"
	print_response(dev_con.get_next_response())

	print "Press buttons"
	while (dev_con.has_queued_responses() == False):
		dev_con.poll_for_response()
		time.sleep(.01)

	print "A response was found"
	print "Call clear_response_queue()"
	dev_con.clear_response_queue()

	print '%-20s%-20s' % ("has_queued_responses (should be False, due to clear_responses() call): ", dev_con.has_queued_responses())

print("OOOH!! FLASHY!! (Hit Ctrl-C to exit)")
time.sleep(sleep_sec)

for d in xrange (0, xds.device_count()):
	dev_con = xds.device_connection_at_index(d)
	dev_con.set_pulse_duration(100)

lines_bitmask = 1
sleep_flash = 1*sleep_sec
#while True:
for d in xrange (0, 12):
	print "raise lines bitmask = ", lines_bitmask
	for d in xrange (0, xds.device_count()):
		xds.device_connection_at_index(d).raise_lines(lines_bitmask)

	lines_bitmask = lines_bitmask << 1
	if lines_bitmask > 255:
		lines_bitmask = 1

	print("sleep " + str(sleep_flash) + " second(s)...")
	time.sleep(sleep_flash)
	print("wake up!")

print("Clearing lines")
for d in xrange (0, xds.device_count()):
	dev_con = xds.device_connection_at_index(d)
	dev_con.clear_lines()

print("Close connections")
for d in xrange (0, xds.device_count()):
	dev_con = xds.device_connection_at_index(d)
	dev_con.close_connection()
	dev_cfg = dev_con.get_device_config()
	print(dev_cfg.get_device_name() + " has lost connection: " + str(dev_con.has_lost_connection()))

print "call dev_con.restore_factory_defaults"
dev_con.restore_factory_defaults()
