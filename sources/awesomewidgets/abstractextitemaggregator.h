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
#include <QObject>
#include <QPushButton>
#include <QWidget>


class AbstractExtItemAggregator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant configArgs READ configArgs WRITE setConfigArgs)

public:
    AbstractExtItemAggregator(QWidget *parent = nullptr, const bool debugCmd = false);
    virtual ~AbstractExtItemAggregator();
    QString getName();
    // ui
    QDialog *dialog = nullptr;
    QListWidget *widgetDialog = nullptr;
    QDialogButtonBox *dialogButtons = nullptr;
    QPushButton *copyButton = nullptr;
    QPushButton *createButton = nullptr;
    QPushButton *deleteButton = nullptr;
    // get methods
    QVariant configArgs() const;
    // set methods
    void setConfigArgs(const QVariant _configArgs);

private slots:
    void editItemActivated(QListWidgetItem *item);
    void editItemButtonPressed(QAbstractButton *button);

private:
    bool debug;
    QVariant m_configArgs;
    // methods
    virtual void copyItem() = 0;
    virtual void createItem() = 0;
    virtual void deleteItem() = 0;
    virtual void editItem() = 0;
};


#endif /* ABSTRACTEXTITEMAGGREGATOR_H */
