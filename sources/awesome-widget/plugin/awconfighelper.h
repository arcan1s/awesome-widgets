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

#pragma once

#include <QObject>
#include <QVariant>


class QSettings;

class AWConfigHelper : public QObject
{
    Q_OBJECT

public:
    explicit AWConfigHelper(QObject *_parent = nullptr);
    ~AWConfigHelper() override;
    Q_INVOKABLE [[nodiscard]] static QString configurationDirectory();
    Q_INVOKABLE static bool dropCache();
    Q_INVOKABLE bool exportConfiguration(QObject *_nativeConfig, const QString &_fileName) const;
    Q_INVOKABLE [[nodiscard]] QVariantMap importConfiguration(const QString &_fileName, bool _importPlasmoid,
                                                              bool _importExtensions, bool _importAdds) const;
    // dataengine
    Q_INVOKABLE static QVariantMap readDataEngineConfiguration();
    Q_INVOKABLE static bool writeDataEngineConfiguration(const QVariantMap &_configuration);

private:
    // methods
    static void copyConfigs(const QString &_localDir);
    void copyExtensions(const QString &_item, const QString &_type, QSettings &_settings, bool _inverse) const;
    static void copySettings(QSettings &_from, QSettings &_to);
    static void readFile(QSettings &_settings, const QString &_key, const QString &_fileName);
    static void writeFile(QSettings &_settings, const QString &_key, const QString &_fileName);
    // properties
    QString m_baseDir;
    QStringList m_dirs = {"desktops", "quotes", "scripts", "upgrade", "weather", "formatters"};
};
