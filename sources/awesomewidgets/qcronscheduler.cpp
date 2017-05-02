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


#include "qcronscheduler.h"

#include <QDateTime>
#include <QTimer>

#include "awdebug.h"


QCronScheduler::QCronScheduler(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    m_timer->setInterval(60 * 1000);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(expired()));

    m_timer->start();
}


QCronScheduler::~QCronScheduler()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_timer->stop();
    delete m_timer;
}


void QCronScheduler::parse(const QString &timer)
{
    qCDebug(LOG_LIB) << "Parse timer string" << timer;

    QStringList fields = timer.split(' ');
    if (fields.count() != 5)
        return;

    m_schedule.minutes = parseField(fields.at(0), 0, 59);
    m_schedule.hours = parseField(fields.at(1), 0, 23);
    m_schedule.days = parseField(fields.at(2), 1, 31);
    m_schedule.months = parseField(fields.at(3), 1, 12);
    m_schedule.weekdays = parseField(fields.at(4), 1, 7);
}


void QCronScheduler::expired()
{
    QDateTime now = QDateTime::currentDateTime();

    if (m_schedule.minutes.contains(now.time().minute())
        && m_schedule.hours.contains(now.time().hour())
        && m_schedule.days.contains(now.date().day())
        && m_schedule.months.contains(now.date().month())
        && m_schedule.weekdays.contains(now.date().dayOfWeek()))
        emit(activated());
}


QList<int> QCronScheduler::parseField(const QString &value, const int min,
                                      const int max) const
{
    qCDebug(LOG_LIB) << "Parse field" << value << "with corner values" << min
                     << max;

    QList<int> parsed;
    auto fields = value.split(',');
    for (auto &field : fields) {
        QCronField parsedField;
        parsedField.fromRange(field.split('/').first(), min, max);
        if (field.contains('/')) {
            bool status;
            parsedField.div = field.split('/', QString::SkipEmptyParts)
                                  .at(1)
                                  .toInt(&status);
            if (!status)
                parsedField.div = 1;
        }
        // append
        parsed.append(parsedField.toList());
    }

    return parsed;
}


void QCronScheduler::QCronField::fromRange(const QString &range, const int min,
                                           const int max)
{
    qCDebug(LOG_LIB) << "Parse field from range" << range
                     << "with corner values" << min << max;

    if (range == '*') {
        minValue = min;
        maxValue = max;
    } else if (range.contains('-')) {
        auto interval = range.split('-', QString::SkipEmptyParts);
        if (interval.count() != 2)
            return;
        bool status;
        // minimal value
        minValue = std::max(min, interval.at(0).toInt(&status));
        if (!status)
            minValue = -1;
        // maximal value
        maxValue = std::min(max, interval.at(1).toInt(&status));
        if (!status)
            maxValue = -1;
        // error check
        if (minValue > maxValue)
            std::swap(minValue, maxValue);
    } else {
        bool status;
        int value = range.toInt(&status);
        if (!status || (value < min) || (value > max))
            value = -1;
        minValue = value;
        maxValue = value;
    }
}


QList<int> QCronScheduler::QCronField::toList()
{
    // error checking
    if ((minValue == -1) || (maxValue == -1))
        return QList<int>();

    QList<int> output;
    for (auto i = minValue; i <= maxValue; ++i) {
        if (i % div != 0)
            continue;
        output.append(i);
    }

    std::sort(output.begin(), output.end());
    return output;
}
