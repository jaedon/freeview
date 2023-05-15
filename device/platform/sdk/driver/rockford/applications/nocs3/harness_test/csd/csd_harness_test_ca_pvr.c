
#include <bstd.h>
#include <stdio.h>
#include <string.h>
#include "bkni.h"
#include <pthread.h>


#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input.h"
#include "nexus_audio_input.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_security.h"
#include "nexus_recpump.h"
#include "nexus_dma.h"
#include "nexus_playpump.h"
#include "nexus_dma.h"

#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_ca_pvr.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"

#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output.h"
#endif

#ifndef CSD_HARNESS_TEST_CA_PVR_NEW_NEXUS_PLAYPUMP
	#include "bchp_xpt_fe.h"
	#if BCHP_XPT_FE_PID_TABLE_i_PLAYBACK_BAND_PARSER_PID_CHANNEL_INPUT_SELECT_MASK == 0x003f0000
		#define CSD_HARNESS_TEST_CA_PVR_PID_CHANNEL_START	64
	#elif BCHP_XPT_FE_PID_TABLE_i_PLAYBACK_BAND_PARSER_PID_CHANNEL_INPUT_SELECT_MASK == 0x000f0000
		#define CSD_HARNESS_TEST_CA_PVR_PID_CHANNEL_START	16
	#else
		#error "INTERNAL ERROR"
	#endif
#endif /* CSD_HARNESS_TEST_CA_PVR_NEW_NEXUS_PLAYPUMP */

#define CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
#ifdef CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
static void dmaCompleteCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle) context);
}
#endif

#define PLAYMUMP_INDEX 0
#define PLAYMUMP_INDEX_ALLPASS (PLAYMUMP_INDEX+1)
#define PLAYPUMP_TIMEOUT 3000 /* BKNI_INFINITE */

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("overflow %s\n", (const char *)context);
}

typedef struct
{
    NEXUS_RecpumpHandle recpump;
	volatile bool * pbRun;
	FILE * data_file;
    FILE *index_file;
    BKNI_EventHandle event;
	unsigned int MaxReadSize;
	unsigned int PacketSize;
	unsigned int ivSize;
	unsigned char ivArray[CSD_MAX_IV_SIZE];
    NEXUS_DmaHandle dma;
	NEXUS_KeySlotHandle hKey;
	NEXUS_DmaDataFormat dataFormat;
	bool bCounterMode;
	bool bEncryption;
}  RecordContext;

static void * DscRecordThread (void * pParam);
static void * ScrRecordThread (void * pParam);

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void dsc_message_callback(void *context, int param);
void dsc_message_thread(void *context);

typedef struct
{
    NEXUS_PlaypumpHandle playpump;
    NEXUS_VideoDecoderHandle videoDecoder;
	volatile bool * pbRun;
	FILE * data_file;
	FILE * out_file;
    BKNI_EventHandle event;
	unsigned int MaxReadSize;
	unsigned int PacketSize;
	unsigned int ivSize;
	unsigned char ivArray[CSD_MAX_IV_SIZE];
    NEXUS_DmaHandle dma;
	NEXUS_KeySlotHandle hKey;
	NEXUS_DmaDataFormat dataFormat;
	bool bEncryption;
	bool bCounterMode;

	/* This is required for supporting all pass */
	BKNI_MutexHandle hMutex;
	unsigned int chanID;
	bool bAllpass;

}  PlaybackContext;

typedef struct
{
	volatile bool * pbRun;
	unsigned int xEmi;
	NEXUS_MessageHandle hMsg;
	TCsdDscKeyPathHandle *pDscVideoKeyHandle;
	TCsdDscKeyPathHandle *pDscAudioKeyHandle;
	NOCS3_Harness_Test_Test_Vector * pVector;
    BKNI_EventHandle event;

	/* IV passed in from command line */
	unsigned int						sizeIV;
	unsigned char						iv[CSD_MAX_IV_SIZE];
}  DscMessageContext;


static void * DscPlaybackThread (void * pParam);
static void * ScrPlaybackThread (void * pParam);
static void * ScrPlaybackThread_Allpass (void * pParam);

static TCsdStatus R2R_UpdateIV(
	NEXUS_KeySlotHandle keyHandle,
  	TCsdUnsignedInt8* pxIvData, 
    TCsdSize xIvSize
	);

static TCsdStatus R2R_CryptoOperation(
	NEXUS_KeySlotHandle keyHandle,
  	TCsdUnsignedInt8* pxInputData, 
    TCsdUnsignedInt8* pxOutputData, 
    TCsdSize xDataSize,
	NEXUS_DmaHandle dma,
	BKNI_EventHandle dma_event,
    NEXUS_DmaDataFormat dmaFormat
	);

NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_ca_pvr (CSD_Harness_Test_CA_PVR_Configuration * pConfig)
{
    FILE *data_file=NULL;
	BKNI_EventHandle record_event=NULL;
    BKNI_EventHandle dsc_event=NULL;
	RecordContext recordContext;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    FILE *play_file=NULL;
    BKNI_EventHandle playback_event=NULL, playback_event_allpass=NULL;
	PlaybackContext playbackContext, playbackContext_allpass;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump=NULL, playpump_allpass=NULL;
	DscMessageContext dscMsgContext;
	BKNI_MutexHandle hMutex=NULL;

	NEXUS_MessageHandle hMsg = NULL;

    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand parserBand_record = NEXUS_ParserBand_e1;
    NEXUS_ParserBandSettings parserBandSettings;

    NEXUS_PidChannelHandle videoPidChannel = NULL, audioPidChannel = NULL, recordPidChannel = NULL;
	NEXUS_PlatformConfiguration platformConfig, *pNexusConfig = &platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window = NULL;
    NEXUS_VideoDecoderHandle vdecode = NULL;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder = NULL, compressedDecoder = NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelHandle stcChannel = NULL;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DmaHandle dma = NULL;
	bool bAllPass = false;
	volatile bool bRecording = false;
	volatile bool bPlayback = false;
	volatile bool bPlayback_allpass = false;
	volatile bool bEcm = false;


#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif


	NEXUS_KeySlotHandle hKey=NULL;

	NOCS3_HARNESS_TEST_STATUS_CODE nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

	if ( pConfig==NULL  )
	{
		CSD_OTHER_ERR_TRACE ("Invalid DSC CA/PCR test configuration info\n");
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
	}

	if ( pConfig->pOutFile ) 
	{
		bRecording = true;
	}
	else
	{
		bRecording = false;
	}
	data_file = pConfig->pOutFile;

	if ( pConfig->pInFile )
	{
		play_file = pConfig->pInFile;
		bPlayback = true;
	}
	else
	{
		bPlayback = false;
	}


	if ( pConfig->bIsDsc==false )
	{
		NEXUS_DmaSettings dmaSettings;
		if  ( (pConfig->bEncryption==true) && (pConfig->pScrKeyHandle->dataFormat==NEXUS_DmaDataFormat_eBlock) )
		{
			/* Allpass is only required for SCR RAW encryption */
			bAllPass = true;
		}

		if (( bRecording==true ) && (pConfig->bEncryption==false))
		{
			/* For SCR, Recording is only for encryption */
			bRecording = false;
		}

#if 0
		if ( pConfig->pInFile==NULL )
		{
			/* For SCR, if the input is from input band ,
			  then use Record Thread for descrambling */
			bRecording = true;
		}
#endif
		if ( pConfig->pScrKeyHandle==NULL )
		{
			CSD_OTHER_ERR_TRACE ("Invalid SCR keypath handle\n");
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
		}
		/* This is not clean.  To be improved later */
		hKey = pConfig->pScrKeyHandle->initHandle->rootHandleNode->handle;

		NEXUS_Dma_GetDefaultSettings (&dmaSettings);
#if(CFG_BIG_ENDIAN==1)
		dmaSettings.endianMode = NEXUS_DmaEndianMode_eBig; 
		dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;     
#endif
		dma = NEXUS_Dma_Open(0, &dmaSettings);
		if ( dma==NULL )
		{
			CSD_OTHER_ERR_TRACE("can't open DMA");
			return NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
		}
	}

	if ( bPlayback==false )
	{

		NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
		parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
		parserBandSettings.transportType = NEXUS_TransportType_eTs;
		NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
		
		if (bAllPass)
		{
			NEXUS_PidChannelSettings pidCfg0;

			NEXUS_ParserBand_GetSettings(parserBand_record, &parserBandSettings);
			parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
			NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
			parserBandSettings.transportType = NEXUS_TransportType_eTs;
			parserBandSettings.allPass=bAllPass;
			parserBandSettings.acceptNullPackets=bAllPass;
			NEXUS_ParserBand_SetSettings(parserBand_record, &parserBandSettings);

			NEXUS_PidChannel_GetDefaultSettings(&pidCfg0) ;
#ifndef CSD_HARNESS_TEST_CA_PVR_NEW_NEXUS_PLAYPUMP
			pidCfg0.pidChannelIndex = parserBand_record; /* for allpass record, pidChannelIndex = parserBand number */
			/* The pid number is redundant here. */
#else
			NEXUS_ParserBand_GetAllPassPidChannelIndex(parserBand_record, (unsigned *) &pidCfg0.pidChannelIndex);
#endif
			recordPidChannel=NEXUS_PidChannel_Open(parserBand_record, 0, &pidCfg0);
		}

		videoPidChannel=NEXUS_PidChannel_Open(parserBand, pConfig->videoPID, NULL);
		audioPidChannel=NEXUS_PidChannel_Open(parserBand, pConfig->audioPID, NULL);
	}
	else
	{

		BKNI_CreateEvent(&playback_event);
		playpump = NEXUS_Playpump_Open(PLAYMUMP_INDEX, NULL);
		NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
		playpumpSettings.dataCallback.callback = play_callback;
		playpumpSettings.dataCallback.context = playback_event;
		playpumpSettings.allPass = false;

		NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
		videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pConfig->videoPID, NULL);
		audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pConfig->audioPID, NULL);

		if (bAllPass)
		{
			NEXUS_PlaypumpOpenPidChannelSettings pidCfg0;
			BKNI_CreateEvent(&playback_event_allpass);

			BKNI_CreateMutex (&hMutex);

			bPlayback_allpass = true;

			playpump_allpass = NEXUS_Playpump_Open(PLAYMUMP_INDEX_ALLPASS, NULL);
			NEXUS_Playpump_GetSettings(playpump_allpass, &playpumpSettings);
			playpumpSettings.dataCallback.callback = play_callback;
			playpumpSettings.dataCallback.context = playback_event_allpass;
			playpumpSettings.allPass=true;
			playpumpSettings.acceptNullPackets=true;

			NEXUS_Playpump_SetSettings(playpump_allpass, &playpumpSettings);

			NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidCfg0);
#ifndef CSD_HARNESS_TEST_CA_PVR_NEW_NEXUS_PLAYPUMP
			pidCfg0.pidSettings.pidChannelIndex = CSD_HARNESS_TEST_CA_PVR_PID_CHANNEL_START + PLAYMUMP_INDEX_ALLPASS;
#else
			NEXUS_Playpump_GetAllPassPidChannelIndex(playpump_allpass, (unsigned *) &pidCfg0.pidSettings.pidSettings.pidChannelIndex );
#endif
			recordPidChannel = NEXUS_Playpump_OpenPidChannel(playpump_allpass, pConfig->videoPID, &pidCfg0);
			/* Open another playpump for all pass */

		}

	}

	if (pConfig->pDscVideoKeyHandle)
	{
    	NEXUS_PidChannelStatus pidStatus; 
		NEXUS_PidChannel_GetStatus(videoPidChannel, &pidStatus);
        pConfig->pDscVideoKeyHandle->pidchannel = pidStatus.pidChannelIndex;
	}
	if (pConfig->pDscAudioKeyHandle)
	{
    	NEXUS_PidChannelStatus pidStatus; 
		NEXUS_PidChannel_GetStatus(audioPidChannel, &pidStatus);
        pConfig->pDscAudioKeyHandle->pidchannel = pidStatus.pidChannelIndex;
	}

	if ( bRecording==true )
	{
		NEXUS_RecpumpOpenSettings recpumpOpenSettings;

		BKNI_CreateEvent(&record_event);

		NEXUS_Recpump_GetDefaultOpenSettings (&recpumpOpenSettings);
		recpumpOpenSettings.data.bufferSize = ((188/4)*4096*12*6) + 68;
		recpumpOpenSettings.index.bufferSize = 6*4*16*48*128;
		recpump = NEXUS_Recpump_Open(0, &recpumpOpenSettings);

		NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
		recpumpSettings.data.dataReady.callback = dataready_callback;
		recpumpSettings.data.dataReady.context = record_event;
		recpumpSettings.index.dataReady.callback = dataready_callback;
		recpumpSettings.index.dataReady.context = record_event;
		recpumpSettings.data.overflow.callback = overflow_callback;
		recpumpSettings.data.overflow.context = "data";
		recpumpSettings.index.overflow.callback = overflow_callback;
		recpumpSettings.index.overflow.context = "index";
		NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);
        
		NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
		addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
		addPidChannelSettings.pidTypeSettings.video.index = true;
		addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
		if ( bAllPass )
		{
			NEXUS_Recpump_AddPidChannel(recpump, 
				recordPidChannel, 
				&addPidChannelSettings);
		}
		else
		{
			NEXUS_Recpump_AddPidChannel(recpump, 
				videoPidChannel, 
				&addPidChannelSettings);
			NEXUS_Recpump_AddPidChannel(recpump, 
				audioPidChannel,
				NULL);
		}
	}

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
	if ( bPlayback==false )
	{
		stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
		stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
	}
	else
	{
		stcSettings.mode = NEXUS_StcChannelMode_eAuto; 
	}
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    /*audioProgram.codec = NEXUS_AudioCodec_eAc3;*/
    audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
	NEXUS_Platform_GetConfiguration(pNexusConfig);

    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(pNexusConfig->outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(pNexusConfig->outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(pNexusConfig->outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

	
    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);


    #if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(pNexusConfig->outputs.component[0]));
    #endif
    #if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(pNexusConfig->inputs.scart[0]));
    #if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(pNexusConfig->inputs.scart[1]));
    #endif
    #endif
    #if NEXUS_NUM_COMPOSITE_OUTPUTS
    if ( pNexusConfig->outputs.composite[0] ) {
        NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(pNexusConfig->outputs.composite[0]));
    }
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

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

	if ( pConfig->bEcmEnabled )
	{
		pthread_t msg_thread;
		NEXUS_MessageSettings settings;
		NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
	    NEXUS_PidChannelHandle ecmPidChannel; 
			
		BKNI_CreateEvent(&dsc_event);

	    /* Prepare XPT to recognize PMT. */
	    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
	    pidChannelSettings.requireMessageBuffer = true;
		if ( bPlayback==false )
		{
			ecmPidChannel = NEXUS_PidChannel_Open(parserBand, pConfig->ecmPID, &pidChannelSettings);
		}
		else
		{
			ecmPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pConfig->ecmPID, NULL);
		}

		memset(&dscMsgContext, 0, sizeof(dscMsgContext));
		dscMsgContext.xEmi = pConfig->emi;
		dscMsgContext.pVector = pConfig->pVector;
		dscMsgContext.pDscVideoKeyHandle = pConfig->pDscVideoKeyHandle;
		dscMsgContext.pDscAudioKeyHandle = pConfig->pDscAudioKeyHandle;
		dscMsgContext.sizeIV = pConfig->sizeIV;
		dscMsgContext.event = dsc_event;
		if ( dscMsgContext.sizeIV != 0 )
		{
			memcpy (dscMsgContext.iv, pConfig->iv, (dscMsgContext.sizeIV)*sizeof(unsigned char));
		}

    	NEXUS_Message_GetDefaultSettings(&settings);
    	settings.dataReady.callback = dsc_message_callback;
     	settings.dataReady.context = dsc_event;
    	settings.maxContiguousMessageSize = 4096;
    	hMsg = NEXUS_Message_Open(&settings);
    	BDBG_ASSERT(hMsg);

		dscMsgContext.hMsg = hMsg;
		bEcm = true;
		dscMsgContext.pbRun = &bEcm;
 		if (pthread_create(&msg_thread, NULL, dsc_message_thread, &dscMsgContext))
		{
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
			bRecording = false;
			goto cleanup;
		}
   	
		/* use the default filter for any data */
    	NEXUS_Message_GetDefaultStartSettings(hMsg, &startSettings);
    	startSettings.pidChannel = ecmPidChannel;
		startSettings.psfCrcDisabled = true;
    	if (NEXUS_Message_Start(hMsg, &startSettings)) 
		{
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
			goto cleanup;
		}
	}

	if ( bRecording )
	{
		pthread_t record_thread;

		memset(&recordContext, 0, sizeof(recordContext));
		recordContext.recpump = recpump;
		recordContext.pbRun = &bRecording;
		recordContext.data_file = data_file;
		recordContext.index_file = NULL;
		recordContext.event = record_event;
		recordContext.PacketSize = 188*16;		/* Multiple of 188 and 16 */
		recordContext.MaxReadSize = 1024*188; /* Hard code for now */
		recordContext.dma = dma;
		if ( pConfig->pScrKeyHandle )
		{
			recordContext.dataFormat = pConfig->pScrKeyHandle->dataFormat;
			recordContext.ivSize = pConfig->pScrKeyHandle->ivSize;
			if ( recordContext.ivSize )
			{
				memcpy (recordContext.ivArray, pConfig->pScrKeyHandle->ivArray, recordContext.ivSize*sizeof(unsigned char));
			}
			recordContext.hKey = hKey;
			recordContext.bEncryption = pConfig->bEncryption;
			recordContext.bCounterMode = pConfig->pScrKeyHandle->bCounterMode;
			if (pthread_create(&record_thread, NULL, ScrRecordThread, &recordContext))
			{
				nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
				bRecording = false;
				goto cleanup;
			}
		}
		else
		{
			if (pthread_create(&record_thread, NULL, DscRecordThread, &recordContext))
			{
				nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
				bRecording = false;
				goto cleanup;
			}
		}


	}

	if ( bPlayback )
	{
		pthread_t playback_thread;

		memset(&playbackContext, 0, sizeof(playbackContext));
		playbackContext.playpump = playpump;
		playbackContext.videoDecoder = vdecode;

		playbackContext.pbRun = &bPlayback;
		playbackContext.data_file = play_file;

		/* For encryption, data_file is used for saving encrypted result */
		playbackContext.out_file = ( pConfig->bIsDsc==true )? NULL:((pConfig->bEncryption==false)?data_file: NULL);
		playbackContext.event = playback_event;
		playbackContext.MaxReadSize = 1024*188; /* Hard code for now */
		playbackContext.PacketSize = 188*16;		/* Multiple of 188 and 16 */
		if ( pConfig->pScrKeyHandle )
		{
			playbackContext.dataFormat = pConfig->pScrKeyHandle->dataFormat;
			playbackContext.bCounterMode = pConfig->pScrKeyHandle->bCounterMode;
			if (pConfig->bEncryption) 
			{
				playbackContext.dma = NULL;
				playbackContext.hKey = NULL;
				playbackContext.ivSize = 0;
			}
			else
			{
				playbackContext.dma = dma;
				playbackContext.hKey = hKey;
				playbackContext.ivSize = pConfig->pScrKeyHandle->ivSize;
				if ( playbackContext.ivSize )
				{
					memcpy (playbackContext.ivArray, pConfig->pScrKeyHandle->ivArray, playbackContext.ivSize*sizeof(unsigned char));
				}
			}

			if (bAllPass)
			{
				pthread_t playback_thread_1;
				playbackContext.hMutex = hMutex;
				playbackContext.chanID = PLAYMUMP_INDEX;
				playbackContext.bAllpass = false;
				if (pthread_create(&playback_thread, NULL, ScrPlaybackThread_Allpass, &playbackContext))
				{
					nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
					goto cleanup;
				}

				memcpy (&playbackContext_allpass, &playbackContext, sizeof (playbackContext));
				playbackContext_allpass.playpump = playpump_allpass;
				playbackContext_allpass.event = playback_event_allpass;
				playbackContext_allpass.chanID = PLAYMUMP_INDEX_ALLPASS;
				playbackContext_allpass.pbRun = &bPlayback_allpass;
				playbackContext_allpass.bAllpass = true;
				if (pthread_create(&playback_thread_1, NULL, ScrPlaybackThread_Allpass, &playbackContext_allpass))
				{
					nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
					goto cleanup;
				}
			}
			else
			{
				if (pthread_create(&playback_thread, NULL, ScrPlaybackThread, &playbackContext))
				{
					nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
					goto cleanup;
				}
			}
		}
		else
		{
			if (pthread_create(&playback_thread, NULL, DscPlaybackThread, &playbackContext))
			{
				nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
				goto cleanup;
			}
		}


	}

	if ( bPlayback == true )
	{
		printf("Wait for playback to finish\n");
		do
		{
			BKNI_Sleep (1);
		} while ( bPlayback == true );

		do
		{
			BKNI_Sleep (50);
		} while ( bPlayback_allpass == true );
	}
	else
	{
		printf("Press ENTER to quit\n");
		getchar();
	}


cleanup:

	bRecording = false;
	bEcm = false;
	if (record_event)
		BKNI_SetEvent(record_event);

	/* Wait long enough for record pump to finish processing of leftover */
	BKNI_Sleep (300);
	if (playback_event)
		BKNI_SetEvent(playback_event);
	if (hMutex)
		BKNI_ReleaseMutex(hMutex);
	if (playback_event_allpass)
		BKNI_SetEvent(playback_event_allpass);
	if (dsc_event)
		BKNI_SetEvent(dsc_event);

	BKNI_Sleep (200);
    
	if ( pConfig->bEcmEnabled )
	{
    	NEXUS_Message_Stop(hMsg);

	}

    if ( bRecording==true )
		NEXUS_Recpump_Close(recpump);
	if (bPlayback==true)
	{
		NEXUS_Playpump_Close(playpump);
		if (playpump_allpass)
		{
			NEXUS_Playpump_Close(playpump_allpass);
		}
	}
	if (dma )
		NEXUS_Dma_Close(dma);
	
	if (record_event)
		BKNI_DestroyEvent(record_event);
	if (playback_event)
		BKNI_DestroyEvent(playback_event);
	if (playback_event_allpass)
		BKNI_DestroyEvent(playback_event_allpass);
	if (hMutex)
		BKNI_DestroyMutex(hMutex);
	if (dsc_event)
		BKNI_DestroyEvent(dsc_event);

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_AudioDecoder_Stop(compressedDecoder);
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(pNexusConfig->outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(pNexusConfig->outputs.spdif[0]));
	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);

    return 	nocs3_status;

}


static void * DscRecordThread (void * pParam)
{
	RecordContext * pContext = (RecordContext*)pParam;
	NEXUS_RecpumpHandle recpump=pContext->recpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * data_file = pContext->data_file;
 	unsigned int PacketSize = pContext->PacketSize;
	BKNI_EventHandle event = pContext->event;


#ifdef NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ Start DSC Recording $$$$$\n");
#endif


	NEXUS_Recpump_Start(recpump);
	while (*pbRun) 
	{
		void *data_buffer, *index_buffer;
		size_t data_buffer_size, index_buffer_size;
		int n;

		if (NEXUS_Recpump_GetDataBuffer(recpump, (const void **)&data_buffer, &data_buffer_size))
			break;
		if (NEXUS_Recpump_GetIndexBuffer(recpump, (const void **)&index_buffer, &index_buffer_size))
			break;
		if (data_buffer_size == 0 && index_buffer_size == 0) 
		{
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}
	       
		/*data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;*/
		if (data_buffer_size)
		{
			if ( data_file )
			{
				n = fwrite(data_buffer, 1, data_buffer_size, data_file);
				if (n < 0) 
					goto error;
			}
			else
			{
				n = data_buffer_size;
			}

			NEXUS_Recpump_DataReadComplete(recpump, n);
		}

		if (index_buffer_size)
		{
			NEXUS_Recpump_IndexReadComplete(recpump, index_buffer_size);
		}	
	}

error:
	printf ("$$$$$ Stop DSC recording $$$$$\n");
	NEXUS_Recpump_Stop(recpump);

	return NULL;

}

static void * ScrRecordThread (void * pParam)
{
	RecordContext * pContext = (RecordContext*)pParam;
	NEXUS_RecpumpHandle recpump=pContext->recpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * data_file = pContext->data_file;
 	unsigned int PacketSize = pContext->PacketSize;

	BKNI_EventHandle event = pContext->event;
	NEXUS_KeySlotHandle hKey = pContext->hKey;
	NEXUS_DmaHandle dma = pContext->dma;
    BKNI_EventHandle dma_event=NULL;
	NEXUS_DmaDataFormat dataFormat = pContext->dataFormat;
	register bool bCounterMode = pContext->bCounterMode;
	register bool bFirstTime = false;
 	unsigned int ivSize = pContext->ivSize;
	unsigned char iv[16];
	register bool bUpdateIV = ( (ivSize!=0) && (dataFormat==NEXUS_DmaDataFormat_eBlock) )? true: false;
	register bool bEncryption = pContext->bEncryption;

#ifdef NOCS3_DEBUG_MESSAGE
	unsigned int total_size = 0;
	printf ("$$$$$ Start SCR Recording bUpdateIV=%d  bEncryption=%d bCounterMode=%d $$$$$\n", bUpdateIV, bEncryption, bCounterMode);
#endif

	BKNI_CreateEvent(&dma_event);
	if ( (bUpdateIV==true) || (bCounterMode==true) )
	{
		memcpy(iv, pContext->ivArray, ivSize);
	}

	NEXUS_Recpump_Start(recpump);
	while (*pbRun) 
	{
		void *data_buffer, *index_buffer;
		size_t data_buffer_size, index_buffer_size;
		int n;

		if (NEXUS_Recpump_GetDataBuffer(recpump, (const void **)&data_buffer, &data_buffer_size))
			break;
		if (NEXUS_Recpump_GetIndexBuffer(recpump, (const void **)&index_buffer, &index_buffer_size))
			break;
		if (data_buffer_size == 0 && index_buffer_size == 0) 
		{
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		if ( ( *pbRun)== true )
		{
			data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;
		}

#ifdef NOCS3_DEBUG_MESSAGE
		total_size += data_buffer_size;
#endif
		if (data_buffer_size)
		{
			TCsdStatus status;

			if ( bUpdateIV ==true )
			{
				if ( bFirstTime == true )
				{
					bFirstTime = false;
				}
				else
				{
					status = R2R_UpdateIV (hKey, iv, ivSize);
					if ( status != CSD_NO_ERROR )
					{
						CSD_OTHER_ERR_TRACE ("SCR R2R Crypto IV Update failed\n");
						goto error;
					}					
				}
				if ( (bEncryption==false )&& (bCounterMode==false) )
				{
					memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
				}

				if ( bCounterMode )
				{
					nocs3_harness_test_utility_increase_16bytes_counter(iv, data_buffer_size>>4);
				}

			}

			status = R2R_CryptoOperation (hKey, 
								(TCsdUnsignedInt8*)data_buffer, 
								(TCsdUnsignedInt8*)data_buffer, 
								data_buffer_size,
								dma,
								dma_event,
								dataFormat);
			if ( status != CSD_NO_ERROR )
			{
				CSD_OTHER_ERR_TRACE ("SCR R2R Crypto operation failed\n");
				goto error;
			}

			if ( (bUpdateIV ==true) && (bEncryption==true) && (bCounterMode==false) )
			{
				/* Save IV */
				memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
			}
			if ( data_file )
			{
				n = fwrite(data_buffer, 1, data_buffer_size, data_file);
				if (n < 0) 
					goto error;
			}
			else
			{
				n = data_buffer_size;
			}

			NEXUS_Recpump_DataReadComplete(recpump, n);
		}

		if (index_buffer_size)
		{
			NEXUS_Recpump_IndexReadComplete(recpump, index_buffer_size);
		}	
	}

error:

#if NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ SCR recording - total_size is %d $$$$$\n", total_size);
	printf ("$$$$$ Stop SCR recording $$$$$\n");
#endif
	NEXUS_Recpump_Stop(recpump);
	if (dma_event)
		BKNI_DestroyEvent(dma_event);

	return NULL;

}


static void * DscPlaybackThread (void * pParam)
{
	PlaybackContext * pContext = (PlaybackContext*)pParam;
	NEXUS_PlaypumpHandle playpump=pContext->playpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * file = pContext->data_file;
	FILE * out_file = pContext->out_file;
    BKNI_EventHandle event = pContext->event;
	unsigned int MaxReadSize = pContext->MaxReadSize;
	unsigned int PacketSize = pContext->PacketSize;
	NEXUS_KeySlotHandle hKey = pContext->hKey;
    NEXUS_VideoDecoderHandle videoDecoder = pContext->videoDecoder;

#ifdef NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ Start DSC Playback $$$$$\n");
#endif

	NEXUS_Playpump_Start(playpump);

	fseek(file, 0, SEEK_SET);
	while (*pbRun) 
	{
		void *data_buffer;
		size_t data_buffer_size;
		int n;

		if (NEXUS_Playpump_GetBuffer(playpump, &data_buffer, &data_buffer_size))
			break;
		if (data_buffer_size == 0 ) 
		{
			if ( BKNI_WaitForEvent(event, PLAYPUMP_TIMEOUT) == BERR_SUCCESS )
			{
				continue;
			}
			else
			{
				/* Playpump hangs up */
				CSD_OTHER_ERR_TRACE ("Playpump hangs up\n");
				goto error;
			}
		}
	       
		/*data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;*/
		if (data_buffer_size > MaxReadSize)
			data_buffer_size = MaxReadSize;

		n = fread(data_buffer, 1, data_buffer_size, file);
		if (n < 0) 
			goto error;
		if (n == 0)  
		{

            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
			goto error;
		}
		else 
		{

			if ( out_file )
			{
				n = fwrite(data_buffer, 1, n, out_file);
				if (n < 0) 
					goto error;
			}
			NEXUS_Playpump_WriteComplete(playpump, 0, n);
		}
	}


error:
	printf ("$$$$$ Stop DSCPlayback $$$$$\n");
	NEXUS_Playpump_Stop(playpump);

	*pbRun = false;

	return NULL;

}

static void * ScrPlaybackThread (void * pParam)
{
	PlaybackContext * pContext = (PlaybackContext*)pParam;
	NEXUS_PlaypumpHandle playpump=pContext->playpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * file = pContext->data_file;
	FILE * out_file = pContext->out_file;
    BKNI_EventHandle event = pContext->event;
	unsigned int MaxReadSize = pContext->MaxReadSize;
	unsigned int PacketSize = pContext->PacketSize;
	NEXUS_KeySlotHandle hKey = pContext->hKey;
    NEXUS_VideoDecoderHandle videoDecoder = pContext->videoDecoder;
	NEXUS_DmaHandle dma = pContext->dma;
    BKNI_EventHandle dma_event=NULL;
	NEXUS_DmaDataFormat dataFormat = pContext->dataFormat;
	register bool bCrypto = ( (hKey!=NULL) && (dma!=NULL) )? true: false;
	register bool bCounterMode = pContext->bCounterMode;
	register bool bFirstTime = false;
 	unsigned int ivSize = pContext->ivSize;
	unsigned char iv[16];
	register bool bUpdateIV = ( (ivSize!=0) && (dataFormat==NEXUS_DmaDataFormat_eBlock) )? true: false;

#ifdef NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ Start SCR Playback IV size is % d DataFormat is %d bCounterMode is %d $$$$$\n", ivSize, dataFormat, bCounterMode);
#endif
	if (bCrypto)
	{
		BKNI_CreateEvent(&dma_event);
	}

	if ( (bUpdateIV==true) || (bCounterMode==true) )
	{
		memcpy(iv, pContext->ivArray, ivSize);
	}

	NEXUS_Playpump_Start(playpump);

	fseek(file, 0, SEEK_SET);
	while (*pbRun) 
	{
		void *data_buffer;
		size_t data_buffer_size;
		int n;

		if (NEXUS_Playpump_GetBuffer(playpump, &data_buffer, &data_buffer_size))
			break;
		if (data_buffer_size == 0 ) 
		{
			if ( BKNI_WaitForEvent(event, PLAYPUMP_TIMEOUT) == BERR_SUCCESS )
			{
				continue;
			}
			else
			{
				/* Playpump hangs up */
				CSD_OTHER_ERR_TRACE ("Playpump hangs up\n");
				goto error;
			}		
		}
	       
		data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;
		if (data_buffer_size > MaxReadSize)
			data_buffer_size = MaxReadSize;

		n = fread(data_buffer, 1, data_buffer_size, file);
		if (n < 0) 
			goto error;
		if (n == 0)  
		{
            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
			goto error;
		}
		else 
		{
			if (bCrypto)
			{
				TCsdStatus status;

				if ( bUpdateIV ==true )
				{
					if ( bFirstTime == true )
					{
						bFirstTime = false;
					}
					else
					{
						status = R2R_UpdateIV (hKey, iv, ivSize);
						if ( status != CSD_NO_ERROR )
						{
							CSD_OTHER_ERR_TRACE ("SCR R2R Crypto IV Update failed\n");
							goto error;
						}					
					}
					
					/* Save IV */
					if (bCounterMode)
					{
						nocs3_harness_test_utility_increase_16bytes_counter(iv, data_buffer_size>>4);
					}
					else
					{
						memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
					}
				}				
				
				status = R2R_CryptoOperation (hKey, 
									(TCsdUnsignedInt8*)data_buffer, 
									(TCsdUnsignedInt8*)data_buffer, 
									n,
									dma,
									dma_event,
									dataFormat);
				if ( status != CSD_NO_ERROR )
				{
					CSD_OTHER_ERR_TRACE ("SCR R2R Crypto operation failed\n");
					goto error;
				}
			}
			if ( out_file )
			{
				n = fwrite(data_buffer, 1, n, out_file);
				if (n < 0) 
					goto error;
			}
			NEXUS_Playpump_WriteComplete(playpump, 0, n);
		}
	}


error:
	printf ("$$$$$ Stop Playback $$$$$\n");
	NEXUS_Playpump_Stop(playpump);
	if (dma_event)
		BKNI_DestroyEvent(dma_event);

	*pbRun = false;

	return NULL;

}

/* This thread is only used for SCR encryption */
static void * ScrPlaybackThread_Allpass (void * pParam)
{
	PlaybackContext * pContext = (PlaybackContext*)pParam;
	NEXUS_PlaypumpHandle playpump=pContext->playpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * file = pContext->data_file;
    BKNI_EventHandle event = pContext->event;
	unsigned int MaxReadSize = pContext->MaxReadSize;
	unsigned int PacketSize = pContext->PacketSize;
    NEXUS_VideoDecoderHandle videoDecoder = pContext->videoDecoder;
	BKNI_MutexHandle hMutex = pContext->hMutex;
	unsigned int offset = 0;
	unsigned int chanID = pContext->chanID;
	bool bAllpass = pContext->bAllpass;

#ifdef NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ Start SCR Playback for Allpass $$$$$\n");
#endif
	NEXUS_Playpump_Start(playpump);

	while (*pbRun) 
	{
		void *data_buffer;
		size_t data_buffer_size;
		int n;

		if (NEXUS_Playpump_GetBuffer(playpump, &data_buffer, &data_buffer_size))
			break;
		if (data_buffer_size == 0 ) 
		{
			if ( BKNI_WaitForEvent(event, PLAYPUMP_TIMEOUT) == BERR_SUCCESS )
			{
				continue;
			}
			else
			{
				/* Playpump hangs up */
				CSD_OTHER_ERR_TRACE ("Playpump hangs up\n");
				goto error;
			}		
		}
	       
		data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;
		if (data_buffer_size > MaxReadSize)
			data_buffer_size = MaxReadSize;


		BKNI_AcquireMutex (hMutex);
		fseek(file, offset, SEEK_SET);
		n = fread(data_buffer, 1, data_buffer_size, file);
		BKNI_ReleaseMutex (hMutex);
		if (n < 0) 
			goto error;
		if (n == 0)  
		{
            while (1) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
			
			goto error;
		}
		else 
		{

			NEXUS_Playpump_WriteComplete(playpump, 0, n);
		}

		offset += n;
	}


error:
#ifdef NOCS3_DEBUG_MESSAGE
	printf ("$$$$$ Total bytes played %d $$$$$\n", offset);
	printf ("$$$$$ Stop Playback $$$$$\n");
#endif
	NEXUS_Playpump_Stop(playpump);

	*pbRun = false;

	return NULL;

}


static TCsdStatus R2R_UpdateIV(
	NEXUS_KeySlotHandle keyHandle,
  	TCsdUnsignedInt8* pxIvData, 
    TCsdSize xIvSize
	)
{
    NEXUS_SecurityClearKey key;

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
	key.keySize = 16;
	memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

	if ( xIvSize==8)
		BKNI_Memcpy(&(key.keyData[8]), pxIvData, sizeof(TCsdUnsignedInt8)*xIvSize);
	else
		BKNI_Memcpy(key.keyData, pxIvData, sizeof(TCsdUnsignedInt8)*xIvSize);

	/* Load IV. */
	if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
	{
    	CSD_OTHER_ERR_TRACE("\nLoad IV failed \n");
 		return CSD_NO_ERROR;
	}	

	return CSD_NO_ERROR;
}


static TCsdStatus R2R_CryptoOperation(
	NEXUS_KeySlotHandle keyHandle,
  	TCsdUnsignedInt8* pxInputData, 
    TCsdUnsignedInt8* pxOutputData, 
    TCsdSize xDataSize,
	NEXUS_DmaHandle dma,
	BKNI_EventHandle dma_event,
    NEXUS_DmaDataFormat dmaFormat
	)
{
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_Error errCode;
#ifndef CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
	BSTD_UNUSED(dma_event);
#endif

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = keyHandle;
	jobSettings.dataFormat = dmaFormat;
#ifdef CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
	jobSettings.completionCallback.callback = dmaCompleteCallback;
	jobSettings.completionCallback.context = (void *) dma_event;
#else
	jobSettings.completionCallback.callback = NULL;
	jobSettings.completionCallback.context = NULL;
#endif
	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = pxInputData;  
	blockSettings.pDestAddr = pxOutputData; 
	blockSettings.blockSize = xDataSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

	errCode = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
#ifdef CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
	if (errCode == NEXUS_DMA_QUEUED )
	{
		if (BKNI_WaitForEvent(dma_event, BKNI_INFINITE) ) 
		{
			NEXUS_DmaJob_Destroy (dmaJob);
			return CSD_NO_ERROR;
		}
	}
	else if ( errCode != NEXUS_SUCCESS )
	{
			NEXUS_DmaJob_Destroy (dmaJob);
			return CSD_ERROR;
	}
#else
	/* Wait for M2M Encryption to complete. */
	for(;;)
	{
		NEXUS_DmaJobStatus jobStatus;
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
		   break;
		}
		BKNI_Delay(1);
	}
#endif

	/* Done.  Close DMA. */
	NEXUS_DmaJob_Destroy(dmaJob);
	

    return CSD_NO_ERROR;
	

}


#define CSD_ECM_DATA_OFFSET 		7
#define CSD_REQUIRED_ECM_SIZE_FOR_8_BYTES_KEY	43
#define CSD_REQUIRED_ECM_SIZE_FOR_16_BYTES_KEY	59

typedef struct CSD_Dsc_Ecm
{
	unsigned int table_id;
	unsigned int keySize;

	unsigned char controlWord_1[16];
	unsigned char controlWord_2[16];

	unsigned int controlWord_1_index;
	unsigned int controlWord_2_index;

	bool bEncryptedContentKey_1;
	bool bEncryptedContentKey_2;
/*
	unsigned int crc[4];
*/
} CSD_Dsc_Ecm;

#define CSD_EVEN_ECM_TABLE_ID 0x80
#define CSD_ODD_ECM_TABLE_ID 0x81

#ifdef NOCS3_DEBUG_MESSAGE
static void DscDumpEcm (CSD_Dsc_Ecm* pEcm)
{
	printf("ECM is :\n");
	printf("table_id: %x\n", pEcm->table_id );
	printf("keySize: %d\n", pEcm->keySize );

	nocs3_harness_test_utility_dump_array(pEcm->controlWord_1, pEcm->keySize);
	nocs3_harness_test_utility_dump_array(pEcm->controlWord_2, pEcm->keySize);

	printf("controlWord_1_index: %d\n", pEcm->controlWord_1_index);
	printf("controlWord_2_index: %d\n", pEcm->controlWord_2_index);
	printf("bEncryptedContentKey_1: %d\n", pEcm->bEncryptedContentKey_1);
	printf("bEncryptedContentKey_2: %d\n", pEcm->bEncryptedContentKey_2);
}
#endif

static TCsdStatus DscParseEcm (unsigned int xEmi, unsigned char * pMsg, unsigned int sizeMsg, CSD_Dsc_Ecm* pEcm)
{
	unsigned int keySize, requiredEcmSize, offset;

	if ( ( pMsg[0] != CSD_EVEN_ECM_TABLE_ID ) && ( pMsg[0] != CSD_ODD_ECM_TABLE_ID ) )
	{
		return CSD_ERROR;
	}

	switch(xEmi)
	{
		case CSD_Algorithm_DVB_CSA2:
			keySize = 8;
			requiredEcmSize = CSD_REQUIRED_ECM_SIZE_FOR_8_BYTES_KEY;
			break;

		case CSD_Algorithm_DVB_CSA3:
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
		case CSD_Algorithm_AES_128_ECB_CLEAR:
		case CSD_Algorithm_AES_128_CBC_CLEAR:
			keySize = 16;
			requiredEcmSize = CSD_REQUIRED_ECM_SIZE_FOR_16_BYTES_KEY;
			break;
			
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
		case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
			keySize = 16;
			requiredEcmSize = CSD_REQUIRED_ECM_SIZE_FOR_16_BYTES_KEY;
			/*	dscCipheredProtectingKeysTable = &(dsc5CipheredProtectingKeysTable[global_nuid]);*/	
			break;
				
		default:
			keySize = 8;
			requiredEcmSize = CSD_REQUIRED_ECM_SIZE_FOR_8_BYTES_KEY;
			break;
		}

	if (requiredEcmSize>sizeMsg )
	{
		return CSD_ERROR;
	}

	offset = 0;
	pEcm->table_id = pMsg[offset];
	pEcm->keySize = keySize;

	offset = CSD_ECM_DATA_OFFSET;
	BKNI_Memcpy(pEcm->controlWord_1, pMsg+offset, sizeof(uint8_t)*keySize);
	offset += keySize;
	BKNI_Memcpy(pEcm->controlWord_2, pMsg+offset, sizeof(uint8_t)*keySize);
	offset += keySize;

	pEcm->controlWord_1_index = pMsg[offset++];
	pEcm->controlWord_2_index = pMsg[offset++];
	pEcm->bEncryptedContentKey_1 = pMsg[offset++];
	pEcm->bEncryptedContentKey_2 = pMsg[offset++];


	return CSD_NO_ERROR;

} 

#define CSD_HARNESS_TEST_LOAD_BOTH_KEYS_ON_ECM
void dsc_message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void dsc_message_thread(void *context)
{
	TCsdStatus status;
	uint8_t *buffer;
	DscMessageContext * pContext = (DscMessageContext*)context;
	NEXUS_MessageHandle hMsg = pContext->hMsg;
	size_t size;
	uint8_t table_id = 0;	
	
	const TCsdDscCipheredProtectingKeys dscCipheredProtectingKeysTable;
	TCsdDscKeyPathHandle *pDscAudioKeyHandle = pContext->pDscAudioKeyHandle;
	TCsdDscKeyPathHandle *pDscVideoKeyHandle = pContext->pDscVideoKeyHandle;
	NOCS3_Harness_Test_Test_Vector * pVector = pContext->pVector;
	CSD_Dsc_Ecm ecm;
	TCsdUnsignedInt8 * pIV; 
	unsigned int ivSize;
	bool bLoadEvenKey = false;
	bool bLoadOddKey = false;
	bool bFirstTime = true;
	bool bEvenKeyLadder = false;
	bool bOddKeyLadder = false;
	bool bEvenClearKey = false;
	bool bOddClearKey = false;
	unsigned char * pEvenKey, * pOddKey;
	unsigned int evenKeySize, oddKeySize;
	BKNI_EventHandle event = pContext->event;
	volatile bool * pbRun = pContext->pbRun;

	if (pContext->sizeIV !=0 )
	{
		ivSize = pContext->sizeIV;
		pIV = pContext->iv;
	}
	else
	{
		switch (pContext->xEmi)
		{
		case CSD_Algorithm_AES_128_CBC_CLEAR:
			ivSize = pVector->sizeIvCiPlus;
			pIV = pVector->IvCiPlus;
			break;
		default:
			ivSize = pVector->sizeIvCbc;
			pIV = pVector->IvCbc;
			break;
		}
	}
	
	if(pContext->xEmi == CSD_Algorithm_AES_128_CBC_CLEAR)
	{
		pDscVideoKeyHandle->initializationVector = pIV;
		pDscAudioKeyHandle->initializationVector = pIV;
	}	

	/* Copy protected keys */
	memcpy ((void*)&dscCipheredProtectingKeysTable[0][0], (void*)pVector->cipheredProtectingKeyL2, 16*sizeof(unsigned char));
	memcpy ((void*)&dscCipheredProtectingKeysTable[1][0], (void*)pVector->cipheredProtectingKeyL1, 16*sizeof(unsigned char));

	
	while (	* pbRun ) 
	{
		NEXUS_Message_GetBuffer(hMsg, (const void **)&buffer, &size); 
		
		if ((buffer==NULL) || (size==0) )
		{
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		memset (&ecm, 0, sizeof(ecm));
		if ( (table_id == buffer[0])||(( DscParseEcm (pContext->xEmi, buffer, size, &ecm)) != CSD_NO_ERROR ))
		{
			/* Same ECM or no valid ECM found */
			NEXUS_Message_ReadComplete(hMsg, size);
			continue;
		}

		/* We just got a new ECM */
#ifdef NOCS3_DEBUG_MESSAGE
		DscDumpEcm (&ecm);
#endif
		table_id = ecm.table_id;
		evenKeySize = oddKeySize = ecm.keySize;
		if ( table_id == CSD_EVEN_ECM_TABLE_ID )
		{
			pEvenKey = ecm.controlWord_1;
			pOddKey = ecm.controlWord_2;
			bEvenKeyLadder = (ecm.bEncryptedContentKey_1)? true: false;
			bEvenClearKey = (ecm.bEncryptedContentKey_1)? false: true;
			bOddKeyLadder = (ecm.bEncryptedContentKey_2)? true: false; 
			bOddClearKey = (ecm.bEncryptedContentKey_2)? false: true;
#ifdef CSD_HARNESS_TEST_LOAD_BOTH_KEYS_ON_ECM
			bLoadEvenKey = true;	
#else
			bLoadEvenKey = false;	/* Should have been loaded inthe previous crypto period */
#endif
			bLoadOddKey = true;		/* Previous ODD key expired */
		}
		else
		{
			pEvenKey = ecm.controlWord_2;
			pOddKey = ecm.controlWord_1;
			bEvenKeyLadder = (ecm.bEncryptedContentKey_2)? true: false;
			bEvenClearKey = (ecm.bEncryptedContentKey_2)? false: true;
			bOddKeyLadder = (ecm.bEncryptedContentKey_1)? true: false; 
			bOddClearKey = (ecm.bEncryptedContentKey_1)? false: true;
			bLoadEvenKey = true;	/* Previous EVEN key expired */
#ifdef CSD_HARNESS_TEST_LOAD_BOTH_KEYS_ON_ECM
			bLoadOddKey = true;		
#else
			bLoadOddKey = false;	/* Should have been loaded inthe previous crypto period */
#endif
		}


		if ( bFirstTime )
		{
			/* Very first ECM.  Load both keys */
			bLoadEvenKey = true;	
			bLoadOddKey = true;		

			bFirstTime = false;
		}

		if ( bLoadEvenKey == false )
		{
			pEvenKey = NULL;
			evenKeySize = 0;
		}
		if ( bLoadOddKey == false )
		{
			pOddKey = NULL;
			oddKeySize = 0;
		}
	
		bEvenKeyLadder = (bLoadEvenKey? bEvenKeyLadder:false);
		bOddKeyLadder = (bLoadOddKey? bOddKeyLadder:false);
		bEvenClearKey = (bLoadEvenKey? bEvenClearKey:false);
		bOddClearKey = (bLoadOddKey? bOddClearKey:false);

		if ( ( bEvenKeyLadder == true ) && ( bOddKeyLadder == true ) )
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Route both keys\n");
#endif
				/* EVEN key loadded using keyladder */
				/* Load Video key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
	       						pDscVideoKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}

				/* Load Audio key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
	       						pDscAudioKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}				
		}
		else if ( bEvenKeyLadder == true )
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Route EVEN key\n");
#endif
				/* EVEN key loadded using keyladder */
				/* Load Video key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)NULL, 
								0,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
	       						pDscVideoKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}

				/* Load Audio key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)NULL, 
								0,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
	       						pDscAudioKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}		
		}

		else if ( bOddKeyLadder == true )
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Route ODD key\n");
#endif
				/* ODD key loadded using keyladder */
				/* Load Video key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)NULL, 
								0,
	       						pDscVideoKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}

				/* Load Audio key */
				if( (status=csdSetProtectedDscContentKeys(
								pContext->xEmi, 
	     						dscCipheredProtectingKeysTable,
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)NULL, 
								0,
	       						pDscAudioKeyHandle
								))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetProtectedDscContentKeys", "Operation failed!"); 
				}		
		}

		if ( (bEvenClearKey==true) && (bOddClearKey==true) )
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Load both keys\n");
#endif
				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
								pDscVideoKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}

				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
								pDscAudioKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}
		}

		else if (bEvenClearKey==true)
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Load EVEN key\n");
#endif
				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								NULL, 
								0,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
								pDscVideoKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}

				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								NULL, 
								0,
								(TCsdUnsignedInt8*)pEvenKey, 
								evenKeySize,
								pDscAudioKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}
		}		
		else if (bOddClearKey==true)
		{
#ifdef NOCS3_DEBUG_MESSAGE
				printf("Load ODD key\n");
#endif
				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								NULL, 
								0,
								pDscVideoKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}

				if( (status=csdSetClearTextDscHostKeys(
								pContext->xEmi, 
								(TCsdUnsignedInt8*)pOddKey, 
								oddKeySize,
								NULL, 
								0,
								pDscAudioKeyHandle))!=CSD_NO_ERROR)
				{
        			CSD_ERR_TRACE(status, "csdSetClearTextDscHostKeys", "Operation failed!"); 
				}		
		}


		NEXUS_Message_ReadComplete(hMsg, size);
	}

	return;
}