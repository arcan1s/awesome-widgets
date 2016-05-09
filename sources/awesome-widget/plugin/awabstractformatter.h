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

#ifndef AWABSTRACTFORMATTER_H
#define AWABSTRACTFORMATTER_H

#include <QDialog>
#include <QVariant>


class AWAbstractFormatter : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QString fileName READ fileName)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString type READ type WRITE setType)

public:
    explicit AWAbstractFormatter(QWidget *parent = nullptr,
                                 const QString filePath = QString());
    virtual ~AWAbstractFormatter();
    virtual AWAbstractFormatter *copy(const QString _fileName) = 0;
    void copyDefaults(AWAbstractFormatter *_other) const;
    virtual QString convert(const QVariant &_value) const = 0;
    QString writtableConfig() const;
    // properties
    QString comment() const;
    QString fileName() const;
    QString name() const;
    QString type() const;
    void setComment(const QString _comment = QString("empty"));
    void setName(const QString _name = QString("none"));
    void setType(const QString _type = QString("NoFormat"));

public slots:
    virtual void readConfiguration();
    virtual int showConfiguration(const QVariant args = QVariant()) = 0;
    bool tryDelete() const;
    virtual void writeConfiguration() const;

private:
    QString m_fileName;
    virtual void translate() = 0;
    // properties
    QString m_comment = QString("empty");
    QString m_name = QString("none");
    QString m_type = QString("NoFormat");
};


#endif /* AWABSTRACTFORMATTER_H */
