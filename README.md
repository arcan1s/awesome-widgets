py-text-monitor
=============

Information
-----------
PyTextMonitor is a minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm.

Configuration
-------------
For edited output you must open Settings window and setup output format in lines:
* label "$uptime" means uptime, ---d--h--m
* label "$cpu" means total load cpu, %
* label "$ccpu" means load CPU for each core, %
* label "$cpucl" - average cpu clock, MHz
* label "$ccpucl" - cpu clock for each core, MHz
* label "$temp" - average temperature in system
* label "$gpu" - GPU usage, %. `aticonfig` or `nvidia-smi` must be installed
* label "$gputemp" - GPU temperature. `aticonfig` or `nvidia-smi` must be installed
* label "$mem" - usage memory, %
* label "$memmb" - usage memory, MB
* label "$swap" - swap, %
* label "$swapmb" - swap, MB
* label "@@/@@" (in hdd label) - mount point ('/' in example) usage, %. Separator for mount points list is ';', for example "@@/;/home;/mnt/global@@". `hddtemp` must be installed
* label "@@/dev/sda@@" (in hddtemp label) - HDD ('/dev/sda' in example) temperature
* label "$net" - download and upload speed, KB/s. You may specify network device: something like @@eth0@@
* label "$netdev" - current network device
* label "$bat" - battery charge, %. Battery device may be set below. File ("/sys/class/power_supply/BAT0/capacity" by default) must contain only battery charge in percent
* label "$ac" - status AC device. Return (*) if AC device is online or ( ) if offline. AC device may be set below. FIle ("/sys/class/power_supply/AC/online" by default) must contain '1' if AC is online
* label "$artist" - current song artist. One of supported music players must be installed
* label "$title" - current song title. One of supported music players must be installed

Label order will changed if you change slider position. HTML tags in label work normally.

**NOTE** you don't may set to show $cpu in swap label for example. $cpu will work only in cpu label.

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
