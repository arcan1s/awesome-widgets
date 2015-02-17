if (BUILD_KDE4)
  set (MO_NAME plasma_applet_awesome-widget.mo)
else ()
  set (MO_NAME plasma_applet_org.kde.plasma.awesomewidget.mo)
endif ()

file (GLOB _po_files *.po)
set (_gmoFiles)

foreach (_current_PO_FILE ${_po_files})
  get_filename_component (_lang ${_current_PO_FILE} NAME_WE)
  set (_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
  add_custom_command (OUTPUT ${_gmoFile}
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      DEPENDS ${_current_PO_FILE}
      )

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES/ RENAME ${MO_NAME})
  list (APPEND _gmoFiles ${_gmoFile})
endforeach (_current_PO_FILE)
add_custom_target (aw_pofiles ALL DEPENDS ${_gmoFiles})
