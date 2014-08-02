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



class TooltipWindow(QWidget):
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
        self.ui = uic.loadUi(parent.package().filePath('ui', 'tooltipconfig.ui'), self)
        self.parent = parent
        self.kcolorcombos = {'background':self.ui.kcolorcombo_background, 'cpu':self.ui.kcolorcombo_cpu,
            'cpuclock':self.ui.kcolorcombo_cpuclock, 'down':self.ui.kcolorcombo_down,
            'mem':self.ui.kcolorcombo_mem, 'swap':self.ui.kcolorcombo_swap,
            'up':self.ui.kcolorcombo_up}
        QObject.connect(self.ui.checkBox_background, SIGNAL("stateChanged(int)"), self.setBackground)


    def setBackground(self, background=0):
        """function to enable/disable background elements"""
        if (background == 0):
            self.kcolorcombos['background'].setDisabled(True)
        else:
            self.kcolorcombos['background'].setEnabled(True)
