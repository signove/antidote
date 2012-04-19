SUBDIRS = dim util communication specializations api trans

INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

# Lib
lib_LTLIBRARIES = libantidote.la
libantidote_la_SOURCES = manager.c agent.c
libantidote_la_LIBADD =  \
                            api/libapi.la \
                            communication/libcom.la \
                            communication/plugin/libcommplugin.la \
                            communication/parser/libparser.la \
			    trans/libtrans.la \
                            dim/libdim.la \
                            util/libutil.la \
                            specializations/libspecializations.la

# pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = antidote.pc

# dbus stuff
#dbusstuffdir = $(sharedir)/dbus-1/system-services/
#dbusstuff_DATA = src/resources/com.signove.health.service

#Install headers
@PACKAGE@_includedir = $(pkgincludedir)
@PACKAGE@_include_HEADERS = manager.h \
			    agent.h \
                            ieee11073.h
@PACKAGE@_include_apidir = $(pkgincludedir)/api
@PACKAGE@_include_api_HEADERS = api/api_definitions.h \
                                api/data_list.h \
                                api/json_encoder.h \
                                api/text_encoder.h \
                                api/xml_encoder.h
@PACKAGE@_include_asn1dir = $(pkgincludedir)/asn1
@PACKAGE@_include_asn1_HEADERS = asn1/phd_types.h
@PACKAGE@_include_communicationdir = $(pkgincludedir)/communication
@PACKAGE@_include_communication_HEADERS = communication/context.h \
					communication/context_manager.h \
					communication/service.h \
					communication/fsm.h \
					communication/stdconfigurations.h \
					communication/communication.h
@PACKAGE@_include_dimdir = $(pkgincludedir)/dim
@PACKAGE@_include_dim_HEADERS = dim/mds.h \
				dim/numeric.h \
				dim/pmstore.h \
				dim/pmstore_req.h \
				dim/scanner.h \
				dim/peri_cfg_scanner.h \
				dim/epi_cfg_scanner.h \
				dim/cfg_scanner.h \
				dim/nomenclature.h \
				dim/pmsegment.h \
				dim/rtsa.h \
				dim/metric.h \
				dim/enumeration.h \
				dim/dim.h
@PACKAGE@_include_plugindir = $(pkgincludedir)/communication/plugin
@PACKAGE@_include_plugin_HEADERS = communication/plugin/plugin.h \
                                   communication/plugin/plugin_tcp.h \
                                   communication/plugin/plugin_tcp_agent.h
@PACKAGE@_include_utildir = $(pkgincludedir)/util
@PACKAGE@_include_util_HEADERS = util/bytelib.h
