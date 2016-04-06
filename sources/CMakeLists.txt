cmake_minimum_required(VERSION 2.8.12)

# some fucking magic
cmake_policy(SET CMP0003 OLD)
cmake_policy(SET CMP0002 OLD)
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD)
endif ()

project(awesomewidgets)
set(PROJECT_AUTHOR "Evgeniy Alekseev")
set(PROJECT_CONTACT "esalexeev@gmail.com")
set(PROJECT_LICENSE "GPL3")
set(PROJECT_VERSION_MAJOR "3")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "1")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
# append git version if any
set(PROJECT_COMMIT_SHA "Commit hash" CACHE INTERNAL "")
include(checkgit.cmake)

string(TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M" UTC)
string(TIMESTAMP CURRENT_YEAR "%Y")

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build date: ${CURRENT_DATE}")

# components
option(BUILD_PLASMOIDS "Build plasmoids" ON)
option(BUILD_DEB_PACKAGE "Build deb package" OFF)
option(BUILD_RPM_PACKAGE "Build rpm package" OFF)
# build details
option(BUILD_FUTURE "Build with the features which will be marked as stable later" OFF)
option(BUILD_TESTING "Build with additional test abilities" OFF)
# some additional targets
set(CLANGFORMAT_EXECUTABLE "/usr/bin/clang-format" CACHE STRING "Path to clang-format executable")
set(CPPCHECK_EXECUTABLE "/usr/bin/cppcheck" CACHE STRING "Path to cppcheck executable")

# generate changelog
set(PROJECT_CHANGELOG "Changelog" CACHE INTERNAL "")
include(changelog.cmake)

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wno-cpp -std=c++14")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_OPTIMIZATION "-Ofast -DNDEBUG")
    # avoid newer gcc warnings
    add_definitions(-D_DEFAULT_SOURCE)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_OPTIMIZATION "-Ofast -DNDEBUG")
    # linker flags
    set(CMAKE_EXE_LINKER_FLAGS "-lc++abi")
    set(CMAKE_MODULE_LINKER_FLAGS "-lc++abi")
    set(CMAKE_SHARED_LINKER_FLAGS "-lc++abi")
else ()
    message(FATAL_ERROR "Unknown compiler")
endif ()
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

set(PROJECT_TRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(PROJECT_LIBRARY awesomewidgets)
include(libraries.cmake)
include(clang-format.cmake)
include(cppcheck.cmake)

get_directory_property(CMAKE_DEFINITIONS COMPILE_DEFINITIONS)
configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
add_subdirectory(awesomewidgets)
add_subdirectory(extsysmon)
if (BUILD_PLASMOIDS)
    add_subdirectory(awesome-widget)
    add_subdirectory(desktop-panel)
    add_subdirectory(translations)
endif ()

include(packages-recipe.cmake)
