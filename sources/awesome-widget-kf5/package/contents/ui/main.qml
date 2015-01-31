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

import org.kde.plasma.private.awesomewidget 1.0
import "."


Item {
    id: main

    property bool debug: AWKeys.isDebugEnabled()

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

            if (!AWActions.checkKeys(data)) return
            if (AWKeys.isReady()) AWKeys.setDataBySource(sourceName, data, general.settings)
        }

        onSourceAdded: {
            if (debug) console.log("[main::onSourceAdded] : Source " + source)

            AWActions.addDevice(source)
        }
    }

    PlasmaCore.DataSource {
        id: extsysmonDE
        engine: "extsysmon"
        connectedSources: extsysmonDE.sources
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            if (!AWActions.checkKeys(data)) return
            if (AWKeys.isReady()) AWKeys.setDataBySource(sourceName, data, general.settings)
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

            if (!AWActions.checkKeys(data)) return
            if (AWKeys.isReady()) AWKeys.setDataBySource(sourceName, data, general.settings)
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
            font.weight: general.fontWeight[plasmoid.configuration.fontWeight]
            horizontalAlignment: general.align[plasmoid.configuration.textAlign]
            textFormat: Text.RichText
            text: plasmoid.configuration.text
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")

        // init submodule
        AWKeys.initKeys(plasmoid.configuration.text, general.settings, general.tooltipSettings)
        // actions
        plasmoid.setAction("showReadme", i18n("Show README"), "text-x-readme")
        plasmoid.setAction("updateText", i18n("Update text"), "stock-refresh")
        plasmoid.setAction("checkUpdates", i18n("Check updates"), "system-software-update")
    }

    onNeedUpdate: {
        if (debug) console.log("[main::onNeedUpdate]")

        text.text = AWKeys.parsePattern(plasmoid.configuration.text)
        // update geometry
        text.update()
        height = text.contentHeight
        width = text.contentWidth
        update()
    }

    function action_checkUpdates() {
        if (debug) console.log("[main::action_checkUpdates]")

        AWActions.checkUpdates()
    }

    function action_showReadme() {
        if (debug) console.log("[main::action_showReadme]")

        AWActions.showReadme()
    }

    function action_updateText() {
        if (debug) console.log("[main::action_updateText]")
    }
}
