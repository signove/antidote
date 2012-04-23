#!/bin/sh

dpkg-buildpackage -rfakeroot -tc -uc -us $*

GEN_STATUS=$?

if test $GEN_STATUS -eq 0
then
	echo "Packages generated"
else
	echo "Error genetating packages"
fi

exit $GEN_STATUS
