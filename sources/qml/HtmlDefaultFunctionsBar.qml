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


Row {
    height: implicitHeight
    width: parent.width

    property var textArea

    // selectors
    HtmlEditorColor {
        width: parent.width * 3 / 15
        text: i18n("Bgcolor")
        textField: textArea
    }

    HtmlEditorFont {
        width: parent.width * 3 / 15
        textField: textArea
        defaultFontColor: plasmoid.configuration.fontColor
        defaultFontFamily: plasmoid.configuration.fontFamily
        defaultFontSize: plasmoid.configuration.fontSize
    }

    // new line
    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-indent-more"
        textField: textArea
        end: "<br>\n"
    }

    // font properties
    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-text-bold"
        textField: textArea
        start: "<b>"
        end: "</b>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-text-italic"
        textField: textArea
        start: "<i>"
        end: "</i>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-text-underline"
        textField: textArea
        start: "<u>"
        end: "</u>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-text-strikethrough"
        textField: textArea
        start: "<s>"
        end: "</s>"
    }

    // indentation
    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-justify-left"
        textField: textArea
        start: "<p align=\"left\">"
        end: "</p>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-justify-center"
        textField: textArea
        start: "<p align=\"center\">"
        end: "</p>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-justify-right"
        textField: textArea
        start: "<p align=\"right\">"
        end: "</p>"
    }

    HtmlEditorButton {
        width: parent.width / 15
        icon.name: "format-justify-fill"
        textField: textArea
        start: "<p align=\"justify\">"
        end: "</p>"
    }
}
