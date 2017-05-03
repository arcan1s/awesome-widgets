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

#include "awabstractselector.h"
#include "ui_awabstractselector.h"

#include "awdebug.h"


AWAbstractSelector::AWAbstractSelector(QWidget *parent,
                                       const QPair<bool, bool> editable)
    : QWidget(parent)
    , ui(new Ui::AWAbstractSelector)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    ui->setupUi(this);
    ui->comboBox_key->setEditable(editable.first);
    ui->comboBox_value->setEditable(editable.second);

    connect(ui->comboBox_key, SIGNAL(currentIndexChanged(int)), this,
            SIGNAL(selectionChanged()));
    connect(ui->comboBox_value, SIGNAL(currentIndexChanged(int)), this,
            SIGNAL(selectionChanged()));
}


AWAbstractSelector::~AWAbstractSelector()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete ui;
}


QPair<QString, QString> AWAbstractSelector::current() const
{
    QString key = ui->comboBox_key->currentText();
    QString value = ui->comboBox_value->currentText();

    return QPair<QString, QString>(key, value);
}


void AWAbstractSelector::init(const QStringList keys, const QStringList values,
                              const QPair<QString, QString> current)
{
    if ((!keys.contains(current.first)) || (!values.contains(current.second))) {
        qCWarning(LOG_AW) << "Invalid current value" << current
                          << "not found in default ones";
        return;
    }
    qCDebug(LOG_AW) << "Init selector with keys" << keys << "and values"
                    << values << "and current ones are" << current;

    // set data
    ui->comboBox_key->clear();
    ui->comboBox_key->addItems(keys);
    ui->comboBox_value->clear();
    ui->comboBox_value->addItems(values);

    // set current values
    ui->comboBox_key->setCurrentText(current.first);
    ui->comboBox_value->setCurrentText(current.second);
}
