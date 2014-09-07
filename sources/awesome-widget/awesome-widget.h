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


#ifndef AWESOME_WIDGET_H
#define AWESOME_WIDGET_H

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/ToolTipContent>

#include <ui_advanced.h>
#include <ui_appearance.h>
#include <ui_deconfig.h>
#include <ui_tooltipconfig.h>
#include <ui_widget.h>


class CustomLabel;
class QGraphicsGridLayout;
class QGraphicsScene;

class AwesomeWidget : public Plasma::Applet
{
    Q_OBJECT

public:
    AwesomeWidget(QObject *parent, const QVariantList &args);
    ~AwesomeWidget();
    QString getNetworkDevice();
    int getNumberCpus();
    float getTemp(const float temp);
    QStringList getTimeKeys();
    void init();
    // de configuration
    QMap<QString, QString> readDataEngineConfiguration();
    void writeDataEngineConfiguration(const QMap<QString, QString> settings);
    QMap<QString, QString> updateDataEngineConfiguration(const QMap<QString, QString> rawConfig);

public slots:
    // contextual actions
    void showKsysguard();
    void showReadme();
    // dataengine
    void addDiskDevice(const QString source);
    void connectToEngine();
    void dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data);
    void disconnectFromEngine();
    // configuration interface
    void configAccepted();
    void configChanged();
    // update events
    void sendNotification(const QString eventId, const QString message);
    void updateNetworkDevice();
    void updateText(const bool clear = false);
    void updateTooltip();

private slots:
    void reinit();
    // configuration interface
    void addNewCustomCommand(QTableWidgetItem *item);
    void addNewPkgCommand(QTableWidgetItem *item);
    void contextMenuCustomCommand(const QPoint pos);
    void contextMenuPkgCommand(const QPoint pos);
    void editHddItem(QListWidgetItem *item);
    void editHddSpeedItem(QListWidgetItem *item);
    void editMountItem(QListWidgetItem *item);
    void editTempItem(QListWidgetItem *item);
    void setFontFormating();
    void setFormating();

protected:
    QList<QAction *> contextualActions();
    void createConfigurationInterface(KConfigDialog *parent);

private:
    // functions
    void createActions();
    QStringList findKeys();
    QStringList getKeys();
    // ui
    QGraphicsGridLayout *mainLayout = nullptr;
    CustomLabel *textLabel = nullptr;
    QTimer *timer;
    QList<QAction *> contextMenu;
    // tooltip
    Plasma::ToolTipContent toolTip;
    QGraphicsScene *toolTipScene;
    QGraphicsView *toolTipView;
    // values
    int networkDeviceUpdate;
    QMap<QString, int> counts;
    QMap<QString, QList<float>> tooltipValues;
    QMap<QString, QString> values;
    // debug
    bool debug;
    // data engine
    Plasma::DataEngine *extsysmonEngine;
    Plasma::DataEngine *sysmonEngine;
    Plasma::DataEngine *timeEngine;
    // configuration interface
    Ui::AdvancedWindow uiAdvancedConfig;
    Ui::AppearanceWindow uiAppConfig;
    Ui::ConfigWindow uiWidConfig;
    Ui::DEWindow uiDEConfig;
    Ui::TooltipWindow uiTooltipConfig;
    // configuration
    QMap<QString, QString> configuration;
    QStringList diskDevices, keys, formatLine, foundKeys;
};


#endif /* AWESOME_WIDGET_H */
