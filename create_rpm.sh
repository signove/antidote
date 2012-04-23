#!/bin/sh

APPNAME=antidote
APPSPEC=$APPNAME.spec

mkdir -p ~/rpmbuild/SOURCES/
mkdir -p ../tmp

# svn export `pwd` ../tmp/$APPNAME || exit
cp rpmstuff/$APPSPEC ../tmp || exit

cd ..
tar czvf ./tmp/$APPNAME.tar.gz ./$APPNAME || exit
cd ./tmp
mv $APPNAME.tar.gz ~/rpmbuild/SOURCES/ || exit
rm -rf $APPNAME || exit

# use --nodeps if test-building in a non-RPM environment
rpmbuild --nodeps $APPSPEC || exit
rpmbuild --nodeps -ba $APPSPEC || exit

echo "See ~/rpmbuild/RPMS/ directory"

