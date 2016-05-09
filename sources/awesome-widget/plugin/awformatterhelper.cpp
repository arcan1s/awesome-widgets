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

#include "awformatterhelper.h"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"
#include "awdatetimeformatter.h"
#include "awfloatformatter.h"
#include "awnoformatter.h"
#include "awscriptformatter.h"


AWFormatterHelper::AWFormatterHelper(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

#ifdef BUILD_FUTURE
    installDirectories();
    initFormatters();
    initKeys();
#endif /* BUILD_FUTURE */
}


AWFormatterHelper::~AWFormatterHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_formatters.clear();
    m_formattersClasses.clear();
}


QString AWFormatterHelper::convert(const QVariant &value,
                                   const QString name) const
{
    qCDebug(LOG_AW) << "Convert value" << value << "for" << name;

    return m_formatters.contains(name) ? m_formatters[name]->convert(value)
                                       : value.toString();
}


QStringList AWFormatterHelper::definedFormatters() const
{
    return m_formatters.keys();
}


QStringList AWFormatterHelper::knownFormatters() const
{
    return m_formattersClasses.keys();
}


AWFormatterHelper::FormatterClass
AWFormatterHelper::defineFormatterClass(const QString stringType) const
{
    qCDebug(LOG_AW) << "Define formatter class for" << stringType;

    FormatterClass formatter = FormatterClass::NoFormat;
    if (stringType == QString("DateTime"))
        formatter = FormatterClass::DateTime;
    else if (stringType == QString("Float"))
        formatter = FormatterClass::Float;
    else if (stringType == QString("Script"))
        formatter = FormatterClass::Script;
    else
        qCWarning(LOG_AW) << "Unknown formatter" << stringType;

    return formatter;
}


void AWFormatterHelper::initFormatters()
{
    m_formattersClasses.clear();

    for (int i = m_directories.count() - 1; i >= 0; i--) {
        QStringList files
            = QDir(m_directories.at(i)).entryList(QDir::Files, QDir::Name);
        for (auto file : files) {
            if (!file.endsWith(QString(".desktop")))
                continue;
            qCInfo(LOG_LIB) << "Found file" << file << "in"
                            << m_directories.at(i);
            QString filePath
                = QString("%1/%2").arg(m_directories.at(i)).arg(file);
            auto metadata = readMetadata(filePath);
            QString name = metadata.first;
            if (m_formattersClasses.contains(name))
                continue;


            switch (metadata.second) {
            case FormatterClass::DateTime:
                m_formattersClasses[name]
                    = new AWDateTimeFormatter(nullptr, filePath);
                break;
            case FormatterClass::Float:
                m_formattersClasses[name]
                    = new AWFloatFormatter(nullptr, filePath);
                break;
            case FormatterClass::Script:
                m_formattersClasses[name]
                    = new AWScriptFormatter(nullptr, filePath);
                break;
            case FormatterClass::NoFormat:
                m_formattersClasses[name]
                    = new AWNoFormatter(nullptr, filePath);
                break;
            }
        }
    }
}


void AWFormatterHelper::initKeys()
{
    m_formatters.clear();

    QSettings settings(m_formatterConfig, QSettings::IniFormat);
    settings.beginGroup(QString("Formatters"));
    QStringList keys = settings.childKeys();
    for (auto key : keys) {
        QString name = settings.value(key).toString();
        qCInfo(LOG_AW) << "Found formatter" << name << "for key" << key;
        m_formatters[key] = m_formattersClasses[name];
    }
    settings.endGroup();
}


void AWFormatterHelper::installDirectories()
{
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/formatters")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        qCInfo(LOG_LIB) << "Created directory" << localDir;

    m_directories = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation,
        QString("awesomewidgets/formatters"), QStandardPaths::LocateDirectory);

    m_formatterConfig = QString("%1/awesomewidgets/formatters/formatters.ini")
                            .arg(QStandardPaths::writableLocation(
                                QStandardPaths::GenericDataLocation));
}


QPair<QString, AWFormatterHelper::FormatterClass>
AWFormatterHelper::readMetadata(const QString filePath) const
{
    qCDebug(LOG_AW) << "Read initial parameters from" << filePath;

    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup(QString("Desktop Entry"));
    QString name = settings.value(QString("Name"), filePath).toString();
    QString type
        = settings.value(QString("Type"), QString("NoFormat")).toString();
    FormatterClass formatter = defineFormatterClass(type);
    settings.endGroup();

    return QPair<QString, AWFormatterHelper::FormatterClass>(name, formatter);
}
