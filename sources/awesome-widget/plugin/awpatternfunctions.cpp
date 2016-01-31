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

    // I suggest the following regex for the internal functions
    // $aw_function_name<some args here if any>{{function body}}
    // * args should be always comma separated (e.g. commas are not supported
    // in this field if they are not screened by $, i.e. '$,'
    // * body depends on the function name, double brackets (i.e. {{ or }}) are
    // not supported
    QRegularExpression regex(
        QString("\\$%1\\<(?<args>.*?)\\>\\{\\{(?<body>.*?)\\}\\}")
            .arg(function));
    regex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QVariantList foundFunctions;
    QRegularExpressionMatchIterator it = regex.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        QVariantHash metadata;
        // work with args
        QString argsString = match.captured(QString("args"));
        if (argsString.isEmpty()) {
            metadata[QString("args")] = QStringList();
        } else {
            // replace '$,' to 0x1d
            argsString.replace(QString("$,"), QString(0x1d));
            QStringList args = argsString.split(QChar(','));
            std::for_each(args.begin(), args.end(), [](QString &arg) {
                arg.replace(QString(0x1d), QString(","));
            });
            metadata[QString("args")] = args;
        }
        // other variables
        metadata[QString("body")] = match.captured(QString("body"));
        metadata[QString("what")] = match.captured();
        foundFunctions.append(metadata);
    }

    return foundFunctions;
}


QString AWPatternFunctions::expandTemplates(QString code)
{
    qCDebug(LOG_AW) << "Expand templates in" << code;

    // match the following construction $template{{some code here}}
    QRegularExpression templatesRegexp(
        QString("\\$template\\{\\{(?<body>.*?)\\}\\}"));
    templatesRegexp.setPatternOptions(
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = templatesRegexp.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString body = match.captured(QString("body"));

        QJSEngine engine;
        qCInfo(LOG_AW) << "Expression" << body;
        QJSValue result = engine.evaluate(body);
        QString templateResult = QString("");
        if (result.isError()) {
            qCWarning(LOG_AW) << "Uncaught exception at line"
                              << result.property("lineNumber").toInt() << ":"
                              << result.toString();
        } else {
            templateResult = result.toString();
        }

        // replace template
        code.replace(match.captured(), templateResult);
    }

    return code;
}


QString AWPatternFunctions::insertAllKeys(QString code, const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QVariantList found
        = AWPatternFunctions::findFunctionCalls(QString("aw_all"), code);
    for (auto function : found) {
        QVariantHash metadata = function.toHash();
        QString separator
            = metadata[QString("args")].toStringList().isEmpty()
                  ? QString(",")
                  : metadata[QString("args")].toStringList().at(0);
        QStringList required
            = keys.filter(QRegExp(metadata[QString("body")].toString()));
        std::for_each(required.begin(), required.end(), [](QString &value) {
            value = QString("%1: $%1").arg(value);
        });

        code.replace(metadata[QString("what")].toString(),
                     required.join(separator));
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
    qCDebug(LOG_AW) << "Looking for key names in code" << code << "using list"
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


QStringList AWPatternFunctions::findBars(const QString code,
                                         const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for bars in code" << code << "using list"
                    << keys;

    QStringList selectedKeys;
    for (auto key : keys)
        if ((key.startsWith(QString("bar")))
            && (code.contains(QString("$%1").arg(key)))) {
            qCInfo(LOG_AW) << "Found bar" << key;
            selectedKeys.append(key);
        }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No bars found";

    return selectedKeys;
}


QStringList AWPatternFunctions::findKeys(const QString code,
                                         const QStringList keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QStringList selectedKeys;
    for (auto key : keys)
        if ((!key.startsWith(QString("bar")))
            && (code.contains(QString("$%1").arg(key)))) {
            qCInfo(LOG_AW) << "Found key" << key;
            selectedKeys.append(key);
        }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No keys found";

    return selectedKeys;
}


QStringList AWPatternFunctions::findLambdas(const QString code)
{
    qCDebug(LOG_AW) << "Looking for lambdas in code" << code;

    QStringList selectedKeys;
    // match the following construction ${{some code here}}
    QRegularExpression lambdaRegexp(QString("\\$\\{\\{(?<body>.*?)\\}\\}"));
    lambdaRegexp.setPatternOptions(
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = lambdaRegexp.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString lambda = match.captured(QString("body"));

        // append
        qCInfo(LOG_AW) << "Found lambda" << lambda;
        selectedKeys.append(lambda);
    }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No lambdas found";

    return selectedKeys;
}
