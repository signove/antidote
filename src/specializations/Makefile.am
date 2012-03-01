INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

noinst_LTLIBRARIES = libspecializations.la

libspecializations_la_SOURCES = \
				blood_pressure_monitor.c \
				pulse_oximeter.c \
				weighing_scale.c \
				glucometer.c

@PACKAGE@_include_specializationsdir = $(pkgincludedir)/specializations

@PACKAGE@_include_specializations_HEADERS = \
				blood_pressure_monitor.h \
				pulse_oximeter.h \
				weighing_scale.h \
				glucometer.h
