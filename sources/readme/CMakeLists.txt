# set project name
set (SUBPROJECT pytextmonitor)

file (GLOB SUBPROJECT_MD *.html)
install (FILES ${SUBPROJECT_MD} DESTINATION share/${SUBPROJECT}/)