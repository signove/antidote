INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

#Bin Programs
bin_PROGRAMS = ieee_manager ieee_agent sample_bt_agent healthd

# Minimal sample app to use the IEEE protocol facade
ieee_manager_SOURCES = sample_manager.c

ieee_manager_LDADD = \
             ../src/communication/plugin/libcommpluginimpl.la \
             ../src/libantidote.la

# Minimal sample app to use the IEEE protocol facade (agent)
ieee_agent_SOURCES = sample_agent.c

ieee_agent_LDADD = \
             ../src/communication/plugin/libcommpluginimpl.la \
             ../src/libantidote.la

# Sample agent that uses Bluetooth (BlueZ) plug-in
sample_bt_agent_SOURCES = sample_bt_agent.c
sample_bt_agent_CFLAGS = @GLIB_CFLAGS@ @DBUS_CFLAGS@

sample_bt_agent_LDADD = \
		../src/communication/plugin/bluez/libbluezplugin.la \
		../src/libantidote.la \
		@GLIB_LIBS@ \
		@GIO_LIBS@ \
		@DBUS_LIBS@ \
		@DBUS_GLIB_LIBS@

#healthd: D-BUS Service for IEEE protocol facade              
healthd_SOURCES = healthd_service.c
healthd_CFLAGS = @DBUS_CFLAGS@ @GLIB_CFLAGS@ @GIO_CFLAGS@

healthd_LDADD = \
             ../src/communication/plugin/bluez/libbluezplugin.la \
             ../src/communication/plugin/usb/libusbplugin.la \
             ../src/communication/plugin/trans/libtransplugin.la \
             ../src/trans/plugin/libtransexampleoximeterplugin.la \
             ../src/libantidote.la \
             @GLIB_LIBS@ \
             @GIO_LIBS@ \
             @DBUS_LIBS@ \
             @DBUS_GLIB_LIBS@ \
	     @USB1_LIBS@

install-data-local: healthd.conf
	$(mkinstalldirs) $(DESTDIR)/etc/dbus-1/system.d
	$(INSTALL_DATA) $(srcdir)/healthd.conf $(DESTDIR)/etc/dbus-1/system.d/
