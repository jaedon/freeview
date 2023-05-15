/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_video.h
	@brief	  video driver의 adaptation layer 지만 av service의 실제 동작을 판단 및 처리한다. \n

	Description: video 동작과 각 parameter의 조합을 판단하여, Display setting한다. \n
	Module: PAL/VIDEO \n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/


#ifndef	__PAL_VIDEO_H__
#define	__PAL_VIDEO_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <dlib.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define MAX_VIDEO_CONTEXT					NUM_VIEW_ACTION

#ifdef CONFIG_DEBUG
//#define __INT_AV_TRACE_FCC__
#endif
#ifdef __INT_AV_TRACE_FCC__
#define AvFccTrace		HxLOG_Warning
#else
#define AvFccTrace
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief	DI Level에서 제공하는 Interrupt 성 EVT다. */
typedef enum
{
	eVIDEO_EVENT_NONE,
	eVIDEO_EVENT_PARAM_CHANGED,
	eVIDEO_EVENT_FRAME_START,
	eVIDEO_EVENT_UNDERRUN,
	eVIDEO_EVENT_MAX_NUM
} PalVideo_Event_e;

typedef enum
{
	eChannelChangeMode_Mute,
	eChannelChangeMode_HoldUntilTsmLock,
	eChannelChangeMode_MuteUntilFirstPicture,
	eChannelChangeMode_HoldUntilFirstPicture
} PalVideo_ChannelChangeMode_e;

/** @brief	captured frame buffer */
typedef struct
{
	HUINT32						ulWidth;					/* Frame의 Width */
	HUINT32						ulHeight;					/* Frame의 Height */
	HUINT32 					*pFrameBuffer;				/* ARGB로 저장됨. */
} PalVideo_FrameBuffer_t;


/* Public API 정의 **************************************************************************************/

extern HERROR PAL_VIDEO_Init(void);
extern HERROR PAL_VIDEO_StartDecoding(HUINT32 ulDecoderId, DxVideoCodec_e eVideoCodec);
extern HERROR PAL_VIDEO_StopDecoding(HUINT32 ulDecoderId);
extern HERROR PAL_VIDEO_StartStillPicture(HUINT32 ulDecoderId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxVideoCodec_e eVideoCodec, HBOOL bForCapture);
extern HERROR PAL_VIDEO_StopStillPicture(HUINT32 ulDecoderId);
extern HERROR PAL_VIDEO_SetFreeze(HUINT32 ulDecoderId, HBOOL bFreeze);
extern HBOOL PAL_VIDEO_IsFreezed(HUINT32 ulDecoderId);
extern HERROR PAL_VIDEO_CreateVideoFrameCapture(HUINT32 ulDecoderId, HUINT32 ulWidth, HUINT32 ulHeight, PalVideo_FrameBuffer_t *pFrame);
extern HERROR PAL_VIDEO_GetThumbNail(HUINT32 ulDecoderId, HUINT32 ulWidth, HUINT32 ulHeight, PalVideo_FrameBuffer_t *pFrame);

extern HERROR PAL_VIDEO_CaptureVideoFrame(HUINT32 ulDecoderId, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 *handle);
extern HERROR PAL_VIDEO_SetEvtTimeOut(HUINT32 msec);
extern HBOOL PAL_VIDEO_IsRunning(HUINT32 ulDecoderId);
extern DecoderState_t PAL_VIDEO_GetState(HUINT32 ulDecId);
extern HERROR PAL_VIDEO_GetSourceTiming(HUINT32 ulDecoderId, VIDEO_Timing_t *pstVideoSourceTiming);
extern HERROR PAL_VIDEO_GetStreamInfo(HUINT32 ulDecoderId, VideoStreamInfo_t *pstVideoStreamInfo);

extern HERROR PAL_VIDEO_RegisterEventCallback(void (*pfnEventCallback)(HUINT32 ulDecoderId, PalVideo_Event_e eVideoEvent));
extern HERROR PAL_VIDEO_RegisterEventCallbackForCas(void (*pfnEventCallback)(HUINT32 ulDecoderId, PalVideo_Event_e eVideoEvent));
extern const HINT8 *PAL_VIDEO_GetStrDeviceEvt(PalVideo_Event_e eVideoDeviceEvent);

extern HUINT32 PAL_VIDEO_GetSupportNumberOfDecoder(void);
extern VIDEO_DecType_t PAL_VIDEO_GetDecoderType(HUINT32 ulDecId);

extern HERROR PAL_VIDEO_SetHide(HUINT32 ulDecId, AV_HideClient_t ulClientMask, HBOOL bHideFlag);
extern HERROR PAL_VIDEO_RegisterEventCallbackForVideoError(void (*pfnVideoErrorEventCallback)(HUINT32 ulDecoder, HUINT32 ulDisplay));
extern HERROR PAL_VIDEO_ConvertPal2Di_Codec(DxVideoCodec_e ePalCodec, HUINT32 *peDiCodec);
extern HERROR PAL_VIDEO_SetOutputEnable(HUINT32 ulDeviceId, HUINT32 ulDisplayId, HBOOL bEnable);

#if defined(CONFIG_SUPPORT_FCC)
extern HERROR PAL_VIDEO_StartPrimerDecoding(HUINT32 ulDecId, DxVideoCodec_e eVideoCodec, HBOOL bMainAV, HUINT32 ulPrimerIdx);
extern HERROR PAL_VIDEO_StopPrimerDecoding(HUINT32 ulDecId, HUINT32 ulPrimerIdx);
extern HERROR PAL_VIDEO_StopPrimer(HUINT32 ulDecId, HUINT32 ulPrimerIdx);
extern HERROR PAL_VIDEO_SetMainDecIdForFcc(HUINT32 ulDecId);
#endif
extern HERROR PAL_VIDEO_SetChannelChangeMode(HUINT32 ulDecId, PalVideo_ChannelChangeMode_e eMode);
extern void PAL_VIDEO_SetShowHideControl(HBOOL bShowHideControlEnabled);

#endif	//__PAL_VIDEO_H__
