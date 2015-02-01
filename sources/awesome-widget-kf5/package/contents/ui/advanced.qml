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
import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.plasma.private.awesomewidget 1.0


Item {
    id: advancedPage
    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: AWKeys.isDebugEnabled()
    property variant settings: {
        "customTime": plasmoid.configuration.customTime,
        "customUptime": plasmoid.configuration.customUptime,
        "tempUnits": plasmoid.configuration.tempUnits,
        "acOnline": plasmoid.configuration.acOnline,
        "acOffline": plasmoid.configuration.acOffline
    }
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
        "batteryTooltip": plasmoid.configuration.batteryTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downTooltipColor": plasmoid.configuration.downTooltipColor,
        "upTooltipColor": plasmoid.configuration.upTooltipColor,
        "batteryTooltipColor": plasmoid.configuration.batteryTooltipColor,
        "batteryInTooltipColor": plasmoid.configuration.batteryInTooltipColor
    }

    property alias cfg_background: background.checked
    property alias cfg_customTime: customTime.text
    property alias cfg_customUptime: customUptime.text
    property string cfg_tempUnits: tempUnits.currentText
    property alias cfg_acOnline: acOnline.text
    property alias cfg_acOffline: acOffline.text


    Column {
        id: pageColumn
        width: units.gridUnit * 25
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: background
                width: parent.width * 3 / 5
                text: i18n("Enable background")
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
                text: i18n("Custom time format")
            }
            QtControls.TextField {
                id: customTime
                width: parent.width * 3 / 5
                text: plasmoid.configuration.customTime
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
                text: i18n("Custom uptime format")
            }
            QtControls.TextField {
                id: customUptime
                width: parent.width * 3 / 5
                text: plasmoid.configuration.customUptime
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
                text: i18n("Temperature units")
            }
            QtControls.ComboBox {
                id: tempUnits
                width: parent.width * 3 / 5
                textRole: "label"
                model: [
                    {
                        'label': i18n("Celsius"),
                        'name': "Celsius"
                    },
                    {
                        'label': i18n("Fahrenheit"),
                        'name': "Fahrenheit"
                    },
                    {
                        'label': i18n("Kelvin"),
                        'name': "Kelvin"
                    },
                    {
                        'label': i18n("Reaumur"),
                        'name': "Reaumur"
                    },
                    {
                        'label': i18n("cm^-1"),
                        'name': "cm^-1"
                    },
                    {
                        'label': i18n("kJ/mol"),
                        'name': "kJ/mol"
                    },
                    {
                        'label': i18n("kcal/mol"),
                        'name': "kcal/mol"
                    }
                ]
                onCurrentIndexChanged: cfg_tempUnits = model[currentIndex]["name"]
                Component.onCompleted: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.tempUnits) {
                            tempUnits.currentIndex = i;
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
                text: i18n("AC online tag")
            }
            QtControls.TextField {
                id: acOnline
                width: parent.width * 3 / 5
                text: plasmoid.configuration.acOnline
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
                text: i18n("AC offline tag")
            }
            QtControls.TextField {
                id: acOffline
                width: parent.width * 3 / 5
                text: plasmoid.configuration.acOffline
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
                text: i18n("Bars")
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Edit bars")
                onClicked: AWKeys.editItem("graphicalitem")
            }
        }
    }

    // we need to initializate DataEngines here too
    // because we need to get keys and values
    PlasmaCore.DataSource {
        id: systemmonitorDE
        engine: "systemmonitor"
        connectedSources: systemmonitorDE.sources
        interval: 5000

        onNewData: {
            if (debug) console.log("[advanced::onNewData] : Update source " + sourceName)

            AWKeys.setDataBySource(sourceName, data, settings)
        }
    }

    PlasmaCore.DataSource {
        id: extsysmonDE
        engine: "extsysmon"
        connectedSources: ["battery", "custom", "desktop", "netdev", "gpu",
                           "gputemp", "hddtemp", "pkg", "player", "ps", "update"]
        interval: 5000

        onNewData: {
            if (debug) console.log("[advanced::onNewData] : Update source " + sourceName)

            AWKeys.setDataBySource(sourceName, data, settings)
        }
    }

    PlasmaCore.DataSource {
        id: timeDE
        engine: "time"
        connectedSources: ["Local"]
        interval: 5000

        onNewData: {
            if (debug) console.log("[advanced::onNewData] : Update source " + sourceName)

            AWKeys.setDataBySource(sourceName, data, settings)
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[advanced::onCompleted]")

        // init submodule
        AWKeys.initKeys(plasmoid.configuration.text, tooltipSettings)
    }
}
