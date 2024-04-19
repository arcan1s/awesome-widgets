# flags
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_OPTIMIZATION "-Ofast -DNDEBUG")

# standard force
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # enable clang-tidy
    set(CMAKE_CXX_CLANG_TIDY
            clang-tidy;
            -format-style='file';
            -header-filter=${CMAKE_CURRENT_SOURCE_DIR};
    )
endif ()

# verbose output for debug builds
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

# required by successfully coverity and cppcheck build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
