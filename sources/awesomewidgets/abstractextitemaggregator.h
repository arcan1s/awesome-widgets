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

#ifndef ABSTRACTEXTITEMAGGREGATOR_H
#define ABSTRACTEXTITEMAGGREGATOR_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QStandardPaths>
#include <QWidget>

#include "abstractextitem.h"
#include "awdebug.h"


// additional class since QObject macro does not allow class templates
class AbstractExtItemAggregator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant configArgs READ configArgs WRITE setConfigArgs)
    Q_PROPERTY(QVariant type READ type)

public:
    explicit AbstractExtItemAggregator(QWidget *parent, const QString type);
    virtual ~AbstractExtItemAggregator();
    // methods
    void copyItem();
    template <class T> void createItem()
    {
        QString fileName = getName();
        int number = uniqNumber();
        QString dir = QString("%1/awesomewidgets/%2")
                          .arg(QStandardPaths::writableLocation(
                              QStandardPaths::GenericDataLocation))
                          .arg(m_type);
        if (fileName.isEmpty()) {
            qCWarning(LOG_LIB) << "Nothing to create";
            return;
        };
        QString filePath = QString("%1/%2").arg(dir).arg(fileName);

        T *newItem = new T(this, filePath);
        newItem->setNumber(number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaintList();
        };
    };
    void deleteItem();
    void editItem();
    QString getName();
    AbstractExtItem *itemFromWidget();
    void repaintList();
    int uniqNumber() const;
    // ui
    QDialog *dialog = nullptr;
    QListWidget *widgetDialog = nullptr;
    QDialogButtonBox *dialogButtons = nullptr;
    QPushButton *copyButton = nullptr;
    QPushButton *createButton = nullptr;
    QPushButton *deleteButton = nullptr;
    // get methods
    QVariant configArgs() const;
    virtual QList<AbstractExtItem *> items() const = 0;
    QString type() const;
    // set methods
    void setConfigArgs(const QVariant _configArgs);

private slots:
    void editItemActivated(QListWidgetItem *);
    void editItemButtonPressed(QAbstractButton *button);

private:
    QVariant m_configArgs;
    QString m_type;
    // ui methods
    virtual void doCreateItem() = 0;
    virtual void initItems() = 0;
};


#endif /* ABSTRACTEXTITEMAGGREGATOR_H */
