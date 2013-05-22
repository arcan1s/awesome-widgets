import commands, time
from PyQt4.QtCore import QThread



class GpuThread(QThread):
    def __init__(self, interval,  gpudev):
        """thread definition"""
        QThread.__init__(self)
        self.stopped = False
        self.interval = interval / 1000
        self.label_exit = 2
        self.gpudev = gpudev
        self.gpu = '----'
    
    
    def run(self):
        """operating function"""
        while ((not self.stopped) and (self.label_exit > 0)):
            time.sleep(self.interval)
            # dirty hack
            self.label_exit = self.label_exit - 1
            if (self.gpudev == 'nvidia'):
                commandOut = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
                if (commandOut.split()[2] == 'N/A'):
                    self.gpu = '  N/A'
                else:
                    self.gpu = "%5s" % (str(round(float(commandOut.split()[2][:-1]), 1)))
            elif (self.gpudev == 'ati'):
                commandOut = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
                self.gpu = "%5s" % (str(round(float(commandOut.split()[3][:-1]), 1)))
            else:
                self.gpu = '-----'


    def stop(self):
        """stop signal"""
        self.stopped = True
