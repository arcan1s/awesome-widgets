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

#include "awesome-widget.h"

#include <KNotification>
#include <Plasma/ToolTipManager>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <math.h>

#include "customlabel.h"
#include "graphicalitem.h"
#include <pdebug/pdebug.h>


void AwesomeWidget::reinit()
{
    if (debug) qDebug() << PDEBUG;

    if (configuration[QString("background")].toInt() == 0)
        setBackgroundHints(NoBackground);
    if (configuration[QString("popup")].toInt() == 0)
        textLabel->setPopupEnabled(false);
    else
        textLabel->setPopupEnabled(true);
    updateText(true);

    keys = getKeys();
    foundKeys = findKeys();
    getGraphicalItems();
    foundBars = findGraphicalItems();
    updateNetworkDevice(false);
    connectToEngine();
}


void AwesomeWidget::sendNotification(const QString eventId, const QString message)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << eventId;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << message;

    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_awesome-widget"));
    notification->setTitle(QString("Awesome Widget ::: ") + eventId);
    notification->setText(message);
    notification->sendEvent();
    delete notification;
}


void AwesomeWidget::updateNetworkDevice(const bool delay)
{
    if (debug) qDebug() << PDEBUG;

    if (delay)
        networkDeviceUpdate = 30;
    else
        values[QString("netdev")] = getNetworkDevice();
}


void AwesomeWidget::updateText(const bool clear)
{
    if (debug) qDebug() << PDEBUG;

    QString text = configuration[QString("text")];
    if (!clear) {
        text.replace(QString("$$"), QString("$\\$\\"));
        for (int i=0; i<foundKeys.count(); i++)
            text.replace(QString("$") + foundKeys[i], values[foundKeys[i]]);
        for (int i=0; i<foundBars.count(); i++)
            text.replace(QString("$") + foundBars[i], getItemByTag(foundBars[i])->image(values[foundBars[i]].toFloat()));
        text.replace(QString("$\\$\\"), QString("$$"));
    }
    textLabel->setText(formatLine[0] + text + formatLine[1]);
    mainLayout->itemAt(0, 0)->updateGeometry();
    updateGeometry();
}


void AwesomeWidget::updateTooltip()
{
    if (debug) qDebug() << PDEBUG;

    toolTipView->resize(100.0 * counts[QString("tooltip")], 105.0);
    // boundaries
    QMap<QString, float> boundaries;
    boundaries[QString("cpu")] = 100.0;
    boundaries[QString("cpucl")] = 4000.0;
    boundaries[QString("mem")] = 100.0;
    boundaries[QString("swap")] = 100.0;
    boundaries[QString("down")] = 1.0;
    if (configuration[QString("downTooltip")].toInt() == 2) {
        for (int i=0; i<tooltipValues[QString("down")].count(); i++)
            if (boundaries[QString("down")] < tooltipValues[QString("down")][i])
                boundaries[QString("down")] = tooltipValues[QString("down")][i];
        for (int i=0; i<tooltipValues[QString("up")].count(); i++)
            if (boundaries[QString("down")] < tooltipValues[QString("up")][i])
                boundaries[QString("down")] = tooltipValues[QString("up")][i];
        boundaries[QString("down")] *= 1.2;
    }
    boundaries[QString("up")] = boundaries[QString("down")];
    boundaries[QString("bat")] = 100.0;

    // create image
    toolTipScene->clear();
    QPen pen = QPen();
    if (configuration[QString("useTooltipBackground")].toInt() == 2)
        toolTipScene->setBackgroundBrush(QColor(configuration[QString("tooltipBackground")]));
    else
        toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    bool down = false;
    bool isBattery = false;
    QStringList trueKeys;
    if (tooltipValues.contains(QString("cpu"))) trueKeys.append(QString("cpu"));
    if (tooltipValues.contains(QString("cpucl"))) trueKeys.append(QString("cpucl"));
    if (tooltipValues.contains(QString("mem"))) trueKeys.append(QString("mem"));
    if (tooltipValues.contains(QString("swap"))) trueKeys.append(QString("swap"));
    if (tooltipValues.contains(QString("down"))) trueKeys.append(QString("down"));
    if (tooltipValues.contains(QString("up"))) trueKeys.append(QString("up"));
    if (tooltipValues.contains(QString("bat"))) trueKeys.append(QString("bat"));
    for (int i=0; i<trueKeys.count(); i++) {
        float normX = 100.0 / (float)tooltipValues[trueKeys[i]].count();
        float normY = 100.0 / (1.5 * boundaries[trueKeys[i]]);
        if (trueKeys[i] == QString("bat"))
            isBattery = true;
        else
            isBattery = false;
        if (!isBattery) pen.setColor(QColor(configuration[trueKeys[i] + QString("Color")]));
        float shift = i * 100.0;
        if (down) shift -= 100.0;
        for (int j=0; j<tooltipValues[trueKeys[i]].count()-1; j++) {
            float x1 = j * normX + shift;
            float y1 = - fabs(tooltipValues[trueKeys[i]][j]) * normY + 5.0;
            float x2 = (j + 1) * normX + shift;
            float y2 = - fabs(tooltipValues[trueKeys[i]][j+1]) * normY + 5.0;
            if (isBattery) {
                if (tooltipValues[trueKeys[i]][j+1] > 0)
                    pen.setColor(QColor(configuration[QString("batteryColor")]));
                else
                    pen.setColor(QColor(configuration[QString("batteryInColor")]));
            }
            toolTipScene->addLine(x1, y1, x2, y2, pen);
        }
        if (trueKeys[i] == QString("down")) down = true;
    }

    toolTip.setImage(QPixmap::grabWidget(toolTipView));
    Plasma::ToolTipManager::self()->setContent(this, toolTip);
}
