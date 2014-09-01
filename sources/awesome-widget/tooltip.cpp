/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/

#include "awesome-widget.h"

#include <pdebug/pdebug.h>


void AwesomeWidget::initValues()
{
    if (debug) qDebug() << PDEBUG;
}


void AwesomeWidget::reinit()
{
    if (debug) qDebug() << PDEBUG;

    mainLayout = new QGraphicsLinearLayout();
    mainLayout->setContentsMargins(1, 1, 1, 1);
    setLayout(mainLayout);

    if (configuration[QString("background")].toInt() == 0)
        setBackgroundHints(NoBackground);
    else
        setBackgroundHints(DefaultBackground);
    if (configuration[QString("layout")].toInt() == 0)
        mainLayout->setOrientation(Qt::Horizontal);
    else
        mainLayout->setOrientation(Qt::Vertical);
    if (configuration[QString("leftStretch")].toInt() == 2)
        mainLayout->addStretch(1);
    if (configuration[QString("popup")].toInt() == 0)
        textLabel->setPopupEnabled(true);
    mainLayout->addItem(textLabel);
    if (configuration[QString("rightStretch")].toInt() == 2)
        mainLayout->addStretch(1);

    keys = getKeys();
    foundKeys = findKeys();
    initValues();
    values[QString("netdev")] = getNetworkDevice();
//    thread()->wait(60000);
    connectToEngine();
    resize(10, 10);
}


void AwesomeWidget::updateText()
{
    if (debug) qDebug() << PDEBUG;

    QString text = configuration[QString("text")];
    for (int i=0; i<foundKeys.count(); i++)
        text.replace(QString("$") + foundKeys[i] + QString("$"), values[foundKeys[i]]);
    textLabel->setText(text);
}


void AwesomeWidget::updateTooltip()
{
    if (debug) qDebug() << PDEBUG;
}
