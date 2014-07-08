# set project name
set (SUBPROJECT plasma_applet_ptm-desktop-panel)
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
find_package (KDE4 REQUIRED)
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_subdirectory (po)

include (KDE4Defaults)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

# set sources
set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB SUBPROJECT_UI *.ui)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
