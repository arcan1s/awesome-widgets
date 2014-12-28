awesome-widgets (ex-pytextmonitor)
==================================

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

* kdebase-workspace
* lm_sensors (*for definition temperature device*)

Optional dependencies
---------------------

* proprietary video driver
* hddtemp
* music player (mpd or supported MPRIS)

Make dependencies
-----------------

* automoc4
* cmake

Installation
------------

* download sources
* install

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix` ../
        make && make install

  Also you may install it to `/`:

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ../
        make && sudo make install

Additional information
======================

TODO (wish list)
----------------

See [milestones](https://github.com/arcan1s/awesome-widgets/milestones) for more details.

Links
-----

* [Homepage](http://arcanis.name/projects/awesome-widgets/)
* [Migration to 2.*](http://arcanis.name/en/2014/09/04/migration-to-v2/)
* [Scripts and bars](http://arcanis.name/en/2014/12/19/aw-v21-bells-and-whistles/)
* Plasmoid on [kde-look](http://kde-look.org/content/show.php/Awesome+Widgets?content=157124)
* DataEngine on [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-awesome-widgets/) package
