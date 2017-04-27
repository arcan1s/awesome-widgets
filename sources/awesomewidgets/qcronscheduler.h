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

#ifndef QCRONSCHEDULER_H
#define QCRONSCHEDULER_H

#include "QObject"


class QTimer;

class QCronScheduler : public QObject
{
    Q_OBJECT

public:
    typedef struct {
        QList<int> minutes;
        QList<int> hours;
        QList<int> days;
        QList<int> months;
        QList<int> weekdays;
    } QCronRunSchedule;
    typedef struct {
        int min = -1;
        int max = -1;
        int div = 1;
        void fromRange(const QString &range, const int min, const int max);
        QList<int> toList();
    } QCronField;

    explicit QCronScheduler(QObject *parent);
    virtual ~QCronScheduler();
    void parse(const QString &timer);

signals:
    void activated();

private slots:
    void expired();

private:
    QCronRunSchedule m_schedule;
    QTimer *m_timer = nullptr;
    QList<int> parseField(const QString &value, const int min,
                          const int max) const;
};


#endif /* QCRONSCHEDULER_H */
