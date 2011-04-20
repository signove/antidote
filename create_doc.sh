#!/bin/bash

TMP_FILE=`mktemp`

cd doc

doxygen doxygen.Doxyfile 2>$TMP_FILE>/dev/null

LN=`cat $TMP_FILE | wc -l`
WC=0
EC=0
RESULT=0

if [ $LN -ne 0 ]; then
	WC=`cat $TMP_FILE | grep warning: |wc -l`
	EC=`cat $TMP_FILE | grep error: |wc -l`
	RESULT=1
echo "**** Doxygen warnings and errors ****"
	cat $TMP_FILE
fi

echo "***** Summary of Doc generation *****"
echo "Totals: $WC warnings and $EC errors"

cd ..
rm $TMP_FILE
exit $RESULT

