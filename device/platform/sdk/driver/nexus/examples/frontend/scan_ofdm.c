/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: scan_ofdm.c $
* $brcm_Revision: 8 $
* $brcm_Date: 11/19/12 12:08p $
*
* Module Description:
*   Frontend OFDM scan
* 
*   This sample app demonstrates how to use ofdm scan feature.
*   1. Frequencies.txt file populated with the frequencies intended to scan must be copied into nexus/bin folder.
*   2. Frequencies.txt file should have one frequency(in Hz) per line.
*   3. NEXUS_Frontend_TuneOfdm() frontend api is used to scan one frequency at a time each for dvbt2 and dvbt, starting with dvbt2.
*   4. NEXUS_FrontendOfdmSettings structure is used to define all the required scan parameters.
*   5. Every frequency tuned/scanned, generates atleast one lock change callback. Time to wait for the lockchange callback can be varied using SCAN_TIMEOUT.
*   6. Upon which NEXUS_Frontend_GetFastStatus() is called to determine(using acquireInProgress in the fast status) if the acquire/scan is completed.
*   7. If completed, the lockstatus within the fast status tells the outcome of the acquire/scan. 
*   8. If not completed, wait again, as there is atleast one more lock change callback is expected from the frontend.
*   9. If completed and the lock status returns unlocked/no_signal, continue to try to acquire/scan the dvbt signal.
* 10. If completed and if the lockstatus return "locked" satus, the application proceeds to scan for the next frequency.
* 11. The results are printed in results.txt file in nexus/bin folder and also displayed on screen.
* 12. ENABLE_DVBT2 and ENABLE_DVBT can be used to enable/disable the respective modes to scan.
* 13. MAX_ITERATION can be used to scan the complete set of frequencies in frequency.txt file MAX_ITERATION times.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/frontend/scan_ofdm.c $
* 
* 8   11/19/12 12:08p erickson
* SW3461-289: Add DVB-T2 Multiple-PLP Capability to Nexus Scan Example
* 
* 7   8/7/12 5:37p mphillip
* SW7231-924: Merge NEXUS_Frontend_Acquire changes to main
* 
* cng_unittest/1   7/31/12 2:53p cng
* SW7231-924: Use NEXUS_Frontend_Acquire
* 
* 6   3/12/12 7:33p mphillip
* SW3461-177: Correct calculation of no_signal channels
* 
* 5   2/20/12 1:58p erickson
* SW7552-204: update scan_ofdm for latest silicon
* 
* 3   2/6/12 6:14p rjlewis
* SW7552-198: upon unlock, wait til scan times out.
* 
* 2   2/3/12 3:54p erickson
* SW7552-198, SW7552-201: merge
* 
* SCAN_FIX/2   2/3/12 3:52p vishk
* SW7552-198, SW7552-201: Lock/unlock callbacks fired are queued and are
*  fired back to back, hence making the sequence of event occurrence non-
*  deterministic. Scan for 7552 is broken.  Fix the comments.
* 
* SCAN_FIX/1   2/2/12 6:38p vishk
* SW7552-198, SW7552-201: Lock/unlock callbacks fired are queued and are
*  fired back to back, hence making the sequence of event occurrence non-
*  deterministic. Scan for 7552 is broken.
* 
* 1   11/23/11 3:14p jtna
* SW3461-50: added scan_dvbt_dvbt2.c
*
***************************************************************************/
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(scan_ofdm);

#define ENABLE_DVBT2 1
#define ENABLE_DVBT 1
#define ENABLE_ISDBT 0

#ifdef ENABLE_DVBT2 
#include "nexus_frontend_dvb_t2.h"
#endif
#define MAX_ITERATION 2

#define LINE_BUF_SIZE 256

/* Per interrupt/(status change event) wait time in ms. */
#define SCAN_TIMEOUT 5000
#define ASYNC_STATUS_TIMEOUT 1500

bool isDvbt2NoSignal;
bool isDvbt2UnLocked;
bool isDvbtNoSignal;
bool isDvbtUnLocked;
bool isIsdbtNoSignal;
bool isIsdbtUnLocked;
unsigned int lockCount;
unsigned int unLockCount;
unsigned int noSignalCount;

FILE *pFileResults;
uint32_t start_time, end_time, start, end;

#include <sys/time.h>
unsigned long gettime(void)
{
    int rc;
    struct timespec now;
    /* It's ok to use clock_gettime even without NPTL. */
    rc = clock_gettime(CLOCK_MONOTONIC, &now);
    if (rc) {
        rc = BERR_TRACE(BERR_OS_ERROR);
        return 0;
    }
    return now.tv_sec * 1000000 + now.tv_nsec / 1000;
}

static void lock_callback(void *context, int param)
{
    BKNI_EventHandle statusEvent = (BKNI_EventHandle)param;
    BSTD_UNUSED(context);

    BKNI_SetEvent((BKNI_EventHandle)statusEvent);
}

static void async_status_ready_callback(void *context, int param)
{ 
    BKNI_EventHandle statusEvent = (BKNI_EventHandle)param;
    BSTD_UNUSED(context);

    BKNI_SetEvent((BKNI_EventHandle)statusEvent);
}

int main(int argc, char **argv)
{
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendOfdmSettings ofdmSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;
    NEXUS_FrontendFastStatus fastStatus;
    unsigned frequencyCount = 0, iterate_count = 1;
    BKNI_EventHandle statusEvent;
    BKNI_EventHandle asyncStatusEvent;
    FILE *pFileFreq=NULL;
    NEXUS_Error rc=NEXUS_SUCCESS;
    char freqBuf[LINE_BUF_SIZE];
#if ENABLE_DVBT2
    NEXUS_FrontendDvbt2Status dvbt2Status;
    NEXUS_FrontendDvbt2StatusReady statusReady;
    unsigned currentPlpIndex = NEXUS_MAX_DVBT2_PLP_STATUS;
#endif
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
    
    rc = NEXUS_Platform_Init(NULL);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    rc = BKNI_CreateEvent(&statusEvent);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BKNI_CreateEvent(&asyncStatusEvent);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    pFileFreq = fopen("frequencies.txt", "r");
    if ( NULL == pFileFreq )
    {
        BDBG_ERR(("frequencies.txt file not found. Copy this file into nexus/bin folder."));
        return 0;
    }
  
    pFileResults = fopen("results.txt", "w");
    if ( NULL == pFileResults )
    {
        BDBG_ERR(("results.txt file not found."));
        return 0;
    }
    
    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.ofdm = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        BDBG_ERR(("Unable to find OFDM-capable frontend"));
        return -1;
    }
    
    NEXUS_Frontend_GetDefaultOfdmSettings(&ofdmSettings);
    ofdmSettings.acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
    ofdmSettings.terrestrial = true;
    ofdmSettings.spectrum = NEXUS_FrontendOfdmSpectrum_eAuto;
    ofdmSettings.lockCallback.callback = lock_callback;
    ofdmSettings.lockCallback.context = frontend;
    ofdmSettings.lockCallback.param = (int)statusEvent;
    ofdmSettings.asyncStatusReadyCallback.callback = async_status_ready_callback;
    ofdmSettings.asyncStatusReadyCallback.param = (int)asyncStatusEvent;

iterate:    
    BDBG_WRN(("Start Channel Scan for %d times.", iterate_count));
    start = (uint32_t) gettime();   
    while ( fgets(freqBuf, LINE_BUF_SIZE, pFileFreq) )
    {
        frequencyCount++;     
        ofdmSettings.frequency  = atoi(freqBuf);

#if ENABLE_DVBT2
        BDBG_WRN(("Tuning frequency %d for Dvbt2", ofdmSettings.frequency));

        ofdmSettings.mode = NEXUS_FrontendOfdmMode_eDvbt2;
        ofdmSettings.bandwidth = 8000000;
        ofdmSettings.dvbt2Settings.plpMode = true; /*Auto plp mode */
iterate_plps:
        if((dvbt2Status.status.l1Plp.plp[currentPlpIndex].plpType == NEXUS_FrontendDvbt2PlpType_eCommon) &&(currentPlpIndex != NEXUS_MAX_DVBT2_PLP_STATUS)){
			if(++currentPlpIndex == dvbt2Status.status.l1Plp.numPlp ){
                dvbt2Status.status.l1Plp.numPlp = 0;
                currentPlpIndex = NEXUS_MAX_DVBT2_PLP_STATUS;
                ofdmSettings.dvbt2Settings.plpMode = true; /* reset to auto plp mode */
                continue;
            }
			else
	            goto iterate_plps;
        }
        isDvbt2NoSignal = false;
        isDvbt2UnLocked = false;
        if(currentPlpIndex != NEXUS_MAX_DVBT2_PLP_STATUS){
            rc = BKNI_WaitForEvent(statusEvent, 0);
        }
        rc = NEXUS_Frontend_TuneOfdm(frontend, &ofdmSettings); 
        if(rc){rc = BERR_TRACE(rc); goto done;}

        while(1) {
            /* Waiting for the lock change callback for dvbt2. */
            rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
            if(rc){
                if(rc == NEXUS_TIMEOUT){
                    BDBG_WRN(("Scan timed out for Dvbt2. Continuing to scan for next plp or frequency or mode."));
                    break;
                }
                else {
                    rc = BERR_TRACE(rc); goto done;
                }
            }

            rc = NEXUS_Frontend_GetFastStatus(frontend, &fastStatus);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            
            BDBG_MSG(("fastStatus.lockStatus = %d, fastStatus.acquireInProgress = %d", fastStatus.lockStatus, fastStatus.acquireInProgress));

            if((!fastStatus.acquireInProgress) && (fastStatus.lockStatus != NEXUS_FrontendLockStatus_eUnlocked))break;
        }

        if(currentPlpIndex == NEXUS_MAX_DVBT2_PLP_STATUS){
        if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
                rc = NEXUS_Frontend_RequestDvbt2AsyncStatus(frontend, NEXUS_FrontendDvbt2StatusType_eL1Plp);
                if(rc) {rc = BERR_TRACE(rc); goto done;}

                rc = BKNI_WaitForEvent(asyncStatusEvent, ASYNC_STATUS_TIMEOUT);
                if(rc){
                    if(rc == NEXUS_TIMEOUT){
                        BDBG_WRN(("Async status retrieval timed out for Dvbt2. Continuing to scan for next mode or frequency."));
                        break;
                    }
                    else {
                        rc = BERR_TRACE(rc); goto done;
                    }
                }

                rc =  NEXUS_Frontend_GetDvbt2AsyncStatusReady(frontend, &statusReady);
                if(rc){rc = BERR_TRACE(rc); goto done;}

                if(statusReady.type[NEXUS_FrontendDvbt2StatusType_eL1Plp]){
                    rc = NEXUS_Frontend_GetDvbt2AsyncStatus(frontend, NEXUS_FrontendDvbt2StatusType_eL1Plp, &dvbt2Status);
                    if(rc) {rc = BERR_TRACE(rc); goto done;}
                }
                
            lockCount++;
                fprintf(pFileResults, "Frequency  %dHz and plpId = %d, Locked for Dvbt2\n",ofdmSettings.frequency, currentPlpIndex);
                
                if(dvbt2Status.status.l1Plp.numPlp > 1) {
                    currentPlpIndex = 0;
                    ofdmSettings.dvbt2Settings.plpMode = false; /* Manual plpId */
                    ofdmSettings.dvbt2Settings.plpId = dvbt2Status.status.l1Plp.plp[currentPlpIndex].plpId;
                    goto iterate_plps;
                }
                else {
                    dvbt2Status.status.l1Plp.numPlp = 0;
                    currentPlpIndex = NEXUS_MAX_DVBT2_PLP_STATUS;
                    ofdmSettings.dvbt2Settings.plpMode = true; /* reset to auto plp mode */
            continue;
        }
            }
        else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eNoSignal){                
            isDvbt2NoSignal = true;
            /* No Dvbt2 signal, so try Dvbt or Isdbt */
        }
        else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eUnlocked){
            isDvbt2UnLocked = true;
            /* Dvbt2 unlockd, so see if there is a dvbt or Isdbt signal at this frequency. */
        }
        else {
            BDBG_ERR(("Unknown lock status for Dvbt2."));
        }
        }
        else if(currentPlpIndex < dvbt2Status.status.l1Plp.numPlp){
			if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
                lockCount++;
                fprintf(pFileResults, "Frequency  %dHz and currentPlpIndex = %d, plpId = %d, Locked for Dvbt2\n",ofdmSettings.frequency, currentPlpIndex, dvbt2Status.status.l1Plp.plp[currentPlpIndex].plpId);
            }/*
                    	else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eNoSignal){
				isDvbt2NoSignal = true;
			}*/
	        else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eUnlocked){
				unLockCount++;
                fprintf(pFileResults, "Frequency  %dHz and currentPlpIndex = %d, plpId = %d, UnLocked for Dvbt2\n",ofdmSettings.frequency, currentPlpIndex, dvbt2Status.status.l1Plp.plp[currentPlpIndex].plpId);
	            isDvbt2UnLocked = true;
	        }
            if(++currentPlpIndex == dvbt2Status.status.l1Plp.numPlp){
                dvbt2Status.status.l1Plp.numPlp = 0;
                currentPlpIndex = NEXUS_MAX_DVBT2_PLP_STATUS;
                ofdmSettings.dvbt2Settings.plpMode = true; /* reset to auto plp mode */
                continue;
            }
            goto iterate_plps;
        }
#else
        isDvbt2NoSignal = true;
#endif
#if ENABLE_DVBT
        BDBG_WRN(("Tuning frequency %d for Dvbt", ofdmSettings.frequency));

        ofdmSettings.mode = NEXUS_FrontendOfdmMode_eDvbt;
        ofdmSettings.bandwidth = 8000000;
        isDvbtNoSignal = false;
        isDvbtUnLocked = false;
        rc = NEXUS_Frontend_TuneOfdm(frontend, &ofdmSettings); 
        if(rc){rc = BERR_TRACE(rc); goto done;}

        while(1) {
            /* Waiting for the lock change callback for dvbt. */
            rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
            if(rc){
                if(rc == NEXUS_TIMEOUT){
                    BDBG_WRN(("Scan timed out for dvbt. Continuing to scan for next mode or frequency."));
                    break;
                }
                else {
                    rc = BERR_TRACE(rc); goto done;
                }
            }

            rc = NEXUS_Frontend_GetFastStatus(frontend, &fastStatus);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            
            BDBG_MSG(("fastStatus.lockStatus = %d, fastStatus.acquireInProgress = %d", fastStatus.lockStatus, fastStatus.acquireInProgress));

            if((!fastStatus.acquireInProgress) && (fastStatus.lockStatus != NEXUS_FrontendLockStatus_eUnlocked))break;
        }

        if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
            lockCount++;
            fprintf(pFileResults, "Frequency  %dHz, Locked for Dvbt\n",ofdmSettings.frequency);
            continue;
        }
        else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eNoSignal){
            isDvbtNoSignal = true;
        }
        else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eUnlocked){
            isDvbtUnLocked = true;
        }
        else {
            BDBG_ERR(("Unknown lock status for Dvbt."));
        }
#else
        isDvbtNoSignal = true;
#endif
#if ENABLE_ISDBT
            BDBG_WRN(("Tuning frequency %d for Isdbt", ofdmSettings.frequency));
    
            ofdmSettings.mode = NEXUS_FrontendOfdmMode_eIsdbt;
        ofdmSettings.bandwidth = 6000000;
            isIsdbtNoSignal = false;
            isIsdbtUnLocked = false;
            rc = NEXUS_Frontend_TuneOfdm(frontend, &ofdmSettings); 
            if(rc){rc = BERR_TRACE(rc); goto done;}
    
            while(1) {
                /* Waiting for the lock change callback for Isdbt. */
                rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
                if(rc){
                    if(rc == NEXUS_TIMEOUT){
                        BDBG_WRN(("Scan timed out for Isdbt. Continuing to scan for next mode or frequency."));
                        break;
                    }
                    else {
                        rc = BERR_TRACE(rc); goto done;
                    }
                }
    
                rc = NEXUS_Frontend_GetFastStatus(frontend, &fastStatus);
                if(rc){rc = BERR_TRACE(rc); goto done;}
                
                BDBG_MSG(("fastStatus.lockStatus = %d, fastStatus.acquireInProgress = %d", fastStatus.lockStatus, fastStatus.acquireInProgress));
    
                if((!fastStatus.acquireInProgress) && (fastStatus.lockStatus != NEXUS_FrontendLockStatus_eUnlocked))break;
            }
    
            if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
                lockCount++;
                fprintf(pFileResults, "Frequency  %dHz, Locked for Isdbt\n",ofdmSettings.frequency);
                continue;
            }
            else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eNoSignal){                
                isIsdbtNoSignal = true;
            }
            else if(fastStatus.lockStatus == NEXUS_FrontendLockStatus_eUnlocked){
                isIsdbtUnLocked = true;
            }
            else {
                BDBG_ERR(("Unknown lock status for Isdbt."));
            }
#else
        isIsdbtNoSignal = true;
#endif
    if(isDvbt2NoSignal && isDvbtNoSignal && isIsdbtNoSignal) {
        noSignalCount++;
    }
    else if(isDvbt2UnLocked || isDvbtUnLocked || isIsdbtUnLocked)
        unLockCount++;
    }
    iterate_count++;
    fseek(pFileFreq,0,SEEK_SET);
    if(iterate_count <= MAX_ITERATION)goto iterate;

    end = (uint32_t) gettime();
    fprintf(pFileResults, "Total scan time: %d seconds\n", (end - start)/1000000);
    fprintf(stderr, "Total scan time: %d milli-seconds\n", (end - start)/1000);

    fprintf(pFileResults, "Total Channels Scanned: %d, locked channels: %d, unlocked channels: %d, noSignal = %d\n", frequencyCount, lockCount, unLockCount, noSignalCount);
    fprintf(stderr, "Total Channels Scanned: %d, locked channels: %d, unlocked channels: %d, noSignal = %d\n", frequencyCount, lockCount, unLockCount, noSignalCount);
done:
    if(pFileFreq) fclose (pFileFreq);
    if(pFileResults) fclose (pFileResults);
    NEXUS_Frontend_Untune(frontend);
    NEXUS_Frontend_Release(frontend);
    NEXUS_Platform_Uninit();
    return 0;
}



