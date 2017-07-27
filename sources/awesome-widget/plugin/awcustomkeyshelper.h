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


#ifndef AWCUSTOMKEYSHELPER_H
#define AWCUSTOMKEYSHELPER_H

#include <QObject>

#include "awabstractpairhelper.h"


class AWCustomKeysHelper : public QObject, public AWAbstractPairHelper
{
    Q_OBJECT

public:
    explicit AWCustomKeysHelper(QObject *_parent = nullptr);
    virtual ~AWCustomKeysHelper();
    // get
    QString source(const QString &_key) const;
    QStringList sources() const;
    QStringList refinedSources() const;
    // configuration related
    virtual void editPairs(){};
    virtual QStringList leftKeys();
    virtual QStringList rightKeys();

private:
};


#endif /* AWCUSTOMKEYSHELPER_H */
