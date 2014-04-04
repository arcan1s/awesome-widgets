# -*- coding: utf-8 -*-

############################################################################
#   This file is part of pytextmonitor                                     #
#                                                                          #
#   pytextmonitor is free software: you can redistribute it and/or         #
#   modify it under the terms of the GNU General Public License as         #
#   published by the Free Software Foundation, either version 3 of the     #
#   License, or (at your option) any later version.                        #
#                                                                          #
#   pytextmonitor is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#   GNU General Public License for more details.                           #
#                                                                          #
#   You should have received a copy of the GNU General Public License      #
#   along with pytextmonitor. If not, see http://www.gnu.org/licenses/     #
############################################################################

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import *
from PyQt4 import uic



class ConfigWindow(QWidget):
    def __init__(self, parent):
        """settings window definition"""
        QWidget.__init__(self)
        self.ui = uic.loadUi(parent.package().filePath('ui', 'configwindow.ui'), self)
        self.parent = parent
        self.kcolorcombos = {'cpu':self.ui.kcolorcombo_cpu,
            'cpuclock':self.ui.kcolorcombo_cpuclock, 'down':self.ui.kcolorcombo_down,
            'mem':self.ui.kcolorcombo_mem, 'swap':self.ui.kcolorcombo_swap,
            'up':self.ui.kcolorcombo_up}
        self.checkboxes = {'bat':self.ui.checkBox_bat, 'cpu':self.ui.checkBox_cpu,
            'cpuclock':self.ui.checkBox_cpuclock, 'custom':self.ui.checkBox_custom,
            'gpu':self.ui.checkBox_gpu, 'gputemp':self.ui.checkBox_gpuTemp,
            'hdd':self.ui.checkBox_hdd, 'hddtemp':self.ui.checkBox_hddTemp,
            'mem':self.ui.checkBox_mem, 'net':self.ui.checkBox_net,
            'swap':self.ui.checkBox_swap, 'temp':self.ui.checkBox_temp,
            'uptime':self.ui.checkBox_uptime, 'player':self.ui.checkBox_player,
            'time':self.ui.checkBox_time}
        self.lineedits = {'bat':self.ui.lineEdit_bat, 'cpu':self.ui.lineEdit_cpu,
            'cpuclock':self.ui.lineEdit_cpuclock, 'custom':self.ui.lineEdit_custom,
            'gpu':self.ui.lineEdit_gpu, 'gputemp':self.ui.lineEdit_gpuTemp,
            'hdd':self.ui.lineEdit_hdd, 'hddtemp':self.ui.lineEdit_hddTemp,
            'mem':self.ui.lineEdit_mem, 'net':self.ui.lineEdit_net,
            'swap':self.ui.lineEdit_swap, 'temp':self.ui.lineEdit_temp,
            'uptime':self.ui.lineEdit_uptime, 'player':self.ui.lineEdit_player,
            'time':self.ui.lineEdit_time}
        self.sliders = {'bat':self.ui.slider_bat, 'cpu':self.ui.slider_cpu,
            'cpuclock':self.ui.slider_cpuclock, 'custom':self.ui.slider_custom,
            'gpu':self.ui.slider_gpu, 'gputemp':self.ui.slider_gpuTemp,
            'hdd':self.ui.slider_hdd, 'hddtemp':self.ui.slider_hddTemp,
            'mem':self.ui.slider_mem, 'net':self.ui.slider_net,
            'swap':self.ui.slider_swap, 'temp':self.ui.slider_temp,
            'uptime':self.ui.slider_uptime, 'player':self.ui.slider_player,
            'time':self.ui.slider_time}

        QObject.connect(self.ui.checkBox_netdev, SIGNAL("stateChanged(int)"), self.setNetdevEnabled)
        QObject.connect(self.ui.pushButton_hddDevice, SIGNAL("clicked()"), self.addHddDevice)
        QObject.connect(self.ui.pushButton_mount, SIGNAL("clicked()"), self.addMount)
        QObject.connect(self.ui.pushButton_tempDevice, SIGNAL("clicked()"), self.addTempDevice)
        QObject.connect(self.ui.listWidget_hddDevice, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_hddDevice.openPersistentEditor)
        QObject.connect(self.ui.listWidget_mount, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_mount.openPersistentEditor)
        QObject.connect(self.ui.listWidget_tempDevice, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_tempDevice.openPersistentEditor)
        for item in self.sliders.values():
            QObject.connect(item, SIGNAL("valueChanged(int)"), self.setSlider)
        for item in self.checkboxes.values():
            QObject.connect(item, SIGNAL("stateChanged(int)"), self.setStatus)


    def keyPressEvent(self, event):
        """delete events"""
        if (event.key() == Qt.Key_Delete):
            if (self.ui.listWidget_hddDevice.hasFocus() and
                (self.ui.listWidget_hddDevice.currentRow() > -1)):
                self.ui.listWidget_hddDevice.takeItem(self.ui.listWidget_hddDevice.currentRow())
            elif (self.ui.listWidget_mount.hasFocus() and
                (self.ui.listWidget_mount.currentRow() > -1)):
                self.ui.listWidget_mount.takeItem(self.ui.listWidget_mount.currentRow())
            elif (self.ui.listWidget_tempDevice.hasFocus() and
                (self.ui.listWidget_tempDevice.currentRow() > -1)):
                self.ui.listWidget_tempDevice.takeItem(self.ui.listWidget_tempDevice.currentRow())


    def addHddDevice(self):
        """function to add mount points"""
        self.ui.listWidget_hddDevice.clearSelection()
        if (self.ui.listWidget_hddDevice.count() > 9):
            self.ui.listWidget_hddDevice.takeItem(0)
        self.ui.listWidget_hddDevice.addItem(self.ui.comboBox_hddDevice.currentText())


    def addMount(self):
        """function to add mount points"""
        self.ui.listWidget_mount.clearSelection()
        if (self.ui.listWidget_mount.count() > 9):
            self.ui.listWidget_mount.takeItem(0)
        self.ui.listWidget_mount.addItem(self.ui.comboBox_mount.currentText())


    def addTempDevice(self):
        """function to add temperature device"""
        self.ui.listWidget_tempDevice.clearSelection()
        if (self.ui.listWidget_tempDevice.count() > 9):
            self.ui.listWidget_tempDevice.takeItem(0)
        self.ui.listWidget_tempDevice.addItem(self.ui.comboBox_tempDevice.currentText())


    def setNetdevEnabled(self):
        """function to set enabled netdev"""
        if (self.ui.checkBox_netdev.checkState() == 0):
            self.ui.comboBox_netdev.setDisabled(True)
        else:
            self.ui.comboBox_netdev.setEnabled(True)


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


    def setStatus(self):
        """function to enable label"""
        for label in self.checkboxes.keys():
            if ((self.checkboxes[label].checkState() > 0) and (self.sliders[label].isEnabled() == False)):
                self.lineedits[label].setEnabled(True)
                self.sliders[label].setEnabled(True)
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
