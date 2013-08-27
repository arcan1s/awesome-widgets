# Author: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>
# Maintainer: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=py-text-monitor
pkgver=1.3.4
pkgrel=3
_dtengine=ext-sysmon
_dtver=1.1
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm"
arch=(any)
url="https://github.com/arcan1s/pytextmonitor"
license=('GPL')
depends=('kdebindings-python2' 'lm_sensors' 'net-tools')
optdepends=('hddtemp: for HDD temperature monitor'
            'catalyst: for GPU monitor'
            'nvidia-utils: for GPU monitor')
makedepends=('unzip' 'cmake' 'automoc4')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.1.3.4/${_pkgname}-${pkgver}.plasmoid
        https://github.com/arcan1s/pytextmonitor/releases/download/V.1.3.4/${_dtengine}-${_dtver}.zip)
md5sums=('cdc55960177a07b52ab0a2713f5df212'
         '0c5580c99d70221cf58d600e0a0db5de')

build ()
{
# build dataengine
  unzip -o -q ${srcdir}/${_dtengine}-${_dtver}.zip -d ${srcdir}/${_dtengine}
  if [ -d ${srcdir}/${_dtengine}/build ]; then
    rm -r build
  fi
  mkdir ${srcdir}/${_dtengine}/build && cd ${srcdir}/${_dtengine}/build
  cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ../
  make
}

package() 
{
# install dataengine
  cd ${srcdir}/${_dtengine}/build
  make DESTDIR=${pkgdir} install

# install plasmoid
  unzip -o -q ${srcdir}/${_pkgname}-${pkgver}.plasmoid -d ${srcdir}/${_pkgname}
  install -D ${srcdir}/${_pkgname}/metadata.desktop ${pkgdir}/`kde4-config --prefix`/share/kde4/services/${_pkgname}.desktop
  mkdir -p ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}
  cp -r ${srcdir}/${_pkgname}/* ${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/
}
