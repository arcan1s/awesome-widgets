#!/bin/bash

SRCDIR="sources"
VERSION="$(git describe --tags --abbrev=0)"

# build widget
ARCHIVE="awesome-widgets"
FILES="AUTHORS CHANGELOG COPYING packages patches sources"
IGNORELIST="build usr .kdev4 *.kdev4 .idea packages/*src.tar.xz"
# create archive
[[ -e ${ARCHIVE}-${VERSION}-src.tar.xz ]] && rm -f "${ARCHIVE}-${VERSION}-src.tar.xz"
[[ -d ${ARCHIVE} ]] && rm -rf "${ARCHIVE}"

cp -r "${SRCDIR}" "${ARCHIVE}"
for FILE in ${FILES[*]}; do cp -r "$FILE" "${ARCHIVE}"; done
for FILE in ${IGNORELIST[*]}; do rm -rf "${ARCHIVE}/${FILE}"; done

tar cJf "${ARCHIVE}-${VERSION}-src.tar.xz" "${ARCHIVE}"

rm -rf "${ARCHIVE}"
