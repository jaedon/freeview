/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_cas.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_CAS_H___
#define	___PMGR_CAS_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_cas.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

BUS_Result_t	pmgr_cas_CreateSubCasProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			pmgr_cas_Init (void);
HERROR 			pmgr_cas_InitInActionMgr (void);
HERROR			pmgr_cas_DefaultSetting (void);
HERROR			pmgr_cas_TotTdtUpdated (void);
HERROR			pmgr_cas_GetCaSystemId (Handle_t hAction, HUINT32 *pucCaSysId);
HERROR			pmgr_cas_OnShowProgramCasControl (Handle_t hAction, Handle_t hSvc, HUINT32 *pulMsg, HUINT32 *pulParam1, HUINT32 *pulParam2, HUINT32 *pulParam3);
HERROR			pmgr_cas_IsSmartcardRemoved (HUINT8 ucSlotIndex, HBOOL *pbRemoved);

HBOOL 			pmgr_cas_NeedCasRatingCheck(void);
HBOOL 			pmgr_cas_IsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating);
HBOOL 			pmgr_cas_NeedCasRatingWaitCheck(void);
HBOOL 			pmgr_cas_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo);

HERROR 			pmgr_cas_GetRatingFromCasMsg(void *pvCasMsg, HUINT32 *pulRating);
HERROR 			pmgr_cas_IsSameCasIdInSc(HUINT8 ucSlotId, HUINT16 usCaSysId, HUINT8 *pbSame);

void			pmgr_cas_CmdInit(HxCMD_Handle_t hCmdHandle);

#endif /* !___PMGR_CAS_H___ */

