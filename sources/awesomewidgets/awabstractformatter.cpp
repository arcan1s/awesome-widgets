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

#include "awabstractformatter.h"

#include <QSettings>

#include "awdebug.h"


AWAbstractFormatter::AWAbstractFormatter(QWidget *parent,
                                         const QString filePath)
    : AbstractExtItem(parent, filePath)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWAbstractFormatter::~AWAbstractFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWAbstractFormatter::copyDefaults(AbstractExtItem *_other) const
{
    AbstractExtItem::copyDefaults(_other);

    static_cast<AWAbstractFormatter *>(_other)->setType(m_type);
}


QString AWAbstractFormatter::uniq() const
{
    return QString("%1(%2)").arg(name()).arg(m_type);
}


QString AWAbstractFormatter::type() const
{
    return m_type;
}


void AWAbstractFormatter::setType(const QString _type)
{
    qCDebug(LOG_AW) << "Type" << _type;

    m_type = _type;
}


void AWAbstractFormatter::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setType(settings.value(QString("Type"), m_type).toString());
    settings.endGroup();
}


void AWAbstractFormatter::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Type"), m_type);
    settings.endGroup();

    settings.sync();
}
