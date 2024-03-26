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
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.awesomewidget 1.0
import "."


PlasmoidItem {
    id: main

    // backend
    AWKeys {
        id: awKeys
    }
    AWActions {
        id: awActions
    }
    AWTelemetryHandler {
        id: awTelemetryHandler
    }
    BugReport {
        id: bugReport
    }

    property variant tooltipSettings: {
        "tooltipNumber": plasmoid.configuration.tooltipNumber,
        "useTooltipBackground": plasmoid.configuration.useTooltipBackground,
        "tooltipBackground": plasmoid.configuration.tooltipBackground,
        "cpuTooltip": plasmoid.configuration.cpuTooltip,
        "cpuclTooltip": plasmoid.configuration.cpuclTooltip,
        "memTooltip": plasmoid.configuration.memTooltip,
        "swapTooltip": plasmoid.configuration.swapTooltip,
        "downkbTooltip": plasmoid.configuration.downkbTooltip,
        "upkbTooltip": plasmoid.configuration.downkbTooltip,
        "batTooltip": plasmoid.configuration.batTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downkbTooltipColor": plasmoid.configuration.downkbTooltipColor,
        "upkbTooltipColor": plasmoid.configuration.upkbTooltipColor,
        "batTooltipColor": plasmoid.configuration.batTooltipColor,
        "batInTooltipColor": plasmoid.configuration.batInTooltipColor,
        // additional field to parse AC status
        "acOnline": plasmoid.configuration.acOnline,
        // additional field to send notifications
        "notify": plasmoid.configuration.notify
    }

    signal needTextUpdate(string newText)
    signal needToolTipUpdate(string newText)
    signal sizeUpdate

    Layout.fillWidth: PlasmoidItem.formFactor !== PlasmaCore.Planar
    Layout.fillHeight: PlasmoidItem.formFactor !== PlasmaCore.Planar
    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"

    // ui
    Text {
        id: text
        anchors.fill: parent
        renderType: Text.NativeRendering
        textFormat: Text.RichText
        wrapMode: plasmoid.configuration.wrapText ? Text.WordWrap : Text.NoWrap

        horizontalAlignment: General.align[plasmoid.configuration.textAlign]
        verticalAlignment: Text.AlignVCenter

        color: plasmoid.configuration.fontColor
        font.family: plasmoid.configuration.fontFamily
        font.italic: plasmoid.configuration.fontStyle === "italic" ? true : false
        font.pointSize: plasmoid.configuration.fontSize
        font.weight: General.fontWeight[plasmoid.configuration.fontWeight]

        style: General.textStyle[plasmoid.configuration.textStyle]
        styleColor: plasmoid.configuration.textStyleColor

        PlasmaCore.ToolTipArea {
            height: tooltip.height
            width: tooltip.width
            mainItem: Text {
                id: tooltip
                height: contentHeight
                width: contentWidth
                textFormat: Text.RichText
            }
        }
    }

    Dialog {
        id: tagSelector
        title: i18n("Select tag")

        ComboBox {
            id: tagSelectorBox
            width: parent.width
            editable: true
        }

        onAccepted: {
            const tag = tagSelectorBox.editText
            let message = i18n("Tag: %1", tag)
            message += "<br>"
            message += i18n("Value: %1", awKeys.valueByKey(tag))
            message += "<br>"
            message += i18n("Info: %1", awKeys.infoByKey(tag))
            awActions.sendNotification("tag", message)
        }
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18n("Request key")
            icon.name: "utilities-system-monitor"
            onTriggered: {
                tagSelectorBox.model = awKeys.dictKeys(true)
                tagSelector.open()
            }
        },
        PlasmaCore.Action {
            text: i18n("Show README")
            icon.name: "text-x-readme"
            onTriggered: awActions.showReadme()
        },
        PlasmaCore.Action {
            text: i18n("Check updates")
            icon.name: "system-software-update"
            onTriggered: awActions.checkUpdates(true)
        },
        PlasmaCore.Action {
            text: i18n("Report bug")
            icon.name: "tools-report-bug"
            onTriggered: {
                bugReport.reset()
                bugReport.open()
            }
        }
    ]


    Component.onCompleted: {
        // init submodule
        Plasmoid.userConfiguringChanged(false)
        // connect data
        awKeys.needTextToBeUpdated.connect(needTextUpdate)
        awKeys.needToolTipToBeUpdated.connect(needToolTipUpdate)
        // check updates if required
        if (plasmoid.configuration.checkUpdates) return awActions.checkUpdates(false)
    }

    onNeedTextUpdate: newText => {
        text.text = newText
        sizeUpdate()
    }

    onNeedToolTipUpdate: newText => {
        tooltip.text = newText
    }

    onSizeUpdate: {
        // 16 is a magic number
        // in other case plasmoid will increase own size on each update
        if (plasmoid.configuration.height === 0) {
            Layout.minimumHeight = text.contentHeight - 16
            Layout.maximumHeight = -1
        } else {
            Layout.minimumHeight = plasmoid.configuration.height
            Layout.maximumHeight = plasmoid.configuration.height
        }
        if (plasmoid.configuration.width === 0) {
            Layout.minimumWidth = text.contentWidth - 16
            Layout.maximumWidth = -1
        } else {
            Layout.minimumWidth = plasmoid.configuration.width
            Layout.maximumWidth = plasmoid.configuration.width
        }
    }

    Plasmoid.onUserConfiguringChanged: {
        if (plasmoid.userConfiguring) return

        // init submodule
        awKeys.initDataAggregator(tooltipSettings)
        awKeys.initKeys(plasmoid.configuration.text, plasmoid.configuration.interval,
                        plasmoid.configuration.queueLimit, plasmoid.configuration.optimize)
        awKeys.setWrapNewLines(plasmoid.configuration.wrapNewLines)
        // configure aggregator
        awKeys.setAggregatorProperty("acOffline", plasmoid.configuration.acOffline)
        awKeys.setAggregatorProperty("acOnline", plasmoid.configuration.acOnline)
        awKeys.setAggregatorProperty("customTime", plasmoid.configuration.customTime)
        awKeys.setAggregatorProperty("customUptime", plasmoid.configuration.customUptime)
        awKeys.setAggregatorProperty("tempUnits", plasmoid.configuration.tempUnits)
        awKeys.setAggregatorProperty("translate", plasmoid.configuration.translateStrings)
        // update telemetry ID
        if (plasmoid.configuration.telemetryId.length === 0)
            plasmoid.configuration.telemetryId = generateUuid()
        // save telemetry
        awTelemetryHandler.init(plasmoid.configuration.telemetryCount,
                                plasmoid.configuration.telemetryRemote,
                                plasmoid.configuration.telemetryId)
        if (awTelemetryHandler.put("awwidgetconfig", plasmoid.configuration.text))
            awTelemetryHandler.uploadTelemetry("awwidgetconfig", plasmoid.configuration.text)
    }

    // code from http://stackoverflow.com/questions/105034/create-guid-uuid-in-javascript
    function generateUuid() {
        return "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g, function(c) {
            let r = Math.random() * 16 | 0, v = c === "x" ? r : (r & 0x3 | 0x8);
            return v.toString(16);
        });
    }
}
