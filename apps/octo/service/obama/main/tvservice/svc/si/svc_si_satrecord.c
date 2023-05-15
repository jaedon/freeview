/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_satrecord.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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
#include <_svc_si_satrecord.h>
#include <psvc_si.h>


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

#define _____LOCAL_FUNCTIONS_____

#define _____SEARCH_RESULT_DATA_____

#define _____MEMBER_FUNCTIONS_____

#define _____SAT_RECORD_RESULT_DATA_____

HERROR svc_si_SatRecordGetSrchData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	if ((pstSatRecordSpec == NULL) || (pulDataNum == NULL) || (ppvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_GetSatRecordData(hAction, pstSatRecordSpec, eDataType, pulDataNum, ppvDataArray);
}

HERROR svc_si_SatRecordFreeSrchData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray)
{
	if ((pstSatRecordSpec == NULL) || (pvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_FreeSatRecordData(hAction, pstSatRecordSpec, eDataType, pvDataArray);
}

HERROR svc_si_SatRecordGetRRInfoFromDataBroadcastIdDes (SIxDataBroadcastIdDes_t *pstDbcIdDes, HUINT32 *pulSelectorNum, svcSi_SatRecord_RRInfo_t **ppstSelector)
{
	HUINT8			*pucRaw;
#if 0
	HBOOL						 bUpdateVerFlag;
	HUINT8						 ucUpdateType, ucUpdateVer;
	HINT32						 nOuiLen, nSelectorLen;
	HUINT32						 ulSelectorNum;
	HUINT32						 ulSiOui;
	svcSi_SatRecord_RRInfo_t	*pstSelector;
#endif

	if (pstDbcIdDes == NULL)
	{
		HxLOG_Error ("pstDbcIdDes  NULL \n");
		return ERR_FAIL;
	}

	if (pstDbcIdDes->usDataBroadcastId != DATA_BROADCAST_ID_SATREC)
	{
		HxLOG_Error ("usDataBroadcastId is Not   DATA_BROADCAST_ID_SATREC (0x%x)  \n", pstDbcIdDes->usDataBroadcastId);
		return ERR_FAIL;
	}

	if (pstDbcIdDes->pucIdSelector == NULL)
	{
		HxLOG_Error ("pucIdSelector is NULL \n");
		return ERR_FAIL;
	}

	pucRaw = pstDbcIdDes->pucIdSelector;
	NOT_USED_PARAM(pucRaw);
#if 0
	ulSelectorNum = 0;
	nOuiLen = (HINT32)*pucRaw;			pucRaw++;

	while (nOuiLen >= 6)			// 최소한 OUI + infos + selector length는 있어야 한다.
	{
		ulSiOui = HxMACRO_Get24Bit (pucRaw);
		pucRaw += 5;					// (OUI=3bytes + etc info=2bytes)
		nOuiLen -= 5;					// (OUI=3bytes + etc info=2bytes)

		nSelectorLen = (HINT32)*pucRaw;
		pucRaw ++;
		nOuiLen --;

		pucRaw += nSelectorLen;
		nOuiLen -= nSelectorLen;

		if (nOuiLen >= 0)
		{
			ulSelectorNum ++;
		}
	}

	if (ulSelectorNum == 0)
	{
		return ERR_FAIL;
	}

	if (pulSelectorNum != NULL)
	{
		*pulSelectorNum = ulSelectorNum;
	}

	if (ppstSelector != NULL)
	{
		pstSelector = (svcSi_SwupCheck_SsuInfo_t *)OxSI_Malloc (sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);
		if (pstSelector == NULL)
		{
			HxLOG_Error ("pstSelector MemAlloc Error \n");
			return ERR_FAIL;
		}
		HxSTD_MemSet(pstSelector, 0, sizeof(svcSi_SwupCheck_SsuInfo_t) * ulSelectorNum);

		// 다시 Parsing 하면서 내용물을 채운다.
		pucRaw = pstDbcIdDes->pucIdSelector;

		ulSelectorNum = 0;
		nOuiLen = (HINT32)*pucRaw;			pucRaw++;

		while (nOuiLen >= 6)			// 최소한 OUI + selector length는 있어야 한다.
		{
			ulSiOui = HxMACRO_Get24Bit (pucRaw);
			pucRaw += 3;
			nOuiLen -= 3;

			ucUpdateType = (*pucRaw) & 0x0f;
			pucRaw ++;
			nOuiLen --;

			bUpdateVerFlag = ((*pucRaw & 0x20) != 0) ? TRUE : FALSE;
			ucUpdateVer = (HUINT8)(*pucRaw & 0x1f);
			pucRaw ++;
			nOuiLen --;

			nSelectorLen = (HINT32)(*pucRaw);
			pucRaw ++;
			nOuiLen --;

			if (nOuiLen < nSelectorLen)			{ break; }

			pstSelector[ulSelectorNum].ulOui = ulSiOui;
			pstSelector[ulSelectorNum].ucUpdateType = ucUpdateType;
			pstSelector[ulSelectorNum].bUpdateVerFlag = bUpdateVerFlag;
			pstSelector[ulSelectorNum].ucUpdateVer = ucUpdateVer;
			pstSelector[ulSelectorNum].ulSelectorLen = (HUINT32)nSelectorLen;
			pstSelector[ulSelectorNum].pucSelectSelectorPnt = pucRaw;

			pucRaw += nSelectorLen;
			nOuiLen -= nSelectorLen;
			ulSelectorNum ++;
		}

		*ppstSelector = pstSelector;
	}
#endif
	return ERR_OK;
}

HERROR svc_si_SatRecordFreeSatRecInfo (svcSi_SatRecord_RRInfo_t *pstSelector)
{
	if (pstSelector == NULL)			{ return ERR_FAIL; }

	OxSI_Free (pstSelector);
	return ERR_OK;
}

/* end of file */

