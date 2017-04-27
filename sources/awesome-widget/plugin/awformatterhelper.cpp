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

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QInputDialog>
#include <QSettings>

#include "awdatetimeformatter.h"
#include "awdebug.h"
#include "awfloatformatter.h"
#include "awjsonformatter.h"
#include "awlistformatter.h"
#include "awnoformatter.h"
#include "awscriptformatter.h"
#include "awstringformatter.h"


AWFormatterHelper::AWFormatterHelper(QWidget *parent)
    : AbstractExtItemAggregator(parent, QString("formatters"))
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_filePath = QString("awesomewidgets/formatters/formatters.ini");
    initItems();
}


AWFormatterHelper::~AWFormatterHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_formatters.clear();
    m_formattersClasses.clear();
}


QString AWFormatterHelper::convert(const QVariant &value,
                                   const QString &name) const
{
    qCDebug(LOG_AW) << "Convert value" << value << "for" << name;

    return m_formatters.contains(name) ? m_formatters[name]->convert(value)
                                       : value.toString();
}


QStringList AWFormatterHelper::definedFormatters() const
{
    return m_formatters.keys();
}


QHash<QString, QString> AWFormatterHelper::getFormatters() const
{
    QHash<QString, QString> map;
    for (auto tag : m_formatters.keys())
        map[tag] = m_formatters[tag]->name();

    return map;
}


QList<AbstractExtItem *> AWFormatterHelper::items() const
{
    QList<AbstractExtItem *> converted;
    for (auto item : m_formattersClasses.values())
        converted.append(item);

    return converted;
}


QStringList AWFormatterHelper::knownFormatters() const
{
    return m_formattersClasses.keys();
}


bool AWFormatterHelper::removeUnusedFormatters(const QStringList keys) const
{
    qCDebug(LOG_AW) << "Remove formatters" << keys;

    QString fileName = QString("%1/%2")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation))
                           .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup(QString("Formatters"));
    QStringList foundKeys = settings.childKeys();
    for (auto key : foundKeys) {
        if (keys.contains(key))
            continue;
        settings.remove(key);
    }
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}


bool AWFormatterHelper::writeFormatters(
    const QHash<QString, QString> configuration) const
{
    qCDebug(LOG_AW) << "Write configuration" << configuration;

    QString fileName = QString("%1/%2")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation))
                           .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup(QString("Formatters"));
    for (auto key : configuration.keys())
        settings.setValue(key, configuration[key]);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}


void AWFormatterHelper::editItems()
{
    repaintList();
    int ret = exec();
    qCInfo(LOG_AW) << "Dialog returns" << ret;
}


AWAbstractFormatter::FormatterClass
AWFormatterHelper::defineFormatterClass(const QString stringType) const
{
    qCDebug(LOG_AW) << "Define formatter class for" << stringType;

    AWAbstractFormatter::FormatterClass formatter
        = AWAbstractFormatter::FormatterClass::NoFormat;
    if (stringType == QString("DateTime"))
        formatter = AWAbstractFormatter::FormatterClass::DateTime;
    else if (stringType == QString("Float"))
        formatter = AWAbstractFormatter::FormatterClass::Float;
    else if (stringType == QString("List"))
        formatter = AWAbstractFormatter::FormatterClass::List;
    else if (stringType == QString("NoFormat"))
        ;
    else if (stringType == QString("Script"))
        formatter = AWAbstractFormatter::FormatterClass::Script;
    else if (stringType == QString("String"))
        formatter = AWAbstractFormatter::FormatterClass::String;
    else if (stringType == QString("Json"))
        formatter = AWAbstractFormatter::FormatterClass::Json;
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
            qCInfo(LOG_AW) << "Found file" << file << "in"
                           << m_directories.at(i);
            QString filePath
                = QString("%1/%2").arg(m_directories.at(i)).arg(file);
            auto metadata = readMetadata(filePath);
            QString name = metadata.first;
            if (m_formattersClasses.contains(name))
                continue;


            switch (metadata.second) {
            case AWAbstractFormatter::FormatterClass::DateTime:
                m_formattersClasses[name]
                    = new AWDateTimeFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Float:
                m_formattersClasses[name]
                    = new AWFloatFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::List:
                m_formattersClasses[name] = new AWListFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Script:
                m_formattersClasses[name]
                    = new AWScriptFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::String:
                m_formattersClasses[name]
                    = new AWStringFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Json:
                m_formattersClasses[name] = new AWJsonFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::NoFormat:
                m_formattersClasses[name] = new AWNoFormatter(this, filePath);
                break;
            }
        }
    }
}


void AWFormatterHelper::initKeys()
{
    m_formatters.clear();

    QStringList configs = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation, m_filePath);

    for (auto fileName : configs) {
        QSettings settings(fileName, QSettings::IniFormat);
        qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

        settings.beginGroup(QString("Formatters"));
        QStringList keys = settings.childKeys();
        for (auto key : keys) {
            QString name = settings.value(key).toString();
            qCInfo(LOG_AW) << "Found formatter" << name << "for key" << key
                           << "in" << settings.fileName();
            if (name.isEmpty()) {
                qCInfo(LOG_AW) << "Skip empty formatter for" << key;
                continue;
            }
            if (!m_formattersClasses.contains(name)) {
                qCWarning(LOG_AW)
                    << "Invalid formatter" << name << "found in" << key;
                continue;
            }
            m_formatters[key] = m_formattersClasses[name];
        }
        settings.endGroup();
    }
}


void AWFormatterHelper::installDirectories()
{
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/formatters")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        qCInfo(LOG_AW) << "Created directory" << localDir;

    m_directories = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation,
        QString("awesomewidgets/formatters"), QStandardPaths::LocateDirectory);
}


QPair<QString, AWAbstractFormatter::FormatterClass>
AWFormatterHelper::readMetadata(const QString filePath) const
{
    qCDebug(LOG_AW) << "Read initial parameters from" << filePath;

    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup(QString("Desktop Entry"));
    QString name = settings.value(QString("Name"), filePath).toString();
    QString type
        = settings.value(QString("X-AW-Type"), QString("NoFormat")).toString();
    AWAbstractFormatter::FormatterClass formatter = defineFormatterClass(type);
    settings.endGroup();

    return QPair<QString, AWAbstractFormatter::FormatterClass>(name, formatter);
}


void AWFormatterHelper::doCreateItem()
{
    QStringList selection = QStringList()
                            << QString("NoFormat") << QString("DateTime")
                            << QString("Float") << QString("List")
                            << QString("Script") << QString("String")
                            << QString("Json");
    bool ok;
    QString select = QInputDialog::getItem(
        this, i18n("Select type"), i18n("Type:"), selection, 0, false, &ok);
    if (!ok) {
        qCWarning(LOG_AW) << "No type selected";
        return;
    }

    qCInfo(LOG_AW) << "Selected type" << select;
    AWAbstractFormatter::FormatterClass formatter
        = defineFormatterClass(select);
    switch (formatter) {
    case AWAbstractFormatter::FormatterClass::DateTime:
        return createItem<AWDateTimeFormatter>();
    case AWAbstractFormatter::FormatterClass::Float:
        return createItem<AWFloatFormatter>();
    case AWAbstractFormatter::FormatterClass::List:
        return createItem<AWListFormatter>();
    case AWAbstractFormatter::FormatterClass::Script:
        return createItem<AWScriptFormatter>();
    case AWAbstractFormatter::FormatterClass::String:
        return createItem<AWStringFormatter>();
    case AWAbstractFormatter::FormatterClass::Json:
        return createItem<AWJsonFormatter>();
    case AWAbstractFormatter::FormatterClass::NoFormat:
        return createItem<AWNoFormatter>();
    }
}


void AWFormatterHelper::initItems()
{
    installDirectories();
    initFormatters();
    initKeys();
}
