# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import *
from PyQt4 import uic
from PyKDE4 import plasmascript



class ConfigWindow(QWidget):
    def __init__(self, parent, settings):
        QWidget.__init__(self)
        self.ui = uic.loadUi(parent.package().filePath('ui', 'configwindow.ui'), self)
        self.parent = parent
        
        QObject.connect(self.ui.checkBox_bat, SIGNAL("stateChanged(int)"), self.batStatus)
        QObject.connect(self.ui.checkBox_cpu, SIGNAL("stateChanged(int)"), self.cpuStatus)
        QObject.connect(self.ui.checkBox_mem, SIGNAL("stateChanged(int)"), self.memStatus)
        QObject.connect(self.ui.checkBox_net, SIGNAL("stateChanged(int)"), self.netStatus)
        QObject.connect(self.ui.checkBox_swap, SIGNAL("stateChanged(int)"), self.swapStatus)
        QObject.connect(self.ui.checkBox_temp, SIGNAL("stateChanged(int)"), self.tempStatus)
        
    def batStatus(self):
        if (self.ui.checkBox_bat.checkState() == 2):
            self.ui.lineEdit_bat.setEnabled(True)
        else:
            self.ui.lineEdit_bat.setDisabled(True)
    
    def cpuStatus(self):
        if (self.ui.checkBox_cpu.checkState() == 2):
            self.ui.lineEdit_cpu.setEnabled(True)
        else:
            self.ui.lineEdit_cpu.setDisabled(True)
    
    def memStatus(self):
        if (self.ui.checkBox_mem.checkState() == 2):
            self.ui.lineEdit_mem.setEnabled(True)
        else:
            self.ui.lineEdit_mem.setDisabled(True)
    
    def netStatus(self):
        if (self.ui.checkBox_net.checkState() == 2):
            self.ui.lineEdit_net.setEnabled(True)
            self.ui.comboBox_numNet.setEnabled(True)
        else:
            self.ui.lineEdit_net.setDisabled(True)
            self.ui.comboBox_numNet.setDisabled(True)
    
    def swapStatus(self):
        if (self.ui.checkBox_swap.checkState() == 2):
            self.ui.lineEdit_swap.setEnabled(True)
        else:
            self.ui.lineEdit_swap.setDisabled(True)
    
    def tempStatus(self):
        if (self.ui.checkBox_temp.checkState() == 2):
            self.ui.lineEdit_temp.setEnabled(True)
        else:
            self.ui.lineEdit_temp.setDisabled(True)