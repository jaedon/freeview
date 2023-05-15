/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_download.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

#ifndef __OAPI_DOWNLOAD__
#define __OAPI_DOWNLOAD__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>


#define __________DEFINE__________
#define OxDOWNLOAD_INFO_CHUNKS_MAX			20

#define OxDOWNLOAD_AUDIO_LANG_NUM				4
#define OxDOWNLOAD_ES_STRING_LEN				81

//TRACK
#define OxDOWNLOAD_VIDEO_ES_NUM_MAX			2
#define OxDOWNLOAD_AUDIO_ES_NUM_MAX			16
#define OxDOWNLOAD_SUBTTL_ES_NUM_MAX			12
#define OxDOWNLOAD_DATA_ES_NUM_MAX			12	/* TR-B14 제3편 2.1.2.5 ES수의 제한 */
#define OxDOWNLOAD_COMP_ROLE_NUM				256
#define OxDOWNLOAD_COMP_NAME_NUM				256


#define __________RPC__________
// APPKIT -> OBAMA
#define RPC_OAPI_DOWNLOAD_Activate						"oapi.download.Activate"
#define RPC_OAPI_DOWNLOAD_Deactivate					"oapi.download.Deactivate"
#define RPC_OAPI_DOWNLOAD_Probe							"oapi.download.Probe"
#define RPC_OAPI_DOWNLOAD_Start							"oapi.download.Start"
#define RPC_OAPI_DOWNLOAD_Stop							"oapi.download.Stop"
#define RPC_OAPI_DOWNLOAD_Pause							"oapi.download.Pause"
#define RPC_OAPI_DOWNLOAD_Resume						"oapi.download.Resume"

// OBAMA -> APPKIT (SETTER)

// OBAMA -> APPKIT (EVENT)
#define RPC_OBAMA_DOWNLOAD_OnStarted					"obama.download.OnStarted"
#define RPC_OBAMA_DOWNLOAD_OnCompleted					"obama.download.OnCompleted"
#define RPC_OBAMA_DOWNLOAD_OnFailed						"obama.download.OnFailed"
#define RPC_OBAMA_DOWNLOAD_OnStopped					"obama.download.OnStopped"
#define RPC_OBAMA_DOWNLOAD_OnReceiving					"obama.download.OnReceiving"
#define RPC_OBAMA_DOWNLOAD_OnProbeFail					"obama.download.OnProbeFail"
#define RPC_OBAMA_DOWNLOAD_OnProbeComplete				"obama.download.OnProbeComplete"
#define RPC_OBAMA_DOWNLOAD_OnDownloadInfoChanged		"obama.download.OnDownloadInfoChanged"
#define RPC_OBAMA_DOWNLOAD_OnComponentVideoChanged		"obama.download.OnComponentVideoChanged"
#define RPC_OBAMA_DOWNLOAD_OnComponentAudioChanged		"obama.download.OnComponentAudioChanged"
#define RPC_OBAMA_DOWNLOAD_OnComponentSubtitleChanged	"obama.download.OnComponentSubtitleChanged"
#define RPC_OBAMA_DOWNLOAD_OnComponentDataChanged		"obama.download.OnComponentDataChanged"
#define RPC_OBAMA_DOWNLOAD_OnComponentSelected			"obama.download.OnComponentSelected"
#define RPC_OBAMA_DOWNLOAD_OnAvailabilityStartTimeChanged "obama.download.OnAvailabilityStartTimeChanged"

#define __________EVENT__________


#define __________ENUMURATION__________

typedef enum
{
	eOxDNLD_STATE_NONE				= 0,

	eOxDNLD_STATE_Requested,
	eOxDNLD_STATE_Started,
	eOxDNLD_STATE_Receiving,
	eOxDNLD_STATE_Paused,
	eOxDNLD_STATE_Stopped,
	eOxDNLD_STATE_Finished,

	eOxDNLD_STATE_EndOfCase
} OxDownload_State_e;

typedef enum
{
	eOxDNLD_COMPONENT_NONE	= 0,

	eOxDNLD_COMPONENT_VIDEO,
	eOxDNLD_COMPONENT_AUDIO,
	eOxDNLD_COMPONENT_SUBTITLE,
	eOxDNLD_COMPONENT_TELETEXT,
	eOxDNLD_COMPONENT_CLOSEDCAPTION,
	eOxDNLD_COMPONENT_DATA,

	eOxDNLD_COMPONENT_ENDOFCASE
} OxDownload_Component_e;

typedef enum
{
	eOxDNLD_AUDMODE_SINGLEMONO	= 0,
	eOxDNLD_AUDMODE_DUALMONO,
	eOxDNLD_AUDMODE_STEREO,
	eOxDNLD_AUDMODE_MULTILINGUAL,
	eOxDNLD_AUDMODE_SURROUND,
	eOxDNLD_AUDMODE_DOLBY,

	eOxDNLD_AUDMODE_ENDOFCASE
} OxDownload_AudioMode_e;

typedef enum
{
	eOxDNLD_AUDIOTYPE_SUPP_MAIN = 0,
	eOxDNLD_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED,
	eOxDNLD_AUDIOTYPE_SUPP_CLEAN_AUDIO_HEARING_IMPAIRED,
	eOxDNLD_AUDIOTYPE_SUPP_SPOKEN_SUBITITE_VISUAL_IMPARED,
	eOxDNLD_AUDIOTYPE_SUPPLEMENTARY_MAIN
} OxDownload_AUDIOType_SUPP_e;

typedef enum
{
	eOxDNLD_SUBTITLETYPE_Dvb				= 0,
	eOxDNLD_SUBTITLETYPE_Ebu				= 1,
	eOxDNLD_SUBTITLETYPE_Media			= 2,
	eOxDNLD_SUBTITLETYPE_Unknown			= 9
} OxDownload_SubtitleType_e;

typedef enum
{
	eOxDNLD_AUDIOTYPE_UNDEFINED			= 0,	/**< Normal Audio. */
	eOxDNLD_AUDIOTYPE_CLEAN_EFFECTS		= 1,
	eOxDNLD_AUDIOTYPE_HEARING_IMPAIRED	= 2,
	eOxDNLD_AUDIOTYPE_VISUAL_IMPAIRED		= 3,	/**< AD(Audio Description). */
	eOxDNLD_AUDIOTYPE_RESERVED			= 4
} OxDownload_AUDIOType_e;


#define	OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(eAudType, ucMixType, eSubAudType)		((eOxDNLD_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) ||(((eOxDNLD_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ( eOxDNLD_AUDIOTYPE_UNDEFINED == eAudType) || (eOxDNLD_AUDIOTYPE_CLEAN_EFFECTS == eAudType)) && ((ucMixType == 1) && (eOxDNLD_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#define	OAPI_AUDIO_IS_AD_RM_TYPE_DOWNLOAD(eAudType, ucMixType, eSubAudType)		((eOxDNLD_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ((eOxDNLD_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eOxDNLD_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))


#define __________STRUCTURE__________

typedef struct
{
	OxDownload_Component_e		 eCompType;
	HINT32						 nPid;
	HINT32						 nComponentTag;
	DxVideoCodec_e				 eCodec;
	HINT32						 nCasFound;
	HINT32						 nHdVideo;

	HINT32						nStreamType;
	HINT32						nComponentType;
	HUINT8						aucStreamName[OxDOWNLOAD_ES_STRING_LEN];
	HUINT8						aucCompName[OxDOWNLOAD_COMP_NAME_NUM];
	HUINT8						aucCompRole[OxDOWNLOAD_COMP_ROLE_NUM];
} OxDownload_VideoComp_t;

typedef struct
{
	OxDownload_AUDIOType_e	eAudType;
	HUINT8				szLangCode[DxLANG_CODE_LEN];
} OxDownload_AudioALangInfo_t;


typedef struct
{
	OxDownload_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nLangNum;
	DxAudioCodec_e				 eCodec;
	HBOOL						 bIsDolby;
	OxDownload_AudioMode_e		 eMode;
	OxDownload_AudioALangInfo_t 	astLangInfo[OxDOWNLOAD_AUDIO_LANG_NUM];
	OxDownload_AUDIOType_SUPP_e 	eSupAudType;
	HUINT8						ucMixType;

	/* For JLabs */
	HUINT8							ucComponentType;
	HUINT8							ucStreamContent;
	HUINT8							aucStreamName[OxDOWNLOAD_AUDIO_LANG_NUM];
	HUINT8							aucCompName[OxDOWNLOAD_COMP_NAME_NUM];
	HUINT8							aucCompRole[OxDOWNLOAD_COMP_ROLE_NUM];

} OxDownload_AudioComp_t;

typedef struct
{
	OxDownload_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1

	HCHAR						 szStreamName[OxDOWNLOAD_ES_STRING_LEN]; // Nothing : NULL
} OxDownload_DataComp_t;

typedef struct
{
	HINT32						 nTypeValue;			// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nCompositionPageId;	// Nothing : -1
	HINT32						 nAncillaryPageId;		// Nothing : -1
	HCHAR						 szLangCode[4];			// Nothing : szLangCode[0] == '\0'
} OxDownload_Subtitle_DVB_t;

typedef struct
{
	HINT32						 nTypeValue;			// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nMagazineNum;			// Nothing : -1
	HINT32						 nPageNum;				// Nothing : -1
	HCHAR						 szLangCode[4];			// Nothing : szLangCode[0] == '\0'
} OxDownload_Subtitle_EBU_t;

typedef struct
{
	HCHAR						 szLangCode[32];
} OxDownload_Subtitle_MEDIA_t;


typedef struct
{
	OxDownload_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	OxDownload_SubtitleType_e	 eSubtitleType;

	union
	{
		OxDownload_Subtitle_DVB_t	 stDvbInfo;
		OxDownload_Subtitle_EBU_t	 stEbuInfo;
		OxDownload_Subtitle_MEDIA_t stMediaInfo;
	} utArg;

} OxDownload_SubtitleComp_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxDownload_VideoComp_t		 astCompArray[OxDOWNLOAD_VIDEO_ES_NUM_MAX];
} OxDownload_VideoCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nCurrSubIndex;	//aux audio
	HINT32						 nArraySize, nMaxSize;
	OxDownload_AudioComp_t		 astCompArray[OxDOWNLOAD_AUDIO_ES_NUM_MAX];
} OxDownload_AudioCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxDownload_SubtitleComp_t		 astCompArray[OxDOWNLOAD_SUBTTL_ES_NUM_MAX];
} OxDownload_SubtitleCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxDownload_DataComp_t		 astCompArray[OxDOWNLOAD_DATA_ES_NUM_MAX];
} OxDownload_DataCompTable_t;

typedef	union
{
	OxDownload_Component_e		 eCompType;
	OxDownload_VideoComp_t		 stVideo;
	OxDownload_AudioComp_t		 stAudio;
	OxDownload_SubtitleComp_t	 stSubtitle;
	OxDownload_DataComp_t		 stData;
	// .... blah blah
} OxDownload_Component_t;

typedef struct
{
	/*
	 *	TODO : 	OAPI Manager <-> OAPI.so 간의 정보 Sync 를 위한 Cache Struct
	 *			VIEW_ID는 LIVE, PVR, TSR, MEDIA 모두 공유 가능한 Resource.
	 *			OBAMA에서 기능 동작이 유지 되는한 CACHE도 유지한다.
	 */

	/* INFO CACHE */
	OxDownload_VideoCompTable_t		stVideoCompTable;
	OxDownload_AudioCompTable_t		stAudioCompTable;
	OxDownload_SubtitleCompTable_t		stSubtitleCompTable;
	OxDownload_DataCompTable_t			stDataCompTable;

	HUINT32 ulDownloadId; // 이 값을 이용해 cso session 과 관련된 cache 정보를 얻는다.
	HCHAR						szAvailabilityStartTime[64];

#if 0 // 사용하지 않음
	/* COMMON CACHE */
	OxDownload_MediaType_e		eType;
	OxDownload_PlayError_e		eError;
	OxDownload_ErrorReason_e	eReason;
	OxDownload_PlayState_e		eState;

		/* PLAYER CACHE */
		HUINT32						ulTsrStartTime;
		HUINT32 					ulDurationTime;
		HUINT32 					ulTsrRecTime;
		HUINT32 					ulBufferRemaindTime;
		HUINT32 					ulBufferPercent;
		HUINT32 					ulBufferred;
		HUINT32						ulBitrate;

		/* SET CACHE */
		HINT32						nSpeed;
		HUINT32 					ulPosition;
		OxDownload_VideoSize_t 	stVideoSize;
		OxDownload_PigSize_t		stPigSize;
		HBOOL						bTSREnable;
		HBOOL						bSubtitleEnable;
		HBOOL						bVideoHideEnable;

		/* INFO CACHE */
		OxDownload_VideoCompTable_t		stVideoCompTable;
		OxDownload_AudioCompTable_t		stAudioCompTable;
		OxDownload_SubtitleCompTable_t		stSubtitleCompTable;
		OxDownload_DataCompTable_t			stDataCompTable;

		/* VIDEO STREAM INFO CACHE */
		OxDownload_VideoStreamInfo_t		stVideoStreamInfo;

		DxEvent_t 					stEventInfo;
		HBOOL						bRadigBG;

		HUINT32					ulNumSpeed;
		HINT32					anSupportSpeeds[OxDOWNLOAD_SUPPORTING_SPEED_MAX];

		HINT32		nBlocked;
		HINT32		nRating;
		HINT32		nDrmSystemId;

	/* LIVE CACHE */
	OxDownload_StartLive_t			stLive;
	OxDownload_ConflictInfo_t		stLiveConflictInfo;
	OxDownload_LiveViewState_e 	eViewState;
	OxDownload_LiveLockState_e 	eLockState;
	OxDownload_AitSectionData_t 	stAitSecData;
	OxDownload_MhegSectionData_t	stMhegSecData;
	OxDownload_RctData_t 			stRctData;
	HUINT8							szRctImageIconUrl[DxRCT_MAX_IMAGE_ICON_URL_LEN + 1];

	/* PVR PLAYBACK CACHE */
	OxDownload_StartPvrPb_t	stPvrPb;

	/* TSR PLAYBACK CACHE */
	OxDownload_StartTsrPb_t	stTsrPb;

	/* MEDIA CACHE */
	OxDownload_StartMediaPlay_t	stMediaPlay;

	/* VERIMATRIX CACHE */
	HUINT8					szVmxIdentifier[128];
	OxDownload_VmxStatus_e eVmxStatus;
	HINT32                  nVmxErrorCode;

	/* PLAYREADY CACHE */
	HUINT8				pucLAURL[OxDOWNLOAD_MEDIAPLAY_URL];

		/* SET SUBTITLE CACHE*/
		HINT32	nSubtitle_SyncTime;
		HINT32	nSubtitle_FontSize;
		HINT32	nSubtitle_TextPosition;

		/* INFO CACHE */
		OxDownload_SubtitleStatus_t 	stMediaSutbitleStatus;
		OxDownload_BufferState_e		eBufferState;
		HUINT32							ulPumpId;

		OxDownload_BufferInfo_t		stBufferInfo;


	DxCopyrightControl_TrickRestrictMode_u				uTrickRestrictMode;
#endif

} OxDownload_Cache_t;

typedef struct
{
	HUINT32 	ulSeekableChunks;	
	HUINT64 	ullSeekableStart[OxMP_MEDIAPB_CHUNKS_MAX];
	HUINT64 	ullSeekableEnd[OxMP_MEDIAPB_CHUNKS_MAX];
} OxDownload_SeekableInfo_t;

typedef struct
{
    HUINT32     ulChunks;
    HUINT32     ullStartOffsets[OxDOWNLOAD_INFO_CHUNKS_MAX];
    HUINT32     ullEndOffsets[OxDOWNLOAD_INFO_CHUNKS_MAX];
	HUINT32		unRemainBufferingTime; /* nRemainBufferSec */
	HUINT32		ulBufferPercent; /* nBufferPercentage */
	
	HINT64		llBufferedBytes;
	HINT64		llTotalBytesRemaining;
	HINT64		llTotalStreamBytes;
	HINT32		nArrivalBytedPerSec;
	HINT32		nStreamBytesPerSec;

	OxDownload_SeekableInfo_t  stSeekableInfo;
} OxDownload_BufferInfo_t;

typedef void (*OxDownload_Notifier_t) (
											HUINT32 ulDownloadId,
											DxDownload_Type_e eType,
											DxDownload_Event_e eEvent,
											DxDownload_NotifierData_t *punNotifyData,
											void *pvUserData
											);

typedef void (*OxDownload_ActivatedCb_t) (void);

#define __________INTERFACE__________

extern HERROR	OAPI_DOWNLOAD_Init (OxDownload_ActivatedCb_t pfActivated);
extern HERROR	OAPI_DOWNLOAD_SetListener (OxDownload_Notifier_t pfListener, void *pvUserData);
extern HERROR	OAPI_DOWNLOAD_ResetListener (OxDownload_Notifier_t pfListener, void *pvUserData);
extern Handle_t	OAPI_DOWNLOAD_Probe (DxDownload_Start_t *punStart);
extern Handle_t	OAPI_DOWNLOAD_Start (Handle_t hDownload, DxDownload_Start_t *punStart);
extern HERROR	OAPI_DOWNLOAD_Stop (Handle_t hDownload);
extern HERROR	OAPI_DOWNLOAD_Pause (Handle_t hDownload);
extern HERROR	OAPI_DOWNLOAD_Resume (Handle_t hDownload);
extern HERROR	OAPI_DOWNLOAD_GetStatus (Handle_t hDownload, DxDownload_Status_t *pstStatus);
extern HERROR 	OAPI_DOWNLOAD_GetBufferingChunkInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets);
extern HERROR 	OAPI_DOWNLOAD_GetSeekableRangeInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd);
extern HERROR 	OAPI_DOWNLOAD_GetRemainBufferingTime (Handle_t hDownload, HUINT32 *punRemainBufferingTime);
extern HERROR 	OAPI_DOWNLOAD_GetBufferingPercent (Handle_t hDownload, HUINT32 *pulBufferPercent);
extern HERROR 	OAPI_DOWNLOAD_GetBufferedBytes (Handle_t hDownload, HINT64 *pllBufferedBytes);
extern HERROR 	OAPI_DOWNLOAD_GetRemainingBytes (Handle_t hDownload, HINT64 *pllRemainingBytes);
extern HERROR OAPI_DOWNLOAD_GetComponentNum (HUINT32 hDownload, OxDownload_Component_e eCompType, HUINT32* pulCompNum);
extern HERROR OAPI_DOWNLOAD_GetComponent (Handle_t hDownload, OxDownload_Component_e eCompType, HUINT32 ulCompIndex, OxDownload_Component_t *pstComponet);
extern HERROR OAPI_DOWNLOAD_GetComponentIndex (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32* pnCompIndex, HINT32* pnCompSubIndex);
extern HERROR OAPI_DOWNLOAD_GetAvailabilityStartTime (Handle_t hDownload, HUINT32 szTimeStringBuffer, HUINT8 *pucTimeString);


#endif // __OAPI_DOWNLOAD__
