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
    property alias cfg_downkbTooltip: downkbTooltip.checked
    property alias cfg_downkbTooltipColor: downkbTooltipColor.value
    property alias cfg_upkbTooltipColor: upkbTooltipColor.value
    property alias cfg_batTooltip: batTooltip.checked
    property alias cfg_batTooltipColor: batTooltipColor.value
    property alias cfg_batInTooltipColor: batInTooltipColor.value


    Column {
        id: pageColumn
        anchors.fill: parent
        QtControls.Label {
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

        QtControls.GroupBox {
            id: useTooltipBackground
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Background")
            ColorSelector {
                id: tooltipBackground
                text: i18n("Background color")
                value: plasmoid.configuration.tooltipBackground
            }
        }

        QtControls.GroupBox {
            id: cpuTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("CPU")
            ColorSelector {
                id: cpuTooltipColor
                text: i18n("CPU color")
                value: plasmoid.configuration.cpuTooltipColor
            }
        }

        QtControls.GroupBox {
            id: cpuclTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("CPU clock")
            ColorSelector {
                id: cpuclTooltipColor
                text: i18n("CPU clock color")
                value: plasmoid.configuration.cpuclTooltipColor
            }
        }

        QtControls.GroupBox {
            id: memTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Memory")
            ColorSelector {
                id: memTooltipColor
                text: i18n("Memory color")
                value: plasmoid.configuration.memTooltipColor
            }
        }

        QtControls.GroupBox {
            id: swapTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Swap")
            ColorSelector {
                id: swapTooltipColor
                text: i18n("Swap color")
                value: plasmoid.configuration.swapTooltipColor
            }
        }

        QtControls.GroupBox {
            id: downkbTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Network")
            Column {
                height: implicitHeight
                width: parent.width
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

        QtControls.GroupBox {
            id: batTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Battery")
            Column {
                height: implicitHeight
                width: parent.width
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
