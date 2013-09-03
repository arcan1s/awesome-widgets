#!/bin/bash

# build dataengine
ARCHIVE="ext-sysmon"
VERSION=`grep Version ext-sysmon/plasma-engine-extsysmon.desktop | awk -F "=" '{print $2}'`
# create archive
if [ -e ${ARCHIVE}-${VERSION}.zip ]; then
  rm -f ${ARCHIVE}-${VERSION}.zip
fi
zip -9 -y -r -q ${ARCHIVE}-${VERSION}.zip ${ARCHIVE}
# update md5sum
MD5SUMS=`md5sum ${ARCHIVE}-${VERSION}.zip | awk '{print $1}'`
sed -i "/         '[0-9A-Fa-f]*/s/[^'][^)]*/         '${MD5SUMS}'/" PKGBUILD
sed -i "s/_dtver=[0-9.]*/_dtver=${VERSION}/" PKGBUILD

# build widget
cd sources
FILES="contents metadata.desktop"
ARCHIVE="py-text-monitor"
VERSION=`grep Version metadata.desktop | awk -F "=" '{print $2}'`
# create archive
if [ -e ${ARCHIVE}-${VERSION}.plasmoid ]; then
  rm -f ${ARCHIVE}-${VERSION}.plasmoid
fi
zip -9 -y -r -q ../${ARCHIVE}-${VERSION}.plasmoid ${FILES}
cd ..
# update md5sum
MD5SUMS=`md5sum ${ARCHIVE}-${VERSION}.plasmoid | awk '{print $1}'`
sed -i "/md5sums=('[0-9A-Fa-f]*/s/[^'][^)]*/md5sums=('${MD5SUMS}'/" PKGBUILD
sed -i "s/pkgver=[0-9.]*/pkgver=${VERSION}/" PKGBUILD
