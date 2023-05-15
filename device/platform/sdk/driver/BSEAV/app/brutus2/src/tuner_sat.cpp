/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_sat.cpp $
* $brcm_Revision: 17 $
* $brcm_Date: 12/7/12 9:55a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_sat.cpp $
* 
* 17   12/7/12 9:55a tokushig
* SW7435-547: fix issue where qam scan terminates early because
* NEXUS_Frontend_GetQamStatus() is not available
* 
* 16   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 15   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 14   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 13   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 12   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 11   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 10   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 9   6/15/12 4:51p tokushig
* SW7231-749: update scan to handle error where intended tuner is
* unavailable.  also better handle display of progress meter so 100% is
* displayed before meter is hidden
* 
* 8   4/20/12 4:47p jrubio
* SW7346-644: fix scan
* 
* 7   4/20/12 1:30p jrubio
* SW7346-644: check in manual mode scan
* 
* 6   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 5   4/11/12 1:19p jrubio
* SW7346-644: add Peak Scan functionality
* 
* 4   3/27/12 10:31a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 3   3/19/12 4:23p tokushig
* SW7405-5581: During channel scan, only update the minor channel number
* if the found channel is deemed valid (unencrypted, valid pcr pid, not
* audio only).  Without this, the resulting channel list will contain
* gaps where the invalid channels are suppoed to be (10.1, 10.2, 10.7,
* etc).
* 
* 2   3/14/12 2:38p jrubio
* SW7346-644: fix live tuning
* 
* 1   3/14/12 12:48p jrubio
* SW7346-644: add tuner sat
* 
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_sat.h"
#include "tuner_sat.h"
#include "ts_psi.h"
#include "ts_pat.h"
#include "tspsimgr2.h"

BDBG_MODULE(brutus_tuner_sat);


/* debug function to print out results of channel scan */
static void printinfo(CHANNEL_INFO_T * info) 
{
	int i,j;

	BDBG_MSG((
		"\n"
        "CHANNEL_INFO_T\n"
        "==============\n"
		"version %d\n"
		"transport_stream_id %d\n"
		"sectionBitmap %d\n"
		"num_programs %d\n\n",
		info->version,
		info->transport_stream_id,
		info->sectionBitmap,
		info->num_programs));
	for (i=0;i<info->num_programs;i++) 
    {
        char str[255];
        char * pStr = NULL;

		BDBG_MSG(("program_number %d", info->program_info[i].program_number));
		BDBG_MSG(("  version %d", info->program_info[i].version));
		BDBG_MSG(("  pcr pid   : 0x%x", info->program_info[i].pcr_pid));
			
			
			
        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_video_pids;j++)
        {
			sprintf(pStr,
                " 0x%x/%d",
                info->program_info[i].video_pids[j].pid,
                info->program_info[i].video_pids[j].streamType);
            pStr = str + strlen(str);
        }
        BDBG_MSG(("  video_pids:%s", str));

        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_audio_pids;j++)
        {
			sprintf(pStr,
                " 0x%x/%d",
                info->program_info[i].audio_pids[j].pid,
                info->program_info[i].audio_pids[j].streamType);
            pStr = str + strlen(str);
        }
		BDBG_MSG(("  audio_pids:%s", str));

        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_other_pids;j++)
        {
			sprintf(pStr, 
                " 0x%x/%d",
                info->program_info[i].other_pids[j].pid,
                info->program_info[i].other_pids[j].streamType);
            pStr = str + strlen(str);
        }
		BDBG_MSG(("  other_pids:%s", str));
	}
}

static void tunerLockCallback(void * context, int param)
{
    CTuner * pTuner = (CTuner *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pTuner);

    B_Event_Set(pTuner->getLockEvent());

    /*NEXUS_Frontend_ResetStatus(pTuner->getFrontend());*/
}


void CTunerSatScanData::dump()
{
    MListItr <uint32_t> itr(&_freqList);
    uint32_t * pFreq = NULL;

    BDBG_WRN(("tuner type:%d", getTunerType()));
    BDBG_WRN(("List of Sat Scan Frequency Requests:"));
    for (pFreq = (uint32_t *)itr.first(); pFreq; pFreq = (uint32_t *)itr.next())
    {
        BDBG_WRN(("freq:%u", *pFreq));
    }

    BDBG_WRN(("append to channel list:%d", _appendToChannelList));
}


CTunerSat::CTunerSat(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CTuner(name, number, eBoardResource_frontendSds, pCfg)
{
    _scanThread_name = "CTunerSat_Scan";
}

CTunerSat::~CTunerSat()
{
}

eRet CTunerSat::tune(satSettings settings)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    settings.sat.lockCallback.callback = tunerLockCallback;
    settings.sat.lockCallback.context  = this;

    NEXUS_StartCallbacks(_frontend);
    B_Event_Reset(_lockEvent);
    B_Event_Reset(_waitEvent);

    NEXUS_Frontend_SetDiseqcSettings(_frontend, &settings.diseqc);
    BDBG_MSG(("Set DiseqcSettings\n"));

    nerror = NEXUS_Frontend_TuneSatellite(_frontend, &settings.sat);
    CHECK_NEXUS_ERROR_GOTO("unable to tune Sat frontend", ret, nerror, error);

error:
    return ret;
}

void CTunerSat::unTune()
{
    NEXUS_StopCallbacks(_frontend);
    NEXUS_Frontend_Untune(_frontend);
}

NEXUS_FrontendLockStatus CTunerSat::isLocked()
{
    eRet                     ret        = eRet_Ok;
    NEXUS_Error              nerror     = NEXUS_SUCCESS;
    NEXUS_FrontendFastStatus fastStatus;
    NEXUS_FrontendLockStatus lockStatus = NEXUS_FrontendLockStatus_eUnknown;

    nerror = NEXUS_Frontend_GetFastStatus(getFrontend(), &fastStatus);
    CHECK_NEXUS_WARN_GOTO("Error retrieving fast status", ret, nerror, error);

    BDBG_MSG(("fastStatus.lockStatus = %d, fastStatus.acquireInProgress = %d", 
              fastStatus.lockStatus, fastStatus.acquireInProgress));

    lockStatus         = fastStatus.lockStatus;
    _acquireInProgress = fastStatus.acquireInProgress;

error:
    return lockStatus;

#if 0
    eRet                     ret        = eRet_Ok;
    NEXUS_Error              nerror     = NEXUS_SUCCESS;
    NEXUS_FrontendSatelliteStatus  status;
    bool                     locked     = false;

    /* DTT - possible problem with NEXUS_Frontend_GetSatelliteStatus() so for now we'll rely on the
       status received during the last lock changed event */
    return _locked;

    nerror = NEXUS_Frontend_GetSatelliteStatus(getFrontend(), &status);
    if (NEXUS_SUCCESS == nerror)
    {
        locked = status.demodLocked;
    }

    return locked;
#endif
}


/* Do a Blind Scan */

#include "nexus_frontend.h"
#include "nexus_frontend_satellite.h"

static void peakscan_callback(void *context, int param) 
{
    CTunerSat * pTuner = (CTunerSat *)context;
    NEXUS_FrontendSatellitePeakscanResult result;
    NEXUS_Error rc;

    BSTD_UNUSED(param);

    rc = NEXUS_Frontend_GetSatellitePeakscanResult(pTuner->getFrontend(), &result);
    BDBG_ASSERT(!rc);
    BDBG_WRN(("Peak scan callback. freq=%u, symRate=%u, power=%#x\n", 
        result.peakFrequency, result.symbolRate, result.peakPower));
    pTuner->_scanThread_psResult = result;
    BKNI_SetEvent(pTuner->_scanThread_psCallbackEvent);
}


void CTunerSat::saveScanData(CTunerScanData * pScanData)
{
    _scanData = *((CTunerSatScanData *)pScanData);
}

void CTunerSat::doScan()
{
    eRet                         ret             = eRet_Ok;
    uint16_t                     major           = 1;
    uint16_t                     minor           = 1;
    uint16_t                     numFreqToScan   = 0;
    uint16_t                     numFreqScanned  = 0;
    CModelChannelSat             chSat(this);         /* temp channel we'll use to do tuning during scan */
    CTunerScanNotificationData   notifyData(this);    /* notification data for reporting scan start/stop/progress */
    MListItr <uint32_t>          freqListItr(&(_scanData._freqList));
    uint32_t                   * pFreq           = NULL;
    NEXUS_Error rc;

    BDBG_ASSERT(NULL != _scanThread_handle);

    numFreqToScan = _scanData._freqList.total();
    if (0 == numFreqToScan)
    {
        /* nothing to scan so we are  */
        goto error;
    }

    /* set the starting major channel number for newly found channels */
    major = _scanData._majorStartNum;

    notifyObservers(eNotify_ScanStarted, &notifyData);
    notifyObservers(eNotify_ScanProgress, &notifyData);

    /* loop through all frequencies in given freq list */
#if 1
    for (pFreq = freqListItr.first(); pFreq; pFreq = freqListItr.next())
    {

        satSettings settings;

        /* handle default settings for optional paramters */
        settings            = chSat.getSettings();
        settings.sat.frequency  = *pFreq*1000000;
        settings.sat.mode       = (-1 != _scanData._mode) ? (NEXUS_FrontendSatelliteMode)_scanData._mode 
                                                          : NEXUS_FrontendSatelliteMode_eDvb;

        settings.diseqc.enabled = true;
        settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
        settings.diseqc.toneEnabled = true;
        settings.sat.symbolRate = 20000000;

        
#else /* Peak Scan */
    while (1) 
    {
        satSettings settings;
        NEXUS_FrontendSatellitePeakscanSettings psSettings;

        /* Set Diseqc Settings */
        settings.diseqc.enabled = true;
        settings.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
        settings.diseqc.toneEnabled = true;

        BKNI_CreateEvent(&_scanThread_psCallbackEvent);
        NEXUS_Frontend_GetDefaultSatellitePeakscanSettings(&psSettings);
        psSettings.frequency = (*freqListItr.first())*10000000;
        psSettings.frequencyRange = ((*freqListItr.last())*10000000) - (*freqListItr.first())*10000000;
        /* Step in 1 Mhz */
        psSettings.peakscanCallback.callback = peakscan_callback;
        psSettings.peakscanCallback.context = this;

        rc = NEXUS_Frontend_SatellitePeakscan(getFrontend(), &psSettings);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(_scanThread_psCallbackEvent, 3000);
        if (rc) 
        {
            BDBG_ERR(("Callback not fired. Aborting\n"));
            goto errorEvent;
        }

            settings.sat.mode = NEXUS_FrontendSatelliteMode_eBlindAcquisition;
            settings.sat.frequency = _scanThread_psResult.peakFrequency;
            settings.sat.symbolRate = _scanThread_psResult.symbolRate;
            settings.sat.carrierOffset = 0;

         if (!_scanThread_psResult.peakFrequency) 
         {
             BDBG_WRN(("No signal found. Continuing peak scan...\n"));
             psSettings.frequency = _scanThread_psResult.lastFrequency + psSettings.frequencyRange + psSettings.frequencyStep;
             continue;
         }

#endif
       BDBG_MSG(("Frequency %d",settings.sat.frequency));
       chSat.setSettings(settings);
       chSat.setTransportType(NEXUS_TransportType_eTs);

        /* try to tune */
        ret = chSat.tune(_scanThread_id, _pModelConfig, true);
        if (eRet_Ok == ret)
        {
            BERR_Code                     err         = BERR_SUCCESS;
            CParserBand                 * pBand       = chSat.getParserBand();
            CHANNEL_INFO_T                chanInfo;
            NEXUS_FrontendSatelliteStatus   scanStatus;

            /* tune successful */

            chSat.setMajor(major);
            if (0 < chSat.addPsiPrograms(_scanThread_callback, _scanThread_context))
            {
                /* found channels so increment major channel number */
                major++;
            }
        }
        else
        {
            /* tune failed */

            BDBG_MSG(("NO sat channel found at freq:%u", chSat.getFrequency()));
        }

        chSat.unTune(_pModelConfig, false);
#if 1
        numFreqScanned++;
        notifyData.setPercent(100 * numFreqScanned / numFreqToScan);
#else
        psSettings.frequency = _scanThread_psResult.lastFrequency + psSettings.frequencyRange + psSettings.frequencyStep;
#endif
        notifyObservers(eNotify_ScanProgress, &notifyData);
    }

errorEvent:
#if 0
BKNI_DestroyEvent(_scanThread_psCallbackEvent);
#endif
error:
    if (chSat.isTuned())
    {
        chSat.unTune(_pModelConfig);
    }
    scanDone(&notifyData);
}

CModelChannel * CTunerSat::createChannel()
{
    CModelChannelSat * pChannel = NULL;

    pChannel = new CModelChannelSat(this);
    return pChannel;
}

void CTunerSat::destroyChannel(CModelChannel * pChannel)
{
    DEL(pChannel);
}

#endif /* NEXUS_HAS_FRONTEND */
