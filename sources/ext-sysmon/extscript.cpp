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

#include "extscript.h"
#include "ui_extscript.h"

#include <QDebug>
#include <QDir>
#include <QTextCodec>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


ExtScript::ExtScript(QWidget *parent, const QString scriptName, const QStringList directories, const bool debugCmd) :
    QDialog(parent),
    name(scriptName),
    dirs(directories),
    debug(debugCmd),
    ui(new Ui::ExtScript)
{
    readConfiguration();
    ui->setupUi(this);
}


ExtScript::~ExtScript()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


int ExtScript::getInterval()
{
    if (debug) qDebug() << PDEBUG;

    return interval;
}


QString ExtScript::getName()
{
    if (debug) qDebug() << PDEBUG;

    return name;
}


QString ExtScript::getPrefix()
{
    if (debug) qDebug() << PDEBUG;

    return _prefix;
}


ExtScript::Redirect ExtScript::getRedirect()
{
    if (debug) qDebug() << PDEBUG;

    return _redirect;
}


bool ExtScript::hasOutput()
{
    if (debug) qDebug() << PDEBUG;

    return _output;
}


bool ExtScript::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return _active;
}


void ExtScript::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    _active = state;
}


void ExtScript::setHasOutput(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    _output = state;
}


void ExtScript::setInterval(const int interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << interval;
    if (interval <= 0) return;

    _interval = interval;
}


void ExtScript::setPrefix(const QString prefix)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Prefix" << prefix;

    _prefix = prefix;
}


void ExtScript::setRedirect(const QString redirect)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Redirect" << redirect;

    if (redirect == QString("stdout2sdterr"))
        _redirect = stdout2stderr;
    else if (redirect == QString("stderr2sdtout"))
        _redirect = stderr2stdout;
    else
        _redirect = nothing;
}


void ExtScript::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    for (int i=dirs.count()-1; i>=0; i--) {
        if (!QDir(dirs[i]).entryList(QDir::Files).contains(name + QString(".conf"))) continue;
        QString fileName = dirs[i] + QDir::separator() + name + QString(".conf");
        QMap<QString, QString> newSettings = getConfigurationFromFile(fileName);
        for (int i=0; i<newSettings.keys().count(); i++)
            settings[newSettings.keys()[i]] = newSettings[newSettings.keys()[i]];
    }

    fromExternalConfiguration(settings);
}


ExtScript::ScriptData ExtScript::run(const int time)
{
    if (debug) qDebug() << PDEBUG;

    ScriptData response;
    response.active = _active;
    response.name = name;
    response.refresh = false;
    if (!_active) return response;
    if (time != _interval) return response;
    response.refresh = true;

    QStringList cmdList;
    if (!_prefix.isEmpty())
        cmdList.append(_prefix);
    QString fullPath = name;
    for (int i=0; i<dirs.count(); i++) {
        if (!QDir(dirs[i]).entryList(QDir::Files).contains(name)) continue;
        fullPath = dirs[i] + QDir::separator() + name;
        break;
    }
    cmdList.append(fullPath);
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmdList.join(QChar(' '));
    TaskResult process = runTask(cmdList.join(QChar(' ')));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString info = QTextCodec::codecForMib(106)->toUnicode(process.error).trimmed();
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    switch(_redirect) {
    case stdout2stderr:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << info;
        if (debug) qDebug() << PDEBUG << ":" << "Output" << qoutput;
        break;
    case stderr2stdout:
        response.output = info + QString("\t") + qoutput;
        break;
    default:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << info;
        response.output = qoutput;
        break;
    }
    if (!_output)
        response.output = QString::number(process.exitCode);

    return response;
}


void ExtScript::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;
}


void ExtScript::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<dirs.count(); i++) {
        QString fileName = dirs[i] + QDir::separator() + name;
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << fileName << QFile::remove(fileName);
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << fileName + QString(".conf") <<
                               QFile::remove(fileName + QString(".conf"));
    }
}


void ExtScript::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = dirs[0] + QDir::separator() + name + QString(".conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::WriteOnly)) return;
    QMap<QString, QString> config = toExternalConfiguration();
    for (int i=0; i<config.keys().count(); i++) {
        QByteArray string = (config.keys()[i] + QString("=") + config[config.keys()[i]] + QString("\n")).toUtf8();
        configFile.write(string);
    }
    configFile.close();
}


void ExtScript::fromExternalConfiguration(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;

    if (settings.contains(QString("ACTIVE")))
        _active = (settings[QString("ACTIVE")] == QString("true"));
    if (settings.contains(QString("INTERVAL")))
        _interval = settings[QString("INTERVAL")].toInt();
    if (settings.contains(QString("PREFIX")))
        _prefix = settings[QString("PREFIX")];
    if (settings.contains(QString("OUTPUT")))
        _output = (settings[QString("OUTPUT")] == QString("true"));
    if (settings.contains(QString("REDIRECT")))
        _redirect = (Redirect)settings[QString("REDIRECT")].toInt();
    if (!_output)
        _redirect = stdout2stderr;
}


QMap<QString, QString> ExtScript::getConfigurationFromFile(const QString fileName)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "File" << fileName;

    QMap<QString, QString> settings;
    QFile configFile(fileName);
    QString fileStr;
    if (!configFile.open(QIODevice::ReadOnly)) return settings;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('=')))
            settings[fileStr.split(QChar('='))[0]] = fileStr.split(QChar('='))[1];
        if (configFile.atEnd()) break;
    }
    configFile.close();
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


QMap<QString, QString> ExtScript::toExternalConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    if (_active)
        settings[QString("ACTIVE")] = QString("true");
    else
        settings[QString("ACTIVE")] = QString("false");
    settings[QString("INTERVAL")] = QString::number(_interval);
    settings[QString("PREFIX")] = _prefix;
    if (_output)
        settings[QString("OUTPUT")] = QString("true");
    else
        settings[QString("OUTPUT")] = QString("false");
    settings[QString("REDIRECT")] = QString::number(_redirect);

    return settings;
}
