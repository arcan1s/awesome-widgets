set(SUBPROJECT awesomewidgets-test)
message(STATUS "Subproject ${SUBPROJECT}")

# find qt test package
add_definitions(${Qt5Test_DEFINITIONS})
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_MONITORSOURCES}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../awesome-widget/plugin/
        ${CMAKE_CURRENT_SOURCE_DIR}/../desktop-panel/plugin/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Qt5Test_INCLUDE_DIRS}
        ${Kf6_INCLUDE}
)

# library
set(AWTESTLIBRARY_HEADERS awtestlibrary.h)
set(AWTESTLIBRARY_SOURCES awtestlibrary.cpp)
add_library(${SUBPROJECT}-awtest STATIC ${AWTESTLIBRARY_SOURCES} ${AWTESTLIBRARY_HEADERS})
target_link_libraries(${SUBPROJECT}-awtest ${Qt_LIBRARIES} ${Qt6Test_LIBRARIES} ${Kf6_LIBRARIES})
set(LIBRARY_TEST_SET ${SUBPROJECT}-awtest ${PROJECT_LIBRARY} ${PROJECT_MONITORSOURCES} awplugin dpplugin
        ${Qt_LIBRARIES} ${Kf6_LIBRARIES} ${Qt6Test_LIBRARIES})

# modules
set(TEST_MODULES
        abstractextitem
        extquotes
        extscript
        extupgrade
        extweather

        abstractformatter
        datetimeformatter
        floatformatter
        jsonformatter
        listformatter
        noformatter
        scriptformatter
        stringformatter

        extitemaggregator

        batterysource
        desktopsource
        networksource
        playersource
        processessource

        awbugreporter
        awconfighelper
        awkeycache
        awkeys
        awpatternfunctions
        awtelemetryhandler
        awupdatehelper

        dpplugin
)

foreach (TEST_MODULE ${TEST_MODULES})
    set(${TEST_MODULE}_HEADERS test${TEST_MODULE}.h)
    set(${TEST_MODULE}_SOURCES test${TEST_MODULE}.cpp)
    add_executable(${SUBPROJECT}-${TEST_MODULE} ${${TEST_MODULE}_HEADERS} ${${TEST_MODULE}_SOURCES})

    target_link_libraries(${SUBPROJECT}-${TEST_MODULE} ${LIBRARY_TEST_SET})
    set_target_properties(${SUBPROJECT}-${TEST_MODULE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    if (NOT TEST_MODULE MATCHES "awbugreporter")
        add_test(NAME ${TEST_MODULE} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-${TEST_MODULE})
    endif (NOT TEST_MODULE MATCHES "awbugreporter")
endforeach (TEST_MODULE)
