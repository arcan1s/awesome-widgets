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
        id: dpAdds
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: dpAdds.isDebugEnabled()

    property alias cfg_background: background.checked
    property alias cfg_verticalLayout: verticalLayout.checked
    property alias cfg_height: widgetHeight.value
    property alias cfg_width: widgetWidth.value
    property string cfg_mark: mark.value
    property string cfg_tooltipType: tooltipType.value
    property alias cfg_tooltipWidth: tooltipWidth.value
    property alias cfg_tooltipColor: tooltipColor.value


    Column {
        id: pageColumn
        anchors.fill: parent

        CheckBoxSelector {
            id: background
            text: i18n("Enable background")
        }

        CheckBoxSelector {
            id: verticalLayout
            text: i18n("Vertical layout")
        }

        IntegerSelector {
            id: widgetHeight
            maximumValue: 4096
            minimumValue: 0
            stepSize: 50
            text: i18n("Widget height, px")
            value: plasmoid.configuration.height
        }

        IntegerSelector {
            id: widgetWidth
            maximumValue: 4096
            minimumValue: 0
            stepSize: 50
            text: i18n("Widget width, px")
            value: plasmoid.configuration.width
        }

        ComboBoxSelector {
            id: mark
            editable: true
            model: [
                {
                    'label': '#',
                    'name': '#'
                },
                {
                    'label': '$',
                    'name': '$'
                },
                {
                    'label': '%',
                    'name': '%'
                },
                {
                    'label': '&',
                    'name': '&'
                },
                {
                    'label': '*',
                    'name': '*'
                },
                {
                    'label': '@',
                    'name': '@'
                },
                {
                    'label': '¤',
                    'name': '¤'
                },
                {
                    'label': '¶',
                    'name': '¶'
                },
                {
                    'label': '·',
                    'name': '·'
                },
                {
                    'label': 'º',
                    'name': 'º'
                },
                {
                    'label': plasmoid.configuration.mark,
                    'name': plasmoid.configuration.mark
                }
            ]
            text: i18n("Mark")
            currentIndex: model.length - 1
            onValueEdited: cfg_mark = newValue
        }

        QtControls.GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("Tooltip")

            Column {
                height: implicitHeight
                width: parent.width

                ComboBoxSelector {
                    id: tooltipType
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
                    text: i18n("Tooltip type")
                    value: plasmoid.configuration.tooltipType
                    onValueEdited: cfg_tooltipType = newValue
                }

                IntegerSelector {
                    id: tooltipWidth
                    maximumValue: 1000
                    minimumValue: 100
                    stepSize: 50
                    text: i18n("Tooltip width")
                    value: plasmoid.configuration.tooltipWidth
                }

                ColorSelector {
                    id: tooltipColor
                    text: i18n("Font color")
                    value: plasmoid.configuration.tooltipColor
                }
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.debug()
    }
}
