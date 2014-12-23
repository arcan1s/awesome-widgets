#!/bin/bash

ARCHIVE="$1"
VERSION="$2"

# prepare
rm -rf rpm-build
mkdir -p rpm-build/build
cd rpm-build
tar xJf "../${ARCHIVE}"

# build
cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=$(kde4-config --prefix) \
      ../awesome-widgets
make
make DESTDIR="../" install

# create tarball
cd ..
"${HOME}/.gem/ruby/2.1.0/bin/fpm" -s dir \
                                  -t deb \
                                  -n awesome-widgets \
                                  -v "${VERSION}" \
                                  -a native \
                                  -C . \
                                  --prefix / \
                                  usr
