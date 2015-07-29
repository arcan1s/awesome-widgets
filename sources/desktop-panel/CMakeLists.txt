# set project name
set (SUBPROJECT plasma_applet_desktop-panel)
message (STATUS "Subproject ${SUBPROJECT}")

# prepare
configure_file (metadata.desktop ${CMAKE_CURRENT_SOURCE_DIR}/package/metadata.desktop)

add_subdirectory (plugin)
plasma_install_package (package org.kde.plasma.desktoppanel)
