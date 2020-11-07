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

#ifndef AWABSTRACTFORMATTER_H
#define AWABSTRACTFORMATTER_H

#include "abstractextitem.h"


class AWAbstractFormatter : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(FormatterClass type READ type WRITE setType)
    Q_PROPERTY(QString strType READ strType WRITE setStrType)

public:
    enum class FormatterClass { DateTime, Float, List, Script, String, NoFormat, Json };

    explicit AWAbstractFormatter(QWidget *_parent = nullptr, const QString &_filePath = "");
    ~AWAbstractFormatter() override;
    [[nodiscard]] virtual QString convert(const QVariant &_value) const = 0;
    void copyDefaults(AbstractExtItem *_other) const override;
    [[nodiscard]] QString uniq() const override;
    // properties
    [[nodiscard]] QString strType() const;
    [[nodiscard]] FormatterClass type() const;
    void setStrType(const QString &_type);
    void setType(FormatterClass _type);

public slots:
    void readConfiguration() override;
    QVariantHash run() override { return QVariantHash(); };
    void writeConfiguration() const override;

private:
    // properties
    FormatterClass m_type = FormatterClass::NoFormat;
};


#endif /* AWABSTRACTFORMATTER_H */
