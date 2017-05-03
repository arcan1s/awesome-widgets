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
import QtQuick.Dialogs 1.2 as QtDialogs

import org.kde.plasma.private.awesomewidget 1.0


Item {
    property var configuration

    AWConfigHelper {
        id: awConfig
    }

    QtDialogs.MessageDialog {
        id: messageDialog
        standardButtons: QtDialogs.StandardButton.Ok
    }

    QtDialogs.FileDialog {
        id: fileDialog
        selectExisting: false
        title: i18n("Export")
        folder: awConfig.configurationDirectory()
        onAccepted: {
            var status = awConfig.exportConfiguration(
                configuration,
                fileDialog.fileUrl.toString().replace("file://", ""))
            if (status) {
                messageDialog.title = i18n("Success")
                messageDialog.text = i18n("Please note that binary files were not copied")
            } else {
                messageDialog.title = i18n("Ooops...")
                messageDialog.text = i18n("Could not save configuration file")
            }
            messageDialog.open()
        }
    }

    function open() {
        return fileDialog.open()
    }
}
