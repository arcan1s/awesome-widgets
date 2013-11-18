/***************************************************************************
 *   Copyright (C) 2013 by Evgeniy Alekseev <esalekseev@gmail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef EXTSYSMON_H
#define EXTSYSMON_H

#include <Plasma/DataEngine>

class ExtendedSysMon : public Plasma::DataEngine
{
  Q_OBJECT

public:
  ExtendedSysMon(QObject *parent, const QVariantList &args);

protected:
  bool sourceRequestEvent(const QString &name);
  bool updateSourceEvent(const QString &source);
  bool readConfiguration(const QString confFileName);
  QStringList sources() const;
// main configuration
  QStringList hdddev;
  QString gpudev;
// configuration
  QString mpdAddress;
  QString mpdPort;
};

#endif // EXTSYSMON_H 
