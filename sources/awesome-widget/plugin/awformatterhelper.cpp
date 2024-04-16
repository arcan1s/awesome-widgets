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


AWFormatterHelper::AWFormatterHelper(QWidget *_parent)
    : AbstractExtItemAggregator(_parent, "formatters")
    , AWAbstractPairHelper("awesomewidgets/formatters/formatters.ini", "Formatters")
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    AWFormatterHelper::initItems();
}


AWFormatterHelper::~AWFormatterHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_formatters.clear();
    m_formattersClasses.clear();
}


void AWFormatterHelper::initItems()
{
    initFormatters();

    // assign internal storage
    m_formatters.clear();
    for (auto [key, name] : pairs().asKeyValueRange()) {
        if (!m_formattersClasses.contains(name)) {
            qCWarning(LOG_AW) << "Invalid formatter" << name << "found in" << key;
            continue;
        }

        m_formatters[key] = m_formattersClasses[name];
    }
}


QString AWFormatterHelper::convert(const QVariant &_value, const QString &_name) const
{
    qCDebug(LOG_AW) << "Convert value" << _value << "for" << _name;

    return m_formatters.contains(_name) ? m_formatters[_name]->convert(_value) : _value.toString();
}


QStringList AWFormatterHelper::definedFormatters() const
{
    return m_formatters.keys();
}


QList<AbstractExtItem *> AWFormatterHelper::items() const
{
    QList<AbstractExtItem *> converted;
    for (auto item : m_formattersClasses.values())
        converted.append(item);

    return converted;
}


void AWFormatterHelper::editPairs()
{
    return editItems();
}


QStringList AWFormatterHelper::leftKeys()
{
    return {};
}


QStringList AWFormatterHelper::rightKeys()
{
    return m_formattersClasses.keys();
}


void AWFormatterHelper::editItems()
{
    auto ret = exec();
    qCInfo(LOG_AW) << "Dialog returns" << ret;
}


AWAbstractFormatter::FormatterClass AWFormatterHelper::defineFormatterClass(const QString &_stringType)
{
    qCDebug(LOG_AW) << "Define formatter class for" << _stringType;

    auto formatter = AWAbstractFormatter::FormatterClass::NoFormat;
    if (_stringType == "DateTime")
        formatter = AWAbstractFormatter::FormatterClass::DateTime;
    else if (_stringType == "Float")
        formatter = AWAbstractFormatter::FormatterClass::Float;
    else if (_stringType == "List")
        formatter = AWAbstractFormatter::FormatterClass::List;
    else if (_stringType == "NoFormat")
        ;
    else if (_stringType == "Script")
        formatter = AWAbstractFormatter::FormatterClass::Script;
    else if (_stringType == "String")
        formatter = AWAbstractFormatter::FormatterClass::String;
    else if (_stringType == "Json")
        formatter = AWAbstractFormatter::FormatterClass::Json;
    else
        qCWarning(LOG_AW) << "Unknown formatter" << _stringType;

    return formatter;
}


void AWFormatterHelper::initFormatters()
{
    m_formattersClasses.clear();

    auto dirs = directories();
    for (auto &dir : dirs) {
        auto files = QDir(dir).entryList(QDir::Files, QDir::Name);
        for (auto &file : files) {
            // check filename
            if (!file.endsWith(".desktop"))
                continue;
            qCInfo(LOG_AW) << "Found file" << file << "in" << dir;
            auto filePath = QString("%1/%2").arg(dir, file);
            // check if already exists
            auto values = m_formattersClasses.values();
            if (std::any_of(values.cbegin(), values.cend(),
                            [&filePath](auto item) { return (item->filePath() == filePath); }))
                continue;

            auto metadata = readMetadata(filePath);
            switch (metadata.second) {
            case AWAbstractFormatter::FormatterClass::DateTime:
                m_formattersClasses[metadata.first] = new AWDateTimeFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Float:
                m_formattersClasses[metadata.first] = new AWFloatFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::List:
                m_formattersClasses[metadata.first] = new AWListFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Script:
                m_formattersClasses[metadata.first] = new AWScriptFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::String:
                m_formattersClasses[metadata.first] = new AWStringFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::Json:
                m_formattersClasses[metadata.first] = new AWJsonFormatter(this, filePath);
                break;
            case AWAbstractFormatter::FormatterClass::NoFormat:
                m_formattersClasses[metadata.first] = new AWNoFormatter(this, filePath);
                break;
            }
        }
    }
}


QPair<QString, AWAbstractFormatter::FormatterClass> AWFormatterHelper::readMetadata(const QString &_filePath)
{
    qCDebug(LOG_AW) << "Read initial parameters from" << _filePath;

    QSettings settings(_filePath, QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    auto name = settings.value("Name", _filePath).toString();
    auto type = settings.value("X-AW-Type", "NoFormat").toString();
    auto formatter = defineFormatterClass(type);
    settings.endGroup();

    return {name, formatter};
}


void AWFormatterHelper::doCreateItem(QListWidget *_widget)
{
    QStringList selection = {"NoFormat", "DateTime", "Float", "List", "Script", "String", "Json"};
    bool ok;
    auto select = QInputDialog::getItem(nullptr, i18n("Select type"), i18n("Type:"), selection, 0, false, &ok);
    if (!ok) {
        qCWarning(LOG_AW) << "No type selected";
        return;
    }

    qCInfo(LOG_AW) << "Selected type" << select;
    auto formatter = defineFormatterClass(select);
    switch (formatter) {
    case AWAbstractFormatter::FormatterClass::DateTime:
        return createItem<AWDateTimeFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::Float:
        return createItem<AWFloatFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::List:
        return createItem<AWListFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::Script:
        return createItem<AWScriptFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::String:
        return createItem<AWStringFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::Json:
        return createItem<AWJsonFormatter>(_widget);
    case AWAbstractFormatter::FormatterClass::NoFormat:
        return createItem<AWNoFormatter>(_widget);
    }
}
