#!/bin/sh

WDIR=`pwd` # working dir
SATELLITE_LIST=satellite.list
BACKGROUND_LIST=background.list

# Background list
cd "$WDIR"
cd ../images
ls -1 background_* | sed -e "s/background_//g" | sed -e "s/_/ /g" | sed -e "s/\.jpg//g" | sed -e "s/\.png//g" | sed -e "s/\.gif//g" | grep -v '^$' | sort --unique --ignore-leading-blanks > "$WDIR/background.list"

# Satellite list
cd "$WDIR"
cd ../data
cat satellite_images.xml | grep image\ name | sed -e "s/.*<image name=\"\([^\"]*\).*/\1/" | sed -e "s/\&amp;/\&/g" | sed -e "s/\&apos;/'/g" | sed -e "s/\&quot;/\"/g" | sed -e "s/\&lt;/</g" | sed -e "s/\&gt;/>/g" | grep -v '^$' | sort --unique --ignore-leading-blanks > "$WDIR/satellite.list"
