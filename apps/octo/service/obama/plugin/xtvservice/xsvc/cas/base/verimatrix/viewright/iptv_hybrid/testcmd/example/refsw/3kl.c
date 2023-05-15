/******************************************************************************
 *    (c)2009 Verimatrix 
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_message.h"
#include "nexus_memory.h"
#include "nexus_keyladder.h"

#include "nexus_pid_channel.h"
#include "nexus_frontend.h"
#include "nexus_parser_band.h"

#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_input.h"


#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_playpump.h"
#include "nexus_recpump.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

extern int nexus_platform_init();
extern int nexus_service_start(unsigned short pid);
extern int nexus_add_key_slot_to_channel(unsigned char service_idx, unsigned short pid);
extern int nexus_write_ts_to_playpump(void *ts, int len);

/* Verimatrix Emulator */
#include "mwapp.h"

//#include "vmx_vr_iptvh_testcmd_join.h" // temp_humandks

#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}


static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    HxLOG_Print ("overflow %s\n", (const char *)context);
}

static void print_status(NEXUS_RecpumpHandle recpump)
{
    NEXUS_RecpumpStatus status;
    NEXUS_Recpump_GetStatus(recpump, &status);
    HxLOG_Print ("status: RAVE %d\n", status.rave.index);
    HxLOG_Print ("  data:  %u total bytes \tfifo %u/%u\n", (unsigned)status.data.bytesRecorded, status.data.fifoDepth, status.data.fifoSize);
    HxLOG_Print ("  index: %u total bytes \tfifo %u/%u\n", (unsigned)status.index.bytesRecorded, status.index.fifoDepth, status.index.fifoSize);
}


/* simple parsing of MPEG2 SCT
see BSEAV/lib/bcmplayer/src/bcmindexer for a complete example of SCT parsing */
struct sct6 { unsigned word[6]; };


#define TEST_ASSERT_EQUAL(EXP, VAL)                                     \
do {                                                                    \
    long __exp = (EXP);                                                 \
    long __val = (VAL);                                                 \
    if (__exp != __val) {                                               \
        fprintf(stderr, "M2M ASSERT FAILS: %s:%d\n", __FILE__, __LINE__);\
        fprintf(stderr, "\tvalue of: " #EXP "\n");                      \
        fprintf(stderr, "\texpected: %ld, got: %ld\n", __val, __exp);   \
        {                                                               \
        }                                                               \
    }                                                                   \
    {                                                                   \
        fprintf(stderr, "M2M ASSERT SUCCESS: %s:%d\n", __FILE__, __LINE__);\
    }                                                                   \
} while (0)




#define DUMP_CIPHER

#ifdef DUMP_CIPHER
/*----------------------------------------------------------------------
|	dump
+---------------------------------------------------------------------*/
void dump(const unsigned char* data, unsigned length, const char* prompt)
{
	unsigned int i;

	if (prompt) {
		fprintf(stderr, "%s:", prompt);
	}
	for (i = 0; i < length; ++i) {
		if (i % 8 == 0) {
			fprintf(stderr, "\n\t");
		}
		fprintf(stderr, "%02x, ", data[i]);
	}
	fprintf(stderr, "\n");
}

#endif

void endian_swap(unsigned int *x)
{
    *x = (*x>>24) | 
        ((*x<<8) & 0x00FF0000) |
        ((*x>>8) & 0x0000FF00) |
        (*x<<24);
}


#if (BCHP_CHIP == 7422) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) || (BCHP_CHIP == 7230) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552)
#define	HSM_IS_40NM    1
#endif


#define FIVE_STAGE_KL
#define KEYLADDER_TYPE NEXUS_SecurityKeyladderType_eAes128 /* NEXUS_SecurityKeyladderType_e3Des */

/* 
#define CA_128_4 
#define CUST_SUB_MODE NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4 
#define ASKM_MODULE_ID NEXUS_SecurityAskmModuleID_eModuleID_7 
#define VKL_ID NEXUS_SecurityVirtualKeyladderID_eVKL2
*/



/*
#define CA_64_7 
#define CUST_SUB_MODE NEXUS_SecurityCustomerSubMode_eReserved8 
#define ASKM_MODULE_ID NEXUS_SecurityAskmModuleID_eModuleID_11 
#define VKL_ID NEXUS_SecurityVirtualKeyladderID_eVKL6
*/


/* CA_128_7 
#define CA_128_7
#define CUST_SUB_MODE NEXUS_SecurityCustomerSubMode_eReserved9 
#define ASKM_MODULE_ID NEXUS_SecurityAskmModuleID_eModuleID_12 
#define VKL_ID NEXUS_SecurityVirtualKeyladderID_eVKL4
*/


#define CUST_SUB_MODE 0x12 
#define ASKM_MODULE_ID 0x11 
#define VKL_ID NEXUS_SecurityVirtualKeyladderID_eVKL2
static NEXUS_PidChannelHandle videoPidChannel;
static NEXUS_PlaypumpHandle playpump = NULL;
static BKNI_EventHandle event;
static NEXUS_VideoDecoderHandle videoDecoder;
static NEXUS_PlatformConfiguration platformConfig;

/* write TS to playpump */
int nexus_write_ts_to_playpump(void *ts, int len)
{
	void *buffer; /* playpump */
	size_t buffer_size;
	NEXUS_Error rc;

        if (playpump == NULL)
          return -1;

	rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size);
	
	if ((int) buffer_size < len) { 
		BKNI_WaitForEvent(event, BKNI_INFINITE);
		return nexus_write_ts_to_playpump(ts, len);
	}
	
#define MAX_READ (188*1024)
	if (buffer_size > MAX_READ)
		buffer_size = MAX_READ;
        memcpy(buffer, ts, len);
	rc = NEXUS_Playpump_WriteComplete(playpump, 0, len);

        return rc;
}

/* wait for playpump flush */
void nexus_flush_playpump(void)
{
	while (1) {
		NEXUS_VideoDecoderStatus status;
		NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
		if (!status.queueDepth) break;
	}
	NEXUS_Playpump_Flush(playpump);
}

/* Init Nexus platform */
int nexus_platform_init()
{
/*
		unsigned char		   authpProcIn3[16]={ 0xb2, 0xd4, 0x0b, 0xc3, 0x70, 0x72, 0x8a, 0x0d, 0xb7, 0x90, 0xe3, 0x7c, 0x25, 0x3e, 0x37 , 0x8b}; 
		
		unsigned char		   authpProcIn4[16]={ 0xf1, 0xa4, 0x34, 0x4b, 0x5a, 0x37, 0x5b, 0x7b, 0xaa, 0x9e, 0xf4, 0x80, 0x91, 0x06, 0xfe , 0x8e}; 
												
		
		unsigned char		   authpProcIn5[16]={ 0xa4, 0xd8, 0x17, 0xce, 0x9a, 0x7f, 0xd7, 0xbc, 0xc2, 0x4a, 0xca, 0xf5, 0xa2, 0xb1, 0x91 , 0x6f};
        unsigned short pid[8];
*/
	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error 			rc = NEXUS_SUCCESS;

	NEXUS_Platform_GetDefaultSettings(&platformSettings); 
	platformSettings.openFrontend = false;
//	NEXUS_Platform_Init(&platformSettings);  // 임의로 막음 for compile
	NEXUS_Platform_GetConfiguration(&platformConfig);

	/* setup the source, play pump, etc */
	BKNI_CreateEvent(&event);
   
/*
        pid[0] = 481;
        VMXSEC_ServiceStart(0, 1, pid);
        VMXSEC_AES_KL(0, 0, authpProcIn3, authpProcIn4, authpProcIn5, authpProcIn5, 16);  
     */

        return rc;
}

/* Init Nexus platform when service is start */
int nexus_service_start(unsigned short pid)
{
/*
		unsigned char		   pProcIn3[16]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
												0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x01 };
		
		unsigned char		   pProcIn4[16]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
												0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x02 };	
												
		
		unsigned char		   pProcIn5[16]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
												0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x03 };
		unsigned char		   authpProcIn3[16]={ 0xb2, 0xd4, 0x0b, 0xc3, 0x70, 0x72, 0x8a, 0x0d, 0xb7, 0x90, 0xe3, 0x7c, 0x25, 0x3e, 0x37 , 0x8b}; 
		
		unsigned char		   authpProcIn4[16]={ 0xf1, 0xa4, 0x34, 0x4b, 0x5a, 0x37, 0x5b, 0x7b, 0xaa, 0x9e, 0xf4, 0x80, 0x91, 0x06, 0xfe , 0x8e}; 
												
		
		unsigned char		   authpProcIn5[16]={ 0xa4, 0xd8, 0x17, 0xce, 0x9a, 0x7f, 0xd7, 0xbc, 0xc2, 0x4a, 0xca, 0xf5, 0xa2, 0xb1, 0x91 , 0x6f};
*/
	
	NEXUS_Error 			rc = NEXUS_SUCCESS;
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_DisplayHandle display;
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_HdmiOutputStatus hdmiStatus;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_PidChannelStatus pidStatus;

	playpump = NEXUS_Playpump_Open(0, NULL);

	NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
	playpumpSettings.dataCallback.callback = play_callback;
	playpumpSettings.dataCallback.context = event;
	NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pid, NULL);
	
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = stcChannel;
	
	/* Bring up video decoder and display. No audio to keep the example simple. */
	display = NEXUS_Display_Open(0, NULL);
	window = NEXUS_VideoWindow_Open(display, 0);
		
#if NEXUS_NUM_COMPONENT_OUTPUTS
	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
	NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
	rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
	if ( !rc && hdmiStatus.connected )
	{
		/* If current display format is not supported by monitor, switch to monitor's preferred format. 
		   If other connected outputs do not support the preferred format, a harmless error will occur. */
		NEXUS_Display_GetSettings(display, &displaySettings);
		displaySettings.format = NEXUS_VideoFormat_e1080p;
		NEXUS_Display_SetSettings(display, &displaySettings);
	}
#endif
	
	videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

	NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

	NEXUS_Playpump_Start(playpump);

        HxLOG_Print ("service start done\n");

        return rc;
}
		
		
/*
           VMXSEC_AES_KL(0, 0, NULL, authpProcIn3, authpProcIn4, authpProcIn5, 16);  
*/


/* 
 * Add key slot 
 */
int nexus_add_key_slot_to_channel(unsigned char service_idx, unsigned short pid)
{
   NEXUS_KeySlotHandle    hKeySlot;
   NEXUS_PidChannelStatus pidStatus;
   unsigned int videoPID;
   NEXUS_Error 			rc = NEXUS_SUCCESS;

   /* Add video PID channel to keyslot */
   rc = NEXUS_PidChannel_GetStatus (videoPidChannel, &pidStatus);
   videoPID = pidStatus.pidChannelIndex;
   hKeySlot = IPTV_GET_KEY_SLOT(service_idx, pid);
   NEXUS_Security_AddPidChannelToKeySlot(hKeySlot, videoPID); 

   return rc;
}

int BRCM_GetAlgorithmConfig( unsigned char bServiceIdx,
                             NEXUS_SecurityAlgorithm *alg,
                             NEXUS_SecurityAlgorithmVariant *algVar,
                             NEXUS_SecurityTerminationMode *termMode,
                             NEXUS_SecurityIVMode *ivMode,
                             NEXUS_SecuritySolitarySelect *solSelect,
                             NEXUS_SecurityAesCounterSize *aesCounterSize,
                             NEXUS_SecurityDvbScrambleLevel *dvbDescrambleLevel,
                             NEXUS_SecurityAlgorithmConfigDestination *configDest
                           )
{
   int result =0;
   mw_config_t *config = mw_app_get_config();
   if (config)
   {
	   switch(config->encAlg)
	   {
	   	   case eEncAlg_DVBCSA:
				HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, DVSBCA\n", __FILE__, __FUNCTION__, __LINE__);
			   if( alg ) *alg = NEXUS_SecurityAlgorithm_eDvb;
			   if( algVar ) *algVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			   if( termMode ) *termMode = NEXUS_SecurityTerminationMode_eClear;
			   if( ivMode ) *ivMode = NEXUS_SecurityIVMode_eRegular;
			   if( solSelect ) *solSelect = NEXUS_SecuritySolitarySelect_eClear;
			   if( dvbDescrambleLevel ) *dvbDescrambleLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			   result = 1;
	   		   break;

	   	   case eEncAlg_AESECBT:
	   	   default:
			   HxLOG_Print ("@@@@@@@@@@ %s, %s, %d, AES\n", __FILE__, __FUNCTION__, __LINE__);
			   if( alg ) *alg = NEXUS_SecurityAlgorithm_eAes;
			   if( algVar ) *algVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			   if( termMode ) *termMode = NEXUS_SecurityTerminationMode_eClear;
			   if( ivMode ) *ivMode = NEXUS_SecurityIVMode_eRegular;
			   if( solSelect ) *solSelect = NEXUS_SecuritySolitarySelect_eClear;
			   if( dvbDescrambleLevel ) *dvbDescrambleLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			   result = 1;
	   }
   }
  return result;
}

/* function to return the needed stream handle for a given pid */
NEXUS_KeySlotHandle BRCM_GetStreamHandle( unsigned short wStreamPid )
{
  NEXUS_SecurityKeySlotSettings keySetting;
  NEXUS_KeySlotHandle    hKeySlot;

  NEXUS_Security_GetDefaultKeySlotSettings(&keySetting);
  keySetting.keySlotEngine = NEXUS_SecurityEngine_eCa;

  hKeySlot = NEXUS_Security_AllocateKeySlot(&keySetting);

  return hKeySlot;
}
