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

import org.kde.plasma.private.desktoppanel 1.0


Item {
    id: advancedPage
    // backend
    DPAdds {
        id: dpAdds;
    }
    
    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: dpAdds.isDebugEnabled()

    property alias cfg_height: widgetHeight.value
    property alias cfg_width: widgetWidth.value
    property string cfg_tooltipType: tooltipType.currentText
    property alias cfg_tooltipWidth: tooltipWidth.value
    property alias cfg_tooltipColor: tooltipColor.text
    property alias cfg_background: background.checked
    property alias cfg_verticalLayout: verticalLayout.checked
    property string cfg_mark: mark.currentText


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
                text: i18n("Widget height, px")
            }
            QtControls.SpinBox {
                id: widgetHeight
                width: parent.width * 3 / 5
                minimumValue: 0
                maximumValue: 4096
                stepSize: 50
                value: plasmoid.configuration.height
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
                text: i18n("Widget width, px")
            }
            QtControls.SpinBox {
                id: widgetWidth
                width: parent.width * 3 / 5
                minimumValue: 0
                maximumValue: 4096
                stepSize: 50
                value: plasmoid.configuration.width
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
                text: i18n("Tooltip type")
            }
            QtControls.ComboBox {
                id: tooltipType
                width: parent.width * 3 / 5
                textRole: "label"
                model: [
                    {
                        'label': i18n("contours"),
                        'name': "contours"
                    },
                    {
                        'label': i18n("windows"),
                        'name': "windows"
                    },
                    {
                        'label': i18n("clean desktop"),
                        'name': "clean"
                    },
                    {
                        'label': i18n("names"),
                        'name': "names"
                    },
                    {
                        'label': i18n("none"),
                        'name': "none"
                    }
                ]
                onCurrentIndexChanged: cfg_tooltipType = model[currentIndex]["name"]
                Component.onCompleted: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.tooltipType) {
                            tooltipType.currentIndex = i;
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
                text: i18n("Tooltip width")
            }
            QtControls.SpinBox {
                id: tooltipWidth
                width: parent.width * 3 / 5
                minimumValue: 100
                maximumValue: 1000
                stepSize: 50
                value: plasmoid.configuration.tooltipWidth
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
                text: i18n("Font color")
            }
            QtControls.Button {
                id: tooltipColor
                width: parent.width * 3 / 5
                style: QtStyles.ButtonStyle {
                    background: Rectangle {
                        color: plasmoid.configuration.tooltipColor
                    }
                }
                text: plasmoid.configuration.tooltipColor
                onClicked: colorDialog.visible = true
            }
        }

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
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: verticalLayout
                width: parent.width * 3 / 5
                text: i18n("Vertical layout")
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
                text: i18n("Mark")
            }
            QtControls.ComboBox {
                id: mark
                width: parent.width * 3 / 5
                editable: true
                model: ["#", "$", "%", "&", "*", "@", "¤", "¶", "·", "º",
                        plasmoid.configuration.mark]
                currentIndex: model.length - 1
                onCurrentIndexChanged: cfg_mark = currentText
            }
        }

//         Row {
//             height: implicitHeight
//             width: parent.width
//             QtControls.Label {
//                 height: parent.height
//                 width: parent.width * 2 / 5
//                 horizontalAlignment: Text.AlignRight
//                 verticalAlignment: Text.AlignVCenter
//                 text: i18n("Panels to be hidden")
//             }
//             QtControls.Button {
//                 width: parent.width * 3 / 5
//                 text: i18n("Edit panels")
//                 onClicked: cfg_panels = dpAdds.editPanelsToContol(plasmoid.configuration.panels)
//             }
//         }
    }

    QtDialogs.ColorDialog {
        id: colorDialog
        title: i18n("Select a color")
        color: tooltipColor.text
        onAccepted: tooltipColor.text = colorDialog.color
    }

    Component.onCompleted: {
        if (debug) console.log("[advanced::onCompleted]")
    }
}
