/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: tru_SRSHD.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 12/8/10 4:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/trusurroundhd/tru_SRSHD.c $
 * 
 * Hydra_Software_Devel/6   12/8/10 4:36p mananp
 * SW7420-1294:Removing capture support from the app for 7420
 * 
 * Hydra_Software_Devel/5   8/4/10 4:05p mananp
 * SW7325-785:Fixed the issue of correct outputmode set
 * 
 * Hydra_Software_Devel/4   8/3/10 11:07a mananp
 * SW7325-785:Setting default values for the certification parameters
 * 
 * Hydra_Software_Devel/3   7/27/10 7:05p mananp
 * SW7325-785:Adding the certification parameters in the tru SRS HD app
 * 
 * Hydra_Software_Devel/2   7/19/10 5:53p mananp
 * SW7325-726:Update the tru_SRSHD app
 * 
 * Hydra_Software_Devel/1   7/19/10 5:21p jrubio
 * SW7325-726: add SRS APP
 * 
 * 2   7/14/10 3:56p jrubio
 * SW7325-726: update app, Manan
 * 
 * 1   7/13/10 4:18p jrubio
 * SW7325-726: adding SRS util app
 *
 *****************************************************************************/


#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"

#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_tru_surround_hd.h"
#include "nexus_surface.h"

#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include "cmdline_args.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "bstd.h"
#include "bkni.h"
#include "fileio_custom.h"

#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"

#if B_HAS_ASF
#include "basf_probe.h"
#endif

#if B_HAS_AVI
#include "bavi_probe.h"
#endif

#include "bfile_stdio.h"

BDBG_MODULE(playback);

/*---------------------------------------------------------------------Tru surround HD command line arguments------------------------------------------------------------------------------------------*/
struct tru_SRSHD_command_args {
	int loopback;
    const char *config_file;
     NEXUS_TruSurroundHdSettings pSettings;
	 NEXUS_AudioDecoderSettings decodeSettings;
	 NEXUS_AudioDecoderCodecSettings codecSettings;
};
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */


static const namevalue_t g_transportTypeStrs[] = {
    {"ts",  NEXUS_TransportType_eTs},
    {"pes", NEXUS_TransportType_eMpeg2Pes},
    {"es",  NEXUS_TransportType_eEs},
    {"vob", NEXUS_TransportType_eVob},
    {"mp4", NEXUS_TransportType_eMp4},
    {"mkv", NEXUS_TransportType_eMkv},
    {"avi", NEXUS_TransportType_eAvi},
    {"asf", NEXUS_TransportType_eAsf},
    {"wav", NEXUS_TransportType_eWav},
    {NULL, 0}
};


static const namevalue_t g_audioCodecStrs[] = {
    {"mpeg", NEXUS_AudioCodec_eMpeg},
    {"mp3", NEXUS_AudioCodec_eMp3},
    {"ac3", NEXUS_AudioCodec_eAc3},
    {"ac3plus", NEXUS_AudioCodec_eAc3Plus},
    {"aac", NEXUS_AudioCodec_eAac},
    {"aacplus", NEXUS_AudioCodec_eAacPlus},
    {"wma", NEXUS_AudioCodec_eWmaStd},
    {"wmastd", NEXUS_AudioCodec_eWmaStd},
    {"wmapro", NEXUS_AudioCodec_eWmaPro},
    {"pcm", NEXUS_AudioCodec_ePcmWav},
    {"dra", NEXUS_AudioCodec_eDra},
    {"dts", NEXUS_AudioCodec_eDts},
    {"dtshd", NEXUS_AudioCodec_eDtsHd},
    {NULL, 0}
};

static const namevalue_t g_stcChannelMasterStrs[] = {
    {"first", NEXUS_StcChannelAutoModeBehavior_eFirstAvailable},
    {"video", NEXUS_StcChannelAutoModeBehavior_eVideoMaster},
    {"audio", NEXUS_StcChannelAutoModeBehavior_eAudioMaster},
    {NULL, 0}
};

static const namevalue_t g_endOfStreamActionStrs[] = {
    {"loop",  NEXUS_PlaybackLoopMode_eLoop},
    {"pause", NEXUS_PlaybackLoopMode_ePause},
    {"play",  NEXUS_PlaybackLoopMode_ePlay},
    {NULL, 0}
};

static const namevalue_t g_tsTimestampType[] = {
    {"none", NEXUS_TransportTimestampType_eNone},
    {"mod300", NEXUS_TransportTimestampType_eMod300},
    {"binary", NEXUS_TransportTimestampType_eBinary},
    {NULL, 0}
};


static const namevalue_t g_panelStrs[] = {
    {"wxga", 0},
    {"wxga50", 1},
    {"fhd", 2},
    {"fhd50", 3},
    {"off", 0xff},
    {NULL, 0}
};

#if NEXUS_HAS_FRONTEND
static const namevalue_t g_vsbModeStrs[] = {
    {"8", NEXUS_FrontendVsbMode_e8},
    {"16", NEXUS_FrontendVsbMode_e16},
    {NULL, 0}
};

static const namevalue_t g_qamModeStrs[] = {
    {"16", NEXUS_FrontendQamMode_e16},
    {"32", NEXUS_FrontendQamMode_e32},
    {"64", NEXUS_FrontendQamMode_e64},
    {"128", NEXUS_FrontendQamMode_e128},
    {"256", NEXUS_FrontendQamMode_e256},
    {"512", NEXUS_FrontendQamMode_e512},
    {"1024", NEXUS_FrontendQamMode_e1024},
    {"2048", NEXUS_FrontendQamMode_e2048},
    {"4096", NEXUS_FrontendQamMode_e4096},
    {"Auto_64_256", NEXUS_FrontendQamMode_eAuto_64_256},
    {NULL, 0}
};

static const namevalue_t g_satModeStrs[] = {
    {"dvb", NEXUS_FrontendSatelliteMode_eDvb},
    {"dss", NEXUS_FrontendSatelliteMode_eDss},
    {NULL, 0}
};
#endif


static unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    errno = 0; /* there is only way to know that strtol failed is to look at errno, so clear it first */
    value = strtol(name, NULL, 0);
    if(errno != 0) {
        errno = 0;
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}

/*--------------------------------------------------------------------- Runtime change in the user config params -------------------------------------------------------------------------------- */
static void change_config(char *input,struct tru_SRSHD_command_args *tru)
{
    char *value;
    value = strchr(input, '=');
	  *value = 0;
	  value++;
	 /*Tru Surround Parameters settings*/ 
	  if(strstr(input,"TRUSRS_ENABLED"))
		  tru->pSettings.enabled= atoi(value);	      
      else if(strstr(input,"HEADPHONE"))
		  tru->pSettings.headphoneEnabled = atoi(value);
	   else if(strstr(input,"INPUTGAIN"))
		   tru->pSettings.inputGain= atoi(value);
	   else if(strstr(input,"OUTPUTGAIN"))
		   tru->pSettings.outputGain= atoi(value);
	   else if(strstr(input,"BYPASSGAIN"))
		   tru->pSettings.bypassGain= atoi(value);

	   /*TruBass Parameters settings*/
	  else if(strstr(input,"TRUBASS_ENABLED"))
		  tru->pSettings.truBassEnabled = atoi(value);	      
     else if(strstr(input,"TRUBASS_LEVEL"))
		 tru->pSettings.truBassLevel = atoi(value);
      else if(strstr(input,"SPEAKERSIZE"))
		  tru->pSettings.speakerSize = atoi(value);
     else if(strstr(input,"TRUBASSMODE"))
		   tru->pSettings.truBassMode = atoi(value);	  

	     /*Definition parameters settings*/
	   else if(strstr(input,"DEFINITION_ENABLED"))
		   tru->pSettings.definitionEnabled= atoi(value);	      
	   else if(strstr(input,"DEFINITION_LEVEL"))
		   tru->pSettings.definitionLevel= atoi(value);	      
	 

	  /*Dialog clarity parameters settings*/
      else if(strstr(input,"DIALOG_CLARITY_ENABLED"))
		  tru->pSettings.dialogClarityEnabled= atoi(value);	      
      else if(strstr(input,"DIALOG_CLARITY_LEVEL"))
		  tru->pSettings.dialogClarityLevel= atoi(value);	  

	  else if(strstr(input,"SURROUND_LEVEL"))
		  tru->pSettings.surroundLevel= atoi(value);	

	  /*Wow HD Parameters settings*/
	  else if(strstr(input,"WOWHD_ENABLED"))
		  tru->pSettings.wowHdEnabled = atoi(value);	      
	  else if(strstr(input,"WOWHD_HIGHBITRATE_ENABLED"))
		 tru->pSettings.wowHdHighBitRateEnabled = atoi(value);
	  else if(strstr(input,"WOWHDMODE"))
		  tru->pSettings.wowHdMode = atoi(value);
	  else if(strstr(input,"WOWHDSPACE_LEVEL"))
		  tru->pSettings.wowHdSpaceLevel = atoi(value);
	  else if(strstr(input,"WOWHDCENTER_LEVEL"))
		  tru->pSettings.wowHdCenterLevel = atoi(value);
	  else if(strstr(input,"WOWHDFOCUS_ENABLED"))
		 tru->pSettings.wowHdFocusEnabled = atoi(value);
	   else if(strstr(input,"WOWHDFOCUS_LEVEL"))
		  tru->pSettings.wowHdFocusLevel = atoi(value);

	   /*Certification Parameters*/
        
        else if(strstr(input,"TSHD_INPUTACMOD")) /*Only applicable if the certification mode is true*/
			tru->pSettings.inputAcmod = atoi(value);

     /*AC3 Decode Config Parameters*/
	   else if(strstr(input,"OUTPUTMODE"))
		   tru->decodeSettings.outputMode= atoi(value);	      
	   else if(strstr(input,"OUTPUTLFECHANNEL"))
		   tru->decodeSettings.outputLfeMode=atoi(value);
	   else if(strstr(input,"DUALMODEENABLED"))
			 tru->decodeSettings.dualMonoMode=atoi(value);

		 else if(!strcmp(input,"LORODOWNMIX")) 
			 {
				 if(atoi(value)==0) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic; /* NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic*/
					 }
				 else if(atoi(value)==1) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible (Lt/Rt)*/
                    }
				 else if(atoi(value)==2) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard (Lo/Ro)*/
                    }
				 else if(atoi(value)==3) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eMax; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eMax*/
                    }
			 }
		 else if(!strcmp(input,"RFMODE"))
		 {
			 if(atoi(value)==0) {
				 tru->codecSettings.codecSettings.ac3.drcMode=NEXUS_AudioDecoderDolbyPulseDrcMode_eLine; /*NEXUS_AudioDecoderDolbyPulseDrcMode_eLine*/
			 }
			 else if(atoi(value)==1) {
				 tru->codecSettings.codecSettings.ac3.drcMode=NEXUS_AudioDecoderDolbyPulseDrcMode_eRf; /*NEXUS_AudioDecoderDolbyPulseDrcMode_eRf*/
			 }	      
		 }         
		   else if(strstr(input,"DRCCUT"))
			 tru->codecSettings.codecSettings.ac3.cut=atoi(value);

	   else if(strstr(input,"DRCBOOST"))
			tru->codecSettings.codecSettings.ac3.boost = atoi(value);

    return;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------Initialize the user configuration parameters through the config file----------------------------------------------------------------- */
static void parse_config_file(struct tru_SRSHD_command_args *tru)
{
	char line[400];
	char *value;
    FILE *f=fopen(tru->config_file,"r");

	while(fgets( line, 400, f )){
	  
	  value = strchr(line, '=');
	  *value = 0;
	  value++;

	  /*Tru Surround Parameters settings*/ 
	   if(!strcmp(line,"TRUSRS_ENABLED"))
		   tru->pSettings.enabled= atoi(value);	      
	   else if(!strcmp(line,"HEADPHONE"))
		   tru->pSettings.headphoneEnabled = atoi(value);
	   else if(!strcmp(line,"INPUTGAIN"))
		   tru->pSettings.inputGain= atoi(value);
	   else if(!strcmp(line,"OUTPUTGAIN"))
		   tru->pSettings.outputGain= atoi(value);
	   else if(!strcmp(line,"BYPASSGAIN"))
		   tru->pSettings.bypassGain= atoi(value);

	    /*TruBass Parameters settings*/
	   else if(!strcmp(line,"TRUBASS_ENABLED"))
		   tru->pSettings.truBassEnabled = atoi(value);	      
	  else if(!strcmp(line,"TRUBASS_LEVEL"))
		  tru->pSettings.truBassLevel = atoi(value);
	   else if(!strcmp(line,"SPEAKERSIZE"))
		   tru->pSettings.speakerSize = atoi(value);
	   else if(!strcmp(line,"TRUBASSMODE"))
		   tru->pSettings.truBassMode = atoi(value);	  

	     /*Definition parameters settings*/
	   else if(!strcmp(line,"DEFINITION_ENABLED"))
		   tru->pSettings.definitionEnabled= atoi(value);	      
	   else if(!strcmp(line,"DEFINITION_LEVEL"))
		   tru->pSettings.definitionLevel= atoi(value);	      

        /*Dialog clarity parameters settings*/
	   else if(!strcmp(line,"DIALOG_CLARITY_ENABLED"))
		   tru->pSettings.dialogClarityEnabled= atoi(value);	      
	   else if(!strcmp(line,"DIALOG_CLARITY_LEVEL"))
		   tru->pSettings.dialogClarityLevel= atoi(value);	


	   else if(!strcmp(line,"SURROUND_LEVEL"))
		   tru->pSettings.surroundLevel= atoi(value);	

	   /*Wow HD Parameters settings*/
	   else if(!strcmp(line,"WOWHD_ENABLED"))
		   tru->pSettings.wowHdEnabled = atoi(value);	      
	   else if(!strcmp(line,"WOWHD_HIGHBITRATE_ENABLED"))
		  tru->pSettings.wowHdHighBitRateEnabled = atoi(value);
	   else if(!strcmp(line,"WOWHDMODE"))
		   tru->pSettings.wowHdMode = atoi(value);
	   else if(!strcmp(line,"WOWHDSPACE_LEVEL"))
		   tru->pSettings.wowHdSpaceLevel = atoi(value);
	   else if(!strcmp(line,"WOWHDCENTER_LEVEL"))
		   tru->pSettings.wowHdCenterLevel = atoi(value);
	   else if(!strcmp(line,"WOWHDFOCUS_ENABLED"))
		  tru->pSettings.wowHdFocusEnabled = atoi(value);
	    else if(!strcmp(line,"WOWHDFOCUS_LEVEL"))
		   tru->pSettings.wowHdFocusLevel = atoi(value);

     
      /*Ac3 Decoder config params*/
		else if(!strcmp(line,"OUTPUTMODE"))
		  tru->decodeSettings.outputMode= atoi(value);	      
	  else if(!strcmp(line,"OUTPUTLFECHANNEL"))
		  tru->decodeSettings.outputLfeMode=atoi(value);
	  else if(!strcmp(line,"DUALMODEENABLED"))
		  tru->decodeSettings.dualMonoMode=atoi(value);

	  else if(!strcmp(line,"LORODOWNMIX"))
	  {
			     if(atoi(value)==0) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic; /* NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic*/
					 }
				 else if(atoi(value)==1) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible (Lt/Rt)*/
                    }
				 else if(atoi(value)==2) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard (Lo/Ro)*/
                    }
				 else if(atoi(value)==3) {
					 tru->codecSettings.codecSettings.ac3.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eMax; /*NEXUS_AudioDecoderDolbyStereoDownmixMode_eMax*/
                    }
	  }
	  else if(!strcmp(line,"RFMODE"))
	  {
			  if(atoi(value)==0) {
				  tru->codecSettings.codecSettings.ac3.drcMode=NEXUS_AudioDecoderDolbyPulseDrcMode_eLine; /*NEXUS_AudioDecoderDolbyPulseDrcMode_eLine*/
			  }
			  else if(atoi(value)==1) {
				  tru->codecSettings.codecSettings.ac3.drcMode=NEXUS_AudioDecoderDolbyPulseDrcMode_eRf; /*NEXUS_AudioDecoderDolbyPulseDrcMode_eRf*/
			  }	      
	  }
	  else if(!strcmp(line,"DRCCUT"))
		  tru->codecSettings.codecSettings.ac3.cut=atoi(value);

	  else if(!strcmp(line,"DRCBOOST"))
		 tru->codecSettings.codecSettings.ac3.boost = atoi(value);
     
	 /*Certification Parameters*/
		
	 else if(!strcmp(line,"TSHD_INPUTACMOD")) /*Only applicable if the certification mode is true*/
			tru->pSettings.inputAcmod = atoi(value);

	tru->pSettings.certificationApp = true; /*Always 1 for this app*/

	tru->pSettings.inputLfeEnabled= 0; /*Always 0 for this app*/

	}
	return;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------- Printing the current set values for the tru SRSHD user config parameters --------------------------------------------------------- */
static void print_settings(NEXUS_AudioDecoderSettings decodeSettings,NEXUS_AudioDecoderCodecSettings codecSettings,NEXUS_TruSurroundHdSettings pSettings)
{
	printf("\n\n---------------- The current values of the configuration parameters are as follows: ----------------- \n\n");
	printf ("\n\n");

	/*Tru Surround Parameters settings*/ 
    printf("TRUSRS_ENABLED=%d\n",pSettings.enabled);
    printf("HEADPHONE=%d\n",pSettings.headphoneEnabled);
    printf("INPUTGAIN=%d\n",pSettings.inputGain);
	printf("OUTPUTGAIN=%d\n",pSettings.outputGain);
	printf("BYPASSGAIN=%d\n",pSettings.bypassGain);

	/*TruBass Parameters settings*/
	printf("TRUBASS_ENABLED=%d\n",pSettings.truBassEnabled);
	printf("TRUBASS_LEVEL=%d\n",pSettings.truBassLevel);
	printf("SPEAKERSIZE=%d\n", pSettings.speakerSize );
	printf("TRUBASSMODE=%d\n", pSettings.truBassMode );

   /*Definition Parameters settings */
	printf("DEFINITION_ENABLED=%d\n", pSettings.definitionEnabled );
	printf("DEFINITION_LEVEL=%d\n", pSettings.definitionLevel );

	 /*Dialog clarity parameters settings*/
    printf("DIALOG_CLARITY_ENABLED=%d\n",pSettings.dialogClarityEnabled);
    printf("DIALOG_CLARITY_LEVEL=%d\n", pSettings.dialogClarityLevel);
    
    printf("SURROUND_LEVEL=%d\n", pSettings.surroundLevel);

	/* Wow Hd Parameters settings */
	printf("WOWHD_ENABLED=%d\n",pSettings.wowHdEnabled);
	printf("WOWHD_HIGHBITRATE_ENABLED=%d\n",pSettings.wowHdHighBitRateEnabled);
	printf("WOWHDMODE=%d\n",pSettings.wowHdMode);
	printf("WOWHDSPACE_LEVEL=%d\n",pSettings.wowHdSpaceLevel);
	printf("WOWHDCENTER_LEVEL=%d\n",pSettings.wowHdCenterLevel);
	printf("WOWHDFOCUS_ENABLED=%d\n",pSettings.wowHdFocusEnabled);
	printf("WOWHDFOCUS_LEVEL=%d\n",pSettings.wowHdFocusLevel);

	/*Certification Parameters*/
	printf("CERTIFICATIONMODE=%d\n",pSettings.certificationApp);
    printf("INPUTLFE_ENABLED=%d\n",pSettings.inputLfeEnabled);
    printf("TSHD_INPUTACMOD=%d\n",pSettings.inputAcmod);
            

	/*Ac3 decode config parameters*/

	printf("\nOUTPUTMODE = %d\nOUTPUTLFECHANNEL = %d\nDUALMODEENABLED = %d\nLORODOWNMIX = %d\nRFMODE = %d\nDRCCUT = %d\nDRCBOOST = %d\n",decodeSettings.outputMode,decodeSettings.outputLfeMode,decodeSettings.dualMonoMode,codecSettings.codecSettings.ac3.stereoDownmixMode,codecSettings.codecSettings.ac3.drcMode,codecSettings.codecSettings.ac3.cut,codecSettings.codecSettings.ac3.boost);

    printf ("\n\n");

	return;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*--------------------------------------------------------------------------Command line parsing------------------------------------------------------------------------------------------------- */
int tru_SRSHD_cmdline_parse(int argc, const char *argv[], struct util_opts_t *opts, struct tru_SRSHD_command_args *tru)
{
    int i;

	tru->loopback=0;

    memset(opts,0,sizeof(*opts));
    opts->transportType = NEXUS_TransportType_eTs;
    opts->audioCodec = NEXUS_AudioCodec_eMpeg;
    opts->compressedAudio = false;
    opts->decodedAudio = true;
    opts->stcChannelMaster = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
    opts->tsTimestampType = NEXUS_TransportTimestampType_eNone;
    opts->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->playpumpTimestampReordering = true;
    opts->customFileIo = false;
    opts->playbackMonitor = false;

    for (i=1;i<argc;i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
           printf("\nRun the app by running the following command:\n");
		   printf("\n ./\ nexus tru_SRSHD -probe streamname -config configfilename -loopback \n");
           printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
			
        }
        else if (!strcmp(argv[i], "-probe")) {
            opts->probe = true;
        }
		else if (!strcmp(argv[i], "-config")) {

            tru->config_file=argv[++i];
			printf("\n\n\n You entered the config name as :%s\n\n\n",tru->config_file);
		}
		else if (!strcmp(argv[i], "-loopback")) {

            tru->loopback=1;
			printf("\n\n\n You have enabled the loopback option\n\n\n");

			
		}
       else if (!opts->filename) {
            opts->filename = argv[i];
        }
        else if (!opts->indexname) {
            opts->indexname = argv[i];
        }
        else {
            printf("unknown param %s\n", argv[i]);
            return -1;
        }
    }

    /* this allows the user to set: "-mpeg_type es -video_type mpeg" and forget the "-video 1" option */
    if (opts->transportType == NEXUS_TransportType_eEs && !opts->videoPid && !opts->audioPid) {
            opts->audioPid = 1;
        
    }

    return 0;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */


static void start_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, const NEXUS_AudioDecoderStartSettings *audioProgram)
{
    NEXUS_Error rc;
    
    if (opts->audioPid) {
#if B_HAS_ASF
        /* if DRC for WMA pro is available apply now */
        if(audioProgram->codec == NEXUS_AudioCodec_eWmaPro && opts->dynamicRangeControlValid ){

            NEXUS_AudioDecoderCodecSettings codecSettings;

            BDBG_WRN(("wma-pro drc enabled,ref peak %d,ref target %d,ave peak %d, ave target %d",
                      opts->dynamicRangeControl.peakReference,opts->dynamicRangeControl.peakTarget,
                      opts->dynamicRangeControl.averageReference,opts->dynamicRangeControl.averageTarget));
            NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, audioProgram->codec, &codecSettings);
            codecSettings.codec = audioProgram->codec;
            codecSettings.codecSettings.wmaPro.dynamicRangeControlValid = true;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakReference = opts->dynamicRangeControl.peakReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakTarget = opts->dynamicRangeControl.peakTarget;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageReference = opts->dynamicRangeControl.averageReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageTarget = opts->dynamicRangeControl.averageTarget;
            NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&codecSettings);
            }
#endif

         if(opts->decodedAudio) {
            rc = NEXUS_AudioDecoder_Start(audioDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
        if(compressedDecoder) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
	}
    return;
}

static void stop_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder)
{
    

    if (opts->audioPid) {
        if(opts->decodedAudio) {
            NEXUS_AudioDecoder_Stop(audioDecoder);
 }
        if(compressedDecoder) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
    }
    return;
}

#define  B_HAS_PLAYBACK_MONITOR 0
#if B_HAS_PLAYBACK_MONITOR
#include <pthread.h>
#include "bkni_multi.h"
/*----------------------------------------------------------------------Monitoring playback--------------------------------------------------------------------------------------------------*/
typedef struct PlaybackMonitorState {
    bool terminate;
    pthread_t thread;
    BKNI_MutexHandle lock;
    const struct util_opts_t *opts;
    NEXUS_PlaybackHandle playback;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderHandle compressedDecoder;
    const NEXUS_AudioDecoderStartSettings *audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_FilePlayHandle customFile;
    NEXUS_FilePlayHandle stickyFile;
    const NEXUS_PlaybackStartSettings *playbackStartSettings;
	
} PlaybackMonitorState;

static void *
monitor_thread(void *state_)
{  
    const PlaybackMonitorState *state=state_;
    while(!state->terminate) {
		
        NEXUS_PlaybackStatus status;
        NEXUS_PlaybackSettings playbackSettings;
        
        BERR_Code rc;
        bool failed;

        rc = NEXUS_Playback_GetStatus(state->playback, &status);
        BDBG_ASSERT(!rc);
        BKNI_Sleep(1000);
		printf("\n\n the playback status is:%d \n\n",status.state);
        FileIoSticky_GetFailBit(state->stickyFile, &failed);
        if(!failed) {
            continue;
        }
        BDBG_WRN(("restarting from %u", status.position));
        BKNI_AcquireMutex(state->lock);
        NEXUS_Playback_Stop(state->playback);
        FileIoSticky_ClearFailBit(state->stickyFile);
        if(state->customFile) {
            FileIoCustomProbabilities probabilities;
            FileIoCustom_GetProbabilities(state->customFile, NULL, &probabilities);
            probabilities.error = 0;
            probabilities.nodata = 0;
            probabilities.partial_read = 0;
            FileIoCustom_SetProbabilities(state->customFile, &probabilities, &probabilities);
        }

        /* stop decoder */
        stop_audio(state->opts, state->audioDecoder, state->compressedDecoder);

        NEXUS_Playback_GetSettings(state->playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(state->playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Start decoders */
        start_audio(state->opts, state->audioDecoder, state->compressedDecoder, state->audioProgram);


        /* start playback  */
        rc = NEXUS_Playback_Start(state->playback, state->file, state->playbackStartSettings);
        BDBG_ASSERT(!rc);

        /* seek into desired location */
        rc = NEXUS_Playback_Seek(state->playback, status.position);
        BDBG_ASSERT(!rc);

        /* start playing */
        rc = NEXUS_Playback_Play(state->playback);
        BDBG_ASSERT(!rc);
        BKNI_ReleaseMutex(state->lock);

		
    }
    return NULL;
}

static void 
monitor_thread_start(PlaybackMonitorState *state)
{
    int rc;
    BKNI_CreateMutex(&state->lock);
    state->terminate = false;
    rc = pthread_create(&state->thread, NULL, monitor_thread, state);
    BDBG_ASSERT(rc==0);
    return;
}

static void 
monitor_thread_stop(PlaybackMonitorState *state)
{
    state->terminate = true;
    pthread_join(state->thread, NULL);
}
#endif /* B_HAS_PLAYBACK_MONITOR */


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int main(int argc, const char *argv[]) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle  audioPidChannel = NULL, pcrPidChannel;
    NEXUS_AudioDecoderHandle audioDecoder, compressedDecoder=NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file,customFile=NULL, stickyFile=NULL;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;
	NEXUS_PlaybackStatus pstatus;
    NEXUS_Error rc;

    NEXUS_TruSurroundHdHandle truSRSHD;
	
    
    FILE  *file_params=NULL;

    struct util_opts_t opts;
	struct tru_SRSHD_command_args tru_args;
    bool exit1;
    


/*-------------------------------------------------------------Initialization---------------------------------------------------------------------------------------------------------------------- */   
    if (tru_SRSHD_cmdline_parse(argc, argv, &opts, &tru_args)) {
        return 0;
    }


   /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

	if(tru_args.loopback==1) {
		opts.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
	}
	else{
		opts.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
	}

    if (cmdline_probe(&opts)) {
        return 1;
    }

    if (!opts.filename) {
        printf("\n**********************************************************************************************************************************************************\n");
		printf("\nRun the app by running the following command:\n");
		printf("\n ./\ nexus tru_SRSHD -probe streamname -config configfilename -loopback \n");
		printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
        printf("\n**********************************************************************************************************************************************************\n");
        return 0;
    }

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    if ((opts.indexname && !strcmp(opts.indexname, "same")) ||
        opts.transportType == NEXUS_TransportType_eMkv ||
        opts.transportType == NEXUS_TransportType_eMp4
        )
    {
        opts.indexname = opts.filename;
    }

    file = NEXUS_FilePlay_OpenPosix(opts.filename, opts.indexname);
    if (!file) {
        fprintf(stderr, "can't open files:%s %s\n", opts.filename, opts.indexname);
        return -1;
    }
    if(opts.customFileIo) {
        customFile = file = FileIoCustom_Attach(file);
        BDBG_ASSERT(file);
    }
    if(opts.playbackMonitor) {
        stickyFile = file = FileIoSticky_Attach(file);
        BDBG_ASSERT(file);
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.modeSettings.Auto.behavior = opts.stcChannelMaster;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = opts.transportType;
    playbackSettings.playpumpSettings.timestamp.pacing = false;
    playbackSettings.playpumpSettings.timestamp.type = opts.tsTimestampType;

    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = opts.stcTrick;
    playbackSettings.beginningOfStreamAction = opts.beginningOfStreamAction;
    playbackSettings.endOfStreamAction = opts.endOfStreamAction;
    playbackSettings.enableStreamProcessing = opts.streamProcessing;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    file_params = fopen(tru_args.config_file, "r");

	/* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	
    
	NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
	NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);

		     if(file_params){

			 parse_config_file(&tru_args);
            
              fclose(file_params);
		  }

    NEXUS_AudioDecoder_SetSettings(audioDecoder,&tru_args.decodeSettings);
	NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&tru_args.codecSettings);

	NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
	NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);

	truSRSHD = NEXUS_TruSurroundHd_Open(&tru_args.pSettings);

	NEXUS_TruSurroundHd_GetSettings(truSRSHD,&tru_args.pSettings);


	printf("\n\n After setting the values from the config file, the current values of the parameters are:\n");
	print_settings(tru_args.decodeSettings,tru_args.codecSettings,tru_args.pSettings); 
	printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");


    

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-----------------------------------------------------------Adding inputs to the corresponding output ports--------------------------------------------------------------------------------------- */
    BDBG_ASSERT(audioDecoder);

	NEXUS_TruSurroundHd_AddInput(truSRSHD, NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));

    rc = NEXUS_AudioOutput_AddInput(
         NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),NEXUS_TruSurroundHd_GetConnector(truSRSHD));

	BDBG_ASSERT(!rc);

	rc = NEXUS_AudioOutput_AddInput(
         NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),NEXUS_TruSurroundHd_GetConnector(truSRSHD));
   
    BDBG_ASSERT(!rc);

 	   
 /*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-------------------------------------------------------------------------Audio pidsettings--------------------------------------------------------------------------------------------- */

    if (opts.audioCodec != NEXUS_AudioCodec_eUnknown) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.audioPid, &playbackPidSettings);
    }

    if (opts.pcrPid && opts.pcrPid!=opts.videoPid && opts.pcrPid!=opts.audioPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
        pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.pcrPid, &playbackPidSettings);
    }

   NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------------------------- Start decoders and initialize the decoder and codec parameters------------------------------------------------------------------------------------*/
  start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram);

  NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
  NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);

  parse_config_file(&tru_args);


  NEXUS_AudioDecoder_SetSettings(audioDecoder,&tru_args.decodeSettings);
  NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&tru_args.codecSettings);

  NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
  NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);


	printf("\n\n After setting the values from the config file, the current values of the parameters are:\n");
	print_settings(tru_args.decodeSettings,tru_args.codecSettings,tru_args.pSettings); 
	printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");



/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------------------------------ Start playback---------------------------------------------------------------------------------------- */
    NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
    if(opts.autoBitrate) {
        playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
    }
    rc = NEXUS_Playback_Start(playback, file, &playbackStartSettings);
    BDBG_ASSERT(!rc);
#if B_HAS_PLAYBACK_MONITOR
    {
        PlaybackMonitorState monitorState;
        BKNI_Memset(&monitorState, 0, sizeof(monitorState));
        monitorState.opts = &opts;
        monitorState.playback = playback;
        monitorState.audioDecoder = audioDecoder;
        monitorState.compressedDecoder = compressedDecoder;
        monitorState.audioProgram = &audioProgram;
        monitorState.file = file;
        monitorState.stickyFile = stickyFile;
        monitorState.customFile = customFile;
        monitorState.playbackStartSettings = &playbackStartSettings;
		
        if(stickyFile) {
           monitor_thread_start(&monitorState);
        }
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------Run time modifications of the user config params if loopback is enabled-------------------------------------------------------------------------- */
if(tru_args.loopback==1) {
		  for(exit1=false;!exit1;) {
	
			char buffer[256];
				char *buf;
	
				
            #if B_HAS_PLAYBACK_MONITOR
					if(stickyFile) {
						BKNI_AcquireMutex(monitorState.lock);
					}
			#endif
					do {

					
					
							printf("Tru_SRSHD>"); fflush(stdout);
	
							fgets(buffer, 256, stdin);
                            
                           if(feof(stdin)) {
								break;
							}
							
							buffer[strlen(buffer)-1] = 0;
					
							buf = strtok(buffer, ";");
							if (!buf) continue;
		
						if (!strcmp(buf, "?")) {
							printf("Current set values are:\n");
							print_settings(tru_args.decodeSettings,tru_args.codecSettings,tru_args.pSettings); 
							printf("\nRun the app by running the following command:\n");
							printf("\n**********************************************************************************************************************************************************\n");
							printf("\n ./\ nexus tru_SRSHD -probe streamname -config configfilename -loopback \n");
                            printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
							printf("\n**********************************************************************************************************************************************************\n");
			
			
						}
						
						else if (!strcmp(buf, "quit")) {
							exit1 = true;
							break;
						}
						
						 else if ((strstr(buf, "TRUSRS_ENABLED")!=NULL)||(strstr(buf, "HEADPHONE")!=NULL)||(strstr(buf, "INPUTGAIN")!=NULL)||(strstr(buf, "OUTPUTGAIN")!=NULL)||(strstr(buf, "BYPASSGAIN")!=NULL)||(strstr(buf, "DIALOG_CLARITY_ENABLED")!=NULL)||(strstr(buf, "DIALOG_CLARITY_LEVEL")!=NULL)||(strstr(buf, "TRUBASS_ENABLED")!=NULL)||(strstr(buf, "TRUBASS_LEVEL")!=NULL)||(strstr(buf, "SPEAKERSIZE")!=NULL)||(strstr(buf, "TRUBASSMODE")!=NULL)||(strstr(buf, "DEFINITION_ENABLED")!=NULL)||(strstr(buf, "DEFINITION_LEVEL")!=NULL)||(strstr(buf, "SURROUND_LEVEL")!=NULL)||(strstr(buf, "WOWHD_ENABLED")!=NULL)||(strstr(buf, "WOWHD_HIGHBITRATE_ENABLED")!=NULL)||(strstr(buf, "WOWHDMODE")!=NULL)||(strstr(buf, "WOWHDSPACE_LEVEL")!=NULL)||(strstr(buf, "WOWHDCENTER_LEVEL")!=NULL)||(strstr(buf, "WOWHDFOCUS_ENABLED")!=NULL)||(strstr(buf, "WOWHDFOCUS_LEVEL")!=NULL)||(strstr(buf, "OUTPUTMODE") != NULL)||(strstr(buf, "OUTPUTLFECHANNEL") != NULL)||(strstr(buf, "DUALMODEENABLED") != NULL)||(strstr(buf, "LORODOWNMIX") != NULL)||(strstr(buf, "RFMODE") != NULL)||(strstr(buf, "DRCCUT") != NULL)||(strstr(buf, "DRCBOOST") != NULL)||(strstr(buf, "CERTIFICATIONMODE") != NULL)||(strstr(buf, "INPUTLFE_ENABLED") != NULL)||(strstr(buf, "TSHD_INPUTACMOD") != NULL) ) {
			
					
							 NEXUS_AudioOutput_RemoveInput(
								 NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
								 NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
							 NEXUS_AudioOutput_RemoveInput(
								  NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
								  NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
							 NEXUS_TruSurroundHd_GetSettings(truSRSHD,&tru_args.pSettings);

							NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
							NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);

                            change_config(buf,&tru_args);
							
							NEXUS_TruSurroundHd_SetSettings(truSRSHD,&tru_args.pSettings);
							NEXUS_AudioDecoder_SetSettings(audioDecoder,&tru_args.decodeSettings);
							NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&tru_args.codecSettings);

							NEXUS_TruSurroundHd_GetSettings(truSRSHD,&tru_args.pSettings);
							NEXUS_AudioDecoder_GetSettings(audioDecoder,&tru_args.decodeSettings);
							NEXUS_AudioDecoder_GetCodecSettings(audioDecoder,audioProgram.codec,&tru_args.codecSettings);

							printf("\n After the modification, now the new values are as follows:\n");
							print_settings(tru_args.decodeSettings,tru_args.codecSettings,tru_args.pSettings); 
							printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");


			
							rc = NEXUS_AudioOutput_AddInput(
								NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
							rc = NEXUS_AudioOutput_AddInput(
								 NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
                           }
	
			   else if (!*buf) {
					/* allow blank line */
				}
				else {
					printf("unknown command: %s\n", buf);
				}
			}
			while ((buf = strtok(NULL, ";"))&&(pstatus.state!=2));
	#if B_HAS_PLAYBACK_MONITOR
			if(stickyFile) {
				BKNI_ReleaseMutex(monitorState.lock);
			}
	#endif
		}
 }
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*------------------------------------------------------------------------Program stops at the end of the playback if loopback is not enabled------------------------------------------------------ */
  else{
	 #if B_HAS_PLAYBACK_MONITOR
					if(stickyFile) {
						BKNI_AcquireMutex(monitorState.lock);
					}
     #endif

	rc = NEXUS_Playback_GetStatus(playback, &pstatus);
	BDBG_ASSERT(!rc);
	while(pstatus.state!=2) {
		sleep(1);
		rc = NEXUS_Playback_GetStatus(playback, &pstatus);
		BDBG_ASSERT(!rc);
     }
    
    #if B_HAS_PLAYBACK_MONITOR
			if(stickyFile) {
				BKNI_ReleaseMutex(monitorState.lock);
			}
	#endif

  }
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------Stopping, Closing and Removing the used functionalities at the end of the run-------------------------------------------------- */
#if B_HAS_PLAYBACK_MONITOR
    if(stickyFile) {
        monitor_thread_stop(&monitorState);
    }
    }
#endif
	NEXUS_Playback_Stop(playback);

    stop_audio(&opts, audioDecoder, compressedDecoder);

    NEXUS_Playback_CloseAllPidChannels(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);


   
			NEXUS_AudioOutput_RemoveInput(
								 NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
								 NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
			NEXUS_AudioOutput_RemoveInput(
								  NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
								  NEXUS_TruSurroundHd_GetConnector(truSRSHD));
			
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    if(compressedDecoder) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        NEXUS_AudioDecoder_Close(compressedDecoder);
    }
    NEXUS_Platform_Uninit(); 
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
    return 0;
}

