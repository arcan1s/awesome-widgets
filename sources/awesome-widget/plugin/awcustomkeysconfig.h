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


#ifndef AWCUSTOMKEYSCONFIG_H
#define AWCUSTOMKEYSCONFIG_H

#include <QDialog>


class AWAbstractSelector;
class AWCustomKeysHelper;
namespace Ui
{
class AWCustomKeysConfig;
}

class AWCustomKeysConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AWCustomKeysConfig(QWidget *_parent = nullptr,
                                const QStringList &_keys = QStringList());
    virtual ~AWCustomKeysConfig();
    Q_INVOKABLE void showDialog();

private slots:
    void updateUi();

private:
    Ui::AWCustomKeysConfig *ui = nullptr;
    AWCustomKeysHelper *m_helper = nullptr;
    QList<AWAbstractSelector *> m_selectors;
    // properties
    QStringList m_keys;
    // methods
    void addSelector(const QStringList &_keys, const QStringList &_values,
                     const QPair<QString, QString> &_current);
    void clearSelectors();
    void execDialog();
    void init();
    QPair<QStringList, QStringList> initKeys() const;
    void updateDialog();
};


#endif /* AWCUSTOMKEYSCONFIG_H */
