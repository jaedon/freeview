/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: save_teletext.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 3/29/12 3:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/save_teletext.c $
 * 
 * 7   3/29/12 3:39p erickson
 * SW7435-77: set allowVps
 * 
 * 6   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 5   8/14/09 10:38a maivu
 * PR 56612: replace -pal option with -palb, -palg (checking in for
 *  darnstein)
 * 
 * 4   8/6/09 11:01a maivu
 * PR 56612: Checking in for DavidA. save_teletext_rf command line usage
 *  has changed. Its second argument must now be an RF frequency, in Hz.
 *  In the previous version, it
 * was a channel number.
 * 
 * 3   7/16/09 4:15p maivu
 * PR 56612: Add option to not capture to text file
 * 
******************************************************************************/
/* Tune an analog channel, route to VDEC, capture teletext, dump it to a 
 * file. */

/* Tuning */
const char* dataFilename = "teletext.txt";

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_input.h"
#include "nexus_video_input_vbi.h"
#include "nexus_analog_video_decoder.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define TERRESTRIAL 0
#define CABLE_STD 1
#define CABLE_IRC 2
#define CABLE_HRC 3

static FILE* fd = NULL;
static bool s_bRecord = false;

void tt_data_ready(void *context, int param)
{
    unsigned num;
    unsigned iLine;
    unsigned iChar;
    NEXUS_Error rc;
    NEXUS_TeletextLine ttLine[20];
    NEXUS_VideoInput videoInput = context;

    BSTD_UNUSED(param);

    if (s_bRecord) printf("tt ");

    rc = NEXUS_VideoInput_ReadTeletext(videoInput, ttLine, 20, &num);
    BDBG_ASSERT(!rc);
    while (num > 0)
    {
        if (s_bRecord)
        {
            for (iLine = 0 ; iLine < num ; ++iLine)
            {
                uint8_t framingCode =  ttLine[iLine].framingCode;
                if (framingCode != 0xFF)
                {
                    printf (". ");
                    if (fd)
                    {
                        fprintf (fd, "%c%02d: %02x:", 
                            (ttLine[iLine].topField ? 'T' : 'B'),
                            ttLine[iLine].lineNumber, framingCode);
                        for (iChar = 0 ; iChar < 42 ; ++iChar)
                        {
                            fprintf (fd, "%02x", ttLine[iLine].data[iChar]);
                        }
                        fprintf (fd, "\n");
                    }
                }
            }
            printf ("\n");
        }
    
        rc = NEXUS_VideoInput_ReadTeletext(videoInput, ttLine, 20, &num);
        BDBG_ASSERT(!rc);
    }
    if (s_bRecord) fflush(stdout);
}

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_VideoInput videoInput;
    NEXUS_FrontendHandle frontend;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_VideoInputVbiSettings videoInputVbiSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAnalogSettings frontendAnalogSettings;
    NEXUS_VideoFormat videoFormat = NEXUS_VideoFormat_eNtsc;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    int i;
    NEXUS_Error rc;
    struct stat buf;
    unsigned channel = 5;
    int curarg = 1;

    if (argc > curarg && !strcmp(argv[curarg], "-palg")) {
        videoFormat = NEXUS_VideoFormat_ePalG;
        curarg++;
    }
    else if (argc > curarg && !strcmp(argv[curarg], "-palb")) {
        videoFormat = NEXUS_VideoFormat_ePalB;
        curarg++;
    }
    if (argc > curarg) {
        channel = atoi(argv[curarg]);
    }

    /* Refuse to overwrite existing dump file */
    if (stat (dataFilename, &buf) == 0)
    {
        fprintf (stderr, "NOTE: file %s exists, will NOT be modified\n", 
            dataFilename);
    }
    else
    {
        /* Prepare for file dump */
        if ((fd = fopen (dataFilename, "w")) == NULL)
        {
            fprintf (stderr, "ERROR: could not open file %s.\n", 
                dataFilename);
            exit (1);
        }
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    platformSettings.displayModuleSettings.vbi.allowVps = true;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support analog? */
            if ( capabilities.analog && capabilities.ifd )
            {
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find analog-capable frontend\n");
        return 0;
    }

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = videoFormat;
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
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
#endif

    /* Tune Frontend */
    NEXUS_Frontend_GetDefaultAnalogSettings(&frontendAnalogSettings);
    frontendAnalogSettings.frequency = channel;
    frontendAnalogSettings.terrestrial = true;
    frontendAnalogSettings.ifdSettings.videoFormat = videoFormat;
    if ((videoFormat == NEXUS_VideoFormat_ePalG) ||
        (videoFormat == NEXUS_VideoFormat_ePalB)    )
    {
        frontendAnalogSettings.ifdSettings.audioMode =
            NEXUS_IfdAudioMode_eNicam;
    }
    printf ("\n*******************************\n");
    printf ("*                             *\n");
    printf ("* Will use frequency %d *\n", frontendAnalogSettings.frequency);
    printf ("* Will use IFD style %d *\n", frontendAnalogSettings.ifdSettings.videoFormat);
    printf ("*                             *\n");
    printf ("*******************************\n\n");
    rc = NEXUS_Frontend_TuneAnalog(frontend, &frontendAnalogSettings);
    BDBG_ASSERT(!rc);

    /* bring up VDEC */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    rc = NEXUS_VideoWindow_AddInput(window, videoInput);
    BDBG_ASSERT(!rc);

    /* Switch between RF input and CVBS input */
    if (strstr (argv[0], "cvbs") == NULL)
    {
        printf ("RF processing\n");
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput (
            NEXUS_Frontend_GetAnalogVideoConnector(frontend), 
            &analogVideoDecoderSettings);
    }
    else
    {
        printf ("CVBS processing\n");
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput (
            NEXUS_CompositeInput_GetConnector (
                platformConfig.inputs.composite[0]),
            &analogVideoDecoderSettings);
    }
    analogVideoDecoderSettings.autoDetectionMode.autoMode = false;
    analogVideoDecoderSettings.autoDetectionMode.manualFormat = videoFormat;
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    displayVbiSettings.vbiSource = videoInput;
    if (videoFormat == NEXUS_VideoFormat_eNtsc) {
        displayVbiSettings.closedCaptionRouting = true;
        displayVbiSettings.closedCaptionEnabled = true;
        displayVbiSettings.cgmsRouting = true;
        displayVbiSettings.cgmsEnabled = true;
    }
    else {
        displayVbiSettings.teletextRouting = true;
        displayVbiSettings.teletextEnabled = true;
        displayVbiSettings.vpsRouting = true;
        displayVbiSettings.vpsEnabled = true;
    }
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
    videoInputVbiSettings.teletextEnabled = true;
    videoInputVbiSettings.teletextDataReady.callback = tt_data_ready;
    videoInputVbiSettings.teletextDataReady.context = videoInput;
    videoInputVbiSettings.teletextBufferSize = 4096;

    rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
    BDBG_ASSERT(!rc);

    printf ("\n************************************************\n");
    printf ("*                                              *\n");
    printf ("* Hit carriage return to begin data collection *\n");
    printf ("*                                              *\n");
    printf ("************************************************\n");
    getchar();
    s_bRecord = true;

    while (1) {

        /* no data */
        BKNI_Sleep(100);
    }

    return 0;
}

