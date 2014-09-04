# set project name
set (SUBPROJECT plasma_applet_awesome-widget)
set (PLUGIN_NAME ${SUBPROJECT})
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
find_package (KDE4 REQUIRED)
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_subdirectory (po)

include (KDE4Defaults)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR})

# set sources
set (SUBPROJECT_MISC_DIR "contents/misc")
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/task/*.cpp
                                           ${PROJECT_TRDPARTY_DIR}/fontdialog/*.cpp)
set (TASK_HEADER ${PROJECT_TRDPARTY_DIR}/task/task.h)
file (GLOB SUBPROJECT_UI *.ui)
file (GLOB SUBPROJECT_NOTIFY ${SUBPROJECT_MISC_DIR}/*.notifyrc)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
qt4_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE} ${TASK_MOC_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
