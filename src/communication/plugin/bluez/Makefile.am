INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src


lib_LTLIBRARIES = libbluezplugin.la

libbluezplugin_la_SOURCES = plugin_glib_socket.c plugin_bluez.c

@PACKAGE@_include_plugindir = $(pkgincludedir)/communication/plugin
@PACKAGE@_include_plugin_HEADERS = plugin_glib_socket.h plugin_bluez.h

libbluezplugin_la_CFLAGS = @GLIB_CFLAGS@ @GIO_CFLAGS@ @DBUS_CFLAGS@
