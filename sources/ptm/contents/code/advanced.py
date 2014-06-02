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



class AdvancedWindow(QWidget):
    def __init__(self, parent):
        """settings window definition"""
        # debug
        environment = QProcessEnvironment.systemEnvironment()
        debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
        if (debugEnv == QString("yes")):
            self.debug = True
        else:
            self.debug = False
        # main
        QWidget.__init__(self)
        self.ui = uic.loadUi(parent.package().filePath('ui', 'advanced.ui'), self)
        self.parent = parent

        QObject.connect(self.ui.checkBox_netdev, SIGNAL("stateChanged(int)"), self.setNetdevEnabled)
        QObject.connect(self.ui.pushButton_hddDevice, SIGNAL("clicked()"), self.addHddDevice)
        QObject.connect(self.ui.pushButton_hddSpeedDevice, SIGNAL("clicked()"), self.addHddSpeedDevice)
        QObject.connect(self.ui.pushButton_mount, SIGNAL("clicked()"), self.addMount)
        QObject.connect(self.ui.pushButton_tempDevice, SIGNAL("clicked()"), self.addTempDevice)
        QObject.connect(self.ui.listWidget_hddDevice, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_hddDevice.openPersistentEditor)
        QObject.connect(self.ui.listWidget_hddSpeedDevice, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_hddSpeedDevice.openPersistentEditor)
        QObject.connect(self.ui.listWidget_mount, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_mount.openPersistentEditor)
        QObject.connect(self.ui.listWidget_tempDevice, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_tempDevice.openPersistentEditor)


    def keyPressEvent(self, event):
        """delete events"""
        if self.debug: qDebug("[PTM] [advanced.py] [keyPressEvent]")
        if self.debug: qDebug("[PTM] [advanced.py] [keyPressEvent] : Run function with event '%s'" %(event.key()))
        if (event.key() == Qt.Key_Delete):
            if (self.ui.listWidget_hddDevice.hasFocus() and
                (self.ui.listWidget_hddDevice.currentRow() > -1)):
                self.ui.listWidget_hddDevice.takeItem(self.ui.listWidget_hddDevice.currentRow())
            elif (self.ui.listWidget_hddSpeedDevice.hasFocus() and
                (self.ui.listWidget_hddSpeedDevice.currentRow() > -1)):
                self.ui.listWidget_hddSpeedDevice.takeItem(self.ui.listWidget_hddSpeedDevice.currentRow())
            elif (self.ui.listWidget_mount.hasFocus() and
                (self.ui.listWidget_mount.currentRow() > -1)):
                self.ui.listWidget_mount.takeItem(self.ui.listWidget_mount.currentRow())
            elif (self.ui.listWidget_tempDevice.hasFocus() and
                (self.ui.listWidget_tempDevice.currentRow() > -1)):
                self.ui.listWidget_tempDevice.takeItem(self.ui.listWidget_tempDevice.currentRow())


    def addHddDevice(self):
        """function to add mount points"""
        if self.debug: qDebug("[PTM] [advanced.py] [addHddDevice]")
        if self.debug: qDebug("[PTM] [advanced.py] [addHddDevice] : Device '%s'" %(self.ui.comboBox_hddDevice.currentText()))
        self.ui.listWidget_hddDevice.clearSelection()
        self.ui.listWidget_hddDevice.addItem(self.ui.comboBox_hddDevice.currentText())


    def addHddSpeedDevice(self):
        """function to add disk device"""
        if self.debug: qDebug("[PTM] [advanced.py] [addHddSpeedDevice]")
        if self.debug: qDebug("[PTM] [advanced.py] [addHddSpeedDevice] : Device '%s'" %(self.ui.comboBox_hddSpeedDevice.currentText()))
        self.ui.listWidget_hddSpeedDevice.clearSelection()
        self.ui.listWidget_hddSpeedDevice.addItem(self.ui.comboBox_hddSpeedDevice.currentText())


    def addMount(self):
        """function to add mount points"""
        if self.debug: qDebug("[PTM] [advanced.py] [addMount]")
        if self.debug: qDebug("[PTM] [advanced.py] [addMount] : Device '%s'" %(self.ui.comboBox_mount.currentText()))
        self.ui.listWidget_mount.clearSelection()
        self.ui.listWidget_mount.addItem(self.ui.comboBox_mount.currentText())


    def addTempDevice(self):
        """function to add temperature device"""
        if self.debug: qDebug("[PTM] [advanced.py] [addTempDevice]")
        if self.debug: qDebug("[PTM] [advanced.py] [addTempDevice] : Device '%s'" %(self.ui.comboBox_tempDevice.currentText()))
        self.ui.listWidget_tempDevice.clearSelection()
        self.ui.listWidget_tempDevice.addItem(self.ui.comboBox_tempDevice.currentText())


    def setNetdevEnabled(self):
        """function to set enabled netdev"""
        if self.debug: qDebug("[PTM] [advanced.py] [setNetdevEnabled]")
        if self.debug: qDebug("[PTM] [advanced.py] [setNetdevEnabled] : State '%s'" %(self.ui.checkBox_netdev.checkState()))
        if (self.ui.checkBox_netdev.checkState() == 0):
            self.ui.comboBox_netdev.setDisabled(True)
        else:
            self.ui.comboBox_netdev.setEnabled(True)
