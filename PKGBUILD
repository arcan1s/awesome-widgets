# Author: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>
# Maintainer: Evgeniy "arcanis" Alexeev <esalexeev@gmail.com>

pkgname=kdeplasma-applets-pytextmonitor
_pkgname=py-text-monitor
pkgver=1.5.0
pkgrel=1
_dtengine=ext-sysmon
_dtver=1.3
pkgdesc="Minimalistic Plasmoid script written on Python2. It looks like widgets in awesome-wm"
arch=('i686' 'x86_64')
url="https://github.com/arcan1s/pytextmonitor"
license=('GPL')
depends=('kdebase-workspace' 'kdebindings-python2' 'lm_sensors' 'net-tools' 'sysstat')
optdepends=("hddtemp: for HDD temperature monitor" 
            "catalyst: for GPU monitor" 
            "nvidia-utils: for GPU monitor" 
            "amarok: for music player monitor" 
            "mpd: for music player monitor" 
            "qmmp: for music player monitor")
makedepends=('automoc4' 'cmake')
source=(https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}.plasmoid
        https://github.com/arcan1s/pytextmonitor/releases/download/V.${pkgver}/${_dtengine}-${_dtver}.zip)
install=${pkgname}.install
md5sums=('df8e9f6df8e7b3caee3a502315756d81'
         'f7fce53d5f616891b30beac1afd99728')

build ()
{
  # build dataengine
  if [[ -d ${srcdir}/${_dtengine}/build ]]; then
    rm -rf "${srcdir}/${_dtengine}/build"
  fi
  mkdir "${srcdir}/${_dtengine}/build"; cd "${srcdir}/${_dtengine}/build"
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` \
        ../
  make
}

package()
{
  # install dataengine
  cd "${srcdir}/${_dtengine}/build"
  make DESTDIR="${pkgdir}" install

  # install plasmoid
  install -D -m644 "${srcdir}/contents/code/plasma_applet_pytextmonitor.notifyrc" \
                   "${pkgdir}/`kde4-config --prefix`/share/apps/plasma_applet_pytextmonitor/plasma_applet_pytextmonitor.notifyrc"
  install -D -m644 "${srcdir}/metadata.desktop" \
                   "${pkgdir}/`kde4-config --prefix`/share/kde4/services/${_pkgname}.desktop"
  install -D -m644 "${srcdir}/metadata.desktop" \
                   "${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/metadata.desktop"
  mkdir -p "${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/"{code,ui}
  install -m644 "${srcdir}/contents/code/"* \
                -t "${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/code"
  install -m644 "${srcdir}/contents/ui/"* \
                -t "${pkgdir}/`kde4-config --prefix`/share/apps/plasma/plasmoids/${_pkgname}/contents/ui"
}
