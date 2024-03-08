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

import QtQuick 2.15
import QtQuick.Controls

import org.kde.plasma.private.awesomewidget 1.0


Item {
    id: tooltipPage
    // backend
    AWActions {
        id: awActions
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awActions.isDebugEnabled()

    property alias cfg_tooltipNumber: tooltipNumber.value
    property alias cfg_useTooltipBackground: useTooltipBackground.checked
    property alias cfg_tooltipBackground: tooltipBackground.value
    property alias cfg_cpuTooltip: cpuTooltip.checked
    property alias cfg_cpuTooltipColor: cpuTooltipColor.value
    property alias cfg_cpuclTooltip: cpuclTooltip.checked
    property alias cfg_cpuclTooltipColor: cpuclTooltipColor.value
    property alias cfg_memTooltip: memTooltip.checked
    property alias cfg_memTooltipColor: memTooltipColor.value
    property alias cfg_swapTooltip: swapTooltip.checked
    property alias cfg_swapTooltipColor: swapTooltipColor.value
    property alias cfg_downkbTooltip: networkTooltip.checked
    property alias cfg_downkbTooltipColor: downkbTooltipColor.value
    property alias cfg_upkbTooltipColor: upkbTooltipColor.value
    property alias cfg_batTooltip: batTooltip.checked
    property alias cfg_batTooltipColor: batTooltipColor.value
    property alias cfg_batInTooltipColor: batInTooltipColor.value


    Column {
        id: pageColumn
        anchors.fill: parent

        Label {
          width: parent.width
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          wrapMode: Text.WordWrap
          text: i18n("CPU, CPU clock, memory, swap and network labels support graphical tooltip. To enable them just make needed checkbox checked.")
        }

        IntegerSelector {
            id: tooltipNumber
            maximumValue: 1000
            minimumValue: 50
            stepSize: 25
            text: i18n("Number of values for tooltips")
            value: plasmoid.configuration.tooltipNumber
        }

        GroupBox {
            id: useTooltipBackground
            width: parent.width

            property alias checked: useTooltipBackgroundLabel.checked
            label: CheckBox {
                id: useTooltipBackgroundLabel
                text: i18n("Background")
            }

            ColorSelector {
                id: tooltipBackground
                enabled: useTooltipBackgroundLabel.checked
                text: i18n("Background color")
                value: plasmoid.configuration.tooltipBackground
            }
        }

        GroupBox {
            id: cpuTooltip
            width: parent.width

            property alias checked: cpuTooltipLabel.checked
            label: CheckBox {
                id: cpuTooltipLabel
                text: i18n("CPU")
            }

            ColorSelector {
                id: cpuTooltipColor
                enabled: cpuTooltipLabel.checked
                text: i18n("CPU color")
                value: plasmoid.configuration.cpuTooltipColor
            }
        }

        GroupBox {
            id: cpuclTooltip
            width: parent.width

            property alias checked: cpuclTooltipLabel.checked
            label: CheckBox {
                id: cpuclTooltipLabel
                text: i18n("CPU clock")
            }

            ColorSelector {
                id: cpuclTooltipColor
                enabled: cpuclTooltipLabel.checked
                text: i18n("CPU clock color")
                value: plasmoid.configuration.cpuclTooltipColor
            }
        }

        GroupBox {
            id: memTooltip
            width: parent.width

            property alias checked: memTooltipLabel.checked
            label: CheckBox {
                id: memTooltipLabel
                text: i18n("Memory")
            }

            ColorSelector {
                id: memTooltipColor
                enabled: memTooltipLabel.checked
                text: i18n("Memory color")
                value: plasmoid.configuration.memTooltipColor
            }
        }

        GroupBox {
            id: swapTooltip
            width: parent.width

            property alias checked: swapTooltipLabel.checked
            label: CheckBox {
                id: swapTooltipLabel
                text: i18n("Swap")
            }

            ColorSelector {
                id: swapTooltipColor
                enabled: swapTooltipLabel.checked
                text: i18n("Swap color")
                value: plasmoid.configuration.swapTooltipColor
            }
        }

        GroupBox {
            id: networkTooltip
            width: parent.width

            property alias checked: networkTooltipLabel.checked
            label: CheckBox {
                id: networkTooltipLabel
                text: i18n("Network")
            }

            Column {
                width: parent.width
                enabled: networkTooltipLabel.checked
                ColorSelector {
                    id: downkbTooltipColor
                    text: i18n("Download speed color")
                    value: plasmoid.configuration.downkbTooltipColor
                }
                ColorSelector {
                    id: upkbTooltipColor
                    text: i18n("Upload speed color")
                    value: plasmoid.configuration.upkbTooltipColor
                }
            }
        }

        GroupBox {
            id: batTooltip
            width: parent.width

            property alias checked: batteryTooltipLabel.checked
            label: CheckBox {
                id: batteryTooltipLabel
                text: i18n("Battery")
            }

            Column {
                width: parent.width
                enabled: batteryTooltipLabel.checked
                ColorSelector {
                    id: batTooltipColor
                    text: i18n("Battery active color")
                    value: plasmoid.configuration.batTooltipColor
                }
                ColorSelector {
                    id: batInTooltipColor
                    text: i18n("Battery inactive color")
                    value: plasmoid.configuration.batInTooltipColor
                }
            }
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
