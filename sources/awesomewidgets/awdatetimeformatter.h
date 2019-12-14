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

#ifndef AWDATETIMEFORMATTER_H
#define AWDATETIMEFORMATTER_H

#include <QLocale>

#include "awabstractformatter.h"


namespace Ui
{
class AWDateTimeFormatter;
}

class AWDateTimeFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString format READ format WRITE setFormat)
    Q_PROPERTY(bool translateString READ translateString WRITE setTranslateString)

public:
    explicit AWDateTimeFormatter(QWidget *_parent = nullptr, const QString &_filePath = "");
    ~AWDateTimeFormatter() override;
    QString convert(const QVariant &_value) const override;
    AWDateTimeFormatter *copy(const QString &_fileName, const int _number) override;
    // properties
    QString format() const;
    bool translateString() const;
    void setFormat(const QString &_format);
    void setTranslateString(const bool _translate);

public slots:
    void readConfiguration() override;
    int showConfiguration(const QVariant &_args) override;
    void writeConfiguration() const override;

private:
    Ui::AWDateTimeFormatter *ui = nullptr;
    void initLocale();
    void translate() override;
    // properties
    QLocale m_locale;
    QString m_format = "";
    bool m_translate = true;
};


#endif /* AWDATETIMEFORMATTER_H */
