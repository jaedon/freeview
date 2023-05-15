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

#ifndef	__EPG_CONTEXTMGR_H__
#define	__EPG_CONTEXTMGR_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_EPG_TVTV)
#define	EPG_DSMCC_SECTION_BUFFER_MAX_NUM		10
#endif
#define	EPG_FILTER_MAX_NUM						7  // CONTENT_PAT, SDT, PMT, CIT_PMT, CPT_PMT, CIT, CPT
//#define	EPG_SECTION_MAX_VERSION					32


// So many package count can cause semaphore deadlock or message queue full.
//#define RAW_SECTION_PACKAGE_COUNT				5
#define RAW_SECTION_PACKAGE_ACT_COUNT			50
//#define RAW_SECTION_PACKAGE_ACT_HIGH_COUNT		100
#define RAW_SECTION_PACKAGE_OTH_COUNT			50
#define EPG_BUFFER_MAX_NUM						RAW_SECTION_PACKAGE_OTH_COUNT

#define RAW_SECTION_PACKAGE_DURATION			(500) // 500ms

#if defined(CONFIG_MW_EPG_TVTV)
#define	ENTER_CRITICAL_EPGRAW							\
{															\
	VK_SEM_Get(SVC_EPGRAWMGR_GetSemaphoreId());					\
}

#define	LEAVE_CRITICAL_EPGRAW							\
{															\
	VK_SEM_Release(SVC_EPGRAWMGR_GetSemaphoreId());				\
}
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eEPG_CNXT_EMPTY	 					= 0,
	eEPG_CNXT_NOTUSED,
	eEPG_CNXT_USED
} svcEpg_ContextUsage_e;


typedef enum
{
	eEPG_CNXT_STATUS_NONE	 				= 0,
	eEPG_CNXT_STATUS_IDLE,
	eEPG_CNXT_STATUS_RUNNING,
	eEPG_CNXT_STATUS_WAITING
} svcEpg_CnxtStatus_e;

#if 0
typedef enum
{
	EPG_CONTEXT_SOURCE_SI	 				= 0,
	EPG_CONTEXT_SOURCE_DSMCC,
	EPG_CONTEXT_SOURCE_MAX,
} EPG_CONTEXT_SOURCE;
#endif

typedef enum
{
	eEPG_CNXT_FILTER_EMPTY					= 0,
	eEPG_CNXT_FILTER_NOTUSED,
	eEPG_CNXT_FILTER_ONCE,
	eEPG_CNXT_FILTER_CONTINUE,
	eEPG_CNXT_FILTER_MONITOR,
	eEPG_CNXT_FILTER_NEXTCONTINUE

} svcEpg_CnxtFilterState_e;

typedef enum
{
	eEPG_CNXT_EIT_PF_AC,
	eEPG_CNXT_EIT_PF_OTH,
	eEPG_CNXT_EIT_SC_AC,
	eEPG_CNXT_EIT_SC_OTH,
	eEPG_CNXT_EIT_MAX

} svcEpg_ContextEit_e;

typedef enum
{
	eEPG_CNXT_DSMCC_PAT,
	eEPG_CNXT_DSMCC_PMT,
	eEPG_CNXT_DSMCC_DSI,
	eEPG_CNXT_DSMCC_DDB,
	eEPG_CNXT_DSMCC_MAX

} svcEpg_ContextDsmcc_e;

typedef struct tagEpgSectionBuffer
{
	HUINT16		usSectionCount;
	HUINT8		*paucSection[EPG_BUFFER_MAX_NUM];
} svcEpg_RawBuffer_t;

#if	defined(CONFIG_MW_EPG_TVTV)
typedef struct tagTvtvMap
{
	HUINT32			usTvtvId;
	HUINT32			ulSvcUid;	
	DbSvc_TripleId_t	stTripleId;
} svcEpg_TvtvMap_t;

typedef struct tagTvtvEventList
{
	POINTER_LIST_T		*pstEventList;
	POINTER_LIST_T		*pstExtList;
} svcEpg_TvtvEventList_t;


/** ulNumOfPostprocessedFile = ulNumOfDownloadFile */
/** ulNumOfPostprocessedFile = ulNumOfConvertedImageFile + ulNumOfParsedFile */
typedef	struct tagEpgDownloadInfo
{
	HBOOL						bCompleteDownload;

	/** sum of group size */
	HUINT32						ulEstimatedDownloadSize;
	/** sum of downloaded module size */
	HUINT32						ulCurrentDownloadedSize;
	/** sum of downloaded files */
	HUINT32						ulNumOfDownloadedFile;
} svcEpg_DownloadInfo_t;


typedef	struct tagEpgPostprocessInfo
{
	HUINT32						eState;

	HUINT8						ucTaskSlot;

	/** flag of system info files(.tvs, .dkt) */
	HBOOL						bSetTvst;
	HBOOL						bSetDgkt;

	HBOOL						bCompletePostprocess;

	/** sum of postprocessed(decrypted & uncompressed) files */
	HUINT32						ulNumOfPostprocessedFile;

	/** sum of files sended parsing thread (xml files) */
	HUINT32						ulSendPostprocessedFile;

	/** sum of image(JPEG to BMP)) files */
	HUINT32						ulNumOfConvertedImageFile;
} svcEpg_PostProcessInfo_t;


typedef	struct tagEpgXMLParseInfo
{
	HUINT32						eState;

	HUINT8						ucTaskSlot;

	/** flag of service info list file(TSMT,  XXXXFFXX.xml) */
	HBOOL						bSetTsmt;

	HBOOL						bCompleteParse;


	/** sum of files received parsing thread (xml files) */
	HUINT32						ulReceivePostprocessedFile;
} svcEpg_XmlParseInfo_t;
#endif



typedef  struct tagEpgContextLocalData
{
	HUINT32					eState;
	EPG_SpecType_e			eSpec;
 	Handle_t				hAction;
 	Handle_t				hService;
 //	Handle_t					hPreparedService;

 	HUINT32					ulActionId;
	HUINT32					ulDemuxId;
	HUINT8					ucAntId;
	HUINT16					usTsUniqId;
	HUINT32					ulOrgDeliveryType;		// DxDeliveryType_e
	svcEpg_Filter_t			astFilterTable[EPG_FILTER_MAX_NUM];	/**< 각 si table 별 filtering 하는 데 필요한 정보 */

#if	defined(CONFIG_MW_EPG_TVTV)
	DbSvc_TripleId_t			stTvtvTripleId;
	svcEpg_DownloadInfo_t		stDownloadInfo;
	svcEpg_PostProcessInfo_t	stPostprocessInfo;
	svcEpg_XmlParseInfo_t		stXMLParseInfo;
#endif
	DbSvc_Info_t				stCurrentSvcInfo;

} svcEpg_ContextData_t;


typedef HERROR				(* svcEpg_CnxtProcCb_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);

typedef struct  tagEpgContext
{
	HUINT32						ulSemaphore;
	HUINT32						eStatus;
	svcEpg_CnxtProcCb_t			fnContextProc;
	svcEpg_ContextData_t		*pCnxtData;
} svcEpg_Context_t;


typedef struct
{
	HUINT16						usProcessType;		// Key1
	HUINT32 					eStatus;			// Key2
	svcEpg_CnxtProcCb_t 		fnContextProc;
	EPG_SpecType_e				eSpec;
} svcEpg_RawProcessDataInit_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#define ____RAWCONTEXT_FUNC____
HERROR		SVC_EPGCONTEXT_SetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t *pContext);
HERROR		SVC_EPGCONTEXT_StartContext(Handle_t hActon, HUINT16 usProcessType,svcEpg_Msg_t *pstMsg);
HERROR		SVC_EPGCONTEXT_StopContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg);
HERROR		SVC_EPGCONTEXT_GetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t **ppContext);
HERROR		SVC_EPGCONTEXT_DoAction(HUINT32 ulContextCommand, Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg);
HERROR		SVC_EPGCONTEXT_GetEitSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer);
#if defined(CONFIG_MW_EPG_TVTV)
HERROR 	SVC_EPGCONTEXT_GetDsmccSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer);
#endif

HERROR		SVC_EPGCONTEXT_SetTimerOnce(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimerId);
HERROR		SVC_EPGCONTEXT_InitContext(void *ActPfProc,void *OthPfProc,void *ActScProc,void *OthScProc);
HERROR		SVC_EPGCONTEXT_GetContextTaskId(HUINT32 *ulTaskId);
HERROR		SVC_EPGCONTEXT_GetMsgQId(HUINT32 *ulMsgQId);
HERROR 		SVC_EPGCONTEXT_GetEmergencyMsgQId(HUINT32 *ulMsgQId);
HERROR		SVC_EPGCONTEXT_GetSemId(HUINT32 *ulSemId);









#if defined(CONFIG_DEBUG)
extern void SVC_EPGCONTEXT_EnableCheckByteRateNSectionCount(void);
#endif




#endif /* !__EPG_CONTEXTMGR_H__ */
