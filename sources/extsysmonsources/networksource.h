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

#ifndef NETWORKSOURCE_H
#define NETWORKSOURCE_H

#include <QObject>

#include "abstractextsysmonsource.h"


class QProcess;

class NetworkSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    explicit NetworkSource(QObject *_parent, const QStringList &_args);
    ~NetworkSource() override;
    QVariant data(const QString &_source) override;
    QVariantMap initialData(const QString &_source) const override;
    void run() override;
    QStringList sources() const override;

private slots:
    void updateSsid();

private:
    QVariantHash m_values;
    QProcess *m_process = nullptr;
    static QString getCurrentDevice();
};


#endif /* NETWORKSOURCE_H */
