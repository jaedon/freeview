/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_video.h
	@brief	  video driver�� adaptation layer ���� av service�� ���� ������ �Ǵ� �� ó���Ѵ�. \n

	Description: video ���۰� �� parameter�� ������ �Ǵ��Ͽ�, Display setting�Ѵ�. \n
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

/** @brief	DI Level���� �����ϴ� Interrupt �� EVT��. */
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
	HUINT32						ulWidth;					/* Frame�� Width */
	HUINT32						ulHeight;					/* Frame�� Height */
	HUINT32 					*pFrameBuffer;				/* ARGB�� �����. */
} PalVideo_FrameBuffer_t;


/* Public API ���� **************************************************************************************/

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
