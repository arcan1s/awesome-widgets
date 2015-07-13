cmake_minimum_required (VERSION 2.8.12)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (awesomewidgets)
set (PROJECT_AUTHOR "Evgeniy Alekseev")
set (PROJECT_CONTACT "esalexeev@gmail.com")
set (PROJECT_LICENSE "GPLv3")
set (PROJECT_VERSION_MAJOR 2)
set (PROJECT_VERSION_MINOR 3)
set (PROJECT_VERSION_PATCH 4)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

string (TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M" UTC)
string (TIMESTAMP CURRENT_YEAR "%Y")

message (STATUS "Project: ${PROJECT_NAME}")
message (STATUS "Version: ${PROJECT_VERSION}")
message (STATUS "Build date: ${CURRENT_DATE}")

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
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set (CMAKE_VERBOSE_MAKEFILE ON)
endif ()

configure_file (${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set (PROJECT_TRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

add_subdirectory (extsysmon)
add_subdirectory (awesome-widget)
add_subdirectory (desktop-panel)

# translations
add_subdirectory (translations)
