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


#ifndef AWABSTRACTPAIRCONFIG_H
#define AWABSTRACTPAIRCONFIG_H

#include <QDialog>

#include "awabstractpairhelper.h"


class AWAbstractSelector;
namespace Ui
{
class AWAbstractPairConfig;
}

class AWAbstractPairConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AWAbstractPairConfig(QWidget *_parent = nullptr, const bool _hasEdit = false,
                                  QStringList _keys = QStringList());
    ~AWAbstractPairConfig() override;
    template <class T> void initHelper()
    {

        delete m_helper;
        m_helper = new T(this);
    }
    void showDialog();
    // properties
    void setEditable(const bool _first, const bool _second);

private slots:
    void edit();
    void updateUi();

private:
    QPushButton *m_editButton = nullptr;
    Ui::AWAbstractPairConfig *ui = nullptr;
    AWAbstractPairHelper *m_helper = nullptr;
    QList<AWAbstractSelector *> m_selectors;
    // properties
    QPair<bool, bool> m_editable = {false, false};
    bool m_hasEdit = false;
    QStringList m_keys;
    // methods
    void addSelector(const QStringList &_keys, const QStringList &_values,
                     const QPair<QString, QString> &_current);
    void clearSelectors();
    void execDialog();
    QPair<QStringList, QStringList> initKeys() const;
    void updateDialog();
};


#endif /* AWABSTRACTPAIRCONFIG_H */
