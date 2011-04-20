#!/bin/bash

CHANGELOG_FILE=debian/changelog
TMP_FILE=tmp_changelog

CURRENT_VERSION=`cat $CHANGELOG_FILE | grep '(' | head -n 1 | cut -d'(' -f2 | cut -d')' -f1`

TODAY=`date +%Y%m%d%H%M%S`
TODAY_CHANGELOG=`date -R`
NEW_VERSION=${CURRENT_VERSION}-${TODAY}

echo "antidote (${NEW_VERSION}) unstable; urgency=low" > $TMP_FILE
echo "" >> $TMP_FILE
echo "  * Automatic daily release generate by Continuous Integration Tool" >> $TMP_FILE
echo "" >> $TMP_FILE
echo " -- Continuous Integration <health-team@signove.com>  ${TODAY_CHANGELOG}" >> $TMP_FILE
echo "" >> $TMP_FILE

cat ${CHANGELOG_FILE} >> $TMP_FILE
mv $TMP_FILE ${CHANGELOG_FILE}

