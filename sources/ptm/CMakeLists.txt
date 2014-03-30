# set project name
set (SUBPROJECT py-text-monitor)
# for notifications
set (PLUGIN_NAME plasma_applet_pytextmonitor)
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

add_subdirectory (po)

# set sources
set (SUBPROJECT_CODE_DIR "contents/code")
set (SUBPROJECT_UI_DIR "contents/ui")
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)
file (GLOB SUBPROJECT_SOURCE ${SUBPROJECT_CODE_DIR}/*.py)
file (GLOB SUBPROJECT_UI ${SUBPROJECT_UI_DIR}/*.ui)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# install
install (FILES ${SUBPROJECT_SOURCE} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}/${SUBPROJECT_CODE_DIR}")
install (FILES ${SUBPROJECT_UI} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}/${SUBPROJECT_UI_DIR}")
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR} RENAME ${SUBPROJECT}.desktop)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}")
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
