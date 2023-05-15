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

#ifndef	__DOWNLOAD_MAIN_H__
#define	__DOWNLOAD_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
#else
#include <linkedlist.h>
#endif
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define GROUP_NAME_LEN				64
#define MODULE_NAME_LEN				256

//#define DNLDCNTS_SEM_DEBUG
#ifdef DNLDCNTS_SEM_DEBUG
#define	ENTER_CRITICAL_DNLDCNTS			{ HxLOG_Warning(HxANSI_COLOR_CYAN("ENTER_CRITICAL_DNLDCNTS\n")); svc_download_ContentsEnterCritical(); }
#define LEAVE_CRITICAL_DNLDCNTS			{ svc_download_ContentsLeaveCritical(); HxLOG_Warning(HxANSI_COLOR_CYAN("LEAVE_CRITICAL_DNLDCNTS\n")); }
#else
#define	ENTER_CRITICAL_DNLDCNTS			svc_download_ContentsEnterCritical()
#define LEAVE_CRITICAL_DNLDCNTS			svc_download_ContentsLeaveCritical()
#endif

// Block Address가 BLOCK_SKIP_DOWNLOAD 일 경우에는 Download 하지도 않고 처리도 하지 않는다.
#define BLOCK_NOT_DOWNLOADED			(HUINT8 *)0xFFFFFFFF

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief  SI service에서 사용 될 message structure  */
// 새로운 메시지를 추가할 경우, 그 메시지당 argument의 종류를 여기에 같이 등록시켜야 한다.
typedef enum
{
	eDownArgType_ActionType,
	eDownArgType_SvcHandle,
	eDownArgType_SyncMode,
	eDownArgType_TsUniqId,
	eDownArgType_NetId,
	eDownArgType_TableStatus,
	eDownArgType_FilterId,
	eDownArgType_ExtId1,
	eDownArgType_ExtId2,
	eDownArgType_ExtId3,
	eDownArgType_Pid,
	eDownArgType_SetType,

	eDownArgType_MaxType
} SvcDownload_MsgArgType_e;

// -----------------------------------
// Download :
// -----------------------------------

typedef struct
{
	HUINT16		 usTsUniqId;
	HUINT16		 usPid;
	HUINT32		 ulActType;
	HUINT32		 ulExtId1, ulExtId2, ulExtId3;		// 다른 Specification이 필요할 ???? 위한 Reservation
} SvcDownload_ArgStartDown_t;

typedef struct
{
	HUINT32		 ulSyncMode;
	HUINT16		 usPid;
	HUINT32		 ulExtId1, ulExtId2, ulExtId3;		// PID별 Stop을 지원하기 위한 변수 및 확장변수 추가. ssyoo
} SvcDownload_ArgStopDown_t;

typedef struct
{
	SvcDownload_SetType_e	 eSetType;

	SvcDownload_SetData_t	 stSetData;
	HUINT32		 ulExtId1, ulExtId2, ulExtId3;
} SvcDownload_ArgSet_t;

// -----------------------------------
// PAL SITHD Callback :
// -----------------------------------
typedef struct
{
	HUINT32		 ulTableStatus;
	HUINT32		 ulFilterId;
} SvcDownload_ArgSecCb_t;

typedef struct
{
	HUINT16		 usExtId;
	HUINT16		 usStatus;
	HUINT16		 usTsUniqId;
	HUINT16		 usFilterId;
} SvcDownload_ArgTableCompleteCb_t;

typedef struct
{
	HUINT32		 ulResult;
} SvcDownload_ArgResult_t;

typedef	struct tagDNLDMSG
{
	HUINT32		 ulMsgClass;
	Handle_t	 hActHandle;
	union
	{
		// For Download Msgs:
		SvcDownload_ArgStartDown_t			 stStartDown;
		SvcDownload_ArgStopDown_t			 stStopDown;
		SvcDownload_SetData_t				 stSet;

		// For Section Callback:
		SvcDownload_ArgSecCb_t				 stSection;
		// For Section All-Receiving Callback:
		SvcDownload_ArgTableCompleteCb_t	 stCompTable;
		// For SI execution results : (Search Result, etc)
		SvcDownload_ArgResult_t				 stSiResult;
	} unArg;
} SvcDownload_Msg_t;

// Event를 새로 추가 할 경우에는
// 새로운 메시지용 argument structure를 밑에 만들어서
// DownMsg_t의 union에 추가하도록 해야 한다.


/** @brief  SI service task에서 사용 되는  events  */
// Event를 새로 추가 할 경우에는
// 새로운 메시지용 argument structure를 밑에 만들어서
// DownMsg_t의 union에 추가하도록 해야 한다.
typedef enum
{
	/* Download */
	CMD_DOWN_START,
	CMD_DOWN_STOP,
	CMD_DOWN_SET,

	/* shutdown */
	CMD_DOWN_SHUTDOWN,

	/** Callback 함수에서 보냄 */
	DOWNCB_TABLE_START,		// Not really used, but for confirming that the message comes from the section callback:

	DOWNCB_PAT_TABLE,
	DOWNCB_CAT_TABLE,
	DOWNCB_PMT_TABLE,
	DOWNCB_NIT_TABLE,
	DOWNCB_NIT_OTH_TABLE,
	DOWNCB_SDT_TABLE,
	DOWNCB_SDT_OTH_TABLE,
	DOWNCB_BAT_TABLE,
	DOWNCB_EIT_TABLE,
	DOWNCB_EIT_OTH_TABLE,
	DOWNCB_EITSCH_TABLE,
	DOWNCB_EITSCH_OTH_TABLE,
	DOWNCB_TDT_TABLE,
	DOWNCB_TOT_TABLE,
	DOWNCB_UNT_TABLE,
	DOWNCB_DSI_TABLE,
	DOWNCB_DDB_TABLE,
	DOWNCB_DESC_TABLE,
	DOWNCB_SGT_TABLE,
	DOWNCB_AIT_TABLE,

	DOWNCB_TABLE_END,		// Not really used, but for confirming that the message comes from the section callback:

	// ALL Retrieve 시 필요한 메시지
	DOWNCB_ALL_RECEIVE_START,

	DOWNCB_SDT_ALL_ONE_RECEIVED,
	DOWNCB_SDT_OTH_ALL_ONE_RECEIVED,
	DOWNCB_NIT_ALL_ONE_RECEIVED,
	DOWNCB_NIT_OTH_ALL_ONE_RECEIVED,
	DOWNCB_SGT_ALL_ONE_RECEIVED,

	DOWNCB_ALL_RECEIVE_END,

	DOWNCB_DDB_ZERO_BLOCK,		// OTA Download 시 0번 Block을 받았음을 알려주는 메시지

	DOWNCB_CAROUSEL_TIMEOUT,	// Object Carousel이 Timeout을 겪게 됨.    (HBBTV Object Carousel Spec)

	CMD_DOWN_EndOfCase
} SvcDownload_Message_e;

/** @brief SI error Table */
typedef enum
{
	ERR_DOWN_RESULT_OK = ERR_OK,			/** < 정상 종료. */
	ERR_DOWN_PARAM_NULL,					/** < param으로 넘어 온 데이터를 사용해야 할 때 그 내용이 NULL 인 경우. */
	ERR_DOWN_TARGET_NULL,					/** < param으로 넘어 온 주소에 내용을 채워야 할 때 그 주소가 NULL인 경우. */
	ERR_DOWN_SOURCE_NULL,					/** < 사용해야 할 data를 찾아 봐도 없을 때 */

	ERR_DOWN_SOURCE_INVALID,
	ERR_DOWN_HANDLE_INVALID,
	ERR_DOWN_PARAM_INVALID,				/** < param value가 invalid한 경우. */

	ERR_DOWN_ALREADY_EXIST,
	ERR_DOWN_MEM_ALLOC_FAIL,

	ERR_DOWN_CALLBACK_NULL,

	ERR_DOWN_RESULT_FAIL
} SvcDownload_Error_e;


typedef struct tagDOWN_Download_Data_
{
	Handle_t			 hUpgrade;						// Handle for upgrade module
 	Handle_t			 hAction;						// action Identification
 	SvcDownload_ActionType_e	 eActType;						// Action Type;
	HUINT16				 usPid;							// PID
	HUINT16				 usTsUniqId;
	HUINT32		 ulDemuxId;						// demux id

	// Download Groups
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t			*pstGroupList;					// Group List : : SvcDownload_DnGrp_t
#else
	POINTER_LIST_T		*pstGroupList;					// Group List : : SvcDownload_DnGrp_t
#endif
	// Specific Filtering Information (changable for each spec)
	void				*pvDownloadInfo;

	// For Displaying the progress status:
	HUINT32				 ulRcvBlocks, ulTotalBlocks;
} SvcDownload_DsmccContents_t;

typedef struct
{
	HUINT32		 ulModuleId;							// Module ID
	HUINT32		 ulGroupId;								// 해당되는 Group ID
	HUINT8		 szModuleName[MODULE_NAME_LEN];			// Module Name (Path 포함 등, spec이나 구현 방법에 따라 다름)
	HUINT32		 ulModuleVer;
	HUINT32		 ulModuleSize;

	HUINT8		*pucBuffer;

	HBOOL		 bModuleDownloaded;

	HUINT32		 ulBlockSize;
	HUINT32		 ulBlockNum;
	HUINT32		 ulReceivedBlockNum, ulSkippedBlockNum;
	HUINT8		**ppucBlockArray;
} SvcDownload_DnMod_t;

typedef struct
{
	HUINT32		 ulGroupId;
	HUINT8		 szGroupName[GROUP_NAME_LEN];
	HUINT32		 ulGroupSize;
	HUINT32		 ulBlockSize;

	HBOOL		 bGroupInfoReceived;					// DII를 통해 이 Group의 정확한 Information을 받았는가?
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t			*pstModuleList; 					// Module List :: SvcSi_DnModule_t;
#else
	POINTER_LIST_T	*pstModuleList;						// Module List :: SvcSi_DnModule_t;
#endif	
} SvcDownload_DnGrp_t;


typedef HERROR (* pfnInit_t)(void **ppvAction, void **ppvData, void *pvSiMsg);
typedef HERROR (* pfnTerm_t)(void **ppvAction, void **ppvData);
typedef HERROR (* pfnGetData_t)(void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
typedef HERROR (* pfnFreeData_t)(SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray);
typedef HERROR (* pfnCheckDownloadComplete_t)(HUINT32 ulSignalVersion, HBOOL *pbCompleted);
typedef HERROR (* pfnClearDownloadInfo_t)(void);
typedef HERROR (* pfnInitMetaInfo_t)(void);

typedef struct
{
	pfnInit_t					pfnInit;
	pfnTerm_t					pfnTerm;
	pfnGetData_t				pfnGetData;
	pfnFreeData_t				pfnFreeData;
	pfnCheckDownloadComplete_t	pfnCheckDonwloadComplete;
	pfnClearDownloadInfo_t		pfnClearDownloadInfo;
	pfnInitMetaInfo_t			pfnInitMetaInfo;
} SvcDownload_CB_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

void svc_download_UtilPostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
void svc_download_UtilPostDataToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize);
HERROR	svc_download_UtilRegisterNotifyCallback(PostMsgToMgrCb_t pfnMsgNotify, PostDataToMgrCb_t pfnDataNotify);


void svc_download_ContentsEnterCritical (void);
void svc_download_ContentsLeaveCritical (void);

HERROR svc_download_SendSectionMessage (SvcDownload_Message_e eMsgType, Handle_t hAction, HUINT32 ulTableStatus, HUINT32 ulFilterId);
HERROR svc_download_GetMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);

HERROR svc_download_GetActionHandleFromMsg (SvcDownload_Msg_t *pstMsg, Handle_t *phAction);
HERROR svc_download_GetSectionMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);
HERROR svc_download_GetTableCompleteMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);

HERROR svc_download_GetStartMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);
HERROR svc_download_GetStopMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);

HERROR svc_download_GetContextData (Handle_t hActHandle, void **ppvContextData);

HERROR svc_download_AllocDsmccContents (SvcDownload_DsmccContents_t **ppstDownload);
HERROR svc_download_FreeDsmccContents (SvcDownload_DsmccContents_t *pstDownload);



// = Group Info = //
HERROR SVC_DOWNLOAD_AllocGroupInfo (SvcDownload_DnGrp_t **ppstGroupItem);
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
void SVC_DOWNLOAD_FreeGroupInfo (void *pstGroupItem);
#else
HERROR SVC_DOWNLOAD_FreeGroupInfo (SvcDownload_DnGrp_t *pstGroupItem);
#endif
HERROR SVC_DOWNLOAD_CountGroup (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulGroupNum);
HERROR SVC_DOWNLOAD_GetGroupByGroupId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, SvcDownload_DnGrp_t **ppstGroupItem);
HERROR SVC_DOWNLOAD_CheckAllGroupInfoReceived (SvcDownload_DsmccContents_t *pstDownload);

// = Module Info = //
HERROR SVC_DOWNLOAD_AllocModuleInfo (SvcDownload_DnMod_t **ppstModuleItem);
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
void SVC_DOWNLOAD_FreeModuleInfo (void *pstModuleItem);
#else
HERROR SVC_DOWNLOAD_FreeModuleInfo (SvcDownload_DnMod_t *pstModuleItem);
#endif
HERROR SVC_DOWNLOAD_GetModuleByGroupModuleId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, HUINT32 ulModuleId, SvcDownload_DnMod_t **ppstModuleItem);
HERROR SVC_DOWNLOAD_PrepareModuleInfoToDownload (SvcDownload_DnMod_t *pstModuleItem);
HERROR SVC_DOWNLOAD_PrepareAllModuleInfoToDownload (SvcDownload_DsmccContents_t *pstDownload);
HERROR SVC_DOWNLOAD_CheckModuleInfoDownloaded (SvcDownload_DnMod_t *pstModuleItem);
HERROR SVC_DOWNLOAD_CheckModuleInfoInGroupDownloaded (SvcDownload_DnGrp_t *pstGroupItem);
HERROR SVC_DOWNLOAD_CheckAllModuleInfoDownloaded (SvcDownload_DsmccContents_t *pstDownload);
HERROR SVC_DOWNLOAD_GetDownloadProgress (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulReceivedBlocks, HUINT32 *pulTotalBlocks);

HERROR svc_download_ActionRegister(SvcDownload_ActionType_e eActType, SvcDownload_CB_t *pstCbList);

HERROR svc_download_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);


#endif /* !__DOWNLOAD_MAIN_H__ */
