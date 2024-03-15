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
#include <QRegularExpression>
#include <QVariant>


namespace KSysGuard
{
class SensorInfo;
}

class AbstractExtSysMonSource : public QObject
{
    Q_OBJECT

public:
    explicit AbstractExtSysMonSource(QObject *_parent, const QStringList &)
        : QObject(_parent){};
    ~AbstractExtSysMonSource() override = default;
    virtual QVariant data(const QString &_source) = 0;
    [[nodiscard]] virtual KSysGuard::SensorInfo *initialData(const QString &_source) const = 0;
    virtual void run() = 0;
    [[nodiscard]] virtual QStringList sources() const = 0;
    // used by extensions
    static int index(const QString &_source)
    {
        QRegularExpression rx("\\d+");
        return rx.match(_source).captured().toInt();
    }

signals:
    void dataReceived(const QVariantHash &);
};


#endif /* ABSTRACTEXTSYSMONSOURCE_H */
