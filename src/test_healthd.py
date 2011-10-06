#!/usr/bin/python

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
		print
		print "Connected from addr %s, dev %s" % (addr, dev)

		# Convert path to an interface
		dev = bus.get_object("com.signove.health", dev)
		dev = dbus.Interface(dev, "com.signove.health.device")

		glib.timeout_add(0, do_something, dev)

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def Associated(self, dev, xmldata):
		print
		print "Associated dev %s: %s" % (dev, xmldata)
		
		# Convert path to an interface
		dev = bus.get_object("com.signove.health", dev)
		dev = dbus.Interface(dev, "com.signove.health.device")

		glib.timeout_add(0, getConfiguration, dev)
		glib.timeout_add(1000, requestMdsAttributes, dev)

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def MeasurementData(self, dev, data):
		print
		print "MeasurementData dev %s" % dev
		print "\tData:\t", data

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def DeviceAttributes(self, dev, data):
		print
		print "DeviceAttributes dev %s" % dev
		print "\tData:\t", data

	@dbus.service.method("com.signove.health.agent", in_signature="s", out_signature="")
	def Disassociated(self, dev):
		print
		print "Disassociated dev %s" % dev

	@dbus.service.method("com.signove.health.agent", in_signature="s", out_signature="")
	def Disconnected(self, dev):
		print
		print "Disconnected %s" % dev

def requestMdsAttributes (dev):
	dev.RequestDeviceAttributes()
	return False

def getConfiguration(dev):
	config = dev.GetConfiguration()
	print
	print "Configuration"
	print config
	print
	return False

def do_something(dev):
	# print dev.AbortAssociation()
	# print dev.Connect()
	# print dev.TestAgent()
	# print dev.RequestMeasurementDataTransmission()
	# print dev.RequestActivationScanner(55)
	# print dev.RequestDeactivationScanner(55)
	# print dev.GetSegmentInfo()
	# print dev.GetSegmentData()
	# print dev.ClearSegmentData()
	# print dev.ReleaseAssociation()
	# print dev.Disconnect()
	return False

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

bus = dbus.SystemBus()

obj = bus.get_object("com.signove.health", "/com/signove/health")
srv = dbus.Interface(obj, "com.signove.health.manager")
agent = Agent(bus, "/com/signove/health/agent/%d" % os.getpid())

print "Configuring..."
srv.ConfigurePassive(agent, [0x1004, 0x1007, 0x1029, 0x100f])
print "Waiting..."

mainloop = glib.MainLoop()
mainloop.run()
