# -*- coding: utf-8 -*-

############################################################################
#   This file is part of pytextmonitor                                     #
#                                                                          #
#   pytextmonitor is free software: you can redistribute it and/or         #
#   modify it under the terms of the GNU General Public License as         #
#   published by the Free Software Foundation, either version 3 of the     #
#   License, or (at your option) any later version.                        #
#                                                                          #
#   pytextmonitor is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#   GNU General Public License for more details.                           #
#                                                                          #
#   You should have received a copy of the GNU General Public License      #
#   along with pytextmonitor. If not, see http://www.gnu.org/licenses/     #
############################################################################

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.kdecore import *
from PyKDE4.kdeui import *
import commands
import config



class ConfigDefinition:
    def __init__(self, parent, configpage, defaults):
        """class definition"""
        # debug
        environment = QProcessEnvironment.systemEnvironment()
        debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
        if (debugEnv == QString("yes")):
            self.debug = True
        else:
            self.debug = False
        # main
        self.parent = parent
        self.configpage = configpage
        self.defaults = defaults


    def configAccepted(self):
        """function to accept settings"""
        if self.debug: qDebug("[PTM] [configdef.py] [configAccepted]")
        settings = config.Config(self.parent)

        # update local variables
        settings.set('interval', self.configpage['appearance'].ui.spinBox_interval.value())
        settings.set('font_family', str(self.configpage['appearance'].ui.fontComboBox.currentFont().family()))
        settings.set('font_size', self.configpage['appearance'].ui.spinBox_fontSize.value())
        settings.set('font_color', str(self.configpage['appearance'].ui.kcolorcombo.color().name()))
        settings.set('font_style', str(self.configpage['appearance'].ui.comboBox_style.currentText()))
        settings.set('font_weight', self.configpage['appearance'].ui.spinBox_weight.value())

        settings.set('background', self.configpage['advanced'].ui.checkBox_background.checkState())
        settings.set('layout', self.configpage['advanced'].ui.checkBox_layout.checkState())
        settings.set('popup', self.configpage['advanced'].ui.checkBox_popup.checkState())
        settings.set('left_stretch', self.configpage['advanced'].ui.checkBox_leftStretch.checkState())
        settings.set('right_stretch', self.configpage['advanced'].ui.checkBox_rightStretch.checkState())
        settings.set('custom_time', str(self.configpage['advanced'].ui.lineEdit_timeFormat.text()))
        settings.set('custom_uptime', str(self.configpage['advanced'].ui.lineEdit_uptimeFormat.text()))
        settings.set('temp_units', str(self.configpage['advanced'].ui.comboBox_tempUnits.currentText()))
        item = QStringList()
        for i in range(self.configpage['advanced'].ui.listWidget_tempDevice.count()):
            if (self.configpage['advanced'].ui.listWidget_tempDevice.item(i).checkState() == Qt.Checked):
                item.append(self.configpage['advanced'].ui.listWidget_tempDevice.item(i).text())
        settings.set('temp_device', str(item.join(QString('@@'))))
        item = QStringList()
        for i in range(self.configpage['advanced'].ui.listWidget_mount.count()):
            if (self.configpage['advanced'].ui.listWidget_mount.item(i).checkState() == Qt.Checked):
                item.append(self.configpage['advanced'].ui.listWidget_mount.item(i).text())
        settings.set('mount', str(item.join(QString('@@'))))
        item = QStringList()
        for i in range(self.configpage['advanced'].ui.listWidget_hddSpeedDevice.count()):
            if (self.configpage['advanced'].ui.listWidget_hddSpeedDevice.item(i).checkState() == Qt.Checked):
                item.append(self.configpage['advanced'].ui.listWidget_hddSpeedDevice.item(i).text())
        settings.set('disk', str(item.join(QString('@@'))))
        item = QStringList()
        for i in range(self.configpage['advanced'].ui.listWidget_hddDevice.count()):
            if (self.configpage['advanced'].ui.listWidget_hddDevice.item(i).checkState() == Qt.Checked):
                item.append(self.configpage['advanced'].ui.listWidget_hddDevice.item(i).text())
        settings.set('hdd', str(item.join(QString('@@'))))
        settings.set('netdir', str(self.configpage['advanced'].ui.lineEdit_netdir.text()))
        settings.set('netdevBool', self.configpage['advanced'].ui.checkBox_netdev.checkState())
        settings.set('custom_netdev', str(self.configpage['advanced'].ui.comboBox_netdev.currentText()))
        settings.set('battery_device', str(self.configpage['advanced'].ui.lineEdit_batdev.text()))
        settings.set('ac_device', str(self.configpage['advanced'].ui.lineEdit_acdev.text()))
        settings.set('ac_online', str(self.configpage['advanced'].ui.lineEdit_acOnline.text()))
        settings.set('ac_offline', str(self.configpage['advanced'].ui.lineEdit_acOffline.text()))

        settings.set('tooltip_num', self.configpage['tooltip'].ui.spinBox_tooltipNum.value())
        if (self.configpage['tooltip'].checkBox_background.checkState() == 0):
            settings.set("tooltip_background", "null")
        else:
            settings.set("tooltip_background", str(self.configpage['tooltip'].kcolorcombos["background"].color().name()))
        for label in ['cpu', 'cpuclock', 'mem', 'swap', 'down', 'up']:
            settings.set(self.defaults['confColor'][label], str(self.configpage['tooltip'].kcolorcombos[label].color().name()))

        dataengineConfig = unicode(KGlobal.dirs().localkdedir()) + "/share/config/extsysmon.conf"
        try:
            with open(dataengineConfig, 'w') as deConfigFile:
                item = QStringList()
                for i in range(self.configpage['dataengine'].ui.listWidget_customCommand.count()):
                    item.append(self.configpage['dataengine'].ui.listWidget_customCommand.item(i).text())
                deConfigFile.write("CUSTOM=" + str(item.join(QString('@@'))) + "\n")
                deConfigFile.write("DESKTOPCMD=" + str(self.configpage['dataengine'].ui.lineEdit_desktopCmd.text()) + "\n")
                deConfigFile.write("GPUDEV=" + str(self.configpage['dataengine'].ui.comboBox_gpudev.currentText()) + "\n")
                deConfigFile.write("HDDDEV=" + str(self.configpage['dataengine'].ui.comboBox_hdddev.currentText()) + "\n")
                deConfigFile.write("HDDTEMPCMD=" + str(self.configpage['dataengine'].ui.lineEdit_hddtempCmd.text()) + "\n")
                deConfigFile.write("MPDADDRESS=" + str(self.configpage['dataengine'].ui.lineEdit_mpdaddress.text()) + "\n")
                deConfigFile.write("MPDPORT=" + str(self.configpage['dataengine'].ui.spinBox_mpdport.value()) + "\n")
                deConfigFile.write("MPRIS=" + str(self.configpage['dataengine'].ui.comboBox_mpris.currentText()) + "\n")
                item = QStringList()
                for i in range(self.configpage['dataengine'].ui.listWidget_pkgCommand.count()):
                    item.append(self.configpage['dataengine'].ui.listWidget_pkgCommand.item(i).text())
                pkgCmd = []
                pkgNull = []
                for command in item:
                    try:
                        pkgCmd.append(str(command.split(QString(":"))[0]))
                        pkgNull.append(str(command.split(QString(":"))[1]))
                    except:
                        pkgCmd.append(str(command))
                        pkgNull.append("0")
                deConfigFile.write("PKGCMD=" + ','.join(pkgCmd) + "\n")
                deConfigFile.write("PKGNULL=" + ','.join(pkgNull) + "\n")
                deConfigFile.write("PLAYER=" + str(self.configpage['dataengine'].ui.comboBox_playerSelect.currentText()) + "\n")
        except:
            pass

        # disconnecting from source and clear layout
        self.parent.disconnectFromSource()

        labelOrder = "--------------------"
        for label in self.defaults['order'].keys():
            if (self.configpage['widget'].checkboxes[self.defaults['order'][label]].checkState() > 0):
                pos = self.configpage['widget'].sliders[self.defaults['order'][label]].value() - 1
                labelOrder = labelOrder[:pos] + label + labelOrder[pos+1:]
            settings.set(self.defaults['confFormat'][self.defaults['order'][label]], str(self.configpage['widget'].lineedits[self.defaults['order'][label]].text()))
            settings.set(self.defaults['confBool'][self.defaults['order'][label]], self.configpage['widget'].checkboxes[self.defaults['order'][label]].checkState())
        labelOrder = ''.join(labelOrder.split('-'))
        settings.set('label_order', labelOrder)

        # reinitializate
        self.parent.reInit()


    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        if self.debug: qDebug("[PTM] [configdef.py] [createConfigurationInterface]")
        settings = config.Config(self.parent)

        font = QFont(str(settings.get('font_family', 'Terminus')), settings.get('font_size', 12).toInt()[0], 400, False)
        self.configpage['appearance'].ui.spinBox_interval.setValue(settings.get('interval', 2000).toInt()[0])
        self.configpage['appearance'].ui.fontComboBox.setCurrentFont(font)
        self.configpage['appearance'].ui.spinBox_fontSize.setValue(settings.get('font_size', 12).toInt()[0])
        self.configpage['appearance'].ui.kcolorcombo.setColor(QColor(str(settings.get('font_color', '#000000'))))
        font = str(settings.get('font_style', 'normal'))
        if (font == 'normal'):
            self.configpage['appearance'].ui.comboBox_style.setCurrentIndex(0)
        else:
            self.configpage['appearance'].ui.comboBox_style.setCurrentIndex(1)
        self.configpage['appearance'].ui.spinBox_weight.setValue(settings.get('font_weight', 400).toInt()[0])

        self.configpage['advanced'].ui.checkBox_background.setCheckState(settings.get('background', 2).toInt()[0])
        self.configpage['advanced'].ui.checkBox_layout.setCheckState(settings.get('layout', 0).toInt()[0])
        self.configpage['advanced'].ui.checkBox_popup.setCheckState(settings.get('popup', 2).toInt()[0])
        self.configpage['advanced'].ui.checkBox_leftStretch.setCheckState(settings.get('left_stretch', 2).toInt()[0])
        self.configpage['advanced'].ui.checkBox_rightStretch.setCheckState(settings.get('right_stretch', 2).toInt()[0])
        self.configpage['advanced'].ui.lineEdit_timeFormat.setText(str(settings.get('custom_time', '$hh:$mm')))
        self.configpage['advanced'].ui.lineEdit_uptimeFormat.setText(str(settings.get('custom_uptime', '$ds,$hs,$ms')))
        index = self.configpage['advanced'].ui.comboBox_tempUnits.findText(str(settings.get('temp_units', "Celsius")))
        self.configpage['advanced'].ui.comboBox_tempUnits.setCurrentIndex(index)
        self.configpage['advanced'].ui.listWidget_tempDevice.clear()
        commandOut = commands.getoutput("sensors")
        for item in commandOut.split("\n\n"):
            for device in item.split("\n"):
                if (device.find('\xc2\xb0C') > -1):
                    try:
                        listItem = QListWidgetItem('lmsensors/' + item.split("\n")[0] + '/' + '_'.join(device.split(":")[0].split()))
                        listItem.setCheckState(0)
                        self.configpage['advanced'].ui.listWidget_tempDevice.addItem(listItem)
                    except:
                        pass
        for item in str(settings.get('temp_device', '')).split('@@'):
            items = self.configpage['advanced'].ui.listWidget_tempDevice.findItems(item, Qt.MatchFixedString)
            for listItem in items:
                listItem.setCheckState(2)
        self.configpage['advanced'].ui.listWidget_mount.clear()
        commandOut = commands.getoutput("mount")
        for item in commandOut.split("\n"):
            try:
                listItem = QListWidgetItem(item.split(' on ')[1].split(' type ')[0])
                listItem.setCheckState(0)
                self.configpage['advanced'].ui.listWidget_mount.addItem(listItem)
            except:
                pass
        for item in str(settings.get('mount', '/')).split('@@'):
            items = self.configpage['advanced'].ui.listWidget_mount.findItems(item, Qt.MatchFixedString)
            for listItem in items:
                listItem.setCheckState(2)
        self.configpage['advanced'].ui.listWidget_hddSpeedDevice.clear()
        for item in self.defaults['disk']:
            listItem = QListWidgetItem(item)
            listItem.setCheckState(0)
            self.configpage['advanced'].ui.listWidget_hddSpeedDevice.addItem(listItem)
        for item in str(settings.get('disk', 'disk/sda_(8:0)')).split('@@'):
            items = self.configpage['advanced'].ui.listWidget_hddSpeedDevice.findItems(item, Qt.MatchFixedString)
            for listItem in items:
                listItem.setCheckState(2)
        self.configpage['advanced'].ui.listWidget_hddDevice.clear()
        commandOut = commands.getoutput("find /dev -name '[hms]d[a-z]'")
        for item in commandOut.split("\n"):
            try:
                listItem = QListWidgetItem(item)
                listItem.setCheckState(0)
                self.configpage['advanced'].ui.listWidget_hddDevice.addItem(listItem)
            except:
                pass
        for item in str(settings.get('hdd', '/dev/sda')).split('@@'):
            items = self.configpage['advanced'].ui.listWidget_hddDevice.findItems(item, Qt.MatchFixedString)
            for listItem in items:
                listItem.setCheckState(2)
        self.configpage['advanced'].ui.lineEdit_netdir.setText(str(settings.get('netdir', '/sys/class/net')))
        self.configpage['advanced'].ui.checkBox_netdev.setCheckState(settings.get('netdevBool', 0).toInt()[0])
        for item in QDir.entryList(QDir(str(settings.get('netdir', '/sys/class/net'))), QDir.Dirs | QDir.NoDotAndDotDot):
            self.configpage['advanced'].ui.comboBox_netdev.addItem(item)
        index = self.configpage['advanced'].ui.comboBox_netdev.findText(str(settings.get('custom_netdev', 'lo')))
        self.configpage['advanced'].ui.comboBox_netdev.setCurrentIndex(index)
        self.configpage['advanced'].ui.lineEdit_batdev.setText(str(settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity')))
        self.configpage['advanced'].ui.lineEdit_acdev.setText(str(settings.get('ac_device', '/sys/class/power_supply/AC/online')))
        self.configpage['advanced'].ui.lineEdit_acOnline.setText(str(settings.get('ac_online', '(*)')))
        self.configpage['advanced'].ui.lineEdit_acOffline.setText(str(settings.get('ac_offline', '( )')))

        self.configpage['tooltip'].ui.spinBox_tooltipNum.setValue(settings.get('tooltip_num', 100).toInt()[0])
        if (str(settings.get('tooltip_background', 'null')) == "null"):
            self.configpage['tooltip'].checkBox_background.setCheckState(0)
        else:
            self.configpage['tooltip'].checkBox_background.setCheckState(2)
            self.configpage['tooltip'].ui.kcolorcombo_cpu.setColor(QColor(str(settings.get('tooltip_background', 'null'))))
        self.configpage['tooltip'].ui.kcolorcombo_cpu.setColor(QColor(str(settings.get('cpu_color', '#ff0000'))))
        self.configpage['tooltip'].ui.kcolorcombo_cpuclock.setColor(QColor(str(settings.get('cpuclock_color', '#00ff00'))))
        self.configpage['tooltip'].ui.kcolorcombo_mem.setColor(QColor(str(settings.get('mem_color', '#0000ff'))))
        self.configpage['tooltip'].ui.kcolorcombo_swap.setColor(QColor(str(settings.get('swap_color', '#ffff00'))))
        self.configpage['tooltip'].ui.kcolorcombo_down.setColor(QColor(str(settings.get('down_color', '#00ffff'))))
        self.configpage['tooltip'].ui.kcolorcombo_up.setColor(QColor(str(settings.get('up_color', '#ff00ff'))))

        deSettings = {'CUSTOM':'wget -qO- http://ifconfig.me/ip', 'DESKTOPCMD':'qdbus org.kde.kwin /KWin currentDesktop',
            'GPUDEV':'auto', 'HDDDEV':'all', 'HDDTEMPCMD':'sudo hddtemp', 'MPDADDRESS':'localhost',
            'MPDPORT':'6600', 'MPRIS':'auto', 'PKGCMD':'pacman -Qu', 'PKGNULL':'0', 'PLAYER':'mpris'}
        dataengineConfig = unicode(KGlobal.dirs().localkdedir()) + "/share/config/extsysmon.conf"
        try:
            with open(dataengineConfig, 'r') as deConfigFile:
                for line in deConfigFile:
                    if ((line[0] != '#') and (line.split('=')[0] != line)):
                        deSettings[line.split('=')[0]] = line.split('=')[1][:-1]
        except:
            pass
        for item in deSettings['CUSTOM'].split('@@'):
            self.configpage['dataengine'].ui.listWidget_customCommand.addItem(item)
        index = self.configpage['dataengine'].ui.comboBox_gpudev.findText(deSettings['GPUDEV'])
        self.configpage['dataengine'].ui.lineEdit_desktopCmd.setText(deSettings['DESKTOPCMD'])
        self.configpage['dataengine'].ui.comboBox_gpudev.setCurrentIndex(index)
        self.configpage['dataengine'].ui.comboBox_hdddev.addItem("all")
        self.configpage['dataengine'].ui.comboBox_hdddev.addItem("disable")
        commandOut = commands.getoutput("find /dev -name '[hms]d[a-z]'")
        for item in commandOut.split("\n"):
            try:
                self.configpage['dataengine'].ui.comboBox_hdddev.addItem(item)
            except:
                pass
        index = self.configpage['dataengine'].ui.comboBox_hdddev.findText(deSettings['HDDDEV'])
        self.configpage['dataengine'].ui.comboBox_hdddev.setCurrentIndex(index)
        self.configpage['dataengine'].ui.lineEdit_hddtempCmd.setText(deSettings['HDDTEMPCMD'])
        self.configpage['dataengine'].ui.lineEdit_mpdaddress.setText(deSettings['MPDADDRESS'])
        self.configpage['dataengine'].ui.spinBox_mpdport.setValue(int(deSettings['MPDPORT']))
        self.configpage['dataengine'].ui.spinBox_mpdport.setValue(int(deSettings['MPDPORT']))
        self.configpage['dataengine'].ui.comboBox_mpris.addItem(deSettings['MPRIS'])
        self.configpage['dataengine'].ui.comboBox_mpris.setCurrentIndex(self.configpage['dataengine'].ui.comboBox_mpris.count()-1)
        self.configpage['dataengine'].ui.listWidget_pkgCommand.clear()
        for i in range(len(deSettings['PKGCMD'].split(','))):
            try:
                num = deSettings['PKGNULL'].split(',')[i]
            except:
                num = "0"
            self.configpage['dataengine'].ui.listWidget_pkgCommand.addItem(deSettings['PKGCMD'].split(',')[i] + ':' + num)
        index = self.configpage['dataengine'].ui.comboBox_playerSelect.findText(deSettings['PLAYER'])
        self.configpage['dataengine'].ui.comboBox_playerSelect.setCurrentIndex(index)

        labelOrder = str(settings.get('label_order', '1345'))
        for label in self.defaults['order'].keys():
            bool = settings.get(self.defaults['confBool'][self.defaults['order'][label]],
                self.defaults['bool'][self.defaults['order'][label]]).toInt()[0]
            self.configpage['widget'].checkboxes[self.defaults['order'][label]].setCheckState(bool)
            if (bool > 0):
                self.configpage['widget'].sliders[self.defaults['order'][label]].setValue(labelOrder.find(label) + 1)
            self.configpage['widget'].lineedits[self.defaults['order'][label]].setText(str(settings.get(self.defaults['confFormat'][self.defaults['order'][label]],
                self.defaults['format'][self.defaults['order'][label]])))

        # add config page
        page = {}
        page['widget'] = parent.addPage(self.configpage['widget'], i18n("Widget"))
        page['widget'].setIcon(KIcon("utilities-system-monitor"))
        page['advanced'] = parent.addPage(self.configpage['advanced'], i18n("Advanced"))
        page['advanced'].setIcon(KIcon("system-run"))
        page['tooltip'] = parent.addPage(self.configpage['tooltip'], i18n("Tooltip"))
        page['tooltip'].setIcon(KIcon("preferences-desktop-color"))
        page['appearance'] = parent.addPage(self.configpage['appearance'], i18n("Appearance"))
        page['appearance'].setIcon(KIcon("preferences-desktop-theme"))
        page['dataengine'] = parent.addPage(self.configpage['dataengine'], i18n("DataEngine"))
        page['dataengine'].setIcon(KIcon("utilities-system-monitor"))

        QObject.connect(parent, SIGNAL("okClicked()"), self.configAccepted)
