INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src


noinst_LTLIBRARIES = libbluezplugin.la

libbluezplugin_la_SOURCES = plugin_glib_socket.c plugin_bluez.c

noinst_HEADERS = plugin_glib_socket.h plugin_bluez.h

libbluezplugin_la_CFLAGS = @GLIB_CFLAGS@ @GIO_CFLAGS@        
                 
