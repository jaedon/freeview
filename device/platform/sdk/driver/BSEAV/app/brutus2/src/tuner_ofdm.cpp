/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_ofdm.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 12/7/12 9:55a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_ofdm.cpp $
* 
* 11   12/7/12 9:55a tokushig
* SW7435-547: fix issue where qam scan terminates early because
* NEXUS_Frontend_GetQamStatus() is not available
* 
* 10   12/5/12 7:03p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 9   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 8   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 7   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 6   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 5   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 3   8/27/12 10:08a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 3   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 2   7/27/12 11:21a jrubio
* SW7346-848: add ofdm PAT/PMT timeouts
* 
* 1   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_ofdm.h"
#include "tuner_ofdm.h"
#include "ts_psi.h"
#include "ts_pat.h"

#include "nexus_frontend.h"
#include "nexus_frontend_ofdm.h"

BDBG_MODULE(brutus_tuner_ofdm);


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
    CTuner                  * pTuner = (CTuner *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pTuner);

    B_Event_Set(pTuner->getLockEvent());

    /*NEXUS_Frontend_ResetStatus(pTuner->getFrontend());*/
}


void CTunerOfdmScanData::dump()
{
    MListItr <uint32_t> itr(&_freqList);
    uint32_t * pFreq = NULL;

    BDBG_WRN(("tuner type:%d", getTunerType()));
    BDBG_WRN(("List of Ofdm Scan Frequency Requests:"));
    for (pFreq = (uint32_t *)itr.first(); pFreq; pFreq = (uint32_t *)itr.next())
    {
        BDBG_WRN(("freq:%u", *pFreq));
    }

    BDBG_WRN(("append to channel list:%d", _appendToChannelList));
}


CTunerOfdm::CTunerOfdm(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CTuner(name, number, eBoardResource_frontendOfdm, pCfg)
{
    _scanThread_name = "CTunerOfdm_Scan";
}

CTunerOfdm::~CTunerOfdm()
{
}

eRet CTunerOfdm::tune(NEXUS_FrontendOfdmSettings settings)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    settings.lockCallback.callback = tunerLockCallback;
    settings.lockCallback.context  = this;

    NEXUS_StartCallbacks(_frontend);
    B_Event_Reset(_lockEvent);
    B_Event_Reset(_waitEvent);

    nerror = NEXUS_Frontend_TuneOfdm(_frontend, &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to tune Ofdm frontend", ret, nerror, error);

error:
    return ret;
}

void CTunerOfdm::unTune()
{
    NEXUS_StopCallbacks(_frontend);
    NEXUS_Frontend_Untune(_frontend);
}

NEXUS_FrontendLockStatus CTunerOfdm::isLocked()
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
}

/* get PSI channel info for current tuned channel */
eRet CTunerOfdm::getChannelInfo(CParserBand * pBand, CHANNEL_INFO_T * pChanInfo, bool bScanning)
{
    BERR_Code err            = BERR_SUCCESS;
    int       patTimeout     = GET_INT(_pCfg, TUNE_OFDM_PAT_TIMEOUT)/10; /* in tsPsi_setTimeout2() this is 500msecs */
    int       patTimeoutOrig = 0;
    int       pmtTimeout     = GET_INT(_pCfg, TUNE_OFDM_PMT_TIMEOUT)/10; /* in tsPsi_setTimeout2() this is 500msecs */
    int       pmtTimeoutOrig = 0;

    if (true == bScanning)
    {
        /* adjust pat/pmt timeouts for faster scanning */
        tsPsi_getTimeout2(&patTimeoutOrig, &pmtTimeoutOrig);
        tsPsi_setTimeout2(patTimeout, pmtTimeout);
    }

    err = tsPsi_getChannelInfo2(pChanInfo, pBand->getBand());

    if (true == bScanning)
    {
        /* restore default pat/pmt timeouts */
        tsPsi_setTimeout2(patTimeoutOrig, pmtTimeoutOrig);
    }

    return ((BERR_SUCCESS == err) ? eRet_Ok : eRet_ExternalError);
}

NEXUS_FrontendOfdmMode CTunerOfdm::getDefaultMode()
{
    if (true == _capabilities.ofdmModes[NEXUS_FrontendOfdmMode_eIsdbt])
    {
        return NEXUS_FrontendOfdmMode_eIsdbt;
    }
    else
    if (true == _capabilities.ofdmModes[NEXUS_FrontendOfdmMode_eDvbt])
    {
        return NEXUS_FrontendOfdmMode_eDvbt;
    }
    else
    if (true == _capabilities.ofdmModes[NEXUS_FrontendOfdmMode_eDvbt2])
    {
        return NEXUS_FrontendOfdmMode_eDvbt2;
    }
    else
    {
        return NEXUS_FrontendOfdmMode_eMax;
    }
}


void CTunerOfdm::saveScanData(CTunerScanData * pScanData)
{
    _scanData = *((CTunerOfdmScanData *)pScanData);
}


void CTunerOfdm::doScan()
{
    eRet                         ret                   = eRet_Ok;
    uint16_t                     major                 = 1;
    uint16_t                     numFreqToScan         = 0;
    uint16_t                     numFreqScanned        = 0;
    CModelChannelOfdm            chOfdm(this);           /* temp channel we'll use to do tuning during scan */
    CTunerScanNotificationData   notifyData(this);       /* notification data for reporting scan start/stop/progress */
    uint32_t                   * pFreq                 = NULL;
    MListItr <uint32_t>          freqListItr(&(_scanData._freqList));

    BDBG_ASSERT(NULL != _scanThread_handle);

    numFreqToScan = _scanData._freqList.total();
    if (0 == numFreqToScan)
    {
        /* nothing to scan so we are done */
        BDBG_WRN(("No scan frequencies given!"));
        goto error;
    }

    /* check to see if fast status is supported */
    {
        NEXUS_FrontendFastStatus fastStatus;
        NEXUS_Error              nerror      = NEXUS_SUCCESS;

        nerror = NEXUS_Frontend_GetFastStatus(getFrontend(), &fastStatus);
        if (NEXUS_NOT_SUPPORTED != nerror)
        {
            nerror = NEXUS_SUCCESS;
        }
        CHECK_NEXUS_ERROR_GOTO("fast status is not supported - aborting scan", ret, nerror, error);
    }

    /* set the starting major channel number for newly found channels */
    major = _scanData._majorStartNum;

    notifyObservers(eNotify_ScanStarted, &notifyData);
    notifyObservers(eNotify_ScanProgress, &notifyData);

    /* loop through all frequencies in given freq list */
    for (pFreq = freqListItr.first(); pFreq; pFreq = freqListItr.next())
    {
        NEXUS_FrontendOfdmSettings settings;

        /* handle default settings for optional paramters */
        settings                 = chOfdm.getSettings();
        settings.frequency       = *pFreq;
        settings.acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
        settings.terrestrial     = true;
        settings.spectrum        = NEXUS_FrontendOfdmSpectrum_eAuto;
        settings.mode            = (-1 != _scanData._mode) ? (NEXUS_FrontendOfdmMode)_scanData._mode 
                                                           : getDefaultMode();
        settings.bandwidth       = (-1 != _scanData._bandwidth) ? _scanData._bandwidth
                                                                : 6000000;
        chOfdm.setSettings(settings);
        chOfdm.setTransportType(NEXUS_TransportType_eTs);

        /* try to tune */
        ret = chOfdm.tune(_scanThread_id, _pModelConfig, true);
        if (eRet_Ok == ret)
        {
            BERR_Code                     err         = BERR_SUCCESS;
            CParserBand                 * pBand       = chOfdm.getParserBand();
            CHANNEL_INFO_T                chanInfo;
            NEXUS_FrontendOfdmStatus      status;
            NEXUS_Error                   nerror      = NEXUS_SUCCESS;

            /* tune successful */
            nerror = NEXUS_Frontend_GetOfdmStatus(getFrontend(), &status);
            if (eRet_Ok == CHECK_NEXUS_ERROR("Error retrieving OFDM scan status", nerror))
            {
                BDBG_WRN(("Scan Status: "));
                BDBG_WRN(("Mode = %d", status.settings.mode));
                BDBG_WRN(("Spectrum inverted = %s", status.spectrumInverted?"True":"False"));
                BDBG_WRN(("Modulation type:%d", status.modulation));
                if (NEXUS_FrontendOfdmMode_eIsdbt == status.settings.mode)
                {
                    BDBG_WRN(("ISDB-T Layer A modulation type:%d", status.modulationA));
                    BDBG_WRN(("ISDB-T Layer B modulation type:%d", status.modulationB));
                    BDBG_WRN(("ISDB-T Layer C modulation type:%d", status.modulationC));
                }
            }
        
            chOfdm.setMajor(major);

            if (0 < chOfdm.addPsiPrograms(_scanThread_callback, _scanThread_context))
            {
                /* found channels so increment major channel number */
                major++;
            }
        }
        else
        {
            /* tune failed */
            BDBG_MSG(("NO Ofdm channel found at freq:%u", chOfdm.getFrequency()));
        }

        chOfdm.unTune(_pModelConfig, false);

        numFreqScanned++;
        notifyData.setPercent(100 * numFreqScanned / numFreqToScan);
        notifyObservers(eNotify_ScanProgress, &notifyData);
    }

error:
    if (chOfdm.isTuned())
    {
        chOfdm.unTune(_pModelConfig);
    }
    scanDone(&notifyData);
}

#endif /* NEXUS_HAS_FRONTEND */