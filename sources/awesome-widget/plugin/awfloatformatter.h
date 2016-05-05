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

#ifndef AWFLOATFORMATTER_H
#define AWFLOATFORMATTER_H

#include "awabstractformatter.h"


class AWFloatFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QChar fillChar READ fillChar WRITE setFillChar)
    Q_PROPERTY(char format READ format WRITE setFormat)
    Q_PROPERTY(double multiplier READ multiplier WRITE setMultiplier)
    Q_PROPERTY(int precision READ precision WRITE setPrecision)
    Q_PROPERTY(double summand READ summand WRITE setSummand)
    Q_PROPERTY(int width READ width WRITE setWidth)

public:
    explicit AWFloatFormatter(QObject *parent, const QString filename,
                              const QString section);
    explicit AWFloatFormatter(QObject *parent, const QChar fillChar,
                              const char format, const double multiplier,
                              const int precision, const double summand,
                              const int width);
    virtual ~AWFloatFormatter();
    QString convert(const QVariant &value) const;
    // properties
    QChar fillChar() const;
    char format() const;
    double multiplier() const;
    int precision() const;
    double summand() const;
    int width() const;
    void setFillChar(const QChar _fillChar);
    void setFormat(char _format);
    void setMultiplier(const double _multiplier);
    void setPrecision(const int _precision);
    void setSummand(const double _summand);
    void setWidth(const int _width);

private:
    void init(const QString filename, const QString section);
    // properties
    QChar m_fillChar;
    char m_format;
    double m_multiplier;
    int m_precision;
    double m_summand;
    int m_width;
};


#endif /* AWFLOATFORMATTER_H */
