# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.kdecore import *
from PyKDE4.kdeui import *
from PyKDE4.kio import *
from PyKDE4 import plasmascript
from PyKDE4.plasma import Plasma
import commands, os, time

import configaccepted
import configwindow
import dataengine
import reinit
from util import *



class pyTextWidget(plasmascript.Applet):
    def __init__(self, parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self,parent)


    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.layout = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        self.configAccepted = configaccepted.ConfigAccepted(self)
        self.dataengine = dataengine.DataEngine(self)
        self.reinit = reinit.Reinit(self)
        self.setupVar()
        self.reinit.reinit()
        self.setHasConfigurationInterface(True)
    
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = configwindow.ConfigWindow(self, self.settings)
        
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
                self.configpage.checkboxes[self.dict_orders[label]].setCheckState(2)
                self.configpage.sliders[self.dict_orders[label]].setValue(self.label_order.find(label)+1)
            else:
                self.configpage.checkboxes[self.dict_orders[label]].setCheckState(0)
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
        
        parent.okClicked.connect(self.configAccepted.configAccepted)


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
            QObject.connect(self.timer, SIGNAL("timeout()"), self.updateLabel)
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
            self.gpuChecker.label_exit = 2
        if (self.gputempBool == 1):
            self.gputempText()
            self.gpuTempChecker.label_exit = 2
        if ((self.memBool == 1) and (self.memInMb == False)):
            self.memText()
        if ((self.swapBool == 1) and (self.swapInMb == False)):
            self.swapText()
        if (self.hddtempBool == 1):
            self.hddtempText()
            self.hddTempChecker.label_exit = 2
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
        if (self.gpuFormat.split('$gpu')[0] != self.gpuFormat):
            line = self.gpuFormat.split('$gpu')[0] + self.gpuChecker.gpu + self.gpuFormat.split('$gpu')[1]
        else:
            line = self.gpuFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_gpu.setText(text)


    def gputempText(self):
        """function to set gpu temperature text"""
        if (self.gputempFormat.split('$gputemp')[0] != self.gputempFormat):
            line = self.gputempFormat.split('$gputemp')[0] + self.gpuTempChecker.gputemp + self.gputempFormat.split('$gputemp')[1]
        else:
            line = self.gputempFormat
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_gputemp.setText(text)


    def hddtempText(self):
        """function to set hdd temperature text"""
        if (self.hddtempFormat.split('@@')[0] != self.hddtempFormat):
            line = self.hddtempFormat.split('@@')[0] + self.hddTempChecker.hddtemp + self.hddtempFormat.split('@@')[2]
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
    
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to update label"""
        self.dataengine.dataUpdated(sourceName, data)



def CreateApplet(parent):
	return pyTextWidget(parent)
