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

import org.kde.plasma.private.desktoppanel 1.0
import "."


Item {
    id: widgetPage
    // backend
    DPAdds {
        id: dpAdds
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: dpAdds.isDebugEnabled()

    property alias cfg_text: textPattern.text


    Column {
        id: pageColumn
        anchors.fill: parent

        AWInfoLabel {}

        HtmlDefaultFunctionsBar {
            textArea: textPattern
        }

        AWTagSelector {
            backend: dpAdds
            notifyBackend: dpAdds
            textArea: textPattern
            groups: general.dpTagRegexp
        }

        AWTextEditor {
            id: textPattern
            backend: dpAdds
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
