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


class AWNoFormatter : public AWAbstractFormatter
{
    Q_OBJECT

public:
    explicit AWNoFormatter(QObject *_parent = nullptr, const QString &_filePath = "");
    [[nodiscard]] QString convert(const QVariant &_value) const override;
    AWNoFormatter *copy(const QString &_fileName, int _number) override;

public slots:
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;

private:
    void translate(void *_ui) override;
    // properties
};
