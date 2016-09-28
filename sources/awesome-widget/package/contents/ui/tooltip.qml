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
import QtQuick.Controls.Styles 1.3 as QtStyles
import QtQuick.Dialogs 1.1 as QtDialogs

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
    property alias cfg_tooltipBackground: tooltipBackground.text
    property alias cfg_cpuTooltip: cpuTooltip.checked
    property alias cfg_cpuTooltipColor: cpuTooltipColor.text
    property alias cfg_cpuclTooltip: cpuclTooltip.checked
    property alias cfg_cpuclTooltipColor: cpuclTooltipColor.text
    property alias cfg_memTooltip: memTooltip.checked
    property alias cfg_memTooltipColor: memTooltipColor.text
    property alias cfg_swapTooltip: swapTooltip.checked
    property alias cfg_swapTooltipColor: swapTooltipColor.text
    property alias cfg_downkbTooltip: downkbTooltip.checked
    property alias cfg_downkbTooltipColor: downkbTooltipColor.text
    property alias cfg_upkbTooltipColor: upkbTooltipColor.text
    property alias cfg_batTooltip: batTooltip.checked
    property alias cfg_batTooltipColor: batTooltipColor.text
    property alias cfg_batInTooltipColor: batInTooltipColor.text


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
            Row {
                height: implicitHeight
                width: parent.width
                QtControls.Label {
                    height: parent.height
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("Background color")
                }
                QtControls.Button {
                    id: tooltipBackground
                    width: parent.width * 3 / 5
                    style: QtStyles.ButtonStyle {
                        background: Rectangle {
                            color: plasmoid.configuration.tooltipBackground
                        }
                    }
                    text: plasmoid.configuration.tooltipBackground
                    onClicked: tooltipBackgroundDialog.visible = true
                }

                QtDialogs.ColorDialog {
                    id: tooltipBackgroundDialog
                    title: i18n("Select a color")
                    color: tooltipBackground.text
                    onAccepted: tooltipBackground.text = tooltipBackgroundDialog.color
                }
            }
        }

        QtControls.GroupBox {
            id: cpuTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("CPU")
            Row {
                height: implicitHeight
                width: parent.width
                QtControls.Label {
                    height: parent.height
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("CPU color")
                }
                QtControls.Button {
                    id: cpuTooltipColor
                    width: parent.width * 3 / 5
                    style: QtStyles.ButtonStyle {
                        background: Rectangle {
                            color: plasmoid.configuration.cpuTooltipColor
                        }
                    }
                    text: plasmoid.configuration.cpuTooltipColor
                    onClicked: cpuTooltipColorDialog.visible = true
                }

                QtDialogs.ColorDialog {
                    id: cpuTooltipColorDialog
                    title: i18n("Select a color")
                    color: cpuTooltipColor.text
                    onAccepted: cpuTooltipColor.text = cpuTooltipColorDialog.color
                }
            }
        }

        QtControls.GroupBox {
            id: cpuclTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("CPU clock")
            Row {
                height: implicitHeight
                width: parent.width
                QtControls.Label {
                    height: parent.height
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("CPU clock color")
                }
                QtControls.Button {
                    id: cpuclTooltipColor
                    width: parent.width * 3 / 5
                    style: QtStyles.ButtonStyle {
                        background: Rectangle {
                            color: plasmoid.configuration.cpuclTooltipColor
                        }
                    }
                    text: plasmoid.configuration.cpuclTooltipColor
                    onClicked: cpuclTooltipColorDialog.visible = true
                }

                QtDialogs.ColorDialog {
                    id: cpuclTooltipColorDialog
                    title: i18n("Select a color")
                    color: cpuclTooltipColor.text
                    onAccepted: cpuclTooltipColor.text = cpuclTooltipColorDialog.color
                }
            }
        }

        QtControls.GroupBox {
            id: memTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Memory")
            Row {
                height: implicitHeight
                width: parent.width
                QtControls.Label {
                    height: parent.height
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("Memory color")
                }
                QtControls.Button {
                    id: memTooltipColor
                    width: parent.width * 3 / 5
                    style: QtStyles.ButtonStyle {
                        background: Rectangle {
                            color: plasmoid.configuration.memTooltipColor
                        }
                    }
                    text: plasmoid.configuration.memTooltipColor
                    onClicked: memTooltipColorDialog.visible = true
                }

                QtDialogs.ColorDialog {
                    id: memTooltipColorDialog
                    title: i18n("Select a color")
                    color: memTooltipColor.text
                    onAccepted: memTooltipColor.text = memTooltipColorDialog.color
                }
            }
        }

        QtControls.GroupBox {
            id: swapTooltip
            height: implicitHeight
            width: parent.width
            checkable: true
            title: i18n("Swap")
            Row {
                height: implicitHeight
                width: parent.width
                QtControls.Label {
                    height: parent.height
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("Swap color")
                }
                QtControls.Button {
                    id: swapTooltipColor
                    width: parent.width * 3 / 5
                    style: QtStyles.ButtonStyle {
                        background: Rectangle {
                            color: plasmoid.configuration.swapTooltipColor
                        }
                    }
                    text: plasmoid.configuration.swapTooltipColor
                    onClicked: swapTooltipColorDialog.visible = true
                }

                QtDialogs.ColorDialog {
                    id: swapTooltipColorDialog
                    title: i18n("Select a color")
                    color: swapTooltipColor.text
                    onAccepted: swapTooltipColor.text = swapTooltipColorDialog.color
                }
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
                Row {
                    height: implicitHeight
                    width: parent.width
                    QtControls.Label {
                        height: parent.height
                        width: parent.width * 2 / 5
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: i18n("Download speed color")
                    }
                    QtControls.Button {
                        id: downkbTooltipColor
                        width: parent.width * 3 / 5
                        style: QtStyles.ButtonStyle {
                            background: Rectangle {
                                color: plasmoid.configuration.downkbTooltipColor
                            }
                        }
                        text: plasmoid.configuration.downkbTooltipColor
                        onClicked: downkbTooltipColorDialog.visible = true
                    }

                    QtDialogs.ColorDialog {
                        id: downkbTooltipColorDialog
                        title: i18n("Select a color")
                        color: downkbTooltipColor.text
                        onAccepted: downkbTooltipColor.text = downkbTooltipColorDialog.color
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
                        text: i18n("Upload speed color")
                    }
                    QtControls.Button {
                        id: upkbTooltipColor
                        width: parent.width * 3 / 5
                        style: QtStyles.ButtonStyle {
                            background: Rectangle {
                                color: plasmoid.configuration.upkbTooltipColor
                            }
                        }
                        text: plasmoid.configuration.upkbTooltipColor
                        onClicked: upkbTooltipColorDialog.visible = true
                    }

                    QtDialogs.ColorDialog {
                        id: upkbTooltipColorDialog
                        title: i18n("Select a color")
                        color: upkbTooltipColor.text
                        onAccepted: upkbTooltipColor.text = upkbTooltipColorDialog.color
                    }
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
                Row {
                    height: implicitHeight
                    width: parent.width
                    QtControls.Label {
                        height: parent.height
                        width: parent.width * 2 / 5
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: i18n("Battery active color")
                    }
                    QtControls.Button {
                        id: batTooltipColor
                        width: parent.width * 3 / 5
                        style: QtStyles.ButtonStyle {
                            background: Rectangle {
                                color: plasmoid.configuration.batTooltipColor
                            }
                        }
                        text: plasmoid.configuration.batTooltipColor
                        onClicked: batTooltipColorDialog.visible = true
                    }

                    QtDialogs.ColorDialog {
                        id: batTooltipColorDialog
                        title: i18n("Select a color")
                        color: batTooltipColor.text
                        onAccepted: batTooltipColor.text = batTooltipColorDialog.color
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
                        text: i18n("Battery inactive color")
                    }
                    QtControls.Button {
                        id: batInTooltipColor
                        width: parent.width * 3 / 5
                        style: QtStyles.ButtonStyle {
                            background: Rectangle {
                                color: plasmoid.configuration.batInTooltipColor
                            }
                        }
                        text: plasmoid.configuration.batInTooltipColor
                        onClicked: batInTooltipColorDialog.visible = true
                    }

                    QtDialogs.ColorDialog {
                        id: batInTooltipColorDialog
                        title: i18n("Select a color")
                        color: batInTooltipColor.text
                        onAccepted: batInTooltipColor.text = batInTooltipColorDialog.color
                    }
                }
            }
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
