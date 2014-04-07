cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (pytextmonitor)
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 7)
set (PROJECT_VERSION_PATCH 5)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

message (STATUS "Version: ${PROJECT_VERSION}")

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
  set (ADD_CXX_FLAGS  "-Wall")
  set (CMAKE_CXX_FLAGS "-O0 ${ADD_CXX_FLAGS}")
  set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
  set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else ()
  message (STATUS "Unknown compiler")
endif ()

add_subdirectory (ext-sysmon)
add_subdirectory (ptm)
