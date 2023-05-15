/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_swup.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_SWUP_H__
#define	__XMGR_SWUP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
// DB
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>

/*******************************************************************/
// MW
/*******************************************************************/
#include <svc_pipe.h>
#include <isosvc.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_resources.h>
#include <xsvc_swupdate.h>

/*******************************************************************/
// MGR
/*******************************************************************/
#include <mgr_swup.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*			ASTRA
 */
void				*XMGR_SWUP_Create_Astra(void *pvData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Astra(void *pstContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Astra(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Astra(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Astra(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Astra(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Astra(DbSvc_TsInfo_t *pstHomeTsInfo);

/*			DVBSSU
 */
void				*XMGR_SWUP_Create_Dvbssu(void *pvUserData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Dvbssu(void *pstContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Dvbssu(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Dvbssu(DbSvc_TsInfo_t *pstHomeTsInfo);


/*			HUMAX
 */
void				*XMGR_SWUP_Create_Humax(void *pvUserData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Humax(void *pstHumaxContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Humax(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Humax(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Humax(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Humax(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Humax(DbSvc_TsInfo_t *pstHomeTsInfo);
HCHAR				*XMGR_SWUP_GetSwUpdateVersionString_Humax(HBOOL bDetected, HBOOL bStandby, HCHAR *pszCurVer, HCHAR *pszFoundVer);


/*			IRDETO
 */
void				*XMGR_SWUP_Create_Irdeto(void *pvUserData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Irdeto(void *pstFreesatContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Irdeto(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Irdeto(DbSvc_TsInfo_t *pstHomeTsInfo);

void				 XMGR_SWUP_SetControlByte_Irdeto (HUINT8 ucControlByte);
void				 XMGR_SWUP_GetControlByte_Irdeto (HUINT8 *pucControlByte);
void				 XMGR_SWUP_CheckDisableIrdetoOta_Irdeto (HUINT8 *pucMenuDisable, HUINT8 *pucEmmDisable);
void				 XMGR_SWUP_SetForcedOtaFlag_Irdeto (HUINT8 ucForcedOta);
void				 XMGR_SWUP_GetForcedOtaFlag_Irdeto (HUINT8 *puForcedOta);



/*			DTT
 */
void 				*XMGR_SWUP_Create_Dtt(void *pvUserData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Dtt(void *pvContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Dtt(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Dtt(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Dtt(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Dtt(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Dtt(DbSvc_TsInfo_t *pstHomeTsInfo);


/*			SES
 */
void				*XMGR_SWUP_Create_Ses(void *pvUserData, DxBatchType_e eBatchProcess);
HERROR 				 XMGR_SWUP_Destroy_Ses(void *pvContents);
BUS_Result_t		 XMGR_SWUP_EnterAction_Ses(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t		 XMGR_SWUP_LeaveAction_Ses(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
MgrSwup_State_e 	 XMGR_SWUP_GetNextState_Ses(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				 XMGR_SWUP_SetManualInfo_Ses(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo);
HERROR				 XMGR_SWUP_GetHomeTpInfo_Ses(DbSvc_TsInfo_t *pstHomeTsInfo);
HCHAR				*XMGR_SWUP_GetSwUpdateVersionString_Ses(HBOOL bDetected, HBOOL bStandby, HUINT8 ucSwupType, HCHAR *pszCurVer, HCHAR *pszFoundVer);

/* scenario functions */
extern DxSwUpdate_Source_e XMGR_SWUP_GetNextSource(DxSwUpdate_Source_e eCurSource);
extern DxSwUpdate_Source_e XMGR_SWUP_GetStartSource(void);
extern DxSwUpdate_Source_e XMGR_SWUP_GetStartSource_DvbSsu (DxBatchType_e eBatchType);

extern MgrSwup_State_e XMGR_SWUP_GetNextState_AfterDownload(DxSwUpdate_Source_e eSource);

/* debug functions */
extern const HINT8*	XMGR_SWUP_GetEnumStrOfMessage(HINT32 nMessage);
extern const HINT8*	XMGR_SWUP_GetEnumStrOfState(MgrSwup_State_e State);
extern const HINT8*	XMGR_SWUP_GetEnumStrOfSource(DxSwUpdate_Source_e eSource);
extern void XMGR_SWUP_PrintTuningInfo(DxDeliveryType_e eDeliType, DbSvc_TuningInfo_t *pstTuningParam);


#endif	/*	!__XMGR_SWUP_H__	*/
