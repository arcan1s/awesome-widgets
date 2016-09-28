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
    id: appearancePage
    // backend
    AWActions {
        id: awActions
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awActions.isDebugEnabled()
    property variant weight: {
        25: 0,
        50: 1,
        63: 3,
        75: 4,
        87: 5
    }

    property alias cfg_fontFamily: selectFont.text
    property alias cfg_fontSize: fontSize.value
    property string cfg_fontWeight: fontWeight.value
    property string cfg_fontStyle: fontStyle.value
    property alias cfg_fontColor: selectColor.text
    property alias cfg_textStyleColor: selectStyleColor.text
    property string cfg_textStyle: textStyle.value


    Column {
        id: pageColumn
        anchors.fill: parent
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Font")
            }
            QtControls.Button {
                id: selectFont
                width: parent.width * 2 / 3
                text: plasmoid.configuration.fontFamily
                onClicked: {
                    if (debug) console.debug()
                    fontDialog.setFont()
                    fontDialog.visible = true
                }
            }
        }

        IntegerSelector {
            id: fontSize
            maximumValue: 32
            minimumValue: 8
            stepSize: 1
            text: i18n("Font size")
            value: plasmoid.configuration.fontSize
        }

        ComboBoxSelector {
            id: fontWeight
            model: [
                {
                    'label': i18n("light"),
                    'name': "light"
                },
                {
                    'label': i18n("normal"),
                    'name': "normal"
                },
                {
                    'label': i18n("demi bold"),
                    'name': "demibold"
                },
                {
                    'label': i18n("bold"),
                    'name': "bold"
                },
                {
                    'label': i18n("black"),
                    'name': "black"
                }
            ]
            text: i18n("Font weight")
            value: plasmoid.configuration.fontWeight
            onValueEdited: cfg_fontWeight = newValue
        }

        ComboBoxSelector {
            id: fontStyle
            model: [
                {
                    'label': i18n("normal"),
                    'name': "normal"
                },
                {
                    'label': i18n("italic"),
                    'name': "italic"
                }
            ]
            text: i18n("Font style")
            value: plasmoid.configuration.fontStyle
            onValueEdited: cfg_fontStyle = newValue
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Font color")
            }
            QtControls.Button {
                id: selectColor
                width: parent.width * 2 / 3
                style: QtStyles.ButtonStyle {
                    background: Rectangle {
                        color: plasmoid.configuration.fontColor
                    }
                }
                text: plasmoid.configuration.fontColor
                onClicked: colorDialog.visible = true
            }
        }

        ComboBoxSelector {
            id: textStyle
            model: [
                {
                    'label': i18n("normal"),
                    'name': "normal"
                },
                {
                    'label': i18n("outline"),
                    'name': "outline"
                },
                {
                    'label': i18n("raised"),
                    'name': "raised"
                },
                {
                    'label': i18n("sunken"),
                    'name': "sunken"
                }
            ]
            text: i18n("Style")
            value: plasmoid.configuration.textStyle
            onValueEdited: cfg_textStyle = newValue
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Style color")
            }
            QtControls.Button {
                id: selectStyleColor
                width: parent.width * 2 / 3
                style: QtStyles.ButtonStyle {
                    background: Rectangle {
                        color: plasmoid.configuration.textStyleColor
                    }
                }
                text: plasmoid.configuration.textStyleColor
                onClicked: textStyleColorDialog.visible = true
            }
        }
    }

    QtDialogs.ColorDialog {
        id: colorDialog
        title: i18n("Select a color")
        color: selectColor.text
        onAccepted: selectColor.text = colorDialog.color
    }

    QtDialogs.ColorDialog {
        id: textStyleColorDialog
        title: i18n("Select a color")
        color: selectStyleColor.text
        onAccepted: selectStyleColor.text = textStyleColorDialog.color
    }

    QtDialogs.FontDialog {
        id: fontDialog
        title: i18n("Select a font")
        signal setFont

        onAccepted: {
            if (debug) console.debug()
            selectFont.text = fontDialog.font.family
            fontSize.value = fontDialog.font.pointSize
            fontStyle.currentIndex = fontDialog.font.italic ? 1 : 0
            fontWeight.currentIndex = weight[fontDialog.font.weight]
        }
        onSetFont: {
            if (debug) console.debug()
            fontDialog.font = Qt.font({
                family: selectFont.text,
                pointSize: fontSize.value > 0 ? fontSize.value : 12,
                italic: fontStyle.currentIndex == 1,
                weight: Font.Normal,
            })
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
