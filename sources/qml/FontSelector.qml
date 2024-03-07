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
import QtQuick.Dialogs


Row {
    height: implicitHeight
    width: parent.width

    property alias text: selector.text
    property alias value: selector.value

    ButtonSelector {
        id: selector
        text: label.text
        onButtonActivated: {
            fontDialog.setFont()
            fontDialog.visible = true
        }
    }

    FontDialog {
        id: fontDialog
        title: i18n("Select a font")

        signal setFont

        onAccepted: value = selectedFont.family
        onSetFont: {
            selectedFont = Qt.font({
                family: value,
                pointSize: 12,
                italic: false,
                weight: Font.Normal,
            })
        }
    }
}
