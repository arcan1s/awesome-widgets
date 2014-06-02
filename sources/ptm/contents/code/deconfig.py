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



class DEConfigWindow(QWidget):
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
        self.ui = uic.loadUi(parent.package().filePath('ui', 'deconfig.ui'), self)
        self.parent = parent

        QObject.connect(self.ui.pushButton_customCommand, SIGNAL("clicked()"), self.addCustomCommand)
        QObject.connect(self.ui.pushButton_pkgCommand, SIGNAL("clicked()"), self.addPkgCommand)
        QObject.connect(self.ui.listWidget_customCommand, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_customCommand.openPersistentEditor)
        QObject.connect(self.ui.listWidget_pkgCommand, SIGNAL("itemActivated(QListWidgetItem*)"), self.ui.listWidget_pkgCommand.openPersistentEditor)
        QObject.connect(self.ui.comboBox_pkgCommand, SIGNAL("currentIndexChanged(int)"), self.updatePkgNullValue)
        QObject.connect(self.ui.comboBox_pkgCommand, SIGNAL("editTextChanged(QString)"), self.updatePkgNullValue)


    def keyPressEvent(self, event):
        """delete events"""
        if self.debug: qDebug("[PTM] [deconfig.py] [keyPressEvent]")
        if self.debug: qDebug("[PTM] [deconfig.py] [keyPressEvent] : Run function with event '%s'" %(event.key()))
        if (event.key() == Qt.Key_Delete):
            if (self.ui.listWidget_customCommand.hasFocus() and
                (self.ui.listWidget_customCommand.currentRow() > -1)):
                self.ui.listWidget_customCommand.takeItem(self.ui.listWidget_customCommand.currentRow())
            elif (self.ui.listWidget_pkgCommand.hasFocus() and
                (self.ui.listWidget_pkgCommand.currentRow() > -1)):
                self.ui.listWidget_pkgCommand.takeItem(self.ui.listWidget_pkgCommand.currentRow())


    def addCustomCommand(self):
        """function to add custom command"""
        if self.debug: qDebug("[PTM] [deconfig.py] [addCustomCommand]")
        if self.debug: qDebug("[PTM] [deconfig.py] [addCustomCommand] : Cmd '%s'" %(self.ui.lineEdit_customCommand.text()))
        self.ui.listWidget_customCommand.clearSelection()
        self.ui.listWidget_customCommand.addItem(self.ui.lineEdit_customCommand.text())


    def addPkgCommand(self):
        """function to add package manager command"""
        if self.debug: qDebug("[PTM] [deconfig.py] [addPkgCommand]")
        if self.debug: qDebug("[PTM] [deconfig.py] [addPkgCommand] : Cmd '%s'" %(self.ui.comboBox_pkgCommand.currentText()))
        self.ui.listWidget_pkgCommand.clearSelection()
        self.ui.listWidget_pkgCommand.addItem(self.ui.comboBox_pkgCommand.currentText() +\
            QString(":") + QString.number(self.ui.spinBox_pkgCommandNum.value()))


    def updatePkgNullValue(self):
        """function to set default values to PKGNULL spinbox"""
        if self.debug: qDebug("[PTM] [deconfig.py] [updatePkgNullValue]")
        if (self.ui.comboBox_pkgCommand.currentText().contains(QString("pacman -Qu"))):
            self.ui.spinBox_pkgCommandNum.setValue(0)
        elif (self.ui.comboBox_pkgCommand.currentText().contains(QString("apt-show-versions -u -b"))):
            self.ui.spinBox_pkgCommandNum.setValue(0)
        elif (self.ui.comboBox_pkgCommand.currentText().contains(QString("aptitude search '~U'"))):
            self.ui.spinBox_pkgCommandNum.setValue(0)
        elif (self.ui.comboBox_pkgCommand.currentText().contains(QString("yum list updates"))):
            self.ui.spinBox_pkgCommandNum.setValue(3)
        elif (self.ui.comboBox_pkgCommand.currentText().contains(QString("pkg_version -I -l '<'"))):
            self.ui.spinBox_pkgCommandNum.setValue(0)
        elif (self.ui.comboBox_pkgCommand.currentText().contains(QString("urpmq --auto-select"))):
            self.ui.spinBox_pkgCommandNum.setValue(0)
