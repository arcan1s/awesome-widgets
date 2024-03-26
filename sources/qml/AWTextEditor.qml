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


ScrollView {
    width: parent.width
    height: parent.height * 4 / 5

    property var backend
    property alias text: textArea.text

    TextArea {
        id: textArea
        textFormat: TextEdit.PlainText

        Column {
            id: tooltip

            property string substring

            Repeater {
                id: tags
                Text {
                    id: text
                    text: modelData
                    MouseArea {
                        anchors.fill: parent
                        onClicked: appendTag(text.text, tooltip.substring)
                    }
                }
            }
        }

        onTextChanged: {
            let currentTag = getLastTag()
            // exit if there are spaces or empty
            if ((currentTag.indexOf(" ") !== -1) || (currentTag.length === 0)) {
                tooltip.visible = false
                return
            }
            // update tooltip and show it
            tooltip.substring = currentTag
            tags.model = backend.dictKeys(true, "^" + tooltip.substring)
            changeTooltipPosition()
            // show tooltip if found more than 1 or current text does not match
            // tag found
            tooltip.visible = ((tags.count > 1)
                               || ((tags.count === 1) && (tags.model[0] !== tooltip.substring)))
        }
    }

    function appendTag(tag, substring) {
        insert(tag.substring(substring.length))
    }

    function changeTooltipPosition() {
        tooltip.x = textArea.cursorRectangle.x
        tooltip.y = textArea.cursorRectangle.y + textArea.cursorRectangle.height
    }

    function getLastTag() {
        // get substring to analyze
        var substring = textArea.getText(0, textArea.cursorPosition)
        // find last position of index in the given substring
        var signIndex = substring.lastIndexOf('$') + 1
        if ((signIndex === 0) || (signIndex === textArea.cursorPosition))
            return ""
        // get current tag text
        return substring.substr(signIndex)
    }

    function insert(text) {
        textArea.insert(textArea.cursorPosition, text)
    }
}


