import commands, time
from PyQt4.QtCore import QThread



class HddTempThread(QThread):
    def __init__(self, interval, hdd):
        """thread definition"""
        QThread.__init__(self)
        self.stopped = False
        self.interval = interval / 1000
        self.label_exit = 2
        self.hdd = hdd
        self.hddtemp = '----'


    def run(self):
        """operating function"""
        while ((not self.stopped) and (self.label_exit > 0)):
            time.sleep(self.interval)
            # dirty hack
            self.label_exit = self.label_exit - 1
            commandOut = commands.getoutput("hddtemp " + self.hdd)
            self.hddtemp = "%4s" % (str(round(float(commandOut.split(':')[2][:-3]), 1)))


    def stop(self):
        """stop signal"""
        self.stopped = True
