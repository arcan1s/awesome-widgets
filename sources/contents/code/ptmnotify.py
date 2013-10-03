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
    
    
    def initText(self, sender):
        """function to create text"""
        content = []
        if (sender == self.parent.parent.label_uptime):
            content.append("system")
            text = "Kernel: " + commands.getoutput("uname -rsm") + "\n"
            text = text + "Hostname: " + commands.getoutput("uname -n") + "\n"
            text = text + "Whoami: " + commands.getoutput("whoami") + "\n"
            text = text + "Uptime: " + commands.getoutput("uptime")
            content.append(text)
        return content
