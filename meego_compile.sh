./autogen.sh || exit 1

mad ./configure --prefix=/usr --enable-tests=no --target=arm --host=`uname -m` DBUS_CFLAGS="`pkg-config --cflags dbus-1`" DBUS_LIBS="`pkg-config --libs dbus-1`" GLIB_CFLAGS="`pkg-config --cflags glib-2.0`" GLIB_LIBS="`pkg-config --libs glib-2.0`" DBUS_GLIB_CFLAGS="`pkg-config --cflags dbus-glib-1`" DBUS_GLIB_LIBS="`pkg-config --libs dbus-glib-1`" || exit 1

mad make || exit 1

