/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_swup.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_SWUP_H___
#define	___MGR_SWUP_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <svc_swup.h>

#include <mgr_common.h>
#include <mgr_tplist.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	SWUPDATE_MODULE_NAME_LEN	32

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_SWUP		= eMEVT_SWUP_START,

	/* - Description : set Software Update State
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : MgrSwup_State_e	: State to Transit
				   p2 : SWUP_SetParam_t	*
				   p3 : 0 () */
	eMEVT_SWUP_SET_STATE,

	/* - Description : Post Message to SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : MgrSwup_State_e	: eSWUP_State_Unknown...
				   p2 : SWUP_SetParam_t	*
				   p3 : 0 () */
	eMEVT_SWUP_STATE_POST_MSG,

	/* - Description : Post Message to SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_INIT,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_CONNECT_START,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */

	eMEVT_SWUP_CONNECT_SUCCESS,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_CONNECT_FAIL,


	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : DxSwUpdate_Source_e
				   p2 : SvcSwUpdate_DetectParamIp_t or SvcSwUpdate_DetectParamRf_t
				   p3 : TBD */
	eMEVT_SWUP_DETECT_START,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_DETECT_STOP,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_DETECT_SUCCESS,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : OTA TS Info ( DbSvc_TsInfo_t )
				   p2 : OTA Signal Info ( SvcSi_SwupSignal_t )
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_DETECT_FAIL,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_DOWNLOAD_START,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : DxSwUpdate_Source_e
				   p2 : SvcSwUpdate_DownloadSourceInfoIp_t or SvcSwUpdate_DownloadSourceInfoRf_t
				   p3 : TBD */
	eMEVT_SWUP_DOWNLOAD_STOP,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_CANCEL,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */

	eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : progress
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_INSTALL_START,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_INSTALL_STOP,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_INSTALL_SUCCESS,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_INSTALL_FAIL,

	/* - Description : Message for SW Statemachine
			   - Parameters Usage :
				   handle : HUINT32 ulTag
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS,


	eMEVT_DN_SWUP_CONNECT_SUCCESS,
	eMEVT_DN_SWUP_CONNECT_FAIL,
	eMEVT_DN_SWUP_DETECT_SUCCESS,
	eMEVT_DN_SWUP_DETECT_FAIL,
	eMEVT_DN_SWUP_DOWNLOAD_SUCCESS,
	eMEVT_DN_SWUP_DOWNLOAD_FAIL,
	eMEVT_DN_SWUP_DOWNLOAD_PROGRESS,
	eMEVT_DN_SWUP_INSTALL_SUCCESS,
	eMEVT_DN_SWUP_INSTALL_FAIL,
	eMEVT_DN_SWUP_INSTALL_PROGRESS,
	eMEVT_DN_SWUP_ERROR,
	eMEVT_DN_SWUP_FINISH,
};

typedef enum
{
	eSWUP_TYPE_Detect				= 0,
	eSWUP_TYPE_Download,
	eSWUP_TYPE_Install,
} MgrSwup_Type_e;

typedef enum
{
	eOtaApp_state_init			= 0,
	eOtaApp_state_idle,					// 1
	eOtaApp_state_tryTuning,			// 2
	eOtaApp_state_locked,				// 3
	eOtaApp_state_waitingSi,			// 4
	eOtaApp_state_siReceived,			// 5
	eOtaApp_state_siFailed,				// 6
	eOtaApp_state_nextTp,				// 7

	eOtaApp_state_checking,				// 8
	eOtaApp_state_checkend,				// 9
	eOtaApp_state_cancel,				// 11
	eOtaApp_state_errorOccurred,		// 13
	OTA_APP_STATE_MAX					// 14
} MgrSwup_AppState_t;

typedef struct
{
	Handle_t				hSwUpdate;
	DxSwUpdate_Spec_e	eSwUpdateSpec;
	HUINT32					ulSignalVer;
	HUINT16					usPid;
} MgrSwup_DownloadArg_t;

typedef enum
{
	eSWUP_STATE_Unknown			= -1,
	eSWUP_STATE_Init			= 0,
	eSWUP_STATE_Connect,		// 1
	eSWUP_STATE_Detect,			// 2
	eSWUP_STATE_Download,		// 3
	eSWUP_STATE_Install,		// 4
	eSWUP_STATE_Error,			// 5
	eSWUP_STATE_Finish,			// 6
	eSWUP_STATE_MAX				// 7
} MgrSwup_State_e;

typedef enum
{
	eSWUP_FAILREASON_Unknown			= 0,
	eSWUP_FAILREASON_Latest_Version,		// Current installed SW is latest version. (Signalled SW version is same or lower than installed SW.)
	eSWUP_FAILREASON_Action,
	eSWUP_FAILREASON_Max,
} MgrSwup_FailReason_e;


typedef BUS_Result_t (* pfnEnterAction_t)(Handle_t hSwUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
typedef BUS_Result_t (* pfnLeaveAction_t)(Handle_t hSwUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
typedef MgrSwup_State_e (* pfnGetNextState_t)(Handle_t hSwUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// SW Update Create
typedef void *		(* pfnSwModuleCreate_t)(void *pvUserSetInfo, DxBatchType_e	eBatchProcess);
typedef HERROR		(* pfnSwModuleDestroy_t)(void *pvModuleData);
typedef HERROR		(* pfnSwModuleSetManual_t)(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
typedef HERROR		(* pfnSwModuleGetHomeTpInfo_t)(DbSvc_TsInfo_t *pstHomeTsInfo);

typedef struct tagSwupdateCallback
{
	pfnSwModuleCreate_t			pfnCreate;
	pfnEnterAction_t			pfnEnterAction;
	pfnLeaveAction_t			pfnLeaveAction;
	pfnGetNextState_t			pfnGetNextState;
	pfnSwModuleDestroy_t 		pfnDestroy;
	pfnSwModuleSetManual_t		pfnSetManualInfo;
	pfnSwModuleGetHomeTpInfo_t	pfnGetHomeTpInfo;
} MgrSwup_ModuleCb_t;

typedef struct tagSwupdateInfo
{
	HCHAR				szModuleName[SWUPDATE_MODULE_NAME_LEN];
	MgrSwup_ModuleCb_t	stCallback;
} MgrSwup_ModuleInfo_t;

/*
	신 OTA 관련 정보 :
*/
typedef struct
{
	DxSwUpdate_Spec_e		 eSwUpdateSpec;
} MgrSwup_InArg_t;

typedef struct _UsbOtaContents
{
	Handle_t	hSwUpdate;
	HBOOL		bFoundImagePath;
	HCHAR		szSwImageFullPath[MAX_PATH_LENGTH];
}MgrSwup_UsbOtaContents_t;


typedef enum
{
	eSWDN_SRC_UNKNOWN,

	eSWDN_SRC_RF,
	eSWDN_SRC_NET,
	eSWDN_SRC_FILE,

	eSWDN_SRC_MAX
} SWDN_Source_e;

typedef struct
{
	SWDN_Source_e	eSrc;
	HINT32			nType;

	HCHAR			*json;
} SWDN_Param_t;


typedef enum
{
	eSWDN_RESULT_OK,
	eSWDN_RESULT_FAIL,
	eSWDN_RESULT_UPDATE,

	eSWDN_RESULT_MAX
} SWDN_Result_e;


typedef void 	(* SWDN_Response_t) (
						Handle_t hModule,
						SWDN_Result_e eResult,
						const SWDN_Param_t *p
					);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*		Start Manager
 */
HERROR			MGR_SWUP_Start(void);
HERROR			MGR_SWUP_Stop(void);

/*		For Batch.
 */
BUS_Callback_t		MGR_SWUP_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType); // Batch 에서만 사용할 init 함수
HERROR			MGR_SWUP_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_SWUP_RescheduleBatch(DxBatchType_e eBatchType);

/*	SWUP State Module
 */
HERROR			MGR_SWUP_RegisterModuleInfo(MgrSwup_ModuleInfo_t *pstModuleInfo);

Handle_t 			MGR_SWUP_Create(void *pvUserSetInfo, DxBatchType_e	eBatchProcess);
// 다음  MGR_SWUP_Destroy  함수는 s/w update 정지시 call 되도록 한다.
HERROR			MGR_SWUP_Destroy(Handle_t hSwUpdate);
HBOOL			MGR_SWUP_IsExec(Handle_t hSwUpdate);
HUINT32 			MGR_SWUP_GetSource(Handle_t hSwUpdate);
// Manual Detect 시 각 operator 별 manual 정보 셋팅. 및 수정.
HERROR			MGR_SWUP_SetManual(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
// Home Tp Set
HERROR			MGR_SWUP_GetHomeTpInfo(DbSvc_TsInfo_t *pstHomeTsInfo);

HERROR 			MGR_SWUP_InitState (Handle_t hSwUpdate, HINT32 p1, HINT32 p2, HINT32 p3);
HUINT32			MGR_SWUP_GetStartUpdateSource(DxBatchType_e eBatchType);
HERROR 			MGR_SWUP_GetUserData (Handle_t hSwUpdate, void **ppvUserData);
HERROR 			MGR_SWUP_PostMessage (HUINT32 ulStateTag, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR 			MGR_SWUP_PostMessageExt (Handle_t hSwUpdate, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/*	USB SW Update
 */
HERROR			MGR_SWUP_UsbCreate(HUINT32 ulTag);
HERROR			MGR_SWUP_UsbDestroy(void);

HERROR			MGR_SWUP_CreateUsb(HUINT32 ulTag);
HERROR			MGR_SWUP_DestroyUsb(void);
HERROR			MGR_SWUP_FindUsbSwImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents);

#if 0
/*	OTA WakeUp
 */
HERROR			MGR_SWUP_WakeupStart (void);
HERROR			MGR_SWUP_WakeupStop (void);
#endif

/*	OTA Download
 */
HERROR			MGR_SWUP_DownloadCreate (Handle_t hActHandle, DxSwUpdate_Source_e eSource, HUINT32 ulSignalver, HUINT16 usPid, Handle_t hSwUpdate, HBOOL bStartByMenu);
HERROR			MGR_SWUP_DownloadDestroy (DxSwUpdate_Source_e eSource);

/*	Install
 */
HERROR			MGR_SWUP_InstallStart (Handle_t hSWUpdate, HUINT8 *pszSourceFile, SvcSwUpdate_EmergencyUpdateInfo_t *pstSourceInfo);
HERROR			MGR_SWUP_InstallStop (void);

/*	SW Update Detect
 */
HERROR			MGR_SWUP_DetectCreate (HINT32 nTunerId, HUINT32 ulTag);
HERROR			MGR_SWUP_DetectDestroy (void);

HERROR			MGR_SWUP_SetDefaultTp (void);
HERROR			MGR_SWUP_GetSpec (DxSwUpdate_Spec_e *peSwUpdateSpec);
DxSwUpdate_Spec_e MGR_SWUP_GetMultipleSpec(DxBatchType_e eBatchType);
HERROR			MGR_SWUP_SetSpec (DxSwUpdate_Spec_e eSwUpdateSpec);
HERROR			MGR_SWUP_GetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);

HBOOL			MGR_SWUP_CheckProc(MgrSwup_Type_e eType, HINT32 nParam, BUS_Callback_t pfnProc);
HBOOL			MGR_SWUP_CheckMainProcExist(void);

/*	Detect Module
 */
HERROR			MGR_SWUP_GetDetectInfo (Handle_t hSwUpdate, SvcSwUpdate_DetectResult_t *pstDetectResult);
HERROR			MGR_SWUP_SetDetectInfo (Handle_t hSwUpdate, SvcSwUpdate_DetectResult_t *pstDetectResult);

/*	Ip Module
 */
HERROR			MGR_SWUP_IpCreate(Handle_t hSwUpdate, MgrSwup_State_e eState, HCHAR *pszServerUrl);
HERROR			MGR_SWUP_IpDestroy(Handle_t hSwUpdate);

/*	SW Info
 */
// Home Tp
HERROR			MGR_SWUPINFO_ResetHomeTp (void);
HERROR			MGR_SWUPINFO_AddOtaHomeTp (DbSvc_TsInfo_t *pstTsInfo);
HUINT32			MGR_SWUPINFO_CountOtaHomeTp (void);
HERROR			MGR_SWUPINFO_GetOtaHomeTp (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo);
// Searched Info
HERROR			MGR_SWUPINFO_ResetSearchedInfo (void);
HERROR			MGR_SWUPINFO_SetSearchedInfo (DbSvc_TsInfo_t *pstTsInfo, HUINT32 ulOtaInfoLenght, void *pvOtaInfo);		// SvcSi_SwupSignal_t *pstOtaInfo);
HERROR			MGR_SWUPINFO_GetSearchedInfo (DbSvc_TsInfo_t *pstTsInfo, HUINT32 ulOtaInfoLength, void *pvOtaInfo);
// Wakeup Flag
HERROR			MGR_SWUPINFO_SetWakeUpOtaFlag (HINT32 nWakeUpOta);
HERROR			MGR_SWUPINFO_GetWakeUpOtaFlag (HINT32 *pnWakeUpOta);

HBOOL			MGR_SWUPINFO_IsOtaFound (HBOOL bSkipUserCancelled);
#if 0	// 현재 사용하는 곳 없음... 아래와 같은 함수 말들지 말기.. 위 함수에 Param 추가해서 처리...
HBOOL			MGR_SWUPINFO_IsOtaFoundWithoutFlag (HBOOL bSkipUserCancelled);
#endif

// Retry
HERROR			MGR_SWUPINFO_GetOtaRetryCount (HUINT16 *pusRetryOtaType, HUINT32 *pulRetryOtaVer, HUINT32 *pulRetryCount);
HERROR			MGR_SWUPINFO_ResetOtaRetryCount (void);
HERROR			MGR_SWUPINFO_IncreaseOtaRetryCount (HUINT16 usRetryOtaType, HUINT32 ulRetryOtaVer);
HERROR			MGR_SWUPINFO_GetOtaCanceledVersion (HUINT16 *pusCanceledType, HUINT32 *pulCanceledVer);
HERROR			MGR_SWUPINFO_SetOtaCanceledVersion (HUINT16 usCanceledType, HUINT32 ulCanceledVer);

HERROR			MGR_SWUPINFO_GetNextOtaTime (HUINT32 ulHour, HUINT32 ulMin, UNIXTIME *putNextTime);
HERROR			MGR_SWUPINFO_SetDailyOta (HUINT32 ulHour, HUINT32 ulMin);

HERROR			MGR_SWUP_GetNextBurstOtaSchedule(UNIXTIME *startTime);

// Standby
HERROR			MGR_SWUPINFO_ResetStandbyOtaInfo (void);
HERROR			MGR_SWUPINFO_GetStandbyOtaInfo (SWUP_DownloadType_e *peOtaType, DbSvc_TsInfo_t *pstTsInfo, SvcSi_SwupSignal_t *pstOtaInfo);
HERROR			MGR_SWUPINFO_SetStandbyOtaInfo (SWUP_DownloadType_e eOtaType, DbSvc_TsInfo_t *pstTsInfo, SvcSi_SwupSignal_t *pstOtaInfo);

// Signal
HERROR			MGR_SWUPINFO_ResetSignal (void);
HINT32			MGR_SWUPINFO_AddSignal (const SvcSi_SwupSignal_t *pstOtaInfo);
HERROR			MGR_SWUPINFO_GetSignal (HINT32 nKey, SvcSi_SwupSignal_t *pstOtaInfo);

HERROR 			MGR_SWUP_GetBatchOtaCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR 			MGR_SWUP_GetBatchOtaTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);

MgrSwup_FailReason_e MGR_SWUP_ConvertFailReasonSvctoMgr(SvcSi_OtaFailReason_e eSvcFailreason);

#endif /* !___MGR_SWUP_H___ */

