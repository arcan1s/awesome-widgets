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

#include <QObject>


class AWTelemetryHandler : public QObject
{
    Q_OBJECT

public:
    explicit AWTelemetryHandler(QObject *_parent = nullptr);
    ~AWTelemetryHandler() override = default;
    Q_INVOKABLE [[nodiscard]] QStringList get(const QString &_group) const;
    Q_INVOKABLE [[nodiscard]] QString getLast(const QString &_group) const;
    Q_INVOKABLE void init(int _count);
    Q_INVOKABLE [[nodiscard]] bool put(const QString &_group, const QString &_value) const;

private:
    static QString getKey(int _count);
    QString m_localFile;
    int m_storeCount = 0;
};
