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

#include <cmath>

#include "awactions.h"
#include "awdebug.h"


AWDataAggregator::AWDataAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_boundaries["cpuTooltip"] = 100.0;
    m_boundaries["cpuclTooltip"] = 4000.0;
    m_boundaries["memTooltip"] = 100.0;
    m_boundaries["swapTooltip"] = 100.0;
    m_boundaries["downkbTooltip"] = 1.0;
    m_boundaries["upkbTooltip"] = 1.0;
    m_boundaries["batTooltip"] = 100.0;

    initScene();
}


AWDataAggregator::~AWDataAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete m_toolTipScene;
}


QList<float> AWDataAggregator::getData(const QString &_key) const
{
    qCDebug(LOG_AW) << "Key" << _key;

    return m_values[QString("%1Tooltip").arg(_key)];
}


QString AWDataAggregator::htmlImage(const QPixmap &_source) const
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    _source.save(&buffer, "PNG");

    return byteArray.isEmpty()
               ? ""
               : QString("<img src=\"data:image/png;base64,%1\"/>")
                     .arg(QString(byteArray.toBase64()));
}


void AWDataAggregator::setParameters(const QVariantMap &_settings)
{
    qCDebug(LOG_AW) << "Settings" << _settings;

    // cast from QVariantMap to QVariantHash without data lost
    m_configuration = qvariant_cast<QVariantHash>(_settings);

    m_enablePopup = m_configuration["notify"].toBool();

    m_counts = 0;
    m_counts += m_configuration["cpuTooltip"].toInt();
    m_counts += m_configuration["cpuclTooltip"].toInt();
    m_counts += m_configuration["memTooltip"].toInt();
    m_counts += m_configuration["swapTooltip"].toInt();
    m_counts += m_configuration["downkbTooltip"].toInt();
    m_counts += m_configuration["batTooltip"].toInt();
    // resize tooltip image
    m_toolTipView->resize(100 * m_counts, 105);

    requiredKeys.clear();
    if (m_configuration["cpuTooltip"].toBool())
        requiredKeys.append("cpuTooltip");
    if (m_configuration["cpuclTooltip"].toBool())
        requiredKeys.append("cpuclTooltip");
    if (m_configuration["memTooltip"].toBool())
        requiredKeys.append("memTooltip");
    if (m_configuration["swapTooltip"].toBool())
        requiredKeys.append("swapTooltip");
    if (m_configuration["downkbTooltip"].toBool())
        requiredKeys.append("downkbTooltip");
    if (m_configuration["upkbTooltip"].toBool())
        requiredKeys.append("upkbTooltip");
    if (m_configuration["batTooltip"].toBool())
        requiredKeys.append("batTooltip");

    // background
    m_toolTipScene->setBackgroundBrush(
        m_configuration["useTooltipBackground"].toBool()
            ? QBrush(QColor(m_configuration["tooltipBackground"].toString()))
            : QBrush(Qt::NoBrush));
}


QPixmap AWDataAggregator::tooltipImage()
{
    // create image
    m_toolTipScene->clear();
    QPen pen;
    bool down = false;
    for (auto &key : requiredKeys) {
        // create frame
        float normX = 100.0f / static_cast<float>(m_values[key].count());
        float normY = 100.0f / (1.5f * m_boundaries[key]);
        float shift = requiredKeys.indexOf(key) * 100.0f;
        if (down)
            shift -= 100.0;
        // apply pen color
        if (key != "batTooltip")
            pen.setColor(QColor(
                m_configuration[QString("%1Color").arg(key)].toString()));
        // paint data inside frame
        for (int j = 0; j < m_values[key].count() - 1; j++) {
            // some magic here
            float x1 = j * normX + shift;
            float y1 = -std::fabs(m_values[key].at(j)) * normY + 5.0f;
            float x2 = (j + 1) * normX + shift;
            float y2 = -std::fabs(m_values[key].at(j + 1)) * normY + 5.0f;
            if (key == "batTooltip") {
                if (m_values[key].at(j + 1) > 0)
                    pen.setColor(
                        QColor(m_configuration["batTooltipColor"].toString()));
                else
                    pen.setColor(QColor(
                        m_configuration["batInTooltipColor"].toString()));
            }
            m_toolTipScene->addLine(x1, y1, x2, y2, pen);
        }
        if (key == "downkbTooltip")
            down = true;
    }

    return m_toolTipView->grab();
}


void AWDataAggregator::dataUpdate(const QVariantHash &_values)
{
    // do not log these arguments
    setData(_values);
    emit(toolTipPainted(htmlImage(tooltipImage())));
}


void AWDataAggregator::checkValue(const QString &_source, const float _value,
                                  const float _extremum) const
{
    qCDebug(LOG_AW) << "Notification source" << _source << "with value"
                    << _value << "called with extremum" << _extremum;

    if (_value >= 0.0) {
        if ((m_enablePopup) && (_value > _extremum)
            && (m_values[_source].last() < _extremum))
            return AWActions::sendNotification(
                "event", notificationText(_source, _value));
    } else {
        if ((m_enablePopup) && (_value < _extremum)
            && (m_values[_source].last() > _extremum))
            return AWActions::sendNotification(
                "event", notificationText(_source, _value));
    }
}


void AWDataAggregator::checkValue(const QString &_source,
                                  const QString &_current,
                                  const QString &_received) const
{
    qCDebug(LOG_AW) << "Notification source" << _source << "with current value"
                    << _current << "and received one" << _received;

    if ((m_enablePopup) && (_current != _received) && (!_received.isEmpty()))
        return AWActions::sendNotification(
            "event", notificationText(_source, _received));
}


void AWDataAggregator::initScene()
{
    m_toolTipScene = new QGraphicsScene(nullptr);
    m_toolTipView = new QGraphicsView(m_toolTipScene);
    m_toolTipView->setStyleSheet("background: transparent");
    m_toolTipView->setContentsMargins(0, 0, 0, 0);
    m_toolTipView->setFrameShape(QFrame::NoFrame);
    m_toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


QString AWDataAggregator::notificationText(const QString &_source,
                                           const float _value) const
{
    qCDebug(LOG_AW) << "Notification source" << _source << "with value"
                    << _value;

    QString output;
    if (_source == "batTooltip")
        output = _value > 0.0 ? i18n("AC online") : i18n("AC offline");
    else if (_source == "cpuTooltip")
        output = i18n("High CPU load");
    else if (_source == "memTooltip")
        output = i18n("High memory usage");
    else if (_source == "swapTooltip")
        output = i18n("Swap is used");
    else if (_source == "gpu")
        output = i18n("High GPU load");

    return output;
}


QString AWDataAggregator::notificationText(const QString &_source,
                                           const QString &_value) const
{
    qCDebug(LOG_AW) << "Notification source" << _source << "with value"
                    << _value;

    QString output;
    if (_source == "netdev")
        output = i18n("Network device has been changed to %1", _value);

    return output;
}


void AWDataAggregator::setData(const QVariantHash &_values)
{
    // do not log these arguments
    // battery update requires info is AC online or not
    setData(_values["ac"].toString() == m_configuration["acOnline"],
            "batTooltip", _values["bat"].toFloat());
    // usual case
    setData("cpuTooltip", _values["cpu"].toFloat(), 90.0);
    setData("cpuclTooltip", _values["cpucl"].toFloat());
    setData("memTooltip", _values["mem"].toFloat(), 80.0);
    setData("swapTooltip", _values["swap"].toFloat(), 0.0);
    setData("downkbTooltip", _values["downkb"].toFloat());
    setData("upkbTooltip", _values["upkb"].toFloat());
    // additional check for network device
    [this](const QString &value) {
        checkValue("netdev", m_currentNetworkDevice, value);
        m_currentNetworkDevice = value;
    }(_values["netdev"].toString());
    // additional check for GPU load
    [this](const float value) {
        checkValue("gpu", value, 90.0);
        m_currentGPULoad = value;
    }(_values["gpu"].toFloat());
}


void AWDataAggregator::setData(const QString &_source, float _value,
                               const float _extremum)
{
    qCDebug(LOG_AW) << "Source" << _source << "to value" << _value
                    << "with extremum" << _extremum;

    if (m_values[_source].count() == 0)
        m_values[_source].append(0.0);
    else if (m_values[_source].count()
             > m_configuration["tooltipNumber"].toInt())
        m_values[_source].removeFirst();
    if (std::isnan(_value))
        _value = 0.0;

    // notifications
    checkValue(_source, _value, _extremum);

    m_values[_source].append(_value);
    if (_source == "downkbTooltip") {
        QList<float> netValues
            = m_values["downkbTooltip"] + m_values["upkbTooltip"];
        // to avoid inf value of normY
        netValues << 1.0;
        m_boundaries["downkbTooltip"]
            = 1.2f * *std::max_element(netValues.cbegin(), netValues.cend());
        m_boundaries["upkbTooltip"] = m_boundaries["downkbTooltip"];
    }
}


void AWDataAggregator::setData(const bool _dontInvert, const QString &_source,
                               float _value)
{
    qCDebug(LOG_AW) << "Do not invert" << _dontInvert << "value" << _value
                    << "for source" << _source;

    // invert values for different battery colours
    _value = _dontInvert ? _value : -_value;
    return setData(_source, _value, 0.0);
}
