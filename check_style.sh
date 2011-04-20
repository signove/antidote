#!/bin/bash
rm -rf vazio cheio
mkdir -p vazio
# astyle --style=linux --indent=force-tab=8 -k3 -f $(find . -name '*.c' -o -name '*.h')
mkdir cheio
( tar cf - $(find . \( -name '*.c' -o -name '*.h' \) -a ! \( -wholename './src/serv_dbus_api_device.h' -o -wholename './src/serv_dbus_api.h' \) ) ) | (cd cheio; tar xf -)
diff -urN vazio cheio > p.diff
./checkpatch.pl p.diff > comentarios.txt
rm -rf vazio cheio
echo
ls -lsa comentarios.txt
