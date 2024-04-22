#!/bin/bash

set -e

# properties
PROJECT="awesome-widgets"
BUGADDR="https://github.com/arcan1s/awesome-widgets/issues"

# root of translatable sources
SCRIPTDIR="$(dirname -- $(readlink -f -- $0))"
BASEDIR="$SCRIPTDIR/../"
WORKDIR="$(pwd)"

find "$BASEDIR" -name '*.cpp' -o -name '*.h' -o -name '*.qml' | sort > "$WORKDIR/infiles.list"
if [ -n "$CMAKE_BINARY_DIR" ]; then
    find "$CMAKE_BINARY_DIR" -name '*.cpp' -o -name '*.h' -o -name '*.qml' | sort >> "$WORKDIR/infiles.list";
fi

xgettext \
    --from-code=UTF-8 --width=200 --add-location=file \
    --files-from="$WORKDIR/infiles.list" \
    -C --kde \
    -ci18n \
    -kui_i18n:1 \
    -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
    -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
    -kxi18n:1 -kxi18nc:1c,2 -kxi18np:1,2 -kxi18ncp:1c,2,3 \
    -kkxi18n:1 -kkxi18nc:1c,2 -kkxi18np:1,2 -kkxi18ncp:1c,2,3 \
    -kI18N_NOOP:1 -kI18NC_NOOP:1c,2 \
    -kI18N_NOOP2:1c,2 -kI18N_NOOP2_NOSTRIP:1c,2 \
    -ktr2i18n:1 -ktr2xi18n:1 \
    -kaliasLocale \
    --package-name="$PROJECT" \
    --msgid-bugs-address="$BUGADDR" \
    -D "$BASEDIR" \
    -D "$WORKDIR" \
    -o "$SCRIPTDIR/$PROJECT.pot.new"

mv "$SCRIPTDIR/$PROJECT.pot"{.new,}

for TR in $(find "$BASEDIR" -name '*.po'); do
    msgmerge \
        --update \
        --quiet \
        --backup=off \
        "$TR" \
        "$SCRIPTDIR/$PROJECT.pot"
    msgattrib \
        --no-obsolete \
        --output-file "$TR" \
        "$TR"
done

rm -f "$WORKDIR/infiles.list"
