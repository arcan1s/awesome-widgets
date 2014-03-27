py-text-monitor
=============

Information
-----------
PyTextMonitor is a minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm.

Configuration
-------------
For edited output you must open Settings window and setup output format in lines:
* label `$time` - time in default format. For example, `fri Nov 6 04:48:01 2013`
* label `$isotime` - time in iso format
* label `$shorttime` - time in short locale format
* label `$longtime` - time in long locale format
* label `$uptime` - uptime, <i>---d--h--m</i>
* label `$cpu` - total load cpu, <i>%</i>
* label `$ccpu` - load CPU for each core, <i>%</i>
* label `$cpucl` - average cpu clock, <i>MHz</i>
* label `$ccpucl` - cpu clock for each core, <i>MHz</i>
* label `$temp` - average temperature in system
* label `$gpu` - GPU usage, <i>%</i>. `aticonfig` or `nvidia-smi` must be installed
* label `$gputemp` - GPU temperature. `aticonfig` or `nvidia-smi` must be installed
* label `$mem` - usage memory, <i>%</i>
* label `$memmb` - usage memory, <i>MB</i>
* label `$swap` - swap, <i>%</i>
* label `$swapmb` - swap, <i>MB</i>
* label `@@/@@` (in hdd label) - mount point (`/` in example) usage, <i>%</i>. Separator for mount points list is `;`, for example `@@/;/home;/mnt/global@@`
* label `@@/dev/sda@@` (in hddtemp label) - HDD (`/dev/sda` in example) temperature. `hddtemp` must be installed
* label `$net` - download and upload speed, <i>KB/s</i>. You may specify network device: something like `@@eth0@@`
* label `$netdev` - current network device
* label `$bat` - battery charge, <i>%</i>. Battery device may be set below. File (`/sys/class/power_supply/BAT0/capacity` by default) must contain only battery charge in percent
* label `$ac` - status AC device. Return <i>(*)</i> if AC device is online or <i>( )</i> if offline. AC device may be set below. FIle (`/sys/class/power_supply/AC/online` by default) must contain `1` if AC is online
* label `$artist` - current song artist. One of supported music players must be installed
* label `$progress` - current song progress. One of supported music players must be installed (mpd does not support yet)
* label `$time` - current song duration. One of supported music players must be installed
* label `$title` - current song title. One of supported music players must be installed

Label order will changed if you change slider position. HTML tags in label work normally.

**NOTE** you don't may set to show $cpu in swap label for example. <b>$cpu will work only in cpu label</b>.

DataEngine configuration
------------------------
You may edit DataEngine configuration. It is `/usr/share/config/extsysmon.conf` or `$HOME/share/config/extsysmon.conf` depending on the type of installation. Uncomment needed line and edit it.

TODO (wish) list
----------------
Tooltip (graphical information):
* cpu, %
* cpuclock, mhz
* memory, %
* swap, %
* network, %

Instruction
===========

Dependencies
------------
* kdebase-workspace
* kdebindings-python2
* lm_sensors (for definition temperature device)
* net-tools (for definition network device)
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
* install dataengine

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix` ../
        make && make install

  Also you may install it to "/":

        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ../
        make && sudo make install

* install plasmoid

        plasmapkg -i py-text-monitor-1.5.0.plasmoid

  Also you may install it to "/" too:

        plasmapkg -g -i py-text-monitor-1.5.0.plasmoid

Additional information
======================

Links
-----
* Plasmoid on [kde-look](http://kde-look.org/content/show.php/Py+Text+Monitor?content=157124)
* DataEngine on [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-pytextmonitor/) package
