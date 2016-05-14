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


#include "awfloatformatter.h"
#include "ui_awfloatformatter.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QSettings>

#include "awdebug.h"


AWFloatFormatter::AWFloatFormatter(QWidget *parent, const QString filePath)
    : AWAbstractFormatter(parent, filePath)
    , ui(new Ui::AWFloatFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    readConfiguration();
    ui->setupUi(this);
    translate();
}


AWFloatFormatter::AWFloatFormatter(const int count, const QChar fillChar,
                                   const char format, const double multiplier,
                                   const int precision, const double summand,
                                   QWidget *parent)
    : AWAbstractFormatter(parent)
    , ui(new Ui::AWFloatFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    setCount(count);
    setFillChar(fillChar);
    setFormat(format);
    setMultiplier(multiplier);
    setPrecision(precision);
    setSummand(summand);

    ui->setupUi(this);
    translate();
}


AWFloatFormatter::~AWFloatFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWFloatFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    return QString("%1").arg(_value.toDouble() * m_multiplier + m_summand,
                             m_count, m_format, m_precision, m_fillChar);
}


AWFloatFormatter *AWFloatFormatter::copy(const QString _fileName,
                                         const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWFloatFormatter *item
        = new AWFloatFormatter(static_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setCount(count());
    item->setFormat(format());
    item->setFillChar(fillChar());
    item->setMultiplier(multiplier());
    item->setNumber(_number);
    item->setPrecision(precision());
    item->setSummand(summand());

    return item;
}


int AWFloatFormatter::count() const
{
    return m_count;
}


QChar AWFloatFormatter::fillChar() const
{
    return m_fillChar;
}


char AWFloatFormatter::format() const
{
    return m_format;
}


double AWFloatFormatter::multiplier() const
{
    return m_multiplier;
}


int AWFloatFormatter::precision() const
{
    return m_precision;
}


double AWFloatFormatter::summand() const
{
    return m_summand;
}


void AWFloatFormatter::setCount(const int _count)
{
    qCDebug(LOG_LIB) << "Set width" << _count;

    m_count = _count;
}


void AWFloatFormatter::setFillChar(const QChar _fillChar)
{
    qCDebug(LOG_LIB) << "Set char" << _fillChar;

    m_fillChar = _fillChar;
}


void AWFloatFormatter::setFormat(char _format)
{
    qCDebug(LOG_LIB) << "Set format" << _format;
    // http://doc.qt.io/qt-5/qstring.html#argument-formats
    if ((_format != 'e') && (_format != 'E') && (_format != 'f')
        && (_format != 'g') && (_format != 'G')) {
        qCWarning(LOG_LIB) << "Invalid format" << _format;
        _format = 'f';
    }

    m_format = _format;
}


void AWFloatFormatter::setMultiplier(const double _multiplier)
{
    qCDebug(LOG_LIB) << "Set multiplier" << _multiplier;

    m_multiplier = _multiplier;
}


void AWFloatFormatter::setPrecision(const int _precision)
{
    qCDebug(LOG_LIB) << "Set precision" << _precision;

    m_precision = _precision;
}


void AWFloatFormatter::setSummand(const double _summand)
{
    qCDebug(LOG_LIB) << "Set summand" << _summand;

    m_summand = _summand;
}


void AWFloatFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setCount(settings.value(QString("X-AW-Width"), m_count).toInt());
    setFillChar(
        settings.value(QString("X-AW-FillChar"), m_fillChar).toString().at(0));
    setFormat(settings.value(QString("X-AW-Format"), QString(m_format))
                  .toString()
                  .at(0)
                  .toLatin1());
    setMultiplier(
        settings.value(QString("X-AW-Multiplier"), m_multiplier).toDouble());
    setPrecision(
        settings.value(QString("X-AW-Precision"), m_precision).toInt());
    setSummand(settings.value(QString("X-AW-Summand"), m_summand).toDouble());
    settings.endGroup();

    bumpApi(AWEFAPI);
}


int AWFloatFormatter::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText(QString("Float"));
    ui->comboBox_format->setCurrentIndex(
        ui->comboBox_format->findText(QString(m_format)));
    ui->spinBox_precision->setValue(m_precision);
    ui->spinBox_width->setValue(m_count);
    ui->lineEdit_fill->setText(QString(m_fillChar));
    ui->doubleSpinBox_multiplier->setValue(m_multiplier);
    ui->doubleSpinBox_summand->setValue(m_summand);

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setType(ui->label_typeValue->text());
    setFormat(ui->comboBox_format->currentText().at(0).toLatin1());
    setPrecision(ui->spinBox_precision->value());
    setCount(ui->spinBox_width->value());
    setFillChar(ui->lineEdit_fill->text().at(0));
    setMultiplier(ui->doubleSpinBox_multiplier->value());
    setSummand(ui->doubleSpinBox_summand->value());

    writeConfiguration();
    return ret;
}


void AWFloatFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Width"), m_count);
    settings.setValue(QString("X-AW-FillChar"), m_fillChar);
    settings.setValue(QString("X-AW-Format"), m_format);
    settings.setValue(QString("X-AW-Multiplier"), m_multiplier);
    settings.setValue(QString("X-AW-Precision"), m_precision);
    settings.setValue(QString("X-AW-Summand"), m_summand);
    settings.endGroup();

    settings.sync();
}


void AWFloatFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->label_format->setText(i18n("Format"));
    ui->label_precision->setText(i18n("Precision"));
    ui->label_width->setText(i18n("Width"));
    ui->label_fill->setText(i18n("Fill char"));
    ui->label_multiplier->setText(i18n("Multiplier"));
    ui->label_summand->setText(i18n("Summand"));
}
