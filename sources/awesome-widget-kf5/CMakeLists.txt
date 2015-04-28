# set project name
set (SUBPROJECT plasma_applet_awesome-widget)
message (STATUS "Subproject ${SUBPROJECT}")

# prepare
configure_file (metadata.desktop ${CMAKE_CURRENT_SOURCE_DIR}/package/metadata.desktop)

find_package (ECM 0.0.12 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package (Qt5 REQUIRED COMPONENTS Core Network Qml Xml)
find_package (KF5 REQUIRED COMPONENTS I18n Notifications Plasma)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)

add_subdirectory (plugin)
plasma_install_package (package org.kde.plasma.awesomewidget)
