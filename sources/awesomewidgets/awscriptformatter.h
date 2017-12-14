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

#ifndef AWSCRIPTFORMATTER_H
#define AWSCRIPTFORMATTER_H

#include "awabstractformatter.h"


namespace Ui
{
class AWScriptFormatter;
}

class AWScriptFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(bool appendCode READ appendCode WRITE setAppendCode)
    Q_PROPERTY(QString code READ code WRITE setCode)
    Q_PROPERTY(bool hasReturn READ hasReturn WRITE setHasReturn)
    Q_PROPERTY(QString program READ program)

public:
    explicit AWScriptFormatter(QWidget *_parent = nullptr, const QString &_filePath = "");
    virtual ~AWScriptFormatter();
    QString convert(const QVariant &_value) const;
    AWScriptFormatter *copy(const QString &_fileName, const int _number);
    // properties
    bool appendCode() const;
    QString code() const;
    bool hasReturn() const;
    QString program() const;
    void setAppendCode(const bool _appendCode);
    void setCode(const QString &_code);
    void setHasReturn(const bool _hasReturn);

public slots:
    void readConfiguration();
    int showConfiguration(const QVariant &_args);
    void writeConfiguration() const;

private:
    Ui::AWScriptFormatter *ui = nullptr;
    void initProgram();
    void translate();
    // properties
    bool m_appendCode = true;
    QString m_code = "";
    bool m_hasReturn = false;
    QString m_program;
};


#endif /* AWSCRIPTFORMATTER_H */
