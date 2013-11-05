# -*- coding: utf-8 -*-

# Copyright 2013 Evgeniy Alekseev <esalexeev@gmail.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
        self.parent.interval = self.configpage.ui.spinBox_interval.value()
        settings.set('interval', self.parent.interval)
        self.parent.font_family = str(self.configpage.ui.fontComboBox.currentFont().family())
        settings.set('font_family', self.parent.font_family)
        self.parent.font_size = self.configpage.ui.spinBox_fontSize.value()
        settings.set('font_size', self.parent.font_size)
        self.parent.font_color = str(self.configpage.ui.kcolorcombo.color().name())
        settings.set('font_color', self.parent.font_color)
        if (self.configpage.ui.comboBox_style.currentIndex() == 0):
            self.parent.font_style = 'normal'
        else:
            self.parent.font_style = 'italic'
        settings.set('font_style', self.parent.font_style)
        self.parent.font_weight = self.configpage.ui.spinBox_weight.value()
        settings.set('font_weight', self.parent.font_weight)
        
        # disconnecting from source and clear layout
        if (self.parent.uptimeBool > 0):
            self.parent.systemmonitor.disconnectSource("system/uptime",  self.parent)
            self.parent.label_uptime.setText('')
            self.parent.layout.removeItem(self.parent.label_uptime)
        if (self.parent.cpuBool > 0):
            self.parent.systemmonitor.disconnectSource("cpu/system/TotalLoad", self.parent)
            if (self.parent.cpuFormat.split('$ccpu')[0] != self.parent.cpuFormat):
                self.parent.label_cpu.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu)
                self.parent.label_cpu1.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu1)
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/TotalLoad", self.parent)
                    exec ("self.parent.label_coreCpu" + str(core) + ".setText('')")
                    exec ("self.parent.layout.removeItem(self.parent.label_coreCpu" + str(core) + ")")
            else:
                self.parent.label_cpu.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu)
        if (self.parent.cpuclockBool > 0):
            self.parent.systemmonitor.disconnectSource("cpu/system/AverageClock", self.parent)
            if (self.parent.cpuclockFormat.split('$ccpu')[0] != self.parent.cpuclockFormat):
                self.parent.label_cpuclock.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock)
                self.parent.label_cpuclock1.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock1)
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/clock", self.parent)
                    exec ("self.parent.label_coreCpuclock" + str(core) + ".setText('')")
                    exec ("self.parent.layout.removeItem(self.parent.label_coreCpuclock" + str(core) + ")")
            else:
                self.parent.label_cpuclock.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock)
        if (self.parent.tempBool > 0):
            self.parent.systemmonitor.disconnectSource(self.parent.tempdev, self.parent)
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
            if (self.parent.memInMb == False):
                self.parent.systemmonitor.disconnectSource("mem/physical/free", self.parent)
                self.parent.systemmonitor.disconnectSource("mem/physical/used", self.parent)
            self.parent.label_mem.setText('')
            self.parent.layout.removeItem(self.parent.label_mem)
        if (self.parent.swapBool > 0):
            self.parent.systemmonitor.disconnectSource("mem/swap/used", self.parent)
            if (self.parent.swapInMb == False):
                self.parent.systemmonitor.disconnectSource("mem/swap/free", self.parent)
            self.parent.label_swap.setText('')
            self.parent.layout.removeItem(self.parent.label_swap)
        if (self.parent.hddBool > 0):
            for mount in self.parent.mountPoints:
                self.parent.systemmonitor.disconnectSource("partitions" + mount + "/filllevel", self.parent)
                exec ('self.parent.label_hdd_' + ''.join(mount.split('/')) + '.setText("")')
                exec ("self.parent.layout.removeItem(self.parent.label_hdd_" + ''.join(mount.split('/')) + ")")
            self.parent.label_hdd0.setText('')
            self.parent.label_hdd1.setText('')
            self.parent.layout.removeItem(self.parent.label_hdd0)
            self.parent.layout.removeItem(self.parent.label_hdd1)
        if (self.parent.hddtempBool > 0):
            self.parent.extsysmon.disconnectSource("hddtemp", self.parent)
            self.parent.label_hddtemp.setText('')
            self.parent.layout.removeItem(self.parent.label_hddtemp)
        if (self.parent.netBool > 0):
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent)
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent)
            self.parent.label_netDown.setText('')
            self.parent.label_netUp.setText('')
            self.parent.layout.removeItem(self.parent.label_netUp)
            self.parent.layout.removeItem(self.parent.label_netDown)
        if (self.parent.batBool > 0):
            self.parent.label_bat.setText('')
            self.parent.layout.removeItem(self.parent.label_bat)
        if (self.parent.playerBool > 0):
            self.parent.label_player.setText('')
            self.parent.layout.removeItem(self.parent.label_player)
        
        self.parent.label_order = "-------------"
        
        for label in self.parent.dict_orders.keys():
            exec ('self.parent.' + self.parent.dict_orders[label] + 'Bool = ' + str(self.configpage.checkboxes[self.parent.dict_orders[label]].checkState()))
            if (self.configpage.checkboxes[self.parent.dict_orders[label]].checkState() > 0):
                pos = self.configpage.sliders[self.parent.dict_orders[label]].value() - 1
                self.parent.label_order = self.parent.label_order[:pos] + label + self.parent.label_order[pos+1:]
            if (self.parent.dict_orders[label] == 'net'):
                exec ('self.parent.' + self.parent.dict_orders[label] + 'NonFormat = str(self.configpage.lineedits[self.parent.dict_orders[label]].text())')
                exec ('settings.set("' + self.parent.dict_orders[label] + 'NonFormat", self.parent.' + self.parent.dict_orders[label] + 'NonFormat)')
            else:
                exec ('self.parent.' + self.parent.dict_orders[label] + 'Format = str(self.configpage.lineedits[self.parent.dict_orders[label]].text())')
                exec ('settings.set("' + self.parent.dict_orders[label] + 'Format", self.parent.' + self.parent.dict_orders[label] + 'Format)')
            exec ('settings.set("' + self.parent.dict_orders[label] + 'Bool", self.parent.' + self.parent.dict_orders[label] + 'Bool)')
            if (self.parent.dict_orders[label] == 'bat'):
                self.parent.battery_device = str(self.configpage.ui.lineEdit_batdev.text())
                self.parent.ac_device = str(self.configpage.ui.lineEdit_acdev.text())
                settings.set('battery_device', self.parent.battery_device)
                settings.set('ac_device', self.parent.ac_device)
            elif (self.parent.dict_orders[label] == 'temp'):
                self.parent.tempdev = str(self.configpage.ui.comboBox_temp.currentText())
                settings.set('temp_device', self.parent.tempdev)
            elif (self.parent.dict_orders[label] == 'player'):
                self.parent.player_name = self.configpage.ui.comboBox_player.currentIndex()
                settings.set('player_name', self.parent.player_name)
        
        self.parent.label_order = ''.join(self.parent.label_order.split('-'))
        settings.set('label_order', self.parent.label_order)
        
        # reinitializate
        self.parent.reinit.reinit(confAccept=True)
    
    
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
        for label in self.parent.dict_orders.keys():
            exec ('bool = self.parent.' + self.parent.dict_orders[label] + 'Bool')
            self.configpage.checkboxes[self.parent.dict_orders[label]].setCheckState(bool)
            if (bool > 0):
                self.configpage.sliders[self.parent.dict_orders[label]].setValue(self.parent.label_order.find(label)+1)
            if (self.parent.dict_orders[label] == 'net'):
                self.configpage.lineedits[self.parent.dict_orders[label]].setText(str(settings.get(self.parent.dict_orders[label] + 'NonFormat', self.parent.dict_defFormat[self.parent.dict_orders[label]])))
            else:
                self.configpage.lineedits[self.parent.dict_orders[label]].setText(str(settings.get(self.parent.dict_orders[label] + 'Format', self.parent.dict_defFormat[self.parent.dict_orders[label]])))
            if (self.parent.dict_orders[label] == 'bat'):
                self.configpage.ui.lineEdit_batdev.setText(str(settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity')))
                self.configpage.ui.lineEdit_acdev.setText(str(settings.get('ac_device', '/sys/class/power_supply/AC/online')))
            elif (self.parent.dict_orders[label] == 'temp'):
                self.configpage.ui.comboBox_temp.addItem(str(settings.get('temp_device', '<select device>')))
                commandOut = commands.getoutput("sensors")
                for adapter in commandOut.split("\n\n"):
                    for device in adapter.split("\n"):
                        if (device.find('\xc2\xb0C') > -1):
                            try:
                                tempdev = 'lmsensors/' + adapter.split('\n')[0] + '/' + '_'.join(device.split(":")[0].split())
                                self.configpage.ui.comboBox_temp.addItem(tempdev)
                            except:
                                pass
            elif (self.parent.dict_orders[label] == 'player'):
                self.configpage.ui.comboBox_player.setCurrentIndex(int(settings.get('player_name', 0)))
        
        # add config page
        page = parent.addPage(self.configpage, i18n(self.parent.name()))
        page.setIcon(KIcon(self.parent.icon()))
        
        parent.okClicked.connect(self.configAccepted)
