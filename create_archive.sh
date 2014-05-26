#!/bin/bash

# update english readme
cp README.md md_readme/en.md
# create html pages
for MARKDOWN in $(find md_readme -name '*.md'); do
  FILENAME=$(basename -- "${MARKDOWN}")
  FILENAME="${FILENAME%.*}"
  markdown "${MARKDOWN}" > "sources/readme/${FILENAME}.html"
done

SRCDIR="sources"
VERSION=$(grep -m1 PROJECT_VERSION_MAJOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_MINOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_PATCH sources/CMakeLists.txt | awk '{print $3}' | cut -c 1)

# build dataengine
ARCHIVE="ext-sysmon"
# create archive
[[ -e ${ARCHIVE}-${VERSION}-src.tar.xz ]] && rm -f ${ARCHIVE}-${VERSION}-src.tar.xz
[[ -d ${ARCHIVE} ]] && rm -rf "${ARCHIVE}"
cp -r "${SRCDIR}/${ARCHIVE}" "${ARCHIVE}"
tar cJf "${ARCHIVE}-${VERSION}-src.tar.xz" "${ARCHIVE}"
rm -rf "${ARCHIVE}"

# build widget
ARCHIVE="pytextmonitor"
FILES="AUTHORS CHANGELOG CHANGELOG-RU COPYING"
IGNORELIST="build usr"
# create archive
[[ -e ${ARCHIVE}-${VERSION}-src.tar.xz ]] && rm -f "${ARCHIVE}-${VERSION}-src.tar.xz"
[[ -d ${ARCHIVE} ]] && rm -rf "${ARCHIVE}"
cp -r "${SRCDIR}" "${ARCHIVE}"
for FILE in ${FILES[*]}; do cp -r "$FILE" "${ARCHIVE}"; done
for FILE in ${IGNORELIST[*]}; do rm -rf "${ARCHIVE}/${FILE}"; done
tar cJf "${ARCHIVE}-${VERSION}-src.tar.xz" "${ARCHIVE}" 
rm -rf "${ARCHIVE}"

# update md5sum
MD5SUMS=$(md5sum ${ARCHIVE}-${VERSION}-src.tar.xz | awk '{print $1}')
sed -i "/md5sums=('[0-9A-Fa-f]*/s/[^'][^)]*/md5sums=('${MD5SUMS}'/" PKGBUILD
sed -i "s/pkgver=[0-9.]*/pkgver=${VERSION}/" PKGBUILD
