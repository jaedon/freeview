#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "nexus_timebase.h"

#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_hdmi.h"
#include "di_video.h"
#include "di_demux.h"
#include "di_audio.h"
#include "di_scart.h"
#include "di_osd.h"
#include "di_nvram.h"
#include "drv_nvram.h"
#include "drv_gpio.h"
#include "drv_hdmi.h"
#include "drv_video.h"
#include "drv_audio.h"
#include "drv_osd.h"
#include "cmd_hwtest_parser_av_decode.h"
#if defined(CONFIG_FAST_AUDIO_CHANGE)
#include "di_pvr.h"
#endif

#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
#include "fcntl.h"
#include "di_transcoder.h"
#include "di_pvr.h"
#define	ENABLE_TEST_TWO_TRANSCODING_ONLY_LIVE 1
#endif
#if defined(CONFIG_IPSTREAM) && defined(CONFIG_IPSTREAM_MULTICAST)
#include "drv_multicast.h"
#endif
#if defined(CONFIG_MIC)
#include "di_mic.h"
#endif
#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, (int *)&val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

extern unsigned char AudioOnlyStillImage[];
extern unsigned char ScrambledStillImage[];
extern unsigned long ulAudioOnlyStillImage_size;
extern unsigned long ulScrambledStillImage_size;

extern int HwTest_OSD_FlushColorBar(void);
extern int HwTest_OSD_ColorBar(void);
extern int HwTest_OSD_ClearScreen(DI_OSD_LAYER nOsdLayer);

#define	PSI_DEFAULT_SIZE	188
unsigned char s_auiTSPacket_PAT[PSI_DEFAULT_SIZE] =
{
	0x47,0x40,0x00,0x30,0xa6,0x40,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,'P','A','T',
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0x00,0x00,0xb0,0x0d,0x00,
	0x00,0x81,0x00,0x00,0x00,0x01,0xe0,0x55,
	0x65,0x80,0x5e,0xdc,
};

unsigned char s_auiTSPacket_PMT[PSI_DEFAULT_SIZE] =
{
	0x47,0x40,0x55,0x30,0x9c,0x40,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,'P','M','T',
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0x00,0x02,0xb0,0x17,0x00,0x01,0xc1,
	0x00,0x00,0xe0,0x13,0xf0,0x00,0x1b,0xe0,
	0x11,0xf0,0x00,0x81,0xe0,0x12,0xf0,0x00,
	0x3d,0x19,0x07,0x2f,
};
unsigned char s_auiTSPacket_EIT[PSI_DEFAULT_SIZE] =
{
   0x47,0x40,0x12,0x1E,0x00,0x4E,0xF0,0x29,
   0x00,0xE1,0xDF,0x00,0x01,0x04,0x47,0x00,
   0x01,0x01,0x4e,0x00,0x00,0x9e,0x8b,0x00,
   0x00,0x00,0x00,0x00,0x00,0x80,0x0e,0x50,
   0x0c,0xe4,0x01,0x00,0x20,0x20,0x20,0x44,
   0x61,0x74,0x61,0x20,0x30,0x38,0x32,0x64,
   0x6e,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0x00,0x00,0xb0,0x0d,0x00,
   0x00,0xc1,0x00,0x00,0x00,0x01,0xe0,0x0A,
   0xff,0xff,0xff,0xff
};

unsigned char patsectiondata[] =
{
	0x00,0xB0,0x2D,0x04,0x47,0xD3,0x00,0x00,0x00,0x00,0xE0,0x10,0x00,0xE1,0xE4,0x4C,
	0x4F,0x6A,0xE4,0x00,0x4F,0x6B,0xE4,0x01,0x4F,0x6C,0xE4,0x02,0x4F,0x6D,0xE4,0x03,
	0x4F,0x6E,0xE4,0x04,0x4F,0x6F,0xE4,0x05,0x4F,0x70,0xE4,0x06,0x5F,0xB7,0x49,0x44
};

unsigned char pmtsectiondata[] =
{
	0x02,0xB0,0x29,0x4F,0x6C,0xF4,0x00,0x00,0xE0,0xA2,0xF0,0x06,0x09,0x04,0x0D,0x02,
	0xE3,0xC6,0x02,0xE0,0xA2,0xF0,0x03,0x52,0x01,0x01,0x03,0xE0,0x58,0xF0,0x09,0x0A,
	0x04,0x70,0x6F,0x6C,0x01,0x52,0x01,0x02,0x0F,0x3D,0x5B,0xD4
};

unsigned char eitsectiondata[] =
{
	0x4F,0xF0,0xEA,0x4F,0x6B,0xDD,0x00,0x01,0x04,0x47,0x00,0x01,0x01,0x4E,0x66,
	0x7B,0xC7,0xFF,0x19,0x00,0x00,0x01,0x35,0x00,0x90,0xCF,0x0A,0x14,0x63,0x7A,
	0x65,0x00,0x65,0x6E,0x67,0x00,0x68,0x75,0x6E,0x00,0x70,0x6F,0x6C,0x00,0x72,
	0x75,0x6D,0x00,0x4D,0x5A,0x70,0x6F,0x6C,0x13,0x52,0x61,0x73,0x63,0x61,0x6C,
	0x73,0x20,0x61,0x6E,0x64,0x20,0x52,0x6F,0x62,0x63,0x65,0x72,0x73,0x42,0x46,
	0x69,0x6C,0x6D,0x20,0x6E,0x61,0x20,0x70,0x6F,0x64,0x73,0x74,0x61,0x77,0x69,
	0x65,0x20,0x70,0x6F,0x77,0x69,0x65,0xC2,0x73,0x63,0x69,0x20,0x4D,0x61,0x72,
	0x6B,0x61,0x20,0x54,0x77,0x61,0x69,0x6E,0x61,0x20,0x6F,0x20,0x70,0x72,0x7A,
	0x79,0x67,0x6F,0x64,0x61,0x63,0x68,0x20,0x54,0x6F,0x6D,0x61,0x20,0x69,0x20,
	0x48,0x75,0x63,0x6B,0x61,0x50,0x0D,0xF1,0x01,0x01,0x20,0x20,0x20,0x56,0x69,
	0x64,0x65,0x6F,0x20,0x31,0x50,0x0D,0xF2,0x01,0x02,0x70,0x6F,0x6C,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x32,0x50,0x0D,0xF2,0x01,0x03,0x68,0x75,0x6E,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x33,0x50,0x0D,0xF2,0x01,0x04,0x65,0x6E,0x67,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x34,0x50,0x0D,0xF2,0x01,0x05,0x63,0x7A,0x65,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x35,0x50,0x10,0xF3,0x01,0x06,0x72,0x75,0x6D,0x54,0x65,
	0x6C,0x65,0x74,0x65,0x78,0x74,0x20,0x36,0xFB,0xED,0x8B,0x14
};

unsigned long PcrTimerId=0;

#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
int trans_filehandle = -1;
void hwtest_TRANCODE_DataRdy(int deviceId, const void *buf, unsigned int length)
{
	int wr_len;
	//VK_printf("[hee] %s called. dev_id=%d data_len=%lu\n", __func__, deviceId, length);
	if(trans_filehandle == -1)
	{
		trans_filehandle = open("/mnt/hd2/heetranscode.ts\0", O_CREAT|O_DIRECT|O_WRONLY);
		if(trans_filehandle < 0)
		{
			VK_printf("[hee] file_openerror.\n");
			trans_filehandle = -1;
		}
	}
	if(trans_filehandle >= 0)
	{
		wr_len = write(trans_filehandle, buf, length);
		if(wr_len != length)
		{
			VK_printf("[hee] file_write error (req: %lu real=%lu)\n", length, wr_len);
		}
	}
}
void hwtest_TRANSCODER_SEQ_INFO_CB(void *pCallbackData)
{
	DI_TRANSCODER_CALLBACK_DATA_t *pCBData = (DI_TRANSCODER_CALLBACK_DATA_t *)pCallbackData;
	DI_TRANSCODER_DEC_StreamInfo_t StreamInfo;
	CMD_Printf("[%d]%s Probe Called=(EVT=%d:DeviceID=%d\n",__LINE__,__func__,pCBData->eEvent,pCBData->ulDeviceId);
	if(pCBData->eEvent == DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS)
	{
		DI_TRANSCODER_GetDecSequenceHeaderInfo(pCBData->ulDeviceId, &StreamInfo);
		CMD_Printf("===================================\n");
		CMD_Printf("Width = %d\n",StreamInfo.ulWidth);
		CMD_Printf("Height = %d\n",StreamInfo.ulHeight);
		CMD_Printf("AspectRatio = %d\n",StreamInfo.eAspectRatio);
		CMD_Printf("usSarX = %d\n",StreamInfo.usSampleAspectRatioX);
		CMD_Printf("usSarY = %d\n",StreamInfo.usSampleAspectRatioY);
		CMD_Printf("FrameRate = %d\n",StreamInfo.eFrameRate);
		CMD_Printf("VideoFormat = %d\n",StreamInfo.eVideoFormat);
		CMD_Printf("I/P = %d\n",StreamInfo.bProgressive);
		CMD_Printf("Codec Level = %d\n",StreamInfo.eCodecLevel);
		CMD_Printf("Codec Profile = %d\n",StreamInfo.eCodecProfile);
		CMD_Printf("===================================\n");
	}

}
#endif

#if defined(CONFIG_SYNC_RECOVERY)
static void CMD_HwTest_GetPcrTimer(unsigned long TimerId, void *params)
{
	unsigned int pcr=0;
	if(TimerId != NULL)
	{
		pcr=DRV_SYNC_GetPcrValue();
		CMD_Printf("Current PCR Value : %ul(0x%x)\n",pcr,pcr);
	}
}
#endif


#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
void CMD_HwTest_3dInfoCallback(void *pCallbackData)
{
	DI_VIDEO_CALLBACK_DATA_t *pVideoCallbackData;
	DI_VIDEO_3D_INFO_EVT_t *p3dInfo;

	pVideoCallbackData=(DI_VIDEO_CALLBACK_DATA_t *)pCallbackData;
	p3dInfo = pVideoCallbackData->pvEventData;

	if(pVideoCallbackData->eEvent == DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED)
	{
		CMD_Printf("[CMD_HwTest_3dInfoCallback] : e3dFormat=%d\n", p3dInfo->e3dFormat);
		switch(p3dInfo->e3dFormat)
		{
			case DI_VIDEO_3D_FORMAT_NONE:
				DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_NONE);
				break;
			case DI_VIDEO_3D_FORMAT_SIDEBYSIDE:
				DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_SIDEBYSIDE);
				break;
			case DI_VIDEO_3D_FORMAT_TOPANDBOTTOM:
				DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_TOPANDBOTTOM);
				break;
			case DI_VIDEO_3D_FORMAT_FRAMEPACKING:
				DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_FRAMEPACKING);
				break;
		}
	}
	else
	{
		CMD_Printf("[CMD_HwTest_3dInfoCallback] : Wrong event\n");
	}
}
#endif

#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
void hwtest_TRANS_TEST_CB(void *pvParam)
{
	return;
}
#endif

static void Cmd_SvcSyncTimer(unsigned long tid, void *arg)
{
#if defined(CONFIG_AUDIO_DECODER) && defined(CONFIG_VIDEO_DECODER)
	HUINT32 ulHighBitSTC = 0;
	HUINT32 ulLowBitSTC = 0;
	NEXUS_AudioDecoderHandle hAuioDecoder;
	NEXUS_AudioDecoderStatus tAudioStatus;
	NEXUS_VideoDecoderStatus tVidStatus;
	NEXUS_TimebaseStatus timebaseStatus;
	NEXUS_VideoDecoderHandle hVideoDecoder;

	BSTD_UNUSED(tid);
	BSTD_UNUSED(arg);

	DI_DEMUX_GetSTC (0,&ulHighBitSTC,&ulLowBitSTC);
	DRV_AUDIO_GetDecoderHandle(0,&hAuioDecoder);
	NEXUS_AudioDecoder_GetStatus(hAuioDecoder, &tAudioStatus);
	DRV_VIDEO_GetVideoHandle(0,&hVideoDecoder);
	NEXUS_VideoDecoder_GetStatus(hVideoDecoder,&tVidStatus);
	NEXUS_Timebase_GetStatus(NEXUS_Timebase_e0, &timebaseStatus);
	CMD_Printf(">> STC 0x%x PCR 0x%x V 0x%x(0x%x, %dms)  A 0x%x(0x%x,%dms)\n",(ulHighBitSTC<<31)|(ulLowBitSTC>>1),timebaseStatus.lastValue,
		tVidStatus.pts,tVidStatus.ptsStcDifference,tVidStatus.ptsStcDifference/90,
		tAudioStatus.pts,tAudioStatus.ptsStcDifference,tAudioStatus.ptsStcDifference/90);
	CMD_Printf("VBuf [0x%x/0x%x] ABuf [0x%x/0x%x]\n",tVidStatus.fifoDepth,tVidStatus.fifoSize,tAudioStatus.fifoDepth,tAudioStatus.fifoSize);
#endif
}

int CMD_HwTest_Parser_Video(void *szArgs)
{
	GET_ARGS;
	DI_VIDEO_AOUT_MODE_t OutputStatus;
	static DI_OSD_HANDLE	s_TEST_SURFACE_HANDLE = 0;
    extern void DRV_VIDEO_SetMnr(int iMnr);
    extern void DRV_VIDEO_SetBnr(int iBnr);
    extern void DRV_VIDEO_SetDcr(int iDcr);
    extern unsigned int DRV_VIDEO_SetDispParam(short contrast, short saturation, short hue, short brightness);
#if defined(CONFIG_MICOM_NONE)
	static DI_VIDEO_AOUT_MODE_t 	OutputStatusBak;
#endif
	// get a osd test surface for video test from LAYER_0
	// usable layer is LAYER_1, LAYER_2
#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	if(DRV_OSD_GetOsdHandle(DI_OSD_LAYER_0, &s_TEST_SURFACE_HANDLE) != DRV_OK)
	{
		CMD_Printf("[%s]Not Avaiable Surface(LAYER_%d)\r\n", __func__, DI_OSD_LAYER_0);
	}
#endif

	if CMD_IS("header")
	{
		DI_VIDEO_StreamInfo_t tVideoStreamInfo;
		DI_VIDEO_GetSequenceHeaderInfo(0,&tVideoStreamInfo);
		CMD_Printf(" Stream info ulWidth = %d\n",tVideoStreamInfo.ulWidth);
		CMD_Printf(" Stream info ulHeight= %d\n",tVideoStreamInfo.ulHeight);
		CMD_Printf(" Stream info ulDisWidth = %d\n",tVideoStreamInfo.ulDisplayWidth);
		CMD_Printf(" Stream info ulDisHeight= %d\n",tVideoStreamInfo.ulDisplayHeight);
		CMD_Printf(" Stream info eAspectRatio = %d\n",tVideoStreamInfo.eAspectRatio);
		CMD_Printf(" Stream info eFrameRate = %d\n",tVideoStreamInfo.eFrameRate);
		CMD_Printf(" Stream info eVideoFormat = %d\n",tVideoStreamInfo.eVideoFormat);
		CMD_Printf(" Stream info bProgressive = %d\n",tVideoStreamInfo.bProgressive);
		CMD_Printf(" Stream info ulColorPrimarie = %d\n",tVideoStreamInfo.ucColorPrimarie);
		iResult=CMD_OK;
	}
#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("bar")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : video bar on -> COLOR BAR On\r\n");
			iResult = HwTest_OSD_FlushColorBar();
			if (iResult != DI_ERR_OK)
			{
				CMD_Printf("Error in HwTest_OSD_FlushColorBar()\r\n");
			}
			else
			{
				iResult=CMD_OK;
			}
		}
		if PARAM_IS("on_noflush")
		{
			iResult = HwTest_OSD_ColorBar();
			if (iResult != DI_ERR_OK)
			{
				CMD_Printf("Error in HwTest_OSD_ColorBar()\r\n");
			}
			else
			{
				iResult=CMD_OK;
			}
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : video bar off -> COLOR BAR Off\r\n");
			iResult = HwTest_OSD_ClearScreen(DI_OSD_LAYER_0);
			if (iResult != DI_ERR_OK)
			{
				CMD_Printf("Error in HwTest_OSD_ClearScreen()\r\n");
			}
			else
			{
				iResult=CMD_OK;
			}
		}
	}
#endif
	else if CMD_IS("cgms")
	{
		DI_VIDEO_CGMS_SETTINGS tCgmsSettings;
		DI_VIDEO_WIN_TYPE eVidWin;
		char* szParm3=CMD_NextArg((char **)&szArgs);
		char* szParm4=CMD_NextArg((char **)&szArgs);

		if PARAM_IS("on")
		{
			if ( (READABLE_IN_DEC(HWTEST_PARAM1, tCgmsSettings.uTypeStatus.tArib.eAspectRatio) != 1 ) ||
			(READABLE_IN_DEC(HWTEST_PARAM2, tCgmsSettings.uTypeStatus.tArib.eCgmsA) != 1 ) ||
			(READABLE_IN_DEC(szParm3, tCgmsSettings.uTypeStatus.tArib.eApsTrigger) != 1 ) ||
			(READABLE_IN_DEC(szParm4, eVidWin) != 1 ))
			{
				return 1;
			}

			CMD_Printf("DI_VIDEO_SetCGMS ON AR %d CGMSA %d ApsTrigger %d eVidWin %d\n",
			tCgmsSettings.uTypeStatus.tArib.eAspectRatio,tCgmsSettings.uTypeStatus.tArib.eCgmsA,tCgmsSettings.uTypeStatus.tArib.eApsTrigger,eVidWin);

			tCgmsSettings.eType = DI_VIDEO_CGMS_ARIB;
			DI_VIDEO_SetCGMS(TRUE,tCgmsSettings,eVidWin);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			if  (READABLE_IN_DEC(HWTEST_PARAM1, eVidWin) != 1 )
			{
				return 1;
			}

			CMD_Printf("DI_VIDEO_SetCGMS OFF  eVidWin %d\n",eVidWin);

			DI_VIDEO_SetCGMS(FALSE,tCgmsSettings,eVidWin);
			iResult=CMD_OK;
		}
	}
	else if CMD_IS("dport")
	{
		if PARAM_IS("1080i")
		{
			CMD_Printf("DPORT 1080i");
			if PARAM1_IS("0")
			{
				CMD_Printf(" 4:3 Value LINE1 5V LINE2 0V LINE3 0V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_1080I, DI_VIDEO_ASPECTRATIO_4_3);
				iResult=CMD_OK;

			}
			else if PARAM1_IS("1")
			{
				CMD_Printf(" 4:3 LETTER Value LINE1 5V LINE2 0V LINE3 2.2V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_1080I, DI_VIDEO_ASPECTRATIO_4_3_LETTER);
				iResult=CMD_OK;

			}
			else if PARAM1_IS("2")
			{
				CMD_Printf(" 16:9 LETTER Value LINE1 5V LINE2 0V LINE3 5V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_1080I,DI_VIDEO_ASPECTRATIO_16_9 );

				iResult=CMD_OK;
			}
		}
		else if PARAM_IS("720p")
		{
			CMD_Printf("DPORT 720p");
			if PARAM1_IS("0")
			{
				CMD_Printf(" 4:3 Value LINE1 2.2V LINE2 5V LINE3 0V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_720P,DI_VIDEO_ASPECTRATIO_4_3 );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("1")
			{
				CMD_Printf(" 4:3 LETTER Value LINE1 2.2V LINE2 5V LINE3 2.2V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_720P,DI_VIDEO_ASPECTRATIO_4_3_LETTER );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("2")
			{
				CMD_Printf(" 16:9 LETTER Value LINE1 2.2V LINE2 5V LINE3 5V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_720P,DI_VIDEO_ASPECTRATIO_16_9 );
				iResult=CMD_OK;

			}

		}
		else if PARAM_IS("480p")
		{
			CMD_Printf("DPORT 480p");
			if PARAM1_IS("0")
			{
				CMD_Printf(" 4:3 Value LINE1 0V LINE2 5V LINE3 0V\n");

				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480P,DI_VIDEO_ASPECTRATIO_4_3 );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("1")
			{
				CMD_Printf(" 4:3 LETTER Value LINE1 0V LINE2 5V LINE3 2.2V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480P,DI_VIDEO_ASPECTRATIO_4_3_LETTER );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("2")
			{
				CMD_Printf(" 16:9 LETTER Value LINE1 0V LINE2 5V LINE3 5V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480P, DI_VIDEO_ASPECTRATIO_16_9);
				iResult=CMD_OK;

			}

		}
		else if PARAM_IS("480i")
		{
			CMD_Printf("DPORT 480i");
			if PARAM1_IS("0")
			{
				CMD_Printf(" 4:3 Value LINE1 0V LINE2 0V LINE3 0V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480I,DI_VIDEO_ASPECTRATIO_4_3 );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("1")
			{
				CMD_Printf(" 4:3 LETTER Value LINE1 0V LINE2 0V LINE3 2.2V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480I,DI_VIDEO_ASPECTRATIO_4_3_LETTER );
				iResult=CMD_OK;

			}
			else if PARAM1_IS("2")
			{
				CMD_Printf(" 16:9 LETTER Value LINE1 0V LINE2 0V LINE3 5V\n");
				DI_VIDEO_ControlDPort(0,DI_VIDEO_HD_RESOLUTION_480I,DI_VIDEO_ASPECTRATIO_16_9 );
				iResult=CMD_OK;

			}
		}

	}
	else if CMD_IS("sd")
	{
		DRV_VIDEO_GetCurrentOutputStatus(&OutputStatus);
		if PARAM_IS("on")
		{
			CMD_Printf("Command : video sd on -> COMPOSITE OUT On\r\n");
			OutputStatus.bCVBS = 1;
			OutputStatus.bSVHS = 1;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : video sd off -> COMPOSITE OUT OFF\r\n");
			OutputStatus.bCVBS = 0;
			OutputStatus.bSVHS = 0;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
	}
	else if CMD_IS("comp")
	{
		DRV_VIDEO_GetCurrentOutputStatus(&OutputStatus);
		if PARAM_IS("on")
		{
			CMD_Printf("Command : video comp on -> COMPONENT OUT ON\r\n");
			DRV_GPIO_Write(GPIO_ID_VIDEO_SEL,0);
			OutputStatus.bYPbPr = 1;
			OutputStatus.bYPbPr_Downscale = 0;
			OutputStatus.bRGB = 0;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : video comp off -> COMPONENT OUT OFF\r\n");
			OutputStatus.bYPbPr = 0;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
	}
	else if CMD_IS("rgb")
	{
		DRV_VIDEO_GetCurrentOutputStatus(&OutputStatus);
		if PARAM_IS("on")
		{
			CMD_Printf("Command : video rgb on -> RGB OUT ON\r\n");
			DRV_GPIO_Write(GPIO_ID_VIDEO_SEL,1);
			OutputStatus.bYPbPr = 0;
			OutputStatus.bYPbPr_Downscale = 0;
			OutputStatus.bRGB = 1;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : video rgb off -> RGB OUT OFF\r\n");
			OutputStatus.bRGB = 0;
			DI_VIDEO_SetOutputSettings (OutputStatus);
			iResult=CMD_OK;
		}
	}
	else if CMD_IS("vf")
	{
		DI_VIDEO_HD_RESOLUTION eHDOutput = DI_VIDEO_HD_RESOLUTION_MAX;
#if defined(CONFIG_HDMI_REFACTORING)
		DRV_Error dResult=DRV_OK;
		DI_NVRAM_VIDEO_RESOLUTION_E eHDNvramOutput;
		HBOOL SupportedFormat = FALSE;
#endif
		if PARAM_IS("480i")
		{
			CMD_Printf("Command : video vf 480i\r\n");
			DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_480I);
			eHDOutput = DI_VIDEO_HD_RESOLUTION_480I;
			iResult=CMD_OK;
		}
		else if PARAM_IS("480p")
		{
			CMD_Printf("Command : video vf 480p\r\n");
			DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_480P);
			eHDOutput = DI_VIDEO_HD_RESOLUTION_480P;
			iResult=CMD_OK;
		}
		else if PARAM_IS("576i")
		{
			CMD_Printf("Command : video vf 576i\r\n");
			DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_576I);
			eHDOutput = DI_VIDEO_HD_RESOLUTION_576I;
			iResult=CMD_OK;
		}
		else if PARAM_IS("576p")
		{
			CMD_Printf("Command : video vf 576p\r\n");
			DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_576P);
			eHDOutput = DI_VIDEO_HD_RESOLUTION_576P;
			iResult=CMD_OK;
		}
		else if PARAM_IS("720p")
		{
			if PARAM1_IS("60")
			{
				CMD_Printf("Command : video vf 720p 60Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_720P);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_720P;
				iResult=CMD_OK;
			}
			else //if PARAM1_IS("50")
			{
				CMD_Printf("Command : video vf 720p 50Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_720P_50HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_720P_50HZ;
				iResult=CMD_OK;
			}
		}
		else if PARAM_IS("1080i")
		{
			if PARAM1_IS("60")
			{
				CMD_Printf("Command : video vf 1080i 60Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080I);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I;
				iResult=CMD_OK;
			}
			else //if PARAM1_IS("50")
			{
				CMD_Printf("Command : video vf 1080i 50Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080I_50HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I_50HZ;
				iResult=CMD_OK;
			}
		}
		else if PARAM_IS("1080p")
		{
			if PARAM1_IS("60")
			{
				CMD_Printf("Command : video vf 1080p 60Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080P);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P;
				iResult=CMD_OK;
			}
			else if PARAM1_IS("24")
			{
				CMD_Printf("Command : video vf 1080p 24Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080P_24HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P_24HZ;
				iResult=CMD_OK;
			}
			else if PARAM1_IS("30")
			{
				CMD_Printf("Command : video vf 1080p 30Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080P_30HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P_30HZ;
				iResult=CMD_OK;
			}
			else //if PARAM1_IS("50")
			{
				CMD_Printf("Command : video vf 1080p 50Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080P_50HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P_50HZ;
				iResult=CMD_OK;
			}
		}
#if defined(CONFIG_HDOUT_2160P)
		else if PARAM_IS("2160p")
		{
			if PARAM1_IS("24")
			{
				CMD_Printf("Command : video vf 2160p 24Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_2160P_24HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_2160P_24HZ;
				iResult=CMD_OK;
			}
			else if PARAM1_IS("25")
			{
				CMD_Printf("Command : video vf 2160p 25Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_2160P_25HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_2160P_25HZ;
				iResult=CMD_OK;
			}
			else if PARAM1_IS("30")
			{
				CMD_Printf("Command : video vf 2160p 30Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_2160P_30HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_2160P_30HZ;
				iResult=CMD_OK;
			}
			else if PARAM1_IS("60")
			{
				CMD_Printf("Command : video vf 2160p 60Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_2160P);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_2160P;
				iResult=CMD_OK;
			}
			else //if PARAM1_IS("50")
			{
				CMD_Printf("Command : video vf 2160p 50Hz\r\n");
				DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_2160P_50HZ);
				eHDOutput = DI_VIDEO_HD_RESOLUTION_2160P_50HZ;
				iResult=CMD_OK;
			}
		}
#endif
		else
		{
			CMD_Printf("Unsupported video format\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Unsupported video format\n");
            }
			iResult=CMD_OK;
		}

#if defined(CONFIG_HDMI_REFACTORING)
		if(eHDOutput != DI_VIDEO_HD_RESOLUTION_MAX)
		{
			dResult = DRV_HDMI_CheckSupportedResolution(DRV_VIDEO_GetFormatType(0,eHDOutput),&SupportedFormat);

			if(dResult != DRV_OK)
			{
				CMD_Printf("[%s] DRV_HDMI_CheckSupportedResolution error!\n", __func__);
			}

			if(SupportedFormat == TRUE)
			{
				CMD_Printf("[%s] Supported Resolution [%d]!\n", __func__,eHDOutput);

				DRV_HDMI_ConverHdVideoFormatToNvramVideoFormat(eHDOutput, &eHDNvramOutput);
				CMD_Printf("[%s][%d]eHDOutput=%x,  eHDNvramOutput=%x\n",__func__,__LINE__,eHDOutput,eHDNvramOutput);

				dResult = DRV_NVRAM_SetField(DI_NVRAM_FIELD_HD_RESOULTION,0,&eHDNvramOutput,sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));
				if(dResult != DRV_OK)
				{
					CMD_Printf("[%s] DRV_NVRAM_SetField[DI_NVRAM_FIELD_HD_RESOULTION] error!\n", __func__);
					#if 0
						rc=DI_ERR_ERROR;
						goto done;
					#endif
				}
			}
			else
			{
				CMD_Printf("[%s] Not Supported Resolution [%d]!\n", __func__,eHDOutput);
			}
		}
#endif

	}
	else if CMD_IS("mv")
	{
		int	mv_test_num;

		if (READABLE_IN_DEC(HWTEST_PARAM, mv_test_num))
		{
			if (mv_test_num<0 || mv_test_num>9)
				mv_test_num = 0;
		}
		else
		{
			mv_test_num = 0;
		}

		switch (mv_test_num)
		{
			case DI_VIDEO_MV_NONE:
				CMD_Printf("Command : video mv 0 -> Macrovision NONE \n");
				break;
			case DI_VIDEO_MV_AGC_ONLY:
				CMD_Printf("Command : video mv 1 -> Macrovision eAgcOnly \n");
				break;
			case DI_VIDEO_MV_AGC_2LINES:
				CMD_Printf("Command : video mv 2 -> Macrovision eAgc2Lines \n");
				break;
			case DI_VIDEO_MV_AGC_4LINES:
				CMD_Printf("Command : video mv 3 -> Macrovision eAgc4Lines \n");
				break;
			case DI_VIDEO_MV_STANDARD_PAL:
				CMD_Printf("Command : video mv 4 -> Macrovision STANDARD PAL\n");
				break;
			case DI_VIDEO_MV_STANDARD_NTSC:
				CMD_Printf("Command : video mv 5 -> Macrovision STANDARD NTSC\n");
				break;
			case DI_VIDEO_MV_AGC_ONLY_RGB:
				CMD_Printf("Command : video mv 6 -> Macrovision eAgcOnlyRgb \n");
				break;
			case DI_VIDEO_MV_TEST1:
				CMD_Printf("Command : video mv 7 -> Macrovision TEST1 \n");
				break;
			case DI_VIDEO_MV_TEST2:
				CMD_Printf("Command : video mv 8 -> Macrovision TEST2 \n");
				break;
			default:
				CMD_Printf("Command : video mv 9 -> Macrovision NONE \n");
				break;
		}

		DI_VIDEO_SetMacroVision((DI_VIDEO_MV_MODE)mv_test_num);
		iResult=CMD_OK;
	}
	else if CMD_IS("info")
	{
		if PARAM_IS("0")
			DRV_VIDEO_PrintStatus(0);
#if defined(CONFIG_PIP)
		else if PARAM_IS("1")
			DRV_VIDEO_PrintStatus(1);
#endif
		else
			DRV_VIDEO_PrintStatus(0);

		iResult=CMD_OK;
	}
	else if CMD_IS("still")
	{
		if PARAM_IS("stop")
		{
			iResult = DI_VIDEO_StopMemoryStream(0);
			AssertDI(iResult, CMD_ERR);

			CMD_Printf("Command : video still stop -> Stop display AudioOnlyStillImage to VIDEO Layer\n");
		}
		else if PARAM_IS("0")	// AudioOnlyStillImage play
		{
			iResult = DI_VIDEO_Stop(0);
			AssertDI(iResult, CMD_ERR);

			iResult = DI_VIDEO_PlayMemoryStream(0, DI_VIDEO_STREAM_TYPE_ES, DI_VIDEO_FORMAT_STILL_MPEG2, (HUINT8*)AudioOnlyStillImage, ulAudioOnlyStillImage_size, NULL);
			AssertDI(iResult, CMD_ERR);

			CMD_Printf("Command : video still 0 -> Display AudioOnlyStillImage to VIDEO Layer\n");
		}
		else if PARAM_IS("1")	// ScrambledStillImage play
		{
			iResult = DI_VIDEO_Stop(0);
			AssertDI(iResult, CMD_ERR);

			iResult = DI_VIDEO_PlayMemoryStream(0, DI_VIDEO_STREAM_TYPE_ES, DI_VIDEO_FORMAT_STILL_MPEG2, (HUINT8*)ScrambledStillImage, ulScrambledStillImage_size, NULL);
			AssertDI(iResult, CMD_ERR);

			CMD_Printf("Command : video still 1 -> Display ScrambledStillImage to VIDEO Layer\n");
		}
		else
		{
			CMD_Printf("Wrong command format! video still [0(audio only stilll play)/1(scrambled still play)/stop(video plane still display stop)]\n");
		}
	}
#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("cap")
	{
		DI_OSD_HANDLE CaptureOsdHandle;
		DI_OSD_RECT  Capture_Rect;
		DI_OSD_RECT  Target_Rect;
		DI_ERR_CODE	nErr = DI_ERR_OK;
		nErr=DI_OSD_CreateFrameBuffer(300,300,DI_OSD_PIXEL_FORMAT_ARGB_8888, &CaptureOsdHandle);
		if (nErr!=DI_ERR_OK)
		{
			VK_printf("CreateFrameBuffer error\n\n");
            return CMD_ERR;
		}
		Capture_Rect.x=0;
		Capture_Rect.y=0;
		Capture_Rect.width=300;
		Capture_Rect.height=300;

		Target_Rect.x=100;
		Target_Rect.y=100;
		Target_Rect.width=300;
		Target_Rect.height=300;

		nErr = DI_VIDEO_StartCapture(0, 0,0,(HUINT8*)&CaptureOsdHandle);
		if (nErr!=DI_ERR_OK)
		{
			VK_printf("DI_VIDEO_StartCapture error\n\n");
            return CMD_ERR;
		}

		if ( DRV_OSD_GetOsdHandle(DI_OSD_LAYER_0, &s_TEST_SURFACE_HANDLE) != DRV_OK )
        {
			VK_printf("DRV_OSD_GetOsdHandle error\n\n");
            return CMD_ERR;
        }
		nErr = DI_OSD_CopyOSDBlock(CaptureOsdHandle,  Capture_Rect, s_TEST_SURFACE_HANDLE, Target_Rect);
		if (nErr!=DI_ERR_OK)
		{
			VK_printf("DI_OSD_CopyOSDBlock error\n\n");
            return CMD_ERR;
		}
        nErr = DI_OSD_Flush(s_TEST_SURFACE_HANDLE, &Target_Rect);
		if (nErr!=DI_ERR_OK)
		{
			VK_printf("DI_OSD_Flush error\n\n");
            return CMD_ERR;
		}

		nErr = DI_OSD_DeleteFrameBuffer(CaptureOsdHandle);
		if (nErr!=DI_ERR_OK)
		{
			VK_printf("DI_OSD_DeleteFrameBuffer error\n\n");
			return CMD_ERR;
		}

		iResult=CMD_OK;
	}
	else if CMD_IS("screen_cap")
	{
		if PARAM_IS("on")
		{
			DI_OSD_HANDLE CaptureOsdHandle;
			DI_OSD_RECT  Capture_Rect;
			DI_OSD_RECT  Target_Rect;
			DI_ERR_CODE	nErr = DI_ERR_OK;
			nErr=DI_OSD_CreateFrameBuffer(1280,720,DI_OSD_PIXEL_FORMAT_ARGB_8888, &CaptureOsdHandle);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("CreateFrameBuffer error\n\n");
				return CMD_ERR;
			}
			Capture_Rect.x=0;
			Capture_Rect.y=0;
			Capture_Rect.width=1280;
			Capture_Rect.height=720;

			Target_Rect.x=0;
			Target_Rect.y=0;
			Target_Rect.width=1280;
			Target_Rect.height=720;

			nErr = DI_VIDEO_StartScreenCapture(0,DI_VIDEO_CAP_ALL,0,0,(HUINT8*)&CaptureOsdHandle);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_VIDEO_StartCapture error\n\n");
				return CMD_ERR;
			}

			if ( DRV_OSD_GetOsdHandle(DI_OSD_LAYER_0, &s_TEST_SURFACE_HANDLE) != DRV_OK )
			{
				VK_printf("DRV_OSD_GetOsdHandle error\n\n");
				return CMD_ERR;
			}
			nErr = DI_OSD_CopyOSDBlock(CaptureOsdHandle,  Capture_Rect, s_TEST_SURFACE_HANDLE, Target_Rect);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_CopyOSDBlock error\n\n");
				return CMD_ERR;
			}
			nErr = DI_OSD_Flush(s_TEST_SURFACE_HANDLE, &Target_Rect);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_Flush error\n\n");
				return CMD_ERR;
			}

			nErr = DI_OSD_DeleteFrameBuffer(CaptureOsdHandle);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_DeleteFrameBuffer error\n\n");
				return CMD_ERR;
			}

			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			DI_OSD_HANDLE CaptureOsdHandle;
			DI_OSD_RECT  Capture_Rect;
			DI_OSD_RECT  Target_Rect;
			DI_ERR_CODE	nErr = DI_ERR_OK;
			nErr=DI_OSD_CreateFrameBuffer(1280,720,DI_OSD_PIXEL_FORMAT_ARGB_8888, &CaptureOsdHandle);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("CreateFrameBuffer error\n\n");
				return CMD_ERR;
			}
			Capture_Rect.x=0;
			Capture_Rect.y=0;
			Capture_Rect.width=1280;
			Capture_Rect.height=720;

			Target_Rect.x=0;
			Target_Rect.y=0;
			Target_Rect.width=1280;
			Target_Rect.height=720;

			nErr=DI_OSD_FillOSDBlock(CaptureOsdHandle, Capture_Rect, 0x00000000);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_FillOSDBlock error\n\n");
				return CMD_ERR;
			}

			nErr = DI_OSD_CopyOSDBlock(CaptureOsdHandle,  Capture_Rect, s_TEST_SURFACE_HANDLE, Target_Rect);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_CopyOSDBlock error\n\n");
				return CMD_ERR;
			}
			nErr = DI_OSD_Flush(s_TEST_SURFACE_HANDLE, &Target_Rect);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_Flush error\n\n");
				return CMD_ERR;
			}

			nErr = DI_OSD_DeleteFrameBuffer(CaptureOsdHandle);
			if (nErr!=DI_ERR_OK)
			{
				VK_printf("DI_OSD_DeleteFrameBuffer error\n\n");
				return CMD_ERR;
			}

			iResult=CMD_OK;
		}
		else
		{
			iResult=CMD_OK;
		}


	}
#endif
	else if CMD_IS("setinout")
	{
		DI_VIDEO_RECT_t InRect;
		DI_VIDEO_RECT_t OutRect;
		HBOOL bTouched;
		DI_VIDEO_WIN_TYPE eType;
		HUINT32 ulDeviceId=0;

		char* szParm3=CMD_NextArg((char **)&szArgs);
		char* szParm4=CMD_NextArg((char **)&szArgs);
		char* szParm5=CMD_NextArg((char **)&szArgs);
		char* szParm6=CMD_NextArg((char **)&szArgs);
		char* szParm7=CMD_NextArg((char **)&szArgs);
		char* szParm8=CMD_NextArg((char **)&szArgs);
		char* szParm9=CMD_NextArg((char **)&szArgs);
		char* szParm10=CMD_NextArg((char **)&szArgs);

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		READABLE_IN_DEC(szParm9, eType);
		if ( (READABLE_IN_DEC(HWTEST_PARAM1, InRect.ulX) != 1 ) ||
            (READABLE_IN_DEC(HWTEST_PARAM2, InRect.ulY) != 1 ) ||
            (READABLE_IN_DEC(szParm3, InRect.ulWidth) != 1 ) ||
            (READABLE_IN_DEC(szParm4, InRect.ulHeight) != 1 ) ||
            (READABLE_IN_DEC(szParm5, OutRect.ulX) != 1 ) ||
            (READABLE_IN_DEC(szParm6, OutRect.ulY) != 1 ) ||
            (READABLE_IN_DEC(szParm7, OutRect.ulWidth) != 1 ) ||
            (READABLE_IN_DEC(szParm8, OutRect.ulHeight) != 1 ) )
		{
			InRect.ulX=0;
			InRect.ulY=0;
			InRect.ulWidth=0;
			InRect.ulHeight=0;
			OutRect.ulX=0;
			OutRect.ulY=0;
			OutRect.ulWidth=0;
			OutRect.ulHeight=0;
		}
		DI_VIDEO_SetInOutWindow(ulDeviceId, &InRect, &OutRect, &bTouched, eType);
		iResult=CMD_OK;
	}
	else if CMD_IS("osd_flush")
	{
		CMD_Printf("Use : osd refresh\r\n");
	}
	else if CMD_IS("osd_show")
	{
		CMD_Printf("Use : osd show\r\n");
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("osd show~~\n");
        }
	}
	else if CMD_IS("osd_hide")
	{
		CMD_Printf("Use : osd hide\r\n");
	}
	else if CMD_IS("show")
	{
		HUINT32 ulDeviceId = 0;
		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		CMD_Printf("Command : video show %s \r\n", (ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Main" : "PIP");
		DI_VIDEO_Show(ulDeviceId, TRUE);
		iResult=CMD_OK;
	}
	else if CMD_IS("hide")
	{
		HUINT32 ulDeviceId = 0;

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		CMD_Printf("Command : video hide %s \r\n", (ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Main" : "PIP");
		DI_VIDEO_Show(ulDeviceId, FALSE);
		iResult=CMD_OK;
	}
	else if CMD_IS("pause")
	{
		CMD_Printf("Command : video live pause\r\n");
		DI_VIDEO_Pause(0,TRUE);
		iResult=CMD_OK;
	}
	else if CMD_IS("resume")
	{
		CMD_Printf("Command : video live resume\r\n");
		DI_VIDEO_Pause(0,FALSE);
		iResult=CMD_OK;
	}

	else if CMD_IS("mnr")
	{
/*
		bsettop_display.c

		int32_t mnr_level[6] = {0,-75,-33,0,50,200};
		int32_t bnr_level[6] = {0,-75,-33,0,50,200};
		int32_t dcr_level[6] = {0,-90,-50,0,50,90};
*/
		if PARAM_IS("0")
		{
			DRV_VIDEO_SetMnr(0);
		}
		else if PARAM_IS("1")
		{
			DRV_VIDEO_SetMnr(-75);
		}
		else if PARAM_IS("2")
		{
			DRV_VIDEO_SetMnr(-33);
		}
		else if PARAM_IS("3")
		{
			DRV_VIDEO_SetMnr(0);
		}
		else if PARAM_IS("4")
		{
			DRV_VIDEO_SetMnr(50);
		}
		else if PARAM_IS("5")
		{
			DRV_VIDEO_SetMnr(200);
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("bnr")
	{
		if PARAM_IS("0")
		{
			DRV_VIDEO_SetBnr(0);
		}
		else if PARAM_IS("1")
		{
			DRV_VIDEO_SetBnr(-75);
		}
		else if PARAM_IS("2")
		{
			DRV_VIDEO_SetBnr(-33);
		}
		else if PARAM_IS("3")
		{
			DRV_VIDEO_SetBnr(0);
		}
		else if PARAM_IS("4")
		{
			DRV_VIDEO_SetBnr(50);
		}
		else if PARAM_IS("5")
		{
			DRV_VIDEO_SetBnr(200);
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("dcr")
	{
		if PARAM_IS("0")
		{
			DRV_VIDEO_SetDcr(0);
		}
		else if PARAM_IS("1")
		{
			DRV_VIDEO_SetDcr(-90);
		}
		else if PARAM_IS("2")
		{
			DRV_VIDEO_SetDcr(-50);
		}
		else if PARAM_IS("3")
		{
			DRV_VIDEO_SetDcr(0);
		}
		else if PARAM_IS("4")
		{
			DRV_VIDEO_SetDcr(50);
		}
		else if PARAM_IS("5")
		{
			DRV_VIDEO_SetDcr(90);
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("param") /*command for fine tuning of parameters: contrast, saturation, hue and brightness*/
	{
		int contrast=0, saturation=0, hue=0, brightness=0;
		GET_NEXT_ARG(szBrightness);

		if (READABLE_IN_DEC(HWTEST_PARAM, contrast) &&  READABLE_IN_DEC(HWTEST_PARAM1, saturation) &&
			READABLE_IN_DEC(HWTEST_PARAM2,hue) && READABLE_IN_DEC(szBrightness,brightness))
		{

			CMD_Printf("Command : video param contrast(%d) saturation(%ld) hue(%ld) brightness(%ld)\r\n",contrast, saturation,hue,brightness);
			DRV_VIDEO_SetDispParam(contrast,saturation,hue,brightness);
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("sync")
	{
		if PARAM_IS("on")
		{
			DRV_VIDEO_Restart(0,TRUE);
			CMD_Printf("Video Restart to TSM mode");
		}
		else if PARAM_IS("off")
		{
			DRV_VIDEO_Restart(0,FALSE);
			CMD_Printf("Video Restart to VSYNC mode");
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("delay")
	{
		HINT32 delayMs=0;
		DRV_Error dResult=DRV_OK;

		CMD_Printf("Command: video delay\n");
		if(szParam == NULL)
		{
			CMD_Printf("Invalid parameter! Delay amount in MilliSecond is needed!\n");
			iResult = CMD_ERR;
		}
		else
		{

			delayMs = VK_atoi((char*) szParam);

			dResult = DRV_VIDEO_DelayOutput(0, delayMs);
			if(dResult != DRV_OK)
			{
				CMD_Printf("DRV_VIDEO_DelayOutput failed!\n");
				iResult = CMD_ERR;
			}
			else
			{
				iResult = CMD_OK;
			}
		}
	}
	else if CMD_IS("start")
	{
		DI_ERR_CODE dResult;
		HUINT32	ulDeviceId=0;

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		CMD_Printf("Command: video start %s (mpeg2)\n", (ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Main" : "PIP");
		dResult = DI_VIDEO_Start(ulDeviceId, DI_VIDEO_FORMAT_MPEG2);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("DI_VIDEO_Start failed!\n");
			iResult = CMD_ERR;
		}
		else
		{
			iResult = CMD_OK;
		}
	}
	else if CMD_IS("stop")
	{
		DI_ERR_CODE dResult;
		HUINT32	ulDeviceId=0;

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		CMD_Printf("Command: video stop %s (mpeg2)\n", (ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Main" : "PIP");

		dResult = DI_VIDEO_Stop(ulDeviceId);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("DI_VIDEO_Stop failed!\n");
			iResult = CMD_ERR;
		}
		else
		{
			iResult = CMD_OK;
		}
	}
	else if CMD_IS("reset")
	{
		DI_ERR_CODE dResult;

		CMD_Printf("Command: video reset\n");
		dResult = DRV_VIDEO_ResetDecoder(0);
		iResult = CMD_OK;
	}
#if defined(CONFIG_SYNC_RECOVERY)
	else if CMD_IS("pcr")
	{
		unsigned int step=1000; /* Default 1 second */

		if(PcrTimerId != NULL)
			VK_TIMER_Cancel(PcrTimerId);

		VK_TIMER_EventEvery(step, CMD_HwTest_GetPcrTimer, NULL, 0, &PcrTimerId);
		iResult = CMD_OK;

	}
#endif
#if defined(CONFIG_VIDEO_ENCODER)
#if 0
	else if CMD_IS("enc")
	{
		if PARAM_IS("start")
		{
			int eCodec=0,eRes=0,eFrameRate=0;
			unsigned int step=200; /* Default 50ms */
			DI_VIDEO_ENC_Set_t Settings;
			char* szParm3=CMD_NextArg((char **)&szArgs);

			READABLE_IN_DEC(HWTEST_PARAM1,eCodec);
			READABLE_IN_DEC(HWTEST_PARAM2,eRes);
			READABLE_IN_DEC(szParm3,eFrameRate);

			Settings.eCodec = eCodec;
			Settings.eRes = eRes;
			Settings.eFrameRate = eFrameRate;

			DRV_VIDEO_EncStart(0,&Settings);
			DRV_VIDEO_EncWriteToFile();

			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DRV_VIDEO_EncStop(0);
			iResult = CMD_OK;
		}
		else if PARAM_IS("set")
		{
			int eRes=0,eFrameRate=0;
			READABLE_IN_DEC(HWTEST_PARAM1,eRes);
			READABLE_IN_DEC(HWTEST_PARAM2,eFrameRate);
			DRV_VIDEO_EncSettings(0, eRes, eFrameRate);
			iResult = CMD_OK;
		}
	}
#endif
#endif
#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
	else if CMD_IS("3dinfo")
	{
		DI_VIDEO_3D_FORMAT e3dFormat;

		DI_VIDEO_Get3DInfo(0, &e3dFormat);

		CMD_Printf("e3dFormat=%d\n", e3dFormat);

		iResult = CMD_OK;
	}
	else if CMD_IS("3dregcb")
	{
		HUINT32 ulCallerId = 0;
		DI_VIDEO_RegisterEventCallback(0, DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED, CMD_HwTest_3dInfoCallback, &ulCallerId);
		iResult = CMD_OK;
	}
	else if CMD_IS("3d")
	{
		if PARAM_IS("sbs")
		{
			DI_VIDEO_Set3DFormat(0,DI_VIDEO_3D_FORMAT_SIDEBYSIDE);
		}
		else if PARAM_IS("tb")
		{
			DI_VIDEO_Set3DFormat(0,DI_VIDEO_3D_FORMAT_TOPANDBOTTOM);
		}
		else if PARAM_IS("fp")
		{
			DI_VIDEO_Set3DFormat(0,DI_VIDEO_3D_FORMAT_FRAMEPACKING);
		}
		else if PARAM_IS("2d")
		{
			DI_VIDEO_Set3DFormat(0,DI_VIDEO_3D_FORMAT_NONE);
		}
		else
		{
			CMD_Printf("video 3d sbs/tb/fp/2d\n");
		}
		iResult=CMD_OK;
	}
#endif
#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
	else if CMD_IS("encinfo")
	{
		HUINT32 ulDeviceId=0;
		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		DRV_VIDEO_EncShowStatus(ulDeviceId);
		iResult = CMD_OK;
	}
	else if CMD_IS("transprobe")
	{
		HUINT32 ulDemuxId = 3;
		HUINT32 ulDeviceId = 0;
		HUINT32 ulMuxId = 0;
		HUINT32 ulNumOfDevice;
		DI_TRANSCODER_CAP_t ppCapInfo[2];
		Pid_Info pidInfo;
		DI_TRANSCODER_GetCapability(&ulNumOfDevice,&ppCapInfo);
		if PARAM_IS("main")
		{
			ulDeviceId=ulMuxId=0;
			pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_PCR_TRANS_1;
		}
		else if PARAM_IS("sub")
		{
			if(ulNumOfDevice == 2)
			{
				ulDeviceId=ulMuxId=1;
				pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_2 | PID_BITMASK_PCR_TRANS_2;
			}
			else
			{
				CMD_Printf(" Not support sub transcoder when enabling PIP function\n");
				return CMD_ERR;
			}
		}
		else
		{
			return CMD_ERR;
		}

		if PARAM1_IS("start")
		{
			pidInfo.usVideoPid = 0x1ff;
			pidInfo.usPCRPid = 0x1ff;
			DI_DEMUX_SetPath(ulDemuxId,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_TRANSCODER, 0,-1);
			DI_DEMUX_StartPID(ulDemuxId, (void *)&pidInfo);
			DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS, hwtest_TRANSCODER_SEQ_INFO_CB);
			DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL, hwtest_TRANSCODER_SEQ_INFO_CB);
			DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO, hwtest_TRANSCODER_SEQ_INFO_CB);
			DI_TRANSCODER_ProbeStart(ulDeviceId, ulDemuxId,DI_VIDEO_FORMAT_MPEG2);
		}
		else if PARAM1_IS("stop")
		{

			DI_TRANSCODER_ProbeStop(ulDeviceId);
			DI_DEMUX_StopPID(ulDemuxId, pidInfo.pidBitmask);
			DI_TRANSCODER_ProbeClose(ulDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
		}
		else
		{
			return CMD_ERR;
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("trans")
	{
		HUINT32 ulDemuxId = 0;
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId = 0;
		HUINT32 ulMuxId = 0;
		HUINT32 ulNumOfDevice;
		DI_TRANSCODER_CAP_t ppCapInfo[2];
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 psicontinuos = 0;
		DI_ERR_CODE	diErr = 0;
		static HUINT32 psiIndex[3] = {0,};

		DI_TRANSCODER_GetCapability(&ulNumOfDevice,&ppCapInfo);
		if PARAM_IS("main")
		{
			ulDeviceId=ulMuxId=0;
			ulRecDeviceId = 5;
		}
		else if PARAM_IS("sub")
		{
			if(ulNumOfDevice == 2)
			{
				ulDeviceId=ulMuxId=1;
				ulRecDeviceId = 6;
			}
			else
			{
				CMD_Printf(" Not support sub transcoder when enabling PIP function\n");
				return CMD_ERR;
			}
		}
		else
		{
			return CMD_ERR;
		}

		if PARAM1_IS("start")
		{
			if PARAM2_IS("video")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_VIDEO;
			else if PARAM2_IS("audio")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AUDIO;
			else
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;
#if 1
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_WATCH;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_720P_50HZ;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.bVideoDefaultProfile = TRUE;
			s_stSetting.ulDemuxId = ulDemuxId;
#else
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_WATCH;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_576P;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_32;
			s_stSetting.eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = TRUE;
			s_stSetting.stVideoSettings.stCustomRes.ulWidth = 1920;
			s_stSetting.stVideoSettings.stCustomRes.ulHeight = 1080;
			s_stSetting.stVideoSettings.stCustomRes.ulRefreshRate = 29970;
			s_stSetting.stVideoSettings.stCustomRes.bInterlaced = FALSE;
			s_stSetting.stVideoSettings.stCustomRes.eAspectRatio = 0;
			s_stSetting.stVideoSettings.ulMaxBitRate= 11520000;
			s_stSetting.stVideoSettings.stCustomIPB.bCustomIPB = TRUE;
			s_stSetting.stVideoSettings.stCustomIPB.ulFrameP = 24;
			s_stSetting.stVideoSettings.stCustomIPB.ulFrameB = 0;
			s_stSetting.bVideoDefaultProfile = FALSE;
			s_stSetting.ulDemuxId = ulDemuxId;
#endif
			diErr = DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);
			VK_printf("=== Trans : 1 (%d) ===\r\n",diErr);


			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x101;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x102;
			s_stMuxInfo.pcrPid=0x103;

			diErr = DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);
			VK_printf("=== Trans : 2 (%d) ===\r\n",diErr);
			psicontinuos = 0; /* 0 ==> Ã³?½ ÇÑ¹ø , 1==> ¿¬¼Ó?û?¸·Î */

			patPid = 0x00;
			pmtPid = 0x402;
			psiPid = 0x12;

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,48,patsectiondata,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);
			pmtsectiondata[8] |= (s_stMuxInfo.pcrPid >> 8);
			pmtsectiondata[9] = (s_stMuxInfo.pcrPid & 0x00FF);

			if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
				pmtsectiondata[18] = 0x1b;
			else
				pmtsectiondata[18] = 0x02;

			pmtsectiondata[19] |= (s_stMuxInfo.videoPid >> 8);
			pmtsectiondata[20] = (s_stMuxInfo.videoPid & 0x00FF);

			if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
				pmtsectiondata[26] = 0x0F;
			else
				pmtsectiondata[26] = 0x04;

			pmtsectiondata[27] |= (s_stMuxInfo.audioPid >> 8);
			pmtsectiondata[28] = (s_stMuxInfo.audioPid & 0x00FF);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,44,pmtsectiondata,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,237,eitsectiondata,psicontinuos,1000,&psiIndex[2]);
			VK_printf("== Trans PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);
			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"/mnt/hd2/trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulRecDeviceId);
			}
			CMD_Printf("hw test s/w transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM1_IS("update")
		{
			diErr = DI_TRANSCODER_UpdatePSIData(ulDeviceId,psiIndex[0],48,patsectiondata,1);
			VK_printf("== Trans Update PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);

			diErr = DI_TRANSCODER_UpdatePSIData(ulDeviceId,psiIndex[1],44,pmtsectiondata,1);
			VK_printf("== Trans Update PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_UpdatePSIData(ulDeviceId,psiIndex[2],237,eitsectiondata,1);
			VK_printf("== Trans Update PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);
			iResult = CMD_OK;

		}
		else if PARAM1_IS("bit")
		{
			HUINT32 ulMaxBitRate;
			DI_TRANSCODER_UpdateSettings_t stSettings;
			READABLE_IN_DEC(HWTEST_PARAM2, ulMaxBitRate);
			if(ulMaxBitRate >= 10000 || ulMaxBitRate <= 30000000)
			{
				DI_TRANSCODER_UpdateGetSettings(ulDeviceId, &stSettings);
				stSettings.stVideoSettings.ulMaxBitRate=ulMaxBitRate;
				stSettings.stVideoSettings.stCustomIPB.bCustomIPB=FALSE;
				stSettings.stVideoSettings.stCustomRes.bCustomRes=FALSE;
				VK_printf("update MaxBitRate = %d\n",ulMaxBitRate);
				DI_TRANSCODER_UpdateSetSettings(ulDeviceId, &stSettings);
				iResult = CMD_OK;
			}
			else
			{
				iResult = CMD_ERR;
			}
		}
		else if PARAM1_IS("res")
		{
			DI_VIDEO_HD_RESOLUTION eRes;
			DI_TRANSCODER_UpdateSettings_t stSettings;
			READABLE_IN_DEC(HWTEST_PARAM2, eRes);
			DI_TRANSCODER_UpdateGetSettings(ulDeviceId, &stSettings);
			stSettings.stVideoSettings.eRes=eRes;
			VK_printf("update Res = %d\n",eRes);
			DI_TRANSCODER_UpdateSetSettings(ulDeviceId, &stSettings);
			iResult = CMD_OK;

		}
		else if PARAM1_IS("stop")
		{
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
			DI_TRANSCODER_Close(ulDeviceId);
			CMD_Printf("hw test s/w transcoding stop!!\n");
			iResult = CMD_OK;
		}

	}
#if 0
	else if CMD_IS("pbtrans")
	{
		DI_ERR_CODE diErr = DI_ERR_OK;
		DRV_Error	drvErr = DRV_ERR;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		DI_VIDEO_FORMAT nVidFmt;
		DI_AUDIO_FORMAT nAudFmt;
		DI_DEMUX_INPUTDEVICETYPE inDevType;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 usVideoPid;
		HUINT16 usAudioPid;
		HUINT16 usPcrPid;
		int buffer[128];
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId;
		HUINT32 ulPbDeviceId;
		HUINT32 ulDemuxId;
		HUINT32 ulMuxId = 0;
		HUINT16 psicontinuos = 0;
		static HUINT32 psiIndex[3] = {0,};
		HUINT32	inDevId;
		Pid_Info pidInfo;

		usAudioPid = 0x210;
		usPcrPid = 0x210;

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "/mnt/hd2/hdd_tsr_test\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = false;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulPacketSize = 188;
		playSetup.ulStartTime = 0;

		stSetup = &playSetup;
		nAudFmt = DI_AUDFORMAT_MPEG;

		inDevType = DI_DEMUX_INPUT_PLAYER;
		inDevId = stSetup->ulStartTime;


		ulPbDeviceId = 1;
		ulRecDeviceId = 7;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex()+1;

		CMD_Printf("(demuxId = %lu recordId = %lu, pbId = %lu) for transcoder device %lu\n", ulDemuxId, ulRecDeviceId, ulPbDeviceId, ulDeviceId);

		if PARAM_IS("start")
		{
			diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_TRANSCODER, 0, -1);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_SetPath\n\r");
				return CMD_ERR;
			}
			stSetup->ulDemuxId = ulDemuxId;
			diErr = DI_PVR_PLAY_Setup(ulPbDeviceId, stSetup);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
				return CMD_ERR;
			}
			pidInfo.pidBitmask = PID_BITMASK_AUDIO_TRANS_1 | PID_BITMASK_PCR_TRANS_1;
			pidInfo.usAudioMainPid = usAudioPid;
			pidInfo.usPCRPid = usPcrPid;
			diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_StartPID\n\r");
				return CMD_ERR;
			}
			diErr = DI_PVR_PLAY_Start(ulPbDeviceId);

			s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AUDIO;
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.ulDemuxId = ulDemuxId;
			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x1101;
			s_stMuxInfo.pcrPid=0x1102;
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, hwtest_TRANS_TEST_CB);
			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);

			VK_printf("=== Trans : 2 (%d) ===\r\n",diErr);
			psicontinuos = 1; /* 0 ==> Ã³?½ ÇÑ¹ø , 1==> ¿¬¼Ó?û?¸·Î */

			patPid = 0x00;
			pmtPid = 0x402;
			psiPid = 0x12;

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,48,patsectiondata,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);
			pmtsectiondata[8] |= (s_stMuxInfo.pcrPid >> 8);
			pmtsectiondata[9] = (s_stMuxInfo.pcrPid & 0x00FF);

			if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
				pmtsectiondata[18] = 0x1b;
			else
				pmtsectiondata[18] = 0x02;

			pmtsectiondata[19] |= (s_stMuxInfo.videoPid >> 8);
			pmtsectiondata[20] = (s_stMuxInfo.videoPid & 0x00FF);

			if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
				pmtsectiondata[26] = 0x0F;
			else
				pmtsectiondata[26] = 0x04;

			pmtsectiondata[27] |= (s_stMuxInfo.audioPid >> 8);
			pmtsectiondata[28] = (s_stMuxInfo.audioPid & 0x00FF);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,44,pmtsectiondata,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,237,eitsectiondata,psicontinuos,1000,&psiIndex[2]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				s_stRecSetup.ulPacketSize = 188;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"/mnt/hd2/trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulRecDeviceId);
			}
			CMD_Printf("hw test s/w tsr transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, NULL);
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
#if 1
			DI_DEMUX_StopPID(ulDemuxId, (HUINT32)PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1| PID_BITMASK_PCR_TRANS_1);
			DI_TRANSCODER_Close(ulDeviceId);
#endif
			DI_PVR_PLAY_Stop(ulPbDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
			CMD_Printf("hw test s/w tsr transcoding stop!!\n");
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
		}
	}
#endif
	else if CMD_IS("tsrtrans")
	{
		DI_ERR_CODE diErr = DI_ERR_OK;
		DRV_Error	drvErr = DRV_ERR;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		DI_VIDEO_FORMAT nVidFmt;
		DI_AUDIO_FORMAT nAudFmt;
		DI_DEMUX_INPUTDEVICETYPE inDevType;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 usVideoPid;
		HUINT16 usAudioPid;
		HUINT16 usPcrPid;
		int buffer[128];
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId;
		HUINT32 ulPbDeviceId;
		HUINT32 ulDemuxId;
		HUINT32 ulMuxId = 0;
		HUINT16 psicontinuos = 0;
		static HUINT32 psiIndex[3] = {0,};
		HUINT32	inDevId;
		Pid_Info pidInfo;

		usVideoPid = 0x17DE;
		usAudioPid = 0x17E8;
		usPcrPid = 0x17DE;

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "/mnt/hd2/hdd_tsr_test\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = true;
		playSetup.eFileType = eDI_PVR_FileType_Circular;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = 0;

		stSetup = &playSetup;
		nVidFmt = DI_VIDEO_FORMAT_H264;
		nAudFmt = DI_AUDFORMAT_MPEG;

		inDevType = DI_DEMUX_INPUT_RECORDER;
		inDevId = stSetup->ulStartTime;


		ulPbDeviceId = 1;
		ulRecDeviceId = 7;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex()+1;

		CMD_Printf("(demuxId = %lu recordId = %lu, pbId = %lu) for transcoder device %lu\n", ulDemuxId, ulRecDeviceId, ulPbDeviceId, ulDeviceId);

		if PARAM_IS("start")
		{
			diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_TRANSCODER, 0, -1);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_SetPath\n\r");
				return CMD_ERR;
			}
			stSetup->ulDemuxId = ulDemuxId;
			diErr = DI_PVR_PLAY_Setup(ulPbDeviceId, stSetup);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
				return CMD_ERR;
			}
			pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1| PID_BITMASK_PCR_TRANS_1;
			pidInfo.usVideoPid = usVideoPid;
			pidInfo.usAudioMainPid = usAudioPid;
			pidInfo.usPCRPid = usPcrPid;
			diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_StartPID\n\r");
				return CMD_ERR;
			}
			s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_576P;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.bVideoDefaultProfile = TRUE;
			s_stSetting.ulDemuxId = ulDemuxId;
			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x1100;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x1101;
			s_stMuxInfo.pcrPid=0x1102;
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, hwtest_TRANS_TEST_CB);
			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);
			psicontinuos = 0;

			diErr = DI_PVR_PLAY_Start(ulPbDeviceId);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			patPid = 0x00;
			pmtPid = 0x55;
			psiPid = 0x12;

			s_auiTSPacket_PAT[182] |= (pmtPid >> 8);
			s_auiTSPacket_PAT[183] = (pmtPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PAT,psicontinuos,1000,&psiIndex[0]);
			CMD_Printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);

			s_auiTSPacket_PMT[170] |= (s_stMuxInfo.pcrPid >> 8);
			s_auiTSPacket_PMT[171] = (s_stMuxInfo.pcrPid & 0x00FF);
			s_auiTSPacket_PMT[175] |= (s_stMuxInfo.videoPid >> 8);
			s_auiTSPacket_PMT[176] = (s_stMuxInfo.videoPid & 0x00FF);
			s_auiTSPacket_PMT[180] |= (s_stMuxInfo.audioPid >> 8);
			s_auiTSPacket_PMT[181] = (s_stMuxInfo.audioPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PMT,psicontinuos,1000,&psiIndex[1]);
			CMD_Printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,PSI_DEFAULT_SIZE,s_auiTSPacket_EIT,psicontinuos,1000,&psiIndex[2]);
			CMD_Printf("== Trans PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);
			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"/mnt/hd2/trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulRecDeviceId);
			}
			CMD_Printf("hw test s/w tsr transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, NULL);
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
#if 0
			DI_DEMUX_StopPID(ulDemuxId, (HUINT32)PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1| PID_BITMASK_PCR_TRANS_1);
			DI_TRANSCODER_Close(ulDeviceId);
#endif
			DI_PVR_PLAY_Stop(ulPbDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
			CMD_Printf("hw test s/w tsr transcoding stop!!\n");
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
		}
	}
	else if CMD_IS("memtrans")
	{
		HUINT32 ulDemuxId = 0;
		HUINT32 ulDeviceId = 0;
		HUINT32 ulMuxId = 0;
		HUINT32 ulNumOfDevice;
		Pid_Info pidInfo;
		DI_TRANSCODER_CAP_t ppCapInfo[2];
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		char buffer[128];
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 psicontinuos = 0;
		DI_ERR_CODE	diErr = 0;
		static HUINT32 psiIndex[3] = {0,};

		DI_TRANSCODER_GetCapability(&ulNumOfDevice,&ppCapInfo);
		if PARAM_IS("main")
		{
			ulDemuxId=0;
			ulDeviceId=ulMuxId=0;
		}
		else
		{
			return CMD_ERR;
		}

		if PARAM1_IS("start")
		{
#if defined(CONFIG_TRANSCODER_MEMIO)
			DI_TRANSCODE_RegisterCallback(ulDeviceId, hwtest_TRANCODE_DataRdy);
#endif
			if PARAM2_IS("video")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_VIDEO;
			else if PARAM2_IS("audio")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AUDIO;
			else
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;

			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_WATCH;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_576P;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.bVideoDefaultProfile = TRUE;
			s_stSetting.ulDemuxId = ulDemuxId;

			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x11;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x12;
			s_stMuxInfo.pcrPid=0x13;

			psicontinuos = 0; /* 0 ==> Ã³?½ ÇÑ¹ø , 1==> ¿¬¼Ó?û?¸·Î */

			patPid = 0x00;
			pmtPid = 0x55;
			psiPid = 0x12;

			s_auiTSPacket_PAT[182] |= (pmtPid >> 8);
			s_auiTSPacket_PAT[183] = (pmtPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PAT,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);

			s_auiTSPacket_PMT[170] |= (s_stMuxInfo.pcrPid >> 8);
			s_auiTSPacket_PMT[171] = (s_stMuxInfo.pcrPid & 0x00FF);
			s_auiTSPacket_PMT[175] |= (s_stMuxInfo.videoPid >> 8);
			s_auiTSPacket_PMT[176] = (s_stMuxInfo.videoPid & 0x00FF);
			s_auiTSPacket_PMT[180] |= (s_stMuxInfo.audioPid >> 8);
			s_auiTSPacket_PMT[181] = (s_stMuxInfo.audioPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PMT,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,PSI_DEFAULT_SIZE,s_auiTSPacket_EIT,psicontinuos,1000,&psiIndex[2]);
			VK_printf("== Trans PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);

			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);

			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 6,"memio\0");
				DI_TRANSCODE_REC_Setup(ulDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulDeviceId);
			}
			CMD_Printf("hw test s/w transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM1_IS("stop")
		{
			DI_TRANSCODE_REC_Stop(ulDeviceId);
			DI_TRANSCODE_REC_Unset(ulDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);

			CMD_Printf("hw test s/w transcoding stop!!\n");
			iResult = CMD_OK;
		}

	}
#define TRANS_1_PCR_PID 0x206
#define TRANS_1_VIDEO_PID 0x206
#define TRANS_1_AUDIO_PID 0x2c6
#define TRANS_2_PCR_PID 0x202
#define TRANS_2_VIDEO_PID 0x202
#define TRANS_2_AUDIO_PID 0x29e

	else if CMD_IS("transback")
	{
		Pid_Info pidInfo;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		HUINT32 ulDemuxId = 3;
		HUINT32 ulDeviceId = 0;
		HUINT32 ulMuxId = 0;
		HUINT32 ulNumOfDevice;
		DI_TRANSCODER_CAP_t ppCapInfo[2];
		char ucChannelLock[50] = "sat 0 0 1 1130 27250 0 1 2 1 0";
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 psicontinuos = 0;
		DI_ERR_CODE	diErr = 0;
		static HUINT32 psiIndex[3] = {0,};

		DI_TRANSCODER_GetCapability(&ulNumOfDevice,&ppCapInfo);
		if PARAM_IS("main")
		{
			ulDemuxId=3;
			ulDeviceId=ulMuxId=0;
			pidInfo.pidBitmask = PID_BITMASK_PCR_TRANS_1 | PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_AUDIO_TRANS_1;
			pidInfo.usPCRPid = TRANS_1_PCR_PID;
			pidInfo.usVideoPid = TRANS_1_VIDEO_PID;
			pidInfo.usAudioMainPid = TRANS_1_AUDIO_PID;
		}
		else if PARAM_IS("sub")
		{
			if(ulNumOfDevice == 2)
			{
				ulDemuxId=2;
				ulDeviceId=ulMuxId=1;
				pidInfo.pidBitmask = PID_BITMASK_PCR_TRANS_2 | PID_BITMASK_VIDEO_TRANS_2 | PID_BITMASK_AUDIO_TRANS_2;
				pidInfo.usPCRPid = TRANS_2_PCR_PID;
				pidInfo.usVideoPid = TRANS_2_VIDEO_PID;
				pidInfo.usAudioMainPid = TRANS_2_AUDIO_PID;
			}
			else
			{
				CMD_Printf(" Not support sub transcoder when enabling PIP function\n");
				return CMD_ERR;
			}
		}
		else
		{
			return CMD_ERR;
		}


		if PARAM1_IS("start")
		{
#if defined(CONFIG_CHANNEL)
			CMD_HwTest_Parser_ChLock(ucChannelLock);
			VK_TASK_Sleep(5000);
#endif

			DI_DEMUX_SetPath(ulDemuxId,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_TRANSCODER, 0,-1);
			DI_DEMUX_StartPID(ulDemuxId, (void *)&pidInfo);

			if PARAM2_IS("video")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_VIDEO;
			else if PARAM2_IS("audio")
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AUDIO;
			else
				s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;

			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_576P;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_MPEG;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.bVideoDefaultProfile = TRUE;
			s_stSetting.ulDemuxId = ulDemuxId;

			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x11;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x12;
			s_stMuxInfo.pcrPid=0x13;

			psicontinuos = 0; /* 0 ==> Ã³?½ ÇÑ¹ø , 1==> ¿¬¼Ó?û?¸·Î */

			patPid = 0x00;
			pmtPid = 0x55;
			psiPid = 0x12;

			s_auiTSPacket_PAT[182] |= (pmtPid >> 8);
			s_auiTSPacket_PAT[183] = (pmtPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PAT,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);

			s_auiTSPacket_PMT[170] |= (s_stMuxInfo.pcrPid >> 8);
			s_auiTSPacket_PMT[171] = (s_stMuxInfo.pcrPid & 0x00FF);
			s_auiTSPacket_PMT[175] |= (s_stMuxInfo.videoPid >> 8);
			s_auiTSPacket_PMT[176] = (s_stMuxInfo.videoPid & 0x00FF);
			s_auiTSPacket_PMT[180] |= (s_stMuxInfo.audioPid >> 8);
			s_auiTSPacket_PMT[181] = (s_stMuxInfo.audioPid & 0x00FF);
			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PMT,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,PSI_DEFAULT_SIZE,s_auiTSPacket_EIT,psicontinuos,1000,&psiIndex[2]);
			VK_printf("== Trans PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);

			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);
			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"/mnt/hd2/trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulDeviceId);
			}
			iResult = CMD_OK;
		}
		else if PARAM1_IS("stop")
		{
			DI_TRANSCODE_REC_Stop(ulDeviceId);
			DI_TRANSCODE_REC_Unset(ulDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
			DI_DEMUX_StopPID(ulDemuxId,pidInfo.pidBitmask);
			DI_DEMUX_ReleasePath(ulDemuxId);
			iResult = CMD_OK;
		}
	}
#endif
#if defined(CONFIG_PIP)
	/* Need hwtest cmd "chlock sat 0 0 1 1130 27250 0 1 2 1 0" */
	else if CMD_IS("master")
	{
		Pid_Info pidInfo;
		char ucChannelLock[50] = "sat 0 0 1 1130 27250 0 1 2 1 0";

#if defined(CONFIG_CHANNEL)
		CMD_HwTest_Parser_ChLock(ucChannelLock);
		VK_TASK_Sleep(5000);
#endif
		pidInfo.pidBitmask = PID_BITMASK_PCR | PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN;
		pidInfo.usPCRPid = 0x206;
		pidInfo.usVideoPid = 0x206;

		DI_VIDEO_Stop(0);
		DI_AUDIO_Stop(0);
		DI_DEMUX_StopPID(0,pidInfo.pidBitmask);
		DI_DEMUX_ReleasePath(0);

		pidInfo.pidBitmask = PID_BITMASK_PCR | PID_BITMASK_VIDEO;

		DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0,-1);
		DI_DEMUX_StartPID(0, (void *)&pidInfo);
		DI_VIDEO_Start(0,DI_VIDEO_FORMAT_MPEG2);	//DI_VIDEO_CODEC_H264
		DI_VIDEO_Show(0,TRUE);
	}
	else if CMD_IS("pip")
	{
		Pid_Info pidInfo;
		char ucChannelLock[50] = "sat 0 0 1 1130 27250 0 1 2 1 0";
#if defined(CONFIG_CHANNEL)
		CMD_HwTest_Parser_ChLock(ucChannelLock);
		VK_TASK_Sleep(5000);
#endif
		pidInfo.pidBitmask = PID_BITMASK_PCR_SUB | PID_BITMASK_VIDEO_SUB;
		pidInfo.usPCRPid = 0x202;
		pidInfo.usVideoPid = 0x202;

		DI_VIDEO_Stop(1);
		DI_DEMUX_StopPID(1,pidInfo.pidBitmask);
		DI_DEMUX_ReleasePath(1);

		DI_DEMUX_SetPath(1,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0,-1);
		DI_DEMUX_StartPID(1, (void *)&pidInfo);
		DI_VIDEO_Start(1,DI_VIDEO_FORMAT_MPEG2);	//DI_VIDEO_CODEC_H264
		DI_VIDEO_Show(1,TRUE);
	}
#endif
	else if CMD_IS("background")
	{
		DI_VIDEO_COLOR_t color;
		HUINT32 ulDeviceId=0;
		HUINT32 input_color=0;

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		READABLE_IN_HEX(HWTEST_PARAM1, input_color);

		color.a = (HUINT8)((input_color&0xFF000000)>>24);
		color.r = (HUINT8)((input_color&0x00FF0000)>>16);
		color.g = (HUINT8)((input_color&0x0000FF00)>>8);
		color.b = (HUINT8)((input_color&0x000000FF));

		DI_VIDEO_SetBackground(ulDeviceId, color);
		iResult=CMD_OK;
	}
	else if CMD_IS("deinter")
	{
		if PARAM_IS("on")
		{
			DRV_VIDEO_SetDeinterlaced(0,TRUE,TRUE);

			iResult = CMD_OK;
		}
		else if PARAM_IS("off")
		{
			DRV_VIDEO_SetDeinterlaced(0,FALSE,TRUE);
			iResult = CMD_OK;
		}


	}
	else if CMD_IS("aspect")
	{
		if PARAM_IS("0")
		{
			DI_VIDEO_SetAspect(0,0);
			iResult = CMD_OK;
		}
		else if PARAM_IS("1")
		{
			DI_VIDEO_SetAspect(0,1);
			iResult = CMD_OK;
		}


	}
	else if CMD_IS("wsscgms")
	{
		DI_VIDEO_WSS_SETTINGS_t stSettings;
		if PARAM_IS("on")
		{
			stSettings.eCpbit = DI_VIDEO_WSS_CP;
			stSettings.eGebit = DI_VIDEO_WSS_GE_REST;
			CMD_Printf(" wss cgms on %d %d \n",stSettings.eCpbit, stSettings.eGebit);
			DI_VIDEO_SetWSS(TRUE, stSettings);
			iResult = CMD_OK;
		}
		else if PARAM_IS("off")
		{
			DI_VIDEO_SetWSS(FALSE, stSettings);
			CMD_Printf(" wss cgms off\n");
			iResult = CMD_OK;
		}
	}
#if defined(CONFIG_DI20)
	else if CMD_IS("flush")
	{
		DI_ERR_CODE diResult;

		if PARAM_IS("main")
		{
			diResult = DI_VIDEO_Flush(DI_VIDEO_DECODER_MAIN);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_VIDEO_Flush failed!\n");
			}

		}
		else if PARAM_IS("sub")
		{
			diResult = DI_VIDEO_Flush(DI_VIDEO_DECODER_SUB);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_VIDEO_Flush failed!\n");
			}
		}
	}
#endif
#if defined(CONFIG_MICOM_NONE)
	else if CMD_IS("standby")
	{
		if PARAM_IS("on") {
			DRV_VIDEO_GetCurrentOutputStatus(&OutputStatusBak);
			OutputStatus.bYPbPr = 0;
			OutputStatus.bYPbPr_Downscale = 0;
			OutputStatus.bRGB = 0;
			OutputStatus.bCVBS = 0;
			OutputStatus.bSVHS = 0;
			DI_VIDEO_SetOutputSettings(OutputStatus);
			CMD_Printf("Command : Video Stanby On\n");
		}
		else if PARAM_IS("off") {
			DI_VIDEO_SetOutputSettings(OutputStatusBak);
			CMD_Printf("Command : Video Stanby Off\n");
		}

		iResult = CMD_OK;
	}
#endif
#if defined(CONFIG_IPSTREAM) && defined(CONFIG_IPSTREAM_MULTICAST)
	else if CMD_IS("multicast")
	{
		/* SD = 0, HD = 1, Color = 2 */
		/* Video Codec  H.264 = 128, MPEG2 = 2*/
		/* Audio Codec  MPEG = 1, AC3 = 64*/
		HUINT32 ulPlayType=0, ulVidPid = 0, ulAudPid = 0;
		DI_VIDEO_FORMAT eVideoCodec;
		DI_AUDIO_FORMAT eAudioCodec;

		char* szParm3=CMD_NextArg((char **)&szArgs);
		char* szParm4=CMD_NextArg((char **)&szArgs);
		char* szParm5=CMD_NextArg((char **)&szArgs);

		READABLE_IN_DEC(HWTEST_PARAM1, ulPlayType);
		READABLE_IN_DEC(HWTEST_PARAM2, ulVidPid);
		READABLE_IN_DEC(szParm3, ulAudPid);
		READABLE_IN_DEC(szParm4, eVideoCodec);
		READABLE_IN_DEC(szParm5, eAudioCodec);

		if PARAM_IS("start") {
			DRV_MULTICAST_Start(ulPlayType, ulVidPid, ulAudPid, eVideoCodec, eAudioCodec);
			CMD_Printf("Command : ipstream play start\n");
		}
		else if PARAM_IS("stop") {
			DRV_MULTICAST_Stop();
			CMD_Printf("Command : ipstream play stop\n");
		}

		iResult = CMD_OK;

	}
#endif

#if defined(CONFIG_VIDEO_MOSAIC_CNT)
	else if CMD_IS("mosaic")
	{
		int index = 0;
		DI_VIDEO_RECT_t InRect;
		DI_VIDEO_RECT_t OutRect;
		//int usWidth = 1280,usHeight=720;		
		//int usWidth = 720,usHeight=480;
		int usWidth = 1920,usHeight=1080;
		//int usWidth = 3840,usHeight=2160;
		int DeviceID=0;
		int MosaicID=0;

		READABLE_IN_DEC(HWTEST_PARAM1,DeviceID);
		READABLE_IN_DEC(HWTEST_PARAM2, MosaicID);

		/** Tuner1 -> Demux1-> Video(0/1)*/
		if PARAM_IS("on")
		{
			Pid_Info pidInfo;
			char ucChannelLock[50] = "cab 1 746 5274 0 0 0";
#if defined(CONFIG_CHANNEL)
			CMD_HwTest_Parser_ChLock(ucChannelLock);
			VK_TASK_Sleep(5000);
#endif
			if(DeviceID == 1)
			{
				pidInfo.pidBitmask = PID_BITMASK_VIDEO_SUB;
				pidInfo.usVideoPid = 512;

			}
			else
			{
				pidInfo.pidBitmask = PID_BITMASK_VIDEO;
				pidInfo.usVideoPid = 512;
			}

			DI_DEMUX_SetPath(1,DI_DEMUX_INPUT_TUNER, 1, DI_DEMUX_OUTPUT_DECODER, DeviceID,-1);
			DI_DEMUX_StartPID(1, (void *)&pidInfo);

			DI_VIDEO_StartMosaic(DeviceID,DI_VIDEO_FORMAT_H264);

			VK_TASK_Sleep(5000);

			for(index = 0; index<CONFIG_VIDEO_MOSAIC_CNT; index++)
			{
				InRect.ulX = (1920/2) & ~0x01;
				InRect.ulY = ((index%8)*((1080/8))) & ~0x01;
				InRect.ulWidth = ((1920)/8) & ~0x01;
				InRect.ulHeight = ((1080)/8) & ~0x01;
				OutRect.ulX = ((usWidth/2) & ~0x01)+30 ;
				OutRect.ulY = ((index%8)*((usHeight/8))) & ~0x01 ;
				OutRect.ulWidth = ((usWidth)/8) & ~0x01 ;
				OutRect.ulHeight = ((usHeight)/8) & ~0x01;
				DI_VIDEO_SetMosaicWindowSettings(DeviceID, index, &InRect ,&OutRect, DI_VIDEO_WIN_ALL);
			}

			for(index = 0; index<CONFIG_VIDEO_MOSAIC_CNT; index++)
			{
				DI_VIDEO_ShowMosaicWindow(DeviceID,index,TRUE);
			}
		}
		else if PARAM_IS("off")
		{
			Pid_Info pidInfo;
#if defined(CONFIG_CHANNEL)
			char ucChannelLock[50] = "cancel";

			CMD_HwTest_Parser_ChLock(ucChannelLock);
#endif
			DI_VIDEO_StopMosaic(DeviceID);
			DI_DEMUX_ReleasePath(1);

			if(DeviceID == 1)
			{
				pidInfo.pidBitmask = PID_BITMASK_VIDEO_SUB;
				pidInfo.usVideoPid = 512;

			}
			else
			{
				pidInfo.pidBitmask = PID_BITMASK_VIDEO;
				pidInfo.usVideoPid = 512;
			}
			DI_DEMUX_StopPID(1,(void *)&pidInfo);

		}
		else if PARAM_IS("show")
		{
			DI_VIDEO_ShowMosaicWindow(DeviceID,MosaicID,TRUE);
		}
		else if PARAM_IS("hide")
		{
			DI_VIDEO_ShowMosaicWindow(DeviceID,MosaicID,FALSE);
		}
		iResult = CMD_OK;

	}
#endif
	else if CMD_IS("ccm")
	{
		DI_ERR_CODE diResult;
		HUINT32 ulDeviceId=0;
		HUINT32 mode=0;

		READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId);
		READABLE_IN_DEC(HWTEST_PARAM1, mode);

		diResult = DI_VIDEO_SetChannelChangeMode(ulDeviceId, (DI_VIDEO_CC_MODE)mode);
		if(diResult != DI_ERR_OK)
		{
			CMD_Printf("DI_VIDEO_SetChannelChangeMode failed!\n");
			iResult = CMD_ERR;
		}
		else
		{
			CMD_Printf("Command : Set Channel Change mode [%d]\n",mode);
			iResult = CMD_OK;
		}
	}
	else
	{
		CMD_Printf("Wrong command format! [command: video bar/sd/comp/rgb/vf/info/still/cap/show/hide/vfsd]\n");
	}

	return iResult;

}

#if defined(CONFIG_AUDIO_DECODER) && defined(CONFIG_VIDEO_DECODER)
int CMD_HwTest_Parser_AV(void *szArgs)
{
	int ret;

	GET_ARGS;

	if CMD_IS("test")
	{
		if PARAM_IS("qam")
		{
			CMD_Printf("Command : av test qam\r\n");

			iResult=CMD_OK;
		}
		else if PARAM_IS("ofdm")
		{
			CMD_Printf("Command : av test ofdm\r\n");

			iResult=CMD_OK;
		}
		else if PARAM_IS("qpsk")
		{
			CMD_Printf("Command : av test qpsk\r\n");

			iResult=CMD_OK;
		}
		else if PARAM_IS("asia")
		{
			CMD_Printf("Command : av test asia\r\n");
			ret = HWTest_AV_Play_ASIA();
			if(ret != CMD_OK)
			{
				CMD_Printf("Command failed\r\n");
				iResult = CMD_ERR;
				goto TEST_AV_EXIT;

			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("custom")
		{
			int tunerId = -1;
            int vidPid = 0x1FFF;
            int audPid = 0x1FFF;
            int pcrPid = 0x1FFF;
            int nCI_Id = -1;
			DI_VIDEO_FORMAT eVideoFormat=0;
			DI_AUDIO_FORMAT eAudioFormat=0;

			GET_NEXT_ARG(szAudioPid);
			GET_NEXT_ARG(szPCRPid);
			GET_NEXT_ARG(szVideoCodec);
			GET_NEXT_ARG(szAudioCodec);
			GET_NEXT_ARG(szCI_ID);

			if (READABLE_IN_DEC(HWTEST_PARAM1, tunerId) &&  READABLE_IN_DEC(HWTEST_PARAM2, vidPid) &&
				READABLE_IN_DEC(szAudioPid,audPid) && READABLE_IN_DEC(szPCRPid,pcrPid))
			{
			    if ( szVideoCodec != NULL )
                {
				eVideoFormat = HWTest_AV_GetVideoFormat(szVideoCodec);
                }

                if ( szAudioCodec != NULL )
                {
				eAudioFormat = HWTest_AV_GetAudioFormat(szAudioCodec);
                }
				CMD_Printf("Command : av test custom tunerid(%d) videoPid(%ld) audioPid(%ld) pcrPid(%ld) vidcodec(%d) audcodec(%d)\r\n",tunerId, vidPid,audPid,pcrPid,eVideoFormat, eAudioFormat);
			}
			if(READABLE_IN_DEC(szCI_ID,nCI_Id)!=1)
			{
				nCI_Id = -1;
			}
			ret = HWTest_AV_Play(tunerId, vidPid, audPid, pcrPid,eVideoFormat, eAudioFormat, nCI_Id);
			if(ret != CMD_OK)
			{
				CMD_Printf("Command failed\r\n");
				iResult = CMD_ERR;
				goto TEST_AV_EXIT;
			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("customavonly")
		{
			int tunerId = -1;
            int vidPid = 0x1FFF;
            int audPid = 0x1FFF;
            int pcrPid = 0x1FFF;
            int nCI_Id = -1;
			DI_VIDEO_FORMAT eVideoFormat=0;
			DI_AUDIO_FORMAT eAudioFormat=0;

			GET_NEXT_ARG(szAudioPid);
			GET_NEXT_ARG(szPCRPid);
			GET_NEXT_ARG(szVideoCodec);
			GET_NEXT_ARG(szAudioCodec);
			GET_NEXT_ARG(szCI_ID);

			if (READABLE_IN_DEC(HWTEST_PARAM1, tunerId) &&  READABLE_IN_DEC(HWTEST_PARAM2, vidPid) &&
				READABLE_IN_DEC(szAudioPid,audPid) && READABLE_IN_DEC(szPCRPid,pcrPid))
			{
			    if ( szVideoCodec != NULL )
                {
				eVideoFormat = HWTest_AV_GetVideoFormat(szVideoCodec);
                }

                if ( szAudioCodec != NULL )
                {
				eAudioFormat = HWTest_AV_GetAudioFormat(szAudioCodec);
                }
				CMD_Printf("Command : av test custom2 tunerid(%d) videoPid(%ld) audioPid(%ld) pcrPid(%ld) vidcodec(%d) audcodec(%d)\r\n",tunerId, vidPid,audPid,pcrPid,eVideoFormat, eAudioFormat);
			}
			if(READABLE_IN_DEC(szCI_ID,nCI_Id)!=1)
			{
				nCI_Id = -1;
			}
			ret = HWTest_AV_PlayOnly(tunerId, vidPid, audPid, pcrPid,eVideoFormat, eAudioFormat, nCI_Id);
			if(ret != CMD_OK)
			{
				CMD_Printf("Command failed\r\n");
				iResult = CMD_ERR;
				goto TEST_AV_EXIT;
			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_VIDEO_Stop(0);
			DI_AUDIO_Stop(0);
			DI_DEMUX_StopPID(0, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);
			DI_DEMUX_ReleasePath(0);
		}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
		else if PARAM_IS("fccaging")
		{
			/* Primer Index */
			#define PRIMER_0 0
			#define PRIMER_1 1
			#define PRIMER_2 2

			/* Demux Device Index */
			#define DEMUX_0 0
			#define DEMUX_1 1
			#define DEMUX_2 2

			/* Video Device Index */
			#define VIDEO_0 0

			/* Audio Device Index */
			#define AUDIO_0 0

			/* FCC test delay (ms) */
			#define FCC_DELAY 2000
			#define DELAY_FOR_MW 100

			PrimerPid_Info primerPidInfo;
			Pid_Info pidInfo;
			DI_VIDEO_PRIMER_t stVideoPrimer;
			int fccAgingCount = 0;
			/* tuner instance, transition, modulation, frequency, symbol rate, FEC, polarization, diseqc, 22khz tone, pilote */
			char TP0[50] = "sat 0 1 0 1744 22000 0 0 3 0 0"; /* TP 0 : Freq 1744 */
			char TP1[50] = "sat 1 1 0 1612 22000 0 0 3 0 0"; /* TP 1 : Freq 1612 */
			char TP2[50] = "sat 0 0 0 1588 27500 0 0 3 1 0"; /* TP 2 : Freq 1588 */
			char TP3[50] = "sat 1 0 0 1945 22000 0 0 3 1 0"; /* TP 3 : Freq 1945 */
			char TP4[50] = "sat 0 1 0 1597 22000 0 1 3 0 0"; /* TP 6 : Freq 1597 */

			char ucChannelLockTP0[50];
			char ucChannelLockTP1[50];
			char ucChannelLockTP2[50];
			char ucChannelLockTP3[50];
			char ucChannelLockTP4[50];

			VK_memset(ucChannelLockTP0, 0, 50);
			VK_memset(ucChannelLockTP1, 0, 50);
			VK_memset(ucChannelLockTP2, 0, 50);
			VK_memset(ucChannelLockTP3, 0, 50);
			VK_memset(ucChannelLockTP4, 0, 50);

			VK_memcpy(ucChannelLockTP0, TP0, 50);
			VK_memcpy(ucChannelLockTP1, TP1, 50);
			VK_memcpy(ucChannelLockTP2, TP2, 50);
			VK_memcpy(ucChannelLockTP3, TP3, 50);
			VK_memcpy(ucChannelLockTP4, TP4, 50);

			/* Video format */
			DI_VIDEO_FORMAT eVFormat[] =
			{
				DI_VIDEO_FORMAT_H264,	/* (0169) : SWR RD HD			*/
				DI_VIDEO_FORMAT_H264,	/* (0001) : Das Erste HD		*/
				DI_VIDEO_FORMAT_H264,	/* (0002) : ZDF HD			*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0003) : RTL Television		*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0004) : SAT.1				*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0005) : VOX				*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0006) : ProSieben			*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0007) : kabel eins			*/
				DI_VIDEO_FORMAT_H264,	/* (0008) : arte HD			*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0009) : RTL 2				*/
				DI_VIDEO_FORMAT_MPEG2,	/* (0010) : super RTL			*/
				DI_VIDEO_FORMAT_H264	/* (0011) : 3sat HD			*/
			};

			/* Audio format */
			DI_AUDIO_FORMAT eAFormat[] =
			{
				DI_AUDFORMAT_AC3,		/* (0169) : SWR RD HD			*/
				DI_AUDFORMAT_AC3,		/* (0001) : Das Erste HD		*/
				DI_AUDFORMAT_AC3,		/* (0002) : ZDF HD			*/
				DI_AUDFORMAT_AC3,		/* (0003) : RTL Television		*/
				DI_AUDFORMAT_AC3,		/* (0004) : SAT.1				*/
				DI_AUDFORMAT_MPEG,		/* (0005) : VOX				*/
				DI_AUDFORMAT_AC3,		/* (0006) : ProSieben			*/
				DI_AUDFORMAT_MPEG,		/* (0007) : kabel eins			*/
				DI_AUDFORMAT_MPEG,		/* (0008) : arte HD			*/
				DI_AUDFORMAT_MPEG,		/* (0009) : RTL 2				*/
				DI_AUDFORMAT_MPEG,		/* (0010) : super RTL			*/
				DI_AUDFORMAT_AC3		/* (0011) : 3sat HD			*/
			};

			/* Video PID */
			HUINT16 usVideoPid[] =
			{
				5121,					/* (0169) : SWR RD HD			*/
				5101,					/* (0001) : Das Erste HD		*/
				6110,					/* (0002) : ZDF HD			*/
				163,					/* (0003) : RTL Television HD	*/
				255,					/* (0004) : SAT.1				*/
				167,					/* (0005) : VOX				*/
				511,					/* (0006) : ProSieben			*/
				767,					/* (0007) : kabel eins			*/
				5111,					/* (0008) : arte HD			*/
				166,					/* (0009) : RTL 2				*/
				165,					/* (0010) : super RTL			*/
				6510					/* (0011) : 3sat HD			*/
			};

			/* Audio PID */
			HUINT16 usAudioPid[] =
			{
				5126,					/* (0169) : SWR RD HD			*/
				5106,					/* (0001) : Das Erste HD		*/
				6122,					/* (0002) : ZDF HD			*/
				106,					/* (0003) : RTL Television HD	*/
				259,					/* (0004) : SAT.1				*/
				136,					/* (0005) : VOX				*/
				515,					/* (0006) : ProSieben			*/
				768,					/* (0007) : kabel eins			*/
				5112,					/* (0008) : arte HD			*/
				128,					/* (0009) : RTL 2				*/
				120, 					/* (0010) : super RTL			*/
				6522					/* (0011) : 3sat HD			*/
			};

			while(1)
			{
				DI_UART_Print("[Fast Channel Change] aging test start (count : %d)\n", fccAgingCount);

				/* Initialize, Stop A/V */
				pidInfo.pidBitmask = PID_BITMASK_PCR | PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN;

				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, pidInfo.pidBitmask);
				DI_DEMUX_ReleasePath(DEMUX_0);

				/* (CH : 0001) **********************************/
				/* SWR RD HD(0169) - Das Erste HD(0001) - ZDF HD(0002) */
				/*********************************************/

				/* Das Erste HD(0001) */
				VK_memset(ucChannelLockTP0, 0, 50);
				VK_memcpy(ucChannelLockTP0, TP0, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP0);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif


				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[1];
				primerPidInfo.usPcrPid = usVideoPid[1];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				stVideoPrimer.eFormat = eVFormat[1];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* ZDF HD(0002) */
				VK_memset(ucChannelLockTP1, 0, 50);
				VK_memcpy(ucChannelLockTP1, TP1, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP1);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_1, DI_DEMUX_INPUT_TUNER, 1, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[2];
				primerPidInfo.usPcrPid = usVideoPid[2];
				DI_DEMUX_StartPrimerPID(DEMUX_1, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				stVideoPrimer.eFormat = eVFormat[2];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* SWR RD HD(0169) */
				DI_DEMUX_SetPath(DEMUX_2, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[0];
				primerPidInfo.usPcrPid = usVideoPid[0];
				DI_DEMUX_StartPrimerPID(DEMUX_2, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				stVideoPrimer.eFormat = eVFormat[0];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[1];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Start(VIDEO_0, eVFormat[1]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[1]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0002) **********************************/
				/* Das Erste HD(0001) - ZDF HD(0002) - RTL Television(0003) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/* Release SWR RD HD(0169) */
				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_2, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_2);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[2];
				DI_DEMUX_StartPID(DEMUX_1, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Start(VIDEO_0, eVFormat[2]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[2]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);
				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0003) **********************************/
				/* ZDF HD(0002) - RTL Television(0003) - SAT.1(0004) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_1, PID_BITMASK_AUDIO_MAIN);

				/* Release Das Erste HD(0001) */
				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/* Release ZDF HD(0002) */
				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_1, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_1);

				/* RTL Television(0003) */
				VK_memset(ucChannelLockTP2, 0, 50);
				VK_memcpy(ucChannelLockTP2, TP2, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP2);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[3];
				primerPidInfo.usPcrPid = usVideoPid[3];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				stVideoPrimer.eFormat = eVFormat[3];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/*  SAT.1(0004) */
				VK_memset(ucChannelLockTP3, 0, 50);
				VK_memcpy(ucChannelLockTP3, TP3, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP3);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_1, DI_DEMUX_INPUT_TUNER, 1, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[4];
				primerPidInfo.usPcrPid = usVideoPid[4];
				DI_DEMUX_StartPrimerPID(DEMUX_1, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				stVideoPrimer.eFormat = eVFormat[4];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[3];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Start(VIDEO_0, eVFormat[3]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[3]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0004) **********************************/
				/* RTL Television(0003) - SAT.1(0004) - VOX(0005)*/
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/*  VOX(0005) */
				DI_DEMUX_SetPath(DEMUX_2, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[5];
				primerPidInfo.usPcrPid = usVideoPid[5];
				DI_DEMUX_StartPrimerPID(DEMUX_2, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				stVideoPrimer.eFormat = eVFormat[5];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[4];
				DI_DEMUX_StartPID(DEMUX_1, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Start(VIDEO_0, eVFormat[4]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[4]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0005) **********************************/
				/* SAT.1(0004) - VOX(0005) - ProSieben(0006) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_1, PID_BITMASK_AUDIO_MAIN);

				/* Release  RTL Television(0003) */
				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/*	ProSeiben(0006) */
				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 1, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[6];
				primerPidInfo.usPcrPid = usVideoPid[6];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				stVideoPrimer.eFormat = eVFormat[6];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[5];
				DI_DEMUX_StartPID(DEMUX_2, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Start(VIDEO_0, eVFormat[5]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[5]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);


				/* (CH : 0006) **********************************/
				/* VOX(0005) - ProSieben(0006) - kabel eins(0007) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_2, PID_BITMASK_AUDIO_MAIN);

				/* Release SAT.1(0004) */
				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_1, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_1);

				/* kabel eins(0007) */
				DI_DEMUX_SetPath(DEMUX_1, DI_DEMUX_INPUT_TUNER, 1, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[7];
				primerPidInfo.usPcrPid = usVideoPid[7];
				DI_DEMUX_StartPrimerPID(DEMUX_1, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				stVideoPrimer.eFormat = eVFormat[7];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[6];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Start(VIDEO_0, eVFormat[6]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[6]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0007) **********************************/
				/* ProSieben(0006) - kabel eins(0007) - arte HD(0008)*/
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_1);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/* Release VOX(0005) */
				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_2, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_2);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[7];
				DI_DEMUX_StartPID(DEMUX_1, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Start(VIDEO_0, eVFormat[7]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[7]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);


				/* (CH : 0008) **********************************/
				/* kabel eins(0007) - arte HD(0008) - RTL 2(0009)*/
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_1, PID_BITMASK_AUDIO_MAIN);

				/* Release ProSieben(0006) */
				stVideoPrimer.ulPrimerIndex = PRIMER_1;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_1;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/* Release kabel eins(0007) */
				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_1, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_1);

				/* comment : no primers are allocated now. */

				/*  arte HD (0008) */
				VK_memset(ucChannelLockTP0, 0, 50);
				VK_memcpy(ucChannelLockTP0, TP0, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP0);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[8];
				primerPidInfo.usPcrPid = usVideoPid[8];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				stVideoPrimer.eFormat = eVFormat[8];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);


				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[8];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Start(VIDEO_0, eVFormat[8]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[8]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0009) **********************************/
				/* arte HD(0008) - RTL 2(0009) - super RTL(0010) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/* Release arte HD(0008) */
				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/*	RTL2 (0009) */
				VK_memset(ucChannelLockTP2, 0, 50);
				VK_memcpy(ucChannelLockTP2, TP2, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP2);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[9];
				primerPidInfo.usPcrPid = usVideoPid[9];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				stVideoPrimer.eFormat = eVFormat[9];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* super RTL (0010) */
				DI_DEMUX_SetPath(DEMUX_1, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[10];
				primerPidInfo.usPcrPid = usVideoPid[10];
				DI_DEMUX_StartPrimerPID(DEMUX_1, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				stVideoPrimer.eFormat = eVFormat[10];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[9];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Start(VIDEO_0, eVFormat[9]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[9]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0010) **********************************/
				/* RTL 2(0009) - super RTL(0010) - 3sat HD(0011) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_2);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[10];
				DI_DEMUX_StartPID(DEMUX_1, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Start(VIDEO_0, eVFormat[10]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[10]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/* (CH : 0011) **********************************/
				/* super RTL(0010) - 3sat HD(0011) - Das Erste HD(0001) */
				/*********************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_1, PID_BITMASK_AUDIO_MAIN);

				/* Release RTL 2 (0009) */
				stVideoPrimer.ulPrimerIndex = PRIMER_2;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_2;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/* Release super RTL(0010) */
				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_1, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_1);

				/* comment : no primers are allocated now. */

				/*  3sat HD (0011) */
				VK_memset(ucChannelLockTP4, 0, 50);
				VK_memcpy(ucChannelLockTP4, TP4, 50);
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLockTP4);
				VK_TASK_Sleep(DELAY_FOR_MW);
#endif
				DI_DEMUX_SetPath(DEMUX_0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				primerPidInfo.usVideoPid = usVideoPid[11];
				primerPidInfo.usPcrPid = usVideoPid[11];
				DI_DEMUX_StartPrimerPID(DEMUX_0, &primerPidInfo);

				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				stVideoPrimer.eFormat = eVFormat[11];
				DI_VIDEO_StartPrimer(VIDEO_0, &stVideoPrimer);

				/* A/V Start */
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[11];
				DI_DEMUX_StartPID(DEMUX_0, &pidInfo);

				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Start(VIDEO_0, eVFormat[11]);
				DI_AUDIO_Start(AUDIO_0, eAFormat[11]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(VIDEO_0,TRUE);

				/*********************************************/

				VK_TASK_Sleep(FCC_DELAY);
				DI_UART_Print("[Fast Channel Change] channel has been changed. (delay:%d ms)\n", FCC_DELAY);

				/*Restart**************************************/

				/* A/V stop */
				DI_VIDEO_Show(VIDEO_0,FALSE);
				DI_VIDEO_SelectPrimer(VIDEO_0, PRIMER_0);
				DI_VIDEO_Stop(VIDEO_0);
				DI_AUDIO_Stop(AUDIO_0);
				DI_DEMUX_StopPID(DEMUX_0, PID_BITMASK_AUDIO_MAIN);

				/* Release 3sat HD (0011) */
				stVideoPrimer.ulPrimerIndex = PRIMER_0;
				DI_VIDEO_StopPrimer(VIDEO_0, &stVideoPrimer);

				primerPidInfo.ulPrimerIndex = PRIMER_0;
				primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
				DI_DEMUX_StopPrimerPID(DEMUX_0, &primerPidInfo);

				DI_DEMUX_ReleasePath(DEMUX_0);

				/*********************************************/

				DI_UART_Print("[Fast Channel Change] aging test[%d] end.\n", fccAgingCount);
				fccAgingCount++;
			}
		}
		else if PARAM_IS("fcc")
		{
			//#define USE_NORMAL_PLAY
			//#define USE_VIDEO_PRIMER_ONLY
			#define TOTAL_CHANNEL_COUNT 3
			static HINT32 s_ulCurrentCh, s_ulCurrentChPrimer;
			HUINT32 i = 0, ulDemuxId = 0;
			PrimerPid_Info primerPidInfo;
			Pid_Info pidInfo;
			char ch;

			/* Test stream : \\stream\stream\FCC_Test\554Mhz[BBC One HD].ts */

//			DI_VIDEO_FORMAT eVFormat[] = {DI_VIDEO_FORMAT_H264, DI_VIDEO_FORMAT_H264, DI_VIDEO_FORMAT_H264};
//			DI_AUDIO_FORMAT eAFormat[] = {DI_AUDFORMAT_AAC_PLUS, DI_AUDFORMAT_AAC_PLUS, DI_AUDFORMAT_AAC_PLUS};
			DI_VIDEO_FORMAT eVFormat[] = {DI_VIDEO_FORMAT_MPEG2, DI_VIDEO_FORMAT_MPEG2, DI_VIDEO_FORMAT_MPEG2};
			DI_AUDIO_FORMAT eAFormat[] = {DI_AUDFORMAT_MPEG, DI_AUDFORMAT_MPEG, DI_AUDFORMAT_MPEG};

//			HUINT16 usVideoPid[] = {101, 201, 301}; // 101, 201, 301, 6601
//			HUINT16 usAudioPid[] = {102, 202, 302}; // 102, 202, 302, 6602
//			HUINT16 usVideoPid[] = {600, 610, 620}; // 600, 610, 620, 640
//			HUINT16 usAudioPid[] = {601, 611, 621}; // 601, 611, 621, 641
			HUINT16 usVideoPid[] = {513, 514, 515}; // 513, 514, 515, 516
			HUINT16 usAudioPid[] = {660, 670, 680}; // 660, 670, 680, 690
//			HUINT16 usAdAudioPid[] = {106, 206, 306}; // 106, 206, 306, 6606

//			char ucChannelLock[50] = "sat 0 1 2 1250 27500 0 0 4 0 0";
			char ucChannelLock[50] = "sat 0 0 1 1130 27250 0 1 2 1 0"; // Sahara live stream
//			char ucChannelLock[50] = "ter 0 546 8 0 1";

			if PARAM1_IS("start")
			{
#if defined(CONFIG_CHANNEL)
				CMD_HwTest_Parser_ChLock(ucChannelLock);
				VK_TASK_Sleep(1000);
#endif
				pidInfo.pidBitmask = PID_BITMASK_PCR | PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN;

				DI_VIDEO_Stop(0);
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId,pidInfo.pidBitmask);
				DI_DEMUX_ReleasePath(ulDemuxId);

#if defined(USE_NORMAL_PLAY)
				/* Demux setting */
				DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0,-1);

				s_ulCurrentCh = 1;
				pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN;
				pidInfo.usVideoPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usPCRPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

				/* Audio path */
#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				/* Start A/V */
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);
				VK_TASK_Sleep(10);
				DI_VIDEO_Show(0,TRUE);
#elif defined(USE_VIDEO_PRIMER_ONLY)
				/* Demux setting */
				DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0,-1);
				DI_VIDEO_PRIMER_t stVideoPrimer;

				for(i=0; i<TOTAL_CHANNEL_COUNT; i++)
				{
					primerPidInfo.ulPrimerIndex = i;
					primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER;
					primerPidInfo.usVideoPid = usVideoPid[i];
					primerPidInfo.usPcrPid = usVideoPid[i];
					DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);

					stVideoPrimer.ulPrimerIndex = i;
					stVideoPrimer.eFormat = eVFormat[i];
					DI_VIDEO_StartPrimer(0, &stVideoPrimer);

				}

				s_ulCurrentCh = 0;
				s_ulCurrentChPrimer = 0;

				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

				/* Audio path */
#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				/* A/V Start */
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(0,TRUE);
#else
				/* Demux setting */
				DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0,-1);
				DI_VIDEO_PRIMER_t stVideoPrimer;
				DI_AUDIO_PRIMER_t stAudioPrimer;

				for(i=0; i<TOTAL_CHANNEL_COUNT; i++)
				{
					primerPidInfo.ulPrimerIndex = i;
					primerPidInfo.ulPidBitmask = PID_BITMASK_PCR_PRIMER | PID_BITMASK_VIDEO_PRIMER | PID_BITMASK_AUDIO_PRIMER;
					primerPidInfo.usVideoPid = usVideoPid[i];
					primerPidInfo.usPcrPid = usVideoPid[i];
					primerPidInfo.usAudioPid = usAudioPid[i];
					DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);

					stVideoPrimer.ulPrimerIndex = i;
					stVideoPrimer.eFormat = eVFormat[i];
					DI_VIDEO_StartPrimer(0, &stVideoPrimer);
					stAudioPrimer.eAudioType = DI_AUDIOTYPE_STREAM;
					stAudioPrimer.ulPrimerIndex = i;
					stAudioPrimer.eFormat = eAFormat[i];
					DI_AUDIO_StartPrimer(0, &stAudioPrimer);
				}

				s_ulCurrentCh = 0;
				s_ulCurrentChPrimer = 0;

				/* Audio path */
#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				/* A/V Start */
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);

				DI_AUDIO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				VK_TASK_Sleep(10);
				DI_VIDEO_Show(0,TRUE);
#endif
			}
			else if PARAM1_IS("up")
			{
#if defined(USE_NORMAL_PLAY)
				/* Normal channel change */
				DI_VIDEO_Stop(0);
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId, PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN);

				s_ulCurrentCh++;
				if(s_ulCurrentCh >= TOTAL_CHANNEL_COUNT)
				{
					s_ulCurrentCh = 0;
				}

				pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN;
				pidInfo.usVideoPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usPCRPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);
#elif defined(USE_VIDEO_PRIMER_ONLY)
				/* Fast channel change */
				DI_VIDEO_Show(0,FALSE);
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Stop(0);
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId, PID_BITMASK_AUDIO_MAIN);

				s_ulCurrentCh++;
				if(s_ulCurrentCh >= TOTAL_CHANNEL_COUNT)
				{
					s_ulCurrentCh = 0;
				}

				pidInfo.pidBitmask =  PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

				s_ulCurrentChPrimer++;
				if(s_ulCurrentChPrimer >= TOTAL_CHANNEL_COUNT)
				{
					s_ulCurrentChPrimer = 0;
				}

#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				DI_VIDEO_Show(0,TRUE);


#else
				/* Fast channel change */
				DI_VIDEO_Show(0,FALSE);
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Stop(0);
				DI_AUDIO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_AUDIO_Stop(0);

				s_ulCurrentCh++;
				if(s_ulCurrentCh >= TOTAL_CHANNEL_COUNT)
				{
					s_ulCurrentCh = 0;
				}

				s_ulCurrentChPrimer++;
				if(s_ulCurrentChPrimer >= TOTAL_CHANNEL_COUNT)
				{
					s_ulCurrentChPrimer = 0;
				}

#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				DI_VIDEO_Show(0,TRUE);
#endif
			}
			else if PARAM1_IS("down")
			{
#if defined(USE_NORMAL_PLAY)
				/* Normal channel change */
				DI_VIDEO_Stop(0);
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId, PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN);

				s_ulCurrentCh--;
				if(s_ulCurrentCh < 0)
				{
					s_ulCurrentCh = TOTAL_CHANNEL_COUNT-1;
				}

				pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN;
				pidInfo.usVideoPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usPCRPid = usVideoPid[s_ulCurrentCh];
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);
#elif defined(USE_VIDEO_PRIMER_ONLY)
				/* Fast channel change */
				DI_VIDEO_Show(0,FALSE);
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Stop(0);
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId, PID_BITMASK_AUDIO_MAIN);

				s_ulCurrentCh--;
				if(s_ulCurrentCh < 0)
				{
					s_ulCurrentCh = TOTAL_CHANNEL_COUNT-1;
				}

				pidInfo.pidBitmask =  PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usAudioPid[s_ulCurrentCh];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

				s_ulCurrentChPrimer--;
				if(s_ulCurrentChPrimer < 0)
				{
					s_ulCurrentChPrimer = TOTAL_CHANNEL_COUNT-1;
				}

#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				DI_VIDEO_Show(0,TRUE);

#else
				/* Fast channel change */
				DI_VIDEO_Show(0,FALSE);
				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Stop(0);
				DI_AUDIO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_AUDIO_Stop(0);

				s_ulCurrentCh--;
				if(s_ulCurrentCh < 0)
				{
					s_ulCurrentCh = TOTAL_CHANNEL_COUNT-1;
				}

				s_ulCurrentChPrimer--;
				if(s_ulCurrentChPrimer < 0)
				{
					s_ulCurrentChPrimer = TOTAL_CHANNEL_COUNT-1;
				}

#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

				DI_VIDEO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_VIDEO_Start(0, eVFormat[s_ulCurrentCh]);
				DI_AUDIO_SelectPrimer(0, s_ulCurrentChPrimer);
				DI_AUDIO_Start(0, eAFormat[s_ulCurrentCh]);

				DI_VIDEO_Show(0,TRUE);
#endif
			}
			else
			{
				CMD_Printf("[Fast Channel Test] wrong command, correct commands are (av test fcc start, av test fcc up, av test fcc down)\n");
			}

			iResult=CMD_OK;
		}
#endif

	}
	else if CMD_IS("sync")
	{
		static HUINT32 snCmdSynTimer = 0;
		int nTime;
		if PARAM_IS("on")
		{
			if( snCmdSynTimer == 0 )
			{
				READABLE_IN_DEC(HWTEST_PARAM1, nTime);

				VK_TIMER_EventEvery((unsigned long)nTime,
						Cmd_SvcSyncTimer,NULL,0,(unsigned long *)&snCmdSynTimer);
				CMD_Printf(" av sync timer setting time %d\n",nTime);
				iResult=CMD_OK;
			}
			else
			{
				CMD_Printf(" av sync alreay started \n");
			}
		}
		else if PARAM_IS("off")
		{
			if( snCmdSynTimer )
			{
				VK_TIMER_Cancel(snCmdSynTimer);
				iResult=CMD_OK;
			}
			else
			{
				CMD_Printf(" av sync is not started \n");
			}
			snCmdSynTimer = 0;
		}
		else
		{
			CMD_Printf("ERR ex.) av sync (on/off) (100)  \n");
		}
	}
TEST_AV_EXIT:
	return iResult;
}
#endif


#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

