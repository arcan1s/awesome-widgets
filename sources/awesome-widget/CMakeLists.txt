set(SUBPROJECT plasma_applet_awesome-widget)
message(STATUS "Subproject ${SUBPROJECT}")

configure_file(metadata.json ${CMAKE_CURRENT_SOURCE_DIR}/package/metadata.json)

add_subdirectory(plugin)
plasma_install_package(package org.kde.plasma.awesomewidget)
