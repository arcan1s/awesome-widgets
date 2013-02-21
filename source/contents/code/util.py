# -*- coding: utf-8 -*-

import os
from shutil import copyfile
from PyKDE4.kdecore import *



class Util():
    def __init__(self, applet):
        self.applet = applet
    
    def createDirectory(self, name):
        if not os.path.isdir(name):
            try:
                os.mkdir(name)
            except:
                print 'Failed to create directory: ' + name
				
    def kdeHome(self):
        return unicode(KGlobal.dirs().localkdedir())
		
    def createNotifyrc(self):
        print '[%s] creating notifyrc' % (self.applet._name)
        self.createDirectory(self.kdeHome() + 'share/apps/%s' % self.applet._name)
        
        source = self.applet.package().path() + 'contents/misc/%s.notifyrc' % self.applet._name
        destination = self.kdeHome() + 'share/apps/%s/%s.notifyrc' % (self.applet._name, self.applet._name)
        copyfile(source, destination)
    
    def createConfig(self):
        self.createDirectory(self.kdeHome() + 'share/apps/%s' % self.applet._name)
        
        source = self.applet.package().path() + 'contents/misc/%s.ini' % self.applet._name
        destination = self.kdeHome() + 'share/apps/%s/%s.ini' % (self.applet._name, self.applet._name)
        copyfile(source, destination)