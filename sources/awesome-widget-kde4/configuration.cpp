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

#include <KConfigDialog>
#include <KGlobal>
#include <KStandardDirs>
#include <QDesktopServices>
#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QNetworkInterface>
#include <QTextCodec>

#include <extscript.h>
#include <graphicalitem.h>
#include <fontdialog/fontdialog.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>
#include "version.h"


QMap<QString, QString> AwesomeWidget::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> rawConfig;
    rawConfig[QString("ACPIPATH")] = QString("/sys/class/power_supply/");
    rawConfig[QString("GPUDEV")] = QString("auto");
    rawConfig[QString("HDDDEV")] = QString("all");
    rawConfig[QString("HDDTEMPCMD")] = QString("sudo hddtemp");
    rawConfig[QString("MPDADDRESS")] = QString("localhost");
    rawConfig[QString("MPDPORT")] = QString("6600");
    rawConfig[QString("MPRIS")] = QString("auto");
    rawConfig[QString("PKGCMD")] = QString("pacman -Qu");
    rawConfig[QString("PKGNULL")] = QString("0");
    rawConfig[QString("PLAYER")] = QString("mpris");

    QString fileName = KGlobal::dirs()->findResource("config", "plasma-dataengine-extsysmon.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::ReadOnly))
        return updateDataEngineConfiguration(rawConfig);
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (configFile.atEnd()) break;
    }
    configFile.close();

    return updateDataEngineConfiguration(rawConfig);
}


void AwesomeWidget::writeDataEngineConfiguration(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config = updateDataEngineConfiguration(settings);
    QString fileName = KGlobal::dirs()->locateLocal("config", "plasma-dataengine-extsysmon.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::WriteOnly)) return;
    for (int i=0; i<config.keys().count(); i++) {
        QByteArray string = (config.keys()[i] + QString("=") + config[config.keys()[i]] + QString("\n")).toUtf8();
        configFile.write(string);
    }
    configFile.close();
}


QMap<QString, QString> AwesomeWidget::updateDataEngineConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("HDDTEMPCMD")) &&
            (key != QString("PKGCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }
    // pkgcmd
    for (int i=config[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts).count();
         i<config[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count()+1;
         i++)
        config[QString("PKGNULL")] += QString(",0");

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << config.keys()[i] + QString("=") + config[config.keys()[i]];

    return config;
}


void AwesomeWidget::createConfigurationInterface(KConfigDialog *parent)
{
    if (debug) qDebug() << PDEBUG;
    QString cmd, qoutput;
    QStringList headerList;
    TaskResult process;

    QWidget *advWidget = new QWidget;
    uiAdvancedConfig.setupUi(advWidget);
    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);
    QWidget *deConfigWidget = new QWidget;
    uiDEConfig.setupUi(deConfigWidget);
    QWidget *tooltipWidget = new QWidget;
    uiTooltipConfig.setupUi(tooltipWidget);
    QWidget *aboutWidget = new QWidget;
    uiAboutConfig.setupUi(aboutWidget);

    //widget
    uiWidConfig.textEdit_elements->setPlainText(configuration[QString("text")]);
    uiWidConfig.comboBox_tags->clear();
    uiWidConfig.comboBox_tags->addItems(getKeys());
    for (int i=0; i<graphicalItems.count(); i++)
        uiWidConfig.comboBox_tags->addItem(graphicalItems[i]->name() + graphicalItems[i]->bar());

    // advanced
    if (configuration[QString("background")].toInt() == 0)
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Checked);
    if (configuration[QString("popup")].toInt() == 0)
        uiAdvancedConfig.checkBox_popup->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_popup->setCheckState(Qt::Checked);
    uiAdvancedConfig.lineEdit_timeFormat->setText(configuration[QString("customTime")]);
    uiAdvancedConfig.lineEdit_uptimeFormat->setText(configuration[QString("customUptime")]);
    uiAdvancedConfig.comboBox_tempUnits->setCurrentIndex(
                uiAdvancedConfig.comboBox_tempUnits->findText(configuration[QString("tempUnits")],
                Qt::MatchFixedString));
    cmd = QString("sensors");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    uiAdvancedConfig.listWidget_tempDevice->clear();
    for (int i=0; i<qoutput.split(QString("\n\n")).count(); i++) {
        QString sensor = qoutput.split(QString("\n\n"))[i];
        for (int j=0; j<sensor.split(QChar('\n')).count(); j++) {
            QString device = sensor.split(QChar('\n'))[j];
            if (device.contains(QChar(0260))) {
                QListWidgetItem *item = new QListWidgetItem(QString("lmsensors/") + sensor.split(QChar('\n'))[0] + QString("/") +
                        device.split(QChar(':'))[0].replace(QChar(' '), QChar('_')));
                item->setCheckState(Qt::Unchecked);
                uiAdvancedConfig.listWidget_tempDevice->addItem(item);
            } else if (device.contains(QString("RPM"))) {
                QListWidgetItem *item = new QListWidgetItem(QString("lmsensors/") + sensor.split(QChar('\n'))[0] + QString("/") +
                        device.split(QChar(':'))[0].replace(QChar(' '), QChar('_')));
                item->setCheckState(Qt::Unchecked);
                uiAdvancedConfig.listWidget_fanDevice->addItem(item);
            }
        }
    }
    for (int i=0; i<configuration[QString("tempDevice")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_tempDevice
                ->findItems(configuration[QString("tempDevice")].split(QString("@@"))[i], Qt::MatchFixedString);
        if ((items.isEmpty()) &&
                (!configuration[QString("tempDevice")].split(QString("@@"))[i].isEmpty())) {
            QListWidgetItem *item = new QListWidgetItem(configuration[QString("tempDevice")].split(QString("@@"))[i]);
            item->setCheckState(Qt::Checked);
            uiAdvancedConfig.listWidget_tempDevice->addItem(item);
        } else
            for (int j=0; j<items.count(); j++)
                items[j]->setCheckState(Qt::Checked);
    }
    for (int i=0; i<configuration[QString("fanDevice")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_fanDevice
                ->findItems(configuration[QString("fanDevice")].split(QString("@@"))[i], Qt::MatchFixedString);
        if ((items.isEmpty()) &&
                (!configuration[QString("fanDevice")].split(QString("@@"))[i].isEmpty())) {
            QListWidgetItem *item = new QListWidgetItem(configuration[QString("fanDevice")].split(QString("@@"))[i]);
            item->setCheckState(Qt::Checked);
            uiAdvancedConfig.listWidget_fanDevice->addItem(item);
        } else
            for (int j=0; j<items.count(); j++)
                items[j]->setCheckState(Qt::Checked);
    }
    cmd = QString("mount");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    uiAdvancedConfig.listWidget_mount->clear();
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++) {
        QString mountPoint = qoutput.split(QChar('\n'))[i].split(QString(" on "))[1].split(QString(" type "))[0];
        QListWidgetItem *item = new QListWidgetItem(mountPoint);
        item->setCheckState(Qt::Unchecked);
        uiAdvancedConfig.listWidget_mount->addItem(item);
    }
    for (int i=0; i<configuration[QString("mount")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_mount
                ->findItems(configuration[QString("mount")].split(QString("@@"))[i], Qt::MatchFixedString);
        if ((items.isEmpty()) &&
                (!configuration[QString("mount")].split(QString("@@"))[i].isEmpty())) {
            QListWidgetItem *item = new QListWidgetItem(configuration[QString("mount")].split(QString("@@"))[i]);
            item->setCheckState(Qt::Checked);
            uiAdvancedConfig.listWidget_mount->addItem(item);
        } else
            for (int j=0; j<items.count(); j++)
                items[j]->setCheckState(Qt::Checked);
    }
    uiAdvancedConfig.listWidget_hddSpeedDevice->clear();
    for (int i=0; i<diskDevices.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(diskDevices[i]);
        item->setCheckState(Qt::Unchecked);
        uiAdvancedConfig.listWidget_hddSpeedDevice->addItem(item);
    }
    for (int i=0; i<configuration[QString("disk")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_hddSpeedDevice
                ->findItems(configuration[QString("disk")].split(QString("@@"))[i], Qt::MatchFixedString);
        if ((items.isEmpty()) &&
                (!configuration[QString("disk")].split(QString("@@"))[i].isEmpty())) {
            QListWidgetItem *item = new QListWidgetItem(configuration[QString("disk")].split(QString("@@"))[i]);
            item->setCheckState(Qt::Checked);
            uiAdvancedConfig.listWidget_hddSpeedDevice->addItem(item);
        } else
            for (int j=0; j<items.count(); j++)
                items[j]->setCheckState(Qt::Checked);
    }
    cmd = QString("find /dev -name [hms]d[a-z]");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    uiAdvancedConfig.listWidget_hddDevice->clear();
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(qoutput.split(QChar('\n'))[i]);
        item->setCheckState(Qt::Unchecked);
        uiAdvancedConfig.listWidget_hddDevice->addItem(item);
    }
    for (int i=0; i<configuration[QString("hdd")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_hddDevice
                ->findItems(configuration[QString("hdd")].split(QString("@@"))[i], Qt::MatchFixedString);
        if ((items.isEmpty()) &&
                (!configuration[QString("hdd")].split(QString("@@"))[i].isEmpty())) {
            QListWidgetItem *item = new QListWidgetItem(configuration[QString("hdd")].split(QString("@@"))[i]);
            item->setCheckState(Qt::Checked);
            uiAdvancedConfig.listWidget_hddDevice->addItem(item);
        } else
            for (int j=0; j<items.count(); j++)
                items[j]->setCheckState(Qt::Checked);
    }
    if (configuration[QString("useCustomNetdev")].toInt() == 0)
        uiAdvancedConfig.checkBox_netdev->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_netdev->setCheckState(Qt::Checked);
    QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
    QStringList interfaceList;
    for (int i=0; i<rawInterfaceList.count(); i++)
        interfaceList.append(rawInterfaceList[i].name());
    if ((!interfaceList.contains(configuration[QString("customNetdev")])) &&
            (!configuration[QString("customNetdev")].isEmpty()))
        interfaceList.append(configuration[QString("customNetdev")]);
    uiAdvancedConfig.comboBox_netdev->clear();
    uiAdvancedConfig.comboBox_netdev->addItems(interfaceList);
    uiAdvancedConfig.comboBox_netdev->setCurrentIndex(
                uiAdvancedConfig.comboBox_netdev->findText(configuration[QString("customNetdev")],
                Qt::MatchFixedString));
    uiAdvancedConfig.lineEdit_acOnline->setText(configuration[QString("acOnline")]);
    uiAdvancedConfig.lineEdit_acOffline->setText(configuration[QString("acOffline")]);
    uiAdvancedConfig.listWidget_bars->clear();
    for (int i=0; i<graphicalItems.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(graphicalItems[i]->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Tag: %1", graphicalItems[i]->name() + graphicalItems[i]->bar()));
        tooltip.append(i18n("Comment: %1", graphicalItems[i]->comment()));
        item->setToolTip(tooltip.join(QChar('\n')));
        uiAdvancedConfig.listWidget_bars->addItem(item);
    }
    if (configuration[QString("checkUpdates")].toInt() == 0)
        uiAdvancedConfig.checkBox_updates->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_updates->setCheckState(Qt::Checked);

    // tooltip
    uiTooltipConfig.spinBox_tooltipNum->setValue(configuration[QString("tooltipNumber")].toInt());
    if (configuration[QString("useTooltipBackground")].toInt() == 0)
        uiTooltipConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_background->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_background->setColor(QColor(configuration[QString("tooltipBackground")]));
    if (configuration[QString("cpuTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_cpu->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_cpu->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_cpu->setColor(QColor(configuration[QString("cpuColor")]));
    if (configuration[QString("cpuclTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_cpuclock->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_cpuclock->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_cpuclock->setColor(QColor(configuration[QString("cpuclColor")]));
    if (configuration[QString("memTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_mem->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_mem->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_mem->setColor(QColor(configuration[QString("memColor")]));
    if (configuration[QString("swapTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_swap->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_swap->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_swap->setColor(QColor(configuration[QString("swapColor")]));
    if (configuration[QString("downTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_down->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_down->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_down->setColor(QColor(configuration[QString("downColor")]));
    uiTooltipConfig.kcolorcombo_up->setColor(QColor(configuration[QString("upColor")]));
    if (configuration[QString("batteryTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_battery->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_battery->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_battery->setColor(QColor(configuration[QString("batteryColor")]));
    uiTooltipConfig.kcolorcombo_batteryIn->setColor(QColor(configuration[QString("batteryInColor")]));

    // appearance
    KConfigGroup cg = config();
    CFont font = CFont(cg.readEntry("fontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("fontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("fontColor", "#000000")));
    font.setHtmlWeight(cg.readEntry("fontWeight", 400));
    font.setItalic(cg.readEntry("fontStyle", "normal") == QString("italic"));
    uiAppConfig.spinBox_interval->setValue(configuration[QString("interval")].toInt());
    uiAppConfig.fontComboBox->setCurrentFont(font.toQFont());
    uiAppConfig.spinBox_size->setValue(font.pointSize());
    uiAppConfig.kcolorcombo->setColor(font.color());
    if (font.italic())
        uiAppConfig.comboBox_style->setCurrentIndex(1);
    else
        uiAppConfig.comboBox_style->setCurrentIndex(0);
    uiAppConfig.spinBox_weight->setValue(font.htmlWeight());

    // dataengine
    QMap<QString, QString> deSettings = readDataEngineConfiguration();
    uiDEConfig.lineEdit_acpi->setText(deSettings[QString("ACPIPATH")]);
    QList<ExtScript *> externalScripts = initScripts();
    uiDEConfig.listWidget_custom->clear();
    for (int i=0; i<externalScripts.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(externalScripts[i]->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", externalScripts[i]->name()));
        tooltip.append(i18n("Comment: %1", externalScripts[i]->comment()));
        tooltip.append(i18n("Exec: %1", externalScripts[i]->exec()));
        item->setToolTip(tooltip.join(QChar('\n')));
        uiDEConfig.listWidget_custom->addItem(item);
    }
    externalScripts.clear();
    uiDEConfig.comboBox_gpudev->setCurrentIndex(
                uiDEConfig.comboBox_gpudev->findText(deSettings[QString("GPUDEV")], Qt::MatchFixedString));
    cmd = QString("find /dev -name [hms]d[a-z]");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    uiDEConfig.comboBox_hdddev->clear();
    uiDEConfig.comboBox_hdddev->addItem(QString("all"));
    uiDEConfig.comboBox_hdddev->addItem(QString("disable"));
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++)
        uiDEConfig.comboBox_hdddev->addItem(qoutput.split(QChar('\n'))[i]);
    uiDEConfig.comboBox_hdddev->setCurrentIndex(
                uiDEConfig.comboBox_hdddev->findText(deSettings[QString("HDDDEV")], Qt::MatchFixedString));
    uiDEConfig.lineEdit_hddtempCmd->setText(deSettings[QString("HDDTEMPCMD")]);
    uiDEConfig.lineEdit_mpdaddress->setText(deSettings[QString("MPDADDRESS")]);
    uiDEConfig.spinBox_mpdport->setValue(deSettings[QString("MPDPORT")].toInt());
    uiDEConfig.comboBox_mpris->addItem(deSettings[QString("MPRIS")]);
    uiDEConfig.comboBox_mpris->setCurrentIndex(uiDEConfig.comboBox_mpris->count() - 1);
    uiDEConfig.tableWidget_pkgCommand->clear();
    uiDEConfig.tableWidget_pkgCommand->setRowCount(deSettings[QString("PKGCMD")].split(QChar(',')).count() + 1);
    headerList.clear();
    headerList.append(i18n("Package manager"));
    headerList.append(i18n("Null lines"));
    uiDEConfig.tableWidget_pkgCommand->setHorizontalHeaderLabels(headerList);
    uiDEConfig.tableWidget_pkgCommand->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    for (int i=0; i<deSettings[QString("PKGCMD")].split(QChar(',')).count(); i++) {
        uiDEConfig.tableWidget_pkgCommand->setItem(i, 0, new QTableWidgetItem(deSettings[QString("PKGCMD")].split(QChar(','))[i]));
        uiDEConfig.tableWidget_pkgCommand->setItem(i, 1, new QTableWidgetItem(deSettings[QString("PKGNULL")].split(QChar(','))[i]));
    }
    uiDEConfig.tableWidget_pkgCommand->setItem(uiDEConfig.tableWidget_pkgCommand->rowCount() - 1, 1,
                                               new QTableWidgetItem(QString("0")));
    uiDEConfig.comboBox_playerSelect->setCurrentIndex(
                uiDEConfig.comboBox_playerSelect->findText(deSettings[QString("PLAYER")], Qt::MatchFixedString));

    // about
    uiAboutConfig.label_name->setText(QString(NAME));
    uiAboutConfig.label_version->setText(i18n("Version %1\n(build date %2)", QString(VERSION), QString(BUILD_DATE)));
    uiAboutConfig.label_description->setText(i18n("A set of minimalistic plasmoid widgets"));
    uiAboutConfig.label_links->setText(i18n("Links:") + QString("<br>") +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(i18n("Homepage")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(i18n("Repository")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(i18n("Bugtracker")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(i18n("Translation issue")) +
                                       QString("<a href=\"%1\">%2</a>").arg(QString(AUR_PACKAGES)).arg(i18n("AUR packages")));
    uiAboutConfig.label_license->setText(QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR)) +
                                         i18n("This software is licensed under %1", QString(LICENSE)) + QString("</small>"));
    // 2nd tab
    QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
    for (int i=0; i<trdPartyList.count(); i++)
        trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                .arg(trdPartyList[i].split(QChar(','))[0])
                .arg(trdPartyList[i].split(QChar(','))[1])
                .arg(trdPartyList[i].split(QChar(','))[2]);
    uiAboutConfig.label_translators->setText(i18n("Translators: %1", QString(TRANSLATORS)));
    uiAboutConfig.label_trdparty->setText(i18n("This software uses: %1", trdPartyList.join(QString(", "))));

    parent->addPage(configWidget, i18n("Widget"), Applet::icon());
    parent->addPage(advWidget, i18n("Advanced"), QString("system-run"));
    parent->addPage(tooltipWidget, i18n("Tooltip"), QString("preferences-desktop-color"));
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));
    parent->addPage(deConfigWidget, i18n("DataEngine"), QString("utilities-system-monitor"));
    parent->addPage(aboutWidget, i18n("About"), QString("help-about"));

    connect(uiAdvancedConfig.listWidget_fanDevice, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editFanItem(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_hddDevice, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editHddItem(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_hddSpeedDevice, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editHddSpeedItem(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_mount, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editMountItem(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_tempDevice, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editTempItem(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_bars, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editBar(QListWidgetItem *)));
    connect(uiAdvancedConfig.listWidget_bars, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuBars(QPoint)));
    connect(uiDEConfig.listWidget_custom, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editCustomCommand(QListWidgetItem *)));
    connect(uiDEConfig.listWidget_custom, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuCustomCommand(QPoint)));
    connect(uiDEConfig.tableWidget_pkgCommand, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(addNewPkgCommand(QTableWidgetItem *)));
    connect(uiDEConfig.tableWidget_pkgCommand, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuPkgCommand(QPoint)));
    connect(uiAdvancedConfig.pushButton_bars, SIGNAL(clicked(bool)), this, SLOT(addBar()));
    connect(uiDEConfig.pushButton_custom, SIGNAL(clicked(bool)), this, SLOT(addCustomScript()));
    connect(uiWidConfig.pushButton_tags, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_br, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_font, SIGNAL(clicked(bool)), this, SLOT(setFontFormating()));
    connect(uiWidConfig.pushButton_bold, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_italic, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_underline, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_strike, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_left, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_center, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_right, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_fill, SIGNAL(clicked(bool)), this, SLOT(setFormating()));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void AwesomeWidget::configAccepted()
{
    if (debug) qDebug() << PDEBUG;
    QStringList items;

    disconnectFromEngine();
    KConfigGroup cg = config();

    // widget
    cg.writeEntry("text", uiWidConfig.textEdit_elements->toPlainText());

    // advanced
    cg.writeEntry("background", QString::number(uiAdvancedConfig.checkBox_background->checkState()));
    cg.writeEntry("popup", QString::number(uiAdvancedConfig.checkBox_popup->checkState()));
    cg.writeEntry("customTime", uiAdvancedConfig.lineEdit_timeFormat->text());
    cg.writeEntry("customUptime", uiAdvancedConfig.lineEdit_uptimeFormat->text());
    cg.writeEntry("tempUnits", uiAdvancedConfig.comboBox_tempUnits->currentText());
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_tempDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_tempDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_tempDevice->item(i)->text());
    cg.writeEntry("tempDevice", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_fanDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_fanDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_fanDevice->item(i)->text());
    cg.writeEntry("fanDevice", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_mount->count(); i++)
        if (uiAdvancedConfig.listWidget_mount->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_mount->item(i)->text());
    cg.writeEntry("mount", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_hddDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_hddDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_hddDevice->item(i)->text());
    cg.writeEntry("hdd", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_hddSpeedDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_hddSpeedDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_hddSpeedDevice->item(i)->text());
    cg.writeEntry("disk", items.join(QString("@@")));
    cg.writeEntry("useCustomNetdev", QString::number(uiAdvancedConfig.checkBox_netdev->checkState()));
    cg.writeEntry("customNetdev", uiAdvancedConfig.comboBox_netdev->currentText());
    cg.writeEntry("acOnline", uiAdvancedConfig.lineEdit_acOnline->text());
    cg.writeEntry("acOffline", uiAdvancedConfig.lineEdit_acOffline->text());
    cg.writeEntry("checkUpdates", QString::number(uiAdvancedConfig.checkBox_updates->checkState()));

    // tooltip
    cg.writeEntry("tooltipNumber", QString::number(uiTooltipConfig.spinBox_tooltipNum->value()));
    cg.writeEntry("useTooltipBackground", QString::number(uiTooltipConfig.checkBox_background->checkState()));
    cg.writeEntry("tooltipBackground", uiTooltipConfig.kcolorcombo_background->color().name());
    cg.writeEntry("cpuTooltip", QString::number(uiTooltipConfig.checkBox_cpu->checkState()));
    cg.writeEntry("cpuColor", uiTooltipConfig.kcolorcombo_cpu->color().name());
    cg.writeEntry("cpuclTooltip", QString::number(uiTooltipConfig.checkBox_cpuclock->checkState()));
    cg.writeEntry("cpuclColor", uiTooltipConfig.kcolorcombo_cpuclock->color().name());
    cg.writeEntry("memTooltip", QString::number(uiTooltipConfig.checkBox_mem->checkState()));
    cg.writeEntry("memColor", uiTooltipConfig.kcolorcombo_mem->color().name());
    cg.writeEntry("swapTooltip", QString::number(uiTooltipConfig.checkBox_swap->checkState()));
    cg.writeEntry("swapColor", uiTooltipConfig.kcolorcombo_swap->color().name());
    cg.writeEntry("downTooltip", QString::number(uiTooltipConfig.checkBox_down->checkState()));
    cg.writeEntry("downColor", uiTooltipConfig.kcolorcombo_down->color().name());
    cg.writeEntry("upColor", uiTooltipConfig.kcolorcombo_up->color().name());
    cg.writeEntry("batteryTooltip", QString::number(uiTooltipConfig.checkBox_battery->checkState()));
    cg.writeEntry("batteryColor", uiTooltipConfig.kcolorcombo_battery->color().name());
    cg.writeEntry("batteryInColor", uiTooltipConfig.kcolorcombo_batteryIn->color().name());

    // appearance
    cg.writeEntry("interval", QString::number(uiAppConfig.spinBox_interval->value()));
    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox->currentFont().family());
    cg.writeEntry("fontSize", QString::number(uiAppConfig.spinBox_size->value()));
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo->color().name());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_style->currentText());
    cg.writeEntry("fontWeight", QString::number(uiAppConfig.spinBox_weight->value()));

    // dataengine
    QMap<QString, QString> deSettings;
    deSettings[QString("ACPIPATH")] = uiDEConfig.lineEdit_acpi->text();
    deSettings[QString("GPUDEV")] = uiDEConfig.comboBox_gpudev->currentText();
    deSettings[QString("HDDDEV")] = uiDEConfig.comboBox_hdddev->currentText();
    deSettings[QString("HDDTEMPCMD")] = uiDEConfig.lineEdit_hddtempCmd->text();
    deSettings[QString("MPDADDRESS")] = uiDEConfig.lineEdit_mpdaddress->text();
    deSettings[QString("MPDPORT")] = QString::number(uiDEConfig.spinBox_mpdport->value());
    deSettings[QString("MPRIS")] = uiDEConfig.comboBox_mpris->currentText();
    items.clear();
    for (int i=0; i<uiDEConfig.tableWidget_pkgCommand->rowCount(); i++)
        if (uiDEConfig.tableWidget_pkgCommand->item(i, 0) != 0)
            items.append(uiDEConfig.tableWidget_pkgCommand->item(i, 0)->text());
    deSettings[QString("PKGCMD")] = items.join(QChar(','));
    items.clear();
    for (int i=0; i<uiDEConfig.tableWidget_pkgCommand->rowCount(); i++)
        if (uiDEConfig.tableWidget_pkgCommand->item(i, 0) != 0)
            items.append(uiDEConfig.tableWidget_pkgCommand->item(i, 1)->text());
    deSettings[QString("PKGNULL")] = items.join(QChar(','));
    deSettings[QString("PLAYER")] = uiDEConfig.comboBox_playerSelect->currentText();
    writeDataEngineConfiguration(deSettings);
}


void AwesomeWidget::configChanged()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    // widget
    configuration[QString("text")] = cg.readEntry("text", "[cpu: $cpu%] [mem: $mem%] [swap: $swap%] [$netdev: $down/$upKB/s]");

    // advanced
    configuration[QString("background")] = cg.readEntry("background", "2");
    configuration[QString("popup")] = cg.readEntry("popup", "2");
    configuration[QString("customTime")] = cg.readEntry("customTime", "$hh:$mm");
    configuration[QString("customUptime")] = cg.readEntry("customUptime", "$dd,$hh,$mm");
    configuration[QString("tempUnits")] = cg.readEntry("tempUnits", "Celsius");
    configuration[QString("tempDevice")] = cg.readEntry("tempDevice", "");
    configuration[QString("fanDevice")] = cg.readEntry("fanDevice", "");
    configuration[QString("mount")] = cg.readEntry("mount", "/");
    configuration[QString("hdd")] = cg.readEntry("hdd", "/dev/sda");
    configuration[QString("disk")] = cg.readEntry("disk", "disk/sda_(8:0)");
    configuration[QString("useCustomNetdev")] = cg.readEntry("useCustomNetdev", "0");
    configuration[QString("customNetdev")] = cg.readEntry("customNetdev", "lo");
    configuration[QString("acOnline")] = cg.readEntry("acOnline", "(*)");
    configuration[QString("acOffline")] = cg.readEntry("acOffline", "( )");
    configuration[QString("checkUpdates")] = cg.readEntry("checkUpdates", "2");

    // tooltip
    tooltipValues.clear();
    configuration[QString("tooltipNumber")] = cg.readEntry("tooltipNumber", "100");
    configuration[QString("useTooltipBackground")] = cg.readEntry("useTooltipBackground", "2");
    configuration[QString("tooltipBackground")] = cg.readEntry("tooltipBackground", "#ffffff");
    configuration[QString("cpuTooltip")] = cg.readEntry("cpuTooltip", "2");
    if (configuration[QString("cpuTooltip")].toInt() == 2) {
        tooltipValues[QString("cpu")].append(0.0);
        tooltipValues[QString("cpu")].append(0.01);
    }
    configuration[QString("cpuColor")] = cg.readEntry("cpuColor", "#ff0000");
    configuration[QString("cpuclTooltip")] = cg.readEntry("cpuclTooltip", "2");
    if (configuration[QString("cpuclTooltip")].toInt() == 2) {
        tooltipValues[QString("cpucl")].append(0.0);
        tooltipValues[QString("cpucl")].append(0.01);
    }
    configuration[QString("cpuclColor")] = cg.readEntry("cpuclColor", "#00ff00");
    configuration[QString("memTooltip")] = cg.readEntry("memTooltip", "2");
    if (configuration[QString("memTooltip")].toInt() == 2) {
        tooltipValues[QString("mem")].append(0.0);
        tooltipValues[QString("mem")].append(0.01);
    }
    configuration[QString("memColor")] = cg.readEntry("memColor", "#0000ff");
    configuration[QString("swapTooltip")] = cg.readEntry("swapTooltip", "2");
    if (configuration[QString("swapTooltip")].toInt() == 2) {
        tooltipValues[QString("swap")].append(0.0);
        tooltipValues[QString("swap")].append(0.01);
    }
    configuration[QString("swapColor")] = cg.readEntry("swapColor", "#ffff00");
    configuration[QString("downTooltip")] = cg.readEntry("downTooltip", "2");
    if (configuration[QString("downTooltip")].toInt() == 2) {
        tooltipValues[QString("down")].append(0.0);
        tooltipValues[QString("down")].append(0.01);
        tooltipValues[QString("up")].append(0.0);
        tooltipValues[QString("up")].append(0.01);
    }
    configuration[QString("downColor")] = cg.readEntry("downColor", "#00ffff");
    configuration[QString("upColor")] = cg.readEntry("upColor", "#ff00ff");
    configuration[QString("batteryTooltip")] = cg.readEntry("batteryTooltip", "2");
    if (configuration[QString("batteryTooltip")].toInt() == 2) {
        tooltipValues[QString("bat")].append(0.0);
        tooltipValues[QString("bat")].append(0.01);
    }
    configuration[QString("batteryColor")] = cg.readEntry("batteryColor", "#008800");
    configuration[QString("batteryInColor")] = cg.readEntry("batteryInColor", "#880000");

    // appearance
    configuration[QString("interval")] = cg.readEntry("interval", "1000");
    CFont font = CFont(cg.readEntry("fontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("fontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("fontColor", "#000000")));
    font.setHtmlWeight(cg.readEntry("fontWeight", 400));
    font.setItalic(cg.readEntry("fontStyle", "normal") == QString("italic"));
    QString fontStyle;
    if (font.italic())
        fontStyle = QString("italic");
    else
        fontStyle = QString("normal");
    formatLine[0] = QString("<html><head><style type=\"text/css\">body { white-space: pre-wrap; }</style>\
                            </head><body style=\"font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4; color:%5;\">")
            .arg(font.family())
            .arg(font.pointSize())
            .arg(font.htmlWeight())
            .arg(fontStyle)
            .arg(font.color().name());
    formatLine[1] = QString("</body></html>");

    // counts
    QMap<QString, QString> deSettings = readDataEngineConfiguration();
    counts[QString("bat")] = 0;
    QStringList acpiDevices = QDir(deSettings[QString("ACPIPATH")]).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QRegExp batRegexp = QRegExp(QString("BAT.*"));
    for (int i=0; i<acpiDevices.count(); i++)
        if (acpiDevices[i].contains(batRegexp))
            counts[QString("bat")]++;
    counts[QString("cpu")] = getNumberCpus();
    counts[QString("custom")] = initScripts().count();
    counts[QString("disk")] = configuration[QString("disk")].split(QString("@@")).count();
    counts[QString("fan")] = configuration[QString("fanDevice")].split(QString("@@")).count();
    counts[QString("hddtemp")] = configuration[QString("hdd")].split(QString("@@")).count();
    counts[QString("mount")] = configuration[QString("mount")].split(QString("@@")).count();
    counts[QString("pkg")] = deSettings[QString("PKGCMD")].split(QChar(',')).count();
    counts[QString("temp")] = configuration[QString("tempDevice")].split(QString("@@")).count();
    counts[QString("tooltip")] = 0;
    counts[QString("tooltip")] += configuration[QString("cpuTooltip")].toInt();
    counts[QString("tooltip")] += configuration[QString("cpuclTooltip")].toInt();
    counts[QString("tooltip")] += configuration[QString("memTooltip")].toInt();
    counts[QString("tooltip")] += configuration[QString("swapTooltip")].toInt();
    counts[QString("tooltip")] += configuration[QString("downTooltip")].toInt();
    counts[QString("tooltip")] += configuration[QString("batteryTooltip")].toInt();
    counts[QString("tooltip")] = counts[QString("tooltip")] / 2;

    reinit();
}


void AwesomeWidget::addBar()
{
    if (debug) qDebug() << PDEBUG;

    int number = 0;
    while (true) {
        bool exit = true;
        for (int i=0; i<graphicalItems.count(); i++)
            if (graphicalItems[i]->name() == QString("bar%1").arg(number)) {
                number++;
                exit = false;
                break;
            }
        if (exit) break;
    }
    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_applet_awesome-widget/desktops");
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");
    QStringList bars;
    bars.append(keys.filter((QRegExp(QString("cpu(?!cl).*")))));
    bars.append(keys.filter((QRegExp(QString("^gpu$")))));
    bars.append(keys.filter((QRegExp(QString("^mem$")))));
    bars.append(keys.filter((QRegExp(QString("^swap$")))));
    bars.append(keys.filter((QRegExp(QString("^hdd[0-9].*")))));
    bars.append(keys.filter((QRegExp(QString("^bat.*")))));

    GraphicalItem *item = new GraphicalItem(0, name, dirs, debug);
    item->setName(QString("bar%1").arg(number));

    item->showConfiguration(bars);
}


void AwesomeWidget::addCustomScript()
{
    if (debug) qDebug() << PDEBUG;

    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_applet_awesome-widget/desktops");
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    ExtScript *script = new ExtScript(0, name, dirs, debug);

    script->showConfiguration();
}


void AwesomeWidget::addNewPkgCommand(QTableWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    if ((item->row() == (uiDEConfig.tableWidget_pkgCommand->rowCount() - 1)) &&
            (item->column() == 0)) {
        uiDEConfig.tableWidget_pkgCommand->insertRow(
                    uiDEConfig.tableWidget_pkgCommand->rowCount());
        uiDEConfig.tableWidget_pkgCommand->setItem(
                    uiDEConfig.tableWidget_pkgCommand->rowCount() - 1, 1,
                    new QTableWidgetItem(QString("0")));
    }
}


void AwesomeWidget::contextMenuBars(const QPoint pos)
{
    if (debug) qDebug() << PDEBUG;
    if (uiAdvancedConfig.listWidget_bars->currentItem() == 0) return;

    QMenu menu(uiAdvancedConfig.listWidget_bars);
    QAction *edit = menu.addAction(QIcon::fromTheme("document-edit"), i18n("Edit"));
    QAction *copy = menu.addAction(QIcon::fromTheme("edit-copy"), i18n("Copy"));
    QAction *remove = menu.addAction(QIcon::fromTheme("edit-delete"), i18n("Remove"));
    QAction *action = menu.exec(uiAdvancedConfig.listWidget_bars->viewport()->mapToGlobal(pos));
    if (action == edit)
        editBar(uiAdvancedConfig.listWidget_bars->currentItem());
    else if (action == copy)
        copyBar(uiAdvancedConfig.listWidget_bars->currentItem()->text());
    else if (action == remove)
        for (int i=0; i<graphicalItems.count(); i++) {
            if (graphicalItems[i]->fileName() != uiAdvancedConfig.listWidget_bars->currentItem()->text())
                continue;
            graphicalItems[i]->tryDelete();
            graphicalItems.takeAt(i);
            uiAdvancedConfig.listWidget_bars->takeItem(uiAdvancedConfig.listWidget_bars->currentRow());
            break;
        }
}


void AwesomeWidget::contextMenuCustomCommand(const QPoint pos)
{
    if (debug) qDebug() << PDEBUG;
    if (uiDEConfig.listWidget_custom->currentItem() == 0) return;

    QMenu menu(uiDEConfig.listWidget_custom);
    QAction *edit = menu.addAction(QIcon::fromTheme("document-edit"), i18n("Edit"));
    QAction *copy = menu.addAction(QIcon::fromTheme("edit-copy"), i18n("Copy"));
    QAction *remove = menu.addAction(QIcon::fromTheme("edit-delete"), i18n("Remove"));
    QAction *action = menu.exec(uiDEConfig.listWidget_custom->viewport()->mapToGlobal(pos));
    if (action == edit)
        editCustomCommand(uiDEConfig.listWidget_custom->currentItem());
    else if (action == copy)
        copyCustomCommand(uiDEConfig.listWidget_custom->currentItem()->text());
    else if (action == remove) {
        QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_dataengine_extsysmon/scripts");
        ExtScript *script = new ExtScript(0, uiDEConfig.listWidget_custom->currentItem()->text(), dirs, debug);
        script->tryDelete();
        delete script;
        uiDEConfig.listWidget_custom->takeItem(uiDEConfig.listWidget_custom->currentRow());
    }
}


void AwesomeWidget::contextMenuPkgCommand(const QPoint pos)
{
    if (debug) qDebug() << PDEBUG;
    if (uiDEConfig.tableWidget_pkgCommand->currentItem() == 0) return;

    QMenu menu(uiDEConfig.tableWidget_pkgCommand);
    QAction *remove = menu.addAction(QIcon::fromTheme("edit-delete"), i18n("Remove"));
    QAction *action = menu.exec(uiDEConfig.tableWidget_pkgCommand->viewport()->mapToGlobal(pos));
    if (action == remove)
        uiDEConfig.tableWidget_pkgCommand->removeRow(
                    uiDEConfig.tableWidget_pkgCommand->currentRow());
}


void AwesomeWidget::copyBar(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    int number = 0;
    while (true) {
        bool exit = true;
        for (int i=0; i<graphicalItems.count(); i++)
            if (graphicalItems[i]->name() == QString("bar%1").arg(number)) {
                number++;
                exit = false;
                break;
            }
        if (exit) break;
    }
    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_applet_awesome-widget/desktops");
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");
    QStringList bars;
    bars.append(keys.filter((QRegExp(QString("cpu(?!cl).*")))));
    bars.append(keys.filter((QRegExp(QString("^gpu$")))));
    bars.append(keys.filter((QRegExp(QString("^mem$")))));
    bars.append(keys.filter((QRegExp(QString("^swap$")))));
    bars.append(keys.filter((QRegExp(QString("^hdd[0-9].*")))));
    bars.append(keys.filter((QRegExp(QString("^bat.*")))));

    GraphicalItem *originalItem = nullptr;
    for (int i=0; i<graphicalItems.count(); i++) {
        if (graphicalItems[i]->fileName() != original) continue;
        originalItem = graphicalItems[i];
        break;
    }
    GraphicalItem *item = new GraphicalItem(0, name, dirs, debug);
    item->setName(QString("bar%1").arg(number));
    item->setComment(originalItem->comment());
    item->setBar(originalItem->bar());
    item->setActiveColor(originalItem->activeColor());
    item->setInactiveColor(originalItem->inactiveColor());
    item->setType(originalItem->type());
    item->setDirection(originalItem->direction());
    item->setHeight(originalItem->height());
    item->setWidth(originalItem->width());
    delete originalItem;

    item->showConfiguration(bars);
    delete item;
}


void AwesomeWidget::copyCustomCommand(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_applet_awesome-widget/desktops");
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    ExtScript *originalScript = new ExtScript(0, original, dirs, debug);
    ExtScript *script = new ExtScript(0, name, dirs, debug);
    script->setActive(originalScript->isActive());
    script->setComment(originalScript->comment());
    script->setExecutable(originalScript->executable());
    script->setHasOutput(originalScript->hasOutput());
    script->setInterval(originalScript->interval());
    script->setName(originalScript->name());
    script->setPrefix(originalScript->prefix());
    script->setRedirect(originalScript->redirect());
    delete originalScript;

    script->showConfiguration();
    delete script;
}


void AwesomeWidget::editBar(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    QStringList bars;
    bars.append(keys.filter((QRegExp(QString("^cpu(?!cl).*")))));
    bars.append(keys.filter((QRegExp(QString("^gpu$")))));
    bars.append(keys.filter((QRegExp(QString("^mem$")))));
    bars.append(keys.filter((QRegExp(QString("^swap$")))));
    bars.append(keys.filter((QRegExp(QString("^hdd[0-9].*")))));
    bars.append(keys.filter((QRegExp(QString("^bat.*")))));

    for (int i=0; i<graphicalItems.count(); i++) {
        if (graphicalItems[i]->fileName() != item->text()) continue;
        graphicalItems[i]->showConfiguration(bars);
        break;
    }
}


void AwesomeWidget::editCustomCommand(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_dataengine_extsysmon/scripts");
    ExtScript *script = new ExtScript(0, item->text(), dirs, debug);
    script->showConfiguration();
}


void AwesomeWidget::editFanItem(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    uiAdvancedConfig.listWidget_fanDevice->openPersistentEditor(item);
}


void AwesomeWidget::editHddItem(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    uiAdvancedConfig.listWidget_hddDevice->openPersistentEditor(item);
}


void AwesomeWidget::editHddSpeedItem(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    uiAdvancedConfig.listWidget_hddSpeedDevice->openPersistentEditor(item);
}


void AwesomeWidget::editMountItem(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    uiAdvancedConfig.listWidget_mount->openPersistentEditor(item);
}


void AwesomeWidget::editTempItem(QListWidgetItem *item)
{
    if (debug) qDebug() << PDEBUG;

    uiAdvancedConfig.listWidget_tempDevice->openPersistentEditor(item);
}


QList<ExtScript *> AwesomeWidget::initScripts()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtScript *> externalScripts;
    // create directory at $HOME
    QString localDir = KStandardDirs::locateLocal("data", "plasma_dataengine_extsysmon/scripts");
    if (KStandardDirs::makeDir(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_dataengine_extsysmon/scripts");
    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalScripts.append(new ExtScript(0, files[j], dirs, debug));
        }
    }

    return externalScripts;
}


void AwesomeWidget::setFontFormating()
{
    if (debug) qDebug() << PDEBUG;

    CFont defaultFont = CFont(uiAppConfig.fontComboBox->currentFont().family(),
                              uiAppConfig.spinBox_size->value(),
                              400, false, uiAppConfig.kcolorcombo->color());
    CFont font = CFontDialog::getFont(i18n("Select font"), defaultFont,
                                      false, false);
    if (font != defaultFont) {
        QString selectedText = uiWidConfig.textEdit_elements->textCursor().selectedText();
        uiWidConfig.textEdit_elements->insertPlainText(QString("<span style=\"color:%1; font-family:'%2'; font-size:%3pt;\">")
                                                       .arg(font.color().name()).arg(font.family()).arg(font.pointSize()) +
                                                       selectedText + QString("</span>"));
    }
}


void AwesomeWidget::setFormating()
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Sender" << sender();

    QString selectedText = uiWidConfig.textEdit_elements->textCursor().selectedText();
    if (sender() == uiWidConfig.pushButton_tags)
        uiWidConfig.textEdit_elements->insertPlainText(QString("$") + uiWidConfig.comboBox_tags->currentText());
    else if (sender() == uiWidConfig.pushButton_br)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<br>\n"));
    else if (sender() == uiWidConfig.pushButton_bold)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<b>") + selectedText + QString("</b>"));
    else if (sender() == uiWidConfig.pushButton_italic)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<i>") + selectedText + QString("</i>"));
    else if (sender() == uiWidConfig.pushButton_underline)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<u>") + selectedText + QString("</u>"));
    else if (sender() == uiWidConfig.pushButton_strike)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<s>") + selectedText + QString("</s>"));
    else if (sender() == uiWidConfig.pushButton_left)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"left\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_center)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"center\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_right)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"right\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_fill)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"justify\">") + selectedText + QString("</p>"));
}
