set(SUBPROJECT ksystemstats_plugin_extsysmon)
message(STATUS "Subproject ${SUBPROJECT}")
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_MONITORSOURCES}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf6_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_HEADER *.h)
file(GLOB SUBPROJECT_CONF *.conf)

# make
add_library(${SUBPROJECT} MODULE ${SUBPROJECT_SOURCE} ${SUBPROJECT_HEADER})
target_link_libraries(${SUBPROJECT} 
        ${PROJECT_LIBRARY} ${PROJECT_MONITORSOURCES} 
        ${Qt_LIBRARIES} ${Kf6_LIBRARIES})

# install
install(TARGETS ${SUBPROJECT} DESTINATION ${KDE_INSTALL_PLUGINDIR}/ksystemstats)
install(FILES ${SUBPROJECT_CONF} DESTINATION ${KDE_INSTALL_CONFDIR})
