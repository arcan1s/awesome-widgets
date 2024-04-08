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
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kcmutils as KCM

import org.kde.plasma.awesomewidgets
import org.kde.plasma.private.awesomewidget


KCM.SimpleKCM {
    id: bugPage

    AWActions {
        id: awActions
    }
    AWBugReporter {
        id: awBugReporter
    }

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: title
            Layout.fillWidth: true
            placeholderText: i18n("Report subject")
        }

        ColumnLayout {
            Layout.fillWidth: true

            GroupBox {
                Layout.fillWidth: true
                title: i18n("Description")

                TextArea {
                    id: description
                    anchors.fill: parent
                    textFormat: TextEdit.PlainText
                }
            }

            GroupBox {
                Layout.fillWidth: true
                title: i18n("Steps to reproduce")

                TextArea {
                    id: reproduce
                    anchors.fill: parent
                    textFormat: TextEdit.PlainText
                }
            }

            GroupBox {
                Layout.fillWidth: true
                title: i18n("Expected result")

                TextArea {
                    id: expected
                    anchors.fill: parent
                    textFormat: TextEdit.PlainText
                }
            }

            GroupBox {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom
                title: i18n("Logs")

                ColumnLayout {
                    anchors.fill: parent
                    Layout.fillWidth: true

                    Row {
                        Layout.fillWidth: true

                        Label {
                            width: parent.width * 2 / 5
                            horizontalAlignment: Text.AlignJustify
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.WordWrap
                            text: i18n("Use command")
                        }

                        TextField {
                            width: parent.width * 3 / 5
                            readOnly: true
                            text: "QT_LOGGING_RULES=*=true plasmawindowed org.kde.plasma.awesomewidget"
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: i18n("Load log file")
                        onClicked: logPath.open()
                    }

                    TextArea {
                        id: logBody
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignBottom
                        textFormat: TextEdit.PlainText
                    }
                }

                FileDialog {
                    id: logPath
                    title: i18n("Open log file")
                    onAccepted: logBody.text = awActions.getFileContent(logPath.selectedFile.toString().replace("file://", ""))
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true

                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Reset
                alignment: Qt.AlignRight
                onAccepted: sendBugReport()
                onReset: resetDialog()
            }
        }
    }

    function sendBugReport() {
        const text = awBugReporter.generateText(description.text, reproduce.text, expected.text, logBody.text)
        awBugReporter.sendBugReport(title.text, text)
    }

    function resetDialog() {
        title.text = ""
        description.text = ""
        reproduce.text = ""
        expected.text = ""
        logBody.text = ""
    }

    Component.onCompleted: {
        resetDialog()
        awBugReporter.doConnect()
    }
}
