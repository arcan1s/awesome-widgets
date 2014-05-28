# set project name
set (SUBPROJECT pytextmonitor)
# for notifications
set (PLUGIN_NAME plasma_applet_pytextmonitor)
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
find_package (KDE4 REQUIRED)
add_definitions (${KDE4_DEFINITIONS})

add_subdirectory (po)

# set sources
set (SUBPROJECT_CODE_DIR "contents/code")
set (SUBPROJECT_MISC_DIR "contents/misc")
set (SUBPROJECT_UI_DIR "contents/ui")
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB SUBPROJECT_NOTIFY ${SUBPROJECT_MISC_DIR}/*.notifyrc)
file (GLOB SUBPROJECT_SOURCE ${SUBPROJECT_CODE_DIR}/*.py)
file (GLOB SUBPROJECT_UI ${SUBPROJECT_UI_DIR}/*.ui)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# install
install (FILES ${SUBPROJECT_SOURCE} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}/${SUBPROJECT_CODE_DIR}")
install (FILES ${SUBPROJECT_UI} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}/${SUBPROJECT_UI_DIR}")
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR} RENAME "plasma-applet-${SUBPROJECT}.desktop")
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}")
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
