awesome-widgets (ex-pytextmonitor)
==================================

[![Build Status](https://travis-ci.org/arcan1s/awesome-widgets.svg?branch=development)](https://travis-ci.org/arcan1s/awesome-widgets)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8254/badge.svg)](https://scan.coverity.com/projects/awesome-widgets)

Information
-----------

A collection of minimalistic widgets which looks like Awesome Window Manager widgets.

**NOTE** [LOOKING FOR TRANSLATORS!](https://github.com/arcan1s/awesome-widgets/issues/14)

Features
========

* easy and fully configurable native Plasma widget which may be used as Conky widget or as Awesome-like information panel
* panel which shows active desktop status
* clear Conky-like configuration with html tags support
* custom command support (it may be simple action as well as special custom tag)
* graphical item support - tooltips, bars

See [links](#Links) for more details.

**Didn't find required feature?** [Just ask for it!](https://github.com/arcan1s/awesome-widgets/issues)

Instruction
===========

Dependencies
------------

* plasma-framework

Optional dependencies
---------------------

* proprietary video driver
* hddtemp
* smartmontools
* music player (mpd or MPRIS supported)

Make dependencies
-----------------

* cmake
* extra-cmake-modules

Installation
------------

* download sources
* install

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ../
        make && sudo make install

  **NOTE** on Plasma 5 it very likely requires `-DKDE_INSTALL_USE_QT_SYS_PATHS=ON` flag

Additional information
======================

TODO (wish list)
----------------

See [milestones](https://github.com/arcan1s/awesome-widgets/milestones) for more details.

Links
-----

* [Homepage](https://arcanis.me/projects/awesome-widgets/)
* [Migration to 2.*](https://arcanis.me/en/2014/09/04/migration-to-v2/)
* [Scripts and bars](https://arcanis.me/en/2014/12/19/aw-v21-bells-and-whistles/)
* Plasmoid on [kde-look](http://kde-look.org/content/show.php/Awesome+Widgets?content=157124)
* DataEngine on [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/plasma5-applet-awesome-widgets/) package
* [OpenSuse](http://software.opensuse.org/search?q=awesome-widgets) packages, [KF5 OpenSuse](http://software.opensuse.org/package/plasma5-awesome-widgets) package
