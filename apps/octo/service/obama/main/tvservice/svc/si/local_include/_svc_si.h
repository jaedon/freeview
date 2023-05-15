/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si.h
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

#ifndef __INT_SVC_SI_H__
#define __INT_SVC_SI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <silib.h>
#include <_svc_si_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TTX_DESTYPE_INITIAL_TELETEXT					0x01
#define TTX_DESTYPE_TELETEXT_SUBTITLE 					0x02
#define TTX_DESTYPE_ADDITIONAL_INFO_PAGE				0x03
#define TTX_DESTYPE_PROG_SCHEDULE_PAGE					0x04
#define TTX_DESTYPE_TTX_SBTL_PAGE_HEARING_IMPAIRED		0x05

#define SVCSI_VIDEO_PRIORITY_NONE					0x00000000			/** < Nothing! */
#define SVCSI_VIDEO_PRIORITY_NORMAL					0x00000001			/** < Nothing! */
#define SVCSI_VIDEO_PRIORITY_HD						0x00000002			/** < Nothing! */


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSvcSi_CtxtState_None = 0,
	eSvcSi_CtxtState_Waiting,
	eSvcSi_CtxtState_Running
} svcSi_ContextState_e;


/** @brief  SI service task에서 사용 되는  events  */
// Event를 새로 추가 할 경우에는
// 새로운 메시지용 argument structure를 밑에 만들어서
// SvcSi_Msg_t 의 union에 추가하도록 해야 한다.
typedef enum
{
	/* Session */
	eSIMSG_CMD_START_SESSION,
	eSIMSG_CMD_END_SESSION,

	/* Live */
	eSIMSG_CMD_START_LIVE,
	eSIMSG_CMD_STOP_LIVE,

	/* Record */
	eSIMSG_CMD_START_REC,
	eSIMSG_CMD_STOP_REC,

	/* Playback */
	eSIMSG_CMD_START_PB,
	eSIMSG_CMD_STOP_PB,

	/* Media Playback */
	eSIMSG_CMD_START_MEDIAPB,
	eSIMSG_CMD_STOP_MEDIAPB,

	/* HBB-TV */
	eSIMSG_CMD_START_HBBTV,
	eSIMSG_CMD_STOP_HBBTV,

	/* Search */
	eSIMSG_CMD_START_SEARCH,
	eSIMSG_CMD_STOP_SEARCH,
	eSIMSG_EVT_SEARCH_DONE,
	eSIMSG_EVT_SEARCH_FAIL,

	/* Ext Search */
	eSIMSG_CMD_START_EXTSEARCH,
	eSIMSG_CMD_STOP_EXTSEARCH,

	/* Sat Record */
	eSIMSG_CMD_START_SATRECORD,
	eSIMSG_CMD_STOP_SATRECORD,

	/* Check OTA */
	eSIMSG_CMD_START_OTACHECK,
	eSIMSG_CMD_STOP_OTACHECK,

	/* Live OTA */
	eSIMSG_CMD_START_OTALIVE,
	eSIMSG_CMD_STOP_OTALIVE,

	/* shutdown */
	eSIMSG_CMD_SHUTDOWN,

	/* Dynamic update */
	eSIMSG_CMD_START_DYNAMICUPDATE,
	eSIMSG_CMD_STOP_DYNAMICUPDATE,

	/* Time update */
	eSIMSG_CMD_START_TIMEUPDATE,
	eSIMSG_CMD_STOP_TIMEUPDATE,

	/* SAT>IP */
	eSIMSG_CMD_START_SATIP,
	eSIMSG_CMD_STOP_SATIP,

	/** Callback 함수에서 보냄 */
	eSIMSG_EVT_TABLE_START,		// Not really used, but for confirming that the message comes from the section callback:

	eSIMSG_EVT_PAT_TABLE,
	eSIMSG_EVT_CAT_TABLE,
	eSIMSG_EVT_PMT_TABLE,
	eSIMSG_EVT_NIT_TABLE,
	eSIMSG_EVT_NIT_OTH_TABLE,
	eSIMSG_EVT_SDT_TABLE,
	eSIMSG_EVT_SDT_OTH_TABLE,
	eSIMSG_EVT_BAT_TABLE,
	eSIMSG_EVT_EIT_TABLE,
	eSIMSG_EVT_EIT_OTH_TABLE,
	eSIMSG_EVT_EITSCH_TABLE,
	eSIMSG_EVT_EITSCH_OTH_TABLE,
	eSIMSG_EVT_TDT_TABLE,
	eSIMSG_EVT_TOT_TABLE,
	eSIMSG_EVT_UNT_TABLE,
	eSIMSG_EVT_DSI_TABLE,
	eSIMSG_EVT_ICT_TABLE,
	eSIMSG_EVT_DDB_TABLE,
	eSIMSG_EVT_SGT_TABLE,
	eSIMSG_EVT_AIT_TABLE,
	eSIMSG_EVT_DIT_TABLE,					// Humax Ota 를 필요한 메시지
	eSIMSG_EVT_RCT_TABLE,					// For TV AnyTime.
	eSIMSG_EVT_PMT_N_TABLE,
	eSIMSG_EVT_BIT_TABLE,

	eSIMSG_EVT_TABLE_END,		// Not really used, but for confirming that the message comes from the section callback:

	// ALL Retrieve 시 필요한 메시지
	eSIMSG_EVT_ALL_RECEIVE_START,

	eSIMSG_EVT_ALL_RECEIVE_END,

	// EXT Section Message : Section Callback용인데 External Value가 필요한 경우
	eSIMSG_EVT_EXT_SECTION_START,


	eSIMSG_EVT_EXT_SECTION_END
}svcSi_MsgType_e;

/** @brief  SI service에서 사용 될 message structure  */
// 새로운 메시지를 추가할 경우, 그 메시지당 argument의 종류를 여기에 같이 등록시켜야 한다.
typedef enum
{
	eSIMSG_ARGTYPE_None,

	eSIMSG_ARGTYPE_SyncMode,
	eSIMSG_ARGTYPE_ApiResultPtr,
	eSIMSG_ARGTYPE_SiSpecs,
	eSIMSG_ARGTYPE_ActionSubKind,
	eSIMSG_ARGTYPE_SvcHandle,
	eSIMSG_ARGTYPE_MasterSvcHandle,
	eSIMSG_ARGTYPE_SuppleSvcHandle,
	eSIMSG_ARGTYPE_SuppleType,
	eSIMSG_ARGTYPE_LiveType,
	eSIMSG_ARGTYPE_SearchType,
	eSIMSG_ARGTYPE_HumaxSvcType,
	eSIMSG_ARGTYPE_ChannelType,
	eSIMSG_ARGTYPE_SessionHandle,
	eSIMSG_ARGTYPE_NetId,
	eSIMSG_ARGTYPE_OtaOption,
	eSIMSG_ARGTYPE_OnId,
	eSIMSG_ARGTYPE_TsId,
	eSIMSG_ARGTYPE_SvcId,

	eSIMSG_ARGTYPE_TableStatus,
	eSIMSG_ARGTYPE_FilterId,
	eSIMSG_ARGTYPE_Result,

	eSIMSG_ARGTYPE_Unknown
} svcSi_MsgArgType_e;

/* Session */
typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	SvcSi_SiSpec_t	 unSpec;
} SvcSi_Arg_OpenSession_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_CloseSession_t;

/* Live */
typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	SvcSi_SiSpec_t	 unSpec;
	Handle_t		 hMasterSvc, hSuppleSvc;
	HUINT32			 ulSupType;
	HBOOL			 bFccSvc;
	SvcSi_LiveType_e	 eLiveType;
} SvcSi_Arg_StartLive_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	Handle_t		 hNextSvc;
} SvcSi_Arg_StopLive_t;

/* Playback */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_PlaybackOption_t	 stOption;
} SvcSi_Arg_StartPb_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopPb_t;

/* Media Playback */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
} SvcSi_Arg_StartMediaPb_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopMediaPb_t;


/* Record */
typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	SvcSi_SiSpec_t	 unSpec;
	Handle_t		 hMasterSvc, hSuppleSvc;
	HUINT32			 ulSupType;
} SvcSi_Arg_StartRec_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	Handle_t		 hNextSvc;
} SvcSi_Arg_StopRec_t;


/* SAT>IP */
typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	SvcSi_SiSpec_t	 unSpec;
	Handle_t		 hMasterSvc, hSuppleSvc;
	HUINT32			 ulSupType;
} SvcSi_Arg_StartSatip_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	Handle_t		 hNextSvc;
} SvcSi_Arg_StopSatip_t;

/* HBB-TV */
typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
	SvcSi_SiSpec_t	 unSpec;
	HUINT16			 usOnId;
	HUINT16			 usTsId;
	HUINT16			 usSvcId;
} SvcSi_Arg_StartHbbtv_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopHbbtv_t;

/* Search */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_SearchOption_t	 stOption;
} SvcSi_Arg_StartSrch_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopSrch_t;

/* Ext search */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_ExtSearchOption_t	 stOption;
} SvcSi_Arg_StartExtSrch_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopExtSrch_t;

/* Sat record */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_SatRecordOption_t	 stOption;
} SvcSi_Arg_StartSatRecord_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopSatRecord_t;

/* OTA check */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_SwupCheckOption_t	 stOption;
} SvcSi_Arg_StartSwupCheck_t;

/* OTA live */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	SvcSi_LiveSwupOption_t	 stOption;
} SvcSi_Arg_StartLiveSwupCheck_t;

/* Dynamic update */
typedef struct
{
	HBOOL					 	bSyncMode;
	HERROR						*phResult;
	SvcSi_SiSpec_t				 unSpec;
	SvcSi_DynamicUpdateOption_t	 stOption;
} SvcSi_Arg_StartDynamicUpdate_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopDynamicUpdate_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopSwupCheck_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopLiveSwupCheck_t;

/* Time update */
typedef struct
{
	HBOOL					 bSyncMode;
	HERROR					*phResult;
	SvcSi_SiSpec_t			 unSpec;
	HBOOL					 bUseTot;
} SvcSi_Arg_StartTimeUpdate_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;
} SvcSi_Arg_StopTimeUpdate_t;

/* Other */
typedef struct
{
	HUINT32			 ulTableStatus;
	HUINT32			 ulFilterId;
} SvcSi_Arg_SecCb_t;

typedef struct
{
	HUINT32			 ulResult;
} SvcSi_Arg_Result_t;

typedef	struct
{
	svcSi_MsgType_e	 eMsgClass;
	Handle_t		 hAction;
	HUINT32			 ulDemuxId;

	union
	{
		// Session Start / End
		SvcSi_Arg_OpenSession_t 	 	stSessionStart;
		SvcSi_Arg_CloseSession_t	 	stSessionEnd;

		// For Live Msgs:
		SvcSi_Arg_StartLive_t		 	stStartLive;
		SvcSi_Arg_StopLive_t		 	stStopLive;
		// For Playback Msgs:
		SvcSi_Arg_StartPb_t		 		stStartPb;
		SvcSi_Arg_StopPb_t		 		stStopPb;
		// For Media Playback Msgs:
		SvcSi_Arg_StartMediaPb_t 		stStartMediaPb;
		SvcSi_Arg_StopMediaPb_t	 		stStopMediaPb;
		// For Record Msgs:
		SvcSi_Arg_StartRec_t		 	stStartRec;
		SvcSi_Arg_StopRec_t		 		stStopRec;
		// For Search Msgs:
		SvcSi_Arg_StartSrch_t		 	stStartSrch;
		SvcSi_Arg_StopSrch_t		 	stStopSrch;
		// For Ext Search Msgs:
		SvcSi_Arg_StartExtSrch_t		stStartExtSrch;
		SvcSi_Arg_StopExtSrch_t		 	stStopExtSrch;
		// For Sat Record Msgs:
		SvcSi_Arg_StartSatRecord_t		stStartSatRecord;
		SvcSi_Arg_StopSatRecord_t		stStopSatRecord;
		// For OTA-Check Msgs:
		SvcSi_Arg_StartSwupCheck_t		stStartSwupCheck;
		SvcSi_Arg_StopSwupCheck_t	 	stStopSwupCheck;
		// For OTA-Live Msgs:
		SvcSi_Arg_StartLiveSwupCheck_t	stStartLiveSwupCheck;
		SvcSi_Arg_StopLiveSwupCheck_t	stStopLiveSwupCheck;
		// For Dynamic update Msgs:
		SvcSi_Arg_StartDynamicUpdate_t	stStartDynamicUpdate;
		SvcSi_Arg_StopDynamicUpdate_t	stStopDynamicUpdate;
		// For Time update Msgs:
		SvcSi_Arg_StartTimeUpdate_t	 	stStartTimeUpdate;
		SvcSi_Arg_StopTimeUpdate_t	 	stStopTimeUpdate;
		// For SAT>IP Msgs:
		SvcSi_Arg_StartSatip_t		 	stStartSatip;
		SvcSi_Arg_StopSatip_t		 	stStopSatip;
		// For HBB-TV Msgs
		SvcSi_Arg_StartHbbtv_t		 	stStartHbbtv;
		SvcSi_Arg_StopHbbtv_t		 	stStopHbbtv;

		// For Section Callback:
		SvcSi_Arg_SecCb_t			 	stSection;
		// For SI execution results : (Search Result, etc)
		SvcSi_Arg_Result_t			 	stSiResult;

	} unArg;
} SvcSi_Msg_t;

typedef void (*SvcSi_ActFunc) (SvcSi_Msg_t *pstMsg, void *pvData);

typedef struct
{
	HBOOL			 bMached;
	HINT32			 nPriority;
} svcSi_EsMatch_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

// svc_si_main.h
extern HERROR		svc_si_InitTask (PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData);
extern void			svc_si_SetCallback (SvcSi_GetSysConfigCb_t fnSetSysConfig);
extern HERROR		svc_si_GetSiState (HUINT32 ulActionId, svcSi_ContextState_e *peState);
extern HERROR		svc_si_GetSiSpec_regardlessOfState (HUINT32 ulActionId, SvcSi_SiSpec_t *punSpec);
extern HERROR		svc_si_GetSiSpec (HUINT32 ulActionId, SvcSi_SiSpec_t *punSpec);
extern void*		svc_si_GetSiData (HUINT32 ulActionId);
extern HERROR		svc_si_GetPrivateSpecifyValue(HUINT32 ulActSpec,HUINT32 *pulPrivateSpecifyValue);
extern const SvcSi_Config_t *svc_si_GetSysConfig(SvcSi_UserSettings_t *pstUserSettings);
extern HERROR		svc_si_SvcInfo2UserSettings(const DbSvc_Info_t *pstSvcInfo, SvcSi_UserSettings_t *pstUserSettings);

extern HERROR		svc_si_SetAudioPriorityTable(SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum);
extern HERROR		svc_si_GetAudioPriorityTable(SvcSi_EsTypePriority_t **pastTable, HUINT32 *pulNum);
extern HERROR		svc_si_SetSubtitlePriorityTable(SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum);
extern HERROR		svc_si_GetSubtitlePriorityTable(SvcSi_EsTypePriority_t **pastTable, HUINT32 *pulNum);

extern HERROR		svc_si_SendMsgToSiTask (SvcSi_Msg_t *pstMsg);
extern HERROR		svc_si_SendSectionMessage (svcSi_MsgType_e eMsgType, Handle_t hAction, HUINT32 ulTableStatus, HUINT32 ulFilterId);
extern void			svc_si_GetSyncSem (HUINT32 ulActionId);
extern void			svc_si_ReleaseSyncSem (HUINT32 ulActionId);
extern void			svc_si_PostMsgToAp (HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
extern void 		svc_si_PostDataToAp (HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize);
extern HERROR		svc_si_LibGetUndefinedServiceName (HUINT32 ulActionId, HUINT8 **ppszServiceName);
extern HERROR		svc_si_LibGetUndefinedProviderName (HUINT32 ulActionId, HUINT8 **ppszProviderName);
extern HERROR		svc_si_LibGetUndefinedNetworkName (HUINT32 ulActionId, HUINT8 **ppszNetworkName);

extern HERROR		svc_si_GetAlternativeAudioPriority (const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect);

#endif	// __INT_SVC_SI_H__
/* end of file */

