#!/bin/bash
# root of translatable sources
BASEDIR="../"
PROJECT="awesome-widgets"
BUGADDR="https://github.com/arcan1s/awesome-widgets/issues"
WORKDIR=$(pwd)
# translations tags
TAGS="-ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 -kI18N_NOOP:1 \
-kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3"

find "${BASEDIR}" -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > "${WORKDIR}/rcfiles.list"
xargs --arg-file="${WORKDIR}/rcfiles.list" > "${WORKDIR}/rc.cpp"
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> "${WORKDIR}/rc.cpp"
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> "${WORKDIR}/rc.cpp"

find "${BASEDIR}" -name '*.cpp' -o -name '*.h' -o -name '*.qml' | sort > "${WORKDIR}/infiles.list"
echo "rc.cpp" >> "${WORKDIR}/infiles.list"

xgettext -C --no-location --msgid-bugs-address="${BUGADDR}" ${TAGS} \
--files-from="infiles.list" -D "${BASEDIR}" -D "${WORKDIR}" -o "${PROJECT}.pot" || exit 1

TRANSLATIONS=$(find . -name '*.po')
for TR in ${TRANSLATIONS}; do
    msgmerge -U -q --backup=off "${TR}" "${PROJECT}.pot"
done

rm -f "rcfiles.list" "infiles.list" "rc.cpp"
