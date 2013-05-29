# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyKDE4.kdecore import *
from PyKDE4 import plasmascript
import commands



class ExtendedSysMon(plasmascript.DataEngine):
    def __init__(self, parent, args=None):
        """dataengine definition"""
        plasmascript.DataEngine.__init__(self, parent)
 
 
    def init(self):
        """initialization"""
        self.setMinimumPollingInterval(333)
        
        # setup gpu device
        self.gpudev = ''
        commandOut = commands.getoutput("lspci")
        if (commandOut.lower().find('nvidia') > -1):
            self.gpudev = 'nvidia'
        elif (commandOut.lower().find('radeon') > -1):
            self.gpudev = 'ati'
        
        # setup hdd devices
        self.hdddev = []
        commandOut = commands.getoutput("ls -1 /dev/sd[a-z] && ls -1 /dev/hd[a-z]")
        for device in commandOut.split('\n'):
            if (device[:3] != "ls:"):
                self.hdddev.append(device)
    
    
    def sources(self):
        """create sources"""
        sources = ["gpu", "gputemp", "hddtemp"]
        return sources
    
    
    def sourceRequestEvent(self, name):
        return self.updateSourceEvent(name)
    
    
    def updateSourceEvent(self, source):
        """update sources and setup values"""
        if (source == "gpu"):
            key = "GPU"
            if (self.gpudev == 'nvidia'):
                commandOut = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
                try:
                    value = "%5s" % (str(round(float(commandOut.split()[2][:-1]), 1)))
                except:
                    value = "  N\A"
            elif (self.gpudev == 'ati'):
                commandOut = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
                try:
                    value = "%5s" % (str(round(float(commandOut.split()[3][:-1]), 1)))
                except:
                    value = "  N\A"
            else:
                value = "  N\A"
            self.setData(source, "GPU", QString(value))
        elif (source == "gputemp"):
            if (self.gpudev == 'nvidia'):
                commandOut = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
                try:
                    value = "%4s" % (str(round(float(commandOut.split()[2]), 1)))
                except:
                    value = " N\A"
            elif (self.gpudev == 'ati'):
                commandOut = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
                try:
                    value = "%4s" % (str(round(float(commandOut.split()[4]), 1)))
                except:
                    value = " N\A"
            else:
                value = " N\A"
            self.setData(source, "GPUTemp", QString(value))
        elif (source == "hddtemp"):
            for device in self.hdddev:
                commandOut = commands.getoutput("hddtemp " + device)
                try:
                    value = "%4s" % (str(round(float(commandOut.split(':')[2][:-3]), 1)))
                except:
                    value = " N\A"
                self.setData(source, device, QString(value))
 
        return True



def CreateDataEngine(parent):
    return ExtendedSysMon(parent)
