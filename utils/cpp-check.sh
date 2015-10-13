#!/bin/bash

# This code is used to check source files with cppcheck

cppcheck --enable=all \
         --std=c++11 \
         --verbose \
         --quiet \
         ../sources 2>&1 | grep -v '3rdparty'
