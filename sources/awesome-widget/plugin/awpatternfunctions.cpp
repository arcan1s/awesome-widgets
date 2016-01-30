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

#include "awpatternfunctions.h"

#include <QJSEngine>
#include <QRegularExpression>

#include "awdebug.h"


QVariantList AWPatternFunctions::findFunctionCalls(const QString function,
                                                   const QString code)
{
    qCDebug(LOG_AW) << "Looking for function" << function << "in" << code;

    QRegularExpression regex(
        QString("%1\\<(?<args>.*?)\\>\\((?<body>.*?)\\)").arg(function));
    regex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QVariantList foundFunctions;
    QRegularExpressionMatchIterator it = regex.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        QVariantHash metadata;
        metadata[QString("args")]
            = match.captured(QString("args")).split(QChar(','));
        metadata[QString("body")] = match.captured(QString("body"));
        metadata[QString("what")] = match.captured();
        foundFunctions.append(metadata);
    }

    return foundFunctions;
}


QString AWPatternFunctions::expandTemplates(QString code)
{
    qCDebug(LOG_AW) << "Expand tempaltes in" << code;

    // match the following construction $template{{some code here}}
    QRegularExpression templatesRegexp(
        QString("\\$template\\{\\{((?!\\$template\\{\\{).)*?\\}\\}"));
    templatesRegexp.setPatternOptions(
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = templatesRegexp.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString fullTemplate = match.captured();

        // drop additional markers
        QString templ = fullTemplate;
        templ.remove(QRegExp(QString("^\\$template\\{\\{")));
        templ.remove(QRegExp(QString("\\}\\}$")));

        QJSEngine engine;
        qCInfo(LOG_AW) << "Expression" << templ;
        QJSValue result = engine.evaluate(templ);
        QString templateResult = QString("");
        if (result.isError()) {
            qCWarning(LOG_AW) << "Uncaught exception at line"
                              << result.property("lineNumber").toInt() << ":"
                              << result.toString();
        } else {
            templateResult = result.toString();
        }

        // replace template
        code.replace(fullTemplate, templateResult);
    }

    return code;
}


QString AWPatternFunctions::insertKeyCount(QString code, const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for count in code" << code << "using list"
                    << keys;

    QVariantList found
        = AWPatternFunctions::findFunctionCalls(QString("aw_count"), code);
    for (auto function : found) {
        QVariantHash metadata = function.toHash();
        int count = keys.filter(QRegExp(metadata[QString("body")].toString()))
                        .count();

        code.replace(metadata[QString("what")].toString(),
                     QString::number(count));
    }

    return code;
}


QString AWPatternFunctions::insertKeyNames(QString code, const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QVariantList found
        = AWPatternFunctions::findFunctionCalls(QString("aw_names"), code);
    for (auto function : found) {
        QVariantHash metadata = function.toHash();
        QString separator
            = metadata[QString("args")].toStringList().isEmpty()
                  ? QString(",")
                  : metadata[QString("args")].toStringList().at(0);
        QStringList required
            = keys.filter(QRegExp(metadata[QString("body")].toString()));

        code.replace(metadata[QString("what")].toString(),
                     required.join(separator));
    }

    return code;
}


QString AWPatternFunctions::insertKeys(QString code, const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QVariantList found
        = AWPatternFunctions::findFunctionCalls(QString("aw_keys"), code);
    for (auto function : found) {
        QVariantHash metadata = function.toHash();
        QString separator
            = metadata[QString("args")].toStringList().isEmpty()
                  ? QString(",")
                  : metadata[QString("args")].toStringList().at(0);
        QStringList required
            = keys.filter(QRegExp(metadata[QString("body")].toString()));
        std::for_each(required.begin(), required.end(), [](QString &value) {
            value = QString("$%1").arg(value);
        });

        code.replace(metadata[QString("what")].toString(),
                     required.join(separator));
    }

    return code;
}
