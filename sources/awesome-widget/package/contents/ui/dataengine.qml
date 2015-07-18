/***************************************************************************
 *   This file is part of awesome-widgets                                  *
 *                                                                         *
 *   awesome-widgets is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   awesome-widgets is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with awesome-widgets. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Dialogs 1.1 as QtDialogs
import QtQuick.Layouts 1.0 as QtLayouts
import QtQuick.Controls.Styles 1.3 as QtStyles

import org.kde.plasma.private.awesomewidget 1.0


Item {
    id: dataenginePage
    // backend
    AWKeys {
        id: awKeys;
    }
    AWActions {
        id: awActions;
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awKeys.isDebugEnabled()
    property variant tooltipSettings: {
        "tooltipNumber": plasmoid.configuration.tooltipNumber,
        "useTooltipBackground": plasmoid.configuration.useTooltipBackground,
        "tooltipBackgroung": plasmoid.configuration.tooltipBackgroung,
        "cpuTooltip": plasmoid.configuration.cpuTooltip,
        "cpuclTooltip": plasmoid.configuration.cpuclTooltip,
        "memTooltip": plasmoid.configuration.memTooltip,
        "swapTooltip": plasmoid.configuration.swapTooltip,
        "downTooltip": plasmoid.configuration.downTooltip,
        "upTooltip": plasmoid.configuration.downTooltip,
        "batTooltip": plasmoid.configuration.batTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downTooltipColor": plasmoid.configuration.downTooltipColor,
        "upTooltipColor": plasmoid.configuration.upTooltipColor,
        "batTooltipColor": plasmoid.configuration.batTooltipColor,
        "batInTooltipColor": plasmoid.configuration.batInTooltipColor
    }

    property variant cfg_dataengine: awActions.readDataEngineConfiguration()

    Column {
        id: pageColumn
        anchors.fill: parent
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("ACPI path")
            }
            QtControls.TextField {
                width: parent.width * 3 / 5
                text: cfg_dataengine["ACPIPATH"]
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Custom scripts")
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Edit scripts")
                onClicked: awKeys.editItem("extscript")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("GPU device")
            }
            QtControls.ComboBox {
                id: gpuDev
                width: parent.width * 3 / 5
                model: ["auto", "disable", "ati", "nvidia"]
                Component.onCompleted: {
                    for (var i=0; i<model.length; i++) {
                        if (model[i] == cfg_dataengine["GPUDEV"]) {
                            currentIndex = i;
                        }
                    }
                }
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("HDD")
            }
            QtControls.ComboBox {
                id: hdd
                width: parent.width * 3 / 5
                model: awKeys.getHddDevices(true)
                Component.onCompleted: {
                    for (var i=0; i<model.length; i++) {
                        if (model[i] == cfg_dataengine["HDDDEV"]) {
                            hdd.currentIndex = i;
                        }
                    }
                }
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("hddtemp cmd")
            }
            QtControls.TextField {
                width: parent.width * 3 / 5
                text: cfg_dataengine["HDDTEMPCMD"]
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("MPD address")
            }
            QtControls.TextField {
                width: parent.width * 3 / 5
                text: cfg_dataengine["MPDADDRESS"]
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("MPD port")
            }
            QtControls.SpinBox {
                width: parent.width * 3 / 5
                minimumValue: 1000
                maximumValue: 65535
                stepSize: 1
                value: cfg_dataengine["MPDPORT"]
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("MPRIS player name")
            }
            QtControls.ComboBox {
                id: mpris
                width: parent.width * 3 / 5
                editable: true
                model: ["auto", "amarok", "audacious", "clementine", "deadbeef",
                        "vlc", "qmmp", "xmms2", cfg_dataengine["MPRIS"]]
                currentIndex: model.length - 1
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Music player")
            }
            QtControls.ComboBox {
                id: player
                width: parent.width * 3 / 5
                model: ["mpris", "mpd"]
                Component.onCompleted: {
                    for (var i=0; i<model.length; i++) {
                        if (model[i] == cfg_dataengine["PLAYER"]) {
                            player.currentIndex = i;
                        }
                    }
                }
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Quotes monitor")
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Edit tickers")
                onClicked: awKeys.editItem("extquotes")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Package manager")
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Edit command")
                onClicked: awKeys.editItem("extupgrade")
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[dataengine::onCompleted]")

        // init submodule
        awKeys.initKeys(plasmoid.configuration.text)
    }

    Component.onDestruction: {
        if (debug) console.log("[dataengine::onDestruction]")

        cfg_dataengine["GPUDEV"] = gpuDev.currentText
        cfg_dataengine["HDDDEV"] = hdd.currentText
        cfg_dataengine["PLAYER"] = player.currentText
        cfg_dataengine["MPRIS"] = mpris.currentText
        awActions.writeDataEngineConfiguration(cfg_dataengine)
    }
}
