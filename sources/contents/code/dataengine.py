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

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import Plasma
from PyKDE4 import plasmascript

import datetime

timeLetters = ['dddd', 'ddd', 'dd', 'd', 'MMMM', 'MMM', 'MM', 'M', 'yyyy', 'yy', \
    'hh', 'h', 'mm', 'm', 'ss', 's']


class DataEngine:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def connectToEngine(self):
        """function to initializate engine"""
        self.parent.systemmonitor = self.parent.dataEngine("systemmonitor")
        if ((self.parent.gputempBool > 0) or (self.parent.gpuBool > 0) or (self.parent.hddtempBool > 0) or (self.parent.playerBool > 0)):
            self.parent.extsysmon = self.parent.dataEngine("ext-sysmon")
        
        if (self.parent.timeBool > 0):
            self.parent.timemon = self.parent.dataEngine("time")
            self.parent.timemon.connectSource("Local", self.parent, 1000)
        
        if (self.parent.uptimeBool > 0):
            self.parent.systemmonitor.connectSource("system/uptime", self.parent, self.parent.interval)
        if (self.parent.cpuBool > 0):
            self.parent.systemmonitor.connectSource("cpu/system/TotalLoad", self.parent, self.parent.interval)
            if (self.parent.cpuFormat.split('$ccpu')[0] != self.parent.cpuFormat):
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.connectSource("cpu/cpu"+str(core)+"/TotalLoad", self.parent, self.parent.interval)
        if (self.parent.cpuclockBool > 0):
            self.parent.systemmonitor.connectSource("cpu/system/AverageClock", self.parent, self.parent.interval)
            if (self.parent.cpuclockFormat.split('$ccpucl')[0] != self.parent.cpuclockFormat):
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.connectSource("cpu/cpu"+str(core)+"/clock", self.parent, self.parent.interval)
        if (self.parent.tempBool > 0):
            self.parent.systemmonitor.connectSource(self.parent.tempdev, self.parent, self.parent.interval)
        if (self.parent.gpuBool > 0):
            self.parent.extsysmon.connectSource("gpu", self.parent, self.parent.interval)
        if (self.parent.gputempBool > 0):
            self.parent.extsysmon.connectSource("gputemp", self.parent, self.parent.interval)
        if (self.parent.memBool > 0):
            if (self.parent.memInMb):
                self.parent.systemmonitor.connectSource("mem/physical/application", self.parent, self.parent.interval)
            else:
                self.parent.systemmonitor.connectSource("mem/physical/free", self.parent, int(self.parent.interval*0.5))
                self.parent.systemmonitor.connectSource("mem/physical/used", self.parent, int(self.parent.interval*0.5))
                self.parent.systemmonitor.connectSource("mem/physical/application", self.parent, int(self.parent.interval*0.5))
        if (self.parent.swapBool > 0):
            if (self.parent.swapInMb):
                self.parent.systemmonitor.connectSource("mem/swap/used", self.parent, self.parent.interval)
            else:
                self.parent.systemmonitor.connectSource("mem/swap/free", self.parent, int(self.parent.interval*0.5))
                self.parent.systemmonitor.connectSource("mem/swap/used", self.parent, int(self.parent.interval*0.5))
        if (self.parent.hddBool > 0):
            for mount in self.parent.mountPoints:
                self.parent.systemmonitor.connectSource("partitions" + mount + "/filllevel", self.parent, self.parent.interval)
        if (self.parent.hddtempBool > 0):
            self.parent.extsysmon.connectSource("hddtemp", self.parent, self.parent.interval)
        if (self.parent.netBool > 0):
            self.parent.updateNetdev = 0
            self.parent.systemmonitor.connectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent, self.parent.interval)
            self.parent.systemmonitor.connectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent, self.parent.interval)
        if (self.parent.playerBool > 0):
            self.parent.extsysmon.connectSource("player", self.parent, self.parent.interval)
    
    def dataUpdated(self, sourceName, data):
        """function to update data"""
        #try:
        if True:
            if (sourceName == "system/uptime"):
                value = datetime.timedelta(0, int(round(float(data[QString(u'value')]), 1)))
                days = value.days
                hours = int(value.seconds / 60 / 60)
                minutes = int(value.seconds / 60 % 60)
                line = self.parent.uptimeFormat
                if (line.split('$uptime')[0] != line):
                    uptimeText = "%3id%2ih%2im" % (days, hours, minutes)
                    line = line.split('$uptime')[0] + uptimeText + line.split('$uptime')[1]
                elif (line.split('$custom')[0] != line):
                    line = ''.join(line.split('$custom'))
                    if (line.split('$ds')[0] != line):
                        line = line.split('$ds')[0] + str(days) + line.split('$ds')[1]
                    if (line.split('$hs')[0] != line):
                        line = line.split('$hs')[0] + str(hours) + line.split('$hs')[1]
                    if (line.split('$ms')[0] != line):
                        line = line.split('$ms')[0] + str(minutes) + line.split('$ms')[1]
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_uptime.setText(text)
            elif (sourceName == "cpu/system/TotalLoad"):
                value = str(round(float(data[QString(u'value')]), 1))
                cpuText = "%5s" % (value)
                if (self.parent.cpuFormat.split('$ccpu')[0] != self.parent.cpuFormat):
                    if (self.parent.cpuFormat.split('$ccpu')[0].split('$cpu')[0] != self.parent.cpuFormat.split('$ccpu')[0]):
                        line = self.parent.cpuFormat.split('$ccpu')[0].split('$cpu')[0] + cpuText + self.parent.cpuFormat.split('$ccpu')[0].split('$cpu')[1]
                    else:
                        line = self.parent.cpuFormat.split('$ccpu')[0]
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpu.setText(text)
                    if (self.parent.cpuFormat.split('$ccpu')[1].split('$cpu')[0] != self.parent.cpuFormat.split('$ccpu')[1]):
                        line = self.parent.cpuFormat.split('$ccpu')[1].split('$cpu')[0] + cpuText + self.parent.cpuFormat.split('$ccpu')[1].split('$cpu')[1]
                    else:
                        line = self.parent.cpuFormat.split('$ccpu')[1]
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpu1.setText(text)
                else:
                    if (self.parent.cpuFormat.split('$cpu')[0] != self.parent.cpuFormat):
                        line = self.parent.cpuFormat.split('$cpu')[0] + cpuText + self.parent.cpuFormat.split('$cpu')[1]
                    else:
                        line = self.parent.cpuFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpu.setText(text)
            elif ((str(sourceName)[:7] == "cpu/cpu") and (str(sourceName).split('/')[2] == "TotalLoad")):
                value = str(round(float(data[QString(u'value')]), 1))
                cpuText = "%5s" % (value)
                text = self.parent.formatLine.split('$LINE')[0] + cpuText + self.parent.formatLine.split('$LINE')[1]
                exec ('self.parent.label_coreCpu' + str(sourceName)[7] + '.setText(text)')
            elif (sourceName == "cpu/system/AverageClock"):
                value = str(data[QString(u'value')]).split('.')[0]
                cpuclockText = "%4s" % (value)
                if (self.parent.cpuclockFormat.split('$ccpucl')[0] != self.parent.cpuclockFormat):
                    if (self.parent.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] != self.parent.cpuclockFormat.split('$ccpucl')[0]):
                        line = self.parent.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[0] + cpuclockText + self.parent.cpuclockFormat.split('$ccpucl')[0].split('$cpucl')[1]
                    else:
                        line = self.parent.cpuclockFormat.split('$ccpucl')[0]
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpuclock.setText(text)
                    if (self.parent.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] != self.parent.cpuclockFormat.split('$ccpucl')[1]):
                        line = self.parent.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[0] + cpuclockText + self.parent.cpuclockFormat.split('$ccpucl')[1].split('$cpucl')[1]
                    else:
                        line = self.parent.cpuclockFormat.split('$ccpucl')[1]
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpuclock1.setText(text)
                else:
                    if (self.parent.cpuclockFormat.split('$cpucl')[0] != self.parent.cpuclockFormat):
                        line = self.parent.cpuclockFormat.split('$cpucl')[0] + cpuclockText + self.parent.cpuclockFormat.split('$cpucl')[1]
                    else:
                        line = self.parent.cpuclockFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_cpuclock.setText(text)
            elif ((str(sourceName)[:7] == "cpu/cpu") and (str(sourceName).split('/')[2] == "clock")):
                value = str(data[QString(u'value')]).split('.')[0]
                cpuclockText = "%4s" % (value)
                text = self.parent.formatLine.split('$LINE')[0] + cpuclockText + self.parent.formatLine.split('$LINE')[1]
                exec ('self.parent.label_coreCpuclock' + str(sourceName)[7] + '.setText(text)')
            elif (sourceName == "network/interfaces/"+self.parent.netdev+"/transmitter/data"):
                value = str(data[QString(u'value')]).split('.')[0]
                up_speed = "%4s" % (value)
                if (self.parent.netFormat.split('$net')[0] != self.parent.netFormat):
                    line = '/' + up_speed + self.parent.netFormat.split('$net')[1]
                else:
                    line = ''
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_netUp.setText(text)
            elif (sourceName == "network/interfaces/"+self.parent.netdev+"/receiver/data"):
                value = str(data[QString(u'value')]).split('.')[0]
                down_speed = "%4s" % (value)
                if (self.parent.netFormat.split('$net')[0] != self.parent.netFormat):
                    line = self.parent.netFormat.split('$net')[0] + down_speed
                else:
                    line = self.parent.netFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_netDown.setText(text)
                # update network device
                self.parent.updateNetdev = self.parent.updateNetdev + 1
                if (self.parent.updateNetdev == 100):
                    self.parent.updateNetdev = 0
                    if (self.parent.netNonFormat.split('@@')[0] == self.parent.netNonFormat):
                        self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent)
                        self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent)
                        self.parent.netdev = self.parent.setupNetdev()
                        self.parent.systemmonitor.connectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent, self.parent.interval)
                        self.parent.systemmonitor.connectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent, self.parent.interval)
                    if (self.parent.netNonFormat.split('$netdev')[0] != self.parent.netNonFormat):
                        self.parent.netFormat = self.parent.netNonFormat.split('$netdev')[0] + self.parent.netdev + self.parent.netNonFormat.split('$netdev')[1]
                    else:
                        self.parent.netFormat = self.parent.netNonFormat
            elif (sourceName == self.parent.tempdev):
                value = str(round(float(data[QString(u'value')]), 1))
                tempText = "%4s" % (value)
                if (self.parent.tempFormat.split('$temp')[0] != self.parent.tempFormat):
                    line = self.parent.tempFormat.split('$temp')[0] + tempText + self.parent.tempFormat.split('$temp')[1]
                else:
                    line = self.parent.tempFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_temp.setText(text)
            elif (str(sourceName).split('/')[0] == "partitions"):
                value = str(round(float(data[QString(u'value')]), 1))
                hddText = "%5s" % (value)
                text = self.parent.formatLine.split('$LINE')[0] + hddText + self.parent.formatLine.split('$LINE')[1]
                exec ('self.parent.label_hdd_' + ''.join(str(sourceName).split('/')[1:-1]) + '.setText(text)')
            elif (sourceName == "mem/physical/free"):
                self.parent.mem_free = float(data[QString(u'value')])
            elif (sourceName == "mem/physical/used"):
                self.parent.mem_uf = float(data[QString(u'value')])
            elif (sourceName == "mem/physical/application"):
                if (self.parent.memInMb):
                    mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                    mem = "%5s" % (mem)
                    if (self.parent.memFormat.split('$memmb')[0] != self.parent.memFormat):
                        line = self.parent.memFormat.split('$memmb')[0] + mem + self.parent.memFormat.split('$memmb')[1]
                    else:
                        line = self.parent.memFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_mem.setText(text)
                else:
                    self.parent.mem_used = float(data[QString(u'value')])
            elif (sourceName == "mem/swap/free"):
                self.parent.swap_free = float(data[QString(u'value')])
            elif (sourceName == "mem/swap/used"):
                if (self.parent.swapInMb):
                    mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                    mem = "%5s" % (mem)
                    if (self.parent.swapFormat.split('$swapmb')[0] != self.parent.swapFormat):
                        line = self.parent.swapFormat.split('$swapmb')[0] + mem + self.parent.swapFormat.split('$swapmb')[1]
                    else:
                        line = self.parent.swapFormat
                    text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                    self.parent.label_swap.setText(text)
                else:
                    self.parent.swap_used = float(data[QString(u'value')])
            elif (sourceName == "gpu"):
                value = str(data[QString(u'GPU')])
                gpuText = "%4s" % (value)
                if (self.parent.gpuFormat.split('$gpu')[0] != self.parent.gpuFormat):
                    line = self.parent.gpuFormat.split('$gpu')[0] + gpuText + self.parent.gpuFormat.split('$gpu')[1]
                else:
                    line = self.parent.gpuFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_gpu.setText(text)
            elif (sourceName == "gputemp"):
                value = str(data[QString(u'GPUTemp')])
                gputempText = "%4s" % (value)
                if (self.parent.gputempFormat.split('$gputemp')[0] != self.parent.gputempFormat):
                    line = self.parent.gputempFormat.split('$gputemp')[0] + gputempText + self.parent.gputempFormat.split('$gputemp')[1]
                else:
                    line = self.parent.gputempFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_gputemp.setText(text)
            elif (sourceName == "hddtemp"):
                value = str(data[QString(self.parent.hddtempFormat.split('@@')[1])])
                hddtempText = "%4s" % (value)
                if (self.parent.hddtempFormat.split('@@')[0] != self.parent.hddtempFormat):
                    line = self.parent.hddtempFormat.split('@@')[0] + hddtempText + self.parent.hddtempFormat.split('@@')[2]
                else:
                    line = self.parent.hddtempFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_hddtemp.setText(text)
            elif (sourceName == "player"):
                if (self.parent.player_name == 0):
                    album = str(data[QString(u'amarok_album')])
                    artist = str(data[QString(u'amarok_artist')])
                    progress = str(data[QString(u'amarok_progress')])
                    time = str(data[QString(u'amarok_duration')])
                    title = str(data[QString(u'amarok_title')])
                elif (self.parent.player_name == 1):
                    album = str(data[QString(u'mpd_album')])
                    artist = str(data[QString(u'mpd_artist')])
                    progress = str(data[QString(u'mpd_progress')])
                    time = str(data[QString(u'mpd_duration')])
                    title = str(data[QString(u'mpd_title')])
                elif (self.parent.player_name == 2):
                    album = str(data[QString(u'qmmp_album')])
                    artist = str(data[QString(u'qmmp_artist')])
                    progress = str(data[QString(u'qmmp_progress')])
                    time = str(data[QString(u'qmmp_duration')])
                    title = str(data[QString(u'qmmp_title')])
                line = self.parent.playerFormat
                if (line.split('$album')[0] != line):
                    line = line.split('$album')[0] + album + line.split('$album')[1]
                if (line.split('$artist')[0] != line):
                    line = line.split('$artist')[0] + artist + line.split('$artist')[1]
                if (line.split('$progress')[0] != line):
                    timeText = '%02i:%02i' % (int(time)/60, int(time)%60)
                    line = line.split('$progress')[0] + timeText + line.split('$progress')[1]
                if (line.split('$time')[0] != line):
                    timeText = '%02i:%02i' % (int(time)/60, int(time)%60)
                    line = line.split('$time')[0] + timeText + line.split('$time')[1]
                if (line.split('$title')[0] != line):
                    line = line.split('$title')[0] + title + line.split('$title')[1]
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_player.setText(text)
            elif (sourceName == "Local"):
                if (self.parent.timeFormat.split('$time')[0] != self.parent.timeFormat):
                    value = str(data[QString(u'DateTime')].toString(Qt.TextDate).toUtf8())
                    line = self.parent.timeFormat.split('$time')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$time')[1]
                elif (self.parent.timeFormat.split('$isotime')[0] != self.parent.timeFormat):
                    value = str(data[QString(u'DateTime')].toString(Qt.ISODate).toUtf8())
                    line = self.parent.timeFormat.split('$isotime')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$isotime')[1]
                elif (self.parent.timeFormat.split('$shorttime')[0] != self.parent.timeFormat):
                    value = str(data[QString(u'DateTime')].toString(Qt.SystemLocaleShortDate).toUtf8())
                    line = self.parent.timeFormat.split('$shorttime')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$shorttime')[1]
                elif (self.parent.timeFormat.split('$longtime')[0] != self.parent.timeFormat):
                    value = str(data[QString(u'DateTime')].toString(Qt.SystemLocaleLongDate).toUtf8())
                    line = self.parent.timeFormat.split('$longtime')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$longtime')[1]
                elif (self.parent.timeFormat.split('$snsntime')[0] != self.parent.timeFormat):
                    rawDate = str(data[QString(u'DateTime')].toString(Qt.TextDate).toUtf8())
                    value = rawDate.split()[0] + " " + rawDate.split()[2] + " " + \
                        rawDate.split()[1] + " " + ':'.join(rawDate.split()[3].split(':')[0:2])
                    line = self.parent.timeFormat.split('$snsntime')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$snsntime')[1]
                elif (self.parent.timeFormat.split('$nsnstime')[0] != self.parent.timeFormat):
                    rawDate = str(data[QString(u'DateTime')].toString(Qt.TextDate).toUtf8())
                    value = ':'.join(rawDate.split()[3].split(':')[0:2]) + " " + rawDate.split()[0] + " " + \
                        rawDate.split()[2] + " " + rawDate.split()[1]
                    line = self.parent.timeFormat.split('$nsnstime')[0] + value.decode("utf-8") + self.parent.timeFormat.split('$nsnstime')[1]
                elif (self.parent.timeFormat.split('$custom')[0] != self.parent.timeFormat):
                    rawDate = data[QString(u'DateTime')]
                    line = ''.join(self.parent.timeFormat.split('$custom'))
                    for letters in timeLetters:
                        if (line.split('$'+letters)[0] != line):
                            line = line.split('$'+letters)[0] + \
                                str(data[QString(u'DateTime')].toString(letters).toUtf8()).decode("utf-8") + \
                                line.split('$'+letters)[1]
                else:
                    line = self.parent.timeFormat
                text = self.parent.formatLine.split('$LINE')[0] + line + self.parent.formatLine.split('$LINE')[1]
                self.parent.label_time.setText(text)
            
            self.parent.update()
        #except:
            #pass
