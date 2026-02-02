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

#pragma once

#include "awpluginmatcherac.h"
#include "awpluginmatcherbattery.h"
#include "awpluginmatcherbrightness.h"
#include "awpluginmatchercpu.h"
#include "awpluginmatchercpucore.h"
#include "awpluginmatchercpufrequency.h"
#include "awpluginmatchercpufrequencycore.h"
#include "awpluginmatchercustom.h"
#include "awpluginmatcherdesktop.h"
#include "awpluginmatcherdesktopcount.h"
#include "awpluginmatcherdesktopnumber.h"
#include "awpluginmatchergpu.h"
#include "awpluginmatchergpucore.h"
#include "awpluginmatchergpumemorytotal.h"
#include "awpluginmatchergpumemorytotalcore.h"
#include "awpluginmatchergpumemoryused.h"
#include "awpluginmatchergpumemoryusedcore.h"
#include "awpluginmatchergputemperature.h"
#include "awpluginmatcherhdd.h"
#include "awpluginmatcherhddfree.h"
#include "awpluginmatcherhddread.h"
#include "awpluginmatcherhddtotal.h"
#include "awpluginmatcherhddused.h"
#include "awpluginmatcherhddwrite.h"
#include "awpluginmatcherload.h"
#include "awpluginmatcherloadaverage.h"
#include "awpluginmatchermemory.h"
#include "awpluginmatchermemoryapplication.h"
#include "awpluginmatchermemoryfree.h"
#include "awpluginmatchermemorytotal.h"
#include "awpluginmatchermemoryused.h"
#include "awpluginmatchernetwork.h"
#include "awpluginmatchernetworkdevice.h"
#include "awpluginmatchernetworkssid.h"
#include "awpluginmatchernetworktotal.h"
#include "awpluginmatcherplayer.h"
#include "awpluginmatcherps.h"
#include "awpluginmatcherpsprocesses.h"
#include "awpluginmatcherpstotal.h"
#include "awpluginmatcherquotes.h"
#include "awpluginmatcherrequest.h"
#include "awpluginmatchersensors.h"
#include "awpluginmatcherswap.h"
#include "awpluginmatcherswapfree.h"
#include "awpluginmatcherswaptotal.h"
#include "awpluginmatcherswapused.h"
#include "awpluginmatchertime.h"
#include "awpluginmatcherupgrade.h"
#include "awpluginmatcheruptime.h"
#include "awpluginmatchervolume.h"
#include "awpluginmatcherweather.h"


namespace AWPluginMatchers
{
static QList<AWPluginMatcherInterface *> matchers = {
    AWPluginMatcherAC::instance(),
    AWPluginMatcherBattery::instance(),
    AWPluginMatcherBrightness::instance(),
    AWPluginMatcherCPU::instance(),
    AWPluginMatcherCPUCore::instance(),
    AWPluginMatcherCPUFrequency::instance(),
    AWPluginMatcherCPUFrequencyCore::instance(),
    AWPluginMatcherCustom::instance(),
    AWPluginMatcherDesktop::instance(),
    AWPluginMatcherDesktopCount::instance(),
    AWPluginMatcherDesktopNumber::instance(),
    AWPluginMatcherGPU::instance(),
    AWPluginMatcherGPUCore::instance(),
    AWPluginMatcherGPUMemoryTotal::instance(),
    AWPluginMatcherGPUMemoryTotalCore::instance(),
    AWPluginMatcherGPUMemoryUsed::instance(),
    AWPluginMatcherGPUMemoryUsedCore::instance(),
    AWPluginMatcherGPUTemperature::instance(),
    AWPluginMatcherHDD::instance(),
    AWPluginMatcherHDDFree::instance(),
    AWPluginMatcherHDDRead::instance(),
    AWPluginMatcherHDDTotal::instance(),
    AWPluginMatcherHDDUsed::instance(),
    AWPluginMatcherHDDWrite::instance(),
    AWPluginMatcherLoad::instance(),
    AWPluginMatcherLoadAverage::instance(),
    AWPluginMatcherMemory::instance(),
    AWPluginMatcherMemoryApplication::instance(),
    AWPluginMatcherMemoryFree::instance(),
    AWPluginMatcherMemoryTotal::instance(),
    AWPluginMatcherMemoryUsed::instance(),
    AWPluginMatcherNetwork::instance(),
    AWPluginMatcherNetworkDevice::instance(),
    AWPluginMatcherNetworkSSID::instance(),
    AWPluginMatcherNetworkTotal::instance(),
    AWPluginMatcherPlayer::instance(),
    AWPluginMatcherPS::instance(),
    AWPluginMatcherPSProcesses::instance(),
    AWPluginMatcherPSTotal::instance(),
    AWPluginMatcherQuotes::instance(),
    AWPluginMatcherRequest::instance(),
    AWPluginMatcherSensors::instance(),
    AWPluginMatcherSwap::instance(),
    AWPluginMatcherSwapFree::instance(),
    AWPluginMatcherSwapTotal::instance(),
    AWPluginMatcherSwapUsed::instance(),
    AWPluginMatcherTime::instance(),
    AWPluginMatcherUpgrade::instance(),
    AWPluginMatcherUptime::instance(),
    AWPluginMatcherVolume::instance(),
    AWPluginMatcherWeather::instance(),
};
};
