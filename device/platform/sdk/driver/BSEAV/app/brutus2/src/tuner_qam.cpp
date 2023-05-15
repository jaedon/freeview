/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_qam.cpp $
* $brcm_Revision: 23 $
* $brcm_Date: 12/7/12 6:06p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_qam.cpp $
* 
* 23   12/7/12 6:06p tokushig
* SW7435-547: handle error case where auto mode is not supported during
* qam scan even tho the tuner capabilities says it is supported
* 
* 21   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 20   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 19   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 18   11/13/12 10:33a tokushig
* SW7231-749: fix typo
* 
* 17   11/13/12 9:56a tokushig
* SW7231-749: fix typo during merge
* 
* 16   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 15   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 14   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 13   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 12   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 11   6/15/12 4:51p tokushig
* SW7231-749: update scan to handle error where intended tuner is
* unavailable.  also better handle display of progress meter so 100% is
* displayed before meter is hidden
* 
* 10   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 9   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 8   4/6/12 10:05a tokushig
* SW7405-5581: comment change
* 
* 7   4/5/12 11:40a tokushig
* SW7405-5581: optimization for 7405 qam scan
* 
* 6   3/30/12 9:37p tokushig
* SW7231-749: modify fast qam scan and test on 97231
* 
* 5   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 4   3/19/12 4:23p tokushig
* SW7405-5581: During channel scan, only update the minor channel number
* if the found channel is deemed valid (unencrypted, valid pcr pid, not
* audio only).  Without this, the resulting channel list will contain
* gaps where the invalid channels are suppoed to be (10.1, 10.2, 10.7,
* etc).
* 
* 3   3/9/12 2:58p tokushig
* SW7405-5581: added fast scan capability for qam scan - not supported on
* 7405 but scan will still work.  further testing required on 7231.
* 
* 2   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 1   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND

#include "channel_qam.h"
#include "tuner_qam.h"
#include "ts_psi.h"
#include "ts_pat.h"

#include "nexus_frontend.h"
#include "nexus_frontend_qam.h"

BDBG_MODULE(brutus_tuner_qam);


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
    eRet                      ret    = eRet_Ok;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pTuner);

    B_Event_Set(pTuner->getLockEvent());

    /*NEXUS_Frontend_ResetStatus(pTuner->getFrontend());*/
}


void CTunerQamScanData::dump()
{
    MListItr <uint32_t> itr(&_freqList);
    uint32_t * pFreq = NULL;

    BDBG_WRN(("tuner type:%d", getTunerType()));
    BDBG_WRN(("List of QAM Scan Frequency Requests:"));
    for (pFreq = (uint32_t *)itr.first(); pFreq; pFreq = (uint32_t *)itr.next())
    {
        BDBG_WRN(("freq:%u", *pFreq));
    }

    BDBG_WRN(("append to channel list:%d", _appendToChannelList));
}


CTunerQam::CTunerQam(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CTuner(name, number, eBoardResource_frontendQam, pCfg)
{
    _scanThread_name = "CTunerQam_Scan";
}

CTunerQam::~CTunerQam()
{
}

eRet CTunerQam::tune(NEXUS_FrontendQamSettings settings)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    settings.lockCallback.callback = tunerLockCallback;
    settings.lockCallback.context  = this;

    NEXUS_StartCallbacks(_frontend);
    B_Event_Reset(_lockEvent);
    B_Event_Reset(_waitEvent);

    nerror = NEXUS_Frontend_TuneQam(_frontend, &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to tune QAM frontend", ret, nerror, error);

error:
    if (NEXUS_NOT_SUPPORTED == nerror)
    {
        ret = eRet_NotSupported;
    }
    return ret;
}

void CTunerQam::unTune()
{
    NEXUS_StopCallbacks(_frontend);
    NEXUS_Frontend_Untune(_frontend);
}

NEXUS_FrontendLockStatus CTunerQam::isLocked()
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

NEXUS_FrontendQamMode CTunerQam::getDefaultMode()
{
    /* dtt todo: 7231 is incorrectly reporting NEXUS_FrontendQamMode_eAuto_64_256 is
       is a valid qam mode, but then chokes on it if you use it while tuning.  for now
       we'll always use e256 as the default mode */
    return NEXUS_FrontendQamMode_e256;

    if (true == _capabilities.qamModes[NEXUS_FrontendQamMode_eAuto_64_256])
    {
        return NEXUS_FrontendQamMode_eAuto_64_256;
    }
    else
    {
        return NEXUS_FrontendQamMode_e256;
    }
}


void CTunerQam::saveScanData(CTunerScanData * pScanData)
{
    _scanData = (*((CTunerQamScanData *)pScanData));
}


void CTunerQam::doScan()
{
    eRet                         ret                   = eRet_Ok;
    uint16_t                     major                 = 1;
    uint16_t                     numFreqToScan         = 0;
    uint16_t                     numFreqScanned        = 0;
    CModelChannelQam             chQam(this);            /* temp channel we'll use to do tuning during scan */
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
        NEXUS_FrontendQamSettings settings;
        NEXUS_Frontend_GetDefaultQamSettings(&settings);

        /* handle default settings for optional paramters */
        settings            = chQam.getSettings();
        settings.frequency  = *pFreq;
        settings.mode       = (-1 != _scanData._mode) ? (NEXUS_FrontendQamMode)_scanData._mode 
                                                          : getDefaultMode();
        settings.annex      = (-1 != _scanData._annex) ? (NEXUS_FrontendQamAnnex)_scanData._annex
                                                           : NEXUS_FrontendQamAnnex_eB;
        settings.bandwidth  = (-1 != _scanData._bandwidth) ? _scanData._bandwidth
                                                           : 6000000;
        settings.autoAcquire = true;
        settings.enablePowerMeasurement = false;
        settings.frequencyOffset = 250000;
        settings.acquisitionMode = NEXUS_FrontendQamAcquisitionMode_eScan;     
        settings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eAuto;       
        settings.enableNullPackets = false;

        chQam.setSettings(settings);
        chQam.setTransportType(NEXUS_TransportType_eTs);

        /* try to tune */
        ret = chQam.tune(_scanThread_id, _pModelConfig, true);
        if (eRet_NotSupported == ret)
        {
            /* some frontends don't accurately represent their capabilities.  for QAM it is most
               likely when specifying auto mode which really may not be supported. if this is the
               case, we will default to 256 mode and try tuning again. */
            settings.mode   = NEXUS_FrontendQamMode_e256;       
            _scanData._mode = NEXUS_FrontendQamMode_e256;

            BDBG_WRN(("Failure tuning with unsupported QAM mode - defaulting to 256"));
            ret = chQam.tune(_scanThread_id, _pModelConfig, true);
        }
        
        if (eRet_Ok == ret)
        {
            BERR_Code                     err         = BERR_SUCCESS;
            CParserBand                 * pBand       = chQam.getParserBand();
            CHANNEL_INFO_T                chanInfo;
            NEXUS_FrontendQamScanStatus   scanStatus;
            NEXUS_Error                   nerror      = NEXUS_SUCCESS;

            /* tune successful */
            nerror = NEXUS_Frontend_GetQamScanStatus(getFrontend(), &scanStatus);
            if (eRet_Ok == CHECK_NEXUS_ERROR("Error retrieving QAM scan status", nerror))
            {
                BDBG_WRN(("Scan Status: "));
                BDBG_WRN(("SymbolRate = %d", scanStatus.symbolRate));
                BDBG_WRN(("Mode = %d", scanStatus.mode));
                BDBG_WRN(("Annex = %d", scanStatus.annex));
                BDBG_WRN(("Interleaver = %d", scanStatus.interleaver));
                BDBG_WRN(("Spectrum inverted = %s", scanStatus.spectrumInverted?"True":"False"));
                BDBG_WRN(("Acquisition status = %d", scanStatus.acquisitionStatus));

                {
                    NEXUS_FrontendQamSettings qamSettings;
                    /* update channel object with scan status data if available */
                    qamSettings            = chQam.getSettings();
                    qamSettings.mode       = scanStatus.mode;
                    qamSettings.annex      = scanStatus.annex;
                    qamSettings.symbolRate = scanStatus.symbolRate;
                    chQam.setSettings(qamSettings);
                }
            }

            chQam.setMajor(major);

                    if (0 < chQam.addPsiPrograms(_scanThread_callback, _scanThread_context))
            {
                /* found channels so increment major channel number */
                major++;
            }
        }
        else
        {
            /* tune failed */
            BDBG_MSG(("NO qam channel found at freq:%u", chQam.getFrequency()));
        }
        
        chQam.unTune(_pModelConfig, false);

        numFreqScanned++;
        notifyData.setPercent(100 * numFreqScanned / numFreqToScan);
        notifyObservers(eNotify_ScanProgress, &notifyData);
    }

error:
    if (chQam.isTuned())
    {
        chQam.unTune(_pModelConfig);
    }
    scanDone(&notifyData);
}

#endif /* NEXUS_HAS_FRONTEND */
