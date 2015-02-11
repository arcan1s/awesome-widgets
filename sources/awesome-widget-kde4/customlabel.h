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

#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>


class AwesomeWidget;

class CustomLabel : public QLabel
{
    Q_OBJECT

public:
    CustomLabel(AwesomeWidget *wid, const bool debugCmd = false);
    ~CustomLabel();
    void setPopupEnabled(const bool state);

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    AwesomeWidget *widget;
    bool debug;
    bool enablePopup = true;
};


#endif /* CUSTOMLABEL_H */
