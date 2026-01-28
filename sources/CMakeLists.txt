cmake_minimum_required(VERSION 3.16.0)

# some fucking magic
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif ()
if (POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif ()
if (POLICY CMP0160)
    cmake_policy(SET CMP0160 OLD)
endif ()

project(awesomewidgets)
set(PROJECT_AUTHOR "Evgeniy Alekseev")
set(PROJECT_CONTACT "esalexeev@gmail.com")
set(PROJECT_LICENSE "GPL3")
set(PROJECT_VERSION_MAJOR "4")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "4")
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
option(BUILD_LOAD "Build with additional load" OFF)
option(BUILD_TESTING "Build with additional test abilities" OFF)

# generate changelog
set(PROJECT_CHANGELOG "Changelog" CACHE INTERNAL "")
include(changelog.cmake)

# directories
set(PROJECT_TRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(PROJECT_LIBRARY awesomewidgets)
set(PROJECT_MONITORSOURCES extsysmonsources)

# modules
include(compiler.cmake)
include(libraries.cmake)
include(clang-format.cmake)
include(cppcheck.cmake)
include(coverity.cmake)

# pre-configure
get_directory_property(CMAKE_DEFINITIONS COMPILE_DEFINITIONS)
configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# build components
add_subdirectory(awesomewidgets)
add_subdirectory(extsysmonsources)
add_subdirectory(extsysmon)
if (BUILD_PLASMOIDS)
    add_subdirectory(qml)
    add_subdirectory(awesome-widget)
    add_subdirectory(desktop-panel)
    add_subdirectory(translations)
endif ()
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()

include(packages-recipe.cmake)
