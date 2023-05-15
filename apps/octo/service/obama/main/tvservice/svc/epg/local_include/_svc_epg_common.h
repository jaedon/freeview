/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__EPG_COMMON_H__
#define	__EPG_COMMON_H__
#define TEST_VERSION_CHANGE

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_epg.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef HERROR (* pfnEpgInitAction_t) (SvcEpg_EpgKind_e eEpgKind);
typedef HERROR (* pfnEpgStart_t) (SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef HERROR (* pfnEpgStop_t) (SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction);
typedef HERROR (* pfnEpgLoadData_t) (SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
typedef HERROR (* pfnEpgUnLoadData_t) (SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);
typedef HERROR (* pfnEpgStartContent_t) (SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
typedef HERROR (* pfnEpgStopContent_t) (SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);

typedef struct tagEpgCb_t
{
	pfnEpgInitAction_t		pfnEpgInitAction;
	pfnEpgStart_t			pfnEpgStart;
	pfnEpgStop_t			pfnEpgStop;
	pfnEpgLoadData_t		pfnEpgLoadData;
	pfnEpgUnLoadData_t		pfnEpgUnLoadData;
	pfnEpgStartContent_t	pfnEpgStartContent;
	pfnEpgStopContent_t		pfnEpgStopContent;
} svcEpg_Callback_t;


// low 16bit
typedef enum
{
	EPG_CMD_START					= 1,
	EPG_CMD_STOP,
	EPG_CMD_START_FILTER,
	EPG_CMD_NEXT_FILTER,
	EPG_CMD_STOP_FILTER,
	EPG_CMD_NEXT_FILTER_TIMER,
	EPG_CMD_PFACT_1CYCLE,

	EPG_CMD_PAT_RECEIVED,
	EPG_CMD_PMT_RECEIVED,
	EPG_CMD_SDT_RECEIVED,
	EPG_CMD_CIT_RECEIVED,
	EPG_CMD_CPT_RECEIVED,
	EPG_CMD_CIT_COMPLETE,
	EPG_CMD_CPT_COMPLETE,

	EPG_CMD_DSI_RECEIVED,
	EPG_CMD_DDB_RECEIVED,

	EPG_CMD_DSMCC_CHANGE,
	EPG_CMD_LOAD_XMLFILE,
	EPG_CMD_STOP_TVTV_ACTION,
	EPG_CMD_UNLOAD_TVTV,
	EPG_CMD_UNLOAD_HUMAX,

	EPG_CMD_SERVICE_LIST_RECEIVED,
	EPG_CMD_EVENT_LIST_RECEIVED,
	EPG_CMD_STOP_HUMAX_ACTION,
    EPG_CMD_STOP_IPEPG_RECEIVED_ACTION,

	EPG_CMD_CONTEXT,
#if defined(CONFIG_DEBUG)
	EPG_CMD_PRINT,
	EPG_CMD_CHECK_DONE,
	EPG_CMD_CHECK_STATUS,
#endif
	CMD_EPG_MAX
} svcEpg_Cmd_e;

// high 16bit
typedef enum
{

	eEPG_PROCESS_EIT_PF			= 0,
	eEPG_PROCESS_EIT_SCH,
	eEPG_PROCESS_CONTENT,
	eEPG_PROCESS_TVTV,
	eEPG_PROCESS_HUMAX,
	eEPG_PROCESS_FSAT_EIT_PFA,
	eEPG_PROCESS_FSAT_EIT_PFO,
	eEPG_PROCESS_FSAT_EIT_SCO,
	eEPG_PROCESS_FSAT_EIT_PFOEXT,
	eEPG_PROCESS_FSAT_EIT_SCOEXT,

	eEPG_PROCESS_MAX
} svcEpg_ProcessType_e;

typedef enum
{
	eEPG_ACTTYPE_VIEW_0			= 0,
	eEPG_ACTTYPE_VIEW_1,
	eEPG_ACTTYPE_REC_0			= eEPG_ACTTYPE_VIEW_0 + NUM_VIEW_ACTION,
	eEPG_ACTTYPE_REC_1,
	eEPG_ACTTYPE_BG_0			= eEPG_ACTTYPE_REC_0 + NUM_MAX_REC_ACTION,
	eEPG_ACTTYPE_BG_1,

	eEPG_ACTTYPE_MAX				= eEPG_ACTTYPE_BG_0 + NUM_BG_ACTION
} svcEpg_ActionType_e;

typedef enum
{
	eEPG_MSG_TABLE_START,

	eEPG_MSG_EIT_TABLE,
	eEPG_MSG_EIT_OTH_TABLE,
	eEPG_MSG_EITSCH_TABLE,
	eEPG_MSG_EITSCH_OTH_TABLE,
	eEPG_MSG_TDT_TABLE,
	eEPG_MSG_TOT_TABLE,
	eEPG_MSG_DSI_TABLE,
	eEPG_MSG_DDB_TABLE,

	eEPG_MSG_CIT_TABLE,
	eEPG_MSG_CPT_TABLE,

#if defined(CONFIG_MW_EPG_TVTV)
	eEPG_MSG_TVTV_POSTPROCESSBUFFER,
	eEPG_MSG_TVTV_POSTPROCESSFILE,
	eEPG_MSG_TVTV_POSTPROCESSCOMPLETE,
	eEPG_MSG_TVTV_PARSEBUFFER,
	eEPG_MSG_TVTV_PARSEFILE,
	eEPG_MSG_TVTV_LOADEVENT,
#endif
	eEPG_MSG_END

} svcEpg_Message_e;


#if 0	// Not Used
typedef enum enEpgStartType
{
	EPG_START_TYPE_LIVE		= 0x01,
	EPG_START_TYPE_BARKER,

	EPG_START_TYPE_MAX
}EPG_START_TYPE_e;
#endif

typedef enum enEpgTuneInfo
{
	eEPG_TUNE_INFO_LIVE		= 0x01,
	eEPG_TUNE_INFO_HOMECH,
	eEPG_TUNE_INFO_TVTV,

	eEPG_TUNE_INFO_MAX
} svcEpg_TuneInfo_e;



typedef enum enEpgEitDescriptorMask
{
	eEPG_DES_SHORT_EVENT				= 0x0001,
	eEPG_DES_EXTENDED_EVENT				= 0x0002,
	eEPG_DES_PARENTAL_RATING			= 0x0004,
	eEPG_DES_LINKAGE					= 0x0008,
	eEPG_DES_MULTILANGUAL_COMPONENT		= 0x0010,
	eEPG_DES_CONTENT					= 0x0020,
	eEPG_DES_TELEPHONE					= 0x0040,
	eEPG_DES_TIMESHIFT					= 0x0080,

	eEPG_DES_CONTENT_IDENTIFIER			= 0x0100,
	eEPG_DES_FTA_CONTENTS_MGM			= 0x0200,

	eEPG_DES_CBI						= 0x0400,

	eEPG_DES_SERIESLINK					= 0x1000,
	eEPG_DES_SAMELINK					= 0x2000,
	eEPG_DES_ETC						= 0x4000,

	eEPG_DES_GUIDANCE					= 0x8000,

	eEPG_DES_MAX
} svcEpg_DescriptorMask_e, svcEpg_DescriptorMask_b;

typedef	struct tagEPGMSG
{
	HUINT32			ulMessage;
	HUINT32			ulParam1;
	HUINT32			ulParam2;
	HUINT32			ulParam3;
	HUINT32			ulParam4;
	HUINT32			ulParam5;
} svcEpg_Msg_t;


typedef struct tagEpgServiceInfo
{
	HUINT8						ucTableId;
	HUINT8						ucLastTableId;
	HUINT16						usTsId;
	HUINT16						usOnId;
	HUINT16						usSvcId;
	HUINT8						ucVersion;
	HUINT8						ucSectionNumber;
	HUINT8						ucLastSectionNumber;
	HUINT8						ucSegmentLastSectionNumber;
	HUINT32						ulCrc;
} svcEpg_ServiceInfo_t;

typedef struct
{
	HUINT32						eState;	 			/**< unused, skipped, init, running, done */

	HUINT32						ulTable;			/**< table kind */
	HUINT16						usPid;				/**< pid */
	HUINT16						usExtId;			/**< table id extension */
	HUINT16						usFilterId;			/**< filter's request id */

	HUINT8						ucTaskSlot;
	HUINT8						ucCycle;
	HBOOL						bComplete;
	HBOOL						bTimeOut;
	HUINT32						ulSectionNum;		/**< only debug */
	HUINT32						ulBufferSize;

#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT32						ulTransactionId;
	HUINT32						ulDsiCrc;
#endif
#if	defined(CONFIG_OP_FREESAT)
	HUINT32						ulTimerId;
	HUINT8						ucIsHomeTP;
#endif
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	HUINT32						ulReStartTimerId;
#endif
	HUINT8						ucDemuxId;
	HUINT32						ulRequestId;


	HUINT32						ulBaseTime;
	svcEpg_ServiceInfo_t		stBaseInfo;
	svcEpg_ServiceInfo_t		stLastCompleteInfo;
} svcEpg_Filter_t;

typedef struct tagEventTimeFilter
{
	UNIXTIME		ulStartTime;
	UNIXTIME		ulEndTime;
} svcEpg_EventTimeFilter_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	SVC_EPG_InitProduct(void);
HERROR	SVC_EPG_InitResource(void);

HERROR	SVC_EPG_RegisterCallback(SvcEpg_EpgKind_e eEpgKind, svcEpg_Callback_t *pstCallback);

#endif /* !__EPG_COMMON_H__ */
