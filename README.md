pytextmonitor
=============

Information
-----------
PyTextMonitor is a minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm.

**NOTE** LOOKING FOR TRANSLATORS!

Configuration
-------------
For edited output you must open Settings window and setup output format in lines:
* label `$time` - time in default format. For example, `fri Nov 6 04:48:01 2013`
* label `$isotime` - time in iso format
* label `$shorttime` - time in short locale format
* label `$longtime` - time in long locale format
* label `$custom` - custom time format
* label `$uptime` - uptime, <i>---d--h--m</i>
* label `$custom` - custom uptime format
* label `$cpu` - total load cpu, <i>%</i>
* label `$cpuN` - load CPU for core N, <i>%</i>. N should be in range 0 to 8
* label `$cpucl` - average cpu clock, <i>MHz</i>
* label `$cpuclN` - cpu clock for core N, <i>MHz</i>. N should be in range 0 to 8
* label `$tempN` - temperature for device N. For example, `$temp0`
* label `$gpu` - GPU usage, <i>%</i>. `aticonfig` or `nvidia-smi` must be installed
* label `$gputemp` - GPU temperature. `aticonfig` or `nvidia-smi` must be installed
* label `$mem` - usage memory, <i>%</i>
* label `$memmb` - usage memory, <i>MB</i>
* label `$swap` - swap, <i>%</i>
* label `$swapmb` - swap, <i>MB</i>
* label `$hddN` - usage for mount point N, <i>%</i>. For example, `$hdd0`
* label `$hddtempN` - temperature for HDD N. For example, `$hddtemp0`
* label `$down` - download speed, <i>KB/s</i>
* label `$up` - upload speed, <i>KB/s</i>
* label `$netdev` - current network device
* label `$bat` - battery charge, <i>%</i>
* label `$ac` - status AC device. Return <i>(*)</i> if AC device is online or <i>( )</i> if offline
* label `$album` - current song album. One of supported music players must be installed
* label `$artist` - current song artist. One of supported music players must be installed
* label `$progress` - current song progress. One of supported music players must be installed (mpd does not support yet)
* label `$time` - current song duration. One of supported music players must be installed
* label `$title` - current song title. One of supported music players must be installed
* label `$custom` - returns output for custom command

Label order will changed if you change slider position. HTML tags in label work normally.

**NOTE** you don't may set to show $cpu in swap label for example. <b>$cpu will work only in cpu label</b>.

Advanced settings
-----------------
**Custom time**

* label `$dddd` - weekday in long format
* label `$ddd` - weekday in short format
* label `$dd` - day
* label `$d` - day without zero
* label `$MMMM` - month in long format
* label `$MMM` - month in short format
* label `$MM` - month
* label `$M` - month without zero
* label `$yyyy` - year
* label `$yy` - year in short format
* label `$hh` - hours
* label `$h` - hours without zero
* label `$mm` - minutes
* label `$m` - minutes without zero
* label `$ss` - seconds
* label `$s` - seconds without zero

**Custom uptime**

* label `$ds` - uptime days
* label `$hs` - uptime hours
* label `$ms` - uptime minutes

**Temperature devices**

List of devices, which will be observed in temp label (combo box items come from `sensors`). List widget is editable, delete key will remove current item.

**Mount points**

List of mount points, which will be observed in hdd label (combo box items come from `mount`). List widget is editable, delete key will remove current item.

**HDD**

List of hard disk devices, which will be observed in hddtem label (combo box items come from `find`). List widget is editable, delete key will remove current item.

**Network directory**

Path to directory, which contains network devices information. Default is `/sys/class/net`. Required for auto select network device.

**Network device**

Use specified device as active. Combo box items come from **network directory**. Will disable auto select network device.

**Battery device**

File with battery information. File (`/sys/class/power_supply/BAT0/capacity` by default) should contain only battery charge in percent.

**AC device**

File with AC information. File (`/sys/class/power_supply/AC/online` by default) must contain `1` if AC is online.

**Music player**

Select one of supported music playes for player label.

Tooltip settings
----------------
Since version 1.7.0 CPU, CPU clock, memory, swap and network labels support graphical tooltip. To enable them just make the needed checkboxes a fully checked. The number of stored values can be set in the tab. Colors of graphs are configurable too.

DataEngine settings
-------------------
**GPU device**

Select one of supported GPU devices. `auto` will enable auto selection. Default is `auto`.

**HDD**

Select one of HDDs for HDD temperature monitor. `all` will enable monitor for all devices. Default is `all`.

**MPD address**

Address of MPD server. Default is `localhost`.

**MPD port**

Port of MPD server. Default is `6600`.

**Custom command**

*NOTE* This can cause the computer freeze.

A command, which will be run for custom label.

DataEngine configuration
------------------------
You may edit DataEngine configuration. It is `/usr/share/config/extsysmon.conf` or `$HOME/share/config/extsysmon.conf` depending on the type of installation. Uncomment needed line and edit it.

Instruction
===========

Dependencies
------------
* kdebase-workspace
* kdebindings-python2
* lm_sensors (for definition temperature device)
* sysstat (for notification)

Optional dependencies
---------------------
ext-sysmon (for GPU, GPU temp, HDD temp and player labels):
* proprietary video driver
* hddtemp
* music player (amarok, mpd or qmmp)

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

  Also you may install it to "/":

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ../
        make && sudo make install

Additional information
======================

Links
-----
* [Homepage](http://arcanis.name/projects/pytextmonitor/)
* Plasmoid on [kde-look](http://kde-look.org/content/show.php/Py+Text+Monitor?content=157124)
* DataEngine on [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-pytextmonitor/) package
