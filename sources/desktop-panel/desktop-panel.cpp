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

#include "desktop-panel.h"
#include "ui_appearance.h"
#include "ui_widget.h"

#include <KDE/KConfigDialog>
#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <plasma/theme.h>
#include <QDebug>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QProcessEnvironment>


DesktopPanel::DesktopPanel(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setHasConfigurationInterface(true);
    // text format init
    currentFormatLine.append(QString(""));
    currentFormatLine.append(QString(""));
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


DesktopPanel::~DesktopPanel()
{
//     for (int i=0; i<labels.count(); i++)
//         delete labels[i];
//     delete layout;
}


void DesktopPanel::init()
{
    if (debug) qDebug() << "[PTM-DP]" << "[init]";
    extsysmonEngine = dataEngine(QString("ext-sysmon"));

    layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(1, 1, 1, 1);
    setLayout(layout);

    currentDesktop = 1;

    // read variables
    configChanged();
}


QStringList DesktopPanel::getDesktopNames()
{
    if (debug) qDebug() << "[PTM-DP]" << "[getDesktopNames]";
    QStringList list;
    QString fileName = KGlobal::dirs()->findResource("config", "kwinrc");
    if (debug) qDebug() << "[PTM-DP]" << "[getDesktopNames]" << ":" << "Configuration file" << fileName;
    QFile confFile(fileName);
    if (!confFile.open(QIODevice::ReadOnly)) return list;

    QString fileStr;
    QStringList value;
    bool desktopSection = false;
    while (true) {
        fileStr = QString(confFile.readLine()).trimmed();
        if (fileStr[0] == QChar('#')) continue;
        if (fileStr[0] == QChar(';')) continue;
        if (fileStr[0] == QChar('[')) desktopSection = false;
        if (fileStr == QString("[Desktops]")) desktopSection = true;
        if (desktopSection) {
            if (fileStr.contains(QChar('='))) {
                value.clear();
                for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                    value.append(fileStr.split(QChar('='))[i]);
                if (fileStr.split(QChar('='))[0].contains(QString("Name_")))
                    list.append(value.join(QChar('=')));
            }
        }
        if (confFile.atEnd())
            break;
    }
    confFile.close();
    return list;
}


void DesktopPanel::reinit()
{
    if (debug) qDebug() << "[PTM-DP]" << "[reinit]";

    // clear
    // labels
    for (int i=0; i<labels.count(); i++) {
        layout->removeItem(labels[i]);
        delete labels[i];
    }
    labels.clear();
    // layout
    layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(1, 1, 1, 1);
    setLayout(layout);

    // add
    // layout
    if (configuration[QString("background")].toInt() == 0)
        setBackgroundHints(NoBackground);
    else
        setBackgroundHints(DefaultBackground);
    if (configuration[QString("layout")].toInt() == 0)
        layout->setOrientation(Qt::Horizontal);
    else
        layout->setOrientation(Qt::Horizontal);
    // left stretch
    if (configuration[QString("leftStretch")].toInt() == 2)
        layout->addStretch(1);
    // labels
    for (int i=0; i<desktopNames.count(); i++) {
        labels.append(new Plasma::Label());
        layout->addItem(labels[i]);
    }
    // right stretch
    if (configuration[QString("rightStretch")].toInt() == 2)
        layout->addStretch(1);

    updateText();
}


void DesktopPanel::setCurrentDesktop()
{
    if (debug) qDebug() << "[PTM-DP]" << "[setCurrentDesktop]";
}


void DesktopPanel::updateText()
{
    if (debug) qDebug() << "[PTM-DP]" << "[updateText]";
    if (labels.isEmpty()) return;
    QString line, mark, text;
    for (int i=0; i<labels.count(); i++) {
        if (debug) qDebug() << "[PTM-DP]" << "[updateText]" << "Label" << i;
        line = configuration[QString("pattern")];
        if (currentDesktop == i + 1)
            mark = configuration[QString("mark")];
        else
            mark = QString("");

        if (line.split(QString("$mark"))[0] != line) {
            if (debug) qDebug() << "[PTM-DP]" << "[updateText]" << "Found mark";
            line = line.split(QString("$mark"))[0] + mark + line.split(QString("$mark"))[1];
        }
        if (line.split(QString("$name"))[0] != line) {
            if (debug) qDebug() << "[PTM-DP]" << "[updateText]" << "Found name";
            line = line.split(QString("$name"))[0] + desktopNames[i] + line.split(QString("$name"))[1];
        }
        if (line.split(QString("$number"))[0] != line) {
            if (debug) qDebug() << "[PTM-DP]" << "[updateText]" << "Found number";
            line = line.split(QString("$number"))[0] + QString::number(i+1) + line.split(QString("$number"))[1];
        }
        if (line.split(QString("$total"))[0] != line) {
            if (debug) qDebug() << "[PTM-DP]" << "[updateText]" << "Found total";
            line = line.split(QString("$total"))[0] + QString::number(desktopNames.count()) + line.split(QString("$total"))[1];
        }

        if (currentDesktop == i + 1)
            text = currentFormatLine[0] + line + currentFormatLine[1];
        else
            text = formatLine[0] + line + formatLine[1];
        labels[i]->setText(text);
    }
    resize(1, 1);
}


// data engine interaction
void DesktopPanel::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << "[PTM-DP]" << "[dataUpdated]";
    if (debug) qDebug() << "[PTM-DP]" << "[dataUpdated]" << ":" << "Run function with source name" << sourceName;
    if (data.keys().count() == 0)
        return;
    if (sourceName == QString("desktop")) {
        currentDesktop = data[QString("currentNumber")].toInt();
        updateText();
    }
}


//  configuration interface
void DesktopPanel::createConfigurationInterface(KConfigDialog *parent)
{
    if (debug) qDebug() << "[PTM-DP]" << "[createConfigurationInterface]";
    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);

    if (configuration[QString("background")].toInt() == 0)
        uiWidConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_background->setCheckState(Qt::Checked);
    if (configuration[QString("layout")].toInt() == 0)
        uiWidConfig.checkBox_layout->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_layout->setCheckState(Qt::Checked);
    if (configuration[QString("leftStretch")].toInt() == 0)
        uiWidConfig.checkBox_leftStretch->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_leftStretch->setCheckState(Qt::Checked);
    if (configuration[QString("rightStretch")].toInt() == 0)
        uiWidConfig.checkBox_rightStretch->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_rightStretch->setCheckState(Qt::Checked);
    uiWidConfig.spinBox_interval->setValue(configuration[QString("interval")].toInt());
    uiWidConfig.lineEdit_mark->setText(configuration[QString("mark")]);
    uiWidConfig.lineEdit_pattern->setText(configuration[QString("pattern")]);

    KConfigGroup cg = config();
    QString fontFamily = cg.readEntry("currentFontFamily", "Terminus");
    int fontSize = cg.readEntry("currentFontSize", 10);
    QString fontColor = cg.readEntry("currentFontColor", "#ff0000");
    int fontWeight = cg.readEntry("currentFontWeight", 400);
    QString fontStyle = cg.readEntry("currentFontStyle", "normal");
    QFont font = QFont(fontFamily, 12, 400, FALSE);
    uiAppConfig.fontComboBox_fontActive->setCurrentFont(font);
    uiAppConfig.spinBox_fontSizeActive->setValue(fontSize);
    uiAppConfig.kcolorcombo_fontColorActive->setColor(fontColor);
    uiAppConfig.spinBox_fontWeightActive->setValue(fontWeight);
    if (fontStyle == "normal")
        uiAppConfig.comboBox_fontStyleActive->setCurrentIndex(0);
    else if (fontStyle == "italic")
        uiAppConfig.comboBox_fontStyleActive->setCurrentIndex(1);

    fontFamily = cg.readEntry("fontFamily", "Terminus");
    fontSize = cg.readEntry("fontSize", 10);
    fontColor = cg.readEntry("fontColor", "#000000");
    fontWeight = cg.readEntry("fontWeight", 400);
    fontStyle = cg.readEntry("fontStyle", "normal");
    font = QFont(fontFamily, 12, 400, FALSE);
    uiAppConfig.fontComboBox_fontInactive->setCurrentFont(font);
    uiAppConfig.spinBox_fontSizeInactive->setValue(fontSize);
    uiAppConfig.kcolorcombo_fontColorInactive->setColor(fontColor);
    uiAppConfig.spinBox_fontWeightInactive->setValue(fontWeight);
    if (fontStyle == "normal")
        uiAppConfig.comboBox_fontStyleInactive->setCurrentIndex(0);
    else if (fontStyle == "italic")
        uiAppConfig.comboBox_fontStyleInactive->setCurrentIndex(1);

    parent->addPage(configWidget, i18n("Widget"), Applet::icon());
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void DesktopPanel::configAccepted()
{
    if (debug) qDebug() << "[PTM-DP]" << "[configAccepted]";
    extsysmonEngine->disconnectSource(QString("desktop"), this);
    KConfigGroup cg = config();

    cg.writeEntry("background", QString::number(uiWidConfig.checkBox_background->checkState()));
    cg.writeEntry("layout", QString::number(uiWidConfig.checkBox_layout->checkState()));
    cg.writeEntry("leftStretch", QString::number(uiWidConfig.checkBox_leftStretch->checkState()));
    cg.writeEntry("rightStretch", QString::number(uiWidConfig.checkBox_rightStretch->checkState()));
    cg.writeEntry("interval", QString::number(uiWidConfig.spinBox_interval->value()));
    cg.writeEntry("mark", uiWidConfig.lineEdit_mark->text());
    cg.writeEntry("pattern", uiWidConfig.lineEdit_pattern->text());

    cg.writeEntry("currentFontFamily", uiAppConfig.fontComboBox_fontActive->currentFont().family());
    cg.writeEntry("currentFontSize", uiAppConfig.spinBox_fontSizeActive->value());
    cg.writeEntry("currentFontColor", uiAppConfig.kcolorcombo_fontColorActive->color().name());
    cg.writeEntry("currentFontWeight", uiAppConfig.spinBox_fontWeightActive->value());
    cg.writeEntry("currentFontStyle", uiAppConfig.comboBox_fontStyleActive->currentText());

    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox_fontInactive->currentFont().family());
    cg.writeEntry("fontSize", uiAppConfig.spinBox_fontSizeInactive->value());
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo_fontColorInactive->color().name());
    cg.writeEntry("fontWeight", uiAppConfig.spinBox_fontWeightInactive->value());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_fontStyleInactive->currentText());
}


void DesktopPanel::configChanged()
{
    if (debug) qDebug() << "[PTM-DP]" << "[configChanged]";
    KConfigGroup cg = config();

    configuration[QString("background")] = cg.readEntry("background", "0");
    configuration[QString("interval")] = cg.readEntry("interval", "1000");
    configuration[QString("layout")] = cg.readEntry("layout", "0");
    configuration[QString("leftStretch")] = cg.readEntry("leftStretch", "2");
    configuration[QString("mark")] = cg.readEntry("mark", "*");
    configuration[QString("pattern")] = cg.readEntry("pattern", "[$mark$number/$total: $name]");
    configuration[QString("rightStretch")] = cg.readEntry("rightStretch", "2");

    desktopNames = getDesktopNames();

    extsysmonEngine->connectSource(QString("desktop"), this, configuration[QString("interval")].toInt());

    QString fontFamily = cg.readEntry("currentFontFamily", "Terminus");
    int fontSize = cg.readEntry("currentFontSize", 10);
    QString fontColor = cg.readEntry("currentFontColor", "#ff0000");
    int fontWeight = cg.readEntry("currentFontWeight", 400);
    QString fontStyle = cg.readEntry("currentFontStyle", "normal");
    currentFormatLine[0] = ("<p align=\"center\"><span style=\" font-family:'" + fontFamily + \
                     "'; font-style:" + fontStyle + \
                     "; font-size:" + QString::number(fontSize) + \
                     "pt; font-weight:" + QString::number(fontWeight) + \
                     "; color:" + fontColor + \
                     ";\"><pre>");
    currentFormatLine[1] = ("</pre></span></p>");

    fontFamily = cg.readEntry("fontFamily", "Terminus");
    fontSize = cg.readEntry("fontSize", 10);
    fontColor = cg.readEntry("fontColor", "#000000");
    fontWeight = cg.readEntry("fontWeight", 400);
    fontStyle = cg.readEntry("fontStyle", "normal");
    formatLine[0] = ("<p align=\"center\"><span style=\" font-family:'" + fontFamily + \
                     "'; font-style:" + fontStyle + \
                     "; font-size:" + QString::number(fontSize) + \
                     "pt; font-weight:" + QString::number(fontWeight) + \
                     "; color:" + fontColor + \
                     ";\"><pre>");
    formatLine[1] = ("</pre></span></p>");

    reinit();
}


#include "desktop-panel.moc"
