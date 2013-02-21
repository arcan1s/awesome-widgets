# -*- coding: utf-8 -*-

from util import *



class Config():
    def __init__(self, applet):
        self.applet = applet
        self.config = self.applet.globalConfig()
    
    def get(self, key, default = ''):
        return self.config.readEntry(key, default).toString()
    
    def set(self, key, value):
        self.config.writeEntry(key, value)