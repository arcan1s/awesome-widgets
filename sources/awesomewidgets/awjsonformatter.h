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

#ifndef AWJSONFORMATTER_H
#define AWJSONFORMATTER_H

#include "awabstractformatter.h"


class AWJsonFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)

public:
    explicit AWJsonFormatter(QObject *_parent = nullptr, const QString &_filePath = "");
    [[nodiscard]] QString convert(const QVariant &_value) const override;
    AWJsonFormatter *copy(const QString &_fileName, int _number) override;
    // properties
    [[nodiscard]] QString path() const;
    void setPath(const QString &_path);

public slots:
    void readConfiguration() override;
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;
    void writeConfiguration() const override;

private:
    static QVariant getFromJson(const QVariant &_value, const QVariant &_element);
    static QVariant getFromList(const QVariant &_value, int _index);
    static QVariant getFromMap(const QVariant &_value, const QString &_key);
    void initPath();
    void translate(void *_ui) override;
    // properties
    QString m_path;
    QVariantList m_splittedPath;
};


#endif /* AWJSONFORMATTER_H */
