project (extsysmon)
 
# Find the required Libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES})

set (extsysmon_engine_SRCS extsysmon.cpp)
 
# Now make sure all files get to the right place
kde4_add_plugin (plasma_engine_extsysmon ${extsysmon_engine_SRCS})
target_link_libraries(plasma_engine_extsysmon
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_PLASMA_LIBS})
 
install(TARGETS plasma_engine_extsysmon
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-engine-extsysmon.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}) 
