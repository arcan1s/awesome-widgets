# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=pytextmonitor
pkgver=1.9.0
pkgrel=1
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in Awesome WM"
arch=('i686' 'x86_64')
url="http://arcanis.name/projects/pytextmonitor"
license=('GPLv3')
depends=('kdebase-workspace' 'kdebindings-python2' 'lm_sensors')
optdepends=("amarok: for music player monitor"
            "catalyst: for GPU monitor"
            "hddtemp: for HDD temperature monitor"
            "mpd: for music player monitor"
            "net-tools: for popup messages"
            "nvidia-utils: for GPU monitor"
            "qmmp: for music player monitor"
            "sysstat: for popup messages")
makedepends=('automoc4' 'cmake')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('c353d7b91178db55184ed4ea1b0b06a8')
backup=('usr/share/config/extsysmon.conf')

prepare() {
  [[ -d ${srcdir}/build ]] && rm -rf "${srcdir}/build"
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
