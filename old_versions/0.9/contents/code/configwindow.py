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