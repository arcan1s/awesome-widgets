# required by translation package
find_package(Gettext REQUIRED)

# main qt libraries
find_package(Qt6 6.6.0 REQUIRED COMPONENTS Core DBus Network Qml Test Widgets)
add_definitions(
        ${Qt6Core_DEFINITIONS} ${Qt6DBus_DEFINITIONS} ${Qt6Network_DEFINITIONS}
        ${Qt6Qml_DEFINITIONS} ${Qt6Widgets_DEFINITIONS}
)
set(Qt_INCLUDE
        ${Qt6Core_INCLUDE_DIRS} ${Qt6DBus_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS}
        ${Qt6Qml_INCLUDE_DIRS} ${Qt6Widgets_INCLUDE_DIRS}
)
set(Qt_LIBRARIES
        ${Qt6Core_LIBRARIES} ${Qt6DBus_LIBRARIES} ${Qt6Network_LIBRARIES}
        ${Qt6Qml_LIBRARIES} ${Qt6Widgets_LIBRARIES}
)

# kf6 libraries
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(KF6 REQUIRED COMPONENTS I18n Notifications Service WindowSystem)
find_package(LibTaskManager REQUIRED)
find_package(Plasma REQUIRED)
find_package(KSysGuard REQUIRED)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
set(Kf6_INCLUDE ${KDE_INSTALL_FULL_INCLUDEDIR_KF})
set(Kf6_LIBRARIES KF6::CoreAddons KF6::I18n KF6::Notifications KF6::WindowSystem KSysGuard::Sensors KSysGuard::SensorFaces KSysGuard::SystemStats PW::LibTaskManager)
