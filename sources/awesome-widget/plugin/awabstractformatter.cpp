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

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


AWAbstractFormatter::AWAbstractFormatter(QWidget *parent,
                                         const QString filePath)
    : QDialog(parent)
    , m_fileName(filePath)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    qCDebug(LOG_AW) << "Desktop name" << filePath;

    m_name = m_fileName;
}


AWAbstractFormatter::~AWAbstractFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWAbstractFormatter::copyDefaults(AWAbstractFormatter *_other) const
{
    _other->setComment(m_comment);
    _other->setName(m_name);
    _other->setType(m_type);
}


QString AWAbstractFormatter::writtableConfig() const
{
    QStringList paths = m_fileName.split(QChar('/'));

    QString name = paths.takeLast();
    QString dir = paths.takeLast();

    return QString("%1/awesomewidgets/%2/%3")
        .arg(QStandardPaths::writableLocation(
            QStandardPaths::GenericDataLocation))
        .arg(dir)
        .arg(name);
}


QString AWAbstractFormatter::comment() const
{
    return m_comment;
}


QString AWAbstractFormatter::fileName() const
{
    return m_fileName;
}


QString AWAbstractFormatter::name() const
{
    return m_name;
}


QString AWAbstractFormatter::type() const
{
    return m_type;
}


void AWAbstractFormatter::setComment(const QString _comment)
{
    qCDebug(LOG_AW) << "Comment" << _comment;

    m_comment = _comment;
}


void AWAbstractFormatter::setName(const QString _name)
{
    qCDebug(LOG_AW) << "Name" << _name;

    m_name = _name;
}


void AWAbstractFormatter::setType(const QString _type)
{
    qCDebug(LOG_AW) << "Type" << _type;

    m_type = _type;
}


void AWAbstractFormatter::readConfiguration()
{
    QSettings settings(m_fileName, QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setName(settings.value(QString("Name"), m_name).toString());
    setComment(settings.value(QString("Comment"), m_comment).toString());
    setType(settings.value(QString("Type"), m_type).toString());
    settings.endGroup();
}


bool AWAbstractFormatter::tryDelete() const
{
    bool status = QFile::remove(m_fileName);
    qCInfo(LOG_AW) << "Remove file" << m_fileName << status;

    return status;
}


void AWAbstractFormatter::writeConfiguration() const
{
    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("Type"), m_type);
    settings.endGroup();

    settings.sync();
}
