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


#include "awlistformatter.h"
#include "ui_awlistformatter.h"

#include <KI18n/KLocalizedString>

#include <QSettings>

#include "awdebug.h"


AWListFormatter::AWListFormatter(QWidget *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
    , ui(new Ui::AWListFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();
}


AWListFormatter::~AWListFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWListFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    QStringList output = _value.toStringList();
    if (isSorted())
        output.sort();

    return output.filter(m_regex).join(separator());
}


AWListFormatter *AWListFormatter::copy(const QString &_fileName,
                                       const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWListFormatter *item
        = new AWListFormatter(static_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setFilter(filter());
    item->setSeparator(separator());
    item->setSorted(isSorted());
    item->setNumber(_number);

    return item;
}


QString AWListFormatter::filter() const
{
    return m_filter;
}


bool AWListFormatter::isSorted() const
{
    return m_sorted;
}


QString AWListFormatter::separator() const
{
    return m_separator;
}


void AWListFormatter::setFilter(const QString &_filter)
{
    qCDebug(LOG_LIB) << "Filter" << _filter;

    m_filter = _filter;
    m_regex = QRegExp(m_filter);
}


void AWListFormatter::setSeparator(const QString &_separator)
{
    qCDebug(LOG_LIB) << "Separtor" << _separator;

    m_separator = _separator;
}


void AWListFormatter::setSorted(const bool _sorted)
{
    qCDebug(LOG_LIB) << "Sorting" << _sorted;

    m_sorted = _sorted;
}


void AWListFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setFilter(settings.value("X-AW-Filter", filter()).toString());
    setSeparator(settings.value("X-AW-Separator", separator()).toString());
    setSorted(settings.value("X-AW-Sort", isSorted()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWListFormatter::showConfiguration(const QVariant &_args)
{
    Q_UNUSED(_args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("List");
    ui->lineEdit_filter->setText(filter());
    ui->lineEdit_separator->setText(separator());
    ui->checkBox_sorted->setCheckState(isSorted() ? Qt::Checked
                                                  : Qt::Unchecked);

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
    setFilter(ui->lineEdit_filter->text());
    setSeparator(ui->lineEdit_separator->text());
    setSorted(ui->checkBox_sorted->checkState() == Qt::Checked);

    writeConfiguration();
    return ret;
}


void AWListFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Filter", filter());
    settings.setValue("X-AW-Separator", separator());
    settings.setValue("X-AW-Sort", isSorted());
    settings.endGroup();

    settings.sync();
}


void AWListFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->label_filter->setText(i18n("Filter"));
    ui->label_separator->setText(i18n("Separator"));
    ui->checkBox_sorted->setText(i18n("Sort"));
}
