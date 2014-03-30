# set project name
set (SUBPROJECT pytextmonitor)

# dependencies
find_package(Gettext REQUIRED)

if (NOT GETTEXT_MSGFMT_EXECUTABLE)
  message(FATAL_ERROR "Please install the msgfmt binary")
endif (NOT GETTEXT_MSGFMT_EXECUTABLE)

file (GLOB _po_files *.po)

set (_gmoFiles)

foreach (_current_PO_FILE ${_po_files})
  get_filename_component (_lang ${_current_PO_FILE} NAME_WE)
    set (_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
    add_custom_command (OUTPUT ${_gmoFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
         DEPENDS ${_current_PO_FILE})

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo DESTINATION "share/apps/plasma/plasmoids/${SUBPROJECT}/locale/${_lang}/LC_MESSAGES" RENAME ${SUBPROJECT}.mo)
  list (APPEND _gmoFiles ${_gmoFile})
endforeach (_current_PO_FILE)
add_custom_target (pofiles ALL DEPENDS ${_gmoFiles})
