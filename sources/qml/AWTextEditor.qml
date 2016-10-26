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
import QtQuick.Controls 2.0 as QtControls


Item {
    width: parent.width
    height: parent.height * 4 / 5

    property var backend
    property alias text: textArea.text

    QtControls.TextArea {
        id: textArea
        anchors.fill: parent
        background: Rectangle {
            color: "white"
        }
        textFormat: TextEdit.PlainText

        QtControls.ToolTip {
            id: tooltip

            property string substring
            property var tags: []
            text: tags.join('\n')

            bottomPadding: 0
            topPadding: 0

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var tagHeight = tooltip.height / tooltip.tags.length
                    var pos = Math.round(mouse.y / tagHeight, 0)
                    appendTag(tooltip.tags[pos], tooltip.substring)
                }
            }
        }

        onTextChanged: {
            var currentTag = getLastTag()
            // exit if there are spaces or empty
            if ((currentTag.indexOf(" ") != -1) || (currentTag.length == 0)) {
                tooltip.visible = false
                return
            }
            // update tooltip and show it
            tooltip.substring = currentTag
            tooltip.tags = backend.dictKeys(true, "^" + tooltip.substring)
            changeTooltipPosition()
            tooltip.visible = (tooltip.tags.length != 0)
        }
    }

    function appendTag(tag, substring) {
        textArea.insert(textArea.cursorPosition, tag.substring(substring.length))
    }

    function changeTooltipPosition() {
        tooltip.x = textArea. cursorRectangle.x
        tooltip.y = textArea. cursorRectangle.y
    }

    function getLastTag() {
        // get substring to analyze
        var substring = textArea.getText(0, textArea.cursorPosition)
        // find last position of index in the given substring
        var signIndex = substring.lastIndexOf('$') + 1
        if ((signIndex == 0) || (signIndex == textArea.cursorPosition))
            return ""
        // get current tag text
        return substring.substr(signIndex)
    }
}


