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
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.desktoppanel 1.0
import "."


Item {
    id: main

    property bool debug: DPAdds.isDebugEnabled()
    property variant tooltipSettings: {
        "tooltipColor": plasmoid.configuration.tooltipColor,
        "tooltipType": plasmoid.configuration.tooltipType,
        "tooltipWidth": plasmoid.configuration.tooltipWidth
    }

    signal needUpdate
    signal needTooltipUpdate


    // init
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    Plasmoid.compactRepresentation: Plasmoid.fullRepresentation

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"


    // ui
    Grid {
        columns: plasmoid.configuration.verticalLayout ? 1 : DPAdds.numberOfDesktops()
        rows: plasmoid.configuration.verticalLayout ? DPAdds.numberOfDesktops() : 1

        Repeater {
            id: repeater
            height: implicitHeight
            width: implicitWidth
            model: DPAdds.numberOfDesktops()
            Text {
                id: text
                height: contentHeight
                width: contentWidth
                textFormat: Text.RichText
                wrapMode: Text.NoWrap

                verticalAlignment: Text.AlignVCenter

                text: DPAdds.parsePattern(plasmoid.configuration.text, index + 1)
                property alias tooltip: tooltip

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: DPAdds.setCurrentDesktop(index + 1);
                    onEntered: needTooltipUpdate()
                }

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
        }
    }

    Timer {
        id: timer
        interval: 1000
        onTriggered: needUpdate()
    }

    onNeedUpdate: {
        if (debug) console.log("[main::onNeedUpdate]")

        var newHeight = 0
        var newWidth = 0
        for (var i=0; i<repeater.count; i++) {
            if (!repeater.itemAt(i)) {
                if (debug) console.log("[main::onNeedUpdate] : Nothing to do here " + i)
                timer.start()
                return
            }
            repeater.itemAt(i).text = DPAdds.parsePattern(plasmoid.configuration.text, i + 1)
            if (DPAdds.currentDesktop() == i + 1) {
                repeater.itemAt(i).color = plasmoid.configuration.currentFontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.currentFontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.currentFontStyle == "italic" ? true : false
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.currentFontSize
                repeater.itemAt(i).font.weight = general.fontWeight[plasmoid.configuration.currentFontWeight]
            } else {
                repeater.itemAt(i).color = plasmoid.configuration.fontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.fontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.fontStyle == "italic" ? true : false
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.fontSize
                repeater.itemAt(i).font.weight = general.fontWeight[plasmoid.configuration.fontWeight]
            }
            repeater.itemAt(i).update()
            newHeight += repeater.itemAt(i).contentHeight
            newWidth += repeater.itemAt(i).contentWidth
        }
        Layout.minimumHeight = newHeight
        Layout.minimumWidth = newWidth

        needTooltipUpdate()
    }

    onNeedTooltipUpdate: {
        if (debug) console.log("[main::onNeedTooltipUpdate]")

        for (var i=0; i<repeater.count; i++) {
            repeater.itemAt(i).tooltip.text = DPAdds.toolTipImage(i + 1)
        }
    }

    Plasmoid.onActivated: {
        if (debug) console.log("[main::onActivated]")

//         DPAdds.changePanelsState()
    }

    Plasmoid.onUserConfiguringChanged: {
        if (debug) console.log("[main::onUserConfiguringChanged]")

        DPAdds.setMark(plasmoid.configuration.mark)
        DPAdds.setPanelsToControl(plasmoid.configuration.panels)
        DPAdds.setToolTipData(tooltipSettings)
        needUpdate()
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")

        // init submodule
        Plasmoid.userConfiguringChanged(true)
        DPAdds.desktopChanged.connect(needUpdate)
        DPAdds.windowListChanged.connect(needTooltipUpdate)
    }
}
