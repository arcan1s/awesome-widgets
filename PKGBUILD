# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

pkgname=kdeplasma-applets-awesome-widgets
_pkgname=awesome-widgets
pkgver=2.0.0
pkgrel=1
pkgdesc="Collection of minimalistic Plasmoids which look like Awesome WM widgets (ex-PyTextMonitor)"
arch=('i686' 'x86_64')
url="http://arcanis.name/projects/awesome-widgets"
license=('GPL3')
depends=('kdebase-workspace' 'lm_sensors')
optdepends=("amarok: for music player monitor"
            "clementine: for music player monitor"
            "catalyst: for GPU monitor"
            "hddtemp: for HDD temperature monitor"
            "mpd: for music player monitor"
            "nvidia-utils: for GPU monitor"
            "qmmp: for music player monitor")
makedepends=('automoc4' 'cmake')
source=(https://github.com/arcan1s/awesome-widgets/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('a09199150ec3c1e2303ebe5878ba1913')
backup=('usr/share/config/extsysmon.conf')

prepare() {
  rm -rf "${srcdir}/build"
  mkdir "${srcdir}/build"
}

build () {
  cd "${srcdir}/build"
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$(kde4-config --prefix) \
        "../${_pkgname}"
  make
}

package() {
  cd "${srcdir}/build"
  make DESTDIR="${pkgdir}" install
}
