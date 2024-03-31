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
import org.kde.kcmutils as KCM

import org.kde.plasma.awesomewidgets
import org.kde.plasma.private.awesomewidget


KCM.SimpleKCM {
    id: dataenginePage

    // backend
    AWKeys {
        id: awKeys
    }
    AWConfigHelper {
        id: awConfig
    }

    property variant cfg_dataengine: awConfig.readDataEngineConfiguration()

    Column {
        id: pageColumn
        anchors.fill: parent

        GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("ACPI")
            LineSelector {
                text: i18n("ACPI path")
                value: cfg_dataengine["ACPIPATH"]
                onValueEdited: newValue => cfg_dataengine["ACPIPATH"] = newValue
            }
        }

        GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("Player")

            Column {
                height: implicitHeight
                width: parent.width
                IntegerSelector {
                    maximumValue: 100
                    minimumValue: 1
                    stepSize: 1
                    text: i18n("Player data symbols")
                    value: cfg_dataengine["PLAYERSYMBOLS"]
                    onValueEdited: newValue => cfg_dataengine["PLAYERSYMBOLS"] = newValue
                }

                ComboBoxSelector {
                    model: [
                        {
                            "label": "disable",
                            "name": "disable"
                        },
                        {
                            "label": "mpris",
                            "name": "mpris"
                        },
                        {
                            "label": "mpd",
                            "name": "mpd"
                        }
                    ]
                    text: i18n("Music player")
                    value: cfg_dataengine["PLAYER"]
                    onValueEdited: newValue => cfg_dataengine["PLAYER"] = newValue
                }

                ComboBoxSelector {
                    id: mpris
                    editable: true
                    model: [
                        {
                            "label": "auto",
                            "name": "auto"
                        },
                        {
                            "label": "amarok",
                            "name": "amarok"
                        },
                        {
                            "label": "audacious",
                            "name": "audacious"
                        },
                        {
                            "label": "clementine",
                            "name": "clementine"
                        },
                        {
                            "label": "DeaDBeeF",
                            "name": "DeaDBeeF"
                        },
                        {
                            "label": "vlc",
                            "name": "vlc"
                        },
                        {
                            "label": "qmmp",
                            "name": "qmmp"
                        },
                        {
                            "label": "xmms2",
                            "name": "xmms2"
                        },
                        {
                            "label": cfg_dataengine["MPRIS"],
                            "name": cfg_dataengine["MPRIS"]
                        }
                    ]
                    text: i18n("MPRIS player name")
                    currentIndex: model.length - 1
                }

                LineSelector {
                    text: i18n("MPD address")
                    value: cfg_dataengine["MPDADDRESS"]
                    onValueEdited: newValue => cfg_dataengine["MPDADDRESS"] = newValue
                }

                IntegerSelector {
                    maximumValue: 65535
                    minimumValue: 1000
                    stepSize: 1
                    text: i18n("MPD port")
                    value: cfg_dataengine["MPDPORT"]
                    onValueEdited: newValue => cfg_dataengine["MPDPORT"] = newValue
                }
            }
        }

        GroupBox {
            height: implicitHeight
            width: parent.width
            title: i18n("Extensions")

            Column {
                height: implicitHeight
                width: parent.width

                ButtonSelector {
                    value: i18n("Custom scripts")
                    onButtonActivated: awKeys.editItem("extscript")
                }

                ButtonSelector {
                    value: i18n("Network requests")
                    onButtonActivated: awKeys.editItem("extnetworkrequest")
                }

                ButtonSelector {
                    value: i18n("Package manager")
                    onButtonActivated: awKeys.editItem("extupgrade")
                }

                ButtonSelector {
                    value: i18n("Quotes monitor")
                    onButtonActivated: awKeys.editItem("extquotes")
                }

                ButtonSelector {
                    value: i18n("Weather")
                    onButtonActivated: awKeys.editItem("extweather")
                }
            }
        }
    }


    Component.onCompleted: {
        // init submodule
        awKeys.updateCache()

        // update hdd model
        hdd.model = awKeys.getHddDevices()
        hdd.onCompleted
    }

    Component.onDestruction: {
        cfg_dataengine["MPRIS"] = mpris.editText
        awConfig.writeDataEngineConfiguration(cfg_dataengine)
    }
}
