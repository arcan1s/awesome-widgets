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
    def __init__(self,parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self,parent)

    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.layout = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        self.setHasConfigurationInterface(True)
        
        self.label = Plasma.Label(self.applet)
        self.label.setText("N\A")
        self.layout.addItem(self.label)
        self.applet.setLayout(self.layout)
        
        self.theme = Plasma.Svg(self)
        self.theme.setImagePath("widgets/background")
        self.setBackgroundHints(Plasma.Applet.DefaultBackground)
        
        #initial configuration
        self.settings = Config(self)
        self.format = str(self.settings.get('format', '[cpu: $cpu%][temp: $temp&deg;C][mem: $mem%][swap: $swap%][net: $netKB/s][bat: $bat%]'))
        self.interval = int(self.settings.get('interval', '2000'))
        self.font_family = str(self.settings.get('font_family', 'Terminus'))
        self.font_size = int(self.settings.get('font_size', 12))
        self.font_color = str(self.settings.get('font_color', '#000000'))
        self.font_style = str(self.settings.get('font_style', 'normal'))
        self.num_dev = int(self.settings.get('num_dev', 0))
        
        self.setupNetdev()
        self.setupTemp()
        self.connectToEngine()
        
        self.cpuText = "  0.0"
        self.up_speed = "   0"
        self.down_speed = "   0"
        self.mem_used = 0.0
        self.mem_free = 1.0
        self.mem_uf = 0.0
        self.swap_free = 1.0
        self.swap_used = 0.0
        self.tempText = " 0.0"
        
        self.resize(250,10)
        
        # start timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(self.interval)
        self.startPolling()
     
    def configAccepted(self):
        """function to accept settings"""
        # update local variables
        self.format = str(self.configpage.ui.lineEdit_format.text())
        self.interval = int(self.configpage.ui.spinBox_interval.value())
        self.font_family = str(self.configpage.ui.fontComboBox.currentFont().family())
        self.font_size = int(self.configpage.ui.spinBox_fontSize.value())
        self.font_color = str(self.configpage.ui.kcolorcombo.color().name())
        self.font_style = str(self.configpage.ui.lineEdit_style.text())
        self.num_dev = int(self.configpage.ui.comboBox_numNet.currentIndex())
        
        # save config to settings
        self.settings.set('format', self.format)
        self.settings.set('interval', self.interval)
        self.settings.set('font_family', self.font_family)
        self.settings.set('font_size', self.font_size)
        self.settings.set('font_color', self.font_color)
        self.settings.set('font_style', self.font_style)
        self.settings.set('num_dev', self.num_dev)
        
        # update timer
        self.timer.setInterval(self.interval)
        self.startPolling()
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = ConfigWindow(self, self.settings)
        
        font = QFont(str(self.settings.get('font_family', 'Terminus')), int(self.settings.get('font_size', 12)), int(int(self.settings.get('font_weight', 50))))
        self.configpage.ui.lineEdit_format.setText(str(self.settings.get('format', '[cpu: $cpu%][temp: $temp&deg;C][mem: $mem%][swap: $swap%][net: $netKB/s][bat: $bat%]')))
        self.configpage.ui.spinBox_interval.setValue(int(self.settings.get('interval', '2000')))
        self.configpage.ui.fontComboBox.setCurrentFont(font)
        self.configpage.ui.spinBox_fontSize.setValue(int(self.settings.get('font_size', 12)))
        self.configpage.ui.kcolorcombo.setColor(QColor(str(self.settings.get('font_color', '#000000'))))
        self.configpage.ui.lineEdit_style.setText(str(self.settings.get('font_style', 'normal')))
        self.configpage.ui.comboBox_numNet.setCurrentIndex(int(self.settings.get('num_dev', 0)))
        
        # add config page
        page = parent.addPage(self.configpage, i18n(self.name()))
        page.setIcon(KIcon(self.icon()))
        
        parent.okClicked.connect(self.configAccepted)
    
    def formating_line(self):
        """function to set format line"""
        output = self.format

        if (output.split("$cpu")[0] != output):
            output = output.split("$cpu")[0] + self.cpuText + output.split("$cpu")[1]
        if (output.split("$temp")[0] != output):
            output = output.split("$temp")[0] + self.tempText + output.split("$temp")[1]
        if (output.split("$mem")[0] != output):
            output = output.split("$mem")[0] + self.memText() + output.split("$mem")[1]
        if (output.split("$swap")[0] != output):
            output = output.split("$swap")[0] + self.swapText() + output.split("$swap")[1]
        if (output.split("$net")[0] != output):
            output = output.split("$net")[0] + self.netText() + output.split("$net")[1]
            print output
        if (output.split("$bat")[0] != output):
            output = output.split("$bat")[0] + self.batText() + output.split("$bat")[1]
        
        return output
    
    def setupNetdev(self):
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
        
    def setupTemp(self):
        commandOut = commands.getoutput("sensors | grep Physical -B2")
        self.tempdev = "lmsensors/"+commandOut.split("\n")[0]+"/"+'_'.join(commandOut.split("\n")[2].split(":")[0].split())
    
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
            self.label.setText('<font color="red">ERROR</font>')
            return
   
    def updateLabel(self):
        """function to update label"""
        string = self.formating_line()
        #string = self.cpuText
        text = "<html><head/><body style=\" font-family:\'"+ self.font_family +"\'; font-size:"+ str(self.font_size)
        text = text +"pt; font-style:"+ self.font_style +";\">"
        text = text + "<p align=\"center\"><span style=\" color:" + self.font_color + ";\"><pre>"+string
        text = text + "</pre></span></p></body></html>"
        self.label.setText(text)
    
    def batText(self):
        """function to set battery text"""
        commandOut = commands.getoutput("acpi")
        bat = commandOut.split()[3][:-1]
        output = "%3s" % (bat)
        
        return output
    
    def memText(self):
        """function to set mem text"""
        full = self.mem_uf + self.mem_free
        mem = 100 * self.mem_used / full
        output = "%5s" % (str(round(mem, 1)))
                
        return output
    
    def netText(self):
        """function to set netspeed text"""
        output = self.down_speed + "/" + self.up_speed
        
        return output
    
    def swapText(self):
        """function to set swap text"""
        full = self.swap_used + self.swap_free
        mem = 100 * self.swap_used / full
        output = "%5s" % (str(round(mem, 1)))
                
        return output
    
    def connectToEngine(self):
        """function to initializate engine"""
        self.systemmonitor = self.dataEngine("systemmonitor")
        self.systemmonitor.connectSource("cpu/system/TotalLoad", self, 200)
        self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/transmitter/data", self, 200)
        self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/receiver/data", self, 200)
        self.systemmonitor.connectSource(self.tempdev, self, 200)
        self.systemmonitor.connectSource("mem/physical/free", self, 200)
        self.systemmonitor.connectSource("mem/physical/used", self, 200)
        self.systemmonitor.connectSource("mem/physical/application", self, 200)
        self.systemmonitor.connectSource("mem/swap/free", self, 200)
        self.systemmonitor.connectSource("mem/swap/used", self, 200)
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to refresh data"""
        if (sourceName == "cpu/system/TotalLoad"):
            value = str(round(float(data[QString(u'value')]), 1))
            self.cpuText = "%5s" % (value)
        elif (sourceName == "network/interfaces/"+self.netdev+"/transmitter/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            self.up_speed = "%4s" % (value)
        elif (sourceName == "network/interfaces/"+self.netdev+"/reciever/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            self.down_speed = "%4s" % (value)
        elif (sourceName == self.tempdev):
            value = str(round(float(data[QString(u'value')]), 1))
            self.tempText = "%s" % (value)
        elif (sourceName == "mem/physical/free"):
            self.mem_free = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/used"):
            self.mem_uf = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/application"):
            self.mem_used = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/free"):
            self.swap_free = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/used"):
            self.swap_used = float(data[QString(u'value')])
        


def CreateApplet(parent):
	return pyTextWidget(parent)