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



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self, parent)
    
    
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
    
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = configwindow.ConfigWindow(self)
        self.configdef = configdef.ConfigDefinition(self, self.configpage)
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
        self.tooltip_pixmap = QPixmap()
        self.tooltip_scene = QGraphicsScene()
        self.tooltip_view = QGraphicsView(self.tooltip_scene)
        self.tooltip_view.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tooltip_view.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        # show tooltip
        Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)
    
    
    def updateTooltip(self):
        """function to update tooltip"""
        self.tooltipBound['mem'] = self.memValues['total']
        self.tooltipBound['swap'] = self.swapValues['total']
        self.tooltip_view.resize(100.0*(len(self.tooltipReq)-self.tooltipReq.count('up')), 100.0)
        self.tooltipAgent.createGraphic(self.tooltipReq, self.tooltipColors, 
            self.tooltipBound, self.tooltipValues, self.tooltip_scene)
        self.tooltip.setImage(QPixmap.grabWidget(self.tooltip_view))
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
        self.ptm['defaults']['order'] = {'6':'bat', '1':'cpu', '7':'cpuclock', 'f':'custom', '9':'gpu', 
            'a':'gputemp', 'b':'hdd', 'c':'hddtemp', '3':'mem', '5':'net', '4':'swap', '2':'temp', 
            '8':'uptime', 'd':'player', 'e':'time'}
        self.ptm['defaults']['format'] = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]', 
            'cpuclock':'[mhz: $cpucl]', 'custom':'[$custom]', 'gpu':'[gpu: $gpu%]', 
            'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: $hdd0%]', 
            'hddtemp':'[hdd temp: $hddtemp0&deg;C]', 'mem':'[mem: $mem%]', 
            'net':'[$netdev: $down/$upKB/s]', 'swap':'[swap: $swap%]', 
            'temp':'[temp: $temp0&deg;C]', 'uptime':'[uptime: $uptime]', 
            'player':'[$artist - $title]', 'time':'[$time]'}
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
        self.ptm['values']['cpu'] = {-1:"  0.0"}
        self.ptm['values']['cpuclock'] = {-1:"   0"}
        numCores = int(commands.getoutput("grep -c '^processor' /proc/cpuinfo"))
        for i in range(numCores):
            self.ptm['values']['cpu'][i] = "  0.0"
            self.ptm['values']['cpuclock'][i] = "   0"
        self.ptm['values']['hdd'] = {}
        self.ptm['values']['hddtemp'] = {}
        self.ptm['values']['mem'] = {'used':0, 'free':0, 'total':1}
        self.ptm['values']['net'] = {"up":"   0", "down":"   0"}
        self.ptm['values']['player'] = {}
        self.ptm['values']['swap'] = {'used':0, 'free':0, 'total':1}
        self.ptm['values']['temp'] = {}
        # variables
        self.ptm['vars'] = {}
        self.ptm['vars']['adv'] = {}
        self.ptm['vars']['app'] = {}
        self.ptm['vars']['bools'] = {}
        self.ptm['vars']['formats'] = {}
        self.ptm['vars']['tooltip'] = {}
        
        self.cpuCore = {-1:"  0.0"}
        self.cpuClockCore = {-1:"   0"}
        numCores = int(commands.getoutput("grep -c '^processor' /proc/cpuinfo"))
        for i in range(numCores):
            self.cpuCore[i] = str("  0.0")
            self.cpuClockCore[i] = str("   0")
        self.hddNames = []
        self.hdd = {}
        self.mountNames = []
        self.mount = {}
        self.memValues = {'app':0.0, 'free':0.0, 'used':0.0}
        self.netdev = ''
        self.netSpeed = {"up":"   0", "down":"   0"}
        self.swapValues = {'free':0.0, 'used':0.0}
        self.tempNames = []
        self.temp = {}
        self.tooltipBound = {'cpu':100.0, 'cpuclock':4000.0, 'mem':16000.0, 
            'swap':16000, 'down':10000.0, 'up':10000.0}
        self.tooltipColors = {}
        self.tooltipNum = 100
        self.tooltipReq = []
        self.tooltipValues = {'cpu':[0.0, 0.01], 'cpuclock':[0.0, 0.01], 'mem':[0.0, 0.01], 
            'swap':[0.0, 0.01], 'down':[0.0, 0.01], 'up':[0.0, 0.01]}
        
        # create dictionaries
        self.dict_orders = {'6':'bat', '1':'cpu', '7':'cpuclock', 'f':'custom', '9':'gpu', 
        'a':'gputemp', 'b':'hdd', 'c':'hddtemp', '3':'mem', '5':'net', '4':'swap', '2':'temp', 
        '8':'uptime', 'd':'player', 'e':'time'}
        self.dict_defFormat = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]', 
        'cpuclock':'[mhz: $cpucl]', 'custom':'[$custom]', 'gpu':'[gpu: $gpu%]', 
        'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: $hdd0%]', 
        'hddtemp':'[hdd temp: $hddtemp0&deg;C]', 'mem':'[mem: $mem%]', 
        'net':'[$netdev: $down/$upKB/s]', 'swap':'[swap: $swap%]', 
        'temp':'[temp: $temp0&deg;C]', 'uptime':'[uptime: $uptime]', 
        'player':'[$artist - $title]', 'time':'[$time]'}


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
        if (self.batBool > 0):
            self.batText()
        if (self.cpuBool > 0):
            self.cpuText()
        if (self.cpuclockBool > 0):
            self.cpuclockText()
        if (self.hddBool > 0):
            self.mountText()
        if ((self.memBool > 0) and (self.memInMb == False)):
            self.memText()
        if (self.netBool > 0):
            self.netText()
        if ((self.swapBool > 0) and (self.swapInMb == False)):
            self.swapText()
        if (self.tempBool > 0):
            self.tempText()
        self.updateTooltip()
    
    
    def batText(self):
        """function to set battery text"""
        line = self.batFormat
        if (line.split('$bat')[0] != line):
            try:
                with open (self.battery_device, 'r') as bat_file:
                    bat = bat_file.readline().split('\n')[0]
            except:
                bat = 'off'
            bat = "%3s" % (bat)
            line = line.split('$bat')[0] + bat + line.split('$bat')[1]
        if (line.split('$ac')[0] != line):
            try:
                with open (self.ac_device, 'r') as bat_file:
                    bat = bat_file.readline().split('\n')[0]
                if (bat == '1'):
                    bat = '(*)'
                else:
                    bat = '( )'
            except:
                bat = '(?)'
            line = line.split('$ac')[0] + bat + line.split('$ac')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_bat.setText(text)
    
    
    def cpuText(self):
        """function to set cpu text"""
        line = self.cpuFormat
        for core in self.cpuCore.keys():
            if (core > -1):
                if (line.split('$cpu'+str(core))[0] != line):
                    line = line.split('$cpu'+str(core))[0] + self.cpuCore[core] + line.split('$cpu'+str(core))[1]
        if (line.split('$cpu')[0] != line):
            line = line.split('$cpu')[0] + self.cpuCore[-1] + line.split('$cpu')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_cpu.setText(text)
    
    
    def cpuclockText(self):
        """function to set cpu clock text"""
        line = self.cpuclockFormat
        for core in self.cpuClockCore.keys():
            if (core > -1):
                if (line.split('$cpucl'+str(core))[0] != line):
                    line = line.split('$cpucl'+str(core))[0] + self.cpuClockCore[core] + line.split('$cpucl'+str(core))[1]
        if (line.split('$cpucl')[0] != line):
            line = line.split('$cpucl')[0] + self.cpuClockCore[-1] + line.split('$cpucl')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_cpuclock.setText(text)
    
    
    def hddText(self):
        """function to set hddtemp text"""
    
    
    def memText(self):
        """function to set mem text"""
        try:
            mem = 100 * self.memValues['used'] / self.memValues['total']
            mem = "%5s" % (str(round(mem, 1)))
        except:
            mem = "  N\\A"
        if (self.memFormat.split('$mem')[0] != self.memFormat):
            line = self.memFormat.split('$mem')[0] + mem + self.memFormat.split('$mem')[1]
        else:
            line = self.memFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_mem.setText(text)
    
    
    def mountText(self):
        """function to set mount text"""
        line = self.hddFormat
        for i in range(len(self.mountNames)):
            if (line.split('$hdd'+str(i))[0] != line):
                line = line.split('$hdd'+str(i))[0] + self.mount[self.mountNames[i]] + line.split('$hdd'+str(i))[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_hdd.setText(text)
    
    
    def netText(self):
        """function to set network text"""
        line = self.netFormat
        if (line.split('$netdev')[0] != 0):
            line = line.split('$netdev')[0] + self.ptm['names']['net'] + line.split('$netdev')[1]
        if (line.split('$up')[0] != line):
            line = line.split('$up')[0] + self.netSpeed['up'] + line.split('$up')[1]
        if (line.split('$down')[0] != line):
            line = line.split('$down')[0] + self.netSpeed['down'] + line.split('$down')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_net.setText(text)
    
    
    def swapText(self):
        """function to set swap text"""
        try:
            mem = 100 * self.swapValues['used'] / self.swapValues['total']
            mem = "%5s" % (str(round(mem, 1)))
        except:
            mem = "  N\\A"
        if (self.swapFormat.split('$swap')[0] != self.swapFormat):
            line = self.swapFormat.split('$swap')[0] + mem + self.swapFormat.split('$swap')[1]
        else:
            line = self.swapFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_swap.setText(text)
    
    
    def tempText(self):
        """function to set temperature text"""
        line = self.tempFormat
        for i in range(len(self.tempNames)):
            if (line.split('$temp'+str(i))[0] != line):
                line = line.split('$temp'+str(i))[0] + self.temp[self.tempNames[i]] + line.split('$temp'+str(i))[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_temp.setText(text)
    
    
    # api's functions
    def addLabel(self, name=None, text=None, add=True):
        """function to add new label"""
        if (add):
            self.ptm['labels'][name] = NewPlasmaLabel(self.applet, self)
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
        self.ptm['vars']['adv']
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
        self.ptm['names']['net'] = netdev
        return netdev
    
    
    def setText(self, name=None, text=None):
        """function to set text to labels"""
        self.ptm['labels'][name].setText(text)
    
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        updatedData = self.dataengine.dataUpdated(str(sourceName), data, self.ptm)
        # update tooltips
        if (updatedData['name'] in ['cpu', 'cpuclock', 'mem', 'swap', 'net']):
            pass
        # update labels where is needed
        if (updatedData['name'] in ['gpu', 'gputemp', 'player']):
            pass
        else:
            if (updatedData['type']):
                pass
            else:
                pass
        self.update()



def CreateApplet(parent):
    return pyTextWidget(parent)
