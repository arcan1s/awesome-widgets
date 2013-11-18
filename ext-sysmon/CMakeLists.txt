# set project name
project (plasma_engine_extsysmon)

# find required libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions (${QT_DEFINITIONS} 
                 ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} 
                     ${CMAKE_BINARY_DIR} 
                     ${KDE4_INCLUDES})

set (PLUGIN_NAME ${PROJECT_NAME})
file (GLOB PROJECT_DESKTOP *.desktop)
file (GLOB PROJECT_SRCS *.cpp)
file (GLOB PROJECT_CONF *.conf)

# make
kde4_add_plugin (${PLUGIN_NAME} ${PROJECT_SRCS})
target_link_libraries (${PLUGIN_NAME} ${KDE4_KDECORE_LIBS} ${KDE4_PLASMA_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${PROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${PROJECT_CONF} DESTINATION ../etc/)
