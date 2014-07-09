# -*- coding: utf-8 -*-

############################################################################
#   This file is part of pytextmonitor                                     #
#                                                                          #
#   pytextmonitor is free software: you can redistribute it and/or         #
#   modify it under the terms of the GNU General Public License as         #
#   published by the Free Software Foundation, either version 3 of the     #
#   License, or (at your option) any later version.                        #
#                                                                          #
#   pytextmonitor is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#   GNU General Public License for more details.                           #
#                                                                          #
#   You should have received a copy of the GNU General Public License      #
#   along with pytextmonitor. If not, see http://www.gnu.org/licenses/     #
############################################################################

from PyQt4.QtCore import *
from PyKDE4.kdecore import KComponentData
from PyKDE4.kdeui import KNotification
import commands



class PTMNotify:
    def __init__(self, parent):
        """class definition"""
        # debug
        environment = QProcessEnvironment.systemEnvironment()
        debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
        if (debugEnv == QString("yes")):
            self.debug = True
        else:
            self.debug = False
        # main


    def init(self, name=None):
        """function to init notification"""
        if self.debug: qDebug("[PTM] [ptmnotify.py] [init]")
        if self.debug: qDebug("[PTM] [ptmnotify.py] [init] : Run function with name '%s'" %(name))
        content = self.initText(name)
        self.createNotify(content)


    def createNotify(self, content):
        """function to create notification for label"""
        if self.debug: qDebug("[PTM] [ptmnotify.py] [createNotify]")
        if self.debug: qDebug("[PTM] [ptmnotify.py] [createNotify] : Run function with content '%s'" %(content))
        notification = KNotification(content[0])
        notification.setComponentData(KComponentData("plasma_applet_pytextmonitor"))
        notification.setTitle("PyTextMonitor info ::: " + content[0]);
        notification.setText(content[1]);
        notification.sendEvent();


    def createText(self, type):
        """function to create text"""
        if self.debug: qDebug("[PTM] [ptmnotify.py] [createText]")
        if self.debug: qDebug("[PTM] [ptmnotify.py] [createText] : Run function with type '%s'" %(type))
        text = ""
        if (type == "battery"):
            try:
                text = "%s" %(commands.getoutput("acpi -abi"))
            except:
                text = "Something wrong"
        elif (type == "disk"):
            try:
                for line in commands.getoutput("df -h --output='source,target,used,size,pcent' --exclude-type=fuseblk --exclude-type=tmpfs --exclude-type=devtmpfs").split("\n")[1:]:
                    text = text + "%s (to %s): %s of %s (%s)\n" % (line.split()[0], line.split()[1], line.split()[2], line.split()[3], line.split()[4])
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
                    text = text + "%s %s\n" % (line.split('"')[0], line.split('"')[5])
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1")
                    try:
                        value = "%5.1f" % (round(float(output.split()[2]), 1))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-getclocks | grep load | tail -n1")
                    try:
                        value = "%5.1f" % (round(float(output.split()[3]), 1))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Load: %s%%\n" % (value)
                if (gpudev == 'nvidia'):
                    output = commands.getoutput("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1")
                    try:
                        value = "%5.1f" % (round(float(output.split()[2]), 1))
                    except:
                        value = "  N\A"
                elif (gpudev == 'ati'):
                    output = commands.getoutput("aticonfig --od-gettemperature | grep Temperature | tail -n1")
                    try:
                        value = "%5.1f" % (round(float(output.split()[3]), 1))
                    except:
                        value = "  N\A"
                else:
                    value = "  N\A"
                text = text + "Temp: %s\xb0C\n" % (value)
            except:
                text = "Something wrong"
        elif (type == "memory"):
            try:
                output = commands.getoutput("free -m -o").split("\n")
                memUsage = int(output[1].split()[1]) - (int(output[1].split()[3]) + int(output[1].split()[5]) + int(output[1].split()[6]))
                text = text + "Memory: %i of %s (%i%%)\n" % (memUsage, output[1].split()[1], int(100*memUsage/int(output[1].split()[1])))
                text = text + "Swap: %s of %s (%i%%)\n" % (output[2].split()[2], output[2].split()[1], int(100*int(output[2].split()[2])/int(output[2].split()[1])))
                output = commands.getoutput("swapon --show").split("\n")
                text = text + "Swap Device: %s (%s)" % (output[1].split()[0], output[1].split()[1])
            except:
                text = "Something wrong"
        elif (type == "network"):
            try:
                output = commands.getoutput("ifconfig -a -s").split("\n")[1:]
                devices = [line.split()[0] for line in output]
                text = text + "Devices: %s\n" % (' '.join(devices))
                for dev in devices:
                    output = commands.getoutput("ifconfig -a -s " + dev + " && sleep 0.2 && ifconfig -a -s " + dev).split("\n")
                    download = int((int(output[3].split()[2]) - int(output[1].split()[2])) / (0.2 * 1024))
                    upload = int((int(output[3].split()[6]) - int(output[1].split()[6])) / (0.2 * 1024))
                    text = text + "%s: %i/%i KB/s" % (dev, download, upload)
                    output = commands.getoutput("ifconfig " + dev + " | grep 'inet ' || echo ' inet Null'").split()[1]
                    text = text + " (IP: %s)\n" % (output)
                output = commands.getoutput("wget -qO- http://ifconfig.me/ip")
                text = text + "External IP: %s" % (output)
            except:
                text = "Something wrong"
        elif (type == "player"):
            try:
                artist = "N\\A"
                album = "N\\A"
                title = "N\\A"
                if (len(commands.getoutput("pgrep amarok")) > 0):
                    player = "amarok"
                elif (len(commands.getoutput("pgrep clementine")) > 0):
                    player = "clementine"
                elif (len(commands.getoutput("pgrep mpd")) > 0):
                    player = "mpd"
                elif (len(commands.getoutput("pgrep qmmp")) > 0):
                    player = "qmmp"
                else:
                    player = ""
                if (player == "amarok"):
                    artist = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep albumartist: | cut -c14-")
                    album = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep album: | cut -c8-")
                    title = commands.getoutput("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep title: | cut -c8-")
                elif (player == "clementine"):
                    artist = commands.getoutput("qdbus org.mpris.clementine /Player GetMetadata 2> /dev/null | grep albumartist: | cut -c14-")
                    album = commands.getoutput("qdbus org.mpris.clementine /Player GetMetadata 2> /dev/null | grep album: | cut -c8-")
                    title = commands.getoutput("qdbus org.mpris.clementine /Player GetMetadata 2> /dev/null | grep title: | cut -c8-")
                elif (player == "mpd"):
                    output = commands.getoutput("echo 'currentsong\nclose' | curl --connect-timeout 1 -fsm 3 telnet://localhost:6600 2> /dev/null")
                    for line in output.split("\n"):
                        if (line.split(": ")[0] == "Artist"):
                            artist = line.split(": ")[1]
                        elif (line.split(": ")[0] == "Album"):
                            album = line.split(": ")[1]
                        elif (line.split(": ")[0] == "Title"):
                            title = line.split(": ")[1]
                elif (player == "qmmp"):
                    artist = commands.getoutput("qmmp --nowplaying '%if(%p,%p,Unknown)' 2> /dev/null")
                    album = commands.getoutput("qmmp --nowplaying '%if(%a,%a,Unknown)' 2> /dev/null")
                    title = commands.getoutput("qmmp --nowplaying '%if(%t,%t,Unknown)' 2> /dev/null")
                text = text + "Artist: %s\nAlbum: %s\nTitle: %s" % (artist, album, title)
            except:
                text = "Something wrong"
        elif (type == "processor"):
            try:
                output = commands.getoutput("grep 'model name' /proc/cpuinfo | head -1")
                text = text + "Model: %s\n" % (' '.join(output.split()[3:]))
                output = commands.getoutput("sar -u | tail -1")
                text = text + "CPU Usage: %5.1f%%\n" % (100-float(output.split()[-1]))
                output = commands.getoutput("grep MHz /proc/cpuinfo | head -1")
                text = text + "CPU Freq: %i MHz\n" % (int(float(output.split()[-1])))
                output = commands.getoutput("sensors -u")
                text = text + "Temps:"
                for line in output.split("\n"):
                    if (line.find("_input") > -1):
                        text = text + " %3.0f\xb0C" % (round(float(line.split()[-1]), 0))
            except:
                text = "Something wrong"
        elif (type == "system"):
            try:
                text = text + "Kernel: %s\n" % (commands.getoutput("uname -rsm"))
                text = text + "Hostname: %s\n" % (commands.getoutput("uname -n"))
                text = text + "Whoami: %s\n" % (commands.getoutput("whoami"))
                text = text + "Uptime: %s\n" % (commands.getoutput("uptime"))
            except:
                text = "Something wrong"
        content = [type, text]
        if self.debug: qDebug("[PTM] [ptmnotify.py] [createText] : Returns '%s'" %(content))
        return content


    def initText(self, name):
        """function to send text"""
        if self.debug: qDebug("[PTM] [ptmnotify.py] [initText]")
        if self.debug: qDebug("[PTM] [ptmnotify.py] [initText] : Run function with name '%s'" %(name))
        if (name == "bat"):
            return self.createText("battery")
        elif (name == "cpu"):
            return self.createText("processor")
        elif (name == "cpuclock"):
            return self.createText("processor")
        elif (name == "custom"):
            return self.createText("system")
        elif (name == "desktop"):
            return self.createText("system")
        elif (name == "disk"):
            return self.createText("disk")
        elif (name == "gpu"):
            return self.createText("graphical")
        elif (name == "gputemp"):
            return self.createText("graphical")
        elif (name == "hdd"):
            return self.createText("disk")
        elif (name == "hddtemp"):
            return self.createText("disk")
        elif (name == "mem"):
            return self.createText("memory")
        elif (name == "net"):
            return self.createText("network")
        elif (name == "pkg"):
            return self.createText("system")
        elif (name == "player"):
            return self.createText("player")
        elif (name == "ps"):
            return self.createText("system")
        elif (name == "swap"):
            return self.createText("memory")
        elif (name == "temp"):
            return self.createText("processor")
        elif (name == "time"):
            return self.createText("system")
        elif (name == "uptime"):
            return self.createText("system")
        else:
            return None
