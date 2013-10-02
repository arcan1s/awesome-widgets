# -*- coding: utf-8 -*-

from gi.repository import Notify



class PTMNotify:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def exampleNotify(self):
        Notify.init ("Hello world")
        Hello=Notify.Notification.new ("Hello world","This is an example notification.","dialog-information")
        Hello.show ()
    
    def createNotification(self):
        """function to create notification for label"""
        
