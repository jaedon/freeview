/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_search.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_SEARCH_H___
#define	___MGR_SEARCH_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <mgr_common.h>
#include <mgr_tplist.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
// Seamless Search를 할 때 한 UI가 다룰 수 있는 Search Action 최대 수
#define MAX_SEARCH_ACTION_PER_UI		8

#define TP_NAME_LEN						128

#define	DEFAULT_NUM_PROVIDER			100
#define	DEFAULT_NUM_NETWORKS			50
#define	DEFAULT_NUM_TS					200
#define	DEFAULT_NUM_BOUQUET				20
#define	DEFAULT_NUM_SVC					1000
#define	DEFAULT_NUM_DELI_TS				20
#define DEFAULT_NUM_SRCHTP				200

#define APPEND_NUM_PROVIDER				50
#define	APPEND_NUM_NETWORKS				20
#define	APPEND_NUM_TS					50
#define	APPEND_NUM_BOUQUET				20
#define	APPEND_NUM_SVC					500
#define	APPEND_NUM_DELI_TS				20
#define APPEND_NUM_SRCHTP				100

#define	DEFAULT_REGION_SAT_INDEX		0

#define SERVICE_NUMBER_INVALID				0xFFFFFF

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	/* - Description : Search 내용을 화면에 Update 하라는 메시지 : Search Action이 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 시작할 Action Application (전체 혹은 무지정일 때에는 NULL)
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_ACTION_START			= eMEVT_SEARCH_START,

	/* - Description : Search action process를 죽이기 위한 메시지 : Search Action이 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 시작할 Action Application (전체 혹은 무지정일 때에는 NULL)
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_ACTION_KILL,

	/* - Description : Search 결과를 저장하라는 메시지 : Search Action이 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 시작할 Action Application (전체 혹은 무지정일 때에는 NULL)
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_SAVE_RESULT,

	/* - Description : Search Cancel을 명령하는 메시지 : Search Action이 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 :  N.A.
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_CANCEL,

	/* - Description : Search 상태가 바뀌었음을 나타내는 메시지. Search UI (Controller)가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 0
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_STATUS_CHANGED,

	/* - Description : 일련의 Search 작업이 시작되었음을 알리는 메시지 : Search UI가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Search가 종료된 Application Handle
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_STARTED,

	/* - Description : 일련의 Search 작업 (서비스 저장 외)가 종료 혹은 cancel 되었음을 알려주는 메시지 : Search UI가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Search가 종료된 Application Handle
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_FINISHED,

	/* - Description : Search Action에서 Save가 종료되었음을 알려 주는 메시지 : Search UI가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Search가 종료된 Application Handle
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_SAVE_FINISHED,

	/* - Description : Search Action에서 Channel List Update가 종료되었음을 알려 주는 메시지 : Search UI가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Search가 종료된 Application Handle
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_CHANNEL_UPDATED,

	/* - Description : Search Action이 destroy 되었음을 알리는 메시지 : Search UI가 받는다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Search가 종료된 Application Handle
		   p2 : 0
		   p3 : 0 */
	eMEVT_SEARCH_ACTION_DESTROYED,

	/* - Description : Antenna Connection Type Detection Action이 종료함을 알리는 메시지
	   - Parameters Usage :
		   handle : NULL.
		   p1 : ERR_OK : 성공, ERR_FAIL : 실패
		   p2 : NA
		   p3 : NA */
	eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED,


	eMEVT_LCNUPDATE_CMD_START,
	eMEVT_LCNUPDATE_CMD_SAVE,
	eMEVT_LCNUPDATE_CMD_STOP,
	eMEVT_LCNUPDATE_START_OK,
	eMEVT_LCNUPDATE_START_FAIL,
	eMEVT_LCNUPDATE_SAVE_OK,
	eMEVT_LCNUPDATE_SAVE_FAIL,
	eMEVT_LCNUPDATE_STOP_OK,
	eMEVT_LCNUPDATE_STOP_FAIL,
	eMEVT_LCNUPDATE_GET_INFO,
	eMEVT_LCNUPDATE_NOTIFY_DETECT_FINISH,

	eMEVT_TRDCONFLICTED_GET_INFO,
	eMEVT_NCD_GET_INFO,
};

typedef enum
{
	eSearchType_None = 0,

	eSearchType_Normal,
	eSearchType_TuneOnly,
	eSearchType_ExtSrch,

	eSearchType_Sat,
	eSearchType_Ter,
	eSearchType_Cab,
	eSearchType_Ext,

	// 추후 Terrestrial / Cable Search 에서 밑의 것들을 없앤다.
	// Auto / Manual 은 존재하지 않는다.
	eSearchType_Ter_Auto,
	eSearchType_Ter_Man,
	eSearchType_Cab_Auto,
	eSearchType_Cab_Man,

	eSearchType_MAX
} MgrSearch_Type_e;

typedef enum
{
	eSearchApp_state_init=0,
	eSearchApp_state_idle,				// 1

	eSearchApp_state_tryTuning,			// 2
	eSearchApp_state_locked,			// 3
	eSearchApp_state_waitingSi,			// 4
	eSearchApp_state_siReceived,		// 5
	eSearchApp_state_siFailed,			// 6
	eSearchApp_state_nextTp,			// 7

	eSearchApp_state_searching,			// 8
	eSearchApp_state_searchend,			// 9
	eSearchApp_state_saveResult,		// 10
	eSearchApp_state_cancel,			// 11
	eSearchApp_state_noSig_onSearching,	// 12
	eSearchApp_state_errorOccurred,		// 13

	SEARCHAPP_STATE_MAX				// 14
} MgrSearch_AppState_e;

// == Search Result Components

typedef struct tagSrchProvInfoArray
{
	HUINT32			 ulProviderNum;
	HUINT32			 ulCurrArraySize;
	DbSvc_PrvInfo_t	*astProviderInfo;
} MgrSearch_PrvInfoArray_t;

typedef struct tagNetInfoArray
{
	HUINT32			 ulNetworkNum;
	HUINT32			 ulCurrArraySize;
	HUINT32			*aulNetIdx;
	DbSvc_NetInfo_t	*astNetworkInfo;
} MgrSearch_NetInfoArray_t;

typedef struct tagSrchTsArray
{
	HUINT32			 ulTsNum;
	HUINT32			 ulCurrArraySize;
	DbSvc_TsInfo_t	*astTsInfo;
} MgrSearch_TsInfoArray_t;

typedef struct tagSrchBqArray
{
	HUINT32			 ulBouquetNum;
	HUINT32			 ulCurrArraySize;
	DbSvc_BouqInfo_t	*astBqInfo;
} MgrSearch_BqInfoArray_t;

typedef struct tagSrchSvcArray
{
	HUINT32			 ulServiceNum;
	HUINT32			 ulCurrArraySize;
	DbSvc_Info_t		*astSvcInfo;
} MgrSearch_SvcInfoArray_t;

#if 0
typedef struct tagSrchDeliveryTsArray
{
	HUINT32			 ulDeliTsNum;
	HUINT32			 ulCurrArraySize;
	DbSvc_TsInfo_t	*astTsInfo;
} MgrSearch_DeliveryTsInfoArray_t;
#endif

#if 0
// == Search Result Components

typedef struct tagSrchTuningArray
{
	HUINT32				 ulTpNum;
	HUINT32				 ulCurrArraySize;
	DbSvc_TuningInfo_t	*astTpInfo;
} MgrSearch_TuningInfoArray_t;

typedef struct tagSrchTuningList
{
	DbSvc_TuningInfo_t			stTpInfo;
	struct tagSrchTuningList	*pstNextList;
} MgrSearch_TuningInfoList_t;
#endif

// Search Menu 에서 Antenna 당 User TP, Tp 선택 위치 등
typedef enum
{
	eSEARCH_TP_ALL = 0,
	eSEARCH_TP_NONE,
	eSEARCH_TP_USERDEFINED,
	eSEARCH_TP_DEFAULT,

	eSEARCH_TP_TYPE_MAX
} MgrSearch_TpType_e;

typedef enum
{
	eSEARCH_NET_TABLE = 0,
	eSEARCH_NET_NETWORK,

	eSEARCH_NET_TYPE_MAX
} MgrSearch_NetType_e;

typedef struct
{
	HUINT8				 ucAvailable;
	HUINT8				 ucUserTpDefined;
	HUINT32				 ulAntIdx;
	MgrSearch_TpType_e	 eSearchTpType;
	HUINT32				 ulTpInfoIndex;			// 현재 Antenna 에서 몇 번째 Item을 선택했는가?
 	DxDeliveryType_e	 eDeliType;
	DbSvc_TuningInfo_t	 stUserTpInfo;
} MgrSearch_TpSelectInfo_t;

/*
	Search Options
*/
#if 0
#if defined(CONFIG_MW_CH_SATELLITE)
typedef struct
{
	HUINT32					 ulAntId;					// 현재 상태에서
	HCHAR					 szAntName[MAX_ANT_NAME];	// Satellite Search 일 경우 속하는 Antenna Name
	DbSvc_AntInfo_t				 stAntInfo;
} SearchAntInfo_t;

typedef struct
{
	HBOOL					 bDeleteAll;
	HCHAR					 szAntName[MAX_ANT_NAME];	// Satellite Search 일 경우 속하는 Antenna Name
} DeleteAntInfo_t;
#endif
#endif

typedef struct
{
 	DxDeliveryType_e		 eDeliType;					// Delivery Type : SAT/CAB/TER
	HUINT32					 eTpType;					// TP의 특정 성격 : Normal / Home TP/ 등등이 있을 수 있다.

	HCHAR					 szAntName[MAX_ANT_NAME];	// Satellite Search 일 경우 속하는 Antenna Name
	DbSvc_TuningInfo_t		 stTuningParam;
} MgrSearch_TpInfo_t;

#if 0
typedef struct
{
 	DxDeliveryType_e		 eDeliType;					// Delivery Type : SAT/CAB/TER
	HCHAR					 szAntName[MAX_ANT_NAME];	// Satellite Search 일 경우 속하는 Antenna Name
	HUINT32					 ulListId;					// Default TP List가 여러개 있을 경우 : 현재는 사용하지 않는다.
} MgrSearch_DefaultTpList_t;
#endif

typedef struct
{
	DxCasType_e				 eCasType;
	DxSvcType_e				 eSvcType;

	HBOOL					 bNetworkSearch;
	HUINT32					 eSearchType;		// SvcSi_SearchType_e
	HBOOL					 bReplaceChannel;
	HUINT16					 usNetwork_id;

	HUINT32					 ulTpNum;
	MgrSearch_TpInfo_t		*pstTpArray;
} MgrSearch_Option_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

BUS_Callback_t		MGR_SEARCH_GetSearchAction (HUINT32	ulActionId, DxDeliveryType_e eDeliveryType);
BUS_Callback_t		MGR_SEARCH_GetFinadSatAction (HUINT32 ulActionId);
HERROR				MGR_SEARCH_SetSearchMode (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pTuningInfo);
HERROR				MGR_SEARCH_GetSearchType (HUINT8 *pucSearchType);
HERROR				MGR_SEARCH_GetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
HERROR 				MGR_SEARCH_GetExtSearchSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec);
HERROR				MGR_SEARCH_GetNetworkIdAndSearchParam (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq);
HERROR				MGR_SEARCH_GetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);

HBOOL				MGR_SEARCH_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo (HUINT32 ulActionId, DxDeliveryType_e eDeliType);
BUS_Callback_t		MGR_SEARCH_GetSatConnectionAction (void);
BUS_Callback_t		MGR_SEARCH_GetLcnUpdateSearchAction (void);
BUS_Callback_t		MGR_SEARCH_GetLcnUpdateSearchMainProc (void);
HERROR			MGR_SEARCH_StartLcnUpdate(HUINT32 ulSearchMode);
HERROR			MGR_SEARCH_StopLcnUpdate(BUS_DestroyReason_t eDestroyReason);

// For Batch.
BUS_Callback_t		MGR_SEARCH_InitLcnUpdateBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_SEARCH_DeinitLcnUpdateBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_SEARCH_RescheduleLcnUpdateBatch(DxBatchType_e eBatchType);
HERROR 			MGR_SEARCH_GetBatchLcnUpdateCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR 			MGR_SEARCH_GetBatchLcnUpdateTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);



/*		Search Result
 */
HERROR				MGR_SEARCH_InitResult (HUINT32 ulActionId);
HERROR				MGR_SEARCH_ClearResult (HUINT32 ulActionId);
HERROR				MGR_SEARCH_ClearSearchSvcList (HUINT32 ulActionId);

HERROR				MGR_SEARCHDATA_InitProviderArray (MgrSearch_PrvInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_ClearProviderArray (MgrSearch_PrvInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_AddProvider (MgrSearch_PrvInfoArray_t *pstArray, DbSvc_PrvInfo_t *pstInfo);
HERROR				MGR_SEARCHDATA_CountProvider (MgrSearch_PrvInfoArray_t *pstArray, HUINT32 *pulNum);
DbSvc_PrvInfo_t*		MGR_SEARCHDATA_GetProvider (MgrSearch_PrvInfoArray_t *pstArray, HUINT32 ulIndex);
HERROR				MGR_SEARCHDATA_InitNetworkArray (MgrSearch_NetInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_ClearNetworkArray (MgrSearch_NetInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_AddNetwork (MgrSearch_NetInfoArray_t *pstArray, DbSvc_NetInfo_t *pstInfo, HUINT32 *pulIndex);
HERROR				MGR_SEARCHDATA_CountNetwork (MgrSearch_NetInfoArray_t *pstArray, HUINT32 *pulNum);
DbSvc_NetInfo_t*		MGR_SEARCHDATA_GetNetwork (MgrSearch_NetInfoArray_t *pstArray, HUINT32 ulIndex);
HERROR				MGR_SEARCHDATA_InitTsArray (MgrSearch_TsInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_ClearTsArray (MgrSearch_TsInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_AddTs (MgrSearch_TsInfoArray_t *pstArray, DbSvc_TsInfo_t *pstInfo, HUINT32 *pulIndex);
HERROR				MGR_SEARCHDATA_CountTs (MgrSearch_TsInfoArray_t *pstArray, HUINT32 *pulNum);
DbSvc_TsInfo_t*		MGR_SEARCHDATA_GetTsInfo (MgrSearch_TsInfoArray_t *pstArray, HUINT32 ulIndex);
HERROR				MGR_SEARCHDATA_InitBouquetArray (MgrSearch_BqInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_ClearBouquetArray (MgrSearch_BqInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_AddBouquet (MgrSearch_BqInfoArray_t *pstArray, DbSvc_BouqInfo_t *pstInfo);
HERROR				MGR_SEARCHDATA_CountBouquet (MgrSearch_BqInfoArray_t *pstArray, HUINT32 *pulNum);
DbSvc_BouqInfo_t*		MGR_SEARCHDATA_GetBouquet (MgrSearch_BqInfoArray_t *pstArray, HUINT32 ulIndex);

/* 사용하는 곳 없어 막음
HERROR				MGR_SEARCHDATA_InitServiceArray (MgrSearch_SvcInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_ClearServiceArray (MgrSearch_SvcInfoArray_t *pstArray);
HERROR				MGR_SEARCHDATA_AddService (MgrSearch_SvcInfoArray_t *pstArray, DbSvc_Info_t *pstSvcInfo);
HERROR				MGR_SEARCHDATA_CountService (MgrSearch_SvcInfoArray_t *pstArray, HUINT32 *pulNum);
DbSvc_Info_t*			MGR_SEARCHDATA_GetService (MgrSearch_SvcInfoArray_t *pstArray, HUINT32 ulIndex);
*/

HERROR				MGR_SEARCHRESULT_GetActionState (HUINT32 ulActionId, MgrSearch_AppState_e *peState);
HERROR				MGR_SEARCHRESULT_SetActionState (HUINT32 ulActionId, MgrSearch_AppState_e eState);
HERROR				MGR_SEARCHRESULT_GetNetwork (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo);
HERROR				MGR_SEARCHRESULT_SetNetwork (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo);
HERROR				MGR_SEARCHRESULT_GetTransponder (HUINT32 ulActionId, DxDeliveryType_e *peChType, DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_SEARCHRESULT_SetTransponder (HUINT32 ulActionId, DxDeliveryType_e eChType, DbSvc_TuningInfo_t stTuneInfo);
HERROR				MGR_SEARCHRESULT_GetNumTpCompleted (HUINT32 ulActionId, HUINT32 *pulNumTp);
HERROR				MGR_SEARCHRESULT_SetNumTpCompleted (HUINT32 ulActionId, HUINT32 ulTpNum);
HERROR				MGR_SEARCHRESULT_GetNumTpTotal (HUINT32 ulActionId, HUINT32 *pulNumTp);
HERROR				MGR_SEARCHRESULT_SetNumTpTotal (HUINT32 ulActionId, HUINT32 ulTpNum);
HERROR 				MGR_SEARCHRESULT_GetAntennaInfo (HUINT32 ulActionId, DbSvc_AntInfo_t *pstAntInfo);
HERROR 				MGR_SEARCHRESULT_SetAntennaInfo (HUINT32 ulActionId, DbSvc_AntInfo_t stAntInfo);
HERROR 				MGR_SEARCHRESULT_GetAntennaId (HUINT32 ulActionId, HUINT32 *pulAntId); // MOONSG_sss
HERROR 				MGR_SEARCHRESULT_SetAntennaId (HUINT32 ulActionId, HUINT32 ulAntId);

HERROR				MGR_SEARCHRESULT_AddOnTsId (HUINT32 ulActionId, HUINT16 usOnId, HUINT16 usTsId);
HERROR				MGR_SEARCHRESULT_AddService (HUINT32 ulActionId, DbSvc_Info_t *pstSvcInfo);
HERROR				MGR_SEARCHRESULT_CountService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum);
HERROR				MGR_SEARCHRESULT_GetService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo);
HERROR				MGR_SEARCHRESULT_GetServiceList (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulArrayNum, DbSvc_Info_t *pstSvcArray, HUINT32 *pulSvcNum);

/*		Util
 */
HERROR				MGR_SEARCHUTIL_GetUserSatTpInfo (DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_SEARCHUTIL_SetUserSatTpInfo (DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_SEARCHUTIL_GetSatTunerIdFromDb (HUINT32 *pulTunerId);
HERROR				MGR_SEARCHUTIL_SetSatTunerIdToDb (HUINT32 ulTunerId);
HERROR				MGR_SEARCHUTIL_GetSatAllAntFlagFromDb (HBOOL *pbSrchAllAnt);
HERROR				MGR_SEARCHUTIL_SetSatAllAntFlagFromDb (HBOOL bSrchAllAnt);
HERROR				MGR_SEARCHUTIL_GetSatAntennaIdFromDb (HUINT32 ulTunerId, HUINT32 *pulAntIdx);
HERROR				MGR_SEARCHUTIL_SetSatAntennaIdToDb (HUINT32 ulTunerId, HUINT32 ulAntIdx);
HERROR				MGR_SEARCHUTIL_GetSatTpIndexFromDb (HUINT32 *pulTpIndex);
HERROR				MGR_SEARCHUTIL_SetSatTpIndexToDb (HUINT32 ulTpIndex);
HERROR				MGR_SEARCHUTIL_GetSatNetworkSearchFromDb (HUINT32 *pulNetwork);
HERROR				MGR_SEARCHUTIL_SetSatNetworkSearchToDb (HUINT32 ulNetwork);
HERROR				MGR_SEARCHUTIL_GetSatScrambleTypeFromDb (HUINT32 *pulCasType);
HERROR				MGR_SEARCHUTIL_SetSatScrambleTypeToDb (HUINT32 ulCasType);
HERROR				MGR_SEARCHUTIL_GetSatChannelTypeFromDb (HUINT32 *pulChType);
HERROR				MGR_SEARCHUTIL_SetSatChannelTypeToDb (HUINT32 ulChType);
HERROR				MGR_SEARCHUTIL_GetSatSearchModeFromDb (HUINT32 *pulSearchMode);
HERROR				MGR_SEARCHUTIL_SetSatSearchModeToDb (HUINT32 ulSearchMode);
HERROR				MGR_SEARCHUTIL_GetSatAntennaTypeFromDb (HUINT32 *pulAntennaType);
HERROR				MGR_SEARCHUTIL_SetSatAntennaTypeToDb (HUINT32 ulAntennaType);
HERROR 				MGR_SEARCHUTIL_GetSatCountryIdx (HxCountry_e *peCountryId);
HERROR				MGR_SEARCHUTIL_GetMotorLnbFreqFromDb (HUINT32 *pulLnbFreq);
HERROR				MGR_SEARCHUTIL_SetMotorLnbFreqToDb (HUINT32 ulLnbFreq);
HERROR				MGR_SEARCHUTIL_GetMotorLnbPowerFromDb (HBOOL *pbLnbPower);
HERROR				MGR_SEARCHUTIL_SetMotorLnbPowerToDb (HBOOL bLnbPower);
HERROR				MGR_SEARCHUTIL_GetMotorTypeFromDb (DxSAT_DiseqcVer_e *peMotorType);
HERROR				MGR_SEARCHUTIL_SetMotorTypeToDb (DxSAT_DiseqcVer_e eMotorType);
HERROR				MGR_SEARCHUTIL_GetMotorLongitudeFromDb (HINT32 *pnLongitude);
HERROR				MGR_SEARCHUTIL_SetMotorLongitudeToDb (HINT32 nLongitude);
HERROR				MGR_SEARCHUTIL_GetMotorLatitudeFromDb (HINT32 *pnLatitude);
HERROR				MGR_SEARCHUTIL_SetMotorLatitudeToDb (HINT32 nLatitude);
HERROR				MGR_SEARCHUTIL_GetPositionSettingSatTypeFromDb (SVC_SatType_t *peSatType);
HERROR				MGR_SEARCHUTIL_SetPositionSettingSatTypeToDb (SVC_SatType_t eSatType);
HERROR				MGR_SEARCHUTIL_GetPositionSettingMotorDriveFromDb (CH_MotorDriveMode_t *peMotorDrive);
HERROR				MGR_SEARCHUTIL_SetPositionSettingMotorDriveToDb (CH_MotorDriveMode_t eMotorDrive);
HERROR				MGR_SEARCHUTIL_GetPositionSettingDriveSizeFromDb (HUINT32 *pulDriveSize);
HERROR				MGR_SEARCHUTIL_SetPositionSettingDriveSizeToDb (HUINT32 ulDriveSize);
HERROR				MGR_SEARCHUTIL_GetLimitSettingMotorLimitFromDb (HBOOL *pbLimitOnOff);
HERROR				MGR_SEARCHUTIL_SetLimitSettingMotorLimitToDb (HBOOL bLimitOnOff);
HERROR				MGR_SEARCHUTIL_GetLimitSettingLimitPositionFromDb (CH_MotorDirection_t *peMotorDir);
HERROR				MGR_SEARCHUTIL_SetLimitSettingLimitPositionToDb (HBOOL bLimitOnOff);

HERROR				MGR_SEARCHUTIL_GetTerTunerId (HUINT32 *pulTunerId);
HERROR				MGR_SEARCHUTIL_SetTerTunerId (HUINT32 ulTunerId);
HERROR				MGR_SEARCHUTIL_GetTerSearchType (HUINT32 *pulSrchType);
HERROR				MGR_SEARCHUTIL_SetTerSearchType (HUINT32 ulSrchType);
HERROR				MGR_SEARCHUTIL_GetUserTerTpInfo (DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_SEARCHUTIL_SetUserTerTpInfo (DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_SEARCHUTIL_GetTerNetworkSearchFromDb (HUINT32 *pulNetwork);
HERROR				MGR_SEARCHUTIL_SetTerNetworkSearchToDb (HUINT32 ulNetwork);
HERROR				MGR_SEARCHUTIL_GetTerNetworkIdFromDb (HUINT32 *pulNetId);
HERROR				MGR_SEARCHUTIL_SetTerNetworkIdToDb (HUINT32 ulNetId);
HERROR				MGR_SEARCHUTIL_GetTerCountryIdx (HxCountry_e *peCountryId);

HERROR 				MGR_SEARCHUTIL_GetCabTunerIdFromDb (HUINT32 *pulTunerId);
HERROR 				MGR_SEARCHUTIL_SetCabTunerIdToDb (HUINT32 ulTunerId);
HERROR 				MGR_SEARCHUTIL_GetCabFreqSearchFromDb (HUINT32 *pulFreq);
HERROR 				MGR_SEARCHUTIL_SetCabFreqSearchToDb (HUINT32 ulFreq);
HERROR 				MGR_SEARCHUTIL_GetCabSymbolRateSearchFromDb (HUINT32 *pulSR);
HERROR 				MGR_SEARCHUTIL_SetCabSymbolRateSearchToDb (HUINT32 ulSR);
HERROR 				MGR_SEARCHUTIL_GetCabModulationSearchFromDb (HUINT32 *pulCon);
HERROR 				MGR_SEARCHUTIL_SetCabModulationSearchToDb (HUINT32 ulCon);

HERROR				MGR_SEARCHUTIL_GetCabLinearStartFreqSearchFromDb (HUINT32 *pulFreq);
HERROR				MGR_SEARCHUTIL_SetCabLinearStartFreqSearchToDb (HUINT32 ulFreq);
HERROR				MGR_SEARCHUTIL_GetCabLinearStopFreqSearchFromDb (HUINT32 *pulFreq);
HERROR				MGR_SEARCHUTIL_SetCabLinearStopFreqSearchToDb (HUINT32 ulFreq);
HERROR				MGR_SEARCHUTIL_GetCabLinearStepSearchFromDb (HUINT32 *pulFreq);
HERROR				MGR_SEARCHUTIL_SetCabLinearStepSearchToDb (HUINT32 ulStep);
HERROR 				MGR_SEARCHUTIL_GetCabLinearModulationSearchFromDb (HUINT32 *pulCon);
HERROR 				MGR_SEARCHUTIL_SetCabLinearModulationSearchToDb (HUINT32 ulCon);
HERROR				MGR_SEARCHUTIL_GetCabLinearSymbolRateSearchFromDb (HUINT32 *pulSR);
HERROR				MGR_SEARCHUTIL_SetCabLinearSymbolRateSearchToDb (HUINT32 ulSR);

HERROR				MGR_SEARCHUTIL_GetCabNetworkSearchFromDb (HUINT32 *pulNetwork);
HERROR				MGR_SEARCHUTIL_SetCabNetworkSearchToDb (HUINT32 ulNetwork);
HERROR				MGR_SEARCHUTIL_GetCabNetworkIdFromDb (HUINT32 *pulNetworkId);
HERROR				MGR_SEARCHUTIL_SetCabNetworkIdToDb (HUINT32 ulNetworkId);
HERROR				MGR_SEARCHUTIL_GetCabScrambleTypeFromDb (HUINT32 *pulCasType);
HERROR				MGR_SEARCHUTIL_SetCabScrambleTypeToDb (HUINT32 ulCasType);
HERROR				MGR_SEARCHUTIL_GetCabChannelTypeFromDb (HUINT32 *pulChType);
HERROR				MGR_SEARCHUTIL_SetCabChannelTypeToDb (HUINT32 ulChType);
HERROR 				MGR_SEARCHUTIL_GetCabSearchModeFromDb (HUINT32 *pulSearchMode);
HERROR 				MGR_SEARCHUTIL_SetCabSearchModeFromDb (HUINT32 ulSearchMode);
HERROR 				MGR_SEARCHUTIL_GetSearchNetworkTypeFromDb (HUINT32 *pulNetworkType);	/* XmgrSearch_CabNetworkType_e */
HERROR 				MGR_SEARCHUTIL_SetSearchNetworkTypeToDb (HUINT32 ulNetworkType);

HERROR 				MGR_SEARCHUTIL_GetCabNetworkSrchModeFromDb (HUINT32 *pulSearchMode);
HERROR 				MGR_SEARCHUTIL_SetCabNetworkSrchModeToDb (HUINT32 ulSearchMode);

HERROR 				MGR_SEARCHUTIL_SetDTSearchModeToDb (HUINT32 ulSearchMode);
HERROR 				MGR_SEARCHUTIL_GetDTSearchModeFromDb (HUINT32 *pulSearchMode);
HERROR 				MGR_SEARCHUTIL_SetDTSearchStartFreqToDb (HUINT32 ulFreq);
HERROR 				MGR_SEARCHUTIL_GetDTSearchStartFreqFromDb (HUINT32 *pulFreq);
HERROR 				MGR_SEARCHUTIL_SetDTSearchEndFreqToDb (HUINT32 ulFreq);
HERROR 				MGR_SEARCHUTIL_GetDTSearchEndtFreqFromDb (HUINT32 *pulFreq);
HERROR				MGR_SEARCHUTIL_SetDTSearchChNumToDb (HUINT32 ulChNum);
HERROR 				MGR_SEARCHUTIL_GetDTSearchChNumFromDb (HUINT32 *pulChNum);

HERROR				MGR_SEARCHUTIL_ConvertSatSvcTuningInfoToChTuningParam (HUINT32 ulTunerId,
																			DbSvc_AntInfo_t *pstAntInfo,
																			DbSvc_TuningInfo_t *pstSvcTuneInfo,
																			DxTuneParam_t *pstChTuneInfo);
HERROR				MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (DxTuneParam_t *pstChTuneInfo,
																		DbSvc_TuningInfo_t *pstSvcTuneInfo);
HERROR				MGR_SEARCHUTIL_MakeDbNetInfoFromSi(Handle_t hAction, HUINT16 usNetworkId, DbSvc_NetInfo_t *pstDbNetworkInfo);


HERROR				MGR_SEARCHUTIL_GetTpSelectionInfo (HUINT32 ulAntIdx, MgrSearch_TpSelectInfo_t *pstTpSelectInfo);
HERROR				MGR_SEARCHUTIL_ResetTpSelectionInfo (void);
HERROR				MGR_SEARCHUTIL_UpdateTpSelectionInfo (MgrSearch_TpSelectInfo_t *pstTpSelectInfo);
HERROR				MGR_SEARCHUTIL_SyncTpSelectionInfo (void);

HBOOL				MGR_SEARCHUTIL_CompareSearchedSvcType (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType);
Handle_t			MGR_SEARCHUTIL_MakeCurrentAppHandle (void);

#endif /* !___MGR_SEARCH_H___ */

