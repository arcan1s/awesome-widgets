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

#include <QString>
#include <QVariant>


class AWKeysAggregator;

namespace __attribute__((visibility("default"))) AWPatternFunctions
{
typedef struct {
    QStringList args;
    QString body;
    QString what;
} AWFunction;

// insert methods
QString expandLambdas(QString _code, AWKeysAggregator *_aggregator, const QVariantHash &_metadata,
                      const QStringList &_usedKeys);
QString expandTemplates(QString _code);
QList<AWFunction> findFunctionCalls(const QString &_function, const QString &_code);
QString insertAllKeys(QString _code, const QStringList &_keys);
QString insertKeyCount(QString _code, const QStringList &_keys);
QString insertKeyNames(QString _code, const QStringList &_keys);
QString insertKeys(QString _code, const QStringList &_keys);
QString insertMacros(QString _code);
// find methods
QStringList findKeys(const QString &_code, const QStringList &_keys, bool _isBars);
QStringList findLambdas(const QString &_code);
} // namespace AWPatternFunctions
