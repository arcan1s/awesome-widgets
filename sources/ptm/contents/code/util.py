# -*- coding: utf-8 -*-

# Copyright 2012 Alex Oleshkevich <alex.oleshkevich@gmail.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

from shutil import copyfile
from PyKDE4.kdecore import *
import os



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
