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
import QtQuick.Dialogs 1.2 as QtDialogs

import org.kde.plasma.private.desktoppanel 1.0


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
        QtControls.Label {
          width: parent.width
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          wrapMode: Text.WordWrap
          text: i18n("Detailed information may be found on <a href=\"https://arcanis.me/projects/awesome-widgets/\">project homepage</a>")
          onLinkActivated: Qt.openUrlExternally(link);
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Button {
                width: parent.width * 3 / 15
                text: i18n("Bgcolor")

                onClicked: backgroundDialog.visible = true

                QtDialogs.ColorDialog {
                    id: backgroundDialog
                    title: i18n("Select a color")
                    onAccepted: {
                        var text = textPattern.text
                        textPattern.text = "<body bgcolor=\"" +
                            backgroundDialog.color + "\">" +
                            text + "</body>"
                    }
                }
            }
            QtControls.Button {
                width: parent.width * 3 / 15
                text: i18n("Font")
                iconName: "font"

                onClicked: {
                    if (debug) console.debug("Font button")
                    var defaultFont = {
                        "color": plasmoid.configuration.fontColor,
                        "family": plasmoid.configuration.fontFamily,
                        "size": plasmoid.configuration.fontSize
                    }
                    var font = dpAdds.getFont(defaultFont)
                    if (font.applied != 1) {
                        if (debug) console.debug("No font selected")
                        return
                    }

                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition,
                        "<span style=\"color:" + font.color +
                        "; font-family:'" + font.family +
                        "'; font-size:" + font.size + "pt;\">" +
                        selected + "</span>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-indent-more"

                onClicked: {
                    if (debug) console.debug("Indent button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, selected + "<br>\n")
                }
            }

            QtControls.Button {
                width: parent.width / 15
                iconName: "format-text-bold"

                onClicked: {
                    if (debug) console.debug("Bold button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<b>" + selected + "</b>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-text-italic"

                onClicked: {
                    if (debug) console.debug("Italic button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<i>" + selected + "</i>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-text-underline"

                onClicked: {
                    if (debug) console.debug("Underline button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<u>" + selected + "</u>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-text-strikethrough"

                onClicked: {
                    if (debug) console.debug("Strike button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<s>" + selected + "</s>")
                }
            }

            QtControls.Button {
                width: parent.width / 15
                iconName: "format-justify-left"

                onClicked: {
                    if (debug) console.debug("Left button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<p align=\"left\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-justify-center"

                onClicked: {
                    if (debug) console.debug("Center button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<p align=\"center\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-justify-right"

                onClicked: {
                    if (debug) console.debug("Right button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<p align=\"right\">" + selected + "</p>")
                }
            }
            QtControls.Button {
                width: parent.width / 15
                iconName: "format-justify-fill"

                onClicked: {
                    if (debug) console.debug("Justify button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, "<p align=\"justify\">" + selected + "</p>")
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
                    if (debug) console.debug("Add tag button")
                    var selected = textPattern.selectedText
                    textPattern.remove(textPattern.selectionStart, textPattern.selectionEnd)
                    textPattern.insert(textPattern.cursorPosition, selected + "$" + tags.currentText)
                }
            }
            QtControls.Button {
                id: showValueButton
                text: i18n("Show value")

                onClicked: {
                    if (debug) console.debug("Show tag button")
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
        if (debug) console.debug()
    }
}
