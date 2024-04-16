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


AWAbstractSelector::AWAbstractSelector(QWidget *_parent, const QPair<bool, bool> &_editable)
    : QWidget(_parent)
    , ui(new Ui::AWAbstractSelector)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    ui->setupUi(this);
    ui->comboBox_key->setEditable(_editable.first);
    ui->comboBox_value->setEditable(_editable.second);

    connect(ui->comboBox_key, &QComboBox::currentIndexChanged, this, &AWAbstractSelector::selectionChanged);
    connect(ui->comboBox_value, &QComboBox::currentIndexChanged, this, &AWAbstractSelector::selectionChanged);
}


AWAbstractSelector::~AWAbstractSelector()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete ui;
}


QPair<QString, QString> AWAbstractSelector::current() const
{
    auto key = ui->comboBox_key->currentText();
    auto value = ui->comboBox_value->currentText();

    return {key, value};
}


void AWAbstractSelector::init(const QStringList &_keys, const QStringList &_values,
                              const QPair<QString, QString> &_current)
{
    if ((!_keys.contains(_current.first)) || (!_values.contains(_current.second))) {
        qCWarning(LOG_AW) << "Invalid current value" << _current << "not found in default ones";
        return;
    }
    qCDebug(LOG_AW) << "Init selector with keys" << _keys << "and values" << _values << "and current ones are"
                    << _current;

    // set data
    ui->comboBox_key->clear();
    ui->comboBox_key->addItems(_keys);
    ui->comboBox_value->clear();
    ui->comboBox_value->addItems(_values);

    // set current values
    ui->comboBox_key->setCurrentText(_current.first);
    ui->comboBox_value->setCurrentText(_current.second);
}
