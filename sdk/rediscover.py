#!/usr/bin/env python

import sys
import os
import glib
import dbus
import socket
import dbus.service
import gobject
from dbus.mainloop.glib import DBusGMainLoop

DBusGMainLoop(set_as_default=True)
loop = gobject.MainLoop()
bus = dbus.SystemBus()

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
	if addr.lower() == sys.argv[1].lower():
		cdev.DiscoverServices("")
		dev = devpath
		break

if not dev:
	print "No device matches parameter"
	sys.exit(1)
