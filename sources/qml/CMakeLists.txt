set(SUBPROJECT awesomewidgets-qml)
message(STATUS "Subproject ${SUBPROJECT}")

file(GLOB SUBPROJECT_SOURCE *.qml)
file(GLOB SUBPROJECT_QMLDIR_IN qmldir.in)

# prepare
configure_file(${SUBPROJECT_QMLDIR_IN} ${CMAKE_CURRENT_SOURCE_DIR}/../awesome-widget/package/contents/ui/qmldir)
configure_file(${SUBPROJECT_QMLDIR_IN} ${CMAKE_CURRENT_SOURCE_DIR}/../desktop-panel/package/contents/ui/qmldir)

# install
install(FILES ${SUBPROJECT_SOURCE} DESTINATION ${DATA_INSTALL_DIR}/${PROJECT_NAME}/qml)
