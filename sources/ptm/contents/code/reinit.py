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
from PyKDE4.plasma import Plasma
import config
import ptmnotify



class NewPlasmaLabel(Plasma.Label):
    """new Label with defined clicked() event"""
    def __init__(self, applet, parent):
        """class definition"""
        Plasma.Label.__init__(self, applet)
        self.parent = parent
        self.notify = ptmnotify.PTMNotify(self)
    
    
    def mousePressEvent(self, event):
        """mouse click event"""
        if (event.button() == Qt.LeftButton):
            self.notify.init()



class Reinit():
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def reinit(self, confAccept=False):
        """function to reinitializate widget"""
        settings = config.Config(self.parent)
        self.parent.interval = settings.get('interval', 2000).toInt()[0]
        self.parent.font_family = str(settings.get('font_family', 'Terminus'))
        self.parent.font_size = settings.get('font_size', 12).toInt()[0]
        self.parent.font_color = str(settings.get('font_color', '#000000'))
        self.parent.font_style = str(settings.get('font_style', 'normal'))
        self.parent.font_weight = settings.get('font_weight', 400).toInt()[0]
        self.parent.formatLine = "<pre><p align=\"center\"><span style=\" font-family:'" + self.parent.font_family + "'; font-style:" + self.parent.font_style
        self.parent.formatLine = self.parent.formatLine + "; font-size:" + str(self.parent.font_size) + "pt; font-weight:" + str(self.parent.font_weight)
        self.parent.formatLine = self.parent.formatLine + "; color:" + self.parent.font_color + ";\">$LINE</span></p></pre>"
        
        self.parent.custom_time = str(settings.get('custom_time', '$hh:$mm'))
        self.parent.custom_uptime = str(settings.get('custom_uptime', '$ds,$hs,$ms'))
        self.parent.tempNames = []
        self.parent.temp = {}
        for item in str(settings.get('temp_device', '')).split('@@'):
            self.parent.tempNames.append(item)
            self.parent.temp[item] =" 0.0"
        self.parent.mountNames = []
        self.parent.mount = {}
        for item in str(settings.get('mount', '/')).split('@@'):
            self.parent.mountNames.append(item)
            self.parent.mount[item] ="  0.0"
        self.parent.hddNames = []
        self.parent.hdd = {}
        for item in str(settings.get('hdd', '/dev/sda')).split('@@'):
            self.parent.hddNames.append(item)
            self.parent.hdd[item] =" 0.0"
        self.parent.netdir = str(settings.get('netdir', '/sys/class/net'))
        self.parent.netdevBool = settings.get('netdevBool', 0).toInt()[0]
        self.parent.custom_netdev = str(settings.get('custom_netdev', 'lo'))
        self.parent.battery_device = str(settings.get('battery_device', '/sys/class/power_supply/BAT0/capacity'))
        self.parent.ac_device = str(settings.get('ac_device', '/sys/class/power_supply/AC/online'))
        self.parent.player_name = settings.get('player_name', 0).toInt()[0]
        
        self.parent.tooltipNum = settings.get('tooltip_num', 100).toInt()[0]
        self.parent.tooltipColors['cpu'] = str(settings.get('cpu_color', '#ff0000'))
        self.parent.tooltipColors['cpuclock'] = str(settings.get('cpuclock_color', '#00ff00'))
        self.parent.tooltipColors['mem'] = str(settings.get('mem_color', '#0000ff'))
        self.parent.tooltipColors['swap'] = str(settings.get('swap_color', '#ffff00'))
        self.parent.tooltipColors['down'] = str(settings.get('down_color', '#00ffff'))
        self.parent.tooltipColors['up'] = str(settings.get('up_color', '#ff00ff'))
        
        self.parent.tooltipReq = []
        self.parent.label_order = str(settings.get('label_order', '1345'))
        for label in self.parent.dict_orders.values():
            if ((label == 'cpu') or (label == 'mem') or (label == 'swap') or (label == 'net')):
                exec ('self.parent.' + label + 'Bool = settings.get("' + label + 'Bool",  2).toInt()[0]')
            else:
                exec ('self.parent.' + label + 'Bool = settings.get("' + label + 'Bool",  0).toInt()[0]')
        
        # labels
        for order in self.parent.label_order:
            if (order == "1"):
                if (self.parent.cpuBool > 0):
                    self.parent.cpuFormat = str(settings.get('cpuFormat', '[cpu: $cpu%]'))
                    self.parent.label_cpu = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.cpuFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpu.setText(text)
                    self.parent.layout.addItem(self.parent.label_cpu)
                    if (self.parent.cpuBool == 2):
                        self.parent.tooltipReq.append('cpu')
            elif (order == "2"):
                if (self.parent.tempBool > 0):
                    self.parent.tempFormat = str(settings.get('tempFormat', '[temp: $temp0&deg;C]'))
                    self.parent.label_temp = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.tempFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_temp.setText(text)
                    self.parent.layout.addItem(self.parent.label_temp)
            elif (order == "3"):
                if (self.parent.memBool > 0):
                    self.parent.memFormat = str(settings.get('memFormat', '[mem: $mem%]'))
                    self.parent.label_mem = NewPlasmaLabel(self.parent.applet, self.parent)
                    if (self.parent.memFormat.split('$memmb')[0] != self.parent.memFormat):
                        self.parent.memInMb = True
                    elif (self.parent.memFormat.split('$mem')[0] != self.parent.memFormat):
                        self.parent.memInMb = False
                    line = self.parent.memFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_mem.setText(text)
                    self.parent.layout.addItem(self.parent.label_mem)
                    if (self.parent.memBool == 2):
                        self.parent.tooltipReq.append('mem')
            elif (order == "4"):
                if (self.parent.swapBool > 0):
                    self.parent.swapFormat = str(settings.get('swapFormat', '[swap: $swap%]'))
                    self.parent.label_swap = NewPlasmaLabel(self.parent.applet, self.parent)
                    if (self.parent.swapFormat.split('$swapmb')[0] != self.parent.swapFormat):
                        self.parent.swapInMb = True
                    elif (self.parent.swapFormat.split('$swap')[0] != self.parent.swapFormat):
                        self.parent.swapInMb = False
                    line = self.parent.swapFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_swap.setText(text)
                    self.parent.layout.addItem(self.parent.label_swap)
                    if (self.parent.swapBool == 2):
                        self.parent.tooltipReq.append('swap')
            elif (order == "5"):
                if (self.parent.netBool > 0):
                    self.parent.netNonFormat = str(settings.get('netNonFormat', '[net: $down/$upKB/s]'))
                    self.parent.label_net = NewPlasmaLabel(self.parent.applet, self.parent)
                    if (self.parent.netdevBool > 0):
                        self.parent.netdev = self.parent.custom_netdev
                    else:
                        self.parent.netdev = self.parent.setupNetdev()
                    if (self.parent.netNonFormat.split('$netdev')[0] != self.parent.netNonFormat):
                        self.parent.netFormat = self.parent.netNonFormat.split('$netdev')[0] + self.parent.netdev + self.parent.netNonFormat.split('$netdev')[1]
                    else:
                        self.parent.netFormat = self.parent.netNonFormat
                    line = self.parent.netFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_net.setText(text)
                    self.parent.layout.addItem(self.parent.label_net)
                    if (self.parent.netBool == 2):
                        self.parent.tooltipReq.append('down')
                        self.parent.tooltipReq.append('up')
            elif (order == "6"):
                if (self.parent.batBool > 0):
                    self.parent.batFormat = str(settings.get('batFormat', '[bat: $bat%$ac]'))
                    self.parent.label_bat = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.batFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_bat.setText(text)
                    self.parent.layout.addItem(self.parent.label_bat)
            elif (order == "7"):
                if (self.parent.cpuclockBool > 0):
                    self.parent.cpuclockFormat = str(settings.get('cpuclockFormat', '[mhz: $cpucl]'))
                    self.parent.label_cpuclock = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.cpuclockFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpuclock.setText(text)
                    self.parent.layout.addItem(self.parent.label_cpuclock)
                    if (self.parent.cpuclockBool == 2):
                        self.parent.tooltipReq.append('cpuclock')
            elif (order == "8"):
                if (self.parent.uptimeBool > 0):
                    self.parent.uptimeFormat = str(settings.get('uptimeFormat', '[uptime: $uptime]'))
                    self.parent.label_uptime = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.uptimeFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_uptime.setText(text)
                    self.parent.layout.addItem(self.parent.label_uptime)
            elif (order == "9"):
                if (self.parent.gpuBool > 0):
                    self.parent.gpuFormat = str(settings.get('gpuFormat', '[gpu: $gpu%]'))
                    self.parent.label_gpu = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.gpuFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_gpu.setText(text)
                    self.parent.layout.addItem(self.parent.label_gpu)
            elif (order == "a"):
                if (self.parent.gputempBool > 0):
                    self.parent.gputempFormat = str(settings.get('gputempFormat', '[gpu temp: $gputemp&deg;C]'))
                    self.parent.label_gputemp = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.gputempFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_gputemp.setText(text)
                    self.parent.layout.addItem(self.parent.label_gputemp)
            elif (order == "b"):
                if (self.parent.hddBool > 0):
                    self.parent.hddFormat = str(settings.get('hddFormat', '[hdd: $hdd0%]'))
                    self.parent.label_hdd = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.hddFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_hdd.setText(text)
                    self.parent.layout.addItem(self.parent.label_hdd)
            elif (order == "c"):
                if (self.parent.hddtempBool > 0):
                    self.parent.hddtempFormat = str(settings.get('hddtempFormat', '[hdd temp: $hddtemp0&deg;C]'))
                    self.parent.label_hddtemp = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.hddtempFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_hddtemp.setText(text)
                    self.parent.layout.addItem(self.parent.label_hddtemp)
            elif (order == "d"):
                if (self.parent.playerBool > 0):
                    self.parent.playerFormat = str(settings.get('playerFormat', '[$artist - $title]'))
                    self.parent.label_player = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.playerFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_player.setText(text)
                    self.parent.layout.addItem(self.parent.label_player)
            elif (order == "e"):
                if (self.parent.timeBool > 0):
                    self.parent.timeFormat = str(settings.get('timeFormat', '[$time]'))
                    self.parent.label_time = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.timeFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_time.setText(text)
                    self.parent.layout.addItem(self.parent.label_time)
            elif (order == "f"):
                if (self.parent.customBool > 0):
                    self.parent.customFormat = str(settings.get('customFormat', '[$custom]'))
                    self.parent.label_custom = NewPlasmaLabel(self.parent.applet, self.parent)
                    line = self.parent.customFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_custom.setText(text)
                    self.parent.layout.addItem(self.parent.label_custom)
        if not confAccept:
            self.parent.layout.setContentsMargins(1, 1, 1, 1)
            self.parent.applet.setLayout(self.parent.layout)
            self.parent.theme = Plasma.Svg(self.parent)
            self.parent.theme.setImagePath("widgets/background")
            self.parent.setBackgroundHints(Plasma.Applet.DefaultBackground)
        self.parent.resize(10, 10)
        
        # create dataengines
        self.parent.thread().wait(60000)
        self.parent.dataengine.connectToEngine()
        
        self.parent.timer.setInterval(self.parent.interval)
        self.parent.startPolling()
