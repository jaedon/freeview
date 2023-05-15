/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: scan_ofdm.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 3/12/12 3:48p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/api/examples/scan_ofdm.c $
 * 
 * 1   3/12/12 3:48p erickson
 * SW3461-169: add btuner_get_fast_status
 * 
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bsettop.h"

BDBG_MODULE(scan_ofdm);

#define ENABLE_DVBT2 1
#define ENABLE_DVBT 1
#define ENABLE_ISDBT 0

#define MAX_ITERATION 1

#define LINE_BUF_SIZE 256

/* Per interrupt/(status change event) wait time in ms. */
#define SCAN_TIMEOUT 2000

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

static void lock_callback(void *context)
{
    BKNI_EventHandle statusEvent = (BKNI_EventHandle)context;
    BSTD_UNUSED(context);

    BKNI_SetEvent((BKNI_EventHandle)statusEvent);
    printf("OFDM Lock callback called.\n");
}

int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.ofdm.objects[0]);
	btuner_ofdm_params ofdm;
	bband_t band;
	unsigned i=0, frequencyCount = 0, iterate_count = 1, frequency=0;
    BKNI_EventHandle statusEvent;
	btuner_fast_status fast_status;
	FILE *pFileFreq=NULL;
    char freqBuf[LINE_BUF_SIZE];

	rc = BKNI_CreateEvent(&statusEvent);
	if(rc){rc = BERR_TRACE(rc); goto done;}

	pFileFreq = fopen("frequencies.txt", "r");
    if ( NULL == pFileFreq )
    {
        BDBG_ERR(("frequencies.txt file not found. Copy this file into /BSEAV/bin folder."));
        goto done;
    }
  
    pFileResults = fopen("results.txt", "w");
    if ( NULL == pFileResults )
    {
        BDBG_ERR(("results.txt file not found."));
        goto done;
    }

	btuner_ofdm_params_init(&ofdm, tuner);
	ofdm.callback_context = statusEvent;
	ofdm.lock_callback = lock_callback;
	ofdm.priority = btuner_ofdm_priority_high;
	ofdm.wait_for_lock = false;

iterate:	
	BDBG_WRN(("Start Channel Scan for %d times.", iterate_count));
	start = (uint32_t) gettime();  
    while ( fgets(freqBuf, LINE_BUF_SIZE, pFileFreq) )
    {
        frequencyCount++;     
        frequency  = atoi(freqBuf);

#if ENABLE_DVBT2
		BDBG_WRN(("Tuning frequency %d for Dvbt2", frequency));

    	ofdm.mode = btuner_ofdm_mode_dvbt2;
        ofdm.bandwidth = btuner_ofdm_bandwidth_8Mhz;
        isDvbt2NoSignal = false;
        isDvbt2UnLocked = false;

		band = btuner_tune_ofdm(tuner, frequency, &ofdm);
		if (!band) goto done;

		while(1) {
            /* Waiting for the lock change callback for dvbt2. */
            rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
            if(rc){
                if(rc == berr_busy){
                    BDBG_WRN(("Scan timed out for Dvbt2. Continuing to scan for next mode or frequency."));
                    break;
                }
                else {
                    rc = BERR_TRACE(rc); goto done;
                }
            }

            rc = btuner_get_fast_status(tuner, &fast_status );
            if(rc){rc = BERR_TRACE(rc); goto done;}
            
            BDBG_MSG(("fast_status.lock_status = %d, fast_status.acquire_in_progress = %d", fast_status.lock_status, fast_status.acquire_in_progress));

            if((!fast_status.acquire_in_progress) && (fast_status.lock_status != btuner_lock_status_unlocked))break;
        }

        if(fast_status.lock_status == btuner_lock_status_locked) {
            lockCount++;
            fprintf(pFileResults, "Frequency  %dHz, Locked for Dvbt2\n",frequency);
            continue;
        }
        else if(fast_status.lock_status == btuner_lock_status_noSignal){                
            isDvbt2NoSignal = true;
            /* No Dvbt2 signal, so try Dvbt or Isdbt */
        }
        else if(fast_status.lock_status == btuner_lock_status_unlocked){
            isDvbt2UnLocked = true;
            /* Dvbt2 unlockd, so see if there is a dvbt or Isdbt signal at this frequency. */
        }
        else {
            BDBG_ERR(("Unknown lock status for Dvbt2."));
        }
#endif
#if ENABLE_DVBT
	BDBG_WRN(("Tuning frequency %d for Dvbt", frequency));

	ofdm.mode = btuner_ofdm_mode_dvbt;
    ofdm.bandwidth = btuner_ofdm_bandwidth_8Mhz;
    isDvbtNoSignal = false;
    isDvbtUnLocked = false;

	band = btuner_tune_ofdm(tuner, frequency, &ofdm);
	if (!band) goto done;

	while(1) {
        /* Waiting for the lock change callback for dvbt. */
        rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
        if(rc){
            if(rc == berr_busy){
                BDBG_WRN(("Scan timed out for Dvbt. Continuing to scan for next mode or frequency."));
                break;
            }
            else {
                BDBG_WRN(("rc = %d", rc));
				rc = BERR_TRACE(rc); goto done;
            }
        }

        rc = btuner_get_fast_status(tuner, &fast_status );
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        BDBG_MSG(("fast_status.lock_status = %d, fast_status.acquire_in_progress = %d", fast_status.lock_status, fast_status.acquire_in_progress));

        if((!fast_status.acquire_in_progress) && (fast_status.lock_status != btuner_lock_status_unlocked))break;
    }

    if(fast_status.lock_status == btuner_lock_status_locked) {
        lockCount++;
        fprintf(pFileResults, "Frequency  %dHz, Locked for Dvbt\n",frequency);
        continue;
    }
    else if(fast_status.lock_status == btuner_lock_status_noSignal){                
        isDvbtNoSignal = true;
        /* No Dvbt signal, so try Isdbt */
    }
    else if(fast_status.lock_status == btuner_lock_status_unlocked){
        isDvbtUnLocked = true;
        /* Dvbt unlockd, so see if there is a Isdbt signal at this frequency. */
    }
    else {
        BDBG_ERR(("Unknown lock status for Dvbt."));
    }
#endif
#if ENABLE_ISDBT
	BDBG_WRN(("Tuning frequency %d for Isdbt", frequency));

	ofdm.mode = btuner_ofdm_mode_isdbt;
    ofdm.bandwidth = btuner_ofdm_bandwidth_6Mhz;
    isIsdbtNoSignal = false;
    isIsdbtUnLocked = false;

	band = btuner_tune_ofdm(tuner, frequency, &ofdm);
	if (!band) goto done;

	while(1) {
        /* Waiting for the lock change callback for Isdbt. */
        rc = BKNI_WaitForEvent(statusEvent, SCAN_TIMEOUT);
        if(rc){
            if(rc == berr_busy){
                BDBG_WRN(("Scan timed out for Isdbt. Continuing to scan for next mode or frequency."));
                break;
            }
            else {
                rc = BERR_TRACE(rc); goto done;
            }
        }

        rc = btuner_get_fast_status(tuner, &fast_status );
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        BDBG_MSG(("fast_status.lock_status = %d, fast_status.acquire_in_progress = %d", fast_status.lock_status, fast_status.acquire_in_progress));

        if((!fast_status.acquire_in_progress) && (fast_status.lock_status != btuner_lock_status_unlocked))break;
    }

    if(fast_status.lock_status == btuner_lock_status_locked) {
        lockCount++;
        fprintf(pFileResults, "Frequency  %dHz, Locked for Isdbtt\n",frequency);
        continue;
    }
    else if(fast_status.lock_status == btuner_lock_status_noSignal){                
        isIsdbtNoSignal = true;
        /* No Dvbt2 signal, so try Dvbt or Isdbt */
    }
    else if(fast_status.lock_status == btuner_lock_status_unlocked){
        isIsdbtUnLocked = true;
        /* Isdbt unlockd, so move on to the next frequency. */
    }
    else {
        BDBG_ERR(("Unknown lock status for Dvbt."));
    }
#endif
    if(isDvbt2NoSignal && isDvbt2NoSignal && isIsdbtNoSignal)
        noSignalCount++;
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

	BDBG_ERR(("Press enter to exit."));
	getchar();
done:	
    if(pFileFreq) fclose (pFileFreq);
    if(pFileResults) fclose (pFileResults);
	btuner_untune(tuner);
	bsettop_uninit();
	return rc;
}


