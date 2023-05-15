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
 * $brcm_Workfile: frontend.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/9/12 1:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/frontend.c $
 * 
 * Hydra_Software_Devel/5   2/9/12 1:37p katrep
 * SW7429-1:conditional compilation of the fronend code
 * 
 * Hydra_Software_Devel/4   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   1/13/09 10:23a jtna
 * PR42679: fix comment for frontend.c
 *
 * Hydra_Software_Devel/1   11/11/08 5:08p jtna
 * PR42679: added unittest frontend.c
 *
 *
******************************************************************************/
/* automated unittest for frontend */

#include "nexus_platform.h"
#if 0
#include "examples_lib.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum TestFrontend
{
    TestFrontend_eAnalog = 0,
    TestFrontend_eQam,
    TestFrontend_eVsb,
    TestFrontend_eSatellite,
    TestFrontend_eOob,
    TestFrontend_eUpstream,
    TestFrontend_eOfdm,
    TestFrontend_eMax
} TestFrontend;

int main(void)
{
	NEXUS_PlatformConfiguration platformConfig;
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendHandle frontend;
    unsigned loops = 100;
    NEXUS_Error rc;
    unsigned i;

    NEXUS_FrontendAnalogSettings analogSettings;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_FrontendVsbSettings vsbSettings;
    NEXUS_FrontendSatelliteSettings satelliteSettings;
    NEXUS_FrontendOutOfBandSettings oobSettings;
    NEXUS_FrontendUpstreamSettings upstreamSettings;
    NEXUS_FrontendOfdmSettings ofdmSettings;

    NEXUS_FrontendAnalogStatus analogStatus;
    NEXUS_FrontendQamStatus qamStatus;
    NEXUS_FrontendVsbStatus vsbStatus;
    NEXUS_FrontendSatelliteStatus satelliteStatus;
    NEXUS_FrontendOutOfBandStatus oobStatus;
    NEXUS_FrontendUpstreamStatus upstreamStatus;
    NEXUS_FrontendOfdmStatus ofdmStatus;

    NEXUS_FrontendCapabilities capabilities;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);
#if NEXUS_HAS_FRONTEND
    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            if (capabilities.qam || capabilities.vsb || capabilities.satellite || capabilities.outOfBand ||
                capabilities.upstream || capabilities.ofdm)
            {
                break;
            }
        }
    }
    if (NULL == frontend)
    {
        fprintf(stderr, "Unable to find capable frontend\n");
        return 1;
    }

#define SLEEP_AND_UNTUNE() \
    BKNI_Sleep(rand() % 3000); \
    if (rand() % 2) { \
        printf("untune\n"); \
        NEXUS_Frontend_Untune(frontend); \
    }

    printf("capabilities: %s%s%s%s%s%s%s\n",
        (capabilities.analog&&capabilities.ifd)?"analog ":"",
        capabilities.qam?"qam ":"",
        capabilities.vsb?"vsb ":"",
        capabilities.satellite?"satellite ":"",
        capabilities.outOfBand?"oob ":"",
        capabilities.upstream?"upstream ":"",
        capabilities.ofdm?"ofdm":"");

    /* We should try checking status three times. before tune, after tune and after untune.
    When checking status before tune, it should return a failure. Hence BDBG_ASSERT(rc) */

    if (capabilities.analog && capabilities.ifd) {
        rc = NEXUS_Frontend_GetAnalogStatus(frontend, &analogStatus);
#if 0 /* TODO: for analog, rc is always 0. we seem to be inconsistent in return values when status is checked before tune. */
        BDBG_ASSERT(rc);
#endif
    }
    if (capabilities.qam) {
        rc = NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
        BDBG_ASSERT(rc);
    }
    if (capabilities.vsb) {
        rc = NEXUS_Frontend_GetVsbStatus(frontend, &vsbStatus);
        BDBG_ASSERT(rc);
    }
    if (capabilities.satellite) {
        rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &satelliteStatus);
        BDBG_ASSERT(rc);
    }
    if (capabilities.outOfBand) {
        rc = NEXUS_Frontend_GetOutOfBandStatus(frontend, &oobStatus);
        BDBG_ASSERT(rc);
    }
    if (capabilities.upstream) {
        rc = NEXUS_Frontend_GetUpstreamStatus(frontend, &upstreamStatus);
        BDBG_ASSERT(rc);
    }
    if (capabilities.ofdm) {
        rc = NEXUS_Frontend_GetOfdmStatus(frontend, &ofdmStatus);
        BDBG_ASSERT(rc);
    }

#if 0 /* TODO: could try cycling through all possible permuations of settings struct like this... */
    if (capabilities.qam) {
        NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
        for (i=0; i<NEXUS_FrontendQamMode_eMax; i++) {
            qamSettings.mode = i;
            for (j=0; j<NEXUS_FrontendQamAnnex_eMax; j++) {
                qamSettings.annex = j;
                for (k=0; k<2; k++) {
                    qamSettings.fastAcquisition = k;
                    for (l=0; l<2; l++) {
                        qamSettings.terrestrial = l;
                        printf("%d %d %d %d\n", i, j, k, l);
                        rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
                        BDBG_ASSERT(!rc || rc==BERR_INVALID_PARAMETER);
                        BKNI_Sleep(500);
                    }
                }
            }
        }
    }
#endif

    /* test hopping around from one tuner to another */
    while (loops--) {
        switch (rand() % TestFrontend_eMax) {
        case TestFrontend_eAnalog:
            if (capabilities.analog && capabilities.ifd) {
                printf("tune analog\n");
                NEXUS_Frontend_GetDefaultAnalogSettings(&analogSettings);
                analogSettings.frequency = 175750000;
                rc = NEXUS_Frontend_TuneAnalog(frontend, &analogSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetAnalogStatus(frontend, &analogStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&analogSettings, &(analogStatus.settings), sizeof(analogSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetAnalogStatus(frontend, &analogStatus); /* GetStatus() after untune may return success but may also not */
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eQam:
            if (capabilities.qam) {
                printf("tune qam\n");
                NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
                qamSettings.frequency = 357*1000000;
                qamSettings.mode = NEXUS_FrontendQamMode_e256;
                qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
                rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&qamSettings, &(qamStatus.settings), sizeof(qamSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eVsb:
            if (capabilities.vsb) {
                printf("tune vsb\n");
                NEXUS_Frontend_GetDefaultVsbSettings(&vsbSettings);
                vsbSettings.frequency = 575*1000000;
                vsbSettings.mode = NEXUS_FrontendVsbMode_e8;
                rc = NEXUS_Frontend_TuneVsb(frontend, &vsbSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetVsbStatus(frontend, &vsbStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&vsbSettings, &(vsbStatus.settings), sizeof(vsbSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetVsbStatus(frontend, &vsbStatus);
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eSatellite:
            if (capabilities.satellite) {
                printf("tune satellite\n");
                NEXUS_Frontend_GetDefaultSatelliteSettings(&satelliteSettings);
                satelliteSettings.frequency = 1119*1000000;
                satelliteSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
                rc = NEXUS_Frontend_TuneSatellite(frontend, &satelliteSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &satelliteStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&satelliteSettings, &(satelliteStatus.settings), sizeof(satelliteSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetSatelliteStatus(frontend, &satelliteStatus);
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eOob:
            if (capabilities.outOfBand) {
                printf("tune oob\n");
                NEXUS_Frontend_GetDefaultOutOfBandSettings(&oobSettings);
                oobSettings.frequency = 0;
                oobSettings.mode = NEXUS_FrontendOutOfBandMode_eAnnexAQpsk;
                rc = NEXUS_Frontend_TuneOutOfBand(frontend, &oobSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetOutOfBandStatus(frontend, &oobStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&oobSettings, &(oobStatus.settings), sizeof(oobSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetOutOfBandStatus(frontend, &oobStatus);
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eUpstream:
            if (capabilities.upstream) {
                printf("tune upstream\n");
                NEXUS_Frontend_GetDefaultUpstreamSettings(&upstreamSettings);
                upstreamSettings.frequency = 0;
                upstreamSettings.mode = NEXUS_FrontendUpstreamMode_eAnnexA;
                rc = NEXUS_Frontend_TuneUpstream(frontend, &upstreamSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetUpstreamStatus(frontend, &upstreamStatus);
                BDBG_ASSERT(!rc);
#if 0 /* upstreamStatus has no settings member */
                BDBG_ASSERT(!BKNI_Memcmp(&upstreamSettings, &(upstreamStatus.settings), sizeof(upstreamSettings)));
#endif

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetUpstreamStatus(frontend, &upstreamStatus);
            }
            else {
                loops++;
            }
            break;
        case TestFrontend_eOfdm:
            if (capabilities.ofdm) {
                printf("tune ofdm\n");
                NEXUS_Frontend_GetDefaultOfdmSettings(&ofdmSettings);
                ofdmSettings.frequency = 666*1000000;
                ofdmSettings.modeSettings.mode  = NEXUS_FrontendOfdmTransmissionMode_e2k;
                rc = NEXUS_Frontend_TuneOfdm(frontend, &ofdmSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Frontend_GetOfdmStatus(frontend, &ofdmStatus);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(!BKNI_Memcmp(&ofdmSettings, &(ofdmStatus.settings), sizeof(ofdmSettings)));

                SLEEP_AND_UNTUNE();
                (void) NEXUS_Frontend_GetOfdmStatus(frontend, &ofdmStatus);
            }
            else {
                loops++;
            }
            break;
        }
    }

    printf("test completed\n");
    NEXUS_Frontend_Untune(frontend);
#endif
    NEXUS_Platform_Uninit();
    return 0;
}
