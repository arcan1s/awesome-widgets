find_package(Gettext REQUIRED)

find_package (ECM 0.0.12 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package (KF5 REQUIRED COMPONENTS I18n)
include (KDEInstallDirs)

add_subdirectory (awesome-widget)
add_subdirectory (desktop-panel)
