INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src


noinst_LTLIBRARIES = libusbplugin.la

libusbplugin_la_SOURCES = plugin_usb.c usb_phdc_drive.c

noinst_HEADERS = plugin_usb.h usb_phdc_definitions.h

libusbplugin_la_CFLAGS = @GLIB_CFLAGS@ @GIO_CFLAGS@ @USB1_CFLAGS@


bin_PROGRAMS = simple_main

simple_main_SOURCES = simple_main.c \
					usb_phdc_drive.c

simple_main_CFLAGS = @USB1_CFLAGS@

simple_main_LDADD = @USB1_LIBS@