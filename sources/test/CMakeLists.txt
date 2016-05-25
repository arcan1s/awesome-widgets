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

# extscript
set(EXTSCRIPT_HEADERS testextscript.h)
set(EXTSCRIPT_SOURCES testextscript.cpp)
add_executable(${SUBPROJECT}-extscript ${EXTSCRIPT_HEADERS} ${EXTSCRIPT_SOURCES})
target_link_libraries(${SUBPROJECT}-extscript ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
add_test(NAME "ExtScript" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extscript)
# extupgrade
set(EXTUPGRADE_HEADERS testextupgrade.h)
set(EXTUPGRADE_SOURCES testextupgrade.cpp)
add_executable(${SUBPROJECT}-extupgrade ${EXTUPGRADE_HEADERS} ${EXTUPGRADE_SOURCES})
target_link_libraries(${SUBPROJECT}-extupgrade ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
add_test(NAME "ExtUpgrade" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extupgrade)

