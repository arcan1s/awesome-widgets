# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import *
from PyQt4 import uic
from PyKDE4 import plasmascript



class ConfigWindow(QWidget):
    def __init__(self, parent):
        """settings window definition"""
        QWidget.__init__(self)
        self.ui = uic.loadUi(parent.package().filePath('ui', 'configwindow.ui'), self)
        self.parent = parent
        self.checkboxes = {'bat':self.ui.checkBox_bat, 'cpu':self.ui.checkBox_cpu, 
        'cpuclock':self.ui.checkBox_cpuclock, 'gpu':self.ui.checkBox_gpu, 
        'gputemp':self.ui.checkBox_gpuTemp, 'hdd':self.ui.checkBox_hdd, 
        'hddtemp':self.ui.checkBox_hddTemp, 'mem':self.ui.checkBox_mem, 
        'net':self.ui.checkBox_net, 'swap':self.ui.checkBox_swap, 
        'temp':self.ui.checkBox_temp, 'uptime':self.ui.checkBox_uptime}
        self.sliders = {'bat':self.ui.slider_bat, 'cpu':self.ui.slider_cpu, 
        'cpuclock':self.ui.slider_cpuclock, 'gpu':self.ui.slider_gpu, 
        'gputemp':self.ui.slider_gpuTemp, 'hdd':self.ui.slider_hdd, 
        'hddtemp':self.ui.slider_hddTemp, 'mem':self.ui.slider_mem, 
        'net':self.ui.slider_net, 'swap':self.ui.slider_swap, 
        'temp':self.ui.slider_temp, 'uptime':self.ui.slider_uptime}
        self.lineedits = {'bat':self.ui.lineEdit_bat, 'cpu':self.ui.lineEdit_cpu, 
        'cpuclock':self.ui.lineEdit_cpuclock, 'gpu':self.ui.lineEdit_gpu, 
        'gputemp':self.ui.lineEdit_gpuTemp, 'hdd':self.ui.lineEdit_hdd, 
        'hddtemp':self.ui.lineEdit_hddTemp, 'mem':self.ui.lineEdit_mem, 
        'net':self.ui.lineEdit_net, 'swap':self.ui.lineEdit_swap, 
        'temp':self.ui.lineEdit_temp, 'uptime':self.ui.lineEdit_uptime}
        
        for item in self.checkboxes.values():
            QObject.connect(item, SIGNAL("stateChanged(int)"), self.setStatus)
        for item in self.sliders.values():
            QObject.connect(item, SIGNAL("valueChanged(int)"), self.setSlider)

    
    def setStatus(self):
        """function to enable label"""
        count = self.sliders['bat'].maximum()
        for label in self.checkboxes.keys():
            if ((self.checkboxes[label].checkState() == 2) and (self.sliders[label].isEnabled() == False)):
                self.lineedits[label].setEnabled(True)
                self.sliders[label].setEnabled(True)
                if (label == 'bat'):
                    self.ui.lineEdit_acdev.setEnabled(True)
                    self.ui.lineEdit_batdev.setEnabled(True)
                elif (label == 'net'):
                    self.ui.comboBox_numNet.setEnabled(True)
                slider_label = 0
                for slider in self.sliders.values():
                    if (slider.isEnabled() == True):
                        slider_label += 1
                for slider in self.sliders.values():
                    if (slider_label > 1):
                        slider.setMaximum(slider.maximum()+1)
                    elif (slider_label == 1):
                        slider.setMaximum(1)
                self.sliders[label].setValue(self.sliders[label].maximum())
            elif ((self.checkboxes[label].checkState() == 0) and (self.sliders[label].isEnabled() == True)):
                self.lineedits[label].setDisabled(True)
                self.sliders[label].setDisabled(True)
                if (label == 'bat'):
                    self.ui.lineEdit_acdev.setDisabled(True)
                    self.ui.lineEdit_batdev.setDisabled(True)
                elif (label == 'net'):
                    self.ui.comboBox_numNet.setDisabled(True)
                for slider in self.sliders.values():
                    if ((slider.value() == slider.maximum()) and (slider != self.sliders[label])):
                        slider.setValue(self.sliders[label].value())
                slider_label = 0
                for slider in self.sliders.values():
                    if (slider.isEnabled() == True):
                        slider_label += 1
                for slider in self.sliders.values():
                    if (slider_label > 0):
                        slider.setMaximum(slider.maximum()-1)
                    else:
                        slider.setMaximum(1)
                self.sliders[label].setValue(1)
    
    def setSlider(self):
        """function to set sliders"""
        if (self.sender().isEnabled() == True):
            second_slider = self.sender()
            order = []
            for slider in self.sliders.values():
                if (slider.isEnabled() == True):
                    order.append(slider.value())
                    if ((slider.value() == self.sender().value()) and (slider != self.sender())):
                        second_slider = slider
            
            if (second_slider == self.sender()):
                return
            
            for value in range(len(order)):
                if (order.count(value+1) == 0):
                    new_value = value + 1
            
            second_slider.setValue(new_value)
