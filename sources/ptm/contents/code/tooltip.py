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



class Tooltip():
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def addValue(self, type, value=0.0, tooltipNum=100):
        """function to add value to list"""
        if (len(self.parent.tooltipValues[type]) > tooltipNum):
            self.parent.tooltipValues[type] = self.parent.tooltipValues[type][1:]
        self.parent.tooltipValues[type].append(value)
    
    
    def createGraphic(self, types, colors, bounds, values, widget):
        """function to create graph"""
        widget.clear()
        maxOne = [100.0, 100.0]
        pen = QPen()
        bounds['down'] = 1.0
        for value in values['down']:
            if (bounds['down'] < value):
                bounds['down'] = value
        for value in values['up']:
            if (bounds['down'] < value):
                bounds['down'] = value
        bounds['up'] = bounds['down']
        down = False
        for type in types:
            norm = [maxOne[0] / len(values[type]), maxOne[1] / (1.5 * bounds[type])]
            pen.setColor(QColor(colors[type]))
            if (down):
                shift = (types.index(type) - 1) * maxOne[0]
            else:
                shift = types.index(type) * maxOne[0]
            for i in range(len(values[type])-1):
                x1 = i * norm[0] + shift
                y1 = -values[type][i] * norm[1]
                x2 = (i + 1) * norm[0] + shift
                y2 = -values[type][i+1] * norm[1]
                widget.addLine(x1, y1, x2, y2, pen)
            if (type == 'down'):
                down = True
    
