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

#include "awcustomkeyshelper.h"

#include <QSet>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


AWCustomKeysHelper::AWCustomKeysHelper(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_filePath = "awesomewidgets/custom.ini";
    initKeys();
}


AWCustomKeysHelper::~AWCustomKeysHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWCustomKeysHelper::initKeys()
{
    m_keys.clear();

    QStringList configs = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation, m_filePath);

    for (auto &fileName : configs) {
        QSettings settings(fileName, QSettings::IniFormat);
        qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

        settings.beginGroup("Custom");
        QStringList keys = settings.childKeys();
        for (auto &key : keys) {
            QString source = settings.value(key).toString();
            qCInfo(LOG_AW) << "Found custom key" << key << "for source"
                           << source << "in" << settings.fileName();
            if (source.isEmpty()) {
                qCInfo(LOG_AW) << "Skip empty source for" << key;
                continue;
            }
            m_keys[key] = source;
        }
        settings.endGroup();
    }
}


bool AWCustomKeysHelper::writeKeys(
    const QHash<QString, QString> &_configuration) const
{
    qCDebug(LOG_AW) << "Write configuration" << _configuration;

    QString fileName = QString("%1/%2")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation))
                           .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup("Custom");
    for (auto &key : _configuration.keys())
        settings.setValue(key, _configuration[key]);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}


QStringList AWCustomKeysHelper::keys() const
{
    return m_keys.keys();
}


QString AWCustomKeysHelper::source(const QString &_key) const
{
    qCDebug(LOG_AW) << "Get source by key" << _key;

    return m_keys[_key];
}


QStringList AWCustomKeysHelper::sources() const
{
    return QSet<QString>::fromList(m_keys.values()).toList();
}


QStringList AWCustomKeysHelper::refinedSources() const
{
    auto allSources = QSet<QString>::fromList(m_keys.values());
    QSet<QString> output;

    while (output != allSources) {
        output.clear();
        for (auto &src : allSources)
            output.insert(m_keys.contains(src) ? source(src) : src);
        allSources = output;
    }

    return output.toList();
}
