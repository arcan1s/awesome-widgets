set(SUBPROJECT ${PROJECT_LIBRARY})
message(STATUS "Subproject ${SUBPROJECT}")
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf6_INCLUDE}
)

file(GLOB SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/qreplytimeout/*.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_HEADER *.h ${PROJECT_TRDPARTY_DIR}/qreplytimeout/*.h)
file(GLOB SUBPROJECT_UI *.ui)
file(GLOB SUBPROJECT_INI *.ini)
set(SUBPROJECT_CONFIGS ${CMAKE_CURRENT_SOURCE_DIR}/configs)
set(SUBPROJECT_FORMATTERS ${CMAKE_CURRENT_SOURCE_DIR}/formatters)
set(SUBPROJECT_GRAPHITEMS ${CMAKE_CURRENT_SOURCE_DIR}/desktops)
set(SUBPROJECT_QUOTES ${CMAKE_CURRENT_SOURCE_DIR}/quotes)
set(SUBPROJECT_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
set(SUBPROJECT_REQUESTS ${CMAKE_CURRENT_SOURCE_DIR}/requests)
set(SUBPROJECT_UPGRADE ${CMAKE_CURRENT_SOURCE_DIR}/upgrade)
set(SUBPROJECT_WEATHER ${CMAKE_CURRENT_SOURCE_DIR}/weather)
file(GLOB SUBPROJECT_WEATHER_JSON_IN *.json)
file(RELATIVE_PATH SUBPROJECT_WEATHER_JSON ${CMAKE_SOURCE_DIR} ${SUBPROJECT_WEATHER_JSON_IN})

# prepare
configure_file(${SUBPROJECT_WEATHER_JSON_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_WEATHER_JSON})

qt6_wrap_ui(SUBPROJECT_UI_HEADER ${SUBPROJECT_UI} OPTIONS --translate ui_i18n)
add_library(${SUBPROJECT} STATIC ${SUBPROJECT_SOURCE} ${SUBPROJECT_HEADER} ${SUBPROJECT_UI_HEADER})
set_property(TARGET ${SUBPROJECT} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${SUBPROJECT} ${Qt_LIBRARIES} ${Kf6_LIBRARIES})

# install
install(DIRECTORY ${SUBPROJECT_CONFIGS} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_FORMATTERS} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_GRAPHITEMS} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_QUOTES} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_SCRIPTS} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_REQUESTS} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_UPGRADE} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(DIRECTORY ${SUBPROJECT_WEATHER} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME})
install(FILES ${SUBPROJECT_INI} DESTINATION /${KDE_INSTALL_CONFDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_WEATHER_JSON} DESTINATION ${KDE_INSTALL_DATADIR}/${PROJECT_NAME}/weather)
