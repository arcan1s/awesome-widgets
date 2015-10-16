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
import QtQuick.Dialogs 1.2 as QtDialogs
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.awesomewidget 1.0
import "."


Item {
    id: main
    // backend
    AWKeys {
        id: awKeys
    }
    AWActions {
        id: awActions
    }

    property bool debug: awActions.isDebugEnabled()
    property variant tooltipSettings: {
        "tooltipNumber": plasmoid.configuration.tooltipNumber,
        "useTooltipBackground": plasmoid.configuration.useTooltipBackground,
        "tooltipBackground": plasmoid.configuration.tooltipBackground,
        "cpuTooltip": plasmoid.configuration.cpuTooltip,
        "cpuclTooltip": plasmoid.configuration.cpuclTooltip,
        "memTooltip": plasmoid.configuration.memTooltip,
        "swapTooltip": plasmoid.configuration.swapTooltip,
        "downTooltip": plasmoid.configuration.downTooltip,
        "upTooltip": plasmoid.configuration.downTooltip,
        "batTooltip": plasmoid.configuration.batTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downTooltipColor": plasmoid.configuration.downTooltipColor,
        "upTooltipColor": plasmoid.configuration.upTooltipColor,
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


    // init
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    Plasmoid.compactRepresentation: Plasmoid.fullRepresentation

    Layout.fillWidth: plasmoid.formFactor != PlasmaCore.Planar
    Layout.fillHeight: plasmoid.formFactor != PlasmaCore.Planar
    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"
    Plasmoid.associatedApplication: "ksysguard"


    // ui
    Text {
        id: text
        anchors.fill: parent
        renderType: Text.NativeRendering
        textFormat: Text.RichText
        wrapMode: plasmoid.configuration.wrapText ? Text.WordWrap : Text.NoWrap

        horizontalAlignment: general.align[plasmoid.configuration.textAlign]
        verticalAlignment: Text.AlignVCenter

        color: plasmoid.configuration.fontColor
        font.family: plasmoid.configuration.fontFamily
        font.italic: plasmoid.configuration.fontStyle == "italic" ? true : false
        font.pointSize: plasmoid.configuration.fontSize
        font.weight: general.fontWeight[plasmoid.configuration.fontWeight]

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

    QtDialogs.Dialog {
        id: tagSelector
        title: i18n("Select tag")

        QtControls.ComboBox {
            id: tagSelectorBox
            width: parent.width
            editable: true
        }

        onAccepted: {
            var tag = tagSelectorBox.editText
            var message = i18n("Tag: %1", tag)
            message += "<br>"
            message += i18n("Value: %1", awKeys.valueByKey(tag))
            message += "<br>"
            message += i18n("Info: %1", awKeys.infoByKey(tag))
            awActions.sendNotification("tag", message)
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()

        // actions
        plasmoid.setAction("requestKey", i18n("Request key"), "utilities-system-monitor")
        plasmoid.setAction("showReadme", i18n("Show README"), "text-x-readme")
        plasmoid.setAction("checkUpdates", i18n("Check updates"), "system-software-update")
        // init submodule
        Plasmoid.userConfiguringChanged(false)
        // connect data
        awKeys.needTextToBeUpdated.connect(needTextUpdate)
        awKeys.needToolTipToBeUpdated.connect(needToolTipUpdate)
        // check updates if required
        if (plasmoid.configuration.checkUpdates) return awActions.checkUpdates(false)
    }

    onNeedTextUpdate: {
        if (debug) console.debug()

        text.text = newText
        sizeUpdate()
    }

    onNeedToolTipUpdate: {
        if (debug) console.debug()

        tooltip.text = newText
    }

    onSizeUpdate: {
        if (debug) console.debug()
        // 16 is a magic number
        // in other case plasmoid will increase own size on each update

        if (plasmoid.configuration.height == 0) {
            Layout.minimumHeight = text.contentHeight - 16
            Layout.maximumHeight = -1
        } else {
            Layout.minimumHeight = plasmoid.configuration.height
            Layout.maximumHeight = plasmoid.configuration.height
        }
        if (plasmoid.configuration.width == 0) {
            Layout.minimumWidth = text.contentWidth - 16
            Layout.maximumWidth = -1
        } else {
            Layout.minimumWidth = plasmoid.configuration.width
            Layout.maximumWidth = plasmoid.configuration.width
        }
    }

    Plasmoid.onUserConfiguringChanged: {
        if (plasmoid.userConfiguring) return
        if (debug) console.debug()

        // init submodule
        awKeys.initKeys(plasmoid.configuration.text, plasmoid.configuration.interval,
                        plasmoid.configuration.queueLimit)
        awKeys.initDataAggregator(tooltipSettings)
        awKeys.setWrapNewLines(plasmoid.configuration.wrapNewLines)
        // configure aggregator
        awKeys.setAggregatorProperty("acOffline", plasmoid.configuration.acOffline)
        awKeys.setAggregatorProperty("acOnline", plasmoid.configuration.acOnline)
        awKeys.setAggregatorProperty("customTime", plasmoid.configuration.customTime)
        awKeys.setAggregatorProperty("customUptime", plasmoid.configuration.customUptime)
        awKeys.setAggregatorProperty("tempUnits", plasmoid.configuration.tempUnits)
        awKeys.setAggregatorProperty("translate", plasmoid.configuration.translateStrings)
    }


    function action_checkUpdates() {
        if (debug) console.debug()

        return awActions.checkUpdates(true)
    }

    function action_showReadme() {
        if (debug) console.debug()

        return awActions.showReadme()
    }

    function action_requestKey() {
        if (debug) console.debug()

        tagSelectorBox.model = awKeys.dictKeys(true)
        return tagSelector.open()
    }
}
