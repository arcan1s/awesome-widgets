/***************************************************************************
 *  Copyright (C) 2014  Evgeniy Alekseev                                      *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 3.0 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library.                                       *
 ***************************************************************************/

#pragma once

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QSpinBox>


class CFont : public QFont
{
public:
    explicit CFont(const QString family, int pointSize = -1,
                   int weight = -1, bool italic = false,
                   QColor color = QColor(QString("#000000")));
    // color properties
    QColor color();
    void setCurrentColor(const QColor color);
    // conversion to QFont
    static CFont fromQFont(const QFont font,
                           const QColor color = QColor(QString("#000000")));
    QFont toQFont();

private:
    QColor currentColor;
};


class CFontDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFontDialog(QWidget *parent = 0,
                         const bool needWeight = true,
                         const bool needItalic = true);
    ~CFontDialog();
    static CFont getFont(const QString title = tr("Select font"),
                         CFont defaultFont = CFont(QString("Arial"),
                                                   12, 400, false,
                                                   QColor(QString("#000000"))),
                         const bool needWeight = true,
                         const bool needItalic = true,
                         int *status = nullptr);

private slots:
    void updateColor(const QString color);

private:
    QComboBox *colorBox;
    QDialogButtonBox *buttons;
    QFontComboBox *fontBox;
    QComboBox *italicBox;
    QSpinBox *sizeBox;
    QSpinBox *weightBox;
};
