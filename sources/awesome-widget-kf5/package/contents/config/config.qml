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

import QtQuick 2.0

import org.kde.plasma.configuration 2.0


ConfigModel {
    ConfigCategory {
        name: i18n("Widget")
        icon: "utilities-system-monitor"
        source: "widget.qml"
    }

    ConfigCategory {
        name: i18n("Advanced")
        icon: "system-run"
        source: "advanced.qml"
    }

    ConfigCategory {
        name: i18n("Tooltip")
        icon: "preferences-desktop-color"
        source: "tooltip.qml"
    }

    ConfigCategory {
        name: i18n("Appearance")
        icon: "preferences-desktop-theme"
        source: "appearance.qml"
    }

    ConfigCategory {
        name: i18n("DataEngine")
        icon: "utilities-system-monitor"
        source: "dataengine.qml"
    }

    ConfigCategory {
        name: i18n("About")
        icon: "help-about"
        source: "about.qml"
    }
}
