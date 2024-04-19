awesome-widgets (ex-pytextmonitor)
==================================

[![build & tests](https://github.com/arcan1s/awesome-widgets/actions/workflows/build.yml/badge.svg)](https://github.com/arcan1s/awesome-widgets/actions/workflows/build.yml)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8254/badge.svg)](https://scan.coverity.com/projects/awesome-widgets)

Information
-----------

A collection of minimalistic widgets which looks like Awesome Window Manager widgets.

**NOTE** [LOOKING FOR TRANSLATORS!](https://github.com/arcan1s/awesome-widgets/issues/14)

Features
========

* easy and fully configurable native Plasma widget which may be used as desktop or panel widget
* additionnal widget which shows active desktop status
* clear text configuration with html tags support
* custom command support (it may be simple action as well as special custom tag)
* graphical widgets support - tooltips, bars

See [links](#Links) for more details.

**Didn't find required feature?** [Just ask for it!](https://github.com/arcan1s/awesome-widgets/issues)

Instruction
===========

Dependencies
------------

* plasma-workspace

Optional dependencies
---------------------

* music player (mpd or MPRIS supported)
* wireless_tools

Make dependencies
-----------------

* cmake
* extra-cmake-modules

In addition, some distros might require to install some -dev packages, e.g. the list of required packages for deb-based distros can be found [here](https://github.com/arcan1s/awesome-widgets/blob/development/.docker/Dockerfile-ubuntu-amd64#L7).

Installation
------------

* download sources
* build package

      cmake -B build -S sources -DCMAKE_BUILD_TYPE=Release
      cmake --build build

* install package 

      cmake --install build

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
