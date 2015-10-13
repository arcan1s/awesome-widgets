#!/bin/bash

# This script will check the source code by using clang-format with required arguments.
# The template comes from https://gist.github.com/Uflex/7086827
# More details may be found here http://clang.llvm.org/docs/ClangFormatStyleOptions.html

clang-format -i \
             -style=file \
             $(find ../sources -not -path '*3rdparty*' \
                               -and \( -name '*.cpp' -or -name '*.h' \))
