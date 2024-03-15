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


Row {
    height: implicitHeight
    width: parent.width

    property alias text: label.text
    property alias value: spinBox.value

    property alias maximumValue: spinBox.to
    property alias minimumValue: spinBox.from
    property alias stepSize: spinBox.stepSize

    signal valueEdited(int newValue)

    Label {
        id: label
        height: parent.height
        width: parent.width * 2 / 5
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }
    SpinBox {
        id: spinBox
        width: parent.width * 3 / 5
        onValueModified: valueEdited(spinBox.value)
    }
}
