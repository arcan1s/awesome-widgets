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
        ${Qt5Test_INCLUDE_DIRS}
)

# extscript
set (EXTSCRIPT_HEADERS testextscript.h)
set (EXTSCRIPT_SOURCES testextscript.cpp)
# qt5_wrap_cpp(EXTSCRIPT_MOC_SOURCES ${EXTSCRIPT_HEADERS})
add_executable (${SUBPROJECT}-extscript ${EXTSCRIPT_HEADERS} ${EXTSCRIPT_SOURCES} ${EXTSCRIPT_MOC_SOURCES})
target_link_libraries (${SUBPROJECT}-extscript ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Qt5Test_LIBRARIES})
add_test (NAME "ExtScript" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-extscript
          "-o" "../Testing/output-extscript.log")

