pytextmonitor
=============

PyTextMonitor is a minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm.

HOWTO
For edited output you must open Settings window and setup output format in lines:
  label "$cpu" means total load cpu, %
  label "$cpucl" means average cpu clock, MHz
  label "$mem" - usage memory, %
  label "$memmb" - usage memory, MB
  label "$swap" - swap, %
  label "$swapmb" - swap, MB
  label "$temp" - average temperature in system (if doesn't work please let me know)
  label "$net" - download and upload speed, KB/s. You may also specify the number of devices (1 or 2) - without their names. And you may specify network device: something like @@netdev=eth0@@
  label "$netdev" - current network device
  label "$bat" - battery charge, %. "acpi" must be installed (if doesn't work please let me know)
Label order will changed if you change slider position.
Attention: you don't may set to show $cpu in swap label for example. $cpu will work only in cpu label.