#!/usr/bin/python

from xml.dom.minidom import *

def childrenByTag(node, tag):
	return [ n for n in node.childNodes if n.nodeName == tag ]

def getText(node):
    rc = []
    for node in node.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return ''.join(rc)


def get_system_id_from_mds(xmldata):
	sid = ""

	if not xmldata:
		return sid

	try:
		doc = parseString(xmldata)
	except:
		return sid

	for entry in doc.getElementsByTagName("entry"):
		for simple in entry.getElementsByTagName("simple"):
			for name in simple.getElementsByTagName("name"):
				sname = getText(name).strip()
				if "System-Id" != sname:
					continue
				values = simple.getElementsByTagName("value")
				if values:
					sid = getText(values[0]).strip()
					return sid
	return sid


def beautify(xmldata):
	try:
		doc = parseString(xmldata)
	except:
		print "Bad XML, keeping it ugly"
		return xmldata
	return doc.toprettyxml(indent="   ")


class DataList(object):
	def __init__(self, xmldata):

		self.valid = False
		self.entries = []
		self.entries_map = {}
		try:
			doc = parseString(xmldata)
		except:
			print "Can not parse xml data"
			return

		datalists = childrenByTag(doc, "data-list")
		if len(datalists) != 1:
			print "Invalid data list count %d" % len(datalists)
			return

		datalist = datalists[0]

		for entry in childrenByTag(datalist, "entry"):
			try:
				o = Entry(entry)
			except InvalidEntryNode:
				continue
			self.entries.append(o)
			self.entries_map[o.name] = o

		self.status = 1


class InvalidEntryNode(Exception):
	pass


class Entry(object):
	def parse_meta(self, node):
		m = {}
		metas = childrenByTag(node, "meta-data")
		if len(metas) > 1:
			print "Error meta-data %d" % len(metas)
			raise InvalidEntryNode()
		elif not metas:
			return m

		metas = childrenByTag(metas[0], "meta")
		for meta in metas:
			name, value = self.parse_metadata(meta)
			m[name] = value
		return m
	
	def parse_metadata(self, node):
		if not node.hasAttributes():
			print "No attribute in meta data"
			raise InvalidEntryNode()
		name = node.getAttribute('name')
		if not name:
			print "No name attribute in meta data"
			raise InvalidEntryNode()
		value = getText(node).strip()
		return name, value

	def detect_type(self, node):
		simple = childrenByTag(node, "simple")
		compound = childrenByTag(node, "compound")
		if len(simple) + len(compound) != 1:
			print "Not a single simple/compound element"
			raise InvalidEntryNode()
		is_compound = len(compound) > 0
		if is_compound:
			cnode = compound[0]
		else:
			cnode = simple[0]
		return is_compound, cnode

	def parse_name(self, node):
		n = childrenByTag(node, "name")
		if not n:
			print "Nameless entry"
			raise InvalidEntryNode()
		return getText(n[0]).strip()

	def parse_children(self, node):
		l = []
		d = {}
		cc = childrenByTag(node, "entries")
		if len(cc) != 1:
			print "Compound with wrong number off entries tag"
			raise InvalidEntryNode()
		c = childrenByTag(cc[0], "entry")
		for entry in c:
			try:
				o = Entry(entry)
			except InvalidEntryNode:
				continue
			l.append(o)
			d[o.name] = o

		return l, d

	def parse_simple(self, node):
		t = childrenByTag(node, "type")
		v = childrenByTag(node, "value")
		if not t:
			print "Typeless entry"
			raise InvalidEntryNode()
		if not v:
			print "Valueless entry"
			raise InvalidEntryNode()
		return getText(t[0]).strip(), getText(v[0]).strip()

	def __init__(self, node):
		self.meta = self.parse_meta(node)
		is_compound, cnode = self.detect_type(node)
		self.name = self.parse_name(cnode)
		if is_compound:
			self.entries, self.entries_map = self.parse_children(cnode)
			self.compound = True
		else:
			self.dtype, self.value = self.parse_simple(cnode)
			self.compound = False


# Objects that interpret a data list

class Configuration(object):
	def __init__(self, datalist):
		self.data = datalist

	def describe(self):
		print
		print "Configuration"
		for obj in self.data.entries:
			print "\t" + obj.name,
			if obj.name == "Numeric":
				print " unit ",
				print obj.entries_map["Unit-Code"].value
			elif obj.name == "PM-Store":
				print " handle ",
				print obj.meta["HANDLE"]
			else:
				print

class Measurement(object):
	def __init__(self, datalist):
		self.data = datalist
		self.handlers = {}
		self.handlers["Simple-Nu-Observed-Value"] = self.simple_nu
		self.handlers["Basic-Nu-Observed-Value"] = self.basic_nu

	def unit(self, entry):
		if "unit" in entry.meta:
			return entry.meta["unit"]
		return "(unit %s)" % entry.meta["unit-code"]

	def simple_nu(self, entry):
		print entry.value + self.unit(entry),

	def basic_nu(self, entry):
		print entry.value + self.unit(entry),

	def describe(self):
		print
		print "Measurement ",
	
		for obj in self.data.entries:
			if not obj.entries:
				continue
			if obj.entries[0].name in self.handlers:
				handler = self.handlers[obj.entries[0].name]
				handler(obj.entries[0])
			else:
				print obj.name,

		print


class DeviceAttributes(object):
	def __init__(self, datalist):
		self.data = datalist

	def describe(self):
		print "Device Attributes"
		


class PMStore(object):
	def __init__(self, datalist):
		self.data = datalist


class SegmentInfo(object):
	def __init__(self, datalist):
		self.data = datalist


class SegmentData(object):
	def __init__(self, datalist):
		self.data = datalist


# testing

if __name__ == '__main__':
	import sys
	f = sys.argv[1]
	d = DataList(file(f).read())

	def print_entries(d, indent):
		for e in d.entries:
			print indent + "* " + e.name
			if e.meta:
				print " " + str(e.meta)
			if not e.compound:
				print indent + " Type: " + e.dtype,
				print " Value: " + e.value
			else:
				print_entries(e, indent + "    ")

	# print_entries(d, "")

	if f.find("config") >= 0:
		Configuration(d).describe()
	elif f.find("measure") >= 0:
		Measurement(d).describe()
	elif f.find("assoc") >= 0:
		DeviceAttributes(d).describe()
	elif f.find("attrib") >= 0:
		DeviceAttributes(d).describe()
	elif f.find("pmstore") >= 0:
		PMStore(d).describe()
	elif f.find("segmentinfo") >= 0:
		SegmentInfo(d).describe()
	elif f.find("segmentdata") >= 0:
		SegmentData(d).describe()
	
	sys.exit(0)

	print
	print 
	print

	def print_nodes(d, indent=""):
		print indent + d.nodeName
		for c in d.childNodes:
			print_nodes(c, indent + "   ")	

	d = parseString(file(sys.argv[1]).read())
	print_nodes(d)

