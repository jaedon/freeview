/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_av.h		\n
	@brief	  av service	\n

	Description: Application에서 요청한 Audio와 Video에 관련된 Service를 제공하는 모듈. \n
	Module: MW/AV									 					\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_AV_H__
#define	__SVC_AV_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define SvcAv_START_SERVICE_NEW

#define AV_MAX_CONTEXT						2						/* main/sub 생각해서 2개로 고정 */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* @brief MW AV에서 Application으로 전송하는 event list */
enum
{
	eSEVT_AV = eSEVT_AV_START,

	/* - Description : Video decoding이 start되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_VIDEO_STARTED,

	/* - Description : Audio decoding이 start되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_AUDIO_STARTED,

	/* - Description : Video decoding이 under-run되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_VIDEO_UNDERRUN,

	/* - Description : Audio decoding이 under-run되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_AUDIO_UNDERRUN,

	/* - Description : Video 소스의 파라미터가 Update 되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_CHECK_VIDEO_STATUS,

	/* - Description : Media rect가 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : HANDLE_NULL
		   p1 : action id
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_MEDIA_RECT_CHANGED,

	/* - Description : Audio 소스의 파라미터가 Update 되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_CHECK_AUDIO_STATUS,

	/* - Description : Audio clip의 play 가 complete 됨.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_AUDIO_CLIP_COMPLETED,

	/* - Description : Audio clip의 play 가 start 됨.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_AUDIO_CLIP_STARTED,

	/* - Description : a/v decoding start 를 ap 로 notify 한다. (pvr 용)
	   - Parameters Usage :
		   handle : Action handle
		   p1 : success, fail
		   p2 : TBD
		   p3 : TBD */
	eSEVT_AV_SESSION_STARTED,

	eSEVT_AV_END
}; /* Message to GWM */

typedef enum
{
	// DI_NUM_DISPLAYS와 관계됨.
	eSvcAv_DisplayType_Hd = 0, // digital
	eSvcAv_DisplayType_Sd = 1, // analog
} eSvcAv_DisplayType_e;

typedef enum
{
	eSvcAv_ChChangeMode_Mute,
	eSvcAv_ChChangeMode_HoldUntilTsmLock,
	eSvcAv_ChChangeMode_MuteUntilFirstPicture,
	eSvcAv_ChChangeMode_HoldUntilFirstPicture,

	eSvcAv_ChChangeMode_NotSupport
} eSvcAv_ChChangeMode_e;

typedef void (* SvcAv_CopyFrameBufferCallback_t)(HUINT8 *pucFrameBuffer, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 ulBufferSize);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR SVC_AV_Init(PostMsgToMgrCb_t pfncPostMsgCallback);
HUINT32 SVC_AV_GetNumberOfVideoDecoder(void);

HERROR SVC_AV_SetMainMedia(HUINT32 ulActionId);				/* for PIP */
HERROR SVC_AV_SetMainFccMedia(HUINT32 ulActionId);			/* for Fast ch change */
HERROR SVC_AV_SetMainMediaActionId(HUINT32 ulActionId);		/* for Fast ch change */


#if defined(SvcAv_START_SERVICE_NEW)
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
						);
#else
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
						DxAudioCodec_e			eDolbyCodec,		//2009.03.23 추가.
						DxVideoCodec_e			eVideoCodec,
						HBOOL					bDolbySelected,
						DxStereoSelect_e	eStereoConfig,
						DxDualMonoSelect_e	eDualMonoConfig,
						HBOOL					bFreezeUnderrun,
						HBOOL					bUsePrimer
						);
#endif

HERROR SVC_AV_StopService(HUINT32 ulActionId, ApiSyncMode_t syncMode);

#if defined(SvcAv_START_SERVICE_NEW)
HERROR SVC_AV_ChangePid(
						HUINT32					ulActionId,
						HUINT16					usAudioPid,
						HUINT16					usAuxPid,
						HUINT16					usVideoPid,
						HUINT16					usPcrPid,
						DxAudioCodec_e			eAudioCodec,
						DxAudioCodec_e			eAuxCodec,
						DxVideoCodec_e			eVideoCodec
						);
#else
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
						DxStereoSelect_e	eStereoConfig
						);
#endif

HERROR SVC_AV_SetVideoFreeze(HUINT32 ulActionId, HBOOL bFreeze);
HERROR SVC_AV_SetAudioFreeze(HUINT32 ulActionId, HBOOL bFreeze);
HERROR SVC_AV_GetVideoFreeze(HUINT32 ulActionId, HBOOL *pbVideoFreeze);
HERROR SVC_AV_GetAudioFreeze(HUINT32 ulActionId, HBOOL *pbAudioFreeze);

HERROR SVC_AV_SetAudioEvtTimeOut(HUINT32 msec);
HERROR SVC_AV_SetVideoEvtTimeOut(HUINT32 msec);

HERROR SVC_AV_CaptureStillPicture(
								Handle_t						hAction,
								HUINT8							*pBuffer,
								HUINT32							ulBufferSize,
								DxVideoCodec_e					eVideoCodec,
								HUINT32							ulCaptureWidth,
								HUINT32							ulCaptureHeight,
								PixelFormat_e					ePixelFormat,
								SvcAv_CopyFrameBufferCallback_t	pfnCopyFrameBuffer
								);
HERROR SVC_AV_StartStillPicture(Handle_t hAction, HUINT8 *pBuffer, HUINT32 ulBufferSize, DxVideoCodec_e eVideoCodec);
HERROR SVC_AV_StopStillPicture(Handle_t hAction);

HERROR SVC_AV_StartPCM(
						HUINT8			*pucBuffer,
						HUINT32 		ulBufferBytes,
						DxAudioCodec_e	eCodec,
						HUINT32 		ulLoopCnt
						);
HERROR SVC_AV_StopPCM(void);
HERROR SVC_AV_PlayAribBeep(HUINT32 ulBeepNum);

HERROR SVC_AV_StartAudioClip(
							HUINT32			ulRequestId,
							HUINT8			*pucBuffer,
							HUINT32			ulBufferBytes,
							DxAudioCodec_e	eCodec,
							HUINT32			ulLoopCnt
							);
HERROR SVC_AV_StopAudioClip(HUINT32 ulRequestId);

HERROR SVC_AV_PauseAudioClip(HUINT32 ulRequestId);
HERROR SVC_AV_ResumeAudioClip(HUINT32 ulRequestId);

HERROR SVC_AV_GetStreamAFD(HUINT32 ulActionId, VIDEO_Afd_t *eAfd);
HERROR SVC_AV_GetStreamAspectRatio(HUINT32 ulActionId, DxAspectRatio_e *arMode );
HERROR SVC_AV_GetVideoSourceTiming(HUINT32 ulActionId, VIDEO_Timing_t *pstVideoSourceTiming);

HERROR SVC_AV_SetStereoSelect(HUINT32 ulActionId, DxStereoSelect_e eStereoConfig);
HERROR SVC_AV_GetStereoSelect(HUINT32 ulActionId, DxStereoSelect_e *peStereoConfig);

HERROR SVC_AV_SetDualMonoSelect(HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoConfig);
HERROR SVC_AV_GetDualMonoSelect(HUINT32 ulActionId, DxDualMonoSelect_e *peDualMonoConfig);

HERROR SVC_AV_SetAudioLipSyncDelay(HINT32 msec);
HERROR SVC_AV_GetAudioLipSyncDelay(HINT32 *pmsec);

DecoderState_t SVC_AV_GetVideoDecoderState(HUINT32 ulActionId);
DecoderState_t SVC_AV_GetAudioDecoderState(void);

HBOOL SVC_AV_isRunning(HUINT32 ulActionId);

HERROR SVC_AV_GetVideoStreamInfo(HUINT32 ulActionId, VideoStreamInfo_t *pstVideoStreamInfo);

HERROR SVC_AV_SetVideoHide(HUINT32 ulActionId, HUINT32 ulClientMask, HBOOL bHideFlag);
HERROR SVC_AV_SetAudioHide(HUINT32 ulActionId, HUINT32 ulClientMask, HBOOL bHideFlag);

HERROR SVC_AV_SetVideoOutputEnable(HUINT32 ulActionId, eSvcAv_DisplayType_e eDisplayType, HBOOL bEnableFlag);

void SVC_AV_SetDefaultChannelChangeMode(void);
void SVC_AV_SetChannelChangeMode(eSvcAv_ChChangeMode_e eMode);
void SVC_AV_SetShowHideControl(HBOOL bShowHideControlEnabled);

void SVC_AV_CMD_SetChannelChangeMode(HINT32 eMode);
void SVC_AV_CMD_SetFccEnable(HBOOL bFccEnableByCommand);
#define ______DEBUG_____________________________________________________________________________________________________



#endif /* !__SVC_AV_H__ */
