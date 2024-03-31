set(SUBPROJECT awesomewidgets-qml)
message(STATUS "Subproject ${SUBPROJECT}")

file(GLOB SUBPROJECT_SOURCE *.qml qmldir)

# install
install(FILES ${SUBPROJECT_SOURCE} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/${PROJECT_NAME})
