import commands, time, os
from PyQt4.QtCore import QThread



class GpuTempThread(QThread):
    def __init__(self, interval,  gpudev):
        """thread definition"""
        QThread.__init__(self)
        self.stopped = False
        self.interval = interval / 1000
        self.label_exit = 2
        self.gpudev = gpudev
        self.gputemp = '----'


    def run(self):
        """operating function"""
        while ((not self.stopped) and (self.label_exit > 0)):
            time.sleep(self.interval)
            # dirty hack
            self.label_exit = self.label_exit - 1
            if (self.gpudev == 'nvidia'):
                commandOut = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
                self.gputemp = "%4s" % (str(round(float(commandOut.split()[2]), 1)))
            elif (self.gpudev == 'ati'):
                commandOut = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
                self.gputemp = "%4s" % (str(round(float(commandOut.split()[4]), 1)))
            else:
                self.gputemp = '----'
    
    
    def stop(self):
        """stop signal"""
        self.stopped = True
