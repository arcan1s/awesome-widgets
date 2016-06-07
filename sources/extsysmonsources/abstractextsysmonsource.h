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

#ifndef ABSTRACTEXTSYSMONSOURCE_H
#define ABSTRACTEXTSYSMONSOURCE_H

#include <QObject>
#include <QRegExp>
#include <QVariant>


class AbstractExtSysMonSource : public QObject
{
    Q_OBJECT

public:
    explicit AbstractExtSysMonSource(QObject *parent, const QStringList)
        : QObject(parent){};
    virtual ~AbstractExtSysMonSource(){};
    virtual QVariant data(QString source) = 0;
    virtual QVariantMap initialData(QString source) const = 0;
    virtual void run() = 0;
    virtual QStringList sources() const = 0;
    // used by extensions
    int index(const QString source) const
    {
        QRegExp rx(QString("\\d+"));
        rx.indexIn(source);
        return rx.cap().toInt();
    }

signals:
    void dataReceived(const QVariantHash &);
};


#endif /* ABSTRACTEXTSYSMONSOURCE_H */
