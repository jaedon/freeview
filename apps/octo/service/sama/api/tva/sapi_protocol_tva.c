/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sapi_protocol_tva.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>
#include <sapi.h>

#include <sapi_int.h>
#include <sama_lib.h>

/*******************************************************************/
/********************      Local Definition         *************************/
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

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
#define _____REGISTRATION_FUNCTIONS_____
HERROR sapi_tva_RegisterRpcCallback (sapi_Inst_t *pstInst)
{
	if (NULL == pstInst)					{ return ERR_FAIL; }

	// sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onResourceInfoSet", "b", (void*)pstInst, sapi_base_OnResourceInfoSet);

	// sama.onRfInputAbilitySet (int nRfInputId, int bAbility)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onRfInputAbilitySet", "ii", (void*)pstInst, sapi_base_OnRfInputAbilitySet);

	// sama.onAvailableSlotsChanged (int nPid, int nSlotNum, int *anSlotArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onAvailableSlotsChanged", "iib", (void*)pstInst, sapi_base_OnAvailableSlotsChanged);

	// sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onSchedulesLoaded", "ib", (void*)pstInst, sapi_base_OnSchedulesLoaded);

	// sama.onScheduleAdded (int nSlotId, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleAdded", "ib", (void*)pstInst, sapi_base_OnScheduleAdded);

	// sama.onScheduleChanged (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleChanged", "iib", (void*)pstInst, sapi_base_OnScheduleChanged);

	// sama.onScheduleChangedEx (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleChangedEx", "ib", (void*)pstInst, sapi_base_OnScheduleChangedEx);

	// sama.onScheduleRemoved (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleRemoved", "ib", (void*)pstInst, sapi_base_OnScheduleRemoved);

	// sama.onSchedulePaused (int nSlotId)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onSchedulePaused", "i", (void*)pstInst, sapi_base_OnSchedulePaused);

	// sama.onScheduleResumed (int nSlotId)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleResumed", "i", (void*)pstInst, sapi_base_OnScheduleResumed);

	// sama.onConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onConflictAddTried", "biib", (void*)pstInst, sapi_base_OnConflictAddTried);

	// sama.onTimeUp (int nSlotId, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onTimeUp", "iib", (void*)pstInst, sapi_base_OnTimeUp);

	// sama.onAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItem_t *pstAltItems)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onAlternativeAddTried", "biibb", (void*)pstInst, sapi_tva_OnAlternativeAddTried);

	// sama.onSystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onSystemTimeChanged", "i", (void*)pstInst, sapi_tva_OnSystemTimeChanged);

	// sama.onResourceRefreshed ( )
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onResourceRefreshed", NULL, (void*)pstInst, sapi_base_OnResourceRefreshed);

	return ERR_OK;
}

#define _____RPC_METHODS_____
// sapi.updateSeries (void)
HERROR sapi_tva_CmdUpdateSeries (sapi_Inst_t *pstInst, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.updateSeries", NULL);
	return ERR_OK;
}

// sapi.cancelWholeSeries (int nSlot)
HERROR sapi_tva_CmdCancelWholeSeries (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.cancelWholeSeries", "i", ulSlotId);
	return ERR_OK;
}



#define _____RPC_CALLBACKS_____

// sama.onAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItem_t *pstAltItems)
HERROR sapi_tva_OnAlternativeAddTried (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_Alternative_t		*pstNotiAlternative;
	SAPI_NotiInfo_t			 stNotiInfo;
	HUINT32					ulCnt;

	DxSchedule_t			*pstSrcSchedule;
	SAPI_ConflictType_e		eSrcConfType;
	HUINT32					ulSrcNumConflict;
	SAPI_ConflictItem_t		*pstSrcConfItemArray;
	SAPI_AlternativeItems_t	*pstSrcAltItems;

	HxLOG_Trace();

	if (5 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	// 외부 notifier 실행
	pstSrcSchedule 		= (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	eSrcConfType		= (SAPI_ConflictType_e)HxOBJECT_INT(apArgv[1]);
	ulSrcNumConflict	= (HUINT32)HxOBJECT_INT(apArgv[2]);
	pstSrcConfItemArray = (SAPI_ConflictItem_t *)HxOBJECT_BIN_DATA(apArgv[3]);
	pstSrcAltItems		= (SAPI_AlternativeItems_t *)HxOBJECT_BIN_DATA(apArgv[4]);

	stNotiInfo.stAltSchAdded.eNotifyType	= eSAPI_NOTIFY_TYPE_AlternativeScheduleAdded;
	pstNotiAlternative = &(stNotiInfo.stAltSchAdded.stAlternative);
	pstNotiAlternative->stSapiConflict.eType	= eSrcConfType;
	pstNotiAlternative->stSapiConflict.ulNumConflict		= ulSrcNumConflict;
	pstNotiAlternative->stSapiConflict.ulConflictArraySize	= sizeof(SAPI_ConflictItem_t)*ulSrcNumConflict;
	pstNotiAlternative->stSapiConflict.pstConflictArray		= (ulSrcNumConflict>0)? pstSrcConfItemArray: NULL;
	HxSTD_MemCopy(&(pstNotiAlternative->stSapiConflict.stTriedSchedule), pstSrcSchedule, sizeof(DxSchedule_t));
	HxSTD_MemCopy(&(pstNotiAlternative->stAltItems), pstSrcAltItems, sizeof(SAPI_AlternativeItems_t));

	HxLOG_Debug("#ofConf=(%d), #ofAlt=(%d) \n", ulSrcNumConflict, pstNotiAlternative->stAltItems.ulNumAlternatives);

	// 받는 쪽에서는 noti 구조체의 정보는 사용하지 않고, sapi_Inst_t를 사용하므로, inst 안에도 alt 정보를 채워준다.
	SAMALIB_CFLT_ClearConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult));
	pstInst->stSamaConflictCheck.stConflictResult.eType = eSrcConfType;
	for (ulCnt = 0; ulCnt < ulSrcNumConflict; ulCnt++)
	{
		if (NULL != pstSrcConfItemArray && NULL != &(pstSrcConfItemArray[ulCnt]))
		{
			SAMALIB_CFLT_AppendConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult), &(pstSrcConfItemArray[ulCnt]));
		}
	}
	HxSTD_MemCopy(&(pstInst->stSamaConflictCheck.stConflictResult.stTriedSchedule), pstSrcSchedule, sizeof(DxSchedule_t));
	HxSTD_MemCopy(&(pstInst->stSamaConflictCheck.stAltItems), pstSrcAltItems, sizeof(SAPI_AlternativeItems_t));

	stNotiInfo.stAltSchAdded.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);

	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	return ERR_OK;
}

// sama.onSystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
HERROR sapi_tva_OnSystemTimeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_NotiInfo_t			 stNotiInfo;

	SAPI_SystemTimeChangedType_e	eSystemTimeChangedType;

	HxLOG_Trace();

	if (1 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	// 외부 notifier 실행
	eSystemTimeChangedType = (SAPI_SystemTimeChangedType_e)HxOBJECT_INT(apArgv[0]);

	stNotiInfo.stSystemTimeChanged.eNotifyType				= eSAPI_NOTIFY_TYPE_SystemTimeChanged;
	stNotiInfo.stSystemTimeChanged.eSystemTimeChangedType	= eSystemTimeChangedType;

	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	return ERR_OK;
}


