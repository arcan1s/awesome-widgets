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

import datetime

timeLetters = ['dddd', 'ddd', 'dd', 'd', \
    'MMMM', 'MMM', 'MM', 'M', 'yyyy', 'yy', \
    'hh', 'h', 'mm', 'm', 'ss', 's']



class DataEngine:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent


    def connectToEngine(self, bools=None, dataEngines=None, interval=1000, names=None):
        """function to initializate engine"""
        if (bools['cpu'] > 0):
            dataEngines['system'].connectSource("cpu/system/TotalLoad", self.parent, interval)
            for core in range(8):
                dataEngines['system'].connectSource("cpu/cpu" + str(core) + "/TotalLoad", self.parent, interval)
        if (bools['cpuclock'] > 0):
            dataEngines['system'].connectSource("cpu/system/AverageClock", self.parent, interval)
            for core in range(8):
                dataEngines['system'].connectSource("cpu/cpu" + str(core) + "/clock", self.parent, interval)
        if (bools['custom'] > 0):
            dataEngines['ext'].connectSource("custom", self.parent, interval)
        if (bools['gpu'] > 0):
            dataEngines['ext'].connectSource("gpu", self.parent, interval)
        if (bools['gputemp'] > 0):
            dataEngines['ext'].connectSource("gputemp", self.parent, interval)
        if (bools['hdd'] > 0):
            for item in names['hdd']:
                dataEngines['system'].connectSource("partitions" + item + "/filllevel", self.parent, interval)
        if (bools['hddtemp'] > 0):
            dataEngines['ext'].connectSource("hddtemp", self.parent, interval)
        if (bools['mem'] > 0):
            dataEngines['system'].connectSource("mem/physical/free", self.parent, interval)
            dataEngines['system'].connectSource("mem/physical/used", self.parent, interval)
            dataEngines['system'].connectSource("mem/physical/application", self.parent, interval)
        if (bools['net'] > 0):
            self.updateNetdev = 0
            dataEngines['system'].connectSource("network/interfaces/" + names['net'] + "/transmitter/data", self.parent, interval)
            dataEngines['system'].connectSource("network/interfaces/" + names['net'] + "/receiver/data", self.parent, interval)
        if (bools['player'] > 0):
            dataEngines['ext'].connectSource("player", self.parent, interval)
        if (bools['swap'] > 0):
            dataEngines['system'].connectSource("mem/swap/free", self.parent, interval)
            dataEngines['system'].connectSource("mem/swap/used", self.parent, interval)
        if (bools['temp'] > 0):
            for item in names['temp']:
                dataEngines['system'].connectSource(item, self.parent, interval)
        if (bools['time'] > 0):
            dataEngines['time'].connectSource("Local", self.parent, 1000)
        if (bools['uptime'] > 0):
            dataEngines['system'].connectSource("system/uptime", self.parent, interval)


    def dataUpdated(self, sourceName, data, ptm):
        """function to update data"""
        adv = ptm['vars']['adv']
        systemDataEngine = ptm['dataengine']['system']
        formats = ptm['vars']['formats']
        interval = ptm['vars']['app']['interval']
        names = ptm['names']
        updatedData = {'name':None, 'type':None, 'value':None}
        if True:
        #try:
            if (sourceName == "cpu/system/TotalLoad"):
                updatedData['name'] = "cpu"
                updatedData['type'] = -1
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif ((sourceName[:7] == "cpu/cpu") and (sourceName.split('/')[2] == "TotalLoad")):
                updatedData['name'] = "cpu"
                updatedData['type'] = int(sourceName[7])
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif (sourceName == "cpu/system/AverageClock"):
                updatedData['name'] = "cpuclock"
                updatedData['type'] = -1
                value = round(data[QString(u'value')].toFloat()[0], 0)
                updatedData['value'] = value
            elif ((sourceName[:7] == "cpu/cpu") and (sourceName.split('/')[2] == "clock")):
                updatedData['name'] = "cpuclock"
                updatedData['type'] = int(sourceName[7])
                value = round(data[QString(u'value')].toFloat()[0], 0)
                updatedData['value'] = value
            elif (sourceName == "custom"):
                updatedData['name'] = "custom"
                value = str(data[QString(u'custom')].toUtf8()).decode("utf-8")
                updatedData['value'] = value
            elif (sourceName == "gpu"):
                updatedData['name'] = "gpu"
                value = round(data[QString(u'GPU')].toFloat()[0], 1)
                updatedData['value'] = "%4.1f" % (value)
            elif (sourceName == "gputemp"):
                updatedData['name'] = "gputemp"
                value = round(data[QString(u'GPUTemp')].toFloat()[0], 1)
                updatedData['value'] = "%4.1f" % (value)
            elif (sourceName.split('/')[0] == "partitions"):
                updatedData['name'] = "hdd"
                updatedData['type'] = '/' + '/'.join(sourceName.split('/')[1:-1])
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = "%5.1f" % (value)
            elif (sourceName == "hddtemp"):
                updatedData['name'] = "hddtemp"
                updatedData['value'] = {}
                for item in names['hddtemp']:
                    value = round(data[QString(item)].toFloat()[0], 1)
                    updatedData['value'][item] = "%4.1f" % (value)
            elif (sourceName == "mem/physical/application"):
                updatedData['name'] = "mem"
                updatedData['type'] = "app"
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif (sourceName == "mem/physical/free"):
                updatedData['name'] = "mem"
                updatedData['type'] = "free"
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif (sourceName == "mem/physical/used"):
                updatedData['name'] = "mem"
                updatedData['type'] = "used"
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif ((sourceName.split('/')[0] == "network") and (sourceName.split('/')[3] == "receiver")):
                updatedData['name'] = "net"
                updatedData['type'] = "down"
                value = round(data[QString(u'value')].toFloat()[0], 0)
                updatedData['value'] = value
                # update network device
                self.updateNetdev = self.updateNetdev + 1
                if (self.updateNetdev == 100):
                    self.updateNetdev = 0
                    if (adv['netdevBool'] > 0):
                        systemDataEngine.disconnectSource("network/interfaces/" + names['net'] + "/transmitter/data", self.parent)
                        systemDataEngine.disconnectSource("network/interfaces/" + names['net'] + "/receiver/data", self.parent)
                        names['net'] = self.parent.setNetdev()
                        systemDataEngine.connectSource("network/interfaces/" + names['net'] + "/transmitter/data", self.parent, interval)
                        systemDataEngine.connectSource("network/interfaces/" + names['net'] + "/receiver/data", self.parent, interval)
            elif ((sourceName.split('/')[0] == "network") and (sourceName.split('/')[3] == "transmitter")):
                updatedData['name'] = "net"
                updatedData['type'] = "up"
                value = round(data[QString(u'value')].toFloat()[0], 0)
                updatedData['value'] = value
            elif (sourceName == "player"):
                updatedData['name'] = "player"
                updatedData['value'] = {}
                if (adv['player'] == 0):
                    updatedData['value']['album'] = str(data[QString(u'amarok_album')].toUtf8()).decode("utf-8")
                    updatedData['value']['artist'] = str(data[QString(u'amarok_artist')].toUtf8()).decode("utf-8")
                    updatedData['value']['progress'] = str(data[QString(u'amarok_progress')].toUtf8()).decode("utf-8")
                    updatedData['value']['time'] = str(data[QString(u'amarok_duration')].toUtf8()).decode("utf-8")
                    updatedData['value']['title'] = str(data[QString(u'amarok_title')].toUtf8()).decode("utf-8")
                elif (adv['player'] == 1):
                    updatedData['value']['album'] = str(data[QString(u'mpd_album')].toUtf8()).decode("utf-8")
                    updatedData['value']['artist'] = str(data[QString(u'mpd_artist')].toUtf8()).decode("utf-8")
                    updatedData['value']['progress'] = str(data[QString(u'mpd_progress')].toUtf8()).decode("utf-8")
                    updatedData['value']['time'] = str(data[QString(u'mpd_duration')].toUtf8()).decode("utf-8")
                    updatedData['value']['title'] = str(data[QString(u'mpd_title')].toUtf8()).decode("utf-8")
                elif (adv['player'] == 2):
                    updatedData['value']['album'] = str(data[QString(u'qmmp_album')].toUtf8()).decode("utf-8")
                    updatedData['value']['artist'] = str(data[QString(u'qmmp_artist')].toUtf8()).decode("utf-8")
                    updatedData['value']['progress'] = str(data[QString(u'qmmp_progress')].toUtf8()).decode("utf-8")
                    updatedData['value']['time'] = str(data[QString(u'qmmp_duration')].toUtf8()).decode("utf-8")
                    updatedData['value']['title'] = str(data[QString(u'qmmp_title')].toUtf8()).decode("utf-8")
            elif (sourceName == "mem/swap/free"):
                updatedData['name'] = "swap"
                updatedData['type'] = "free"
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif (sourceName == "mem/swap/used"):
                updatedData['name'] = "swap"
                updatedData['type'] = "used"
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = value
            elif (sourceName.split('/')[0] == "lmsensors"):
                updatedData['name'] = "temp"
                updatedData['type'] = sourceName
                value = round(data[QString(u'value')].toFloat()[0], 1)
                updatedData['value'] = "%4.1f" % (value)
            elif (sourceName == "Local"):
                updatedData['name'] = "time"
                if (formats['time'].split('$time')[0] != formats['time']):
                    value = str(data[QString(u'DateTime')].toString(Qt.TextDate).toUtf8()).decode("utf-8")
                elif (formats['time'].split('$isotime')[0] != formats['time']):
                    value = str(data[QString(u'DateTime')].toString(Qt.ISODate).toUtf8()).decode("utf-8")
                elif (formats['time'].split('$shorttime')[0] != formats['time']):
                    value = str(data[QString(u'DateTime')].toString(Qt.SystemLocaleShortDate).toUtf8()).decode("utf-8")
                elif (formats['time'].split('$longtime')[0] != formats['time']):
                    value = str(data[QString(u'DateTime')].toString(Qt.SystemLocaleLongDate).toUtf8()).decode("utf-8")
                elif (formats['time'].split('$custom')[0] != formats['time']):
                    rawDate = data[QString(u'DateTime')]
                    value = adv['customTime']
                    for letters in timeLetters:
                        if (value.split('$' + letters)[0] != value):
                            value = value.split('$' + letters)[0] + str(rawDate.toString(letters).toUtf8()).decode("utf-8") + value.split('$' + letters)[1]
                updatedData['value'] = value
            elif (sourceName == "system/uptime"):
                updatedData['name'] = "uptime"
                value = datetime.timedelta(0, int(round(float(data[QString(u'value')]), 1)))
                days = value.days
                hours = int(value.seconds / 60 / 60)
                minutes = int(value.seconds / 60 % 60)
                if (formats['uptime'].split('$uptime')[0] != formats['uptime']):
                    updatedData['value'] = "%3id%2ih%2im" % (days, hours, minutes)
                elif (formats['uptime'].split('$custom')[0] != formats['uptime']):
                    updatedData['value'] = adv['customUptime']
                    if (updatedData['value'].split('$dd')[0] != updatedData['value']):
                        updatedData['value'] = "%s%03i%s" % (updatedData['value'].split('$dd')[0], days, updatedData['value'].split('$dd')[1])
                    if (updatedData['value'].split('$d')[0] != updatedData['value']):
                        updatedData['value'] = "%s%3i%s" % (updatedData['value'].split('$d')[0], days, updatedData['value'].split('$d')[1])
                    if (updatedData['value'].split('$hh')[0] != updatedData['value']):
                        updatedData['value'] = "%s%02i%s" % (updatedData['value'].split('$hh')[0], hours, updatedData['value'].split('$hh')[1])
                    if (updatedData['value'].split('$h')[0] != updatedData['value']):
                        updatedData['value'] = "%s%2i%s" % (updatedData['value'].split('$h')[0], hours, updatedData['value'].split('$h')[1])
                    if (updatedData['value'].split('$mm')[0] != updatedData['value']):
                        updatedData['value'] = "%s%02i%s" % (updatedData['value'].split('$mm')[0], minutes, updatedData['value'].split('$mm')[1])
                    if (updatedData['value'].split('$m')[0] != updatedData['value']):
                        updatedData['value'] = "%s%2i%s" % (updatedData['value'].split('$m')[0], minutes, updatedData['value'].split('$m')[1])
        #except:
            #pass

        return updatedData
