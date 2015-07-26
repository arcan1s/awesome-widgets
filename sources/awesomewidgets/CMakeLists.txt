# set project name
set (SUBPROJECT ${PROJECT_LIBRARY})
message (STATUS "Subproject ${SUBPROJECT}")
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

# find required libaries
find_package (Qt5 REQUIRED COMPONENTS Core Network Widgets)

add_definitions (${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS}
                 ${Qt5Network_DEFINITIONS} ${Qt5Widgets_DEFINITIONS})
set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS}
                ${Qt5Network_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt_INCLUDE})

file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/qreplytimeout/*.cpp)
file (GLOB SUBPROJECT_HEADER *.h ${PROJECT_TRDPARTY_DIR}/qreplytimeout/*.h)
file (GLOB SUBPROJECT_UI *.ui)
set (SUBPROJECT_GRAPHITEMS ${CMAKE_CURRENT_SOURCE_DIR}/desktops)
set (SUBPROJECT_QUOTES ${CMAKE_CURRENT_SOURCE_DIR}/quotes)
set (SUBPROJECT_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
set (SUBPROJECT_UPGRADE ${CMAKE_CURRENT_SOURCE_DIR}/upgrade)
set (SUBPROJECT_WEATHER ${CMAKE_CURRENT_SOURCE_DIR}/weather)

# make
qt5_wrap_cpp (SUBPROJECT_MOC_SOURCE ${SUBPROJECT_HEADER})
qt5_wrap_ui (SUBPROJECT_UI_HEADER ${SUBPROJECT_UI})
add_library (${SUBPROJECT} STATIC ${SUBPROJECT_SOURCE} ${SUBPROJECT_MOC_SOURCE}
                                  ${SUBPROJECT_HEADER} ${SUBPROJECT_UI_HEADER})
target_link_libraries (${SUBPROJECT} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Widgets_LIBRARIES})

# install
install (DIRECTORY ${SUBPROJECT_GRAPHITEMS} DESTINATION share/${PROJECT_NAME})
install (DIRECTORY ${SUBPROJECT_QUOTES} DESTINATION share/${PROJECT_NAME})
install (DIRECTORY ${SUBPROJECT_SCRIPTS} DESTINATION share/${PROJECT_NAME})
install (DIRECTORY ${SUBPROJECT_UPGRADE} DESTINATION share/${PROJECT_NAME})
install (DIRECTORY ${SUBPROJECT_WEATHER} DESTINATION share/${PROJECT_NAME})
