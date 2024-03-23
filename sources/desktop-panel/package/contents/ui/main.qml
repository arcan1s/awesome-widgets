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
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.desktoppanel 1.0
import "."


PlasmoidItem {
    id: main
    // backend
    DPAdds {
        id: dpAdds
    }

    property bool debug: dpAdds.isDebugEnabled()
    property variant tooltipSettings: {
        "tooltipColor": plasmoid.configuration.tooltipColor,
        "tooltipType": plasmoid.configuration.tooltipType,
        "tooltipWidth": plasmoid.configuration.tooltipWidth
    }

    signal needUpdate
    signal needTooltipUpdate
    signal sizeUpdate


    // init
    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"


    // ui
    GridLayout {
        anchors.fill: parent
        columns: plasmoid.configuration.verticalLayout ? 1 : dpAdds.numberOfDesktops()
        rows: plasmoid.configuration.verticalLayout ? dpAdds.numberOfDesktops() : 1

        Repeater {
            id: repeater
            Layout.columnSpan: 0
            Layout.rowSpan: 0
            model: dpAdds.numberOfDesktops()

            Text {
                id: text
                height: contentHeight
                width: contentWidth
                renderType: Text.NativeRendering
                textFormat: Text.RichText
                wrapMode: Text.NoWrap

                verticalAlignment: Text.AlignVCenter

                text: dpAdds.parsePattern(plasmoid.configuration.text, index)
                property alias tooltip: tooltip

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: dpAdds.setCurrentDesktop(index)
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
        if (debug) console.debug()

        for (var i=0; i<repeater.count; i++) {
            if (!repeater.itemAt(i)) {
                if (debug) console.info("Nothing to do here yet", i)
                timer.start()
                return
            }
            repeater.itemAt(i).text = dpAdds.parsePattern(plasmoid.configuration.text, i)
            if (dpAdds.currentDesktop() == i) {
                repeater.itemAt(i).color = plasmoid.configuration.currentFontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.currentFontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.currentFontStyle == "italic"
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.currentFontSize
                repeater.itemAt(i).font.weight = General.fontWeight[plasmoid.configuration.currentFontWeight]
                repeater.itemAt(i).style = General.textStyle[plasmoid.configuration.currentTextStyle]
                repeater.itemAt(i).styleColor = plasmoid.configuration.currentTextStyleColor
            } else {
                repeater.itemAt(i).color = plasmoid.configuration.fontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.fontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.fontStyle == "italic"
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.fontSize
                repeater.itemAt(i).font.weight = General.fontWeight[plasmoid.configuration.fontWeight]
                repeater.itemAt(i).style = General.textStyle[plasmoid.configuration.textStyle]
                repeater.itemAt(i).styleColor = plasmoid.configuration.textStyleColor
            }
            repeater.itemAt(i).update()
        }

        sizeUpdate()
        needTooltipUpdate()
    }

    onNeedTooltipUpdate: {
        if (debug) console.debug()

        for (var i=0; i<repeater.count; i++) {
            repeater.itemAt(i).tooltip.text = dpAdds.toolTipImage(i)
            // resize text tooltip to content size
            // this hack does not work for images-based tooltips
            if (tooltipSettings.tooltipType == "names") {
                repeater.itemAt(i).tooltip.height = repeater.itemAt(i).tooltip.implicitHeight
                repeater.itemAt(i).tooltip.width = repeater.itemAt(i).tooltip.implicitWidth
            }
        }
    }

    onSizeUpdate: {
        if (debug) console.debug()

        if (plasmoid.configuration.height == 0) {
            var newHeight = 0
            for (var i=0; i<repeater.count; i++)
                newHeight += repeater.itemAt(i).contentHeight
            Layout.minimumHeight = newHeight
            Layout.maximumHeight = -1
        } else {
            Layout.minimumHeight = plasmoid.configuration.height
            Layout.maximumHeight = plasmoid.configuration.height
        }
        if (plasmoid.configuration.width == 0) {
            var newWidth = 0
            for (var i=0; i<repeater.count; i++)
                newWidth += repeater.itemAt(i).contentWidth
            Layout.minimumWidth = newWidth
            Layout.maximumWidth = -1
        } else {
            Layout.minimumWidth = plasmoid.configuration.width
            Layout.maximumWidth = plasmoid.configuration.width
        }
    }

    Plasmoid.onUserConfiguringChanged: {
        if (plasmoid.userConfiguring) return
        if (debug) console.debug()

        dpAdds.setMark(plasmoid.configuration.mark)
        dpAdds.setToolTipData(tooltipSettings)

        needUpdate()
    }

    Component.onCompleted: {
        if (debug) console.debug()

        // init submodule
        Plasmoid.userConfiguringChanged(false)
        dpAdds.desktopChanged.connect(needUpdate)
        dpAdds.windowListChanged.connect(needTooltipUpdate)
    }
}
