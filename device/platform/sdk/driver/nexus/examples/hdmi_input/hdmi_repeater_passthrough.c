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
* $brcm_Workfile: hdmi_repeater_passthrough.c $
* $brcm_Revision: 22 $
* $brcm_Date: 10/3/12 1:23p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/hdmi_input/hdmi_repeater_passthrough.c $
* 
* 22   10/3/12 1:23p rgreen
* SW7425-4029: Stop Nexus callbacks for hdmiInput
* 
* 21   7/5/12 5:21p vle
* SW7425-1988: Fix merge errors
* 
* 20   7/5/12 1:18p vle
* SW7425-1988: Merge to mainline
* 
* SW7425-1988/3   6/29/12 3:44p vle
* SW7425-1988: Add device count and KSV to KSV list if attached device is
*  a repeater. This resolve compliance test 3C-II-01 and 3C-II-06.
* 
* SW7425-1988/2   6/27/12 11:26a vle
* SW7425-1988: Remove HDCP production keys
* 
* SW7425-1988/1   6/26/12 7:08p vle
* SW7425-1988: Update repeater failures from Tx to Rx, resolve 3C-II-06
*  to 3C-II-09 hdcp compliance tests
* 
* 19   6/20/12 5:54p vle
* SW7425-1988: Do not restart HDCP authentication if Hdmi Tx is currently
*  authentication with a downstream repeater. Fix 3B-02 HDCP compliance
*  test failure
* 
* 18   6/4/12 1:21p rgreen
* SW7425-1988: Disable audio when shutting down app; Add error trace for
*  failures; update app to use hdmiOutput vs platformConfig
* 
* 17   6/2/12 2:21p rgreen
* SW7425-1988: Update repeater app to use audio capture vs Audio Decoder.
*  fix app shutdown
* 
* 16   5/14/12 11:05a rgreen
* SW7425-3011: Stop HDMI callbacks prior to NEXUS_Display_Close to
*  prevent HDMI callbacks from accessing the freed Display resource
* 
* 15   5/1/12 3:34p rgreen
* SW7425-1988:  Add -compliance option to allow app to retry downstream
*  authentication when doing compliance testing
* 
* 14   4/12/12 11:36a rgreen
* SW7425-1988:  Add declaration for encrypted Tx HDCP Key Set.  Make Tx
*  Key Set declaration consistent with hdmi_output app. Remove hardware-
*  passhthrough option
* 
* 13   9/16/11 11:04a rgreen
* SW7425-991: use MSG vs WRN for printing menu, diagnostic info
* 
* 12   9/15/11 5:04p rgreen
* SW7425-991: Add menu option to toggle HDMI passthrough settings; Update
*  EDID copying of downstream EDID; Add app force_exit for testing
* 
* 11   8/16/11 6:15p vle
* SW7425-991: add 1080p support to hdmi_repeater_passthrough
* 
* 10   6/29/11 4:56p jgarrett
* SW7425-620: Fixing audio handling for stop/start
* 
* 9   5/20/11 4:11p rgreen
* SWDTV-7092: Update repeater app to display status on demand
* 
* 8   5/6/11 6:39p rgreen
* SW7425-457: Fix shutdown of HDMI apps
* 
* 7   4/20/11 11:46a vle
* SW7425-353: Make sure to free allocated memory
* 
* 6   4/19/11 6:02p vle
* SW7425-353: Add ability to duplicate EDID from attached downstream
*  receiver - Take 2
* 
* 5   4/19/11 11:39a vle
* SW7425-353: Add ability to duplicate EDID from attached downstream
*  receiver
* 
* 4   4/18/11 6:39p rgreen
* SW7422-186: Downgrade WRN to MSG message
* 
* 3   4/18/11 3:53p rgreen
* SW7422-186: Update examples for HDMI Repeater Support
* 
* 2   4/11/11 2:55p rgreen
* SW7422-186: Add HDCP Repeater passthrough support
* 
* 1   3/31/11 1:57p rgreen
* SW7422-186: Add HDCP Repeater passthrough support
* 
***************************************************************************/

#include "nexus_platform.h"
#if NEXUS_NUM_HDMI_OUTPUTS && NEXUS_NUM_HDMI_INPUTS

#include "nexus_pid_channel.h"
#include "nexus_display.h"
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
#include "nexus_core_utils.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

BDBG_MODULE(repeater_passthrough) ;

#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
#include "nexus_audio_input_capture.h"
static NEXUS_AudioInputCaptureHandle inputCapture;
static NEXUS_AudioInputCaptureStartSettings inputCaptureStartSettings;

static bool hdmiCompressedAudio = false ;
static bool hdmiOutputHdcpEnabled = false ;

static NEXUS_PlatformConfiguration platformConfig;


static NEXUS_DisplayHandle display;
static NEXUS_HdmiInputHandle hdmiInput;
static NEXUS_HdmiOutputHandle hdmiOutput;

 

static bool force_exit  = false ;
static bool compliance_test  = false ;

static void initializeHdmiOutputHdcpSettings(void) ;

#define USE_PRODUCTION_KEYS 0

#if USE_PRODUCTION_KEYS

/*****************************/
/* INSERT PRODUCTION KeySet HERE */
/*****************************/

#else


/**************************************/
/* HDCP Specification Test Key Set    */
/*                                    */
/* NOTE: the default declared Test    */
/* KeySet below is from the HDCP Spec */ 
/* and it *IS NOT* compatible with    */
/* production devices                 */
/**************************************/


static NEXUS_HdmiOutputHdcpKsv hdcpTxAksv =
{	{0x14, 0xF7, 0x61, 0x03, 0xB7} };

static NEXUS_HdmiOutputHdcpKey encryptedTxKeySet[NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x691e138f, 0x58a44d00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x0950e658, 0x35821f00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x0d98b9ab, 0x476a8a00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xcac5cb52, 0x1b18f300},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xb4d89668, 0x7f14fb00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x818f4878, 0xc98be000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x412c11c8, 0x64d0a000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x44202428, 0x5a9db300},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6b56adbd, 0xb228b900},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf6e46c4a, 0x7ba49100},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x589d5e20, 0xf8005600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xa03fee06, 0xb77f8c00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x28bc7c9d, 0x8c2dc000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x059f4be5, 0x61125600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xcbc1ca8c, 0xdef07400},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6adbfc0e, 0xf6b83b00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xd72fb216, 0xbb2ba000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x98547846, 0x8e2f4800},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x38472762, 0x25ae6600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf2dd23a3, 0x52493d00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x543a7b76, 0x31d2e200},	
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x2561e6ed, 0x1a584d00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf7227bbf, 0x82603200},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6bce3035, 0x461bf600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6b97d7f0, 0x09043600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf9498d61, 0x05e1a100}, 
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x063405d1, 0x9d8ec900}, 
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x90614294, 0x67c32000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xc34facce, 0x51449600},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x8a8ce104, 0x45903e00},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xfc2d9c57, 0x10002900},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x80b1e569, 0x3b94d700},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x437bdd5b, 0xeac75400},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xba90c787, 0x58fb7400}, 
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xe01d4e36, 0xfa5c9300},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xae119a15, 0x5e070300},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x01fb788a, 0x40d30500},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0xb34da0d7, 0xa5590000},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x409e2c4a, 0x633b3700},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0x412056b4, 0xbb732500}
} ;

 #endif


/* 
from HDCP Spec:
Table 51 gives the format of the HDCP SRM. All values are stored in big endian format.

Specify KSVs here in big endian;
*/
#define NUM_REVOKED_KSVS 3
static uint8_t NumRevokedKsvs = NUM_REVOKED_KSVS ;
static const NEXUS_HdmiOutputHdcpKsv RevokedKsvs[NUM_REVOKED_KSVS] =  
{
    /* MSB ... LSB */
    {{0xa5, 0x1f, 0xb0, 0xc3, 0x72}},
    {{0x65, 0xbf, 0x04, 0x8a, 0x7c}},
    {{0x65, 0x65, 0x1e, 0xd5, 0x64}}
} ;


static uint8_t *pEDID ;
static uint16_t edidSize ;
static uint16_t SampleEDIDSize = 256 ;


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


void source_changed(void *context, int param)
{
    NEXUS_HdmiInputHandle hdmiInput ;

    NEXUS_HdmiInputStatus hdmiInputStatus ;
    NEXUS_HdmiOutputSettings hdmiOutputSettings ;
    NEXUS_DisplaySettings displaySettings ;
    BSTD_UNUSED(param);

    hdmiInput = (NEXUS_HdmiInputHandle) context ;

    NEXUS_HdmiInput_GetStatus(hdmiInput, &hdmiInputStatus) ;
    if (!hdmiInputStatus.validHdmiStatus)
    {
        if (hdmiOutputHdcpEnabled)
        {
            NEXUS_HdmiOutput_DisableHdcpAuthentication(hdmiOutput);				
            hdmiOutputHdcpEnabled = false ;
        }
        return ;
    }
	
    NEXUS_Display_GetSettings(display, &displaySettings);
        displaySettings.format = hdmiInputStatus.originalFormat ;
     NEXUS_Display_SetSettings(display, &displaySettings);

    NEXUS_HdmiOutput_GetSettings(hdmiOutput, &hdmiOutputSettings);
	
	/* debug messages */
	BDBG_MSG(("hdmiInput Colorspace: %d",  hdmiInputStatus.colorSpace)) ;
	BDBG_MSG(("hdmiOutput Colorspace: %d",  hdmiOutputSettings.colorSpace)) ;

	hdmiOutputSettings.autoColorSpace = true ;
	hdmiOutputSettings.colorSpace = hdmiInputStatus.colorSpace ;

	/* debug messages */
	BDBG_MSG(("hdmiInput Colorspace: %d",  hdmiInputStatus.colorSpace)) ;
	BDBG_MSG(("hdmiOutput Colorspace: %d",  hdmiOutputSettings.colorSpace)) ;

	
    NEXUS_HdmiOutput_SetSettings(hdmiOutput, &hdmiOutputSettings);

    BDBG_MSG(("source changed to %d\n", hdmiInputStatus.originalFormat)) ;
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
        BDBG_WRN(("avmute_changed callback: Unable to get hdmiInput status")) ;
        return ;
    }

    BDBG_WRN(("avmute_changed callback: %s\n", 
    hdmiInputStatus.avMute ? "Set_AvMute" : "Clear_AvMute")) ;
	
}


void hdmiInputHdcpStateChanged(void *context, int param)
{
    NEXUS_HdmiInputHandle hdmiInput ;
    NEXUS_HdmiInputHdcpStatus hdmiInputHdcpStatus ;
    NEXUS_HdmiOutputHdcpStatus outputHdcpStatus;
    NEXUS_Error rc ;

    BSTD_UNUSED(param) ;

    hdmiInput = (NEXUS_HdmiInputHandle) context ;

    /* check the authentication state and process accordingly */
    NEXUS_HdmiInput_HdcpGetStatus(hdmiInput, &hdmiInputHdcpStatus) ;
    
    switch (hdmiInputHdcpStatus.eAuthState)
    {
    case NEXUS_HdmiInputHdcpAuthState_eKeysetInitialization :
		
        BDBG_WRN(("Change in HDCP Key Set detected")) ;
        switch (hdmiInputHdcpStatus.eKeyStorage)
        {
        case NEXUS_HdmiInputHdcpKeyStorage_eOtpROM :
            BDBG_WRN(("HDCP KeySet stored in OTP ROM")) ;
            break ;
            
        case NEXUS_HdmiInputHdcpKeyStorage_eOtpRAM :
            BDBG_WRN(("HDCP Keys stored in OTP RAM")) ;
            break ;
            
        default :
            BDBG_WRN(("Unknown Key Storage type %d", hdmiInputHdcpStatus.eKeyStorage));
        }

        BDBG_WRN(("NOTE: EACH DEVICE REQUIRES A UNIQUE HDCP KEY SET; The same KeySet cannot be used in multiple devices\n\n")) ;

        break ;

    case NEXUS_HdmiInputHdcpAuthState_eWaitForKeyloading :
        BDBG_WRN(("Upstream HDCP Authentication request ...")) ;
        rc = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmiOutput);	
        if (rc) BERR_TRACE(rc) ;
        break ;
    
    case NEXUS_HdmiInputHdcpAuthState_eWaitForDownstreamKsvs :
        BDBG_WRN(("Downstream FIFO Request; Start hdmiOuput Authentication...")) ;
    
        NEXUS_HdmiOutput_GetHdcpStatus(platformConfig.outputs.hdmi[0], &outputHdcpStatus);
        if ((outputHdcpStatus.hdcpState != NEXUS_HdmiOutputHdcpState_eWaitForRepeaterReady)
            && (outputHdcpStatus.hdcpState != NEXUS_HdmiOutputHdcpState_eCheckForRepeaterReady))
        {
            rc = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmiOutput);          
            if (rc) BERR_TRACE(rc) ;
        }
        break ;

    default :
        BDBG_WRN(("Unknown State %d", hdmiInputHdcpStatus.eAuthState )) ;
    }	
}

	
static void hdmiOutputHdcpStateChanged(void *pContext, int param)
{
    bool success = (bool)param;
    NEXUS_HdmiOutputHandle handle = pContext;

    NEXUS_HdmiOutputHdcpStatus hdmiOutputHdcpStatus;
    NEXUS_HdmiHdcpDownStreamInfo downStream  ;
    NEXUS_HdmiHdcpKsv *pKsvs ;
    NEXUS_Error rc ;

    unsigned returnedDevices ;
    uint8_t i ;

    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdmiOutputHdcpStatus);
	
    if (!success )
    {
        BDBG_WRN(("\nHDCP Authentication Failed.  Current State %d\n", 
            hdmiOutputHdcpStatus.hdcpState)) ;

        if ((hdmiOutputHdcpStatus.hdcpError == NEXUS_HdmiOutputHdcpError_eRxDevicesExceeded)
            || (hdmiOutputHdcpStatus.hdcpError == NEXUS_HdmiOutputHdcpError_eRepeaterDepthExceeded))
        {
            goto uploadDownstreamInfo ;
        }
		
        /* always retry */
        if (compliance_test)
        {
            rc = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmiOutput) ;
            if (rc) BERR_TRACE(rc) ;
        }
        
        return ;
    }


    BDBG_WRN(("HDCP Tx Authentication Successful")) ;
    hdmiOutputHdcpEnabled = true ;

/* Load Rx KSV FIFO for upstream device */	
uploadDownstreamInfo:	

    NEXUS_HdmiOuput_HdcpGetDownstreamInfo(handle, &downStream) ;
	

    /* allocate space to hold ksvs for the downstream devices */
    pKsvs = 
        BKNI_Malloc((downStream.devices + (uint8_t) hdmiOutputHdcpStatus.isHdcpRepeater) 
        	* NEXUS_HDMI_HDCP_KSV_LENGTH) ;
  
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

static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmiOutput = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;
    NEXUS_HdmiOutputBasicEdidData hdmiOutputBasicEdidData;
    NEXUS_HdmiOutputEdidBlock edidBlock;
    uint8_t *attachedRxEdid;
    uint16_t attachedRxEdidSize ;
    uint8_t i, j;
    bool useSampleEdid = false ;
    	
    NEXUS_Error rc = NEXUS_SUCCESS;

    NEXUS_HdmiOutput_GetStatus(hdmiOutput, &status);
    BDBG_WRN(("HDMI hotplug event: %s\n", 
		status.connected ? "connected" : "not connected")) ;

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( status.connected )
    {
    
        /* Get EDID of attached receiver*/
        rc = NEXUS_HdmiOutput_GetBasicEdidData(hdmiOutput, &hdmiOutputBasicEdidData);
        if (rc)
        {
            BDBG_ERR(("Unable to get downstream EDID; Use declared EDID in app for repeater's EDID")) ;
            pEDID = (uint8_t *) &SampleEDID[0] ;
            edidSize = SampleEDIDSize ;
    
            useSampleEdid = true ;
            goto load_edid ;
        }
    
        /* allocate space to hold the EDID blocks */
        attachedRxEdidSize = (hdmiOutputBasicEdidData.extensions + 1) * sizeof(edidBlock.data) ;
        attachedRxEdid = BKNI_Malloc(attachedRxEdidSize) ;
    	
        for (i = 0; i <= hdmiOutputBasicEdidData.extensions ; i++)
        {
            rc = NEXUS_HdmiOutput_GetEdidBlock(hdmiOutput, i, &edidBlock);
            if (rc)
            {
                BDBG_ERR(("Error retrieving EDID Block %d from attached receiver;", i));
                pEDID = (uint8_t *) &SampleEDID[0] ;
                edidSize = SampleEDIDSize ;
    
                BKNI_Free(attachedRxEdid) ;
    
                useSampleEdid = true ;
                goto load_edid ;
            }
    
            /* copy EDID data */    
            for (j=0; j < sizeof(edidBlock.data); j++) 
                attachedRxEdid[i*sizeof(edidBlock.data)+j] = edidBlock.data[j];
        }  
    
        pEDID = attachedRxEdid ;
        edidSize = attachedRxEdidSize;
    
  load_edid:	
    
        /********************************************************/
        /* manipulation of EDID to add/remove capabilities can be done here        */
        /********************************************************/
    	
        /* 
              -- load the repeater's EDID with the selected EDID 
              -- (either the attached device, internally declared SampleEDID, or  combination of the two) 
              */
        rc = NEXUS_HdmiInput_LoadEdidData(hdmiInput, pEDID, edidSize);
        if (rc) BERR_TRACE(rc);
    
        if (!useSampleEdid)
        {
            /* release memory resources */
            BKNI_Free(attachedRxEdid);
        }	
    

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

        BDBG_WRN(("Toggle Rx HOT PLUG to force upstream re-authentication...")) ;
        NEXUS_HdmiInput_ToggleHotPlug(hdmiInput) ; 

    }
    else    /* device disconnected. Load internal EDID */
    {   
        rc = NEXUS_HdmiInput_LoadEdidData(hdmiInput, &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
        if (rc) BERR_TRACE(rc);
    }
}

void hdmi_input_status(void ) 
{
    NEXUS_HdmiInputStatus status ;
    BDBG_Level saveLevel ;
    static const char *textColorSpace[] =
    {
        "RGB ",
        "YCbCr 4:2:2",
        "YCbCr 4:4:4",
        "Max",
    } ;

    BDBG_GetModuleLevel("repeater_passthrough", &saveLevel) ;
	
    BDBG_SetModuleLevel("repeater_passthrough", BDBG_eMsg) ;

    NEXUS_HdmiInput_GetStatus(hdmiInput, &status) ;
    if (!status.validHdmiStatus)
    {
        BDBG_WRN(("Cannot determine input status...")) ;
        return  ;
    }

    BDBG_MSG(("hdmiInput Mode  : %s", status.hdmiMode ? "HDMI" : "DVI")) ;
    BDBG_MSG(("hdmiInput Format: %d x %d %c %s", 
		status.avWidth, status.avHeight, status.interlaced ? 'i' : 'p',
		textColorSpace[status.colorSpace])) ;
	
	
    BDBG_MSG(("hdmiInput Clock : %d", status.lineClock)) ;
    BDBG_MSG(("HDCP Enabled    : %s", status.hdcpRiUpdating ? "Yes" : "No")) ;

    /* restore debug level */
    BDBG_SetModuleLevel("repeater_passthrough", saveLevel) ;

}


static void initializeHdmiOutputHdcpSettings(void) 
{
    NEXUS_HdmiOutputHdcpSettings hdmiOutputHdcpSettings;

    NEXUS_HdmiOutput_GetHdcpSettings(hdmiOutput, &hdmiOutputHdcpSettings);

        /* copy the encrypted key set and its Aksv here  */
        BKNI_Memcpy(hdmiOutputHdcpSettings.encryptedKeySet, encryptedTxKeySet, 
            NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS * sizeof(NEXUS_HdmiOutputHdcpKey)) ;
        BKNI_Memcpy(&hdmiOutputHdcpSettings.aksv, &hdcpTxAksv, 
            NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH) ;

        /* install HDCP success  callback */
        hdmiOutputHdcpSettings.successCallback.callback = hdmiOutputHdcpStateChanged ;
        hdmiOutputHdcpSettings.successCallback.context =  hdmiOutput ;
        hdmiOutputHdcpSettings.successCallback.param = true;

        /* install HDCP failure callback */
        hdmiOutputHdcpSettings.failureCallback.callback = hdmiOutputHdcpStateChanged ;
        hdmiOutputHdcpSettings.failureCallback.context = hdmiOutput ;
        hdmiOutputHdcpSettings.failureCallback.param = false;

    NEXUS_HdmiOutput_SetHdcpSettings(hdmiOutput, &hdmiOutputHdcpSettings);

    /* install list of revoked KSVs from SRMs (System Renewability Message) if available */
    NEXUS_HdmiOutput_SetHdcpRevokedKsvs(hdmiOutput, 
        RevokedKsvs, NumRevokedKsvs) ;
     
 }

#endif

int main(int argc, char **argv)
{
#if NEXUS_HAS_HDMI_INPUT
    NEXUS_Error rc;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_HdmiInputSettings hdmiInputSettings;
    NEXUS_HdmiOutputSettings hdmiOutputSettings;
    NEXUS_HdmiOutputStatus hdmiOutputStatus;
    
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformSettings platformSettings ;

    unsigned hdmiInputIndex = 0;
    
    NEXUS_HdmiOutputBasicEdidData hdmiOutputBasicEdidData;
    uint8_t *attachedRxEdid;
    uint16_t attachedRxEdidSize ;
    bool useSampleEdid = false ;
    unsigned timeout=4;

    BDBG_Level debugLevel ;
	
    int curarg = 1;

    
    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-exit")) {
            force_exit = true  ;
        }
        else if (!strcmp(argv[curarg], "-compliance")) {
            compliance_test = true  ;
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
        
        /* all HDMI Tx/Rx combo chips have EDID RAM */
        hdmiInputSettings.useInternalEdid = true ;

    /* check for connected downstream device */
    do
    {
        /* check for connected downstream device */
        rc = NEXUS_HdmiOutput_GetStatus(hdmiOutput, &hdmiOutputStatus);
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
        NEXUS_HdmiOutputEdidBlock edidBlock;
        unsigned i, j;
                
        /* Get EDID of attached receiver*/
        rc = NEXUS_HdmiOutput_GetBasicEdidData(hdmiOutput, &hdmiOutputBasicEdidData);
        if (rc)
        {
            BDBG_ERR(("Unable to get downstream EDID; Use declared EDID in app for repeater's EDID")) ;
            goto use_sample_edid ;
        }
        
        /* allocate space to hold the EDID blocks */
        attachedRxEdidSize = (hdmiOutputBasicEdidData.extensions + 1) * sizeof(edidBlock.data) ;
        attachedRxEdid = BKNI_Malloc(attachedRxEdidSize) ;
	
        for (i = 0; i <= hdmiOutputBasicEdidData.extensions ; i++)
        {
            rc = NEXUS_HdmiOutput_GetEdidBlock(hdmiOutput, i, &edidBlock);
            if (rc)
            {
                BDBG_ERR(("Error retrieving EDID Block %d from attached receiver;", i));
                        BKNI_Free(attachedRxEdid) ;
    
                goto use_sample_edid ;
            }
            
            /* copy EDID data */    
            for (j = 0 ; j < sizeof(edidBlock.data) ; j++) 
                attachedRxEdid[i*sizeof(edidBlock.data)+j] = edidBlock.data[j];
        }

        pEDID = attachedRxEdid ;
        edidSize = attachedRxEdidSize;

        goto open_with_edid ;
    
    }

use_sample_edid:
     pEDID = (uint8_t *) &SampleEDID[0] ;
     edidSize = SampleEDIDSize ;
     useSampleEdid = true ;

    /* fall through and open with sample EDID */

open_with_edid:

    /********************************************************/
    /* manipulation of EDID to add/remove capabilities can be done here        */
    /********************************************************/
	
    hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, 
		&hdmiInputSettings, pEDID, (uint16_t) edidSize);

    if (!useSampleEdid)
    {
        /* release memory resources */
        BKNI_Free(pEDID);
    }

    if (!hdmiInput) 
    {
        BDBG_WRN(("Can't get hdmi input\n")) ;
        return -1;
    }

    NEXUS_HdmiInput_GetSettings(hdmiInput, &hdmiInputSettings) ;
        hdmiInputSettings.avMuteChanged.callback = avmute_changed;
        hdmiInputSettings.avMuteChanged.context = hdmiInput ;
    
        hdmiInputSettings.sourceChanged.callback = source_changed;
        hdmiInputSettings.sourceChanged.context = hdmiInput ;	
    NEXUS_HdmiInput_SetSettings(hdmiInput, &hdmiInputSettings) ;

    {
        NEXUS_HdmiInputHdcpSettings hdmiInputHdcpSettings ;
            
        NEXUS_HdmiInput_HdcpGetDefaultSettings(hdmiInput, &hdmiInputHdcpSettings) ;
		
            /* chips with both hdmi rx and tx cores should always set repeater to TRUE */
            BDBG_WRN(("HDCP default Repeater Setting: %d", 
                hdmiInputHdcpSettings.repeater)) ;
            hdmiInputHdcpSettings.repeater = true ;
                
            hdmiInputHdcpSettings.hdcpRxChanged.callback = hdmiInputHdcpStateChanged ;
            hdmiInputHdcpSettings.hdcpRxChanged.context = hdmiInput ;
            hdmiInputHdcpSettings.hdcpRxChanged.param = 0 ;			

        NEXUS_HdmiInput_HdcpSetSettings(hdmiInput, &hdmiInputHdcpSettings) ;
    }

    /* Initialize HDCP settings / keys */
    initializeHdmiOutputHdcpSettings() ;
 

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);

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
    enable_audio(hdmiOutput) ;

    if (force_exit)
    {
        BDBG_WRN(("\n\n\n\n")) ;
        return 0 ;
    }

    for ( ;; )
    {
        int tmp;

        BDBG_GetModuleLevel("repeater_passthrough",  &debugLevel) ;
        BDBG_SetModuleLevel("repeater_passthrough",  BDBG_eMsg) ;

            BDBG_MSG(("Main Menu"));
            BDBG_MSG(("1) Change Video Format")) ;
            BDBG_MSG(("2) Toggle PCM/Compressed audio (currently %s)", 
    			hdmiCompressedAudio?"Compressed":"PCM")) ;
            BDBG_MSG(("3) hdmiInput Status")) ;
    		
            BDBG_MSG(("0) Exit\n")) ;
            BDBG_MSG(("Enter Selection: ")) ;

        BDBG_SetModuleLevel("repeater_passthrough", debugLevel) ;


        scanf("%d", &tmp);
        switch ( tmp )
        {
        case 0:
            goto exit_menu ;
        case 1:
            NEXUS_Display_GetSettings(display, &displaySettings);
            switch ( displaySettings.format )
            {
            case NEXUS_VideoFormat_e1080p: tmp=0; break;
            case NEXUS_VideoFormat_e1080i: tmp=1; break;
            case NEXUS_VideoFormat_e720p: tmp=2; break;
            case NEXUS_VideoFormat_e480p: tmp=3; break;
            default: tmp=4; break;
            }

            BDBG_GetModuleLevel("repeater_passthrough",  &debugLevel) ;
            BDBG_SetModuleLevel("repeater_passthrough",  BDBG_eMsg) ;

                BDBG_MSG(("Current format is %d", tmp)) ;
                BDBG_MSG(("Enter new format (0=1080p 1=1080i 2=720p 3=480p 4=NTSC): ", tmp)) ;

            BDBG_SetModuleLevel("repeater_passthrough", debugLevel) ;

            scanf("%d", &tmp);
            switch ( tmp )
            {
            case 0: displaySettings.format = NEXUS_VideoFormat_e1080p; break;            
            case 1: displaySettings.format = NEXUS_VideoFormat_e1080i; break;
            case 2: displaySettings.format = NEXUS_VideoFormat_e720p; break;
            case 3: displaySettings.format = NEXUS_VideoFormat_e480p; break;
            default: displaySettings.format = NEXUS_VideoFormat_eNtsc; break;
            }
            NEXUS_Display_SetSettings(display, &displaySettings);
            break;
        case 2:
            disable_audio(hdmiOutput);
            enable_audio(hdmiOutput);
            break;

        case 3:
            hdmi_input_status() ;

            break;


        default:
            break;
        }
    }

exit_menu:

    disable_audio(hdmiOutput) ;

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

