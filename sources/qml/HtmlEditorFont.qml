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

import QtQuick 2.2

import org.kde.plasma.private.awesomewidget 1.0


HtmlEditorButton {
    end: "</span>"
    iconName: "font"
    text: i18n("Font")

    // backend
    AWActions {
        id: awActions
    }
    // default font properties
    property string defaultFontColor
    property string defaultFontFamily
    property int defaultFontSize

    clickedEvent: function() {
        // get new font
        var defaultFont = {
            "color": defaultFontColor,
            "family": defaultFontFamily,
            "size": defaultFontSize
        }
        // we are using custom selector as soon as we need to select color as well
        var font = awActions.getFont(defaultFont)

        // check status
        if (!font.applied)
            return

        // apply font
        start = "<span style=\"color:" + font.color +
            "; font-family:'" + font.family +
            "'; font-size:" + font.size + "pt;\">"
        updateText()
    }
}
