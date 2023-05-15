/***************************************************************************
* Copyright (c) 2004-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_frontend.cpp $
* $brcm_Revision: 65 $
* $brcm_Date: 3/21/12 12:01p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus/src/brutus_frontend.cpp $
* 
* 65   3/21/12 12:01p erickson
* SW7425-2664: remove old code
* 
* 64   7/2/09 10:57a erickson
* PR56558: defer frontend init and channel scan until after the GUI is up
*
 ************************************************************************/
#include "brutus_app.h"
#include "cfgsettings.h"
#include "psichannelscan.h"
#include <bstd.h>

BDBG_MODULE(brutus_frontend);

ChannelManager *createChannelManager(const Config *cfg)
{
    ChannelManager::Settings settings;
    ChannelScan::Settings scanSettings;

    ChannelScan::initialize(&scanSettings);
    scanSettings.filterOutEncrypted = !cfg->getBool(Config::ENCRYPTED_CHANNELS_ENABLED);
    scanSettings.filterOutAudioOnly = !cfg->getBool(Config::AUDIO_ONLY_CHANNELS_ENABLED);
    scanSettings.filterOutDuplicatePids = !cfg->getBool(Config::DUPLICATE_CHANNELS_ENABLED);
    scanSettings.filterOutDataPids = !cfg->getBool(Config::DATA_CHANNELS_ENABLED);
    scanSettings.demuxPrograms = cfg->getBool(Config::DEMUX_PROGRAMS);

    /* Open frontend objects with appropriate links */
    ChannelMap *map = new ChannelMap();
    ChannelScan *scan;
    scan = new PSIChannelScan(map, &scanSettings);
    ChannelManager *channelManager = new ChannelManager(map, scan);

    channelManager->getSettings(&settings);
    settings.audioDecodeEnabled = cfg->getBool(Config::AUDIODECODE_ENABLED);
    settings.videoDecodeEnabled = cfg->getBool(Config::VIDEODECODE_ENABLED);
    settings.sapEnabled = cfg->getBool(Config::BTSC_SAP_ENABLED);
    settings.overrideBand = cfg->getInt("OVERRIDE_BAND", -1);
    settings.forceUseOfDaughterCards = cfg->getBool("FORCE_USE_OF_DAUGHTERCARDS");
    channelManager->setSettings(&settings);

    if (cfg->getBool("IMMEDIATE_FRONTEND")) {
        /* init the frontend before putting up the GUI */
        bsettop_init_frontend();
        channelManager->populateFrontEnd();
    }

    /* Read in the channel map. The particular implementation of ChannelScan
    may not need or support a channel map file. */
    scan->read(cfg->get(Config::CHANNEL_MAP),cfg->get(Config::IPSERVER_IPV4));

    return channelManager;
}

void destroyChannelManager(ChannelManager *channelManager)
{
    delete channelManager->map();
    delete channelManager->channelScan();
    delete channelManager;
}

