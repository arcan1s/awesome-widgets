# Author: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>
# Maintainer: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=pytextmonitor
pkgver=1.6.0
pkgrel=1
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm"
arch=('i686' 'x86_64')
url="http://arcanis.name/projects/pytextmonitor"
license=('GPL')
depends=('kdebase-workspace' 'kdebindings-python2' 'lm_sensors' 'net-tools' 'sysstat')
optdepends=("hddtemp: for HDD temperature monitor" 
            "catalyst: for GPU monitor" 
            "nvidia-utils: for GPU monitor" 
            "amarok: for music player monitor" 
            "mpd: for music player monitor" 
            "qmmp: for music player monitor")
makedepends=('automoc4' 'cmake')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('5941cde214f5f93c3ec8963622ef1fdf')
backup=('usr/share/config/extsysmon.conf')

build () {
  if [[ -d ${srcdir}/build ]]; then
    rm -rf "${srcdir}/build"
  fi
  mkdir "${srcdir}/build"
  cd "${srcdir}/build"
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` \
        "../${_pkgname}"
  make
}

package() {
  cd "${srcdir}/build"
  make DESTDIR="${pkgdir}" install
}
