# set project name
set (SUBPROJECT icons)
message (STATUS "Subproject ${SUBPROJECT}")

# main icon
install (FILES ptm.png DESTINATION share/pixmaps RENAME ptm.png)
# configuration interface
install (FILES ptm-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME ptm.png)
install (FILES ptm-advanced-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME ptm-advanced.png)
install (FILES ptm-appearance-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME ptm-appearance.png)
install (FILES ptm-tooltip-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME ptm-tooltip.png)
