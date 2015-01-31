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

import QtQuick 2.4
import QtQuick.Controls 1.3 as QtControls
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents


Item {
    id: main

    Loader { id: connector; source: "connector.qml" }
    property bool debug: connector.item.debug

    signal needUpdate

    // init
    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: "DefaultBackground"
    Plasmoid.toolTipMainText: "Awesome Widget"
    Plasmoid.associatedApplication: "ksysguard"

    PlasmaCore.DataSource {
        id: systemmonitorDE
        engine: "systemmonitor"
        connectedSources: systemmonitorDE.sources
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            if (!connector.item.checkKeys(data)) return
            if (connector.item.isReady()) connector.item.setDataBySource(sourceName, data)
        }

        onSourceAdded: {
            if (debug) console.log("[main::onSourceAdded] : Source " + source)

            connector.item.addDevice(source)
        }
    }

    PlasmaCore.DataSource {
        id: extsysmonDE
        engine: "extsysmon"
        connectedSources: extsysmonDE.sources
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            if (!connector.item.checkKeys(data)) return
            if (connector.item.isReady()) connector.item.setDataBySource(sourceName, data)
            // update
            if (sourceName == "update") needUpdate()
        }
    }

    PlasmaCore.DataSource {
        id: timeDE
        engine: "time"
        connectedSources: ["Local"]
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            if (!connector.item.checkKeys(data)) return
            if (connector.item.isReady()) connector.item.setDataBySource(sourceName, data)
        }
    }

    // ui
    Grid {
        id: mainGrid
        columns: 1

        Text {
            id: text
            color: plasmoid.configuration.fontColor
            font.family: plasmoid.configuration.fontFamily
            font.italic: plasmoid.configuration.fontStyle == "italic" ? true : false
            font.pointSize: plasmoid.configuration.fontSize
            font.weight: connector.item.fontWeight[plasmoid.configuration.fontWeight]
            horizontalAlignment: connector.item.align[plasmoid.configuration.textAlign]
            textFormat: Text.RichText
            text: plasmoid.configuration.text
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")

        // actions
        plasmoid.setAction("showReadme", i18n("Show README"), "text-x-readme")
        plasmoid.setAction("updateText", i18n("Update text"), "stock-refresh")
        plasmoid.setAction("checkUpdates", i18n("Check updates"), "system-software-update")
    }

    onNeedUpdate: {
        if (debug) console.log("[main::onNeedUpdate]")

        text.text = connector.item.parsePattern()
        // update geometry
        text.update()
        height = text.contentHeight
        width = text.contentWidth
        update()
    }

    function action_checkUpdates() {
        if (debug) console.log("[main::action_checkUpdates]")

        connector.item.checkUpdates()
    }

    function action_showReadme() {
        if (debug) console.log("[main::action_showReadme]")

        connector.item.showReadme()
    }

    function action_updateText() {
        if (debug) console.log("[main::action_updateText]")
    }
}
