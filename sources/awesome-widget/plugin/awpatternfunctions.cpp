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
#include "awkeysaggregator.h"


QString AWPatternFunctions::expandLambdas(QString code,
                                          AWKeysAggregator *aggregator,
                                          const QVariantHash &metadata,
                                          const QStringList &usedKeys)
{
    qCDebug(LOG_AW) << "Expand lamdas in" << code;

    QJSEngine engine;
    // apply $this values
    code.replace(QString("$this"), metadata[code].toString());
    // parsed values
    for (auto &lambdaKey : usedKeys)
        code.replace(QString("$%1").arg(lambdaKey),
                     aggregator->formatter(metadata[lambdaKey], lambdaKey));
    qCInfo(LOG_AW) << "Expression" << code;
    QJSValue result = engine.evaluate(code);
    if (result.isError()) {
        qCWarning(LOG_AW) << "Uncaught exception at line"
                          << result.property("lineNumber").toInt() << ":"
                          << result.toString();
        return QString();
    } else {
        return result.toString();
    }
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


QList<AWPatternFunctions::AWFunction>
AWPatternFunctions::findFunctionCalls(const QString &function,
                                      const QString &code)
{
    qCDebug(LOG_AW) << "Looking for function" << function << "in" << code;

    // I suggest the following regex for the internal functions
    // $aw_function_name<some args here if any>{{function body}}
    // * args should be always comma separated (e.g. commas are not supported
    // in this field if they are not screened by $, i.e. '$,'
    // * body depends on the function name, double brackets should be screened
    // by using $, e.g. ${
    QRegularExpression regex(
        QString("\\$%1\\<(?<args>.*?)\\>\\{\\{(?<body>.*?)\\}\\}")
            .arg(function));
    regex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QList<AWPatternFunctions::AWFunction> foundFunctions;
    QRegularExpressionMatchIterator it = regex.globalMatch(code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        AWPatternFunctions::AWFunction metadata;
        // work with args
        QString argsString = match.captured(QString("args"));
        if (argsString.isEmpty()) {
            metadata.args = QStringList();
        } else {
            // replace '$,' to 0x1d
            argsString.replace(QString("$,"), QString(0x1d));
            QStringList args = argsString.split(QChar(','));
            std::for_each(args.begin(), args.end(), [](QString &arg) {
                arg.replace(QString(0x1d), QString(","));
            });
            metadata.args = args;
        }
        // other variables
        metadata.body = match.captured(QString("body"));
        metadata.what = match.captured();
        // replace brackets
        metadata.body.replace(QString("${"), QString("{"));
        metadata.body.replace(QString("$}"), QString("}"));
        foundFunctions.append(metadata);
    }

    return foundFunctions;
}


QString AWPatternFunctions::insertAllKeys(QString code, const QStringList &keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(QString("aw_all"), code);
    for (auto &function : found) {
        QString separator
            = function.args.isEmpty() ? QString(",") : function.args.at(0);
        QStringList required = keys.filter(QRegExp(function.body));
        std::for_each(required.begin(), required.end(), [](QString &value) {
            value = QString("%1: $%1").arg(value);
        });

        code.replace(function.what, required.join(separator));
    }

    return code;
}


QString AWPatternFunctions::insertKeyCount(QString code,
                                           const QStringList &keys)
{
    qCDebug(LOG_AW) << "Looking for count in code" << code << "using list"
                    << keys;

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(QString("aw_count"), code);
    for (auto &function : found) {
        int count = keys.filter(QRegExp(function.body)).count();

        code.replace(function.what, QString::number(count));
    }

    return code;
}


QString AWPatternFunctions::insertKeyNames(QString code,
                                           const QStringList &keys)
{
    qCDebug(LOG_AW) << "Looking for key names in code" << code << "using list"
                    << keys;

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(QString("aw_names"), code);
    for (auto &function : found) {
        QString separator
            = function.args.isEmpty() ? QString(",") : function.args.at(0);
        QStringList required = keys.filter(QRegExp(function.body));

        code.replace(function.what, required.join(separator));
    }

    return code;
}


QString AWPatternFunctions::insertKeys(QString code, const QStringList &keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(QString("aw_keys"), code);
    for (auto &function : found) {
        QString separator
            = function.args.isEmpty() ? QString(",") : function.args.at(0);
        QStringList required = keys.filter(QRegExp(function.body));
        std::for_each(required.begin(), required.end(), [](QString &value) {
            value = QString("$%1").arg(value);
        });

        code.replace(function.what, required.join(separator));
    }

    return code;
}


QString AWPatternFunctions::insertMacros(QString code)
{
    qCDebug(LOG_AW) << "Looking for macros in code" << code;

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(QString("aw_macro"), code);
    for (auto &macro : found) {
        // get macro params
        if (macro.args.isEmpty()) {
            qCWarning(LOG_AW) << "No macro name found for" << macro.what;
            continue;
        }
        QString name = macro.args.takeFirst();
        // find macro usage
        QList<AWPatternFunctions::AWFunction> macroUsage
            = AWPatternFunctions::findFunctionCalls(
                QString("aw_macro_%1").arg(name), code);
        for (auto &function : macroUsage) {
            if (function.args.count() != macro.args.count()) {
                qCWarning(LOG_AW)
                    << "Invalid args count found for call" << function.what
                    << "with macro" << macro.what;
                continue;
            }
            // generate body to replace
            QString result = macro.body;
            std::for_each(macro.args.cbegin(), macro.args.cend(),
                          [&result, macro, function](const QString &arg) {
                              int index = macro.args.indexOf(arg);
                              result.replace(QString("$%1").arg(arg),
                                             function.args.at(index));
                          });
            // do replace
            code.replace(function.what, result);
        }

        // remove macro from source pattern
        code.remove(macro.what);
    }

    return code;
}


QStringList AWPatternFunctions::findKeys(const QString &code,
                                         const QStringList &keys,
                                         const bool isBars)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << code << "using list"
                    << keys;

    QStringList selectedKeys;
    QString replacedCode = code;
    for (auto &key : keys)
        if ((key.startsWith(QString("bar")) == isBars)
            && (replacedCode.contains(QString("$%1").arg(key)))) {
            qCInfo(LOG_AW) << "Found key" << key << "with bar enabled"
                           << isBars;
            selectedKeys.append(key);
            replacedCode.replace(QString("$%1").arg(key), "");
        }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No keys found";

    return selectedKeys;
}


QStringList AWPatternFunctions::findLambdas(const QString &code)
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
