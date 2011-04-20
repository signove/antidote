SUBDIRS = parser encoder

INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src

noinst_LIBRARIES = libtestcom.a

libtestcom_a_SOURCES = testfsm.c \
                       testservice.c \
                       testcontextmanager.c \
                       testextconfiguration.c

noinst_HEADERS = testfsm.h \
                 testservice.h \
                 testextconfiguration.h \
                 testcontextmanager.h

