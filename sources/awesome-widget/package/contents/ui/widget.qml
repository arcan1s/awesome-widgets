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
import org.kde.kcmutils as KCM

import org.kde.plasma.awesomewidgets
import org.kde.plasma.private.awesomewidget


KCM.SimpleKCM {
    id: widgetPage

    // backend
    AWKeys {
        id: awKeys
    }
    AWActions {
        id: awActions
    }

    property alias cfg_text: textPattern.text
    property bool lock: true

    signal needTextUpdate(string newText)

    Column {
        anchors.fill: parent

        AWInfoLabel {}

        HtmlDefaultFunctionsBar {
            textArea: textPattern
        }

        AWTagSelector {
            backend: awKeys
            notifyBackend: awActions
            textArea: textPattern
            groups: General.awTagRegexp
        }

        AWExtensions {
            id: extensions
            backend: awKeys
            textArea: textPattern
            onUnlock: lock = false
        }

        AWTextEditor {
            id: textPattern
            backend: awKeys
        }
    }

    Component.onCompleted: {
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

    onNeedTextUpdate: newText => {
        if (lock) return

        extensions.showMessage(newText)
        lock = true
    }
}
