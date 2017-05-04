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

import QtQuick 2.2
import QtQuick.Controls 1.3 as QtControls


Row {
    height: implicitHeight
    width: parent.width

    // backend
    property var backend
    property var notifyBackend
    // parent object in which text will be replaced
    property var textArea
    property alias groups: tagGroups.model

    QtControls.ComboBox {
        id: tagGroups
        width: parent.width * 2 / 5
        textRole: "label"

        onCurrentIndexChanged: {
            tags.model = backend.dictKeys(true, model[currentIndex]["regexp"])
            tags.currentIndex = -1
        }
    }

    QtControls.ComboBox {
        id: tags
        width: parent.width * 1 / 5
    }

    QtControls.Button {
        width: parent.width * 1 / 5
        text: i18n("Add")

        onClicked: {
            if (!tags.currentText)
                return
            textArea.insert("$" + tags.currentText)
        }
    }

    QtControls.Button {
        width: parent.width * 1 / 5
        text: i18n("Show value")

        onClicked: {
            if ((!notifyBackend)
                || (!tags.currentText))
                return
            // generate message
            var message = i18n("Tag: %1", tags.currentText)
            message += "<br>"
            message += i18n("Value: %1", backend.valueByKey(tags.currentText))
            message += "<br>"
            message += i18n("Info: %1", backend.infoByKey(tags.currentText))
            notifyBackend.sendNotification("tag", message)
        }
    }
}
