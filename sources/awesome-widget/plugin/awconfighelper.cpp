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

#include "awconfighelper.h"

#include <QDir>
#include <QQmlPropertyMap>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


AWConfigHelper::AWConfigHelper(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_baseDir = QString("%1/awesomewidgets").arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
}


QString AWConfigHelper::configurationDirectory()
{
    // get readable directory
    auto localDir = QString("%1/awesomewidgets/configs")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));

    // create directory and copy files from default settings
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir))) {
        qCInfo(LOG_AW) << "Created directory" << localDir;
        copyConfigs(localDir);
    }

    return localDir;
}


bool AWConfigHelper::dropCache()
{
    auto fileName
        = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));

    return QFile(fileName).remove();
}


bool AWConfigHelper::exportConfiguration(QObject *_nativeConfig, const QString &_fileName) const
{
    qCDebug(LOG_AW) << "Selected filename" << _fileName;

    QSettings settings(_fileName, QSettings::IniFormat);
    // plasmoid configuration
    auto configuration = dynamic_cast<const QQmlPropertyMap *>(_nativeConfig);
    settings.beginGroup("plasmoid");
    for (auto &key : configuration->keys()) {
        auto value = configuration->value(key);
        if (!value.isValid())
            continue;
        settings.setValue(key, value);
    }
    settings.endGroup();

    // extensions
    for (auto &item : m_dirs) {
        auto items = QDir(QString("%1/%2").arg(m_baseDir, item)).entryList({"*.desktop"}, QDir::Files);
        settings.beginGroup(item);
        for (auto &it : items)
            copyExtensions(it, item, settings, false);
        settings.endGroup();
    }

    // additional files
    settings.beginGroup("json");
    // script filters
    readFile(settings, "filters", QString("%1/scripts/awesomewidgets-extscripts-filters.json").arg(m_baseDir));
    // weather icon settings
    readFile(settings, "weathers", QString("%1/weather/awesomewidgets-extweather-ids.json").arg(m_baseDir));
    settings.endGroup();

    settings.beginGroup("ini");
    // formatter settings
    readFile(settings, "formatters", QString("%1/formatters/formatters.ini").arg(m_baseDir));
    // custom keys settings
    readFile(settings, "custom", QString("%1/custom.ini").arg(m_baseDir));
    settings.endGroup();

    // sync settings
    settings.sync();
    // show additional message
    return settings.status() == QSettings::NoError;
}


QVariantMap AWConfigHelper::importConfiguration(const QString &_fileName, const bool _importPlasmoid,
                                                const bool _importExtensions, const bool _importAdds) const
{
    qCDebug(LOG_AW) << "Selected filename" << _fileName;

    QVariantMap configuration;
    QSettings settings(_fileName, QSettings::IniFormat);

    // extensions
    if (_importExtensions) {
        for (auto &item : m_dirs) {
            settings.beginGroup(item);
            for (auto &it : settings.childGroups())
                copyExtensions(it, item, settings, true);
            settings.endGroup();
        }
    }

    // additional files
    if (_importAdds) {
        settings.beginGroup("json");
        // script filters
        writeFile(settings, "filters", QString("%1/scripts/awesomewidgets-extscripts-filters.json").arg(m_baseDir));
        // weather icon settings
        writeFile(settings, "weathers", QString("%1/weather/awesomewidgets-extweather-ids.json").arg(m_baseDir));
        settings.endGroup();

        settings.beginGroup("ini");
        // formatter settings
        writeFile(settings, "formatters", QString("%1/formatters/formatters.ini").arg(m_baseDir));
        // custom keys settings
        writeFile(settings, "custom", QString("%1/custom.ini").arg(m_baseDir));
        settings.endGroup();
    }

    // plasmoid configuration
    if (_importPlasmoid) {
        settings.beginGroup("plasmoid");
        for (auto &key : settings.childKeys())
            configuration[key] = settings.value(key);
        settings.endGroup();
    }

    return configuration;
}


QVariantMap AWConfigHelper::readDataEngineConfiguration()
{
    auto fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, "plasma-dataengine-extsysmon.conf");
    qCInfo(LOG_AW) << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QVariantMap configuration;

    settings.beginGroup("Configuration");
    configuration["ACPIPATH"] = settings.value("ACPIPATH", "/sys/class/power_supply/");
    configuration["MPDADDRESS"] = settings.value("MPDADDRESS", "localhost");
    configuration["MPDPORT"] = settings.value("MPDPORT", "6600");
    configuration["MPRIS"] = settings.value("MPRIS", "auto");
    configuration["PLAYER"] = settings.value("PLAYER", "mpris");
    configuration["PLAYERSYMBOLS"] = settings.value("PLAYERSYMBOLS", "10");
    settings.endGroup();

    qCInfo(LOG_AW) << "Configuration" << configuration;

    return configuration;
}


bool AWConfigHelper::writeDataEngineConfiguration(const QVariantMap &_configuration)
{
    qCDebug(LOG_AW) << "Configuration" << _configuration;

    auto fileName = QString("%1/plasma-dataengine-extsysmon.conf")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

    settings.beginGroup("Configuration");
    settings.setValue("ACPIPATH", _configuration["ACPIPATH"]);
    settings.setValue("MPDADDRESS", _configuration["MPDADDRESS"]);
    settings.setValue("MPDPORT", _configuration["MPDPORT"]);
    settings.setValue("MPRIS", _configuration["MPRIS"]);
    settings.setValue("PLAYER", _configuration["PLAYER"]);
    settings.setValue("PLAYERSYMBOLS", _configuration["PLAYERSYMBOLS"]);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}


void AWConfigHelper::copyConfigs(const QString &_localDir)
{
    qCDebug(LOG_AW) << "Local directory" << _localDir;

    auto dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "awesomewidgets/configs",
                                          QStandardPaths::LocateDirectory);
    for (auto &dir : dirs) {
        if (dir == _localDir)
            continue;
        auto files = QDir(dir).entryList(QDir::Files);
        for (auto &source : files) {
            auto destination = QString("%1/%2").arg(_localDir, source);
            auto status = QFile::copy(QString("%1/%2").arg(dir, source), destination);
            qCInfo(LOG_AW) << "File" << source << "has been copied to" << destination << "with status" << status;
        }
    }
}


void AWConfigHelper::copyExtensions(const QString &_item, const QString &_type, QSettings &_settings,
                                    const bool _inverse) const
{
    qCDebug(LOG_AW) << "Extension" << _item << "has type" << _type << "inverse copying" << _inverse;

    _settings.beginGroup(_item);
    QSettings itemSettings(QString("%1/%2/%3").arg(m_baseDir, _type, _item), QSettings::IniFormat);
    itemSettings.beginGroup("Desktop Entry");
    if (_inverse)
        copySettings(_settings, itemSettings);
    else
        copySettings(itemSettings, _settings);
    itemSettings.endGroup();
    _settings.endGroup();

    if (_inverse)
        itemSettings.sync();
}


void AWConfigHelper::copySettings(QSettings &_from, QSettings &_to)
{
    for (auto &key : _from.childKeys())
        _to.setValue(key, _from.value(key));
}


void AWConfigHelper::readFile(QSettings &_settings, const QString &_key, const QString &_fileName)
{
    qCDebug(LOG_AW) << "Key" << _key << "from file" << _fileName;

    QFile file(_fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        auto text = QString::fromUtf8(file.readAll());
        file.close();
        _settings.setValue(_key, text);
    } else {
        qCWarning(LOG_AW) << "Could not open to read" << file.fileName();
    }
}


void AWConfigHelper::writeFile(QSettings &_settings, const QString &_key, const QString &_fileName)
{
    qCDebug(LOG_AW) << "Key" << _key << "to file" << _fileName;

    if (!_settings.contains(_key))
        return;

    QFile file(_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << _settings.value(_key).toString().toUtf8();
        out.flush();
        file.close();
    } else {
        qCWarning(LOG_AW) << "Could not open to write" << file.fileName();
    }
}
