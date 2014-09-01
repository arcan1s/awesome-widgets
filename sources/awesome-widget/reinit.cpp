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

#include <Plasma/ToolTipManager>
#include <QGraphicsLinearLayout>
#include <QGraphicsView>
//#include <QThread>

#include "customlabel.h"
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
    updateText(true);
    resize(10, 10);
}


void AwesomeWidget::updateText(bool clear)
{
    if (debug) qDebug() << PDEBUG;

    QString text = configuration[QString("text")];
    if (!clear)
        for (int i=0; i<foundKeys.count(); i++)
            text.replace(QString("$") + foundKeys[i] + QString("$"), values[foundKeys[i]]);
    textLabel->setText(formatLine[0] + text + formatLine[1]);
}


void AwesomeWidget::updateTooltip()
{
    if (debug) qDebug() << PDEBUG;

    toolTipView->resize(100.0 * counts[QString("tooltip")], 100.0);

    // boundaries
    QMap<QString, float> boundaries;
    boundaries[QString("cpu")] = 100.0;
    boundaries[QString("cpucl")] = 4000.0;
    boundaries[QString("mem")] = 100.0;
    boundaries[QString("swap")] = 100.0;
    boundaries[QString("down")] = 1.0;
    if (configuration[QString("downTooltip")].toInt() == 2)
        for (int i=0; i<tooltipValues[QString("down")].count(); i++)
            if (boundaries[QString("down")] < tooltipValues[QString("down")][i])
                boundaries[QString("down")] = tooltipValues[QString("down")][i];
    if (configuration[QString("upTooltip")].toInt() == 2)
        for (int i=0; i<tooltipValues[QString("up")].count(); i++)
            if (boundaries[QString("down")] < tooltipValues[QString("up")][i])
                boundaries[QString("down")] = tooltipValues[QString("up")][i];
    boundaries[QString("up")] = boundaries[QString("down")];

    // create image
    toolTipScene->clear();
    QPen pen = QPen();
    if (configuration[QString("useTooltipBackground")].toInt() == 2)
        toolTipScene->setBackgroundBrush(QColor(configuration[QString("tooltipBackground")]));
    else
        toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    bool down = false;
    for (int i=0; i<tooltipValues.keys().count(); i++) {
        float normX = 100.0 / tooltipValues[tooltipValues.keys()[i]].count();
        float normY = 100.0 / (1.5 * boundaries[tooltipValues.keys()[i]]);
        pen.setColor(QColor(configuration[tooltipValues.keys()[i] + QString("Color")]));
        float shift = i * 100.0;
        if (down)
            shift -= 100.0;
        for (int j=0; j<tooltipValues[tooltipValues.keys()[i]].count()-1; j++) {
            float x1 = j * normX + shift;
            float y1 = -tooltipValues[tooltipValues.keys()[i]][j] * normY;
            float x2 = (j + 1) * normX + shift;
            float y2 = -tooltipValues[tooltipValues.keys()[i]][j+1] * normY;
            toolTipScene->addLine(x1, y1, x2, y2, pen);
        }
        if (tooltipValues.keys()[i] == QString("down"))
            down = true;
    }

    toolTip.setImage(QPixmap::grabWidget(toolTipView));
    Plasma::ToolTipManager::self()->setContent(this, toolTip);
}
