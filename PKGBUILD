# Author: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>
# Maintainer: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=py-text-monitor
pkgver=1.3.4
pkgrel=6
_dtengine=ext-sysmon
_dtver=1.2
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm"
arch=(any)
url="https://github.com/arcan1s/pytextmonitor"
license=('GPL')
depends=('kdebase-workspace' 'kdebindings-python2' 'lm_sensors' 'net-tools')
optdepends=("hddtemp: for HDD temperature monitor"
            "catalyst: for GPU monitor"
            "nvidia-utils: for GPU monitor")
makedepends=('cmake' 'automoc4')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}.plasmoid
        https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_dtengine}-${_dtver}.zip)
install=${pkgname}.install
md5sums=('7cca5fd76735add215f35d406c020dea'
         '9b547781732ab19ad103710b7bd07d8d')

build ()
{
# build dataengine
  if [ -d ${srcdir}/${_dtengine}/build ]; then
    rm -r ${srcdir}/${_dtengine}/build
  fi
  mkdir ${srcdir}/${_dtengine}/build && cd ${srcdir}/${_dtengine}/build
  cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` -DCMAKE_BUILD_TYPE=Release ../
  make || return 1
}

package()
{
# install dataengine
  cd ${srcdir}/${_dtengine}/build
  make DESTDIR=${pkgdir} install || return 1

# install plasmoid
  install -D ${srcdir}/metadata.desktop ${pkgdir}/`kde4-config --prefix`/share/kde4/services/${_pkgname}.desktop || return 1
  install -D ${srcdir}/metadata.desktop ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/metadata.desktop || return 1
  cp -r ${srcdir}/contents ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/ || return 1
}
