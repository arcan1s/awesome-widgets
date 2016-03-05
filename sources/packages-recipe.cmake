# generators
if (BUILD_DEB_PACKAGE)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} "DEB")
endif ()
if (BUILD_RPM_PACKAGE)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} "RPM")
endif ()

# build properties
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# general metadata
set(CPACK_PACKAGE_NAME "plasma-widget-awesome-widgets")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_RELEASE "1")
# doesn't match to official arch :/
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

# deb specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS "plasma-framework")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Collection of minimalistic Plasmoids which look like Awesome WM widgets (ex-PyTextMonitor)")
set(CPACK_DEBIAN_PACKAGE_ENHANCES "mpd, smartmontools")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://arcanis.me/projects/awesome-widgets")
set(CPACK_DEBIAN_PACKAGE_SECTION "KDE")

# rpm specific
set(CPACK_RPM_PACKAGE_DESCRIPTION "Collection of minimalistic Plasmoids which look like Awesome WM widgets (ex-PyTextMonitor)")
set(CPACK_RPM_PACKAGE_GROUP "User Interface/Desktops")
set(CPACK_RPM_PACKAGE_LICENSE "${PROJECT_LICENSE}")
set(CPACK_RPM_PACKAGE_REQUIRES "plasma-framework")
set(CPACK_RPM_PACKAGE_SUGGESTS "mpd, smartmontools")
set(CPACK_RPM_PACKAGE_URL "https://arcanis.me/projects/awesome-widgets")

if (BUILD_DEB_PACKAGE OR BUILD_RPM_PACKAGE)
    include(CPack)
endif ()
