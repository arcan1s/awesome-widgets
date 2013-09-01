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
      f_out = popen("nvidia-smi -q -d UTILIZATION | grep Gpu | tail -n1 2>&1", "r");
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
      f_out = popen("aticonfig --od-getclocks | grep load | tail -n1 2>&1", "r");
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
      f_out = popen("nvidia-smi -q -d TEMPERATURE | grep Gpu | tail -n1 2>&1", "r");
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
      f_out = popen("aticonfig --od-gettemperature | grep Temperature | tail -n1 2>&1", "r");
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
      sprintf(command, "hddtemp %s", dev);
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

  return true;
}

K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc" 
