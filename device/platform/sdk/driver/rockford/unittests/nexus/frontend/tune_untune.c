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
 * $brcm_Workfile: tune_untune.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 7/30/12 4:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/tune_untune.c $
 * 
 * Hydra_Software_Devel/9   7/30/12 4:15p katrep
 * SW7429-1:compiler warning
 * 
 * Hydra_Software_Devel/8   6/7/12 6:30p mphillip
 * SW7425-2993: Update old tune/untune test to support extended testing
 * via the commandline.
 * 
 * Hydra_Software_Devel/7   2/15/12 12:14p katrep
 * SW7429-1:compiler warning
 * 
 * Hydra_Software_Devel/6   2/9/12 1:37p katrep
 * SW7429-1:conditional compilation of the fronend code
 * 
 * Hydra_Software_Devel/5   5/15/09 12:00p erickson
 * PR54843: rework test to be more generic
 *
 * Hydra_Software_Devel/4   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   12/11/08 4:04p erickson
 * PR49556: increase randomness of test
 *
 * Hydra_Software_Devel/2   10/31/08 11:36a erickson
 * PR48572: added printfs
 *
 * Hydra_Software_Devel/1   10/30/08 10:41p erickson
 * PR48572: added test
 *
******************************************************************************/

#include "nexus_platform.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#include "../common/parse_commandline.h"

int main(int argc, char **argv)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    unsigned i;
    BSTD_UNUSED(i);
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);


    /* Bring up all modules for a platform in a default configuration for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
#if NEXUS_HAS_FRONTEND
    while (loop_from_commandline(argc,argv,1)) {
        for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
        {
            NEXUS_FrontendHandle frontend;
            frontend = platformConfig.frontend[i];
            if (frontend) {
                unsigned loops = 50;
                NEXUS_FrontendCapabilities capabilities;

                NEXUS_Frontend_GetCapabilities(frontend, &capabilities);

                while (loops--) {
                    switch (rand() % 6) {
                    case 0:
                        if (capabilities.analog) {
                            NEXUS_FrontendAnalogSettings frontendAnalogSettings;

                            printf("tune analog\n");
                            NEXUS_Frontend_GetDefaultAnalogSettings(&frontendAnalogSettings);
                            frontendAnalogSettings.frequency = 175750000;
                            rc = NEXUS_Frontend_TuneAnalog(frontend, &frontendAnalogSettings);
                            BDBG_ASSERT(!rc);
                        }
                        break;
                    case 1:
                        if (capabilities.vsb) {
                            NEXUS_FrontendVsbSettings vsbSettings;

                            printf("tune vsb\n");
                            NEXUS_Frontend_GetDefaultVsbSettings(&vsbSettings);
                            vsbSettings.frequency = 575000000;
                            vsbSettings.mode = NEXUS_FrontendVsbMode_e8;
                            rc = NEXUS_Frontend_TuneVsb(frontend, &vsbSettings);
                            BDBG_ASSERT(!rc);
                        }
                        break;
                    case 2:
                        if (capabilities.satellite) {
                            NEXUS_FrontendSatelliteSettings satSettings;

                            NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
                            if (rand() % 2) {
                                printf("tune sat (DVB)\n");
                                satSettings.frequency = 1119000000;
                                satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
                            } else {
                                printf("tune sat (DSS)\n");
                                satSettings.frequency = 1396820000;
                                satSettings.mode = NEXUS_FrontendSatelliteMode_eDss;
                            }
                            rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
                            BDBG_ASSERT(!rc);
                        }
                        break;
                    case 3:
                        if (capabilities.qam) {
                            NEXUS_FrontendQamSettings qamSettings;

                            printf("tune qam\n");
                            NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
                            qamSettings.frequency = 765000000;
                            qamSettings.mode = NEXUS_FrontendQamMode_e64;
                            rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
                            BDBG_ASSERT(!rc);
                        }
                        break;
                    case 4:
                        printf("untune\n");
                        NEXUS_Frontend_Untune(frontend);
                        break;
                    case 5:
                        BKNI_Sleep(rand() % 3000);
                        break;
                    default:
                        /* fix numbering in switch */
                        BDBG_ASSERT(0);
                        break;
                    }
                }
            }
        }
    }
#endif
    NEXUS_Platform_Uninit();

    return 0;
}

