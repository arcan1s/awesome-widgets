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


ColumnLayout {
    anchors.fill: parent

    property var textProvider

    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: i18n("About")
        }
        TabButton {
            text: i18n("Acknowledgment")
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex

        ColumnLayout {
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: textProvider.getAboutText("header")
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: textProvider.getAboutText("version")
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignJustify
                text: textProvider.getAboutText("description")
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                textFormat: Text.RichText
                text: textProvider.getAboutText("links")
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.capitalization: Font.SmallCaps
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignBottom
                textFormat: Text.RichText
                text: textProvider.getAboutText("copy")
            }
        }

        ColumnLayout {
                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    text: textProvider.getAboutText("translators")
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("3rdparty")
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    verticalAlignment: Text.AlignTop
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("thanks")
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
    }
}
