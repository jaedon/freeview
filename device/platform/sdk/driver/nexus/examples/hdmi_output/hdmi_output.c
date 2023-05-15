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
 * $brcm_Workfile: hdmi_output.c $
 * $brcm_Revision: 21 $
 * $brcm_Date: 10/23/12 7:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Q:/projects/7425/latest/nexus/examples/hdmi_output/hdmi_output.c $
 * 
 * 21   10/23/12 7:32p rgreen
 * SW7425-4139: Create separate toggle menu; add toggle for AvMute
 * 
 * 20   7/26/12 4:51p rgreen
 * SW7425-2989: Update selection of test formats (cng)
 * 
 * 19   7/18/12 3:46p rgreen
 * SW7425-3060:  Modify HDCP example apps to use
 *  NEXUS_HdmiOutputHdcpStatus to determine the success/failure of
 *  authentication instead of a callback parameter
 * 
 * 18   5/7/12 2:00p rgreen
 * SW7425-3011: Stop HDMI callbacks prior to NEXUS_Display_Close to
 *  prevent HDMI callbacks from  accessing the freed Display resource
 * 
 * 17   4/12/12 11:36a rgreen
 * SW7425-1988: Make HDCP Tx Key Set declaration/examples consistent
 *  accross all hdmi examples
 * 
 * 16   1/19/12 4:05p rgreen
 * SW7425-2226: Make HDCP Ksv and KeySet declarartions consistent between
 *  examples
 * 
 * 15   10/7/11 1:55p rgreen
 * SW7425-1435:  Add code showing the loading of Tx HDCP Key Set
 * 
 * 14   7/12/11 3:02p erickson
 * SW7420-1979: add clean shutdown code
 * 
 * 13   1/7/11 3:21p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 12   12/21/10 3:04p rgreen
 * SW7422-10: allow all HDMI examples to build on Settop and DTV
 *  platforms.
 * 
 * 11   12/6/10 3:03p rgreen
 * SW7422-10: allow all HDMI examples to build on Settop and DTV platforms
 * 
 * 10   8/12/10 4:20p jgarrett
 * SW7342-238: Connecting PCM HDMI audio by default
 * 
 * 9   3/22/10 2:37a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 8   11/4/09 5:47p rgreen
 * SW7405-3314: Add support for checking for HDCP Revoked KSVs
 * 
 * 7   6/26/09 5:17p erickson
 * PR56102: simplify hotplug handler. NEXUS_Display_RemoveOutput no longer
 *  needed.
 *
 * 6   4/2/08 11:56a jgarrett
 * PR 41213: Coverity CID 7894
 *
 * 5   2/26/08 7:45p jgarrett
 * PR 39017: Adding HDCP
 *
 * 4   2/25/08 9:59a jgarrett
 * PR 39435: Fixing typo
 *
 * 3   2/25/08 9:52a jgarrett
 * PR 39435: Fixing includes
 *
 * 2   2/5/08 1:49p jgarrett
 * PR 39017: Adding Audio
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
******************************************************************************/
/* Nexus example app: single live a/v decode from an input band, routed to hdmi output */

#include "nexus_platform.h"
#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_component_output.h"
#include "nexus_hdmi_output_hdcp.h"
#include "nexus_core_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"

#include "bkni.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

BDBG_MODULE(hdmi_output) ;

static bool hdmiCompressedAudio = false, audioStarted = false, hdmiAvMute = false ;
static bool hdmiHdcpEnabled = false ;

static NEXUS_PlatformConfiguration platformConfig;
static NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
static NEXUS_AudioDecoderStartSettings audioProgram;

static void initializeHdmiOutputHdcpSettings(void)  ;


/*****************/
/* For HDCP TESTING  */
/*    1) insert the Production Key Set set generated by BCrypt  */
/*    2) set the USE_PRODUCTION_KEYS macro to to 1 */
/*****************/

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
{    {0x14, 0xF7, 0x61, 0x03, 0xB7} };

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


static void disconnect_hdmi_audio(NEXUS_HdmiOutputHandle hdmi)
{
    NEXUS_AudioDecoderHandle decoder;

    if ( hdmiCompressedAudio )
    {
        decoder = compressedDecoder;
    }
    else
    {
        decoder = pcmDecoder;
    }

    if ( audioStarted )
        NEXUS_AudioDecoder_Stop(decoder);

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmi));

    if ( audioStarted )
        NEXUS_AudioDecoder_Start(decoder, &audioProgram);
}

static void connect_hdmi_audio(NEXUS_HdmiOutputHandle hdmi)
{
    NEXUS_AudioDecoderHandle decoder;
    NEXUS_AudioInput connector;

    if ( hdmiCompressedAudio )
    {
        decoder = compressedDecoder;
        connector = NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eCompressed);
    }
    else
    {
        decoder = pcmDecoder;
        connector = NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo);
    }

    if ( audioStarted )
        NEXUS_AudioDecoder_Stop(decoder);

    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi), connector);

    if ( audioStarted )
        NEXUS_AudioDecoder_Start(decoder, &audioProgram);
}

static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;
    NEXUS_DisplaySettings displaySettings;

    NEXUS_HdmiOutput_GetStatus(hdmi, &status);

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( !status.connected )
    {
        BDBG_WRN(("No RxDevice Connected")) ;
        return ;
    }

    NEXUS_Display_GetSettings(display, &displaySettings);
    if ( !status.videoFormatSupported[displaySettings.format] )
    {
        BDBG_ERR(("Current format not supported by attached monitor. Switching to preferred format %d", 
            status.preferredVideoFormat)) ;
        displaySettings.format = status.preferredVideoFormat;
        NEXUS_Display_SetSettings(display, &displaySettings);
    }

    /* restart HDCP if it was previously enabled */
    if (hdmiHdcpEnabled)
    {
        NEXUS_HdmiOutput_StartHdcpAuthentication(platformConfig.outputs.hdmi[0]);
    }
}


static void hdmiOutputHdcpStateChanged(void *pContext, int param)
{
	
    bool success = false ; 
    NEXUS_HdmiOutputHandle handle = pContext;
    NEXUS_HdmiOutputHdcpStatus hdcpStatus;

    BSTD_UNUSED(param) ;

    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdcpStatus);
    switch (hdcpStatus.hdcpError)
    {
    case NEXUS_HdmiOutputHdcpError_eSuccess :
        BDBG_WRN(("HDCP Authentication Successful\n"));
        success = true ;
        hdmiHdcpEnabled = true ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRxBksvError :
        BDBG_ERR(("HDCP Rx BKsv Error")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRxBksvRevoked :
        BDBG_ERR(("HDCP Rx BKsv/Keyset Revoked")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRxBksvI2cReadError :
    case NEXUS_HdmiOutputHdcpError_eTxAksvI2cWriteError :
        BDBG_ERR(("HDCP I2C Read Error")) ;
        break ;
		
    case NEXUS_HdmiOutputHdcpError_eTxAksvError :
        BDBG_ERR(("HDCP Tx Aksv Error")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eReceiverAuthenticationError :
        BDBG_ERR(("HDCP Receiver Authentication Failure")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRepeaterAuthenticationError :
    case NEXUS_HdmiOutputHdcpError_eRepeaterLinkFailure :    /* Repeater Error; unused */    
        BDBG_ERR(("HDCP Repeater Authentication Failure")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRxDevicesExceeded :
        BDBG_ERR(("HDCP Repeater MAX Downstram Devices Exceeded")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRepeaterDepthExceeded :
        BDBG_ERR(("HDCP Repeater MAX Downstram Levels Exceeded")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRepeaterFifoNotReady :
        BDBG_ERR(("Timeout waiting for Repeater")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eRepeaterDeviceCount0 : /* unused */
        break ;

    case NEXUS_HdmiOutputHdcpError_eLinkRiFailure :
        BDBG_ERR(("HDCP Ri Integrity Check Failure")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eLinkPjFailure :
        BDBG_ERR(("HDCP Pj Integrity Check Failure")) ;
        break ;

    case NEXUS_HdmiOutputHdcpError_eFifoUnderflow :
    case NEXUS_HdmiOutputHdcpError_eFifoOverflow :
        BDBG_ERR(("Video configuration issue")) ;
        break ;
		
    case NEXUS_HdmiOutputHdcpError_eMultipleAnRequest : /* Should not reach here; but flag if occurs */
        BDBG_WRN(("Multiple Authentication Request... ")) ;  
		
    default :
        BDBG_WRN(("Unknown HDCP Authentication Error %d", hdcpStatus.hdcpError)) ;
    }
    
    if (!success)
    {
        fprintf(stderr, "\nHDCP Authentication Failed.  Current State %d\n", hdcpStatus.hdcpState);

        /* always retry */        
        NEXUS_HdmiOutput_StartHdcpAuthentication(platformConfig.outputs.hdmi[0]);        
    }
}


static void initializeHdmiOutputHdcpSettings(void) 
{
    NEXUS_HdmiOutputHdcpSettings hdmiOutputHdcpSettings;

    NEXUS_HdmiOutput_GetHdcpSettings(platformConfig.outputs.hdmi[0], &hdmiOutputHdcpSettings);

        /* copy the encrypted key set and its Aksv here  */
        BKNI_Memcpy(hdmiOutputHdcpSettings.encryptedKeySet, encryptedTxKeySet, 
            NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS * sizeof(NEXUS_HdmiOutputHdcpKey)) ;
        BKNI_Memcpy(&hdmiOutputHdcpSettings.aksv, &hdcpTxAksv, 
            NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH) ;

        /* install HDCP success  callback */
        hdmiOutputHdcpSettings.successCallback.callback = hdmiOutputHdcpStateChanged ;
        hdmiOutputHdcpSettings.successCallback.context = platformConfig.outputs.hdmi[0];

        /* install HDCP failure callback */
        hdmiOutputHdcpSettings.failureCallback.callback = hdmiOutputHdcpStateChanged ;
        hdmiOutputHdcpSettings.failureCallback.context = platformConfig.outputs.hdmi[0];

    NEXUS_HdmiOutput_SetHdcpSettings(platformConfig.outputs.hdmi[0], &hdmiOutputHdcpSettings);

    /* install list of revoked KSVs from SRMs (System Renewability Message) if available */
    NEXUS_HdmiOutput_SetHdcpRevokedKsvs(platformConfig.outputs.hdmi[0], 
        RevokedKsvs, NumRevokedKsvs) ;
     
 }


void toggle_menu(void)
{
    bool done = false;

    while (!done)
    {
        int tmp;
        
        /* Display Menu */
        printf("Toggle Menu\n");
        printf(" 1) Toggle PCM/Compressed audio (currently %s)\n", 
            hdmiCompressedAudio ? "Compressed" : "PCM");
        printf(" 2) Toggle AvMute (currently %s)\n",
		hdmiAvMute ? "Set_AvMute" : "Clear_AvMute");
        printf(" 0) Exit\n");
        printf("Enter Selection: ");
        scanf("%d", &tmp);
        switch ( tmp )
        {
        case 0:
	     done = true ;
            break;
			
        case 1:
            disconnect_hdmi_audio(platformConfig.outputs.hdmi[0]);
            hdmiCompressedAudio = !hdmiCompressedAudio;
            connect_hdmi_audio(platformConfig.outputs.hdmi[0]);
            break;

        case 2:
	      hdmiAvMute = !hdmiAvMute  ;
            NEXUS_HdmiOutput_SetAVMute(platformConfig.outputs.hdmi[0], hdmiAvMute) ;
            break;
        }
    }
}



int main(void)
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_HdmiOutputSettings hdmiSettings;
    NEXUS_HdmiOutputStatus hdmiStatus;
    bool done = false;
    NEXUS_Error rc;
    unsigned i, menuIndex ;
    
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

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    audioProgram.stcChannel = videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up audio decoders and connect to outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    /* Output PCM to DAC, SPDIF, and HDMI by default */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* bring up display */
    NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);
    
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if(platformConfig.outputs.component[0]){
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif    
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    /* Install hotplug callback -- video only for now */
    NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hotplug_callback;
    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
    hdmiSettings.hotplugCallback.param = (int)display;
    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);

    /* initalize HDCP settings, keys, etc. */
    initializeHdmiOutputHdcpSettings() ;

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Force a hotplug to switch to preferred format */
    hotplug_callback(platformConfig.outputs.hdmi[0], (int)display);

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    audioStarted = true;

    
    NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.format = hdmiStatus.preferredVideoFormat;
    NEXUS_Display_SetSettings(display, &displaySettings);
    
    while (!done)
    {
        int tmp;
        
        /* Find Current Format */
        NEXUS_Display_GetSettings(display, &displaySettings);
        for ( i = 0; testFormats[i].format ; i++)
        {
            if ( displaySettings.format == testFormats[i].format ) 
                break ;
        }

        
        /* Display Menu */
        printf("Main Menu\n");
        printf(" 1) Change Video Format (currently %s) \n", testFormats[i].name );
        printf(" 2) Toggle Menu\n") ;
        printf(" 3) Print decode status\n");
        printf(" 4) Enable HDCP\n");
        printf(" 5) Disable HDCP\n");
        printf(" 0) Exit\n");
        printf("Enter Selection: ");
        scanf("%d", &tmp);
        switch ( tmp )
        {
        case 0:
            done = true;
            break;
        case 1:

            /* get/build a list of formats supported by attached Receiver */
            NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);

            /* Display available formats */
            menuIndex = 0;
            printf("\n\n 0) Return to Main Menu\n");                

            i = 0 ;
            while ( testFormats[i].format ) 
            {
                /* if format is supported; add to menu */		
                if ( hdmiStatus.videoFormatSupported[testFormats[i].format] )
                {
                    testFormats[i].menuEntry = ++ menuIndex ;
                    printf("%2d) %s\n", menuIndex, testFormats[i].name );
                }
                i++;
            }

            /* Read user input for desired format */
            printf("\nEnter new format-> ");                
            scanf("%d", &tmp);

            if( !tmp )  /* 0 - Exit */
                break;  /* Exit to Main Menu*/

            if ( (uint8_t) tmp > menuIndex) /* selection not listed */
            { 
                printf("\n'%d' is an invalid choice\n\n", tmp);
                break; 
            }
			
            for ( i = 0; ; i++) 
            {
                if ( tmp != testFormats[i].menuEntry ) /* search for match */
                    continue ;
				
                /* Update display to selected format */
                NEXUS_Display_GetSettings(display, &displaySettings);
                    displaySettings.format = testFormats[i].format;
                rc = NEXUS_Display_SetSettings(display, &displaySettings);
                if (rc)
                {
                    printf("ERROR changing format\n") ;
                }

                break;
            }
            
            break;
        case 2:
            toggle_menu() ;
            break;

        case 3:
            {
                NEXUS_VideoDecoderStatus status;
                NEXUS_AudioDecoderStatus audioStatus;
                uint32_t stc;

                NEXUS_VideoDecoder_GetStatus(vdecode, &status);
                NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
                printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                    status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
                NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
                printf("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                    audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
                NEXUS_AudioDecoder_GetStatus(compressedDecoder, &audioStatus);
                if ( audioStatus.started )
                {
                    printf("audio1            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                        audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
                }
            }
            break;
        case 4:
            {
                NEXUS_HdmiOutputStatus pStatus    ;

                NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &pStatus) ;
                if (!pStatus.connected)
                {
                    BDBG_WRN(("No Receiver Device Available")) ;
                    break ;
                }

                NEXUS_HdmiOutput_StartHdcpAuthentication(platformConfig.outputs.hdmi[0]);

            }
            break;
        case 5:
            NEXUS_HdmiOutput_DisableHdcpAuthentication(platformConfig.outputs.hdmi[0]);
            hdmiHdcpEnabled = false ;
            break;
        default:
            break;
        }
    }
    
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_VideoWindow_Close(window);

    /* stop/remove HDMI callbacks associated with display, 
    so those callbacks do not access display once it is removed */
    NEXUS_StopCallbacks(platformConfig.outputs.hdmi[0]);
    NEXUS_Display_Close(display);

    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_PidChannel_Close(audioProgram.pidChannel);
    NEXUS_Platform_Uninit();

    return 0;
}

#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
int main(int argc, char **argv)
{
    BSTD_UNUSED(argc) ;
    fprintf(stderr, "%s not supported on this platform\n", argv[0]);
    return 0;
}
#endif

