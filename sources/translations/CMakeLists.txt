find_package(Gettext REQUIRED)

if (BUILD_KDE4)
  find_package (KDE4 REQUIRED)
else ()
  find_package (ECM 0.0.12 REQUIRED NO_MODULE)
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
  find_package (KF5 REQUIRED COMPONENTS I18n)
  include (KDEInstallDirs)
endif ()

add_subdirectory (awesome-widget)
add_subdirectory (desktop-panel)