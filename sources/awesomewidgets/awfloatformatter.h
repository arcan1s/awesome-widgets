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


namespace Ui
{
class AWFloatFormatter;
}

class AWFloatFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(QChar fillChar READ fillChar WRITE setFillChar)
    Q_PROPERTY(bool forceWidth READ forceWidth WRITE setForceWidth)
    Q_PROPERTY(char format READ format WRITE setFormat)
    Q_PROPERTY(double multiplier READ multiplier WRITE setMultiplier)
    Q_PROPERTY(int precision READ precision WRITE setPrecision)
    Q_PROPERTY(double summand READ summand WRITE setSummand)

public:
    explicit AWFloatFormatter(QWidget *parent,
                              const QString filePath = QString());
    virtual ~AWFloatFormatter();
    QString convert(const QVariant &_value) const;
    AWFloatFormatter *copy(const QString _fileName, const int _number);
    // properties
    int count() const;
    QChar fillChar() const;
    bool forceWidth() const;
    char format() const;
    double multiplier() const;
    int precision() const;
    double summand() const;
    void setCount(const int _count);
    void setFillChar(const QChar _fillChar);
    void setForceWidth(const bool _forceWidth);
    void setFormat(char _format);
    void setMultiplier(const double _multiplier);
    void setPrecision(const int _precision);
    void setSummand(const double _summand);

public slots:
    void readConfiguration();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private:
    Ui::AWFloatFormatter *ui = nullptr;
    void translate();
    // properties
    int m_count = 0;
    QChar m_fillChar = QChar();
    bool m_forceWidth = false;
    char m_format = 'f';
    double m_multiplier = 1.0;
    int m_precision = -1;
    double m_summand = 0.0;
};


#endif /* AWFLOATFORMATTER_H */
