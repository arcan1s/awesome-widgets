class ConfigAccepted:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def configAccepted(self):
        """function to accept settings"""
        # update local variables
        self.parent.interval = int(self.parent.configpage.ui.spinBox_interval.value())
        self.parent.settings.set('interval', self.parent.interval)
        self.parent.font_family = str(self.parent.configpage.ui.fontComboBox.currentFont().family())
        self.parent.settings.set('font_family', self.parent.font_family)
        self.parent.font_size = int(self.parent.configpage.ui.spinBox_fontSize.value())
        self.parent.settings.set('font_size', self.parent.font_size)
        self.parent.font_color = str(self.parent.configpage.ui.kcolorcombo.color().name())
        self.parent.settings.set('font_color', self.parent.font_color)
        if (self.parent.configpage.ui.comboBox_style.currentIndex() == 0):
            self.parent.font_style = 'normal'
        else:
            self.parent.font_style = 'italic'
        self.parent.settings.set('font_style', self.parent.font_style)
        self.parent.font_weight = int(self.parent.configpage.ui.spinBox_weight.value())
        self.parent.settings.set('font_weight', self.parent.font_weight)
        
        # disconnecting from source and clear layout
        if (self.parent.uptimeBool == 1):
            self.parent.systemmonitor.disconnectSource("system/uptime",  self.parent)
            self.parent.label_uptime.setText('')
            self.parent.layout.removeItem(self.parent.label_uptime)
        if (self.parent.cpuBool == 1):
            self.parent.systemmonitor.disconnectSource("cpu/system/TotalLoad", self.parent)
            if (self.parent.cpuFormat.split('$ccpu')[0] != self.parent.cpuFormat):
                self.parent.label_cpu0.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu0)
                self.parent.label_cpu1.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu1)
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/TotalLoad", self.parent)
                    exec ("self.parent.label_coreCpu" + str(core) + ".setText('')")
                    exec ("self.parent.layout.removeItem(self.parent.label_coreCpu" + str(core) + ")")
            else:
                self.parent.label_cpu.setText('')
                self.parent.layout.removeItem(self.parent.label_cpu)
        if (self.parent.cpuclockBool == 1):
            self.parent.systemmonitor.disconnectSource("cpu/system/AverageClock", self.parent)
            if (self.parent.cpuclockFormat.split('$ccpu')[0] != self.parent.cpuclockFormat):
                self.parent.label_cpuclock0.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock0)
                self.parent.label_cpuclock1.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock1)
                for core in range(self.parent.numCores):
                    self.parent.systemmonitor.disconnectSource("cpu/cpu"+str(core)+"/clock", self.parent)
                    exec ("self.parent.label_coreCpuclock" + str(core) + ".setText('')")
                    exec ("self.parent.layout.removeItem(self.parent.label_coreCpuclock" + str(core) + ")")
            else:
                self.parent.label_cpuclock.setText('')
                self.parent.layout.removeItem(self.parent.label_cpuclock)
        if (self.parent.tempBool == 1):
            self.parent.systemmonitor.disconnectSource(self.parent.tempdev, self.parent)
            self.parent.label_temp.setText('')
            self.parent.layout.removeItem(self.parent.label_temp)
        if (self.parent.gpuBool == 1):
            self.parent.gpuChecker.stop()
            self.parent.label_gpu.setText('')
            self.parent.layout.removeItem(self.parent.label_gpu)
        if (self.parent.gputempBool == 1):
            self.parent.gpuTempChecker.stop()
            self.parent.label_gputemp.setText('')
            self.parent.layout.removeItem(self.parent.label_gputemp)
        if (self.parent.memBool == 1):
            self.parent.systemmonitor.disconnectSource("mem/physical/application", self.parent)
            if (self.parent.memInMb == False):
                self.parent.systemmonitor.disconnectSource("mem/physical/free", self.parent)
                self.parent.systemmonitor.disconnectSource("mem/physical/used", self.parent)
            self.parent.label_mem.setText('')
            self.parent.layout.removeItem(self.parent.label_mem)
        if (self.parent.swapBool == 1):
            self.parent.systemmonitor.disconnectSource("mem/swap/used", self.parent)
            if (self.parent.swapInMb == False):
                self.parent.systemmonitor.disconnectSource("mem/swap/free", self.parent)
            self.parent.label_swap.setText('')
            self.parent.layout.removeItem(self.parent.label_swap)
        if (self.parent.hddBool == 1):
            for mount in self.parent.mountPoints:
                self.parent.systemmonitor.disconnectSource("partitions" + mount + "/filllevel", self.parent)
                exec ('self.parent.label_hdd_' + ''.join(mount.split('/')) + '.setText("")')
                exec ("self.parent.layout.removeItem(self.parent.label_hdd_" + ''.join(mount.split('/')) + ")")
            self.parent.label_hdd0.setText('')
            self.parent.label_hdd1.setText('')
            self.parent.layout.removeItem(self.parent.label_hdd0)
            self.parent.layout.removeItem(self.parent.label_hdd1)
        if (self.parent.hddtempBool == 1):
            self.parent.hddTempChecker.stop()
            self.parent.label_hddtemp.setText('')
            self.parent.layout.removeItem(self.parent.label_hddtemp)
        if (self.parent.netBool == 1):
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/transmitter/data", self.parent)
            self.parent.systemmonitor.disconnectSource("network/interfaces/"+self.parent.netdev+"/receiver/data", self.parent)
            self.parent.label_netDown.setText('')
            self.parent.label_netUp.setText('')
            self.parent.layout.removeItem(self.parent.label_netUp)
            self.parent.layout.removeItem(self.parent.label_netDown)
        if (self.parent.batBool == 1):
            self.parent.label_bat.setText('')
            self.parent.layout.removeItem(self.parent.label_bat)
        
        self.parent.label_order = "------------"
        
        for label in self.parent.dict_orders.keys():
            if (self.parent.configpage.checkboxes[self.parent.dict_orders[label]].checkState() == 2):
                exec ('self.parent.' + self.parent.dict_orders[label] + 'Bool = 1')
                pos = self.parent.configpage.sliders[self.parent.dict_orders[label]].value() - 1
                self.parent.label_order = self.parent.label_order[:pos] + label + self.parent.label_order[pos+1:]
            else:
                exec ('self.parent.' + self.parent.dict_orders[label] + 'Bool = 0')
            if (self.parent.dict_orders[label] == 'net'):
                exec ('self.parent.' + self.parent.dict_orders[label] + 'NonFormat = str(self.parent.configpage.lineedits[self.parent.dict_orders[label]].text())')
                exec ('self.parent.settings.set("' + self.parent.dict_orders[label] + 'NonFormat", self.parent.' + self.parent.dict_orders[label] + 'NonFormat)')
            else:
                exec ('self.parent.' + self.parent.dict_orders[label] + 'Format = str(self.parent.configpage.lineedits[self.parent.dict_orders[label]].text())')
                exec ('self.parent.settings.set("' + self.parent.dict_orders[label] + 'Format", self.parent.' + self.parent.dict_orders[label] + 'Format)')
            exec ('self.parent.settings.set("' + self.parent.dict_orders[label] + 'Bool", self.parent.' + self.parent.dict_orders[label] + 'Bool)')
            if (self.parent.dict_orders[label] == 'net'):
                self.parent.num_dev = int(self.parent.configpage.ui.comboBox_numNet.currentIndex())
                self.parent.settings.set('num_dev', self.parent.num_dev)
            elif (self.parent.dict_orders[label] == 'bat'):
                self.parent.battery_device = str(self.parent.configpage.ui.lineEdit_batdev.text())
                self.parent.ac_device = str(self.parent.configpage.ui.lineEdit_acdev.text())
                self.parent.settings.set('battery_device', self.parent.battery_device)
                self.parent.settings.set('ac_device', self.parent.ac_device)
        
        self.parent.label_order = ''.join(self.parent.label_order.split('-'))
        self.parent.settings.set('label_order', self.parent.label_order)
        
        # reinitializate
        self.parent.reinit.reinit()
