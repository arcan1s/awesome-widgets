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

#include <ui_about.h>
#include <ui_advanced.h>
#include <ui_appearance.h>
#include <ui_deconfig.h>
#include <ui_tooltipconfig.h>
#include <ui_widget.h>


class CustomLabel;
class ExtScript;
class GraphicalItem;
class QGraphicsGridLayout;
class QGraphicsScene;
class QNetworkReply;

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
    void showUpdates(QString version);
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
    void updateNetworkDevice(const bool delay = true);
    void updateText(const bool clear = false);
    void updateTooltip();

private slots:
    void checkUpdates();
    void reinit();
    void replyRecieved(QNetworkReply *reply);
    // configuration interface
    void addBar();
    void addCustomScript();
    void addNewPkgCommand(QTableWidgetItem *item);
    void contextMenuPkgCommand(const QPoint pos);
    void editBar(QListWidgetItem *item);
    void editCustomCommand(QListWidgetItem *item);
    void editFanItem(QListWidgetItem *item);
    void editHddItem(QListWidgetItem *item);
    void editHddSpeedItem(QListWidgetItem *item);
    void editMountItem(QListWidgetItem *item);
    void editTempItem(QListWidgetItem *item);
    void setFontFormating();
    void setFormating();

signals:
    void thereIsUpdates(QString version);

protected:
    QList<QAction *> contextualActions();
    void createConfigurationInterface(KConfigDialog *parent);

private:
    // functions
    void createActions();
    QStringList findKeys();
    QStringList getKeys();
    QStringList findGraphicalItems();
    void getGraphicalItems();
    QList<ExtScript *> initScripts();
    // ui
    QGraphicsGridLayout *mainLayout = nullptr;
    CustomLabel *textLabel = nullptr;
    QTimer *timer = nullptr;
    QList<QAction *> contextMenu;
    // tooltip
    Plasma::ToolTipContent toolTip;
    QGraphicsScene *toolTipScene = nullptr;
    QGraphicsView *toolTipView = nullptr;
    // values
    int networkDeviceUpdate;
    QMap<QString, int> counts;
    QMap<QString, QList<float>> tooltipValues;
    QMap<QString, QString> values;
    // debug
    bool debug;
    // data engine
    Plasma::DataEngine *extsysmonEngine = nullptr;
    Plasma::DataEngine *sysmonEngine = nullptr;
    Plasma::DataEngine *timeEngine = nullptr;
    // configuration interface
    Ui::About uiAboutConfig;
    Ui::AdvancedWindow uiAdvancedConfig;
    Ui::AppearanceWindow uiAppConfig;
    Ui::ConfigWindow uiWidConfig;
    Ui::DEWindow uiDEConfig;
    Ui::TooltipWindow uiTooltipConfig;
    // configuration
    QMap<QString, QString> configuration;
    QStringList diskDevices, keys, formatLine, foundKeys;
    QMap<QString, GraphicalItem *> graphicalItems;
    QStringList foundBars;
};


#endif /* AWESOME_WIDGET_H */
