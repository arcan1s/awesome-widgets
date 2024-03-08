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


QString AWPatternFunctions::expandLambdas(QString _code, AWKeysAggregator *_aggregator, const QVariantHash &_metadata,
                                          const QStringList &_usedKeys)
{
    qCDebug(LOG_AW) << "Expand lambdas in" << _code;

    QJSEngine engine;
    // apply $this values
    _code.replace("$this", _metadata[_code].toString());
    // parsed values
    for (auto &lambdaKey : _usedKeys)
        _code.replace(QString("$%1").arg(lambdaKey), _aggregator->formatter(_metadata[lambdaKey], lambdaKey, false));
    qCInfo(LOG_AW) << "Expression" << _code;
    QJSValue result = engine.evaluate(_code);
    if (result.isError()) {
        qCWarning(LOG_AW) << "Uncaught exception at line" << result.property("lineNumber").toInt() << ":"
                          << result.toString();
        return "";
    } else {
        return result.toString();
    }
}


QString AWPatternFunctions::expandTemplates(QString _code)
{
    qCDebug(LOG_AW) << "Expand templates in" << _code;

    // match the following construction $template{{some code here}}
    QRegularExpression templatesRegexp(R"(\$template\{\{(?<body>.*?)\}\})");
    templatesRegexp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = templatesRegexp.globalMatch(_code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString body = match.captured("body");

        QJSEngine engine;
        qCInfo(LOG_AW) << "Expression" << body;
        QJSValue result = engine.evaluate(body);
        QString templateResult = "";
        if (result.isError()) {
            qCWarning(LOG_AW) << "Uncaught exception at line" << result.property("lineNumber").toInt() << ":"
                              << result.toString();
        } else {
            templateResult = result.toString();
        }

        // replace template
        _code.replace(match.captured(), templateResult);
    }

    return _code;
}


QList<AWPatternFunctions::AWFunction> AWPatternFunctions::findFunctionCalls(const QString &_function,
                                                                            const QString &_code)
{
    qCDebug(LOG_AW) << "Looking for function" << _function << "in" << _code;

    // I suggest the following regex for the internal functions
    // $aw_function_name<some args here if any>{{function body}}
    // * args should be always comma separated (e.g. commas are not supported
    // in this field if they are not screened by $, i.e. '$,'
    // * body depends on the function name, double brackets should be screened
    // by using $, e.g. ${
    QRegularExpression regex(QString(R"(\$%1\<(?<args>.*?)\>\{\{(?<body>.*?)\}\})").arg(_function));
    regex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QList<AWPatternFunctions::AWFunction> foundFunctions;
    QRegularExpressionMatchIterator it = regex.globalMatch(_code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        AWPatternFunctions::AWFunction metadata;
        // work with args
        QString argsString = match.captured("args");
        if (argsString.isEmpty()) {
            metadata.args = QStringList();
        } else {
            // replace '$,' to 0x1d
            argsString.replace("$,", QChar(0x1d));
            QStringList args = argsString.split(',');
            std::for_each(args.begin(), args.end(), [](QString &arg) { arg.replace(QChar(0x1d), ","); });
            metadata.args = args;
        }
        // other variables
        metadata.body = match.captured("body");
        metadata.what = match.captured();
        // replace brackets
        metadata.body.replace("${", "{");
        metadata.body.replace("$}", "}");
        foundFunctions.append(metadata);
    }

    return foundFunctions;
}


QString AWPatternFunctions::insertAllKeys(QString _code, const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << _code << "using list" << _keys;

    QList<AWPatternFunctions::AWFunction> found = AWPatternFunctions::findFunctionCalls("aw_all", _code);
    for (auto &function : found) {
        QString separator = function.args.isEmpty() ? "," : function.args.at(0);
        QStringList required = _keys.filter(QRegularExpression(function.body));
        std::for_each(required.begin(), required.end(), [](QString &value) { value = QString("%1: $%1").arg(value); });

        _code.replace(function.what, required.join(separator));
    }

    return _code;
}


QString AWPatternFunctions::insertKeyCount(QString _code, const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Looking for count in code" << _code << "using list" << _keys;

    QList<AWPatternFunctions::AWFunction> found = AWPatternFunctions::findFunctionCalls("aw_count", _code);
    for (auto &function : found) {
        int count = _keys.filter(QRegularExpression(function.body)).count();

        _code.replace(function.what, QString::number(count));
    }

    return _code;
}


QString AWPatternFunctions::insertKeyNames(QString _code, const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Looking for key names in code" << _code << "using list" << _keys;

    QList<AWPatternFunctions::AWFunction> found = AWPatternFunctions::findFunctionCalls("aw_names", _code);
    for (auto &function : found) {
        QString separator = function.args.isEmpty() ? "," : function.args.at(0);
        QStringList required = _keys.filter(QRegularExpression(function.body));

        _code.replace(function.what, required.join(separator));
    }

    return _code;
}


QString AWPatternFunctions::insertKeys(QString _code, const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << _code << "using list" << _keys;

    QList<AWPatternFunctions::AWFunction> found = AWPatternFunctions::findFunctionCalls("aw_keys", _code);
    for (auto &function : found) {
        QString separator = function.args.isEmpty() ? "," : function.args.at(0);
        QStringList required = _keys.filter(QRegularExpression(function.body));
        std::for_each(required.begin(), required.end(), [](QString &value) { value = QString("$%1").arg(value); });

        _code.replace(function.what, required.join(separator));
    }

    return _code;
}


QString AWPatternFunctions::insertMacros(QString _code)
{
    qCDebug(LOG_AW) << "Looking for macros in code" << _code;

    QList<AWPatternFunctions::AWFunction> found = AWPatternFunctions::findFunctionCalls("aw_macro", _code);
    for (auto &macro : found) {
        // get macro params
        if (macro.args.isEmpty()) {
            qCWarning(LOG_AW) << "No macro name found for" << macro.what;
            continue;
        }
        QString name = macro.args.takeFirst();
        // find macro usage
        QList<AWPatternFunctions::AWFunction> macroUsage
            = AWPatternFunctions::findFunctionCalls(QString("aw_macro_%1").arg(name), _code);
        for (auto &function : macroUsage) {
            if (function.args.count() != macro.args.count()) {
                qCWarning(LOG_AW) << "Invalid args count found for call" << function.what << "with macro" << macro.what;
                continue;
            }
            // generate body to replace
            QString result = macro.body;
            std::for_each(macro.args.cbegin(), macro.args.cend(), [&result, macro, function](const QString &arg) {
                int index = macro.args.indexOf(arg);
                result.replace(QString("$%1").arg(arg), function.args.at(index));
            });
            // do replace
            _code.replace(function.what, result);
        }

        // remove macro from source pattern
        _code.remove(macro.what);
    }

    return _code;
}


QStringList AWPatternFunctions::findKeys(const QString &_code, const QStringList &_keys, const bool _isBars)
{
    qCDebug(LOG_AW) << "Looking for keys in code" << _code << "using list" << _keys;

    QStringList selectedKeys;
    QString replacedCode = _code;
    for (auto &key : _keys)
        if ((key.startsWith("bar") == _isBars) && (replacedCode.contains(QString("$%1").arg(key)))) {
            qCInfo(LOG_AW) << "Found key" << key << "with bar enabled" << _isBars;
            selectedKeys.append(key);
            replacedCode.replace(QString("$%1").arg(key), "");
        }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No keys found";

    return selectedKeys;
}


QStringList AWPatternFunctions::findLambdas(const QString &_code)
{
    qCDebug(LOG_AW) << "Looking for lambdas in code" << _code;

    QStringList selectedKeys;
    // match the following construction ${{some code here}}
    QRegularExpression lambdaRegexp(R"(\$\{\{(?<body>.*?)\}\})");
    lambdaRegexp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = lambdaRegexp.globalMatch(_code);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString lambda = match.captured("body");

        // append
        qCInfo(LOG_AW) << "Found lambda" << lambda;
        selectedKeys.append(lambda);
    }
    if (selectedKeys.isEmpty())
        qCWarning(LOG_AW) << "No lambdas found";

    return selectedKeys;
}
