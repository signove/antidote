#!/usr/bin/env python

import sys
import os
import glib
import dbus
import socket
import dbus.service
import gobject
import replay_agent
from dbus.mainloop.glib import DBusGMainLoop

DBusGMainLoop(set_as_default=True)
loop = gobject.MainLoop()
bus = dbus.SystemBus()

tape_pos = -1
apdu = None
tape = replay_agent.tape

def go_forward():
	global apdu
	global tape_pos
	global tape
	tape_pos += 1
	if tape_pos >= len(tape):
		print "End of tape"
		sys.exit(0)
	else:
		apdu = tape[tape_pos]

def send_data(sk):
	global apdu
	while True:
		go_forward()
		if apdu["direction"] == "M":
			print "Expecting %04x, %d bytes" % (apdu["choice"], apdu["length"] + 4)
			break
		else:
			print "Sent %04x" % apdu["choice"]
			print sk.send(apdu["data"])


def recv_data(sk, data):
	print "Received %d octets" % len(data)
	send_data(sk)
	

watch_bitmap = glib.IO_IN | glib.IO_ERR | glib.IO_HUP | glib.IO_NVAL

def data_received(sk, evt):
	data = None
	if evt & glib.IO_IN:
		try:
			data = sk.recv(1024)
		except IOError:
			data = ""
		if data:
			print "Data received"
			recv_data(sk, data)

	more = (evt == glib.IO_IN and data)

	if not more:
		print "EOF"
		try:
			sk.shutdown(2)
		except IOError:
			pass
		print "Closing"
		sk.close()

	return more


class SignalHandler(object):
	def __init__(self):
		bus.add_signal_receiver(self.ChannelConnected,
			signal_name="ChannelConnected",
			bus_name="org.bluez",
			path_keyword="device",
			interface_keyword="interface",
			dbus_interface="org.bluez.HealthDevice")

		bus.add_signal_receiver(self.ChannelDeleted,
			signal_name="ChannelDeleted",
			bus_name="org.bluez",
			path_keyword="device",
			interface_keyword="interface",
			dbus_interface="org.bluez.HealthDevice")

	def ChannelConnected(self, channel, interface, device):
		print "Device %s channel %s up" % (device, channel)
		channel = bus.get_object("org.bluez", channel)
		channel = dbus.Interface(channel, "org.bluez.HealthChannel")
		fd = channel.Acquire()
		print "Got raw rd %s" % fd
		# take fd ownership
		fd = fd.take()
		print "FD number is %d" % fd
		# encapsulate numericfd in Python socket object
		sk = socket.fromfd(fd, socket.AF_UNIX, socket.SOCK_STREAM)
		# fromfd() does dup() so we need to close the original
		os.close(fd)
		print "FD acquired"
		glib.io_add_watch(sk, watch_bitmap, data_received)
		send_data(sk)

	def ChannelDeleted(self, channel, interface, device):
		print "Device %s channel %s deleted" % (device, channel)


signal_handler = SignalHandler()

config = {"Role": "Source", "DataType": dbus.types.UInt16(0x1004),
		"Description": "Oximeter source"}

manager = dbus.Interface(bus.get_object("org.bluez", "/"),
					"org.bluez.Manager")

adapter = manager.DefaultAdapter()
adapter = dbus.Interface(bus.get_object("org.bluez", adapter),
					"org.bluez.Adapter")
dev = None
for devpath in adapter.GetProperties()["Devices"]:
	print "Found device %s" % devpath
	cdev = dbus.Interface(bus.get_object("org.bluez", devpath),
						"org.bluez.Device")
	addr = cdev.GetProperties()["Address"]
	if addr.lower() == sys.argv[2].lower():
		cdev.DiscoverServices("")
		dev = devpath
		break

if not dev:
	print "No device matches parameter"
	sys.exit(1)

hmanager = dbus.Interface(bus.get_object("org.bluez", "/org/bluez"),
					"org.bluez.HealthManager")
app = hmanager.CreateApplication(config)

dev = dbus.Interface(bus.get_object("org.bluez", dev),
					"org.bluez.HealthDevice")
channel = dev.CreateChannel(app, "Reliable")

print "channel %s up" % channel
# channel = bus.get_object("org.bluez", channel)
# channel = dbus.Interface(channel, "org.bluez.HealthChannel")
# fd = channel.Acquire()
# print "Got raw rd %s" % fd
# take fd ownership
# fd = fd.take()
# print "FD number is %d" % fd
# encapsulate numericfd in Python socket object
# sk = socket.fromfd(fd, socket.AF_UNIX, socket.SOCK_STREAM)
# fromfd() does dup() so we need to close the original
# os.close(fd)
# print "FD acquired"
# glib.io_add_watch(sk, watch_bitmap, data_received)
# send_data(sk)

try:
	loop = glib.MainLoop()
	loop.run()
except KeyboardInterrupt:
	pass

finally:
	hmanager.DestroyApplication(app)
	print
	print "Application stopped"
	print
