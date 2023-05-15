/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_protocol.c
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
#include <vkernel.h>
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <sapi.h>

#include "sama_int.h"
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

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
HERROR SAMA_PROTOCOL_RegisterRpcProtocols (HINT32 nRpcHandle)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		// TODO:  make schedule( add, add series, add alt )를 위해 따로 나가야 한다??
		return SAMA_PROTOCOL_TVA_RegisterRpcProtocols (nRpcHandle);
#endif
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_RegisterRpcProtocols (nRpcHandle);
	}

	return ERR_FAIL;
}

/*
	SAMA -> SAPI 의 Notify 계열 Function들: 파생에 따라 달라질 수 있는 항목이라 이곳에 넣어서 sama_protocol_xxxx 로 가도록 한다.
*/
HERROR SAMA_PROTOCOL_NotifyRscInfoSet (SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (NULL == pstRscInfo) 				{ return ERR_FAIL; }

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyRscInfoSet (pstContext, pstRscInfo, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyRfInputAbilitySet (HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyRfInputAbilitySet (pstContext, ulRfInputId, bAbility, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyAvailableSlotsChanged (HINT32 nPid, HUINT32 ulSlotNum, HUINT32 *pulSlotArray, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyAvailableSlotsChanged (pstContext, nPid, ulSlotNum, pulSlotArray, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifySchedulesLoaded (HUINT32 ulSchNum, DxSchedule_t *pstSchArray, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifySchedulesLoaded (pstContext, ulSchNum, pstSchArray, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyScheduleAdded (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyScheduleAdded (pstContext, ulSlot, pstSchedule, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyScheduleModified (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bExcuteNotifier, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyScheduleModified (pstContext, ulSlot, pstSchedule, bExcuteNotifier, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyScheduleModifiedEx (HUINT32 ulSlot, SAPI_ScheduleChanged_t *pstChangedSchedule, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyScheduleModifiedEx (pstContext, ulSlot, pstChangedSchedule, nRequestId);
	}

	return ERR_FAIL;
}



HERROR SAMA_PROTOCOL_NotifyScheduleRemoved (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyScheduleRemoved (pstContext, ulSlot, pstRemovedInfo, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifySchedulePaused (HUINT32 ulSlot, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifySchedulePaused(pstContext, ulSlot, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyScheduleResumed (HUINT32 ulSlot, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyScheduleResumed(pstContext, ulSlot, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyConflictAddTried (pstContext, pstSchedule, eType, nNumConflict, pstConfArray, nRequestId);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyTimeUp (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyTimeUp (pstContext, ulSlot, eTimerType, pstSchedule);
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItems_t *pstAltItems, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		return SAMA_PROTOCOL_TVA_NotifyAlternativeAddTried(pstContext, pstSchedule, eType, nNumConflict, pstConfArray, pstAltItems, nRequestId);
#endif
	case eDxSchedule_Spec_Base:
	default:
		return ERR_FAIL;
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifySystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		return SAMA_PROTOCOL_TVA_NotifySystemTimeChanged(pstContext, eSystemTimeChangedType);
#endif
	case eDxSchedule_Spec_Base:
	default:
		return ERR_FAIL;
	}

	return ERR_FAIL;
}

HERROR SAMA_PROTOCOL_NotifyResourceRefreshed (HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_PROTOCOL_BASE_NotifyResourceRefreshed (pstContext, nRequestId);
	}

	return ERR_FAIL;
}


