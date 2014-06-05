pytextmonitor
=============

Información
-----------
PyTextMonitor es un plasmoide minimalista escrito en Python2. Luce como los componentes visuales de awesome-wm.

**NOTA** [¡Se buscan traductores!](https://github.com/arcan1s/pytextmonitor/issues/14)

Configuración
-------------

Para editar la visualización debes abrir la ventana de Ajustes y establecer el formato de visualización en líneas:

* etiqueta `$time` - hora en formato por defecto. Por ejemplo, *vie 06 Nov 04:48:01 2013*
* etiqueta `$isotime` - hora en formato ISO
* etiqueta `$shorttime` - hora en formato local corto
* etiqueta `$longtime` - hora en formato local largo
* etiqueta `$custom` - formato de hora personalizado
* etiqueta `$uptime` - tiempo de actividad del sistema, *---d--h--m*
* etiqueta `$custom` - formato personalizado del tiempo de actividad del sistema
* etiqueta `$cpu` - carga total de la CPU, *%*
* etiqueta `$cpuN` - carga de la CPU para el núcleo N, *%*
* etiqueta `$cpucl` - frecuencia promedio de la CPU, *MHz*
* etiqueta `$cpuclN` - frecuencia de la CPU para el núcleo N, *MHz*
* etiqueta `$tempN` - temperatura para el dispositivo N. Por ejemplo, `$temp0`
* etiqueta `$gpu` - uso de la GPU, *%*. Debe estar instalado `aticonfig` o `nvidia-smi`
* etiqueta `$gputemp` - temperatura de la GPU. Debe estar instalado `aticonfig` o `nvidia-smi`
* etiqueta `$mem` - uso de la memoria, *%*
* etiqueta `$memmb` - uso de la memoria, *MB*
* etiqueta `$memgb` - uso de la memoria, *GB*
* etiqueta `$memtotmb` - uso total de memoria, *MB*
* etiqueta `$memtotgb` - uso total de memoria, *GB*
* etiqueta `$swap` - swap, *%*
* etiqueta `$swapmb` - swap, *MB*
* etiqueta `$swapgb` - swap, *GB*
* etiqueta `$swaptotmb` - uso total de swap, *MB*
* etiqueta `$swaptotgb` - uso total de swap, *GB*
* etiqueta `$hddN` - uso para el punto de montaje N, *%*. Por ejemplo, `$hdd0`
* etiqueta `$hddmbN` - uso para el punto de montaje N, *MB*. Por ejemplo, `$hddmb0`
* etiqueta `$hddgbN` - uso para el punto de montaje N, *GB*. Por ejemplo, `$hddgb0`
* etiqueta `$hddtotmbN` - tamaño completo del punto de montaje N, *MB*. Por ejemplo, `$hddtotmb0`
* etiqueta `$hddtotgbN` - tamaño completo del punto de montaje N, *GB*. Por ejemplo, `$hddtotgb0`
* etiqueta `$hddrN` - leer la velocidad del disco N, *KB/s*. Por ejemplo, `$hddr0`
* etiqueta `$hddwN` - velocidad de escritura del disco N, *KB/s*. Por ejemplo: `$hddw0`
* etiqueta `$hddtempN` - temperatura para el disco duro N. Por ejemplo, `$hddtemp0`

**NOTA** no puedes establecer la etiqueta `$cpu` para que se muestre en la información de la swap, por citar un ejemplo. **`$cpu` solo funcionará en la etiqueta de la CPU**.

Trucos
------

Es posible usar diferentes colores para las etiquetas. Solo tienes que ponerles el texto en formato HTML. Para más detalles, ver [el tema](https://github.com/arcan1s/pytextmonitor/issues/9) relacionado.

La enumeración de los elementos de temperatura, uso del disco duro, velocidad del disco duro y la temperatura del disco duro, se refiere al orden de los elementos de la segunda pestaña (*Ajustes avanzados*). Primeramente deberías añadir el elemento a la lista requerida. El primer elemento de la lista será $tag0. Para más información, ver [el tema](https://github.com/arcan1s/pytextmonitor/issues/17) relacionado.

Ajustes avanzados
-----------------

**Orientación vertical**

Usar la orientación vertical en lugar de la horizontal.

**Habilitar ventanas emergentes**

Desmarcar esta caja si no deseas usar ventanas emergentes.

**Añadir extensión**

Añadir extensión (espaciador) al lado seleccionado del widget.

**Hora personalizada**

* etiqueta `$dddd` - día de la semana en foramto largo
* etiqueta `$ddd` - día de la semana en formato corto
* etiqueta `$dd` - día
* etiqueta `$d` - día sin ceros
* etiqueta `$MMMM` - mes en formato largo
* etiqueta `$MMM` - mes en formato corto
* etiqueta `$MM` - mes
* etiqueta `$M` - mes sin ceros
* etiqueta `$yyyy` - año
* etiqueta `$yy` - año en formato corto
* etiqueta `$hh` - horas
* etiqueta `$h` - horas sin ceros
* etiqueta `$mm` - minutos
* etiqueta `$m` - minutos sin ceros
* etiqueta `$ss` - segundos
* etiqueta `$s` - segundos sin ceros

**Tiempo de ejecución del sistema personalizado**

* etiqueta `$dd` - días
* etiqueta `$d` - días sin ceros
* etiqueta `$hh` - horas
* etiqueta `$h` - horas sin ceros
* etiqueta `$mm` - minutos
* etiqueta `$m` - minutos sin ceros

**Unidades de temperatura**

Elija las unidades de temperatura. Las unidades disponibles son: Celsius, Farenheit y Kelvin.

**Dispositivos de temperatura**

El listado de dispositivos, será observado en la etiqueta de temperatura (los elementos de la lista desplegable provienen de `sensors`). El listado es editable, la tecla Suprimir borrará el elemento activo.

**Puntos de montaje**

El listado de puntos de montaje, será observado en la etiqueta de disco duro (los elementos de la lista desplegable provienen de `mount`). El listado es editable, la tecla Suprimir borrará el elemento activo.

**Disco duro (velocidad)**

Listado de dispositivos de disco duro, será observado en la etiqueta hddspeed (los elementos de la lista desplegable provienen de DataEngine).

**Disco duro (temperatura)**

El listado de dispositivos de disco duro, será observado desde la etiqueta disco duro (los elementos de la lista desplegable provienen de `find`). El listado es editable, la tecla Suprimir borrará el elemento activo.

**Directorio de red**

Ruta hacia el directorio que contiene información de los dispositivos de red. Por defecto es `/sys/class/net`. Se requiere para la selección automática del dispositivo de red.

**Dispositivo de red**

Usar el dispositivo especificado como activo. Los elementos de la lista desplegable provienen del **directorio de red**. Esto inhabilitará la selección automática del dispositivo de red.

**Dispositivo de batería**

El archivo que contiene información de la batería. El archivo (`/sys/class/power_supply/BAT0/capacity` por defecto) solo debería contener la carga de la batería en por ciento.

**Etiqueta alimentación conectada**

Línea que será mostrada cuando la alimentación esté conectada.

**Etiqueta alimentación conectada**

Línea que será mostrada cuando la alimentación esté desconectada.

**Dispositivo de alimentación**

El archivo con información de la alimentación. El archivo (`/sys/class/power_supply/AC/online` por defecto) debe contener `1` si la alimentación está conectada.

Ajustes de las ventanas emergentes
----------------------------------

Desde la versión 1.7.0 las etiquetas CPU, frecuencia de la CPU, memoria, swap y red soportan ventanas emergentes. Para habilitarlas es necesario tener marcadas las casillas de verificación pertinentes. El número de valores almacenados puede ser establecido en la pestaña. También es posible configurar los colores de los gráficos.

Ajustes de DataEngine
---------------------

**Comando personalizado**

*NOTA* Esto puede causar congelamiento de la computadora.

**Dispositivo GPU**

Elegir uno de los dispositivos GPU soportados. `auto` habilitará la selección automática, `disable` inhabilitará la definición de los estados de la GPU. El valor por defecto es `auto`.

**Disco duro**

Elegir uno de los discos duros para el monitor de temperatura del disco. `all` habilitará el monitor para todos los dispositivos, `disable` inhabilitará la definición de la temperatura del disco. El valor por defecto es `all`.

**Comando hddtemp**

Escriba el comando que será ejecutado por el DataEngine hddtemp. El valor por defecto es `sudo hddtemp`.

**Dirección MPD**

Dirección del servidor MPD. El valor por defecto es `localhost`.

**Puerto MPD**

Puerto del servidor MPD. El valor por defecto es `6600`.

**Gestor de paquetes**

Listado de comandos que serán ejecutados. La cantidad de líneas nulas es el número de líneas innecesarias. Predeterminados:

* *Arch*: `PKGCMD=pacman -Qu`, `PKGNULL=0`
* *Debian*: `PKGCMD=apt-show-versions -u -b`, `PKGNULL=0`
* *Ubuntu*: `PKGCMD=aptitude search '~U'`, `PKGNULL=0`
* *Fedora*: `PKGCMD=yum list updates`, `PKGNULL=3`
* *FreeBSD*: `PKGCMD=pkg_version -I -l '<'`, `PKGNULL=0`
* *Mandriva*: `PKGCMD=urpmq --auto-select`, `PKGNULL=0`

**Reproductor de música**

Selecciona uno de los reproductores de música soportados para la etiqueta del reproductor.

Configuración de DataEngine
---------------------------
Se puede editar la configuración de DataEngine. Se encuentra en `/usr/share/config/extsysmon.conf` o `$HOME/share/config/extsysmon.conf` en dependencia del tipo de instalación. Quitar el comentario de la línea requerida y editarlo.

Instrucción
===========

Dependencias
------------

* kdebase-workspace
* kdebindings-python2
* lm_sensors (*para definir el dispositivo de temperatura*)

Dependencias opcionales
-----------------------

* sysstat (*para las notificaciones*)
* controlador propietario de video
* hddtemp (asegúrate de que puede ser ejecutado con `sudo` y sin contraseña. Solo hay que agregar la siguiente línea al archivo `/etc/sudoers`: `$USERNAME ALL=NOPASSWD: /usr/bin/hddtemp`)
* reproductor de música (amarok, clementine, mpd o qmmp)

Dependencias de construcción
----------------------------

* automoc4
* cmake

Instalación
-----------
* descargar fuentes
* instalar

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix` ../
        make && make install

  También es posible instalarlo en `/`:

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ../
        make && sudo make install

Información adicional
=====================

TODO (funcionalidades solicitadas)
----------------------------------

Enlaces
-----
* [Sitio oficial](http://arcanis.name/projects/pytextmonitor/)
* Plasmoide en [kde-look](http://kde-look.org/content/show.php/Py+Text+Monitor?content=157124)
* DataEngine en [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Archlinux [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-pytextmonitor/) package
