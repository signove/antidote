SUBDIRS = api dim communication functional_test_cases
INCLUDES =  -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src @DBUS_CFLAGS@ @USB1_CFLAGS@


bin_PROGRAMS = main_test_suite ieee_manager_console


#Main Test Suite application
main_test_suite_SOURCES = main_test_suite.c testtimer.c  testlinkedlist.c
main_test_suite_LDADD = dim/libtestdim.a \
                        api/libtestxml.a \
                        functional_test_cases/libtestfunctional.a \
                        ../src/.libs/libantidote.a \
                        communication/encoder/libtestencoder.a \
                        communication/parser/libtestparser.a \
                        communication/libtestcom.a \
                        ../src/communication/plugin/.libs/libcommpluginimpl.a

#Main Test Console
ieee_manager_console_SOURCES = main_test_console.c
ieee_manager_console_LDADD =   \
             ../src/.libs/libantidote.a \
             ../src/communication/plugin/.libs/libcommpluginimpl.a
