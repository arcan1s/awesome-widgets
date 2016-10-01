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

    property alias cfg_fontFamily: font.value
    property alias cfg_fontSize: fontSize.value
    property string cfg_fontWeight: fontWeight.value
    property string cfg_fontStyle: fontStyle.value
    property alias cfg_fontColor: selectColor.value
    property alias cfg_textStyleColor: selectStyleColor.value
    property string cfg_textStyle: textStyle.value


    Column {
        id: pageColumn
        anchors.fill: parent

        FontSelector {
            id: font
            text: i18n("Font")
            value: plasmoid.configuration.fontFamily
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

        ColorSelector {
            id: selectColor
            text: i18n("Font color")
            value: plasmoid.configuration.fontColor
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

        ColorSelector {
            id: selectStyleColor
            text: i18n("Style color")
            value: plasmoid.configuration.textStyleColor
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
