/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: heaps.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 7/11/11 1:07p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/heaps.c $
* 
* Hydra_Software_Devel/1   7/11/11 1:07p erickson
* SWDEPRECATED-2425: add display/heaps
* 
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(heaps);

static struct {
    NEXUS_VideoFormat format;
    const char *str;
} g_mainDisplayFormat[] = {
    {NEXUS_VideoFormat_eNtsc, "ntsc"},
    {NEXUS_VideoFormat_ePal, "pal"},
    {NEXUS_VideoFormat_e480p, "480p"},
    {NEXUS_VideoFormat_e720p, "720p"},
    {NEXUS_VideoFormat_e1080i, "1080i"},
    {NEXUS_VideoFormat_e1080p30hz, "1080p30hz"}
};

static struct {
    NEXUS_VideoFormat format;
    const char *str;
} g_secondaryDisplayFormat[] = {
    {NEXUS_VideoFormat_eNtsc, "ntsc"}, 
    {NEXUS_VideoFormat_ePal, "pal"}
};

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder[NEXUS_NUM_VIDEO_DECODERS];
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayHandle display[NEXUS_NUM_DISPLAYS];
    NEXUS_VideoWindowHandle window[NEXUS_NUM_DISPLAYS][NEXUS_NUM_VIDEO_WINDOWS];
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned i, j;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    BDBG_CASSERT(NEXUS_NUM_VIDEO_WINDOWS <= NEXUS_NUM_VIDEO_DECODERS);
    
    for (i=0;i<NEXUS_NUM_VIDEO_DECODERS;i++) {
        videoDecoder[i] = NEXUS_VideoDecoder_Open(i, NULL);
        BDBG_ASSERT(videoDecoder[i]);
    }

    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        display[i] = NEXUS_Display_Open(i, &displaySettings);
        BDBG_ASSERT(display[i]);
        for (j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++) {
            window[i][j] = NEXUS_VideoWindow_Open(display[i], j);
            BDBG_ASSERT(window[i][j]);
            
            rc = NEXUS_VideoWindow_AddInput(window[i][j], NEXUS_VideoDecoder_GetConnector(videoDecoder[j]));
            BDBG_ASSERT(!rc);
        }
    }
    
    NEXUS_DisplayModule_SetAutomaticPictureQuality();
    
    for (i=0;i<sizeof(g_mainDisplayFormat)/sizeof(g_mainDisplayFormat[0]);i++) {
        BDBG_WRN(("display[0] = %s", g_mainDisplayFormat[i].str));
        NEXUS_Display_GetSettings(display[0], &displaySettings);
        displaySettings.format = g_mainDisplayFormat[i].format;
        rc = NEXUS_Display_SetSettings(display[0], &displaySettings);
        BDBG_ASSERT(!rc);
        
        for (j=0;j<sizeof(g_secondaryDisplayFormat)/sizeof(g_secondaryDisplayFormat[0]);j++) {
            BDBG_WRN(("display[1] = %s", g_secondaryDisplayFormat[j].str));
            NEXUS_Display_GetSettings(display[1], &displaySettings);
            displaySettings.format = g_secondaryDisplayFormat[j].format;
            rc = NEXUS_Display_SetSettings(display[1], &displaySettings);
            BDBG_ASSERT(!rc);
        }
    }

    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        for (j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++) {
            NEXUS_VideoWindow_Close(window[i][j]);
        }
        NEXUS_Display_Close(display[i]);
    }
    for (i=0;i<NEXUS_NUM_VIDEO_DECODERS;i++) {
        NEXUS_VideoDecoder_Close(videoDecoder[i]);
    }    
    NEXUS_Platform_Uninit();

    return 0;
}

