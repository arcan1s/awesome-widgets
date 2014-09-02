# set project name
set (SUBPROJECT awesome-widgets)

file (GLOB SUBPROJECT_MD *.html)
install (FILES ${SUBPROJECT_MD} DESTINATION share/${SUBPROJECT}/)