/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_ota_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

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
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>
#include <_xsvc_si_lib_swup_base.h>
#include <_svc_si_search.h>

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
#define ___LOCAL_FUNCTIONS___

STATIC HERROR xsvc_si_LibSwupOnSiTableReceivedInternal (svcSi_SwupCheck_Base_t *pstSwupCheck, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	svcSi_FilteringInfo_t	*pstFilterInfo = NULL;

	if (hAction != pstSwupCheck->hAction)
	{
		HxLOG_Error ("Action Handle Diff: DataActHandle(0x%08x), MsgActHandle(0x%08x)\n", pstSwupCheck->hAction, hAction);
		return ERR_FAIL;
	}

	pstFilterInfo = svc_si_SwupCheckFindFilterInfoByFilterId(pstSwupCheck, pstSecCb->ulFilterId);
	if (NULL == pstFilterInfo)
	{
		HxLOG_Error ("No Such Filter...\n");
		return ERR_FAIL;
	}

	if (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus)
	{
		pstFilterInfo->eFilterStatus = eSI_TABLE_TIMEOUT;
		return ERR_FAIL;
	}

	pstFilterInfo->eFilterStatus = eSI_TABLE_RECEIVED;
	return ERR_OK;
}


#define _____COMPOSING_FUNCTIONS_____

#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_LibSwupOnPatReceived_Base (svcSi_SwupCheck_Base_t *pstSwupCheck, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HBOOL					 bPmtFiltered = FALSE;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT16					 usPmtPid = PID_NULL;
	SIxTable_PatProgram_t	*pstProgramArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	hErr = xsvc_si_LibSwupOnSiTableReceivedInternal(pstSwupCheck, hAction, pstSecCb);
	if ((ERR_OK != hErr) || (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus))
	{
		HxLOG_Error ("xsvc_si_LibSwupOnSiTableReceivedInternal err or Timeout:\n");
		goto END_FUNC;
	}

	// base context에 저장된 pmt svc id로 pmt pid를 가져와서 filter 건다
	hErr = svc_si_LibGetPmtPidFromPat(ulActionId, pstSwupCheck->usPmtSvcId, &usPmtPid);
	if ((ERR_OK != hErr) || (PID_NULL == usPmtPid))
	{
		HxLOG_Error ("svc_si_LibGetPmtPidFromPat err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	hErr = svc_si_SwupCheckStartFilter(pstSwupCheck, eSIxTABLETYPE_Pmt, (HUINT32)pstSwupCheck->usPmtSvcId, (HUINT32)usPmtPid);
	if (ERR_OK == hErr)
	{
		bPmtFiltered = TRUE;
	}
	else
	{
		HxLOG_Error ("svc_si_SrchStartFilter err: ProgNum(%d), PID(%d)\n", (HUINT32)pstSwupCheck->usPmtSvcId, (HUINT32)usPmtPid);
	}

	hResult = (TRUE == bPmtFiltered) ? ERR_OK : ERR_FAIL;

END_FUNC:
	if (NULL != pstProgramArray)					{ OxSI_Free(pstProgramArray); }

	return hResult;
}

HERROR xsvc_si_LibSwupOnSiTableReceived_Base (svcSi_SwupCheck_Base_t *pstSwupCheck, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	return xsvc_si_LibSwupOnSiTableReceivedInternal(pstSwupCheck, hAction, pstSecCb);
}

HERROR xsvc_si_LibSwupMakeUntExtIdByOui_Base (HUINT32 ulOui, HUINT8 ucUntActionType, HUINT16 *pusUntExtId)
{
	if (pusUntExtId == NULL) 						{ return ERR_FAIL; }

	*pusUntExtId  = (HUINT16)(((HUINT32)ucUntActionType) << 8);
	*pusUntExtId |= (HUINT16)(((ulOui >> 16) & 0xff) ^ ((ulOui >> 8) & 0xff) ^ (ulOui	& 0xff));

	return ERR_OK;
}

#define _____COMPOSING_FUNCTIONS_____
HERROR xsvc_si_LibSwupCollectServiceDataFromSi_Base (svcSi_SwupCheck_Base_t *pstSwupCheck)
{
	HERROR					 hErr = ERR_FAIL;

//	hErr = xsvc_si_LibSrchGetSearchedResultFromSi_Base(pstSwupCheck);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("xsvc_si_LibSrchGetSearchedResultFromSi_Base failed:\n");
	}

	return hErr;
}
/* end of file */

