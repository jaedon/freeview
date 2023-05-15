/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_sat_asrc.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/bluetooth/nexus_sat_asrc.c $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/

/******************************************************************************
 * This file was used to demo Tuning to a Direct Tv Xm Radio Channel, caputing the Pcm audio and
 * then sending it out to the bluetooth headsets. 
 * Hardcoded values below are:
 *   -bluetooth headset address- can be overided by cmd line
 *   -Satellite Tunning Parmeters: sat.frequency, sat.mode, audioPid, pcrPid. ParserBand settings 
 * Use the DSS settings for Directv channels.  
 * Notes: Logitech FreePulse headsets and Parrot BT Speaker do Not work with this Demo. 
 * Todo: 1. Flush pcm data on channel change  
 *       2. use buttons on bt headset to be able to toggle between what channels 
*****************************************************************************/


#include <stdio.h>
#include "nexus_platform.h"
#include "nexus_platform_features.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include "nexus_frontend.h"

#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_parser_band.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_core_utils.h"
#include "bt_hook.h"
#include <string.h>
#include <unistd.h>


/*#define USE_DVB*/   /* else use DSS */
/*#define  CH_100*/   /* else settings for ch 806 */

int first_get_buffer_flag=0; 
int initBTOnce =0;
NEXUS_AudioCaptureHandle capture;
NEXUS_AudioCaptureStartSettings captureSettings;
  
static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;

    param=param;    /* unused */

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    fprintf(stderr, "Lock callback - state %d\n", status.demodLocked);
}

int audio_source_get_buffer(void **data, unsigned *size, void *context)
{
    NEXUS_Error rc;

    /* Need to start AudioCapture_Start on first audio_source_get_buffer call */	
    if(first_get_buffer_flag ==0)
    {
     	  rc = NEXUS_AudioCapture_Start(capture, &captureSettings);
	  BDBG_ASSERT(!rc);
	  first_get_buffer_flag=1;
    }	
	
    rc = NEXUS_AudioCapture_GetBuffer(context, data, size);
    BDBG_ASSERT(!rc);

    return rc;
}

int audio_source_write_complete(unsigned size, void *context)
{
    NEXUS_Error rc;

    rc = NEXUS_AudioCapture_WriteComplete(context, size);
    BDBG_ASSERT(!rc);

    return rc;
}

int main(int argc, char **argv)
{
    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    unsigned i;

    NEXUS_ParserBand parserBand;
    NEXUS_AudioDecoderHandle decoder;
  
    NEXUS_AudioCaptureOpenSettings captureOpenSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_AudioDecoderStartSettings decoderSettings;
  
    NEXUS_Error rc;
    BtHookInitSettings btInitSettings;
    BtHookAudioSourceSettings audioSourceSettings;
    /* const char *bd_addr_str = "00:0D:44:83:0D:12"; Developer's Logiech Freepulse. Override on cmdline. */
    const char *bd_addr_str = "00:07:a4:f3:5c:eb"; /* Developer's Motorola HT820BT.  Override on cmdline. */
    int channelChange=0;

    if (argc > 1) {
        bd_addr_str = argv[1];
    }

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    decoder = NEXUS_AudioDecoder_Open(0, NULL);
    if ( NULL == decoder )
    {
        fprintf(stderr, "Unable to open decoder 0\n");
        return 0;
    }
	
     NEXUS_AudioCapture_GetDefaultOpenSettings(&captureOpenSettings);
	captureOpenSettings.fifoSize= 48 *1024;
    capture = NEXUS_AudioCapture_Open(0, &captureOpenSettings);
    if ( NULL == capture )
    {
        fprintf(stderr, "Unable to open capture channel\n");
        return 0;
    }

    /*capture  satellite tuner audio */
   for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support satellite? */
            if ( capabilities.satellite )
            {
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to  find Satellite-capable frontend\n");
        return 0;
    }

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
#ifdef USE_DVB
    satSettings.frequency = 1119000000;
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
#else
#ifdef CH_100
    satSettings.frequency = 1396820000;
#else
    satSettings.frequency = 1425980000;
#endif
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDss;
#endif
    satSettings.lockCallback.callback = lock_callback;
    satSettings.lockCallback.context = frontend;
    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e2; /* USER: Change this value to match your platform */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    printf("Input Band is %d\n",userParams.param1);
#ifdef USE_DVB 
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
#else
     parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
#endif

    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
#ifdef USE_DVB
    diseqcSettings.toneEnabled = true;
    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
#else
    diseqcSettings.toneEnabled = false;
    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
#endif
    NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);
    printf("\n\n Set up DisegcSettings\n\n");

           /* Connect DAC to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Connect capture to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
	
    while (1) {
        uint16_t pcrPid, videoPid, audioPid;
  	 NEXUS_AudioCodec audioCodec;
  	 NEXUS_VideoCodec videoCodec;
        NEXUS_FrontendSatelliteStatus satStatus;
        int i =1;
	 
       if(channelChange==0)
	 {
 		satSettings.frequency = 1425980000;
		videoPid = 4095;
		pcrPid = 180; /*should be the same as the audio pid*/
		audioPid = 180;		
	}
	else if(channelChange ==1)
	{
		satSettings.frequency = 1280180000;
		videoPid = 4095;
		pcrPid = 200; 
		audioPid = 200;
	}
	else
	{
		satSettings.frequency = 1425980000;
		videoPid = 4095;
		pcrPid = 170; /*should be the same as the audio pid*/
		audioPid = 170;

	}
	videoCodec = NEXUS_VideoCodec_eMpeg2;
	audioCodec = NEXUS_AudioCodec_eMpeg;
	
	printf("frequency %d videoPid %d pcrPid %d audioPid %d\n",
		 satSettings.frequency,
		videoPid,
		pcrPid,
		audioPid);


		
        NEXUS_Frontend_TuneSatellite(frontend, &satSettings);

        while (i)
        {
            memset(&satStatus, 0, sizeof(NEXUS_FrontendSatelliteStatus));
            NEXUS_Frontend_GetSatelliteStatus(frontend, &satStatus);
            printf("\n\n Sat Status locked  %d\n\n",(int)satStatus.demodLocked);
            if(satStatus.demodLocked)
                break;
            else
                BKNI_Sleep(1000);
        }

    /* Open the StcChannel to do TSM with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = NEXUS_PidChannel_Open(parserBand,  pcrPid, NULL);
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Start the audio decoder */
    NEXUS_AudioDecoder_GetDefaultStartSettings(&decoderSettings);
    decoderSettings.codec = audioCodec;
    decoderSettings.pidChannel = NEXUS_PidChannel_Open(parserBand, audioPid, NULL);
    decoderSettings.stcChannel = stcChannel;
    NEXUS_AudioDecoder_Start(decoder, &decoderSettings);
	
    if(initBTOnce==0)
    {
        /* bring up bluetooth stack */
       BtHook_GetDefaultInitSettings(&btInitSettings);
       rc = BtHook_Init(&btInitSettings);
       BDBG_ASSERT(!rc);

		rc = BtHook_ConnectHeadset(bd_addr_str);
		BDBG_ASSERT(!rc);
	
		/* connect the capture-from-decode to the audio source hook in the BT sw */
		BtHook_GetAudioSourceSettings(&audioSourceSettings);
		audioSourceSettings.getBuffer = audio_source_get_buffer;
		audioSourceSettings.writeComplete = audio_source_write_complete;
		audioSourceSettings.callbackContext = capture;
		rc = BtHook_SetAudioSourceSettings(&audioSourceSettings);
		BDBG_ASSERT(!rc);
	
	  initBTOnce=1;
    }
 	 printf("Press key to rotate through channels.Ctrl-C to exit\n");
        getchar();
 
	if(channelChange ==0)
		channelChange =1;
	else if(channelChange==1)
		channelChange=2;
	else
		channelChange=0;
	
	NEXUS_AudioDecoder_Stop(decoder);
	
    }

/* end of capture  satellite tuner audio */

    NEXUS_AudioCapture_Stop(capture);
    NEXUS_AudioDecoder_Stop(decoder);
    BtHook_Uninit();

    return 0;
}

