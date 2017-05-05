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


AWFloatFormatter::AWFloatFormatter(QWidget *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
    , ui(new Ui::AWFloatFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        readConfiguration();
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

    QString output
        = QString("%1").arg(_value.toDouble() * multiplier() + summand(),
                            count(), format(), precision(), fillChar());
    if (forceWidth())
        output = output.left(count());

    return output;
}


AWFloatFormatter *AWFloatFormatter::copy(const QString &_fileName,
                                         const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWFloatFormatter *item
        = new AWFloatFormatter(static_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setCount(count());
    item->setFormat(format());
    item->setFillChar(fillChar());
    item->setForceWidth(forceWidth());
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


bool AWFloatFormatter::forceWidth() const
{
    return m_forceWidth;
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


void AWFloatFormatter::setFillChar(const QChar &_fillChar)
{
    qCDebug(LOG_LIB) << "Set char" << _fillChar;

    m_fillChar = _fillChar;
}


void AWFloatFormatter::setForceWidth(const bool _forceWidth)
{
    qCDebug(LOG_LIB) << "Set force strip" << _forceWidth;

    m_forceWidth = _forceWidth;
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

    settings.beginGroup("Desktop Entry");
    setCount(settings.value("X-AW-Width", count()).toInt());
    setFillChar(settings.value("X-AW-FillChar", fillChar()).toString().at(0));
    setForceWidth(settings.value("X-AW-ForceWidth", forceWidth()).toBool());
    setFormat(settings.value("X-AW-Format", QString(format()))
                  .toString()
                  .at(0)
                  .toLatin1());
    setMultiplier(settings.value("X-AW-Multiplier", multiplier()).toDouble());
    setPrecision(settings.value("X-AW-Precision", precision()).toInt());
    setSummand(settings.value("X-AW-Summand", summand()).toDouble());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWFloatFormatter::showConfiguration(const QVariant &_args)
{
    Q_UNUSED(_args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("Float");
    ui->comboBox_format->setCurrentIndex(
        ui->comboBox_format->findText(QString(format())));
    ui->spinBox_precision->setValue(precision());
    ui->spinBox_width->setValue(count());
    ui->lineEdit_fill->setText(QString(fillChar()));
    ui->checkBox_forceWidth->setCheckState(forceWidth() ? Qt::Checked
                                                        : Qt::Unchecked);
    ui->doubleSpinBox_multiplier->setValue(multiplier());
    ui->doubleSpinBox_summand->setValue(summand());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
    setFormat(ui->comboBox_format->currentText().at(0).toLatin1());
    setPrecision(ui->spinBox_precision->value());
    setCount(ui->spinBox_width->value());
    setFillChar(ui->lineEdit_fill->text().at(0));
    setForceWidth(ui->checkBox_forceWidth->checkState() == Qt::Checked);
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

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Width", count());
    settings.setValue("X-AW-FillChar", fillChar());
    settings.setValue("X-AW-Format", QString(format()));
    settings.setValue("X-AW-ForceWidth", forceWidth());
    settings.setValue("X-AW-Multiplier", multiplier());
    settings.setValue("X-AW-Precision", precision());
    settings.setValue("X-AW-Summand", summand());
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
    ui->checkBox_forceWidth->setText(i18n("Force width"));
    ui->label_multiplier->setText(i18n("Multiplier"));
    ui->label_summand->setText(i18n("Summand"));
}
