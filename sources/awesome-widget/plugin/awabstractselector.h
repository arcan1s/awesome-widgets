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


#ifndef AWABSTRACTSELECTOR_H
#define AWABSTRACTSELECTOR_H

#include <QWidget>


namespace Ui
{
class AWAbstractSelector;
}

class AWAbstractSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AWAbstractSelector(QWidget *parent = nullptr);
    virtual ~AWAbstractSelector();
    QPair<QString, QString> current() const;
    void init(const QStringList keys, const QStringList values,
              const QPair<QString, QString> current);

signals:
    void selectionChanged();

private:
    Ui::AWAbstractSelector *ui = nullptr;
};


#endif /* AWABSTRACTSELECTOR_H */
