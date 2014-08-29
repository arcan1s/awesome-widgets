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

        QObject.connect(self.ui.tableWidget_customCommand, SIGNAL("itemActivated(QTableWidgetItem*)"), self.ui.tableWidget_customCommand.openPersistentEditor)
        QObject.connect(self.ui.tableWidget_customCommand, SIGNAL("itemChanged(QTableWidgetItem*)"), self.addCustomCommand)
        QObject.connect(self.ui.tableWidget_pkgCommand, SIGNAL("itemActivated(QTableWidgetItem*)"), self.ui.tableWidget_pkgCommand.openPersistentEditor)
        QObject.connect(self.ui.tableWidget_pkgCommand, SIGNAL("itemChanged(QTableWidgetItem*)"), self.addPkgCommand)


    def keyPressEvent(self, event):
        """delete events"""
        if self.debug: qDebug("[PTM] [deconfig.py] [keyPressEvent]")
        if self.debug: qDebug("[PTM] [deconfig.py] [keyPressEvent] : Run function with event '%s'" %(event.key()))
        if (event.key() == Qt.Key_Delete):
            if (self.ui.tableWidget_customCommand.hasFocus() and
                (self.ui.tableWidget_customCommand.currentRow() > -1)):
                self.ui.tableWidget_customCommand.removeRow(self.ui.tableWidget_customCommand.currentRow())
            elif (self.ui.tableWidget_pkgCommand.hasFocus() and
                (self.ui.tableWidget_pkgCommand.currentRow() > -1)):
                self.ui.tableWidget_pkgCommand.removeRow(self.ui.tableWidget_pkgCommand.currentRow())


    def addCustomCommand(self, item):
        """function to add custom command"""
        if self.debug: qDebug("[PTM] [deconfig.py] [addCustomCommand]")
        if (item.row() == (self.ui.tableWidget_customCommand.rowCount() - 1)):
            self.ui.tableWidget_customCommand.insertRow(self.ui.tableWidget_customCommand.rowCount());


    def addPkgCommand(self, item):
        """function to add new row"""
        if self.debug: qDebug("[PTM] [deconfig.py] [addPkgCommand]")
        if ((item.row() == (self.ui.tableWidget_pkgCommand.rowCount() - 1)) and
            (item.column() == 0)):
            self.ui.tableWidget_pkgCommand.insertRow(self.ui.tableWidget_pkgCommand.rowCount());
            self.ui.tableWidget_pkgCommand.setItem(self.ui.tableWidget_pkgCommand.rowCount()-1, 1, QTableWidgetItem("0"))
