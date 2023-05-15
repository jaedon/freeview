/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_pipe.h
	@brief

	Description:  									\n
	Module: SVC / PIPE					\n

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

#ifndef ___SVC_PIPE_H___
#define ___SVC_PIPE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rlib.h>
#include <octo_common.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Data Type (enum)         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Data Type (Structure)      *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR				SVC_PIPE_Init (void);

// Action ID / Action Handle related:
extern HUINT32				SVC_PIPE_GetActionId (Handle_t hAction);
extern HUINT32				SVC_PIPE_GetSessionVersion (Handle_t hAction);
extern Handle_t				SVC_PIPE_GetActionHandle (HUINT32 ulActionId);
extern Handle_t				SVC_PIPE_IncreaseActionVersion (HUINT32 ulActionId);
extern HERROR				SVC_PIPE_IsActionHandleLatest (Handle_t hAction);

// Resource ID Related:
extern HERROR				SVC_PIPE_GetResourceNumber (RxRscType_e eRscType, HUINT32 *pulRscNum);
extern HERROR				SVC_PIPE_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId);
extern HERROR				SVC_PIPE_GetActionIdByResourceId (HUINT32 ulRscId, RxRscType_e eRscType, HUINT32 *pulActionId);
extern HERROR				SVC_PIPE_ChangeResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId);

// Action Resource Take / Release
extern Handle_t				SVC_PIPE_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority);
extern HERROR				SVC_PIPE_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority);
extern HERROR				SVC_PIPE_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId);
extern HERROR				SVC_PIPE_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType);
extern HERROR				SVC_PIPE_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg);
extern HERROR				SVC_PIPE_TryResourceRequest (Handle_t hRscReq);
extern HERROR				SVC_PIPE_SetResourcesByTrialResults (Handle_t hRscReq);
extern RxRscTrialResult_t*	SVC_PIPE_GetRequestTrialResult (Handle_t hRscReq);
extern HERROR				SVC_PIPE_DestroyResourceRequestHandle (Handle_t hRscReq);
extern HERROR				SVC_PIPE_ClearAction (HUINT32 ulActionId);
extern HERROR				SVC_PIPE_GetResourceObject (HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj);
extern HERROR				SVC_PIPE_GetRfInputAbility (HUINT32 ulRfInputId, HBOOL *pbAbility);
extern HERROR				SVC_PIPE_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility);
extern HERROR				SVC_PIPE_GetRfInputCapacity (HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity);
extern HERROR				SVC_PIPE_GetTunerCapacity (HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType);
extern HERROR				SVC_PIPE_GetResourceUsageType (RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType);

#endif	// ___SVC_PIPE_H___
/* end of file */
