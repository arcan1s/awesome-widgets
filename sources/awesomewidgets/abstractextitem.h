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

#ifndef ABSTRACTEXTITEM_H
#define ABSTRACTEXTITEM_H

#include <QDialog>
#include <QVariant>


class QCronScheduler;
class QLocalServer;

class AbstractExtItem : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QString cron READ cron WRITE setCron)
    Q_PROPERTY(QString fileName READ fileName)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString socket READ socket WRITE setSocket)
    Q_PROPERTY(QString uniq READ uniq)

public:
    explicit AbstractExtItem(QWidget *_parent = nullptr, const QString &_filePath = "");
    ~AbstractExtItem() override;
    virtual void bumpApi(const int _newVer);
    virtual AbstractExtItem *copy(const QString &_fileName, const int _number) = 0;
    virtual void copyDefaults(AbstractExtItem *_other) const;
    virtual void startTimer();
    QString writtableConfig() const;
    // get methods
    int apiVersion() const;
    QString comment() const;
    QString cron() const;
    QString fileName() const;
    int interval() const;
    bool isActive() const;
    QString name() const;
    int number() const;
    QString socket() const;
    QString tag(const QString &_type) const;
    virtual QString uniq() const = 0;
    // set methods
    void setApiVersion(const int _apiVersion);
    void setActive(const bool _state);
    void setComment(const QString &_comment);
    void setCron(const QString &_cron);
    void setInterval(const int _interval);
    void setName(const QString &_name);
    void setNumber(int _number);
    void setSocket(const QString &_socket);

signals:
    void dataReceived(const QVariantHash &_data);
    void requestDataUpdate();

public slots:
    virtual void deinitSocket();
    virtual void initSocket();
    virtual void readConfiguration();
    virtual QVariantHash run() = 0;
    virtual int showConfiguration(const QVariant &_args) = 0;
    virtual bool tryDelete() const;
    virtual void writeConfiguration() const;

private slots:
    void newConnectionReceived();

private:
    QCronScheduler *m_scheduler = nullptr;
    QString m_fileName = "/dev/null";
    int m_times = 0;
    virtual void translate() = 0;
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = "empty";
    QString m_cron = "";
    int m_interval = 1;
    QString m_name = "none";
    int m_number = -1;
    QLocalServer *m_socket = nullptr;
    QString m_socketFile = "";
};


#endif /* ABSTRACTEXTITEM_H */
