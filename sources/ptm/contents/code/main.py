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
from PyKDE4.kio import *
from PyKDE4 import plasmascript
from PyKDE4.plasma import Plasma
import commands, os

import advanced
import appearance
import configdef
import dataengine
import deconfig
import ptmnotify
import reinit
import tooltip
import tooltipconfig
import widget



def getTemp(temp, unit):
    """function to return temperature"""
    if (unit == "Celsius"):
        pass
    elif (unit == "Fahrenheit"):
        temp = str(round(((float(temp) * 9.0 / 5.0) + 32.0), 1))
    elif (unit == "Kelvin"):
        temp = str(round((float(temp) + 273.15), 1))
    elif (unit == "Reaumur"):
        temp = str(round((float(temp) * 0.8), 1))
    elif (unit == "cm^-1"):
        kelvin = round((float(temp) + 273.15), 1)
        temp = str(round((kelvin * 0.695), 1))
    elif (unit == "kJ/mol"):
        kelvin = round((float(temp) + 273.15), 1)
        temp = str(round((kelvin * 8.31), 1))
    elif (unit == "kcal/mol"):
        kelvin = round((float(temp) + 273.15), 1)
        temp = str(round((kelvin * 1.98), 1))
    return temp



class CustomPlasmaLabel(Plasma.Label):
    """new Label with defined clicked() event"""
    def __init__(self, applet, name, enablePopup):
        """class definition"""
        Plasma.Label.__init__(self, applet)
        self.enablePopup = enablePopup
        self.name = name
        self.notify = ptmnotify.PTMNotify(self)


    def mousePressEvent(self, event):
        """mouse click event"""
        if (self.enablePopup > 0):
            if (event.button() == Qt.LeftButton):
                self.notify.init(self.name)



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        # debug
        environment = QProcessEnvironment.systemEnvironment()
        debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
        if (debugEnv == QString("yes")):
            self.debug = True
        else:
            self.debug = False
        # main
        plasmascript.Applet.__init__(self, parent)


    # initialization
    def init(self):
        """function to initializate widget"""
        if self.debug: qDebug("[PTM] [main.py] [init]")
        self.setupVar()

        self.dataengine = dataengine.DataEngine(self)
        self.reinit = reinit.Reinit(self, self.ptm['defaults'])
        self.tooltipAgent = tooltip.Tooltip(self)

        self.timer = QTimer()
        QObject.connect(self.timer, SIGNAL("timeout()"), self.startPolling)

        self.theme = Plasma.Svg(self)
        self.initTooltip()
        self.reInit()
        self.applet.setLayout(self.ptm['layout'])

        self.setHasConfigurationInterface(True)

        # actions
        self.createActions()


    # context menu
    def createActions(self):
        """function to create actions"""
        if self.debug: qDebug("[PTM] [main.py] [createActions]")
        self.ptmActions = {}
        self.ptmActions['ksysguard'] = QAction(i18n("Run ksysguard"), self)
        QObject.connect(self.ptmActions['ksysguard'], SIGNAL("triggered(bool)"), self.runKsysguard)
        self.ptmActions['readme'] = QAction(i18n("Show README"), self)
        QObject.connect(self.ptmActions['readme'], SIGNAL("triggered(bool)"), self.showReadme)
        self.ptmActions['update'] = QAction(i18n("Update text"), self)
        QObject.connect(self.ptmActions['update'], SIGNAL("triggered(bool)"),
                        self.ptm['dataengine']['ext'], SLOT("updateAllSources()"))
        QObject.connect(self.ptmActions['update'], SIGNAL("triggered(bool)"),
                        self.ptm['dataengine']['system'], SLOT("updateAllSources()"))
        QObject.connect(self.ptmActions['update'], SIGNAL("triggered(bool)"),
                        self.ptm['dataengine']['time'], SLOT("updateAllSources()"))
        QObject.connect(self.ptmActions['update'], SIGNAL("triggered(bool)"), self.updateNetdev)


    def contextualActions(self):
        """function to create context menu"""
        if self.debug: qDebug("[PTM] [main.py] [contextualActions]")
        contextMenu = []
        contextMenu.append(self.ptmActions['ksysguard'])
        contextMenu.append(self.ptmActions['readme'])
        contextMenu.append(self.ptmActions['update'])
        return contextMenu


    def runKsysguard(self, event):
        """function to run ksysguard"""
        if self.debug: qDebug("[PTM] [main.py] [runKsysguard]")
        if self.debug: qDebug("[PTM] [main.py] [runKsysguard] : Run cmd " + "'ksysguard &'")
        os.system("ksysguard &")


    def showReadme(self):
        """function to show readme file"""
        if self.debug: qDebug("[PTM] [main.py] [showReadme]")
        kdehome = unicode(KGlobal.dirs().localkdedir())
        if (os.path.exists("/usr/share/pytextmonitor/")):
            dirPath = "/usr/share/pytextmonitor/"
        elif (os.path.exists(kdehome + "/share/pytextmonitor/")):
            dirPath = kdehome + "/share/pytextmonitor/"
        else:
            return
        locale = str(QLocale.system().name())
        if (os.path.exists(dirPath + locale + ".html")):
            filePath = dirPath + locale + ".html"
        else:
            locale = locale.split('_')[0]
            if (os.path.exists(dirPath + locale + ".html")):
                filePath = dirPath + locale + ".html"
            elif (os.path.exists(dirPath + "en.html")):
                filePath = dirPath + "en.html"
            else:
                return
        if self.debug: qDebug("[PTM] [main.py] [showReadme] : Run cmd " + "'kioclient exec " + str(filePath) + " &'")
        os.system("kioclient exec " + str(filePath) + " &")


    # internal functions
    def addDiskDevice(self, sourceName):
        if self.debug: qDebug("[PTM] [main.py] [addDiskDevice]")
        diskRegexp = QRegExp("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)")
        if (diskRegexp.indexIn(sourceName) > -1):
            if self.debug: qDebug("[PTM] [main.py] [addDiskDevice] : Add device '%s'" %(str(sourceName).split('/')[1]))
            self.ptm['defaults']['disk'].append('/'.join(str(sourceName).split('/')[0:2]))


    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        if self.debug: qDebug("[PTM] [main.py] [createConfigurationInterface]")
        configpage = {}
        configpage['advanced'] = advanced.AdvancedWindow(self)
        configpage['appearance'] = appearance.AppearanceWindow(self)
        configpage['dataengine'] = deconfig.DEConfigWindow(self)
        configpage['tooltip'] = tooltipconfig.TooltipWindow(self)
        configpage['widget'] = widget.WidgetWindow(self)

        self.confdef = configdef.ConfigDefinition(self, configpage, self.ptm['defaults'])
        self.confdef.createConfigurationInterface(parent)


    def initTooltip(self):
        """function to create tooltip"""
        if self.debug: qDebug("[PTM] [main.py] [initTooltip]")
        self.tooltip = Plasma.ToolTipContent()
        self.tooltip.setMainText("PyTextMonitor")
        self.tooltip.setSubText('')
        Plasma.ToolTipManager.self().registerWidget(self.applet)
        # graphical tooltip
        self.tooltipScene = QGraphicsScene()
        self.tooltipView = QGraphicsView(self.tooltipScene)
        self.tooltipView.setStyleSheet("background: transparent")
        self.tooltipView.setContentsMargins(0, 0, 0, 0)
        self.tooltipView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tooltipView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        # show tooltip
        Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)


    def setupVar(self):
        """function to setup variables"""
        if self.debug: qDebug("[PTM] [main.py] [setupVar]")
        # variables
        self.ptm = {}
        # dataengines
        self.ptm['dataengine'] = {'ext':None, 'system':None, 'time':None}
        # defaults
        self.ptm['defaults'] = {}
        self.ptm['defaults']['confBool'] = {'bat':'batBool', 'cpu':'cpuBool',
            'cpuclock':'cpuclockBool', 'custom':'customBool', 'desktop':'desktopBool',
            'disk':'diskBool', 'gpu':'gpuBool', 'gputemp':'gputempBool', 'hdd':'hddBool',
            'hddtemp':'hddtempBool', 'mem':'memBool', 'net':'netBool', 'pkg':'pkgBool',
            'player':'playerBool', 'ps':'psBool', 'swap':'swapBool', 'temp':'tempBool',
            'uptime':'uptimeBool', 'time':'timeBool'}
        self.ptm['defaults']['confColor'] = {'cpu':'cpu_color', 'cpuclock':'cpuclock_color',
            'down':'down_color', 'mem':'mem_color', 'swap':'swap_color', 'up':'up_color'}
        self.ptm['defaults']['confFormat'] = {'bat':'batFormat', 'cpu':'cpuFormat',
            'cpuclock':'cpuclockFormat', 'custom':'customFormat', 'desktop':'desktopFormat',
            'disk':'diskFormat', 'gpu':'gpuFormat', 'gputemp':'gputempFormat', 'hdd':'hddFormat',
            'hddtemp':'hddtempFormat', 'mem':'memFormat', 'net':'netFormat',
            'pkg':'pkgFormat', 'player':'playerFormat', 'ps':'psFormat', 'swap':'swapFormat',
            'temp':'tempFormat', 'time':'timeFormat', 'uptime':'uptimeFormat'}
        self.ptm['defaults']['bool'] = {'bat':0, 'cpu':2, 'cpuclock':0, 'custom':0,
            'desktop':0, 'disk':0, 'gpu':0, 'gputemp':0, 'hdd':0, 'hddtemp':0, 'mem':2,
            'net':2, 'pkg':0, 'player':0, 'ps':0, 'swap':2, 'temp':0, 'time':0, 'uptime':0}
        self.ptm['defaults']['format'] = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]',
            'cpuclock':'[mhz: $cpucl]', 'custom':'[$custom]', 'desktop':'[$number/$total: $name]',
            'disk':'[disk: $hddr0/$hddw0 KB/s]', 'gpu':'[gpu: $gpu%]',
            'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: $hdd0%]',
            'hddtemp':'[hdd temp: $hddtemp0&deg;C]', 'mem':'[mem: $mem%]',
            'net':'[$netdev: $down/$upKB/s]', 'pkg':'[upgrade: $pkgcount0]',
            'player':'[$artist - $title]', 'ps':'[proc: $pscount]', 'swap':'[swap: $swap%]',
            'temp':'[temp: $temp0&deg;C]', 'time':'[$time]', 'uptime':'[uptime: $uptime]'}
        self.ptm['defaults']['order'] = {'1':'cpu', '2':'temp', '3':'mem', '4':'swap', '5':'net',
            '6':'bat', '7':'cpuclock', '8':'uptime', '9':'gpu', 'a':'gputemp', 'b':'hdd',
            'c':'hddtemp', 'd':'player', 'e':'time', 'f':'custom', 'g':'ps', 'h':'pkg', 'i':'disk',
            'j':'desktop'}
        # disk devices for r/w speed
        self.ptm['defaults']['disk'] = []
        # labels
        self.ptm['labels'] = {}
        # names
        self.ptm['names'] = {}
        self.ptm['names']['disk'] = []
        self.ptm['names']['hdd'] = []
        self.ptm['names']['hddtemp'] = []
        self.ptm['names']['net'] = ""
        self.ptm['names']['temp'] = []
        # tooltips
        self.ptm['tooltip'] = {}
        self.ptm['tooltip']['bounds'] = {'cpu':100.0, 'cpuclock':4000.0, 'mem':16000.0,
            'swap':16000, 'down':10000.0, 'up':10000.0}
        self.ptm['tooltip']['values'] = {'cpu':[0.0, 0.0], 'cpuclock':[0, 0], 'mem':[0, 0],
            'swap':[0, 0], 'down':[0, 0], 'up':[0, 0]}
        # values
        self.ptm['values'] = {}
        self.ptm['values']['cpu'] = {-1:0.0}
        self.ptm['values']['cpuclock'] = {-1:0.0}
        numCores = int(commands.getoutput("grep -c '^processor' /proc/cpuinfo"))
        if self.debug: qDebug("[PTM] [main.py] [setupVar] : Number of cores '%s'" %(numCores))
        for i in range(numCores):
            self.ptm['values']['cpu'][i] = 0.0
            self.ptm['values']['cpuclock'][i] = 0.0
        self.ptm['values']['desktop'] = {'names':[], 'current':1}
        self.ptm['values']['disk-r'] = {}
        self.ptm['values']['disk-w'] = {}
        self.ptm['values']['hdd'] = {}
        self.ptm['values']['hddfreemb'] = {}
        self.ptm['values']['hddmb'] = {}
        self.ptm['values']['hddtemp'] = {}
        self.ptm['values']['mem'] = {'app':0.0, 'used':0.0, 'free':1.0}
        self.ptm['values']['net'] = {"up":0.0, "down":0.0}
        self.ptm['values']['swap'] = {'used':0.0, 'free':1.0}
        self.ptm['values']['temp'] = {}
        # variables
        self.ptm['vars'] = {}
        self.ptm['vars']['adv'] = {}
        self.ptm['vars']['app'] = {}
        self.ptm['vars']['bools'] = {}
        self.ptm['vars']['formats'] = {}
        self.ptm['vars']['tooltip'] = {}


    def showConfigurationInterface(self):
        """function to show configuration window"""
        if self.debug: qDebug("[PTM] [main.py] [showConfigurationInterface]")
        plasmascript.Applet.showConfigurationInterface(self)


    def startPolling(self):
        """function to update"""
        if self.debug: qDebug("[PTM] [main.py] [startPolling]")
        try:
            self.updateLabel()
            self.tooltip.setSubText('')
            if self.debug: qDebug("[PTM] [main.py] [startPolling] : Update without errors")
        except Exception as strerror:
            self.tooltip.setSubText(str(strerror))
            if self.debug: qDebug("[PTM] [main.py] [startPolling] : There is some errors '%s'" %(str(strerror)))
            return


    def updateLabel(self):
        """function to update label"""
        if self.debug: qDebug("[PTM] [main.py] [updateLabel]")
        if (self.ptm['vars']['bools']['bat'] > 0):
            self.batText()
        if (self.ptm['vars']['bools']['cpu'] > 0):
            self.cpuText()
        if (self.ptm['vars']['bools']['cpuclock'] > 0):
            self.cpuclockText()
        if (self.ptm['vars']['bools']['desktop'] > 0):
            self.desktopText()
        if (self.ptm['vars']['bools']['disk'] > 0):
            self.diskText()
        if (self.ptm['vars']['bools']['hdd'] > 0):
            self.hddText()
        if (self.ptm['vars']['bools']['hddtemp'] > 0):
            self.hddtempText()
        if (self.ptm['vars']['bools']['mem'] > 0):
            self.memText()
        if (self.ptm['vars']['bools']['net'] > 0):
            self.netText()
        if (self.ptm['vars']['bools']['swap'] > 0):
            self.swapText()
        if (self.ptm['vars']['bools']['temp'] > 0):
            self.tempText()
        self.updateTooltip()


    def updateNetdev(self):
        """function to update netdev"""
        if self.debug: qDebug("[PTM] [main.py] [updateNetdev]")
        self.ptm['names']['net'] = self.setNetdev()


    def updateTooltip(self):
        """function to update tooltip"""
        if self.debug: qDebug("[PTM] [main.py] [updateTooltip]")
        self.tooltipView.resize(100.0*(len(self.ptm['vars']['tooltip']['required']) - self.ptm['vars']['tooltip']['required'].count('up')), 100.0)
        self.tooltipAgent.createGraphic(self.ptm['vars']['tooltip'], self.ptm['tooltip'], self.tooltipScene)
        self.tooltip.setImage(QPixmap.grabWidget(self.tooltipView))
        Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)


    # update functions
    # update dataengines
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        if self.debug: qDebug("[PTM] [main.py] [dataUpdated]")
        if self.debug: qDebug("[PTM] [main.py] [dataUpdated] : Run function with source '%s'" %(sourceName))
        if self.debug: qDebug("[PTM] [main.py] [dataUpdated] : Run function with data '%s'" %(data))
        updatedData = self.dataengine.dataUpdated(str(sourceName), data, self.ptm)
        if self.debug: qDebug("[PTM] [main.py] [dataUpdated] : Received data '%s'" %(updatedData))
        if (updatedData['value'] == None):
            return
        # update values where is needed
        if (updatedData['type'] != None):
            self.ptm['values'][updatedData['name']][updatedData['type']] = updatedData['value']
        elif (updatedData['name'] in ['custom', 'gpu', 'gputemp', 'pkg', 'player', 'ps', 'time', 'uptime']):
            text = self.textPrepare(updatedData['name'], updatedData['value'])
            self.setText(updatedData['name'], text)
        else:
            self.ptm['values'][updatedData['name']] = updatedData['value']
        # update tooltips
        if ((updatedData['name'] in ['cpu', 'cpuclock', 'mem', 'swap', 'net']) and (self.ptm['vars']['bools'][updatedData['name']] == 2)):
            if (updatedData['name'] == "net"):
                if (len(self.ptm['tooltip']['values'][updatedData['type']]) > self.ptm['vars']['tooltip']['num']):
                    self.ptm['tooltip']['values'][updatedData['type']] = self.ptm['tooltip']['values'][updatedData['type']][1:]
            else:
                if (len(self.ptm['tooltip']['values'][updatedData['name']]) > self.ptm['vars']['tooltip']['num']):
                    self.ptm['tooltip']['values'][updatedData['name']] = self.ptm['tooltip']['values'][updatedData['name']][1:]
            if ((updatedData['name'] in ['cpu', 'cpuclock']) and (updatedData['type'] == -1)):
                self.ptm['tooltip']['values'][updatedData['name']].append(updatedData['value'])
            elif ((updatedData['name'] == "mem") and (updatedData['type'] == "app")):
                self.ptm['tooltip']['values'][updatedData['name']].append(updatedData['value'])
            elif ((updatedData['name'] == "mem") and (updatedData['type'] == "used")):
                self.ptm['tooltip']['bounds']['mem'] = self.ptm['values']['mem']['free'] + self.ptm['values']['mem']['used']
            elif ((updatedData['name'] == "swap") and (updatedData['type'] == "used")):
                self.ptm['tooltip']['values'][updatedData['name']].append(updatedData['value'])
                self.ptm['tooltip']['bounds']['swap'] = self.ptm['values']['swap']['free'] + self.ptm['values']['swap']['used']
            elif (updatedData['name'] == "net"):
                self.ptm['tooltip']['values'][updatedData['type']].append(updatedData['value'])
        self.update()


    # update labels
    def batText(self):
        """function to set battery text"""
        if self.debug: qDebug("[PTM] [main.py] [batText]")
        line = self.ptm['vars']['formats']['bat']
        if (line.split('$bat')[0] != line):
            try:
                with open (self.ptm['vars']['adv']['batDev'], 'r') as batFile:
                    bat = batFile.readline()[:-1]
            except:
                bat = "off"
            bat = "%3s" % (bat)
            line = line.split('$bat')[0] + bat + line.split('$bat')[1]
        if (line.split('$ac')[0] != line):
            try:
                with open (self.ptm['vars']['adv']['acDev'], 'r') as batFile:
                    bat = batFile.readline()[:-1]
                if (bat == "1"):
                    bat = self.ptm['vars']['adv']['acOnline']
                else:
                    bat = self.ptm['vars']['adv']['acOffline']
            except:
                bat = "N\\A"
            line = line.split('$ac')[0] + bat + line.split('$ac')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("bat", text)


    def cpuText(self):
        """function to set cpu text"""
        if self.debug: qDebug("[PTM] [main.py] [cpuText]")
        line = self.ptm['vars']['formats']['cpu']
        keys = self.ptm['values']['cpu'].keys()
        keys.sort()
        keys.reverse()
        for core in keys[:-1]:
            if (line.split('$cpu' + str(core))[0] != line):
                value = "%5.1f" % (self.ptm['values']['cpu'][core])
                line = line.split('$cpu' + str(core))[0] + value + line.split('$cpu' + str(core))[1]
        if (line.split('$cpu')[0] != line):
            value = "%5.1f" % (self.ptm['values']['cpu'][-1])
            line = line.split('$cpu')[0] + value + line.split('$cpu')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("cpu", text)


    def cpuclockText(self):
        """function to set cpu clock text"""
        if self.debug: qDebug("[PTM] [main.py] [cpuclockText]")
        line = self.ptm['vars']['formats']['cpuclock']
        keys = self.ptm['values']['cpuclock'].keys()
        keys.sort()
        keys.reverse()
        for core in keys[:-1]:
            if (line.split('$cpucl' + str(core))[0] != line):
                value = "%4.0f" % (self.ptm['values']['cpuclock'][core])
                line = line.split('$cpucl' + str(core))[0] + value + line.split('$cpucl' + str(core))[1]
        if (line.split('$cpucl')[0] != line):
            value = "%4.0f" % (self.ptm['values']['cpuclock'][-1])
            line = line.split('$cpucl')[0] + value + line.split('$cpucl')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("cpuclock", text)


    def desktopText(self):
        """function to update desktop text"""
        if self.debug: qDebug("[PTM] [main.py] [desktopText]")
        line = self.ptm['vars']['formats']['desktop']
        if (line.split('$name')[0] != line):
            line = line.split('$name')[0] + \
                   self.ptm['values']['desktop']['names'][self.ptm['values']['desktop']['current']-1] + \
                   line.split('$name')[1]
        if (line.split('$number')[0] != line):
            line = line.split('$number')[0] + \
                   str(self.ptm['values']['desktop']['current']) + \
                   line.split('$number')[1]
        if (line.split('$total')[0] != line):
            line = line.split('$total')[0] + \
                   str(len(self.ptm['values']['desktop']['names'])) + \
                   line.split('$total')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("desktop", text)


    def diskText(self):
        """function to update hdd speed text"""
        if self.debug: qDebug("[PTM] [main.py] [diskText]")
        line = self.ptm['vars']['formats']['disk']
        devices = range(len(self.ptm['names']['disk']))
        devices.reverse()
        for i in devices:
            if (line.split('$hddr' + str(i))[0] != line):
                hdd = "%4i" % (self.ptm['values']['disk-r'][self.ptm['names']['disk'][i]])
                line = line.split('$hddr' + str(i))[0] + hdd + line.split('$hddr' + str(i))[1]
            if (line.split('$hddw' + str(i))[0] != line):
                hdd = "%4i" % (self.ptm['values']['disk-w'][self.ptm['names']['disk'][i]])
                line = line.split('$hddw' + str(i))[0] + hdd + line.split('$hddw' + str(i))[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("disk", text)


    def hddText(self):
        """function to set hdd text"""
        if self.debug: qDebug("[PTM] [main.py] [hddText]")
        line = self.ptm['vars']['formats']['hdd']
        devices = range(len(self.ptm['names']['hdd']))
        devices.reverse()
        for i in devices:
            if (line.split('$hdd' + str(i))[0] != line):
                hdd = "%4.1f" % (self.ptm['values']['hdd'][self.ptm['names']['hdd'][i]])
                line = line.split('$hdd' + str(i))[0] + hdd + line.split('$hdd' + str(i))[1]
            if (line.split('$hddmb' + str(i))[0] != line):
                hdd = "%i" % (self.ptm['values']['hddmb'][self.ptm['names']['hdd'][i]] / 1024.0)
                line = line.split('$hddmb' + str(i))[0] + hdd + line.split('$hddmb' + str(i))[1]
            if (line.split('$hddgb' + str(i))[0] != line):
                hdd = "%5.1f" % (self.ptm['values']['hddmb'][self.ptm['names']['hdd'][i]] / (1024.0 * 1024.0))
                line = line.split('$hddgb' + str(i))[0] + hdd + line.split('$hddgb' + str(i))[1]
            if (line.split('$hddtotmb' + str(i))[0] != line):
                hdd = "%i" % ((self.ptm['values']['hddfreemb'][self.ptm['names']['hdd'][i]] +\
                               self.ptm['values']['hddmb'][self.ptm['names']['hdd'][i]]) / 1024.0)
                line = line.split('$hddtotmb' + str(i))[0] + hdd + line.split('$hddtotmb' + str(i))[1]
            if (line.split('$hddtotgb' + str(i))[0] != line):
                hdd = "%5.1f" % ((self.ptm['values']['hddfreemb'][self.ptm['names']['hdd'][i]] +\
                               self.ptm['values']['hddmb'][self.ptm['names']['hdd'][i]]) / (1024.0 * 1024.0))
                line = line.split('$hddtotgb' + str(i))[0] + hdd + line.split('$hddtotgb' + str(i))[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("hdd", text)


    def hddtempText(self):
        """function to set hddtemp text"""
        if self.debug: qDebug("[PTM] [main.py] [hddtempText]")
        line = self.ptm['vars']['formats']['hddtemp']
        devices = range(len(self.ptm['names']['hddtemp']))
        devices.reverse()
        for i in devices:
            if (line.split('$hddtemp' + str(i))[0] != line):
                line = line.split('$hddtemp' + str(i))[0] +\
                    getTemp(str(self.ptm['values']['hddtemp'][self.ptm['names']['hddtemp'][i]]), self.ptm['vars']['adv']['tempUnits']) +\
                    line.split('$hddtemp' + str(i))[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("hddtemp", text)


    def memText(self):
        """function to set mem text"""
        if self.debug: qDebug("[PTM] [main.py] [memText]")
        line = self.ptm['vars']['formats']['mem']
        if (line.split('$memtotgb')[0] != line):
            mem = "%4.1f" %((self.ptm['values']['mem']['free'] + self.ptm['values']['mem']['used']) / (1024.0 * 1024.0))
            line = line.split('$memtotgb')[0] + mem + line.split('$memtotgb')[1]
        if (line.split('$memtotmb')[0] != line):
            mem = "%i" %((self.ptm['values']['mem']['free'] + self.ptm['values']['mem']['used']) / 1024.0)
            line = line.split('$memtotmb')[0] + mem + line.split('$memtotmb')[1]
        if (line.split('$memgb')[0] != line):
            mem = "%4.1f" % (self.ptm['values']['mem']['app'] / (1024.0 * 1024.0))
            line = line.split('$memgb')[0] + mem + line.split('$memgb')[1]
        if (line.split('$memmb')[0] != line):
            mem = "%i" % (self.ptm['values']['mem']['app'] / 1024.0)
            line = line.split('$memmb')[0] + mem + line.split('$memmb')[1]
        if (line.split('$mem')[0] != line):
            try:
                mem = 100 * self.ptm['values']['mem']['app'] / (self.ptm['values']['mem']['free'] + self.ptm['values']['mem']['used'])
                mem = "%5.1f" % (round(mem, 1))
            except:
                mem = "  N\\A"
            line = line.split('$mem')[0] + mem + line.split('$mem')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("mem", text)


    def netText(self):
        """function to set network text"""
        if self.debug: qDebug("[PTM] [main.py] [netText]")
        line = self.ptm['vars']['formats']['net']
        if (line.split('$netdev')[0] != 0):
            line = line.split('$netdev')[0] + self.ptm['names']['net'] + line.split('$netdev')[1]
        if (line.split('$down')[0] != line):
            value = "%4.0f" %(self.ptm['values']['net']['down'])
            line = line.split('$down')[0] + value + line.split('$down')[1]
        if (line.split('$up')[0] != line):
            value = "%4.0f" %(self.ptm['values']['net']['up'])
            line = line.split('$up')[0] + value + line.split('$up')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("net", text)


    def swapText(self):
        """function to set swap text"""
        if self.debug: qDebug("[PTM] [main.py] [swapText]")
        line = self.ptm['vars']['formats']['swap']
        if (line.split('$swaptotgb')[0] != line):
            mem = "%4.1f" % ((self.ptm['values']['swap']['free'] + self.ptm['values']['swap']['used']) / (1024.0 * 1024.0))
            line = line.split('$swaptotgb')[0] + mem + line.split('$swaptotgb')[1]
        if (line.split('$swaptotmb')[0] != line):
            mem = "%i" % ((self.ptm['values']['swap']['free'] + self.ptm['values']['swap']['used']) / 1024.0)
            line = line.split('$swaptotmb')[0] + mem + line.split('$swaptotmb')[1]
        if (line.split('$swapgb')[0] != line):
            mem = "%4.1f" % (self.ptm['values']['swap']['used'] / (1024.0 * 1024.0))
            line = line.split('$swapgb')[0] + mem + line.split('$swapgb')[1]
        if (line.split('$swapmb')[0] != line):
            mem = "%i" % (self.ptm['values']['swap']['used'] / 1024.0)
            line = line.split('$swapmb')[0] + mem + line.split('$swapmb')[1]
        if (line.split('$swap')[0] != line):
            try:
                mem = 100 * self.ptm['values']['swap']['used'] / (self.ptm['values']['swap']['free'] + self.ptm['values']['swap']['used'])
                mem = "%5.1f" % (round(mem, 1))
            except:
                mem = "  N\\A"
            line = line.split('$swap')[0] + mem + line.split('$swap')[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("swap", text)


    def tempText(self):
        """function to set temperature text"""
        if self.debug: qDebug("[PTM] [main.py] [tempText]")
        line = self.ptm['vars']['formats']['temp']
        devices = range(len(self.ptm['names']['temp']))
        devices.reverse()
        for i in devices:
            if (line.split('$temp' + str(i))[0] != line):
                line = line.split('$temp' + str(i))[0] +\
                    getTemp(str(self.ptm['values']['temp'][self.ptm['names']['temp'][i]]), self.ptm['vars']['adv']['tempUnits']) +\
                    line.split('$temp' + str(i))[1]
        text = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        self.setText("temp", text)


    # external functions
    def addLabel(self, name=None, text=None, add=True, enablePopup=2):
        """function to add new label"""
        if self.debug: qDebug("[PTM] [main.py] [addLabel]")
        if self.debug: qDebug("[PTM] [main.py] [addLabel] : Run function with name '%s'" %(name))
        if self.debug: qDebug("[PTM] [main.py] [addLabel] : Run function with text '%s'" %(text))
        if self.debug: qDebug("[PTM] [main.py] [addLabel] : Run function with add '%s'" %(add))
        if self.debug: qDebug("[PTM] [main.py] [addLabel] : Run function with popup '%s'" %(enablePopup))
        if (add):
            self.ptm['labels'][name] = CustomPlasmaLabel(self.applet, name, enablePopup)
            self.ptm['layout'].addItem(self.ptm['labels'][name])
            self.setText(name, text)
        else:
            self.setText(name, '')
            self.ptm['layout'].removeItem(self.ptm['labels'][name])


    def applySettings(self, name=None, ptm=None):
        """function to read settings"""
        if self.debug: qDebug("[PTM] [main.py] [applySettings]")
        if self.debug: qDebug("[PTM] [main.py] [applySettings] : Run function with name '%s'" %(name))
        if self.debug: qDebug("[PTM] [main.py] [applySettings] : Run function with settings '%s'" %(ptm))
        self.ptm[name] = ptm
        if (name == "names"):
            for item in ['hddtemp', 'temp']:
                for value in self.ptm['names'][item]:
                    self.ptm['values'][item][value] = 0.0
            for value in self.ptm['names']['hdd']:
                self.ptm['values']['hdd'][value] = 0.0
                self.ptm['values']['hddfreemb'][value] = 0.0
                self.ptm['values']['hddmb'][value] = 0.0
            for value in self.ptm['names']['disk']:
                self.ptm['values']['disk-r'][value] = 0.0
                self.ptm['values']['disk-w'][value] = 0.0



    def connectToEngine(self):
        """function to connect to dataengines"""
        if self.debug: qDebug("[PTM] [main.py] [connectToEngine]")
        self.ptm['dataengine']['ext'] = self.dataEngine("ext-sysmon")
        self.ptm['dataengine']['system'] = self.dataEngine("systemmonitor")
        self.ptm['dataengine']['time'] = self.dataEngine("time")
        QObject.connect(self.ptm['dataengine']['system'], SIGNAL("sourceAdded(QString)"), self.addDiskDevice)
        self.dataengine.connectToEngine(self.ptm['vars']['bools'], self.ptm['dataengine'],
            self.ptm['vars']['app']['interval'], self.ptm['names'])


    def createLayout(self, verticalLayout=0, leftStretch=2):
        """function to create layout"""
        if self.debug: qDebug("[PTM] [main.py] [createLayout]")
        if self.debug: qDebug("[PTM] [main.py] [createLayout] : Run function with vertical layout '%s'" %(verticalLayout))
        if self.debug: qDebug("[PTM] [main.py] [createLayout] : Run function with left stretch '%s'" %(leftStretch))
        if (verticalLayout == 0):
            self.ptm['layout'] = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        else:
            self.ptm['layout'] = QGraphicsLinearLayout(Qt.Vertical, self.applet)
        self.ptm['layout'].setContentsMargins(1, 1, 1, 1)
        if (leftStretch == 2):
            self.ptm['layout'].addStretch(1)


    def disconnectFromSource(self):
        """function to disconnect from sources"""
        if self.debug: qDebug("[PTM] [main.py] [disconnectFromSource]")
        for label in self.ptm['defaults']['format'].keys():
            if (self.ptm['vars']['bools'][label] > 0):
                self.addLabel(label, None, False)
                keys = {'cpu':self.ptm['values']['cpu'].keys(), 'cpuclock':self.ptm['values']['cpuclock'].keys(),
                    'disk':self.ptm['values']['disk-r'].keys(), 'hdd':self.ptm['values']['hdd'].keys(),
                    'net':self.ptm['names']['net'], 'temp':self.ptm['values']['temp'].keys()}
                self.dataengine.disconnectFromSource(self.ptm['dataengine'], keys, label)


    def reInit(self):
        """function to run reinit"""
        if self.debug: qDebug("[PTM] [main.py] [reInit]")
        self.reinit.reinit()
        # add stretch
        if (self.ptm['vars']['adv']['rightStretch'] == 2):
            self.ptm['layout'].addStretch(1)
        self.updateNetdev()
        self.resize(10, 10)

        # create dataengines
        self.thread().wait(60000)
        self.connectToEngine()

        self.timer.setInterval(self.ptm['vars']['app']['interval'])
        self.timer.start()
        self.startPolling()


    def setTransparentBackground(self, hasBackground=2):
        """function to set background to the widget"""
        if self.debug: qDebug("[PTM] [main.py] [setTransparentBackground]")
        if self.debug: qDebug("[PTM] [main.py] [setTransparentBackground] : Run function with hasBackground '%s'" %(hasBackground))
        if (hasBackground == 2):
            self.theme.setImagePath("widgets/background")
            self.setBackgroundHints(Plasma.Applet.DefaultBackground)
        else:
            self.theme.setImagePath("")
            self.setBackgroundHints(Plasma.Applet.NoBackground)



    def setNetdev(self):
        """function to set network device"""
        if self.debug: qDebug("[PTM] [main.py] [setNetdev]")
        if (self.ptm['vars']['adv']['netdevBool'] > 0):
            return self.ptm['vars']['adv']['netdev']
        netdev = "lo"
        netdir = self.ptm['vars']['adv']['netDir']
        interfaces = QDir.entryList(QDir(netdir), QDir.Dirs | QDir.NoDotAndDotDot)
        for device in interfaces:
            if (str(device) != "lo"):
                try:
                    with open(netdir + '/' + str(device) + '/operstate', 'r') as stateFile:
                        if (stateFile.readline() == "up\n"):
                            netdev = str(device)
                except:
                    pass
        if self.debug: qDebug("[PTM] [main.py] [setNetdev] : Returns '%s'" %(netdev))
        return netdev


    def setText(self, name=None, text=None):
        """function to set text to labels"""
        if self.debug: qDebug("[PTM] [main.py] [setText]")
        if self.debug: qDebug("[PTM] [main.py] [setText] : Run function with name '%s'" %(name))
        if self.debug: qDebug("[PTM] [main.py] [setText] : Run function with text '%s'" %(text))
        self.ptm['labels'][name].setText(text)


    def textPrepare(self, name=None, text=None):
        """function to prepare text"""
        if self.debug: qDebug("[PTM] [main.py] [textPrepare]")
        if self.debug: qDebug("[PTM] [main.py] [textPrepare] : Run function with name '%s'" %(name))
        if self.debug: qDebug("[PTM] [main.py] [textPrepare] : Run function with text '%s'" %(text))
        line = self.ptm['vars']['formats'][name]
        if (name == "custom"):
            cmds = range(len(text.keys()))
            cmds.reverse()
            for i in cmds:
                if (line.split('$custom' + str(i))[0] != line):
                    line = line.split('$custom' + str(i))[0] + \
                        text['custom'+str(i)] + \
                        line.split('$custom' + str(i))[1]
        elif (name == "gpu"):
            if (line.split('$gpu')[0] != line):
                line = line.split('$gpu')[0] + text + line.split('$gpu')[1]
        elif (name == "gputemp"):
            if (line.split('$gputemp')[0] != line):
                line = line.split('$gputemp')[0] + getTemp(text, self.ptm['vars']['adv']['tempUnits']) + line.split('$gputemp')[1]
        elif (name == "pkg"):
            pkgs = range(len(text.keys()))
            pkgs.reverse()
            for i in pkgs:
                if (line.split('$pkgcount' + str(i))[0] != line):
                    line = line.split('$pkgcount' + str(i))[0] + \
                        text['pkgcount'+str(i)] + \
                        line.split('$pkgcount' + str(i))[1]
        elif (name == "player"):
            if (line.split('$album')[0] != line):
                line = line.split('$album')[0] + text['album'] + line.split('$album')[1]
            if (line.split('$artist')[0] != line):
                line = line.split('$artist')[0] + text['artist'] + line.split('$artist')[1]
            if (line.split('$progress')[0] != line):
                line = line.split('$progress')[0] + text['progress'] + line.split('$progress')[1]
            if (line.split('$time')[0] != line):
                line = line.split('$time')[0] + text['time'] + line.split('$time')[1]
            if (line.split('$title')[0] != line):
                line = line.split('$title')[0] + text['title'] + line.split('$title')[1]
        elif (name == "ps"):
            if (line.split('$pscount')[0] != line):
                line = line.split('$pscount')[0] + text['num'] + line.split('$pscount')[1]
            if (line.split('$pstotal')[0] != line):
                line = line.split('$pstotal')[0] + text['total'] + line.split('$pstotal')[1]
            if (line.split('$ps')[0] != line):
                line = line.split('$ps')[0] + text['list'] + line.split('$ps')[1]
        elif (name == "time"):
            if (line.split('$time')[0] != line):
                line = line.split('$time')[0] + text + line.split('$time')[1]
            elif (line.split('$isotime')[0] != line):
                line = line.split('$isotime')[0] + text + line.split('$isotime')[1]
            elif (line.split('$shorttime')[0] != line):
                line = line.split('$shorttime')[0] + text + line.split('$shorttime')[1]
            elif (line.split('$longtime')[0] != line):
                line = line.split('$longtime')[0] + text + line.split('$longtime')[1]
            elif (line.split('$custom')[0] != line):
                line = line.split('$custom')[0] + text + line.split('$custom')[1]
        elif (name == "uptime"):
            if (line.split('$uptime')[0] != line):
                line = line.split('$uptime')[0] + text + line.split('$uptime')[1]
            elif (line.split('$custom')[0] != line):
                line = line.split('$custom')[0] + text + line.split('$custom')[1]
        output = self.ptm['vars']['app']['format'][0] + line + self.ptm['vars']['app']['format'][1]
        if self.debug: qDebug("[PTM] [main.py] [textPrepare] : Returns '%s'" %(output))
        return output



def CreateApplet(parent):
    return pyTextWidget(parent)
