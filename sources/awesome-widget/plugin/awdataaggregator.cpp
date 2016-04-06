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

#include "awdataaggregator.h"

#include <KI18n/KLocalizedString>

#include <QtConcurrent/QtConcurrent>
#include <QBuffer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>

#include <cmath>

#include "awactions.h"
#include "awdebug.h"


AWDataAggregator::AWDataAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    boundaries[QString("cpuTooltip")] = 100.0;
    boundaries[QString("cpuclTooltip")] = 4000.0;
    boundaries[QString("memTooltip")] = 100.0;
    boundaries[QString("swapTooltip")] = 100.0;
    boundaries[QString("downkbTooltip")] = 1.0;
    boundaries[QString("upkbTooltip")] = 1.0;
    boundaries[QString("batTooltip")] = 100.0;

    initScene();
    connect(this, SIGNAL(updateData(const QVariantHash &)), this,
            SLOT(dataUpdate(const QVariantHash &)));
}


AWDataAggregator::~AWDataAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete toolTipScene;
}


QList<float> AWDataAggregator::getData(const QString key) const
{
    qCDebug(LOG_AW) << "Key" << key;

    return data[QString("%1Tooltip").arg(key)];
}


QString AWDataAggregator::htmlImage(const QPixmap &source) const
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    source.save(&buffer, "PNG");

    return byteArray.isEmpty()
               ? QString()
               : QString("<img src=\"data:image/png;base64,%1\"/>")
                     .arg(QString(byteArray.toBase64()));
}


void AWDataAggregator::setParameters(QVariantMap settings)
{
    qCDebug(LOG_AW) << "Settings" << settings;

    // cast from QVariantMap to QVariantHash without data lost
    configuration = qvariant_cast<QVariantHash>(settings);

    m_enablePopup = configuration[QString("notify")].toBool();

    counts = 0;
    counts += configuration[QString("cpuTooltip")].toInt();
    counts += configuration[QString("cpuclTooltip")].toInt();
    counts += configuration[QString("memTooltip")].toInt();
    counts += configuration[QString("swapTooltip")].toInt();
    counts += configuration[QString("downkbTooltip")].toInt();
    counts += configuration[QString("batTooltip")].toInt();
    // resize tooltip image
    toolTipView->resize(100 * counts, 105);

    requiredKeys.clear();
    if (configuration[QString("cpuTooltip")].toBool())
        requiredKeys.append(QString("cpuTooltip"));
    if (configuration[QString("cpuclTooltip")].toBool())
        requiredKeys.append(QString("cpuclTooltip"));
    if (configuration[QString("memTooltip")].toBool())
        requiredKeys.append(QString("memTooltip"));
    if (configuration[QString("swapTooltip")].toBool())
        requiredKeys.append(QString("swapTooltip"));
    if (configuration[QString("downkbTooltip")].toBool())
        requiredKeys.append(QString("downkbTooltip"));
    if (configuration[QString("upkbTooltip")].toBool())
        requiredKeys.append(QString("upkbTooltip"));
    if (configuration[QString("batTooltip")].toBool())
        requiredKeys.append(QString("batTooltip"));

    // background
    toolTipScene->setBackgroundBrush(
        configuration[QString("useTooltipBackground")].toBool()
            ? QBrush(QColor(
                  configuration[QString("tooltipBackground")].toString()))
            : QBrush(Qt::NoBrush));
}


QPixmap AWDataAggregator::tooltipImage()
{
    // create image
    toolTipScene->clear();
    QPen pen;
    bool down = false;
    for (auto key : requiredKeys) {
        // create frame
        float normX = 100.0f / static_cast<float>(data[key].count());
        float normY = 100.0f / (1.5f * boundaries[key]);
        float shift = requiredKeys.indexOf(key) * 100.0f;
        if (down)
            shift -= 100.0;
        // apply pen color
        if (key != QString("batTooltip"))
            pen.setColor(
                QColor(configuration[QString("%1Color").arg(key)].toString()));
        // paint data inside frame
        for (int j = 0; j < data[key].count() - 1; j++) {
            // some magic here
            float x1 = j * normX + shift;
            float y1 = -fabs(data[key].at(j)) * normY + 5.0f;
            float x2 = (j + 1) * normX + shift;
            float y2 = -fabs(data[key].at(j + 1)) * normY + 5.0f;
            if (key == QString("batTooltip")) {
                if (data[key].at(j + 1) > 0)
                    pen.setColor(QColor(
                        configuration[QString("batTooltipColor")].toString()));
                else
                    pen.setColor(
                        QColor(configuration[QString("batInTooltipColor")]
                                   .toString()));
            }
            toolTipScene->addLine(x1, y1, x2, y2, pen);
        }
        if (key == QString("downkbTooltip"))
            down = true;
    }

    return toolTipView->grab();
}


void AWDataAggregator::dataUpdate(const QVariantHash &values)
{
    // do not log these arguments
    setData(values);
    emit(toolTipPainted(htmlImage(tooltipImage())));
}


void AWDataAggregator::checkValue(const QString source, const float value,
                                  const float extremum) const
{
    qCDebug(LOG_AW) << "Notification source" << source << "with value" << value
                    << "called with extremum" << extremum;

    if (value >= 0.0) {
        if ((m_enablePopup) && (value > extremum)
            && (data[source].last() < extremum))
            return AWActions::sendNotification(QString("event"),
                                               notificationText(source, value));
    } else {
        if ((m_enablePopup) && (value < extremum)
            && (data[source].last() > extremum))
            return AWActions::sendNotification(QString("event"),
                                               notificationText(source, value));
    }
}


void AWDataAggregator::checkValue(const QString source, const QString current,
                                  const QString received) const
{
    qCDebug(LOG_AW) << "Notification source" << source << "with current value"
                    << current << "and received one" << received;

    if ((m_enablePopup) && (current != received) && (!received.isEmpty()))
        return AWActions::sendNotification(QString("event"),
                                           notificationText(source, received));
}


void AWDataAggregator::initScene()
{
    toolTipScene = new QGraphicsScene(nullptr);
    toolTipView = new QGraphicsView(toolTipScene);
    toolTipView->setStyleSheet(QString("background: transparent"));
    toolTipView->setContentsMargins(0, 0, 0, 0);
    toolTipView->setFrameShape(QFrame::NoFrame);
    toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


QString AWDataAggregator::notificationText(const QString source,
                                           const float value) const
{
    qCDebug(LOG_AW) << "Notification source" << source << "with value" << value;

    QString output;
    if (source == QString("batTooltip"))
        output = value > 0.0 ? i18n("AC online") : i18n("AC offline");
    else if (source == QString("cpuTooltip"))
        output = i18n("High CPU load");
    else if (source == QString("memTooltip"))
        output = i18n("High memory usage");
    else if (source == QString("swapTooltip"))
        output = i18n("Swap is used");
    else if (source == QString("gpu"))
        output = i18n("High GPU load");

    return output;
}


QString AWDataAggregator::notificationText(const QString source,
                                           const QString value) const
{
    qCDebug(LOG_AW) << "Notification source" << source << "with value" << value;

    QString output;
    if (source == QString("netdev"))
        output = i18n("Network device has been changed to %1", value);

    return output;
}


void AWDataAggregator::setData(const QVariantHash &values)
{
    // do not log these arguments
    // battery update requires info is AC online or not
    setData(values[QString("ac")].toString()
                == configuration[QString("acOnline")],
            QString("batTooltip"), values[QString("bat")].toFloat());
    // usual case
    setData(QString("cpuTooltip"), values[QString("cpu")].toFloat(), 90.0);
    setData(QString("cpuclTooltip"), values[QString("cpucl")].toFloat());
    setData(QString("memTooltip"), values[QString("mem")].toFloat(), 80.0);
    setData(QString("swapTooltip"), values[QString("swap")].toFloat(), 0.0);
    setData(QString("downkbTooltip"), values[QString("downkb")].toFloat());
    setData(QString("upkbTooltip"), values[QString("upkb")].toFloat());
    // additional check for network device
    [this](const QString value) {
        checkValue(QString("netdev"), currentNetworkDevice, value);
        currentNetworkDevice = value;
    }(values[QString("netdev")].toString());
    // additional check for GPU load
    [this](const float value) {
        checkValue(QString("gpu"), value, 90.0);
        currentGPULoad = value;
    }(values[QString("gpu")].toFloat());
}


void AWDataAggregator::setData(const QString &source, float value,
                               const float extremum)
{
    qCDebug(LOG_AW) << "Source" << source << "to value" << value
                    << "with extremum" << extremum;

    if (data[source].count() == 0)
        data[source].append(0.0);
    else if (data[source].count()
             > configuration[QString("tooltipNumber")].toInt())
        data[source].removeFirst();
    if (std::isnan(value))
        value = 0.0;

    // notifications
    checkValue(source, value, extremum);

    data[source].append(value);
    if (source == QString("downkbTooltip")) {
        QList<float> netValues
            = data[QString("downkbTooltip")] + data[QString("upkbTooltip")];
        // to avoid inf value of normY
        netValues << 1.0;
        boundaries[QString("downkbTooltip")]
            = 1.2f * *std::max_element(netValues.cbegin(), netValues.cend());
        boundaries[QString("upkbTooltip")]
            = boundaries[QString("downkbTooltip")];
    }
}


void AWDataAggregator::setData(const bool dontInvert, const QString &source,
                               float value)
{
    qCDebug(LOG_AW) << "Do not invert" << dontInvert << "value" << value
                    << "for source" << source;

    // invert values for different battery colours
    value = dontInvert ? value : -value;
    return setData(source, value, 0.0);
}
