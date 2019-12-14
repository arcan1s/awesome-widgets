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

#include "awpairconfigfactory.h"

#include "awcustomkeysconfig.h"
#include "awdebug.h"
#include "awformatterconfig.h"


AWPairConfigFactory::AWPairConfigFactory(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWPairConfigFactory::~AWPairConfigFactory()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWPairConfigFactory::showFormatterDialog(const QStringList &_keys)
{
    auto *config = new AWFormatterConfig(nullptr, _keys);
    config->showDialog();
    config->deleteLater();
}


void AWPairConfigFactory::showKeysDialog(const QStringList &_keys)
{
    auto *config = new AWCustomKeysConfig(nullptr, _keys);
    config->showDialog();
    config->deleteLater();
}
