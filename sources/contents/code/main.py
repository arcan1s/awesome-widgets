# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.kdecore import *
from PyKDE4.kdeui import *
from PyKDE4.kio import *
from PyKDE4 import plasmascript
from PyKDE4.plasma import Plasma
import commands, os, time

import configdef
import configwindow
import dataengine
import reinit
from util import *



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self, parent)


    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.initTooltip()
        self.layout = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        
        self.dataengine = dataengine.DataEngine(self)
        self.reinit = reinit.Reinit(self)
        
        self.timer = QTimer()
        QObject.connect(self.timer, SIGNAL("timeout()"), self.updateLabel)
        
        self.setupVar()
        self.reinit.reinit()
        
        self.setHasConfigurationInterface(True)
    
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = configwindow.ConfigWindow(self)
        self.configdef = configdef.ConfigDefinition(self, self.configpage)
        self.configdef.createConfigurationInterface(parent)
    
    
    def initTooltip(self):
        """function to create tooltip"""
        self.tooltip = Plasma.ToolTipContent()
        self.tooltip.setMainText("PyTextMonitor")
        self.tooltip.setSubText('')
        Plasma.ToolTipManager.self().registerWidget(self.applet)
        # show tooltip
        #Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)
    

    def mouseDoubleClickEvent(self, event):
        """function to doubleclick event"""
        os.system("ksysguard")
    

    def setupNetdev(self):
        """function to setup network device"""
        self.netdev = "lo"
        try:
            interfaces = []
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if ((line != '') and (line[0] != ' ') and (line.split(":")[0].split()[0] != 'lo')):
                    interfaces.append(line.split(":")[0].split()[0])
            
            for device in interfaces:
                if (commands.getoutput("ifconfig " + device + " | grep 'inet '") != ''):
                    self.netdev = device
                    break
        except:
            pass


    def setupVar(self):
        """function to setup variables"""
        self.netdev = ''
        # setup number of cores
        commandOut = commands.getoutput("grep -c '^processor' /proc/cpuinfo")
        self.numCores = int(commandOut)
        
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


    def startPolling(self):
        try:
            self.timer.start()
            self.updateLabel()
            self.tooltip.setSubText('')
        except Exception as strerror:
            self.tooltip.setSubText(str(strerror))
            self.label_error = Plasma.Label(self.applet)
            self.label_error.setText('<font color="red">ERROR</font>')
            self.layout.addItem(self.label_error)
            return
   
    def updateLabel(self):
        """function to update label"""
        if ((self.memBool == 1) and (self.memInMb == False)):
            self.memText()
        if ((self.swapBool == 1) and (self.swapInMb == False)):
            self.swapText()
        if (self.batBool == 1):
            self.batText()


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
    
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        self.dataengine.dataUpdated(sourceName, data)



def CreateApplet(parent):
	return pyTextWidget(parent)
