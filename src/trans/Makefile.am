if BUILD_LINUX
	MAYBE_OPT = plugin
endif
SUBDIRS = $(MAYBE_OPT)

INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

noinst_LTLIBRARIES = libtrans.la

libtrans_la_SOURCES = trans.c

noinst_HEADERS = trans.h
