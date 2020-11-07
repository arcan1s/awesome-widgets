#!/bin/bash

set -e

rm -rf build-ubuntu
mkdir build-ubuntu

# patches
git apply patches/qt5.6-qversionnumber.patch
git apply patches/qt5.5-qstringlist-and-qinfo.patch

# build
cd build-ubuntu
cmake -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DCMAKE_BUILD_TYPE=Optimization -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_FUTURE=ON -DBUILD_TESTING=ON ../sources
make

# tests
xvfb-run -a make test
