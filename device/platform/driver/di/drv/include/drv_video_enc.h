/**
 * drv_video.h
*/

#ifndef __DRV_VIDEO_ENC_H__
#define __DRV_VIDEO_ENC_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "htype.h"
#include "drv_err.h"

#include "di_video.h"
#include "di_transcoder.h"
#include "nexus_video_encoder.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct
{
	HBOOL					bCustom;
	HUINT32					ulWidth;
	HUINT32					ulHeight;
	HUINT32					ulRefreshRate;
	HBOOL					bInterlaced;
	HUINT32					ulSampleAspectRatioX;
	HUINT32					ulSampleAspectRatioY;
} DRV_VIDEO_ENC_RES_CUSTOM_Set_t;

typedef struct
{
	HBOOL					bCustomIPB;
	HUINT32					ulFrameP;
	HUINT32					ulFrameB;

} DRV_VIDEO_ENC_GOP_Set_t;

typedef struct
{

	DI_TRANSCODER_SOURCE_TYPE eTransType;
	DI_VIDEO_FORMAT eCodec;
	DI_VIDEO_FORMAT eDecCodec;

	DI_VIDEO_HD_RESOLUTION eRes;
	DI_VIDEO_FrameRate_t eFrameRate;
	DI_VIDEO_AspectRatio_t eAspectRatio;

	HBOOL					bDefaultProfile;

	DRV_VIDEO_ENC_RES_CUSTOM_Set_t stCustomRes;

	HUINT32					ulMaxBitRate;

	DI_TRANSCODER_VIDEO_CODEC_LEVEL		eLevel;
	DI_TRANSCODER_VIDEO_CODEC_PROFILE	eProfile;

	DRV_VIDEO_ENC_GOP_Set_t	stCustomIPB;

	HUINT32 ulDemuxId;

} DRV_VIDEO_ENC_Set_t;

typedef enum
{
	DRV_VIDEO_NOTIFY_ENC_DEC_SEQ_INFO,
	DRV_VIDEO_NOTIFY_ENC_DEC_PROBE,
	DRV_VIDEO_NOTIFY_ENC_MAX
} DRV_VIDEO_NOTIFY_ENC;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

typedef void (*pfnDRV_VidEncCallback_Notify)(HUINT32 ulDeviceId, DRV_VIDEO_NOTIFY_ENC eEvent, void *pvParam);
DRV_Error DRV_VIDEO_EncInit(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_EncUnInit(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_GetEncHandle(HUINT32 ulDeviceId, NEXUS_VideoEncoderHandle *hEncoder);
DRV_Error DRV_VIDEO_EncStart(HUINT32 ulDeviceId, HUINT32 ulDelay);
DRV_Error DRV_VIDEO_EncStop(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_EncRestart(HUINT32 ulDeviceId,HUINT32 ulDemuxId);
DRV_Error DRV_VIDEO_EncSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings);
DRV_Error DRV_VIDEO_EncUnsettings(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_GetEncDelayRange(HUINT32 ulDeviceId, HUINT32 *min, HUINT32 *max);
DRV_Error DRV_VIDEO_UpdateEncSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings);
DRV_Error DRV_VIDEO_UpdateEncGetSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings);
DRV_Error DRV_VIDEO_EncShowStatus(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_DecProbeStart(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_VIDEO_FORMAT eVideoCodec);
DRV_Error DRV_VIDEO_DecProbeStop(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_DecProbeClose(HUINT32 ulDeviceId);
DRV_Error DRV_VIDEO_GetDecSequenceInfo(HUINT32 ulDeviceId, NEXUS_VideoDecoderStreamInformation *StreamInfo, NEXUS_VideoDecoderStatus *Status);
DI_VIDEO_AspectRatio_t DRV_VIDEO_EncGetDiAspectRatio(HUINT32 ulDeviceId, NEXUS_AspectRatio eAspectRatio);
DI_TRANSCODER_VIDEO_CODEC_LEVEL DRV_VIDEO_EncGetDiCodecLevel(HUINT32 ulDeviceId, NEXUS_VideoCodecLevel eCodecLevel);
DI_TRANSCODER_VIDEO_CODEC_PROFILE DRV_VIDEO_EncGetDiCodecProfile(HUINT32 ulDeviceId, NEXUS_VideoCodecProfile eCodecProfile);
void DRV_VIDEO_EncRegisterEventCallback(pfnDRV_VidEncCallback_Notify pfnNotify);

#endif /* !__DRV_VIDEO_ENC_H__ */

