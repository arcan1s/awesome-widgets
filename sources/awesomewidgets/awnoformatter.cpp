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

#include "awnoformatter.h"
#include "ui_awnoformatter.h"

#include "awdebug.h"


AWNoFormatter::AWNoFormatter(QObject *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWNoFormatter::readConfiguration();
}


QString AWNoFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    return _value.toString();
}


AWNoFormatter *AWNoFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new AWNoFormatter(parent(), _fileName);
    AWAbstractFormatter::copyDefaults(item);

    item->setNumber(_number);

    return item;
}


int AWNoFormatter::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::AWNoFormatter();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("NoFormat");

    auto ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setApiVersion(AW_FORMATTER_API);
        setStrType(ui->label_typeValue->text());

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}
