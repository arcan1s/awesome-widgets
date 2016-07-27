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

#include <QBuffer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

#include <cmath>

#include "awactions.h"
#include "awdebug.h"


AWDataAggregator::AWDataAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_boundaries[QString("cpuTooltip")] = 100.0;
    m_boundaries[QString("cpuclTooltip")] = 4000.0;
    m_boundaries[QString("memTooltip")] = 100.0;
    m_boundaries[QString("swapTooltip")] = 100.0;
    m_boundaries[QString("downkbTooltip")] = 1.0;
    m_boundaries[QString("upkbTooltip")] = 1.0;
    m_boundaries[QString("batTooltip")] = 100.0;

    initScene();
    connect(this, SIGNAL(updateData(const QVariantHash &)), this,
            SLOT(dataUpdate(const QVariantHash &)));
}


AWDataAggregator::~AWDataAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete m_toolTipScene;
}


QList<float> AWDataAggregator::getData(const QString key) const
{
    qCDebug(LOG_AW) << "Key" << key;

    return m_values[QString("%1Tooltip").arg(key)];
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
    m_configuration = qvariant_cast<QVariantHash>(settings);

    m_enablePopup = m_configuration[QString("notify")].toBool();

    m_counts = 0;
    m_counts += m_configuration[QString("cpuTooltip")].toInt();
    m_counts += m_configuration[QString("cpuclTooltip")].toInt();
    m_counts += m_configuration[QString("memTooltip")].toInt();
    m_counts += m_configuration[QString("swapTooltip")].toInt();
    m_counts += m_configuration[QString("downkbTooltip")].toInt();
    m_counts += m_configuration[QString("batTooltip")].toInt();
    // resize tooltip image
    m_toolTipView->resize(100 * m_counts, 105);

    requiredKeys.clear();
    if (m_configuration[QString("cpuTooltip")].toBool())
        requiredKeys.append(QString("cpuTooltip"));
    if (m_configuration[QString("cpuclTooltip")].toBool())
        requiredKeys.append(QString("cpuclTooltip"));
    if (m_configuration[QString("memTooltip")].toBool())
        requiredKeys.append(QString("memTooltip"));
    if (m_configuration[QString("swapTooltip")].toBool())
        requiredKeys.append(QString("swapTooltip"));
    if (m_configuration[QString("downkbTooltip")].toBool())
        requiredKeys.append(QString("downkbTooltip"));
    if (m_configuration[QString("upkbTooltip")].toBool())
        requiredKeys.append(QString("upkbTooltip"));
    if (m_configuration[QString("batTooltip")].toBool())
        requiredKeys.append(QString("batTooltip"));

    // background
    m_toolTipScene->setBackgroundBrush(
        m_configuration[QString("useTooltipBackground")].toBool()
            ? QBrush(QColor(
                  m_configuration[QString("tooltipBackground")].toString()))
            : QBrush(Qt::NoBrush));
}


QPixmap AWDataAggregator::tooltipImage()
{
    // create image
    m_toolTipScene->clear();
    QPen pen;
    bool down = false;
    for (auto key : requiredKeys) {
        // create frame
        float normX = 100.0f / static_cast<float>(m_values[key].count());
        float normY = 100.0f / (1.5f * m_boundaries[key]);
        float shift = requiredKeys.indexOf(key) * 100.0f;
        if (down)
            shift -= 100.0;
        // apply pen color
        if (key != QString("batTooltip"))
            pen.setColor(QColor(
                m_configuration[QString("%1Color").arg(key)].toString()));
        // paint data inside frame
        for (int j = 0; j < m_values[key].count() - 1; j++) {
            // some magic here
            float x1 = j * normX + shift;
            float y1 = -fabs(m_values[key].at(j)) * normY + 5.0f;
            float x2 = (j + 1) * normX + shift;
            float y2 = -fabs(m_values[key].at(j + 1)) * normY + 5.0f;
            if (key == QString("batTooltip")) {
                if (m_values[key].at(j + 1) > 0)
                    pen.setColor(
                        QColor(m_configuration[QString("batTooltipColor")]
                                   .toString()));
                else
                    pen.setColor(
                        QColor(m_configuration[QString("batInTooltipColor")]
                                   .toString()));
            }
            m_toolTipScene->addLine(x1, y1, x2, y2, pen);
        }
        if (key == QString("downkbTooltip"))
            down = true;
    }

    return m_toolTipView->grab();
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
            && (m_values[source].last() < extremum))
            return AWActions::sendNotification(QString("event"),
                                               notificationText(source, value));
    } else {
        if ((m_enablePopup) && (value < extremum)
            && (m_values[source].last() > extremum))
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
    m_toolTipScene = new QGraphicsScene(nullptr);
    m_toolTipView = new QGraphicsView(m_toolTipScene);
    m_toolTipView->setStyleSheet(QString("background: transparent"));
    m_toolTipView->setContentsMargins(0, 0, 0, 0);
    m_toolTipView->setFrameShape(QFrame::NoFrame);
    m_toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
                == m_configuration[QString("acOnline")],
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
        checkValue(QString("netdev"), m_currentNetworkDevice, value);
        m_currentNetworkDevice = value;
    }(values[QString("netdev")].toString());
    // additional check for GPU load
    [this](const float value) {
        checkValue(QString("gpu"), value, 90.0);
        m_currentGPULoad = value;
    }(values[QString("gpu")].toFloat());
}


void AWDataAggregator::setData(const QString &source, float value,
                               const float extremum)
{
    qCDebug(LOG_AW) << "Source" << source << "to value" << value
                    << "with extremum" << extremum;

    if (m_values[source].count() == 0)
        m_values[source].append(0.0);
    else if (m_values[source].count()
             > m_configuration[QString("tooltipNumber")].toInt())
        m_values[source].removeFirst();
    if (std::isnan(value))
        value = 0.0;

    // notifications
    checkValue(source, value, extremum);

    m_values[source].append(value);
    if (source == QString("downkbTooltip")) {
        QList<float> netValues = m_values[QString("downkbTooltip")]
                                 + m_values[QString("upkbTooltip")];
        // to avoid inf value of normY
        netValues << 1.0;
        m_boundaries[QString("downkbTooltip")]
            = 1.2f * *std::max_element(netValues.cbegin(), netValues.cend());
        m_boundaries[QString("upkbTooltip")]
            = m_boundaries[QString("downkbTooltip")];
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
