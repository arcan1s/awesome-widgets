# -*- coding: utf-8 -*-

# Copyright 2013 Evgeniy Alekseev <esalexeev@gmail.com>
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

from PyKDE4.kdecore import KComponentData
from PyKDE4.kdeui import KNotification
import commands



class PTMNotify:
    def __init__(self, parent):
        """class definition"""
        self.parent = parent
    
    
    def init(self):
        """function to init notification"""
        content = self.initText(self.parent)
        self.createNotify(content)
    
    
    def createNotify(self, content):
        """function to create notification for label"""
        notification = KNotification(content[0])
        notification.setComponentData(KComponentData("plasma_applet_pytextmonitor"))
        notification.setTitle("PyTextMonitor info ::: " + content[0]);
        notification.setText(content[1]);
        notification.sendEvent();
    
    
    def createText(self, type):
        """function to create text"""
        text = ""
        if (type == "system"):
            try:
                text = text + "Kernel: %s\n" %(commands.getoutput("uname -rsm"))
                text = text + "Hostname: %s\n" %(commands.getoutput("uname -n"))
                text = text + "Whoami: %s\n" %(commands.getoutput("whoami"))
                text = text + "Uptime: %s\n" %(commands.getoutput("uptime"))
            except:
                text = "Something wrong"
        elif (type == "processor"):
            try:
                output = commands.getoutput("grep 'model name' /proc/cpuinfo | head -1")
                text = text + "Model: %s\n" %(' '.join(output.split()[3:]))
                output = commands.getoutput("sar -u | tail -1")
                text = text + "CPU Usage: %s%%\n" %(str(100-float(output.split()[-1])))
                output = commands.getoutput("grep MHz /proc/cpuinfo | head -1")
                text = text + "CPU Freq: %s MHz\n" %(str(int(float(output.split()[-1]))))
                output = commands.getoutput("sensors -u")
                text = text + "Temps:"
                for line in output.split("\n"):
                    if (line.find("_input") > -1):
                        text = text + " %s\xb0C" %(str(round(float(line.split()[-1]), 0)))
            except:
                text = "Something wrong"
        elif (type == "graphical"):
            try:
                output = commands.getoutput("lspci -m | grep 'VGA\|3D'")
                if (output.lower().find('nvidia') > -1):
                    gpudev = "nvidia"
                elif (output.lower().find('radeon') > -1):
                    gpudev = "ati"
                for line in output.split("\n"):
                    text = text + "%s %s\n" %(line.split('"')[0], line.split('"')[5])
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[2][:-1]), 1)))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[3][:-1]), 1)))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Load: %s%%\n" %(value)
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[2][:-1]), 1)))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
                    try:
                        value = "%5s" % (str(round(float(output.split()[3][:-1]), 1)))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Temp: %s\xb0C\n" %(value)
            except:
                text = "Something wrong"
        elif (type == "memory"):
            try:
                output = commands.getoutput("free -m -o").split("\n")
                memusage = int(output[1].split()[1]) - (int(output[1].split()[3]) + int(output[1].split()[5]) + int(output[1].split()[6]))
                text = text + "Memory: %s of %s (%s%%)\n" %(str(memusage), output[1].split()[1], str(int(100*memusage/int(output[1].split()[1]))))
                text = text + "Swap: %s of %s (%s%%)\n" %(output[2].split()[2], output[2].split()[1], str(int(100*int(output[2].split()[2])/int(output[2].split()[1]))))
                output = commands.getoutput("swapon --show").split("\n")
                text = text + "Swap Device: %s (%s)" %(output[1].split()[0], output[1].split()[1])
            except:
                text = "Something wrong"
        elif (type == "disk"):
            try:
                output = commands.getoutput("df -h --output='source,target,used,size,pcent' --exclude-type=fuseblk --exclude-type=tmpfs --exclude-type=devtmpfs").split("\n")[1:]
                for line in output:
                    text = text + "%s (to %s): %s of %s (%s)\n" %(line.split()[0], line.split()[1], line.split()[2], line.split()[3], line.split()[4])
            except:
                text = "Something wrong"
        elif (type == "network"):
            try:
                output = commands.getoutput("ifconfig -a -s").split("\n")[1:]
                text = text + "Devices:"
                for line in output:
                    text = text + " %s" %(line.split()[0])
                output = commands.getoutput("ifconfig -a -s " + self.parent.parent.netdev + " && sleep 0.2 && ifconfig -a -s " + self.parent.parent.netdev).split("\n")
                download = int((int(output[3].split()[2]) - int(output[1].split()[2])) / (0.2 * 1024))
                upload = int((int(output[3].split()[6]) - int(output[1].split()[6])) / (0.2 * 1024))
                text = text + "\n%s: %s/%s KB/s\n" %(self.parent.parent.netdev, download, upload)
                output = commands.getoutput("ifconfig " + self.parent.parent.netdev + " | grep 'inet '").split()[1]
                text = text + "IP: %s\n" %(output[:-1])
                output = commands.getoutput("wget http://checkip.dyndns.org/ -q -O - | awk '{print $6}' | sed 's/<.*>//g'")
                text = text + "External IP: %s" %(output[:-1])
            except:
                text = "Something wrong"
        elif (type == "battery"):
            try:
                text = text + "%s" %(commands.getoutput("acpi -abi"))
            except:
                text = "Something wrong"
        elif (type == "musicplayer"):
            try:
                artist = "N\\A"
                album = "N\\A"
                title = "N\\A"
                if (self.parent.parent.player_name == 0):
                    artist = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep albumartist: | cut -c14-")
                    album = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep album: | cut -c8-")
                    title = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep title: | cut -c8-")
                elif (self.parent.parent.player_name == 1):
                    output = commands.getoutput("echo 'currentsong\nclose' | curl --connect-timeout 1 -fsm 3 telnet://localhost:6600 2> /dev/null")
                    for line in output.split("\n"):
                        if (line.split(": ")[0] == "Artist"):
                            artist = line.split(": ")[1]
                        elif (line.split(": ")[0] == "Album"):
                            album = line.split(": ")[1]
                        elif (line.split(": ")[0] == "Title"):
                            title = line.split(": ")[1]
                elif (self.parent.parent.player_name == 2):
                    artist = commands.getoutput("qmmp --nowplaying '%if(%p,%p,Unknown)' 2> /dev/null")
                    album = commands.getoutput("qmmp --nowplaying '%if(%a,%a,Unknown)' 2> /dev/null")
                    title = commands.getoutput("qmmp --nowplaying '%if(%t,%t,Unknown)' 2> /dev/null")
                text = text + "Artist: %s\nAlbum: %s\nTitle: %s" %(artist, album, title)
            except:
                text = "Something wrong"
        
        content = [type, text]
        return content
    
    
    def initText(self, sender):
        """function to send text"""
        try:
            if (sender == self.parent.parent.label_time):
                content = self.createText("system")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_uptime):
                content = self.createText("system")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_cpu):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_cpuclock):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_temp):
                content = self.createText("processor")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_gpu):
                content = self.createText("graphical")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_gputemp):
                content = self.createText("graphical")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_mem):
                content = self.createText("memory")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_swap):
                content = self.createText("memory")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_hdd0):
                content = self.createText("disk")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_hddtemp):
                content = self.createText("disk")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_netDown):
                content = self.createText("network")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_bat):
                content = self.createText("battery")
                return content
        except:
            pass
        try:
            if (sender == self.parent.parent.label_player):
                content = self.createText("musicplayer")
                return content
        except:
            pass
