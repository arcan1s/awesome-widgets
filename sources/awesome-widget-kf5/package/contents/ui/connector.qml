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

import org.kde.plasma.private.awesomewidget 1.0


QtObject {
    id: connector

    // variables
    property bool debug: AWKeys.isDebugEnabled()
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
    // external
    property variant settings: {
        "customTime": plasmoid.configuration.customTime,
        "customUptime": plasmoid.configuration.customUptime,
        "tempUnits": plasmoid.configuration.tempUnits,
        "tempDevice": plasmoid.configuration.tempDevice,
        "fanDevice": plasmoid.configuration.fanDevice,
        "mount": plasmoid.configuration.mount,
        "hdd": plasmoid.configuration.hdd,
        "disk": plasmoid.configuration.disk,
        "customNetdev": plasmoid.configuration.customNetdev,
        "acOnline": plasmoid.configuration.acOnline,
        "acOffline": plasmoid.configuration.acOffline
    }
    property variant tooltipSettings: {
        "tooltipNumber": plasmoid.configuration.tooltipNumber,
        "useTooltipBackground": plasmoid.configuration.useTooltipBackground,
        "tooltipBackgroung": plasmoid.configuration.tooltipBackgroung,
        "cpuTooltip": plasmoid.configuration.cpuTooltip,
        "cpuclTooltip": plasmoid.configuration.cpuclTooltip,
        "memTooltip": plasmoid.configuration.memTooltip,
        "swapTooltip": plasmoid.configuration.swapTooltip,
        "downTooltip": plasmoid.configuration.downTooltip,
        "upTooltip": plasmoid.configuration.downTooltip,
        "batteryTooltip": plasmoid.configuration.batteryTooltip,
        "cpuTooltipColor": plasmoid.configuration.cpuTooltipColor,
        "cpuclTooltipColor": plasmoid.configuration.cpuclTooltipColor,
        "memTooltipColor": plasmoid.configuration.memTooltipColor,
        "swapTooltipColor": plasmoid.configuration.swapTooltipColor,
        "downTooltipColor": plasmoid.configuration.downTooltipColor,
        "upTooltipColor": plasmoid.configuration.upTooltipColor,
        "batteryTooltipColor": plasmoid.configuration.batteryTooltipColor,
        "batteryInTooltipColor": plasmoid.configuration.batteryInTooltipColor
    }
    property string pattern: plasmoid.configuration.text

    Component.onCompleted: {
        if (debug) console.log("[connector::onCompleted]")

        // init submodule
        AWKeys.initKeys(pattern, settings, tooltipSettings)
    }

    function addDevice(source) {
        AWActions.addDevice(source)
    }

    function checkKeys(data) {
        return AWActions.checkKeys(data)
    }

    function checkUpdates() {
        AWActions.checkUpdates()
    }

    function dictKeys() {
        return AWKeys.dictKeys()
    }

    function editItem(type) {
        AWKeys.editItem(type)
    }

    function getAboutText(type) {
        AWActions.getAboutText(type)
    }

    function getDiskDevices() {
        return AWActions.getDiskDevices()
    }

    function getFanDevices() {
        return AWActions.getFanDevices()
    }

    function getFont(defaultFont) {
        return AWActions.getFont(defaultFont)
    }

    function getHddDevices() {
        return AWActions.getHddDevices()
    }

    function getMountDevices() {
        return AWActions.getMountDevices()
    }

    function getNetworkDevices() {
        return AWActions.getNetworkDevices()
    }

    function getTempDevices() {
        return AWActions.getTempDevices()
    }

    function isReady() {
        return AWKeys.isReady()
    }

    function parsePattern() {
        return AWKeys.parsePattern(pattern)
    }

    function selectDevice(all, current) {
        return AWActions.selectDevices(all, current)
    }

    function sendNotification(event, message) {
        AWActions.sendNotification(event, message)
    }

    function setDataBySource(sourceName, data) {
        AWKeys.setDataBySource(sourceName, data, settings)
    }

    function showValue(tag) {
        return AWKeys.valueByKey(tag)
    }

    function showReadme() {
        AWActions.showReadme()
    }
}
