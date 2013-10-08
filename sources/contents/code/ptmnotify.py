# -*- coding: utf-8 -*-

from PyKDE4.kdecore import KComponentData
from PyKDE4.kdeui import KNotification
from PyKDE4.plasma import Plasma
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
        text = ""
        if (type == "system"):
            try:
                text = text + "Kernel: " + commands.getoutput("uname -rsm") + "\n"
                text = text + "Hostname: " + commands.getoutput("uname -n") + "\n"
                text = text + "Whoami: " + commands.getoutput("whoami") + "\n"
                text = text + "Uptime: " + commands.getoutput("uptime")
            except:
                pass
        elif (type == "processor"):
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
        elif (type == "graphical"):
            try:
                output = commands.getoutput("lspci -m | grep 'VGA\|3D'")
                if (output.lower().find('nvidia') > -1):
                    gpudev = "nvidia"
                elif (output.lower().find('radeon') > -1):
                    gpudev = "ati"
                for line in output.split("\n"):
                    text = text + line.split('"')[0] + " "
                    text = text + line.split('"')[5] + "\n"
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[2][:-1]), 1)))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[3][:-1]), 1)))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Load: " + value + "%\n"
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[2][:-1]), 1)))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[3][:-1]), 1)))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Temp: " + value + "\xb0C"
            except:
                pass
        elif (type == "memory"):
            try:
                output = commands.getoutput("free -mo").split("\n")
                memusage = int(output[1].split()[1]) - (int(output[1].split()[3]) + int(output[1].split()[5]) + int(output[1].split()[6]))
                text = text + "Memory: " + str(memusage) + " of " + output[1].split()[1] + " (" + str(int(100*memusage/int(output[1].split()[1]))) + "%)\n"
                text = text + "Swap: " + output[2].split()[2] + " of " + output[2].split()[1] + " (" + str(int(100*int(output[2].split()[2])/int(output[2].split()[1]))) + "%)\n"
                output = commands.getoutput("swapon --show").split("\n")
                text = text + "Swap Device: " + output[1].split()[0] + " (" + output[1].split()[1] + ")"
            except:
                pass
        
        content = [type, text]
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
                content = self.createText("disk")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_hddtemp):
                content = self.createText("disk")
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
                content = self.createText("battery")
                return content
        except:
            pass
