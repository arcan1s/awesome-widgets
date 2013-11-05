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

#include "extsysmon.h"

#include <Plasma/DataContainer>

#include <stdio.h>


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList& args)
  : Plasma::DataEngine(parent, args)
{
  Q_UNUSED(args)

  setMinimumPollingInterval(333);

  FILE *f_out;
  f_out = popen("lspci 2>&1", "r");
  char device[256];
  QString dev;
  while (fgets(device, 256, f_out) != NULL)
  {
    dev = QString(device);
    if (dev.toLower().contains("nvidia"))
      gpudev = QString("nvidia");
    else if (dev.toLower().contains("radeon"))
      gpudev = QString("ati");
  }
  pclose(f_out);

  f_out = popen("ls -1 /dev/sd[a-z] && ls -1 /dev/hd[a-z] 2>&1", "r");
  while (fgets(device, 256, f_out) != NULL)
  {
    dev = QString(device);
    if (dev[0] == '/')
      hdddev.append(dev);
  }
  pclose(f_out);
}

QStringList ExtendedSysMon::sources() const
{
  QStringList source;
  source.append(QString("gpu"));
  source.append(QString("gputemp"));
  source.append(QString("hddtemp"));
  source.append(QString("player"));
  return source;
}

bool ExtendedSysMon::sourceRequestEvent(const QString &name)
{
  return updateSourceEvent(name);
}

bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
  FILE *f_out;
  QString key, out, tmp_out, value;
  bool ok = false;
  char output[256], val[5];
  if (source == QString("gpu"))
  {
    key = QString("GPU");
    if (gpudev == QString("nvidia"))
    {
      f_out = popen("nvidia-smi -q -d UTILIZATION 2> /dev/null | grep Gpu | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 2))
        value = QString("  N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[2];
        sprintf (val, "%5.1f", out.left(out.count()-2).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else if (gpudev == QString("ati"))
    {
      f_out = popen("aticonfig --od-getclocks 2> /dev/null | grep load | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 3))
        value = QString("  N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[3];
        sprintf (val, "%5.1f", out.left(out.count()-2).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else
    {
      value = QString("  N\\A");
    }
    if (ok == false)
      value = QString("  N\\A");
    value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
    setData(source, key, value);
  }
  else if (source == QString("gputemp"))
  {
    key = QString("GPUTemp");
    if (gpudev == QString("nvidia"))
    {
      f_out = popen("nvidia-smi -q -d TEMPERATURE 2> /dev/null | grep Gpu | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 2))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[2];
        sprintf (val, "%4.1f", out.toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else if (gpudev == QString("ati"))
    {
      f_out = popen("aticonfig --od-gettemperature 2> /dev/null | grep Temperature | tail -n1", "r");
      fgets (output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(" "), QString::SkipEmptyParts).count() < 4))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(" "), QString::SkipEmptyParts)[4];
        sprintf (val, "%4.1f", out.toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
    }
    else
    {
      value = QString(" N\\A");
    }
    if (ok == false)
      value = QString(" N\\A");
    value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
    setData(source, key, value);
  }
  else if (source == QString("hddtemp"))
  {
    char command[256], *dev;
    QByteArray qb;
    for (int i=0; i<hdddev.count(); i++)
    {
      qb = hdddev[i].toUtf8();
      dev = qb.data();
      sprintf(command, "hddtemp %s 2> /dev/null", dev);
      f_out = popen(command, "r");
      fgets(output, 256, f_out);
      if ((output[0] == '\0') ||
          (QString(output).split(QString(":"), QString::SkipEmptyParts).count() < 3))
        value = QString(" N\\A");
      else
      {
        out = QString(output).split(QString(":"), QString::SkipEmptyParts)[2];
        sprintf (val, "%4.1f", out.left(out.count()-4).toFloat(&ok));
        value = QString(val);
      }
      pclose(f_out);
      if (ok == false)
        value = QString(" N\\A");
      value = value.split(QString(","), QString::SkipEmptyParts).join(QString("."));
      setData(source, hdddev[i], value);
    }
  }
  else if (source == QString("player"))
  {
    // qmmp
    output[0] = '\0';
    key = QString("qmmp_artist");
    f_out = popen("qmmp --nowplaying '%if(%p,%p,Unknown)' 2> /dev/null", "r");
    fgets(output, 256, f_out);
    if (output[0] == '\0')
      value = QString("N\\A");
    else
      value = QString(output).split(QString("\n"), QString::SkipEmptyParts)[0];
    pclose(f_out);
    setData(source, key, value);
    output[0] = '\0';
    key = QString("qmmp_title");
    f_out = popen("qmmp --nowplaying '%if(%t,%t,Unknown)' 2> /dev/null", "r");
    fgets(output, 256, f_out);
    if (output[0] == '\0')
      value = QString("N\\A");
    else
      value = QString(output).split(QString("\n"), QString::SkipEmptyParts)[0];
    pclose(f_out);
    setData(source, key, value);
    // amarok
    output[0] = '\0';
    key = QString("amarok_artist");
    f_out = popen("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep albumartist: | cut -c14-", "r");
    fgets(output, 256, f_out);
    if (output[0] == '\0')
      value = QString("N\\A");
    else
      value = QString(output).split(QString("\n"), QString::SkipEmptyParts)[0];
    pclose(f_out);
    setData(source, key, value);
    output[0] = '\0';
    key = QString("amarok_title");
    f_out = popen("qdbus org.kde.amarok /Player GetMetadata 2> /dev/null | grep title: | cut -c8-", "r");
    fgets(output, 256, f_out);
    if (output[0] == '\0')
      value = QString("N\\A");
    else
      value = QString(output).split(QString("\n"), QString::SkipEmptyParts)[0];
    pclose(f_out);
    setData(source, key, value);
    // mpd
    QString value_artist;
    value = QString("N\\A");
    value_artist = QString("N\\A");
    f_out = popen("echo 'currentsong\nclose' | curl --connect-timeout 1 -fsm 3 telnet://localhost:6600 2> /dev/null", "r");
    while (true)
    {
      fgets(output, 256, f_out);
      if (feof (f_out))
        break;
      if (QString(output).split(QString(": "), QString::SkipEmptyParts)[0] == QString("Artist"))
        value_artist = QString(output).split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
      else if (QString(output).split(QString(": "), QString::SkipEmptyParts)[0] == QString("Title"))
        value = QString(output).split(QString(": "), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
    }
    pclose(f_out);
    key = QString("mpd_artist");
    setData(source, key, value_artist);
    key = QString("mpd_title");
    setData(source, key, value);
  }

  return true;
}

K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc"
