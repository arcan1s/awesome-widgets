# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import *
from PyQt4 import uic
from PyKDE4 import plasmascript



class ConfigWindow(QWidget):
    def __init__(self, parent, settings):
        """settings window definition"""
        QWidget.__init__(self)
        self.ui = uic.loadUi(parent.package().filePath('ui', 'configwindow.ui'), self)
        self.parent = parent
        
        QObject.connect(self.ui.checkBox_bat, SIGNAL("stateChanged(int)"), self.batStatus)
        QObject.connect(self.ui.checkBox_cpu, SIGNAL("stateChanged(int)"), self.cpuStatus)
        QObject.connect(self.ui.checkBox_cpuclock, SIGNAL("stateChanged(int)"), self.cpuclockStatus)
        QObject.connect(self.ui.checkBox_gpuMem, SIGNAL("stateChanged(int)"), self.gpuMemStatus)
        QObject.connect(self.ui.checkBox_gpuTemp, SIGNAL("stateChanged(int)"), self.gpuTempStatus)
        QObject.connect(self.ui.checkBox_hdd, SIGNAL("stateChanged(int)"), self.hddStatus)
        QObject.connect(self.ui.checkBox_hddTemp, SIGNAL("stateChanged(int)"), self.hddTempStatus)
        QObject.connect(self.ui.checkBox_mem, SIGNAL("stateChanged(int)"), self.memStatus)
        QObject.connect(self.ui.checkBox_net, SIGNAL("stateChanged(int)"), self.netStatus)
        QObject.connect(self.ui.checkBox_swap, SIGNAL("stateChanged(int)"), self.swapStatus)
        QObject.connect(self.ui.checkBox_temp, SIGNAL("stateChanged(int)"), self.tempStatus)
        QObject.connect(self.ui.checkBox_uptime, SIGNAL("stateChanged(int)"), self.uptimeStatus)
        
        QObject.connect(self.ui.slider_bat, SIGNAL("valueChanged(int)"), self.setBat)
        QObject.connect(self.ui.slider_cpu, SIGNAL("valueChanged(int)"), self.setCpu)
        QObject.connect(self.ui.slider_cpuclock, SIGNAL("valueChanged(int)"), self.setCpuclock)
        QObject.connect(self.ui.slider_gpuMem, SIGNAL("valueChanged(int)"), self.setGpuMem)
        QObject.connect(self.ui.slider_gpuTemp, SIGNAL("valueChanged(int)"), self.setGpuTemp)
        QObject.connect(self.ui.slider_hdd, SIGNAL("valueChanged(int)"), self.setHdd)
        QObject.connect(self.ui.slider_hddTemp, SIGNAL("valueChanged(int)"), self.setHddTemp)
        QObject.connect(self.ui.slider_mem, SIGNAL("valueChanged(int)"), self.setMem)
        QObject.connect(self.ui.slider_net, SIGNAL("valueChanged(int)"), self.setNet)
        QObject.connect(self.ui.slider_swap, SIGNAL("valueChanged(int)"), self.setSwap)
        QObject.connect(self.ui.slider_temp, SIGNAL("valueChanged(int)"), self.setTemp)
        QObject.connect(self.ui.slider_uptime, SIGNAL("valueChanged(int)"), self.setUptime)
        
    def batStatus(self):
        """function to enable battery label"""
        if (self.ui.checkBox_bat.checkState() == 2):
            self.ui.lineEdit_bat.setEnabled(True)
            self.ui.slider_bat.setEnabled(True)
            self.ui.lineEdit_acdev.setEnabled(True)
            self.ui.lineEdit_batdev.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_bat.setDisabled(True)
            self.ui.slider_bat.setDisabled(True)
            self.ui.lineEdit_acdev.setDisabled(True)
            self.ui.lineEdit_batdev.setDisabled(True)
            self.setSlider()
    
    def cpuStatus(self):
        """function to enable cpu label"""
        if (self.ui.checkBox_cpu.checkState() == 2):
            self.ui.lineEdit_cpu.setEnabled(True)
            self.ui.slider_cpu.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_cpu.setDisabled(True)
            self.ui.slider_cpu.setDisabled(True)
            self.setSlider()
    
    def cpuclockStatus(self):
        """function to enable cpuclock label"""
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            self.ui.lineEdit_cpuclock.setEnabled(True)
            self.ui.slider_cpuclock.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_cpuclock.setDisabled(True)
            self.ui.slider_cpuclock.setDisabled(True)
            self.setSlider()
    
    def gpuMemStatus(self):
        """function to enable gpu memory label"""
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            self.ui.lineEdit_gpuMem.setEnabled(True)
            self.ui.slider_gpuMem.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_gpuMem.setDisabled(True)
            self.ui.slider_gpuMem.setDisabled(True)
            self.setSlider()
    
    def gpuTempStatus(self):
        """function to enable gpu temperature label"""
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            self.ui.lineEdit_gpuTemp.setEnabled(True)
            self.ui.slider_gpuTemp.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_gpuTemp.setDisabled(True)
            self.ui.slider_gpuTemp.setDisabled(True)
            self.setSlider()
    
    def hddStatus(self):
        """function to enable hdd label"""
        if (self.ui.checkBox_hdd.checkState() == 2):
            self.ui.lineEdit_hdd.setEnabled(True)
            self.ui.slider_hdd.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_hdd.setDisabled(True)
            self.ui.slider_hdd.setDisabled(True)
            self.setSlider()
    
    def hddTempStatus(self):
        """function to enable hdd temperature label"""
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            self.ui.lineEdit_hddTemp.setEnabled(True)
            self.ui.slider_hddTemp.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_hddTemp.setDisabled(True)
            self.ui.slider_hddTemp.setDisabled(True)
            self.setSlider()
    
    def memStatus(self):
        """function to enable memory label"""
        if (self.ui.checkBox_mem.checkState() == 2):
            self.ui.lineEdit_mem.setEnabled(True)
            self.ui.slider_mem.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_mem.setDisabled(True)
            self.ui.slider_mem.setDisabled(True)
            self.setSlider()
    
    def netStatus(self):
        """function to enable network label"""
        if (self.ui.checkBox_net.checkState() == 2):
            self.ui.lineEdit_net.setEnabled(True)
            self.ui.comboBox_numNet.setEnabled(True)
            self.ui.slider_net.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_net.setDisabled(True)
            self.ui.comboBox_numNet.setDisabled(True)
            self.ui.slider_net.setDisabled(True)
            self.setSlider()
    
    def swapStatus(self):
        """function to enable swap label"""
        if (self.ui.checkBox_swap.checkState() == 2):
            self.ui.lineEdit_swap.setEnabled(True)
            self.ui.slider_swap.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_swap.setDisabled(True)
            self.ui.slider_swap.setDisabled(True)
            self.setSlider()
    
    def tempStatus(self):
        """function to enable temperature label"""
        if (self.ui.checkBox_temp.checkState() == 2):
            self.ui.lineEdit_temp.setEnabled(True)
            self.ui.slider_temp.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_temp.setDisabled(True)
            self.ui.slider_temp.setDisabled(True)
            self.setSlider()
    
    def uptimeStatus(self):
        """function to enable uptime label"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            self.ui.lineEdit_uptime.setEnabled(True)
            self.ui.slider_uptime.setEnabled(True)
            self.setSlider()
        else:
            self.ui.lineEdit_uptime.setDisabled(True)
            self.ui.slider_uptime.setDisabled(True)
            self.setSlider()
    
    def setBat(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_bat.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_bat.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_bat.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_bat.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_bat.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_bat.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_bat.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_bat.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_bat.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_bat.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_bat.value()):
                self.ui.slider_net.setValue(self.oldValue())
    
    def setCpu(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_cpu.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_cpu.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_cpu.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_cpu.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_cpu.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_cpu.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_cpu.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_cpu.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_cpu.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_cpu.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_cpu.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setCpuclock(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_cpuclock.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setGpuMem(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_gpuMem.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setGpuTemp(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_gpuTemp.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setHdd(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_hdd.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_hdd.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_hdd.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_hdd.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_hdd.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_hdd.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_hdd.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_hdd.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_hdd.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_hdd.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_hdd.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setHddTemp(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_hddTemp.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setMem(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_mem.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_mem.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_mem.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_mem.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_mem.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_mem.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_mem.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_mem.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_mem.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_mem.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_mem.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setNet(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_net.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_net.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_net.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_net.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_net.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_net.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_net.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_net.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_net.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_net.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_net.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setSwap(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_swap.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_swap.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_swap.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_swap.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_swap.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_swap.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_swap.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_swap.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_swap.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_swap.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_swap.value()):
                self.ui.slider_bat.setValue(self.oldValue())
        
    def setTemp(self):
        """function to set sliders"""
        if (self.ui.checkBox_uptime.checkState() == 2):
            if (self.ui.slider_uptime.value() == self.ui.slider_temp.value()):
                self.ui.slider_uptime.setValue(self.oldValue())
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_temp.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_temp.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_temp.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_temp.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_temp.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_temp.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_temp.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_temp.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_temp.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_temp.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setUptime(self):
        """function to set sliders"""
        if (self.ui.checkBox_cpu.checkState() == 2):
            if (self.ui.slider_cpu.value() == self.ui.slider_uptime.value()):
                self.ui.slider_cpu.setValue(self.oldValue())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            if (self.ui.slider_cpuclock.value() == self.ui.slider_uptime.value()):
                self.ui.slider_cpuclock.setValue(self.oldValue())
        if (self.ui.checkBox_temp.checkState() == 2):
            if (self.ui.slider_temp.value() == self.ui.slider_uptime.value()):
                self.ui.slider_temp.setValue(self.oldValue())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            if (self.ui.slider_gpuMem.value() == self.ui.slider_uptime.value()):
                self.ui.slider_gpuMem.setValue(self.oldValue())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            if (self.ui.slider_gpuTemp.value() == self.ui.slider_uptime.value()):
                self.ui.slider_gpuTemp.setValue(self.oldValue())
        if (self.ui.checkBox_mem.checkState() == 2):
            if (self.ui.slider_mem.value() == self.ui.slider_uptime.value()):
                self.ui.slider_mem.setValue(self.oldValue())
        if (self.ui.checkBox_swap.checkState() == 2):
            if (self.ui.slider_swap.value() == self.ui.slider_uptime.value()):
                self.ui.slider_swap.setValue(self.oldValue())
        if (self.ui.checkBox_hdd.checkState() == 2):
            if (self.ui.slider_hdd.value() == self.ui.slider_uptime.value()):
                self.ui.slider_hdd.setValue(self.oldValue())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            if (self.ui.slider_hddTemp.value() == self.ui.slider_uptime.value()):
                self.ui.slider_hddTemp.setValue(self.oldValue())
        if (self.ui.checkBox_net.checkState() == 2):
            if (self.ui.slider_net.value() == self.ui.slider_uptime.value()):
                self.ui.slider_net.setValue(self.oldValue())
        if (self.ui.checkBox_bat.checkState() == 2):
            if (self.ui.slider_bat.value() == self.ui.slider_uptime.value()):
                self.ui.slider_bat.setValue(self.oldValue())
    
    def setSlider(self):
        """function to reset sliders"""
        self.ui.slider_bat.setMaximum(12)
        self.ui.slider_cpu.setMaximum(12)
        self.ui.slider_cpuclock.setMaximum(12)
        self.ui.slider_gpuMem.setMaximum(12)
        self.ui.slider_gpuTemp.setMaximum(12)
        self.ui.slider_hdd.setMaximum(12)
        self.ui.slider_hddTemp.setMaximum(12)
        self.ui.slider_net.setMaximum(12)
        self.ui.slider_swap.setMaximum(12)
        self.ui.slider_temp.setMaximum(12)
        self.ui.slider_uptime.setMaximum(12)
        count = 0
        if (self.ui.checkBox_bat.checkState() == 2):
            count += 1
            self.ui.slider_bat.setValue(count)
        if (self.ui.checkBox_cpu.checkState() == 2):
            count += 1
            self.ui.slider_cpu.setValue(count)
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            count += 1
            self.ui.slider_cpuclock.setValue(count)
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            count += 1
            self.ui.slider_gpuMem.setValue(count)
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            count += 1
            self.ui.slider_gpuTemp.setValue(count)
        if (self.ui.checkBox_hdd.checkState() == 2):
            count += 1
            self.ui.slider_hdd.setValue(count)
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            count += 1
            self.ui.slider_hddTemp.setValue(count)
        if (self.ui.checkBox_mem.checkState() == 2):
            count += 1
            self.ui.slider_mem.setValue(count)
        if (self.ui.checkBox_net.checkState() == 2):
            count += 1
            self.ui.slider_net.setValue(count)
        if (self.ui.checkBox_swap.checkState() == 2):
            count += 1
            self.ui.slider_swap.setValue(count)
        if (self.ui.checkBox_temp.checkState() == 2):
            count += 1
            self.ui.slider_temp.setValue(count)
        if (self.ui.checkBox_uptime.checkState() == 2):
            count += 1
            self.ui.slider_uptime.setValue(count)
        self.ui.slider_bat.setMaximum(count)
        self.ui.slider_cpu.setMaximum(count)
        self.ui.slider_cpuclock.setMaximum(count)
        self.ui.slider_gpuMem.setMaximum(count)
        self.ui.slider_gpuTemp.setMaximum(count)
        self.ui.slider_hdd.setMaximum(count)
        self.ui.slider_hddTemp.setMaximum(count)
        self.ui.slider_mem.setMaximum(count)
        self.ui.slider_net.setMaximum(count)
        self.ui.slider_swap.setMaximum(count)
        self.ui.slider_temp.setMaximum(count)
        self.ui.slider_uptime.setMaximum(count)
    
    def oldValue(self):
        """function for set* functions"""
        order = []
        if (self.ui.checkBox_bat.checkState() == 2):
            order.append(self.ui.slider_bat.value())
        if (self.ui.checkBox_cpu.checkState() == 2):
            order.append(self.ui.slider_cpu.value())
        if (self.ui.checkBox_cpuclock.checkState() == 2):
            order.append(self.ui.slider_cpuclock.value())
        if (self.ui.checkBox_gpuMem.checkState() == 2):
            order.append(self.ui.slider_gpuMem.value())
        if (self.ui.checkBox_gpuTemp.checkState() == 2):
            order.append(self.ui.slider_gpuTemp.value())
        if (self.ui.checkBox_hdd.checkState() == 2):
            order.append(self.ui.slider_hdd.value())
        if (self.ui.checkBox_hddTemp.checkState() == 2):
            order.append(self.ui.slider_hddTemp.value())
        if (self.ui.checkBox_mem.checkState() == 2):
            order.append(self.ui.slider_mem.value())
        if (self.ui.checkBox_net.checkState() == 2):
            order.append(self.ui.slider_net.value())
        if (self.ui.checkBox_swap.checkState() == 2):
            order.append(self.ui.slider_swap.value())
        if (self.ui.checkBox_temp.checkState() == 2):
            order.append(self.ui.slider_temp.value())
        if (self.ui.checkBox_uptime.checkState() == 2):
            order.append(self.ui.slider_uptime.value())
        
        for value in range(len(order)+1)[1:]:
            if (order.count(value) == 0):
                return int(value)
