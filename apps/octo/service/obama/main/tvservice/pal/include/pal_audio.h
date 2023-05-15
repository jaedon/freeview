/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_audio.h.
	@brief	  pal layer의 audio module header file.

	Description: audio play/stop, 등의 동작을 처리.	\n
	Module: PAL/AUDIO							 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef	__PAL_AUDIO_H__
#define	__PAL_AUDIO_H__

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
#define MAX_AUDIO_CONTEXT 			2

#if defined(CONFIG_SUPPORT_FCC)
// FCC에서 audio primer를 사용하려면 아래 define을 enable시켜야 함
//#define FCC_USE_AUDIO_PRIMER
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
 /* @brief PAL Audio의 Device Context State ..... */
typedef enum
{
	eAUDIO_EVENT_NONE,
	eAUDIO_EVENT_UNDERRUN,
	eAUDIO_EVENT_FRAME_START,
	eAUDIO_EVENT_PARAM_CHANGED,
	eAUDIO_EVENT_CLIP_COMPLETE,			// Audio clip play가 stop 된 경우에 ...
	eAUDIO_EVENT_STARTED,			// Audio clip play가 start 된 경우에 ...
	eAUDIO_EVENT_MAX
} PalAudio_Event_e;


/** @brief 오디오 스트림 속성 파라미터 정보 */
typedef struct
{
	DxAudioCodec_e					eCodec;							/**< */
	DxStereoFormat_e 				eStereoType;					/**< */
	HUINT32							ulSampleRate;					/**< */
//	HUINT32							layer;
//	HUINT32							ulBitRate;
} PalAudio_StreamInfo_t;

typedef struct
{
	HUINT16							usMainPid;
	DxAudioCodec_e 					eMainCodec;					/* main audio codec */

	HUINT16							usSubPid;
	DxAudioCodec_e 					eSubCodec;					/* aux audio(audio description) codec */
} PalAudio_DecodeParam_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR PAL_AUDIO_Init(void);
HERROR PAL_AUDIO_StartDecoding(PalAudio_DecodeParam_t *pParam);
HERROR PAL_AUDIO_StopDecoding(void);
HERROR PAL_AUDIO_RegisterEventCallback(void (*pfnEventCallback)(PalAudio_Event_e eAudioEvent, HUINT32 ulRequestId));
HERROR PAL_AUDIO_RegisterEventCallbackForCas(void (*pfnEventCallback)(PalAudio_Event_e eAudioEvent));

HERROR PAL_AUDIO_SetEvtTimeOut(HUINT32 msec);

HBOOL PAL_AUDIO_IsSupportCodec(AudioDecType_t eType, DxAudioCodec_e eAudCodec);

HERROR PAL_AUDIO_SetStereoConfig(DxStereoSelect_e eStereoConfig);

HERROR PAL_AUDIO_SetDelayAudioLipSync(HINT32 msec);
HERROR PAL_AUDIO_GetDelayAudioLipSync(HINT32 *pmsec);
HBOOL PAL_AUDIO_GetFreeze(void);
void PAL_AUDIO_SetFreeze(HBOOL bFreeze);
HERROR PAL_AUDIO_SetHide(AV_HideClient_t eAvHideClient, HBOOL bHide);
DecoderState_t PAL_AUDIO_GetState(void);

const HINT8 *PAL_AUDIO_GetStrDeviceEvt(PalAudio_Event_e cmd);

HERROR PAL_AUDIO_StartPCM(HUINT8		*pucBuffer,
						HUINT32			ulBufferBytes,
						DxAudioCodec_e	eCodec,
						HUINT32			ulloopCnt
						);
HERROR PAL_AUDIO_StopPCM(void);

HERROR PAL_AUDIO_StartAudioClip(HUINT32 		ulRequestId,
							HUINT8			*pucBuffer,
							HUINT32			ulBufferBytes,
							DxAudioCodec_e	eCodec,
							HUINT32			ulloopCnt
							);
HERROR PAL_AUDIO_StopAudioClip(HUINT32 ulRequestId);
HERROR PAL_AUDIO_PauseAudioClip(HUINT32 ulRequestId);
HERROR PAL_AUDIO_ResumeAudioClip(HUINT32 ulRequestId);

HERROR PAL_AUDIO_GetStreamInfo(PalAudio_StreamInfo_t *pstStreamInfo);
HERROR PAL_AUDIO_RegisterEventCallbackForAudioError(void (*pfnEventCallback)(HUINT32 ulPtsCountError, HUINT32 ulFrameError));

#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
HERROR PAL_AUDIO_StartPrimerDecoding(PalAudio_DecodeParam_t *pstDecodeParam, HBOOL bMainAv, HUINT32 ulPrimerIdx, HBOOL bStartPrimer);
HERROR PAL_AUDIO_StopPrimerDecoding(HUINT32 ulPrimerIdx);
HERROR PAL_AUDIO_StopPrimer(HUINT32 ulPrimerIdx);
#endif

#endif	//__PAL_AUDIO_H__

