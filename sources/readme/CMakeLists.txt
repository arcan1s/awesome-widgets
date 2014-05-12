# set project name
set (SUBPROJECT pytextmonitor)

file (GLOB SUBPROJECT_MD *.md)
install (FILES ${SUBPROJECT_MD} DESTINATION share/${SUBPROJECT}/)