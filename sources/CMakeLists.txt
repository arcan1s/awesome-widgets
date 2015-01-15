cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (awesome-widgets)
set (PROJECT_AUTHOR "Evgeniy Alekseev")
set (PROJECT_CONTACT "esalexeev@gmail.com")
set (PROJECT_LICENSE "GPLv3")
set (PROJECT_VERSION_MAJOR 2)
set (PROJECT_VERSION_MINOR 1)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

string (TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M" UTC)
string (TIMESTAMP CURRENT_YEAR "%Y")

message (STATUS "Project: ${PROJECT_NAME}")
message (STATUS "Version: ${PROJECT_VERSION}")
message (STATUS "Build date: ${CURRENT_DATE}")

option (BUILD_KDE4 "Build on KDE4" OFF)

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "-Wall -Wno-cpp -std=c++11")
  set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
  set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
  # avoid newer gcc warnings
  add_definitions (-D_DEFAULT_SOURCE)
else ()
  message (STATUS "Unknown compiler")
endif ()

configure_file (${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set (PROJECT_TRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

add_subdirectory (ext-sysmon)
add_subdirectory (awesome-widget)
add_subdirectory (desktop-panel)
