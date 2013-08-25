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
  QStringList hdddev;
  QString gpudev;
  QStringList sources() const;
};

#endif // EXTSYSMON_H 
