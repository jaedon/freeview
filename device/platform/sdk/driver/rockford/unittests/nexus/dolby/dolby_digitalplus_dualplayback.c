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
 * $brcm_Workfile: dolby_digitalplus_dualplayback.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 9/6/12 6:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/dolby/dolby_digitalplus_dualplayback.c $
 * 
 * Hydra_Software_Devel/11   9/6/12 6:02p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_surface.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "cmdline_args.h"

#include "nexus_ac3_encode.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "bstd.h"
#include "bkni.h"
#include "fileio_custom.h"

BDBG_MODULE(playback);
/*----------------------------------------------------------------------------------------DDP dual playback command line arguments------------------------------------------------------------------*/
struct dolby_digital_plus_command_args {


	const char *config_file;
	const char *primary_stream,*secondary_stream;

    bool ddc_enabled;
	unsigned secondary_substreamId;

    int loopback;
	NEXUS_AudioDecoderSettings decodeSettings;
	NEXUS_AudioDecoderCodecSettings codecSettings;


   };
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static bool g_isVideoEs = true;


/*--------------------------------------------------------------------------------------Capture callback--------------------------------------------------------------------------------------------*/
static void capture_callback(void *pParam, int param)
{
    NEXUS_AudioCaptureHandle capture = pParam;
    FILE *pFile = (FILE *)param;
    NEXUS_Error errCode;

    for ( ;; )
    {
        void *pBuffer;
        size_t bufferSize;

        /* Check available buffer space */
        errCode = NEXUS_AudioCapture_GetBuffer(capture, (void **)&pBuffer, &bufferSize);
        if ( errCode )
        {
            fprintf(stderr, "Error getting capture buffer\n");
            NEXUS_AudioCapture_Stop(capture);
            return;
        }

        if ( bufferSize > 0 )
        {
            /* Write samples to disk */
            if ( 1 != fwrite(pBuffer, bufferSize, 1, pFile) )
            {
                fprintf(stderr, "Error writing to disk\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }

            /*fprintf(stderr, "Data callback - Wrote %d bytes\n", (int)bufferSize);*/
            errCode = NEXUS_AudioCapture_WriteComplete(capture, bufferSize);
            if ( errCode )
            {
                fprintf(stderr, "Error committing capture buffer\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }
        }
        else
        {
            break;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------Monitoring playback----------------------------------------------------------------------------------------------------------*/
#define  B_HAS_PLAYBACK_MONITOR 0
#if B_HAS_PLAYBACK_MONITOR
#include <pthread.h>
#include "bkni_multi.h"

typedef struct PlaybackMonitorState {
    bool terminate;
	int iter;
    pthread_t thread;
    BKNI_MutexHandle lock;
    const struct util_opts_t *opts;
    NEXUS_PlaybackHandle playback;
    NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_AudioDecoderHandle secondaryDecoder;
    NEXUS_AudioDecoderHandle compressedDecoder;
    const NEXUS_AudioDecoderStartSettings *audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_FilePlayHandle customFile;
    NEXUS_FilePlayHandle stickyFile;
    const NEXUS_PlaybackStartSettings *playbackStartSettings;
} PlaybackMonitorState;

static void *monitor_thread(void *state_)
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
    
    /*  stop_audio(state->opts, state->audioDecoder,state->secondaryDecoder, state->compressedDecoder,capture,state->iter);*/

        NEXUS_Playback_GetSettings(state->playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(state->playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Start decoders */
        
     /*   start_audio(state->opts, state->audioDecoder,state->secondaryDecoder, state->state->compressedDecoder, state->audioProgram,capture,pFile,state->iter);*/


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
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------Setting the configuration parameters through the config file------------------------------------------------------------*/
static void parse_config_file(struct dolby_digital_plus_command_args *dolby)
{
	char line[400];
	char *value;
    FILE *f=fopen(dolby->config_file,"r");

	while(fgets( line, 400, f )){
	  
	  value = strchr(line, '=');
	  *value = 0;
	  value++;
	  
	  if(!strcmp(line,"OUTPUTMODE"))
		  dolby->decodeSettings.outputMode= atoi(value);	      
	  else if(!strcmp(line,"OUTPUTLFECHANNEL"))
		  dolby->decodeSettings.outputLfeMode=atoi(value);
	  else if(!strcmp(line,"DUALMONO_MODE"))
		  dolby->decodeSettings.dualMonoMode=atoi(value);

	  else if(!strcmp(line,"DOWNMIX_MODE"))
	  {
              if(atoi(value)==2) {
				  dolby->codecSettings.codecSettings.ac3Plus.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*Lo/Ro always*/
			  }
			  else{
				  dolby->codecSettings.codecSettings.ac3Plus.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*Lo/Ro always*/
				  printf("\n\n Only DOWNMIX_MODE=2 (LoRo Downmix) is allowed for this application. \n\n");
			  }

	  }
	  else if(!strcmp(line,"RFMODE"))
	  {
			  if(atoi(value)==0) {
				  dolby->codecSettings.codecSettings.ac3Plus.drcMode=NEXUS_AudioDecoderDolbyDrcMode_eLine;
			  }
			  else if(atoi(value)==1) {
				  dolby->codecSettings.codecSettings.ac3Plus.drcMode=NEXUS_AudioDecoderDolbyDrcMode_eRf;
			  }	      
	  }
	  else if(!strcmp(line,"DRCCUT"))
		  dolby->codecSettings.codecSettings.ac3Plus.cut = atoi(value);

	  else if(!strcmp(line,"DRCBOOST"))
		 dolby->codecSettings.codecSettings.ac3Plus.boost = atoi(value);

	  else if(!strcmp(line,"SECONDARY_SUBSTREAMID"))
	  {
			 
				  dolby->secondary_substreamId = atoi(value);
			 
	  }

     else if(!strcmp(line,"DDC_ENABLED"))
	  {
		  if(atoi(value)==1)
		 {
			  dolby->ddc_enabled=true;
		  }
		  else{
			  dolby->ddc_enabled=false;
		  }

	  }
	}

	return;
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------Managing the run time change into the config parameters--------------------------------------------------------------------------------*/

static void change_config(char *input,struct dolby_digital_plus_command_args *dolby)
{
    char *value;
   value = strchr(input, '=');
					*value = 0;
					value++;
                
				  if(strstr(input,"OUTPUTMODE"))
					  dolby->decodeSettings.outputMode= atoi(value);	      
				  else if(strstr(input,"OUTPUTLFECHANNEL"))
					  dolby->decodeSettings.outputLfeMode=atoi(value);
				  else if(strstr(input,"DUALMONO_MODE"))
					    dolby->decodeSettings.dualMonoMode=atoi(value);
 
					else if(!strcmp(input,"DOWNMIX_MODE")) 
						{
                            if(atoi(value)==2) {
								dolby->codecSettings.codecSettings.ac3Plus.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*Lo/Ro always*/
							}
							else{
                                dolby->codecSettings.codecSettings.ac3Plus.stereoDownmixMode=NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard; /*Lo/Ro always*/
								printf("\n\n No other value than LoRo downmix (DOWNMIX_MODE=2) is allowed \n\n");
							}
						}

					else if(!strcmp(input,"RFMODE"))
					{
						 if(atoi(value)==0) {
							 dolby->codecSettings.codecSettings.ac3Plus.drcMode=NEXUS_AudioDecoderDolbyDrcMode_eLine;
						 }
						 else if(atoi(value)==1) {
							 dolby->codecSettings.codecSettings.ac3Plus.drcMode=NEXUS_AudioDecoderDolbyDrcMode_eRf;
						 }
					}         
	  				  else if(strstr(input,"DRCCUT"))
					    dolby->codecSettings.codecSettings.ac3Plus.cut=atoi(value);

				  else if(strstr(input,"DRCBOOST"))
					   dolby->codecSettings.codecSettings.ac3Plus.boost = atoi(value);
			
                  else if(strstr(input,"DDC_ENABLED"))
				  {
					  if(atoi(value)==1) {
						  dolby->ddc_enabled=true;
					  }
					  else{
						  dolby->ddc_enabled=false;
					  }

				  }    
				  return;
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------Printing the current values of the config parameters--------------------------------------------------------------------------*/

static void print_settings(NEXUS_AudioDecoderSettings decodeSettings,NEXUS_AudioDecoderCodecSettings codecSettings,struct dolby_digital_plus_command_args *dolby)
{
	printf("\n\n---------------- The current values of the configuration parameters are as follows: ----------------- \n\n");
    
    printf("\n(1)OUTPUTMODE = %d\n(2)OUTPUTLFECHANNEL = %d\n(3)DUALMONO_MODE = %d\n(4)DOWNMIX_MODE = %d\n(5)RFMODE = %d\n(6)DRCCUT = %d\n(7)DRCBOOST = %d\n(8)DDC_ENABLED = %d\n(9)SECONDARY_SUBSTREAMID = %d\n\n",decodeSettings.outputMode,decodeSettings.outputLfeMode,decodeSettings.dualMonoMode,(codecSettings.codecSettings.ac3Plus.stereoDownmixMode),codecSettings.codecSettings.ac3Plus.drcMode,codecSettings.codecSettings.ac3Plus.cut,codecSettings.codecSettings.ac3Plus.boost,dolby->ddc_enabled,codecSettings.codecSettings.ac3Plus.substreamId);

	printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------Command line arguments parsing---------------------------------------------------------------------------------------------*/

int dolby_digital_plus_cmdline_parse(int argc, const char *argv[], struct util_opts_t *opts, struct dolby_digital_plus_command_args *dolby,int j)
{
    int i;
	int codectypereturn = 0;
	dolby->loopback=0;

	memset(opts,0,sizeof(*opts));
    opts->common.transportType = NEXUS_TransportType_eTs;
    
    opts->common.audioCodec = NEXUS_AudioCodec_eMpeg;
    opts->common.contentMode = NEXUS_VideoWindowContentMode_eFull;
    opts->common.compressedAudio = false;
    opts->common.decodedAudio = true;
#if NEXUS_DTV_PLATFORM
    opts->common.usePanelOutput = true;
    opts->common.displayType = NEXUS_DisplayType_eLvds;
    opts->common.displayFormat = NEXUS_VideoFormat_eCustom0;
#else
    opts->common.useCompositeOutput = true;
    opts->common.useComponentOutput = true;
    opts->common.displayFormat = NEXUS_VideoFormat_eNtsc;
    opts->common.displayType = NEXUS_DisplayType_eAuto;
#endif
    opts->stcChannelMaster = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
    opts->common.tsTimestampType = NEXUS_TransportTimestampType_eNone;
    opts->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->common.playpumpTimestampReordering = true;
    opts->customFileIo = false;
    opts->playbackMonitor = false;

    for (i=1;i<argc;i++) {
        
        if (!strcmp(argv[i], "-probe")) {
            opts->common.probe = true;
        }
		else if (!strcmp(argv[i], "-audio_type") ) { /* Added the feature to enforce the audiotype considering few special cases where streams have both types of packets*/

			opts->common.audioCodec=lookup(g_audioCodecStrs, argv[++i]);

			if(opts->common.audioCodec == NEXUS_AudioCodec_eAc3Plus) {
				printf("\n You have selected Ac3Plus as an audiocodec \n");
				codectypereturn = 1;
			}
			else if(opts->common.audioCodec == NEXUS_AudioCodec_eAc3) {
				printf("\n You have selected Ac3 as an audiocodec \n");
				codectypereturn = 2;
			}
            g_isVideoEs = false;
        }
		else if (!strcmp(argv[i], "-config")) {

            dolby->config_file=argv[++i];
			printf("\n\n\n You entered the config file name :%s\n\n\n",dolby->config_file);
			
		}
		else if (!strcmp(argv[i], "-loopback")) {

            dolby->loopback=1;
			printf("\n\n\n You have enabled the loopback option\n\n\n");

        }
		
        else if (!strcmp(argv[i], "-secondary")) {

			dolby->secondary_stream=argv[++i];
		}
       
	   else if (!strcmp(argv[i], "-primary")) {
		   
			   dolby->primary_stream = argv[++i];
           }
	  
        else if (!opts->indexname) {
            opts->indexname = argv[i];
        }
        else {
            printf("unknown param %s\n", argv[i]);
            return -1;
        }
    }
     if(j==0) {
		   opts->filename=dolby->primary_stream;
		   	printf("\n\n\n You have selected the primary stream : %s \n\n\n",dolby->primary_stream);
	   }
	 if(j==1) {
		   opts->filename=dolby->secondary_stream;
		   printf("\n\n\n You have selected the secondary stream : %s \n\n\n",dolby->secondary_stream);
		    
	   }

    if (opts->common.usePanelOutput) {
        opts->common.displayType = NEXUS_DisplayType_eLvds;
        opts->common.useComponentOutput = false;
        opts->common.useCompositeOutput = false;
    }
    else {
        opts->common.displayType = NEXUS_DisplayType_eAuto;
    }

    /* this allows the user to set: "-mpeg_type es -video_type mpeg" and forget the "-video 1" option */
    if (opts->common.transportType == NEXUS_TransportType_eEs && !opts->common.videoPid && !opts->common.audioPid) {
	
        if (g_isVideoEs) {
            opts->common.videoPid = 1;
        }
        else {
            opts->common.audioPid = 1;
	
        }
    }

    return codectypereturn;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int main(int argc, const char *argv[]) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle audioPidChannel = NULL, pcrPidChannel;
    NEXUS_AudioDecoderOpenSettings audioOpenSettings;
    NEXUS_AudioDecoderHandle audioDecoder[2];
	NEXUS_AudioMixerSettings mixerSettings;
	NEXUS_AudioMixerHandle mixer;
    NEXUS_AudioDecoderStartSettings audioProgram[2];
    NEXUS_FilePlayHandle file[2],customFile=NULL, stickyFile=NULL;
    NEXUS_PlaypumpHandle playpump[2];
    NEXUS_PlaybackStatus pstatus;
    NEXUS_PlaybackHandle playback[2];
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;

	 NEXUS_AudioCaptureStartSettings captureSettings;

	int i,no_of_streams = 2;
    NEXUS_Error rc;
 

	struct dolby_digital_plus_command_args dolby_args;
    struct util_opts_t opts[2];

	FILE  *file_params=NULL;

    bool exit;
	
	NEXUS_AudioCaptureHandle capture;

	FILE *pFile;
	const char *pFileName = "audio_capture.dat";

	int audcodec;

/*-------------------------------------------------------------------Initialization----------------------------------------------------------------------------------------------------------------*/  

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    
	pFile = fopen(pFileName, "wb+");
        if ( NULL == pFile )
		{
			fprintf(stderr, "Unable to open file '%s' for writing captured data\n", pFileName);
			return 0;
		}

	

  capture = NEXUS_AudioCapture_Open(0, NULL); 
  BDBG_ASSERT(capture);
  NEXUS_AudioMixer_GetDefaultSettings(&mixerSettings);
  mixerSettings.mixUsingDsp = true;
  mixer = NEXUS_AudioMixer_Open(&mixerSettings);

  for(i=0;i<no_of_streams;i++) {

	  	if(i==0) {
			audioDecoder[i] = NEXUS_AudioDecoder_Open(i, NULL);
			BDBG_ASSERT(audioDecoder[i]);
		}
		else if(i==1) {
			NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
			audioDecoder[i] = NEXUS_AudioDecoder_Open(i,&audioOpenSettings);
			BDBG_ASSERT(audioDecoder[i]);

		}
	  playpump[i] = NEXUS_Playpump_Open(i, NULL);
	  BDBG_ASSERT(playpump[i]);
	  playback[i] = NEXUS_Playback_Create();
	  BDBG_ASSERT(playback[i]);

  audcodec=dolby_digital_plus_cmdline_parse(argc, argv, &opts[i], &dolby_args,i);
		 
    /* Bring up all modules for a platform in a default configuraiton for this platform */
   
  if(dolby_args.loopback==1) {
			opts[i].endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
		}
		else{
			opts[i].endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
		}
 
		if (cmdline_probe(&opts[i].common, opts[i].filename, &opts[i].indexname)) {
        return 1;
    }
 /*------------------------------Added the feature to enforce the audiotype considering few special cases where streams have both types of packets--------------------------------------------------*/
	 if(audcodec == 1) {
		 printf("\n\n***********************Applying Ac3Plus codec ***********************\n\n");
		 opts[i].common.audioCodec = NEXUS_AudioCodec_eAc3Plus;
	 }
	 else if(audcodec == 2) {
		 printf("\n\n***********************Applying Ac3 codec ***********************\n\n");
		 opts[i].common.audioCodec = NEXUS_AudioCodec_eAc3;
	 }
 /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/    


    if (!opts[i].filename) {
        print_usage(argv[0]);
        return 0;
    }

    if ((opts[i].indexname && !strcmp(opts[i].indexname, "same")) ||
        opts[i].common.transportType == NEXUS_TransportType_eMkv ||
        opts[i].common.transportType == NEXUS_TransportType_eMp4
        )
    {
        opts[i].indexname = opts[i].filename;
    }

    file[i] = NEXUS_FilePlay_OpenPosix(opts[i].filename, opts[i].indexname);
    if (!file[i]) {
        fprintf(stderr, "can't open files:%s %s\n", opts[i].filename, opts[i].indexname);
        return -1;
    }
    if(opts[i].customFileIo) {
        customFile = file[i] = FileIoCustom_Attach(file[i]);
        BDBG_ASSERT(file[i]);
    }
    if(opts[i].playbackMonitor) {
        stickyFile = file[i] = FileIoSticky_Attach(file[i]);
        BDBG_ASSERT(file[i]);
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.modeSettings.Auto.behavior = opts[i].stcChannelMaster;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback[i], &playbackSettings);
    playbackSettings.playpump = playpump[i];
    playbackSettings.playpumpSettings.transportType = opts[i].common.transportType;
    playbackSettings.playpumpSettings.timestamp.pacing = false;
    playbackSettings.playpumpSettings.timestamp.type = opts[i].common.tsTimestampType;

    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = opts[i].stcTrick;
    playbackSettings.beginningOfStreamAction = opts[i].beginningOfStreamAction;
    playbackSettings.endOfStreamAction = opts[i].endOfStreamAction;
    playbackSettings.enableStreamProcessing = opts[i].streamProcessing;
    rc = NEXUS_Playback_SetSettings(playback[i], &playbackSettings);
    BDBG_ASSERT(!rc);

	if (opts[i].common.audioCodec != NEXUS_AudioCodec_eUnknown) {
				NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
				playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
                playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder[i];
                playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts[i].common.audioCodec;
				audioPidChannel = NEXUS_Playback_OpenPidChannel(playback[i], opts[i].common.audioPid, &playbackPidSettings);
			}



		if (opts[i].common.pcrPid && opts[i].common.pcrPid!=opts[i].common.videoPid && opts[i].common.pcrPid!=opts[i].common.audioPid) {
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
			pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback[i], opts[i].common.pcrPid, &playbackPidSettings);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram[i]);
		audioProgram[i].codec = opts[i].common.audioCodec;
		audioProgram[i].pidChannel = audioPidChannel;
		audioProgram[i].stcChannel = stcChannel;
		if(i==1) {
			audioProgram[i].secondaryDecoder=true;

		}

		
  }
    file_params = fopen(dolby_args.config_file, "r");

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/  	
	
   for(i=0;i<no_of_streams;i++) {
	 
	  NEXUS_AudioDecoder_GetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	  NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[i],audioProgram[i].codec,&dolby_args.codecSettings);

	  if(file_params){
	  parse_config_file(&dolby_args);
	  }
	  if(i==0) {
		  dolby_args.codecSettings.codecSettings.ac3Plus.substreamId=0;
	  }
	  else if(i==1) {
		  dolby_args.codecSettings.codecSettings.ac3Plus.substreamId=dolby_args.secondary_substreamId;
	  }

	  NEXUS_AudioDecoder_SetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	  NEXUS_AudioDecoder_SetCodecSettings(audioDecoder[i],&dolby_args.codecSettings);

	  NEXUS_AudioDecoder_GetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	  NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[i],audioProgram[i].codec,&dolby_args.codecSettings);
	}
	
/*---------------------------------------------------------------------Audio Mixing-----------------------------------------------------------------------------------------------------------------*/  	    
	/* Add both decoders to the mixer */
    
	NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eStereo));
	NEXUS_AudioMixer_AddInput(mixer, NEXUS_AudioDecoder_GetConnector(audioDecoder[1], NEXUS_AudioDecoderConnectorType_eStereo));

	/* Set the Mixer to use DSP mixing */
	NEXUS_AudioMixer_GetSettings(mixer, &mixerSettings);
	mixerSettings.master = NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eStereo);
	NEXUS_AudioMixer_SetSettings(mixer, &mixerSettings);
			 
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/   

/*----------------------------------------------------Adding the audio to the respective audio output ports--------------------------------------------------------------------------------------------------------------------------------*/
   if(dolby_args.ddc_enabled==false) {
								NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
												  NEXUS_AudioMixer_GetConnector(mixer));
									NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
															   NEXUS_AudioMixer_GetConnector(mixer));
									NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
															  NEXUS_AudioMixer_GetConnector(mixer));


						}
						else if(dolby_args.ddc_enabled==true){
							printf("\n\n You should expect AC3 audio at the SPDIF output\n\n");
							NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
													   NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eCompressed));
							NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
												  NEXUS_AudioMixer_GetConnector(mixer));
							NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
														   NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eCompressed));


						}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/	


/*--------------------------------------------------------------------------------Start decoders----------------------------------------------------------------------------------------------------*/


  
   NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = capture;
    captureSettings.dataCallback.param = (int)pFile;
	printf("\n\n Starting the capture \n\n");
    NEXUS_AudioCapture_Start(capture, &captureSettings);

	
	
  for(i=0;i<no_of_streams;i++) {
    printf("\n\n\n Starting decoder:%d \n\n\n",i);
	rc = NEXUS_AudioDecoder_Start(audioDecoder[i], &audioProgram[i]);
			BDBG_ASSERT(!rc);

/*	if(compressedDecoder) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram[i]);
            BDBG_ASSERT(!rc);
        }*/
 
	NEXUS_AudioDecoder_GetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[i],audioProgram[i].codec,&dolby_args.codecSettings);
    if(file_params){
	parse_config_file(&dolby_args);
	}
	if(i==0) {
		dolby_args.codecSettings.codecSettings.ac3Plus.substreamId=0;
	}
	else if(i==1) {
		dolby_args.codecSettings.codecSettings.ac3Plus.substreamId=dolby_args.secondary_substreamId;
	}


	NEXUS_AudioDecoder_SetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	NEXUS_AudioDecoder_SetCodecSettings(audioDecoder[i],&dolby_args.codecSettings);

	NEXUS_AudioDecoder_GetSettings(audioDecoder[i],&dolby_args.decodeSettings);
	NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[i],audioProgram[i].codec,&dolby_args.codecSettings);

/*	print_settings(dolby_args.decodeSettings,dolby_args.codecSettings,&dolby_args);*/

	printf("\n\n---------------- The current values of the configuration parameters are as follows: ----------------- \n\n");
	printf("\n(1)OUTPUTMODE = %d\n(2)OUTPUTLFECHANNEL = %d\n(3)DUALMONO_MODE = %d\n(4)DOWNMIX_MODE = %d\n(5)RFMODE = %d\n(6)DRCCUT = %d\n(7)DRCBOOST = %d\n(8)DDC_ENABLED = %d\n(9)SUBSTREAMID = %d\n\n",dolby_args.decodeSettings.outputMode,dolby_args.decodeSettings.outputLfeMode,dolby_args.decodeSettings.dualMonoMode,dolby_args.codecSettings.codecSettings.ac3Plus.stereoDownmixMode,dolby_args.codecSettings.codecSettings.ac3Plus.drcMode,dolby_args.codecSettings.codecSettings.ac3Plus.cut,dolby_args.codecSettings.codecSettings.ac3Plus.boost,dolby_args.ddc_enabled,dolby_args.codecSettings.codecSettings.ac3Plus.substreamId);
	printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");

  
	NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
	if(opts[i].autoBitrate) {
		playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
	}
	rc = NEXUS_Playback_Start(playback[i], file[i], &playbackStartSettings);
	BDBG_ASSERT(!rc);

  }
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/  	

/*----------------------------------------------------------------------Managing the runtime change of the config parameters----------------------------------------------------------------------------------------------------------*/  	
   if(dolby_args.loopback==1) {
	for(exit=false;!exit;) {
        char buffer[256];
        char *buf;

        printf("Dolby_DigitalPlus>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        if(feof(stdin)) {
            break;
        }

        buffer[strlen(buffer)-1] = 0;

        buf = strtok(buffer, ";");
        if (!buf) continue;
#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_AcquireMutex(monitorState.lock);
        }
#endif
        do {
            if (!strcmp(buf, "?")) {
                printf(
                "Commands:\n"
                "  quit - quit\n"
                "  Parametername=value  - Change the value of any user configurable parameter\n"
                );
            }
			 else if (!strcmp(buf, "quit")) {
                exit = true;
                break;
            }
			else if ((strstr(buf, "OUTPUTMODE") != NULL)||(strstr(buf, "OUTPUTLFECHANNEL") != NULL)||(strstr(buf, "DUALMONO_MODE") != NULL)||(strstr(buf, "DOWNMIX_MODE") != NULL)||(strstr(buf, "RFMODE") != NULL)||(strstr(buf, "DRCCUT") != NULL)||(strstr(buf, "DRCBOOST") != NULL)||(strstr(buf, "DDC_ENABLED") != NULL)) {
				

				NEXUS_AudioDecoder_GetSettings(audioDecoder[0],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[0],audioProgram[0].codec,&dolby_args.codecSettings);
				NEXUS_AudioDecoder_GetSettings(audioDecoder[1],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[1],audioProgram[1].codec,&dolby_args.codecSettings);

                
				change_config(buf,&dolby_args);
					
                NEXUS_AudioDecoder_SetSettings(audioDecoder[0],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_SetCodecSettings(audioDecoder[0],&dolby_args.codecSettings);
				NEXUS_AudioDecoder_SetSettings(audioDecoder[1],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_SetCodecSettings(audioDecoder[1],&dolby_args.codecSettings);

				NEXUS_AudioDecoder_GetSettings(audioDecoder[0],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[0],audioProgram[0].codec,&dolby_args.codecSettings);
                NEXUS_AudioDecoder_GetSettings(audioDecoder[1],&dolby_args.decodeSettings);
				NEXUS_AudioDecoder_GetCodecSettings(audioDecoder[1],audioProgram[1].codec,&dolby_args.codecSettings);


				printf("\n\n After changing the values, the new values are:\n");
				print_settings(dolby_args.decodeSettings,dolby_args.codecSettings,&dolby_args);

        
        	}
            else if (!*buf) {
                /* allow blank line */
            }
            else {
                printf("unknown command: %s\n", buf);
            }
        }while ((buf = strtok(NULL, ";")));
#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_ReleaseMutex(monitorState.lock);
        }
#endif
    }
   }
    else{
	 #if B_HAS_PLAYBACK_MONITOR
					if(stickyFile) {
						BKNI_AcquireMutex(monitorState.lock);
					}
     #endif

	rc = NEXUS_Playback_GetStatus(playback[0], &pstatus);
	BDBG_ASSERT(!rc);
	while(pstatus.state!=2) {
		sleep(1);
		rc = NEXUS_Playback_GetStatus(playback[0], &pstatus);
		BDBG_ASSERT(!rc);
     }
    
    #if B_HAS_PLAYBACK_MONITOR
			if(stickyFile) {
				BKNI_ReleaseMutex(monitorState.lock);
			}
	#endif

  }
#if B_HAS_PLAYBACK_MONITOR
    if(stickyFile) {
        monitor_thread_stop(&monitorState);
    }
    
#endif
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/  	

/*----------------------------------------------------------------Stopping,Closing and Removing the Resources---------------------------------------------------------------------------------------*/  	
    NEXUS_Playback_Stop(playback[0]);
	NEXUS_Playback_Stop(playback[1]);

	fprintf(stderr, "Stopping capture\n");
	NEXUS_StopCallbacks(capture);
	NEXUS_AudioCapture_Stop(capture);

    NEXUS_AudioDecoder_Stop(audioDecoder[1]);
	NEXUS_AudioDecoder_Stop(audioDecoder[0]);
    /*if(compressedDecoder) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }*/

  
    NEXUS_Playback_CloseAllPidChannels(playback[0]);
	 NEXUS_Playback_CloseAllPidChannels(playback[1]);
    NEXUS_FilePlay_Close(file[0]);
	NEXUS_FilePlay_Close(file[1]);
    NEXUS_Playback_Destroy(playback[0]);
    NEXUS_Playpump_Close(playpump[0]);
	NEXUS_Playback_Destroy(playback[1]);
	NEXUS_Playpump_Close(playpump[1]);


    
	if(dolby_args.ddc_enabled==false) {
		NEXUS_AudioOutput_RemoveInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
						  NEXUS_AudioMixer_GetConnector(mixer));
			NEXUS_AudioOutput_RemoveInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
						  NEXUS_AudioMixer_GetConnector(mixer));
			NEXUS_AudioOutput_RemoveInput(NEXUS_AudioCapture_GetConnector(capture),
								   NEXUS_AudioMixer_GetConnector(mixer) );

}
else if(dolby_args.ddc_enabled==true){

	NEXUS_AudioOutput_RemoveInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
							   NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eCompressed));
	NEXUS_AudioOutput_RemoveInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
						  NEXUS_AudioMixer_GetConnector(mixer));
	NEXUS_AudioOutput_RemoveInput(NEXUS_AudioCapture_GetConnector(capture),
								   NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eCompressed) );
}

/*	if(compressedDecoder) {
        NEXUS_AudioOutput_RemoveInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }*/

	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder[0], NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder[0]);
	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder[1], NEXUS_AudioDecoderConnectorType_eStereo));
	NEXUS_AudioDecoder_Close(audioDecoder[1]);

   /* if(compressedDecoder) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        NEXUS_AudioDecoder_Close(compressedDecoder);
	}*/

	if(file_params) {
		 fclose(file_params);
	}
    NEXUS_Platform_Uninit();
#endif
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    return 0;
}

