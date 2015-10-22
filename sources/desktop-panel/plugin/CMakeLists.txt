set(PLUGIN_NAME dpplugin)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.desktoppanel\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf5_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/fontdialog/*.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_NOTIFY *.notifyrc)

add_library(${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE})
target_link_libraries(${PLUGIN_NAME} ${Qt_LIBRARIES} ${Kf5_LIBRARIES})

install(TARGETS ${PLUGIN_NAME} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/desktoppanel)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/desktoppanel)
install(FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KNOTIFYRC_INSTALL_DIR})
