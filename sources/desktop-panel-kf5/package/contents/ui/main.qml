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
    property variant settings: {
    }


    // init
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    Plasmoid.compactRepresentation: Plasmoid.fullRepresentation

    Layout.fillWidth: plasmoid.formFactor != PlasmaCore.Planar
    Layout.fillHeight: plasmoid.formFactor != PlasmaCore.Planar
    Layout.minimumHeight: text.height
    Layout.minimumWidth: text.width

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"


    // ui
    Grid {
        columns: 1

        Text {
            id: text
            height: contentHeight
            width: contentWidth
            textFormat: Text.RichText
            wrapMode: Text.NoWrap

            horizontalAlignment: general.align[plasmoid.configuration.textAlign]
            verticalAlignment: Text.AlignVCenter

            color: plasmoid.configuration.fontColor
            font.family: plasmoid.configuration.fontFamily
            font.italic: plasmoid.configuration.fontStyle == "italic" ? true : false
            font.pointSize: plasmoid.configuration.fontSize
            font.weight: general.fontWeight[plasmoid.configuration.fontWeight]

            text: plasmoid.configuration.text

            PlasmaCore.ToolTipArea {
                id: rootTooltip
                mainItem: Text {
                id: tooltip
                textFormat: Text.RichText
                }
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")
    }
}
