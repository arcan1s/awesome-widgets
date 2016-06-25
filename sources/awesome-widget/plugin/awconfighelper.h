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


#ifndef AWCONFIGHELPER_H
#define AWCONFIGHELPER_H

#include <QObject>
#include <QStandardPaths>
#include <QVariant>


class QSettings;

class AWConfigHelper : public QObject
{
    Q_OBJECT

public:
    explicit AWConfigHelper(QObject *parent = nullptr);
    virtual ~AWConfigHelper();
    Q_INVOKABLE QString configurationDirectory() const;
    Q_INVOKABLE bool dropCache() const;
    Q_INVOKABLE bool exportConfiguration(const QObject *nativeConfig,
                                         const QString fileName) const;
    Q_INVOKABLE QVariantMap importConfiguration(const QString fileName,
                                                const bool importPlasmoid,
                                                const bool importExtensions,
                                                const bool importAdds) const;
    // dataengine
    Q_INVOKABLE QVariantMap readDataEngineConfiguration() const;
    Q_INVOKABLE bool
    writeDataEngineConfiguration(const QVariantMap configuration) const;

private:
    // methods
    void copyConfigs(const QString localDir) const;
    void copyExtensions(const QString item, const QString type,
                        QSettings &settings, const bool inverse) const;
    void copySettings(QSettings &from, QSettings &to) const;
    void readFile(QSettings &settings, const QString key,
                  const QString fileName) const;
    void writeFile(QSettings &settings, const QString key,
                   const QString fileName) const;
    // properties
    QString m_baseDir = QString("%1/awesomewidgets")
                            .arg(QStandardPaths::writableLocation(
                                QStandardPaths::GenericDataLocation));
    QStringList m_dirs = QStringList()
                         << QString("desktops") << QString("quotes")
                         << QString("scripts") << QString("upgrade")
                         << QString("weather");
};


#endif /* AWCONFIGHELPER_H */
