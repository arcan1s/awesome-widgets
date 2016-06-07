set(SUBPROJECT awesomewidgets-test)
message(STATUS "Subproject ${SUBPROJECT}")

# find qt test package
add_definitions(${Qt5Test_DEFINITIONS})
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_MONITORSOURCES}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Qt5Test_INCLUDE_DIRS}
        ${Kf5_INCLUDE}
)

## library
set(AWTESTLIBRARY_HEADERS awtestlibrary.h)
set(AWTESTLIBRARY_SOURCES awtestlibrary.cpp)
add_library(${SUBPROJECT}-awtest STATIC ${AWTESTLIBRARY_SOURCES} ${AWTESTLIBRARY_HEADERS})
target_link_libraries(${SUBPROJECT}-awtest ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
set(LIBRARY_TEST_SET ${SUBPROJECT}-awtest ${PROJECT_LIBRARY} ${PROJECT_MONITORSOURCES} ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})

## modules
set(TEST_MODULES
        abstractextitem extquotes extscript extupgrade extweather
        abstractformatter datetimeformatter floatformatter noformatter scriptformatter
        extitemaggregator
        hddtempsource)
foreach (TEST_MODULE ${TEST_MODULES})
    set(${TEST_MODULE}_HEADERS test${TEST_MODULE}.h)
    set(${TEST_MODULE}_SOURCES test${TEST_MODULE}.cpp)
    add_executable(${SUBPROJECT}-${TEST_MODULE} ${${TEST_MODULE}_HEADERS} ${${TEST_MODULE}_SOURCES})
    target_link_libraries(${SUBPROJECT}-${TEST_MODULE} ${LIBRARY_TEST_SET})
    add_test(NAME ${TEST_MODULE} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-${TEST_MODULE})
endforeach (TEST_MODULE)
