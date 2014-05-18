#!/bin/sh
BASEDIR="../contents" # root of translatable sources
PROJECT="pytextmonitor" # project name
BUGADDR="http://kde-look.org/content/show.php?content=157124" # MSGID-Bugs
WDIR=`pwd` # working dir
SATELLITE_LIST=satellite.list
BACKGROUND_LIST=background.list

add_list()
{
  LIST=$1

  if [ -z "$LIST" ]
  then
    echo ">>ERR<< add_list() - missing parameter LIST - exiting"
    return
  fi

  if [ -f "$LIST" ]
  then
    cat $LIST | while read ROW
    do
      echo "tr2i18n(\"${ROW}\")" >> ${WDIR}/rc.cpp
    done
  else
    echo ">>ERR<< add_list() - file $LIST does not exist."
  fi

}

echo "Preparing rc files"
cd ${BASEDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
find . -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' -o -name '*.py' | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp

# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp

cd ${WDIR}

# Add Satellite list
add_list "$SATELLITE_LIST"

# Add Background list
add_list "$BACKGROUND_LIST"

echo "Done preparing rc files"


echo "Extracting messages"
cd ${BASEDIR}
# see above on sorting
find . -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.py' | sort > ${WDIR}/infiles.list
echo "rc.cpp" >> ${WDIR}/infiles.list
cd ${WDIR}
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
--msgid-bugs-address="${BUGADDR}" --language=Python \
--files-from=infiles.list -D ${BASEDIR} -D ${WDIR} -o ${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }
echo "Done extracting messages"


echo "Merging translations"
catalogs=`find . -name '*.po'`
for cat in $catalogs; do
echo $cat
msgmerge -o $cat.new $cat ${PROJECT}.pot
mv $cat.new $cat
done
echo "Done merging translations"


echo "Cleaning up"
cd ${WDIR}
rm rcfiles.list
rm infiles.list
rm rc.cpp
echo "Done"

