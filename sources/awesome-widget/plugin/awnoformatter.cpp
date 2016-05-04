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


#include "awnoformatter.h"

#include "awdebug.h"


AWNoFormatter::AWNoFormatter(QObject *parent, const QString filename,
                             const QString section)
    : AWAbstractFormatter(parent, filename, section)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    init(filename, section);
}


AWNoFormatter::AWNoFormatter(QObject *parent)
    : AWAbstractFormatter(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWNoFormatter::~AWNoFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWNoFormatter::convert(const QVariant &value) const
{
    qCDebug(LOG_AW) << "Convert value" << value;

    return value.toString();
}


void AWNoFormatter::init(const QString filename, const QString section)
{
    qCDebug(LOG_AW) << "Looking for section" << section << "in" << filename;
    // dummy method for future references
}
