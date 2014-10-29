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

#include <QDebug>
#include <QDir>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


ExtScript::ExtScript(const QString scriptName, const QStringList directories, const bool debugCmd)
      : QObject(0),
        name(scriptName),
        dirs(directories),
        debug(debugCmd)
{
}


ExtScript::~ExtScript()
{
    if (debug) qDebug() << PDEBUG;
}


void ExtScript::addDirectory(const QString dir)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Directory" << dir;

    QString absPath = QDir(dir).absolutePath();
    if (!QDir(absPath).exists()) return;
    for (int i=0; i<dirs.count(); i++)
        if (dirs[i] == absPath) return;

    dirs.append(absPath);
}


QStringList ExtScript::directories()
{
    if (debug) qDebug() << PDEBUG;

    return dirs;
}


int ExtScript::getInterval()
{
    if (debug) qDebug() << PDEBUG;

    return interval;
}


QString ExtScript::getPrefix()
{
    if (debug) qDebug() << PDEBUG;

    return prefix;
}


ExtScript::Redirect ExtScript::getRedirect()
{
    if (debug) qDebug() << PDEBUG;

    return redirect;
}


bool ExtScript::hasOutput()
{
    if (debug) qDebug() << PDEBUG;

    return output;
}


bool ExtScript::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return active;
}


void ExtScript::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    active = state;
}


void ExtScript::setDirectories(const QStringList directories)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Directories" << directories;

    for (int i=0; i<directories.count(); i++)
        addDirectory(directories[i]);
}


void ExtScript::setHasOutput(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    output = state;
}


void ExtScript::setInterval(const int inter)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << inter;

    if (inter <= 0) return;

    interval = inter;
}


void ExtScript::setPrefix(const QString pref)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Prefix" << pref;

    prefix = pref;
}


void ExtScript::setRedirect(const Redirect redir)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Redirect" << redir;

    redirect = redir;
}


void ExtScript::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    for (int i=0; i<dirs.count(); i++) {
        if (!QDir(dirs[i]).entryList().contains(name + QString(".conf"))) continue;
        QString fileName = dirs[i] + QDir::separator() + name + QString(".conf");
        QMap<QString, QString> newSettings = getConfigurationFromFile(fileName);
        for (int i=0; i<newSettings.keys().count(); i++)
            settings[newSettings.keys()[i]] = newSettings[newSettings.keys()[i]];
    }

    fromExternalConfiguration(settings);
}


void ExtScript::fromExternalConfiguration(const QMap<QString, QString> settings)
{
    if (settings.contains(QString("ACTIVE"))) {
        if (settings[QString("ACTIVE")] == QString("true"))
            setActive(true);
        else
            setActive(false);
    }
    if (settings.contains(QString("INTERVAL"))) {
        setInterval(settings[QString("INTERVAL")].toInt());
    }
    if (settings.contains(QString("PREFIX"))) {
        setPrefix(settings[QString("PREFIX")]);
    }
    if (settings.contains(QString("OUTPUT"))) {
        if (settings[QString("OUTPUT")] == QString("true"))
            setHasOutput(true);
        else
            setHasOutput(false);
    }
    if (settings.contains(QString("REDIRECT"))) {
        setRedirect((Redirect)settings[QString("REDIRECT")].toInt());
    }
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
}
