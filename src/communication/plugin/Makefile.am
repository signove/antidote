SUBDIRS = usb bluez trans

INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

noinst_LTLIBRARIES = libcommplugin.la libcommpluginimpl.la

libcommplugin_la_SOURCES = plugin.c

libcommpluginimpl_la_SOURCES = \
                   plugin_fifo.c \
                   plugin_tcp.c \
                   plugin_tcp_agent.c \
		   plugin_pthread.c

noinst_HEADERS = plugin.h \
                   plugin_fifo.h \
                   plugin_tcp.h \
                   plugin_tcp_agent.h \
		   plugin_pthread.h

