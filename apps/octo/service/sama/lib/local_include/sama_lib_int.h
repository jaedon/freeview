/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_lib_int.h
	@brief

	Description:  									\n
	Module: SAMA		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#ifndef __SAMA_LIB_INT_H__
#define __SAMA_LIB_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "../sama_lib.h"
#include <dapi.h>
#include <dlib.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/


// sama_spec.c

// sama_time.c

// sama_time_base.c
extern HERROR samalib_time_base_GetPrevTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putPrevTime);
extern HERROR samalib_time_base_GetNextTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putNextTime);
extern HERROR samalib_time_base_GetRepeatedScheduleStartTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, UNIXTIME *putStartTime);
extern HERROR samalib_time_base_AdjustScheduleTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, HBOOL *pbUpdated);
extern HERROR samalib_time_base_GetPaddingStartEndTime (DxSchedule_t *pstSchedule, UNIXTIME utStartTime, UNIXTIME *putStartTime, UNIXTIME *putEndTime);

// sama_conflict.c
extern HINT32 samalib_cflt_cbCmpCfltTimeEvtByTime (void *pvUser, void *pvListItem);
extern HERROR samalib_cflt_DuplicateConflictResult (SAPI_Conflict_t *pstDstConflict, SAPI_Conflict_t *pstSrcConflict);
//extern HERROR sama_cflt_AppendConflictResult (SAPI_Conflict_t *pstConflict, SAPI_ConflictItem_t *pstItem);
extern HERROR samalib_cflt_ClearActions (sama_ConflictCheck_t *pstInst);
extern HERROR samalib_cflt_GetActionPriority(sama_ActionState_e eState, HUINT32 *pulPriority);
extern sama_ActionItem_t *samalib_cflt_TakeAction (sama_ConflictCheck_t *pstInst, HUINT32 ulPriority);
extern void samalib_cflt_ReleaseAction (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction);
extern sama_ActionItem_t *samalib_cflt_GetActionByActionId (sama_ConflictCheck_t *pstInst, HUINT32 ulActionId);
extern HUINT32 samalib_cflt_CountActionByActionType (sama_ConflictCheck_t *pstInst, SAPI_ActionType_e eActType);
extern sama_ActionItem_t *samalib_cflt_GetActionByActionType (sama_ConflictCheck_t *pstInst, SAPI_ActionType_e eActType, HUINT32 ulIndex);
extern HUINT32 samalib_cflt_CountActionByActionState (sama_ConflictCheck_t *pstInst, sama_ActionState_e eState);
extern sama_ActionItem_t *samalib_cflt_GetActionByActionState (sama_ConflictCheck_t *pstInst, sama_ActionState_e eState, HUINT32 ulIndex);
extern sama_ActionItem_t *samalib_cflt_GetActionBySlotId (sama_ConflictCheck_t *pstInst, HUINT32 ulSlot);
extern HERROR samalib_cflt_ForEachAction (sama_ConflictCheck_t *pstInst, SAMA_ForEachResult_e (*pfCbFunc)(sama_ActionItem_t *pstAction, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv);
extern HERROR samalib_cflt_CheckConflictUsingTimeLine (sama_ConflictCheck_t *pstInst);
extern HERROR samalib_cflt_MakeConflictTimeLine (sama_ConflictCheck_t *pstInst, HBOOL bOmitOneSch, HUINT32 ulSlotToOmit);
extern sama_CnftTimeEvt_t *samalib_cflt_FindFirstRelatedTimeLineEvent (DxSchedule_Spec_e eSapiSpec, HxList_t *pstTimeLine, HUINT32 ulBaseTime);
extern HERROR samalib_cflt_TakeResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_TakeResource2 (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule, HxList_t *pstRemovableActList);
extern HERROR samalib_cflt_GetTpInfo (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder);
extern HxList_t *samalib_cflt_GetConflictedActionList (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);
extern sama_ActionItem_t *samalib_cflt_TryStartAction (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, sama_ActionState_e eState, SAPI_Conflict_t *pstConflict);
extern HERROR samalib_cflt_TryResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);


// sama_conflict_base.c
extern SAPI_ActionType_e samalib_cflt_base_getSapiActionTypeBySchedule (DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_base_CheckStreamingConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_base_CheckLiveConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_base_CheckRecConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_base_CheckConflictUsingTimeLine (sama_ConflictCheck_t *pstInst);
extern HERROR samalib_cflt_base_MakeConflictTimeLine (sama_ConflictCheck_t *pstInst, HBOOL bOmitOneSch, HUINT32 ulSlotToOmit);
extern sama_CnftTimeEvt_t *samalib_cflt_base_FindFirstRelatedTimeLineEvent (HxList_t *pstTimeLine, HUINT32 ulBaseTime);
extern HERROR samalib_cflt_base_TakeResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);
extern HERROR samalib_cflt_base_GetTpInfoUsingSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder);
extern HERROR samalib_cflt_base_GetTpInfoUsingSvcUid (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder);
extern HERROR samalib_cflt_base_TakeResource2 (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule, HxList_t *pstRemovableActList);
extern HxList_t *samalib_cflt_base_GetConflictedActionList (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);
extern sama_ActionItem_t *samalib_cflt_base_TryStartAction (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, sama_ActionState_e eState, SAPI_Conflict_t *pstConflict);
extern HERROR samalib_cflt_base_TryResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule);


//sama_service.c
extern sama_ServiceCache_t *samalib_svc_GetCache (sama_ConflictCheck_t *pstInst);
extern HERROR samalib_svc_GetService (sama_ConflictCheck_t *pstInst, HUINT32 ulSvcUid, DxService_t *pstService);
extern HERROR samalib_svc_GetTransponder (sama_ConflictCheck_t *pstInst, HUINT32 ulTsUid, DxTransponder_t *pstTransponder);
extern HERROR samalib_svc_GetAntenna (sama_ConflictCheck_t *pstInst, HUINT32 ulAntUid, DxAntInfo_t *pstAntInfo);
extern HERROR samalib_svc_GetNetwork (sama_ConflictCheck_t *pstInst, HUINT32 ulNetUid, DxNetwork_t *pstNetInfo);

// Deviated Headers:


#endif	// __SAMA_LIB_INT_H__
