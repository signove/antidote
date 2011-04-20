#!/bin/sh

rm -rfv build
./autogen.sh && \
./configure && \
make && \
dpkg-buildpackage -rfakeroot

GEN_STATUS=$?

if test $GEN_STATUS -eq 0
then
	echo "Packages generated"
else
	echo "Error genetating packages"
fi

exit $GEN_STATUS
