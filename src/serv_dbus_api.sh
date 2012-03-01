doxy ()
{
echo '/**                        ' >> tmppiggy
echo ' * \cond Undocumented     ' >> tmppiggy
echo ' */                        ' >> tmppiggy
echo >> tmppiggy

cat $1 >> tmppiggy

echo '/**              ' >> tmppiggy
echo ' * \endcond     ' >> tmppiggy
echo ' */              ' >> tmppiggy

mv -f tmppiggy $1
}

dbus-binding-tool --mode=glib-server --prefix=srv serv_dbus_api.xml > serv_dbus_api.h
dbus-binding-tool --mode=glib-server --prefix=device serv_dbus_api_device.xml > serv_dbus_api_device.h

doxy serv_dbus_api.h
doxy serv_dbus_api_device.h
