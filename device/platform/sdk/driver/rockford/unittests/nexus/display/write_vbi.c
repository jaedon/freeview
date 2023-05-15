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
 * $brcm_Workfile: write_vbi.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 7/13/12 3:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/write_vbi.c $
 * 
 * Hydra_Software_Devel/11   7/13/12 3:22p erickson
 * SW7425-3107: compile in cc data for test
 * 
 * Hydra_Software_Devel/10   7/13/12 11:11a ahulse
 * SW7425-3310: fix write_vbi and Makefile so it compiles and runs.
 * 
 * Hydra_Software_Devel/9   3/29/12 3:38p erickson
 * SW7435-77: set allowVps
 * 
 * Hydra_Software_Devel/8   3/27/12 12:00p erickson
 * SW7435-77: set allowTeletext
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
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_video_window.h"
#include "nexus_parser_band.h"

#include "ttdata.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* cc data is "[YELLS FIERCELY]" */
static uint8_t g_ccData[] = {
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80,
0x94, 0x20, 0x80, 0x80,
0x94, 0x20, 0x80, 0x80,
0x94, 0xae, 0x80, 0x80,
0x94, 0xae, 0x80, 0x80,
0x94, 0xf4, 0x80, 0x80,
0x94, 0xf4, 0x80, 0x80,
0x97, 0x23, 0x80, 0x80,
0x97, 0x23, 0x80, 0x80,
0x5b, 0xd9, 0x80, 0x80,
0x45, 0x4c, 0x80, 0x80,
0x4c, 0xd3, 0x80, 0x80,
0x20, 0x46, 0x80, 0x80,
0x49, 0x45, 0x80, 0x80,
0x52, 0x43, 0x80, 0x80,
0x45, 0x4c, 0x80, 0x80,
0xd9, 0x5d, 0x80, 0x80,
0x94, 0x2f, 0x80, 0x80,
0x94, 0x2f, 0x80, 0x80,
};

int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_PlatformConfiguration platformConfig;
    char buf[256];
    NEXUS_PlatformSettings platformSettings;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    platformSettings.displayModuleSettings.vbi.allowVps = true;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;

    printf("Enter format (NTSC or PAL):\n");
    fgets(buf, sizeof(buf), stdin);
    if (!strcasecmp(buf, "pal\n"))
        displaySettings.format = NEXUS_VideoFormat_ePal;
    else
        displaySettings.format = NEXUS_VideoFormat_eNtsc;

    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));

    while (1) {
        NEXUS_Error rc;

        printf(
        "Option\n"
        "1) Set CGMS\n"
        "2) Set WSS\n"
        "3) Set VPS\n"
        "4) Write Closed Caption\n"
        "5) Write Teletext from compiled-in data structure\n"
        "6) Write GemStar from file\n"
        "0) Disable all VBI\n"
        );
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == '0') {
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.cgmsEnabled = false;
            displayVbiSettings.wssEnabled = false;
            displayVbiSettings.vpsEnabled = false;
            displayVbiSettings.teletextEnabled = false;
            displayVbiSettings.closedCaptionEnabled = false;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        else if (buf[0] == '1') {
            printf("Enter CGMS value:\n");
            fgets(buf, sizeof(buf), stdin);

            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.cgmsEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            rc = NEXUS_Display_SetCgms(display, atoi(buf));
            BDBG_ASSERT(!rc);
        }
        else if (buf[0] == '2') {
            printf("Enter WSS value:\n");
            fgets(buf, sizeof(buf), stdin);

            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.wssEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            rc = NEXUS_Display_SetWss(display, atoi(buf));
            BDBG_ASSERT(!rc);
        }
        else if (buf[0] == '3') {
            NEXUS_VpsData vpsData;

            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.vpsEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            printf("Enter VPS byte 05:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte05 = strtoul(buf, NULL, 0);

            printf("Enter VPS byte 11:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte11 = strtoul(buf, NULL, 0);

            printf("Enter VPS byte 12:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte12 = strtoul(buf, NULL, 0);

            printf("Enter VPS byte 13:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte13 = strtoul(buf, NULL, 0);

            printf("Enter VPS byte 14:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte14 = strtoul(buf, NULL, 0);

            printf("Enter VPS byte 15:\n");
            fgets(buf, sizeof(buf), stdin);
            vpsData.byte15 = strtoul(buf, NULL, 0);

            rc = NEXUS_Display_SetVps(display, &vpsData);
            BDBG_ASSERT(!rc);
        }
        else if (buf[0] == '4') {
            unsigned total_written = 0;
            NEXUS_ClosedCaptionData ccData;
    
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.closedCaptionEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            BKNI_Memset(&ccData, 0, sizeof(ccData));
            while (total_written < sizeof(g_ccData)) {
                unsigned num;
                ccData.data[0] = g_ccData[total_written];
                ccData.data[1] = g_ccData[total_written+1];
                rc = NEXUS_Display_WriteClosedCaption(display, &ccData, 1, &num);
                BDBG_ASSERT(!rc);
                if (num == 1) {
                    total_written += 2;
                    ccData.field = !ccData.field;
                }
                else {
                    BKNI_Sleep(10);
                }
            }
            printf("wrote %d of %d\n", total_written, sizeof(g_ccData));
        }
        else if (buf[0] == '5') {
            unsigned i;
            const BVBI_TT_Line *s_aLines;

            printf("select data 0 or 1:\n");
            fgets(buf, sizeof(buf), stdin);
            if (atoi(buf) == 0) {
                s_aLines = s_aLines0;
            }
            else {
                s_aLines = s_aLines1;
            }
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.teletextEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            for (i=0;s_aLines[i].ucFramingCode != BVBI_TT_INVALID_FRAMING_CODE;i += 2) {
                NEXUS_TeletextLine ttData[2];
                unsigned num;

                /* The data is encoded for 2 lines per field */
                ttData[0].lineNumber = 0;
                ttData[0].framingCode = s_aLines[i].ucFramingCode;
                BKNI_Memcpy(ttData[0].data, s_aLines[i].aucData, NEXUS_TELETEXT_LINESIZE);

                ttData[1].lineNumber = 1;
                ttData[1].framingCode = s_aLines[i+1].ucFramingCode;
                BKNI_Memcpy(ttData[0].data, s_aLines[i+1].aucData, NEXUS_TELETEXT_LINESIZE);

                rc = NEXUS_Display_WriteTeletext(display, ttData, 2, &num);
                BDBG_ASSERT(!rc);

                /* Nexus must consume both lines or neither. */
                BDBG_ASSERT(num == 2 || num == 0);
                if (!num) {
                    BKNI_Sleep(1);
                    i -= 2; /* write those lines again */
                }
            }
        }
        else if (buf[0] == '6') {
            FILE *finput;
            char *nl;

            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.gemStarEnabled = true;
            displayVbiSettings.gemStar.baseLineTop = 15;
            displayVbiSettings.gemStar.lineMaskTop = 0x1f;
            displayVbiSettings.gemStar.baseLineBottom = 15 + 263;
            displayVbiSettings.gemStar.lineMaskBottom = 0x1f;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);

            printf("Enter file:\n");
            fgets(buf, sizeof(buf), stdin);
            nl = strchr(buf, '\n');
            if(nl) {
                *nl = '\0';
            }

            finput = fopen(buf, "rb");

            BDBG_ASSERT(finput);
            while (1) {
                unsigned i = 0;
                int n = fread(buf, 1, 256, finput);
                if (n <= 0) break;
                printf("read %d bytes\n", n);

                while (i < (unsigned)n) {
                    unsigned j;
                    unsigned num;
                    NEXUS_GemStarData gsData;
                    BKNI_Memset(&gsData, 0, sizeof(gsData));

                    gsData.topField = true;
                    gsData.lineMask =
                        (1 << 15) |
                        (1 << 16) |
                        (1 << 17) |
                        (1 << 18) |
                        (1 << 19);
                    gsData.errorMask = 0;

                    for (j = 0; j<NEXUS_GEMSTAR_MAX_LINES && i<(unsigned)n; j++,i++) {
                        gsData.data[j] = buf[i];
                    }

                    rc = NEXUS_Display_WriteGemStar(display, &gsData, 1, &num);
                    BDBG_ASSERT(!rc);
                    if (num != 1) {
                        i -= NEXUS_GEMSTAR_MAX_LINES;
                        BKNI_Sleep(10);
                    }
                }
            }
            
            fclose(finput);
        }
    }

    return 0;
}

