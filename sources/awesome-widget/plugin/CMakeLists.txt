set(PLUGIN_NAME awplugin)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_LIBRARY}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf6_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp formatters/*.cpp ${PROJECT_TRDPARTY_DIR}/fontdialog/*.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_UI *.ui)
file(GLOB SUBPROJECT_NOTIFY *.notifyrc)

qt6_wrap_ui(SUBPROJECT_UI_HEADER ${SUBPROJECT_UI} OPTIONS --translate ui_i18n)
add_library(${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE} ${SUBPROJECT_UI_HEADER})
target_link_libraries(${PLUGIN_NAME} ${PROJECT_LIBRARY} ${Qt_LIBRARIES} ${Kf6_LIBRARIES})

install(TARGETS ${PLUGIN_NAME} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/awesomewidget)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/awesomewidget)
install(FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
