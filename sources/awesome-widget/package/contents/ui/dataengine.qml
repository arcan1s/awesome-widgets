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
import QtQuick.Controls 1.3 as QtControls
import QtQuick.Dialogs 1.1 as QtDialogs

import org.kde.plasma.private.awesomewidget 1.0


Item {
    id: dataenginePage
    // backend
    AWKeys {
        id: awKeys
    }
    AWActions {
        id: awActions
    }
    AWConfigHelper {
        id: awConfig
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awActions.isDebugEnabled()

    property variant cfg_dataengine: awConfig.readDataEngineConfiguration()


    Column {
        id: pageColumn
        anchors.fill: parent
        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("ACPI")
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
                    onEditingFinished: cfg_dataengine["ACPIPATH"] = text
                }
            }
        }

        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("GPU")
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
                        if (debug) console.debug()
                        for (var i=0; i<model.length; i++) {
                            if (model[i] == cfg_dataengine["GPUDEV"]) {
                                if (debug) console.info("Found", model[i], "on", i)
                                currentIndex = i
                            }
                        }
                    }
                }
            }
        }

        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("HDD temperature")
            Column {
                height: implicitHeight
                width: parent.width
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
                        onEditingFinished: cfg_dataengine["HDDTEMPCMD"] = text
                    }
                }
            }
        }

        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("Player")
            Column {
                height: implicitHeight
                width: parent.width
                Row {
                    height: implicitHeight
                    width: parent.width
                    QtControls.Label {
                        height: parent.height
                        width: parent.width * 2 / 5
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: i18n("Player data symbols")
                    }
                    QtControls.SpinBox {
                        width: parent.width * 3 / 5
                        minimumValue: 1
                        maximumValue: 100
                        stepSize: 1
                        value: cfg_dataengine["PLAYERSYMBOLS"]
                        onEditingFinished: cfg_dataengine["PLAYERSYMBOLS"] = value
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
                        model: ["disable", "mpris", "mpd"]
                        Component.onCompleted: {
                            if (debug) console.debug()
                            for (var i=0; i<model.length; i++) {
                                if (model[i] == cfg_dataengine["PLAYER"]) {
                                    if (debug) console.info("Found", model[i], "on", i)
                                    player.currentIndex = i
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
                        text: i18n("MPD address")
                    }
                    QtControls.TextField {
                        width: parent.width * 3 / 5
                        text: cfg_dataengine["MPDADDRESS"]
                        onEditingFinished: cfg_dataengine["MPDADDRESS"] = text
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
                        onEditingFinished: cfg_dataengine["MPDPORT"] = value
                    }
                }
            }
        }

        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("Extensions")
            Column {
                height: implicitHeight
                width: parent.width
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

                Row {
                    height: implicitHeight
                    width: parent.width
                    QtControls.Label {
                        height: parent.height
                        width: parent.width * 2 / 5
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: i18n("Weather")
                    }
                    QtControls.Button {
                        width: parent.width * 3 / 5
                        text: i18n("Edit weather")
                        onClicked: awKeys.editItem("extweather")
                    }
                }
            }
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()

        // init submodule
        awKeys.updateCache()

        // update hdd model
        hdd.model = awKeys.getHddDevices()
        for (var i=0; i<hdd.model.length; i++) {
            if (hdd.model[i] == cfg_dataengine["HDDDEV"]) {
                if (debug) console.info("Found", hdd.model[i], "on", i)
                hdd.currentIndex = i
            }
        }
    }

    Component.onDestruction: {
        if (debug) console.debug()

        cfg_dataengine["GPUDEV"] = gpuDev.currentText
        cfg_dataengine["HDDDEV"] = hdd.currentText
        cfg_dataengine["PLAYER"] = player.currentText
        cfg_dataengine["MPRIS"] = mpris.currentText
        awConfig.writeDataEngineConfiguration(cfg_dataengine)
    }
}
