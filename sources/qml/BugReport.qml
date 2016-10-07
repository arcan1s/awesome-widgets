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
import QtQuick.Controls 1.3 as QtControls
import QtQuick.Dialogs 1.2 as QtDialogs

import org.kde.plasma.private.awesomewidget 1.0


QtDialogs.Dialog {
    id: reportDialog
    AWActions {
        id: awActions
    }
    AWBugReporter {
        id: awBugReporter
    }

    width: 480
    height: 640
    property bool debug: awActions.isDebugEnabled()


    title: i18n("Report a bug")
    standardButtons: QtDialogs.StandardButton.Ok | QtDialogs.StandardButton.Cancel | QtDialogs.StandardButton.Reset

    QtControls.TextField {
        id: title
        width: parent.width
        placeholderText: i18n("Report subject")
    }
    Column {
        id: body
        width: parent.width
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
 
        QtControls.GroupBox {
            width: parent.width
            height: parent.height / 5
            title: i18n("Description")
            QtControls.TextArea {
                id: description
                width: parent.width
                height: parent.height
                textFormat: TextEdit.PlainText
            }
        }
        QtControls.GroupBox {
            width: parent.width
            height: parent.height / 5
            title: i18n("Steps to reproduce")
            QtControls.TextArea {
                id: reproduce
                width: parent.width
                height: parent.height
                textFormat: TextEdit.PlainText
            }
        }
        QtControls.GroupBox {
            width: parent.width
            height: parent.height / 5
            title: i18n("Expected result")
            QtControls.TextArea {
                id: expected
                width: parent.width
                height: parent.height
                textFormat: TextEdit.PlainText
            }
        }
        QtControls.GroupBox {
            width: parent.width
            height: parent.height * 2 / 5
            title: i18n("Logs")
            Row {
                id: debugCmdLabel
                width: parent.width
                QtControls.Label {
                    width: parent.width * 2 / 5
                    horizontalAlignment: Text.AlignJustify
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    text: i18n("Use command")
                }
                QtControls.TextField {
                    id: customTime
                    width: parent.width * 3 / 5
                    readOnly: true
                    text: "QT_LOGGING_RULES=*=true plasmawindowed org.kde.plasma.awesomewidget"
                }
            }
            QtControls.Button {
                id: logButton
                anchors.top: debugCmdLabel.bottom
                width: parent.width
                text: i18n("Load log file")
                onClicked: logPath.open()
            }
            QtControls.TextArea {
                anchors.top: logButton.bottom
                anchors.bottom: parent.bottom
                id: logBody
                width: parent.width
                textFormat: TextEdit.PlainText
            }

            QtDialogs.FileDialog {
                id: logPath
                title: i18n("Open log file")
                onAccepted: 
                    logBody.text = awActions.getFileContent(logPath.fileUrl.toString().replace("file://", ""))
            }
        }
    }

    onAccepted: {
        if (debug) console.debug()

        var text = awBugReporter.generateText(description.text, reproduce.text,
                                              expected.text, logBody.text)
        awBugReporter.sendBugReport(title.text, text)
    }

    onReset: {
        if (debug) console.debug()

        title.text = ""
        description.text = ""
        reproduce.text = ""
        expected.text = ""
    }

    Component.onCompleted: {
        if (debug) console.debug()

        awBugReporter.doConnect()
    }
}

