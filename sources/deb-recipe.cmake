set(CPACK_GENERATOR "DEB")
set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/DebPackageConfig.cmake")

# versioning and naming
set(CPACK_PACKAGE_NAME "plasma-widget-awesome-widgets")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_RELEASE "1")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

# metadata
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR}")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Collection of minimalistic Plasmoids which look like Awesome WM widgets (ex-PyTextMonitor)")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://arcanis.name/projects/awesome-widgets")
set(CPACK_DEBIAN_PACKAGE_SECTION "KDE")

# build data
set(CPACK_DEBIAN_PACKAGE_DEPENDS "plasma-framework")
set(CPACK_DEBIAN_PACKAGE_ENHANCES "mpd, smartmontools")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# go go go!
include(CPack)
add_custom_target(deb COMMAND "${CMAKE_CPACK_COMMAND}" "-C" "$<CONFIGURATION>" "--config" "${CPACK_OUTPUT_CONFIG_FILE}")
