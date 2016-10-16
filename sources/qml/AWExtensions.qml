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

import org.kde.plasma.private.awesomewidget 1.0


Row {
    height: implicitHeight
    width: parent.width

    // backend
    property var backend
    AWFormatterConfigFactory {
        id: awFormatter
    }
    // parent object in which text will be replaced
    property var textArea

    signal unlock

    QtControls.Button {
        width: parent.width * 3 / 10
        text: i18n("Edit bars")
        onClicked: backend.editItem("graphicalitem")
    }

    QtControls.Button {
        width: parent.width * 3 / 10
        text: i18n("Formatters")
        onClicked: awFormatter.showDialog(backend.dictKeys(true))
    }

    QtControls.Button {
        width: parent.width * 5 / 15
        text: i18n("Preview")
        onClicked: {
            unlock()
            backend.initKeys(textArea.text, plasmoid.configuration.interval,
                             plasmoid.configuration.queueLimit, false)
        }
    }

    QtControls.Button {
        width: parent.width / 15
        iconName: "view-history"
        menu: QtControls.Menu {
            id: historyConfig
            Instantiator {
                model: awTelemetryHandler.get("awwidgetconfig")
                QtControls.MenuItem {
                    text: modelData
                    onTriggered: textArea.text = modelData
                }
                onObjectAdded: historyConfig.insertItem(index, object)
                onObjectRemoved: historyConfig.removeItem(object)
            }
        }
    }
}
