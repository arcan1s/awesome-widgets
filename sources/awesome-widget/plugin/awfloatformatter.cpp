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


#include "awfloatformatter.h"

#include <QSettings>

#include "awdebug.h"


AWFloatFormatter::AWFloatFormatter(QObject *parent, const QString filename,
                                   const QString section)
    : AWAbstractFormatter(parent, filename, section)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    init(filename, section);
}


AWFloatFormatter::AWFloatFormatter(QObject *parent, const QChar fillChar,
                                   const char format, const double multiplier,
                                   const int precision, const double summand,
                                   const int width)
    : AWAbstractFormatter(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    setFillChar(fillChar);
    setFormat(format);
    setMultiplier(multiplier);
    setPrecision(precision);
    setSummand(summand);
    setWidth(width);
}


AWFloatFormatter::~AWFloatFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWFloatFormatter::convert(const QVariant &value) const
{
    qCDebug(LOG_AW) << "Convert value" << value;

    return QString("%1").arg(value.toDouble() * m_multiplier + m_summand,
                             m_width, m_format, m_precision, m_fillChar);
}


QChar AWFloatFormatter::fillChar() const
{
    return m_fillChar;
}


char AWFloatFormatter::format() const
{
    return m_format;
}


double AWFloatFormatter::multiplier() const
{
    return m_multiplier;
}


int AWFloatFormatter::precision() const
{
    return m_precision;
}


double AWFloatFormatter::summand() const
{
    return m_summand;
}


int AWFloatFormatter::width() const
{
    return m_width;
}


void AWFloatFormatter::setFillChar(const QChar _fillChar)
{
    qCDebug(LOG_AW) << "Set char" << _fillChar;

    m_fillChar = _fillChar;
}


void AWFloatFormatter::setFormat(char _format)
{
    qCDebug(LOG_AW) << "Set format" << _format;
    // http://doc.qt.io/qt-5/qstring.html#argument-formats
    if ((_format != 'e') && (_format != 'E') && (_format != 'f')
        && (_format != 'g') && (_format != 'G')) {
        qCWarning(LOG_AW) << "Invalid format" << _format;
        _format = 'f';
    }

    m_format = _format;
}


void AWFloatFormatter::setMultiplier(const double _multiplier)
{
    qCDebug(LOG_AW) << "Set multiplier" << _multiplier;

    m_multiplier = _multiplier;
}


void AWFloatFormatter::setPrecision(const int _precision)
{
    qCDebug(LOG_AW) << "Set precision" << _precision;

    m_precision = _precision;
}


void AWFloatFormatter::setSummand(const double _summand)
{
    qCDebug(LOG_AW) << "Set summand" << _summand;

    m_summand = _summand;
}


void AWFloatFormatter::setWidth(const int _width)
{
    qCDebug(LOG_AW) << "Set width" << _width;

    m_width = _width;
}


void AWFloatFormatter::init(const QString filename, const QString section)
{
    qCDebug(LOG_AW) << "Looking for section" << section << "in" << filename;

    QSettings settings(filename, QSettings::IniFormat);

    settings.beginGroup(section);
    setFillChar(
        settings.value(QString("FillChar"), QString()).toString().at(0));
    setFormat(settings.value(QString("Format"), QString("f"))
                  .toString()
                  .at(0)
                  .toLatin1());
    setMultiplier(settings.value(QString("Multiplier"), 1.0).toDouble());
    setPrecision(settings.value(QString("Precision"), -1).toInt());
    setSummand(settings.value(QString("Summand"), 0.0).toDouble());
    setWidth(settings.value(QString("Width"), 0).toInt());
    settings.endGroup();
}
