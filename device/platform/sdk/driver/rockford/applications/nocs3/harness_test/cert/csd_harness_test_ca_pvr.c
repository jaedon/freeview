
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

#include "nocs3x_csd_impl.h"
#include "nocs3x_csd.h"
#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_ca_pvr.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "ca_cert.h"
#include "ca_cert_impl.h"

#include "cert_global.h"


#define CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
#ifdef CSD_HARNESS_TEST_M2M_USE_INTERRUPT_MODE
static void dmaCompleteCallback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle) context);
}
#endif

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
	DATA_IO *pIO; /* CFV */
	BKNI_EventHandle event;
	unsigned int MaxReadSize;
	unsigned int PacketSize;
	unsigned int ivSize;
	unsigned char ivArray[CSD_MAX_IV_SIZE];
	NEXUS_DmaHandle dma;
	NEXUS_KeySlotHandle hKey;
	NEXUS_DmaDataFormat dataFormat;
	bool bEncryption;
}  RecordContext;

static void * RecordThread (void * pParam);
static void * BufferRecordThread (void * pParam);

static void play_callback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}

void dsc_message_callback(void *context, int param);

typedef struct
{
	NEXUS_PlaypumpHandle playpump;
	NEXUS_VideoDecoderHandle videoDecoder;
	volatile bool * pbRun;
	FILE * data_file;
	FILE * out_file;
	DATA_IO *pIO; /* CFV */
	BKNI_EventHandle event;
	unsigned int MaxReadSize;
	unsigned int PacketSize;
	unsigned int ivSize;
	unsigned char ivArray[CSD_MAX_IV_SIZE];
	NEXUS_DmaHandle dma;
	NEXUS_KeySlotHandle hKey;
	NEXUS_DmaDataFormat dataFormat;
	bool bEncryption;
}  PlaybackContext;

typedef struct
{
	unsigned int xEmi;
	NEXUS_MessageHandle hMsg;
	TCsdDscKeyPathHandle *pDscVideoKeyHandle;
	TCsdDscKeyPathHandle *pDscAudioKeyHandle;
	TCertKeyPathHandle *pDscKeyHandle;
	NOCS3_Harness_Test_Test_Vector * pVector;

	/* IV passed in from command line */
	unsigned int						sizeIV;
	unsigned char						iv[CSD_MAX_IV_SIZE];
}  DscMessageContext;


static void * PlaybackThread (void * pParam);

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
	BKNI_EventHandle record_event = NULL;
	RecordContext recordContext;
	NEXUS_RecpumpSettings recpumpSettings;
	NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
	NEXUS_RecpumpHandle recpump = NULL;
	FILE *play_file=NULL;
	DATA_IO *pIO=pConfig->pIO;
	BKNI_EventHandle playback_event = NULL;
	PlaybackContext playbackContext;
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_PlaypumpHandle playpump = NULL;
	DscMessageContext dscMsgContext;

	NEXUS_MessageHandle hMsg = NULL;

	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
	NEXUS_ParserBandSettings parserBandSettings;

	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;

	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_StcChannelSettings stcSettings;

	pthread_t record_thread;
	pthread_t playback_thread;


	volatile bool bRecording;
	volatile bool bPlayback;
	bool bRecordingWasStarted=false;
	bool bPlaybackWasStarted=false;
	bool bpIOallocatedHere=false;

	long input_file_size;

	NEXUS_KeySlotHandle hKey=NULL;

	NOCS3_HARNESS_TEST_STATUS_CODE nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

	if ( pConfig==NULL  )
	{
		printf ("Invalid DSC CA/PCR test configuration info\n");
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
		fseek(play_file, 0, SEEK_END);
		input_file_size = ftell(play_file);
		fseek(play_file, 0, SEEK_SET);
		printf("Input file size: %ld\n", input_file_size);
		bPlayback = true;


		if ( input_file_size < 188*16 ) 
		{
			bRecording = true; /* note that in this case we will start recording even if no file to output was specified */
			if(!pIO)
				pIO = (DATA_IO*)malloc(sizeof(DATA_IO));
			else {
				printf("pIO already allocated!\n");
				nocs3_status = -1;
				goto cleanup;
			}

			if(pIO) 
			{
				memset(pIO, 0, sizeof(DATA_IO));
				if(!pIO->din)
					pIO->din = malloc(188*16);
				else
				{
					printf("pIO->din was alread allocated.\n");
					nocs3_status=-2;
					goto cleanup;
				}
				if(!pIO->dout)
					pIO->dout = malloc(188*16);
				else
				{
					printf("pIO->dout was alread allocated.\n");
					nocs3_status=-3;
					goto cleanup;
				}
					
				pIO->din_size = 188*16;
				pIO->dout_size = 188*16;

				if(pIO->din)
				{
					pIO->din_len = fread(pIO->din, 1, input_file_size, play_file);
				} else {
					printf("pIO->din not allocated\n");
					nocs3_status = -3;
					goto cleanup; 
				}

				bpIOallocatedHere = true;
			} else {
				printf(" pIO not allocated.\n");
				nocs3_status = -4;
				goto cleanup;
			}
		}
	}
	else
	{
		bPlayback = false;
	}

	if(pIO)
	{
		bPlayback = true;
		bRecording = true;
	}

	if ( pConfig->bIsDsc==false )
	{


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
			printf ("Invalid SCR keypath handle\n");
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
		}
		
		/* This is not clean.  To be improved later */
		hKey = pConfig->pScrKeyHandle->initHandle->rootHandleNode->handle;

		
	}
	if ( bPlayback==false )
	{
	
		NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
		parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
		parserBandSettings.transportType = NEXUS_TransportType_eTs;
		NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
		videoPidChannel=NEXUS_PidChannel_Open(parserBand, pConfig->videoPID, NULL);
		audioPidChannel=NEXUS_PidChannel_Open(parserBand, pConfig->audioPID, NULL);
	}
	else
	{	
	
		BKNI_CreateEvent(&playback_event);
		playpump = NEXUS_Playpump_Open(0, NULL);
		NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
		playpumpSettings.dataCallback.callback = play_callback;
		playpumpSettings.dataCallback.context = playback_event;
		playpumpSettings.allPass = false;

		NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
		videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pConfig->videoPID, NULL);
		audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pConfig->audioPID, NULL);
	}


	if(pConfig->pDscKeyHandle != NULL)
	{

		if (pConfig->pDscKeyHandle->dscVideoHandle)
		{
			NEXUS_PidChannelStatus pidStatus; 
			NEXUS_PidChannel_GetStatus(videoPidChannel, &pidStatus);
			pConfig->pDscKeyHandle->dscVideoHandle->initHandle->pidchannel = pidStatus.pidChannelIndex;
		}
	       	
		if (pConfig->pDscKeyHandle->dscAudioHandle)
		{
			NEXUS_PidChannelStatus pidStatus; 
			NEXUS_PidChannel_GetStatus(audioPidChannel, &pidStatus);
			pConfig->pDscKeyHandle->dscAudioHandle->initHandle->pidchannel = pidStatus.pidChannelIndex;
		}
		
	}


	
	
	
	
	/* Load a key from the CERT. */
	if( (pConfig->bEcmEnabled == false) && (pConfig->pDscKeyHandle != NULL) )
   	{
		TCertStatus useKeyStatus = CERT_ERROR;

		/* Indicate that the destination is for CA. */
		pConfig->pDscKeyHandle->dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		pConfig->pDscKeyHandle->keySlotEngine = NEXUS_SecurityEngine_eCa;
	
		if((pConfig->use_odd_parity)==TRUE)
		{
			useKeyStatus = certUseDecryptionKey(
				pConfig->emi, 
				pConfig->pDscKeyHandle,
				CERT_KEY_PARITY_ODD);
		}
		else
		{
			useKeyStatus = certUseDecryptionKey(
				pConfig->emi, 
				pConfig->pDscKeyHandle,
				CERT_KEY_PARITY_EVEN);
	        
	        }

		if( useKeyStatus != CERT_NO_ERROR ) {
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
			printf("certUse*Key failed with status=%d.\n", useKeyStatus);
			goto cleanup;
		}
	}
	
#if 0	
	else
	{
		if( (pCsvFile  = fopen( csv_file, "r" )) == NULL )
		{
			perror(csv_file);
			return;
		}
		/* Parse csv file and store the commands for the cert . */
		parseCertCsvFile(pCsvFile, keyPathHandle.certIo);
	            
	}	
#endif
	
	if ( bRecording==true )
	{

	
		BKNI_CreateEvent(&record_event);
		recpump = NEXUS_Recpump_Open(0, NULL);
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
		NEXUS_Recpump_AddPidChannel(recpump, 
					    videoPidChannel, 
					    &addPidChannelSettings);
		NEXUS_Recpump_AddPidChannel(recpump, 
					    audioPidChannel,
					    NULL);
		
	}

	/* Open the StcChannel to do lipsync with the PCR */
	/* NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings); */
	NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
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

	
	/* stcChannel = NEXUS_StcChannel_Open(0, &stcSettings); */
	NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);

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
		NEXUS_MessageSettings settings;
		NEXUS_MessageStartSettings startSettings;
		NEXUS_PidChannelSettings pidChannelSettings;
		NEXUS_PidChannelHandle ecmPidChannel; 
			
		/* Prepare XPT to recognize PMT. */
		NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
		pidChannelSettings.requireMessageBuffer = true;
		ecmPidChannel = NEXUS_PidChannel_Open(parserBand, pConfig->ecmPID, &pidChannelSettings);
		
		memset(&dscMsgContext, 0, sizeof(dscMsgContext));
		dscMsgContext.xEmi = pConfig->emi;
		dscMsgContext.pVector = pConfig->pVector;
		dscMsgContext.pDscVideoKeyHandle = pConfig->pDscKeyHandle->dscVideoHandle;
		dscMsgContext.pDscAudioKeyHandle = pConfig->pDscKeyHandle->dscAudioHandle;
		dscMsgContext.pDscKeyHandle = pConfig->pDscKeyHandle;
		dscMsgContext.sizeIV = pConfig->sizeIV;
		if ( dscMsgContext.sizeIV != 0 )
		{
			memcpy (dscMsgContext.iv, pConfig->iv, (dscMsgContext.sizeIV)*sizeof(unsigned char));
		}
		
		NEXUS_Message_GetDefaultSettings(&settings);
		settings.dataReady.callback = dsc_message_callback;
		settings.dataReady.context = &dscMsgContext;
		settings.maxContiguousMessageSize = 4096;
		hMsg = NEXUS_Message_Open(&settings);
		BDBG_ASSERT(hMsg);

		dscMsgContext.hMsg = hMsg;
		NEXUS_Message_GetDefaultStartSettings(hMsg, &startSettings);
		startSettings.pidChannel = ecmPidChannel;
		startSettings.psfCrcDisabled = true;

		/* use the default filter for any data */
		if (NEXUS_Message_Start(hMsg, &startSettings)) 
		{
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
			printf("NEXUS_Message_Start failed.\n");
			goto cleanup;
		}
	}

	if ( bRecording )
	{

		memset(&recordContext, 0, sizeof(recordContext));
		recordContext.recpump = recpump;
		recordContext.pbRun = &bRecording;
		recordContext.data_file = data_file;
		recordContext.pIO = pIO;
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
		}
		else
		{
			playbackContext.dataFormat = NEXUS_DmaDataFormat_eMpeg;
		}

		if( pIO ) {
			if (pthread_create(&record_thread, NULL, BufferRecordThread, &recordContext))
			{
				nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
				printf("Could not create buffer record_thread.\n");
				goto cleanup;
			}
		} 
		else 
		{
			if (pthread_create(&record_thread, NULL, RecordThread, &recordContext))
			{
				nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
				printf("Could not create record_thread.\n");
				goto cleanup;
			}
		}
		bRecordingWasStarted=true;
	}

	if ( bPlayback )
	{

		memset(&playbackContext, 0, sizeof(playbackContext));
		playbackContext.playpump = playpump;
		playbackContext.videoDecoder = vdecode;

		playbackContext.pbRun = &bPlayback;

		if( input_file_size < 188*16 )
			playbackContext.data_file = NULL; /* input is in pIO buffer */
		else 
			playbackContext.data_file = play_file;

		/* For encryption, data_file is used for saving encrypted result */
		playbackContext.out_file = (pConfig->bIsDsc)?NULL:((pConfig->bEncryption==false)?data_file: NULL);
		playbackContext.pIO = pIO; /* CFV */
		playbackContext.event = playback_event;
		playbackContext.MaxReadSize = 1024*188; /* Hard code for now */
		playbackContext.PacketSize = 188*16;		/* Multiple of 188 and 16 */
		if ( pConfig->pScrKeyHandle )
		{
			playbackContext.dataFormat = pConfig->pScrKeyHandle->dataFormat;
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
		}
		else
		{
			playbackContext.dataFormat = NEXUS_DmaDataFormat_eMpeg;
		}

		if (pthread_create(&playback_thread, NULL, PlaybackThread, &playbackContext))
		{
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
			printf("Could not create playback_thread.\n");
			goto cleanup;
		}
		bPlaybackWasStarted=true;

	}


	if ( bPlaybackWasStarted )
	{
		if(bRecordingWasStarted) {
			printf("Wait for playback or recording to finish\n");
			do
			{
				BKNI_Sleep (100);
			} while ( bPlayback && bRecording );
		} else {
			printf("Wait for playback to finish\n");
			do
			{
				BKNI_Sleep (100);
			} while ( bPlayback );
		}
	}
	else
	{ /* this is the case where data comes from frontend. We have no way of knowing when it ends */
		printf("Press ENTER to quit\n");
		getchar();
	}


cleanup:
	printf("csd_harness_test_ca_pvr: cleanup\n");

	/* both rec and play threads are told to stop at the next chance they got to their main loop */
	bPlayback = false; 
	bRecording = false;

	
	NEXUS_VideoDecoder_Stop(vdecode);
	NEXUS_AudioDecoder_Stop(compressedDecoder);
	NEXUS_AudioDecoder_Stop(pcmDecoder);

	NEXUS_AudioOutput_RemoveAllInputs( NEXUS_SpdifOutput_GetConnector(pNexusConfig->outputs.spdif[0]) );

	if(bpIOallocatedHere)
	{
		if(pIO->din)
			free(pIO->din);
		if(pIO->dout)
			free(pIO->dout);
		free(pIO);		
	}
		
	if(record_event)
		BKNI_SetEvent(record_event);

	if(recpump)
	{
		NEXUS_Recpump_Stop(recpump);
		NEXUS_Recpump_RemoveAllPidChannels(recpump);
		NEXUS_Recpump_Close(recpump);
	}

	if(playback_event)
		BKNI_SetEvent(playback_event);

	if(playpump)
	{
		NEXUS_Playpump_Stop(playpump);
		NEXUS_Playpump_CloseAllPidChannels(playpump);
		NEXUS_Playpump_Close(playpump);
	}
	
	if ( bPlaybackWasStarted ) 
	{
		int res;
		res = pthread_cancel(playback_thread);
		/* printf("playback thread canceled res=%d\n", res); */
		printf("Joining playback_thread\n");
		pthread_join(playback_thread, NULL);
	}
	if ( bRecordingWasStarted ) 
	{
		/* int res; */
		/* pthread_cancel(record_thread); */
		printf("Joining record_thread\n");
		pthread_join(record_thread, NULL);
	}

	/* BKNI_Sleep (100); */
	
	if ( pConfig->bEcmEnabled )
	{
		NEXUS_Message_Stop(hMsg);
	}


	/* if (bRecording==true) */
	/* 	NEXUS_Recpump_Close(recpump); */
	/* if (bPlayback==true) */
	/* 	NEXUS_Playpump_Close(playpump); */
	/* if (dma ) */
	/* 	NEXUS_Dma_Close(dma); */

	/* Bring down system */
	/* NEXUS_StcChannel_Close(stcChannel); */

	return 	nocs3_status;

}


static void * RecordThread (void * pParam)
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
	register bool bCrypto = ( (hKey!=NULL) && (dma!=NULL) )? true: false;
	register bool bFirstTime = false;
 	unsigned int ivSize = pContext->ivSize;
	unsigned char iv[16];
	register bool bUpdateIV = ( (ivSize!=0) && (dataFormat==NEXUS_DmaDataFormat_eBlock) )? true: false;
	register bool bEncryption = pContext->bEncryption;

	printf ("$$$$$ Start Recording bCrypto=%d bUpdateIV=%d  bEncryption=%d $$$$$\n", bCrypto, bUpdateIV, bEncryption);

	if (bCrypto)
	{
		BKNI_CreateEvent(&dma_event);
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
	       
		data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;

		/* printf("Record thread: data_buffer_size=%d\n", data_buffer_size); */

		if (data_buffer_size)
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
							printf ("SCR R2R Crypto IV Update failed\n");
							goto error;
						}					
					}
					if ( bEncryption==false )
					{
						/* Save IV */
						memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
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
					printf ("SCR R2R Crypto operation failed\n");
					goto error;
				}

				if ( (bUpdateIV ==true) && (bEncryption==true) )
				{
					/* Save IV */
					memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
				}
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
	*pbRun = false; 
	/* printf ("$$$$$ Stop recording $$$$$\n"); */
	/* NEXUS_Recpump_Stop(recpump); */
	if (dma_event)
		BKNI_DestroyEvent(dma_event);

	printf("$$$$$ Buffer Thread Exiting $$$$$\n");
	pthread_exit(NULL);

	return NULL;

}

static void * PlaybackThread (void * pParam)
{
	PlaybackContext * pContext = (PlaybackContext*)pParam;
	NEXUS_PlaypumpHandle playpump=pContext->playpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * in_file = pContext->data_file;
	FILE * out_file = pContext->out_file;
	DATA_IO *pIO = pContext->pIO;
	BKNI_EventHandle event = pContext->event;
	unsigned int MaxReadSize = pContext->MaxReadSize;
	unsigned int PacketSize = pContext->PacketSize;
	unsigned int already_read, left_to_read;
	NEXUS_KeySlotHandle hKey = pContext->hKey;
	NEXUS_VideoDecoderHandle videoDecoder = pContext->videoDecoder;
	NEXUS_DmaHandle dma = pContext->dma;
	BKNI_EventHandle dma_event=NULL;
	NEXUS_DmaDataFormat dataFormat = pContext->dataFormat;
	register bool bCrypto = ( (hKey!=NULL) && (dma!=NULL) )? true: false;
	register bool bFirstTime = false;
 	unsigned int ivSize = pContext->ivSize;
	unsigned char iv[16];
	register bool bUpdateIV = ( (ivSize!=0) && (dataFormat==NEXUS_DmaDataFormat_eBlock) )? true: false;
	unsigned int remain_to_save;
	unsigned int out_buffer_filled, out_buffer_available;



	already_read = 0;
	if( pIO )
	{
		if (out_file)
		{
			remain_to_save = pIO->din_len;
		}
		left_to_read = pIO->din_len;

		if (pIO->dout)
		{
			out_buffer_filled = 0;
			out_buffer_available = (pIO->din_len>pIO->dout_size)? pIO->dout_size:pIO->din_len;
		}
	}

	printf ("$$$$$ Start Playback IV size is % d DataFormat is %d $$$$$\n", ivSize, dataFormat);
	if (bCrypto)
	{
		BKNI_CreateEvent(&dma_event);
	}

	NEXUS_Playpump_Start(playpump);

	if( in_file )
		fseek(in_file, 0, SEEK_SET);
	while (*pbRun) 
	{
		void *data_buffer;
		size_t data_buffer_size;
		int n;

		if (NEXUS_Playpump_GetBuffer(playpump, &data_buffer, &data_buffer_size))
			break;
		if (data_buffer_size == 0 ) 
		{
			/* printf("playback thread data_buffer_size==0. Waiting...\n"); */
			/* BKNI_WaitForEvent(event, 100000); /\* CFV: wait 100000ms instead of BKNI_INFINITE); *\/ */
			continue;
		}
	       
		data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;
		if (data_buffer_size > MaxReadSize)
			data_buffer_size = MaxReadSize;

		/* printf("Playback thread: data_buffer_size=%d\n", data_buffer_size); */

		if( in_file )
			n = fread(data_buffer, 1, data_buffer_size, in_file);
		else if (pIO) {
			if ( pIO->din ) {
				/* CFV */
				
				if( left_to_read > data_buffer_size )
				{
					memcpy(data_buffer, pIO->din+already_read, data_buffer_size);
					left_to_read -= data_buffer_size;
				}
				else 
				{
					int i;
					int runs, left_over;
					unsigned char * pDataBuffer;

					memcpy(data_buffer, pIO->din+already_read, left_to_read);

					pDataBuffer = (U8*)data_buffer + left_to_read;
					runs=(data_buffer_size-left_to_read)/pIO->din_len;
					left_over=(data_buffer_size-left_to_read)%pIO->din_len;
					for (i=0;i<runs;i++)
					{
						memcpy(pDataBuffer, pIO->din, pIO->din_len);
						pDataBuffer += pIO->din_len;
					}
					memcpy(pDataBuffer, pIO->din, left_over );
					already_read = left_over;
					left_to_read = pIO->din_len - already_read;


				}
				n = data_buffer_size;				
				
			}
		}
		if (n < 0 )
			goto error;
		if( n == 0 ) {
			printf("Playback thread n==0\n");
			while (1) {
				NEXUS_VideoDecoderStatus status;
				NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
				if (!status.queueDepth) break;
				if (*pbRun==false) break;
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
							printf ("SCR R2R Crypto IV Update failed\n");
							goto error;
						}					
					}
					
					/* Save IV */
					memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
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
					printf ("SCR R2R Crypto operation failed\n");
					goto error;
				}
			
				if ( out_file )
				{
					if (pIO==NULL)
						n = fwrite(data_buffer, 1, n, out_file); /* note that nsave=n in most cases except when the first packet is smaller than PacketSize */
					else if (remain_to_save>0)
					{
						if ( remain_to_save > n )
						{
							fwrite (data_buffer, 1, n, out_file);
							remain_to_save -= n;
						}
						else
						{
							fwrite (data_buffer, 1, remain_to_save, out_file);
							remain_to_save = 0;
							break;
						}
					}
				
					if (n < 0) 
						goto error;
				}
				if (pIO) {
					if( pIO->dout ) {
#if 0
						nsave = (nsave < pIO->dout_size)? nsave : pIO->dout_size;
						memcpy(pIO->dout, data_buffer, nsave);
						pIO->dout_len = nsave;
						if (nsave < 0)
							goto error;
#else
						if ( out_buffer_available > n )
						{
							memcpy(pIO->dout+out_buffer_filled, data_buffer, n);
							out_buffer_filled += n;
							out_buffer_available -= n;
							pIO->dout_len = out_buffer_filled;
						}
						else
						{
							memcpy(pIO->dout+out_buffer_filled, data_buffer, out_buffer_available);
							out_buffer_filled += out_buffer_available;
							out_buffer_available = 0;
							pIO->dout_len = out_buffer_filled;
							break;
						}
						
#endif
					}
				}
			      
			} /* end of bCrypto */
			NEXUS_Playpump_WriteComplete(playpump, 0, n); /* always acknowledge the total number of bytes sent to the playpump */
		}
	}


error:
	*pbRun = false; 
	/* printf ("$$$$$ Stop Playback $$$$$\n"); */
	/* NEXUS_Playpump_Stop(playpump); */
	if (dma_event)
		BKNI_DestroyEvent(dma_event);
	printf("$$$$$ Playback Thread Exiting $$$$$\n");
	pthread_exit(NULL);
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
		printf("\nLoad IV failed \n");
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
/*				
				case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
				case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
				keySize = 16;
				dscCipheredProtectingKeysTable = &(dsc5CipheredProtectingKeysTable[global_nuid]);
				break;
*/				
	default:
		keySize = 8;
		requiredEcmSize = CSD_REQUIRED_ECM_SIZE_FOR_8_BYTES_KEY;
		break;
	}

#if 0
	if (requiredEcmSize>sizeMsg )
	{
		return CSD_ERROR;
	}
#endif

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

void dsc_message_callback(void *context, int param)
{
	uint8_t *buffer;
	DscMessageContext * pContext = (DscMessageContext*)context;
	NEXUS_MessageHandle hMsg = pContext->hMsg;
	size_t size;
	uint8_t table_id = 0;	
	
	TCertKeyPathHandle *pDscVideoKeyHand = pContext->pDscKeyHandle;
	NOCS3_Harness_Test_Test_Vector * pVector = pContext->pVector;
	CSD_Dsc_Ecm ecm;
	TCsdUnsignedInt8 * pIV; 
	unsigned int ivSize;
	bool bLoadEvenKey = false;
	bool bLoadOddKey = false;
	bool bFirstTime = true;
	int bo=0; /* nb bytes written in xpOutBuf */
	TCertResourceHandle certRessource;
	unsigned int oddKeyIndex;
	unsigned int evenKeyIndex;
	size_t numOfProcessedCmds;
	unsigned int i;
    
	BSTD_UNUSED(param);
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
		pDscVideoKeyHand->dscVideoHandle->initializationVector = pIV;
		pDscVideoKeyHand->dscAudioHandle->initializationVector = pIV;
	}	

	
	while (1) 
	{
		/* wait 5 seconds */	
		if (NEXUS_Message_GetBuffer(hMsg, (const void **)&buffer, &size)) 
			continue;
		
		if (!buffer)
			continue;

		memset (&ecm, 0, sizeof(ecm));
		if ( (table_id == buffer[0])||(( DscParseEcm (pContext->xEmi, buffer, size, &ecm)) != CSD_NO_ERROR ))
		{
			/* Same ECM or no valid ECM found */
			NEXUS_Message_ReadComplete(hMsg, size);
			continue;
		}

		/* We just got a new ECM */
		/* DscDumpEcm (&ecm);*/

		table_id = ecm.table_id;

		if ( table_id == CSD_EVEN_ECM_TABLE_ID )
		{
			evenKeyIndex = ecm.controlWord_1_index;
			oddKeyIndex = ecm.controlWord_2_index;
            
			bLoadEvenKey = false;	/* Should have been loaded inthe previous crypto period */
			bLoadOddKey = true;		/* Previous ODD key expired */
		}
		else
		{
			evenKeyIndex = ecm.controlWord_2_index;
			oddKeyIndex = ecm.controlWord_1_index;
            
			bLoadEvenKey = true;	/* Previous EVEN key expired */
			bLoadOddKey = false;	/* Should have been loaded inthe previous crypto period */
		}

		if(bFirstTime)
		{
			bLoadEvenKey = true;
			bLoadOddKey = true;
			bFirstTime = false;
		}

		/* Indicate that the destination is for CA. */
		pContext->pDscKeyHandle->dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		pContext->pDscKeyHandle->keySlotEngine = NEXUS_SecurityEngine_eCa;


		if(bLoadOddKey==TRUE)
		{
			/* Search for odd control word index. */
            
			for(i=0;i++;i<32)
				if(pDscVideoKeyHand->certIo[i].cwIndex == oddKeyIndex)
					break;

			certLock(&certRessource);
            
			certExchange(certRessource, 
				     pDscVideoKeyHand->certIo[i].numOfCommands,  
				     pDscVideoKeyHand->certIo[i].cert_cmd, 
				     &numOfProcessedCmds);
            
			certUnlock(certRessource);
            
			pContext->pDscKeyHandle->bLoadVideo=true;
    
			certUseDecryptionKey(
				pContext->xEmi, 
				pContext->pDscKeyHandle,
				CERT_KEY_PARITY_ODD);

			pContext->pDscKeyHandle->bLoadVideo=false;

			certUseDecryptionKey(
				pContext->xEmi, 
				pContext->pDscKeyHandle,
				CERT_KEY_PARITY_ODD);
		}
		if(bLoadEvenKey==TRUE)
		{

			/* Search for odd control word index. */

			for(i=0;i++;i<32)
				if(pDscVideoKeyHand->certIo[i].cwIndex == evenKeyIndex)
					break;

			certLock(&certRessource);
            
			certExchange(certRessource, 
				     pDscVideoKeyHand->certIo[i].numOfCommands,  
				     pDscVideoKeyHand->certIo[i].cert_cmd, 
				     &numOfProcessedCmds);
            
			certUnlock(certRessource);

			pContext->pDscKeyHandle->bLoadVideo=true;
        
			certUseDecryptionKey(
				pContext->xEmi, 
				pContext->pDscKeyHandle,
				CERT_KEY_PARITY_EVEN);

			pContext->pDscKeyHandle->bLoadVideo=false;

			certUseDecryptionKey(
				pContext->xEmi, 
				pContext->pDscKeyHandle,
				CERT_KEY_PARITY_EVEN);
		}

		NEXUS_Message_ReadComplete(hMsg, size);
	}

	return;
}

static void * BufferRecordThread (void * pParam)
{
	RecordContext * pContext = (RecordContext*)pParam;
	NEXUS_RecpumpHandle recpump=pContext->recpump;
	volatile bool * pbRun = pContext->pbRun;
	FILE * out_file = pContext->data_file;
	DATA_IO *pIO = pContext->pIO;
 	unsigned int PacketSize = pContext->PacketSize;
	unsigned int available, filled;

	BKNI_EventHandle event = pContext->event;
	NEXUS_KeySlotHandle hKey = pContext->hKey;
	NEXUS_DmaHandle dma = pContext->dma;
	BKNI_EventHandle dma_event=NULL;
	NEXUS_DmaDataFormat dataFormat = pContext->dataFormat;
	register bool bCrypto = ( (hKey!=NULL) && (dma!=NULL) )? true: false;
	register bool bFirstTime = false;
 	unsigned int ivSize = pContext->ivSize;
	unsigned char iv[16];
	register bool bUpdateIV = ( (ivSize!=0) && (dataFormat==NEXUS_DmaDataFormat_eBlock) )? true: false;
	register bool bEncryption = pContext->bEncryption;


	if(pIO)
		available = pIO->din_len;
	else
		available = 0;
	filled = 0;


	printf ("$$$$$ Start Buffer Recording bCrypto=%d bUpdateIV=%d  bEncryption=%d $$$$$\n", bCrypto, bUpdateIV, bEncryption);

	if (bCrypto)
	{
		BKNI_CreateEvent(&dma_event);
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
	       
		data_buffer_size = (data_buffer_size/PacketSize)*PacketSize;

		/* printf("Buffer Record Thread: data_buffer_size=%d\n", data_buffer_size); */
		fflush(stdout);

		if (data_buffer_size)
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
							printf ("SCR R2R Crypto IV Update failed\n");
							goto error;
						}					
					}
					if ( bEncryption==false )
					{
						/* Save IV */
						memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
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
					printf ("SCR R2R Crypto operation failed\n");
					goto error;
				}

				if ( (bUpdateIV ==true) && (bEncryption==true) )
				{
					/* Save IV */
					memcpy(iv, (void *)(((unsigned int)data_buffer)+ data_buffer_size - ivSize), ivSize*sizeof(TCsdUnsignedInt8));
				}
			}	
			if ( pIO->dout )
			{

				if(available > data_buffer_size) 
				{
					memcpy(pIO->dout+filled, data_buffer, data_buffer_size);
					filled += data_buffer_size;
					available -= data_buffer_size;
				} 
				else 
				{
					memcpy(pIO->dout+filled, data_buffer, available);
					pIO->dout_len =  pIO->din_len;
					break;
				}	
			}
			

			NEXUS_Recpump_DataReadComplete(recpump, data_buffer_size);
		}

		if (index_buffer_size)
		{
			NEXUS_Recpump_IndexReadComplete(recpump, index_buffer_size);
		}	
	}

	if( out_file )
	{
		if (pIO && pIO->dout)
		{
			fwrite( pIO->dout, 1, pIO->dout_len, out_file );
			fflush(out_file);
		}
	}

error:
	*pbRun = false; 

	/* printf ("$$$$$ Stop Buffer recording $$$$$\n"); */
	/* NEXUS_Recpump_Stop(recpump); */
	if (dma_event)
		BKNI_DestroyEvent(dma_event);
	printf("$$$$$ BufferRecord Thread Exiting $$$$$\n");
	pthread_exit(NULL);

	return NULL;

}

