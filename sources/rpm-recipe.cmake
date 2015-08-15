set(CPACK_GENERATOR "RPM")
set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/RpmPackageConfig.cmake")

# versioning and naming
# I suppose it should be named as kde-plasma-* or plasma5-addons-* according to pkgs.org search
set(CPACK_PACKAGE_NAME "plasma5-awesome-widgets")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_RELEASE "1")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RPM_PACKAGE_LICENSE "${PROJECT_LICENSE}")

# metadata
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR}")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Collection of minimalistic Plasmoids which look like Awesome WM widgets (ex-PyTextMonitor)")
set(CPACK_RPM_PACKAGE_URL "https://arcanis.name/projects/awesome-widgets")
set(CPACK_RPM_PACKAGE_GROUP "User Interface/Desktops")

# build data
set(CPACK_RPM_PACKAGE_REQUIRES "plasma-framework")
set(CPACK_RPM_PACKAGE_SUGGESTS "mpd, smartmontools")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# go go go!
include(CPack)
add_custom_target(rpm COMMAND "${CMAKE_CPACK_COMMAND}" "-C" "$<CONFIGURATION>" "--config" "${CPACK_OUTPUT_CONFIG_FILE}")
