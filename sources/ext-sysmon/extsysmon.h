/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
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
  bool readConfiguration();
  QStringList sources() const;
// main configuration
  QStringList hddDev;
  QString gpuDev;
// configuration
  QString mpdAddress;
  QString mpdPort;
};

#endif /* EXTSYSMON_H */
