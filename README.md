awesome-widgets
===============

Information
-----------

A collection of minimalistic widgets which looks like Awesome WM widgets.

**NOTE** [LOOKING FOR TRANSLATORS!](https://github.com/arcan1s/awesome-widgets/issues/14)

Configuration
-------------

For edited output you must open Settings window and setup output format in lines:

* tag `$time$` - time in default format. For example, *fri Nov 6 04:48:01 2013*
* tag `$isotime$` - time in iso format
* tag `$shorttime$` - time in short locale format
* tag `$longtime$` - time in long locale format
* tag `$ctime$` - custom time format
* tag `$uptime$` - uptime, *---d--h--m*
* tag `$cuptime$` - custom uptime format
* tag `$cpu$` - total load cpu, *%*
* tag `$cpuN$` - load CPU for core N, *%*
* tag `$cpucl$` - average cpu clock, *MHz*
* tag `$cpuclN$` - cpu clock for core N, *MHz*
* tag `$tempN$` - temperature for device N. For example, `$temp0`
* tag `$gpu$` - GPU usage, *%*. `aticonfig` or `nvidia-smi` must be installed
* tag `$gputemp$` - GPU temperature. `aticonfig` or `nvidia-smi` must be installed
* tag `$mem$` - usage memory, *%*
* tag `$memmb$` - usage memory, *MB*
* tag `$memgb$` - usage memory, *GB*
* tag `$memfreemb$` - free RAM, *MB*
* tag `$memfreegb$` - free RAM, *GB*
* tag `$memtotmb$` - total RAM, *MB*
* tag `$memtotgb$` - total RAM, *GB*
* tag `$memusedmb$` - used and cached RAM, *MB*
* tag `$memusedgb$` - used and cached RAM, *GB*
* tag `$swap$` - swap, *%*
* tag `$swapmb$` - swap, *MB*
* tag `$swapgb$` - swap, *GB*
* tag `$swapfreemb$` - free swap, *MB*
* tag `$swapfreegb$` - free swap, *GB*
* tag `$swaptotmb$` - total swap, *MB*
* tag `$swaptotgb$` - total swap, *GB*
* tag `$hddN$` - usage for mount point N, *%*. For example, `$hdd0`
* tag `$hddmbN$` - usage for mount point N, *MB*. For example, `$hddmb0`
* tag `$hddgbN$` - usage for mount point N, *GB*. For example, `$hddgb0`
* tag `$hddfreembN$` - free space for mount point N, *MB*. For example, `$hddfreemb0`
* tag `$hddfreegbN$` - free space for mount point N, *GB*. For example, `$hddfreegb0`
* tag `$hddtotmbN$` - total size of mount point N, *MB*. For example, `$hddtotmb0`
* tag `$hddtotgbN$` - total size of mount point N, *GB*. For example, `$hddtotgb0`
* tag `$hddrN$` - read speed of disk N, *KB/s*. For example, `$hddr0`
* tag `$hddwN$` - write speed of disk N, *KB/s*. For example, `$hddw0`
* tag `$hddtempN$` - temperature for HDD N. For example, `$hddtemp0`
* tag `$down$` - download speed, *KB/s*
* tag `$up$` - upload speed, *KB/s*
* tag `$netdev$` - current network device
* tag `$bat$` - battery charge, *%*
* tag `$ac$` - status AC device. Return (\*) if AC device is online or *( )* if offline
* tag `$album$` - current song album. One of supported music players must be installed
* tag `$artist$` - current song artist. One of supported music players must be installed
* tag `$duration$` - current song duration. One of supported music players must be installed
* tag `$progress$` - current song progress. One of supported music players must be installed
* tag `$title$` - current song title. One of supported music players must be installed
* tag `$pscount$` - number of running processes
* tag `$pstotal$` - total number of processes
* tag `$ps$` - list of running processes
* tag `$pkgcountN$` - number of packages, which available to upgrade for command N. For example, `$pkgcount0`
* tag `$customN$` - returns output for custom command N. For example, `$custom0`
* tag `$desktop$` - name of the current desktop
* tag `$ndesktop$` - number of the current desktop
* tag `$tdesktops$` - total number of desktops

Tips & tricks
-------------

You may use different colors for labels. Just put label text into html code. See [issue](https://github.com/arcan1s/awesome-widgets/issues/9) for more details.

Numbering of elements of temperature, HDD usage, HDD speed, HDD temperature refers to elements order from second tab (*Advanced settings*). You should add item to the required listWidget first. And the first element in the listWidget will be `$tag0`. See [issue](https://github.com/arcan1s/awesome-widgets/issues/17) for more details.

Advanced settings
-----------------

**Enable background**

Uncheck to disable default background and set transparent one.

**Vertical layout**

Use vertical layout instead of horizontal one.

**Enable popup**

Uncheck box if you do not use popup messages.

**Add stretch**

Add stretch (spacer) to the selected side of the widget.

**Custom time**

* tag `$dddd$` - weekday in long format
* tag `$ddd$` - weekday in short format
* tag `$dd$` - day
* tag `$d$` - day without zero
* tag `$MMMM$` - month in long format
* tag `$MMM$` - month in short format
* tag `$MM$` - month
* tag `$M$` - month without zero
* tag `$yyyy$` - year
* tag `$yy$` - year in short format
* tag `$hh$` - hours
* tag `$h$` - hours without zero
* tag `$mm$` - minutes
* tag `$m$` - minutes without zero
* tag `$ss$` - seconds
* tag `$s$` - seconds without zero

**Custom uptime**

* tag `$dd$` - uptime days
* tag `$d$` - uptime days without zero
* tag `$hh$` - uptime hours
* tag `$h$` - uptime hours without zero
* tag `$mm$` - uptime minutes
* tag `$m$` - uptime minutes without zero

**Temperature units**

Select units for temperature. Available units are Celsius, Farenheit, Kelvin, Reaumur, cm^-1, kJ/mol, kcal/mol.

**Temperature devices**

List of devices, which will be observed in temp label (items come from `sensors`). List widget is editable,

**Mount points**

List of mount points, which will be observed in hdd label (items come from `mount`). List widget is editable.

**HDD (speed)**

List of hard disk devices, which will be observed in hddspeed label (items come from DataEngine). List widget is editable.

**HDD (temperature)**

List of hard disk devices, which will be observed in hddtemp label (items come from `find`). List widget is editable.

**Network device**

Use specified device as active. Will disable auto select network device.

**AC online tag**

Line which will be shown when AC is online.

**AC offline tag**

Line which will be shown when AC is offline.

Tooltip settings
----------------

Since version 1.7.0 CPU, CPU clock, memory, swap and network labels support graphical tooltip. To enable them just make the needed checkboxes a fully checked. The number of stored values can be set in the tab. Colors of graphs are configurable too.

DataEngine settings
-------------------

**AC device**

File with AC information. File (`/sys/class/power_supply/AC/online` by default) must contain `1` if AC is online.

**Battery device**

File with battery information. File (`/sys/class/power_supply/BAT0/capacity` by default) should contain only battery charge in percent.

**Custom command**

Commands, which will be run for custom label.

**Desktop cmd**

Type a command which will be run for desktop DataEngine. Default is `qdbus org.kde.kwin /KWin currentDesktop`.

**GPU device**

Select one of supported GPU devices. `auto` will enable auto selection, `disable` will disable definition of GPU states. Default is `auto`.

**HDD**

Select one of HDDs for HDD temperature monitor. `all` will enable monitor for all devices, `disable` will disable definition of HDD temperature. Default is `all`.

**hddtemp cmd**

Type a command which will be run for hddtemp DataEngine. Default is `sudo hddtemp`.

**MPD address**

Address of MPD server. Default is `localhost`.

**MPD port**

Port of MPD server. Default is `6600`.

**MPRIS player name**

MPRIS player name. The DBus interface of an music player should be enabled. Default is `auto`.

**Package manager**

List of commands, which will be run. Number of null lines is a number of unneeded lines. Defaults:

* *Arch*: `PKGCMD=pacman -Qu`, `PKGNULL=0`
* *Debian*: `PKGCMD=apt-show-versions -u -b`, `PKGNULL=0`
* *Ubuntu*: `PKGCMD=aptitude search '~U'`, `PKGNULL=0`
* *Fedora*: `PKGCMD=yum list updates`, `PKGNULL=3`
* *FreeBSD*: `PKGCMD=pkg_version -I -l '<'`, `PKGNULL=0`
* *Mandriva*: `PKGCMD=urpmq --auto-select`, `PKGNULL=0`

**Music player**

Select one of supported music playes for player label.

Desktop panel
=================

Since version 1.11.0 it provides a minimalistic panel for monitoring on desktops written on `C++`. And yes, it looks like the same panel in Awesome.

Desktop panel configuration
---------------------------

**Enable background**

Uncheck to disable default background and set transparent one.

**Vertical layout**

Use vertical layout instead of horizontal one.

**Add stretch**

Add stretch (spacer) to the selected side of the widget.

**Mark**

Type symbol (or string) which will be shown if this desktop is active now.

**Pattern tags**

* tag `$mark$` - show mark if this desktop is active
* tag `$name$` - name of the current desktop
* tag `$number$` - number of the current desktop
* tag `$total$` - total number of desktops

**Command**

Type command which will be run on left click on the selected desktop. Available variables are same as for pattern. Default is `dbus org.kde.kwin /KWin setCurrentDesktop $number$`.

Instruction
===========

Dependencies
------------

* kdebase-workspace
* kdebindings-python2
* lm_sensors (*for definition temperature device*)

Optional dependencies
---------------------

* sysstat (*for notification*)
* proprietary video driver
* hddtemp (make sure that it may be run with `sudo` without password. Just add following line to `/etc/sudoers`: `$USERNAME ALL=NOPASSWD: /usr/bin/hddtemp`)
* music player (amarok, clementine, mpd or qmmp)

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

Links
-----

* [Homepage](http://arcanis.name/projects/awesome-widgets/)
* Plasmoid on [kde-look](http://kde-look.org/content/show.php/Py+Text+Monitor?content=157124)
* DataEngine on [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-awesome-widgets/) package
