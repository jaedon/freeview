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
 * $brcm_Workfile: display_vbi_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 3/29/12 3:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/display_vbi_stress.c $
 * 
 * Hydra_Software_Devel/9   3/29/12 3:38p erickson
 * SW7435-77: set allowVps
 * 
 * Hydra_Software_Devel/8   3/27/12 12:00p erickson
 * SW7435-77: set allowTeletext
 * 
 * Hydra_Software_Devel/7   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 * 
 * Hydra_Software_Devel/6   10/18/10 11:16a katrep
 * SW7405-4950:add command line option -disabled_cgmsb
 * 
 * Hydra_Software_Devel/5   8/10/10 3:39p randyjew
 * SW7208-105: Fix compile warning for implicit declaration for time
 * 
 * Hydra_Software_Devel/4   5/26/10 12:59p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/3   4/16/10 2:13p erickson
 * SWDEPRECATED-3044: fix DTV
 *
 * Hydra_Software_Devel/2   3/5/10 12:32p erickson
 * SW3556-1043: improve tt stress test
 *
 * Hydra_Software_Devel/1   1/8/10 3:15p erickson
 * SW3556-979: added vbi encode stress test
 *
 * Hydra_Software_Devel/7   1/23/09 4:06p erickson
 * PR47892: fix pal selection
 *
 * Hydra_Software_Devel/6   10/16/08 1:56p erickson
 * PR47892: fixes
 *
 * Hydra_Software_Devel/5   10/16/08 12:40p jtna
 * PR47892: Added GemStar encoding interface
 *
 * Hydra_Software_Devel/4   2/29/08 10:27a jgarrett
 * PR 39435: Fixing includes
 *
 * Hydra_Software_Devel/3   2/5/08 10:09a erickson
 * PR38679: added teletext encode
 *
 * Hydra_Software_Devel/2   2/4/08 10:39a erickson
 * PR38679: update for 740x
 *
 * Hydra_Software_Devel/1   2/1/08 4:15p erickson
 * PR38679: added vbi loopback steps
 *
 * 1   2/1/08 3:21p erickson
 * PR38679: add vbi example
 *
 * 2   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/
/* Example to tune an analog channel using nexus */

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#include "nexus_parser_band.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(display_vbi_stress);



void print_usage(const char *app)
{
    printf("%s usage:\n", app);
    printf(
        "  -h|--help  - this usage information\n"
        "  -disable_cgmsb   - Disables CGMS-B on by default\n"
        );
}

int main(int argc, const char *argv[])
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned count = 0;
    int i=0;

    srand(time(NULL));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    platformSettings.displayModuleSettings.vbi.allowVps = true;
    platformSettings.displayModuleSettings.vbi.allowGemStar = true;
    platformSettings.displayModuleSettings.vbi.allowCgmsB = true;

    for (i=1;i<argc;i++) 
     {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) 
        {
            print_usage(argv[0]);
            return -1;
        }
        else if (!strcmp(argv[i], "-disable_cgmsb")) {
            platformSettings.displayModuleSettings.vbi.allowCgmsB = false;
            printf("CGMS-B Disabled\n");
        }
        else
        {
            printf("unknown param %s\n", argv[i]);
            return -1;
        }
    }
    
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#endif

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    displayVbiSettings.gemStar.baseLineTop = 15;
    displayVbiSettings.gemStar.lineMaskTop = 0x1f;
    displayVbiSettings.gemStar.baseLineBottom = 15 + 263;
    displayVbiSettings.gemStar.lineMaskBottom = 0x1f;
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

    for (count = 0; count < 2000; count++) {
        if (count % 100 == 0) BDBG_WRN(("count %d", count));
        switch (rand() % 10) {
        case 0:
            if (rand() % 20 == 0) {
                displayVbiSettings.teletextEnabled = false;
                displayVbiSettings.wssEnabled = false;
                displayVbiSettings.vpsEnabled = false;
                displayVbiSettings.wssEnabled = false;
                rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
                BDBG_ASSERT(!rc);
                displaySettings.format = NEXUS_VideoFormat_eNtsc;
                rc = NEXUS_Display_SetSettings(display, &displaySettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 1:
            if (rand() % 20 == 0) {
                displayVbiSettings.cgmsEnabled = false;
                rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
                BDBG_ASSERT(!rc);
                displaySettings.format = NEXUS_VideoFormat_ePal;
                rc = NEXUS_Display_SetSettings(display, &displaySettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 2:
            if (displaySettings.format == NEXUS_VideoFormat_ePal) {
                displayVbiSettings.teletextEnabled = rand() % 2;
                displayVbiSettings.wssEnabled = rand() % 2;
                displayVbiSettings.vpsEnabled = rand() % 2;
                displayVbiSettings.wssEnabled = rand() % 2;
                displayVbiSettings.cgmsEnabled = false;
            }
            else {
                displayVbiSettings.teletextEnabled = false;
                displayVbiSettings.wssEnabled = false;
                displayVbiSettings.vpsEnabled = false;
                displayVbiSettings.wssEnabled = false;
                displayVbiSettings.cgmsEnabled = rand() % 2;
            }
            displayVbiSettings.closedCaptionEnabled = rand() % 2;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
            break;
        case 3:
            if (displayVbiSettings.cgmsEnabled) {
                rc = NEXUS_Display_SetCgms(display, rand());
                BDBG_ASSERT(!rc);
            }
            break;
        case 4:
            if (displayVbiSettings.wssEnabled && displaySettings.format == NEXUS_VideoFormat_ePal) {
                rc = NEXUS_Display_SetWss(display, rand());
                BDBG_ASSERT(!rc);
            }
            break;
        case 5:
            if (displayVbiSettings.cgmsEnabled && platformSettings.displayModuleSettings.vbi.allowCgmsB ) {
                uint32_t cgmsData[5]; /* intentionally uninitialized */
                rc = NEXUS_Display_SetCgmsB(display, cgmsData, 5);
                BDBG_ASSERT(!rc);
            }
            break;
        case 6:
            if (displayVbiSettings.vpsEnabled && displaySettings.format == NEXUS_VideoFormat_ePal) {
                NEXUS_VpsData vpsData; /* intentionally uninitialized */
                rc = NEXUS_Display_SetVps(display, &vpsData);
                BDBG_ASSERT(!rc);
            }
            break;
        case 7:
            if (displayVbiSettings.closedCaptionEnabled && displaySettings.format == NEXUS_VideoFormat_eNtsc) {
                NEXUS_ClosedCaptionData ccData[10]; /* intentionally uninitialized */
                unsigned num;
                unsigned i;
                for (i=0;i<10;i++) {
                    ccData[i].field = rand() % 2;
                }
                rc = NEXUS_Display_WriteClosedCaption(display, ccData, rand() % 11, &num);
                BDBG_ASSERT(!rc);
                if (!rc) {
                    BDBG_ASSERT(num <= 10);
                }
            }
            break;
        case 8:
            if (displayVbiSettings.teletextEnabled && displaySettings.format == NEXUS_VideoFormat_ePal) {
#define MAX_TT_LINES 40 /* intentionally more than the internal field limit */
                NEXUS_TeletextLine ttData[MAX_TT_LINES]; /* intentionally uninitialized */
                unsigned num;
                unsigned n = rand() % (MAX_TT_LINES+1);
                if (rand() % 5 == 0) {
                    /* make sure the boundary case of "too many" is hit */
                    for (n=0;n<(unsigned)MAX_TT_LINES - (rand()%5);n++) {
                        ttData[n].lineNumber = n;
                        ttData[n].framingCode = n;
                    }
                }
                rc = NEXUS_Display_WriteTeletext(display, ttData, n, &num);
                BDBG_ASSERT(!rc);
                if (!rc) {
                    BDBG_ASSERT(num <= MAX_TT_LINES);
                }
            }
            break;
        case 9:
            if (displayVbiSettings.gemStarEnabled && displaySettings.format == NEXUS_VideoFormat_ePal) {
                NEXUS_GemStarData gsData[10]; /* intentionally uninitialized */
                unsigned num;
                rc = NEXUS_Display_WriteGemStar(display, gsData, rand() % 11, &num);
                BDBG_ASSERT(!rc);
                if (!rc) {
                    BDBG_ASSERT(num <= 10);
                }
            }
            break;
        }
        BKNI_Sleep(rand() % 100);
    }

    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}

