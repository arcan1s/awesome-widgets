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
    def __init__(self, parent, configpage):
        """class definition"""
        self.parent = parent
        self.configpage = configpage
    
    
    def configAccepted(self):
        """function to accept settings"""
        settings = config.Config(self.parent)
        
        # update local variables
        settings.set('interval', self.configpage.ui.spinBox_interval.value())
        settings.set('font_family', str(self.configpage.ui.fontComboBox.currentFont().family()))
        settings.set('font_size', self.configpage.ui.spinBox_fontSize.value())
        settings.set('font_color', str(self.configpage.ui.kcolorcombo.color().name()))
        settings.set('font_style', str(self.configpage.ui.comboBox_style.currentText()))
        settings.set('font_weight', self.configpage.ui.spinBox_weight.value())
        
        settings.set('custom_time', str(self.configpage.ui.lineEdit_timeFormat.text()))
        settings.set('custom_uptime', str(self.configpage.ui.lineEdit_uptimeFormat.text()))
        item = QStringList()
        for i in range(self.configpage.ui.listWidget_tempDevice.count()):
            item.append(self.configpage.ui.listWidget_tempDevice.item(i).text())
        settings.set('temp_device', str(item.join(QString('@@'))))
        item = QStringList()
        for i in range(self.configpage.ui.listWidget_mount.count()):
            item.append(self.configpage.ui.listWidget_mount.item(i).text())
        settings.set('mount', str(item.join(QString('@@'))))
        item = QStringList()
        for i in range(self.configpage.ui.listWidget_hddDevice.count()):
            item.append(self.configpage.ui.listWidget_hddDevice.item(i).text())
        settings.set('hdd', str(item.join(QString('@@'))))
        settings.set('netdir', str(self.configpage.ui.lineEdit_netdir.text()))
        settings.set('netdevBool', self.configpage.ui.checkBox_netdev.checkState())
        settings.set('custom_netdev', str(self.configpage.ui.comboBox_netdev.currentText()))
        settings.set('battery_device', str(self.configpage.ui.lineEdit_batdev.text()))
        settings.set('ac_device', str(self.configpage.ui.lineEdit_acdev.text()))
        settings.set('player_name', self.configpage.ui.comboBox_playerSelect.currentIndex())

        settings.set('tooltip_num', self.configpage.ui.spinBox_tooltipNum.value())
        for label in ['cpu', 'cpuclock', 'mem', 'swap', 'down', 'up']:
            exec('settings.set("' + label + '_color", str(self.configpage.kcolorcombo_' + label + '.color().name()))')
        
        dataengineConfig = unicode(KGlobal.dirs().localkdedir()) + "/share/config/extsysmon.conf"
        try:
            with open(dataengineConfig, 'w') as deConfigFile:
                deConfigFile.write("GPUDEV=" + str(self.configpage.ui.comboBox_gpudev.currentText()) + "\n")
                deConfigFile.write("HDDDEV=" + str(self.configpage.ui.comboBox_hdddev.currentText()) + "\n")
                deConfigFile.write("MPDADDRESS=" + str(self.configpage.ui.lineEdit_mpdaddress.text()) + "\n")
                deConfigFile.write("MPDPORT=" + str(self.configpage.ui.spinBox_mpdport.value()) + "\n")
                deConfigFile.write("CUSTOM=" + str(self.configpage.ui.lineEdit_customCommand.text()) + "\n")
        except:
            pass
        
        # disconnecting from source and clear layout
        self.parent.disconnectFromSource()
        if (self.parent.uptimeBool > 0):
            self.parent.systemmonitor.disconnectSource("system/uptime",  self.parent)
            self.parent.label_uptime.setText('')
            self.parent.layout.removeItem(self.parent.label_uptime)
        if (self.parent.cpuBool > 0):
            self.parent.systemmonitor.disconnectSource("cpu/system/TotalLoad", self.parent)
            for core in self.parent.cpuCore.keys():
                self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/TotalLoad", self.parent)
            self.parent.label_cpu.setText('')
            self.parent.layout.removeItem(self.parent.label_cpu)
        if (self.parent.cpuclockBool > 0):
            self.parent.systemmonitor.disconnectSource("cpu/system/AverageClock", self.parent)
            for core in self.parent.cpuClockCore.keys():
                self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/clock", self.parent)
            self.parent.label_cpuclock.setText('')
            self.parent.layout.removeItem(self.parent.label_cpuclock)
        if (self.parent.tempBool > 0):
            for item in self.parent.temp:
                self.parent.systemmonitor.disconnectSource(item, self.parent)
            self.parent.label_temp.setText('')
            self.parent.layout.removeItem(self.parent.label_temp)
        if (self.parent.gpuBool > 0):
            self.parent.extsysmon.disconnectSource("gpu", self.parent)
            self.parent.label_gpu.setText('')
            self.parent.layout.removeItem(self.parent.label_gpu)
        if (self.parent.gputempBool > 0):
            self.parent.extsysmon.disconnectSource("gputemp", self.parent)
            self.parent.label_gputemp.setText('')
            self.parent.layout.removeItem(self.parent.label_gputemp)
        if (self.parent.memBool > 0):
            self.parent.systemmonitor.disconnectSource("mem/physical/application", self.parent)
            self.parent.systemmonitor.disconnectSource("mem/physical/free", self.parent)
            self.parent.systemmonitor.disconnectSource("mem/physical/used", self.parent)
            self.parent.label_mem.setText('')
            self.parent.layout.removeItem(self.parent.label_mem)
        if (self.parent.swapBool > 0):
            self.parent.systemmonitor.disconnectSource("mem/swap/used", self.parent)
            self.parent.systemmonitor.disconnectSource("mem/swap/free", self.parent)
            self.parent.label_swap.setText('')
            self.parent.layout.removeItem(self.parent.label_swap)
        if (self.parent.hddBool > 0):
            for item in self.parent.mount:
                self.parent.systemmonitor.disconnectSource("partitions" + item + "/filllevel", self.parent)
            self.parent.label_hdd.setText('')
            self.parent.layout.removeItem(self.parent.label_hdd)
        if (self.parent.hddtempBool > 0):
            self.parent.extsysmon.disconnectSource("hddtemp", self.parent)
            self.parent.label_hddtemp.setText('')
            self.parent.layout.removeItem(self.parent.label_hddtemp)
        if (self.parent.netBool > 0):
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent)
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent)
            self.parent.label_net.setText('')
            self.parent.layout.removeItem(self.parent.label_net)
        if (self.parent.batBool > 0):
            self.parent.label_bat.setText('')
            self.parent.layout.removeItem(self.parent.label_bat)
        if (self.parent.playerBool > 0):
            self.parent.extsysmon.disconnectSource("player", self.parent)
            self.parent.label_player.setText('')
            self.parent.layout.removeItem(self.parent.label_player)
        if (self.parent.timeBool > 0):
            self.parent.timemon.disconnectSource("Local", self.parent)
            self.parent.label_time.setText('')
            self.parent.layout.removeItem(self.parent.label_time)
        if (self.parent.customBool > 0):
            self.parent.label_custom.setText('')
            self.parent.layout.removeItem(self.parent.label_custom)
        
        self.parent.label_order = "---------------"
        
        for label in self.parent.dict_orders.keys():
            exec ('self.parent.' + self.parent.dict_orders[label] + 'Bool = ' + str(self.configpage.checkboxes[self.parent.dict_orders[label]].checkState()))
            if (self.configpage.checkboxes[self.parent.dict_orders[label]].checkState() > 0):
                pos = self.configpage.sliders[self.parent.dict_orders[label]].value() - 1
                self.parent.label_order = self.parent.label_order[:pos] + label + self.parent.label_order[pos+1:]
            exec ('self.parent.' + self.parent.dict_orders[label] + 'Format = str(self.configpage.lineedits[self.parent.dict_orders[label]].text())')
            exec ('settings.set("' + self.parent.dict_orders[label] + 'Format", self.parent.' + self.parent.dict_orders[label] + 'Format)')
            exec ('settings.set("' + self.parent.dict_orders[label] + 'Bool", self.parent.' + self.parent.dict_orders[label] + 'Bool)')        
        self.parent.label_order = ''.join(self.parent.label_order.split('-'))
        settings.set('label_order', self.parent.label_order)
        
        # reinitializate
        self.parent.reInit()
    
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        settings = config.Config(self.parent)
        
        font = QFont(str(settings.get('font_family', 'Terminus')), settings.get('font_size', 12).toInt()[0], 400, False)
        self.configpage.ui.spinBox_interval.setValue(settings.get('interval', 2000).toInt()[0])
        self.configpage.ui.fontComboBox.setCurrentFont(font)
        self.configpage.ui.spinBox_fontSize.setValue(settings.get('font_size', 12).toInt()[0])
        self.configpage.ui.kcolorcombo.setColor(QColor(str(settings.get('font_color', '#000000'))))
        font = str(settings.get('font_style', 'normal'))
        if (font == 'normal'):
            self.configpage.ui.comboBox_style.setCurrentIndex(0)
        else:
            self.configpage.ui.comboBox_style.setCurrentIndex(1)
        self.configpage.ui.spinBox_weight.setValue(settings.get('font_weight', 400).toInt()[0])
        
        self.configpage.ui.lineEdit_timeFormat.setText(str(settings.get('custom_time', '$hh:$mm')))
        self.configpage.ui.lineEdit_uptimeFormat.setText(str(settings.get('custom_uptime', '$ds,$hs,$ms')))
        commandOut = commands.getoutput("sensors")
        for item in commandOut.split("\n\n"):
            for device in item.split("\n"):
                if (device.find('\xc2\xb0C') > -1):
                    try:
                        tempdev = 'lmsensors/' + item.split("\n")[0] + '/' + '_'.join(device.split(":")[0].split())
                        self.configpage.ui.comboBox_tempDevice.addItem(tempdev)
                    except:
                        pass
        self.configpage.ui.listWidget_tempDevice.clear()
        for item in str(settings.get('temp_device', '')).split('@@'):
            if (len(item) > 0):
                self.configpage.ui.listWidget_tempDevice.addItem(item)
        commandOut = commands.getoutput("mount")
        for item in commandOut.split("\n"):
            try:
                mount = item.split(' on ')[1].split(' type ')[0]
                self.configpage.ui.comboBox_mount.addItem(mount)
            except:
                pass
        self.configpage.ui.listWidget_mount.clear()
        for item in str(settings.get('mount', '/')).split('@@'):
            self.configpage.ui.listWidget_mount.addItem(item)
        commandOut = commands.getoutput("find /dev -name '[hs]d[a-z]'")
        for item in commandOut.split("\n"):
            try:
                self.configpage.ui.comboBox_hddDevice.addItem(item)
            except:
                pass
        self.configpage.ui.listWidget_hddDevice.clear()
        for item in str(settings.get('hdd', '/dev/sda')).split('@@'):
            self.configpage.ui.listWidget_hddDevice.addItem(item)
        self.configpage.ui.lineEdit_netdir.setText(str(settings.get('netdir', '/sys/class/net')))
        self.configpage.ui.checkBox_netdev.setCheckState(settings.get('netdevBool', 0).toInt()[0])
        for item in QDir.entryList(QDir(str(settings.get('netdir', '/sys/class/net'))), QDir.Dirs | QDir.NoDotAndDotDot):
            self.configpage.ui.comboBox_netdev.addItem(item)
        index = self.configpage.ui.comboBox_netdev.findText(str(settings.get('custom_netdev', 'lo')))
        self.configpage.ui.comboBox_netdev.setCurrentIndex(index)
        self.configpage.ui.lineEdit_batdev.setText(str(settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity')))
        self.configpage.ui.lineEdit_acdev.setText(str(settings.get('ac_device', '/sys/class/power_supply/AC/online')))
        self.configpage.ui.comboBox_playerSelect.setCurrentIndex(settings.get('player_name', 0).toInt()[0])
        
        self.configpage.ui.spinBox_tooltipNum.setValue(settings.get('tooltip_num', 100).toInt()[0])
        self.configpage.ui.kcolorcombo_cpu.setColor(QColor(str(settings.get('cpu_color', '#ff0000'))))
        self.configpage.ui.kcolorcombo_cpuclock.setColor(QColor(str(settings.get('cpuclock_color', '#00ff00'))))
        self.configpage.ui.kcolorcombo_mem.setColor(QColor(str(settings.get('mem_color', '#0000ff'))))
        self.configpage.ui.kcolorcombo_swap.setColor(QColor(str(settings.get('swap_color', '#ffff00'))))
        self.configpage.ui.kcolorcombo_down.setColor(QColor(str(settings.get('down_color', '#00ffff'))))
        self.configpage.ui.kcolorcombo_up.setColor(QColor(str(settings.get('up_color', '#ff00ff'))))
        
        deSettings = {'GPUDEV':'auto', 'HDDDEV':'all', 'MPDADDRESS':'localhost', 
            'MPDPORT':'6600', 'CUSTOM':'wget -qO- http://ifconfig.me/ip'}
        dataengineConfig = unicode(KGlobal.dirs().localkdedir()) + "/share/config/extsysmon.conf"
        try:
            with open(dataengineConfig, 'r') as deConfigFile:
                for line in deConfigFile:
                    if ((line[0] != '#') and (line.split('=')[0] != line)):
                        deSettings[line.split('=')[0]] = line.split('=')[1][:-1]
        except:
            pass
        index = self.configpage.ui.comboBox_gpudev.findText(deSettings['GPUDEV'])
        self.configpage.ui.comboBox_gpudev.setCurrentIndex(index)
        self.configpage.ui.comboBox_hdddev.addItem("all")
        commandOut = commands.getoutput("find /dev -name '[hs]d[a-z]'")
        for item in commandOut.split("\n"):
            try:
                self.configpage.ui.comboBox_hdddev.addItem(item)
            except:
                pass
        index = self.configpage.ui.comboBox_hdddev.findText(deSettings['HDDDEV'])
        self.configpage.ui.comboBox_hdddev.setCurrentIndex(index)
        self.configpage.ui.lineEdit_mpdaddress.setText(deSettings['MPDADDRESS'])
        self.configpage.ui.spinBox_mpdport.setValue(int(deSettings['MPDPORT']))
        self.configpage.ui.spinBox_mpdport.setValue(int(deSettings['MPDPORT']))
        self.configpage.ui.lineEdit_customCommand.setText(deSettings['CUSTOM'])
        
        for label in self.parent.dict_orders.keys():
            exec ('bool = self.parent.' + self.parent.dict_orders[label] + 'Bool')
            self.configpage.checkboxes[self.parent.dict_orders[label]].setCheckState(bool)
            if (bool > 0):
                self.configpage.sliders[self.parent.dict_orders[label]].setValue(self.parent.label_order.find(label)+1)
            self.configpage.lineedits[self.parent.dict_orders[label]].setText(str(settings.get(self.parent.dict_orders[label] + 'Format', self.parent.dict_defFormat[self.parent.dict_orders[label]])))
        
        # add config page
        page = parent.addPage(self.configpage, i18n(self.parent.name()))
        page.setIcon(KIcon(self.parent.icon()))
        
        parent.okClicked.connect(self.configAccepted)
