set(SUBPROJECT awesomewidgets-test)
message(STATUS "Subproject ${SUBPROJECT}")

# find qt test package
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf5_INCLUDE}
)

## library
set(AWTESTLIBRARY_HEADERS awtestlibrary.h)
set(AWTESTLIBRARY_SOURCES awtestlibrary.cpp)
add_library(${SUBPROJECT}-awtest STATIC ${AWTESTLIBRARY_SOURCES} ${AWTESTLIBRARY_HEADERS})
target_link_libraries(${SUBPROJECT}-awtest ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
set(LIBRARY_TEST_SET ${SUBPROJECT}-awtest ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
## extensions
# abstractextitem
set(ABSTRACTEXTITEM_HEADERS testabstractextitem.h)
set(ABSTRACTEXTITEM_SOURCES testabstractextitem.cpp)
add_executable(${SUBPROJECT}-abstractextitem ${ABSTRACTEXTITEM_HEADERS} ${ABSTRACTEXTITEM_SOURCES})
target_link_libraries(${SUBPROJECT}-abstractextitem ${LIBRARY_TEST_SET})
add_test(NAME "AbstractExtItem" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-abstractextitem)
# extquotes
set(EXTQUOTES_HEADERS testextquotes.h)
set(EXTQUOTES_SOURCES testextquotes.cpp)
add_executable(${SUBPROJECT}-extquotes ${EXTQUOTES_HEADERS} ${EXTQUOTES_SOURCES})
target_link_libraries(${SUBPROJECT}-extquotes ${LIBRARY_TEST_SET})
add_test(NAME "ExtQuotes" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extquotes)
# extscript
set(EXTSCRIPT_HEADERS testextscript.h)
set(EXTSCRIPT_SOURCES testextscript.cpp)
add_executable(${SUBPROJECT}-extscript ${EXTSCRIPT_HEADERS} ${EXTSCRIPT_SOURCES})
target_link_libraries(${SUBPROJECT}-extscript ${LIBRARY_TEST_SET})
add_test(NAME "ExtScript" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extscript)
# extupgrade
set(EXTUPGRADE_HEADERS testextupgrade.h)
set(EXTUPGRADE_SOURCES testextupgrade.cpp)
add_executable(${SUBPROJECT}-extupgrade ${EXTUPGRADE_HEADERS} ${EXTUPGRADE_SOURCES})
target_link_libraries(${SUBPROJECT}-extupgrade ${LIBRARY_TEST_SET})
add_test(NAME "ExtUpgrade" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extupgrade)
# extweather
set(EXTWEATHER_HEADERS testextweather.h)
set(EXTWEATHER_SOURCES testextweather.cpp)
add_executable(${SUBPROJECT}-extweather ${EXTWEATHER_HEADERS} ${EXTWEATHER_SOURCES})
target_link_libraries(${SUBPROJECT}-extweather ${LIBRARY_TEST_SET})
add_test(NAME "ExtWeather" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extweather)
## formatters
# float formatter
set(FLOATFORMATTER_HEADERS testfloatformatter.h)
set(FLOATFORMATTER_SOURCES testfloatformatter.cpp)
add_executable(${SUBPROJECT}-floatformatter ${FLOATFORMATTER_HEADERS} ${FLOATFORMATTER_SOURCES})
target_link_libraries(${SUBPROJECT}-floatformatter ${LIBRARY_TEST_SET})
add_test(NAME "Float4Formatter" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-floatformatter)
# no formatter
set(NOFORMATTER_HEADERS testnoformatter.h)
set(NOFORMATTER_SOURCES testnoformatter.cpp)
add_executable(${SUBPROJECT}-noformatter ${NOFORMATTER_HEADERS} ${NOFORMATTER_SOURCES})
target_link_libraries(${SUBPROJECT}-noformatter ${LIBRARY_TEST_SET})
add_test(NAME "NoFormatter" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-noformatter)
