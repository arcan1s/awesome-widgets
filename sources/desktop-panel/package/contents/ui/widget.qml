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
import QtQuick.Controls 1.3 as QtControls

import org.kde.plasma.private.desktoppanel 1.0


Item {
    id: widgetPage
    // backend
    DPAdds {
        id: dpAdds;
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
        QtControls.Label {
          width: parent.width
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          wrapMode: Text.WordWrap
          text: i18n("Detailed information may be found on <a href=\"http://arcanis.name/projects/awesome-widgets/\">project homepage</a>")
          onLinkActivated: Qt.openUrlExternally(link);
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Button {
                width: parent.width * 3 / 12
                text: i18n("Font")
                iconName: "font"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Font button")

                    var defaultFont = {
                        "color": plasmoid.configuration.fontColor,
                        "family": plasmoid.configuration.fontFamily,
                        "size": plasmoid.configuration.fontSize
                    }
                    var font = dpAdds.getFont(defaultFont)
                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<span style=\"color:" + font.color +
                                            "; font-family:'" + font.family +
                                            "'; font-size:" + font.size + "pt;\">" +
                                            selected + "</span>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-indent-more"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Indent button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, selected + "<br>\n")
                }
            }

            QtControls.Button {
                width: parent.width / 12
                iconName: "format-text-bold"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Bold button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<b>" + selected + "</b>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-text-italic"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Italic button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<i>" + selected + "</i>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-text-underline"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Underline button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<u>" + selected + "</u>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-text-strikethrough"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Strike button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<s>" + selected + "</s>")
                }
            }

            QtControls.Button {
                width: parent.width / 12
                iconName: "format-justify-left"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Left button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<p align=\"left\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-justify-center"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Center button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<p align=\"center\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-justify-right"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Right button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<p align=\"right\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 12
                iconName: "format-justify-fill"

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Justify button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, "<p align=\"justify\">" + selected + "</p>")
                }
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.ComboBox {
                id: tags
                width: parent.width - addTagButton.width - showValueButton.width
                model: dpAdds.dictKeys()
            }
            QtControls.Button {
                id: addTagButton
                text: i18n("Add")

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Add tag button")

                    var pos = textPattern.cursorPosition
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(pos, selected + "$" + tags.currentText)
                }
            }
            QtControls.Button {
                id: showValueButton
                text: i18n("Show value")

                onClicked: {
                    if (debug) console.log("[widget::onClicked] : Show tag button")

                    var message = i18n("Tag: %1", tags.currentText)
                    message += "<br>"
                    message += i18n("Value: %1", dpAdds.valueByKey(tags.currentText))
                    dpAdds.sendNotification("tag", message)
                }
            }
        }

        QtControls.TextArea {
            id: textPattern
            width: parent.width
            height: parent.height * 4 / 5
            textFormat: TextEdit.PlainText
            text: plasmoid.configuration.text
        }
    }


    Component.onCompleted: {
        if (debug) console.log("[widget::onCompleted]")
    }
}
