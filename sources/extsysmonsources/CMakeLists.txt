set(SUBPROJECT ${PROJECT_MONITORSOURCES})
message(STATUS "Subproject ${SUBPROJECT}")
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf6_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp)
file(GLOB SUBPROJECT_HEADER *.h)

add_library(${SUBPROJECT} STATIC ${SUBPROJECT_SOURCE} ${SUBPROJECT_HEADER})
target_link_libraries(${SUBPROJECT} ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Kf6_LIBRARIES})
