dbus-binding-tool --mode=glib-server --prefix=srv serv_dbus_api.xml > serv_dbus_api.h
dbus-binding-tool --mode=glib-server --prefix=device serv_dbus_api_device.xml > serv_dbus_api_device.h

# FIXME add cond Undocumented fence 
