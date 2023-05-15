/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: signal_quality_satellite.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/7/12 6:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/signal_quality_satellite.c $
 * 
 * Hydra_Software_Devel/1   6/7/12 6:27p mphillip
 * SW7425-2907: Add satellite signal quality test
 *
 *
 *
******************************************************************************/

#include "nexus_platform.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#include "../common/parse_commandline.h"
#include "../common/parse_channels.h"

int main(int argc, char **argv)
{
    channel_entries *entries = NULL;
    int num_entries = 0;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    unsigned i;
    int num_failures = 0;

    BSTD_UNUSED(i);

    /* Bring up all modules for a platform in a default configuration for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    entries = parse_channels(channels_file_name(argc,argv),&num_entries);

#if NEXUS_HAS_FRONTEND
    while (loop_from_commandline(argc,argv,1)) {
        for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
        {
            NEXUS_FrontendHandle frontend;
            frontend = platformConfig.frontend[i];
            if (frontend) {
                NEXUS_FrontendCapabilities capabilities;

                NEXUS_Frontend_GetCapabilities(frontend, &capabilities);

                if (capabilities.satellite) {
                    int j;
                    for (j=0; j < num_entries; j++) {
                        NEXUS_FrontendSatelliteSettings satSettings;
                        NEXUS_FrontendFastStatus fastStatus;
                        NEXUS_FrontendSatelliteStatus status;
                        NEXUS_FrontendDiseqcSettings diseqcSettings;
                        bool success = false;

                        BKNI_Memset(&diseqcSettings,0,sizeof(NEXUS_FrontendDiseqcSettings));
                        NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
                        diseqcSettings.toneEnabled = entries[j].fe_settings.satellite.diseqc.toneEnabled;
                        diseqcSettings.voltage = entries[j].fe_settings.satellite.diseqc.voltage;
                        NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);

                        printf("Tuning %d ('%s')\n",j,entries[j].trailing_comment);
                        BKNI_Memcpy(&satSettings, &entries[j].fe_settings.satellite.satellite, sizeof(satSettings));
                        rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
                        BDBG_ASSERT(!rc);

                        /* Sleep 1 second, check lock status */
                        BKNI_Sleep(1000);

                        rc = NEXUS_Frontend_GetFastStatus(frontend, &fastStatus);
                        BDBG_ASSERT(!rc);
                        rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
                        BDBG_ASSERT(!rc);
                        printf("fast lock: %d, demod: %d, tuner: %d\n", fastStatus.lockStatus, status.demodLocked, status.tunerLocked);

                        /* no signal should not lock */
                        if (comment_has_token(entries[j].trailing_comment,"no_signal")) {
                            if (fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
                                success = false;
                            } else {
                                success = true;
                            }
                        } else {
                            /* we expect to lock on this signal */
                            int k;
                            int prev_uncorrected = 0, prev_corrected = 0;
                            int delta_uncorrected = 0, delta_corrected = 0;

                            rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
                            BDBG_ASSERT(!rc);
                            if (status.tunerLocked && status.demodLocked && fastStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked) {
                                rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
                                BDBG_ASSERT(!rc);
                                prev_uncorrected = status.fecUncorrected;
                                prev_corrected = status.fecCorrected;
                                /* we're locked, check signal strength */
                                for (k=0; k < 5; k++) {
                                    BKNI_Sleep(1000);
                                    rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
                                    delta_uncorrected += status.fecUncorrected - prev_uncorrected;
                                    delta_corrected += status.fecCorrected - prev_corrected;
                                    prev_uncorrected = status.fecUncorrected;
                                    prev_corrected = status.fecCorrected;
                                    printf("actual: %d,%d; delta corrected: %d, delta uncorrected: %d\n", status.fecCorrected, status.fecUncorrected, delta_corrected, delta_uncorrected);
                                }
                                if (comment_has_token(entries[j].trailing_comment,"weak")) {
                                    /* for a weak signal, both values should change */
                                    if ((delta_uncorrected > 0) && (delta_corrected > 0))
                                        success = true;
                                    else
                                        success = false;
                                } else if (comment_has_token(entries[j].trailing_comment,"strong")) {
                                    /* for a strong signal, only uncorrected should change */
                                    if (delta_uncorrected > 0)
                                        success = false;
                                    else
                                        success = true;
                                }
                                printf("corrected: %d, uncorrected: %d\n", delta_corrected, delta_uncorrected);
                                printf("snr: %d\n",status.snrEstimate);
                            } else {
                                success = false;
                            }
                        }
                        if (!success) {
                            printf("test failure\n");
                            num_failures++;
                        } else {
                            printf("test success\n");
                        }
                    }
                }
            }
        }
    }
#endif
    NEXUS_Platform_Uninit();

    printf("%d failures\n", num_failures);
    return num_failures;
}

