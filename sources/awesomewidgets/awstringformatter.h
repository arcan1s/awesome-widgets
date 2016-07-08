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

#ifndef AWSTRINGFORMATTER_H
#define AWSTRINGFORMATTER_H

#include "awabstractformatter.h"


namespace Ui
{
class AWStringFormatter;
}

class AWStringFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(QChar fillChar READ fillChar WRITE setFillChar)
    Q_PROPERTY(bool forceWidth READ forceWidth WRITE setForceWidth)

public:
    explicit AWStringFormatter(QWidget *parent,
                               const QString filePath = QString());
    virtual ~AWStringFormatter();
    QString convert(const QVariant &_value) const;
    AWStringFormatter *copy(const QString _fileName, const int _number);
    // properties
    int count() const;
    QChar fillChar() const;
    bool forceWidth() const;
    void setCount(const int _count);
    void setFillChar(const QChar _fillChar);
    void setForceWidth(const bool _forceWidth);

public slots:
    void readConfiguration();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private:
    Ui::AWStringFormatter *ui = nullptr;
    void translate();
    // properties
    int m_count = 0;
    QChar m_fillChar = QChar();
    bool m_forceWidth = false;
};


#endif /* AWSTRINGFORMATTER_H */
