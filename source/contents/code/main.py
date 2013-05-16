# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.kdecore import *
from PyKDE4.kdeui import *
from PyKDE4.kio import *
from PyKDE4.plasma import Plasma
from PyKDE4 import plasmascript
from PyQt4 import QtCore
from configwindow import *
from config import *
from util import *
import commands, os, time



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self,parent)


    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.layout = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        self.setupVar()
        self.reinit()
        self.setHasConfigurationInterface(True)


    def configAccepted(self):
        """function to accept settings"""
        # update local variables
        self.interval = int(self.configpage.ui.spinBox_interval.value())
        self.settings.set('interval', self.interval)
        self.font_family = str(self.configpage.ui.fontComboBox.currentFont().family())
        self.settings.set('font_family', self.font_family)
        self.font_size = int(self.configpage.ui.spinBox_fontSize.value())
        self.settings.set('font_size', self.font_size)
        self.font_color = str(self.configpage.ui.kcolorcombo.color().name())
        self.settings.set('font_color', self.font_color)
        if (self.configpage.ui.comboBox_style.currentIndex() == 0):
            self.font_style = 'normal'
        else:
            self.font_style = 'italic'
        self.settings.set('font_style', self.font_style)
        self.font_weight = int(self.configpage.ui.spinBox_weight.value())
        self.settings.set('font_weight', self.font_weight)
        
        # disconnecting from source and clear layout
        if (self.uptimeBool == 1):
            self.systemmonitor.disconnectSource("system/uptime",  self)
            self.label_uptime.setText('')
            self.layout.removeItem(self.label_uptime)
        if (self.cpuBool == 1):
            self.systemmonitor.disconnectSource("cpu/system/TotalLoad", self)
            if (self.cpuFormat.split('$ccpu')[0] != self.cpuFormat):
                self.label_cpu0.setText('')
                self.layout.removeItem(self.label_cpu0)
                self.label_cpu1.setText('')
                self.layout.removeItem(self.label_cpu1)
                for core in range(self.numCores):
                    self.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/TotalLoad", self)
                    exec ("self.label_coreCpu" + str(core) + ".setText('')")
                    exec ("self.layout.removeItem(self.label_coreCpu" + str(core) + ")")
            else:
                self.label_cpu.setText('')
                self.layout.removeItem(self.label_cpu)
        if (self.cpuclockBool == 1):
            self.systemmonitor.disconnectSource("cpu/system/AverageClock", self)
            if (self.cpuclockFormat.split('$ccpu')[0] != self.cpuclockFormat):
                self.label_cpuclock0.setText('')
                self.layout.removeItem(self.label_cpuclock0)
                self.label_cpuclock1.setText('')
                self.layout.removeItem(self.label_cpuclock1)
                for core in range(self.numCores):
                    self.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/clock", self)
                    exec ("self.label_coreCpuclock" + str(core) + ".setText('')")
                    exec ("self.layout.removeItem(self.label_coreCpuclock" + str(core) + ")")
            else:
                self.label_cpuclock.setText('')
                self.layout.removeItem(self.label_cpuclock)
        if (self.tempBool == 1):
            self.systemmonitor.disconnectSource(self.tempdev, self)
            self.label_temp.setText('')
            self.layout.removeItem(self.label_temp)
        if (self.gpuBool == 1):
            self.label_gpu.setText('')
            self.layout.removeItem(self.label_gpu)
        if (self.gputempBool == 1):
            self.label_gputemp.setText('')
            self.layout.removeItem(self.label_gputemp)
        if (self.memBool == 1):
            self.systemmonitor.disconnectSource("mem/physical/application", self)
            if (self.memInMb == False):
                self.systemmonitor.disconnectSource("mem/physical/free", self)
                self.systemmonitor.disconnectSource("mem/physical/used", self)
            self.label_mem.setText('')
            self.layout.removeItem(self.label_mem)
        if (self.swapBool == 1):
            self.systemmonitor.disconnectSource("mem/swap/used", self)
            if (self.swapInMb == False):
                self.systemmonitor.disconnectSource("mem/swap/free", self)
            self.label_swap.setText('')
            self.layout.removeItem(self.label_swap)
        if (self.hddBool == 1):
            for mount in self.mountPoints:
                self.systemmonitor.disconnectSource("partitions" + mount + "/filllevel", self)
                exec ('self.label_hdd_' + ''.join(mount.split('/')) + '.setText("")')
                exec ("self.layout.removeItem(self.label_hdd_" + ''.join(mount.split('/')) + ")")
            self.label_hdd0.setText('')
            self.label_hdd1.setText('')
            self.layout.removeItem(self.label_hdd0)
            self.layout.removeItem(self.label_hdd1)
        if (self.hddtempBool == 1):
            self.label_hddtemp.setText('')
            self.layout.removeItem(self.label_hddtemp)
        if (self.netBool == 1):
            self.systemmonitor.disconnectSource("network/interfaces/"+self.netdev+"/transmitter/data", self)
            self.systemmonitor.disconnectSource("network/interfaces/"+self.netdev+"/receiver/data", self)
            self.label_netDown.setText('')
            self.label_netUp.setText('')
            self.layout.removeItem(self.label_netUp)
            self.layout.removeItem(self.label_netDown)
        if (self.batBool == 1):
            self.label_bat.setText('')
            self.layout.removeItem(self.label_bat)
        
        self.label_order = "------------"
        
        for label in self.dict_orders.keys():
            if (self.configpage.checkboxes[self.dict_orders[label]].checkState() == 2):
                exec ('self.' + self.dict_orders[label] + 'Bool = 1')
                pos = self.configpage.sliders[self.dict_orders[label]].value() - 1
                self.label_order = self.label_order[:pos] + label + self.label_order[pos+1:]
            else:
                exec ('self.' + self.dict_orders[label] + 'Bool = 0')
            if (self.dict_orders[label] == 'net'):
                exec ('self.' + self.dict_orders[label] + 'NonFormat = str(self.configpage.lineedits[self.dict_orders[label]].text())')
                exec ('self.settings.set("' + self.dict_orders[label] + 'NonFormat", self.' + self.dict_orders[label] + 'NonFormat)')
            else:
                exec ('self.' + self.dict_orders[label] + 'Format = str(self.configpage.lineedits[self.dict_orders[label]].text())')
                exec ('self.settings.set("' + self.dict_orders[label] + 'Format", self.' + self.dict_orders[label] + 'Format)')
            exec ('self.settings.set("' + self.dict_orders[label] + 'Bool", self.' + self.dict_orders[label] + 'Bool)')
            if (self.dict_orders[label] == 'net'):
                self.num_dev = int(self.configpage.ui.comboBox_numNet.currentIndex())
                self.settings.set('num_dev', self.num_dev)
            elif (self.dict_orders[label] == 'bat'):
                self.battery_device = str(self.configpage.ui.lineEdit_batdev.text())
                self.ac_device = str(self.configpage.ui.lineEdit_acdev.text())
                self.settings.set('battery_device', self.battery_device)
                self.settings.set('ac_device', self.ac_device)
        
        self.label_order = ''.join(self.label_order.split('-'))
        self.settings.set('label_order', self.label_order)
        
        # reinitializate
        self.reinit()


    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = ConfigWindow(self, self.settings)
        
        font = QFont(str(self.settings.get('font_family', 'Terminus')), int(self.settings.get('font_size', 12)), 50)
        self.configpage.ui.spinBox_interval.setValue(int(self.settings.get('interval', 2000)))
        self.configpage.ui.fontComboBox.setCurrentFont(font)
        self.configpage.ui.spinBox_fontSize.setValue(int(self.settings.get('font_size', 12)))
        self.configpage.ui.kcolorcombo.setColor(QColor(str(self.settings.get('font_color', '#000000'))))
        font = str(self.settings.get('font_style', 'normal'))
        if (font == 'normal'):
            self.configpage.ui.comboBox_style.setCurrentIndex(0)
        else:
            self.configpage.ui.comboBox_style.setCurrentIndex(1)
        self.configpage.ui.spinBox_weight.setValue(int(self.settings.get('font_weight', 400)))
        for label in self.dict_orders.keys():
            exec ('bool = self.' + self.dict_orders[label] + 'Bool')
            if (bool == 1):
                self.configpage.sliders[self.dict_orders[label]].setMaximum(len(self.label_order))
                self.configpage.sliders[self.dict_orders[label]].setValue(self.label_order.find(label)+1)
                self.configpage.sliders[self.dict_orders[label]].setEnabled(True)
                self.configpage.lineedits[self.dict_orders[label]].setEnabled(True)
                self.configpage.checkboxes[self.dict_orders[label]].setCheckState(2)
                if (self.dict_orders[label] == 'net'):
                    self.configpage.ui.comboBox_numNet.setEnabled(True)
                elif (self.dict_orders[label] == 'bat'):
                    self.configpage.ui.lineEdit_acdev.setEnabled(True)
                    self.configpage.ui.lineEdit_batdev.setEnabled(True)
            else:
                self.configpage.sliders[self.dict_orders[label]].setMaximum(len(self.label_order))
                self.configpage.sliders[self.dict_orders[label]].setValue(1)
                self.configpage.sliders[self.dict_orders[label]].setDisabled(True)
                self.configpage.lineedits[self.dict_orders[label]].setDisabled(True)
                self.configpage.checkboxes[self.dict_orders[label]].setCheckState(0)
                if (self.dict_orders[label] == 'net'):
                    self.configpage.ui.comboBox_numNet.setDisabled(True)
                elif (self.dict_orders[label] == 'bat'):
                    self.configpage.ui.lineEdit_acdev.setDisabled(True)
                    self.configpage.ui.lineEdit_batdev.setDisabled(True)
            if (self.dict_orders[label] == 'net'):
                self.configpage.ui.comboBox_numNet.setCurrentIndex(int(self.settings.get('num_dev', 0)))
                self.configpage.lineedits[self.dict_orders[label]].setText(str(self.settings.get(self.dict_orders[label] + 'NonFormat', self.dict_defFormat[self.dict_orders[label]])))
            else:
                self.configpage.lineedits[self.dict_orders[label]].setText(str(self.settings.get(self.dict_orders[label] + 'Format', self.dict_defFormat[self.dict_orders[label]])))
            if (self.dict_orders[label] == 'bat'):
                self.configpage.ui.lineEdit_batdev.setText(str(self.settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity')))
                self.configpage.ui.lineEdit_acdev.setText(str(self.settings.get('ac_device', '/sys/class/power_supply/AC/online')))
        
        # add config page
        page = parent.addPage(self.configpage, i18n(self.name()))
        page.setIcon(KIcon(self.icon()))
        
        parent.okClicked.connect(self.configAccepted)


    def reinit(self):
        """function to reinitializate widget"""
        self.settings = Config(self)
        self.interval = int(self.settings.get('interval', 2000))
        self.font_family = str(self.settings.get('font_family', 'Terminus'))
        self.font_size = int(self.settings.get('font_size', 12))
        self.font_color = str(self.settings.get('font_color', '#000000'))
        self.font_style = str(self.settings.get('font_style', 'normal'))
        self.font_weight = int(self.settings.get('font_weight', 400))
        self.formatLine = "<pre><p align=\"center\"><span style=\" font-family:'" + self.font_family + "'; font-style:" + self.font_style
        self.formatLine = self.formatLine + "; font-size:" + str(self.font_size) + "pt; font-weight:" + str(self.font_weight)
        self.formatLine = self.formatLine + "; color:" + self.font_color + ";\">$LINE</span></p></pre>"
        self.label_order = str(self.settings.get('label_order', '1345'))
        for label in self.dict_orders.values():
            if ((label == 'cpu') or (label == 'mem') or (label == 'swap') or (label == 'net')):
                exec ('self.' + label + 'Bool = int(self.settings.get("' + label + 'Bool",  1))')
            else:
                exec ('self.' + label + 'Bool = int(self.settings.get("' + label + 'Bool",  0))')
        # small function for update if errors exist
        summ = 0
        for label in self.dict_orders.values():
            exec ('summ += self.' + label + 'Bool')
        if (len(self.label_order) != summ):
            for label in self.dict_orders.values():
                if ((label == 'cpu') or (label == 'mem') or (label == 'swap') or (label == 'net')):
                    exec ('self.' + label + 'Bool = 1')
                else:
                    exec ('self.' + label + 'Bool = 0')
                exec ('self.settings.set("' + label + 'Bool", self.' + label + 'Bool)')
            self.label_order = '1345'
            self.settings.set('label_order', self.label_order)
        
        for order in self.label_order:
            if (order == "1"):
                if (self.cpuBool == 1):
                    self.cpuFormat = str(self.settings.get('cpuFormat', '[cpu: $cpu%]'))
                    if (self.cpuFormat.split('$ccpu')[0] != self.cpuFormat):
                        self.label_cpu0 = Plasma.Label(self.applet)
                        self.label_cpu1 = Plasma.Label(self.applet)
                        if (self.cpuFormat.split('$ccpu')[0].split('$cpu')[0] != self.cpuFormat.split('$ccpu')[0]):
                            line = self.cpuFormat.split('$ccpu')[0].split('$cpu')[0] + '-----' + self.cpuFormat.split('$ccpu')[0].split('$cpu')[1]
                        else:
                            line = self.cpuFormat.split('$ccpu')[0]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpu0.setText(text)
                        self.layout.addItem(self.label_cpu0)
                        text = self.formatLine.split('$LINE')[0] + "-----" + self.formatLine.split('$LINE')[1]
                        for core in range(self.numCores):
                            #text = self.formatLine.split('$LINE')[0] + "Core" + str(core) + "\n-----" + self.formatLine.split('$LINE')[1]
                            exec ('self.label_coreCpu' + str(core) + ' = Plasma.Label(self.applet)')
                            exec ('self.label_coreCpu' + str(core) + '.setText(text)')
                            exec ('self.layout.addItem(self.label_coreCpu' + str(core) + ')')
                        if (self.cpuFormat.split('$ccpu')[1].split('$cpu')[0] != self.cpuFormat.split('$ccpu')[1]):
                            line = self.cpuFormat.split('$ccpu')[1].split('$cpu')[0] + '-----' + self.cpuFormat.split('$ccpu')[1].split('$cpu')[1]
                        else:
                            line = self.cpuFormat.split('$ccpu')[1]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpu1.setText(text)
                        self.layout.addItem(self.label_cpu1)
                    else:
                        self.label_cpu = Plasma.Label(self.applet)
                        if (self.cpuFormat.split('$cpu')[0] != self.cpuFormat):
                            line = self.cpuFormat.split('$cpu')[0] + '-----' + self.cpuFormat.split('$cpu')[1]
                        else:
                            line = self.cpuFormat
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpu.setText(text)
                        self.layout.addItem(self.label_cpu)
            elif (order == "2"):
                if (self.tempBool == 1):
                    self.tempFormat = str(self.settings.get('tempFormat', '[temp: $temp&deg;C]'))
                    self.label_temp = Plasma.Label(self.applet)
                    if (self.tempFormat.split('$temp')[0] != self.tempFormat):
                        line = self.tempFormat.split('$temp')[0] + '----' + self.tempFormat.split('$temp')[1]
                    else:
                        line = self.tempFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_temp.setText(text)
                    self.layout.addItem(self.label_temp)
            elif (order == "3"):
                if (self.memBool == 1):
                    self.memFormat = str(self.settings.get('memFormat', '[mem: $mem%]'))
                    if (self.memFormat.split('$memmb')[0] != self.memFormat):
                        self.memInMb = True
                        text = self.formatLine.split('$LINE')[0] + self.memFormat.split('$memmb')[0] + '-----' + self.memFormat.split('$memmb')[1] + self.formatLine.split('$LINE')[1]
                    elif (self.memFormat.split('$mem')[0] != self.memFormat):
                        self.memInMb = False
                        self.mem_used = 0.0
                        self.mem_free = 1.0
                        self.mem_uf = 0.0
                        line = self.memFormat.split('$mem')[0] + '-----' + self.memFormat.split('$mem')[1]
                    else:
                        line = self.memFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_mem = Plasma.Label(self.applet)
                    self.label_mem.setText(text)
                    self.layout.addItem(self.label_mem)
            elif (order == "4"):
                if (self.swapBool == 1):
                    self.swapFormat = str(self.settings.get('swapFormat', '[swap: $swap%]'))
                    if (self.swapFormat.split('$swapmb')[0] != self.swapFormat):
                        self.swapInMb = True
                        text = self.formatLine.split('$LINE')[0] + self.swapFormat.split('$swapmb')[0] + '-----' + self.swapFormat.split('$swapmb')[1] + self.formatLine.split('$LINE')[1]
                    elif (self.swapFormat.split('$swap')[0] != self.swapFormat):
                        self.swapInMb = False
                        self.swap_free = 1.0
                        self.swap_used = 0.0
                        line = self.swapFormat.split('$swap')[0] + '-----' + self.swapFormat.split('$swap')[1]
                    else:
                        line = self.swapFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_swap = Plasma.Label(self.applet)
                    self.label_swap.setText(text)
                    self.layout.addItem(self.label_swap)
            elif (order == "5"):
                if (self.netBool == 1):
                    self.netNonFormat = str(self.settings.get('netNonFormat', '[net: $netKB/s]'))
                    if (self.netNonFormat.split('@@')[0] != self.netNonFormat):
                        self.netdev = self.netNonFormat.split('@@')[1]
                        self.netNonFormat = self.netNonFormat.split('@@')[0] + self.netNonFormat.split('@@')[2]
                    else:
                        self.num_dev = int(self.settings.get('num_dev', 0))
                        self.setupNetdev()
                    if (self.netNonFormat.split('$netdev')[0] != self.netNonFormat):
                        self.netFormat = self.netNonFormat.split('$netdev')[0] + self.netdev + self.netNonFormat.split('$netdev')[1]
                    else:
                        self.netFormat = self.netNonFormat
                    self.label_netDown = Plasma.Label(self.applet)
                    if (self.netFormat.split('$net')[0] != self.netFormat):
                        line = self.netFormat.split('$net')[0] + '----'
                    else:
                        line = self.netFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_netDown.setText(text)
                    self.layout.addItem(self.label_netDown)
                    self.label_netUp = Plasma.Label(self.applet)
                    if (self.netFormat.split('$net')[0] != self.netFormat):
                        line = '/----' + self.netFormat.split('$net')[1]
                    else:
                        line = ''
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_netUp.setText(text)
                    self.layout.addItem(self.label_netUp)
            elif (order == "6"):
                if (self.batBool == 1):
                    self.batFormat = str(self.settings.get('batFormat', '[bat: $bat%$ac]'))
                    self.battery_device= str(self.settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity'))
                    self.ac_device = str(self.settings.get('ac_device', '/sys/class/power_supply/AC/online'))
                    self.label_bat = Plasma.Label(self.applet)
                    line = self.batFormat
                    if (line.split('$ac')[0] != line):
                        line = line.split('$ac')[0] + '(-)' + line.split('$ac')[1]
                    if (line.split('$bat')[0] != line):
                        line = line.split('$bat')[0] + '---' + line.split('$bat')[1]
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_bat.setText(text)
                    self.layout.addItem(self.label_bat)
            elif (order == "7"):
                if (self.cpuclockBool == 1):
                    self.cpuclockFormat = str(self.settings.get('cpuclockFormat', '[mhz: $cpucl]'))
                    if (self.cpuclockFormat.split('$ccpucl')[0] != self.cpuclockFormat):
                        self.label_cpuclock0 = Plasma.Label(self.applet)
                        self.label_cpuclock1 = Plasma.Label(self.applet)
                        if (self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] != self.cpuclockFormat.split('$ccpucl')[0]):
                            line = self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] + '----' + self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[1]
                        else:
                            line = self.cpuclockFormat.split('$ccpucl')[0]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpuclock0.setText(text)
                        self.layout.addItem(self.label_cpuclock0)
                        text = self.formatLine.split('$LINE')[0] + "----" + self.formatLine.split('$LINE')[1]
                        for core in range(self.numCores):
                            exec ('self.label_coreCpuclock' + str(core) + ' = Plasma.Label(self.applet)')
                            exec ('self.label_coreCpuclock' + str(core) + '.setText(text)')
                            exec ('self.layout.addItem(self.label_coreCpuclock' + str(core) + ')')
                        if (self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] != self.cpuclockFormat.split('$ccpucl')[1]):
                            line = self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] + '----' + self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[1]
                        else:
                            line = self.cpuclockFormat.split('$ccpucl')[1]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpuclock1.setText(text)
                        self.layout.addItem(self.label_cpuclock1)
                    else:
                        self.label_cpuclock = Plasma.Label(self.applet)
                        if (self.cpuclockFormat.split('$cpucl')[0] != self.cpuclockFormat):
                            line = self.cpuclockFormat.split('$cpucl')[0] + '----' + self.cpuclockFormat.split('$cpucl')[1]
                        else:
                            line = self.cpuclockFormat
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_cpuclock.setText(text)
                        self.layout.addItem(self.label_cpuclock)
            elif (order == "8"):
                if (self.uptimeBool == 1):
                    self.uptimeFormat = str(self.settings.get('uptimeFormat', '[uptime: $uptime]'))
                    self.label_uptime = Plasma.Label(self.applet)
                    if (self.uptimeFormat.split('$uptime')[0] != self.uptimeFormat):
                        line = self.uptimeFormat.split('$uptime')[0] + '---d--h--m' + self.uptimeFormat.split('$uptime')[1]
                    else:
                        line = self.uptimeFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_uptime.setText(text)
                    self.layout.addItem(self.label_uptime)
            elif (order == "9"):
                if (self.gpuBool == 1):
                    self.gpuFormat = str(self.settings.get('gpuFormat', '[gpu: $gpu%]'))
                    self.label_gpu = Plasma.Label(self.applet)
                    if (self.gpuFormat.split('$gpu')[0] != self.gpuFormat):
                        line = self.gpuFormat.split('$gpu')[0] + '-----' + self.gpuFormat.split('$gpu')[1]
                    else:
                        line = self.gpuFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_gpu.setText(text)
                    self.layout.addItem(self.label_gpu)
            elif (order == "a"):
                if (self.gputempBool == 1):
                    self.gputempFormat = str(self.settings.get('gputempFormat', '[gpu temp: $gputemp&deg;C]'))
                    self.label_gputemp = Plasma.Label(self.applet)
                    if (self.gputempFormat.split('$gputemp')[0] != self.gputempFormat):
                        line = self.gputempFormat.split('$gputemp')[0] + '----' + self.gputempFormat.split('$gputemp')[1]
                    else:
                        line = self.gputempFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_gputemp.setText(text)
                    self.layout.addItem(self.label_gputemp)
            elif (order == "b"):
                if (self.hddBool == 1):
                    self.hddFormat = str(self.settings.get('hddFormat', '[hdd: @@/@@%]'))
                    if (self.hddFormat.split('@@')[0] != self.hddFormat):
                        self.mountPoints = self.hddFormat.split('@@')[1].split(';')
                        line = self.hddFormat.split('@@')[0]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_hdd0 = Plasma.Label(self.applet)
                        self.label_hdd0.setText(text)
                        self.layout.addItem(self.label_hdd0)
                        text = self.formatLine.split('$LINE')[0] + "-----" + self.formatLine.split('$LINE')[1]
                        for mount in self.mountPoints:
                            exec ('self.label_hdd_' + ''.join(mount.split('/')) + ' = Plasma.Label(self.applet)')
                            exec ('self.label_hdd_' + ''.join(mount.split('/')) + '.setText(text)')
                            exec ('self.layout.addItem(self.label_hdd_' + ''.join(mount.split('/')) + ')')
                        line = self.hddFormat.split('@@')[2]
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_hdd1 = Plasma.Label(self.applet)
                        self.label_hdd1.setText(text)
                        self.layout.addItem(self.label_hdd1)
                    else:
                        line = self.hddFormat
                        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                        self.label_hdd0.setText(text)
                        self.layout.addItem(self.label_hdd0)
            elif (order == "c"):
                if (self.hddtempBool == 1):
                    self.hddtempFormat = str(self.settings.get('hddtempFormat', '[hdd temp: @@/dev/sda@@&deg;C]'))
                    self.label_hddtemp = Plasma.Label(self.applet)
                    if (self.hddtempFormat.split('@@')[0] != self.hddtempFormat):
                        line = self.hddtempFormat.split('@@')[0] + '----' + self.hddtempFormat.split('@@')[2]
                    else:
                        line = self.hddtempFormat
                    text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                    self.label_hddtemp.setText(text)
                    self.layout.addItem(self.label_hddtemp)
        self.applet.setLayout(self.layout)        
        self.theme = Plasma.Svg(self)
        self.theme.setImagePath("widgets/background")
        self.setBackgroundHints(Plasma.Applet.DefaultBackground)
        self.resize(10,10)
        
        # start timer
        self.connectToEngine()
        self.timer = QtCore.QTimer()
        self.timer.setInterval(self.interval)
        self.startPolling()


    def setupNetdev(self):
        """function to setup network device"""
        if (self.num_dev == 0):
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        self.netdev = line.split()[0][:-1]
        else:
            interfaces = []
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        interfaces.append(line.split()[0][:-1])
                    
            command_line = "if ! (ifconfig "+ interfaces[1] + " | grep 'inet ' > /dev/null); then "
            command_line = command_line + "if ! (ifconfig "+ interfaces[0] + " | grep 'inet ' > /dev/null); then echo lo; "
            command_line = command_line + "else echo "+ interfaces[0] + "; fi; else echo "+ interfaces[1] + "; fi"
            self.netdev = commands.getoutput(command_line)


    def setupVar(self):
        """function to setup variables"""
        self.netdev = ''
        # setup number of cores
        commandOut = commands.getoutput("grep -c '^processor' /proc/cpuinfo")
        self.numCores = int(commandOut)
        # setup temperature device
        commandOut = commands.getoutput("sensors | grep Physical -B2")
        self.tempdev = "lmsensors/"+commandOut.split("\n")[0]+"/"+'_'.join(commandOut.split("\n")[2].split(":")[0].split())
        # setup gpu device
        commandOut = commands.getoutput("lspci")
        if (commandOut.lower().find('nvidia') > -1):
            self.gpudev = 'nvidia'
        elif (commandOut.lower().find('radeon') > -1):
            self.gpudev = 'ati'
        
        # create dictionaries
        self.dict_orders = {'6':'bat', '1':'cpu', '7':'cpuclock', '9':'gpu', 'a':'gputemp', 
        'b':'hdd', 'c':'hddtemp', '3':'mem', '5':'net', '4':'swap', '2':'temp', '8':'uptime'}
        self.dict_defFormat = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]', 
        'cpuclock':'[mhz: $cpucl]', 'gpu':'[gpu: $gpu%]', 
        'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: @@/@@%]', 
        'hddtemp':'[hdd temp: @@/dev/sda@@&deg;C]', 'mem':'[mem: $mem%]', 
        'net':'[$netdev: $netKB/s]', 'swap':'[swap: $swap%]', 
        'temp':'[temp: $temp&deg;C]', 'uptime':'[uptime: $uptime]'}
    
    def showConfigurationInterface(self):
        """function to show configuration window"""
        plasmascript.Applet.showConfigurationInterface(self)


    def showTooltip(self, text):
        """function to create and set tooltip"""
        tooltip = Plasma.ToolTipContent()
        tooltip.setImage(KIcon(self.icon()))
        tooltip.setSubText(text)
        tooltip.setAutohide(False)
        Plasma.ToolTipManager.self().setContent(self.applet, tooltip)
        Plasma.ToolTipManager.self().registerWidget(self.applet)


    def startPolling(self):
        try:
            self.timer.start()
            QtCore.QObject.connect(self.timer, QtCore.SIGNAL("timeout()"), self.updateLabel)
            
            self.updateLabel()
            self.showTooltip('')
        except Exception as (strerror):
            self.showTooltip(str(strerror))
            self.label_error = Plasma.Label(self.applet)
            self.label_error.setText('<font color="red">ERROR</font>')
            self.layout.addItem(self.label_error)
            return
   
    def updateLabel(self):
        """function to update label"""
        if (self.gpuBool == 1):
            self.gpuText()
        if (self.gputempBool == 1):
            self.gputempText()
        if ((self.memBool == 1) and (self.memInMb == False)):
            self.memText()
        if ((self.swapBool == 1) and (self.swapInMb == False)):
            self.swapText()
        if (self.hddtempBool == 1):
            self.hddtempText()
        if (self.batBool == 1):
            self.batText()


    def batText(self):
        """function to set battery text"""
        line = self.batFormat
        if (line.split('$bat')[0] != line):
            if os.path.exists(self.battery_device):
                with open (self.battery_device, 'r') as bat_file:
                    bat = bat_file.readline().split('\n')[0]
            else:
                bat = 'off'
            bat = "%3s" % (bat)
            line = line.split('$bat')[0] + bat + line.split('$bat')[1]
        
        if (line.split('$ac')[0] != line):
            if os.path.exists(self.ac_device):
                with open (self.ac_device, 'r') as bat_file:
                    bat = bat_file.readline().split('\n')[0]
                if (bat == '1'):
                    bat = '(*)'
                else:
                    bat = '( )'
            else:
                bat = '(?)'
            line = line.split('$ac')[0] + bat + line.split('$ac')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_bat.setText(text)


    def gpuText(self):
        """function to set gpu text"""
        if (self.gpudev == 'nvidia'):
            commandOut = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
            if (commandOut.split()[2] == 'N/A'):
                gpu = '  N/A'
            else:
                gpu = "%5s" % (str(round(float(commandOut.split()[2][:-1]), 1)))
        elif (self.gpudev == 'ati'):
            commandOut = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
            gpu = "%5s" % (str(round(float(commandOut.split()[3][:-1]), 1)))
        else:
            gpu = '-----'
        if (self.gpuFormat.split('$gpu')[0] != self.gpuFormat):
            line = self.gpuFormat.split('$gpu')[0] + gpu + self.gpuFormat.split('$gpu')[1]
        else:
            line = self.gpuFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_gpu.setText(text)


    def gputempText(self):
        """function to set gpu temperature text"""
        if (self.gpudev == 'nvidia'):
            commandOut = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
            gputemp = "%4s" % (str(round(float(commandOut.split()[2]), 1)))
        elif (self.gpudev == 'ati'):
            commandOut = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
            gputemp = "%4s" % (str(round(float(commandOut.split()[4]), 1)))
        else:
            gputemp = '----'
        if (self.gputempFormat.split('$gputemp')[0] != self.gputempFormat):
            line = self.gputempFormat.split('$gputemp')[0] + gputemp + self.gputempFormat.split('$gputemp')[1]
        else:
            line = self.gputempFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_gputemp.setText(text)


    def hddtempText(self):
        """function to set hdd temperature text"""
        commandOut = commands.getoutput("hddtemp "+self.hddtempFormat.split('@@')[1])
        hddtemp = "%4s" % (str(round(float(commandOut.split(':')[2][:-3]), 1)))
        if (self.hddtempFormat.split('@@')[0] != self.hddtempFormat):
            line = self.hddtempFormat.split('@@')[0] + hddtemp + self.hddtempFormat.split('@@')[2]
        else:
            line = self.hddtempFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_hddtemp.setText(text)


    def memText(self):
        """function to set mem text"""
        full = self.mem_uf + self.mem_free
        mem = 100 * self.mem_used / full
        mem = "%5s" % (str(round(mem, 1)))
        if (self.memFormat.split('$mem')[0] != self.memFormat):
            line = self.memFormat.split('$mem')[0] + mem + self.memFormat.split('$mem')[1]
        else:
            line = self.memFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_mem.setText(text)


    def swapText(self):
        """function to set swap text"""
        full = self.swap_used + self.swap_free
        mem = 100 * self.swap_used / full
        mem = "%5s" % (str(round(mem, 1)))
        if (self.swapFormat.split('$swap')[0] != self.swapFormat):
            line = self.swapFormat.split('$swap')[0] + mem + self.swapFormat.split('$swap')[1]
        else:
            line = self.swapFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_swap.setText(text)


    def connectToEngine(self):
        """function to initializate engine"""
        self.systemmonitor = self.dataEngine("systemmonitor")
        if (self.uptimeBool == 1):
            self.systemmonitor.connectSource("system/uptime", self, self.interval)
        if (self.cpuBool == 1):
            self.systemmonitor.connectSource("cpu/system/TotalLoad", self, self.interval)
            if (self.cpuFormat.split('$ccpu')[0] != self.cpuFormat):
                for core in range(self.numCores):
                    self.systemmonitor.connectSource("cpu/cpu"+str(core)+"/TotalLoad", self, self.interval)
        if (self.cpuclockBool == 1):
            self.systemmonitor.connectSource("cpu/system/AverageClock", self, self.interval)
            if (self.cpuclockFormat.split('$ccpucl')[0] != self.cpuclockFormat):
                for core in range(self.numCores):
                    self.systemmonitor.connectSource("cpu/cpu"+str(core)+"/clock", self, self.interval)
        if (self.netBool == 1):
            self.updateNetdev = 0
            self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/transmitter/data", self, self.interval)
            self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/receiver/data", self, self.interval)
        if (self.tempBool == 1):
            self.systemmonitor.connectSource(self.tempdev, self, self.interval)
        if (self.memBool == 1):
            if (self.memInMb):
                self.systemmonitor.connectSource("mem/physical/application", self, self.interval)
            else:
                self.systemmonitor.connectSource("mem/physical/free", self, 200)
                self.systemmonitor.connectSource("mem/physical/used", self, 200)
                self.systemmonitor.connectSource("mem/physical/application", self, 200)
        if (self.swapBool == 1):
            if (self.swapInMb):
                self.systemmonitor.connectSource("mem/swap/used", self, self.interval)
            else:
                self.systemmonitor.connectSource("mem/swap/free", self, 200)
                self.systemmonitor.connectSource("mem/swap/used", self, 200)
        if (self.hddBool == 1):
            for mount in self.mountPoints:
                self.systemmonitor.connectSource("partitions" + mount + "/filllevel", self, self.interval)

    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to refresh data"""
        if (sourceName == "system/uptime"):
            value = int(round(float(data[QString(u'value')]), 1))
            uptimeText = '%3sd%2sh%2sm' % (str(int(value/(24*60*60))), int(value/60/60)-int(value/24/60/60)*24, (value-value%60)/60%60)
            if (self.uptimeFormat.split('$uptime')[0] != self.uptimeFormat):
                line = self.uptimeFormat.split('$uptime')[0] + uptimeText + self.uptimeFormat.split('$uptime')[1]
            else:
                line = self.uptimeFormat
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_uptime.setText(text)
        elif (sourceName == "cpu/system/TotalLoad"):
            value = str(round(float(data[QString(u'value')]), 1))
            cpuText = "%5s" % (value)
            if (self.cpuFormat.split('$ccpu')[0] != self.cpuFormat):
                if (self.cpuFormat.split('$ccpu')[0].split('$cpu')[0] != self.cpuFormat.split('$ccpu')[0]):
                    line = self.cpuFormat.split('$ccpu')[0].split('$cpu')[0] + cpuText + self.cpuFormat.split('$ccpu')[0].split('$cpu')[1]
                else:
                    line = self.cpuFormat.split('$ccpu')[0]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpu0.setText(text)
                if (self.cpuFormat.split('$ccpu')[1].split('$cpu')[0] != self.cpuFormat.split('$ccpu')[1]):
                    line = self.cpuFormat.split('$ccpu')[1].split('$cpu')[0] + cpuText + self.cpuFormat.split('$ccpu')[1].split('$cpu')[1]
                else:
                    line = self.cpuFormat.split('$ccpu')[1]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpu1.setText(text)
            else:
                if (self.cpuFormat.split('$cpu')[0] != self.cpuFormat):
                    line = self.cpuFormat.split('$cpu')[0] + cpuText + self.cpuFormat.split('$cpu')[1]
                else:
                    line = self.cpuFormat
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpu.setText(text)
        elif ((str(sourceName)[:7] == "cpu/cpu") and (str(sourceName).split('/')[2] == "TotalLoad")):
            value = str(round(float(data[QString(u'value')]), 1))
            cpuText = "%5s" % (value)
            text = self.formatLine.split('$LINE')[0] + cpuText + self.formatLine.split('$LINE')[1]
            #text = self.formatLine.split('$LINE')[0] + "Core" + str(sourceName)[7] + "\n" + cpuText + self.formatLine.split('$LINE')[1]
            exec ('self.label_coreCpu' + str(sourceName)[7] + '.setText(text)')
        elif (sourceName == "cpu/system/AverageClock"):
            value = str(data[QString(u'value')]).split('.')[0]
            cpuclockText = "%4s" % (value)
            if (self.cpuclockFormat.split('$ccpucl')[0] != self.cpuclockFormat):
                if (self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] != self.cpuclockFormat.split('$ccpucl')[0]):
                    line = self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] + cpuclockText + self.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[1]
                else:
                    line = self.cpuclockFormat.split('$ccpucl')[0]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpuclock0.setText(text)
                if (self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] != self.cpuclockFormat.split('$ccpucl')[1]):
                    line = self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] + cpuclockText + self.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[1]
                else:
                    line = self.cpuclockFormat.split('$ccpucl')[1]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpuclock1.setText(text)
            else:
                if (self.cpuclockFormat.split('$cpucl')[0] != self.cpuclockFormat):
                    line = self.cpuclockFormat.split('$cpucl')[0] + cpuclockText + self.cpuclockFormat.split('$cpucl')[1]
                else:
                    line = self.cpuclockFormat
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_cpuclock.setText(text)
        elif ((str(sourceName)[:7] == "cpu/cpu") and (str(sourceName).split('/')[2] == "clock")):
            value = str(data[QString(u'value')]).split('.')[0]
            cpuclockText = "%4s" % (value)
            text = self.formatLine.split('$LINE')[0] + cpuclockText + self.formatLine.split('$LINE')[1]
            exec ('self.label_coreCpuclock' + str(sourceName)[7] + '.setText(text)')
        elif (sourceName == "network/interfaces/"+self.netdev+"/transmitter/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            up_speed = "%4s" % (value)
            if (self.netFormat.split('$net')[0] != self.netFormat):
                line = '/' + up_speed + self.netFormat.split('$net')[1]
            else:
                line = ''
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_netUp.setText(text)
        elif (sourceName == "network/interfaces/"+self.netdev+"/receiver/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            down_speed = "%4s" % (value)
            if (self.netFormat.split('$net')[0] != self.netFormat):
                line = self.netFormat.split('$net')[0] + down_speed
            else:
                line = self.netFormat
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_netDown.setText(text)
            # update network device
            self.updateNetdev = self.updateNetdev + 1
            if (self.updateNetdev == 100):
                self.updateNetdev = 0
                if (self.netNonFormat.split('@@')[0] == self.netNonFormat):
                    self.systemmonitor.disconnectSource("network/interfaces/"+self.netdev+"/transmitter/data", self)
                    self.systemmonitor.disconnectSource("network/interfaces/"+self.netdev+"/receiver/data", self)
                    self.setupNetdev()
                    self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/transmitter/data", self, self.interval)
                    self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/receiver/data", self, self.interval)
                if (self.netNonFormat.split('$netdev')[0] != self.netNonFormat):
                    self.netFormat = self.netNonFormat.split('$netdev')[0] + self.netdev + self.netNonFormat.split('$netdev')[1]
                else:
                    self.netFormat = self.netNonFormat
        elif (sourceName == self.tempdev):
            value = str(round(float(data[QString(u'value')]), 1))
            tempText = "%4s" % (value)
            if (self.tempFormat.split('$temp')[0] != self.tempFormat):
                line = self.tempFormat.split('$temp')[0] + tempText + self.tempFormat.split('$temp')[1]
            else:
                line = self.tempFormat
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_temp.setText(text)
        elif (str(sourceName).split('/')[0] == "partitions"):
            value = str(round(float(data[QString(u'value')]), 1))
            hddText = "%5s" % (value)
            text = self.formatLine.split('$LINE')[0] + hddText + self.formatLine.split('$LINE')[1]
            exec ('self.label_hdd_' + ''.join(str(sourceName).split('/')[1:-1]) + '.setText(text)')
        elif (sourceName == "mem/physical/free"):
            self.mem_free = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/used"):
            self.mem_uf = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/application"):
            if (self.memInMb):
                mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                mem = "%5s" % (mem)
                if (self.memFormat.split('$memmb')[0] != self.memFormat):
                    line = self.memFormat.split('$memmb')[0] + mem + self.memFormat.split('$memmb')[1]
                else:
                    line = self.memFormat
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_mem.setText(text)
            else:
                self.mem_used = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/free"):
            self.swap_free = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/used"):
            if (self.swapInMb):
                mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                mem = "%5s" % (mem)
                if (self.swapFormat.split('$swapmb')[0] != self.swapFormat):
                    line = self.swapFormat.split('$swapmb')[0] + mem + self.swapFormat.split('$swapmb')[1]
                else:
                    line = self.swapFormat
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_swap.setText(text)
            else:
                self.swap_used = float(data[QString(u'value')])



def CreateApplet(parent):
	return pyTextWidget(parent)
