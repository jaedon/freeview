/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/**
	@file     svc_av.c
	@brief    svc_av.c (AV Service)

	Description: Application에서 요청한 Audio와 Video에 관련된 Service를 제공하는 모듈.      \n
	Module: MW/AV 															\n
	Remarks : 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <get_enum_str.h>

#include <pal_pipe.h>
//#include <mwc_util.h>
#include <db_param.h>

#include <svc_av.h>

#include <pal_video.h>
#include <pal_audio.h>
#include <pal_hdmi.h>
#include <pal_scart.h>
#include <pal_demux.h>
#include <pal_output.h>
#include <pal_scaler.h>
#include <pal_thumbnail.h>
#include <pal_pipe.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


//#define WORKTHROUGH_AV_START_AFTER_PIPE_CONNECTED

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eCMD_AV_START_SESSION = 1,			/* Audio, Video Decoding 시작 */
	eCMD_AV_STOP_SESSION,				/* Audio, Video Decoding 중지 */
	eCMD_AV_CHANGE_PID,					/* Audio, Video Pid & Codec 변경 */
	eCMD_AV_FREEZE_VIDEO, 			/* Live에서 Video Decoding 일시 정지 실행. */
	eCMD_AV_FREEZE_AUDIO, 			/* Live에서 Audio Decoding 일시 정지 실행. */
	eCMD_AV_AUDIO_EVENT,				/* Driver Callback 발생 시 필요한 경우 MW에 알려준다. */
	eCMD_AV_VIDEO_EVENT,				/* Driver Callback 발생 시 필요한 경우 MW에 알려준다. */
	eCMD_AV_CAPTURE_STILL,				/* I Frame (ES) Decoding 처리. */
	eCMD_AV_DECODE_STILL,				/* I Frame (ES) Decoding 처리. */
	eCMD_AV_PLAY_AUDIOCLIP,
	eCMD_AV_STOP_AUDIOCLIP,
	eCMD_AV_PAUSE_AUDIOCLIP,
	eCMD_AV_RESUME_AUDIOCLIP,
	eCMD_AV_CONFIG_STEREO,
	eCMD_AV_CONFIG_DUALMONO,
	eCMD_AV_HIDE_VIDEO,
	eCMD_AV_HIDE_AUDIO,
	eCMD_AV_SET_MAINMEDIA,				/* PIP */
	eCMD_AV_SET_MAINFCCMEDIA,			/* Fast ch change */
	eCMD_AV_SET_MAINMEDIA_ACTIONID,		/* set main media actionId for Fast ch change */
	eCMD_AV_PING,
	eCMD_AV_ENABLE_VIDEO,
	eCMD_AV_MAX
} AV_Command_t;

/* General 1 parameter 메시지 */
typedef struct
{
	AV_Command_t 			eCommand;
	HUINT32 				ulActionId;
} AV_CommandMsg_t;

/* 채널 변경 후 Video, Audio Decoding 시작 하는 메시지 . */
typedef struct
{
	AV_Command_t 			eCommand;
	Handle_t 				hAction;				/* action handle */

	HUINT32					ulDecId;
	HUINT32					ulDemuxId;

	HUINT16					usVideoPid;				/* video pid */
	HUINT16					usMainAudioPid;			/* audio pid */
	HUINT16					usAuxAudioPid;			/* audio description pid */
#if !defined(SvcAv_START_SERVICE_NEW)
	HUINT16					usDolbyPid;				/* dolby pd */
#endif
	HUINT16					usPcrPid;				/* pcr pid */

	DxVideoCodec_e			eVideoCodec;			/* video codec */
	DxAudioCodec_e			eMainAudioCodec;		/* main audio codec */
	DxAudioCodec_e			eAuxAudioCodec;			/* audio description codec */
#if !defined(SvcAv_START_SERVICE_NEW)
	DxAudioCodec_e			eDolbyCodec;			/* dolby codec */
	HBOOL					bDolbySelected;			/* dolby flag, wtv에서 결정해준다. */
	DxStereoSelect_e		eStereoSelect;			/* service 단위로 config가 다름. */
	DxDualMonoSelect_e		eDualMonoSelect;		/* service 단위로 config가 다름. */
#endif
	HBOOL					bFreezeUnderrun;		/* Underrrun시에 freeze 또는 mute */
	HBOOL					bUsePrimer;				/* fcc enable되었다면 live에서 true, pb에선 false 와야 한다. fcc disable이라면 always false  */

} AV_SessionStartMsg_t;

/* 채널 변경 후 Video, Audio Decoding 시작 하는 메시지 . */
typedef struct
{
	AV_Command_t 			eCommand;
	HUINT32 				ulActionId;
	ApiSyncMode_t 			syncMode;				/* Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
} AV_SessionStopMsg_t;

/* Video Source/Destination Window Rect 설정. */
typedef struct
{
	AV_Command_t			eCommand;
	HUINT32					ulDecId;
	HUINT32					eDeviceEvent;
	HUINT32					ulRequestId;
	HUINT32					version;
}AV_DeviceEventMsg_t;

typedef struct
{
	AV_Command_t			eCommand;
	Handle_t				hAction;
	HBOOL					bPlay;				/* TRUE : PLAY, FALSE : STOP */
	DxVideoCodec_e			eVideoCodec;				/* Iframe format */
	HUINT32					bufferSize;
	HUINT8					*buffer;
 } AV_DecodeStillMsg_t;

typedef struct
{
	AV_Command_t					eCommand;
	Handle_t						hAction;
 	DxVideoCodec_e					eVideoCodec;				/* Iframe format */
	HUINT32							bufferSize;
	HUINT8							*buffer;
	HUINT32							ulCaptureWidth;
	HUINT32							ulCaptureHeight;
	PixelFormat_e					ePixelFormat;
	SvcAv_CopyFrameBufferCallback_t	pfnCopyFrameBuffer;
} AV_CaptureStillMsg_t;

typedef struct
{
	AV_Command_t			eCommand;
	HUINT32 				ulRequestId;
	HUINT8					*pucBuffer;
	HUINT32 				ulBufferBytes;
	DxAudioCodec_e			eCodec;
	HUINT32 				ulLoopCnt;
} AV_AudioClipMsg_t;

typedef struct
{
	AV_Command_t			eCommand;
	HUINT32					ulActionId;
	DxStereoSelect_e		eSteteoSelect;
}AV_StereoCfgMsg_t;

typedef struct
{
	AV_Command_t			eCommand;
	HUINT32					ulActionId;
	DxDualMonoSelect_e		eDualMonoSelect;
}AV_DualMonoCfgMsg_t;

typedef struct
{
	AV_Command_t			eCommand;
	HUINT32					ulActionId;
	HBOOL					bFreeze;
}AV_FreezeMsg_t;

typedef struct
{
	AV_Command_t 			eCommand;
	HUINT32 				ulActionId;
	HUINT32					ulClientMask;
	HBOOL					bHideFlag;
} AV_SetHideMsg_t;

typedef struct
{
	AV_Command_t 			eCommand;
	HUINT32 				ulActionId;
	eSvcAv_DisplayType_e	eDisplayType;
	HBOOL					bEnableFlag;
} AV_SetEnableMsg_t;

typedef struct
{
	AV_Command_t 			eCommand;
	HUINT32 				ulParam1;
	HUINT32 				ulParam2;
} AV_DebugMsg_t;

/* make union with all of the message types */
typedef union
{
	AV_Command_t				eCommand;
	AV_CommandMsg_t				stMainMediaMsg;
	AV_SessionStartMsg_t		stSessionStartMsg;
	AV_SessionStopMsg_t			stSessionStopMsg;
	AV_SessionStartMsg_t		stChangePidMsg;
	AV_DeviceEventMsg_t			stDeviceEvtMsg;
	AV_CaptureStillMsg_t		stCaptureStillMsg;
	AV_DecodeStillMsg_t			stDecodeStillMsg;
	AV_AudioClipMsg_t			stAudioClipMsg;
	AV_StereoCfgMsg_t			stStereoCfgMsg;
	AV_DualMonoCfgMsg_t			stDualMonoCfgMsg;
	AV_FreezeMsg_t				stFreezeMsg;
	AV_SetHideMsg_t				stSetHideMsg;
	AV_SetEnableMsg_t			stSetEnableMsg;
	AV_DebugMsg_t				stDebugMsg;
} AV_Message_t;

/* MW_AV 의 Context Structure 이다. */
typedef struct
{
/* Session state information */
	Handle_t						hAction;				/* Action handle */
	HBOOL							bContextRunning;		/* Context activated */

/* Resource information */
	HUINT32							ulDecId;				/* Video decoder ID */
	HUINT32							ulDemuxId;				/* Demux ID */

/* Video decoding information */
	HUINT16							usVideoPid;				/* Video PID */
	DxVideoCodec_e					eVideoCodec;			/* Video codec */
	HBOOL							bVideoFreeze;
	HUINT32							ulVideoHideMask;		/* TV manager에서 요청한 action 단위의 video Hide mask */

/* Still picture decoding information */
	HUINT8							*pucStillBuffer;
	HUINT32 						ulStillBufferBytes;
	DxVideoCodec_e					eStillCodec;

/* Still picture capture information */
	HUINT32							ulCaptureWidth;
	HUINT32							ulCaptureHeight;
	PixelFormat_e					eCapturePixelFormat;
	SvcAv_CopyFrameBufferCallback_t	pfnCopyFrameBuffer;

/* Audio decoding information */
	HUINT16							usMainAudioPid;			/* audio pid */
	DxAudioCodec_e					eMainAudioCodec;		/* main audio codec */
	HUINT16							usAuxAudioPid;			/* audio description pid */
	DxAudioCodec_e					eAuxAudioCodec;			/* audio description codec */
#if !defined(SvcAv_START_SERVICE_NEW)
	HUINT16							usDolbyPid;				/* dolby pid */
	DxAudioCodec_e					eDolbyCodec;			/* dolby codec */
	HBOOL							bDolbySelected;			/* dolby flag, wtv에서 결정해준다. */
#endif
	DxStereoSelect_e				eStereoSelect;			/* 스테레오 설정. */
	DxDualMonoSelect_e				eDualMonoSelect;		/* 듀얼모노 설정. */
	HBOOL							bAudioFreeze;
	HUINT32							ulAudioHideMask;		/* TV manager에서 요청한 action 단위의 audio hide mask */

/* PCR PID */
	HUINT16							usPcrPid;				/* PCT PID */

	HBOOL							bFreezeUnderrun;
	HBOOL							bUsePrimer;				/* for fast ch change */

} AvContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC PostMsgToMgrCb_t 	s_fnSvcAv_PostMsgToAp = NULL;

static HULONG 				s_ulAvSyncSema;		/* semaphore for sync API */

static HULONG				s_ulAVCmdMsgQId;
static HULONG 				s_ulAVTaskId;
static HBOOL 				s_bAVInt;

static HUINT32				s_ulMainActionId;

static AvContext_t 			s_stAvContext[NUM_VIEW_ACTION];
static HUINT32 				s_ulCheckVideoEventVer[NUM_VIEW_ACTION][eVIDEO_EVENT_MAX_NUM];
static HUINT32 				s_ulCheckAudioEventVer[eAUDIO_EVENT_MAX];

static HUINT32				s_ulAudioClipRequestId;

static HINT8				*s_pcLastCommand;
#ifdef CONFIG_DEBUG
static HBOOL				s_bFccEnableByCommand;
#endif

static eSvcAv_ChChangeMode_e	s_eChannelChangeMode = eSvcAv_ChChangeMode_Mute;

#define	CHECK_ACTION_ID(ulActionId)		if(ulActionId >= NUM_VIEW_ACTION) \
										{ \
											HxLOG_Error("INVALID ACTION ID (%d). Exit ...\n", ulActionId); \
											return ERR_FAIL; \
										}

#if defined(CONFIG_SUPPORT_FCC)
#define FCC_MAIN_DECODER_ID				(0)
#endif


/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/

static void svc_av_Task(void *pParam);
static void svc_av_HandleMsg(AV_Message_t *stAvMsg);
static void svc_av_ResetContext(HUINT32 ulContextId);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ____DEBUG_______________________________________________________________________________

static const HINT8 *svc_av_Cmd2Str(AV_Command_t cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCMD_AV_START_SESSION);
		ENUM_TO_STR(eCMD_AV_STOP_SESSION);
		ENUM_TO_STR(eCMD_AV_CHANGE_PID);
		ENUM_TO_STR(eCMD_AV_FREEZE_VIDEO);
		ENUM_TO_STR(eCMD_AV_FREEZE_AUDIO);

		ENUM_TO_STR(eCMD_AV_AUDIO_EVENT);
		ENUM_TO_STR(eCMD_AV_VIDEO_EVENT);

		ENUM_TO_STR(eCMD_AV_CAPTURE_STILL);
		ENUM_TO_STR(eCMD_AV_DECODE_STILL);

		ENUM_TO_STR(eCMD_AV_PLAY_AUDIOCLIP);
		ENUM_TO_STR(eCMD_AV_STOP_AUDIOCLIP);
		ENUM_TO_STR(eCMD_AV_PAUSE_AUDIOCLIP);
		ENUM_TO_STR(eCMD_AV_RESUME_AUDIOCLIP);

		ENUM_TO_STR(eCMD_AV_CONFIG_STEREO);
		ENUM_TO_STR(eCMD_AV_CONFIG_DUALMONO);

		ENUM_TO_STR(eCMD_AV_HIDE_VIDEO);
		ENUM_TO_STR(eCMD_AV_HIDE_AUDIO);

		ENUM_TO_STR(eCMD_AV_SET_MAINMEDIA);
		ENUM_TO_STR(eCMD_AV_SET_MAINFCCMEDIA);
		ENUM_TO_STR(eCMD_AV_SET_MAINMEDIA_ACTIONID);

		ENUM_TO_STR(eCMD_AV_PING);
		ENUM_TO_STR(eCMD_AV_ENABLE_VIDEO);

		ENUM_TO_STR(eCMD_AV_MAX);
		default:
			break;
	}

	return ("Unknown");
}

static void svc_av_PrintSessionStartMessage(AV_SessionStartMsg_t *pParam)
{
	if(pParam == NULL)
		return;

	HxLOG_Print("========== TVSVC/AV session start parameters ==========\n");
	HxLOG_Print(" ActionID[%d], Main Action Id[%d]\n"	, PAL_PIPE_GetActionId(pParam->hAction), s_ulMainActionId);
	HxLOG_Print(" video pid       (0x%x), (%s)\n"		, pParam->usVideoPid, OTL_ENUM2STR_VideoCodec(pParam->eVideoCodec));
	HxLOG_Print(" pcr pid         (0x%x)\n"				, pParam->usPcrPid);
	HxLOG_Print(" main audio pid  (0x%x), (%s)\n"		, pParam->usMainAudioPid, OTL_ENUM2STR_AudioCodec(pParam->eMainAudioCodec));
	HxLOG_Print(" sub audio pid   (0x%x), (%s)\n"		, pParam->usAuxAudioPid, OTL_ENUM2STR_AudioCodec(pParam->eAuxAudioCodec));
#if !defined(SvcAv_START_SERVICE_NEW)
	HxLOG_Print(" dolby audio pid (0x%x), (%s)\n"		, pParam->usDolbyPid, OTL_ENUM2STR_AudioCodec(pParam->eDolbyCodec));
	HxLOG_Print(" dolby flag      (%d)\n"				, pParam->bDolbySelected);
	HxLOG_Print(" stereo select   (%s)\n"				, OTL_ENUM2STR_StereoSelect(pParam->eStereoSelect));
	HxLOG_Print(" dualmono select (%s)\n"				, OTL_ENUM2STR_DualMonoSelect(pParam->eDualMonoSelect));
#endif
	HxLOG_Print("=======================================================\n");
	HxLOG_Print("\n\n");

	return;
}

#define ____CONTEXT_____________________________________________________________________________________



#define _____DEVICE_EVT_____________________________________________________________________________________________

static void svc_av_VideoEventCallback(HUINT32 ulDecId, PalVideo_Event_e eVideoEvent)
{
	AV_Message_t		stAvMsg;
	HERROR				err;

	HxLOG_Debug("\t%s(Decoder[%d]. Event (%s) \n", __FUNCTION__, ulDecId, PAL_VIDEO_GetStrDeviceEvt(eVideoEvent));

	stAvMsg.stDeviceEvtMsg.eCommand = eCMD_AV_VIDEO_EVENT;
	stAvMsg.stDeviceEvtMsg.eDeviceEvent = eVideoEvent;
	stAvMsg.stDeviceEvtMsg.ulDecId= ulDecId;
	stAvMsg.stDeviceEvtMsg.version = ++s_ulCheckVideoEventVer[ulDecId][eVideoEvent];

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\t\tERROR.Message Send Failure. \n");
		return;
	}

	return;
}

static void svc_av_AudioEventCallback(PalAudio_Event_e eAudioEvent, HUINT32 ulRequestId)
{
	AV_Message_t 		stAvMsg;
	HERROR				err;

	HxLOG_Debug("\t%s(Event (%s), Req ID(%d)\n", __FUNCTION__, PAL_AUDIO_GetStrDeviceEvt(eAudioEvent), ulRequestId);

	stAvMsg.stDeviceEvtMsg.eCommand = eCMD_AV_AUDIO_EVENT;
	stAvMsg.stDeviceEvtMsg.eDeviceEvent = eAudioEvent;
	stAvMsg.stDeviceEvtMsg.ulDecId= 0;
	stAvMsg.stDeviceEvtMsg.version = ++s_ulCheckAudioEventVer[eAudioEvent];
	stAvMsg.stDeviceEvtMsg.ulRequestId = ulRequestId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\t\tERROR.Message Send Failure. \n");
		return;
	}

	return;
}

#define _____OUTPUT_SETTING___________________________________________________________________________________

STATIC HINT32 svc_av_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcAv_PostMsgToAp)
	{
		return (* s_fnSvcAv_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return 1;	// error !
}

static HERROR svc_av_CheckStereoConfig(HUINT32 ulContextId)
{
	HERROR					err;
	PalAudio_StreamInfo_t		stStreamInfo;
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	err = PAL_AUDIO_GetStreamInfo(&stStreamInfo);
	if(err != ERR_OK)
	{
		HxLOG_Error("ERROR from PAL_AUDIO_GetStreamInfo(). Exit ...\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("\t\tStream stereo type is %s.\n", OTL_ENUM2STR_StereoFormat(stStreamInfo.eStereoType));
	}

	if(stStreamInfo.eStereoType == eDxSTEREO_FORMAT_DUALMONO)
	{
		HxLOG_Debug("\t\tApply dual mono setting %s.\n"
						, OTL_ENUM2STR_DualMonoSelect(pstAvContext->eDualMonoSelect));

		switch(pstAvContext->eDualMonoSelect)
		{
			case eDxDUALMONO_SELECT_LEFT:
				PAL_AUDIO_SetStereoConfig(eDxSTEREO_SELECT_MONOLEFT);
				break;

			case eDxDUALMONO_SELECT_RIGHT:
				PAL_AUDIO_SetStereoConfig(eDxSTEREO_SELECT_MONORIGHT);
				break;

			default:
				PAL_AUDIO_SetStereoConfig(eDxSTEREO_SELECT_STEREO);
				break;
		}
	}
	else
	{
		HxLOG_Debug("\t\tApply stereo setting %s.\n"
						, OTL_ENUM2STR_StereoSelect(pstAvContext->eStereoSelect));

		PAL_AUDIO_SetStereoConfig(pstAvContext->eStereoSelect);
	}

	return ERR_OK;
}

static void svc_av_CmdStereoCfg(AV_StereoCfgMsg_t *pstStereoCfgMsg)
{
	HUINT32				ulContextId = pstStereoCfgMsg->ulActionId;

	HxLOG_Debug("\t(%d)\n", ulContextId);

	s_stAvContext[ulContextId].eStereoSelect = pstStereoCfgMsg->eSteteoSelect;

	if(ulContextId == s_ulMainActionId)
	{
		svc_av_CheckStereoConfig(ulContextId);
	}
	else
	{
		HxLOG_Debug("\t\tNot an ACTIVE Audio action\n");
	}

	return;
}

static void svc_av_CmdDualMonoCfg(AV_DualMonoCfgMsg_t *pstDualMonoCfgMsg)
{
	HUINT32				ulContextId = pstDualMonoCfgMsg->ulActionId;

	HxLOG_Debug("\t(%d)\n", ulContextId);

	s_stAvContext[ulContextId].eDualMonoSelect = pstDualMonoCfgMsg->eDualMonoSelect;

	if(ulContextId == s_ulMainActionId)
	{
		svc_av_CheckStereoConfig(ulContextId);
	}
	else
	{
		HxLOG_Debug("\t\tNot an ACTIVE Audio action\n");
	}

	return;
}

static void svc_av_CmdVideoEvent(AV_DeviceEventMsg_t *pstDeviceEventMsg)
{
	HUINT32				i;
	HUINT32				ulContextId = (HUINT32)-1;
	HUINT32				ulDecId = pstDeviceEventMsg->ulDecId;
	AvContext_t			*pstAvContext = NULL;
	VideoStreamInfo_t	stStreamInfo;
	HERROR				err;

	HxLOG_Debug("\t(Event:%s, Decoder ID:%d, version:%d)\n"
					, PAL_VIDEO_GetStrDeviceEvt(pstDeviceEventMsg->eDeviceEvent)
					, pstDeviceEventMsg->ulDecId
					, pstDeviceEventMsg->version
					);

/* 1. Version expire 확인 */
	if(s_ulCheckVideoEventVer[ulDecId][pstDeviceEventMsg->eDeviceEvent] != pstDeviceEventMsg->version)
	{
		HxLOG_Debug("\t\tversion %d != %d\n",
									s_ulCheckVideoEventVer[ulDecId][pstDeviceEventMsg->eDeviceEvent],
									pstDeviceEventMsg->version);
		return;
	}

/* 2. 해당 디코더 할당받은 context 검색 */
	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		pstAvContext = &s_stAvContext[i];
		if(pstAvContext->ulDecId == pstDeviceEventMsg->ulDecId)
		{
			ulContextId = i;

#if defined(CONFIG_SUPPORT_FCC)
			// fcc는 context 관리를 위해 가상의 decId를 사용한다. (DI에 call할때는 0 사용)
			// 따라서 decId 만으로는 실제 context를 못 찾는 경우가 발생하는데, 이때는 main action id로 설정해 준다
			if ((s_stAvContext[ulContextId].bContextRunning != TRUE) || (s_stAvContext[ulContextId].hAction == HANDLE_NULL))
			{
				ulContextId = s_ulMainActionId;
				pstAvContext = &s_stAvContext[ulContextId];
			}
#endif

			break;
		}
	}

	if(ulContextId == (HUINT32)-1)
	{ /* 왜 발견이 안되지? */
		HxLOG_Error("\t\tERROR \n");
		return;
	}

	/* Buffer Underrun 이 발생할 수 있는 원인 List */
	/*
		1. Signal Out
		2. CAS 권한 없음.
		3. Video pid is wrong.
		4. Audio pid is wrong.
		5. Video codec is wrong.
		6. Audio codec is wrong.
		7. unknown (driver fault).
	*/

	switch(pstDeviceEventMsg->eDeviceEvent)
	{
		case eVIDEO_EVENT_UNDERRUN:
		{
			svc_av_PostMsgToAp(eSEVT_AV_VIDEO_UNDERRUN, pstAvContext->hAction, 0, 0, 0);

			if(pstAvContext->bFreezeUnderrun == FALSE)
			{
				PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Decoder, TRUE);
			}

			break;
		}

		case eVIDEO_EVENT_FRAME_START:
		{
			if(pstAvContext->pfnCopyFrameBuffer != NULL)
			{ /* Capture 모드 */
				HUINT32					ulBufferSize;
				HUINT8					*pucFrameBuffer;

				/* Video scaling */
				err = PAL_VIDEO_GetStreamInfo(ulDecId, &stStreamInfo);
				if(err != ERR_OK)
				{
					HxLOG_Error("\t\tERROR \n");
					break;
				}

				err = PAL_SCALER_SetVideoAttribute(ulDecId,
											ulContextId,
											(ulContextId == s_ulMainActionId),
											stStreamInfo.ulHorizontalSize,
											stStreamInfo.ulVerticalSize,
											stStreamInfo.eAspectRatio,
											eAfd_Full,
											stStreamInfo.bProgressive,
											stStreamInfo.eFrameRate,
											stStreamInfo.e3DFormat
											);

				if(err != ERR_OK)
				{
					HxLOG_Error("\t\tERROR \n");
					break;
				}

				/* Capture */
				PAL_THUMB_CaptureImageToBuffer(ulDecId,
													pstAvContext->ulCaptureWidth,
													pstAvContext->ulCaptureHeight,
													pstAvContext->eCapturePixelFormat,
													&pucFrameBuffer, &ulBufferSize);

				PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);

				pstAvContext->pfnCopyFrameBuffer(pucFrameBuffer, pstAvContext->ulCaptureWidth, pstAvContext->ulCaptureHeight, ulBufferSize);

				PAL_THUMB_FreeBuffer(pucFrameBuffer);

				pstAvContext->pfnCopyFrameBuffer = NULL;

				if(pstAvContext->pucStillBuffer != NULL)
				{ /* 이전에 decode still 중이었으면 다시 살린다. Decode still 먼저 live 나중. */
					PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
												pstAvContext->pucStillBuffer,
												pstAvContext->ulStillBufferBytes,
												pstAvContext->eStillCodec,
												FALSE);
				}
				else if(pstAvContext->usVideoPid < PID_NULL)
				{ /* 이전 pid video가 valid하면 다시 살린다. */
#if defined(CONFIG_SUPPORT_FCC)
					if (pstAvContext->bUsePrimer == FALSE)
					{
						PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
					}
					else
					{
						PAL_VIDEO_StartPrimerDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec, (ulContextId == s_ulMainActionId), ulContextId);
					}
#else
					PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
#endif
				}
				else
				{
					PAL_SCALER_SetVideoUsage(pstAvContext->ulDecId, ePAL_SCALER_VIDEOUSAGE_NORMAL);
				}
			}
			else
			{ /* Still picture, PID video 모드 */
				PAL_VIDEO_GetStreamInfo(ulDecId, &stStreamInfo);

				PAL_SCALER_SetVideoAttribute(ulDecId,
											ulContextId,
											(ulContextId == s_ulMainActionId),
											stStreamInfo.ulHorizontalSize,
											stStreamInfo.ulVerticalSize,
											stStreamInfo.eAspectRatio,
											stStreamInfo.eAfd,
											stStreamInfo.bProgressive,
											stStreamInfo.eFrameRate,
											stStreamInfo.e3DFormat
											);

				PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Decoder, FALSE);

				if(pstAvContext->pucStillBuffer == NULL)
				{
					svc_av_PostMsgToAp(eSEVT_AV_VIDEO_STARTED, pstAvContext->hAction, 0, 0, 0);
				}
			}

			break;
		}

		case eVIDEO_EVENT_PARAM_CHANGED:
		{
			PAL_VIDEO_GetStreamInfo(ulDecId, &stStreamInfo);

			PAL_SCALER_SetVideoAttribute(ulDecId,
										ulContextId,
										(ulContextId == s_ulMainActionId),
										stStreamInfo.ulHorizontalSize,
										stStreamInfo.ulVerticalSize,
										stStreamInfo.eAspectRatio,
										stStreamInfo.eAfd,
										stStreamInfo.bProgressive,
										stStreamInfo.eFrameRate,
										stStreamInfo.e3DFormat
										);

			svc_av_PostMsgToAp(eSEVT_AV_CHECK_VIDEO_STATUS, pstAvContext->hAction, 0, 0, 0);

			break;
		}

		default:
			HxLOG_Error("\t\tERROR Unknown DI EVENT [%d]\n", pstDeviceEventMsg->eDeviceEvent);
			break;
	}

	return;
}

static void  svc_av_CmdAudioEvent(AV_DeviceEventMsg_t *pstDeviceEventMsg)
{
	HUINT32					ulContextId = s_ulMainActionId;
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];

	HxLOG_Debug("\t(Event:%s, ulDecId:%d, requestId:%d, version:%d)\n"
					, PAL_AUDIO_GetStrDeviceEvt(pstDeviceEventMsg->eDeviceEvent)
					, pstDeviceEventMsg->ulDecId
					, pstDeviceEventMsg->ulRequestId
					, pstDeviceEventMsg->version
					);

	if(s_ulCheckAudioEventVer[pstDeviceEventMsg->eDeviceEvent] != pstDeviceEventMsg->version)
	{
		HxLOG_Debug("\t\tversion %d != %d\n",
						s_ulCheckAudioEventVer[pstDeviceEventMsg->eDeviceEvent],
						pstDeviceEventMsg->version);
		return;
	}

	switch(pstDeviceEventMsg->eDeviceEvent)
	{
		case eAUDIO_EVENT_UNDERRUN:
			PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);

			svc_av_PostMsgToAp(eSEVT_AV_AUDIO_UNDERRUN, pstAvContext->hAction, 0, 0, 0);

			break;

		case eAUDIO_EVENT_FRAME_START:
			svc_av_CheckStereoConfig(ulContextId);

			PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);

			svc_av_PostMsgToAp(eSEVT_AV_AUDIO_STARTED, pstAvContext->hAction, 0, 0, 0);

			break;

		case eAUDIO_EVENT_PARAM_CHANGED:
			svc_av_CheckStereoConfig(ulContextId);

			svc_av_PostMsgToAp(eSEVT_AV_CHECK_AUDIO_STATUS, pstAvContext->hAction, 0, 0, 0);

			break;

		case eAUDIO_EVENT_CLIP_COMPLETE:

			svc_av_PostMsgToAp(eSEVT_AV_AUDIO_CLIP_COMPLETED, pstAvContext->hAction, pstDeviceEventMsg->ulRequestId, 0, 0);

			break;

		default:

			HxLOG_Error("\t\tERROR Unknown AUDIO EVENT [%d]\n", pstDeviceEventMsg->eDeviceEvent);

			break;
	}

	return;
}

static HERROR svc_av_MakeAudioDecodeParam(
												HUINT16 				usMainAudioPid,
												DxAudioCodec_e			eMainAudioCodec,
												HUINT16 				usAuxAudioPid,
												DxAudioCodec_e			eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
												HBOOL					bDolbySelected,
												HUINT16 				usDolbyPid,
												DxAudioCodec_e			eDolbyCodec,
#endif
												PalAudio_DecodeParam_t		*pstAudioDecParam
												)
{
	HxLOG_Debug("\n");
	HxLOG_Debug("MAIN : PID[0x%X], %s\n", usMainAudioPid, OTL_ENUM2STR_AudioCodec(eMainAudioCodec));
	HxLOG_Debug("SUB : PID[0x%X], %s\n", usAuxAudioPid, OTL_ENUM2STR_AudioCodec(eAuxAudioCodec));
#if !defined(SvcAv_START_SERVICE_NEW)
	HxLOG_Debug("DOLBY[%d] : PID[0x%X], %s\n", bDolbySelected, usDolbyPid, OTL_ENUM2STR_AudioCodec(eDolbyCodec));
#endif

/* Main audio decoder */
	if (usMainAudioPid < PID_NULL)
	{
		pstAudioDecParam->usMainPid = usMainAudioPid;
		pstAudioDecParam->eMainCodec = eMainAudioCodec;
	}
	else
	{
		pstAudioDecParam->usMainPid = PID_NULL;
		pstAudioDecParam->eMainCodec = eDxAUDIO_CODEC_UNKNOWN;
	}

/* Sub audio decoder */
	if(usAuxAudioPid < PID_NULL)
	{
		pstAudioDecParam->usSubPid = usAuxAudioPid;
		pstAudioDecParam->eSubCodec = eAuxAudioCodec;
	}
	else
	{
		pstAudioDecParam->usSubPid = PID_NULL;
		pstAudioDecParam->eSubCodec = eDxAUDIO_CODEC_UNKNOWN;
	}

#if !defined(SvcAv_START_SERVICE_NEW)
#ifdef CONFIG_MW_AV_DOLBY_DECODE
	if(bDolbySelected == TRUE)
	{
		if( (PAL_AUDIO_IsSupportCodec(eAUDIO_DEC_MAIN, eDxAUDIO_CODEC_DOLBY_AC3) == TRUE)
	#ifdef CONFIG_MW_AV_DOLBY_PLUS
			|| (PAL_AUDIO_IsSupportCodec(eAUDIO_DEC_MAIN, eDxAUDIO_CODEC_DOLBY_AC3P) == TRUE)
	#endif
		)
		{
			if( (pstAudioDecParam->eMainCodec == eDxAUDIO_CODEC_DOLBY_AC3)
				|| (pstAudioDecParam->eMainCodec == eDxAUDIO_CODEC_DOLBY_AC3P) )
			{
				HxLOG_Debug("\t\tMAIN is Dolby : PID[0x%X], %s\n", pstAudioDecParam->usMainPid, OTL_ENUM2STR_AudioCodec(pstAudioDecParam->eMainCodec));
			}
			else
			{
				HxLOG_Debug("\t\tSelect Dolby : PID[0x%X], %s\n", usDolbyPid, OTL_ENUM2STR_AudioCodec(eDolbyCodec));
				pstAudioDecParam->usMainPid = usDolbyPid;
				pstAudioDecParam->eMainCodec = eDolbyCodec;
			}
		}
		else
		{
			HxLOG_Error("\t\tERROR. dolby is not supported by platform \n");
		}
	}
#else
	if(bDolbySelected == TRUE)
	{
		pstAudioDecParam->usMainPid = PID_NULL;
		pstAudioDecParam->eMainCodec = eDxAUDIO_CODEC_UNKNOWN;
	}
#endif
#endif

	HxLOG_Debug("\t\tDecParam : MAIN : PID[0x%X], %s\n", pstAudioDecParam->usMainPid, OTL_ENUM2STR_AudioCodec(pstAudioDecParam->eMainCodec));
	HxLOG_Debug("\t\tDecParam : SUB : PID[0x%X], %s\n", pstAudioDecParam->usSubPid, OTL_ENUM2STR_AudioCodec(pstAudioDecParam->eSubCodec));

	return ERR_OK;
}

static void  svc_av_CmdCaptureStill(AV_CaptureStillMsg_t *pstCaptureStillMsg)
{
	HUINT32			ulContextId = PAL_PIPE_GetActionId(pstCaptureStillMsg->hAction);
	AvContext_t		*pstAvContext = &s_stAvContext[ulContextId];

	HxLOG_Debug("\t(%d, %s, bufferSize(0x%x), *buffer(0x%x), %d, %d, Format(%d), pfn(0x%x))\n",
						ulContextId,
						OTL_ENUM2STR_VideoCodec(pstCaptureStillMsg->eVideoCodec),
						pstCaptureStillMsg->bufferSize,
						(HUINT32)pstCaptureStillMsg->buffer,
						pstCaptureStillMsg->ulCaptureWidth,
						pstCaptureStillMsg->ulCaptureHeight,
						pstCaptureStillMsg->ePixelFormat,
						pstCaptureStillMsg->pfnCopyFrameBuffer
						);

	HxLOG_Debug("\t\tStarting with Dec Id (%d)\n", pstAvContext->ulDecId);
	/* 현재 돌고 있는 것 모두 stop */
	PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
	PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);

	PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
								pstCaptureStillMsg->buffer,
								pstCaptureStillMsg->bufferSize,
								pstCaptureStillMsg->eVideoCodec,
								TRUE);

	pstAvContext->ulCaptureWidth = pstCaptureStillMsg->ulCaptureWidth;
	pstAvContext->ulCaptureHeight = pstCaptureStillMsg->ulCaptureHeight;
	pstAvContext->eCapturePixelFormat = pstCaptureStillMsg->ePixelFormat;
	pstAvContext->pfnCopyFrameBuffer = pstCaptureStillMsg->pfnCopyFrameBuffer;
}

static void  svc_av_CmdDecodeStill(AV_DecodeStillMsg_t *pstDecodeStillMsg)
{
	HERROR			err;
	HUINT32			ulContextId = PAL_PIPE_GetActionId(pstDecodeStillMsg->hAction);
	AvContext_t		*pstAvContext = &s_stAvContext[ulContextId];

	HxLOG_Debug("\t(%d, bPlay(%d), %s, bufferSize(0x%x), *buffer(0x%x)\n",
					ulContextId, pstDecodeStillMsg->bPlay,
					OTL_ENUM2STR_VideoCodec(pstDecodeStillMsg->eVideoCodec),
					pstDecodeStillMsg->bufferSize,
					(HUINT32)pstDecodeStillMsg->buffer);

	if(pstDecodeStillMsg->bPlay == TRUE)
	{
		HxLOG_Debug("\t\tStarting with Dec Id (%d)\n", pstAvContext->ulDecId);
		/* 현재 돌고 있는 것 모두 stop */
		PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
		PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);

		PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
									pstDecodeStillMsg->buffer,
									pstDecodeStillMsg->bufferSize,
									pstDecodeStillMsg->eVideoCodec,
									FALSE);

		pstAvContext->pucStillBuffer = pstDecodeStillMsg->buffer;
		pstAvContext->ulStillBufferBytes = pstDecodeStillMsg->bufferSize;
		pstAvContext->eStillCodec = pstDecodeStillMsg->eVideoCodec;
	}
	else
	{
		err = PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);
		if(err == ERR_OK)
		{
			PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Decoder, TRUE);
		}

		pstAvContext->pucStillBuffer = NULL;
		pstAvContext->ulStillBufferBytes = 0;
		pstAvContext->eStillCodec = eDxVIDEO_CODEC_UNKNOWN;

		if(pstAvContext->usVideoPid < PID_NULL)
		{ /* 이전 pid video가 valid하면 다시 살린다. */
			PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
		}
	}

	return;
}

static void  svc_av_CmdAudioClip(AV_AudioClipMsg_t *pstAudioClipMsg)
{
	HERROR					err;
	AvContext_t				*pstAvContext = &s_stAvContext[s_ulMainActionId];
	PalAudio_DecodeParam_t		stAudioDecodeParam;

	HxLOG_Debug("\n");

	switch(pstAudioClipMsg->eCommand)
	{
		case eCMD_AV_PLAY_AUDIOCLIP:
			/* 혹시 돌고 있을 clip/live stop */
			err = PAL_AUDIO_StopAudioClip(s_ulAudioClipRequestId);
			err = PAL_AUDIO_StopDecoding();

			err = PAL_AUDIO_StartAudioClip(pstAudioClipMsg->ulRequestId,
											pstAudioClipMsg->pucBuffer,
											pstAudioClipMsg->ulBufferBytes,
											pstAudioClipMsg->eCodec,
											pstAudioClipMsg->ulLoopCnt
											);

			if(err == ERR_OK)
			{
				s_ulAudioClipRequestId = pstAudioClipMsg->ulRequestId;
			}

			break;

		case eCMD_AV_STOP_AUDIOCLIP:
			if(pstAudioClipMsg->ulRequestId == s_ulAudioClipRequestId)
			{
				err = PAL_AUDIO_StopAudioClip(s_ulAudioClipRequestId);

				if(err == ERR_OK)
				{ /* Clip이 제대로 멈춘 경우 live 동작 중이었으면 다시 시작시킨다. */
					svc_av_MakeAudioDecodeParam(pstAvContext->usMainAudioPid,
													pstAvContext->eMainAudioCodec,
													pstAvContext->usAuxAudioPid,
													pstAvContext->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
													pstAvContext->bDolbySelected,
													pstAvContext->usDolbyPid,
													pstAvContext->eDolbyCodec,
#endif
													&stAudioDecodeParam);

					if(stAudioDecodeParam.usMainPid < PID_NULL)
					{
						PAL_AUDIO_StartDecoding(&stAudioDecodeParam);
					}
				}
			}
			else
			{
				HxLOG_Debug("\t\t=> Request ID(%d) is not current play ID(%d). Request Ignored ... sorry.\n",
											pstAudioClipMsg->ulRequestId,
											s_ulAudioClipRequestId
											);
			}

			break;

		case eCMD_AV_PAUSE_AUDIOCLIP:
			if(pstAudioClipMsg->ulRequestId == s_ulAudioClipRequestId)
			{
				PAL_AUDIO_PauseAudioClip(s_ulAudioClipRequestId);
			}
			else
			{
				HxLOG_Debug("\t\t=> Request ID(%d) is not current play ID(%d). Request Ignored ... sorry.\n",
											pstAudioClipMsg->ulRequestId,
											s_ulAudioClipRequestId
											);
			}

			break;

		case eCMD_AV_RESUME_AUDIOCLIP:
			if(pstAudioClipMsg->ulRequestId == s_ulAudioClipRequestId)
			{
				PAL_AUDIO_ResumeAudioClip(s_ulAudioClipRequestId);
			}
			else
			{
				HxLOG_Debug("\t\t=> Request ID(%d) is not current play ID(%d). Request Ignored ... sorry.\n",
											pstAudioClipMsg->ulRequestId,
											s_ulAudioClipRequestId
											);
			}

			break;

		default:
			break;
	}
}

static HERROR svc_av_StopSession(HUINT32 ulContextId)
{
	AvContext_t		*pstAvContext = &s_stAvContext[ulContextId];
	HERROR			err;

	if(pstAvContext->bContextRunning == TRUE)
	{
		HxLOG_Debug("\t(%d)\n", ulContextId);
	}
	else
	{ /* 이미 stop 됨. */
		HxLOG_Debug("\t(%d) Already inactive context. Exit...\n", ulContextId);
		return ERR_FAIL;
	}

	/*
		di interface와 정의한 sequence. 추후 platform 변경 후에도 유지되어야 함.

		1. audio decoding stop.
		2. video decoding stop.
		3. pid filter stop.
	*/
#if defined(CONFIG_SUPPORT_FCC)
	if(ulContextId == s_ulMainActionId)
#endif
	{
		PAL_SCALER_ClearVideoAttribute(pstAvContext->ulDecId);
	}

/* 1. 순서 지킬 것 : PID audio -> Audio clip -> PID video
주의 : I-frame 및 Audio Clip은 session 과 연동되지 않는다. Still context를 reset하거나 stop 하지 말것. */
	/* PID audio stop */
	if(ulContextId == s_ulMainActionId)
	{
#if defined(CONFIG_SUPPORT_FCC)
		if (pstAvContext->bUsePrimer == FALSE)
		{
			err = PAL_AUDIO_StopDecoding();
		}
		else
		{
#if defined(FCC_USE_AUDIO_PRIMER)
			err = PAL_AUDIO_StopPrimerDecoding(ulContextId);
			err = PAL_AUDIO_StopPrimer(ulContextId);
#else
			err = PAL_AUDIO_StopDecoding();
#endif
		}
#else
		err = PAL_AUDIO_StopDecoding();
#endif
		if(err == ERR_OK)
		{
			PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
		}
	}
#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
	else
	{
		if (pstAvContext->bUsePrimer == FALSE)
		{
			err = PAL_AUDIO_StopDecoding();
		}
		else
		{
			err = PAL_AUDIO_StopPrimer(ulContextId);
		}

		if (err != ERR_OK)
		{
			HxLOG_Error("\t\tERROR. PAL_AUDIO_StopPrimer() failed.. \n");
		}
	}
#endif

	/* PID video stop */
#if defined(CONFIG_SUPPORT_FCC)
	if (pstAvContext->bUsePrimer == FALSE)
	{
		err = PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
	}
	else
	{
		err = PAL_VIDEO_StopPrimerDecoding(pstAvContext->ulDecId, ulContextId);
		err |= PAL_VIDEO_StopPrimer(pstAvContext->ulDecId, ulContextId);
	}
#else
	err = PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
#endif
	if(err == ERR_OK)
	{
		PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Decoder, TRUE);
	}

/* 2. PID filter stop */
#if defined(CONFIG_SUPPORT_FCC)
	if (pstAvContext->bUsePrimer == FALSE)
	{
		err = PAL_DMX_StopAllAVPid(pstAvContext->ulDemuxId);
	}
	else
	{
		err = PAL_DMX_StopAllAVPrimerPid(pstAvContext->ulDemuxId, ulContextId);

#if !defined(FCC_USE_AUDIO_PRIMER)
		if(ulContextId == s_ulMainActionId)
		{
			if(ERR_OK!=err)		{ HxLOG_Critical("Critical Eror!!!\n"); }

			err = PAL_DMX_StopAllAVPid(pstAvContext->ulDemuxId);
		}
#endif
	}
#else
	err = PAL_DMX_StopAllAVPid(pstAvContext->ulDemuxId);
#endif
	if(ERR_OK!=err)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	return ERR_OK;
}


static void  svc_av_CmdStopSession(AV_SessionStopMsg_t *pstSessionStopMsg)
{
	HERROR		err;

	HxLOG_Debug("\t(%d)\n", pstSessionStopMsg->ulActionId);

	err = svc_av_StopSession(pstSessionStopMsg->ulActionId);

	if(err == ERR_OK)
	{
		svc_av_ResetContext(pstSessionStopMsg->ulActionId);
	}

	return;
}

static void svc_av_StartSession(HUINT32 ulContextId)
{
	HERROR					err ;
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];
	PalAudio_DecodeParam_t		stAudioDecodeParam;
	PalDmx_PidInfo_t			stPidInfo;
	HBOOL						 bHide = FALSE;

	if(pstAvContext->bContextRunning == TRUE)
	{
		HxLOG_Debug("\t(%d, %s)\n", ulContextId, (ulContextId == s_ulMainActionId) ? "MAIN" : "SUB");
	}
	else
	{
		HxLOG_Debug("\t(%d) This context is not active. Exit ...\n", ulContextId);
		return;
	}

	svc_av_MakeAudioDecodeParam(pstAvContext->usMainAudioPid,
									pstAvContext->eMainAudioCodec,
									pstAvContext->usAuxAudioPid,
									pstAvContext->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
									pstAvContext->bDolbySelected,
									pstAvContext->usDolbyPid,
									pstAvContext->eDolbyCodec,
#endif
									&stAudioDecodeParam);

	stPidInfo.usPcrPid = pstAvContext->usPcrPid;
	stPidInfo.usVideoPid = pstAvContext->usVideoPid;
	stPidInfo.usMainAudioPid = stAudioDecodeParam.usMainPid;
	stPidInfo.usSubAudioPid = stAudioDecodeParam.usSubPid;
#if 0//defined(CONFIG_DEBUG)
	{
		HxLOG_Print("\n\n[%s:%d] PCR(0x%X) VID(0x%X) M_AUD(0x%X) S_AUD(0x%X) \n\n",__FUNCTION__,__LINE__,
				stPidInfo.usPcrPid,
				stPidInfo.usVideoPid,
				stPidInfo.usMainAudioPid,
				stPidInfo.usSubAudioPid);
	}
#endif

	/*
		di interface와 정의한 sequence. 추후 platform 변경 후에도 유지되어야 함.

		1. pid filter start.
		2. video decoding start.
		3. audio decoding start.
	*/
#if defined(CONFIG_SUPPORT_FCC)
	if (pstAvContext->bUsePrimer == FALSE)
	{
		err = PAL_DMX_StartAVPid(pstAvContext->ulDemuxId, &stPidInfo, (ulContextId == s_ulMainActionId));
		if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");
	}
	else
	{
#if !defined(FCC_USE_AUDIO_PRIMER)
		PalDmx_PidInfo_t			 stDmxPidInfo;

		HxSTD_MemCopy(&stDmxPidInfo, &stPidInfo, sizeof(PalDmx_PidInfo_t));
		stDmxPidInfo.usMainAudioPid = PID_NULL;
		stDmxPidInfo.usSubAudioPid= PID_NULL;
		if (PID_NULL == stPidInfo.usVideoPid)
		{
			// video pid가 없다면 pcr pid는 PAL_DMX_StartAVPid()에서 설정해주고 여기서는 null로 설정해주자.
			stDmxPidInfo.usPcrPid = PID_NULL;
		}

		err = PAL_DMX_StartAVPrimerPid(pstAvContext->ulDemuxId, &stDmxPidInfo, (ulContextId == s_ulMainActionId), ulContextId);
		if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");

		if (ulContextId == s_ulMainActionId)
		{
			HxSTD_MemCopy(&stDmxPidInfo, &stPidInfo, sizeof(PalDmx_PidInfo_t));
			stDmxPidInfo.usVideoPid = PID_NULL;
			if (stPidInfo.usVideoPid != PID_NULL)
			{
				// video pid가 있으면  pcr pid는 PAL_DMX_StartAVPrimerPid()에서 설정해주었으므로 여기서는 null.
				stDmxPidInfo.usPcrPid = PID_NULL;
			}

			err = PAL_DMX_StartAVPid(pstAvContext->ulDemuxId, &stDmxPidInfo, TRUE);
			if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");
		}
#else
		err = PAL_DMX_StartAVPrimerPid(pstAvContext->ulDemuxId, &stPidInfo, TRUE, ulContextId);
		if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");
#endif


	}
#else
	err = PAL_DMX_StartAVPid(pstAvContext->ulDemuxId, &stPidInfo, (ulContextId == s_ulMainActionId));
	if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");
#endif


/************************************************************************************
Step 3) 디코더를 시작 시킨다.
*************************************************************************************/
	if(pstAvContext->usVideoPid < PID_NULL)
	{
		/* PID가 valid하면 still picture에 우선한다. 혹시 있을 still 먼저 stop 하고... */
		PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);

		/* PID decoding 으로 다시 시작 */
#if defined(CONFIG_SUPPORT_FCC)
		if (pstAvContext->bUsePrimer == FALSE)
		{
			PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
		}
		else
		{
			PAL_VIDEO_StartPrimerDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec, (ulContextId == s_ulMainActionId), ulContextId);
		}
#else
		PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
#endif
	}
	else if(pstAvContext->pucStillBuffer != NULL)
	{ /* PID 가 없는데, still picture가 해제되어 있지 않다면 다시 살려야 한다. */
		PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
									pstAvContext->pucStillBuffer,
									pstAvContext->ulStillBufferBytes,
									pstAvContext->eStillCodec,
									FALSE);
	}

	/* Session이 start 되기전에 또는 PIP swap 으로 인하여 action hide는 다시 해줘야 한다. */
	PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Action, (pstAvContext->ulVideoHideMask != 0));

#if !defined(CONFIG_SUPPORT_FCC) || !defined(FCC_USE_AUDIO_PRIMER)
	if(ulContextId == s_ulMainActionId)
#endif
	{
#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
		if (pstAvContext->bUsePrimer == FALSE)
		{
			PAL_AUDIO_StartDecoding(&stAudioDecodeParam);
		}
		else
		{
			PAL_AUDIO_StartPrimerDecoding(&stAudioDecodeParam, (ulContextId == s_ulMainActionId), ulContextId, TRUE);
		}
#else
		PAL_AUDIO_StartDecoding(&stAudioDecodeParam);
#endif

		bHide = (pstAvContext->ulAudioHideMask != 0) ? TRUE : FALSE;
		PAL_AUDIO_SetHide(eAvHideClient_Action, bHide);
	}
}

static void  svc_av_CmdStartSession(AV_SessionStartMsg_t *pstSessionStartMsg)
{
	HUINT32					ulContextId = PAL_PIPE_GetActionId(pstSessionStartMsg->hAction);
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];
	HERROR					err ;

	HxLOG_Debug("\tEnter : ulContextId(%d)\n", ulContextId);

	err = PAL_PIPE_IsActionHandleLatest(pstSessionStartMsg->hAction);
	if(err != ERR_OK)
	{
		HxLOG_Error("\t\tSession expired. Version(0x%x)\n", pstSessionStartMsg->hAction);
		return;
	}

	pstAvContext->hAction = pstSessionStartMsg->hAction;

	svc_av_PrintSessionStartMessage(pstSessionStartMsg);

/* 혹시 session stop 안되어 있을까 확인 ... */
	svc_av_StopSession(ulContextId);

	pstAvContext->bContextRunning = TRUE;

/* Resource ID 정보 복사 */
	pstAvContext->ulDecId = pstSessionStartMsg->ulDecId;
	pstAvContext->ulDemuxId = pstSessionStartMsg->ulDemuxId;

/* Action context에 decoding 정보 복사 */
	// Video
	pstAvContext->usVideoPid = pstSessionStartMsg->usVideoPid;
	pstAvContext->eVideoCodec = pstSessionStartMsg->eVideoCodec;

	// Audio
	pstAvContext->usMainAudioPid = pstSessionStartMsg->usMainAudioPid;
	pstAvContext->eMainAudioCodec = pstSessionStartMsg->eMainAudioCodec,
	pstAvContext->usAuxAudioPid = pstSessionStartMsg->usAuxAudioPid,
	pstAvContext->eAuxAudioCodec = pstSessionStartMsg->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
	pstAvContext->bDolbySelected = pstSessionStartMsg->bDolbySelected,
	pstAvContext->usDolbyPid = pstSessionStartMsg->usDolbyPid,
	pstAvContext->eDolbyCodec = pstSessionStartMsg->eDolbyCodec,
	pstAvContext->eStereoSelect = pstSessionStartMsg->eStereoSelect;
	pstAvContext->eDualMonoSelect = pstSessionStartMsg->eDualMonoSelect;
#endif

	// PCR
	pstAvContext->usPcrPid = pstSessionStartMsg->usPcrPid;

	// etc..
	pstAvContext->bFreezeUnderrun = pstSessionStartMsg->bFreezeUnderrun;
	pstAvContext->bUsePrimer = pstSessionStartMsg->bUsePrimer;

/* Action context 시작 */
	/* Video decoder pipe 연결 */
	PAL_PIPE_SetOutputResource (ulContextId, ePAL_PIPE_OUTPUT_DECORDER, pstAvContext->ulDecId);

	svc_av_StartSession(ulContextId);

	svc_av_PostMsgToAp(eSEVT_AV_SESSION_STARTED, pstAvContext->hAction, ERR_OK, 0, 0);

	HxLOG_Debug("\tLeave : ulContextId(%d)\n", ulContextId);

	return;
}

static void  svc_av_CmdChangePid(AV_SessionStartMsg_t *pstChangePidMsg)
{
	HUINT32 				ulContextId = PAL_PIPE_GetActionId(pstChangePidMsg->hAction);
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];
	HBOOL					bVideoChanged = FALSE, bAudioChanged = FALSE;
	PalDmx_PidInfo_t			stNewPidInfo;
	PalAudio_DecodeParam_t		stCurrentAudioDecodeParam, stNewAudioDecodeParam;
	HERROR					err;

	if(pstAvContext->bContextRunning == TRUE)
	{
		HxLOG_Debug("\t(%d)\n", ulContextId);
	}
	else
	{
		HxLOG_Debug("\t(%d) This context is not active. Exit ...\n", ulContextId);
		return;
	}

	svc_av_PrintSessionStartMessage(pstChangePidMsg);

	/*
		di interface와 정의한 sequence. 추후 platform 변경 후에도 유지되어야 함.

		1. audio/video decoding stop.
		2. pid change.
		3. audio/video decoding start.

		octo 정책.

		1. pcr filter가 변경 된 경우, audio/video 모두 restart한다.
	*/

/****************
Step 1) PCR 비교
*****************/
	if ( (pstChangePidMsg->usPcrPid != PID_CURRENT)
		&& (pstChangePidMsg->usPcrPid != pstAvContext->usPcrPid)
	)
	{ /* PCR이 바뀌면 모두 바뀐 것으로 한다. */
		HxLOG_Debug("\t\tPCR PID Changed : (0x%4X) -> (0x%4X)\n", pstAvContext->usPcrPid, pstChangePidMsg->usPcrPid);
		bVideoChanged = TRUE;
		bAudioChanged = TRUE;

		pstAvContext->usPcrPid = pstChangePidMsg->usPcrPid;
	}

/****************
Step 2) Video 비교
*****************/
	if( (pstChangePidMsg->usVideoPid != PID_CURRENT )
		&& (pstChangePidMsg->usVideoPid != pstAvContext->usVideoPid)
	)
	{
		HxLOG_Debug("\t\tVideo PID Changed : (0x%4X) -> (0x%4X)\n", pstAvContext->usVideoPid, pstChangePidMsg->usVideoPid);
		bVideoChanged = TRUE;
		pstAvContext->usVideoPid = pstChangePidMsg->usVideoPid;
	}

	if( (pstChangePidMsg->eVideoCodec != CODEC_CURRENT)
		&& (pstChangePidMsg->eVideoCodec != pstAvContext->eVideoCodec)
	)
	{
		HxLOG_Debug("\t\tVideo Codec Changed : (0x%4X) -> (0x%4X)\n", pstAvContext->eVideoCodec, pstChangePidMsg->eVideoCodec);
		bVideoChanged = TRUE;
 		pstAvContext->eVideoCodec = pstChangePidMsg->eVideoCodec;
	}

 /****************
 Step 3) Audio 비교
 *****************/
 	HxSTD_MemSet(&stCurrentAudioDecodeParam, 0, sizeof(PalAudio_DecodeParam_t));
	HxSTD_MemSet(&stNewAudioDecodeParam, 0, sizeof(PalAudio_DecodeParam_t));

/* 1. Current audio parameter 백업 */
	svc_av_MakeAudioDecodeParam( pstAvContext->usMainAudioPid,
									pstAvContext->eMainAudioCodec,
									pstAvContext->usAuxAudioPid,
									pstAvContext->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
									pstAvContext->bDolbySelected,
									pstAvContext->usDolbyPid,
									pstAvContext->eDolbyCodec,
#endif
									&stCurrentAudioDecodeParam);

/* 2. 새로운 PID 정보 복사 */
	if ( (pstChangePidMsg->usMainAudioPid != PID_CURRENT)
		&& (pstChangePidMsg->usMainAudioPid != pstAvContext->usMainAudioPid)
	)
	{
		pstAvContext->usMainAudioPid = pstChangePidMsg->usMainAudioPid;
	}

	if ((pstChangePidMsg->eMainAudioCodec != CODEC_CURRENT)
		&& (pstChangePidMsg->eMainAudioCodec != pstAvContext->eMainAudioCodec)
	)
	{
		pstAvContext->eMainAudioCodec = pstChangePidMsg->eMainAudioCodec;
	}

	if ( (pstChangePidMsg->usAuxAudioPid != PID_CURRENT)
		&&(pstChangePidMsg->usAuxAudioPid != pstAvContext->usAuxAudioPid)
	)
	{
		pstAvContext->usAuxAudioPid = pstChangePidMsg->usAuxAudioPid;
	}

	if ((pstChangePidMsg->eAuxAudioCodec != CODEC_CURRENT)
		&& (pstChangePidMsg->eAuxAudioCodec != pstAvContext->eAuxAudioCodec)
	)
	{
		pstAvContext->eAuxAudioCodec = pstChangePidMsg->eAuxAudioCodec;
	}

#if !defined(SvcAv_START_SERVICE_NEW)
	if (pstChangePidMsg->bDolbySelected != pstAvContext->bDolbySelected)
	{
		pstAvContext->bDolbySelected = pstChangePidMsg->bDolbySelected;
	}

	if ( (pstChangePidMsg->usDolbyPid != PID_CURRENT)
		&& (pstChangePidMsg->usDolbyPid != pstAvContext->usDolbyPid)
	)
	{
		pstAvContext->usDolbyPid = pstChangePidMsg->usDolbyPid;
	}

	if (pstChangePidMsg->eDolbyCodec != pstAvContext->eDolbyCodec)
	{
		pstAvContext->eDolbyCodec = pstChangePidMsg->eDolbyCodec;
	}
#endif

/* 3. New audio parameter 만들어 PID 비교 */
	svc_av_MakeAudioDecodeParam(	pstAvContext->usMainAudioPid,
									pstAvContext->eMainAudioCodec,
									pstAvContext->usAuxAudioPid,
									pstAvContext->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
									pstAvContext->bDolbySelected,
									pstAvContext->usDolbyPid,
									pstAvContext->eDolbyCodec,
#endif
									&stNewAudioDecodeParam);

	if( (stNewAudioDecodeParam.usMainPid != stCurrentAudioDecodeParam.usMainPid)
		|| (stNewAudioDecodeParam.eMainCodec != stCurrentAudioDecodeParam.eMainCodec)
		|| (stNewAudioDecodeParam.usSubPid != stCurrentAudioDecodeParam.usSubPid)
		|| (stNewAudioDecodeParam.eSubCodec != stCurrentAudioDecodeParam.eSubCodec)
	)
	{
		HxLOG_Debug("\t\tAudio Parameters Changed\n");
		HxLOG_Debug("\t\t\t=> usMainPid : (0x%4X) -> ((0x%4X))\n", stCurrentAudioDecodeParam.usMainPid, stNewAudioDecodeParam.usMainPid);
		HxLOG_Debug("\t\t\t=> eMainCodec : (%d) -> (%d)\n", stCurrentAudioDecodeParam.eMainCodec, stNewAudioDecodeParam.eMainCodec);
		HxLOG_Debug("\t\t\t=> usSubPid : (0x%4X) -> ((0x%4X))\n", stCurrentAudioDecodeParam.usSubPid, stNewAudioDecodeParam.usSubPid);
		HxLOG_Debug("\t\t\t=> eSubCodec : (%d) -> (%d)\n", stCurrentAudioDecodeParam.eSubCodec, stNewAudioDecodeParam.eSubCodec);
		bAudioChanged = TRUE;
	}

#if !defined(SvcAv_START_SERVICE_NEW)
	if (pstChangePidMsg->eStereoSelect != pstAvContext->eStereoSelect)
	{
		HxLOG_Debug("\t\tStereo Config Changed\n");
		HxLOG_Debug("\t\t\t=> eStereoSelect : (%s) -> (%s)\n",
										OTL_ENUM2STR_StereoSelect(pstAvContext->eStereoSelect),
										OTL_ENUM2STR_StereoSelect(pstChangePidMsg->eStereoSelect));
		pstAvContext->eStereoSelect = pstChangePidMsg->eStereoSelect;
		bAudioChanged = TRUE;
	}
#endif

/*********************
Step 3) 디코딩 다시 시작
**********************/

	stNewPidInfo.usPcrPid = pstAvContext->usPcrPid;
	stNewPidInfo.usVideoPid = pstAvContext->usVideoPid;
	stNewPidInfo.usMainAudioPid = stNewAudioDecodeParam.usMainPid;
	stNewPidInfo.usSubAudioPid = stNewAudioDecodeParam.usSubPid;

	if(bVideoChanged == TRUE)
	{
#if defined(CONFIG_SUPPORT_FCC)
		if (pstAvContext->bUsePrimer == FALSE)
		{
			err = PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
		}
		else
		{
			err = PAL_VIDEO_StopPrimerDecoding(pstAvContext->ulDecId, ulContextId);
			err |= PAL_VIDEO_StopPrimer(pstAvContext->ulDecId, ulContextId);
		}
#else
		err = PAL_VIDEO_StopDecoding(pstAvContext->ulDecId);
#endif

		if(err == ERR_OK)
		{
			PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Decoder, TRUE);
		}

	}

	if(ulContextId == s_ulMainActionId)
	{
		if(bAudioChanged == TRUE)
		{
#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
			if (pstAvContext->bUsePrimer == FALSE)
			{
				err = PAL_AUDIO_StopDecoding();
			}
			else
			{
				err = PAL_AUDIO_StopPrimerDecoding(ulContextId);
				err = PAL_AUDIO_StopPrimer(ulContextId);
			}
#else
			err = PAL_AUDIO_StopDecoding();
#endif
			if(err == ERR_OK)
			{
				PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
			}
		}
	}

#if defined(CONFIG_SUPPORT_FCC)
	if (pstAvContext->bUsePrimer == FALSE)
	{
		err = PAL_DMX_ChangeAVPid(pstAvContext->ulDemuxId, &stNewPidInfo);
	}
	else
	{
		PalDmx_PidInfo_t			 stDmxPidInfo;

		HxSTD_MemCopy(&stDmxPidInfo, &stNewPidInfo, sizeof(PalDmx_PidInfo_t));
		stDmxPidInfo.usMainAudioPid = PID_NULL;
		stDmxPidInfo.usSubAudioPid= PID_NULL;
		err = PAL_DMX_ChangePrimerAVPid(pstAvContext->ulDemuxId, &stDmxPidInfo, ulContextId);
		if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");

#if !defined(FCC_USE_AUDIO_PRIMER)
		if (ulContextId == s_ulMainActionId)
		{
			HxSTD_MemCopy(&stDmxPidInfo, &stNewPidInfo, sizeof(PalDmx_PidInfo_t));
			stDmxPidInfo.usVideoPid = PID_NULL;
			stDmxPidInfo.usPcrPid = PID_NULL;

			err = PAL_DMX_ChangeAVPid(pstAvContext->ulDemuxId, &stDmxPidInfo);
			if(ERR_OK != err)			HxLOG_Error("ERROR Critical Eror!!!\n");
		}
#endif
	}
#else
	err = PAL_DMX_ChangeAVPid(pstAvContext->ulDemuxId, &stNewPidInfo);
#endif
	if(ERR_OK!=err)
	{
		HxLOG_Error("\t\tERROR Critical Eror!!!\n");
	}

	if(bVideoChanged == TRUE)
	{
		if(pstAvContext->usVideoPid < PID_NULL)
		{
			/* PID가 valid하면 still picture에 우선한다. 혹시 있을 still 먼저 stop 하고... */
			PAL_VIDEO_StopStillPicture(pstAvContext->ulDecId);

			/* PID decoding 으로 다시 시작 */
#if defined(CONFIG_SUPPORT_FCC)
			if (pstAvContext->bUsePrimer == FALSE)
			{
				PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
			}
			else
			{
				PAL_VIDEO_StartPrimerDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec, (ulContextId == s_ulMainActionId), ulContextId);
			}
#else
			PAL_VIDEO_StartDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec);
#endif
		}
		else if(pstAvContext->pucStillBuffer != NULL)
		{ /* PID 가 없는데, still picture가 해제되어 있지 않다면 다시 살려야 한다. */
			PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
										pstAvContext->pucStillBuffer,
										pstAvContext->ulStillBufferBytes,
										pstAvContext->eStillCodec,
										FALSE);
		}
	}

	if( (bAudioChanged == TRUE) && (ulContextId == s_ulMainActionId) )
	{
#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
		if (pstAvContext->bUsePrimer == FALSE)
		{
			PAL_AUDIO_StartDecoding(&stNewAudioDecodeParam);
		}
		else
		{
			PAL_AUDIO_StartPrimerDecoding(&stNewAudioDecodeParam, (ulContextId == s_ulMainActionId), ulContextId, TRUE);
		}
#else
		PAL_AUDIO_StartDecoding(&stNewAudioDecodeParam);
#endif
	}

	return;
}

static void svc_av_CmdSetFreeze(AV_FreezeMsg_t *pstFreezeMsg)
{
	AvContext_t			*pstAvContext = &s_stAvContext[pstFreezeMsg->ulActionId];

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, pstFreezeMsg->ulActionId);

	if(pstFreezeMsg->eCommand == eCMD_AV_FREEZE_VIDEO)
	{ /* video freeze 처리 */
		PAL_VIDEO_SetFreeze(pstAvContext->ulDecId, pstFreezeMsg->bFreeze);
		pstAvContext->bVideoFreeze = pstFreezeMsg->bFreeze;
	}

	if(pstFreezeMsg->eCommand == eCMD_AV_FREEZE_AUDIO)
	{ /* audio freeze 처리 */
		pstAvContext->bAudioFreeze = pstFreezeMsg->bFreeze;

		if(pstFreezeMsg->ulActionId == s_ulMainActionId)
		{
			PAL_AUDIO_SetFreeze(pstFreezeMsg->bFreeze);
		}
	}

	return;
}

static void svc_av_CmdSetHide(AV_SetHideMsg_t *pstSetHideMsg)
{
	AvContext_t			*pstAvContext = &s_stAvContext[pstSetHideMsg->ulActionId];

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, pstSetHideMsg->ulActionId);

	if(pstSetHideMsg->eCommand == eCMD_AV_HIDE_VIDEO)
	{
		if(pstSetHideMsg->bHideFlag == TRUE)
		{
			pstAvContext->ulVideoHideMask |= pstSetHideMsg->ulClientMask;
		}
		else
		{
			pstAvContext->ulVideoHideMask &= ~pstSetHideMsg->ulClientMask;
		}

		PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Action, (pstAvContext->ulVideoHideMask != 0));
	}
	else
	{
		if(pstSetHideMsg->bHideFlag == TRUE)
		{
			pstAvContext->ulAudioHideMask |= pstSetHideMsg->ulClientMask;
		}
		else
		{
			pstAvContext->ulAudioHideMask &= ~pstSetHideMsg->ulClientMask;
		}

		if(pstSetHideMsg->ulActionId == s_ulMainActionId)
		{
			PAL_AUDIO_SetHide(eAvHideClient_Action, (pstAvContext->ulAudioHideMask != 0));
		}
	}

	return;
}

static void svc_av_CmdSetEnableVideo(AV_SetEnableMsg_t *pstSetEnableMsg)
{
	AvContext_t			*pstAvContext = &s_stAvContext[pstSetEnableMsg->ulActionId];

	HxLOG_Debug("\t(%d)\n", pstSetEnableMsg->ulActionId);

#if defined(CONFIG_SUPPORT_FCC)
	// fcc live인 경우엔 output control하지 않는다
	if (pstSetEnableMsg->ulActionId == s_ulMainActionId)
#endif
	{
		PAL_VIDEO_SetOutputEnable(pstAvContext->ulDecId, pstSetEnableMsg->eDisplayType, pstSetEnableMsg->bEnableFlag);
	}

	return;
}


static void svc_av_CmdSetMainMedia(AV_CommandMsg_t *pstMainViewMsg)
{
	HUINT32		ulOldMainId, ulNewMainId;
	HUINT32		ulOldDecId, ulNewDecId;
	HUINT32		i;
	HERROR		err;

	HxLOG_Debug("\t(%d)\n", pstMainViewMsg->ulActionId);

	if(pstMainViewMsg->ulActionId != s_ulMainActionId)
	{
		if(NUM_VIEW_ACTION == 1)
		{ /* View가 1이면 그냥 셋팅만 하고 아무것도 안한다. */
			s_ulMainActionId = pstMainViewMsg->ulActionId;
			return;
		}

		ulOldMainId = s_ulMainActionId;
		ulNewMainId = pstMainViewMsg->ulActionId;

		/* Sub context 먼저 stop */
		for(i = 0; i < NUM_VIEW_ACTION; i++)
		{
			if(i != ulOldMainId)
			{
				svc_av_StopSession(i);
			}
		}
		/* 맨 마지막으로 main context stop */
		svc_av_StopSession(ulOldMainId);

		/* Main context id 변경 */
		s_ulMainActionId = ulNewMainId;

		/* Pipe에서 decoder 삭제 */
		if(s_stAvContext[ulOldMainId].bContextRunning == TRUE)
		{
			PAL_PIPE_SetOutputResource(ulOldMainId, ePAL_PIPE_OUTPUT_NONE, 0);
		}

		if(s_stAvContext[ulNewMainId].bContextRunning == TRUE)
		{
			PAL_PIPE_SetOutputResource(ulNewMainId, ePAL_PIPE_OUTPUT_NONE, 0);
		}

		/* Decoder ID 교환 */
		err = PAL_PIPE_GetResourceId(ulOldMainId, eRxRSCTYPE_AV_DEC, &ulOldDecId);
		if(err == ERR_OK)
		{
			PAL_PIPE_ChangeResourceId(ulNewMainId, eRxRSCTYPE_AV_DEC, ulOldDecId);
			s_stAvContext[ulNewMainId].ulDecId = ulOldDecId;
		}
		else
		{
			s_stAvContext[ulNewMainId].ulDecId = -1;
		}

		err = PAL_PIPE_GetResourceId(ulNewMainId, eRxRSCTYPE_AV_DEC, &ulNewDecId);
		if(err == ERR_OK)
		{
			PAL_PIPE_ChangeResourceId(ulOldMainId, eRxRSCTYPE_AV_DEC, ulNewDecId);
			s_stAvContext[ulOldMainId].ulDecId = ulNewDecId;
		}
		else
		{
			s_stAvContext[ulOldMainId].ulDecId = -1;
		}

		/* Main context 먼저 restart */
		svc_av_StartSession(ulNewMainId);
		/* Sub context들 restart */
		for(i = 0; i < NUM_VIEW_ACTION; i++)
		{
			if(i != ulNewMainId)
			{
				svc_av_StartSession(i);
			}
		}
	}
	else
	{
		HxLOG_Debug("\t\tSame main action id... Do nothing...\n");
	}

	return;
}

#if defined(CONFIG_SUPPORT_FCC)
static void svc_av_StartFccSession(HUINT32 ulContextId)
{
	HERROR					err ;
	AvContext_t				*pstAvContext = &s_stAvContext[ulContextId];
	PalAudio_DecodeParam_t		stAudioDecodeParam;
	PalDmx_PidInfo_t			stPidInfo;

	if(pstAvContext->bContextRunning == TRUE)
	{
		HxLOG_Debug("\t(%d, %s)\n", ulContextId, (ulContextId == s_ulMainActionId) ? "MAIN" : "SUB");
	}
	else
	{
		HxLOG_Debug("\t(%d) This context is not active. Exit ...\n", ulContextId);
		return;
	}

	svc_av_MakeAudioDecodeParam(pstAvContext->usMainAudioPid,
									pstAvContext->eMainAudioCodec,
									pstAvContext->usAuxAudioPid,
									pstAvContext->eAuxAudioCodec,
#if !defined(SvcAv_START_SERVICE_NEW)
									pstAvContext->bDolbySelected,
									pstAvContext->usDolbyPid,
									pstAvContext->eDolbyCodec,
#endif
									&stAudioDecodeParam);

	stPidInfo.usPcrPid = pstAvContext->usPcrPid;
	stPidInfo.usVideoPid = pstAvContext->usVideoPid;
	stPidInfo.usMainAudioPid = stAudioDecodeParam.usMainPid;
	stPidInfo.usSubAudioPid = stAudioDecodeParam.usSubPid;
#if 0//defined(CONFIG_DEBUG)
	{
		HxLOG_Print("\n\n[%s:%d] PCR(0x%X) VID(0x%X) M_AUD(0x%X) S_AUD(0x%X) \n\n",__FUNCTION__,__LINE__,
				stPidInfo.usPcrPid,
				stPidInfo.usVideoPid,
				stPidInfo.usMainAudioPid,
				stPidInfo.usSubAudioPid);
	}
#endif

	/*
		di interface와 정의한 sequence. 추후 platform 변경 후에도 유지되어야 함.

		1. pid filter start.
		2. video decoding start.
		3. audio decoding start.
	*/
#if !defined(FCC_USE_AUDIO_PRIMER)
	if (ulContextId == s_ulMainActionId)
	{
		PalDmx_PidInfo_t			 stDmxPidInfo;

		HxSTD_MemCopy(&stDmxPidInfo, &stPidInfo, sizeof(PalDmx_PidInfo_t));
		stDmxPidInfo.usVideoPid = PID_NULL;
		stDmxPidInfo.usPcrPid = PID_NULL;

		err = PAL_DMX_StartAVPid(pstAvContext->ulDemuxId, &stDmxPidInfo, TRUE);
		if(ERR_OK != err)			HxLOG_Error("ERROR Critical Error!!!\n");
	}
#endif

#if 0
	err = PAL_DMX_StartAVPrimerPid(pstAvContext->ulDemuxId, &stPidInfo, (ulContextId == s_ulMainActionId), ulContextId);
	if(ERR_OK != err)			HxLOG_Error("ERROR Critical Error!!!\n");
#endif


/************************************************************************************
Step 3) 디코더를 시작 시킨다.
*************************************************************************************/
	// 여기서 decoder hide 시킨다. decoding이 되서 evt가 날아오면 show로 바뀔것임..
	PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Decoder, TRUE);

	if(pstAvContext->usVideoPid < PID_NULL)
	{
		/* PID decoding 으로 다시 시작 */
		err = PAL_VIDEO_StartPrimerDecoding(pstAvContext->ulDecId, pstAvContext->eVideoCodec, (ulContextId == s_ulMainActionId), ulContextId);
		if (err != ERR_OK)
		{
			HxLOG_Error("PAL_VIDEO_StartPrimerDecoding() failed.. \n");
		}
	}
	else if(pstAvContext->pucStillBuffer != NULL)
	{ /* PID 가 없는데, still picture가 해제되어 있지 않다면 다시 살려야 한다. */
		PAL_VIDEO_StartStillPicture(pstAvContext->ulDecId,
									pstAvContext->pucStillBuffer,
									pstAvContext->ulStillBufferBytes,
									pstAvContext->eStillCodec,
									FALSE);
	}

	/* 현재 main svc가 video underrun 상태일 수도 있으니 fcc 전환시엔 항상 video show 처리 해주자 */
	PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Action, (pstAvContext->ulVideoHideMask != 0));

	if(ulContextId == s_ulMainActionId)
	{
#if defined(FCC_USE_AUDIO_PRIMER)
		PAL_AUDIO_StartPrimerDecoding(&stAudioDecodeParam, (ulContextId == s_ulMainActionId), ulContextId, FALSE);
#else
		PAL_AUDIO_StartDecoding(&stAudioDecodeParam);
#endif
		PAL_AUDIO_SetHide(eAvHideClient_Action, (pstAvContext->ulAudioHideMask != 0));
	}
}

static HERROR svc_av_StopFccSession(HUINT32 ulContextId)
{
	AvContext_t		*pstAvContext = &s_stAvContext[ulContextId];
	HERROR			err = ERR_OK;

	if(pstAvContext->bContextRunning == TRUE)
	{
		//HxLOG_Debug("\t(%d)\n", ulContextId);
	}
	else
	{ 	/* 이미 stop 됨. */
		HxLOG_Debug("\tcontexId(%d) Already inactive context. Exit...\n", ulContextId);
		return ERR_FAIL;
	}

	/*
		di interface와 정의한 sequence. 추후 platform 변경 후에도 유지되어야 함.

		1. audio decoding stop.
		2. video decoding stop.
		3. pid filter stop.
	*/

#if defined(CONFIG_SUPPORT_FCC)
	if(ulContextId == s_ulMainActionId)
#endif
	{
		PAL_SCALER_ClearVideoAttribute(pstAvContext->ulDecId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (SCALER_ClearVideoAttribute) pstAvContext->ulDecId(%d)\n"), pstAvContext->ulDecId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));
	}

/* 1. 순서 지킬 것 : PID audio -> Audio clip -> PID video
주의 : I-frame 및 Audio Clip은 session 과 연동되지 않는다. Still context를 reset하거나 stop 하지 말것. */
	/* PID audio stop */
#if defined(FCC_USE_AUDIO_PRIMER)
	err = PAL_AUDIO_StopPrimerDecoding(ulContextId);
#else
	if(ulContextId == s_ulMainActionId)
	{
		err = PAL_AUDIO_StopDecoding();
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (AUDIO_StopDecoding) ulContextId(%d)\n"), ulContextId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));
	}
#endif

	if(err == ERR_OK)
	{
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (AUDIO_SetHide) ulContextId(%d)\n"), ulContextId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));
	}

	/* PID video stop */
	err = PAL_VIDEO_StopPrimerDecoding(pstAvContext->ulDecId, ulContextId);
	AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
	AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (StopPrimerDecoding) pstAvContext->ulDecId(%d), ulContextId(%d)\n"), pstAvContext->ulDecId, ulContextId);
	AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));
	if(err == ERR_OK)
	{
		PAL_VIDEO_SetHide(pstAvContext->ulDecId, eAvHideClient_Decoder, TRUE);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (VIDEO_SetHide) pstAvContext->ulDecId(%d), ulContextId(%d)\n"), pstAvContext->ulDecId, ulContextId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));
	}

#if !defined(FCC_USE_AUDIO_PRIMER)
	if(ulContextId == s_ulMainActionId)
	{
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (Stop) ulDemuxId(%d)\n"), pstAvContext->ulDemuxId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));

		err = PAL_DMX_StopAllAVPid(pstAvContext->ulDemuxId);
		if(ERR_OK!=err) 	{ HxLOG_Critical("Critical Eror!!!\n"); }
	}
#endif

	return ERR_OK;
}

static void svc_av_CmdSetMainFccMedia(AV_CommandMsg_t *pstMainViewMsg)
{
	HUINT32		ulOldMainId, ulNewMainId;

	HxLOG_Debug("\t(actionId %d) s_ulMainActionId(%d)\n",  pstMainViewMsg->ulActionId, s_ulMainActionId);

	if(pstMainViewMsg->ulActionId != s_ulMainActionId)
	{
		if(NUM_VIEW_ACTION == 1)
		{ /* View가 1이면 그냥 셋팅만 하고 아무것도 안한다. */
			s_ulMainActionId = pstMainViewMsg->ulActionId;
			return;
		}

		ulOldMainId = s_ulMainActionId;
		ulNewMainId = pstMainViewMsg->ulActionId;

		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
		AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (Stop) ulOldMainId(%d), ulNewMainId(%d)\n"), ulOldMainId, ulNewMainId);
		AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));

		// stop old main session
		svc_av_StopFccSession(ulOldMainId);

		/* Main context id 변경 */
		s_ulMainActionId = ulNewMainId;

		// start new main session
		svc_av_StartFccSession(ulNewMainId);
	}
	else
	{
		HxLOG_Debug("\t\tSame main action id... Do nothing...\n");
	}

	return;
}

static void svc_av_CmdSetMainMediaActionId(AV_CommandMsg_t *pstMainViewMsg)
{
	// svc_av에서 관리하는 main context id를 변경한다
	if(pstMainViewMsg->ulActionId != s_ulMainActionId)
	{
		/* Main context id 변경 */
		s_ulMainActionId = pstMainViewMsg->ulActionId;
	}

	// pal_video에서 관리하는 main dec id도 변경한다
	if (PAL_VIDEO_SetMainDecIdForFcc(pstMainViewMsg->ulActionId) != ERR_OK)
	{
		HxLOG_Error("PAL_VIDEO_SetMainDecIdForFcc() failed..!! actionId(%d) \n", pstMainViewMsg->ulActionId);
	}

	return;
}
#endif

STATIC eSvcAv_ChChangeMode_e svc_av_GetDefaultChannelChangeMode(void)
{
	eSvcAv_ChChangeMode_e eMode = eSvcAv_ChChangeMode_NotSupport;
	
#ifdef CONFIG_MW_AV_DEFAULT_CHANNEL_CHANGE_MODE	
	switch(CONFIG_MW_AV_DEFAULT_CHANNEL_CHANGE_MODE)
	{
		case 0:	eMode = eSvcAv_ChChangeMode_Mute;					break;
		case 1:	eMode = eSvcAv_ChChangeMode_HoldUntilTsmLock;		break;
		case 2:	eMode = eSvcAv_ChChangeMode_MuteUntilFirstPicture;	break;
		case 3:	eMode = eSvcAv_ChChangeMode_HoldUntilFirstPicture;	break;
		default: break;
	}
#endif

	return eMode;
}

STATIC void svc_av_SetChannelChangeMode(eSvcAv_ChChangeMode_e eMode)
{
	HINT32							i = 0;
	HUINT32 						ulNumOfDecoder = 0;
	PalVideo_ChannelChangeMode_e	ePalCcm = eChannelChangeMode_Mute;

	if(s_eChannelChangeMode == eMode)
	{
		// same mode
		return;
	}

	switch(eMode)
	{
		case eSvcAv_ChChangeMode_Mute: 
			ePalCcm = eChannelChangeMode_Mute;
			break;
			
		case eSvcAv_ChChangeMode_HoldUntilTsmLock: 
			ePalCcm = eChannelChangeMode_HoldUntilTsmLock;
			break;
			
		case eSvcAv_ChChangeMode_MuteUntilFirstPicture:
			ePalCcm = eChannelChangeMode_MuteUntilFirstPicture;
			break;
			
		case eSvcAv_ChChangeMode_HoldUntilFirstPicture:
			ePalCcm = eChannelChangeMode_HoldUntilFirstPicture;
			break;
			
		case eSvcAv_ChChangeMode_NotSupport:
		default:
			HxLOG_Warning("Unknown ccm mode( %d )\n", eMode);
			return;			
	}

	ulNumOfDecoder = PAL_VIDEO_GetSupportNumberOfDecoder();
	for(i = 0; i < ulNumOfDecoder; i++)
	{
		(void)PAL_VIDEO_SetChannelChangeMode(i, ePalCcm);
	}
	s_eChannelChangeMode = eMode;
}


static HCHAR* svc_av_GetEnumStr_AV_Command_t(AV_Command_t eAvCmd)
{
	switch(eAvCmd)
	{
#ifdef CONFIG_DEBUG
	ENUM_TO_STR(eCMD_AV_START_SESSION);					//			/* Audio); // Video Decoding 시작 */
	ENUM_TO_STR(eCMD_AV_STOP_SESSION);					 //				/* Audio); // Video Decoding 중지 */
	ENUM_TO_STR(eCMD_AV_CHANGE_PID);					 //					/* Audio); // Video Pid & Codec 변경 */
	ENUM_TO_STR(eCMD_AV_FREEZE_VIDEO);					 // 			/* Live에서 Video Decoding 일시 정지 실행. */
	ENUM_TO_STR(eCMD_AV_FREEZE_AUDIO);					 // 			/* Live에서 Audio Decoding 일시 정지 실행. */
	ENUM_TO_STR(eCMD_AV_AUDIO_EVENT);					 //				/* Driver Callback 발생 시 필요한 경우 MW에 알려준다. */
	ENUM_TO_STR(eCMD_AV_VIDEO_EVENT);					 //				/* Driver Callback 발생 시 필요한 경우 MW에 알려준다. */
	ENUM_TO_STR(eCMD_AV_CAPTURE_STILL);					 //				/* I Frame (ES) Decoding 처리. */
	ENUM_TO_STR(eCMD_AV_DECODE_STILL);	 				//				/* I Frame (ES) Decoding 처리. */
	ENUM_TO_STR(eCMD_AV_PLAY_AUDIOCLIP);				 //
	ENUM_TO_STR(eCMD_AV_STOP_AUDIOCLIP);				 //
	ENUM_TO_STR(eCMD_AV_PAUSE_AUDIOCLIP);				 //
	ENUM_TO_STR(eCMD_AV_RESUME_AUDIOCLIP);				 //
	ENUM_TO_STR(eCMD_AV_CONFIG_STEREO);					 //
	ENUM_TO_STR(eCMD_AV_CONFIG_DUALMONO);				 //
	ENUM_TO_STR(eCMD_AV_HIDE_VIDEO);					 //
	ENUM_TO_STR(eCMD_AV_HIDE_AUDIO);					 //
	ENUM_TO_STR(eCMD_AV_SET_MAINMEDIA);					 //				/* PIP */
	ENUM_TO_STR(eCMD_AV_SET_MAINFCCMEDIA);				 //			/* Fast ch change */
	ENUM_TO_STR(eCMD_AV_SET_MAINMEDIA_ACTIONID);		 //		/* set main media actionId for Fast ch change */
	ENUM_TO_STR(eCMD_AV_PING);								 //
	ENUM_TO_STR(eCMD_AV_ENABLE_VIDEO);						 //
	ENUM_TO_STR(eCMD_AV_MAX);
#endif
	default:
		return "??";
	} ;
}

static void svc_av_HandleMsg(AV_Message_t	 *pstMsg)
{
	s_pcLastCommand = (HINT8*)svc_av_Cmd2Str(pstMsg->eCommand);

	HxLOG_Print(HxANSI_COLOR_CYAN("[%s (0x%x)]\n"), svc_av_GetEnumStr_AV_Command_t(pstMsg->eCommand), pstMsg->stDebugMsg.ulParam1);

	switch (pstMsg->eCommand)
	{
		case eCMD_AV_PING:
			HxLOG_Debug("\n\n\t\t=============== SVC AV alive =================\n");
			break;

		case eCMD_AV_START_SESSION: /* Async API */
		{
			svc_av_CmdStartSession(&pstMsg->stSessionStartMsg);

			break;
		}

		case eCMD_AV_STOP_SESSION: /* SYNC or ASYNC API */
		{
			svc_av_CmdStopSession(&pstMsg->stSessionStopMsg);

			if(pstMsg->stSessionStopMsg.syncMode == eSyncMode)
			{
				VK_SEM_Release(s_ulAvSyncSema);
			}

			break;
		}

		case eCMD_AV_CHANGE_PID: /* Sync API */
		{
			svc_av_CmdChangePid(&pstMsg->stChangePidMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_FREEZE_VIDEO: /* Sync API */
		case eCMD_AV_FREEZE_AUDIO: /* Sync API */
		{
			svc_av_CmdSetFreeze(&pstMsg->stFreezeMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_VIDEO_EVENT:
		{
			svc_av_CmdVideoEvent(&pstMsg->stDeviceEvtMsg);

			break;
		}

		case eCMD_AV_AUDIO_EVENT:
		{
			svc_av_CmdAudioEvent(&pstMsg->stDeviceEvtMsg);

			break;
		}

		case eCMD_AV_CAPTURE_STILL: /* Async API */
		{
			svc_av_CmdCaptureStill(&pstMsg->stCaptureStillMsg);

			break;
		}

		case eCMD_AV_DECODE_STILL: /* Async API */
		{
			svc_av_CmdDecodeStill(&pstMsg->stDecodeStillMsg);

			break;
		}

		case eCMD_AV_PLAY_AUDIOCLIP: /* Async API */
		case eCMD_AV_STOP_AUDIOCLIP: /* Async API */
		case eCMD_AV_PAUSE_AUDIOCLIP: /* Async API */
		case eCMD_AV_RESUME_AUDIOCLIP: /* Async API */
		{
			svc_av_CmdAudioClip(&pstMsg->stAudioClipMsg);

			break;
		}

		case eCMD_AV_CONFIG_STEREO:
		{
			svc_av_CmdStereoCfg(&pstMsg->stStereoCfgMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_CONFIG_DUALMONO:
		{
			svc_av_CmdDualMonoCfg(&pstMsg->stDualMonoCfgMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_SET_MAINMEDIA:
		{
			svc_av_CmdSetMainMedia(&pstMsg->stMainMediaMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

#if defined(CONFIG_SUPPORT_FCC)
		case eCMD_AV_SET_MAINFCCMEDIA:
		{
			svc_av_CmdSetMainFccMedia(&pstMsg->stMainMediaMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_SET_MAINMEDIA_ACTIONID:
		{
			svc_av_CmdSetMainMediaActionId(&pstMsg->stMainMediaMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}
#endif

		case eCMD_AV_HIDE_VIDEO:
		case eCMD_AV_HIDE_AUDIO:
		{
			svc_av_CmdSetHide(&pstMsg->stSetHideMsg);
			VK_SEM_Release(s_ulAvSyncSema);

			break;
		}

		case eCMD_AV_ENABLE_VIDEO:
		{
			svc_av_CmdSetEnableVideo(&pstMsg->stSetEnableMsg);
			VK_SEM_Release(s_ulAvSyncSema);
			break;
		}
		default:
		{
			HxLOG_Error("unknown eCommand \n");
		}
		break;
	}
}

static void svc_av_Task(void *pParam)
{
	HUINT32 			err;
	AV_Message_t 		stAvMsg;

	pParam = NULL;

	while (1)
	{
		err = VK_MSG_Receive(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
		if(err == ERR_OK)
		{
			if(stAvMsg.eCommand < eCMD_AV_MAX) /* 메시지를 받음 */
			{
				svc_av_HandleMsg(&stAvMsg);
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
		}
	}

	/* this task will not be here any more */
}

void svc_av_ResetContext(HUINT32 ulContextId)
{
	AvContext_t			*pstAvContext = &s_stAvContext[ulContextId];

	pstAvContext->bContextRunning = FALSE;
	pstAvContext->hAction = HANDLE_NULL;

#if 0	/* WA-7000에서 DLNA재생시 Live AVBlock이 해제 되지 않는 현상으로 임시 수정 */
	pstAvContext->ulDecId = -1;
#endif
	pstAvContext->ulDemuxId = -1;

	pstAvContext->usVideoPid = PID_NULL;
	pstAvContext->eVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
	pstAvContext->bVideoFreeze = FALSE;

	pstAvContext->usMainAudioPid = PID_NULL;
	pstAvContext->usAuxAudioPid = PID_NULL;
#if !defined(SvcAv_START_SERVICE_NEW)
	pstAvContext->usDolbyPid = PID_NULL;
	pstAvContext->bDolbySelected = FALSE;
#endif
	pstAvContext->eStereoSelect = eDxSTEREO_SELECT_UNKNOWN;
	pstAvContext->eDualMonoSelect = eDxDUALMONO_SELECT_UNKNOWN;
	pstAvContext->bAudioFreeze = FALSE;

	pstAvContext->usPcrPid = PID_NULL;

#if defined(CONFIG_SUPPORT_FCC)
	pstAvContext->bUsePrimer = FALSE;
#endif

}

#if defined(CONFIG_DEBUG)
void svc_av_PrintStatus(void)
{
	HUINT32 i = 0;

	HLIB_CMD_Printf("============= Main Context [%d] ============\n", s_ulMainActionId);

#if defined(CONFIG_SUPPORT_FCC)
	for(i = 0; i < NUM_VIEW_ACTION; i++)
#else
	for(i = 0; i < AV_MAX_CONTEXT; i++)
#endif
	{
		HLIB_CMD_Printf("============= s_stAvContext[%d] Info. ============\n", i);

		HLIB_CMD_Printf("\tActionhandle : 0x%x\n",s_stAvContext[i].hAction);
		HLIB_CMD_Printf("\tContext Running : %d\n", s_stAvContext[i].bContextRunning);
		HLIB_CMD_Printf("\tAV Dec ID : %d\n", s_stAvContext[i].ulDecId);
		HLIB_CMD_Printf("\tDemux ID : %d\n", s_stAvContext[i].ulDemuxId);

		HLIB_CMD_Printf("\tVideo Pid : 0x%x\n", s_stAvContext[i].usVideoPid);
		HLIB_CMD_Printf("\tVideo Codec : %s\n", OTL_ENUM2STR_VideoCodec(s_stAvContext[i].eVideoCodec));
		HLIB_CMD_Printf("\tVideo Freeze : %d\n", s_stAvContext[i].bVideoFreeze);
		HLIB_CMD_Printf("\tVideo Hide Mask : 0x%x\n", s_stAvContext[i].ulVideoHideMask);

		HLIB_CMD_Printf("\tStill buffer : 0x%x\n", s_stAvContext[i].pucStillBuffer);
		HLIB_CMD_Printf("\tStill buffer byte : 0x%x\n", s_stAvContext[i].ulStillBufferBytes);
		HLIB_CMD_Printf("\tStill codec : %s\n", OTL_ENUM2STR_VideoCodec(s_stAvContext[i].eStillCodec));

		HLIB_CMD_Printf("\tMain Audio Pid : 0x%x\n", s_stAvContext[i].usMainAudioPid);
		HLIB_CMD_Printf("\tSub Audio Pid : 0x%x\n", s_stAvContext[i].usAuxAudioPid);
#if !defined(SvcAv_START_SERVICE_NEW)
		HLIB_CMD_Printf("\tDolby Audio Pid : 0x%x\n", s_stAvContext[i].usDolbyPid);
#endif
		HLIB_CMD_Printf("\tAudio Codec : %s\n", OTL_ENUM2STR_AudioCodec(s_stAvContext[i].eMainAudioCodec));
		HLIB_CMD_Printf("\tAux Codec : %s\n", OTL_ENUM2STR_AudioCodec(s_stAvContext[i].eAuxAudioCodec));
#if !defined(SvcAv_START_SERVICE_NEW)
		HLIB_CMD_Printf("\tDolby Codec : %s\n", OTL_ENUM2STR_AudioCodec(s_stAvContext[i].eDolbyCodec));
		HLIB_CMD_Printf("\tDolby Flag : %d\n", s_stAvContext[i].bDolbySelected);
#endif
		HLIB_CMD_Printf("\tStereo Config : %s\n", OTL_ENUM2STR_StereoSelect(s_stAvContext[i].eStereoSelect));
		HLIB_CMD_Printf("\tDualMono Config : %s\n", OTL_ENUM2STR_DualMonoSelect(s_stAvContext[i].eDualMonoSelect));
		HLIB_CMD_Printf("\tAudio Freeze : %d\n", s_stAvContext[i].bAudioFreeze);
		HLIB_CMD_Printf("\tAudio Hide Mask : 0x%x\n", s_stAvContext[i].ulAudioHideMask);

		HLIB_CMD_Printf("\tPCR Pid : 0x%x\n", s_stAvContext[i].usPcrPid);

		HLIB_CMD_Printf("============= End of s_stAvContext Info. ============\n");
	}

}

static int svc_av_PingTask(void)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("%s(last command : %s)\n", __FUNCTION__, s_pcLastCommand);

	stAvMsg.eCommand = eCMD_AV_PING;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static int svc_av_ProcessCommand_status(void *szArgList)
{
	svc_av_PrintStatus();

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_hide(void *szArgList)
{
	char			 *aucArg=NULL;
	HBOOL bAudio = 0, bVideo = 0;

	/* 2nd argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &bAudio) == 0 )
			return HxCMD_ERR;
	}

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &bVideo) == 0 )
			return HxCMD_ERR;
	}

	HxLOG_Print(" hide audio : (%d) video : (%d) \n", bAudio, bVideo);

	SVC_AV_SetVideoHide(0, 1, bAudio);
	SVC_AV_SetVideoHide(0, 1, bVideo);

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_freeze(void *szArgList)
{
	SVC_AV_SetVideoFreeze(0, TRUE);
	SVC_AV_SetVideoFreeze(0, TRUE);

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_unfreeze(void *szArgList)
{
	SVC_AV_SetVideoFreeze(0, FALSE);
	SVC_AV_SetVideoFreeze(0, FALSE);

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_nullpid(void *szArgList)
{
	HERROR			hErr;

#if defined(SvcAv_START_SERVICE_NEW)
	hErr = SVC_AV_ChangePid(0,
						PID_NULL,
						PID_NULL,
						PID_NULL,
						PID_NULL,
						s_stAvContext[0].eMainAudioCodec,
						s_stAvContext[0].eAuxAudioCodec,
						s_stAvContext[0].eVideoCodec
						);
#else
	hErr = SVC_AV_ChangePid(0,
						PID_NULL,
						PID_NULL,
						s_stAvContext[0].usDolbyPid,
						PID_NULL,
						PID_NULL,
						s_stAvContext[0].eMainAudioCodec,
						s_stAvContext[0].eAuxAudioCodec,
						s_stAvContext[0].eDolbyCodec,
						s_stAvContext[0].eVideoCodec,
						FALSE,
						s_stAvContext[0].eStereoSelect
						);
#endif
	if(hErr)
	{
		HxLOG_Error("SVC_AV_ChangePid ret :0x%X\n",hErr);
	}

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_ping(void *szArgList)
{
	svc_av_PingTask();

	return HxCMD_OK;
}

static int svc_av_ProcessCommand_enable_video(void *szArgList)
{
	char			 *aucArg=NULL;
	HUINT32 ulDisplayType = 0;
	HBOOL bVideoOutput = FALSE;

	/* 2nd argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulDisplayType) == 0 )
			return HxCMD_ERR;
	}

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &bVideoOutput) == 0 )
			return HxCMD_ERR;
	}

	HLIB_CMD_Printf(" ulDisplayType : (%d) bVideoOutput : (%d) \n", ulDisplayType, bVideoOutput);

	SVC_AV_SetVideoOutputEnable(0, ulDisplayType, bVideoOutput);

	return HxCMD_OK;
}

static void svc_av_RegisterCommand(void)
{
#define		hCmdHandle		"svc_av"

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_status,
						"status",
						"Print svc_av status.",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_hide,
						"hide",
						"Print svc_av status.",
						"hide [0|1|2] [0|1|2] - Hide [audio] [video] by action. 0:current, 1:block, 2:unblock");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_freeze,
						"freeze",
						"Freeze AV.",
						"freeze");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_unfreeze,
						"unfreeze",
						"unfreeze AV.",
						"unfreeze");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_nullpid,
						"nullpid",
						"Set audio and video pid to NULL PID(0x2000).",
						"nullpid");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_ping,
						"ping",
						"Check if SVC_AV module is alive.",
						"ping");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_av_ProcessCommand_enable_video,
						"enable_video",
						"Enable/Disable video output by action 0.",
						"enable_video [0(hd)|1(sd)] [0(disable)|1(enable)]");
}
#endif

#if defined(CONFIG_SUPPORT_FCC)
STATIC HUINT32 svc_av_GetAdjustActionId(HUINT32 ulActionId)
{
	HUINT32 i;
	HBOOL	bFccActivated = FALSE;
	HUINT32 ulAdjustActionId = ulActionId;

	if (ulActionId == s_ulMainActionId)
	{
		for (i = 0; i < NUM_VIEW_ACTION; i++)
		{
			if ((i != ulActionId)
				&& (s_stAvContext[i].bContextRunning == TRUE))
			{
				// main이 아닌데 돌고 있는 context가 있다.. fcc 동작중..!
				bFccActivated = TRUE;
				break;
			}
		}

		if (bFccActivated == FALSE)
		{
			// fcc가 동작중이 아니다.
			// 그런데 di -> pal로 올라오는 decId는 항상 0이므로 0번 context를 사용해야 한다.
			ulAdjustActionId = 0;
		}
	}

	return ulAdjustActionId;
}
#endif


#define _____PUBLIC_APIs________________________________________________________________

HERROR SVC_AV_Init(PostMsgToMgrCb_t pfncPostMsgCallback)
{
	HINT32 		nErr = 0;
	HUINT32		i;

	if(s_bAVInt == TRUE)
	{
		HxLOG_Critical("\n\n");
		return ERR_OK;
	}

#ifdef CONFIG_DEBUG
#ifdef CONFIG_SUPPORT_FCC
	s_bFccEnableByCommand = TRUE;
#else
	s_bFccEnableByCommand = FALSE;
#endif
#endif

	if (pfncPostMsgCallback == NULL)
	{
		HxLOG_Error("[%s:%s] invalid msg callback function !! \n", __FUNCTION__, __LINE__);
	}
	s_fnSvcAv_PostMsgToAp = pfncPostMsgCallback;

#ifdef CONFIG_MW_AV_DEFAULT_CHANNEL_CHANGE_MODE
		svc_av_SetChannelChangeMode(svc_av_GetDefaultChannelChangeMode());
#endif

	/* Context Initialize */
	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		svc_av_ResetContext(i);

		/* Still picture 정보는 reset context 와 별개로 한다.
			즉, session 과 연동되지 않고 별도로 start/stop 된다.
			reset context 에 넣지 말것. */
		s_stAvContext[i].pucStillBuffer = NULL;
		s_stAvContext[i].ulStillBufferBytes = 0;
		s_stAvContext[i].eStillCodec = eDxVIDEO_CODEC_UNKNOWN;

		PAL_VIDEO_SetHide(i, eAvHideClient_Decoder, TRUE);
	}

	PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);

	/* Sync API용 semaphore 생성. */
	nErr = VK_SEM_Create(&s_ulAvSyncSema, "AvSyncSem", VK_SUSPENDTYPE_PRIORITY);
	if (nErr != VK_OK)
	{
		HxLOG_Error("Creation Error: s_ulAvSyncSema sema \n");
		return ERR_FAIL;
	}
	else
	{
		VK_SEM_Get(s_ulAvSyncSema); /* 세마포를 get 해 놓아야 sync API가 동작한다. */
	}

	nErr = VK_MSG_Create(MW_AV_MSGQ_SIZE, sizeof(AV_Message_t), "AvMsgQ", &s_ulAVCmdMsgQId, VK_SUSPENDTYPE_FIFO);
	if (nErr != VK_OK)
	{
		HxLOG_Error("Creation Error: AvMessageQ \n");
	}

	nErr = VK_TASK_Create (svc_av_Task,
							MW_AV_TASK_PRIORITY,
							MW_AV_TASK_STACK_SIZE,
							"tAVx",
							NULL,
							&s_ulAVTaskId, 0);
	if (nErr != VK_OK)
	{
		HxLOG_Error("Creation Error: MW AV Command Task\n");
		return ERR_FAIL;
	}

	VK_TASK_Start(s_ulAVTaskId);

	/* Driver Interrupt 성 Event 를 전달 받기 위한 Callback 등록 */
	PAL_VIDEO_RegisterEventCallback(svc_av_VideoEventCallback);
	PAL_AUDIO_RegisterEventCallback(svc_av_AudioEventCallback);

#if defined(CONFIG_DEBUG)
	svc_av_RegisterCommand();
#endif

	return ERR_OK;
}

HUINT32 SVC_AV_GetNumberOfVideoDecoder(void)
{
	return PAL_VIDEO_GetSupportNumberOfDecoder();
}

HERROR SVC_AV_SetMainMedia(HUINT32 ulActionId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stMainMediaMsg.eCommand = eCMD_AV_SET_MAINMEDIA;
	stAvMsg.stMainMediaMsg.ulActionId = ulActionId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

HERROR SVC_AV_SetMainFccMedia(HUINT32 ulActionId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);

	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stMainMediaMsg.eCommand = eCMD_AV_SET_MAINFCCMEDIA;
	stAvMsg.stMainMediaMsg.ulActionId = ulActionId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

// fcc 사용하는 경우에 한해서 매우 제한적으로 사용해야 함 !!
HERROR SVC_AV_SetMainMediaActionId(HUINT32 ulActionId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulActionId);

	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stMainMediaMsg.eCommand = eCMD_AV_SET_MAINMEDIA_ACTIONID;
	stAvMsg.stMainMediaMsg.ulActionId = ulActionId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

#if defined(WORKTHROUGH_AV_START_AFTER_PIPE_CONNECTED)
STATIC void svc_av_sendStartServiceMsgToAvThread (HUINT32 ulActionId, void *pvUserData, HUINT32 ulDataSize)
{
	AV_Message_t	*pstMsg = (AV_Message_t *)pvUserData;
	HERROR			 hErr;

	hErr = VK_MSG_Send(s_ulAVCmdMsgQId, pstMsg, sizeof(AV_Message_t));
	if (VK_OK != hErr)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
	}
}
#endif

#if defined(SvcAv_START_SERVICE_NEW)
//Async API
HERROR SVC_AV_StartService(
						Handle_t				hAction,
						HUINT32					ulDecId,
						HUINT32					ulDemuxId,
						HUINT16					usAudioPid,
						HUINT16					usAuxPid,
						HUINT16					usVideoPid,
						HUINT16					usPcrPid,
						DxAudioCodec_e			eAudioCodec,
						DxAudioCodec_e			eAuxCodec,
						DxVideoCodec_e			eVideoCodec,
						HBOOL					bFreezeUnderrun,
						HBOOL					bUsePrimer
						)
{
	AV_Message_t	stAvMsg;
	HUINT32			ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

#ifdef CONFIG_DEBUG
	if(FALSE == s_bFccEnableByCommand)
	{
		bUsePrimer = FALSE;
	}
#endif

/*
	1. MPEG Audio Only
	2. Dolby Audio Only
	3. Audio Description Only
	4. Dolby Audio + Audio Description
	5. Mpeg Audio + Audio Description
*/

/*
	1. Dolby의 선택은 Dolby Flag로
	2. AD의 판단은 Aux Pid로
	3. Audio Pid는 Mpeg Only, 만약 돌비 디코더가 지원이 되지 않는 경우 Audio Pid를 이용해서 Analog Out한다.
*/
	/* 입력 파라미터들 체크 */
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");

		return ERR_FAIL;
	}

	stAvMsg.stSessionStartMsg.eCommand = eCMD_AV_START_SESSION;
	stAvMsg.stSessionStartMsg.hAction = hAction;
	stAvMsg.stSessionStartMsg.ulDecId = ulDecId;
	stAvMsg.stSessionStartMsg.ulDemuxId = ulDemuxId;
	stAvMsg.stSessionStartMsg.usVideoPid = usVideoPid;
	stAvMsg.stSessionStartMsg.usMainAudioPid = usAudioPid;
	stAvMsg.stSessionStartMsg.usAuxAudioPid = usAuxPid;
//	stAvMsg.stSessionStartMsg.usDolbyPid = usDolbyPid;
	stAvMsg.stSessionStartMsg.usPcrPid = usPcrPid;
	stAvMsg.stSessionStartMsg.eMainAudioCodec = eAudioCodec;
	stAvMsg.stSessionStartMsg.eAuxAudioCodec = eAuxCodec;
//	stAvMsg.stSessionStartMsg.eDolbyCodec = eDolbyCodec;
	stAvMsg.stSessionStartMsg.eVideoCodec = eVideoCodec;
//	stAvMsg.stSessionStartMsg.bDolbySelected = bDolbySelected;
//	stAvMsg.stSessionStartMsg.eStereoSelect = eStereoSelect;
//	stAvMsg.stSessionStartMsg.eDualMonoSelect = eDualMonoSelect;
	stAvMsg.stSessionStartMsg.bFreezeUnderrun = bFreezeUnderrun;
	stAvMsg.stSessionStartMsg.bUsePrimer = bUsePrimer;

#if defined(WORKTHROUGH_AV_START_AFTER_PIPE_CONNECTED)
	err = PAL_PIPE_NotifyPipeConnected(ulActionId, svc_av_sendStartServiceMsgToAvThread, (void *)&stAvMsg, sizeof(AV_Message_t));
	if (ERR_OK != err)
	{
		HxLOG_Error("PAL_PIPE_NotifyPipeConnected err: ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#endif
}
#else
//Async API
HERROR SVC_AV_StartService(
						Handle_t				hAction,
						HUINT32					ulDecId,
						HUINT32					ulDemuxId,
						HUINT16					usAudioPid,
						HUINT16					usAuxPid,
						HUINT16					usDolbyPid,
						HUINT16					usVideoPid,
						HUINT16					usPcrPid,
						DxAudioCodec_e			eAudioCodec,
						DxAudioCodec_e			eAuxCodec,
						DxAudioCodec_e			eDolbyCodec,
						DxVideoCodec_e			eVideoCodec,
						HBOOL					bDolbySelected,
						DxStereoSelect_e		eStereoSelect,
						DxDualMonoSelect_e		eDualMonoSelect,
						HBOOL					bFreezeUnderrun,
						HBOOL					bUsePrimer
						)
{
	AV_Message_t	stAvMsg;
	HUINT32			ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

/*
	1. MPEG Audio Only
	2. Dolby Audio Only
	3. Audio Description Only
	4. Dolby Audio + Audio Description
	5. Mpeg Audio + Audio Description
*/

/*
	1. Dolby의 선택은 Dolby Flag로
	2. AD의 판단은 Aux Pid로
	3. Audio Pid는 Mpeg Only, 만약 돌비 디코더가 지원이 되지 않는 경우 Audio Pid를 이용해서 Analog Out한다.
*/
	/* 입력 파라미터들 체크 */
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");

		return ERR_FAIL;
	}

	stAvMsg.stSessionStartMsg.eCommand = eCMD_AV_START_SESSION;
	stAvMsg.stSessionStartMsg.hAction = hAction;
	stAvMsg.stSessionStartMsg.ulDecId = ulDecId;
	stAvMsg.stSessionStartMsg.ulDemuxId = ulDemuxId;
	stAvMsg.stSessionStartMsg.usVideoPid = usVideoPid;
	stAvMsg.stSessionStartMsg.usMainAudioPid = usAudioPid;
	stAvMsg.stSessionStartMsg.usAuxAudioPid = usAuxPid;
	stAvMsg.stSessionStartMsg.usDolbyPid = usDolbyPid;
	stAvMsg.stSessionStartMsg.usPcrPid = usPcrPid;
	stAvMsg.stSessionStartMsg.eMainAudioCodec = eAudioCodec;
	stAvMsg.stSessionStartMsg.eAuxAudioCodec = eAuxCodec;
	stAvMsg.stSessionStartMsg.eDolbyCodec = eDolbyCodec;
	stAvMsg.stSessionStartMsg.eVideoCodec = eVideoCodec;
	stAvMsg.stSessionStartMsg.bDolbySelected = bDolbySelected;
	stAvMsg.stSessionStartMsg.eStereoSelect = eStereoSelect;
	stAvMsg.stSessionStartMsg.eDualMonoSelect = eDualMonoSelect;
	stAvMsg.stSessionStartMsg.bFreezeUnderrun = bFreezeUnderrun;
	stAvMsg.stSessionStartMsg.bUsePrimer = bUsePrimer;

#if defined(WORKTHROUGH_AV_START_AFTER_PIPE_CONNECTED)
	err = PAL_PIPE_NotifyPipeConnected(ulActionId, svc_av_sendStartServiceMsgToAvThread, (void *)&stAvMsg, sizeof(AV_Message_t));
	if (ERR_OK != err)
	{
		HxLOG_Error("PAL_PIPE_NotifyPipeConnected err: ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
#endif
}
#endif

//Async or Sync API....depend on sync mode
HERROR SVC_AV_StopService(HUINT32 ulActionId, ApiSyncMode_t syncMode)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n",  ulActionId);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stSessionStopMsg.eCommand = eCMD_AV_STOP_SESSION;
	stAvMsg.stSessionStopMsg.ulActionId = ulActionId;
	stAvMsg.stSessionStopMsg.syncMode = syncMode;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulAvSyncSema);
	}

	return ERR_OK;
}

#if defined(SvcAv_START_SERVICE_NEW)
// Async API
HERROR SVC_AV_ChangePid(
						HUINT32					ulActionId,
						HUINT16					usAudioPid,
						HUINT16					usAuxPid,
						HUINT16					usVideoPid,
						HUINT16					usPcrPid,
						DxAudioCodec_e			eAudioCodec,
						DxAudioCodec_e			eAuxCodec,
						DxVideoCodec_e			eVideoCodec
						)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

/*
	1. MPEG Audio Only
	2. Dolby Audio Only
	3. Audio Description Only
	4. Dolby Audio + Audio Description
	5. Mpeg Audio + Audio Description
*/

/*
	1. Dolby의 선택은 Dolby Flag로
	2. AD의 판단은 Aux Pid로
	3. Audio Pid는 Mpeg Only, 만약 돌비 디코더가 지원이 되지 않는 경우 Audio Pid를 이용해서 Analog Out한다.
*/

	stAvMsg.stChangePidMsg.eCommand = eCMD_AV_CHANGE_PID;
	stAvMsg.stChangePidMsg.hAction = PAL_PIPE_GetActionHandle(ulActionId);
	stAvMsg.stChangePidMsg.usVideoPid = usVideoPid;
	stAvMsg.stChangePidMsg.usMainAudioPid = usAudioPid;
	stAvMsg.stChangePidMsg.usAuxAudioPid = usAuxPid;
//	stAvMsg.stChangePidMsg.usDolbyPid = usDolbyPid;
	stAvMsg.stChangePidMsg.usPcrPid = usPcrPid;
	stAvMsg.stChangePidMsg.eMainAudioCodec = eAudioCodec;
	stAvMsg.stChangePidMsg.eAuxAudioCodec = eAuxCodec;
//	stAvMsg.stChangePidMsg.eDolbyCodec = eDolbyCodec; 				// 2009.03.23 추가.
	stAvMsg.stChangePidMsg.eVideoCodec = eVideoCodec;
//	stAvMsg.stChangePidMsg.bDolbySelected = bDolbySelected;
//	stAvMsg.stChangePidMsg.eStereoSelect = eStereoSelect;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("Message Send Failure. \n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}
#else
// Async API
HERROR SVC_AV_ChangePid(
						HUINT32					ulActionId,
						HUINT16					usAudioPid,
						HUINT16					usAuxPid,
						HUINT16					usDolbyPid,
						HUINT16					usVideoPid,
						HUINT16					usPcrPid,
						DxAudioCodec_e			eAudioCodec,
						DxAudioCodec_e			eAuxCodec,
						DxAudioCodec_e			eDolbyCodec,
						DxVideoCodec_e			eVideoCodec,
						HBOOL					bDolbySelected,
						DxStereoSelect_e	eStereoSelect
						)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

/*
	1. MPEG Audio Only
	2. Dolby Audio Only
	3. Audio Description Only
	4. Dolby Audio + Audio Description
	5. Mpeg Audio + Audio Description
*/

/*
	1. Dolby의 선택은 Dolby Flag로
	2. AD의 판단은 Aux Pid로
	3. Audio Pid는 Mpeg Only, 만약 돌비 디코더가 지원이 되지 않는 경우 Audio Pid를 이용해서 Analog Out한다.
*/

	stAvMsg.stChangePidMsg.eCommand = eCMD_AV_CHANGE_PID;
	stAvMsg.stChangePidMsg.hAction = PAL_PIPE_GetActionHandle(ulActionId);
	stAvMsg.stChangePidMsg.usVideoPid = usVideoPid;
	stAvMsg.stChangePidMsg.usMainAudioPid = usAudioPid;
	stAvMsg.stChangePidMsg.usAuxAudioPid = usAuxPid;
	stAvMsg.stChangePidMsg.usDolbyPid = usDolbyPid;
	stAvMsg.stChangePidMsg.usPcrPid = usPcrPid;
	stAvMsg.stChangePidMsg.eMainAudioCodec = eAudioCodec;
	stAvMsg.stChangePidMsg.eAuxAudioCodec = eAuxCodec;
	stAvMsg.stChangePidMsg.eDolbyCodec = eDolbyCodec; 				// 2009.03.23 추가.
	stAvMsg.stChangePidMsg.eVideoCodec = eVideoCodec;
	stAvMsg.stChangePidMsg.bDolbySelected = bDolbySelected;
	stAvMsg.stChangePidMsg.eStereoSelect = eStereoSelect;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("Message Send Failure. \n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}
#endif
HERROR SVC_AV_SetVideoFreeze(HUINT32 ulActionId, HBOOL bFreeze)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulActionId);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stFreezeMsg.eCommand = eCMD_AV_FREEZE_VIDEO;
	stAvMsg.stFreezeMsg.ulActionId = ulActionId;
	stAvMsg.stFreezeMsg.bFreeze = bFreeze;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");

		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

HERROR SVC_AV_SetAudioFreeze(HUINT32 ulActionId, HBOOL bFreeze)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n",  ulActionId);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stFreezeMsg.eCommand = eCMD_AV_FREEZE_AUDIO;
	stAvMsg.stFreezeMsg.ulActionId = ulActionId;
	stAvMsg.stFreezeMsg.bFreeze = bFreeze;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");

		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

/* MW_AV_FreezeService()가 sync 이므로 inline으로 해도 최종 결과를 알아볼 수 있다. */
HERROR SVC_AV_GetVideoFreeze(HUINT32 ulActionId, HBOOL *pbVideoFreeze)
{
	CHECK_ACTION_ID(ulActionId);

	*pbVideoFreeze = s_stAvContext[ulActionId].bVideoFreeze;

	return ERR_OK;
}

HERROR SVC_AV_GetAudioFreeze(HUINT32 ulActionId, HBOOL *pbAudioFreeze)
{
	CHECK_ACTION_ID(ulActionId);

	*pbAudioFreeze = s_stAvContext[ulActionId].bAudioFreeze;

	return ERR_OK;
}

// Async API
HERROR SVC_AV_CaptureStillPicture(
								Handle_t						hAction,
								HUINT8							*pBuffer,
								HUINT32							ulBufferSize,
								DxVideoCodec_e					eVideoCodec,
								HUINT32							ulCaptureWidth,
								HUINT32							ulCaptureHeight,
								PixelFormat_e					ePixelFormat,
								SvcAv_CopyFrameBufferCallback_t	pfnCopyFrameBuffer
								)
{
	AV_Message_t	stAvMsg;
	HUINT32			ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			err;

	HxLOG_Debug("(%d)\n",  ulActionId);
	CHECK_ACTION_ID(ulActionId);

	if( (pBuffer == NULL)
		|| (ulBufferSize == 0)
		|| (eVideoCodec == eDxVIDEO_CODEC_UNKNOWN)
		|| (pfnCopyFrameBuffer == NULL)
	)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	stAvMsg.stCaptureStillMsg.eCommand = eCMD_AV_CAPTURE_STILL;
	stAvMsg.stCaptureStillMsg.hAction = hAction;
	stAvMsg.stCaptureStillMsg.buffer = pBuffer;
	stAvMsg.stCaptureStillMsg.bufferSize = ulBufferSize;
	stAvMsg.stCaptureStillMsg.eVideoCodec = eVideoCodec;
	stAvMsg.stCaptureStillMsg.ulCaptureWidth = ulCaptureWidth;
	stAvMsg.stCaptureStillMsg.ulCaptureHeight = ulCaptureHeight;
	stAvMsg.stCaptureStillMsg.ePixelFormat = ePixelFormat;
	stAvMsg.stCaptureStillMsg.pfnCopyFrameBuffer = pfnCopyFrameBuffer;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_AV_StartStillPicture(Handle_t hAction, HUINT8 *pBuffer, HUINT32 ulBufferSize, DxVideoCodec_e eVideoCodec)
{
	AV_Message_t	stAvMsg;
	HUINT32			ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, PAL_PIPE_GetActionId(hAction));
	CHECK_ACTION_ID(ulActionId);

	if( (pBuffer == NULL) || (ulBufferSize == 0) || (eVideoCodec == eDxVIDEO_CODEC_UNKNOWN) )
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	stAvMsg.stDecodeStillMsg.eCommand = eCMD_AV_DECODE_STILL;
	stAvMsg.stDecodeStillMsg.hAction= hAction;
	stAvMsg.stDecodeStillMsg.buffer = pBuffer;
	stAvMsg.stDecodeStillMsg.bufferSize = ulBufferSize;
	stAvMsg.stDecodeStillMsg.bPlay = TRUE;
	stAvMsg.stDecodeStillMsg.eVideoCodec = eVideoCodec;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

// Async API
HERROR SVC_AV_StopStillPicture(Handle_t hAction)
{
	AV_Message_t	stAvMsg;
	HUINT32			ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			err;

	HxLOG_Debug("(%d)\n",  PAL_PIPE_GetActionId(hAction));
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stDecodeStillMsg.eCommand = eCMD_AV_DECODE_STILL;
	stAvMsg.stDecodeStillMsg.hAction= hAction;
	stAvMsg.stDecodeStillMsg.bPlay = FALSE;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR SVC_AV_SetAudioEvtTimeOut(HUINT32 msec)
{
	return PAL_AUDIO_SetEvtTimeOut(msec);
}

HERROR SVC_AV_SetVideoEvtTimeOut(HUINT32 msec)
{
	return PAL_VIDEO_SetEvtTimeOut(msec);
}

HERROR SVC_AV_GetVideoSourceTiming(HUINT32 ulActionId, VIDEO_Timing_t *pstVideoSourceTiming)
{
	HERROR			err ;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);
	CHECK_ACTION_ID(ulActionId);

	if(pstVideoSourceTiming == NULL)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_SUPPORT_FCC)
	ulActionId = svc_av_GetAdjustActionId(ulActionId);
#endif

	err = PAL_VIDEO_GetSourceTiming(s_stAvContext[ulActionId].ulDecId, pstVideoSourceTiming);
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_AV_GetStreamAspectRatio(HUINT32 ulActionId, DxAspectRatio_e *arMode )
{
	VideoStreamInfo_t	stStreamInfo;
	HERROR				err;

	HxLOG_Debug("(%d)\n",  ulActionId);
	CHECK_ACTION_ID(ulActionId);

#if defined(CONFIG_SUPPORT_FCC)
	ulActionId = svc_av_GetAdjustActionId(ulActionId);
#endif

	err = PAL_VIDEO_GetStreamInfo(s_stAvContext[ulActionId].ulDecId, &stStreamInfo);
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	*arMode = stStreamInfo.eAspectRatio;

	return ERR_OK;
}


HERROR SVC_AV_GetStreamAFD(HUINT32 ulActionId, VIDEO_Afd_t *eAfd)
{
	VideoStreamInfo_t	stStreamInfo;
	HERROR				err;

	HxLOG_Debug("(%d)\n",  ulActionId);
	CHECK_ACTION_ID(ulActionId);

#if defined(CONFIG_SUPPORT_FCC)
	ulActionId = svc_av_GetAdjustActionId(ulActionId);
#endif

	err = PAL_VIDEO_GetStreamInfo(s_stAvContext[ulActionId].ulDecId, &stStreamInfo);
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	*eAfd = stStreamInfo.eAfd;

	return ERR_OK;
}

HERROR SVC_AV_SetStereoSelect(HUINT32 ulActionId, DxStereoSelect_e eStereoSelect)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d, %s)\n",  ulActionId, OTL_ENUM2STR_StereoSelect(eStereoSelect));
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stStereoCfgMsg.eCommand = eCMD_AV_CONFIG_STEREO;
	stAvMsg.stStereoCfgMsg.ulActionId = ulActionId;
	stAvMsg.stStereoCfgMsg.eSteteoSelect = eStereoSelect;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

HERROR SVC_AV_GetStereoSelect(HUINT32 ulActionId, DxStereoSelect_e *peStereoConfig)
{
	CHECK_ACTION_ID(ulActionId);

	*peStereoConfig = s_stAvContext[ulActionId].eStereoSelect;

	return ERR_OK;
}

HERROR SVC_AV_SetDualMonoSelect(HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoSelect)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d, %s)\n",  ulActionId, OTL_ENUM2STR_DualMonoSelect(eDualMonoSelect));
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stDualMonoCfgMsg.eCommand = eCMD_AV_CONFIG_DUALMONO;
	stAvMsg.stDualMonoCfgMsg.ulActionId = ulActionId;
	stAvMsg.stDualMonoCfgMsg.eDualMonoSelect = eDualMonoSelect;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

HERROR SVC_AV_GetDualMonoSelect(HUINT32 ulActionId, DxDualMonoSelect_e *peDualMonoConfig)
{
	CHECK_ACTION_ID(ulActionId);

	*peDualMonoConfig = s_stAvContext[ulActionId].eDualMonoSelect;

	return ERR_OK;
}

HERROR SVC_AV_SetAudioLipSyncDelay(HINT32 msec)
{
	HxLOG_Debug("(%d ms)\n",  msec);

	return PAL_AUDIO_SetDelayAudioLipSync(msec);
}

HERROR SVC_AV_GetAudioLipSyncDelay(HINT32 *pmsec)
{
	return PAL_AUDIO_GetDelayAudioLipSync(pmsec);
}

DecoderState_t SVC_AV_GetVideoDecoderState(HUINT32 ulActionId)
{
	return PAL_VIDEO_GetState(s_stAvContext[ulActionId].ulDecId);
}

DecoderState_t SVC_AV_GetAudioDecoderState(void)
{
	return PAL_AUDIO_GetState();
}

HBOOL SVC_AV_isRunning(HUINT32 ulActionId)
{
	//HxLOG_Debug("ulActionId = [%d], bContextRunning = [%d] \n", ulActionId, s_stAvContext[ulActionId].bContextRunning);
	return s_stAvContext[ulActionId].bContextRunning;
}

HERROR SVC_AV_GetVideoStreamInfo(HUINT32 ulActionId, VideoStreamInfo_t *pstVideoStreamInfo)
{
	HxLOG_Debug("%d)\n", ulActionId);
	return PAL_VIDEO_GetStreamInfo(s_stAvContext[ulActionId].ulDecId, pstVideoStreamInfo);
}

#define _____AUDIO_CLIP_____________________________________________________
HERROR SVC_AV_StartPCM(
						HUINT8			*pucBuffer,
						HUINT32 		ulBufferBytes,
						DxAudioCodec_e	eCodec,
						HUINT32 		ulLoopCnt
						)
{
	HxLOG_Debug("\n");

	return PAL_AUDIO_StartPCM(pucBuffer, ulBufferBytes, eCodec, ulLoopCnt);
}

HERROR SVC_AV_StopPCM(void)
{
	HxLOG_Debug("\n");

	return PAL_AUDIO_StopPCM();
}

HERROR SVC_AV_StartAudioClip(
							HUINT32			ulRequestId,
							HUINT8			*pucBuffer,
							HUINT32			ulBufferBytes,
							DxAudioCodec_e	eCodec,
							HUINT32			ulLoopCnt
							)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulRequestId);

	if( (pucBuffer == NULL) || (ulBufferBytes == 0) )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	stAvMsg.stAudioClipMsg.eCommand = eCMD_AV_PLAY_AUDIOCLIP;
	stAvMsg.stAudioClipMsg.ulRequestId = ulRequestId;
	stAvMsg.stAudioClipMsg.pucBuffer = pucBuffer;
	stAvMsg.stAudioClipMsg.ulBufferBytes = ulBufferBytes;
	stAvMsg.stAudioClipMsg.eCodec = eCodec;
	stAvMsg.stAudioClipMsg.ulLoopCnt = ulLoopCnt;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_AV_StopAudioClip(HUINT32 ulRequestId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulRequestId);

	stAvMsg.stAudioClipMsg.eCommand = eCMD_AV_STOP_AUDIOCLIP;
	stAvMsg.stAudioClipMsg.ulRequestId = ulRequestId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_AV_PauseAudioClip(HUINT32 ulRequestId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulRequestId);

	stAvMsg.stAudioClipMsg.eCommand = eCMD_AV_PAUSE_AUDIOCLIP;
	stAvMsg.stAudioClipMsg.ulRequestId = ulRequestId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}



HERROR SVC_AV_ResumeAudioClip(HUINT32 ulRequestId)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d)\n", ulRequestId);

	stAvMsg.stAudioClipMsg.eCommand = eCMD_AV_RESUME_AUDIOCLIP;
	stAvMsg.stAudioClipMsg.ulRequestId = ulRequestId;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_AV_SetVideoHide(HUINT32 ulActionId, HUINT32 ulClientMask, HBOOL bHideFlag)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d, ulClientMask:0x%x, %d)\n", ulActionId, ulClientMask, bHideFlag);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stSetHideMsg.eCommand = eCMD_AV_HIDE_VIDEO;
	stAvMsg.stSetHideMsg.ulActionId = ulActionId;
	stAvMsg.stSetHideMsg.ulClientMask = ulClientMask;
	stAvMsg.stSetHideMsg.bHideFlag = bHideFlag;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}

HERROR SVC_AV_SetAudioHide(HUINT32 ulActionId, HUINT32 ulClientMask, HBOOL bHideFlag)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d, ulClientMask:0x%x, %d)\n", ulActionId, ulClientMask, bHideFlag);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stSetHideMsg.eCommand = eCMD_AV_HIDE_AUDIO;
	stAvMsg.stSetHideMsg.ulActionId = ulActionId;
	stAvMsg.stSetHideMsg.ulClientMask = ulClientMask;
	stAvMsg.stSetHideMsg.bHideFlag = bHideFlag;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;
}


//action Id를 가지고 svc_av context 에서 decorder ID를찾아 control 할 수 있게 하기 위해서 여기에 둔다.
HERROR SVC_AV_SetVideoOutputEnable(HUINT32 ulActionId, eSvcAv_DisplayType_e eDisplayType, HBOOL bEnableFlag)
{
	AV_Message_t	stAvMsg;
	HERROR			err;

	HxLOG_Debug("(%d, %d)\n", ulActionId, bEnableFlag);
	CHECK_ACTION_ID(ulActionId);

	stAvMsg.stSetEnableMsg.eCommand = eCMD_AV_ENABLE_VIDEO;
	stAvMsg.stSetEnableMsg.ulActionId = ulActionId;
	stAvMsg.stSetEnableMsg.eDisplayType = eDisplayType;
	stAvMsg.stSetEnableMsg.bEnableFlag = bEnableFlag;

	err = VK_MSG_Send(s_ulAVCmdMsgQId, &stAvMsg, sizeof(AV_Message_t));
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR Critical Eror!!!\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulAvSyncSema);

	return ERR_OK;

}

void SVC_AV_SetDefaultChannelChangeMode(void)
{
	svc_av_SetChannelChangeMode(svc_av_GetDefaultChannelChangeMode());
}

void SVC_AV_SetChannelChangeMode(eSvcAv_ChChangeMode_e eMode)
{
	svc_av_SetChannelChangeMode(eMode);
}

void SVC_AV_SetShowHideControl(HBOOL bShowHideControlEnabled)
{
#if defined(CONFIG_DEBUG)
	if( bShowHideControlEnabled == FALSE )
	{
		HxLOG_Warning("#############################################\n");
		HxLOG_Warning("## CAUTION !!!!!!!\n");
		HxLOG_Warning("## bShowHideControlEnabled :: FALSE !!\n");
		HxLOG_Warning("## If you set flag to false then\n");
		HxLOG_Warning("## YOU MUST HAVE TO RESTORE FLAG to TRUE");
		HxLOG_Warning("#############################################\n");
	}
	else
	{
		HxLOG_Warning("#############################################\n");
		HxLOG_Warning("## bShowHideControlEnabled :: TRUE\n");
		HxLOG_Warning("## Good job..");
		HxLOG_Warning("#############################################\n");
	}
#endif

	PAL_VIDEO_SetShowHideControl(bShowHideControlEnabled);
}

void SVC_AV_CMD_SetChannelChangeMode(HINT32 nMode)
{
	eSvcAv_ChChangeMode_e eMode = (eSvcAv_ChChangeMode_e)nMode;

	svc_av_SetChannelChangeMode(eMode);
}

void SVC_AV_CMD_SetFccEnable(HBOOL bFccEnableByCommand)
{
#ifdef CONFIG_DEBUG
	s_bFccEnableByCommand = bFccEnableByCommand;
#endif
}

/*********************** End of File ******************************/
