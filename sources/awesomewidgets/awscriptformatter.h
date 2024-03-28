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

#include "awabstractformatter.h"


class AWScriptFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(bool appendCode READ appendCode WRITE setAppendCode)
    Q_PROPERTY(QString code READ code WRITE setCode)
    Q_PROPERTY(bool hasReturn READ hasReturn WRITE setHasReturn)
    Q_PROPERTY(QString program READ program)

public:
    explicit AWScriptFormatter(QObject *_parent = nullptr, const QString &_filePath = "");
    [[nodiscard]] QString convert(const QVariant &_value) const override;
    AWScriptFormatter *copy(const QString &_fileName, int _number) override;
    // properties
    [[nodiscard]] bool appendCode() const;
    [[nodiscard]] QString code() const;
    [[nodiscard]] bool hasReturn() const;
    [[nodiscard]] QString program() const;
    void setAppendCode(bool _appendCode);
    void setCode(const QString &_code);
    void setHasReturn(bool _hasReturn);

public slots:
    void readConfiguration() override;
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;
    void writeConfiguration() const override;

private:
    void initProgram();
    void translate(void *_ui) override;
    // properties
    bool m_appendCode = true;
    QString m_code = "";
    bool m_hasReturn = false;
    QString m_program;
};
