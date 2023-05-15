/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: autodetect.c $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 2/4/08 10:39a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/autodetect.c $
* 
* Hydra_Software_Devel/3   2/4/08 10:39a erickson
* PR38679: update for 740x
* 
* Hydra_Software_Devel/2   1/23/08 12:38p erickson
* PR38919: update unittests
* 
* Hydra_Software_Devel/1   12/19/07 12:47p erickson
* PR38213: added autodetect
* 
* Hydra_Software_Devel/1   12/19/07 11:37a erickson
* PR38213: added display unittests
* 
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
#include "nexus_analog_video_decoder.h"
#include "nexus_video_adj.h"
#include "nexus_picture_ctrl.h"
#endif
#include "nexus_component_output.h"
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
void print_analogVideoDecoder_status(NEXUS_AnalogVideoDecoderHandle analogVideoDecoder)
{
    NEXUS_AnalogVideoDecoderStatus status;
    NEXUS_AnalogVideoDecoder_GetStatus(analogVideoDecoder, &status);
  
    printf("Status:\n");        
    printf("  locked: %d\n", status.locked);
    if (status.unknownFormat) {
        printf("  unknown format\n");
    }
    else{
        printf("  freq:     h=%d, v=%d\n", status.hFreq, status.vFreq);
        printf("  polarity: h=%d, v=%d\n", status.hPolarity, status.hPolarity);
        printf("  size:     h=%d, v=%d\n", status.avWidth, status.avHeight);
        printf("  blank:    h=%d, v=%d\n", status.hBlank, status.vBlank);
    }
    printf("\n");
}

NEXUS_VideoFormat get_format()
{
    char cmdstr[256];
    printf("Enter format: (ntsc,480p,1080i,720p,pal,576p,1080i50hz,720p50hz,none):\n");
    gets(cmdstr);
    if (!strcasecmp(cmdstr, "ntsc")) {
        return NEXUS_VideoFormat_eNtsc;
    }
    else if (!strcasecmp(cmdstr, "1080i")) {
        return NEXUS_VideoFormat_e1080i;
    }
    else if (!strcasecmp(cmdstr, "480p")) {
        return NEXUS_VideoFormat_e480p;
    }
    else if (!strcasecmp(cmdstr, "720p")) {
        return NEXUS_VideoFormat_e720p;
    }
    else if (!strcasecmp(cmdstr, "pal")) {
        return NEXUS_VideoFormat_ePal;
    }
    else if (!strcasecmp(cmdstr, "1080i50hz")) {
        return NEXUS_VideoFormat_e1080i50hz;
    }
    else if (!strcasecmp(cmdstr, "720p50hz")) {
        return NEXUS_VideoFormat_e720p50hz;
    }
    return NEXUS_VideoFormat_eUnknown;
}
#endif

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_ComponentInputHandle componentInput = NULL;
    NEXUS_ComponentInputSettings componentInputSettings;
    char cmdstr[256];
    int done = 0;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up display for 720p component output only */
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    displayCfg.displayType = NEXUS_DisplayType_eAuto;
    displayCfg.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up VDEC */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    
    NEXUS_ComponentInput_GetDefaultSettings(&componentInputSettings);
    componentInputSettings.adc[NEXUS_AnalogVideoChannel_eY] = 0; /* TODO: move to platform */
    componentInputSettings.adc[NEXUS_AnalogVideoChannel_ePr] = 1;
    componentInputSettings.adc[NEXUS_AnalogVideoChannel_ePb] = 2;
    componentInput = NEXUS_ComponentInput_Open(0, &componentInputSettings);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_ComponentInput_GetConnector(componentInput), &analogVideoDecoderSettings);
    NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    
    /* select input */
    done = 0;
    while (!done) {
        printf("Enable autodetect? (yes=1, no=0):\n");
        gets(cmdstr);
        
        analogVideoDecoderSettings.autoDetectionMode.autoMode = atoi(cmdstr);
        if (!analogVideoDecoderSettings.autoDetectionMode.autoMode) {
            printf("Select manual format\n");
            analogVideoDecoderSettings.autoDetectionMode.manualFormat = get_format();
        }
        else {
            printf("Enable NTSC? (yes=1, no=0):\n");
            gets(cmdstr);
            analogVideoDecoderSettings.autoDetectionMode.autoDetectNtsc = atoi(cmdstr);
            
            printf("Enable PAL? (yes=1, no=0):\n");
            gets(cmdstr);
            analogVideoDecoderSettings.autoDetectionMode.autoDetectPal = atoi(cmdstr);
        }
        NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);

        printf("Waiting a bit before reading status...\n");        
        BKNI_Sleep(1000);
        
        print_analogVideoDecoder_status(analogVideoDecoder);
    }
#endif

    return 0;
}

