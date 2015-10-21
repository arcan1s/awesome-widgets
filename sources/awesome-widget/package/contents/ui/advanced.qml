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

import org.kde.plasma.private.awesomewidget 1.0


Item {
    id: advancedPage
    // backend
    AWActions {
        id: awActions
    }
    AWConfigHelper {
        id: awConfig
    }

    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: awActions.isDebugEnabled()

    property alias cfg_background: background.checked
    property alias cfg_translateStrings: translate.checked
    property alias cfg_wrapNewLines: wrapNewLines.checked
    property alias cfg_wrapText: wordWrap.checked
    property alias cfg_notify: notify.checked
    property alias cfg_checkUpdates: updates.checked
    property alias cfg_height: widgetHeight.value
    property alias cfg_width: widgetWidth.value
    property alias cfg_interval: update.value
    property alias cfg_queueLimit: queueLimit.value
    property string cfg_tempUnits: tempUnits.currentText
    property alias cfg_customTime: customTime.text
    property alias cfg_customUptime: customUptime.text
    property alias cfg_acOnline: acOnline.text
    property alias cfg_acOffline: acOffline.text


    Column {
        id: pageColumn
        anchors.fill: parent
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: background
                width: parent.width * 3 / 5
                text: i18n("Enable background")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: translate
                width: parent.width * 3 / 5
                text: i18n("Translate strings")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: wrapNewLines
                width: parent.width * 3 / 5
                text: i18n("Wrap new lines")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: wordWrap
                width: parent.width * 3 / 5
                text: i18n("Enable word wrap")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: notify
                width: parent.width * 3 / 5
                text: i18n("Enable notifications")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.heigth
                width: parent.width * 2 / 5
            }
            QtControls.CheckBox {
                id: updates
                width: parent.width * 3 / 5
                text: i18n("Check updates on startup")
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Widget height, px")
            }
            QtControls.SpinBox {
                id: widgetHeight
                width: parent.width * 3 / 5
                minimumValue: 0
                maximumValue: 4096
                stepSize: 50
                value: plasmoid.configuration.height
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Widget width, px")
            }
            QtControls.SpinBox {
                id: widgetWidth
                width: parent.width * 3 / 5
                minimumValue: 0
                maximumValue: 4096
                stepSize: 50
                value: plasmoid.configuration.width
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Time interval")
            }
            QtControls.SpinBox {
                id: update
                width: parent.width * 3 / 5
                minimumValue: 1000
                maximumValue: 10000
                stepSize: 500
                value: plasmoid.configuration.interval
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Messages queue limit")
            }
            QtControls.SpinBox {
                id: queueLimit
                width: parent.width * 3 / 5
                minimumValue: 0
                maximumValue: 99
                stepSize: 1
                value: plasmoid.configuration.queueLimit
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Temperature units")
            }
            QtControls.ComboBox {
                id: tempUnits
                width: parent.width * 3 / 5
                textRole: "label"
                model: [
                    {
                        'label': i18n("Celsius"),
                        'name': "Celsius"
                    },
                    {
                        'label': i18n("Fahrenheit"),
                        'name': "Fahrenheit"
                    },
                    {
                        'label': i18n("Kelvin"),
                        'name': "Kelvin"
                    },
                    {
                        'label': i18n("Reaumur"),
                        'name': "Reaumur"
                    },
                    {
                        'label': i18n("cm^-1"),
                        'name': "cm^-1"
                    },
                    {
                        'label': i18n("kJ/mol"),
                        'name': "kJ/mol"
                    },
                    {
                        'label': i18n("kcal/mol"),
                        'name': "kcal/mol"
                    }
                ]
                onCurrentIndexChanged: cfg_tempUnits = model[currentIndex]["name"]
                Component.onCompleted: {
                    if (debug) console.debug()
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.tempUnits) {
                            if (debug) console.info("Found", model[i]["name"], "on", i)
                            tempUnits.currentIndex = i;
                        }
                    }
                }
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Custom time format")
            }
            QtControls.TextField {
                id: customTime
                width: parent.width * 3 / 5
                text: plasmoid.configuration.customTime
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Custom uptime format")
            }
            QtControls.TextField {
                id: customUptime
                width: parent.width * 3 / 5
                text: plasmoid.configuration.customUptime
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("AC online tag")
            }
            QtControls.TextField {
                id: acOnline
                width: parent.width * 3 / 5
                text: plasmoid.configuration.acOnline
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("AC offline tag")
            }
            QtControls.TextField {
                id: acOffline
                width: parent.width * 3 / 5
                text: plasmoid.configuration.acOffline
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Drop key cache")
                onClicked: awActions.dropCache()
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Export configuration")
                onClicked: saveConfigAs.open()
            }

            QtDialogs.FileDialog {
                id: saveConfigAs
                selectExisting: false
                title: i18n("Export")
                onAccepted: {
                    var status = awConfig.exportConfiguration(
                        plasmoid.configuration,
                        saveConfigAs.fileUrl.toString().replace("file://", ""))
                    if (status) {
                        messageDialog.title = i18n("Success")
                        messageDialog.text = i18n("Please note that binary files were not copied")
                    } else {
                        messageDialog.title = i18n("Ooops...")
                        messageDialog.text = i18n("Could not save configuration file")
                    }
                    messageDialog.open()
                }
            }

            QtDialogs.MessageDialog {
                id: messageDialog
                standardButtons: QtDialogs.StandardButton.Ok
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
            }
            QtControls.Button {
                width: parent.width * 3 / 5
                text: i18n("Import configuration")
                onClicked: openConfig.open()
            }

            QtDialogs.FileDialog {
                id: openConfig
                title: i18n("Import")
                onAccepted: importSelection.open()
            }

            QtDialogs.Dialog {
                id: importSelection

                Column {
                    QtControls.CheckBox {
                        id: importPlasmoid
                        text: i18n("Import plasmoid settings")
                    }

                    QtControls.CheckBox {
                        id: importExtensions
                        text: i18n("Import extensions")
                    }

                    QtControls.CheckBox {
                        id: importAdds
                        text: i18n("Import additional files")
                    }
                }

                onAccepted: {
                    if (debug) console.debug()
                    var importConfig = awConfig.importConfiguration(
                        openConfig.fileUrl.toString().replace("file://", ""),
                        importPlasmoid.checked, importExtensions.checked,
                        importAdds.checked)
                    for (var key in importConfig)
                        plasmoid.configuration[key] = importConfig[key]
                }
            }
        }
    }


    Component.onCompleted: {
        if (debug) console.debug()
    }
}
