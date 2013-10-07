# -*- coding: utf-8 -*-

from PyKDE4.kdecore import KComponentData
from PyKDE4.kdeui import KNotification
import commands



class PTMNotify:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def init(self):
        """function to init notification"""
        content = self.initText(self.parent)
        self.createNotify(content)
    
    
    def createNotify(self, content):
        """function to create notification for label"""
        notification = KNotification(content[0])
        notification.setComponentData(KComponentData("plasma_applet_pytextmonitor"))
        notification.setTitle("PyTextMonitor info ::: " + content[0]);
        notification.setText(content[1]);
        notification.sendEvent();
    
    
    def createText(self, type):
        """function to create text"""
        content = []
        if (type == "system"):
            content.append("system")
            text = ""
            try:
                text = text + "Kernel: " + commands.getoutput("uname -rsm") + "\n"
                text = text + "Hostname: " + commands.getoutput("uname -n") + "\n"
                text = text + "Whoami: " + commands.getoutput("whoami") + "\n"
                text = text + "Uptime: " + commands.getoutput("uptime")
            except:
                pass
            content.append(text)
        elif (type == "processor"):
            content.append("processor")
            text = ""
            try:
                output = commands.getoutput("grep 'model name' /proc/cpuinfo | head -1")
                text = text + "Model: " + ' '.join(output.split()[3:]) + "\n"
                output = commands.getoutput("sar -u | tail -1")
                text = text + "CPU Usage: " + str(100-float(output[-1])) + "%\n"
                output = commands.getoutput("grep MHz /proc/cpuinfo | head -1")
                text = text + "CPU Freq: " + str(int(float(output.split()[-1]))) + " MHz\n"
                output = commands.getoutput("sensors -u")
                text = text + "Temps:"
                for line in output.split("\n"):
                    if (line.find("_input") > -1):
                        text = text + " " + str(round(float(line.split()[-1]), 0)) + "\xb0C"
            except:
                pass
            content.append(text)
        
        return content
    
    
    def initText(self, sender):
        """function to send text"""
        try:
            if (sender == self.parent.parent.label_uptime):
                content = self.createText("system")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_cpu):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_cpuclock):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_temp):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_gpu):
                content = self.createText("graphical")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_gputemp):
                content = self.createText("graphical")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_mem):
                content = self.createText("memory")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_swap):
                content = self.createText("memory")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_hdd0):
                content = self.createText("hdd")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_hddtemp):
                content = self.createText("hdd")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_netDown):
                content = self.createText("network")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_bat):
                content = self.createText("acpi")
                return content
        except:
            pass
