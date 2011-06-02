SUBDIRS = parser plugin

INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

noinst_LTLIBRARIES = libcom.la

libcom_la_SOURCES = association.c \
                   communication.c \
                   configuring.c \
                   disassociating.c \
		   agent_ops.c \
                   extconfigurations.c \
                   fsm.c \
                   service.c \
                   operating.c \
                   stdconfigurations.c \
                   context_manager.c

noinst_HEADERS = association.h \
                 communication.h \
                 configuring.h \
                 disassociating.h \
                 extconfigurations.h \
		 agent_ops.h \
                 fsm.h \
                 service.h \
                 operating.h \
                 stdconfigurations.h \
                 context_manager.h
