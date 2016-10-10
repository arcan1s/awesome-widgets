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

import org.kde.plasma.private.desktoppanel 1.0
import "."


Item {
    id: activeAppearancePage
    // backend
    DPAdds {
        id: dpAdds
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: dpAdds.isDebugEnabled()

    property alias cfg_currentFontFamily: font.value
    property alias cfg_currentFontSize: fontSize.value
    property string cfg_currentFontWeight: fontWeight.value
    property string cfg_currentFontStyle: fontStyle.value
    property alias cfg_currentFontColor: selectColor.text
    property alias cfg_currentTextStyleColor: selectStyleColor.text
    property string cfg_currentTextStyle: textStyle.value


    Column {
        id: pageColumn
        anchors.fill: parent

        FontSelector {
            id: font
            text: i18n("Font")
            value: plasmoid.configuration.currentFontFamily
        }

        IntegerSelector {
            id: fontSize
            maximumValue: 32
            minimumValue: 8
            stepSize: 1
            text: i18n("Font size")
            value: plasmoid.configuration.currentFontSize
        }

        ComboBoxSelector {
            id: fontWeight
            model: general.fontWeightModel
            text: i18n("Font weight")
            value: plasmoid.configuration.currentFontWeight
            onValueEdited: cfg_currentFontWeight = newValue
        }

        ComboBoxSelector {
            id: fontStyle
            model: general.fontStyleModel
            text: i18n("Font style")
            value: plasmoid.configuration.currentFontStyle
            onValueEdited: cfg_currentFontStyle = newValue
        }

        ColorSelector {
            id: selectColor
            text: i18n("Font color")
            value: plasmoid.configuration.currentFontColor
        }

        ComboBoxSelector {
            id: textStyle
            model: general.textStyleModel
            text: i18n("Style")
            value: plasmoid.configuration.currentTextStyle
            onValueEdited: cfg_currentTextStyle = newValue
        }

        ColorSelector {
            id: selectStyleColor
            text: i18n("Style color")
            value: plasmoid.configuration.currentTextStyleColor
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
