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


Button {
    // parent object in which text will be replaced
    property var textField
    // start and end tags
    property string start: ""
    property string end: ""

    property var clickedEvent: function() { return updateText() }
    onClicked: clickedEvent()

    function updateText() {
        // get selected text
        const selected = textField.selectedText()
        // remove it from widget
        textField.removeSelection()
        // insert edited text
        textField.insert(start + selected + end)
    }
}
