set(PLUGIN_NAME awplugin)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_LIBRARY}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf5_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/fontdialog/*.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_NOTIFY *.notifyrc)

add_library(${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE})
target_link_libraries(${PLUGIN_NAME} ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Kf5_LIBRARIES})

install(TARGETS ${PLUGIN_NAME} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/awesomewidget)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/awesomewidget)
install(FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KNOTIFYRC_INSTALL_DIR})
