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

#include "fontdialog.h"

#include <QGridLayout>


CFont::CFont(const QString family, int pointSize, int weight, bool italic, QColor color)
    : QFont(family, pointSize, weight, italic)
{
    setCurrentColor(color);
}


QColor CFont::color()
{
    return currentColor;
}


void CFont::setCurrentColor(const QColor color)
{
    currentColor = color;
}


int CFont::html2QFont(const int htmlWeight)
{
    int weight = 16;
    switch(htmlWeight) {
    case 100:
        weight = 16;
        break;
    case 200:
    case 300:
        weight = 25;
        break;
    case 400:
        weight = 50;
        break;
    case 500:
    case 600:
        weight = 63;
        break;
    case 700:
    case 800:
        weight = 75;
        break;
    case 900:
        weight = 87;
        break;
    default:
        break;
    }

    return weight;
}


int CFont::qFont2html(const int weight)
{
    int htmlWeight = 400;
    switch(weight) {
    case 16:
        htmlWeight = 100;
        break;
    case 25:
        htmlWeight = 300;
        break;
    case 50:
        htmlWeight = 400;
        break;
    case 63:
        htmlWeight = 600;
        break;
    case 75:
        htmlWeight = 800;
        break;
    case 87:
        htmlWeight = 900;
        break;
    default:
        break;
    }

    return htmlWeight;
}


int CFont::htmlWeight()
{
    return CFont::qFont2html(weight());
}


void CFont::setHtmlWeight(const int htmlWeight)
{
    setWeight(CFont::html2QFont(htmlWeight));
}


CFont CFont::fromQFont(const QFont font, const QColor color)
{
    return CFont(font.family(), font.pointSize(), font.weight(), font.italic(), color);
}


QFont CFont::toQFont()
{
    return QFont(family(), pointSize(), weight(), italic());
}


CFontDialog::CFontDialog(QWidget *parent, bool needWeight, bool needItalic)
    : QDialog(parent)
{
    QGridLayout *mainGrid = new QGridLayout(this);
    setLayout(mainGrid);

    colorBox = new QComboBox(this);
    connect(colorBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateColor(QString)));
    QStringList colorNames = QColor::colorNames();
    int index = 0;
    for (int i=0; i<colorNames.count(); i++) {
        QColor color(colorNames[i]);
        colorBox->addItem(colorNames[i], color);
        QModelIndex idx = colorBox->model()->index(index++, 0);
        colorBox->model()->setData(idx, color, Qt::BackgroundRole);
    }
    mainGrid->addWidget(colorBox, 0, 0);
    fontBox = new QFontComboBox(this);
    mainGrid->addWidget(fontBox, 0, 1);
    sizeBox = new QSpinBox(this);
    mainGrid->addWidget(sizeBox, 0, 2);
    weightBox = new QSpinBox(this);
    mainGrid->addWidget(weightBox, 0, 3);
    italicBox = new QComboBox(this);
    italicBox->addItem(tr("normal"));
    italicBox->addItem(tr("italic"));
    mainGrid->addWidget(italicBox, 0, 4);

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, this);
    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    mainGrid->addWidget(buttons, 1, 0, 1, 5);

    italicBox->setHidden(!needItalic);
    weightBox->setHidden(!needWeight);
}


CFontDialog::~CFontDialog()
{
    delete colorBox;
    delete buttons;
    delete fontBox;
    delete italicBox;
    delete sizeBox;
    delete weightBox;
}


void CFontDialog::updateColor(const QString color)
{
    colorBox->setStyleSheet(QString("background:%1").arg(QColor(color).name()));
}


CFont CFontDialog::getFont(const QString title, CFont defaultFont, bool needWeight, bool needItalic, int *status)
{
    CFontDialog dlg(0, needWeight, needItalic);

    dlg.setWindowTitle(title);
    QStringList colorNames = QColor::colorNames();
    for (int i=0; i<colorNames.count(); i++)
        if (QColor(colorNames[i]) == defaultFont.color()) {
            dlg.colorBox->setCurrentIndex(i);
            break;
        }
    dlg.fontBox->setCurrentFont(defaultFont);
    dlg.sizeBox->setValue(defaultFont.pointSize());
    dlg.weightBox->setValue(defaultFont.weight());
    if (defaultFont.italic())
        dlg.italicBox->setCurrentIndex(1);
    else
        dlg.italicBox->setCurrentIndex(0);

    CFont font = CFont(defaultFont);
    int ret = dlg.exec();
    if (ret == 1)
        font = CFont(dlg.fontBox->currentFont().family(),
                     dlg.sizeBox->value(),
                     dlg.weightBox->value(),
                     dlg.italicBox->currentIndex() == 1,
                     QColor(dlg.colorBox->currentText()));
    if (status != nullptr)
        *status = ret;
    return font;
}
