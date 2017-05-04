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


#ifndef AWFORMATTERCONFIG_H
#define AWFORMATTERCONFIG_H

#include <QDialog>


class AWAbstractSelector;
class AWFormatterHelper;
namespace Ui
{
class AWFormatterConfig;
}

class AWFormatterConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AWFormatterConfig(QWidget *parent = nullptr,
                               const QStringList &keys = QStringList());
    virtual ~AWFormatterConfig();
    Q_INVOKABLE void showDialog();

private slots:
    void editFormatters();
    void updateUi();

private:
    QPushButton *m_editButton = nullptr;
    Ui::AWFormatterConfig *ui = nullptr;
    AWFormatterHelper *m_helper = nullptr;
    QList<AWAbstractSelector *> m_selectors;
    // properties
    QStringList m_keys;
    // methods
    void addSelector(const QStringList &keys, const QStringList &values,
                     const QPair<QString, QString> &current);
    void clearSelectors();
    void execDialog();
    void init();
    QPair<QStringList, QStringList> initKeys() const;
    void updateDialog();
};


#endif /* AWFORMATTERCONFIG_H */
