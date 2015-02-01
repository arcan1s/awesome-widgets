set (PLUGIN_NAME awplugin)

add_definitions (${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS})
set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Network_INCLUDE_DIRS}
                ${Qt5Qml_INCLUDE_DIRS})
set (Qt_LIBRARIES ${Qt5Core_LIBRARIES}
                  ${Qt5Network_LIBRARIES}
                  ${Qt5Qml_LIBRARIES})
set (Kf5_INCLUDE ${I18n_INCLUDE_DIR}
                 ${Notifications_INCLUDE_DIR})
set (Kf5_LIBRARIES KF5::I18n
                   KF5::Notifications)

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ../../extsysmon
                     ${CMAKE_CURRENT_BINARY_DIR}/../../extsysmon
                     ${Qt_INCLUDE}
                     ${Kf5_INCLUDE})

file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)
file (GLOB SUBPROJECT_UI *.ui)
# task source is required by extscripts
file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/task/*.cpp
                                     ${PROJECT_TRDPARTY_DIR}/fontdialog/*.cpp
                                     ../../extsysmon/extscript.cpp
                                     ../../extsysmon/extupgrade.cpp)
set (TASK_HEADER ${PROJECT_TRDPARTY_DIR}/task/task.h)
set (SUBPROJECT_DESKTOPS ${CMAKE_CURRENT_SOURCE_DIR}/desktops)

qt5_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
qt5_wrap_ui (SUBPROJECT_UI_HEADER ${SUBPROJECT_UI})
add_library (${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE} ${SUBPROJECT_UI_HEADER} ${TASK_MOC_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${Qt_LIBRARIES} ${Kf5_LIBRARIES})

install (TARGETS ${PLUGIN_NAME} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/awesomewidget)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/awesomewidget)
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KNOTIFYRC_INSTALL_DIR})
install (DIRECTORY ${SUBPROJECT_DESKTOPS} DESTINATION ${DATA_INSTALL_DIR}/${PROJECT_NAME})
