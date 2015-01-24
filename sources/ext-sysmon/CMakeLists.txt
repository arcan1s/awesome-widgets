# set project name
set (SUBPROJECT plasma_engine_extsysmon)
set (PLUGIN_NAME ${SUBPROJECT})
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
if (BUILD_KDE4)
  find_package (KDE4 REQUIRED)
  include (KDE4Defaults)

  add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
  set (KDE_INCLUDE ${KDE4_INCLUDES})
else ()
  find_package (Qt5 REQUIRED COMPONENTS Network Widgets)
  find_package (ECM 0.0.11 REQUIRED NO_MODULE)
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
  find_package (KF5 REQUIRED COMPONENTS Plasma Service WindowSystem)

  include (KDEInstallDirs)
  include (KDECMakeSettings)
  include (KDECompilerSettings)

  add_definitions (${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS} ${Qt5Widgets_DEFINITIONS})
  set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                  ${Qt5Network_INCLUDE_DIRS}
                  ${Qt5Widgets_INCLUDE_DIRS})
  set (KDE_INCLUDE ${Plasma_INCLUDE_DIR})
endif ()

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt_INCLUDE}
                     ${KDE_INCLUDE})

file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/task/*.cpp)
set (TASK_HEADER ${PROJECT_TRDPARTY_DIR}/task/task.h)
file (GLOB SUBPROJECT_UI *.ui)
file (GLOB SUBPROJECT_CONF *.conf)
set (SUBPROJECT_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
if (BUILD_KDE4)
  qt4_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
  kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
  kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE} ${TASK_MOC_SOURCE})
  target_link_libraries (${PLUGIN_NAME} ${KDE4_KDECORE_LIBS} ${KDE4_PLASMA_LIBS})
else ()
  qt5_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
  qt5_wrap_ui (SUBPROJECT_UI_HEADER ${SUBPROJECT_UI})
  add_library (${PLUGIN_NAME} MODULE ${SUBPROJECT_SOURCE} ${SUBPROJECT_UI_HEADER} ${TASK_MOC_SOURCE})
  target_link_libraries (${PLUGIN_NAME} ${Plasma_LIBRARIES} KF5::WindowSystem ${Qt5Network_LIBRARIES} ${Qt5Widgets_LIBRARIES})
  kcoreaddons_desktop_to_json (${PLUGIN_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})
endif ()

# install
if (BUILD_KDE4)
  install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
else ()
  install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/dataengine)
endif ()
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_CONF} DESTINATION ${CONFIG_INSTALL_DIR})
install (DIRECTORY ${SUBPROJECT_SCRIPTS} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
