cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (awesome-widgets)
set (PROJECT_VERSION_MAJOR 2)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 5)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set (PROJECT_TRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

message (STATUS "Version: ${PROJECT_VERSION}")

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "-Wall -Wno-cpp -std=c++11")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else ()
    message (STATUS "Unknown compiler")
endif ()

add_subdirectory (awesome-widget)
add_subdirectory (ext-sysmon)
add_subdirectory (desktop-panel)
