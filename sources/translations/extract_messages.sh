#!/bin/bash

# properties
PROJECT="awesome-widgets"
BUGADDR="https://github.com/arcan1s/awesome-widgets/issues"

# root of translatable sources
SCRIPTDIR="$(dirname -- $(readlink -f -- $0))"
BASEDIR="${SCRIPTDIR}/../"
WORKDIR="$(pwd)"

# translations tags
# dunno what does this magic do actually
TAGS="-ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 -kI18N_NOOP:1 \
-kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3"

find "${BASEDIR}" \
    -name '*.cpp' -o -name '*.h' -o -name '*.qml' -o -name '*.ui' -o -name '*.rc' | \
    sort > "${WORKDIR}/infiles.list"

xgettext -C --no-location --msgid-bugs-address="${BUGADDR}" ${TAGS} \
    --files-from="${WORKDIR}/infiles.list" -D "${BASEDIR}" -D "${WORKDIR}" \
    -o "${PROJECT}.pot" || exit 1

TRANSLATIONS=$(find "${BASEDIR}" -name '*.po')
for TR in ${TRANSLATIONS}; do
    msgmerge -U -q --backup=off "${TR}" "${WORKDIR}/${PROJECT}.pot"
    msgattrib --no-obsolete -o "${TR}" "${TR}"
done

rm -f "${WORKDIR}/infiles.list"
