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

    width: 640
    height: 480
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
            height: parent.height / 3
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
            height: parent.height / 3
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
            height: parent.height / 3
            title: i18n("Expected result")
            QtControls.TextArea {
                id: expected
                width: parent.width
                height: parent.height
                textFormat: TextEdit.PlainText
            }
        }
    }

    onAccepted: {
        if (debug) console.debug()

        var text = awBugReporter.generateText(description.text, reproduce.text,
                                              expected.text)
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
    }
}

