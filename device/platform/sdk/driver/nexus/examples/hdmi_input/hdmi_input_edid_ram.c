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
* $brcm_Workfile: hdmi_input_edid_ram.c $
* $brcm_Revision: 15 $
* $brcm_Date: 3/21/12 4:46p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/hdmi_input/hdmi_input_edid_ram.c $
* 
* 15   3/21/12 4:46p erickson
* SW7425-2677: add -cmp_crc option, remove unused code, add audiocapture
* 
* 14   8/17/11 11:33a jgarrett
* SWDTV-8271: Adding 7425
* 
* 13   8/16/11 4:10p jgarrett
* SWDTV-8271: Adding EDID RAM for 35233/35126
* 
* 12   5/3/11 1:34p rgreen
* SWDTV-6184: Add command line support to disable HDMI Rx FE Equalization
* 
* 11   2/16/11 10:17a rgreen
* SW35125-14,SW7422-10: Fix 742x compilation
* 
* 10   1/27/11 11:12a rgreen
* SW35125-14: Add support for 35125
* 
* 9   1/19/11 10:23a rgreen
* SW35230-2782: Add support to enable/disable LVDS link outputs
* 
* 8   11/29/10 3:37p erickson
* SW7422-10: allow all examples to build on settops
* 
* 7   11/17/10 2:04p rgreen
* SW7422-10: Enable compilation of hdmi_input_edid_ram sample app on 7422
*  platform
* 
* 6   11/5/10 2:21p rgreen
* SW35230-2116: Call NEXUS_VideoInput_Shutdown prior to closing display
* 
* 5   11/5/10 11:16a rgreen
* SW35230-2116: Merge changes
* 
* SW35230-517/5   11/5/10 10:26a rgreen
* SW35230-2116: Add code to test release of hdmiInput resources, test on
*  50 and 60Hz platforms
* 
* SW35230-517/4   9/23/10 11:07a rgreen
* SW35230-517: Update sample app with command line option (-sixty) to
*  switch to 60Hz vs 120Hz.  Add information on AvMute
* 
* SW35230-517/3   7/13/10 1:55p adtran
* SW35230-517: added new EDID for AC3 support
* 
* SW35230-517/2   7/2/10 6:25p rgreen
* SW35230-517, SW35230-502: force display output to 1080p for EDID ram
*  example
* 
* SW35230-517/2   7/2/10 6:17p rgreen
* SW35230-517,SW35230-502: override panel output format for for edid ram
*  example to 1080p 120
* 
* SW35230-517/1   6/30/10 11:22a adtran
* SW35230-517: modify application for 35230
* 
* 2   7/16/10 12:09p adtran
* SW35230-517: merged changes from 517 branch
* 
* 4   11/2/10 4:54p rgreen
* SW7425-21,SW7422-69: Fix compilation for non DTV platforms
* 
* 3   9/23/10 2:36p erickson
* SW35230-517: merge
* 
* SW35230-517/4   9/23/10 11:07a rgreen
* SW35230-517: Update sample app with command line option (-sixty) to
*  switch to 60Hz vs 120Hz.  Add information on AvMute
* 
* SW35230-517/3   7/13/10 1:55p adtran
* SW35230-517: added new EDID for AC3 support
* 
* SW35230-517/2   7/2/10 6:25p rgreen
* SW35230-517, SW35230-502: force display output to 1080p for EDID ram
*  example
* 
* SW35230-517/2   7/2/10 6:17p rgreen
* SW35230-517,SW35230-502: override panel output format for for edid ram
*  example to 1080p 120
* 
* SW35230-517/1   6/30/10 11:22a adtran
* SW35230-517: modify application for 35230
* 
* 2   7/16/10 12:09p adtran
* SW35230-517: merged changes from 517 branch
* 
* 1   4/9/09 3:28p rgreen
* PR52879: Add example app using the HDMI on-chip EDID RAM
* 
* 15   2/5/09 9:30a erickson
* PR51841: remove examples_lib code
*
* 14   10/29/08 12:16p rgreen
* PR36814: Report only changes in the HdmiInput status
*
* 13   10/22/08 7:30p jgarrett
* PR 47360: Adding compressed SPDIF passthrough for HDMI/SPDIF inputs
*
* 12   10/8/08 2:33a rgreen
* PR46176:Fix example to remove use of hdmiInputFrontend
*
* 11   7/18/08 11:31a jgarrett
* PR 44953: Removing -Wstrict-prototypes warnings
*
* 10   6/19/08 2:36p vsilyaev
* PR 40921: There is no component output on the 93556 platform
*
* 9   6/19/08 10:46a erickson
* PR42678: switch to output_type=panel|component
*
* 8   6/13/08 9:53a erickson
* PR39453: added NEXUS_HdmiInputFrontendHandle for 3548/3556
*
* 7   5/21/08 10:33a erickson
* PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
*
* 6   5/14/08 1:50p erickson
* PR39453: 3548 modes
*
* 5   4/1/08 11:39a erickson
* PR38442: use is_panel_output() for consistent DVO output selection
*
* 4   3/26/08 10:51a erickson
* PR40742: added more status info
*
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
*
* 2   1/23/08 12:39p erickson
* PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/11   12/11/07 4:13p jgarrett
* PR 38107: Removing non-portable strings.h
*
* Nexus_Devel/10   11/30/07 3:57p erickson
* PR34925: rename stc_channel
*
* Nexus_Devel/9   11/30/07 2:14p jgarrett
* PR 27938: Adding DTV specifics
*
* Nexus_Devel/8   11/30/07 12:22p jgarrett
* PR 37778: Using kylin variable name for panel vs component
*
* Nexus_Devel/7   11/29/07 5:58p jgarrett
* PR 37778: Standardizing DTV apps
*
* Nexus_Devel/6   11/29/07 10:32a jgarrett
* PR 37471: Adding audio, refactoring to use platform config
*
* Nexus_Devel/5   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
*
* Nexus_Devel/4   11/16/07 3:32p erickson
* PR36814: added NEXUS_HdmiInput_SetHdcpKeySet call
*
* Nexus_Devel/3   11/15/07 9:31a erickson
* PR34662: added #include nexus_video_window. removed unnecessary
* windowSettings.visible = true.
*
* Nexus_Devel/2   11/9/07 3:16p erickson
* PR36814: added config of timebase
*
* Nexus_Devel/1   11/8/07 2:39p erickson
* PR36814: add hdmi_input
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#include "nexus_audio_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_output.h"
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
#include "nexus_audio_input_capture.h"
static NEXUS_AudioInputCaptureHandle inputCapture;
static NEXUS_AudioInputCaptureStartSettings inputCaptureStartSettings;
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if NEXUS_HAS_HDMI_INPUT

#if 1
/* Sample Panasonic EDID for 35230 
        Audio format code: Linear PCM (e.g., IEC60958) 
        Maximum number of channels: 2 
        Sampling frequency (kHz): 32 44.1 48 
        Number of bits per sample: 16 bits 
*/
static uint8_t SampleEDID[] = 
{
    0x00,   0xFF,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    0x00, 0x34, 0xA9,   0xAF, 0xA0, 0x01, 0x01, 0x01, 0x01,  
    0x00,   0x14,  0x01, 0x03, 0x80, 0x00, 0x00,    0x78, 0x0A, 0xDA,   0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29, 
    0x17,   0x49,  0x4B, 0x00, 0x00, 0x00, 0x01,    0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  
    0x01,   0x01,  0x01, 0x01, 0x01, 0x01, 0x02,    0x3A, 0x80, 0x18,   0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 
    0x45,   0x00,  0xBA, 0x88, 0x21, 0x00, 0x00,    0x1E, 0x01, 0x1D,   0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,  
    0x58,   0x2C,  0x25, 0x00, 0xBA, 0x88, 0x21,    0x00, 0x00, 0x9E,   0x00, 0x00, 0x00, 0xFC, 0x00, 0x50,  
    0x61,   0x6E,  0x61, 0x73, 0x6F, 0x6E, 0x69,    0x63, 0x54, 0x56,   0x30, 0x0A, 0x00, 0x00, 0x00, 0xFD,  
    0x00,   0x17,  0x3D, 0x0F, 0x44, 0x0F, 0x00,    0x0A, 0x20, 0x20,   0x20, 0x20, 0x20, 0x20, 0x01, 0xC7,  
    
    0x02,   0x03,  0x36, 0x71, 0x7F, 0x03, 0x0C,    0x00, 0x40, 0x00,   0xB8, 0x2D, 0x2F, 0x00, 0x00, 0x00,  
    0x00,   0x00,  0x00, 0x00, 0x00, 0x00, 0x00,    0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
    0x00,   0x00,  0x00, 0x00, 0xE3, 0x05, 0x1F,    0x01, 0x49, 0x90,   0x05, 0x20, 0x04, 0x03, 0x02, 0x07,  
    0x06,   0x01,  0x23, 0x09, 0x07, 0x01, 0x01,    0x1D, 0x00, 0x72,   0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,  
    0x55,   0x00,  0xBA, 0x88, 0x21, 0x00, 0x00,    0x1E, 0x8C, 0x0A,   0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,  
    0x10,   0x3E,  0x96, 0x00, 0xBA, 0x88, 0x21,    0x00, 0x00, 0x18,   0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0,  
    0x2D,   0x10,  0x10, 0x3E, 0x96, 0x00, 0x0B,    0x88, 0x21, 0x00,   0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14,  
    0x51,   0xF0,  0x16, 0x00, 0x26, 0x7C, 0x43,    0x00, 0xBA, 0x88,   0x21, 0x00, 0x00, 0x98, 0x00, 0xDC
};
#else
/* 
    Audio format code: AC-3
    Maximum number of channels: 6
    Sampling frequency (kHz): 32 44.1 48 88.2 96 176.4 192 
    Maximum bit rate: 2040 kbps
*/
static uint8_t SampleEDID[] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x34, 0xA9, 0xAF, 0xA0, 0x01, 0x01, 0x01, 0x01, 
    0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29, 
    0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 
    0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 
    0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x50, 
    0x61, 0x6E, 0x61, 0x73, 0x6F, 0x6E, 0x69, 0x63, 0x54, 0x56, 0x30, 0x0A, 0x00, 0x00, 0x00, 0xFD, 
    0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xC7,

    0x02, 0x03, 0x36, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07, 
    0x06, 0x01, 0x23, 0x15, 0x7F, 0xFF, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,    
    0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 
    0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 
    0x51, 0xF0, 0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x98, 0x00, 0x5A
};
#endif

void source_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("source changed\n");
}

void avmute_changed(void *context, int param)
{
    NEXUS_HdmiInputHandle hdmiInput ;
    NEXUS_HdmiInputStatus hdmiInputStatus ;
    BSTD_UNUSED(param);

    hdmiInput = (NEXUS_HdmiInputHandle) context ;
    NEXUS_HdmiInput_GetStatus(hdmiInput, &hdmiInputStatus) ;

    if (!hdmiInputStatus.validHdmiStatus)
    {
        printf("avmute_changed callback: Unable to get hdmiInput status\n") ;
    }
    else
    {
        printf("avmute_changed callback: %s\n", 
            hdmiInputStatus.avMute ? "Set_AvMute" : "Clear_AvMute") ;
    }
}

static void print_cmp_crc(NEXUS_DisplayHandle display)
{
    /* loop forever printing CMP CRC's */
    for (;;) {
        NEXUS_DisplayCrcData data[16];
        unsigned num, i;
        int rc;

        rc = NEXUS_Display_GetCrcData(display, data, 16, &num);
        BDBG_ASSERT(!rc);
        if (!num) {
            BKNI_Sleep(10);
            continue;
        }

        for (i=0;i<num;i++) {
            printf("CMP CRC %x %x %x\n", data[i].cmp.luma, data[i].cmp.cb, data[i].cmp.cr);
        }
    }
}

int main(int argc, char **argv)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;

    NEXUS_HdmiOutputHandle hdmiOutput;
    NEXUS_HdmiOutputStatus hdmiStatus;

    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings ;
    NEXUS_AudioInput connector;
    bool cmp_crc = false;
    unsigned hdmiInputIndex = 0;
    unsigned loopCount = 0 ;
    bool equalization = true;
    int curarg = 1;
    NEXUS_Error rc;


    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-port") && curarg+1<argc) {
            hdmiInputIndex = atoi(argv[++curarg]) ;
        }
        else if (!strcmp(argv[curarg], "-no_equalization")) {
            equalization = false;
        }
        else if (!strcmp(argv[curarg], "-loop") && curarg+1<argc) {
            loopCount = atoi(argv[++curarg]) ;
        }
        else if (!strcmp("-cmp_crc", argv[curarg])) {
            cmp_crc = true;
        }
        curarg++;
    }

    /* Bring up all modules for a platform in a default configuration for this platform */

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    NEXUS_Platform_Init(&platformSettings); 

    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);

    hdmiOutput = platformConfig.outputs.hdmi[0] ;   

    /* set hpdDisconnected to true if a HDMI switch is in front of the Broadcom HDMI Rx.  
    -- The NEXUS_HdmiInput_ConfigureAfterHotPlug should be called to inform the hw of 
    -- the current state,  the Broadcom SV reference boards have no switch so 
    -- the value should always be false 
    */
    hdmiInputSettings.frontend.hpdDisconnected = false ;
    hdmiInputSettings.frontend.equalizationEnabled = equalization ;
    
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    hdmiInputSettings.sourceChanged.callback = source_changed;
    /* use NEXUS_HdmiInput_OpenWithEdid ()
        if EDID PROM (U1304 and U1305) is NOT installed; 
        reference boards usually have the PROMs installed.
        this example assumes Port1 EDID has been removed 
    */

        hdmiInputSettings.useInternalEdid = true ;
        hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, 
            &SampleEDID[0], (uint16_t) sizeof(SampleEDID));

    if (!hdmiInput) 
    {
        fprintf(stderr, "Can't get hdmi input %d\n", hdmiInputIndex);
        return -1;
    }

    NEXUS_HdmiInput_GetSettings(hdmiInput, &hdmiInputSettings) ;
    hdmiInputSettings.avMuteChanged.callback = avmute_changed;
    hdmiInputSettings.avMuteChanged.context = hdmiInput ;
    NEXUS_HdmiInput_SetSettings(hdmiInput, &hdmiInputSettings) ;

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    displaySettings.background = 0xFF00FF00;
    if (cmp_crc) {
        displaySettings.crcQueueSize = 60; /* This enables CMP CRC capture */
    }
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmiOutput));
    rc = NEXUS_HdmiOutput_GetStatus(hdmiOutput, &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }

#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
    /* add audio support */
    inputCapture = NEXUS_AudioInputCapture_Open(0, NULL);
    NEXUS_AudioInputCapture_GetDefaultStartSettings(&inputCaptureStartSettings);
    inputCaptureStartSettings.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);


    connector = NEXUS_AudioInputCapture_GetConnector(inputCapture);

    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput), connector); 

    NEXUS_AudioInputCapture_Start(inputCapture, &inputCaptureStartSettings);
#endif  

#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));


    if (cmp_crc) {
        print_cmp_crc(display);
    }
    
    curarg = 0;
    while ((unsigned) curarg <= loopCount) {
        NEXUS_HdmiInputStatus status, previous_status ;

        NEXUS_HdmiInput_GetStatus(hdmiInput, &status);

         if (!BKNI_Memcmp(&status, &previous_status, sizeof (NEXUS_HdmiInputStatus)))
         {

           printf(
               "HdmiInput Status\n"
               "  original format: %d\n"
               "  interlaced:      %c\n"
               "  noSignal:        %c\n",
               status.originalFormat,
               status.interlaced?'y':'n',
               status.noSignal?'y':'n');
           previous_status = status ;
         }

           BKNI_Sleep(1000);

        if (loopCount)
            curarg++ ;
    }

    NEXUS_VideoWindow_RemoveInput(window, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display) ;
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
    NEXUS_AudioInputCapture_Stop(inputCapture);
    NEXUS_AudioInputCapture_Close(inputCapture);
#endif

    NEXUS_HdmiInput_Close(hdmiInput) ;  
    NEXUS_Platform_Uninit(); 
    return 0;
}
#else
int main(void) {printf("no hdmi_input support\n");return 0;}
#endif
