INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_builddir)/src

noinst_LTLIBRARIES = libapi.la

libapi_la_SOURCES = text_encoder.c \
					data_encoder.c \
					json_encoder.c \
					xml_encoder.c \
					oid_string.c

noinst_HEADERS = api_definitions.h \
				 text_encoder.h \
				 data_encoder.h \
				 json_encoder.h \
				 xml_encoder.h	\
				 oid_string.h
