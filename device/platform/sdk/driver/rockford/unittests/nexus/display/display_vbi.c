/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: display_vbi.c $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 6/19/12 2:47p $
*
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/display_vbi.c $
* 
* Hydra_Software_Devel/5   6/19/12 2:47p jessem
* SW7425-3274: Corrected evaluation of returned param  of several Nexus
* VBI function calls.
*
* Hydra_Software_Devel/4   3/29/12 3:38p erickson
* SW7435-77: set allowVps
*
* Hydra_Software_Devel/3   3/27/12 12:00p erickson
* SW7435-77: set allowTeletext
*
* Hydra_Software_Devel/2   5/27/11 12:24p erickson
* SWDEPRECATED-3044: fix gemstar
*
* Hydra_Software_Devel/1   10/27/08 4:28p jtna
* PR42679: added new unittest display_vbi
*
*
***************************************************************************/
/* automated unit test to exercise all public functions in nexus_display_vbi.h */

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_composite_output.h"
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
#if !NEXUS_DTV_PLATFORM /* Settop platforms only for now */
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned i;
    NEXUS_TeletextLine ttData;
    NEXUS_ClosedCaptionData ccData;
    NEXUS_GemStarData gsData;
    uint16_t wssData;
    uint32_t cgmsData;
    NEXUS_VpsData vpsData;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    platformSettings.displayModuleSettings.vbi.allowVps = true;
    platformSettings.displayModuleSettings.vbi.allowGemStar = true;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;

    /* test all permutations of NTSC/PAL and enabled=true/false */
    /* in general, let's keep input data simple. provide just enough to coax a positive pNumLinesWritten out of each function */
    for (i = 0; i < 4; i++) {
        NEXUS_Error rc;
        size_t num;
        if (i<2) {
            displaySettings.format = NEXUS_VideoFormat_eNtsc;
        }
        else {
            displaySettings.format = NEXUS_VideoFormat_ePal;
        }
        BKNI_Printf("display_vbi: Test format %s, settings %s (i=%d)\n", i<2?"NTSC":"PAL", i%2?"enabled":"disabled", i);
        if (i%2==0) {
            if (i==2) {
               (void)NEXUS_Display_RemoveAllOutputs(display);
               NEXUS_Display_Close(display);
            }
            display = NEXUS_Display_Open(0, &displaySettings);
            NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        }

        if (i%2==0) {
            /* disable all VBI */
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.teletextEnabled = false;
            displayVbiSettings.closedCaptionEnabled = false;
            displayVbiSettings.wssEnabled = false;
            displayVbiSettings.cgmsEnabled = false;
            displayVbiSettings.vpsEnabled = false;
            displayVbiSettings.gemStarEnabled = false;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }

        /* Teletext */
        BKNI_Printf("\t>Teletext\n");
        ttData.lineNumber = 0;
        ttData.framingCode = 0;
        ttData.data[0] = 0x00;
        if (i%2) {
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.teletextEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        rc = NEXUS_Display_WriteTeletext(display, &ttData, 1, &num);
        if (rc) {
                /* expect failure if not enabled */
            BDBG_ASSERT(!displayVbiSettings.teletextEnabled);
        }
        else {
                /* expect success if enabled */
            BDBG_ASSERT(displayVbiSettings.teletextEnabled);
            BDBG_ASSERT(num);
        }

        /* ClosedCaption */
        BKNI_Printf("\t>ClosedCaption\n");
        ccData.field = 0;
        ccData.data[0] = 0x00;
        ccData.data[1] = 0x01;
        if (i%2) {
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.closedCaptionEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        rc = NEXUS_Display_WriteClosedCaption(display, &ccData, 1, &num);
        if (rc) {
                /* expect failure if not enabled */
            BDBG_ASSERT(!displayVbiSettings.closedCaptionEnabled);
        }
        else {
                /* expect success if enabled */
            BDBG_ASSERT(displayVbiSettings.closedCaptionEnabled);
            BDBG_ASSERT(num);
        }

        /* GemStar */
        BKNI_Printf("\t>GemStar\n");
        gsData.topField = true;
        gsData.lineMask = 0xF << 15;
        gsData.errorMask = 0;
        gsData.data[0] = 0x00;
        if (i==1) { /* GemStar is NTSC only */
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.gemStarEnabled = true;
            displayVbiSettings.gemStar.baseLineTop = 15;
            displayVbiSettings.gemStar.lineMaskTop = 0xF;
            displayVbiSettings.gemStar.baseLineBottom = 15 + 263;
            displayVbiSettings.gemStar.lineMaskBottom = 0xF;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        if (i < 2) {
            rc = NEXUS_Display_WriteGemStar(display, &gsData, 1, &num);
			if (rc) {
                /* expect failure if not enabled */
                BDBG_ASSERT(!displayVbiSettings.gemStarEnabled);
            }
            else {
                /* expect success if enabled */
                BDBG_ASSERT(displayVbiSettings.gemStarEnabled);
                BDBG_ASSERT(num);
            }
        }

        /* WSS */
        BKNI_Printf("\t>WSS\n");
        wssData = 0x00;
        if (i%2) {
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.wssEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        rc = NEXUS_Display_SetWss(display, wssData);
        BDBG_ASSERT(displayVbiSettings.wssEnabled ? !rc : rc);

        /* CGMS */
        BKNI_Printf("\t>CGMS\n");
        cgmsData = 0x00;
        if (i==1) { /* CGMS not supported with PAL */
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.cgmsEnabled = true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        if (i < 2) {
            rc = NEXUS_Display_SetCgms(display, cgmsData);
            BDBG_ASSERT(displayVbiSettings.cgmsEnabled ? !rc : rc);
        }

        /* VPS */
        BKNI_Printf("\t>VPS\n");
        vpsData.byte05 = vpsData.byte11 = vpsData.byte12 = vpsData.byte13 = vpsData.byte14 = vpsData.byte15 = 0x00;
        if (i==3) { /* VPS is PAL only */
            NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
            displayVbiSettings.vpsEnabled= true;
            rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
            BDBG_ASSERT(!rc);
        }
        if (i>1) {
            rc = NEXUS_Display_SetVps(display, &vpsData);
            BDBG_ASSERT(displayVbiSettings.vpsEnabled ? !rc : rc);
        }

        /* Macrovision and DCS is not enabled by default. For now, let's just ensure that it doesn't assert within the function */
        BKNI_Printf("\t>Macrovision, DCS\n");
        NEXUS_Display_SetMacrovision(display, NEXUS_DisplayMacrovisionType_eNone, NULL);
        NEXUS_Display_SetDcs(display, NEXUS_DisplayDcsType_off);
    }

    BKNI_Printf("display_vbi: Test completed\n");
#endif /* !NEXUS_DTV_PLATFORM */
    return 0;
}
