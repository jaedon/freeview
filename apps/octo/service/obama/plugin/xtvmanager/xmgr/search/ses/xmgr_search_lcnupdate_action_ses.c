/**
	@file     xmgr_search_lcnupdate_action_ses.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <dlib.h>
#include <octo_common.h>

#include <mgr_action.h>
#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_svc.h>
#include <xdb_svc.h>
#include <db_param.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_epg.h>
#if 1		// yjlim (CONFIG_MW_CAS)
#include <svc_cas.h>
#endif

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_antenna.h>
#include <mgr_search.h>
#include <mgr_spec.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <xmgr_search.h>
#include <xmgr_cas.h>
#include <xmgr_module_search.h>
#include "../local_include/_xmgr_search.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#if defined(CONFIG_DEBUG)
#define	LOG							HxLOG_Print
#define	ERR_LOG						HxLOG_Error
#define	MSG_LOG						HxLOG_Message
#else
#define	LOG							while (0) ((int (*)(char *, ...)) 0)
#define	ERR_LOG						while (0) ((int (*)(char *, ...)) 0)
#define MSG_LOG						while (0) ((int (*)(char *, ...)) 0)
#endif

#if defined(CONFIG_DEBUG)
//#define DBG_LCN_STATUS
#define DBG_LCN_STATUS
#endif


#define SEARCH_TIMER_LOCKING_ID						0x2001
#define SEARCH_TIMER_SI_ID							0x2002
#define SEARCH_TIMER_LOCKING_TIME					(6*1000)	/* 6 sec */
#define SEARCH_TIMER_SI_TIME						(60*1000)	/* 60 sec */

#define MAX_BOUQUET_NUM_PER_SVC						8
#define IGNORED_LCNNUM								0xffff

#define LCN_SIMUL_CONDITION_INIT					0x00
#define LCN_SIMUL_CONDITION_SC_INSERTED				0x01
#define LCN_SIMUL_CONDITION_SC_SYSINFO				0x02

#define LCN_SIMUL_CONDITION_READY					(LCN_SIMUL_CONDITION_SC_INSERTED | LCN_SIMUL_CONDITION_SC_SYSINFO)

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
#define ASTRA_PORTAL_BOUQUET_ID		0x11

#define ASTRA_PORTAL_LINKAGE			0x94
#define ASTRA_REGIONAL_PORTAL_LINKAGE	0x95

#define ASTRA_PORTAL_SVC_NUM			0
#define ASTRA_REGIONAL_PORTAL_SVC_NUM	99
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eSesLcnTp_None = 0,
	eSesLcnTp_HomeTp,
	eSesLcnTp_RefNitTp,
	eSesLcnTp_SgtTp,

} SesLcnTpState_t;


typedef enum
{
	eSesLcnState_Start = 0,

	eSesLcnState_HomeTp_Tuning,
	eSesLcnState_HomeTp_WaitingSi,
	eSesLcnState_RefNitTp_Tuning,
	eSesLcnState_RefNitTp_WaitingSi,
	eSesLcnState_SgtTp_Tuning,
	eSesLcnState_SgtTp_WaitingSi,
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	eSesLcnState_AstraPortalRefNitTp_Tuning,	//6
	eSesLcnState_AstraPortalRefNitTp_WaitingSi,
	eSesLcnState_AstraPortalTp_Tuning,
	eSesLcnState_AstraPortalTp_WaitingSi,
#endif
	eSesLcnState_SearchDone,
	eSesLcnState_SearchFail,

	eSesLcnState_Finish,
} SesLcnState_t;

typedef struct
{
	HUINT32			ulTsRefCnt;
	DbSvc_TsInfo_t	stTsInfo;
	POINTER_LIST_T	*pstSvcs;
} SesLcn_TsInfo_t;

typedef struct
{
	HUINT32			 ulBqRefCnt;
	DbSvc_BouqInfo_t	 stBqInfo;
	POINTER_LIST_T	*pstSvcs;
} SesLcn_BouqInfo_t;

typedef struct
{
	HUINT16			 usSvcId;
	DbSvc_TsInfo_t	 stTsInfo;
} SesLcn_SgtTsInfo_t;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)			// TODO
typedef struct
{
	HUINT8		ucLinkType;
	HUINT16		usSvcId;
	DbSvc_TsInfo_t	 stTsInfo;
} Ses_PortalTsInfo_t;

typedef struct
{
	HUINT32			 ulSvcNum;
	DbSvc_Info_t		*pstSvcArray;
} Ses_PortalSvcList_t;

#endif

typedef struct
{
	HUINT16			 usSvcListId;
	HUINT8			 ucVer;
	HUINT8			 szSvcListName[DxLCN_SVC_LIST_NAME_LEN];

	HUINT32			 ulSvcNum;
	DbSvc_Info_t		*pstSvcArray;

	// Bouquet List
	POINTER_LIST_T	*pstBqList;
} SesLcn_SvcList_t;

typedef struct
{
	DbSvc_Info_t		 stSdSvcInfo;
	DbSvc_Info_t		 stHdSvcInfo;
} SesLcn_SimulcastedItem_t;

typedef struct
{
	Handle_t		 	hOldHandle;
	Handle_t		 	hNewHandle;
} SesLcn_TrackingHandle_t;

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagXmgrSearch_lcnupdate_Action_SatContents_t
{
	// Search type
	HUINT32					ulSearchType;

	// Arguments
	HBOOL					 bAllSgtSearch;
	HUINT16					 usSvcListId;
	HUINT8					 ucSvcListVer;
	HUINT16					 usSgtSvcId;

	// Action
	HBOOL					 bActionTaken;
	HUINT32					 ulActionId;
	Handle_t				 hAction;
	HINT32					 nAntId;
	SVC_SatType_t			 eSatType;

	SesLcnState_t			 eState;

	SesHdFuncMode_t			 eFuncMode;				// Function Mode (Normal / Boot / Standby)

	DbSvc_NetInfo_t			 stNetInfo;				// Network Info
	POINTER_LIST_T			*pstHomeTsInfo;			// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstRefNitTsInfo;			// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstSgtTsInfo;			// Contents : DbSvc_TsInfo_t
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	POINTER_LIST_T			*pstAstraPortalRefNitTsInfo;			// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstAstraPortalTsInfo;			// Contents : DbSvc_TsInfo_t
#endif

	POINTER_LIST_T			*pstDeliTsInfo;			// Contents : SesLcn_TsInfo_t

	POINTER_LIST_T			*pstSimulcastList;		// Contents : SesLcn_SimulcastedItem_t

	HUINT32					 ulSvcListNum;
	SesLcn_SvcList_t			*pstLcnSvcListArray;

	// 기존 LCN Service들을 찾기 위한 항목
	HUINT32					 ulPrevLcnSvcNum;
	Handle_t					*phPrevLcnSvcHandles;
	DbSvc_Info_t				*pstPrevLcnSvcInfos;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	Ses_PortalSvcList_t			*pstPortalSvcArray;
#endif

	// Service Insertion , Delete 를 통한 교환의 경우, 삭제는 최후의 순간에 일어나야 TsInfo가 남는다.
	// 지울 Service Handle들을 여기에 남겨둔다.
	POINTER_LIST_T			*pstDeleteSvcHandles;
	// Virtual Service ID는 같지만 triple ID중에 일부라도 변경이 있는 service handle에 대해서 처기하기 위함.
	POINTER_LIST_T			*pstTrackingSvcHandles;


	// For checking the need of the renumbering:
	HUINT32					 ulPrevLastLcnTvNo;
	HUINT32					 ulPrevLastLcnRadioNo;
} XmgrSearch_lcnupdate_Action_Contents_t;


//STATIC XmgrSearch_SatContents_t	s_SatSrchApp_Contents[NUM_SEARCH_ACTION];				/**< Search Appl. runtime context */
static XmgrSearch_lcnupdate_Action_Contents_t		s_stSesLcnAct_Contents;
static HINT32					s_ucSimulConditionReady = LCN_SIMUL_CONDITION_INIT;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HBOOL	s_bIsReceiveSGT = FALSE;
static HBOOL	s_bAddProtalService = FALSE;
#endif

#define	_________________________Body___________________________________

// Local Functions
static  XmgrSearch_lcnupdate_Action_Contents_t*	xmgr_search_lcnupdate_Action_GetContents (void);
static HERROR		xmgr_search_lcnupdate_Action_CheckActionId (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction);
static void			xmgr_search_lcnupdate_Action_IncreaseSessionVersion (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_InitContents (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_LoadVariables (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_DestroyContents (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static void			xmgr_search_lcnupdate_Action_FreeSesLcnTsInfo (void *pvItem);
static HBOOL		xmgr_search_lcnupdate_Action_FilterSesLcnTsInfoSameOnTsId (void *pvItem, void *pvFilter);
static void			xmgr_search_lcnupdate_Action_FreeSesLcnBouquetInfo (void *pvItem);
static void			xmgr_search_lcnupdate_Action_FreeTsInfo (void *pvItem);
static HBOOL		xmgr_search_lcnupdate_Action_FilterTsInfoSameOnTsId (void *pvItem, void *pvFilter);
static HBOOL		xmgr_search_lcnupdate_Action_FilterSgtTsInfoSameTripleId (void *pvItem, void *pvFilter);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HBOOL 		xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameTripleId (void *pvItem, void *pvFilter);
static HBOOL 		xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameSvcId (void *pvItem, void *pvFilter);
#endif
static void			xmgr_search_lcnupdate_Action_FreeSesLcnSimulcastItem (void *pvItem);
static HERROR		xmgr_search_lcnupdate_Action_ClearDeliTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_AppendDeliTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_CountDeliveryTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 *pulNumTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT16 usOnId, HUINT16 usTsId, SesLcn_TsInfo_t **ppstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_ClearHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_AppendHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_ClearRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_AppendRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_ClearSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusSvcId);
static HERROR		xmgr_search_lcnupdate_Action_AppendSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 usSvcId);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HERROR 		xmgr_search_lcnupdate_Action_ProcessAstraPortalRefNitTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_ProcessAstraPortalTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_ClearAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR 		xmgr_search_lcnupdate_Action_AppendAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR 		xmgr_search_lcnupdate_Action_ClearAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_GetAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusSvcId);
static HERROR 		xmgr_search_lcnupdate_Action_AppendAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 usSvcId, HUINT8 ucLinkType);
static HERROR 		xmgr_search_lcnupdate_Action_ClearPortalSvcListArray (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
#endif
static HERROR		xmgr_search_lcnupdate_Action_ClearLcnSvcListArray (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetCurrentTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo);
static HERROR		xmgr_search_lcnupdate_Action_SetToNextTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_ClearBouquetList (SesLcn_SvcList_t *pstSvcList);
static HERROR		xmgr_search_lcnupdate_Action_ClearSimulcastList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_AppendSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo);
static HERROR		xmgr_search_lcnupdate_Action_CountSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 *pulNumSimul);
static HERROR		xmgr_search_lcnupdate_Action_GetSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulIndex, DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo);
static HERROR		xmgr_search_lcnupdate_Action_AppendTrackingHandles (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hOldHandle, Handle_t hNewHandle);
static HBOOL		xmgr_search_lcnupdate_Action_FilterOldTrackingHandle (void *pvItem, void *pvFilter);
static void			xmgr_search_lcnupdate_Action_FreeTrackingHandle (void *pvItem);
static HERROR		xmgr_search_lcnupdate_Action_GetNewTrackingHandle (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hOldHandle, Handle_t *phNewHandle);
static HERROR		xmgr_search_lcnupdate_Action_ClearTrackingHandle (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_GetDeliveryTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetRefNitTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetSgtTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_GetServiceListsFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HERROR 		xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_GetAstraPortalTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_GetAstraPortalSvcInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
#endif

static HERROR		xmgr_search_lcnupdate_Action_ProcessHomeTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_ProcessRefNitTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_ProcessSgtTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_CheckHdSimulcast (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSvcInfo1, DbSvc_Info_t *pstSvcInfo2);
static HERROR		xmgr_search_lcnupdate_Action_AssignSearchSvcToTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
static HERROR		xmgr_search_lcnupdate_Action_GatherSimulcastedInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
static HERROR		xmgr_search_lcnupdate_Action_SaveSearchNetInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_SaveSearchTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
static HERROR		xmgr_search_lcnupdate_Action_SaveSearchBqInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
static HERROR		xmgr_search_lcnupdate_Action_SaveSearchSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
#if defined(CONFIG_DEBUG)
static char 		xmgr_search_lcnupdate_Action_GetSvcTypeChar(DxSvcType_e eSvcType);
static void 		xmgr_search_lcnupdate_Action_PrintProtalSvcInfo(void);
#endif
static HERROR		xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_AddPortalSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
#endif
static HERROR		xmgr_search_lcnupdate_Action_MakeSvcUid(HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HINT32 *ulSvcUid);
static HERROR		xmgr_search_lcnupdate_Action_SendMsgReplaceSvc (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_ReleaseAction (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_TuneSearchTp (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_SaveSearchedData (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static HERROR		xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_UpdateSimulcastedFavorites (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_RecordSimulcastedChannels (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_CheckPreferredListChanged (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex);
static HERROR		xmgr_search_lcnupdate_Action_ReplaceSvcRelatedInfos (Handle_t hOrgSvcHandle, Handle_t hNewSvcHandle);
static HERROR		xmgr_search_lcnupdate_Action_SimulcastFavoriteChannel (DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo);

static HERROR		xmgr_search_lcnupdate_Action_SendSearchStartedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_SendSearchSaveFinishedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_SendSearchActionDestroyed (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static HERROR		xmgr_search_lcnupdate_Action_MakeLastLcnServiceLists (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_UpdateOrAddService (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSvcInfo, Handle_t *phRstSvcHandle);
static HERROR		xmgr_search_lcnupdate_Action_DeleteOldServices (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hSvcHandle);
static HERROR		xmgr_search_lcnupdate_Action_DeleteSvcDeletionList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);
static HERROR 		xmgr_search_lcnupdate_Action_RecoverFavoritesWithTrackingInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents);

static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusCreate (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusDestroyed (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchActionStart (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchSaveResult (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 nSvcListId, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchCancel(XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppBgJobFinished (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusTimer (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 nTimerId, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtChLocked (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtChNoSignal (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtSiSearchDone (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtSiSearchFail (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


#define _________________________________________________________________________________

#if defined(DBG_LCN_STATUS)
HINT8 *xmgr_search_lcnupdate_Action_GetLcnStatusStr(SesLcnState_t eLcnState)
{
	switch(eLcnState)
	{
		ENUM_TO_STR(eSesLcnState_Start);
		ENUM_TO_STR(eSesLcnState_HomeTp_Tuning);
		ENUM_TO_STR(eSesLcnState_HomeTp_WaitingSi);
		ENUM_TO_STR(eSesLcnState_RefNitTp_Tuning);
		ENUM_TO_STR(eSesLcnState_RefNitTp_WaitingSi);
		ENUM_TO_STR(eSesLcnState_SgtTp_Tuning);
		ENUM_TO_STR(eSesLcnState_SgtTp_WaitingSi);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
		ENUM_TO_STR(eSesLcnState_AstraPortalRefNitTp_Tuning);
		ENUM_TO_STR(eSesLcnState_AstraPortalRefNitTp_WaitingSi);
		ENUM_TO_STR(eSesLcnState_AstraPortalTp_Tuning);
		ENUM_TO_STR(eSesLcnState_AstraPortalTp_WaitingSi);
#endif
		ENUM_TO_STR(eSesLcnState_SearchDone);
		ENUM_TO_STR(eSesLcnState_SearchFail);
		ENUM_TO_STR(eSesLcnState_Finish);
	}

	return "UNKNOWN LCN Status";
}
#endif

static XmgrSearch_lcnupdate_Action_Contents_t *xmgr_search_lcnupdate_Action_GetContents (void)
{
	return &s_stSesLcnAct_Contents;
}

static HERROR xmgr_search_lcnupdate_Action_CheckActionId (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);
	if (pstContents->ulActionId != ulActionId)
	{
		MSG_LOG ("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
		return MESSAGE_PASS;
	}

	return ERR_BUS_NO_ERROR;
}

static void xmgr_search_lcnupdate_Action_IncreaseSessionVersion (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}


#define	_____CONTENTS_INITIATION_____

static HERROR xmgr_search_lcnupdate_Action_InitContents (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32				nSvcNo;
	HUINT32				ulResult;
	HERROR				hErr;

	// 1. Clear all the contents:
	hErr = XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb(&ulResult);
	if ((hErr == ERR_OK) && (ulResult > 1))
	{
		// Coldboot시 LCN Update 결과가 존재하는 경우 바로 search finished 메세지를 보내도록 함
		pstContents->eState = eSesLcnState_SearchDone;
		XMGR_SEARCH_INFO_SetSesLcnColdBootResultToDb(0);
	}
	else
	{
		HxSTD_memset (pstContents, 0, sizeof(XmgrSearch_lcnupdate_Action_Contents_t));
		pstContents->eState = eSesLcnState_Start;
	}

	DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_NUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACESVC_NUM, 0, 0);

	// 2. Load the arguments
	hErr = xmgr_search_lcnupdate_Action_LoadVariables (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSesLcnState_SearchFail;
		return ERR_OK;
	}

	// 3. 현재 상태에서 LCN Service의 최대 번호를 읽어 온다.
	hErr = DB_SVC_GetMaxLcnInLcnServices (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, &nSvcNo);
	pstContents->ulPrevLastLcnTvNo = (hErr == ERR_OK) ? (HUINT32)nSvcNo : 0;

	hErr = DB_SVC_GetMaxLcnInLcnServices (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_RADIO, &nSvcNo);
	pstContents->ulPrevLastLcnRadioNo = (hErr == ERR_OK) ? (HUINT32)nSvcNo : 0;

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_LoadVariables (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32					ulAntIdx;
	HUINT32					ulTpCnt, ulTpNum;
	HUINT32					ulNetNum;
	DbSvc_TsInfo_t			stTsInfo;
	DbSvc_TripleId_t		stTripleId;
	DbSvc_NetInfo_t			stNetInfo;
	DbSvc_AntInfo_t		stAntInfo;
	HERROR					hErr;

	hErr = XMGR_SEARCH_INFO_GetSesHdFuncMode (&(pstContents->eFuncMode));
	if (hErr != ERR_OK)
	{
		pstContents->eFuncMode = eSesHdFuncMode_Normal;
	}

	// 1. Antenna ID : ASTRA1 을 얻어 와야 한다.
	hErr = XMGR_SEARCH_INFO_GetAstraAntIdx (&ulAntIdx);
	if (hErr != ERR_OK)					{ return ERR_FAIL; }

	pstContents->nAntId = (HINT32)ulAntIdx;
	hErr = DB_SVC_GetAntInfo ((HUINT16)ulAntIdx, &stAntInfo);
	pstContents->eSatType = (hErr == ERR_OK) ? stAntInfo.eSatType : eSatType_ASTRA_1;

	// 2. Home TP (Anchor TP)
	ulTpNum = XMGR_SEARCH_INFO_CountAnchorTsInfo();
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAnchorTsInfo (ulTpCnt, &stTsInfo);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_search_lcnupdate_Action_AppendHomeTsInfo (pstContents, &stTsInfo);
			}
		}
	}

	// 3. Reference TP
	ulTpNum = XMGR_SEARCH_INFO_CountAstraReferenceTsInfo();
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAstraReferenceTsInfo (ulTpCnt, &stTsInfo);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_search_lcnupdate_Action_AppendRefNitTsInfo (pstContents, &stTsInfo);
			}
		}
	}

	// 4. SGT TP
	ulTpNum = XMGR_SEARCH_INFO_CountAstraSgtTsInfo();
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAstraSgtTsInfo (ulTpCnt, &stTsInfo, &stTripleId);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_search_lcnupdate_Action_AppendSgtTsInfo (pstContents, &stTsInfo, stTripleId.usSvcId);
			}
		}
	}
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	// 4.1 Astra Portal  Reference TP
	ulTpNum = XMGR_SEARCH_INFO_CountAstraPortalReferenceTsInfo();
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAstraPortalReferenceTsInfo (ulTpCnt, &stTsInfo);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_search_lcnupdate_Action_AppendAstraPortalRefNitTsInfo (pstContents, &stTsInfo);
			}
		}
	}
#endif

	// 5. Delivery Data && Network Info
	ulNetNum = XMGR_SEARCH_INFO_CountNetworkInfo();
	ulTpNum = XMGR_SEARCH_INFO_CountDeliveryTsInfo();

	if (ulNetNum > 0 && ulTpNum > 0)
	{
		hErr = XMGR_SEARCH_INFO_GetNetworkInfo (0, &stNetInfo);
		if (hErr == ERR_OK)
		{
			// Copy Network Info to the contents
			pstContents->stNetInfo = stNetInfo;
			pstContents->stNetInfo.eOrgDeliType = eDxDELIVERY_TYPE_SAT;

			// Get and copy the Delivery TS Info
			for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
			{
				hErr = XMGR_SEARCH_INFO_GetDeliveryTsInfo (ulTpCnt, &stTsInfo);
				if (hErr == ERR_OK)
				{
					if (stTsInfo.eDeliType == eDxDELIVERY_TYPE_SAT)
					{
						xmgr_search_lcnupdate_Action_AppendDeliTsInfo (pstContents, &stTsInfo);
					}
				}
			}
		}
	}

	// Arguments :
	hErr = XMGR_SEARCH_INFO_GetSesLcnAllSearchFlag (&(pstContents->bAllSgtSearch));
	if (hErr != ERR_OK)
	{
		pstContents->bAllSgtSearch = TRUE;
	}

	hErr = XMGR_SEARCH_INFO_GetSesLcnServiceListIdFromDb (&(pstContents->usSvcListId));
	if (hErr != ERR_OK)
	{
		pstContents->usSvcListId = 0;
	}

	hErr = XMGR_SEARCH_INFO_GetSesLcnServiceListVerFromDb (&(pstContents->ucSvcListVer));
	if (hErr != ERR_OK)
	{
		pstContents->ucSvcListVer = 0;
	}

	return ERR_OK;
}


#define	_____CONTENTS_MODIFICATION_____



#define	_____CONTENTS_DESTRUCTION_____

static HERROR xmgr_search_lcnupdate_Action_DestroyContents (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	xmgr_search_lcnupdate_Action_ClearDeliTsInfo (pstContents);
	xmgr_search_lcnupdate_Action_ClearHomeTsInfo (pstContents);
	xmgr_search_lcnupdate_Action_ClearRefNitTsInfo (pstContents);
	xmgr_search_lcnupdate_Action_ClearSgtTsInfo (pstContents);
	xmgr_search_lcnupdate_Action_ClearLcnSvcListArray (pstContents);
	xmgr_search_lcnupdate_Action_ClearSimulcastList (pstContents);
	xmgr_search_lcnupdate_Action_ClearTrackingHandle (pstContents);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	xmgr_search_lcnupdate_Action_ClearAstraPortalRefNitTsInfo(pstContents);
	xmgr_search_lcnupdate_Action_ClearAstraPortalTsInfo(pstContents);
	xmgr_search_lcnupdate_Action_ClearPortalSvcListArray(pstContents);
#endif

	DB_SVC_FreeServiceInfoList (NULL, pstContents->phPrevLcnSvcHandles, pstContents->pstPrevLcnSvcInfos);

	HxSTD_memset (pstContents, 0, sizeof(XmgrSearch_lcnupdate_Action_Contents_t));

	return ERR_BUS_NO_ERROR;
}


#define	_____DATA_CONVERSION_____


#define _____CONTENTS_INFOS_MANAGEMENT_____

static void xmgr_search_lcnupdate_Action_FreeSesLcnTsInfo (void *pvItem)
{
	SesLcn_TsInfo_t		*pstItem;

	pstItem = (SesLcn_TsInfo_t *)pvItem;
	if (pstItem != NULL)
	{
		if (pstItem->pstSvcs)
		{
			pstItem->pstSvcs = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstItem->pstSvcs, NULL);
		}

		OxAP_Free (pstItem);
	}
}

static HBOOL xmgr_search_lcnupdate_Action_FilterSesLcnTsInfoSameOnTsId (void *pvItem, void *pvFilter)
{
	SesLcn_TsInfo_t		*pstItem, *pstFilter;

	pstItem = (SesLcn_TsInfo_t *)pvItem;
	pstFilter = (SesLcn_TsInfo_t *)pvFilter;

	if ((pstItem->stTsInfo.eDeliType == pstFilter->stTsInfo.eDeliType) &&
		(pstItem->stTsInfo.usOnId == pstFilter->stTsInfo.usOnId) &&
		(pstItem->stTsInfo.usTsId == pstFilter->stTsInfo.usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

static void xmgr_search_lcnupdate_Action_FreeSesLcnBouquetInfo (void *pvItem)
{
	SesLcn_BouqInfo_t		*pstItem;

	pstItem = (SesLcn_BouqInfo_t *)pvItem;
	if (pstItem != NULL)
	{
		if (pstItem->pstSvcs)
		{
			pstItem->pstSvcs = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstItem->pstSvcs, NULL);
		}

		OxAP_Free (pstItem);
	}
}

static void xmgr_search_lcnupdate_Action_FreeTsInfo (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static HBOOL xmgr_search_lcnupdate_Action_FilterTsInfoSameOnTsId (void *pvItem, void *pvFilter)
{
	DbSvc_TsInfo_t		*pstItem, *pstFilter;

	pstItem = (DbSvc_TsInfo_t *)pvItem;
	pstFilter = (DbSvc_TsInfo_t *)pvFilter;

	if ((pstItem->eDeliType == pstFilter->eDeliType) &&
		(pstItem->usOnId == pstFilter->usOnId) &&
		(pstItem->usTsId == pstFilter->usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL xmgr_search_lcnupdate_Action_FilterSgtTsInfoSameTripleId (void *pvItem, void *pvFilter)
{
	SesLcn_SgtTsInfo_t		*pstItem, *pstFilter;

	pstItem = (SesLcn_SgtTsInfo_t *)pvItem;
	pstFilter = (SesLcn_SgtTsInfo_t *)pvFilter;

	if ((pstItem->usSvcId == pstFilter->usSvcId) &&
		(pstItem->stTsInfo.usOnId == pstFilter->stTsInfo.usOnId) &&
		(pstItem->stTsInfo.usTsId == pstFilter->stTsInfo.usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HBOOL xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameTripleId (void *pvItem, void *pvFilter)
{
	Ses_PortalTsInfo_t		*pstItem, *pstFilter;

	pstItem = (Ses_PortalTsInfo_t *)pvItem;
	pstFilter = (Ses_PortalTsInfo_t *)pvFilter;

	if ((pstItem->usSvcId == pstFilter->usSvcId) &&
		(pstItem->stTsInfo.usOnId == pstFilter->stTsInfo.usOnId) &&
		(pstItem->stTsInfo.usTsId == pstFilter->stTsInfo.usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameSvcId (void *pvItem, void *pvFilter)
{
	Ses_PortalTsInfo_t		*pstItem;
	HUINT16 				*pusSvcId ;

	pstItem = (Ses_PortalTsInfo_t *)pvItem;
	pusSvcId = (HUINT16 *)pvFilter;

	if (pstItem->usSvcId == *pusSvcId)
	{
		return TRUE;
	}

	return FALSE;
}

#endif

static void xmgr_search_lcnupdate_Action_FreeSesLcnSimulcastItem (void *pvItem)
{
	SesLcn_SimulcastedItem_t	*pstItem;

	pstItem = (SesLcn_SimulcastedItem_t *)pvItem;
	if (pstItem != NULL)
	{
		OxAP_Free (pstItem);
	}
}


// Delivery TS Infos
static HERROR xmgr_search_lcnupdate_Action_ClearDeliTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstDeliTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstDeliTsInfo, xmgr_search_lcnupdate_Action_FreeSesLcnTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_AppendDeliTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	SesLcn_TsInfo_t		 stFilterTsInfo;
	SesLcn_TsInfo_t		*pstDeliTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	HxSTD_memset (&stFilterTsInfo, 0, sizeof(SesLcn_TsInfo_t));

	stFilterTsInfo.stTsInfo = *pstTsInfo;
	stFilterTsInfo.stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stFilterTsInfo.stTsInfo.ucTunerId = (HUINT8)-1;
	stFilterTsInfo.stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)pstContents->nAntId;
	stFilterTsInfo.stTsInfo.stTuningParam.sat.eSatType = pstContents->eSatType;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstDeliTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterSesLcnTsInfoSameOnTsId,
															(void *)&stFilterTsInfo);
	if (pstItem != NULL)
	{
		pstDeliTsInfo = (SesLcn_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstDeliTsInfo != NULL)
		{
			pstDeliTsInfo->stTsInfo = stFilterTsInfo.stTsInfo;
		}
	}
	else
	{
		pstDeliTsInfo = (SesLcn_TsInfo_t *)OxAP_Malloc (sizeof(SesLcn_TsInfo_t));
		if (pstDeliTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		*pstDeliTsInfo = stFilterTsInfo;
		pstContents->pstDeliTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstDeliTsInfo,
																				(void *)pstDeliTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_CountDeliveryTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 *pulNumTsInfo)
{
	if (pulNumTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	*pulNumTsInfo = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstDeliTsInfo, NULL, NULL);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT16 usOnId, HUINT16 usTsId, SesLcn_TsInfo_t **ppstTsInfo)
{
	SesLcn_TsInfo_t		 stFilterTsInfo;
	SesLcn_TsInfo_t		*pstDeliTsInfo;
	POINTER_LIST_T		*pstItem;

	if (ppstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	stFilterTsInfo.stTsInfo.eDeliType	= eDxDELIVERY_TYPE_SAT;
	stFilterTsInfo.stTsInfo.usOnId		= usOnId;
	stFilterTsInfo.stTsInfo.usTsId		= usTsId;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstDeliTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterSesLcnTsInfoSameOnTsId,
															(void *)&stFilterTsInfo);
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}

	pstDeliTsInfo = (SesLcn_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
	if (pstDeliTsInfo == NULL)
	{
		return ERR_FAIL;
	}

	*ppstTsInfo = pstDeliTsInfo;
	return ERR_OK;
}


// Home TS Info
static HERROR xmgr_search_lcnupdate_Action_ClearHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstHomeTsInfo != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstHomeTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_AppendHomeTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstHomeTsInfo;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Home TS는 겹치지 않고 들어 왔다고 가정한다. (Hardcoding 된 정보)

	pstHomeTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstHomeTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

	*pstHomeTsInfo = *pstTsInfo;
	pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTsInfo,
																			(void *)pstHomeTsInfo);

	return ERR_OK;
}

// Reference NIT TS Info
static HERROR xmgr_search_lcnupdate_Action_ClearRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstRefNitTsInfo != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstRefNitTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_AppendRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstRefNitTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstRefNitTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterTsInfoSameOnTsId,
															(void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstRefNitTsInfo != NULL)
		{
			*pstRefNitTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstRefNitTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		*pstRefNitTsInfo = *pstTsInfo;
		pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstRefNitTsInfo,
																				(void *)pstRefNitTsInfo);
	}

	return ERR_OK;
}

// SGT TS Info
static HERROR xmgr_search_lcnupdate_Action_ClearSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstSgtTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSgtTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusSvcId)
{
	SesLcn_SgtTsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL || pusSvcId == NULL)				{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstSgtTsInfo != NULL)
	{
		pstSrcTsInfo = (SesLcn_SgtTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstSgtTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = pstSrcTsInfo->stTsInfo;
			*pusSvcId  = pstSrcTsInfo->usSvcId;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_AppendSgtTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 usSvcId)
{
	SesLcn_SgtTsInfo_t	 stSgtTsInfo;
	SesLcn_SgtTsInfo_t	*pstSgtTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	stSgtTsInfo.stTsInfo = *pstTsInfo;
	stSgtTsInfo.usSvcId  = usSvcId;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSgtTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterSgtTsInfoSameTripleId,
															(void *)&stSgtTsInfo);
	if (pstItem != NULL)
	{
		pstSgtTsInfo = (SesLcn_SgtTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSgtTsInfo != NULL)
		{
			pstSgtTsInfo->stTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstSgtTsInfo = (SesLcn_SgtTsInfo_t *)OxAP_Malloc (sizeof(SesLcn_SgtTsInfo_t));
		if (pstSgtTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		pstSgtTsInfo->stTsInfo = *pstTsInfo;
		pstSgtTsInfo->usSvcId  = usSvcId;
		pstContents->pstSgtTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSgtTsInfo,
																				(void *)pstSgtTsInfo);
	}

	return ERR_OK;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HERROR xmgr_search_lcnupdate_Action_ClearAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstAstraPortalRefNitTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstAstraPortalRefNitTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_FAIL; }

	if (pstContents->pstAstraPortalRefNitTsInfo != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstAstraPortalRefNitTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_AppendAstraPortalRefNitTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstRefNitTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstAstraPortalRefNitTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterTsInfoSameOnTsId,
															(void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstRefNitTsInfo != NULL)
		{
			*pstRefNitTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstRefNitTsInfo == NULL)			{ return ERR_FAIL; }

		*pstRefNitTsInfo = *pstTsInfo;
		pstContents->pstAstraPortalRefNitTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstAstraPortalRefNitTsInfo,
																				(void *)pstRefNitTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_ClearAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstAstraPortalTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstAstraPortalTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusSvcId)
{
	Ses_PortalTsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL || pusSvcId == NULL)				{ return ERR_FAIL; }

	if (pstContents->pstAstraPortalTsInfo != NULL)
	{
		pstSrcTsInfo = (Ses_PortalTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstAstraPortalTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = pstSrcTsInfo->stTsInfo;
			*pusSvcId  = pstSrcTsInfo->usSvcId;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_AppendAstraPortalTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 usSvcId, HUINT8 ucLinkType)
{
	Ses_PortalTsInfo_t	 stAstraPortalTsInfo;
	Ses_PortalTsInfo_t	*pstAstraPortalTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	stAstraPortalTsInfo.stTsInfo = *pstTsInfo;
	stAstraPortalTsInfo.usSvcId  = usSvcId;
	stAstraPortalTsInfo.ucLinkType = ucLinkType;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstAstraPortalTsInfo, 0,
															xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameTripleId,
															(void *)&stAstraPortalTsInfo);
	if (pstItem != NULL)
	{
		pstAstraPortalTsInfo = (Ses_PortalTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstAstraPortalTsInfo != NULL)
		{
			pstAstraPortalTsInfo->stTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstAstraPortalTsInfo = (Ses_PortalTsInfo_t *)OxAP_Malloc (sizeof(Ses_PortalTsInfo_t));
		if (pstAstraPortalTsInfo == NULL)			{ return ERR_FAIL; }

		pstAstraPortalTsInfo->stTsInfo = *pstTsInfo;
		pstAstraPortalTsInfo->usSvcId  = usSvcId;
		pstAstraPortalTsInfo->ucLinkType = ucLinkType;

		pstContents->pstAstraPortalTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstAstraPortalTsInfo,
																				(void *)pstAstraPortalTsInfo);
	}

	return ERR_OK;
}

// Ses_PortalSvcList_t
static HERROR xmgr_search_lcnupdate_Action_ClearPortalSvcListArray (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	Ses_PortalSvcList_t		*pstProtalSvcList;

	if (pstContents->pstPortalSvcArray!= NULL)
	{
		pstProtalSvcList = pstContents->pstPortalSvcArray;

		// Service Info
		if (pstProtalSvcList->pstSvcArray != NULL)
		{
			OxAP_Free (pstProtalSvcList->pstSvcArray);
		}

		OxAP_Free (pstContents->pstPortalSvcArray);
		pstContents->pstPortalSvcArray = NULL;
	}

	return ERR_OK;

}

#endif

// SesLcn_SvcList_t
static HERROR xmgr_search_lcnupdate_Action_ClearLcnSvcListArray (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32				 ulListIdx;
	SesLcn_SvcList_t	*pstSvcList;

	if (pstContents->pstLcnSvcListArray != NULL)
	{
		for (ulListIdx = 0; ulListIdx < pstContents->ulSvcListNum; ulListIdx++)
		{
			pstSvcList = &(pstContents->pstLcnSvcListArray[ulListIdx]);

			// Service Info
			if (pstSvcList->pstSvcArray != NULL)
			{
				OxAP_Free (pstSvcList->pstSvcArray);
			}

			// Bouquet Info
			xmgr_search_lcnupdate_Action_ClearBouquetList (pstSvcList);
		}

		OxAP_Free (pstContents->pstLcnSvcListArray);
		pstContents->pstLcnSvcListArray = NULL;
	}

	return ERR_OK;

}


// EXT
static HERROR xmgr_search_lcnupdate_Action_GetCurrentTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT16			 usSvcId;

	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_GetCurrentTsInfo] State (%s) \n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_Tuning:
	case eSesLcnState_HomeTp_WaitingSi:
		return xmgr_search_lcnupdate_Action_GetHomeTsInfo (pstContents, pstTsInfo);

	case eSesLcnState_RefNitTp_Tuning:
	case eSesLcnState_RefNitTp_WaitingSi:
		return xmgr_search_lcnupdate_Action_GetRefNitTsInfo (pstContents, pstTsInfo);

	case eSesLcnState_SgtTp_Tuning:
	case eSesLcnState_SgtTp_WaitingSi:
		return xmgr_search_lcnupdate_Action_GetSgtTsInfo (pstContents, pstTsInfo, &usSvcId);

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSesLcnState_AstraPortalRefNitTp_Tuning:
	case eSesLcnState_AstraPortalRefNitTp_WaitingSi:
		return xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfo (pstContents, pstTsInfo);

	case eSesLcnState_AstraPortalTp_Tuning:
	case eSesLcnState_AstraPortalTp_WaitingSi:
		return xmgr_search_lcnupdate_Action_GetAstraPortalTsInfo (pstContents, pstTsInfo, &usSvcId);
#endif

	default:
		break;
	}

	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_SetToNextTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_SetToNextTsInfo] State (%s) \n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_Tuning:
	case eSesLcnState_HomeTp_WaitingSi:
		pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTsInfo, pstContents->pstHomeTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
		if (pstContents->pstHomeTsInfo == NULL)
		{
			return ERR_FAIL;
		}

		return ERR_OK;

	case eSesLcnState_RefNitTp_Tuning:
	case eSesLcnState_RefNitTp_WaitingSi:
		pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstRefNitTsInfo, pstContents->pstRefNitTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
		if (pstContents->pstRefNitTsInfo == NULL)
		{
			pstContents->eState = eSesLcnState_HomeTp_Tuning;
			return xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		}

		return ERR_OK;

	case eSesLcnState_SgtTp_Tuning:
	case eSesLcnState_SgtTp_WaitingSi:
		pstContents->pstSgtTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSgtTsInfo, pstContents->pstSgtTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
		if (pstContents->pstSgtTsInfo == NULL)
		{
#if defined(CONFIG_OP_SES_ASTRA_PORTAL) // temp 임..
			pstContents->eState = eSesLcnState_AstraPortalRefNitTp_Tuning;
#else
			pstContents->eState = eSesLcnState_RefNitTp_Tuning;
#endif
			return xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		}

		return ERR_OK;
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSesLcnState_AstraPortalRefNitTp_Tuning:
	case eSesLcnState_AstraPortalRefNitTp_WaitingSi:
		pstContents->pstAstraPortalRefNitTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstAstraPortalRefNitTsInfo, pstContents->pstAstraPortalRefNitTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
		if (pstContents->pstAstraPortalRefNitTsInfo == NULL)
		{
			return ERR_FAIL;
		}

		return ERR_OK;

	case eSesLcnState_AstraPortalTp_Tuning:
	case eSesLcnState_AstraPortalTp_WaitingSi:
		pstContents->pstAstraPortalTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstAstraPortalTsInfo, pstContents->pstAstraPortalTsInfo, xmgr_search_lcnupdate_Action_FreeTsInfo);
		if (pstContents->pstAstraPortalTsInfo == NULL)
		{
			pstContents->eState = eSesLcnState_AstraPortalRefNitTp_Tuning;

			return xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		}
		return ERR_OK;

#endif

	default:
		break;
	}

	return ERR_FAIL;
}

// Bouquet List In Service List
static HERROR xmgr_search_lcnupdate_Action_ClearBouquetList (SesLcn_SvcList_t *pstSvcList)
{
	pstSvcList->pstBqList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSvcList->pstBqList, xmgr_search_lcnupdate_Action_FreeSesLcnBouquetInfo);
	return ERR_OK;
}

// HD Simulcast service List
static HERROR xmgr_search_lcnupdate_Action_ClearSimulcastList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstSimulcastList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSimulcastList, xmgr_search_lcnupdate_Action_FreeSesLcnSimulcastItem);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_AppendSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo)
{
	SesLcn_SimulcastedItem_t	*pstItem;

	if (pstSdSvcInfo == NULL || pstHdSvcInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	pstItem = (SesLcn_SimulcastedItem_t *)OxAP_Malloc (sizeof(SesLcn_SimulcastedItem_t));
	if (pstItem == NULL)					{ return ERR_BUS_OUT_OF_MEMORY; }

	pstItem->stSdSvcInfo = *pstSdSvcInfo;
	pstItem->stHdSvcInfo = *pstHdSvcInfo;

	pstContents->pstSimulcastList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSimulcastList,
																				(void *)pstItem);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_CountSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 *pulNumSimul)
{
	if (pulNumSimul == NULL)				{ return ERR_BUS_TARGET_NULL; }

	*pulNumSimul = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstSimulcastList, NULL, NULL);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetSimulcastItem (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulIndex, DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo)
{
	POINTER_LIST_T				*pstItem;
	SesLcn_SimulcastedItem_t	*pstSimulItem;

	if (pstSdSvcInfo == NULL || pstHdSvcInfo == NULL)
	{
		return ERR_FAIL;
	}

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSimulcastList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem == NULL)			{ return ERR_FAIL; }

	pstSimulItem = (SesLcn_SimulcastedItem_t *)UTIL_LINKEDLIST_GetContents (pstItem);
	if (pstSimulItem == NULL)		{ return ERR_FAIL; }

	*pstSdSvcInfo = pstSimulItem->stSdSvcInfo;
	*pstHdSvcInfo = pstSimulItem->stHdSvcInfo;

	return ERR_OK;
}


static HERROR xmgr_search_lcnupdate_Action_AppendTrackingHandles (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hOldHandle, Handle_t hNewHandle)
{
	SesLcn_TrackingHandle_t		*pstItem;


 	pstItem = (SesLcn_TrackingHandle_t *)OxAP_Malloc (sizeof(SesLcn_TrackingHandle_t));
	if (pstItem == NULL)					{ return ERR_BUS_OUT_OF_MEMORY; }

	pstItem->hOldHandle = hOldHandle;
	pstItem->hNewHandle = hNewHandle;

	pstContents->pstTrackingSvcHandles = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTrackingSvcHandles, (void *)pstItem);

	return ERR_OK;
}


static HBOOL xmgr_search_lcnupdate_Action_FilterOldTrackingHandle (void *pvItem, void *pvFilter)
{
	SesLcn_TrackingHandle_t		*pstItem, *pstFilter;

	pstItem = (SesLcn_TrackingHandle_t *)pvItem;
	pstFilter = (SesLcn_TrackingHandle_t *)pvFilter;

	if (pstItem->hOldHandle == pstFilter->hOldHandle)
	{
		return TRUE;
	}

	return FALSE;
}

static void xmgr_search_lcnupdate_Action_FreeTrackingHandle (void *pvItem)
{
	SesLcn_TrackingHandle_t	*pstItem;

	pstItem = (SesLcn_TrackingHandle_t *)pvItem;
	if (pstItem != NULL)
	{
		OxAP_Free (pstItem);
	}
}

static HERROR xmgr_search_lcnupdate_Action_GetNewTrackingHandle (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hOldHandle, Handle_t *phNewHandle)
{
	SesLcn_TrackingHandle_t		 stFilterTrackingHandle;
	SesLcn_TrackingHandle_t		*pstTrackingHandle = NULL;
	POINTER_LIST_T				*pstItem = NULL;
	HUINT16						usIdx;

	if (phNewHandle == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	*phNewHandle = HANDLE_NULL;

	stFilterTrackingHandle.hOldHandle = hOldHandle;
	stFilterTrackingHandle.hNewHandle = HANDLE_NULL;

	pstItem = UTIL_LINKEDLIST_FindListItemFromPointerList (pstContents->pstTrackingSvcHandles, &usIdx, xmgr_search_lcnupdate_Action_FilterOldTrackingHandle, (void *)&stFilterTrackingHandle);
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}

	pstTrackingHandle = (SesLcn_TrackingHandle_t *)UTIL_LINKEDLIST_GetContents (pstItem);
	if (pstTrackingHandle == NULL)
	{
		return ERR_FAIL;
	}

	*phNewHandle = pstTrackingHandle->hNewHandle;

	return ERR_OK;
}


static HERROR xmgr_search_lcnupdate_Action_ClearTrackingHandle (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	pstContents->pstTrackingSvcHandles = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTrackingSvcHandles, xmgr_search_lcnupdate_Action_FreeTrackingHandle);
	return ERR_OK;
}


#define	_____SI_DATA_____

static HERROR xmgr_search_lcnupdate_Action_GetDeliveryTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32						ulTsCnt, ulTsNum;
	HUINT32						ulNetNum;
	DbSvc_TsInfo_t				*pstTsArray;
	DbSvc_NetInfo_t				*pstNetInfo;
	HERROR			 			hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	// Get the Network Info
	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_NETWORK_NETINFO, &ulNetNum, (void **)&pstNetInfo);
	if (hErr != ERR_OK || ulNetNum == 0)
	{
		return ERR_FAIL;
	}

	pstContents->stNetInfo = pstNetInfo[0];
	pstContents->stNetInfo.eOrgDeliType = eDxDELIVERY_TYPE_SAT;

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_NETWORK_NETINFO, (void *)pstNetInfo);

	// Get the Delivery TS Info
	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO, &ulTsNum, (void **)&pstTsArray);
	if (hErr != ERR_OK || ulTsNum == 0)
	{
		return ERR_FAIL;
	}

	for (ulTsCnt = 0; ulTsCnt < ulTsNum; ulTsCnt++)
	{
		if (pstTsArray[ulTsCnt].eDeliType == eDxDELIVERY_TYPE_SAT)
		{
			xmgr_search_lcnupdate_Action_AppendDeliTsInfo (pstContents, &(pstTsArray[ulTsCnt]));
		}
	}

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO, (void *)pstTsArray);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetRefNitTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HBOOL			 			bNitTsInfoFound;
	HUINT32			 			ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t			*pstTripleIdArray;
	SesLcn_TsInfo_t				*pstTsInfo;
	HERROR			 			hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr != ERR_OK || ulLinkNum == 0)
	{
		return ERR_FAIL;
	}

	bNitTsInfoFound = FALSE;

	for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
													pstTripleIdArray[ulLinkCnt].usOnId,
													pstTripleIdArray[ulLinkCnt].usTsId,
													&pstTsInfo);
		if (hErr == ERR_OK)
		{
			bNitTsInfoFound = TRUE;
			xmgr_search_lcnupdate_Action_AppendRefNitTsInfo (pstContents, &(pstTsInfo->stTsInfo));
		}
	}

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID, (void *)pstTripleIdArray);

	return (bNitTsInfoFound == TRUE) ? ERR_OK : ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_GetSgtTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HBOOL						 bNitTsInfoFound;
	HUINT32						 ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t				*pstTripleIdArray;
	SesLcn_TsInfo_t				*pstTsInfo;
	HERROR						 hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SGTLINK_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr != ERR_OK || ulLinkNum == 0)
	{
		MSG_LOG ("hErr(%d) , ulLinkNum (%d) \n", hErr, ulLinkNum);
		return ERR_FAIL;
	}

	bNitTsInfoFound = FALSE;

	for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
													pstTripleIdArray[ulLinkCnt].usOnId,
													pstTripleIdArray[ulLinkCnt].usTsId,
													&pstTsInfo);
		if (hErr == ERR_OK)
		{
			bNitTsInfoFound = TRUE;

			xmgr_search_lcnupdate_Action_AppendSgtTsInfo (pstContents, &(pstTsInfo->stTsInfo), pstTripleIdArray[ulLinkCnt].usSvcId);
			break;
		}
	}

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SGTLINK_TRIPLEID, (void *)pstTripleIdArray);

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfoFromSi(pstContents);
#endif

	return (bNitTsInfoFound == TRUE) ? ERR_OK : ERR_FAIL;
}


static HERROR xmgr_search_lcnupdate_Action_GetServiceListsFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32						ulSvcCnt = 0, ulSvcNum = 0;
	HUINT32						ulBqCnt = 0, ulBqNum = 0;
	HUINT32						ulCntForSvc = 0, ulNumBqIdsForSvc = 0;
	HUINT32						ulIndex = 0, ulSvcListNum = 0;
	HUINT16						ausBqIdArray[MAX_BOUQUET_NUM_PER_SVC] = {0, };
	SvcSi_LcnSvcListInfo_t		*pstSvcListInfoArray = NULL;
	SesLcn_SvcList_t			*pstSvcListArray = NULL;
	DbSvc_Info_t				*astSvcInfoArray = NULL, *pstDstSvcArray = NULL;
	DbSvc_BouqInfo_t			*pstBqInfoArray = NULL;
	SesLcn_BouqInfo_t			*pstSesBqInfo = NULL;
	HERROR						hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SVCLISTINFO, &ulSvcListNum, (void **)&pstSvcListInfoArray);
	if (hErr != ERR_OK || ulSvcListNum == 0)
	{
		return ERR_FAIL;
	}

	pstSvcListArray = (SesLcn_SvcList_t *)OxAP_Malloc (sizeof(SesLcn_SvcList_t) * ulSvcListNum);
	if (pstSvcListArray == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstSvcListArray, 0, sizeof(SesLcn_SvcList_t) * ulSvcListNum);

	for (ulIndex = 0; ulIndex < ulSvcListNum; ulIndex++)
	{
		pstSvcListArray[ulIndex].usSvcListId	= pstSvcListInfoArray[ulIndex].usSvcListId;
		pstSvcListArray[ulIndex].ucVer			= pstSvcListInfoArray[ulIndex].ucSecVer;
		HxSTD_memcpy (pstSvcListArray[ulIndex].szSvcListName, pstSvcListInfoArray[ulIndex].szSvcListName, DxLCN_SVC_LIST_NAME_LEN);
	}

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SVCLISTINFO, (void *)pstSvcListInfoArray);

	for (ulIndex = 0; ulIndex < ulSvcListNum; ulIndex++)
	{
		SVC_SI_SetExtSearchOption (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_OPTION_SES_SVCLISTID,
								(HUINT32)pstSvcListArray[ulIndex].usSvcListId, 0, 0);

		// Service Infos
		ulSvcNum = 0;
		astSvcInfoArray = NULL;

		hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SERVICE_SVCINFO, &ulSvcNum, (void **)&astSvcInfoArray);
		if (hErr == ERR_OK && ulSvcNum > 0 && astSvcInfoArray != NULL)
		{
			pstDstSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
			if (pstDstSvcArray != NULL)
			{
				HxSTD_memcpy (pstDstSvcArray, astSvcInfoArray, sizeof(DbSvc_Info_t) * ulSvcNum);

				pstSvcListArray[ulIndex].ulSvcNum = ulSvcNum;
				pstSvcListArray[ulIndex].pstSvcArray = pstDstSvcArray;
			}
		}

		if (astSvcInfoArray != NULL)
		{
			SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_SERVICE_SVCINFO, (void *)astSvcInfoArray);
			astSvcInfoArray = NULL;
		}

		// Bouquet Info
		pstBqInfoArray = NULL;
		pstSvcListArray[ulIndex].pstBqList = NULL;

		// Service가 없으면 Bouquet 도 의미없다 :
		if (ulSvcNum == 0 || pstDstSvcArray == NULL)
		{
			continue;
		}

#ifdef CONFIG_OP_SES
		hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_BOUQUET_BQINFO, &ulBqNum, (void **)&pstBqInfoArray);
		if (hErr == ERR_OK && ulBqNum > 0 && pstBqInfoArray != NULL)
		{
			for (ulBqCnt = 0; ulBqCnt < ulBqNum; ulBqCnt ++)
			{
				pstSesBqInfo = (SesLcn_BouqInfo_t *)OxAP_Malloc (sizeof(SesLcn_BouqInfo_t));
				if (pstSesBqInfo != NULL)
				{
					HxSTD_memset (pstSesBqInfo, 0, sizeof(SesLcn_BouqInfo_t));
					pstSesBqInfo->stBqInfo = pstBqInfoArray[ulBqCnt];

					for (ulSvcCnt = 0; ulSvcCnt < ulSvcNum; ulSvcCnt++)
					{
						hErr = DB_SVC_GetBouquetIndexArrayInSvcInfo (&(pstDstSvcArray[ulSvcCnt]), MAX_BOUQUET_NUM_PER_SVC, &ulNumBqIdsForSvc, ausBqIdArray);
						if (hErr != ERR_OK || ulNumBqIdsForSvc == 0)			{ continue; }

						for (ulCntForSvc = 0; ulCntForSvc < ulNumBqIdsForSvc; ulCntForSvc++)
						{
							if (pstSesBqInfo->stBqInfo.usBouqIdx == ausBqIdArray[ulCntForSvc])
							{
								pstSesBqInfo->pstSvcs = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSesBqInfo->pstSvcs, (void *)&(pstDstSvcArray[ulSvcCnt]));
								break;
							}
						}
					}

					pstSvcListArray[ulIndex].pstBqList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSvcListArray[ulIndex].pstBqList, (void *)pstSesBqInfo);
				}
			}
		}

		if (pstBqInfoArray != NULL)
		{
			SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_BOUQUET_BQINFO, (void *)pstBqInfoArray);
			pstBqInfoArray = NULL;
		}

		// Bouquet List에 등록하고 Service까지 연결한 다음에는 SvcInfo 내의 Bouquet Info를 Clear 시킨다
		// 정확한 Bouquet Info를 나중에 추가하기 위한 Reset 작업이다.
		for (ulSvcCnt = 0; ulSvcCnt < ulSvcNum; ulSvcCnt++)
		{
			DB_SVC_ClearBouquetInfoInSvcInfo (&(pstDstSvcArray[ulSvcCnt]));
		}
#endif
	}


#if 0 //test
{
#define SVC_NAME "Astra Portal"

	SVC_Info_t	stPortalSvc;

	HxSTD_memset(&stPortalSvc, 0x00, sizeof(SVC_Info_t));

	stPortalSvc.usOnId = 0x01;
	stPortalSvc.usTsId =0x41d;
	stPortalSvc.usSvcId =0x5277;
	stPortalSvc.eOrgDeliType= eDxDELIVERY_TYPE_SAT;
	stPortalSvc.eSection= eDxSvcSection_PreProgrammed;
	stPortalSvc.bVisibleSvcFlag= TRUE;
	stPortalSvc.usLcn = ASTRA_PORTAL_SVC_NUM;
	stPortalSvc.eSvcType= eDxSVC_TYPE_TV;

	stPortalSvc.usTsIdx= 0x01;

	MWC_UTIL_DvbStrcpy(stPortalSvc.szSvcName, SVC_NAME);

	DB_SVC_AddServiceInfo(&stPortalSvc,  NULL);
}
#endif
	pstContents->ulSvcListNum = ulSvcListNum;
	pstContents->pstLcnSvcListArray = pstSvcListArray;

	return ERR_OK;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HERROR xmgr_search_lcnupdate_Action_GetAstraPortalRefNitTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HBOOL						bNitTsInfoFound;
	HUINT32			 			ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t			*pstTripleIdArray;
	SesLcn_TsInfo_t				*pstTsInfo;
	HERROR			 			hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr != ERR_OK || ulLinkNum == 0)
	{
		return ERR_FAIL;
	}

	bNitTsInfoFound = FALSE;

	for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
													pstTripleIdArray[ulLinkCnt].usOnId,
													pstTripleIdArray[ulLinkCnt].usTsId,
													&pstTsInfo);
		if (hErr == ERR_OK)
		{
			bNitTsInfoFound = TRUE;
			xmgr_search_lcnupdate_Action_AppendAstraPortalRefNitTsInfo (pstContents, &(pstTsInfo->stTsInfo));
		}
	}

	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID, (void *)pstTripleIdArray);

	return (bNitTsInfoFound == TRUE) ? ERR_OK : ERR_FAIL;
}


static HERROR xmgr_search_lcnupdate_Action_GetAstraPortalTsInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HBOOL			 			bAstraPortalTsInfoFound = FALSE;
	HBOOL						bAstraRegionalPortalTsInfoFound = FALSE;
	HUINT32						ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t			*pstTripleIdArray = NULL;
	SesLcn_TsInfo_t				*pstTsInfo = NULL;
	HERROR			 			hErr;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_ASTRA_PORTAL_TS_INFO, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && ulLinkNum != 0)
	{
		MSG_LOG ("[ASTRA PORTAL] ulLinkNum(%d) \n", ulLinkNum);

		for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
		{
			hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
														pstTripleIdArray[ulLinkCnt].usOnId,
														pstTripleIdArray[ulLinkCnt].usTsId,
														&pstTsInfo);
			if (hErr == ERR_OK)
			{
				bAstraPortalTsInfoFound = TRUE;

				MSG_LOG ("[xmgr_search_lcnupdate_Action_GetAstraPortalTsInfoFromSi]   OnID (0x%04x) TsID(0x%04x) Freq(%d) \n",
					pstTripleIdArray[ulLinkCnt].usOnId, pstTripleIdArray[ulLinkCnt].usTsId, pstTsInfo->stTsInfo.stTuningParam.sat.ulFrequency);

				xmgr_search_lcnupdate_Action_AppendAstraPortalTsInfo (pstContents, &(pstTsInfo->stTsInfo), pstTripleIdArray[ulLinkCnt].usSvcId, ASTRA_PORTAL_LINKAGE);
			}
		}

	 	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_ASTRA_PORTAL_TS_INFO, (void *)pstTripleIdArray);
	}
	else
	{
		MSG_LOG ("[ASTRA PORTAL] hErr(%d) , ulLinkNum (%d) \n", hErr, ulLinkNum);
	}

	ulLinkNum = 0;

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_ASTRA_REGIONAL_PORTAL_TS_INFO, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && ulLinkNum != 0)
	{
		MSG_LOG ("[REGIONAL PORTAL] ulLinkNum(%d) \n", ulLinkNum);

		for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
		{
			hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
														pstTripleIdArray[ulLinkCnt].usOnId,
														pstTripleIdArray[ulLinkCnt].usTsId,
														&pstTsInfo);
			if (hErr == ERR_OK)
			{
				bAstraRegionalPortalTsInfoFound = TRUE;

				MSG_LOG ("[xmgr_search_lcnupdate_Action_GetAstraPortalTsInfoFromSi]   OnID (0x%04x) TsID(0x%04x) Freq(%d) \n",
					pstTripleIdArray[ulLinkCnt].usOnId, pstTripleIdArray[ulLinkCnt].usTsId, pstTsInfo->stTsInfo.stTuningParam.sat.ulFrequency);

				xmgr_search_lcnupdate_Action_AppendAstraPortalTsInfo (pstContents, &(pstTsInfo->stTsInfo), pstTripleIdArray[ulLinkCnt].usSvcId, ASTRA_REGIONAL_PORTAL_LINKAGE);
			}
		}

	 	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_ASTRA_REGIONAL_PORTAL_TS_INFO, (void *)pstTripleIdArray);
	}
	else
	{
		MSG_LOG ("[REGIONAL PORTAL]  hErr(%d) , ulLinkNum (%d) \n", hErr, ulLinkNum);
	}


	MSG_LOG ("bAstraPortalTsInfoFound(%d) bAstraRegionalPortalTsInfoFound(%d) \n", bAstraPortalTsInfoFound, bAstraRegionalPortalTsInfoFound);

	if((bAstraPortalTsInfoFound == FALSE) && (bAstraRegionalPortalTsInfoFound == FALSE))
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GetAstraPortalSvcInfoFromSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR						hErr;
	HUINT32						ulSvcListNum;
	DbSvc_Info_t				*astSvcInfoArray = NULL;
	Ses_PortalSvcList_t			*pstPortalSvcListArray = NULL;

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_PORTAL_INFO, &ulSvcListNum, (void **)&astSvcInfoArray);
	if (hErr != ERR_OK || ulSvcListNum == 0)
	{
		MSG_LOG ("[%s][%d] hErr(%d) , ulSvcListNum (%d) \n", __FILE__, __LINE__, hErr, ulSvcListNum);
		return ERR_FAIL;
	}

	if (astSvcInfoArray != NULL)
	{
		pstPortalSvcListArray = (Ses_PortalSvcList_t *)OxAP_Malloc (sizeof(Ses_PortalSvcList_t));
		if(pstPortalSvcListArray != NULL)
		{
			pstPortalSvcListArray->pstSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcListNum);
			if (pstPortalSvcListArray->pstSvcArray != NULL)
			{
				HxSTD_memcpy (pstPortalSvcListArray->pstSvcArray, astSvcInfoArray, sizeof(DbSvc_Info_t) * ulSvcListNum);
			}

			pstPortalSvcListArray->ulSvcNum = ulSvcListNum;

			pstContents->pstPortalSvcArray = pstPortalSvcListArray;

//			MSG_LOG ("[xmgr_search_lcnupdate_Action_GetAstraPortalSvcInfoFromSi] ulSvcListNum(%d) \n", ulSvcListNum);

		}

	 	SVC_SI_FreeExtSearchData (pstContents->hAction, &stExtSrchSpec, eSVCSI_EXTSEARCH_DATA_SES_PORTAL_INFO, (void *)astSvcInfoArray);
		astSvcInfoArray = NULL;
	}

	return (ulSvcListNum != 0) ? ERR_OK : ERR_FAIL;
}

#endif



#define	_____SI_PROCESS_____

static HERROR xmgr_search_lcnupdate_Action_ProcessHomeTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32		 ulNumTsInfo;
	HERROR		 hErr;

	hErr = xmgr_search_lcnupdate_Action_CountDeliveryTsInfo (pstContents, &ulNumTsInfo);
	if (hErr != ERR_OK || ulNumTsInfo == 0)
	{
		hErr = xmgr_search_lcnupdate_Action_GetDeliveryTsInfoFromSi (pstContents);
	}

	if (hErr == ERR_OK)
	{
		hErr = xmgr_search_lcnupdate_Action_GetSgtTsInfoFromSi (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eState = eSesLcnState_SgtTp_Tuning;
		}
		else
		{
			hErr = xmgr_search_lcnupdate_Action_GetRefNitTsInfoFromSi (pstContents);
			if (hErr == ERR_OK)
			{
				pstContents->eState = eSesLcnState_RefNitTp_Tuning;
			}
		}
	}

	SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_ProcessRefNitTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32		 ulNumTsInfo;
	HERROR		 hErr;

	hErr = xmgr_search_lcnupdate_Action_CountDeliveryTsInfo (pstContents, &ulNumTsInfo);
	if (hErr != ERR_OK || ulNumTsInfo == 0)
	{
		hErr = xmgr_search_lcnupdate_Action_GetDeliveryTsInfoFromSi (pstContents);
	}

	if (hErr == ERR_OK)
	{
		hErr = xmgr_search_lcnupdate_Action_GetSgtTsInfoFromSi (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eState = eSesLcnState_SgtTp_Tuning;
		}
	}

	SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_ProcessSgtTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR		 hErr;

	hErr = xmgr_search_lcnupdate_Action_GetServiceListsFromSi (pstContents);

	SVC_SI_StopExtSearch (pstContents->hAction, TRUE);

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)

	if (hErr == ERR_OK)
		s_bIsReceiveSGT=TRUE;

	if(pstContents->pstAstraPortalRefNitTsInfo == NULL)
	{
		if (hErr != ERR_OK)
		{
			pstContents->eState = eSesLcnState_SearchFail;
			return ERR_FAIL;
		}

		pstContents->eState = eSesLcnState_SearchDone;

		return ERR_OK;
	}
	else
	{
		return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
	}
#else
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSesLcnState_SearchFail;
		return ERR_FAIL;
	}

	pstContents->eState = eSesLcnState_SearchDone;
#endif
	return ERR_OK;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static HERROR xmgr_search_lcnupdate_Action_ProcessAstraPortalRefNitTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR		 hErr;

	hErr = xmgr_search_lcnupdate_Action_GetAstraPortalTsInfoFromSi (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSesLcnState_AstraPortalRefNitTp_Tuning;
	}
	else
	{
		pstContents->eState = eSesLcnState_AstraPortalTp_Tuning;
	}

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_ProcessAstraPortalRefNitTpSi] Status (%s)\n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_ProcessAstraPortalTpSi (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR		 hErr;

	hErr = xmgr_search_lcnupdate_Action_GetAstraPortalSvcInfoFromSi (pstContents);
	if(hErr == ERR_OK)
	{
		xmgr_search_lcnupdate_Action_AddPortalSvcInfo(pstContents);
	}

	if(pstContents->pstAstraPortalTsInfo != NULL)
	{
		pstContents->eState = eSesLcnState_AstraPortalTp_Tuning;
	}

	SVC_SI_StopExtSearch (pstContents->hAction, TRUE);

	return (hErr == ERR_OK) ? ERR_OK : ERR_FAIL;
}

#endif

#define _____SAVE_FUNCTION_____

static HERROR xmgr_search_lcnupdate_Action_CheckHdSimulcast (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSvcInfo1, DbSvc_Info_t *pstSvcInfo2)
{
	DbSvc_Info_t	*pstSdSvcInfo, *pstHdSvcInfo;
	DxVideoType_e	 eVideoType1 = DbSvc_GetVideoType(pstSvcInfo1);
	DxVideoType_e	 eVideoType2 = DbSvc_GetVideoType(pstSvcInfo2);

	if ((eVideoType1 == eDxVIDEO_TYPE_HD) && (eVideoType2 != eDxVIDEO_TYPE_HD))
	// pstSvcInfo1 쪽이 HD Simulcast
	{
		pstHdSvcInfo = pstSvcInfo1;
		pstSdSvcInfo = pstSvcInfo2;
	}
	else if ((eVideoType1 != eDxVIDEO_TYPE_HD) && (eVideoType2 == eDxVIDEO_TYPE_HD))
	// pstSvcInfo2 쪽이 HD Simulcast
	{
		pstHdSvcInfo = pstSvcInfo2;
		pstSdSvcInfo = pstSvcInfo1;
	}
	else if ((eVideoType1 == eDxVIDEO_TYPE_SD) && (eVideoType2 == eDxVIDEO_TYPE_SD))
	// 둘 다 SD Video : 한 쪽이 AVC이면 그 쪽이 우선된다.
	{
		if ((DbSvc_GetVideoCodec(&pstSvcInfo1) == eDxVIDEO_CODEC_H264) && (DbSvc_GetVideoCodec(&pstSvcInfo2) != eDxVIDEO_CODEC_H264))
		{
			pstHdSvcInfo = pstSvcInfo1;
			pstSdSvcInfo = pstSvcInfo2;
		}
		else if ((DbSvc_GetVideoCodec(&pstSvcInfo1) != eDxVIDEO_CODEC_H264) && (DbSvc_GetVideoCodec(&pstSvcInfo2) == eDxVIDEO_CODEC_H264))
		{
			pstHdSvcInfo = pstSvcInfo2;
			pstSdSvcInfo = pstSvcInfo1;
		}
		else
		{
			DbSvc_SetLcn(pstSvcInfo2, IGNORED_LCNNUM);
			return ERR_OK;
		}
	}
	else
	// nCheckSvcCnt < nSvcCnt
	// HD / SD Simulcast 문제가 아니면 뒤에 있는 것을 무시한다.
	{
		DbSvc_SetLcn(pstSvcInfo2, IGNORED_LCNNUM);
		return ERR_OK;
	}

	// HD Simulcast 된 반대 쪽은 반드시 Skip하도록 해야 한다.
	DbSvc_SetLcn(pstSdSvcInfo, IGNORED_LCNNUM);

	// Simulcast 된 정보를 기록해야 한다. 나중에 Pre-programmed List도 수정하기 위해 반드시 필요.
#if defined(CONFIG_OP_SES)
	if (XdbSvc_SesGetVirtualSvcIdPtr(pstSdSvcInfo) == 0)
	{
		return ERR_OK;
	}
#endif

	MSG_LOG ("[xmgr_search_lcnupdate_Action_CheckHdSimulcast] HD Simulcasted LCN : %d\n", pstHdSvcInfo->stDxInfo.lcn);
	xmgr_search_lcnupdate_Action_AppendSimulcastItem (pstContents, pstSdSvcInfo, pstHdSvcInfo);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_AssignSearchSvcToTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
	HUINT32				 ulSvcCnt;
	DbSvc_Info_t			*pstSvcInfo;
	SesLcn_SvcList_t		*pstSvcList;
	SesLcn_TsInfo_t		*pstLcnTsInfo;
	HERROR				 hErr;

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulSvcListIndex]);

	for (ulSvcCnt = 0; ulSvcCnt < pstSvcList->ulSvcNum; ulSvcCnt++)
	{
		pstSvcInfo = &(pstSvcList->pstSvcArray[ulSvcCnt]);
		hErr = xmgr_search_lcnupdate_Action_GetDeliTsInfoByOnTsId (pstContents,
													DbSvc_GetOnId(pstSvcInfo),
													DbSvc_GetTsId(pstSvcInfo),
													&pstLcnTsInfo);
		if (hErr == ERR_OK)
		{
			pstLcnTsInfo->ulTsRefCnt ++;
			pstLcnTsInfo->pstSvcs = UTIL_LINKEDLIST_AppendListItemToPointerList (pstLcnTsInfo->pstSvcs, (void *)pstSvcInfo);
		}
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_GatherSimulcastedInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
	HINT32				 nSvcCnt, nSvcNum, nCheckSvcCnt;
	SesLcn_SvcList_t	*pstSvcList;

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulSvcListIndex]);

	nSvcNum = (HINT32)pstSvcList->ulSvcNum;
	for (nSvcCnt = 1; nSvcCnt < nSvcNum; nSvcCnt++)
	{
		if (DbSvc_GetLcn(&pstSvcList->pstSvcArray[nSvcCnt]) == IGNORED_LCNNUM)
		{
			continue;
		}

		for (nCheckSvcCnt = 0; nCheckSvcCnt < nSvcCnt; nCheckSvcCnt++)
		{
			if (DbSvc_GetLcn(&pstSvcList->pstSvcArray[nCheckSvcCnt]) == IGNORED_LCNNUM)
			{
				continue;
			}

			// 같은 Service Type, 같은 번호의 LCN이 있다 :
			if ((DbSvc_GetSvcType(&(pstSvcList->pstSvcArray[nCheckSvcCnt])) == DbSvc_GetSvcType(&(pstSvcList->pstSvcArray[nSvcCnt]))) &&
				(DbSvc_GetLcn(&pstSvcList->pstSvcArray[nCheckSvcCnt]) == DbSvc_GetLcn(&pstSvcList->pstSvcArray[nSvcCnt])))
			{
				// nCheckSvcCnt < nSvcCnt
				xmgr_search_lcnupdate_Action_CheckHdSimulcast (pstContents, &(pstSvcList->pstSvcArray[nCheckSvcCnt]), &(pstSvcList->pstSvcArray[nSvcCnt]));
			}
		}
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_SaveSearchNetInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT16			 usNetIdx ;
	SesLcn_TsInfo_t	*pstDeliTsInfo;
	POINTER_LIST_T	*pstDeliTsList;
	HERROR			 hErr;

	hErr = DB_SVC_FindNetworkIndexById (eDxDELIVERY_TYPE_ALL, pstContents->stNetInfo.usNetId, &usNetIdx);
	if (hErr != ERR_OK)
	{
		hErr = DB_SVC_AddNetworkInfo (&(pstContents->stNetInfo), &usNetIdx);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}
	}

	pstDeliTsList = pstContents->pstDeliTsInfo;

	while (pstDeliTsList != NULL)
	{
		pstDeliTsInfo = (SesLcn_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstDeliTsList);
		if (pstDeliTsInfo != NULL)
		{
			pstDeliTsInfo->stTsInfo.usNetIdx = usNetIdx;
		}

		pstDeliTsList = pstDeliTsList->pNext;
		if (pstDeliTsList == pstContents->pstDeliTsInfo)		{ break; }
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_SaveSearchTsInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
	HUINT16				 usTsIdx;
	HINT32				 nDbTsCnt, nDbTsNum;
	DbSvc_TsCondition_t	 stTsCond;
	DbSvc_Info_t			*pstSvcInfo;
	DbSvc_TsInfo_t		*pstTsInfoList;
	SesLcn_TsInfo_t		*pstDeliTsInfo;
	POINTER_LIST_T		*pstDeliTsList;
	POINTER_LIST_T		*pstSvcList;
	HERROR				 hErr;

	(void)ulSvcListIndex;

	pstDeliTsList = pstContents->pstDeliTsInfo;
	pstContents->pstDeliTsInfo = NULL;

	while (pstDeliTsList != NULL)
	{
		pstDeliTsInfo = (SesLcn_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstDeliTsList);
		if (pstDeliTsInfo != NULL && pstDeliTsInfo->ulTsRefCnt > 0)
		{
			// 1. Register the DbSvc_TsInfo_t to DB
			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
			stTsCond.nOnId		= (HINT32)pstDeliTsInfo->stTsInfo.usOnId;
			stTsCond.nTsId		= (HINT32)pstDeliTsInfo->stTsInfo.usTsId;

			usTsIdx = 0xffff;
			pstTsInfoList = NULL;
			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
			if (hErr == ERR_OK && pstTsInfoList != NULL)
			{
				for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
				{
					if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->nAntId)
					// Found the same TsInfo including the antenna index
					{
						usTsIdx = pstTsInfoList[nDbTsCnt].usTsIdx;
						if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_SAT, &(pstDeliTsInfo->stTsInfo.stTuningParam), &(pstTsInfoList[nDbTsCnt].stTuningParam)) != 0)
						{
							DB_SVC_UpdateTsInfo (usTsIdx, &(pstDeliTsInfo->stTsInfo));
						}

						break;
					}
				}

				DB_SVC_FreeTsInfoList (pstTsInfoList);
			}

			// No same TS in DB
			if (usTsIdx == 0xffff)
			{
				DB_SVC_AddTsInfo (&(pstDeliTsInfo->stTsInfo), &usTsIdx);
			}

			// 2. Set the ts_idx to the associated services
			pstSvcList = pstDeliTsInfo->pstSvcs;
			pstDeliTsInfo->pstSvcs = NULL;

			while (pstSvcList != NULL)
			{
				pstSvcInfo = (DbSvc_Info_t *)UTIL_LINKEDLIST_GetContents (pstSvcList);
				if (pstSvcInfo != NULL)
				{
					DbSvc_SetTsIdx(pstSvcInfo, usTsIdx);
					DbSvc_SetDeliTypePtr(pstSvcInfo, pstDeliTsInfo->stTsInfo.eDeliType);
					if (DbSvc_GetDeliTypePtr(pstSvcInfo) == eDxDELIVERY_TYPE_SAT)
					{
						DbSvc_SetAntIdx(pstSvcInfo, pstDeliTsInfo->stTsInfo.stTuningParam.sat.ucAntId);
						DbSvc_SetSatType(pstSvcInfo, pstDeliTsInfo->stTsInfo.stTuningParam.sat.eSatType);
					}
				}

				pstSvcList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSvcList, pstSvcList, NULL);
			}
		}

		pstDeliTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstDeliTsList, pstDeliTsList, xmgr_search_lcnupdate_Action_FreeSesLcnTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_SaveSearchBqInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
#ifdef CONFIG_OP_SES
	HUINT16				 usBouqIdx;
	DbSvc_Info_t			*pstSvcInfo;
	SesLcn_SvcList_t	*pstSvcList;
	SesLcn_BouqInfo_t	*pstLcnBqInfo;
	POINTER_LIST_T		*pstBqList;
	POINTER_LIST_T		*pstSvcInfoList;
	HERROR				 hErr;

	MSG_LOG ("[xmgr_search_lcnupdate_Action_SaveSearchBqInfo] (+)\n");

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulSvcListIndex]);

	pstBqList = pstSvcList->pstBqList;
	pstSvcList->pstBqList = NULL;

	while (pstBqList != NULL)
	{
		pstLcnBqInfo = (SesLcn_BouqInfo_t *)UTIL_LINKEDLIST_GetContents (pstBqList);
		if (pstLcnBqInfo != NULL)
		{
			hErr = DB_SVC_FindBouquetIndexByName (pstLcnBqInfo->stBqInfo.szBouqName, &usBouqIdx);
			if (hErr == ERR_OK)
			{
				MSG_LOG ("==> Bouquet [%s] Already Exists : ID (%02d)\n", pstLcnBqInfo->stBqInfo.szBouqName, usBouqIdx);
			}
			else
			{
				hErr = DB_SVC_AddBouquetInfo (&(pstLcnBqInfo->stBqInfo), &usBouqIdx);
				if (hErr == ERR_OK)
				{
					MSG_LOG ("==> Bouquet [%s] Added : ID (%02d)\n", pstLcnBqInfo->stBqInfo.szBouqName, usBouqIdx);
				}
				else
				{
					// SesLcnActAssert (0);
					pstBqList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstBqList, pstBqList, xmgr_search_lcnupdate_Action_FreeSesLcnBouquetInfo);
					continue;
				}
			}

			// 연결된 Service 들에 Bouquet 을 등록시켜 준다.
			pstSvcInfoList = pstLcnBqInfo->pstSvcs;
			pstLcnBqInfo->pstSvcs = NULL;

			while (pstSvcInfoList != NULL)
			{
				pstSvcInfo = (DbSvc_Info_t *)UTIL_LINKEDLIST_GetContents (pstSvcInfoList);
				if (pstSvcInfo != NULL)
				{
					MSG_LOG ("----> SVC No.[%03d] [%s] [%d]\n", DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo), XdbSvc_SesGetVirtualSvcIdPtr(pstSvcInfo));
					DB_SVC_AddBouquetIndexToSvcInfo (pstSvcInfo, usBouqIdx);
				}

				pstSvcInfoList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSvcInfoList, pstSvcInfoList, NULL);
			}
		}

		pstBqList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstBqList, pstBqList, xmgr_search_lcnupdate_Action_FreeSesLcnBouquetInfo);
	}
#else
	(void)pstContents;
	(void)ulSvcListIndex;
#endif

	MSG_LOG ("[xmgr_search_lcnupdate_Action_SaveSearchBqInfo] (-)\n");

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_SaveSearchSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
	HUINT32				 ulSvcCnt, ulNewSvcNum;
	HUINT32				 ulTvNum, ulRadioNum;
	Handle_t			 hSvcHandle;
	Handle_t			*phSvcHandle, *phSvcArray;
	DbSvc_Info_t			*pstSvcInfo;
	SesLcn_SvcList_t	*pstSvcList;
	POINTER_LIST_T		*pstNewSvcHandleList;
	HERROR				 hErr;

	MSG_LOG ("[xmgr_search_lcnupdate_Action_SaveSearchSvcInfo] (+)\n");

	// 이전 서비스들을 우선 찾는다.
	xmgr_search_lcnupdate_Action_MakeLastLcnServiceLists (pstContents);

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulSvcListIndex]);
	ulTvNum = 0;
	ulRadioNum = 0;
	ulNewSvcNum = 0;
	pstNewSvcHandleList = NULL;

	for (ulSvcCnt = 0; ulSvcCnt < pstSvcList->ulSvcNum; ulSvcCnt++)
	{
		pstSvcInfo = &(pstSvcList->pstSvcArray[ulSvcCnt]);
		if (DbSvc_GetLcn(pstSvcInfo) == IGNORED_LCNNUM)
		{
			// Simulcast 등의 이유로 Skip 되는 Service
			continue;
		}

		// TS Info가 추가되지 않은 Service
		if (DbSvc_GetDeliTypePtr(pstSvcInfo) == eDxDELIVERY_TYPE_ALL)
		{
			continue;
		}

#if defined(CONFIG_OP_SES) && defined(CONFIG_DEBUG)
{
	HUINT32			 ulBqIdxCnt, ulBqIdxNum;
	HUINT16			 ausBqIdxArray[8];

	MSG_LOG ("    = Service [%04d:%s] : Bouquet (", DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo));

	hErr = DB_SVC_GetBouquetIndexArrayInSvcInfo (pstSvcInfo, 8, &ulBqIdxNum, ausBqIdxArray);
	if (hErr == ERR_OK)
	{
		for (ulBqIdxCnt = 0; ulBqIdxCnt < ulBqIdxNum; ulBqIdxCnt ++)
		{
			MSG_LOG ("[0x%04x]", ausBqIdxArray[ulBqIdxCnt]);
		}
	}

	MSG_LOG (")\n");

}
#endif
		switch (DbSvc_GetSvcType(pstSvcInfo))
		{
		case eDxSVC_TYPE_TV:
			ulTvNum ++;
			break;

		case eDxSVC_TYPE_RADIO:
			ulRadioNum ++;
			break;

		default:
			break;
		}

		hErr = xmgr_search_lcnupdate_Action_UpdateOrAddService (pstContents, pstSvcInfo, &hSvcHandle);
#if defined(CONFIG_OP_SES)
		if (hErr == ERR_OK && XdbSvc_SesGetNewSvcPtr(pstSvcInfo) == TRUE)
		{
			phSvcHandle = (Handle_t *)OxAP_Malloc (sizeof(Handle_t));
			if (phSvcHandle != NULL)
			{
				*phSvcHandle = hSvcHandle;
				pstNewSvcHandleList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstNewSvcHandleList, (void *)phSvcHandle);
				ulNewSvcNum ++;
			}
		}
#endif
	}

	// New Service 들을 DB에 저장해 놓도록 한다.
	ulSvcCnt = 0;
	phSvcArray = NULL;

	if (ulNewSvcNum > 0)
	{
		phSvcArray = (Handle_t *)OxAP_Malloc (sizeof(Handle_t) * ulNewSvcNum);

		while (pstNewSvcHandleList != NULL)
		{
			phSvcHandle = (Handle_t *)UTIL_LINKEDLIST_GetContents (pstNewSvcHandleList);
			if (phSvcHandle != NULL)
			{
				if (phSvcArray != NULL)
				{
					phSvcArray[ulSvcCnt] = *phSvcHandle;
					ulSvcCnt ++;
				}

				OxAP_Free (phSvcHandle);
			}

			pstNewSvcHandleList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNewSvcHandleList, pstNewSvcHandleList, NULL);
		}
	}

	if (ulSvcCnt > 0 && phSvcArray != NULL)
	{
		// DB에 New Service 를 추가해야 한다.
//		DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_NEWSVC_NUM, (HINT32)ulSvcCnt);
//		DB_SETUP_SetMenuItemBinary (eDxSETUP_SYSTEM_SES_NEWSVC_ITEMS, (sizeof(Handle_t) * ulSvcCnt), (HUINT8 *)phSvcArray);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_NUM, ulSvcCnt, 0);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_ITEMS, (HUINT32)phSvcArray, (HUINT32)(sizeof(Handle_t) * ulSvcCnt));
		DB_PARAM_Sync ();
	}
	else
	{
//		DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_NEWSVC_NUM, 0);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_NUM, 0, 0);
		DB_PARAM_Sync ();
	}

	if (phSvcArray != NULL)
	{
		OxAP_Free (phSvcArray);
	}

	XMGR_SEARCH_INFO_SetSesLcnServiceNumberToDb (ulTvNum, ulRadioNum);

	// 모든 서비스가 추가된 후에는 남은 이전 Service 들을 지워야 한다.
	xmgr_search_lcnupdate_Action_DeleteOldServices (pstContents);

	MSG_LOG ("[xmgr_search_lcnupdate_Action_SaveSearchSvcInfo] (-)\n");

	return ERR_OK;
}


#if defined(CONFIG_OP_SES_ASTRA_PORTAL)

#if defined(CONFIG_DEBUG)
static char xmgr_search_lcnupdate_Action_GetSvcTypeChar(DxSvcType_e eSvcType)
{
	switch(eSvcType)
	{
		case eDxSVC_TYPE_TV:			return 'T';
		case eDxSVC_TYPE_RADIO:			return 'R';
		case eDxSVC_TYPE_DATA:			return 'D';
		case eDxSVC_TYPE_SUBFEED:		return 'S';
		case eDxSVC_TYPE_REPLACEMENT:	return 'P';
		default:						return '?';
	}
}

static void xmgr_search_lcnupdate_Action_PrintProtalSvcInfo(void)
{
	Handle_t				hSvcHandle = HANDLE_NULL;
	HERROR				hErr;
	DbSvc_Info_t			stSvcInfo;

	MSG_LOG ("\n[xmgr_search_lcnupdate_Action_PrintProtalSvcInfo] -------------- (+)\n");

	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, ASTRA_PORTAL_SVC_NUM, &hSvcHandle);
	if(hErr ==ERR_OK || hSvcHandle != HANDLE_NULL)
	{

		MSG_LOG ("   < ASTRA_PORTAL_SVC_NUM >\n");

		VK_memset32(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hErr == ERR_OK)
		{
			MSG_LOG ("\t============================================================================\n") ;
			MSG_LOG ("\t= [%c%3d, %s] TSIDX:0x%04X SVC_ID:0x%04x(%d) TS_ID(0x%04x) ON_ID(0x%x)\n",
				xmgr_search_lcnupdate_Action_GetSvcTypeChar(DbSvc_GetSvcType(&stSvcInfo)), DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo), DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetOnId(&stSvcInfo));
			MSG_LOG ("\t= LcnFlag[%d] eSection[%d]\n", DbSvc_GetLcnFlag(&stSvcInfo), DbSvc_GetSvcSection(&stSvcInfo));
			MSG_LOG ("\t============================================================================\n") ;
		}
		else
		{
			MSG_LOG ("\tDB_SVC_GetServiceInfo error(%d) \n", hErr);
		}
	}
	else
	{
		MSG_LOG ("\tDB_SVC_GetServiceHandleByNumber error(%d) hSvcHandle(0x%08x)\n", hErr, hSvcHandle);
	}

	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, ASTRA_REGIONAL_PORTAL_SVC_NUM, &hSvcHandle);
	if(hErr ==ERR_OK || hSvcHandle != HANDLE_NULL)
	{
		MSG_LOG ("\n   < ASTRA_REGIONAL_PORTAL_SVC_NUM >\n");

		VK_memset32(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hErr == ERR_OK)
		{
			MSG_LOG ("\t============================================================================\n") ;
			MSG_LOG ("\t= [%c%3d, %s] TSIDX:0x%04X SVC_ID:0x%04x(%d) TS_ID(0x%04x) ON_ID(0x%x)\n",
				xmgr_search_lcnupdate_Action_GetSvcTypeChar(DbSvc_GetSvcType(&stSvcInfo)), DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo), DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), DbSvc_GetOnId(&stSvcInfo));
			MSG_LOG ("\t= LcnFlag[%d] eSection[%d]\n", DbSvc_GetLcnFlag(&stSvcInfo), DbSvc_GetSvcSection(&stSvcInfo));
			MSG_LOG ("\t============================================================================\n") ;
		}
		else
		{
			MSG_LOG ("DB_SVC_GetServiceInfo error(%d) \n", hErr);
		}
	}
	else
	{
		MSG_LOG ("\tDB_SVC_GetServiceHandleByNumber error(%d) hSvcHandle(0x%08x)\n", hErr, hSvcHandle);
	}

	MSG_LOG ("[xmgr_search_lcnupdate_Action_PrintProtalSvcInfo] -------------- (-)\n\n");

}
#endif

static HERROR xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	Handle_t				hSvcHandle;
	HERROR				hErr;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TsInfo_t			*pstTsInfoList = NULL;
	HUINT16				usTsIdx, usTsId, usOnId;
	HINT32				nDbTsNum, nDbTsCnt;

	hSvcHandle = HANDLE_NULL;

	MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] (+)\n");

#if defined(CONFIG_DEBUG) && 0
	if(g_ulSesLcnActDbg_Level == DBG_DATA)
		xmgr_search_lcnupdate_Action_PrintProtalSvcInfo();
#endif

	MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] ---------------------- ASTRA_PORTAL_SVC_NUM\n");

	// Find the Astra Portal
	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, ASTRA_PORTAL_SVC_NUM, &hSvcHandle);
	if(hErr ==ERR_OK || hSvcHandle != HANDLE_NULL)
	{
		VK_memset32(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);

		MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] hSvcHandle(0x%08x) usTsIdx (0x%x) usOnId(0x%x) usTsId(0x%x) \n", hSvcHandle, DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo));

		if(hErr == ERR_OK && DbSvc_GetTsIdx(&stSvcInfo) == 0)
		{
			usTsId = DbSvc_GetTsId(&stSvcInfo);
			usOnId = DbSvc_GetOnId(&stSvcInfo);

			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
			stTsCond.nOnId		= (HINT32)usOnId;
			stTsCond.nTsId		= (HINT32)usTsId;

			MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] usOnId(0x%x) usTsId(0x%x) \n", stTsCond.nOnId, stTsCond.nTsId);

			pstTsInfoList = NULL;
			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);

			MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] hErr(%d) nDbTsNum (0x%x) pstTsInfoList(0x%x) \n", hErr ,nDbTsNum, (HUINT32)pstTsInfoList);

			usTsIdx = 0;

			if (hErr == ERR_OK && pstTsInfoList != NULL)
			{
				for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
				{
					// Find the same TsInfo including the antenna index
					if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->nAntId)
					{
						usTsIdx = pstTsInfoList[nDbTsCnt].usTsIdx;
						break;
					}
				}
				DB_SVC_FreeTsInfoList (pstTsInfoList);
			}
			else
			{
				// SesLcnActAssert(0);
			}

			MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] usTsIdx(0x%x) \n", usTsIdx);

			if(usTsIdx != DbSvc_GetTsIdx(&stSvcInfo) && usTsIdx !=0)
			{
				DbSvc_SetTsIdx(&stSvcInfo, usTsIdx);
				MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] stSvcInfo.usTsIdx (0x%x) stSvcInfo.eSatType(%d) \n", DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetSatType(&stSvcInfo));

				hErr=DB_SVC_UpdateServiceInfo(hSvcHandle, &stSvcInfo);
			}
			else
			{
				// SesLcnActAssert(0);
			}
		}
	}
	else
	{
		// SesLcnActAssert(0);
	}

	hSvcHandle = HANDLE_NULL;

	MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] ---------------------- ASTRA_REGIONAL_PORTAL_SVC_NUM\n");

	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, ASTRA_REGIONAL_PORTAL_SVC_NUM, &hSvcHandle);
	if(hErr ==ERR_OK || hSvcHandle != HANDLE_NULL)
	{
		HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);

		MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] hSvcHandle(0x%08x) usTsIdx (0x%x) usOnId(0x%x) usTsId(0x%x) \n", hSvcHandle, DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo));

		if(hErr == ERR_OK && DbSvc_GetTsIdx(&stSvcInfo) == 0)
		{
			usTsId = DbSvc_GetTsId(&stSvcInfo);
			usOnId = DbSvc_GetOnId(&stSvcInfo);

			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
			stTsCond.nOnId		= (HINT32)usOnId;
			stTsCond.nTsId		= (HINT32)usTsId;

			pstTsInfoList = NULL;
			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);

			MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] hErr(%d) nDbTsNum (0x%x) pstTsInfoList(0x%x) \n", hErr ,nDbTsNum, (HUINT32)pstTsInfoList);

			usTsIdx = 0;

			if (hErr == ERR_OK && pstTsInfoList != NULL)
			{
				for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
				{
					// Find the same TsInfo including the antenna index
					if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->nAntId )
					{
						usTsIdx = pstTsInfoList[nDbTsCnt].usTsIdx;
						break;
					}
				}
				DB_SVC_FreeTsInfoList (pstTsInfoList);
			}
			else
			{
				// SesLcnActAssert(0);
			}

			MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] usTsIdx(0x%x) \n", usTsIdx);

			if(usTsIdx != DbSvc_GetTsIdx(&stSvcInfo) && usTsIdx !=0)
			{
				DbSvc_SetTsIdx(&stSvcInfo, usTsIdx);
				MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] stSvcInfo.usTsIdx (0x%x) stSvcInfo.eSatType(%d) \n", DbSvc_GetTsIdx(&stSvcInfo), DbSvc_GetSatType(&stSvcInfo));

				hErr=DB_SVC_UpdateServiceInfo(hSvcHandle, &stSvcInfo);
			}
			else
			{
				// SesLcnActAssert(0);
			}
		}
	}
	else
	{
		// SesLcnActAssert(0);
	}

#if defined(CONFIG_DEBUG) && 0
	if(g_ulSesLcnActDbg_Level == DBG_DATA)
		xmgr_search_lcnupdate_Action_PrintProtalSvcInfo();
#endif
	MSG_LOG ("[xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo] (-)\n");

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_AddPortalSvcInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	Ses_PortalTsInfo_t		*pstAstraPortalTsInfo = NULL;
	Ses_PortalSvcList_t		*pstAstraPortaSvcInfo = NULL;
	POINTER_LIST_T			*pstItem = NULL;
	HUINT32					ulIndex;
	HERROR					hErr;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_Condition_t 		stCond;
	Handle_t				hSvcHandle;
	HUINT16					usTsIdx;
	HUINT16 				usPortalTsId, usPortalOnId, usPortalSvcId;
	DbSvc_TsInfo_t			*pstTsInfoList;
	DbSvc_Info_t			*pstSvcInfoList =NULL;
	HINT32					nDbTsNum = 0, nDbTsCnt = 0, nDbSvcNun = 0, nDbSvcCnt = 0 ;
	HUINT32					ulAntIdx = 0;


	if(pstContents->pstAstraPortalTsInfo !=NULL)
	{
		pstAstraPortaSvcInfo = pstContents->pstPortalSvcArray;
		if(pstAstraPortaSvcInfo !=NULL)
		{
			MSG_LOG ("Portal SvcNum(%d) \n", pstAstraPortaSvcInfo->ulSvcNum);

			for(ulIndex= 0 ; ulIndex<pstAstraPortaSvcInfo->ulSvcNum; ulIndex++)
			{
				usPortalSvcId = DbSvc_GetSvcId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex]);

				pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstAstraPortalTsInfo, 0,
																		xmgr_search_lcnupdate_Action_FilterPortalTsInfoSameSvcId,
																		(void *)&usPortalSvcId);

				if (pstItem != NULL)
				{
					pstAstraPortalTsInfo = (Ses_PortalTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);

					usPortalTsId = pstAstraPortalTsInfo->stTsInfo.usTsId;
					usPortalOnId = pstAstraPortalTsInfo->stTsInfo.usOnId;

					// Fine TS Info
					DB_SVC_InitTsFindCondition (&stTsCond);
					stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
					stTsCond.nOnId		= (HINT32)usPortalOnId;
					stTsCond.nTsId		= (HINT32)usPortalTsId;

					usTsIdx = 0;
					pstTsInfoList = NULL;
					hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
					if (hErr == ERR_OK && pstTsInfoList != NULL)
					{
						for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
						{
							if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->nAntId)
							// Found the same TsInfo including the antenna index
							{
								usTsIdx = pstTsInfoList[nDbTsCnt].usTsIdx;
								break;
							}
						}
						DB_SVC_FreeTsInfoList (pstTsInfoList);
					}

					MSG_LOG ("Portal TripleID(0x%04x | 0x%04x | 0x%04x) usTsIdx(0x%x) ucLinkType(0x%x) \n", usPortalOnId, usPortalTsId, usPortalSvcId, usTsIdx, pstAstraPortalTsInfo->ucLinkType);

					if(pstAstraPortalTsInfo->ucLinkType == ASTRA_PORTAL_LINKAGE)
					{

						DB_SVC_InitSvcFindCondition(&stCond);

						stCond.nOnId = usPortalOnId;
						stCond.nTsId = usPortalTsId;
						stCond.nSvcId= usPortalSvcId;
						stCond.nSection = eDxSvcSection_NonLcn_Priority2;

						hErr = DB_SVC_FindSvcInfoList(&stCond, &nDbSvcNun, &pstSvcInfoList);
						if(hErr != ERR_OK || nDbSvcNun <= 0 )
						{
							DbSvc_SetLcn(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], ASTRA_PORTAL_SVC_NUM);
							DbSvc_SetOnId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalOnId);
							DbSvc_SetTsId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalTsId);
							DbSvc_SetSvcId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalSvcId);
							DbSvc_SetDeliType(pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxDELIVERY_TYPE_SAT);
							DbSvc_SetSvcSection(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxSvcSection_NonLcn_Priority2);
							DbSvc_SetVisibleFlag(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], TRUE);
							DbSvc_SetTsIdx(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usTsIdx);
							DbSvc_SetLcnFlag(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], TRUE);
							DbSvc_SetSvcType(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxSVC_TYPE_TV);
							DbSvc_SetSatType(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eSatType_ASTRA_1);

							XMGR_SEARCH_INFO_GetAstraAntIdx (&ulAntIdx);
							DbSvc_SetAntIdx(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], ulAntIdx);

							DB_SVC_AddServiceInfo(&(pstAstraPortaSvcInfo->pstSvcArray[ulIndex]),  &hSvcHandle);
							s_bAddProtalService = TRUE;

							MSG_LOG ("ADD :: hSvcHandle(0x%08x) LinkType(0x%02x) usTsIdx(0x%04x) OnID(0x%04x) TSID(0x%04x) SvcID(0x%04x) SvcName(%s) \n", hSvcHandle, pstAstraPortalTsInfo->ucLinkType, usTsIdx, usPortalOnId, usPortalTsId, usPortalSvcId, DbSvc_GetName(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex]));
						}
						DB_SVC_FreeServiceInfoList (NULL, NULL, pstSvcInfoList);

					}
					else if(pstAstraPortalTsInfo->ucLinkType == ASTRA_REGIONAL_PORTAL_LINKAGE)
					{
						HBOOL			bAddFlag = FALSE;

						DB_SVC_InitSvcFindCondition(&stCond);

						stCond.nSection = eDxSvcSection_PreProgrammed;
						stCond.nSvcType= eDxSVC_TYPE_TV;

						hErr = DB_SVC_FindSvcInfoList(&stCond, &nDbSvcNun, &pstSvcInfoList);
						if(hErr !=  ERR_OK || nDbSvcNun <= 0)
						{
							bAddFlag = TRUE;
						}
						else
						{
							for(nDbSvcCnt = 0; nDbSvcCnt < nDbSvcNun ; nDbSvcCnt++)
							{
								if((usPortalOnId == DbSvc_GetOnId(&pstSvcInfoList[nDbSvcCnt]))
									&&(usPortalTsId == DbSvc_GetTsId(&pstSvcInfoList[nDbSvcCnt]))
									&&(usPortalSvcId == DbSvc_GetSvcId(&pstSvcInfoList[nDbSvcCnt])))
								{
									break;
								}
							}

							// preprogrammed list 의 개수가 99개보다 작으면서, PreProgrammed list에 없을 경우에만 추가..
							// mwc_svcnum.c 에 PRECH_LCN_BASE_ASTRA_START(1), PRECH_LCN_BASE_ASTRA_END(99) 정의 되어 있다.
							if(nDbSvcCnt == nDbSvcNun && nDbSvcNun < 99)
							{
								bAddFlag = TRUE;
							}
						}

						MSG_LOG ("Region Portal === bAddFlag (%d) nDbSvcCnt (%d)  nDbSvcNun(%d)  \n", bAddFlag, nDbSvcCnt, nDbSvcNun );

						if(bAddFlag == TRUE)
						{
							DbSvc_SetLcn(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], ASTRA_REGIONAL_PORTAL_SVC_NUM);
							DbSvc_SetOnId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalOnId);
							DbSvc_SetTsId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalTsId);
							DbSvc_SetSvcId(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usPortalSvcId);
							DbSvc_SetDeliType(pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxDELIVERY_TYPE_SAT);
							DbSvc_SetSvcSection(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxSvcSection_PreProgrammed);
							DbSvc_SetVisibleFlag(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], TRUE);
							DbSvc_SetTsIdx(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], usTsIdx);
							DbSvc_SetLcnFlag(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], TRUE);
							DbSvc_SetSvcType(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eDxSVC_TYPE_TV);
							DbSvc_SetSatType(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], eSatType_ASTRA_1);

							XMGR_SEARCH_INFO_GetAstraAntIdx (&ulAntIdx);
							DbSvc_SetAntIdx(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex], ulAntIdx);

							DB_SVC_AddServiceInfo(&(pstAstraPortaSvcInfo->pstSvcArray[ulIndex]),  &hSvcHandle);
							s_bAddProtalService = TRUE;

							MSG_LOG ("ADD :: hSvcHandle(0x%08x) LinkType(0x%02x) usTsIdx(0x%04x) OnID(0x%04x) TSID(0x%04x) SvcID(0x%04x) SvcName(%s) \n", hSvcHandle, pstAstraPortalTsInfo->ucLinkType, usTsIdx, usPortalOnId, usPortalTsId, usPortalSvcId, DbSvc_GetName(&pstAstraPortaSvcInfo->pstSvcArray[ulIndex]));
						}

						DB_SVC_FreeServiceInfoList (NULL, NULL, pstSvcInfoList);
					}
					else
					{
						continue;
					}
				}
				else
				{
					MSG_LOG ("Not Match the SvcID(0x%04x) , So Can't find TS Info \n", usPortalSvcId);
				}
			}
		}
	}

	// 다 사용해서 free 한다..
	xmgr_search_lcnupdate_Action_ClearPortalSvcListArray(pstContents);

	return ERR_OK;
}
#endif

static HERROR xmgr_search_lcnupdate_Action_MakeSvcUid(HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HINT32 *ulSvcUid)
{
	DbSvc_Condition_t		cond;
	HINT32				svcuid[16];
	HINT32				count;

	DB_SVC_InitSvcFindCondition(&cond);

	cond.nOrgDeliType = (HINT32)eDxDELIVERY_TYPE_SAT;
	cond.nSvcId = (HINT32)usSvcId;
	cond.nTsId  = (HINT32)usTsId;
	cond.nOnId  = (HINT32)usOnId;

	count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);

	if(count == 0)
	{
		HxLOG_Error("err: cannot get svcuid\n");
		return ERR_FAIL;
	}

	*ulSvcUid = svcuid[0];

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_SendMsgReplaceSvc (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR						hErr = ERR_FAIL, hResult = ERR_FAIL;
	DxLcnUpdateSaveData_t		*pstSaveData = NULL;
	HUINT32						ulNumOfReplacedSvc = 0;
	Handle_t					*pReplacedSvcHandle = NULL;
	DbSvc_Info_t				*astReplacedSvcInfo = NULL;
	int							i;

	if(ERR_OK != DB_PARAM_GetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, (HUINT32*)&ulNumOfReplacedSvc, 0))
		ulNumOfReplacedSvc = 0;
	HxLOG_Error("\t\t[%s:%d], num of replaced svc : %d\n", __FUNCTION__, __LINE__, ulNumOfReplacedSvc);

	pstSaveData = (DxLcnUpdateSaveData_t*)OxAP_Malloc(sizeof(DxLcnUpdateSaveData_t));
	HxSTD_MemSet(pstSaveData, 0x00, sizeof(DxLcnUpdateSaveData_t));

	pstSaveData->ulReplacedSvcNum = ulNumOfReplacedSvc;

	if(ulNumOfReplacedSvc > 0)
	{
		pstSaveData->replacedSvc= (DxLcnUpdateService_t*)OxAP_Malloc(sizeof(DxLcnUpdateService_t) * ulNumOfReplacedSvc);
		HxSTD_MemSet(pstSaveData->replacedSvc, 0x00, sizeof(DxLcnUpdateSaveData_t) * ulNumOfReplacedSvc);

		pReplacedSvcHandle = (Handle_t*)OxAP_Malloc(sizeof(Handle_t) * ulNumOfReplacedSvc);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_REPLACESVC_ITEMS, (HUINT32*)pReplacedSvcHandle, (sizeof(Handle_t) * ulNumOfReplacedSvc));
		if (ERR_OK != hErr)
		{
			HxLOG_Error("DB_PARAM_GetItem (eDB_PARAM_ITEM_REPLACESVC_ITEMS) err:\n");
			goto END_FUNC;
		}

		astReplacedSvcInfo = (DbSvc_Info_t*)OxAP_Malloc(sizeof(DbSvc_Info_t) * ulNumOfReplacedSvc);

		for(i = 0; i<ulNumOfReplacedSvc; i++)
		{
			hErr = DB_SVC_GetServiceInfo(pReplacedSvcHandle[i], &(astReplacedSvcInfo[i]));
			if(hErr == ERR_OK)
			{
				HINT32		ulSvcUid = 0;

				// save lcn & service name
				pstSaveData->replacedSvc[i].lcn = DbSvc_GetLcn(&astReplacedSvcInfo[i]);
				HxSTD_MemCopy(pstSaveData->replacedSvc[i].szSvcName, DbSvc_GetName(&astReplacedSvcInfo[i]), DbSvc_GetNameLength());

				// save triple id
				pstSaveData->replacedSvc[i].usOnId = DbSvc_GetOnId(&astReplacedSvcInfo[i]);
				pstSaveData->replacedSvc[i].usTsId = DbSvc_GetTsId(&astReplacedSvcInfo[i]);
				pstSaveData->replacedSvc[i].usSvcId = DbSvc_GetSvcId(&astReplacedSvcInfo[i]);

				// make & save service uid
				xmgr_search_lcnupdate_Action_MakeSvcUid(DbSvc_GetOnId(&astReplacedSvcInfo[i]), DbSvc_GetTsId(&astReplacedSvcInfo[i]), DbSvc_GetSvcId(&astReplacedSvcInfo[i]), &ulSvcUid);
				pstSaveData->replacedSvc[i].ulSvcUid = ulSvcUid;

				HxLOG_Print("\t\t[%s:%d], [lcn : %d, name : %s] onid : 0x%x, tsid : 0x%x, svcid : 0x%x, svcuid : 0x%x\n", __FUNCTION__, __LINE__,
					DbSvc_GetLcn(&astReplacedSvcInfo[i]), DbSvc_GetName(&astReplacedSvcInfo[i]), DbSvc_GetOnId(&astReplacedSvcInfo[i]), DbSvc_GetTsId(&astReplacedSvcInfo[i]), DbSvc_GetSvcId(&astReplacedSvcInfo[i]), ulSvcUid);
			}
		}
	}

	hResult = (HERROR)BUS_PostMessageToOM(NULL, eMEVT_LCNUPDATE_SAVE_OK, HANDLE_NULL, (HINT32)pstSaveData, 0, 0);

	// initialize
	DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACESVC_NUM, 0, 0);

END_FUNC:
	if(pReplacedSvcHandle)
	{
		HLIB_STD_MemFree(pReplacedSvcHandle);
	}

	if(astReplacedSvcInfo)
	{
		HLIB_STD_MemFree(astReplacedSvcInfo);
	}
	return hResult;
}


#define	_____ACTION_FUNCTIONS_____

static HERROR xmgr_search_lcnupdate_Action_ReleaseAction (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR		 hErr;

	HxLOG_Print("Release Action [%d]\n", pstContents->bActionTaken);
	if (pstContents->bActionTaken == TRUE)
	{
		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			// SesLcnActAssert (0);
		}

		// Release the action :
		MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
		pstContents->bActionTaken = FALSE;
	}

	return ERR_OK;
}


#define	_____MESSAGE_FUNCTIONS_____



#define	_____PROCESSING_FUNCTIONS_____

static HERROR xmgr_search_lcnupdate_Action_TuneSearchTp (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HBOOL				 bRetry;
	DbSvc_TsInfo_t		 stTsInfo;
	DxTuneParam_t		 stChTuneInfo;
	SvcRsc_Info_t		 stRscInfo;
	HERROR				 hErr;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_search_lcnupdate_Action_GetCurrentTsInfo (pstContents, &stTsInfo);
		if (hErr != ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
			bRetry = (hErr == ERR_OK) ? TRUE : FALSE;
			continue;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			// SesLcnActAssert (0);
			break;
		}

		xmgr_search_lcnupdate_Action_IncreaseSessionVersion (pstContents);

		OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
		hErr = SVC_CH_StartTune (pstContents->hAction, &stRscInfo, (HUINT16)-1, &stChTuneInfo);
		if (hErr == ERR_OK)
		{
			return ERR_OK;
		}

		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		bRetry = (hErr == ERR_OK) ? TRUE : FALSE;
	}

	// Failed :
	pstContents->eState = eSesLcnState_SearchFail;
	return ERR_FAIL;
}

static HERROR xmgr_search_lcnupdate_Action_SaveSearchedData (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HINT32					 nSvcNo;
	HUINT32					 ulSvcListIdx = ulParam1;
	HUINT32					 ulLastLcnTvNo, ulLastLcnRadioNo;
	unsigned long			 utTime;
	XmgrSearch_lcnupdate_Action_Contents_t	*pstContents;
	HERROR					 hErr;

	(void)hAction;
	(void)ulParam1;
	(void)ulParam2;
	(void)ulParam3;

	pstContents = xmgr_search_lcnupdate_Action_GetContents();
	if (pstContents == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (ulSvcListIdx >= pstContents->ulSvcListNum)
	{
		return ERR_BUS_OUT_OF_RANGE;
	}

	xmgr_search_lcnupdate_Action_AssignSearchSvcToTsInfo (pstContents, ulSvcListIdx);
	xmgr_search_lcnupdate_Action_SaveSearchNetInfo (pstContents);
	xmgr_search_lcnupdate_Action_SaveSearchTsInfo (pstContents, ulSvcListIdx);
	xmgr_search_lcnupdate_Action_SaveSearchBqInfo (pstContents, ulSvcListIdx);

	// Simulcast 체크를 통해 HD/SD가 있으면 SD를 Skip한다.
	xmgr_search_lcnupdate_Action_GatherSimulcastedInfo (pstContents, ulSvcListIdx);

	// Preferred List에 있는 Service들도 Virtual Svc ID로 보아서 내용이 바뀌었으면 수정해야 한다.
	xmgr_search_lcnupdate_Action_CheckPreferredListChanged (pstContents, ulSvcListIdx);

	// 실제로 Service Save 한다.
	xmgr_search_lcnupdate_Action_SaveSearchSvcInfo (pstContents, ulSvcListIdx);

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	xmgr_search_lcnupdate_Action_UpdatePortalSvcInfo(pstContents);
	s_bAddProtalService = FALSE;

#if defined(CONFIG_DEBUG)
	//xmgr_search_lcnupdate_Action_PrintProtalSvcInfo();
#endif
#endif

	// HD Smartcard가 꽃여 있으면 Simulcast 결과를 Pre-programmed List에 적용한다.
#if defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	{
		HUINT32			 ulValue;
		HUINT32			 ulCasSlotIdx = 0, ulCasSlotNum = 0;
		HUINT8			 bHdCardInserted = FALSE;		// 현재 Smart Card가 들어 가 있는가를 나타내는 값
		HBOOL			 bHdCardCheckedSofar;			// 이제까지 한번이라도 smart card가 들어간 적이 있는가를 물어보는 값

		hErr = SVC_CAS_GetNumOfSCSlot (&ulCasSlotNum);
		if (hErr == ERR_OK && ulCasSlotNum > 0)
		{
			for (ulCasSlotIdx = 0; ulCasSlotIdx < ulCasSlotNum; ulCasSlotIdx++)
			{
				//hErr = MW_CA_NA_IsHdPlusSmartCard ((HUINT8)ulCasSlotIdx, &bHdCardInserted);
				hErr = XMGR_CAS_NaIsHdPlusSmartCard ((HUINT8)ulCasSlotIdx, &bHdCardInserted);
				if (hErr == ERR_OK && bHdCardInserted != FALSE)
				{
					break;
				}
			}
		}

		ulValue = FALSE;
//		hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_SCINSERTED, &ulValue);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SCINSERTED, (HUINT32 *)&ulValue, 0);
		bHdCardCheckedSofar = (hErr == ERR_OK && ulValue == TRUE) ? TRUE : FALSE;

		if (bHdCardInserted != FALSE)
		{
			xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels (pstContents);
			xmgr_search_lcnupdate_Action_UpdateSimulcastedFavorites (pstContents);
		}
		else if (bHdCardCheckedSofar == FALSE)
		{
			// 현재 Smart Card가 들어 있지 않고, Smart Card가 꽃여진 적도 없다면 나중에 꽃혀질 때를 대비하여
			// DB에 Replace Service 정보를 기록해두도록 해야 한다.
			xmgr_search_lcnupdate_Action_RecordSimulcastedChannels (pstContents);
		}
	}
#endif

	// Save the settings:
	XMGR_SEARCH_INFO_SetSesLcnServiceListIdToDb (pstContents->pstLcnSvcListArray[ulSvcListIdx].usSvcListId);
	XMGR_SEARCH_INFO_SetSesLcnServiceListVerToDb (pstContents->pstLcnSvcListArray[ulSvcListIdx].ucVer);
	XMGR_SEARCH_INFO_SetSesLcnServiceListNameToDb (pstContents->pstLcnSvcListArray[ulSvcListIdx].szSvcListName);
	hErr = VK_CLOCK_GetTime (&utTime);
	if (hErr == ERR_OK)
	{
		XMGR_SEARCH_INFO_SetSesLcnLastUpdatedTimeToDb ((UNIXTIME)utTime);
	}

	// Last Service Numbers가 변했다면 Renumbering이 필요하다.
	hErr = DB_SVC_GetMaxLcnInLcnServices (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, &nSvcNo);
	ulLastLcnTvNo = (hErr == ERR_OK) ? (HUINT32)nSvcNo : 0;

	hErr = DB_SVC_GetMaxLcnInLcnServices (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_RADIO, &nSvcNo);
	ulLastLcnRadioNo = (hErr == ERR_OK) ? (HUINT32)nSvcNo : 0;

	if ((pstContents->ulPrevLastLcnTvNo != ulLastLcnTvNo) || (pstContents->ulPrevLastLcnRadioNo != ulLastLcnRadioNo))
	{
		// 기존 non-LCN 채널의 넘버를 re-numbering해준다
		DB_SVC_MakeNonLcnNumber (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV);
		DB_SVC_MakeNonLcnNumber (eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_RADIO);
	}

	// 여기에서 Favorite List 중에 tracking에 등록된 handle이 있으면 반영.
	xmgr_search_lcnupdate_Action_RecoverFavoritesWithTrackingInfo (pstContents);

	// 실제로 지울 서비스는 이 단계에서 지워야 한다.
	xmgr_search_lcnupdate_Action_DeleteSvcDeletionList (pstContents);

	DB_SVC_RemoveSearchFlag();
	BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, (eCheckDbType_SvcAdded | eCheckDbType_SvcDeleted), 0, 0);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32				 ulIndex, ulNumSimulSvc, ulUpdatedNum;
	HUINT32				 ulSvcCnt;
	HINT32				 nPreSvcIdx, nPreSvcNum;
	HINT32				 nFoundHdSvcNum;
	Handle_t			 hPreSvcHandle, hNewSvcHandle;
	Handle_t			*phSvcHandle, *phSvcArray;
	Handle_t			*phFoundHdSvcHandles;
	Handle_t			*phPreSvcHandleArray = NULL;
	DbSvc_Info_t			*pstPreSvcInfo;
	DbSvc_Info_t			*pstPreSvcInfoArray;
	POINTER_LIST_T		*pstPreSvcList;
	DbSvc_Condition_t		 stSvcCond;
	DbSvc_Info_t			 stSdSvcInfo, stHdSvcInfo, stSvcInfo;
	HERROR				 hErr;
	HBOOL			fFound = FALSE;
	HINT32			nSubPrefIdx = 0;
	HBOOL			bDoubleFound = FALSE;

	ulSvcCnt = 0;
	ulUpdatedNum = 0;
	phSvcArray = NULL;
	pstPreSvcList = NULL;

	// 1. Simulcasted Service가 있는지 count
	hErr = xmgr_search_lcnupdate_Action_CountSimulcastItem (pstContents, &ulNumSimulSvc);
	if (hErr != ERR_OK || ulNumSimulSvc <= 0)
	{
		return ERR_OK;
	}

	// 2. Pre-programmed List 를 읽어 온다.
	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nAntIdx		= pstContents->nAntId;
	stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_SAT;
	stSvcCond.nSection		= eDxSvcSection_PreProgrammed;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nPreSvcNum, &phPreSvcHandleArray);
	if (hErr != ERR_OK || nPreSvcNum <= 0)
	{
		if(phPreSvcHandleArray)
		{
			DB_SVC_FreeServiceList(phPreSvcHandleArray);
		}
		return ERR_OK;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nPreSvcNum, &pstPreSvcInfoArray);
	if (hErr != ERR_OK || nPreSvcNum <= 0)
	{
		// DB_SVC_FreeServiceList(phPreSvcHandleArray);
		DB_SVC_FreeServiceInfoList (NULL,phPreSvcHandleArray, pstPreSvcInfoArray);
		return ERR_OK;
	}
//	MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] nPreSvcNum=%d, ulNumSimulSvc=%d"C_CODE_RESET"\n", nPreSvcNum, ulNumSimulSvc);

	// 3. Simulcast Service가 Pre-programmed List에 해당되는지 확인
#if defined(CONFIG_OP_SES)				// Service Info 때문에 막은 것임
	for (ulIndex = 0; ulIndex < ulNumSimulSvc; ulIndex++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetSimulcastItem (pstContents, ulIndex, &stSdSvcInfo, &stHdSvcInfo);
		if (hErr == ERR_OK)
		{
			if (XdbSvc_SesGetVirtualSvcId(stSdSvcInfo) == 0)			{ continue; }

			for (nPreSvcIdx = 0; nPreSvcIdx < nPreSvcNum; nPreSvcIdx++)
			{
				hPreSvcHandle = phPreSvcHandleArray[nPreSvcIdx];
				pstPreSvcInfo = &(pstPreSvcInfoArray[nPreSvcIdx]);

				// Simulcast된 SD 채널과 동일한 Virtual Service ID :
				// 이 Pre-programmed List도 업데이트되어야 한다.
				if (XdbSvc_SesGetVirtualSvcIdPtr(pstPreSvcInfo) == XdbSvc_SesGetVirtualSvcId(stSdSvcInfo))
				{
					nFoundHdSvcNum = 0;
					phFoundHdSvcHandles = NULL;

					DB_SVC_InitSvcFindCondition (&stSvcCond);
					stSvcCond.nLcnFlag	= TRUE;
					stSvcCond.nSvcType	= (HINT32)DbSvc_GetSvcType(&stHdSvcInfo);
					stSvcCond.nStartLcn = (HINT32)DbSvc_GetLcn(&stHdSvcInfo);
					stSvcCond.nEndLcn	= (HINT32)DbSvc_GetLcn(&stHdSvcInfo);
					stSvcCond.nOnId		= (HINT32)DbSvc_GetOnId(&stHdSvcInfo);
					stSvcCond.nTsId		= (HINT32)DbSvc_GetTsId(&stHdSvcInfo);
					stSvcCond.nSvcId	= (HINT32)DbSvc_GetSvcId(&stHdSvcInfo);

					hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nFoundHdSvcNum, &phFoundHdSvcHandles);
					if (hErr == ERR_OK && nFoundHdSvcNum > 0 && phFoundHdSvcHandles != NULL)
					{
						// 실제 DB에 있는 값으로 가져온다.
						hErr = DB_SVC_GetServiceInfo (phFoundHdSvcHandles[0], &stSvcInfo);
						if (hErr == ERR_OK)
						{
//							MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] phFoundHdSvcHandles(0x%x), nFoundHdSvcNum=%d"C_CODE_RESET"\n", phFoundHdSvcHandles[0], nFoundHdSvcNum);
							fFound = FALSE;

							for (nSubPrefIdx = 0; nSubPrefIdx < nPreSvcNum; nSubPrefIdx++)
							{
								if( XdbSvc_SesGetVirtualSvcId(pstPreSvcInfoArray[nSubPrefIdx]) == XdbSvc_SesGetVirtualSvcId(stSvcInfo))
								{
									fFound = TRUE;
									break;
								}
							}

							if(fFound == FALSE)
							{
								// LCN Number / Section 등만 기존 pre-programmed service 을 유지한다.
								DbSvc_SetLcn(&stSvcInfo, DbSvc_GetLcn(pstPreSvcInfo));
								DbSvc_SetSvcType(&stSvcInfo, DbSvc_GetSvcType(pstPreSvcInfo));
								DbSvc_SetSvcSection(&stSvcInfo, DbSvc_GetSvcSection(pstPreSvcInfo));
								DbSvc_SetLocked(&stSvcInfo, DbSvc_GetLocked(pstPreSvcInfo));



#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)
								/*
								 * #53052 : preferred list 에서 서비스 중복 문제. LCN, TripleId, SvcType 이 완전히 일치하는 preferred list 삭제 후, 다시 add
								 */
								{
									HINT32			 nTmpSvcNum;
									Handle_t			*phTmpSvc;

									DB_SVC_InitSvcFindCondition (&stSvcCond);
									stSvcCond.nLcnFlag	= TRUE;
									stSvcCond.nSvcType	= (HINT32)DbSvc_GetSvcType(&stSvcInfo);
									stSvcCond.nStartLcn = (HINT32)DbSvc_GetLcn(&stSvcInfo);
									stSvcCond.nEndLcn	= (HINT32)DbSvc_GetLcn(&stSvcInfo);
									stSvcCond.nOnId		= (HINT32)DbSvc_GetOnId(&stSvcInfo);
									stSvcCond.nTsId		= (HINT32)DbSvc_GetTsId(&stSvcInfo);
									stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(&stSvcInfo);
									stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_SAT;
									stSvcCond.nSection		= eDxSvcSection_PreProgrammed;

									phTmpSvc = NULL;
									bDoubleFound = FALSE;

									hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nTmpSvcNum, &phTmpSvc);
									if (hErr == ERR_OK && nTmpSvcNum > 0 && phTmpSvc != NULL)
									{
//										MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] hPreSvcHandle(0x%x)"C_CODE_RESET"\n", hPreSvcHandle);
//										MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] nTmpSvcNum = %d"C_CODE_RESET"\n", nTmpSvcNum);
//										MSG_LOG (C_CODE_F_GREEN"\t PRE: usLcn(%04d), eVideoType(%d), TsIdx(0x%x), (0x%x, 0x%x, 0x%x), Name(%s)"C_CODE_RESET"\n",
//														pstPreSvcInfo->usLcn, pstPreSvcInfo->eVideoType, pstPreSvcInfo->usTsIdx,
//														pstPreSvcInfo->usTsId, pstPreSvcInfo->usOnId, pstPreSvcInfo->usSvcId, pstPreSvcInfo->szSvcName);

//										MSG_LOG (C_CODE_F_GREEN"\t SVC: usLcn(%04d), eVideoType(%d), TsIdx(0x%x), (0x%x, 0x%x, 0x%x), Name(%s)"C_CODE_RESET"\n",
//														stSvcInfo.usLcn, stSvcInfo.eVideoType, stSvcInfo.usTsIdx,
//														stSvcInfo.usTsId, stSvcInfo.usOnId, stSvcInfo.usSvcId, stSvcInfo.szSvcName);

										DB_SVC_DeleteServiceInfo(phTmpSvc[0]);
//										MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] preferred duplication -> delete hSvc(0x%x) & add new service"C_CODE_RESET"\n", phTmpSvc[0]);
									}

									if(phTmpSvc != NULL)
									{
										DB_SVC_FreeServiceList (phTmpSvc);

										bDoubleFound = TRUE;

										// 기존 Pre-programmed Service를 지우고 새로 Service를 추가해야 한다.
										hErr = DB_SVC_AddServiceInfo (&stSvcInfo, &hNewSvcHandle);
										if (hErr == ERR_OK)
										{
											xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (pstContents, hPreSvcHandle);
//											MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] addeld hPreSvcHandle(0x%x)"C_CODE_RESET"\n", hPreSvcHandle);

											phSvcHandle = (Handle_t *)OxAP_Malloc (sizeof(Handle_t));
											if (phSvcHandle != NULL)
											{
												*phSvcHandle = hNewSvcHandle;
												pstPreSvcList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstPreSvcList, (void *)phSvcHandle);
												ulUpdatedNum ++;
//												MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] add hNewSvcHandle(0x%x)"C_CODE_RESET"\n", hNewSvcHandle);
											}
										}
									}
								}
#endif

								if (bDoubleFound == FALSE)
								{
									// 기존 Pre-programmed Service를 지우고 새로 Service를 추가해야 한다.
									hErr = DB_SVC_AddServiceInfo (&stSvcInfo, &hNewSvcHandle);
									if (hErr == ERR_OK)
									{
										xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (pstContents, hPreSvcHandle);
										xmgr_search_lcnupdate_Action_ReplaceSvcRelatedInfos (hPreSvcHandle, hNewSvcHandle);
//										MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] replace hPreSvcHandle(0x%x) -> hNewSvcHandle(0x%x)"C_CODE_RESET"\n", hPreSvcHandle, hNewSvcHandle);

										phSvcHandle = (Handle_t *)OxAP_Malloc (sizeof(Handle_t));
										if (phSvcHandle != NULL)
										{
											*phSvcHandle = hNewSvcHandle;
											pstPreSvcList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstPreSvcList, (void *)phSvcHandle);
											ulUpdatedNum ++;
										}
									}
								}
							}
						}
					}

					if (phFoundHdSvcHandles != NULL)
					{
						DB_SVC_FreeServiceList (phFoundHdSvcHandles);
					}
				}
			}
		}
	}
#endif

	ulSvcCnt = 0;
	phSvcArray = NULL;

//	MSG_LOG (C_CODE_F_GREEN"[xmgr_search_lcnupdate_Action_UpdateSimulcastedChannels] ulUpdatedNum = %d"C_CODE_RESET"\n", ulUpdatedNum);

	if (ulUpdatedNum > 0)
	{
		phSvcArray = (Handle_t *)OxAP_Malloc (sizeof(Handle_t) * ulUpdatedNum);

		while (pstPreSvcList != NULL)
		{
			phSvcHandle = (Handle_t *)UTIL_LINKEDLIST_GetContents (pstPreSvcList);
			if (phSvcHandle != NULL)
			{
				if (phSvcArray != NULL)
				{
					phSvcArray[ulSvcCnt] = *phSvcHandle;
					ulSvcCnt ++;
				}

				OxAP_Free (phSvcHandle);
			}

			pstPreSvcList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstPreSvcList, pstPreSvcList, NULL);
		}
	}

	if (ulSvcCnt > 0 && phSvcArray != NULL)
	{
		// DB에 New Service 를 추가해야 한다.
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, ulSvcCnt, 0);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_ITEMS, (HUINT32)phSvcArray, (sizeof(Handle_t) * ulSvcCnt));
		DB_PARAM_Sync ();
	}
	else
	{
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, 0, 0);
		DB_PARAM_Sync ();
	}

	if (phSvcArray != NULL)
	{
		OxAP_Free (phSvcArray);
	}

	DB_SVC_FreeServiceInfoList (NULL,phPreSvcHandleArray, pstPreSvcInfoArray);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_UpdateSimulcastedFavorites (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32				 ulIndex, ulNumSimulSvc;
	DbSvc_Info_t			 stSdSvcInfo, stHdSvcInfo;
	HERROR				 hErr;

	// 1. Simulcasted Service가 있는지 count
	hErr = xmgr_search_lcnupdate_Action_CountSimulcastItem (pstContents, &ulNumSimulSvc);
	if (hErr != ERR_OK || ulNumSimulSvc == 0)
	{
		return ERR_OK;
	}

	for (ulIndex = 0; ulIndex < ulNumSimulSvc; ulIndex++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetSimulcastItem (pstContents, ulIndex, &stSdSvcInfo, &stHdSvcInfo);
		if (hErr == ERR_OK)
		{
			xmgr_search_lcnupdate_Action_SimulcastFavoriteChannel (&stSdSvcInfo, &stHdSvcInfo);
		}
	}

	return ERR_OK;
}


static HERROR xmgr_search_lcnupdate_Action_RecordSimulcastedChannels (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32				 ulIndex, ulNumSimulSvc, ulUpdatedNum;
	HUINT32				 ulSvcCnt;
	HINT32				 nPreSvcIdx, nPreSvcNum;
	HINT32				 nFoundHdSvcNum;
	Handle_t			 hPreSvcHandle;
	Handle_t			*phFoundHdSvcHandles = NULL;
	Handle_t			*phPreSvcHandleArray = NULL;
	DbSvc_Info_t			*pstPreSvcInfo = NULL;
	DbSvc_Info_t			*pstPreSvcInfoArray = NULL;
	SesLcnReplaceInfo_t	*pstReplaceInfo = NULL, *pstReplaceArray = NULL;
	POINTER_LIST_T		*pstPreSvcList = NULL;
	DbSvc_Condition_t		 stSvcCond;
	DbSvc_Info_t			 stSdSvcInfo, stHdSvcInfo;
	HERROR				 hErr;

	ulSvcCnt = 0;
	ulUpdatedNum = 0;
	pstReplaceArray = NULL;
	pstPreSvcList = NULL;

	// 1. Simulcasted Service가 있는지 count
	hErr = xmgr_search_lcnupdate_Action_CountSimulcastItem (pstContents, &ulNumSimulSvc);
	if (hErr != ERR_OK || ulNumSimulSvc <= 0)
	{
		return ERR_OK;
	}

	// 2. Pre-programmed List 를 읽어 온다.
	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nAntIdx		= pstContents->nAntId;
	stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_SAT;
	stSvcCond.nSection		= eDxSvcSection_PreProgrammed;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nPreSvcNum, &phPreSvcHandleArray);
	if (hErr != ERR_OK || nPreSvcNum <= 0)
	{
		if(phPreSvcHandleArray)
		{
			DB_SVC_FreeServiceList(phPreSvcHandleArray);
		}
		return ERR_OK;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nPreSvcNum, &pstPreSvcInfoArray);
	if (hErr != ERR_OK || nPreSvcNum <= 0)
	{
		DB_SVC_FreeServiceInfoList (NULL, phPreSvcHandleArray, pstPreSvcInfoArray);
		return ERR_OK;
	}

	// 3. Simulcast Service가 Pre-programmed List에 해당되는지 확인
#if defined(CONFIG_OP_SES)				// Service Info 때문에 막은 것임
	for (ulIndex = 0; ulIndex < ulNumSimulSvc; ulIndex++)
	{
		hErr = xmgr_search_lcnupdate_Action_GetSimulcastItem (pstContents, ulIndex, &stSdSvcInfo, &stHdSvcInfo);
		if (hErr == ERR_OK)
		{
			if (XdbSvc_SesGetVirtualSvcId(stSdSvcInfo) == 0)			{ continue; }

			for (nPreSvcIdx = 0; nPreSvcIdx < nPreSvcNum; nPreSvcIdx++)
			{
				hPreSvcHandle = phPreSvcHandleArray[nPreSvcIdx];
				pstPreSvcInfo = &(pstPreSvcInfoArray[nPreSvcIdx]);

				// Simulcast된 SD 채널과 동일한 Virtual Service ID :
				// 이 Pre-programmed List도 업데이트되어야 한다.
				if (XdbSvc_SesGetVirtualSvcIdPtr(pstPreSvcInfo) == XdbSvc_SesGetVirtualSvcId(stSdSvcInfo))
				{
					nFoundHdSvcNum = 0;
					phFoundHdSvcHandles = NULL;

					DB_SVC_InitSvcFindCondition (&stSvcCond);
					stSvcCond.nLcnFlag		= TRUE;
					stSvcCond.nSvcType		= (HINT32)DbSvc_GetSvcType(&stHdSvcInfo);
					stSvcCond.nStartLcn 	= (HINT32)DbSvc_GetLcn(&stHdSvcInfo);
					stSvcCond.nEndLcn		= (HINT32)DbSvc_GetLcn(&stHdSvcInfo);
					stSvcCond.nOnId			= (HINT32)DbSvc_GetOnId(&stHdSvcInfo);
					stSvcCond.nTsId			= (HINT32)DbSvc_GetTsId(&stHdSvcInfo);
					stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(&stHdSvcInfo);

					hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nFoundHdSvcNum, &phFoundHdSvcHandles);
					if (hErr == ERR_OK && nFoundHdSvcNum > 0 && phFoundHdSvcHandles != NULL)
					{
						pstReplaceInfo = (SesLcnReplaceInfo_t *)OxAP_Malloc (sizeof(SesLcnReplaceInfo_t));
						if (pstReplaceInfo != NULL)
						{
							pstReplaceInfo->hSdSvcHandle = hPreSvcHandle;
							pstReplaceInfo->hHdSimulSvcHandle = phFoundHdSvcHandles[0];

							pstPreSvcList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstPreSvcList, (void *)pstReplaceInfo);
							ulUpdatedNum ++;
						}
					}

					if (phFoundHdSvcHandles != NULL)
					{
						DB_SVC_FreeServiceList (phFoundHdSvcHandles);
					}
				}
			}
		}
	}
#endif

	ulSvcCnt = 0;
	pstReplaceArray = NULL;

	if (ulUpdatedNum > 0)
	{
		pstReplaceArray = (SesLcnReplaceInfo_t *)OxAP_Malloc (sizeof(SesLcnReplaceInfo_t) * ulUpdatedNum);
		while (pstPreSvcList != NULL)
		{
			pstReplaceInfo = (SesLcnReplaceInfo_t *)UTIL_LINKEDLIST_GetContents (pstPreSvcList);
			if (pstReplaceInfo != NULL)
			{
				if (pstReplaceArray != NULL)
				{
					pstReplaceArray[ulSvcCnt] = *pstReplaceInfo;
					ulSvcCnt ++;
				}

				OxAP_Free (pstReplaceInfo);
			}

			pstPreSvcList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstPreSvcList, pstPreSvcList, NULL);
		}
	}

	if (ulSvcCnt > 0 && pstReplaceArray != NULL)
	{
		// DB에 New Service 를 추가해야 한다.
		DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACELATER_NUM, ulSvcCnt, 0);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACELATER_ITEMS, (HUINT32)pstReplaceArray, (sizeof(SesLcnReplaceInfo_t) * ulSvcCnt));
		DB_PARAM_Sync ();
	}
	else
	{
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACELATER_NUM, 0, 0);
		DB_PARAM_Sync ();
	}

	if (pstReplaceArray != NULL)
	{
		OxAP_Free (pstReplaceArray);
	}

	DB_SVC_FreeServiceInfoList (NULL, phPreSvcHandleArray, pstPreSvcInfoArray);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_CheckPreferredListChanged (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, HUINT32 ulSvcListIndex)
{
	HBOOL				 bSvcUpdated;
	HINT32				 nPrefSvcIdx, nPrefSvcCnt1, nPrefSvcCnt2;
	HUINT32				 ulSvcCnt;
	Handle_t			 hNewSvcHandle;
	DbSvc_Condition_t	 stSvcCond;
	DbSvc_Info_t		 stSvcInfo;
	Handle_t			*ahSvcHandleArray;
	DbSvc_Info_t		*astSvcInfoArray;
	SesLcn_SvcList_t	*pstSvcList;
	HERROR				 hErr;

	bSvcUpdated = FALSE;
	astSvcInfoArray = NULL;
	ahSvcHandleArray = NULL;

	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nAntIdx		= pstContents->nAntId;
	stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_SAT;
	stSvcCond.nSection		= eDxSvcSection_PreProgrammed;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nPrefSvcCnt1, &ahSvcHandleArray);
	if (hErr != ERR_OK || nPrefSvcCnt1 <= 0 || ahSvcHandleArray == NULL)
	{
		goto Error;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nPrefSvcCnt2, &astSvcInfoArray);
	if (hErr != ERR_OK || nPrefSvcCnt2 <= 0 || astSvcInfoArray == NULL)
	{
		goto Error;
	}

	if (nPrefSvcCnt1 != nPrefSvcCnt2)
	{
		goto Error;
	}

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulSvcListIndex]);
	for (ulSvcCnt = 0; ulSvcCnt < pstSvcList->ulSvcNum; ulSvcCnt++)
	{
		stSvcInfo = pstSvcList->pstSvcArray[ulSvcCnt];
		if (DbSvc_GetLcn(&stSvcInfo) == IGNORED_LCNNUM)
		{
			// Simulcast 등의 이유로 Skip 되는 Service
			continue;
		}

#if defined(CONFIG_OP_SES)
		if (XdbSvc_SesGetVirtualSvcId(stSvcInfo) == 0)
		{
			continue;
		}
#endif
		// TS Info가 추가되지 않은 Service
		if (DbSvc_GetDeliType(stSvcInfo) == eDxDELIVERY_TYPE_ALL)
		{
			continue;
		}

#if defined(CONFIG_OP_SES)
		// Preferred List에 같은 Virtual Service ID 이고 특정 내용이 바뀌었을 경우
		for (nPrefSvcIdx = 0; nPrefSvcIdx < nPrefSvcCnt1; nPrefSvcIdx ++)
		{
			if (XdbSvc_SesGetVirtualSvcId(astSvcInfoArray[nPrefSvcIdx]) == XdbSvc_SesGetVirtualSvcId(stSvcInfo))
			{
				// Triple ID나 TsIdx 가 바뀌었다 :
				// 그 서비스 자체가 지워진 거나 다름없으니 지웠다가 새로 만들어야 한다.

				if ((DbSvc_GetSvcId(&astSvcInfoArray[nPrefSvcIdx]) != DbSvc_GetSvcId(&stSvcInfo)) ||
					(DbSvc_GetTsId(&astSvcInfoArray[nPrefSvcIdx]) != DbSvc_GetTsId(&stSvcInfo)) ||
					(DbSvc_GetOnId(&astSvcInfoArray[nPrefSvcIdx]) != DbSvc_GetOnId(&stSvcInfo)) ||
					(DbSvc_GetTsIdx(&astSvcInfoArray[nPrefSvcIdx]) != DbSvc_GetTsIdx(&stSvcInfo)))
				{
					DbSvc_SetLcn(&stSvcInfo, DbSvc_GetLcn(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetOpType(&stSvcInfo, DbSvc_GetOpType(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetSvcType(&stSvcInfo, DbSvc_GetSvcType(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetSvcSection(&stSvcInfo, eDxSvcSection_PreProgrammed);

					bSvcUpdated = TRUE;
					hErr = DB_SVC_AddServiceInfo (&stSvcInfo, &hNewSvcHandle);
					if (hErr == ERR_OK)
					{
						hErr = xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (pstContents, ahSvcHandleArray[nPrefSvcIdx]);
						if (hErr == ERR_OK)
						{
							ahSvcHandleArray[nPrefSvcIdx] = hNewSvcHandle;
							xmgr_search_lcnupdate_Action_ReplaceSvcRelatedInfos (ahSvcHandleArray[nPrefSvcIdx], hNewSvcHandle);
						}
						else
						{
							// SesLcnActAssert (0);
						}
					}
					else
					{
						// SesLcnActAssert (0);
					}

					break;
				}
				// 이름만 달라졌다 : 그냥 Update 한다.
				else if (MWC_UTIL_DvbStrncmp (DbSvc_GetName(&astSvcInfoArray[nPrefSvcIdx]), DbSvc_GetName(&stSvcInfo), DbSvc_GetNameLength()) != 0)
				{
					DbSvc_SetLcn(&stSvcInfo, DbSvc_GetLcn(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetOpType(&stSvcInfo, DbSvc_GetOpType(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetSvcType(&stSvcInfo, DbSvc_GetSvcType(&astSvcInfoArray[nPrefSvcIdx]));
					DbSvc_SetSvcSection(&stSvcInfo, eDxSvcSection_PreProgrammed);
					// TODO: 일단 s/w stage3 인증을 위해 이렇게 처리함
#if 0		// lcnFlag를 FALSE로 설정하면 clib에서 renumbering 하여 LCN 번호가 꼬임 -> default 값인 TRUE로 설정함 ([SES TEST SUITE] HD02-06227)
					DbSvc_SetLcnFlag(&stSvcInfo, FALSE);	/* preferred 가 lcn은 아님 */
#endif
					bSvcUpdated = TRUE;
					hErr = DB_SVC_UpdateServiceInfo (ahSvcHandleArray[nPrefSvcIdx], &stSvcInfo);
					if (hErr != ERR_OK)
					{
						// SesLcnActAssert (0);
					}

					break;
				}
			}
		}
#endif
	}

	if (bSvcUpdated == TRUE)
	{
		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
	}

Error:
	DB_SVC_FreeServiceInfoList (NULL, ahSvcHandleArray, astSvcInfoArray);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_ReplaceSvcRelatedInfos (Handle_t hOrgSvcHandle, Handle_t hNewSvcHandle)
{
	HERROR		 hErr;

	if (hOrgSvcHandle == HANDLE_NULL || hNewSvcHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	hErr = ERR_OK;

	// 1. Favorite List의 등록 사항을 고쳐야 한다.
	hErr |= DB_SVC_ReplaceFavoriteServiceHandle (eFavGroup_All, hOrgSvcHandle, hNewSvcHandle);

	// 2. 예약 사항을 고친다.
	// ToDo:

	return hErr;
}

static HERROR xmgr_search_lcnupdate_Action_SimulcastFavoriteChannel (DbSvc_Info_t *pstSdSvcInfo, DbSvc_Info_t *pstHdSvcInfo)
{
	HUINT32				 ulFavIdx = 0, ulSubFavIdx = 0, ulNumFavSvc = 0;
	HINT32				 nFoundHdSvcNum = 0, nSubIdx = 0;
	DbSvc_Group_e			 eSvcGroup;
	DbSvc_Attr_t			 stSvcAttr;
	DbSvc_Condition_t		 stSvcCond;
	DbSvc_Info_t			 stSvcInfo;
	Handle_t				 hReplaceHdHandle;
	Handle_t			*phFavSvcArray, *phFoundHdSvcHandles;
	HBOOL				bIsAlreadyRegisteredHdService;
	HERROR				 hErr;

	// Favorite Channel 을 읽어 온다.
	DB_SVC_InitSvcAttribute (&stSvcAttr);
	for (eSvcGroup = eSvcGroup_Fav_1; eSvcGroup <= eSvcGroup_Fav_5; eSvcGroup++)
	{
		ulNumFavSvc = 0;
		hErr = DB_SVC_GetServiceCount (eSvcGroup, &stSvcAttr, &ulNumFavSvc);
		if (hErr != ERR_OK || ulNumFavSvc == 0)					{ continue; }

		phFavSvcArray = NULL;
		hErr = DB_SVC_FindServiceList (eSvcGroup, &stSvcAttr, 0, HANDLE_NULL, eSvcFindDirection_Current, &ulNumFavSvc, &phFavSvcArray);
		if (hErr == ERR_OK && ulNumFavSvc > 0 && phFavSvcArray != NULL)
		{
			for (ulFavIdx = 0; ulFavIdx < ulNumFavSvc; ulFavIdx ++)
			{
				hErr = DB_SVC_GetServiceInfo (phFavSvcArray[ulFavIdx], &stSvcInfo);
				if (hErr != ERR_OK)								{ continue; }

#if defined(CONFIG_OP_SES)				// Service Info 때문에 막은 것임
				if ((XdbSvc_SesGetVirtualSvcId(stSvcInfo) != 0) &&
					(XdbSvc_SesGetVirtualSvcId(stSvcInfo) == XdbSvc_SesGetVirtualSvcIdPtr(pstSdSvcInfo)))
				{
					/*
					===========================================          RULE          ===========================================
					 1. Replace 대상인 (서비스 번호는 제외하고) HD 서비스중 하나라도 이미 등록이 되어 있으면 replace하지 않는다.
					 2. Pre-programmed 영역에서 먼저 검색을 한다.
					 3. 동일한 HD 서비스가 존재하고 favorite에도 등록되어 있으면 해당 서비스에 대한 검사는 종료한다. (아래 과정 skip)
					 4. 동일한 HD 서비스는 존재하나 favorite에는 등록되어 있지 않으면 발견한 HD중 첫번째 것으로 replace할 준비를 한다. (LCN 영역에 없을 경우를 대비하는 것임)
					 5. LCN 영역을 검색한다.
					 6. 동일한 HD 서비스가 존재하고 favorite에도 등록되어 있으면 해당 서비스에 대한 검사는 종료한다. (아래 과정 skip)
					 7. 동일한 HD 서비스는 존재하나 favorite에는 등록되어 있지 않으면 발견한 HD중 첫번째 것으로 replace할 준비를 한다. (Pre-programmed에서 등록한게 있어도 overwrite)
					 8. Replace는 해당하는 favorite group에 대해서만 수행한다.
					 ====================================================================================================
					*/

					hReplaceHdHandle = HANDLE_NULL;
					bIsAlreadyRegisteredHdService = FALSE;

					// Pre-programmed 영역 확인.
					{
						nFoundHdSvcNum = 0;
						phFoundHdSvcHandles = NULL;

						DB_SVC_InitSvcFindCondition (&stSvcCond);
						stSvcCond.nLcnFlag		= TRUE;
						stSvcCond.nSvcType		= (HINT32)DbSvc_GetSvcType(pstHdSvcInfo);
						stSvcCond.nOnId 		= (HINT32)DbSvc_GetOnId(pstHdSvcInfo);
						stSvcCond.nTsId 		= (HINT32)DbSvc_GetTsId(pstHdSvcInfo);
						stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(pstHdSvcInfo);
						stSvcCond.nSection		= eDxSvcSection_PreProgrammed;
						if (DbSvc_GetLcn(&stSvcInfo) == DbSvc_GetLcn(pstSdSvcInfo))
						{
							stSvcCond.nStartLcn		= (HINT32)DbSvc_GetLcn(pstHdSvcInfo);
							stSvcCond.nEndLcn		= (HINT32)DbSvc_GetLcn(pstHdSvcInfo);
						}

						hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nFoundHdSvcNum, &phFoundHdSvcHandles);
						if (hErr == ERR_OK && nFoundHdSvcNum > 0 && phFoundHdSvcHandles != NULL)
						{
							for (nSubIdx = 0; nSubIdx < nFoundHdSvcNum; nSubIdx++)
							{
								for (ulSubFavIdx = 0; ulSubFavIdx < ulNumFavSvc; ulSubFavIdx++)
								{
									if (phFoundHdSvcHandles[nSubIdx] == phFavSvcArray[ulSubFavIdx])
									{
										bIsAlreadyRegisteredHdService = TRUE;
										break;
									}
								}

								if (bIsAlreadyRegisteredHdService == TRUE)
								{
									hReplaceHdHandle = HANDLE_NULL;
									break;
								}
							}

							if (bIsAlreadyRegisteredHdService == FALSE)
							{
								// 해당 Favorite list에 동일 조건의 HD service가 등록된 것이 없다. 처음것을 등록.
								hReplaceHdHandle = phFoundHdSvcHandles[0];
							}
						}

						if (phFoundHdSvcHandles != NULL)
						{
							DB_SVC_FreeServiceList (phFoundHdSvcHandles);
							phFoundHdSvcHandles = NULL;
						}
					}

					// Pre-programmed 영역의 HD 서비스 중에는 이미 등록된 것이 없음.
					// LCN 영역 확인.
					if (bIsAlreadyRegisteredHdService == FALSE)
					{
						nFoundHdSvcNum = 0;
						phFoundHdSvcHandles = NULL;

						DB_SVC_InitSvcFindCondition (&stSvcCond);
						stSvcCond.nLcnFlag		= TRUE;
						stSvcCond.nSvcType		= (HINT32)DbSvc_GetSvcType(pstHdSvcInfo);
						stSvcCond.nOnId 		= (HINT32)DbSvc_GetOnId(pstHdSvcInfo);
						stSvcCond.nTsId 		= (HINT32)DbSvc_GetTsId(pstHdSvcInfo);
						stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(pstHdSvcInfo);
						stSvcCond.nSection		= (HINT32)DbSvc_GetSvcSection(&stSvcInfo);
						if (DbSvc_GetLcn(&stSvcInfo) == DbSvc_GetLcn(pstSdSvcInfo))
						{
							stSvcCond.nStartLcn		= (HINT32)DbSvc_GetLcn(pstHdSvcInfo);
							stSvcCond.nEndLcn		= (HINT32)DbSvc_GetLcn(pstHdSvcInfo);
						}

						hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nFoundHdSvcNum, &phFoundHdSvcHandles);
						if (hErr == ERR_OK && nFoundHdSvcNum > 0 && phFoundHdSvcHandles != NULL)
						{
						for (nSubIdx = 0; nSubIdx < nFoundHdSvcNum; nSubIdx++)
						{
							for (ulSubFavIdx = 0; ulSubFavIdx < ulNumFavSvc; ulSubFavIdx++)
							{
								if (phFoundHdSvcHandles[nSubIdx] == phFavSvcArray[ulSubFavIdx])
								{
										bIsAlreadyRegisteredHdService = TRUE;
									break;
								}
							}

								if (bIsAlreadyRegisteredHdService == TRUE)
							{
									hReplaceHdHandle = HANDLE_NULL;
								break;
							}
						}

							if (bIsAlreadyRegisteredHdService == FALSE)
						{
								// 해당 Favorite list에 동일 조건의 HD service가 등록된 것이 없다. 처음것을 등록.
								// LCN 영역에 Replace 조건에 맞는 HD service가 존재하므로 무조건 overwrite한다.
								hReplaceHdHandle = phFoundHdSvcHandles[0];
							}

						}
					}

					if (phFoundHdSvcHandles != NULL)
					{
						DB_SVC_FreeServiceList (phFoundHdSvcHandles);
						phFoundHdSvcHandles = NULL;
					}

					if (hReplaceHdHandle != HANDLE_NULL)
					{
						DbSvc_FavGroup_e		eSvcFavGroup;

						switch (eSvcGroup)
						{
							case eSvcGroup_Fav_1 :
								eSvcFavGroup = eFavGroup_1;
								break;

							case eSvcGroup_Fav_2 :
								eSvcFavGroup = eFavGroup_2;
								break;

							case eSvcGroup_Fav_3 :
								eSvcFavGroup = eFavGroup_3;
								break;

							case eSvcGroup_Fav_4 :
								eSvcFavGroup = eFavGroup_4;
								break;

							case eSvcGroup_Fav_5 :
								eSvcFavGroup = eFavGroup_5;
								break;

							default :
								eSvcFavGroup = eFavGroup_All;
								break;
						}

						DB_SVC_ReplaceFavoriteServiceHandle (eSvcFavGroup, phFavSvcArray[ulFavIdx], hReplaceHdHandle);
						phFavSvcArray[ulFavIdx] = hReplaceHdHandle;
					}

				}
#endif
			}
		}

		if (phFavSvcArray != NULL)
		{
			DB_SVC_FreeServiceList (phFavSvcArray);
		}
	}

	return ERR_OK;
}



#define _____SEARCH_UI_MESSAGE_FUNCTION_____

// UI 에게 상태를 알려주는 Message
static HERROR xmgr_search_lcnupdate_Action_SendSearchStartedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32		 p1, p2, p3;

	(void)pstContents;

	p1 = (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle();
	p2 = 0;
	p3 = 0;

	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STARTED, (Handle_t)pstContents->hAction, p1, p2, p3);
}

static HERROR xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32						p1, p2, p3;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	if(s_bIsReceiveSGT == TRUE)
		pstContents->eState = eSesLcnState_SearchDone;

//	MSG_LOG ("\n\n[xmgr_search_lcnupdate_Action_SendSearchFinishedMsg] s_bIsReceiveSGT(%d) \n\n\n", s_bIsReceiveSGT);
#endif

	p1 = (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle();
	p2 = (pstContents->eState == eSesLcnState_SearchDone) ? TRUE : FALSE;
	p3 = 0;

	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, (Handle_t)pstContents->hAction, p1, p2, p3);
}

static HERROR xmgr_search_lcnupdate_Action_SendSearchSaveFinishedMsg (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32		 p1, p2, p3;

	(void)pstContents;

	p1 = (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle();
	p2 = 0;
	p3 = 0;

	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_SAVE_FINISHED, (Handle_t)pstContents->hAction, p1, p2, p3);
}

static HERROR xmgr_search_lcnupdate_Action_SendSearchActionDestroyed (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32		 p1, p2, p3;

	(void)pstContents;

	p1 = (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle();
	p2 = 0;
	p3 = 0;

	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_DESTROYED, (Handle_t)pstContents->hAction, p1, p2, p3);
}

static HERROR xmgr_search_lcnupdate_Action_SendDetectFinished (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	(void)pstContents;

	return (HERROR)BUS_PostMessage(NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMEVT_LCNUPDATE_NOTIFY_DETECT_FINISH, 0, 0);
}

#define	_____LAST_LCNSVC_UPDATE_DELETE_FUNCTIONS_____

static HERROR xmgr_search_lcnupdate_Action_MakeLastLcnServiceLists (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HINT32				 nSvcCnt1, nSvcCnt2;
	DbSvc_Condition_t		 stSvcCond;
	Handle_t			*ahSvcHandleArray;
	DbSvc_Info_t			*pstSvcInfoArray;
	HERROR				 hErr;

	pstContents->phPrevLcnSvcHandles = NULL;
	pstContents->pstPrevLcnSvcInfos = NULL;

	pstSvcInfoArray = NULL;
	ahSvcHandleArray = NULL;

	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nAntIdx		= pstContents->nAntId;
	stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_SAT;
	stSvcCond.nLcnFlag		= TRUE;
	stSvcCond.nSection		= eDxSvcSection_Lcn;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nSvcCnt1, &ahSvcHandleArray);
	if (hErr != ERR_OK || nSvcCnt1 <= 0 || ahSvcHandleArray == NULL)
	{
		goto Error;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nSvcCnt2, &pstSvcInfoArray);
	if (hErr != ERR_OK || nSvcCnt2 <= 0 || pstSvcInfoArray == NULL)
	{
		goto Error;
	}

	if (nSvcCnt1 != nSvcCnt2)
	{
		goto Error;
	}

	pstContents->ulPrevLcnSvcNum = (HUINT32)nSvcCnt1;
	pstContents->phPrevLcnSvcHandles = ahSvcHandleArray;
	pstContents->pstPrevLcnSvcInfos = pstSvcInfoArray;

	return ERR_OK;

Error:
	DB_SVC_FreeServiceInfoList (NULL, ahSvcHandleArray, pstSvcInfoArray);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_UpdateOrAddService (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, DbSvc_Info_t *pstSvcInfo, Handle_t *phRstSvcHandle)
{
	HUINT32			 ulSvcIndex;
	HERROR			 hErr;

	// 1. Find whether there is same service in the previous LCN
	if (pstContents->phPrevLcnSvcHandles != NULL && pstContents->pstPrevLcnSvcInfos != NULL)
	{
		for (ulSvcIndex = 0; ulSvcIndex < pstContents->ulPrevLcnSvcNum; ulSvcIndex ++)
		{
			if (pstContents->phPrevLcnSvcHandles[ulSvcIndex] == HANDLE_NULL)
			{
				continue;
			}

			// TRIPLE ID 가 같은 LCN 서비스 끼리는 그냥 Update 해 준다.
			if ((DbSvc_GetOnId(pstSvcInfo) == DbSvc_GetOnId(&pstContents->pstPrevLcnSvcInfos[ulSvcIndex])) &&
				(DbSvc_GetTsId(pstSvcInfo) == DbSvc_GetTsId(&pstContents->pstPrevLcnSvcInfos[ulSvcIndex])) &&
				(DbSvc_GetSvcId(pstSvcInfo) == DbSvc_GetSvcId(&pstContents->pstPrevLcnSvcInfos[ulSvcIndex])))
			{
				// TODO: icordhd+ sw fix를 위해 이렇게 처리함
				if(DbSvc_GetSvcSection(pstSvcInfo) == eDxSvcSection_Lcn)
				{
					hErr = DB_SVC_UpdateServiceInfo (pstContents->phPrevLcnSvcHandles[ulSvcIndex], pstSvcInfo);
				}
				else
				{
					DbSvc_Info_t	stSvcInfo;

					stSvcInfo = *pstSvcInfo;
					DbSvc_SetLcnFlag(&stSvcInfo, FALSE);
					hErr = DB_SVC_UpdateServiceInfo (pstContents->phPrevLcnSvcHandles[ulSvcIndex], &stSvcInfo);
				}
				*phRstSvcHandle = pstContents->phPrevLcnSvcHandles[ulSvcIndex];
				pstContents->phPrevLcnSvcHandles[ulSvcIndex] = HANDLE_NULL;

				return hErr;
			}
			else if ((XdbSvc_SesGetVirtualSvcIdPtr(pstSvcInfo) != 0)
					&& (XdbSvc_SesGetVirtualSvcId(pstContents->pstPrevLcnSvcInfos[ulSvcIndex]) != 0)
					&& (XdbSvc_SesGetVirtualSvcIdPtr(pstSvcInfo) == XdbSvc_SesGetVirtualSvcId(pstContents->pstPrevLcnSvcInfos[ulSvcIndex]))
					)	// TRIPLE ID 가 달라도 Virtual Service ID가 0이 아니면서 동일한 경우에 필요한 정보를 기억해 두었다가 Favorite등에 반영한다. (TP 변경등...)

			{
				// 신규 서비스로 등록 후 tracking에 정보 추가..
				hErr = DB_SVC_AddServiceInfo (pstSvcInfo, phRstSvcHandle);
				if (hErr == ERR_OK)
				{
					xmgr_search_lcnupdate_Action_AppendTrackingHandles (pstContents, pstContents->phPrevLcnSvcHandles[ulSvcIndex], *phRstSvcHandle);
				}

				return hErr;
			}

		}
	}

	// 2. Triple ID 가 같은 LCN 서비스가 전에 없었다 : 새로 추가하면 된다.
	return DB_SVC_AddServiceInfo (pstSvcInfo, phRstSvcHandle);
}

static HERROR xmgr_search_lcnupdate_Action_DeleteOldServices (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HUINT32			 ulSvcIndex;

	// 1. Find whether there is same service in the previous LCN
	if (pstContents->phPrevLcnSvcHandles != NULL && pstContents->pstPrevLcnSvcInfos != NULL)
	{
		for (ulSvcIndex = 0; ulSvcIndex < pstContents->ulPrevLcnSvcNum; ulSvcIndex ++)
		{
			if (pstContents->phPrevLcnSvcHandles[ulSvcIndex] == HANDLE_NULL)
			{
				continue;
			}

			xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (pstContents, pstContents->phPrevLcnSvcHandles[ulSvcIndex]);
		}
	}

	// 2. Triple ID 가 같은 LCN 서비스가 전에 없었다 : 새로 추가하면 된다.
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_AppendSvcHandleToSvcDeletionList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hSvcHandle)
{
	pstContents->pstDeleteSvcHandles = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstDeleteSvcHandles, (void *)hSvcHandle);
	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_DeleteSvcDeletionList (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	Handle_t	 hSvcHandle;

	while (pstContents->pstDeleteSvcHandles != NULL)
	{
		hSvcHandle = (Handle_t)UTIL_LINKEDLIST_GetContents (pstContents->pstDeleteSvcHandles);
		DB_SVC_DeleteServiceInfo (hSvcHandle);
		pstContents->pstDeleteSvcHandles = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstDeleteSvcHandles, pstContents->pstDeleteSvcHandles, NULL);
	}

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_Action_RecoverFavoritesWithTrackingInfo (XmgrSearch_lcnupdate_Action_Contents_t *pstContents)
{
	HERROR		 	hErr;
	DbSvc_Group_e		eSvcGroup;
	DbSvc_Attr_t		stSvcAttr;
	Handle_t			*phFavSvcArray, hNewHandle;
	HUINT32			ulFavIdx, ulNumFavSvc;

	DB_SVC_InitSvcAttribute (&stSvcAttr);
	for (eSvcGroup = eSvcGroup_Fav_1; eSvcGroup <= eSvcGroup_Fav_5; eSvcGroup++)
	{
		ulNumFavSvc = 0;
		hErr = DB_SVC_GetServiceCount (eSvcGroup, &stSvcAttr, &ulNumFavSvc);
		if (hErr != ERR_OK || ulNumFavSvc == 0)					{ continue; }

		phFavSvcArray = NULL;
		hErr = DB_SVC_FindServiceList (eSvcGroup, &stSvcAttr, 0, HANDLE_NULL, eSvcFindDirection_Current, &ulNumFavSvc, &phFavSvcArray);
		if (hErr == ERR_OK && ulNumFavSvc > 0 && phFavSvcArray != NULL)
		{
			for (ulFavIdx = 0; ulFavIdx < ulNumFavSvc; ulFavIdx ++)
			{
				xmgr_search_lcnupdate_Action_GetNewTrackingHandle (pstContents, phFavSvcArray[ulFavIdx], &hNewHandle);
				if (hNewHandle != HANDLE_NULL)
				{
					DbSvc_FavGroup_e		eSvcFavGroup;

					switch (eSvcGroup)
					{
						case eSvcGroup_Fav_1 :
							eSvcFavGroup = eFavGroup_1;
							break;

						case eSvcGroup_Fav_2 :
							eSvcFavGroup = eFavGroup_2;
							break;

						case eSvcGroup_Fav_3 :
							eSvcFavGroup = eFavGroup_3;
							break;

						case eSvcGroup_Fav_4 :
							eSvcFavGroup = eFavGroup_4;
							break;

						case eSvcGroup_Fav_5 :
							eSvcFavGroup = eFavGroup_5;
							break;

						default :
							eSvcFavGroup = eFavGroup_All;
							break;
					}

					DB_SVC_ReplaceFavoriteServiceHandle (eSvcFavGroup, phFavSvcArray[ulFavIdx], hNewHandle);
					phFavSvcArray[ulFavIdx] = hNewHandle;
				}
			}
		}
	}

	return ERR_OK;
}

#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusCreate (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		 hErr;

	(void)hAction;
	(void)p1;
	(void)p2;
	(void)p3;

	// 1. Initialize the contents
	hErr = xmgr_search_lcnupdate_Action_InitContents (pstContents);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}
	// store search mode
	pstContents->ulSearchType = (HUINT32)p1;

	return MESSAGE_PASS;
}

// eMEVT_BUS_DESTROY:
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusDestroyed (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulValue;
	HBOOL		bLcnUpdated = FALSE;
	HERROR		hErr = ERR_FAIL;

	hErr = XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb(&ulValue);
	if ( (hErr == ERR_OK) && (ulValue > 1) )
	{
		bLcnUpdated = TRUE;
	}
	else
	{
		bLcnUpdated = FALSE;
	}

	(void)hAction;
	(void)p1;
	(void)p2;
	(void)p3;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	if (pstContents->eState == eSesLcnState_HomeTp_WaitingSi ||
		pstContents->eState == eSesLcnState_RefNitTp_WaitingSi ||
		pstContents->eState == eSesLcnState_SgtTp_WaitingSi||
		pstContents->eState == eSesLcnState_AstraPortalRefNitTp_WaitingSi||
		pstContents->eState == eSesLcnState_AstraPortalTp_WaitingSi)
#else
	if (pstContents->eState == eSesLcnState_HomeTp_WaitingSi ||
		pstContents->eState == eSesLcnState_RefNitTp_WaitingSi ||
		pstContents->eState == eSesLcnState_SgtTp_WaitingSi)
#endif
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch (pstContents->hAction, TRUE);
	}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	// LCN Update 없이 Portal Service 만 추가된 경우에 대한 처리 ..
	if(s_bAddProtalService == TRUE)
	{
		DB_SVC_RemoveSearchFlag();
		BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_SvcAdded, 0, 0);
	}
#endif

	xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);

	// Destroy the contents
	if (bLcnUpdated == FALSE)
	{
		xmgr_search_lcnupdate_Action_DestroyContents (pstContents);
	}

	// Send the destoyed message to the UI procedure
	xmgr_search_lcnupdate_Action_SendSearchActionDestroyed (pstContents);

	// Send the detect finished message to the cold boot procedure
	xmgr_search_lcnupdate_Action_SendDetectFinished(pstContents);

	return MESSAGE_PASS;
}

// MSG_APP_SEARCH_ACTION_START:
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchActionStart (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT16				 usSvcId;
	unsigned long		 utTime;
	SesLcnState_t		 eNextState;
	DbSvc_TsInfo_t		 stTsInfo;
	DxTuneParam_t		 stChTuneInfo;
	HERROR				 hErr;

	(void)hAction;
	(void)p1;
	(void)p2;
	(void)p3;

	if (pstContents->eState != eSesLcnState_Start)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		return MESSAGE_PASS;
	}

	hErr = VK_CLOCK_GetTime (&utTime);
	if (hErr == ERR_OK)
	{
		XMGR_SEARCH_INFO_SetSesLcnLastCheckedTimeToDb ((UNIXTIME)utTime);
	}

//	pstContents->ulActionId = (pstContents->eFuncMode == eSesHdFuncMode_StandBy) ? eActionId_BG_FIRST : MGR_ACTION_GetMainActionId();
	hErr = MGR_ACTION_GetEmptyBgActionId(&(pstContents->ulActionId));
	if (hErr != ERR_OK)
	{
		BUS_MGR_DestroyWithReason(NULL, eDESTROY_BUS_ACTION, 0);
		return MESSAGE_PASS;
	}

	pstContents->hAction = SVC_PIPE_GetActionHandle (pstContents->ulActionId);

	if (xmgr_search_lcnupdate_Action_GetSgtTsInfo (pstContents, &stTsInfo, &usSvcId) == ERR_OK)
	{
		eNextState = eSesLcnState_SgtTp_Tuning;
	}
	else if (xmgr_search_lcnupdate_Action_GetRefNitTsInfo (pstContents, &stTsInfo) == ERR_OK)
	{
		eNextState = eSesLcnState_RefNitTp_Tuning;
	}
	else if (xmgr_search_lcnupdate_Action_GetHomeTsInfo (pstContents, &stTsInfo) == ERR_OK)
	{
		eNextState = eSesLcnState_HomeTp_Tuning;
	}
	else
	{
		// SesLcnActAssert (0);
		BUS_MGR_Destroy (NULL);
		return MESSAGE_PASS;
	}

	// Take the Action
	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &stChTuneInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		BUS_MGR_DestroyWithReason(NULL, eDESTROY_BUS_ACTION, 0);
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		BUS_MGR_DestroyWithReason(NULL, eDESTROY_BUS_ACTION, 0);
		return MESSAGE_PASS;
	}

	pstContents->bActionTaken = TRUE;
	pstContents->eState = eNextState;

	hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		// SesLcnActAssert (0);
		pstContents->eState = eSesLcnState_SearchFail;
	}

	xmgr_search_lcnupdate_Action_SendSearchStartedMsg (pstContents);

	if (pstContents->eState == eSesLcnState_SearchDone ||
		pstContents->eState == eSesLcnState_SearchFail)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);
	}

	return MESSAGE_PASS;
}

// MSG_APP_SEARCH_SAVE_RESULT:

static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchSaveResult (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 nSvcListId, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulCount;

	(void)hAction;
	(void)p2;
	(void)p3;

	for (ulCount = 0; ulCount < pstContents->ulSvcListNum; ulCount++)
	{
		if (pstContents->pstLcnSvcListArray[ulCount].usSvcListId != (HUINT16)nSvcListId)
		{
			continue;
		}

//		MSG_LOG ("\nYSMOON ********** [xmgr_search_lcnupdate_Action_MsgAppSearchSaveResult] ulCount(%d) \n\n", ulCount);

		MGR_ACTION_BgJobExecuteFunction (xmgr_search_lcnupdate_Action_SaveSearchedData, hAction, ulCount, 0, 0);
		return MESSAGE_PASS;
	}

	xmgr_search_lcnupdate_Action_SendSearchSaveFinishedMsg (pstContents);
	return MESSAGE_PASS;
}

// MSG_APP_SEARCH_CANCEL:
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppSearchCancel(XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	(void)pstContents;
	(void)hAction;
	(void)p1;
	(void)p2;
	(void)p3;

	return MESSAGE_PASS;
}

// MSG_APP_BGJOB_FINISHED:
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgAppBgJobFinished (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		 nCbPnt	 = p1;

	(void)pstContents;
	(void)hAction;
	(void)p1;
	(void)p2;
	(void)p3;

	if (nCbPnt == (HINT32)xmgr_search_lcnupdate_Action_SaveSearchedData)
	{
		xmgr_search_lcnupdate_Action_SendSearchSaveFinishedMsg (pstContents);
	}

	return MESSAGE_PASS;
}


// eMEVT_BUS_TIMER:
static BUS_Result_t xmgr_search_lcnupdate_Action_MsgBusTimer (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 nTimerId, HINT32 p2, HINT32 p3)
{
	(void)hAction;
	(void)nTimerId;
	(void)p2;
	(void)p3;

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CH_LOCKED:
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtChLocked (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT16						usSgtSvcId;
	SesLcnState_t				eNextState;
	SvcSi_SearchType_e			eExtSrchType;
	DbSvc_TsInfo_t				stTsInfo;
	HINT32					 	nArg1, nArg2, nArg3;
	HERROR					 	hErr;
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	HUINT16 					usProtalSvcId;
#endif

	SvcSi_ExtSearchSpec_t		stExtSrchSpec;
	SvcSi_ExtSearchOption_t		stExtSrchOption;

	hErr = xmgr_search_lcnupdate_Action_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_EvtChLocked] State (%s)\n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif
	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_Tuning:
		eNextState	 = eSesLcnState_HomeTp_WaitingSi;
		eExtSrchType = eSVCSI_SEARCHTYPE_SES_LCN_HOME_TP;
		nArg1 = nArg2 = nArg3 = 0;
		break;

	case eSesLcnState_RefNitTp_Tuning:
		eNextState	 = eSesLcnState_RefNitTp_WaitingSi;
		eExtSrchType = eSVCSI_SEARCHTYPE_SES_LCN_REFNIT_TP;
		nArg1 = nArg2 = nArg3 = 0;
		break;

	case eSesLcnState_SgtTp_Tuning:
		xmgr_search_lcnupdate_Action_GetSgtTsInfo (pstContents, &stTsInfo, &usSgtSvcId);

		eNextState	 = eSesLcnState_SgtTp_WaitingSi;
		eExtSrchType = eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP;
		nArg1 = (HINT32)pstContents->bAllSgtSearch;
		nArg2 = (HINT32)pstContents->usSvcListId;
		nArg3 = (HINT32)usSgtSvcId;
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSesLcnState_AstraPortalRefNitTp_Tuning:
		eNextState	 = eSesLcnState_AstraPortalRefNitTp_WaitingSi;
		eExtSrchType = eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_REFNIT_TP;
		nArg1 = nArg2 = nArg3 = 0;
		break;

	case	eSesLcnState_AstraPortalTp_Tuning :
		xmgr_search_lcnupdate_Action_GetAstraPortalTsInfo (pstContents, &stTsInfo, &usProtalSvcId);
		eNextState	 = eSesLcnState_AstraPortalTp_WaitingSi;
		eExtSrchType = eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP;
		nArg1 = nArg2 = 0;
		nArg3 = (HINT32)usProtalSvcId;
		break;
#endif

	default:
		return MESSAGE_PASS;
	}


	HxSTD_MemSet(&stExtSrchOption, 0x0, sizeof(SvcSi_ExtSearchOption_t));
	stExtSrchOption.ulExtSearchType	= (HUINT32)eExtSrchType;
	stExtSrchOption.usExtId1		= (HUINT16)nArg1;
	stExtSrchOption.usExtId2		= (HUINT16)nArg2;
	stExtSrchOption.usExtId3		= (HUINT16)nArg3;

	hErr = MGR_SEARCH_GetExtSearchSiSpec(&stExtSrchOption, &stExtSrchSpec);
	if(hErr != ERR_FAIL)
	{
		hErr = SVC_SI_StartExtSearch (pstContents->hAction,
											&stExtSrchSpec,
											&stExtSrchOption,
											TRUE);
		if (hErr == ERR_OK)
		{
			pstContents->eState = eNextState;
			// 필요하면 Timer 를 건다 : ToDo
			return MESSAGE_PASS;
		}
	}

	hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSesLcnState_SearchFail;
	}

	if (pstContents->eState == eSesLcnState_SearchDone ||
		pstContents->eState == eSesLcnState_SearchFail)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);
	}

	return MESSAGE_PASS;
}

// eSEVT_CH_NO_SIGNAL:
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtChNoSignal (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

	(void)p1;
	(void)p2;
	(void)p3;

	hErr = xmgr_search_lcnupdate_Action_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)				{ return MESSAGE_PASS; }

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_EvtChNoSignal] State (%s)\n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_Tuning:
	case eSesLcnState_RefNitTp_Tuning:
	case eSesLcnState_SgtTp_Tuning:
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case	eSesLcnState_AstraPortalRefNitTp_Tuning :
	case	eSesLcnState_AstraPortalTp_Tuning :
#endif
		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}

		break;

	case eSesLcnState_HomeTp_WaitingSi:
	case eSesLcnState_RefNitTp_WaitingSi:
	case eSesLcnState_SgtTp_WaitingSi:
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case	eSesLcnState_AstraPortalRefNitTp_WaitingSi :
	case	eSesLcnState_AstraPortalTp_WaitingSi :
#endif
		hErr = SVC_SI_StopExtSearch (hAction, TRUE);
		// SesLcnActAssert (hErr == ERR_OK);

		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}

		break;

	default:
		pstContents->eState = eSesLcnState_SearchFail;
		break;
	}

	if (pstContents->eState == eSesLcnState_SearchDone ||
		pstContents->eState == eSesLcnState_SearchFail)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);
	}

	return MESSAGE_PASS;
}

// eSEVT_SI_SEARCH_DONE:
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtSiSearchDone (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						hErr;

	(void)p1;
	(void)p2;
	(void)p3;

	hErr = xmgr_search_lcnupdate_Action_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)				{ return MESSAGE_PASS; }

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_EvtSiSearchDone] State (%s)\n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_WaitingSi:
		// Delivery TP Info가 이미 저장된 것이 없으면 읽어 온다.
		hErr = xmgr_search_lcnupdate_Action_ProcessHomeTpSi (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_PASS;
			}
		}

		// Failure :
		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}

		break;

	case eSesLcnState_RefNitTp_WaitingSi:
		hErr = xmgr_search_lcnupdate_Action_ProcessRefNitTpSi (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_PASS;
			}
		}

		// Failure :
		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}

		break;

	case eSesLcnState_SgtTp_WaitingSi:
		hErr = xmgr_search_lcnupdate_Action_ProcessSgtTpSi (pstContents);

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
		if(hErr == ERR_OK)
		{
			if(pstContents->pstAstraPortalRefNitTsInfo != NULL)
			{
				pstContents->eState = eSesLcnState_AstraPortalRefNitTp_Tuning;

				hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
				if (hErr == ERR_OK)
				{
					return MESSAGE_PASS;
				}
			}
		}
#endif
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSesLcnState_AstraPortalRefNitTp_WaitingSi:
		hErr = xmgr_search_lcnupdate_Action_ProcessAstraPortalRefNitTpSi(pstContents);
		if(hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_PASS;
			}
		}

		// Failure :
		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}
		break;

	case eSesLcnState_AstraPortalTp_WaitingSi:
		hErr = xmgr_search_lcnupdate_Action_ProcessAstraPortalTpSi(pstContents);
#if 0//
		hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
		if (hErr == ERR_OK)
		{
			return MESSAGE_BREAK;
		}
#endif
		// Failure :
		hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSesLcnState_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSesLcnState_SearchFail;
		}
		break;
#endif


	default:
		return MESSAGE_PASS;
	}

	if (pstContents->eState == eSesLcnState_SearchDone ||
		pstContents->eState == eSesLcnState_SearchFail)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);
	}

	return MESSAGE_PASS;
}

// eSEVT_SI_SEARCH_FAIL:
static BUS_Result_t xmgr_search_lcnupdate_Action_EvtSiSearchFail (XmgrSearch_lcnupdate_Action_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

	(void)p1;
	(void)p2;
	(void)p3;

	hErr = xmgr_search_lcnupdate_Action_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)				{ return MESSAGE_PASS; }

#if defined(DBG_LCN_STATUS)
	MSG_LOG ("[xmgr_search_lcnupdate_Action_EvtSiSearchFail] Status (%s)\n", xmgr_search_lcnupdate_Action_GetLcnStatusStr(pstContents->eState));
#endif

	switch (pstContents->eState)
	{
	case eSesLcnState_HomeTp_WaitingSi:
		SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
		pstContents->eState = eSesLcnState_HomeTp_Tuning;
		break;

	case eSesLcnState_RefNitTp_WaitingSi:
		SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
		pstContents->eState = eSesLcnState_RefNitTp_Tuning;
		break;

	case eSesLcnState_SgtTp_WaitingSi:
		SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
		pstContents->eState = eSesLcnState_SgtTp_Tuning;
		break;

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	case eSesLcnState_AstraPortalRefNitTp_WaitingSi:
		SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
		pstContents->eState = eSesLcnState_AstraPortalRefNitTp_Tuning;
		break;

	case eSesLcnState_AstraPortalTp_WaitingSi:
		SVC_SI_StopExtSearch (pstContents->hAction, TRUE);
		pstContents->eState = eSesLcnState_AstraPortalTp_Tuning;
		break;
#endif

	default:
		return MESSAGE_PASS;
	}

	hErr = xmgr_search_lcnupdate_Action_SetToNextTsInfo (pstContents);
	if (hErr == ERR_OK)
	{
		hErr = xmgr_search_lcnupdate_Action_TuneSearchTp (pstContents);
		if (hErr != ERR_OK)
		{
			// SesLcnActAssert (0);
			pstContents->eState = eSesLcnState_SearchFail;
		}
	}
	else
	{
		pstContents->eState = eSesLcnState_SearchFail;
	}

	if (pstContents->eState == eSesLcnState_SearchDone ||
		pstContents->eState == eSesLcnState_SearchFail)
	{
		xmgr_search_lcnupdate_Action_SendSearchFinishedMsg (pstContents);
		xmgr_search_lcnupdate_Action_ReleaseAction (pstContents);
	}

	return MESSAGE_PASS;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

// =============================================================================================
//
//   Connection Type 을 자동적으로 알아 내는 Action
// 		여기서는 Single (Loopthrough) 과 Dual Same을 알아내도록 한다.
//
// =============================================================================================

BUS_Result_t xproc_search_lcnupdate_Action_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSearch_lcnupdate_Action_Contents_t	*pstContents;
	BUS_Result_t			eResult = ERR_BUS_INITIAL_FAILED;


	pstContents = xmgr_search_lcnupdate_Action_GetContents();
	if (pstContents == NULL)			{ return MESSAGE_PASS; }

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		// Argument :
		MSG_LOG ("[eMEVT_BUS_CREATE] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgBusCreate (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		// Argument :
		MSG_LOG ("[eMEVT_BUS_DESTROY] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgBusDestroyed (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_SEARCH_ACTION_START:
		// Argument :
		MSG_LOG ("[eMEVT_SEARCH_ACTION_START] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgAppSearchActionStart (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_SEARCH_SAVE_RESULT:
		// Argument :
		// p1 : Service List ID
		MSG_LOG ("[eMEVT_SEARCH_SAVE_RESULT] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgAppSearchSaveResult (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_SEARCH_CANCEL:
		// Argument :
		MSG_LOG ("[eMEVT_SEARCH_CANCEL] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgAppSearchCancel (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
		// Argument :
		// p1 : Callback Function
		// p2 : Result Value
		MSG_LOG ("[MSG_APP_BGJOB_FINISHED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgAppBgJobFinished (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		// Argument :
		// p1 : Timer ID
		MSG_LOG ("[eMEVT_BUS_TIMER] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_MsgBusTimer (pstContents, hAction, p1, p2, p3);
		break;

	case eSEVT_CH_LOCKED:
		// Argument :
		MSG_LOG ("[eSEVT_CH_LOCKED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_EvtChLocked (pstContents, hAction, p1, p2, p3);
		break;

	case eSEVT_CH_NO_SIGNAL:
		// Argument :
		MSG_LOG ("[eSEVT_CH_NO_SIGNAL] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_EvtChNoSignal (pstContents, hAction, p1, p2, p3);
		break;

	case eSEVT_SI_SEARCH_DONE:
		// Argument :
		MSG_LOG ("[eSEVT_SI_SEARCH_DONE] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_EvtSiSearchDone (pstContents, hAction, p1, p2, p3);
		break;

	case eSEVT_SI_SEARCH_FAIL:
		// Argument :
		MSG_LOG ("[eSEVT_SI_SEARCH_FAIL] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
		eResult = xmgr_search_lcnupdate_Action_EvtSiSearchFail (pstContents, hAction, p1, p2, p3);
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return eResult;
}

HERROR XMGR_SEARCH_LCNUPDATE_CountLcnSvcList (HUINT32 *pulListNum)
{
	XmgrSearch_lcnupdate_Action_Contents_t	*pstContents;

	if (pulListNum == NULL)				{ return ERR_BUS_TARGET_NULL; }

	pstContents = xmgr_search_lcnupdate_Action_GetContents();
	if (pstContents == NULL)			{ return ERR_BUS_SOURCE_NULL; }

	if (pstContents->eState != eSesLcnState_SearchDone)
	{
		return ERR_FAIL;
	}

	*pulListNum = pstContents->ulSvcListNum;
	return (*pulListNum > 0) ? ERR_OK : ERR_FAIL;
}

HERROR XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo (HUINT32 ulListIdx, HUINT16 *pusSvcListId, HUINT8 *pucSvcListVer, HUINT8 *szSvcListName)
{
	XmgrSearch_lcnupdate_Action_Contents_t	*pstContents;
	SesLcn_SvcList_t		*pstSvcList;

	pstContents = xmgr_search_lcnupdate_Action_GetContents();
	if (pstContents == NULL)			{ return ERR_BUS_SOURCE_NULL; }

	if (pstContents->eState != eSesLcnState_SearchDone)
	{
		return ERR_FAIL;
	}

	if (ulListIdx >= pstContents->ulSvcListNum || pstContents->pstLcnSvcListArray == NULL)
	{
		return ERR_FAIL;
	}

	pstSvcList = &(pstContents->pstLcnSvcListArray[ulListIdx]);


//	MSG_LOG ("YSMOON ********** [AP_SesLcnSearch_GetLcnSvcListInfo] ==== usSvcListId(0x%x) ucVer(%d) szSvcListName(%s) \n", pstSvcList->usSvcListId,  pstSvcList->ucVer, pstSvcList->szSvcListName);

	if (pusSvcListId != NULL)					{ *pusSvcListId	 = pstSvcList->usSvcListId; }
	if (pucSvcListVer != NULL)					{ *pucSvcListVer = pstSvcList->ucVer; }
	if (szSvcListName != NULL)					{ MWC_UTIL_DvbStrcpy (szSvcListName, pstSvcList->szSvcListName); }

	return ERR_OK;
}

// CAS insert시에 호출되어야 함 		-> yjlim
HERROR XMGR_SEARCH_LCNUPDATE_SimulcastPreferredList (HINT32 nCasMessage)
{
	HBOOL				 bHdCardCheckedSofar, bSuccess;
	HUINT32				 ulSvcCnt, ulSvcNum, ulReplacedSvcCnt;
	HUINT32				 ulValue /*, nDataSize*/;
	Handle_t			 hHdSvcHandle, hSdSvcHandle;
	Handle_t				*phSvcArray;
	SesLcnReplaceInfo_t	*pstReplaceInfo;
	POINTER_LIST_T		*pstDeleteList;
	DbSvc_Info_t			 stSdSvcInfo, stHdSvcInfo;
	HERROR				 hErr;
	XmgrSearch_lcnupdate_Action_Contents_t	*pstContents;

	pstContents = xmgr_search_lcnupdate_Action_GetContents();
	if (pstContents == NULL)			{ return ERR_BUS_SOURCE_NULL; }

	switch (nCasMessage)
	{
		case eSEVT_CAS_SC_INSERTED :
			s_ucSimulConditionReady |= LCN_SIMUL_CONDITION_SC_INSERTED;
			break;

		case eMEVT_CAS_SYSINFO_UPDATED :
			if (s_ucSimulConditionReady & LCN_SIMUL_CONDITION_SC_INSERTED)		// SC insert 전에 올라오는 것이 있으면 무시하도록....
			{
				s_ucSimulConditionReady |= LCN_SIMUL_CONDITION_SC_SYSINFO;
			}
			break;
	}

	if (s_ucSimulConditionReady != LCN_SIMUL_CONDITION_READY)
	{
		return ERR_OK;
	}

	s_ucSimulConditionReady = LCN_SIMUL_CONDITION_INIT;

//	hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_SCINSERTED, &ulValue);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SCINSERTED, (HUINT32 *)&ulValue, 0);
	bHdCardCheckedSofar = (hErr == ERR_OK && ulValue == TRUE) ? TRUE : FALSE;

	if (bHdCardCheckedSofar == TRUE)
	{
		return ERR_OK;
	}

// SES HD+ Card가 꽃여 있는가?
#if defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	{
		HUINT32			 ulCasSlotIdx = 0, ulCasSlotNum = 0;
		HUINT8			 bHdCardInserted = FALSE;		// 현재 Smart Card가 들어 가 있는가를 나타내는 값

		hErr = SVC_CAS_GetNumOfSCSlot (&ulCasSlotNum);
		if (hErr == ERR_OK && ulCasSlotNum > 0)
		{
			for (ulCasSlotIdx = 0; ulCasSlotIdx < ulCasSlotNum; ulCasSlotIdx++)
			{
				//hErr = MW_CA_NA_IsHdPlusSmartCard ((HUINT8)ulCasSlotIdx, &bHdCardInserted);
				hErr = XMGR_CAS_NaIsHdPlusSmartCard ((HUINT8)ulCasSlotIdx, &bHdCardInserted);
				if (hErr == ERR_OK && bHdCardInserted != FALSE)
				{
					break;
				}
			}
		}

		if (bHdCardInserted == FALSE)
		{
			return ERR_OK;
		}
}
#endif

	// 일단 체크는 들어 왔으므로
//	DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_SCINSERTED, (HINT32)TRUE);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SCINSERTED, (HUINT32)TRUE, 0);
	DB_PARAM_Sync ();

//	hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_REPLACELATER_NUM, &ulValue);
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_REPLACELATER_NUM, (HUINT32 *)&ulValue, 0);
	ulSvcNum = (hErr == ERR_OK) ? (HUINT32)ulValue : 0;

	if (ulSvcNum == 0)
	{
		return ERR_OK;
	}

	pstReplaceInfo = (SesLcnReplaceInfo_t *)OxAP_Malloc (sizeof(SesLcnReplaceInfo_t) * ulSvcNum);
	if (pstReplaceInfo == NULL)
	{
		return ERR_OK;
	}

	bSuccess = FALSE;
	pstDeleteList = NULL;

	ulReplacedSvcCnt = 0;
	phSvcArray = NULL;

//	hErr = DB_SETUP_GetMenuItemBinary (eDxSETUP_SYSTEM_SES_REPLACELATER_ITEMS, &nDataSize, (HUINT8 *)pstReplaceInfo);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_REPLACELATER_ITEMS, (HUINT32 *)pstReplaceInfo, sizeof(SesLcnReplaceInfo_t) * ulSvcNum);
	if (hErr == ERR_OK /*&& nDataSize > 0*/)
	{
		phSvcArray = (Handle_t *)OxAP_Malloc (sizeof(Handle_t) * ulSvcNum);

		for (ulSvcCnt = 0; ulSvcCnt < ulSvcNum; ulSvcCnt ++)
		{
			hErr  = DB_SVC_GetServiceInfo (pstReplaceInfo[ulSvcCnt].hSdSvcHandle, &stSdSvcInfo);
			hErr |= DB_SVC_GetServiceInfo (pstReplaceInfo[ulSvcCnt].hHdSimulSvcHandle, &stHdSvcInfo);
			if (hErr == ERR_OK)
			{
				// LCN Number / Section 등만 기존 pre-programmed service 을 유지한다.
				DbSvc_SetLcn(&stHdSvcInfo, DbSvc_GetLcn(&stSdSvcInfo));
				DbSvc_SetSvcType(&stHdSvcInfo, DbSvc_GetSvcType(&stSdSvcInfo));
				DbSvc_SetSvcSection(&stHdSvcInfo, DbSvc_GetSvcSection(&stSdSvcInfo));
				DbSvc_SetLocked(&stHdSvcInfo, DbSvc_GetLocked(&stSdSvcInfo));

				hErr = DB_SVC_AddServiceInfo (&stHdSvcInfo, &hHdSvcHandle);
				if (hErr == ERR_OK)
				{
					pstDeleteList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDeleteList, (void *)pstReplaceInfo[ulSvcCnt].hSdSvcHandle);
					xmgr_search_lcnupdate_Action_ReplaceSvcRelatedInfos (pstReplaceInfo[ulSvcCnt].hSdSvcHandle, hHdSvcHandle);
					bSuccess = TRUE;

					phSvcArray[ulSvcCnt] = hHdSvcHandle;
					ulReplacedSvcCnt ++;
				}

				// Simulcast the favorite lists also
				xmgr_search_lcnupdate_Action_SimulcastFavoriteChannel (&stSdSvcInfo, &stHdSvcInfo);
			}
		}
	}

	if (ulReplacedSvcCnt > 0 && phSvcArray != NULL)
	{
		// DB에 New Service 를 추가해야 한다.
//		DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_REPLACESVC_NUM, (HINT32)ulReplacedSvcCnt);
//		DB_SETUP_SetMenuItemBinary (eDxSETUP_SYSTEM_SES_REPLACESVC_ITEMS, (sizeof(Handle_t) * ulReplacedSvcCnt), (HUINT8 *)phSvcArray);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, ulReplacedSvcCnt, 0);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_ITEMS, (HUINT32)phSvcArray, (sizeof(Handle_t) * ulReplacedSvcCnt));
		DB_PARAM_Sync ();
	}
	else
	{
//		DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_REPLACESVC_NUM, 0);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, 0, 0);
		DB_PARAM_Sync ();
	}

	if (phSvcArray != NULL)
	{
		OxAP_Free (phSvcArray);
	}

	// Delete를 작업중에 하면 TsInfo가 날아갈 위험이 있으므로, 다 추가된 다음 Delete를 한번에 하도록 한다.
	while (pstDeleteList != NULL)
	{
		hSdSvcHandle = (Handle_t)UTIL_LINKEDLIST_GetContents (pstDeleteList);
		DB_SVC_DeleteServiceInfo (hSdSvcHandle);
		pstDeleteList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstDeleteList, pstDeleteList, NULL);
	}

	if (bSuccess == TRUE)
	{
		DB_SVC_RemoveSearchFlag();

		// Create the LCN Replace Channel window
//		BUS_SendMessage (NULL, MSG_APPC2U_CREATE_APSLCN_REPLACE_CHANNEL, HANDLE_NULL, 0, 0, 0);		// have to check (by yjlim)
		xmgr_search_lcnupdate_Action_SendMsgReplaceSvc (pstContents);
	}

	BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, (eCheckDbType_SvcAdded | eCheckDbType_SvcDeleted), 0, 0);

	OxAP_Free (pstReplaceInfo);
//	DB_SETUP_SetMenuItemValue (eDxSETUP_SYSTEM_SES_REPLACELATER_NUM, 0);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_REPLACELATER_NUM, 0, 0);
	DB_PARAM_Sync ();

	return ERR_OK;
}


/* End of File. ---------------------------------------------------------- */

