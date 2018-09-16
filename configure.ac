AC_INIT(antidote, 2.1)

PACKAGE=antidote
VERSION=2.1

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

AM_PROG_CC_C_O

AC_CHECK_HEADER([stdio.h])

build_linux=no
build_mac=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

AC_ARG_ENABLE([tests], \
              [AS_HELP_STRING([--enable-tests], \
              [Enable tests compilation, \
              enabled by default ])])

if test "$enable_tests" = no; then
	have_tests="no"
else
	AC_CHECK_HEADER(/usr/include/CUnit/CUnit.h, \
	                [have_tests="yes"], \
	                [have_tests="no"],[])
fi

#Avoids a warning in pthread_mutexattr_settype
AC_DEFINE(_GNU_SOURCE)

#Enabling test compilation
if test "$have_tests" = yes; then 
	AC_MSG_NOTICE([ -- Tests enabled.])
	CUNIT_CFLAGS="-I/usr/include/CUnit -Itests"
	CUNIT_LIBS="-lcunit"
	AC_DEFINE([TEST_ENABLED], 1, [])
else
	AC_MSG_NOTICE([ -- Tests disabled.])
fi

AC_ARG_ENABLE([coverage], \
              [AS_HELP_STRING([--enable-coverage], \
              [Enable code coverage])])
    
if test "$enable_coverage" = "yes"; then 
	AC_MSG_NOTICE([ -- Code coverage enabled.])
	CFLAGS="$CFLAGS  -fprofile-arcs -ftest-coverage -lgcov -O0"
fi

if test "$build_linux" = "yes"; then
	#Enabling D-BUS network module
	PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.4.0])
	PKG_CHECK_MODULES(GLIB, glib-2.0)
	PKG_CHECK_MODULES(GIO, gio-2.0)
	PKG_CHECK_MODULES(DBUS_GLIB, dbus-glib-1)
	#usb libraries
	PKG_CHECK_MODULES(USB1, libusb-1.0)
fi

CFLAGS="$CFLAGS $CUNIT_CFLAGS -Isrc -g -pthread -Wall -Wextra -Wno-unused-parameter -Werror -fPIC"
LIBS="$LIBS -lm $CUNIT_LIBS"

AM_CONDITIONAL([BUILD_LINUX], [test "x$build_linux" = xyes])
AM_CONDITIONAL([BUILD_MAC], [test "x$build_mac" = xyes])

AC_CONFIG_FILES([Makefile \
          apps/Makefile \
          src/Makefile \
          src/api/Makefile \
          src/dim/Makefile \
          src/util/Makefile \
          src/communication/Makefile \
          src/communication/plugin/Makefile \
          src/communication/parser/Makefile \
	  src/trans/Makefile \
          src/specializations/Makefile \
          src/antidote.pc \
          sdk/Makefile \
          tests/Makefile \
          tests/api/Makefile \
          tests/dim/Makefile \
          tests/communication/Makefile \
          tests/communication/parser/Makefile \
          tests/communication/encoder/Makefile \
          tests/functional_test_cases/Makefile] \
          )

AM_COND_IF([BUILD_LINUX],
	[AC_CONFIG_FILES([
          src/communication/plugin/bluez/Makefile \
          src/communication/plugin/usb/Makefile \
          src/communication/plugin/trans/Makefile \
	  src/trans/plugin/Makefile \
          ])])

AC_OUTPUT
