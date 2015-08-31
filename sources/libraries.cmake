find_package(Gettext REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Network Script Qml Widgets)
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(KF5 REQUIRED COMPONENTS I18n Notifications Plasma Service WindowSystem)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_definitions(
    ${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS} ${Qt5Network_DEFINITIONS}
    ${Qt5Script_DEFINITIONS} ${Qt5Qml_DEFINITIONS} ${Qt5Widgets_DEFINITIONS}
)
set(Qt_INCLUDE
    ${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Script_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
)
set(Kf5_INCLUDE ${I18n_INCLUDE_DIR} ${Notifications_INCLUDE_DIR} ${Plasma_INCLUDE_DIR})

set(Qt_LIBRARIES
    ${Qt5Core_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Network_LIBRARIES}
    ${Qt5Script_LIBRARIES} ${Qt5Qml_LIBRARIES} ${Qt5Widgets_LIBRARIES}
)
set(Kf5_LIBRARIES KF5::I18n KF5::Notifications KF5::Plasma KF5::WindowSystem)
