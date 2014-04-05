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
import commands, os, shutil

import configdef
import configwindow
import dataengine
import ptmnotify
import reinit
import tooltip
from util import *



class CustomPlasmaLabel(Plasma.Label):
    """new Label with defined clicked() event"""
    def __init__(self, applet, name):
        """class definition"""
        Plasma.Label.__init__(self, applet)
        self.name = name
        self.notify = ptmnotify.PTMNotify(self)


    def mousePressEvent(self, event):
        """mouse click event"""
        if (event.button() == Qt.LeftButton):
            self.notify.init(self.name)



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self, parent)


    # initialization
    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.setupVar()

        self.dataengine = dataengine.DataEngine(self)
        self.reinit = reinit.Reinit(self, self.ptm['defaults'])
        self.tooltipAgent = tooltip.Tooltip(self)

        self.timer = QTimer()
        QObject.connect(self.timer, SIGNAL("timeout()"), self.updateLabel)

        self.initTooltip()
        self.reInit()
        self.applet.setLayout(self.ptm['layout'])
        self.theme = Plasma.Svg(self)
        self.theme.setImagePath("widgets/background")
        self.setBackgroundHints(Plasma.Applet.DefaultBackground)

        self.setHasConfigurationInterface(True)
        # Create notifyrc file if required
        kdehome = unicode(KGlobal.dirs().localkdedir())
        if ((not os.path.exists(kdehome + "/share/apps/plasma_applet_pytextmonitor/plasma_applet_pytextmonitor.notifyrc")) and
            (not os.path.exists("/usr" + "/share/apps/plasma_applet_pytextmonitor/plasma_applet_pytextmonitor.notifyrc"))):
            self.createNotifyrc(kdehome)


    # internal functions
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = configwindow.ConfigWindow(self)
        self.configdef = configdef.ConfigDefinition(self, self.configpage, self.ptm['defaults'])
        self.configdef.createConfigurationInterface(parent)


    def createNotifyrc(self, kdehome):
        """function to create *.notifyrc"""
        if (not os.path.isdir(kdehome + "/share/apps/plasma_applet_pytextmonitor")):
            os.mkdir(kdehome + "/share/apps/plasma_applet_pytextmonitor")
        shutil.copy(kdehome + "/share/apps/plasma/plasmoids/py-text-monitor/contents/code/plasma_applet_pytextmonitor.notifyrc",
                    kdehome + "/share/apps/plasma_applet_pytextmonitor/")


    def initTooltip(self):
        """function to create tooltip"""
        self.tooltip = Plasma.ToolTipContent()
        self.tooltip.setMainText("PyTextMonitor")
        self.tooltip.setSubText('')
        Plasma.ToolTipManager.self().registerWidget(self.applet)
        # graphical tooltip
        self.tooltipScene = QGraphicsScene()
        self.tooltipView = QGraphicsView(self.tooltipScene)
        self.tooltipView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tooltipView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        # show tooltip
        Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)


    def mouseDoubleClickEvent(self, event):
        """function to doubleclick event"""
        os.system("ksysguard &")


    def setupVar(self):
        """function to setup variables"""
        self.ptm = {}
        # dataengines
        self.ptm['dataengine'] = {'ext':None, 'system':None, 'time':None}
        # defaults
        self.ptm['defaults'] = {}
        self.ptm['defaults']['confBool'] = {'bat':'batBool', 'cpu':'cpuBool',
            'cpuclock':'cpuclockBool', 'custom':'customBool', 'gpu':'gpuBool',
            'gputemp':'gputempBool', 'hdd':'hddBool', 'hddtemp':'hddtempBool',
            'mem':'memBool', 'net':'netBool', 'swap':'swapBool', 'temp':'tempBool',
            'uptime':'uptimeBool', 'player':'playerBool', 'time':'timeBool'}
        self.ptm['defaults']['confColor'] = {'cpu':'cpu_color', 'cpuclock':'cpuclock_color',
            'down':'down_color', 'mem':'mem_color', 'swap':'swap_color', 'up':'up_color'}
        self.ptm['defaults']['confFormat'] = {'bat':'batFormat', 'cpu':'cpuFormat',
            'cpuclock':'cpuclockFormat', 'custom':'customFormat', 'gpu':'gpuFormat',
            'gputemp':'gputempFormat', 'hdd':'hddFormat', 'hddtemp':'hddtempFormat',
            'mem':'memFormat', 'net':'netFormat', 'player':'playerFormat',
            'swap':'swapFormat', 'temp':'tempFormat', 'time':'timeFormat',
            'uptime':'uptimeFormat'}
        self.ptm['defaults']['bool'] = {'bat':0, 'cpu':2, 'cpuclock':0, 'custom':0,
            'gpu':0, 'gputemp':0, 'hdd':0, 'hddtemp':0, 'mem':2, 'net':2, 'player':0,
            'swap':2, 'temp':0, 'time':0, 'uptime':0}
        self.ptm['defaults']['format'] = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]',
            'cpuclock':'[mhz: $cpucl]', 'custom':'[$custom]', 'gpu':'[gpu: $gpu%]',
            'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: $hdd0%]',
            'hddtemp':'[hdd temp: $hddtemp0&deg;C]', 'mem':'[mem: $mem%]',
            'net':'[$netdev: $down/$upKB/s]', 'player':'[$artist - $title]',
            'swap':'[swap: $swap%]', 'temp':'[temp: $temp0&deg;C]',
            'time':'[$time]', 'uptime':'[uptime: $uptime]'}
        self.ptm['defaults']['order'] = {'6':'bat', '1':'cpu', '7':'cpuclock', 'f':'custom', '9':'gpu',
            'a':'gputemp', 'b':'hdd', 'c':'hddtemp', '3':'mem', '5':'net', '4':'swap', '2':'temp',
            '8':'uptime', 'd':'player', 'e':'time'}
        # labels
        self.ptm['labels'] = {}
        self.ptm['layout'] = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        self.ptm['layout'].setContentsMargins(1, 1, 1, 1)
        # names
        self.ptm['names'] = {}
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
        for i in range(numCores):
            self.ptm['values']['cpu'][i] = 0.0
            self.ptm['values']['cpuclock'][i] = 0.0
        self.ptm['values']['hdd'] = {}
        self.ptm['values']['hddtemp'] = {}
        self.ptm['values']['mem'] = {'app':0.0, 'used':0.0, 'free':1.0}
        self.ptm['values']['net'] = {"up":0.0, "down":0.0}
        self.ptm['values']['player'] = {}
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
        plasmascript.Applet.showConfigurationInterface(self)


    def startPolling(self):
        try:
            self.timer.start()
            self.updateLabel()
            self.tooltip.setSubText('')
        except Exception as strerror:
            self.tooltip.setSubText(str(strerror))
            return


    def updateLabel(self):
        """function to update label"""
        if (self.ptm['vars']['bools']['bat'] > 0):
            self.batText()
        if (self.ptm['vars']['bools']['cpu'] > 0):
            self.cpuText()
        if (self.ptm['vars']['bools']['cpuclock'] > 0):
            self.cpuclockText()
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


    def updateTooltip(self):
        """function to update tooltip"""
        self.tooltipView.resize(100.0*(len(self.ptm['vars']['tooltip']['required']) - self.ptm['vars']['tooltip']['required'].count('up')), 100.0)
        self.tooltipAgent.createGraphic(self.ptm['vars']['tooltip'], self.ptm['tooltip'], self.tooltipScene)
        self.tooltip.setImage(QPixmap.grabWidget(self.tooltipView))
        Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)


    # update functions
    # update dataengines
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        updatedData = self.dataengine.dataUpdated(str(sourceName), data, self.ptm)
        if (updatedData['value'] == None):
            return
        # update falues where is needed
        if (updatedData['type'] != None):
            self.ptm['values'][updatedData['name']][updatedData['type']] = updatedData['value']
        else:
            self.ptm['values'][updatedData['name']] = updatedData['value']
        # update labels where is needed
        if (updatedData['name'] in ['custom', 'gpu', 'gputemp', 'player', 'time', 'uptime']):
            text = self.textPrepare(updatedData['name'], updatedData['value'])
            self.setText(updatedData['name'], text)
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
                if (bat == '1'):
                    bat = "(*)"
                else:
                    bat = "( )"
            except:
                bat = "(?)"
            line = line.split('$ac')[0] + bat + line.split('$ac')[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("bat", text)


    def cpuText(self):
        """function to set cpu text"""
        line = self.ptm['vars']['formats']['cpu']
        for core in self.ptm['values']['cpu'].keys():
            if ((core > -1) and (line.split('$cpu' + str(core))[0] != line)):
                value = "%5.1f" % (self.ptm['values']['cpu'][core])
                line = line.split('$cpu' + str(core))[0] + value + line.split('$cpu' + str(core))[1]
        if (line.split('$cpu')[0] != line):
            value = "%5.1f" % (self.ptm['values']['cpu'][-1])
            line = line.split('$cpu')[0] + value + line.split('$cpu')[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("cpu", text)


    def cpuclockText(self):
        """function to set cpu clock text"""
        line = self.ptm['vars']['formats']['cpuclock']
        for core in self.ptm['values']['cpuclock'].keys():
            if ((core > -1) and (line.split('$cpucl' + str(core))[0] != line)):
                value = "%4.0f" % (self.ptm['values']['cpuclock'][core])
                line = line.split('$cpucl' + str(core))[0] + value + line.split('$cpucl' + str(core))[1]
        if (line.split('$cpucl')[0] != line):
            value = "%4.0f" % (self.ptm['values']['cpuclock'][-1])
            line = line.split('$cpucl')[0] + value + line.split('$cpucl')[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("cpuclock", text)


    def hddText(self):
        """function to set hdd text"""
        line = self.ptm['vars']['formats']['hdd']
        for i in range(len(self.ptm['names']['hdd'])):
            if (line.split('$hdd' + str(i))[0] != line):
                line = line.split('$hdd' + str(i))[0] + str(self.ptm['values']['hdd'][self.ptm['names']['hdd'][i]]) + line.split('$hdd' + str(i))[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("hdd", text)


    def hddtempText(self):
        """function to set hddtemp text"""
        line = self.ptm['vars']['formats']['hddtemp']
        for i in range(len(self.ptm['names']['hddtemp'])):
            if (line.split('$hddtemp' + str(i))[0] != line):
                line = line.split('$hddtemp' + str(i))[0] + self.ptm['values']['hddtemp'][self.ptm['names']['hddtemp'][i]] + line.split('$hddtemp' + str(i))[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("hddtemp", text)


    def memText(self):
        """function to set mem text"""
        line = self.ptm['vars']['formats']['mem']
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
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("mem", text)


    def netText(self):
        """function to set network text"""
        line = self.ptm['vars']['formats']['net']
        if (line.split('$netdev')[0] != 0):
            line = line.split('$netdev')[0] + self.ptm['names']['net'] + line.split('$netdev')[1]
        if (line.split('$down')[0] != line):
            value = "%4.0f" %(self.ptm['values']['net']['down'])
            line = line.split('$down')[0] + value + line.split('$down')[1]
        if (line.split('$up')[0] != line):
            value = "%4.0f" %(self.ptm['values']['net']['up'])
            line = line.split('$up')[0] + value + line.split('$up')[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("net", text)


    def swapText(self):
        """function to set swap text"""
        line = self.ptm['vars']['formats']['swap']
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
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("swap", text)


    def tempText(self):
        """function to set temperature text"""
        line = self.ptm['vars']['formats']['temp']
        for i in range(len(self.ptm['names']['temp'])):
            if (line.split('$temp' + str(i))[0] != line):
                line = line.split('$temp' + str(i))[0] + self.ptm['values']['temp'][self.ptm['names']['temp'][i]] + line.split('$temp' + str(i))[1]
        text = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        self.setText("temp", text)


    # external functions
    def addLabel(self, name=None, text=None, add=True):
        """function to add new label"""
        if (add):
            self.ptm['labels'][name] = CustomPlasmaLabel(self.applet, name)
            self.ptm['layout'].addItem(self.ptm['labels'][name])
            self.setText(name, text)
        else:
            self.setText(name, '')
            self.ptm['layout'].removeItem(self.ptm['labels'][name])


    def applySettings(self, name=None, ptm=None):
        """function to read settings"""
        self.ptm[name] = ptm
        if (name == "names"):
            for item in ['hdd', 'hddtemp', 'temp']:
                for value in self.ptm['names'][item]:
                    self.ptm['values'][item][value] = 0.0


    def connectToEngine(self):
        """function to connect to dataengines"""
        self.ptm['dataengine']['ext'] = self.dataEngine("ext-sysmon")
        self.ptm['dataengine']['system'] = self.dataEngine("systemmonitor")
        self.ptm['dataengine']['time'] = self.dataEngine("time")
        self.dataengine.connectToEngine(self.ptm['vars']['bools'], self.ptm['dataengine'],
            self.ptm['vars']['app']['interval'], self.ptm['names'])


    def disconnectFromSource(self):
        """function to disconnect from sources"""
        for label in self.ptm['defaults']['format'].keys():
            if (self.ptm['vars']['bools'][label] > 0):
                self.addLabel(label, None, False)
                keys = {'cpu':self.ptm['values']['cpu'].keys(), 'cpuclock':self.ptm['values']['cpuclock'].keys(),
                    'hdd':self.ptm['values']['hdd'].keys(), 'net':self.ptm['names']['net'],
                    'temp':self.ptm['values']['temp'].keys()}
                self.dataengine.disconnectFromSource(self.ptm['dataengine'], keys, label)


    def reInit(self):
        """function to run reinit"""
        self.reinit.reinit()
        if (self.ptm['vars']['adv']['netdevBool'] == 0):
            self.ptm['names']['net'] = self.setNetdev()
        self.resize(10, 10)

        # create dataengines
        self.thread().wait(60000)
        self.connectToEngine()

        self.timer.setInterval(self.ptm['vars']['app']['interval'])
        self.startPolling()


    def setNetdev(self):
        """function to set network device"""
        netdev = "lo"
        netdir = self.ptm['vars']['adv']['netDir']
        interfaces = QDir.entryList(QDir(netdir), QDir.Dirs | QDir.NoDotAndDotDot)
        for device in interfaces:
            if (str(device) != "lo"):
                #try:
                if True:
                    with open(netdir + '/' + str(device) + '/operstate', 'r') as stateFile:
                        if (stateFile.readline() == "up\n"):
                            netdev = str(device)
                #except:
                    #pass
        return netdev


    def setText(self, name=None, text=None):
        """function to set text to labels"""
        self.ptm['labels'][name].setText(text)


    def textPrepare(self, name=None, text=None):
        """function to prepare text"""
        line = self.ptm['vars']['formats'][name]
        if (name == "custom"):
            if (line.split('$custom')[0] != line):
                line = line.split('$custom')[0] + text + line.split('$custom')[1]
        elif (name == "gpu"):
            if (line.split('$gpu')[0] != line):
                line = line.split('$gpu')[0] + text + line.split('$gpu')[1]
        elif (name == "gputemp"):
            if (line.split('$gputemp')[0] != line):
                line = line.split('$gputemp')[0] + text + line.split('$gputemp')[1]
        elif (name == "player"):
            if (line.split('$album')[0] != line):
                line = line.split('$album')[0] + self.ptm['values']['player']['album'] + line.split('$album')[1]
            if (line.split('$artist')[0] != line):
                line = line.split('$artist')[0] + self.ptm['values']['player']['artist'] + line.split('$artist')[1]
            if (line.split('$progress')[0] != line):
                line = line.split('$progress')[0] + self.ptm['values']['player']['progress'] + line.split('$progress')[1]
            if (line.split('$time')[0] != line):
                line = line.split('$time')[0] + self.ptm['values']['player']['time'] + line.split('$time')[1]
            if (line.split('$title')[0] != line):
                line = line.split('$title')[0] + self.ptm['values']['player']['title'] + line.split('$title')[1]
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
        output = self.ptm['vars']['app']['format'].split('$LINE')[0] + line + self.ptm['vars']['app']['format'].split('$LINE')[1]
        return output



def CreateApplet(parent):
    return pyTextWidget(parent)
