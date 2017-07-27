#!/bin/bash

set -e

rm -rf build-arch
mkdir build-arch

# build
cd build-arch
cmake -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DCMAKE_BUILD_TYPE=Optimization -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_FUTURE=ON -DBUILD_TESTING=ON ../sources
make

# tests
xvfb-run make test
