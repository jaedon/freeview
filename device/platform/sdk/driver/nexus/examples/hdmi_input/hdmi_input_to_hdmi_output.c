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
* $brcm_Workfile: hdmi_input_to_hdmi_output.c $
* $brcm_Revision: 24 $
* $brcm_Date: 10/3/12 1:23p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/hdmi_input/hdmi_input_to_hdmi_output.c $
* 
* 24   10/3/12 1:23p rgreen
* SW7425-4029: Stop Nexus callbacks for hdmiInput
* 
* 23   9/25/12 10:53a jgarrett
* SW7358-278: Adding mute option
* 
* 22   7/26/12 4:27p rgreen
* SW7425-2989: Update selection of test formats (cng)
* 
* SW7358-278/1   7/19/12 11:59a mpeteri
* add audio mute capability
* 
* 21   5/14/12 11:04a rgreen
* SW7425-3011: Stop HDMI callbacks prior to NEXUS_Display_Close to
*  prevent HDMI callbacks from accessing the freed Display resource
* 
* 20   5/4/12 4:26p jgarrett
* SW7425-2075: Waiting up to 1 sec for HDMI Tx connection, matching
*  output format to input format
* 
* 19   2/17/12 3:22p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/2   2/17/12 3:20p jgarrett
* SW7425-2074: Refactored to use AudioInputCapture exclusively.  Fixed
*  crash on shutdown if graphics was not enabled.
* 
* SW7425-2074/1   1/18/12 6:08p jgarrett
* SW7425-2074: Initial prototype for AudioInputCapture
* 
* 18   1/9/12 7:03p rgreen
* SW7425-2115: add macro to enable/disable support of HDCP Keys on
*  hdmiInput (default is disabled); free allocated events; update
*  comments
* 
* 17   8/16/11 5:45p vle
* SW7425-991: add 1080p support to hdmi_input_to_hdmi_output app
* 
* 16   8/5/11 10:38a rgreen
* SW7425-136: Add  exit command line option to support HdmiInput
*  Open/Close stress testing
* 
* 15   6/29/11 5:00p jgarrett
* SW7425-620: Fixing audio handling for stop/start
* 
* 15   6/29/11 4:56p jgarrett
* SW7425-620: Fixing audio handling for stop/start
* 
* 14   6/29/11 12:44p rgreen
* SW7425-621: Update usage of checkpoint for graphics; use checkpoint
*  after graphics setup
* 
* 13   6/7/11 3:53p rgreen
* SW7425-621: Add usage of checkpoint for graphics;
* 
* 13   6/7/11 3:52p rgreen
* SW7425-621: Add usage of checkpoint for graphics
* 
* 12   5/24/11 4:33p rgreen
* SW7425-621: Add simple graphic overlay to hdmiInput picture and
*  retransmit out hdmiOutput ; Add command to get hdmiInput status
* 
* 11   5/6/11 6:39p rgreen
* SW7425-457: Fix shutdown of HDMI apps
* 
* 10   4/18/11 3:53p rgreen
* SW7422-186: Update examples for HDMI Repeater Support
* 
* 9   3/3/11 11:03a rgreen
* SW7422-240: allow app to stop/shutdown modules before exiting
* 
* 8   1/5/11 1:32p rgreen
* SW7422-10: Update EDID loaded to on-chip RAM
* 
* 7   1/4/11 10:36a erickson
* SW7422-10: Allow build if NEXUS_NUM_HDMI_INPUTS is not defined.
* 
* 6   12/31/10 10:55a rgreen
* SW7422-110: Update nexus call to add audio from hdmi
* 
* 5   12/29/10 10:31p rgreen
* SW7422-110: Update to always use on chip EDID RAM for combo chips;
*  Start audio decoder after audio inputs have been configured.
* 
* 4   12/21/10 3:04p rgreen
* SW7422-10: allow all HDMI examples to build on Settop and DTV
*  platforms.
* 
* 3   12/6/10 3:21p rgreen
* SW7422-10: allow all HDMI examples to build on Settop and DTV platforms
* 
* 2   11/29/10 3:37p erickson
* SW7422-10: allow all examples to build on settops
* 
* 1   11/18/10 2:10p rgreen
* SW7422-105: Initial hdmi In to hdmi Out app
* 
***************************************************************************/

#include "nexus_platform.h"
#if NEXUS_NUM_HDMI_OUTPUTS && NEXUS_NUM_HDMI_INPUTS
#include "nexus_core_utils.h"

#include "nexus_display.h"
#include "nexus_graphics2d.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_decoder.h"

#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_hdcp.h"
#include "nexus_hdmi_output_hdcp.h"

#include "nexus_audio_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_output.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(hdmi_input_to_hdmi_output) ;

static    NEXUS_PlatformConfiguration platformConfig;

#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
#include "nexus_audio_input_capture.h"
static NEXUS_AudioInputCaptureHandle inputCapture;
static NEXUS_AudioInputCaptureStartSettings inputCaptureStartSettings;

#define OUTPUT_VIDEO_FORMAT_FOLLOWS_INPUT 1

/* enable the following only if HDCP keys are available for the Input and Output Port */
/* Production source devices connected to hdmiInput will fail if
        1) no keys  have been loaded (see hdmi_input_hdcpkeyloader app for example) or
        2)  if macro HDCP_SUPPORT_ON_HDMI_OUTPUT is 0 
*/
        
#define HDCP_SUPPORT_ON_HDMI_INPUT 0
#define HDCP_SUPPORT_ON_HDMI_OUTPUT 0

#if HDCP_SUPPORT_ON_HDMI_OUTPUT

#endif


static uint8_t SampleEDID[] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
	0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
	0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

	0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
	0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
	0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
	0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};

    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;

    NEXUS_HdmiOutputHandle hdmiOutput;
    NEXUS_HdmiOutputSettings hdmiOutputSettings;


    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_SurfaceCreateSettings createSurfaceSettings;
    NEXUS_SurfaceHandle framebuffer, offscreen;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;

    bool graphicsOverlayEnabled ;
    bool avEnabled = false ;

    BKNI_EventHandle checkpointEvent, spaceAvailableEvent;

void source_changed(void *context, int param)
{
#if OUTPUT_VIDEO_FORMAT_FOLLOWS_INPUT
    NEXUS_HdmiInputHandle hdmiInput ;
    NEXUS_HdmiInputStatus hdmiInputStatus ;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)param;

    hdmiInput = (NEXUS_HdmiInputHandle) context ;
    NEXUS_HdmiInput_GetStatus(hdmiInput, &hdmiInputStatus) ;

    NEXUS_Display_GetSettings(display, &displaySettings);
    if ( displaySettings.format != hdmiInputStatus.originalFormat )
    {
        BDBG_WRN(("Video Format Change - Updating to %u", hdmiInputStatus.originalFormat));

        displaySettings.format = hdmiInputStatus.originalFormat;
        NEXUS_Display_SetSettings(display, &displaySettings);
    }
#else
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
#endif
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

#if HDCP_SUPPORT_ON_HDMI_INPUT

static void hdmiInputHdcpStateChanged(void *context, int param)
{
    NEXUS_HdmiInputHandle hdmiInput ;
    NEXUS_HdmiInputHdcpStatus hdmiInputHdcpStatus ;

    BSTD_UNUSED(param) ;


    hdmiInput = (NEXUS_HdmiInputHandle) context ;
    NEXUS_HdmiInput_HdcpGetStatus(hdmiInput, &hdmiInputHdcpStatus) ;
    
    switch (hdmiInputHdcpStatus.eAuthState)
    {
    case NEXUS_HdmiInputHdcpAuthState_eKeysetInitialization :
    	
        BDBG_WRN(( "Change in HDCP Key Set detected\n")) ;
        switch (hdmiInputHdcpStatus.eKeyStorage)
        {
            case NEXUS_HdmiInputHdcpKeyStorage_eOtpROM :
            BDBG_WRN(( "HDCP Keys stored in OTP ROM\n")) ;
            break ;
        
        case NEXUS_HdmiInputHdcpKeyStorage_eOtpRAM :
            BDBG_WRN(( "HDCP Keys stored in OTP RAM\n")) ;
            break ;
        
        default :
            BDBG_WRN(( "Unknown Key Storage type %d\n", hdmiInputHdcpStatus.eKeyStorage)) ;
        }
        
        BDBG_WRN(( "NOTE: EACH DEVICE REQUIRES A UNIQUE HDCP KEY SET; The same KeySet cannot be used in multiple devices\n\n")) ;
        
        break ;
    

    case NEXUS_HdmiInputHdcpAuthState_eWaitForKeyloading :
        BDBG_WRN(("Upstream HDCP Authentication request ...")) ;
		
        BDBG_WRN(("Aksv: %02x %02x %02x %02x %02x",
            hdmiInputHdcpStatus.aksvValue[4], hdmiInputHdcpStatus.aksvValue[3],
            hdmiInputHdcpStatus.aksvValue[2], hdmiInputHdcpStatus.aksvValue[1],
            hdmiInputHdcpStatus.aksvValue[0])) ;
		
        BDBG_WRN(("An:   %02x %02x %02x %02x %02x %02x %02x %02x ",
            hdmiInputHdcpStatus.anValue[7], hdmiInputHdcpStatus.anValue[6],
            hdmiInputHdcpStatus.anValue[5], hdmiInputHdcpStatus.anValue[4],
            hdmiInputHdcpStatus.anValue[3], hdmiInputHdcpStatus.anValue[2],
            hdmiInputHdcpStatus.anValue[1], hdmiInputHdcpStatus.anValue[0])) ;		

        break ;
    
    case NEXUS_HdmiInputHdcpAuthState_eWaitForDownstreamKsvs :
        BDBG_WRN(("Downstream FIFO Request; Start hdmiOuput Authentication...")) ;
        NEXUS_HdmiOutput_StartHdcpAuthentication(hdmiOutput);
        break ;
    
    default :
    	BDBG_WRN(("Unknown State %d", hdmiInputHdcpStatus.eAuthState )) ;
    }

}

#endif


#if HDCP_SUPPORT_ON_HDMI_OUTPUT
static void hdmiOutputHdcpStateChanged(void *pContext, int param)
{
    bool success = (bool)param;
    NEXUS_HdmiOutputHandle handle = pContext;

    NEXUS_HdmiOutputHdcpStatus hdmiOutputHdcpStatus;
    NEXUS_HdmiHdcpDownStreamInfo downStream  ;
    NEXUS_HdmiHdcpKsv *pKsvs ;

    unsigned returnedDevices ;
    uint8_t i ;

    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdmiOutputHdcpStatus);
	
    if (!success )
    {
        BDBG_WRN(("HDCP Authentication Failed.  Current State %d", 
			hdmiOutputHdcpStatus.hdcpState);
        return ;
    }

    BDBG_WRN(("HDCP Tx Authentication Successful")) ;

    NEXUS_HdmiOuput_HdcpGetDownstreamInfo(handle, &downStream) ;
	

    /* allocate space to hold ksvs for the downstream devices */
    pKsvs = 
        BKNI_Malloc(downStream.devices * NEXUS_HDMI_HDCP_KSV_LENGTH) ;
  
    NEXUS_HdmiOuput_HdcpGetDownstreamKsvs(handle, 
        pKsvs, downStream.devices, &returnedDevices) ;
    
    BDBG_WRN(("hdmiOutput Downstream Levels:  %d  Devices: %d", 
        downStream.depth, downStream.devices)) ;

    /* display the downstream device KSVs */
    for (i = 0 ; i < downStream.devices; i++)
   {
        BDBG_WRN(("Device %02d BKsv: %02X %02X %02X %02X %02X",
            i + 1,
            *(pKsvs->data + i + 4), *(pKsvs->data + i + 3),
            *(pKsvs->data + i + 2), *(pKsvs->data + i + 1),
            *(pKsvs->data + i ))) ;
    }
    NEXUS_HdmiInput_HdcpLoadKsvFifo(hdmiInput, 
		&downStream, pKsvs, downStream.devices) ;
   	
    BKNI_Free(pKsvs) ;
}
#endif

static void disable_audio(NEXUS_HdmiOutputHandle hdmiOutput)
{
    NEXUS_AudioInputCapture_Stop(inputCapture);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput));
}

static void enable_audio(NEXUS_HdmiOutputHandle hdmiOutput)
{
    NEXUS_AudioInput connector;

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput));

    connector = NEXUS_AudioInputCapture_GetConnector(inputCapture);

    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput), connector);

    NEXUS_AudioInputCapture_Start(inputCapture, &inputCaptureStartSettings);
}

static void mute_audio(NEXUS_HdmiOutputHandle hdmiOutput)
{
    NEXUS_AudioOutputSettings outputSettings;

    NEXUS_AudioOutput_GetSettings(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput), &outputSettings);
    outputSettings.muted = !outputSettings.muted;
    NEXUS_AudioOutput_SetSettings(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput), &outputSettings);
}

static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmiOutput = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;

    NEXUS_HdmiOutput_GetStatus(hdmiOutput, &status);
    BDBG_WRN(("hotplug_callback: %s\n", status.connected ? 
		"DEVICE CONNECTED" : "DEVICE REMOVED")) ;


    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( status.connected )
    {
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !status.videoFormatSupported[displaySettings.format] )
        {
            BDBG_WRN(("Current format not supported by attached monitor. Switching to preferred format %d", 
                status.preferredVideoFormat)) ;
            displaySettings.format = status.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
}

void hdmi_input_status(void ) 
{
    NEXUS_HdmiInputStatus hdmiInputStatus ;
    BDBG_Level saveLevel ;
    static const char *textColorSpace[] =
    {
        "RGB ",
        "YCbCr 4:2:2",
        "YCbCr 4:4:4",
        "Max",
    } ;

    BDBG_GetModuleLevel("hdmi_input_to_hdmi_output", &saveLevel) ;
	
    BDBG_SetModuleLevel("hdmi_input_to_hdmi_output", BDBG_eMsg) ;

    NEXUS_HdmiInput_GetStatus(hdmiInput, &hdmiInputStatus) ;
    if (!hdmiInputStatus.validHdmiStatus)
    {
        BDBG_WRN(("Cannot determine input status...")) ;
        return  ;
    }


    BDBG_MSG(("hdmiInput Mode  : %s", hdmiInputStatus.hdmiMode ? "HDMI" : "DVI")) ;
    BDBG_MSG(("hdmiInput Format: %d x %d %c %s", 
		hdmiInputStatus.avWidth, hdmiInputStatus.avHeight, 
		hdmiInputStatus.interlaced ? 'i' : 'p',
		textColorSpace[hdmiInputStatus.colorSpace])) ;
	
	
    BDBG_MSG(("hdmiInput Clock : %d", hdmiInputStatus.lineClock)) ;
    BDBG_MSG(("HDCP Enabled    : %s", hdmiInputStatus.hdcpRiUpdating ? "Yes" : "No")) ;

    /* restore debug level */
    BDBG_SetModuleLevel("hdmi_input_to_hdmi_output", saveLevel) ;

}


void hdmi_output_status(void ) 
{
    NEXUS_HdmiOutputStatus status ;
    BDBG_Level saveLevel ;

    BDBG_GetModuleLevel("hdmi_input_to_hdmi_output", &saveLevel) ;
	
    BDBG_SetModuleLevel("hdmi_input_to_hdmi_output", BDBG_eMsg) ;

    NEXUS_HdmiOutput_GetStatus(hdmiOutput, &status) ;
    if (!status.connected)
    {
        BDBG_WRN(("Cannot determine output status...")) ;
        return  ;
    }

    BDBG_MSG(("Monitor <%s> (Nexus) hdmiOutput Format: %d", 
		status.monitorName, status.videoFormat)) ;

    BDBG_MSG(("Audio Format: %d; Sample Rate: %dHz;  Sample Size: %d Channels: %d", 
		status.audioFormat,
		status.audioSamplingRate, /* in units of Hz */
		status.audioSamplingSize, status.audioChannelCount)) ;

    /* restore debug level */
    BDBG_SetModuleLevel("hdmi_input_to_hdmi_output", saveLevel) ;

}


void graphicsComplete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle) data);
}

void setupGraphics(
	NEXUS_DisplayHandle display, NEXUS_DisplaySettings *displaySettings) 
{

    NEXUS_Error rc;

    NEXUS_VideoFormat_GetInfo(displaySettings->format, &displayFormatInfo);

    /* create actual framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&createSurfaceSettings);	
        /* older platforms cannot support 32bpp pixel format on a large framebuffer due to bandwidth issues. 
           you can either use a 16bpp pixel format, or reduce the framebuffer width */
        createSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
#define FRAMEBUFFER_WIDTH 1440
        createSurfaceSettings.width = FRAMEBUFFER_WIDTH;
        createSurfaceSettings.height = 1080;
        createSurfaceSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&createSurfaceSettings);

    /* create app offscreen buffer */
    NEXUS_Surface_GetDefaultCreateSettings(&createSurfaceSettings);
        createSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        createSurfaceSettings.width = 720;
        createSurfaceSettings.height = 480;
    offscreen = NEXUS_Surface_Create(&createSurfaceSettings);
	
    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&spaceAvailableEvent);
    
    gfx = NEXUS_Graphics2D_Open(0, NULL);
	
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = graphicsComplete;
    gfxSettings.checkpointCallback.context = checkpointEvent ;
    gfxSettings.packetSpaceAvailable.callback = graphicsComplete;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
	

	/* build the offscreen buffer */
	NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
		fillSettings.surface = offscreen;
		fillSettings.color = 0x00000000; /* transparent black */
	rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
	BDBG_ASSERT(!rc);

	NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
		fillSettings.surface = offscreen;
		fillSettings.rect.x = 100;
		fillSettings.rect.y = 100;
		fillSettings.rect.width = 100;
		fillSettings.rect.height = 100;
		fillSettings.color = 0xFFFFFFFF; /* opaque white */
	rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
	BDBG_ASSERT(!rc);

	NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
		fillSettings.surface = framebuffer;
		fillSettings.color = 0x00000000; /* transparent black */
	rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
	BDBG_ASSERT(!rc);
	
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.clip.width = FRAMEBUFFER_WIDTH;
        graphicsSettings.clip.height = displayFormatInfo.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);


    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    /* blit can fail because an internal queue is full. wait for all blits to complete, then continue. */
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) 
    {
        BKNI_WaitForEvent(checkpointEvent, 0xffffffff);
    }	
}


void updateGraphics(void)
{
    NEXUS_Error rc;
    static uint8_t index = 0 ;

#define NUM_TEST_MODES 14
struct {
    bool sbs;
    int zoffset;
} g_testmode[NUM_TEST_MODES] = 
{
    {true, 0},
    {true, -5},
    {true, -10},
    {true, 0},
    {true, 5},
    {true, 10},
    {true, 0},
    {false, 0},
    {false, -5},
    {false, -10},
    {false, 0},
    {false, 5},
    {false, 10},
    {false, 0}
};


    if (index == NUM_TEST_MODES) index = 0;

#if NO_ZNORM
/**
The zoffset == 0 case is included to keep the example code simple.
**/
    if (g_testmode[i].sbs) 
    {
        printf("mode: L/R half\n");
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreen;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = FRAMEBUFFER_WIDTH/2;
        blitSettings.output.rect.height = displayFormatInfo.height;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
    
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreen;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect.x = FRAMEBUFFER_WIDTH/2;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = FRAMEBUFFER_WIDTH/2;
        blitSettings.output.rect.height = displayFormatInfo.height;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
    }
    else 
    {
        printf("mode: O/U half\n");
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreen;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = FRAMEBUFFER_WIDTH;
        blitSettings.output.rect.height = displayFormatInfo.height/2;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
    
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreen;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = displayFormatInfo.height/2;
        blitSettings.output.rect.width = FRAMEBUFFER_WIDTH;
        blitSettings.output.rect.height = displayFormatInfo.height/2;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
    }
#else

#define abs(X) ((X)>0?(X):-(X))

/* This code handles positive, negative and zero zoffset. */
    if (g_testmode[index].sbs) 
    {
        int zoffset = g_testmode[index].zoffset;
        int scaled_zoffset = zoffset * (int)createSurfaceSettings.width / (int)(FRAMEBUFFER_WIDTH/2);
        
        printf("mode: L/R half with %d zoffset\n", zoffset);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = offscreen;
            blitSettings.source.rect.x = scaled_zoffset<0?-scaled_zoffset:0;
            blitSettings.source.rect.y = 0;
            blitSettings.source.rect.width = createSurfaceSettings.width - abs(scaled_zoffset);
            blitSettings.source.rect.height = createSurfaceSettings.height;
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect.x = zoffset>0?zoffset:0;
            blitSettings.output.rect.y = 0;
            blitSettings.output.rect.width = FRAMEBUFFER_WIDTH/2 - abs(zoffset);
            blitSettings.output.rect.height = displayFormatInfo.height;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = offscreen;
            blitSettings.source.rect.x = scaled_zoffset>0?scaled_zoffset:0;
            blitSettings.source.rect.y = 0;
            blitSettings.source.rect.width = createSurfaceSettings.width - abs(scaled_zoffset);
            blitSettings.source.rect.height = createSurfaceSettings.height;
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect.x = FRAMEBUFFER_WIDTH/2 - (zoffset<0?zoffset:0);
            blitSettings.output.rect.y = 0;
            blitSettings.output.rect.width = FRAMEBUFFER_WIDTH/2 - abs(zoffset);
            blitSettings.output.rect.height = displayFormatInfo.height;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        }
    else 
    {
        int zoffset = g_testmode[index].zoffset;
        int scaled_zoffset = zoffset; /* no scaling in horizontal direction */
        
        printf("mode: O/U half with %d zoffset\n", zoffset);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = offscreen;
            blitSettings.source.rect.x = scaled_zoffset<0?-scaled_zoffset:0;
            blitSettings.source.rect.y = 0;
            blitSettings.source.rect.width = createSurfaceSettings.width - abs(scaled_zoffset);
            blitSettings.source.rect.height = createSurfaceSettings.height;
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect.x = zoffset>0?zoffset:0;
            blitSettings.output.rect.y = 0;
            blitSettings.output.rect.width = FRAMEBUFFER_WIDTH - abs(zoffset);
            blitSettings.output.rect.height = displayFormatInfo.height/2;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = offscreen;
            blitSettings.source.rect.x = scaled_zoffset>0?scaled_zoffset:0;
            blitSettings.source.rect.y = 0;
            blitSettings.source.rect.width = createSurfaceSettings.width - abs(scaled_zoffset);
            blitSettings.source.rect.height = createSurfaceSettings.height;
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect.x = zoffset<0?-zoffset:0;
            blitSettings.output.rect.y = displayFormatInfo.height/2;
            blitSettings.output.rect.width = FRAMEBUFFER_WIDTH - abs(zoffset);
            blitSettings.output.rect.height = displayFormatInfo.height/2;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        }
#endif		
        /* blit can fail because an internal queue is full. wait for all blits to complete, then continue. */
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) 
        {
            BKNI_WaitForEvent(checkpointEvent, 0xffffffff);
        }

        index++ ;

    }

#endif


void shutdownGraphics(void) 
{
    if (!graphicsOverlayEnabled)
        return ;
	
	NEXUS_Graphics2D_Close(gfx) ;

    if (spaceAvailableEvent)
        BKNI_DestroyEvent(spaceAvailableEvent) ;
	
	if (checkpointEvent)
        BKNI_DestroyEvent(checkpointEvent) ;

    NEXUS_Surface_Destroy(offscreen) ;
    NEXUS_Surface_Destroy(framebuffer) ;
}



int main(int argc, char **argv)
{
#if NEXUS_HAS_HDMI_INPUT
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformSettings platformSettings ;

    NEXUS_HdmiOutputStatus hdmiOutputStatus;
	
    NEXUS_Error rc;
    bool exitTest = 0 ;
	
    unsigned i, j, menuIndex;    
    unsigned timeout=4;
    unsigned hdmiInputIndex = 0;
    int curarg ;
    int waitTime ;

	typedef struct {
        int menuEntry;
        NEXUS_VideoFormat format;
        char* name;
    } formatTable;

    static formatTable testFormats[] = {

        { 0, NEXUS_VideoFormat_e1080i, "1080i" },
        { 0, NEXUS_VideoFormat_e720p, "720p" },
        { 0, NEXUS_VideoFormat_e480p, "480p" },
        { 0, NEXUS_VideoFormat_eNtsc, "480i (NTSC)" },
        { 0, NEXUS_VideoFormat_e720p50hz, "720p 50Hz" },
        { 0, NEXUS_VideoFormat_e1080p24hz, "1080p 24Hz" },
        { 0, NEXUS_VideoFormat_e1080i50hz, "1080i 50Hz" },
        { 0, NEXUS_VideoFormat_e1080p50hz, "1080p 50Hz" },
        { 0, NEXUS_VideoFormat_e1080p60hz, "1080p 60Hz" },
        { 0, NEXUS_VideoFormat_ePal, "576i (PAL)" },
        { 0, NEXUS_VideoFormat_e576p, "576p" },
        
        /* END of ARRAY */
        { 0, 0, "" }
    };

	
    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-exit")) {
            exitTest = 1 ;
        }
        curarg++;
    }
	
    
    /* Bring up all modules for a platform in a default configuration for this platform */

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
        platformSettings.openFrontend = false;   
    NEXUS_Platform_Init(&platformSettings); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

    hdmiOutput = platformConfig.outputs.hdmi[0];


    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);


    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
        hdmiInputSettings.timebase = NEXUS_Timebase_e0;
		
        /* set hpdDisconnected to true if a HDMI switch is in front of the Broadcom HDMI Rx.  
             -- The NEXUS_HdmiInput_ConfigureAfterHotPlug should be called to inform the hw of 
             -- the current state,  the Broadcom SV reference boards have no switch so 
             -- the value should always be false 
             */
       hdmiInputSettings.frontend.hpdDisconnected = false ;
        
        /* use NEXUS_HdmiInput_OpenWithEdid ()
              if EDID PROM (U1304 and U1305) is NOT installed; 
                reference boards usually have the PROMs installed.
                this example assumes Port1 EDID has been removed 
            */
    
        /* all HDMI Tx/Rx combo chips have EDID RAM */
        hdmiInputSettings.useInternalEdid = true ;


    do
    {
        /* check for connected downstream device */
        rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiOutputStatus);
        if (rc) BERR_TRACE(rc);
        if ( !hdmiOutputStatus.connected )
        {
            BDBG_WRN(("Waiting for HDMI Tx Device"));
            BKNI_Sleep(250);
        }
        else
        {
            break;
        }
    } while ( timeout-- > 0 );
    
    if (hdmiOutputStatus.connected)
    {
        NEXUS_HdmiOutputBasicEdidData hdmiOutputBasicEdidData;
        uint8_t *attachedRxEdid;
        NEXUS_HdmiOutputEdidBlock edidBlock;
        unsigned i, j;

        
        /* Get EDID of attached receiver*/
        NEXUS_HdmiOutput_GetBasicEdidData(platformConfig.outputs.hdmi[0], &hdmiOutputBasicEdidData);

        /* allocate space to hold the EDID blocks */
        attachedRxEdid = BKNI_Malloc((hdmiOutputBasicEdidData.extensions+1)* sizeof(edidBlock.data)) ;
        for (i=0; i<= hdmiOutputBasicEdidData.extensions; i++)
        {
            rc = NEXUS_HdmiOutput_GetEdidBlock(platformConfig.outputs.hdmi[0], i, &edidBlock);
            if (rc)
                BDBG_ERR(("Error retrieve EDID from attached receiver"));

            for (j=0; j < sizeof(edidBlock.data); j++) {
                attachedRxEdid[i*sizeof(edidBlock.data)+j] = edidBlock.data[j];
            }
        }
        
        /* Load hdmiInput EDID RAM with the EDID from the Rx attached to the hdmiOutput (Tx) . */
        hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, 
            attachedRxEdid, (uint16_t) (sizeof(NEXUS_HdmiOutputEdidBlock) * (hdmiOutputBasicEdidData.extensions+1)));

        /* release memory resources */
        BKNI_Free(attachedRxEdid);
    }
    else
    {
        /* Load EDID RAM with the internal declared EDID. */
        hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, 
            &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
    } 

		
    if (!hdmiInput) 
    {
        fprintf(stderr, "Can't get hdmi input\n");
        return -1;
    }

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);

    NEXUS_HdmiInput_GetSettings(hdmiInput, &hdmiInputSettings) ;
    hdmiInputSettings.avMuteChanged.callback = avmute_changed;
    hdmiInputSettings.avMuteChanged.context = hdmiInput ;
    hdmiInputSettings.sourceChanged.callback = source_changed;
    hdmiInputSettings.sourceChanged.context = hdmiInput ;
    hdmiInputSettings.sourceChanged.param = (int)display;
    NEXUS_HdmiInput_SetSettings(hdmiInput, &hdmiInputSettings) ;

    {
        NEXUS_HdmiInputHdcpSettings hdmiInputHdcpSettings ;
#if HDCP_SUPPORT_ON_HDMI_INPUT
        NEXUS_HdmiInputHdcpStatus hdmiInputHdcpStatus ;
#endif

        NEXUS_HdmiInput_HdcpGetDefaultSettings(hdmiInput, &hdmiInputHdcpSettings) ;
            /* 
            -- for this sample app override the HDCP repeater functionality 
            -- so hdmi in to hdmi out path can be tested 
            --
            -- in a normal application, overriding the HDCP Repeater is a violation of the HDCP license
            -- See hdmi_repeater_passthrough app for full HDCP support.
            */    
            hdmiInputHdcpSettings.repeater = false  ;

#if HDCP_SUPPORT_ON_HDMI_INPUT
            hdmiInputHdcpSettings.hdcpRxChanged.callback = hdmiInputHdcpStateChanged ;
            hdmiInputHdcpSettings.hdcpRxChanged.context = hdmiInput ;
            hdmiInputHdcpSettings.hdcpRxChanged.param = 0 ;
 #endif

        NEXUS_HdmiInput_HdcpSetSettings(hdmiInput, &hdmiInputHdcpSettings) ;

#if HDCP_SUPPORT_ON_HDMI_INPUT
        /* get status of HDCP Keys stored */
        NEXUS_HdmiInput_HdcpGetStatus(hdmiInput, &hdmiInputHdcpStatus) ;
        if (hdmiInputHdcpStatus.eOtpState != NEXUS_HdmiInputHdcpKeySetOtpState_eCrcMatch)
        {
            BDBG_ERR(("HDCP Keys are not available")) ;
            goto shutdown ;
        }
#endif		
    }



#if HDCP_SUPPORT_ON_HDMI_OUTPUT
    {
        NEXUS_HdmiOutputHdcpSettings hdmiOutputHdcpSettings;
        
        NEXUS_HdmiOutput_GetHdcpSettings(hdmiOutput, &hdmiOutputHdcpSettings);
            /* If you want to use a custom key, insert that here */
            hdmiOutputHdcpSettings.successCallback.callback = hdmiOutputHdcpStateChanged;
            hdmiOutputHdcpSettings.successCallback.context = hdmiOutput;
            hdmiOutputHdcpSettings.successCallback.param = true;
            
            hdmiOutputHdcpSettings.failureCallback.callback = hdmiOutputHdcpStateChanged;
            hdmiOutputHdcpSettings.failureCallback.context = hdmiOutput;
            hdmiOutputHdcpSettings.failureCallback.param = false;
        NEXUS_HdmiOutput_SetHdcpSettings(hdmiOutput, &hdmiOutputHdcpSettings);
        
        NEXUS_HdmiOutput_SetHdcpRevokedKsvs(hdmiOutput, 
            RevokedKsvs, NumRevokedKsvs) ;
    }	
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmiOutput));

    /* Install hotplug callback -- video only for now */
    NEXUS_HdmiOutput_GetSettings(hdmiOutput, &hdmiOutputSettings);
        hdmiOutputSettings.hotplugCallback.callback = hotplug_callback;
        hdmiOutputSettings.hotplugCallback.context = hdmiOutput;
        hdmiOutputSettings.hotplugCallback.param = (int)display;
    NEXUS_HdmiOutput_SetSettings(hdmiOutput, &hdmiOutputSettings);

    /* add audio support */
    inputCapture = NEXUS_AudioInputCapture_Open(0, NULL);
    NEXUS_AudioInputCapture_GetDefaultStartSettings(&inputCaptureStartSettings);
    inputCaptureStartSettings.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);

    NEXUS_VideoWindow_AddInput(window, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));
    enable_audio(hdmiOutput);

    graphicsOverlayEnabled  = false ;
    avEnabled = true ;


    if (exitTest)
    {
		srand(time(NULL)) ;

		waitTime = rand() % 9 + 1;
		BDBG_WRN(("Randomly wait %d seconds", waitTime)) ;
		BKNI_Sleep(waitTime * 1000) ;
		goto exit_menu ;
    }

    for (i=0;;i++)
    {
        unsigned char chr ;
        unsigned int tmp;

		/* Find Current Format */
        NEXUS_Display_GetSettings(display, &displaySettings);
        for ( j = 0; testFormats[j].format ; j++)
        {
            if ( displaySettings.format == testFormats[j].format ) { break; }
        }

        printf("Main Menu\n");
		
        printf("1) Change Video Format %s%s\n", i ? "-> Currently " : "" ,i ? testFormats[j].name : "" );
        printf("2) Restart Audio\n");
        printf("3) hdmiInput Status\n");
        printf("4) hdmiOutput Status\n");
        printf("5) Enable/Update Graphics\n");
        printf("6) Toggle Audio Mute\n");
		
        printf("0) Exit\n");
        printf("Enter Selection: ");
        scanf("%u", &tmp);
        switch ( tmp )
        {
        case 0:
            goto exit_menu;

        case 1:
			/* get/build a list of formats supported by attached Receiver */
            NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiOutputStatus);

            /* Display available formats */
            menuIndex = 0;
            printf("\n\n 0) Return to Main Menu\n");                

            j = 0 ;
            while ( testFormats[j].format ) 
            {
                /* if format is supported; add to menu */		
                if ( hdmiOutputStatus.videoFormatSupported[testFormats[j].format] )
                {
                    testFormats[j].menuEntry = ++ menuIndex ;
                    printf("%2d) %s\n", menuIndex, testFormats[j].name );
                }
                j++;
            }

            /* Read user input for desired format */
            printf("\nEnter new format-> ");                
            scanf("%u", &tmp);

            if( !tmp )  /* 0 - Exit */
                break;  /* Exit to Main Menu*/

            if ( (uint8_t) tmp > menuIndex)
            { 
                printf("\n'%u' is an invalid choice\n\n", tmp);
                break; 
            }
			
            /* confirm valid format selection */
            for ( j = 0; ; j++) 
            {
                if ( (int)tmp == testFormats[j].menuEntry ) 
                {
                    /* Set Format */
                    NEXUS_Display_GetSettings(display, &displaySettings);
                        displaySettings.format = testFormats[j].format;
                    rc = NEXUS_Display_SetSettings(display, &displaySettings);
                    if (rc)
                    {
                        printf("ERROR changing format\n") ;
                    }

                    break;
                }
            }
            
            break;
            
        case 2:
            disable_audio(hdmiOutput);
            enable_audio(hdmiOutput);
            break;
        case 3:  hdmi_input_status() ;  break ;     
        case 4:  hdmi_output_status() ;  break;

        case 5:
            if (!graphicsOverlayEnabled)
            {
               /* enable graphics overlay */
                setupGraphics(display, &displaySettings) ;
                graphicsOverlayEnabled = true ;
            }
		else
                updateGraphics() ;
            break;
			
        case 6:
            mute_audio(hdmiOutput);
            break;

        default:
           scanf("%c", &chr); 
            break;
        }

    }


exit_menu:
    if (exitTest)
    {
        waitTime = rand() % 2 + 1;
        if (waitTime == 2)
        {
            BDBG_WRN(("\n\n\n\n\nExit without formal NEXUS shutdown (%d)", waitTime)) ;
            return 0 ;
        }
    }
			
#if HDCP_SUPPORT_ON_HDMI_INPUT
shutdown :
#endif

    shutdownGraphics() ;

    if (avEnabled)
    {
        disable_audio(hdmiOutput);
    
        NEXUS_VideoWindow_RemoveInput(window, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));
        NEXUS_VideoWindow_Close(window);
        NEXUS_Display_RemoveOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmiOutput));

		/* stop/remove HDMI callbacks associated with display, 
		so those callbacks do not access display once it is removed */
        NEXUS_StopCallbacks(hdmiOutput);
        NEXUS_StopCallbacks(hdmiInput);

        NEXUS_Display_Close(display) ;
    
        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmiOutput));
        NEXUS_AudioInput_Shutdown(NEXUS_AudioInputCapture_GetConnector(inputCapture));
        NEXUS_AudioInput_Shutdown(NEXUS_HdmiInput_GetAudioConnector(hdmiInput));
        NEXUS_AudioInputCapture_Close(inputCapture);
    }
	
    NEXUS_HdmiInput_Close(hdmiInput) ;  

    NEXUS_Platform_Uninit(); 


#else
    BSTD_UNUSED(argc);
    printf("%s not supported on this platform", argv[0]) ;
#endif

    return 0;
}
#else
#include "bstd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NEXUS_NUM_HDMI_INPUTS
#define NEXUS_NUM_HDMI_INPUTS 0
#endif

#ifndef NEXUS_NUM_HDMI_OUTPUTS
#define NEXUS_NUM_HDMI_OUTPUTS 0
#endif

int main(int argc, char **argv)
{
    BSTD_UNUSED(argc);
	printf("%d Platform has %d HDMI Inputs and %d HDMI Outputs; App requires one of each.\n",NEXUS_PLATFORM,
           NEXUS_NUM_HDMI_INPUTS, NEXUS_NUM_HDMI_OUTPUTS);
	
    printf("%s not supported on the %d platform.\n", argv[0], BCHP_CHIP) ;
    return 0 ;
}
#endif

