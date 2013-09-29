# Author: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>
# Maintainer: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=py-text-monitor
pkgver=1.3.6
pkgrel=1
_dtengine=ext-sysmon
_dtver=1.2
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm"
arch=('i686' 'x86_64')
url="https://github.com/arcan1s/pytextmonitor"
license=('GPL')
depends=('kdebase-workspace' 'kdebindings-python2' 'lm_sensors' 'net-tools')
optdepends=("hddtemp: for HDD temperature monitor" 
            "catalyst: for GPU monitor" 
            "nvidia-utils: for GPU monitor")
makedepends=('automoc4' 'cmake')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}.plasmoid
        https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_dtengine}-${_dtver}.zip)
install=${pkgname}.install
md5sums=('5f619e6770bec8dbf7d18907d3a2143e'
         '775bc3a6057f2dc1696f09b8d315a3e9')

build ()
{
  # build dataengine
  if [ -d ${srcdir}/${_dtengine}/build ]; then
    rm -r ${srcdir}/${_dtengine}/build
  fi
  mkdir ${srcdir}/${_dtengine}/build && cd ${srcdir}/${_dtengine}/build
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` \
        ../
  make
}

package()
{
  # install dataengine
  cd ${srcdir}/${_dtengine}/build
  make DESTDIR=${pkgdir} install

  # install plasmoid
  install -D -m644 ${srcdir}/metadata.desktop \
                   ${pkgdir}/`kde4-config --prefix`/share/kde4/services/${_pkgname}.desktop
  install -D -m644 ${srcdir}/metadata.desktop \
                   ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/metadata.desktop
  mkdir -p ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/{code,ui}
  install -m644 ${srcdir}/contents/code/* \
                -t ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/code
  install -m644 ${srcdir}/contents/ui/* \
                -t ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/ui
}
