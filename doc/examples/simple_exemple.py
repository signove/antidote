#!/usr/bin/env python

import sys
import dbus
import re
import dbus.service
import dbus.mainloop.glib
import os
import glib

class Agent(dbus.service.Object):

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def Connected(self, dev, addr):
		print "Connected from addr %s, dev %s" % (addr, dev)

		# Convert path to an interface
		dev = bus.get_object("com.signove.health", dev)
		dev = dbus.Interface(dev, "com.signove.health.device")

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def Associated(self, dev, xmldata):
		print "Associated dev %s: %s" % (dev, xmldata)

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def MeasurementData(self, dev, data):
		print "MeasurementData dev %s" % dev
		print "\tData:\t", data

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def DeviceAttributes(self, dev, data):
		print "DeviceAttributes dev %s" % dev
		print "\tData:\t", data

	@dbus.service.method("com.signove.health.agent", in_signature="s", out_signature="")
	def Disassociated(self, dev):
		print "Disassociated dev %s" % dev

	@dbus.service.method("com.signove.health.agent", in_signature="s", out_signature="")
	def Disconnected(self, dev):
		print "Disconnected %s" % dev

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

bus = dbus.SystemBus()

obj = bus.get_object("com.signove.health", "/com/signove/health")
srv = dbus.Interface(obj, "com.signove.health.manager")
agent = Agent(bus, "/com/signove/health/agent/%d" % os.getpid())

# These are the data types, or specializations, that HDP will accept as
# sources. The values are taken from HDP spec. For example, 0x1004 is
# oximeter.
specializations = [0x1004, 0x1007, 0x1029, 0x100f]

print "Configuring..."
srv.ConfigurePassive(agent, specializations)
print "Waiting..."

mainloop = glib.MainLoop()
mainloop.run()
