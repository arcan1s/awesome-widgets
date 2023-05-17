# flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_OPTIMIZATION "-Ofast -DNDEBUG")
    # avoid newer gcc warnings
    add_definitions(-D_DEFAULT_SOURCE)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_OPTIMIZATION "-Ofast -DNDEBUG")
    # linker flags
else ()
    message(FATAL_ERROR "Unknown compiler")
endif ()

# some flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# verbose output for debug builds
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

# required by successfully coverity and cppcheck build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
