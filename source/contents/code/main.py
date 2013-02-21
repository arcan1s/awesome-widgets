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
        
        #initial configuration
        self.settings = Config(self)
        self.format = str(self.settings.get('format', '[cpu: $cpu%][temp: $temp&deg;C][mem: $mem%][swap: $swap%][ip: $ip][net: $netKB/s][bat: $bat%]'))
        self.interval = int(self.settings.get('interval', '2000'))
        self.font_family = str(self.settings.get('font_family', 'Terminus'))
        self.font_size = int(self.settings.get('font_size', 12))
        self.font_color = str(self.settings.get('font_color', 'black'))
        self.font_style = str(self.settings.get('font_style', 'normal'))
        self.num_dev = int(self.settings.get('num_dev', 0))
        
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
        self.font_color = str(self.configpage.ui.lineEdit_color.text())
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
        self.configpage.ui.lineEdit_format.setText(str(self.settings.get('format', '[cpu: $cpu%][temp: $temp&deg;C][mem: $mem%][swap: $swap%][ip: $ip][net: $netKB/s][bat: $bat%]')))
        self.configpage.ui.spinBox_interval.setValue(int(self.settings.get('interval', '2000')))
        self.configpage.ui.fontComboBox.setCurrentFont(font)
        self.configpage.ui.spinBox_fontSize.setValue(int(self.settings.get('font_size', 12)))
        self.configpage.ui.lineEdit_color.setText(str(self.settings.get('font_color', 'black')))
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
            output = output.split("$cpu")[0] + self.cpuText() + output.split("$cpu")[1]
        if (output.split("$temp")[0] != output):
            output = output.split("$temp")[0] + self.tempText() + output.split("$temp")[1]
        if (output.split("$mem")[0] != output):
            output = output.split("$mem")[0] + self.memText() + output.split("$mem")[1]
        if (output.split("$swap")[0] != output):
            output = output.split("$swap")[0] + self.swapText() + output.split("$swap")[1]
        if (output.split("$ip")[0] != output):
            output = output.split("$ip")[0] + self.ipText() + output.split("$ip")[1]
        if (output.split("$net")[0] != output):
            output = output.split("$net")[0] + self.netText() + output.split("$net")[1]
        if (output.split("$bat")[0] != output):
            output = output.split("$bat")[0] + self.batText() + output.split("$bat")[1]
        
        return output
    
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
        #string = self.cpuText() + self.tempText()
        #string = string + self.memText() + self.swapText()
        #string = string + self.ipText() + self.netText() + self.batText()
        string = self.formating_line()
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
    
    def cpuText(self):
        """function to set cpu text"""
        with open ("/proc/stat", 'r') as stat:
            for line in stat:
                if (line[0:4] == "cpu "):
                    cpu_1 = line.split()[1:5]
        time.sleep(0.2)
        with open ("/proc/stat", 'r') as stat:
            for line in stat:
                if (line[0:4] == "cpu "):
                    cpu_2 = line.split()[1:5]
        duser = float(cpu_2[0]) - float(cpu_1[0])
        dnice = float(cpu_2[1]) - float(cpu_1[1])
        dsys = float(cpu_2[2]) - float(cpu_1[2])
        didle = float(cpu_2[3]) - float(cpu_1[3])
        full = duser + dnice + dsys + didle
        cpu = (full-didle) / full * 100
        output = "%5s" % (str(round(cpu, 1)))
        
        return output
    
    def ipText(self):
        """function to set ip text"""
        command_line = "wget http://checkip.dyndns.org/ -q -O - | awk '{print $6}' | sed 's/<.*>//g'"
        ip = commands.getoutput(command_line)[:-1].split('.')
        output = "%3s.%3s.%3s.%3s" % (str(ip[0]), str(ip[1]), str(ip[2]), str(ip[3]))
        
        return output
    
    def memText(self):
        """function to set mem text"""
        line = commands.getoutput("free | grep Mem:")
        used = float(line.split()[2]) - float(line.split()[4]) - float(line.split()[5]) - float(line.split()[6])
        full = float(line.split()[1])
        mem = 100 * used / full
        output = "%5s" % (str(round(mem, 1)))
                
        return output
    
    def netText(self):
        """function to set netspeed text"""
        if (self.num_dev == 0):
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        netdev = line.split()[0][:-1]
        else:
            interfaces = []
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        interfaces.append(line.split()[0][:-1])
                    
            command_line = "if ! (ifconfig "+ interfaces[1] + " | grep 'inet ' > /dev/null); then "
            command_line = command_line + "if ! (ifconfig "+ interfaces[0] + " | grep 'inet ' > /dev/null); then echo lo; "
            command_line = command_line + "else echo "+ interfaces[0] + "; fi; else echo "+ interfaces[1] + "; fi"
            netdev = commands.getoutput(command_line)
        
        command_line = "RXB=$(cat /sys/class/net/" + netdev
        command_line = command_line + "/statistics/rx_bytes) && TXB=$(cat /sys/class/net/" + netdev
        command_line = command_line + "/statistics/tx_bytes) && sleep 0.2 && RXBN=$(cat /sys/class/net/" + netdev
        command_line = command_line + "/statistics/rx_bytes) && TXBN=$(cat /sys/class/net/" + netdev
        command_line = command_line + "/statistics/tx_bytes) && RXDIF=$(echo $((RXBN - RXB)) ) && TXDIF=$(echo $((TXBN - TXB)) ) "
        command_line = command_line + "&& echo -en $((10*RXDIF/1024/2)) && echo -n ' / ' && echo -en $((10*TXDIF/1024/2)) && echo ' KB/s'"
        speed = commands.getoutput(command_line)
        
        downSpeed = speed.split()[0]
        upSpeed = speed.split()[2]
        output = "%4s/%4s" % (downSpeed, upSpeed)
        
        return output
    
    def swapText(self):
        """function to set swap text"""
        line = commands.getoutput("free | grep Swap:")
        used = float(line.split()[2])
        full = float(line.split()[1])
        mem = 100 * used / full
        output = "%5s" % (str(round(mem, 1)))
        
        return output
    
    def tempText(self):
        """function to set temp text"""
        commandOut = commands.getoutput("sensors | grep Physical")
        temp = commandOut[17:21]
        output = "%4s" % (temp)
        
        return output



def CreateApplet(parent):
	return pyTextWidget(parent)