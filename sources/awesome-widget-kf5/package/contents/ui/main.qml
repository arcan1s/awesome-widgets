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
    property variant settings: {
        "customTime": plasmoid.configuration.customTime,
        "customUptime": plasmoid.configuration.customUptime,
        "tempUnits": plasmoid.configuration.tempUnits,
        "acOnline": plasmoid.configuration.acOnline,
        "acOffline": plasmoid.configuration.acOffline
    }
    property variant tooltipSettings: {
        "tooltipNumber": plasmoid.configuration.tooltipNumber,
        "useTooltipBackground": plasmoid.configuration.useTooltipBackground,
        "tooltipBackgroung": plasmoid.configuration.tooltipBackgroung,
        "cpuTooltip": plasmoid.configuration.cpuTooltip,
        "cpuclTooltip": plasmoid.configuration.cpuclTooltip,
        "memTooltip": plasmoid.configuration.memTooltip,
        "swapTooltip": plasmoid.configuration.swapTooltip,
        "downTooltip": plasmoid.configuration.downTooltip,
        "upTooltip": plasmoid.configuration.downTooltip,
        "batteryTooltip": plasmoid.configuration.batteryTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downTooltipColor": plasmoid.configuration.downTooltipColor,
        "upTooltipColor": plasmoid.configuration.upTooltipColor,
        "batteryTooltipColor": plasmoid.configuration.batteryTooltipColor,
        "batteryInTooltipColor": plasmoid.configuration.batteryInTooltipColor
    }

    signal needUpdate


    // init
    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"
    Plasmoid.associatedApplication: "ksysguard"

    PlasmaCore.DataSource {
        id: systemmonitorDE
        engine: "systemmonitor"
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            AWKeys.setDataBySource(sourceName, data, settings)
        }

        onSourceAdded: {
            if (debug) console.log("[main::onSourceAdded] : Source " + source)

            if (AWKeys.addDevice(source)) systemmonitorDE.connectSource(source)
        }
    }

    PlasmaCore.DataSource {
        id: extsysmonDE
        engine: "extsysmon"
        connectedSources: ["battery", "custom", "desktop", "netdev", "gpu",
                           "gputemp", "hddtemp", "pkg", "player", "ps", "update"]
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            if (AWKeys.setDataBySource(sourceName, data, settings)) needUpdate()
        }
    }

    PlasmaCore.DataSource {
        id: timeDE
        engine: "time"
        connectedSources: ["Local"]
        interval: plasmoid.configuration.interval

        onNewData: {
            if (debug) console.log("[main::onNewData] : Update source " + sourceName)

            AWKeys.setDataBySource(sourceName, data, settings)
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

            PlasmaCore.ToolTipArea {
                id: tooltip
                height: 100
                width: 500
                mainText: "Awesome Widget"
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")

        // init submodule
        AWKeys.initKeys(plasmoid.configuration.text, tooltipSettings)
        // actions
        plasmoid.setAction("requestKey", i18n("Request key"), "utilities-system-monitor")
        plasmoid.setAction("showReadme", i18n("Show README"), "text-x-readme")
        plasmoid.setAction("checkUpdates", i18n("Check updates"), "system-software-update")
    }

    onNeedUpdate: {
        if (debug) console.log("[main::onNeedUpdate]")

        text.text = AWKeys.parsePattern(plasmoid.configuration.text)
        // FIXME change to pixmap/image/etc
        tooltip.mainText = AWKeys.toolTipImage()
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

    function action_requestKey() {
        if (debug) console.log("[main::action_requestKey]")

        var message = AWKeys.graphicalValueByKey();
        if (message.length == 0) return
        AWActions.sendNotification("tag", message)
    }
}
