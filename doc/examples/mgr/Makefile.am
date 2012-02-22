bin_PROGRAMS = mgr agt

mgr_SOURCES = mgr.c
mgr_CFLAGS = @GLIB_CFLAGS@ @ANTIDOTE_CFLAGS@

agt_SOURCES = agt.c
agt_CFLAGS = @GLIB_CFLAGS@ @ANTIDOTE_CFLAGS@

mgr_LDADD = \
             @ANTIDOTE_LIBS@ \
	     /usr/local/lib/libbluezplugin.la \
             @GLIB_LIBS@ \
             @GIO_LIBS@ \
             @DBUS_LIBS@ \
             @DBUS_GLIB_LIBS@

agt_LDADD = \
             @ANTIDOTE_LIBS@ \
	     /usr/local/lib/libbluezplugin.la \
             @GLIB_LIBS@ \
             @GIO_LIBS@ \
             @DBUS_LIBS@ \
             @DBUS_GLIB_LIBS@
