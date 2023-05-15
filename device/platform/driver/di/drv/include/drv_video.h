/**
 * drv_video.h
*/

#ifndef __DRV_VIDEO_H__
#define __DRV_VIDEO_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include "htype.h"
#include "drv_err.h"
#include "nexus_display.h"
#include "nexus_vbi.h"
#if defined(CONFIG_VIDEO_DECODER)
#include "nexus_video_decoder.h"
#include "nexus_still_decoder.h"
#endif
/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

#define VBI_TT_ENTRY_SIZE 16
#define VBI_TT_INVALID_FRAMING_CODE 0xFF
#define VBI_TT_FRAMING_CODE 0xE4
#define VBI_TT_FIELD_OFFSET 7
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct
{
	uint8_t framingCode;
	uint8_t data[NEXUS_TELETEXT_LINESIZE];
} DRV_VIDEO_VBI_TT;

/* The define value is X * Y value */
typedef enum {
	DRV_VIDEO_SAR_11	= 1,
	DRV_VIDEO_SAR_1211	= 132,
	DRV_VIDEO_SAR_1011	= 110,
	DRV_VIDEO_SAR_1611	= 176,
	DRV_VIDEO_SAR_4033	= 1320,
	DRV_VIDEO_SAR_2411	= 264,
	DRV_VIDEO_SAR_2011	= 220,
	DRV_VIDEO_SAR_3211	= 352,
	DRV_VIDEO_SAR_8033	= 2640,
	DRV_VIDEO_SAR_1811	= 198,
	DRV_VIDEO_SAR_1511	= 165,
	DRV_VIDEO_SAR_6433	= 2112,
	DRV_VIDEO_SAR_16099	= 15840,
	DRV_VIDEO_SAR_2215	= 330,
	DRV_VIDEO_SAR_2218	= 396,
	DRV_VIDEO_SAR_NONE	= 0
} DRV_VIDEO_SAR;

/* Total logical decoders are different among chipsets. See Broadcom's Nexus platform feature. */
typedef enum {
	DRV_VIDEO_DECODER_INDEX_0	= 0,	/* Main video */
	DRV_VIDEO_DECODER_INDEX_1	= 1,	/* PIP (or the second transcoder : When a chipset has 3 logical decoders) */
	DRV_VIDEO_DECODER_INDEX_2	= 2,	/* The first transcoder */
	DRV_VIDEO_DECODER_INDEX_3	= 3,	/* The second transcoder (When a chipset has 4 logical decoders) */
	DRV_VIDEO_DECODER_INDEX_MAX			/* The Wrong index type */
} DRV_VIDEO_DECODER_INDEX;

typedef struct
{
	HBOOL bOnOff;
} DRV_VIDEO_PRESET_DATA_t;

typedef enum {
	DRV_VIDEO_PRESET_UNKNOWN = 0,
	DRV_VIDEO_PRESET_SSTR	 = 1,
	DRV_VIDEO_PRESET_HLS	 = 2
} DRV_VIDEO_PRESET;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_VIDEO_Init(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_UnInit(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_GetDisplayHandle(HUINT32 ulDeviceId, NEXUS_DisplayHandle *dispHD, NEXUS_DisplayHandle *dispSD);
#if defined(CONFIG_VIDEO_DECODER)
DRV_Error DRV_VIDEO_GetDecoderHandle(HUINT32 ulDeviceId, NEXUS_VideoDecoderHandle* phDecoder);
DRV_Error DRV_VIDEO_GetStillDecoderHandle(HUINT32 ulDeviceId, NEXUS_StillDecoderHandle* phStillDecoder);
#endif
DRV_Error DRV_VIDEO_VBITeletext_Write(DRV_VIDEO_VBI_TT *vbi_tt_data, unsigned long lines, unsigned char polarity);
DRV_Error DRV_VIDEO_Start(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec, HBOOL bTsm);
DRV_Error DRV_VIDEO_Restart(HUINT32 ulDeviceId, HBOOL bTsm);
DRV_Error DRV_VIDEO_Stop(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDOutput);
#if defined(CONFIG_VIDEO_DECODER)
DRV_Error DRV_VIDEO_GetVideoHandle(HUINT32 ulDeviceId, NEXUS_VideoDecoderHandle *hDecoder);
#endif
DRV_Error DRV_VIDEO_GetCurrentStc(HUINT32 ulDeviceId, HUINT32 *uiStc);
void DRV_VIDEO_GetCurrentOutputStatus(void* pParam);
void DRV_VIDEO_GetVideoFormatStr(HUINT32 ulIndex, char* pStr);
void DRV_VIDEO_GetVideoCodecStr(HUINT32 ulIndex, char* pStr);
void DRV_VIDEO_PrintStatus(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_SetDeinterlaced(HUINT32 ulDeviceId, HBOOL OnOff, HBOOL bForce);
DRV_Error DRV_VIDEO_DelayOutput(HUINT32 ulDeviceId, HINT32 delayMs);
DRV_Error DRV_VIDEO_ResetDecoder(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_SetSmoothStreaming(HUINT32 ulDeviceId, HBOOL OnOff);
NEXUS_VideoCodec DRV_VIDEO_GetCodecType(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec);
NEXUS_VideoFrameRate DRV_VIDEO_GetFrameRateType(HUINT32 ulDeviceId, DI_VIDEO_FrameRate_t eFrameRate);
NEXUS_VideoFormat DRV_VIDEO_GetFormatType(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eFormat);
NEXUS_DisplayAspectRatio DRV_VIDEO_GetAspectRatio(HUINT32 ulDeviceId, DI_VIDEO_AspectRatio_t AspectRatio);
DI_VIDEO_FORMAT DRV_VIDEO_GetDiFormat(HUINT32 ulDeviceId, NEXUS_VideoCodec VideoCodec);
DI_VIDEO_FrameRate_t DRV_VIDEO_GetDiFrameRate(HUINT32 ulDeviceId, NEXUS_VideoFrameRate FrameRate);
DRV_Error DRV_VIDEO_GetVideoInputHandle(HUINT32 ulDeviceId, NEXUS_VideoInput* hInput);
DRV_Error DRV_VIDEO_SetPresetProperties(HUINT32 ulDeviceId, DRV_VIDEO_PRESET ePreset, DRV_VIDEO_PRESET_DATA_t *pData);

void DRV_VIDEO_ClearTimeoutTimer(void);
DRV_Error DRV_VIDEO_ApplySetInOutWindows(HUINT32 ulDeviceId, HBOOL bApply);

DRV_Error DRV_VIDEO_PlayMemoryStreamVideoPlane(HUINT32 ulDeviceId, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize);
DRV_Error DRV_VIDEO_PlayMemoryStreamOsdPlane(HUINT32 ulDeviceId, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize, void *stTarget);
DRV_Error DRV_VIDEO_PlayMemoryStreamPlaypumpStart(HUINT32 ulDeviceId, DI_VIDEO_STREAM_TYPE eStreamType);
DRV_Error DRV_VIDEO_PlayMemoryStreamPlaypumpStop(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_StartMosaic(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec);
DRV_Error DRV_VIDEO_StopMosaic(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_GetPrimerIndex(HUINT32 ulDeviceId, HUINT32 *pPrimerIndex);
HBOOL DRV_VIDEO_IsPrimerStarted(void);
#endif /* !__DRV_VIDEO_H__ */

