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

#include "awabstractpairhelper.h"

#include <QSettings>
#include <QStandardPaths>
#include <utility>

#include "awdebug.h"


AWAbstractPairHelper::AWAbstractPairHelper(QString _filePath, QString _section)
    : m_filePath(std::move(_filePath))
    , m_section(std::move(_section))
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    initItems();
}


AWAbstractPairHelper::~AWAbstractPairHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QStringList AWAbstractPairHelper::keys() const
{
    return m_pairs.keys();
}


QHash<QString, QString> AWAbstractPairHelper::pairs() const
{
    return m_pairs;
}


QStringList AWAbstractPairHelper::values() const
{
    return m_pairs.values();
}


void AWAbstractPairHelper::initItems()
{
    m_pairs.clear();

    QStringList configs
        = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, m_filePath);

    for (auto &fileName : configs) {
        QSettings settings(fileName, QSettings::IniFormat);
        qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

        settings.beginGroup(m_section);
        QStringList keys = settings.childKeys();
        for (auto &key : keys) {
            QString value = settings.value(key).toString();
            qCInfo(LOG_AW) << "Found key" << key << "for value" << value << "in"
                           << settings.fileName();
            if (value.isEmpty()) {
                qCInfo(LOG_AW) << "Skip empty value for" << key;
                continue;
            }
            m_pairs[key] = value;
        }
        settings.endGroup();
    }
}


bool AWAbstractPairHelper::writeItems(const QHash<QString, QString> &_configuration) const
{
    qCDebug(LOG_AW) << "Write configuration" << _configuration;

    QString fileName
        = QString("%1/%2")
              .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
              .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup(m_section);
    for (auto &key : _configuration.keys())
        settings.setValue(key, _configuration[key]);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}


bool AWAbstractPairHelper::removeUnusedKeys(const QStringList &_keys) const
{
    qCDebug(LOG_AW) << "Remove keys" << _keys;

    QString fileName
        = QString("%1/%2")
              .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
              .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup(m_section);
    QStringList foundKeys = settings.childKeys();
    for (auto &key : foundKeys) {
        if (_keys.contains(key))
            continue;
        settings.remove(key);
    }
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}
