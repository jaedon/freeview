/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_resource.c
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
HERROR SAMA_RSC_SetResourceInfo (SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (NULL == pstRscInfo)					{ return ERR_FAIL; }

	SAMA_MemCpy (&(pstContext->stSamaConflictCheck.stRscInfo), pstRscInfo, sizeof(SAPI_RscInfo_t));
	pstContext->stSamaConflictCheck.bRscSet = TRUE;

	(void)SAMALIB_CFLT_SetResourceInfo (&(pstContext->stSamaConflictCheck));

	(void)SAMA_PROTOCOL_NotifyRscInfoSet (&(pstContext->stSamaConflictCheck.stRscInfo), nRequestId);
	return ERR_OK;
}

HERROR SAMA_RSC_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId)
{
	HBOOL				 bValue;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (ulRfInputId >= RxRFINPUT_NUM_MAX)	{ return ERR_FAIL; }

	bValue = (TRUE == bAbility);
	pstContext->stSamaConflictCheck.anRfInputAbility[ulRfInputId] = (HINT32)bValue;

	(void)SAMALIB_CFLT_SetRfInputAbility(&(pstContext->stSamaConflictCheck), ulRfInputId, bAbility);

	(void)SAMA_PROTOCOL_NotifyRfInputAbilitySet (ulRfInputId, bValue, nRequestId);
	return ERR_OK;
}

HERROR SAMA_RSC_RefreshResource (HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	(void)SAMALIB_SVC_ClearCache(&(pstContext->stSamaConflictCheck));

	(void)SAMA_PROTOCOL_NotifyResourceRefreshed(nRequestId);

	return ERR_OK;
}

