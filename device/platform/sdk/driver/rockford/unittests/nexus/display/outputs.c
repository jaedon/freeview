/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: outputs.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 3/9/12 10:22a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/outputs.c $
* 
* Hydra_Software_Devel/8   3/9/12 10:22a randyjew
* SW7425-2344: include <time.h> for implicit declartion
* 
* Hydra_Software_Devel/7   3/7/12 6:51p mward
* SW7425-2344:  Condition in case NEXUS_NUM_xxx_OUTPUTS=0.
* 
* Hydra_Software_Devel/6   3/7/12 3:51p erickson
* SW7425-2344: add connection test and non-dac settings test
*
* Hydra_Software_Devel/5   5/10/10 6:56p mward
* SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
* and other errors and warnings.
*
* Hydra_Software_Devel/4   3/23/09 1:31p erickson
* PR42679: update
*
* Hydra_Software_Devel/3   2/20/09 11:43a erickson
* PR52347: update test for settop
*
* Hydra_Software_Devel/2   2/20/09 11:21a erickson
* PR52347: update test
*
* Hydra_Software_Devel/1   2/20/09 10:54a erickson
* PR52270: add test
*
* Hydra_Software_Devel/5   12/8/08 1:55p erickson
* PR50020: protect against destroyed tf_isr/bf_isr INT callback handles
*
* Hydra_Software_Devel/4   9/2/08 5:24p erickson
* PR46473: remove TODO
*
* Hydra_Software_Devel/3   8/19/08 12:22p erickson
* PR45794: add more AnalogVideoDecoder shutdown cases
*
* Hydra_Software_Devel/2   8/4/08 11:40a erickson
* PR45280: update tests for current 3556
*
* Hydra_Software_Devel/1   7/7/08 4:06p erickson
* PR44619: test variations of open/close
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
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* macros to collapse all video outputs into unified index for connection management */
#define B_COMPOSITE_INDEX(i) (i)
#define B_SVIDEO_INDEX(i) (NEXUS_NUM_COMPOSITE_OUTPUTS+(i))
#define B_COMPONENT_INDEX(i) (NEXUS_NUM_COMPOSITE_OUTPUTS+NEXUS_NUM_SVIDEO_OUTPUTS+(i))
#define B_HDMI_INDEX(i) (NEXUS_NUM_COMPOSITE_OUTPUTS+NEXUS_NUM_SVIDEO_OUTPUTS+NEXUS_NUM_COMPONENT_OUTPUTS+(i))
#define B_TOTAL_INDEX (NEXUS_NUM_COMPOSITE_OUTPUTS+NEXUS_NUM_SVIDEO_OUTPUTS+NEXUS_NUM_COMPONENT_OUTPUTS+NEXUS_NUM_HDMI_OUTPUTS)

static int connect_test(void)
{
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayHandle display;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned i;
    NEXUS_Error rc;
    bool connected[B_TOTAL_INDEX];
    NEXUS_VideoOutput videoOutput[B_TOTAL_INDEX];
    unsigned loops = 1000;

    /* print macros so debug logs can be interpretted */
    printf("index map: %d %d %d %d\n", NEXUS_NUM_COMPOSITE_OUTPUTS, NEXUS_NUM_SVIDEO_OUTPUTS, NEXUS_NUM_COMPONENT_OUTPUTS, NEXUS_NUM_HDMI_OUTPUTS);
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.openOutputs = true;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_Memset(&connected, 0, sizeof(connected));
    BKNI_Memset(&videoOutput, 0, sizeof(videoOutput));

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

    #if NEXUS_NUM_COMPOSITE_OUTPUTS
    for (i=0;i<NEXUS_NUM_COMPOSITE_OUTPUTS;i++) {
        if (platformConfig.outputs.composite[i]) {
            videoOutput[B_COMPOSITE_INDEX(i)] = NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[i]);
        }
    }
    #endif
    #if NEXUS_NUM_SVIDEO_OUTPUTS
    for (i=0;i<NEXUS_NUM_SVIDEO_OUTPUTS;i++) {
        if (platformConfig.outputs.svideo[i]) {
            videoOutput[B_SVIDEO_INDEX(i)] = NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[i]);
        }
    }
    #endif
    #if NEXUS_NUM_COMPONENT_OUTPUTS
    for (i=0;i<NEXUS_NUM_COMPONENT_OUTPUTS;i++) {
        if (platformConfig.outputs.component[i]) {
            videoOutput[B_COMPONENT_INDEX(i)] = NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[i]);
        }
    }
    #endif
    #if NEXUS_NUM_HDMI_OUTPUTS
    for (i=0;i<NEXUS_NUM_HDMI_OUTPUTS;i++) {
        if (platformConfig.outputs.hdmi[i]) {
            videoOutput[B_HDMI_INDEX(i)] = NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[i]);
        }
    }
    #endif
    
    while (--loops) {
        switch (rand() % 4) {
        case 0:
            i = rand()%B_TOTAL_INDEX;
            if (videoOutput[i] && !connected[i]) {
                printf("connect %d\n", i);
                rc = NEXUS_Display_AddOutput(display, videoOutput[i]);
                BDBG_ASSERT(!rc);
                connected[i] = true;
            }
            break;
        case 1:
            i = rand()%B_TOTAL_INDEX;
            if (connected[i]) {
                printf("disconnect %d\n", i);
                rc = NEXUS_Display_RemoveOutput(display, videoOutput[i]);
                BDBG_ASSERT(!rc);
                connected[i] = false;
            }
            break;
        case 2:
            #if NEXUS_NUM_COMPONENT_OUTPUTS
            i = rand()%NEXUS_NUM_COMPONENT_OUTPUTS;
            if (platformConfig.outputs.component[i]) {
                NEXUS_ComponentOutputSettings settings;
                NEXUS_ComponentOutput_GetSettings(platformConfig.outputs.component[i], &settings);
                settings.mpaaDecimationEnabled = rand()%2;
                rc = NEXUS_ComponentOutput_SetSettings(platformConfig.outputs.component[i], &settings);
                BDBG_ASSERT(!rc);
            }
            #endif
            break;
        case 3:
            #if NEXUS_NUM_HDMI_OUTPUTS
            i = rand()%NEXUS_NUM_HDMI_OUTPUTS;
            if (platformConfig.outputs.hdmi[i]) {
                NEXUS_HdmiOutputSettings settings;
                NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[i], &settings);
                settings.mpaaDecimationEnabled = rand()%2;
                rc = NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[i], &settings);
                BDBG_ASSERT(!rc);
            }
            #endif
            break;
        }
    }

    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}

static int open_test(void)
{
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayHandle display;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned i;
    void *handle0;
    void *handle1;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.openOutputs = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPOSITE_OUTPUTS
    for (i=0;i<10;i++) {
        NEXUS_CompositeOutputSettings settings;

        NEXUS_CompositeOutput_GetDefaultSettings(&settings);
        handle0 = NEXUS_CompositeOutput_Open(0, &settings);
        BDBG_ASSERT(handle0);

        printf("expected failure next:\n");
        handle1 = NEXUS_CompositeOutput_Open(0, &settings);
        BDBG_ASSERT(!handle1);
        printf("expected failure done.\n");

        NEXUS_CompositeOutput_GetSettings((NEXUS_CompositeOutputHandle)handle0, &settings);
        settings.dac = NEXUS_VideoDac_e0;
        rc = NEXUS_CompositeOutput_SetSettings((NEXUS_CompositeOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector((NEXUS_CompositeOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_CompositeOutput_GetSettings((NEXUS_CompositeOutputHandle)handle0, &settings);
        rc = NEXUS_CompositeOutput_SetSettings((NEXUS_CompositeOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_RemoveOutput(display, NEXUS_CompositeOutput_GetConnector((NEXUS_CompositeOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_CompositeOutput_Close((NEXUS_CompositeOutputHandle)handle0);
    }
#endif

#if NEXUS_NUM_SVIDEO_OUTPUTS
    for (i=0;i<10;i++) {
        NEXUS_SvideoOutputSettings settings;

        NEXUS_SvideoOutput_GetDefaultSettings(&settings);
        handle0 = NEXUS_SvideoOutput_Open(0, &settings);
        BDBG_ASSERT(handle0);

        printf("expected failure next:\n");
        handle1 = NEXUS_SvideoOutput_Open(0, &settings);
        BDBG_ASSERT(!handle1);
        printf("expected failure done.\n");

        NEXUS_SvideoOutput_GetSettings((NEXUS_SvideoOutputHandle)handle0, &settings);
        settings.dacY = NEXUS_VideoDac_e0;
        settings.dacC = NEXUS_VideoDac_e1;
        rc = NEXUS_SvideoOutput_SetSettings((NEXUS_SvideoOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector((NEXUS_SvideoOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_SvideoOutput_GetSettings((NEXUS_SvideoOutputHandle)handle0, &settings);
        rc = NEXUS_SvideoOutput_SetSettings((NEXUS_SvideoOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_RemoveOutput(display, NEXUS_SvideoOutput_GetConnector((NEXUS_SvideoOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_SvideoOutput_Close((NEXUS_SvideoOutputHandle)handle0);
    }
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
    for (i=0;i<10;i++) {
        NEXUS_ComponentOutputSettings settings;

        NEXUS_ComponentOutput_GetDefaultSettings(&settings);
        handle0 = NEXUS_ComponentOutput_Open(0, &settings);
        BDBG_ASSERT(handle0);

        printf("expected failure next:\n");
        handle1 = NEXUS_ComponentOutput_Open(0, &settings);
        BDBG_ASSERT(!handle1);
        printf("expected failure done.\n");

        NEXUS_ComponentOutput_GetSettings((NEXUS_ComponentOutputHandle)handle0, &settings);
        settings.dacs.YPrPb.dacY = NEXUS_VideoDac_e0;
        settings.dacs.YPrPb.dacPr = NEXUS_VideoDac_e1;
        settings.dacs.YPrPb.dacPb = NEXUS_VideoDac_e2;
        rc = NEXUS_ComponentOutput_SetSettings((NEXUS_ComponentOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector((NEXUS_ComponentOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_ComponentOutput_GetSettings((NEXUS_ComponentOutputHandle)handle0, &settings);
        rc = NEXUS_ComponentOutput_SetSettings((NEXUS_ComponentOutputHandle)handle0, &settings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Display_RemoveOutput(display, NEXUS_ComponentOutput_GetConnector((NEXUS_ComponentOutputHandle)handle0));
        BDBG_ASSERT(!rc);

        NEXUS_ComponentOutput_Close((NEXUS_ComponentOutputHandle)handle0);
    }
#endif

    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
    return 0;
}

int main(void)
{
    srand(time(NULL));
    open_test();
    connect_test();
    printf("success\n");
    return 0;
}
