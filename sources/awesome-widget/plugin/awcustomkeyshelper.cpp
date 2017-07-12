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

#include "awcustomkeyshelper.h"

#include <QSet>

#include "awdebug.h"


AWCustomKeysHelper::AWCustomKeysHelper(QObject *_parent)
    : QObject(_parent)
    , AWAbstractPairHelper("awesomewidgets/custom.ini", "Custom")
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWCustomKeysHelper::~AWCustomKeysHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWCustomKeysHelper::source(const QString &_key) const
{
    qCDebug(LOG_AW) << "Get source by key" << _key;

    return pairs()[_key];
}


QStringList AWCustomKeysHelper::sources() const
{
    return QSet<QString>::fromList(values()).toList();
}


QStringList AWCustomKeysHelper::refinedSources() const
{
    auto allSources = QSet<QString>::fromList(pairs().values());
    QSet<QString> output;

    while (output != allSources) {
        output.clear();
        for (auto &src : allSources)
            output.insert(pairs().contains(src) ? source(src) : src);
        allSources = output;
    }

    return output.toList();
}


QStringList AWCustomKeysHelper::leftKeys()
{
    return keys();
}


QStringList AWCustomKeysHelper::rightKeys()
{
    return QStringList();
}
