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
* $brcm_Workfile: display_vbi.c $
* $brcm_Revision: 9 $
* $brcm_Date: 7/23/12 10:20a $
*
*
* Revision History:
*
* $brcm_Log: /nexus/examples/video/display_vbi.c $
* 
* 9   7/23/12 10:20a erickson
* SW7425-3545: add clean shutdown
* 
* 8   6/22/12 1:37p erickson
* SW7425-3310: show how NEXUS_Display_WriteTeletext/WriteClosedCaption
*  flow control works. remove HDMI output preferred format for VBI test.
* 
* 7   3/29/12 3:39p erickson
* SW7435-77: set allowVps
* 
* 6   3/27/12 11:59a erickson
* SW7435-77: set allowTeletext
* 
* 5   1/26/12 11:23a rjlewis
* SW7425-1136: Added HDMI Support.
* 
* 4   2/25/11 4:43p erickson
* SW7422-250: add #if NEXUS_AMOL_SUPPORTED
*
* 2   2/15/11 2:11p erickson
* SW7420-1491: fix gemstar, add printf's
*
* 1   4/2/09 5:11p erickson
* PR52993: add example
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This example only shows the api's for encoding VBI on a display */

#if BCHP_CHIP == 7425
#define NEXUS_AMOL_SUPPORTED 1
#endif

int main(int argc, char **argv)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    NEXUS_VideoFormat format = NEXUS_VideoFormat_eNtsc;

    if (argc > 1 && !strcmp(argv[1], "pal")) {
        format = NEXUS_VideoFormat_ePal;
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb = false; /* be aware that your teletext system may require setting this */
    platformSettings.displayModuleSettings.vbi.allowVps = true;
    platformSettings.displayModuleSettings.vbi.allowGemStar = true;
    platformSettings.displayModuleSettings.vbi.allowAmol = true;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = format;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    /* do not change to preferred format because of VBI requirements */
#endif

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    if (format == NEXUS_VideoFormat_ePal) {
        displayVbiSettings.teletextEnabled = true;
        displayVbiSettings.wssEnabled = true;
        displayVbiSettings.vpsEnabled = true;
    }
    else {
        displayVbiSettings.gemStarEnabled = true;
        displayVbiSettings.gemStar.baseLineTop = 15;
        displayVbiSettings.gemStar.lineMaskTop = 0xF;
        displayVbiSettings.gemStar.baseLineBottom = 15 + 263;
        displayVbiSettings.gemStar.lineMaskBottom = 0xF;

        displayVbiSettings.closedCaptionEnabled = true;
        displayVbiSettings.cgmsEnabled = true;

#if NEXUS_AMOL_SUPPORTED
/* can't enable AMOL unless HW supports it. for this example, we rely on a compile time switch. */
        displayVbiSettings.amolEnabled = true;
        displayVbiSettings.amol.type = NEXUS_AmolType_eII_Highrate;
#endif
    }
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

    if (displayVbiSettings.teletextEnabled) {
/* intentionally make it larger than encoder queue to show flow control */
#define NUM_TT_DATA 50
        NEXUS_TeletextLine ttData[NUM_TT_DATA];
        unsigned num;
        unsigned i;
        unsigned total = 0;

        for (i=0;i<NUM_TT_DATA;i++) {
            ttData[i].lineNumber = 0;
            ttData[i].framingCode = 0;
            ttData[i].data[0] = 0x00;
        }

        printf("write %d teletext entries\n", NUM_TT_DATA);
        while (1) {
            rc = NEXUS_Display_WriteTeletext(display, &ttData[total], NUM_TT_DATA-total, &num);
            BDBG_ASSERT(!rc);
            printf(" wrote %d\n", num);
            total += num;
            if (total == NUM_TT_DATA) break;
            BKNI_Sleep(10); /* simple flow control */
        }
    }

    if (displayVbiSettings.closedCaptionEnabled) {
/* intentionally make it larger than encoder queue to show flow control */
#define NUM_CC_DATA 50
        NEXUS_ClosedCaptionData ccData[NUM_CC_DATA];
        unsigned num;
        unsigned i;
        unsigned total = 0;

        for (i=0;i<NUM_CC_DATA;i++) {
            ccData[i].field = 0;
            ccData[i].data[0] = 0x00;
            ccData[i].data[1] = 'A'+i; /* TODO: write valid sequence */
        }
        printf("write %d CC entries\n", NUM_CC_DATA);
        while (1) {
            rc = NEXUS_Display_WriteClosedCaption(display, &ccData[total], NUM_CC_DATA-total, &num);
            BDBG_ASSERT(!rc);
            printf(" wrote %d\n", num);
            total += num;
            if (total == NUM_CC_DATA) break;
            BKNI_Sleep(10); /* simple flow control */
        }
    }

    if (displayVbiSettings.gemStarEnabled) {
        NEXUS_GemStarData gsData;
        unsigned num;

        gsData.topField = true;
        gsData.lineMask = 0xF << 15;
        gsData.errorMask = 0;
        gsData.data[0] = 0x00;
        printf("write GemStar\n");
        rc = NEXUS_Display_WriteGemStar(display, &gsData, 1, &num);
        BDBG_ASSERT(!rc);
    }

    if (displayVbiSettings.wssEnabled) {
        printf("set WSS\n");
        rc = NEXUS_Display_SetWss(display, 0x00);
        BDBG_ASSERT(!rc);
    }

    if (displayVbiSettings.cgmsEnabled) {
        printf("set CGMS\n");
        rc = NEXUS_Display_SetCgms(display, 0x00);
        BDBG_ASSERT(!rc);
    }

    if (displayVbiSettings.vpsEnabled) {
        NEXUS_VpsData vpsData;
        vpsData.byte05 = 0;
        vpsData.byte11 = 0;
        vpsData.byte12 = 0;
        vpsData.byte13 = 0;
        vpsData.byte14 = 0;
        vpsData.byte15 = 0x00;
        printf("set VPS\n");
        rc = NEXUS_Display_SetVps(display, &vpsData);
        BDBG_ASSERT(!rc);
    }

    if (displayVbiSettings.amolEnabled) {
        NEXUS_AmolData amolData[3];
        unsigned i, num;

        for (i=0;i<3;i++) {
            amolData[i].topField = rand()%2;
            /* leave .data[] uninitialized. data format is private. */
            amolData[i].length = 24; /* max for NEXUS_AmolType_eII_Highrate */
        }
        printf("write AMOL\n");
        rc = NEXUS_Display_WriteAmol(display, amolData, 3, &num);
        BDBG_ASSERT(!rc);
    }

    printf("Press ENTER to exit\n");
    getchar();
    
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}
