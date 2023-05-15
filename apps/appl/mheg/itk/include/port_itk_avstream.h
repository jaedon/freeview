/*
	@file     port_itk_avstream.h
	@brief    ITK Host avstream header file
	Description: Porinting Layer.
	Module: mw/mheg/itk

	Copyright (c) 2008 HUMAX Co., Ltd.
	All rights reserved.
*/

#ifndef _PORT_ITK_AVSTREAM_H_
#define _PORT_ITK_AVSTREAM_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include "itk_display.h"
#include "itk_avstream.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ITK_DISPLAY_AV_STATUS_CREATED			0x00
#define ITK_DISPLAY_AV_STATUS_PRELOAD			0x01
#define ITK_DISPLAY_AV_STATUS_PLAYING			0x02
#define ITK_DISPLAY_AV_STATUS_STOPPED			0x04
#define ITK_DISPLAY_AV_STATUS_TRIGGER			0x08
#define ITK_DISPLAY_AV_STATUS_V_SELECTED		0x10
#define ITK_DISPLAY_AV_STATUS_A_SELECTED		0x20
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
#define ITK_DISPLAY_AV_STATUS_PRESENT			0x40
#define ITK_DISPLAY_AV_STATUS_STALLED			0x80

/* AV Status */
#define ITK_DISPLAY_AV_STREAMING_STATUS_ALL_OFF			0x00
#define ITK_DISPLAY_AV_STREAMING_STATUS_ENTER_VIEW		0x01	//2:Start the loading
#define ITK_DISPLAY_AV_STREAMING_STATUS_SETTING			0x02	//2:Set up a content
#define ITK_DISPLAY_AV_STREAMING_STATUS_SETUP			0x04	//2:Loaded AV
#define ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING			0x08	//2:Played AV

#define ITK_DISPLAY_REF_MAX_LEN	(1024)
#endif

typedef	enum tagMheg_Av_Mode{
	MHEG_AV_MODE_BROADCAST = 0,
	MHEG_AV_MODE_PLAYBACK,
	MHEG_AV_MODE_IPSTREAM, // Not encrypted.
	MHEG_AV_MODE_IPSTREAM_AES,
	MHEG_AV_MODE_IPSTREAM_DES,
	MHEG_SN_AV_MODE_MAX
} MHEG_Av_Mode_t;

typedef	enum tagMheg_Streaming_Status{
	MHEG_STREAMING_STATUS_WAITING = 0,
	MHEG_STREAMING_STATUS_DOWNLOADING,
	MHEG_STREAMING_STATUS_PAUSE,
	MHEG_STREAMING_STATUS_DONE
} MHEG_Streaming_Status_e;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct ITK_Media_Triggers_t
{
	itk_int32_t trigger;
	itk_int32_t triggerValue;

	struct ITK_Media_Triggers_t	*pPrev;
	struct ITK_Media_Triggers_t	*pNext;
} ITK_Media_Triggers_t;

typedef struct ITK_AvstreamHandle_t
{
	itk_avstream_t 				vtbl;
	itk_int32_t 				presentation_count;
	itk_media_buffer_priority_t	bufferPriority;
	itk_avstream_callback_t 	callback;
	void*						callback_context;

	HUINT32						ulViewId;
	ITK_RefSvc_t 				eStreamRef;
	HINT32 						svcUid;
	HINT32	 					serviceId;
	HINT32	 					audioPid;	/* PID_NULL --> deselected which means to be stopped */
	HINT32	 					videoPid;
	HINT32	 					pcrPid;
	HUINT8 						audioType;
	HUINT8 						videoType;

	HUINT8 						audioStatus;
	HUINT8 						videoStatus;
	itk_present_t 				subtitle;
	itk_int32_t 				position;
	itk_int32_t 				endPosition;

	itk_int32_t 				audioComponent;
	itk_int32_t 				videoComponent;
	itk_uint8_t 				mediaStatus;
	itk_media_termination_t 	mediaTerm;

	HBOOL					bAudioFreeze;
	HBOOL					bVideoFreeze;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	char						reference[ITK_DISPLAY_REF_MAX_LEN];
	Handle_t					downloadHandle;
	HUINT32						downloadSessionId;
	HUINT32						contentLength;
	HUINT32						maxBitrate;
	HUINT32						time2pos;
	HUINT32						pos2time;
	HUINT32						duration;
	HUINT8						avMode;
	HUINT8						avStatus;				/* AV mode status for media (e.g Interaction Channel Streaming) */
	MHEG_Streaming_Status_e		downloadStatus;
	HBOOL						autoStart;
	HBOOL						discardBuffer;
	HBOOL						setPosZero;				/* MHEG ICS011 Trick play/stop 후 play가 제대로 안되는 이슈 수정을 위한 flag*/
	struct ITK_Media_Triggers_t	*pHead;
	struct ITK_Media_Triggers_t	*pTail;
#endif

	struct ITK_AvstreamHandle_t	*pPrev;
	struct ITK_AvstreamHandle_t	*pNext;
} ITK_AvstreamHandle_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern itk_avstream_t *port_itk_avs_NewAvStream(struct itk_display_t *thiz, const char *reference, itk_int32_t presentation_count, itk_media_buffer_priority_t bufferPriority, itk_avstream_callback_t callback, void *callback_context);
extern void	port_itk_avs_init(void);
extern ITK_AvstreamHandle_t* port_itk_avs_GetHandle(struct ITK_AvstreamHandle_t *pstStreamHandler);
#endif



