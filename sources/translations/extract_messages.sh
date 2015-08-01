#!/bin/bash
# root of translatable sources
BASEDIR="../"
PROJECT="awesome-widgets"
BUGADDR="https://github.com/arcan1s/awesome-widgets/issues"
WORKDIR=$(pwd)

find "${BASEDIR}" -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > "${WORKDIR}/rcfiles.list"
xargs --arg-file="${WORKDIR}/rcfiles.list" > "${WORKDIR}/rc.cpp"
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> "${WORKDIR}/rc.cpp"
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> "${WORKDIR}/rc.cpp"

find "${BASEDIR}" -name '*.cpp' -o -name '*.h' -o -name '*.qml' | sort > "${WORKDIR}/infiles.list"
echo "rc.cpp" >> "${WORKDIR}/infiles.list"

xgettext -C --no-location --msgid-bugs-address="${BUGADDR}" \
--files-from="infiles.list" -D "${BASEDIR}" -D "${WORKDIR}" -o "${PROJECT}.pot" || exit 1

TRANSLATIONS=$(find . -name '*.po')
for TR in ${TRANSLATIONS}; do
    msgmerge -U -q --backup=off "${TR}" "${PROJECT}.pot"
done

rm -f "rcfiles.list" "infiles.list" "rc.cpp"
