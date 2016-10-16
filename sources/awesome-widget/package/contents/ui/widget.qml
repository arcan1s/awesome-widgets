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
import QtQuick.Dialogs 1.2 as QtDialogs

import org.kde.plasma.private.awesomewidget 1.0
import "."


Item {
    id: widgetPage
    // backend
    AWKeys {
        id: awKeys
    }
    AWActions {
        id: awActions
    }
    AWTelemetryHandler {
        id: awTelemetryHandler
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awActions.isDebugEnabled()

    property alias cfg_text: textPattern.text
    property bool lock: true

    signal needTextUpdate(string newText)


    Column {
        id: pageColumn
        anchors.fill: parent

        QtControls.Label {
          width: parent.width
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          wrapMode: Text.WordWrap
          text: i18n("Detailed information may be found on <a href=\"https://arcanis.me/projects/awesome-widgets/\">project homepage</a>")
          onLinkActivated: Qt.openUrlExternally(link)
        }

        HtmlDefaultFunctionsBar {
            textArea: textPattern
        }

        AWTagSelector {
            backend: awKeys
            notifyBackend: awActions
            textArea: textPattern
            groups: general.awTagRegexp
        }

        AWExtensions {
            backend: awKeys
            textArea: textPattern
            onUnlock: lock = false
        }

        QtControls.TextArea {
            id: textPattern
            width: parent.width
            height: parent.height * 4 / 5
            textFormat: TextEdit.PlainText
            text: plasmoid.configuration.text
        }
    }

    QtDialogs.MessageDialog {
        id: compiledText
        modality: Qt.NonModal
        title: i18n("Preview")
    }


    Component.onCompleted: {
        if (debug) console.debug()

        awKeys.needTextToBeUpdated.connect(needTextUpdate)
        // init submodule
        awKeys.initKeys(plasmoid.configuration.text, plasmoid.configuration.interval,
                        plasmoid.configuration.queueLimit, false)
        awKeys.setAggregatorProperty("acOffline", plasmoid.configuration.acOffline)
        awKeys.setAggregatorProperty("acOnline", plasmoid.configuration.acOnline)
        awKeys.setAggregatorProperty("customTime", plasmoid.configuration.customTime)
        awKeys.setAggregatorProperty("customUptime", plasmoid.configuration.customUptime)
        awKeys.setAggregatorProperty("tempUnits", plasmoid.configuration.tempUnits)
        awKeys.setAggregatorProperty("translate", plasmoid.configuration.translateStrings)
    }

    onNeedTextUpdate: {
        if (lock) return
        if (debug) console.debug()

        compiledText.text = newText.replace("&nbsp;", " ")
        compiledText.open()
        lock = true
    }
}
