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
    AWFormatterConfigFactory {
        id: awFormatter
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

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Button {
                width: parent.width * 3 / 10
                text: i18n("Edit bars")
                onClicked: awKeys.editItem("graphicalitem")
            }
            QtControls.Button {
                width: parent.width * 3 / 10
                text: i18n("Formatters")
                onClicked: awFormatter.showDialog(awKeys.dictKeys(true))
            }
            QtControls.Button {
                width: parent.width * 5 / 15
                text: i18n("Preview")
                onClicked: {
                    lock = false
                    awKeys.initKeys(textPattern.text, plasmoid.configuration.interval,
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
                            onTriggered: textPattern.text = modelData
                        }
                        onObjectAdded: historyConfig.insertItem(index, object)
                        onObjectRemoved: historyConfig.removeItem(object)
                    }
                }
            }
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

        compiledText.text = newText.replace(/&nbsp;/g, " ")
        compiledText.open()
        lock = true
    }
}
