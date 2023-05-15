/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: custom_format.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 3/22/11 11:03a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/custom_format.c $
 * 
 * Hydra_Software_Devel/4   3/22/11 11:03a mward
 * SW7425-64:  set_xga_60hz_panel() is used only #if NEXUS_DTV_PLATFORM.
 * 
 * Hydra_Software_Devel/3   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   3/19/09 2:27p erickson
 * PR53151: added stress test of NEXUS_Display_SetCustomFormatSettings
 *
 ******************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_core_utils.h" /* NEXUS_VideoFormatInfo */
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

/* Copied from magnum/commonutils/fmt/bfmt.h.
These bitmasks are noted as deprecated, but still used. Instead of mapping a Nexus API, we'll just do copy-and-paste. */
#define BFMT_VERT_50Hz               0x00000001
#define BFMT_VERT_59_94Hz            0x00000002
#define BFMT_VERT_60Hz               0x00000004
#define BFMT_PXL_148_5MHz            0x20000000 /* 60Hz */
#define BFMT_PXL_148_5MHz_DIV_1_001  0x40000000 /* 59.94 Hz */

/* WXGA microcode */
static const uint32_t g_DtRamBVBInput_CUSTOM_1024x768p_T1344x806[] =
{
    0x0064A001, /*  64 */
    0x00650005, /*  65 */
    0x00656001, /*  66 */
    0x0065C00D, /*  67 */
    0x00668300, /*  68 */
    0x0065C011, /*  69 */
    0x00662001, /*  70 */
    0x00840000, /*  71 */
    0x00000000, /*  72 */
    0x00000000, /*  73 */
    0x00284018, /*  74 */
    0x002D4088, /*  75 */
    0x002440A0, /*  76 */
    0x003453F5, /*  77 */
    0x00000000, /*  78 */
    0x00000000, /*  79 */
    0x00244018, /*  80 */
    0x00254088, /*  81 */
    0x002440A0, /*  82 */
    0x003453F5, /*  83 */
    0x00000000, /*  84 */
    0x00000000, /*  85 */
    0x00244018, /*  86 */
    0x00214088, /*  87 */
    0x002040A0, /*  88 */
    0x003053F5, /*  89 */
    0x00000000, /*  90 */
    0x00000000, /*  91 */
    0x00204018, /*  92 */
    0x00214088, /*  93 */
    0x002040A0, /*  94 */
    0x003053F5, /*  95 */
    0x00000000, /*  96 */
    0x00000000, /*  97 */
    0x00204018, /*  98 */
    0x00214088, /*  99 */
    0x002040A0, /* 100 */
    0x003053EA, /* 101 */
    0x00000000, /* 102 */
    0x00000000, /* 103 */
    0x00202018, /* 104 */
    0x00212088, /* 105 */
    0x002020A0, /* 106 */
    0x003233F5, /* 107 */
    0x00000000, /* 108 */
    0x00000000, /* 109 */
    0x00202018, /* 110 */
    0x00212088, /* 111 */
    0x002020A0, /* 112 */
    0x003233EA, /* 113 */
    0x00000000, /* 114 */
    0x00000000, /* 115 */
    0x00000000, /* 116 */
    0x00000000, /* 117 */
    0x00000000, /* 118 */
    0x00000000, /* 119 */
    0x00000000, /* 120 */
    0x00000000, /* 121 */
    0x00000000, /* 122 */
    0x00000000, /* 123 */
    0x00000000, /* 124 */
    0x00000000, /* 125 */
    0x00072508, /* 126 */
    0x005D47B6, /* 127 */
};


#if NEXUS_DTV_PLATFORM
void set_xga_60hz_panel(NEXUS_DisplayHandle display, NEXUS_VideoFormat videoFormat)
{
    NEXUS_DisplayCustomFormatSettings xga60Settings;
    NEXUS_Error rc;

    NEXUS_Display_GetDefaultCustomFormatSettings(&xga60Settings);

    xga60Settings.width = 1024;
    xga60Settings.height = 768;
    xga60Settings.scanWidth = 1344;
    xga60Settings.scanHeight = 806;
    xga60Settings.topActive = 160; /* Philip = 18; */
    xga60Settings.verticalFrequencyMask = BFMT_VERT_60Hz | BFMT_VERT_59_94Hz;
    xga60Settings.verticalFrequency = 6000;
    xga60Settings.pixelFrequencyMask = BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz;
    xga60Settings.pixelFrequency = 6493; /* Philip = 6500; */
    xga60Settings.aspectRatio = NEXUS_DisplayAspectRatio_e16x9;
    strcpy(xga60Settings.name, "XGA 60Hz");

    BKNI_Memcpy(xga60Settings.vecMicrocode, g_DtRamBVBInput_CUSTOM_1024x768p_T1344x806, sizeof(xga60Settings.vecMicrocode));

    /* 60.00 rate */
    xga60Settings.totalRates = 2;
    xga60Settings.rate[0].pixelClockRate = BFMT_PXL_148_5MHz;
    xga60Settings.rate[0].mDiv = 2;
    xga60Settings.rate[0].nDiv = 0x10D9CC47;
    xga60Settings.rate[0].rDiv = 112;
    xga60Settings.rate[0].sampleInc = 3;
    xga60Settings.rate[0].numerator = 912;
    xga60Settings.rate[0].denominator = 2821;
    xga60Settings.rate[0].vcoRange = 1;
    xga60Settings.rate[0].linkDivCtrl = 1;
    xga60Settings.rate[0].p2 = 1;
    strcpy(xga60Settings.rate[0].name, "65.00");

    /* 59.94 rate */
    xga60Settings.rate[1].pixelClockRate = BFMT_PXL_148_5MHz_DIV_1_001;
    xga60Settings.rate[1].mDiv = 2;
    xga60Settings.rate[1].nDiv = 0x10D57D0C;
    xga60Settings.rate[1].rDiv = 112;
    xga60Settings.rate[1].sampleInc = 3;
    xga60Settings.rate[1].numerator = 81;
    xga60Settings.rate[1].denominator = 248;
    xga60Settings.rate[1].vcoRange = 1;
    xga60Settings.rate[1].linkDivCtrl = 1;
    xga60Settings.rate[1].p2 = 1;
    strcpy(xga60Settings.rate[1].name, "64.93");

    rc = NEXUS_Display_SetCustomFormatSettings(display, videoFormat, &xga60Settings);
    BDBG_ASSERT(!rc);
}
#endif
int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
    unsigned loops = 300;

    setenv("no_dynamic_rts", "y", true);

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;

    /**
    Bringing up a custom panel has two parts:
    1) Set LVDS settings. This must be done at NEXUS_Platform_Init time.
    2) Set VDC (CMP/VEC) settings. This can be done later.

    TODO: add support for FHD/WXGA/XGA/etc. Add support for 50hz/60hz/etc.
    **/

#if NEXUS_DTV_PLATFORM
    /* Part 1: Set LVDS settings for XGA panel */
    platformSettings.displayModuleSettings.panel.dvoLinkMode = NEXUS_PanelOutputLinkMode_eCustom;
    platformSettings.displayModuleSettings.panel.customLinkSettings.lvdsColorMode = NEXUS_LvdsColorMode_e10Bit;
    platformSettings.displayModuleSettings.panel.customLinkSettings.lvdsLinkMap1 = NEXUS_LvdsLinkMap_eInternalLink1;
    platformSettings.displayModuleSettings.panel.lvdsMappingRule = NEXUS_PanelOutputMappingRule_eJeidaRule;
    platformSettings.displayModuleSettings.panel.linkSettings[0].maskLinkHSync = 0;
    platformSettings.displayModuleSettings.panel.linkSettings[0].maskLinkVSync = 0;
    platformSettings.displayModuleSettings.panel.linkSettings[0].mapLinkChannel4 = 0;
    platformSettings.displayModuleSettings.panel.linkSettings[0].mapLinkChannel3 = 1;
    platformSettings.displayModuleSettings.panel.linkSettings[0].mapLinkChannel2 = 2;
    platformSettings.displayModuleSettings.panel.linkSettings[0].mapLinkChannel1 = 3;
    platformSettings.displayModuleSettings.panel.linkSettings[0].mapLinkChannel0 = 4;
#endif

    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);

    while (loops--) {
        unsigned state = rand() % 5;
        printf("entering state %d, %d loops to go\n", state, loops);
        switch (state) {
#if NEXUS_DTV_PLATFORM
        case 0:
            set_xga_60hz_panel(display, NEXUS_VideoFormat_eCustom2);
            break;
#endif
        case 1:
            rc = NEXUS_Display_SetCustomFormatSettings(display, NEXUS_VideoFormat_eCustom2, NULL);
            BDBG_ASSERT(!rc);
            break;
        case 2:
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.format = NEXUS_VideoFormat_eCustom0;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            break;
        case 3:
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.format = NEXUS_VideoFormat_eCustom0;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            break;
        case 4:
            NEXUS_Display_Close(display);
            NEXUS_Display_GetDefaultSettings(&displaySettings);
            BKNI_Sleep(rand() % 500);
            display = NEXUS_Display_Open(0, &displaySettings);
            break;
        default:
            break;
        }
        if (rand() % 3 == 0) BKNI_Sleep(rand() % 500);
    }

    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}
