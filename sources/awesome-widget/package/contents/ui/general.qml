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

pragma Singleton
import QtQuick 2.0


QtObject {
    property variant fontWeight: {
        "light": Font.Light,
        "normal": Font.Normal,
        "demibold": Font.DemiBold,
        "bold": Font.Bold,
        "black": Font.Black
    }
    property variant align: {
        "left": Text.AlignLeft,
        "center": Text.AlignHCenter,
        "right": Text.AlignRight,
        "justify": Text.AlignJustify
    }
    property variant textStyle: {
        "normal": Text.Normal,
        "outline": Text.Outline,
        "raised": Text.Raised,
        "sunken": Text.Sunken
    }

    // models
    property variant fontStyleModel: [
        {
            'label': i18n("normal"),
            'name': "normal"
        },
        {
            'label': i18n("italic"),
            'name': "italic"
        }
    ]
    property variant fontWeightModel: [
        {
            'label': i18n("light"),
            'name': "light"
        },
        {
            'label': i18n("normal"),
            'name': "normal"
        },
        {
            'label': i18n("demi bold"),
            'name': "demibold"
        },
        {
            'label': i18n("bold"),
            'name': "bold"
        },
        {
            'label': i18n("black"),
            'name': "black"
        }
    ]
    property variant textStyleModel: [
        {
            'label': i18n("normal"),
            'name': "normal"
        },
        {
            'label': i18n("outline"),
            'name': "outline"
        },
        {
            'label': i18n("raised"),
            'name': "raised"
        },
        {
            'label': i18n("sunken"),
            'name': "sunken"
        }
    ]
}
