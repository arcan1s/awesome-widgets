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

import org.kde.plasma.private.awesomewidget 1.0


Item {
    AWConfigHelper {
        id: awConfig
    }

    signal configurationReceived(var configuration)

    FileDialog {
        id: fileDialog
        title: i18n("Import")
        currentFolder: awConfig.configurationDirectory()
        onAccepted: importSelection.open()
    }

    Dialog {
        id: importSelection

        Column {
            CheckBox {
                id: importPlasmoid
                text: i18n("Import plasmoid settings")
            }

            CheckBox {
                id: importExtensions
                text: i18n("Import extensions")
            }

            CheckBox {
                id: importAdds
                text: i18n("Import additional files")
            }
        }

        onAccepted: {
            var importConfig = awConfig.importConfiguration(
                fileDialog.fileUrl.toString().replace("file://", ""),
                importPlasmoid.checked, importExtensions.checked,
                importAdds.checked)
            configurationReceived(importConfig)
        }
    }

    function open() {
        return fileDialog.open()
    }
}
