#!/usr/bin/python

import sys
import dbus
import re
import dbus.service
import dbus.mainloop.glib
import os
import glib
from xml.dom.minidom import *

dump_prefix = "XML"

# TODO create module to interpret XML and show condensed results

def beautify(xmldata):
	try:
		doc = parseString(xmldata)
	except:
		print "Bad XML, keeping it ugly"
		return xmldata
	return doc.toprettyxml(indent="   ")

def dump(suffix, xmldata):
	xmldata = beautify(xmldata)
	f = open(dump_prefix + "_" + suffix + ".xml", "w")
	f.write(xmldata)
	f.close()

gsdr = {0: "Success", 1: "Segment unknown", 2: "Fail, try later",
	3: "Fail, segment empty", 512: "Fail, other"}

def getsegmentdata_response_interpret(i):
	try:
		s = gsdr[i]
	except KeyError:
		s = "Unknown fail code"
	return s

pmstore_handle = 11
pmsegment_instance = 0
clear_segment = 0
get_segment = 0
get_pmstore = 0
get_mds = 0

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
		print "Associated dev %s: XML with %d bytes" % (dev, len(xmldata))

		dump("associated", xmldata)
		
		# Convert path to an interface
		dev = bus.get_object("com.signove.health", dev)
		dev = dbus.Interface(dev, "com.signove.health.device")

		glib.timeout_add(0, getConfiguration, dev)
		if clear_segment == 1:
			glib.timeout_add(1000, clearSegment, dev, pmstore_handle, pmsegment_instance)
			return
		elif clear_segment == 2:
			glib.timeout_add(1000, clearAllSegments, dev, pmstore_handle)
			return

		if get_mds:
			glib.timeout_add(1000, requestMdsAttributes, dev)
		if get_pmstore:
			glib.timeout_add(2000, getPMStore, dev, pmstore_handle)
			glib.timeout_add(3000, getSegmentInfo, dev, pmstore_handle)
		if get_segment:
			glib.timeout_add(5000, getSegmentData, dev, pmstore_handle, pmsegment_instance)

	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def MeasurementData(self, dev, xmldata):
		print
		print "MeasurementData dev %s" % dev
		print "=== Data: ", xmldata
		dump("measurement", xmldata)

	@dbus.service.method("com.signove.health.agent", in_signature="sis", out_signature="")
	def PMStoreData(self, dev, pmstore_handle, xmldata):
		print
		print "PMStore dev %s handle %d" % (dev, pmstore_handle)
		print "=== Data: XML with %d bytes" % len(xmldata)
		dump("pmstore_%d" % pmstore_handle, xmldata)

	@dbus.service.method("com.signove.health.agent", in_signature="sis", out_signature="")
	def SegmentInfo(self, dev, pmstore_handle, xmldata):
		print
		print "SegmentInfo dev %s PM-Store handle %d" % (dev, pmstore_handle)
		print "=== XML with %d bytes" % len(xmldata)
		# print "\tData:\t", data
		dump("segmentinfo_%d" % pmstore_handle, xmldata)

	@dbus.service.method("com.signove.health.agent", in_signature="siii", out_signature="")
	def SegmentDataResponse(self, dev, pmstore_handle, pmsegment, response):
		print
		print "SegmentDataResponse dev %s PM-Store handle %d" % (dev, pmstore_handle)
		print "=== InstNumber %d" % pmsegment
		print "=== Response %s" % getsegmentdata_response_interpret(response)
		if response != 0 and pmsegment < 7:
			dev = bus.get_object("com.signove.health", dev)
			dev = dbus.Interface(dev, "com.signove.health.device")
			glib.timeout_add(0, getSegmentData, dev, pmstore_handle, pmsegment + 1)

	@dbus.service.method("com.signove.health.agent", in_signature="siis", out_signature="")
	def SegmentData(self, dev, pmstore_handle, pmsegment, xmldata):
		print
		print "SegmentData dev %s PM-Store handle %d" % (dev, pmstore_handle)
		print "=== InstNumber %d" % pmsegment
		print "=== Data: %d bytes XML" % len(xmldata)
		dump("segmentdata_%d_%d" % (pmstore_handle, pmsegment), xmldata)

	@dbus.service.method("com.signove.health.agent", in_signature="siii", out_signature="")
	def SegmentCleared(self, dev, pmstore_handle, pmsegment, retstatus):
		print
		print "SegmentCleared dev %s PM-Store handle %d" % (dev, pmstore_handle)
		print "=== InstNumber %d retstatus %d" % (pmsegment, retstatus)
		print
		
	@dbus.service.method("com.signove.health.agent", in_signature="ss", out_signature="")
	def DeviceAttributes(self, dev, xmldata):
		print
		print "DeviceAttributes dev %s" % dev
		print "=== Data: XML with %d bytes" % len(xmldata)
		dump("attributes", xmldata)

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
	print "Configuration: XML with %d bytes" % len(config)
	print
	dump("config", config)
	return False

def getSegmentInfo(dev, handle):
	ret = dev.GetSegmentInfo(handle)
	print
	print "GetSegmentInfo ret %d" % ret
	print
	return False

def getSegmentData(dev, handle, instance):
	ret = dev.GetSegmentData(handle, instance)
	print
	print "GetSegmentData ret %d" % ret
	print
	return False

def clearSegment(dev, handle, instance):
	ret = dev.ClearSegment(handle, instance)
	print
	print "ClearSegment ret %d" % ret
	print
	return False

def clearAllSegments(dev, handle):
	ret = dev.ClearAllSegments(handle)
	print
	print "ClearAllSegments ret %d" % ret
	print
	return False

def getPMStore(dev, handle):
	ret = dev.GetPMStore(handle)
	print
	print "GetPMStore ret %d" % ret
	print
	return False

def do_something(dev):
	# print dev.AbortAssociation()
	# print dev.Connect()
	# print dev.RequestMeasurementDataTransmission()
	# print dev.RequestActivationScanner(55)
	# print dev.RequestDeactivationScanner(55)
	# print dev.ReleaseAssociation()
	# print dev.Disconnect()
	return False

args = sys.argv[1:]

i = 0
while i < len(args):
	arg = args[i]

	if arg == '--mds':
		get_mds = 1
	elif arg == '--prefix':
		dump_prefix = args[i + 1]
		i += 1
	elif arg == '--segment':
		get_segment = 1
		get_pmstore = 1
	elif arg == '--clear-segment':
		clear_segment = 1
		get_pmstore = 1
	elif arg == '--clear-all-segments':
		clear_segment = 2
		get_pmstore = 1
	elif arg == '--store' or arg == '--pmstore' or arg == '--pm-store':
		pmstore_handle = int(args[i + 1])
		get_pmstore = 1
		i += 1
	elif arg == '--instance' or arg == '--inst' or arg == '--segment' or \
				arg == '--pm-segment'  or arg == '--pmsegment':
		pmsegment_instance = int(args[i + 1])
		i += 1
	else:
		raise Exception("Invalid argument %s" % arg)

	i += 1
	

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
