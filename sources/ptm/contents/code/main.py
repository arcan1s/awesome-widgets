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
import commands, os, shutil, time

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
        self.reinit.reinit(confAccept=False)
        
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
        # show tooltip
        #Plasma.ToolTipManager.self().setContent(self.applet, self.tooltip)
    
    
    def mouseDoubleClickEvent(self, event):
        """function to doubleclick event"""
        os.system("ksysguard &")
    
    
    def setupNetdev(self):
        """function to setup network device"""
        netdev = "lo"
        interfaces = QDir.entryList(QDir(self.netdir), QDir.Dirs | QDir.NoDotAndDotDot)
        for device in interfaces:
            if (str(device) != "lo"):
                try:
                    with open(self.netdir+"/"+str(device)+"/operstate", "r") as stateFile:
                        if (stateFile.readline() == "up\n"):
                            netdev = str(device)
                except:
                    pass
        return netdev
    
    
    def setupVar(self):
        """function to setup variables"""
        self.netdev = ''
        self.cpuCore = {-1:"  0.0"}
        self.cpuClockCore = {-1:"   0"}
        numCores = int(commands.getoutput("grep -c '^processor' /proc/cpuinfo"))
        for i in range(numCores):
            self.cpuCore[i] = str("  0.0")
            self.cpuClockCore[i] = str("   0")
        self.netSpeed = {"up":"   0", "down":"   0"}
        self.tempNames = []
        self.temp = {}
        self.mountNames = []
        self.mount = {}
        self.hddNames = []
        self.hdd = {}
        
        # create dictionaries
        self.dict_orders = {'6':'bat', '1':'cpu', '7':'cpuclock', 'f':'custom', '9':'gpu', 
        'a':'gputemp', 'b':'hdd', 'c':'hddtemp', '3':'mem', '5':'net', '4':'swap', '2':'temp', 
        '8':'uptime', 'd':'player', 'e':'time'}
        self.dict_defFormat = {'bat':'[bat: $bat%$ac]', 'cpu':'[cpu: $cpu%]', 
        'cpuclock':'[mhz: $cpucl]', 'custom':'[$custom]', 'gpu':'[gpu: $gpu%]', 
        'gputemp':'[gpu temp: $gputemp&deg;C]', 'hdd':'[hdd: $hdd0%]', 
        'hddtemp':'[hdd temp: $hddtemp0&deg;C]', 'mem':'[mem: $mem%]', 
        'net':'[$netdev: $netKB/s]', 'swap':'[swap: $swap%]', 
        'temp':'[temp: $temp&deg;C]', 'uptime':'[uptime: $uptime]', 
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
            self.label_error = Plasma.Label(self.applet)
            self.label_error.setText('<font color="red">ERROR</font>')
            self.layout.addItem(self.label_error)
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
        for core in self.cpuclockCore.keys():
            if (core > -1):
                if (line.split('$cpucl'+str(core))[0] != line):
                    line = line.split('$cpucl'+str(core))[0] + self.cpuclockCore[core] + line.split('$cpucl'+str(core))[1]
        if (line.split('$cpucl')[0] != line):
            line = line.split('$cpucl')[0] + self.cpuclockCore[-1] + line.split('$cpucl')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_cpuclock.setText(text)
    
    
    def memText(self):
        """function to set mem text"""
        full = self.mem_uf + self.mem_free
        try:
            mem = 100 * self.mem_used / full
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
        for i in self.mountNames:
            if (line.split('$hdd'+str(i))[0] != line):
                line = line.split('$hdd'+str(i))[0] + self.mount[self.mountNames[i]] + line.split('$hdd'+str(i))[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_hdd.setText(text)
    
    
    def netText(self):
        """function to set network text"""
        line = self.netFormat
        if (line.split('$up')[0] != line):
            line = line.split('$up')[0] + self.netSpeed['up'] + line.split('$up')[1]
        if (line.split('$down')[0] != line):
            line = line.split('$down')[0] + self.netSpeed['down'] + line.split('$down')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_net.setText(text)
    
    
    def swapText(self):
        """function to set swap text"""
        full = self.swap_used + self.swap_free
        try:
            mem = 100 * self.swap_used / full
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
        for i in self.tempNames:
            if (line.split('$temp'+str(i))[0] != line):
                line = line.split('$temp'+str(i))[0] + self.temp[self.tempNames[i]] + line.split('$temp'+str(i))[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_temp.setText(text)
    
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        self.dataengine.dataUpdated(sourceName, data)



def CreateApplet(parent):
    return pyTextWidget(parent)
