/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_tunnel_ukfsat.c
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
#include <_svc_si_live.h>
#include <_xsvc_si_spec_ukfsat.h>
#include <_xsvc_si_lib_tunnel_ukfsat.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TUNNEL_INFO_HEAD		"FSATTUNNEL"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT8					 ucTunnelledDataType;
	HUINT16					 usPid;
} XsvcSi_AltTunnelledData_t;

typedef struct
{
	HCHAR					 szHeadItem[12];
	HxList_t				*pstTnDataDesList;		// SIxTunnelledDataInfo_t
	HxList_t				*pstTnCpntTagList;		// svcSi_BasicEs_t
	HxList_t				*pstAltTnDataList;		// XsvcSi_AltTunnelledData_t
} XsvcSi_TunnelInfo_Fsat_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC XsvcSi_TunnelInfo_Fsat_t *xsvc_si_GetTunnelledInfo (Handle_t hTunnelled)
{
	if ((HANDLE_NULL != hTunnelled) && ((Handle_t)NULL != hTunnelled))
	{
		XsvcSi_TunnelInfo_Fsat_t	*pstTunnelled = (XsvcSi_TunnelInfo_Fsat_t *)pstTunnelled;

		if (HxSTD_StrNCmp(pstTunnelled->szHeadItem, TUNNEL_INFO_HEAD, 12) == 0)
		{
			return pstTunnelled;
		}
	}

	return NULL;
}

STATIC void xsvc_si_FreeFreeDes_UkFsat (void *pvData)
{
	SIxDescriptor_t			*pstDes = (SIxDescriptor_t *)pvData;

	if (NULL != pstDes)
	{
		PAL_SIRAWPOOL_FreeDes(pstDes);
	}
}

STATIC HxList_t *xsvc_si_GetTunnelledDataDesList_UkFsat (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTable)
{
	HBOOL				 bFoundPrivate;
	HxList_t			*pstDesItem;
	HxList_t			*pstDesList = NULL;

	bFoundPrivate = FALSE;
	for (pstDesItem = pstTable->pstDesLoop; pstDesItem != NULL; pstDesItem = pstDesItem->next)
	{
		HUINT8		*pucRawDes = HLIB_LIST_Data(pstDesItem);

		if (NULL != pucRawDes)
		{
			if ((eSIxDESCTAG_FSAT_TUNNELLED_DATA == *pucRawDes) && (TRUE == bFoundPrivate))
			{
				SIxTunnelledDataInfo_t		*pstTnDataDes = (SIxTunnelledDataInfo_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, pucRawDes);

				if (NULL != pstTnDataDes)
				{
					pstDesList = HLIB_LIST_Append(pstDesList, (void *)pstTnDataDes);
				}
			}
			else if (eSIxDESCTAG_PRIVATE_DATA_SPECIFIER == *pucRawDes)
			{
				SIxPrivateDataSpecDes_t		*pstSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, pucRawDes);

				if (NULL != pstSpecDes)
				{
					if (PRIVATE_DATA_SPECIFIER_FOR_UK_FSAT == pstSpecDes->ulPrivateDataSpec)
					{
						bFoundPrivate = TRUE;
					}

					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstSpecDes);
				}
			}
			else if (eSIxDESCTAG_PRIVATE_DATA_INDICATOR == *pucRawDes)
			{
				bFoundPrivate = FALSE;
			}
		}
	}

	return pstDesList;
}

STATIC HxList_t *xsvc_si_GetTunnelledComponentTagList_UkFsat (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTable)
{
	HxList_t			*pstElemItem, *pstDesItem;
	HxList_t			*pstTagList = NULL;
	HERROR				 hResult = ERR_FAIL;

	for (pstElemItem = pstTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data(pstElemItem);
		SIxTable_PmtElement_t	*pstPmtElem;

		if (NULL == pst2ndItem)					{ continue; }

		pstPmtElem = &(pst2ndItem->unInfo.stPmt);
		if ((0x05 != pstPmtElem->ucStreamType) && (0x0B != pstPmtElem->ucStreamType))
		{
			continue;
		}

		for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data(pstDesItem);

			if ((NULL != pucRawDes) && (eSIxDESCTAG_STREAM_IDENTIFIER == *pucRawDes))
			{
				SIxStmIdentDes_t		*pstStrmIdenDes = (SIxStmIdentDes_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, pucRawDes);

				if (NULL != pstStrmIdenDes)
				{
					svcSi_BasicEs_t			*pstTag = (svcSi_BasicEs_t *)OxSI_Malloc(sizeof(svcSi_BasicEs_t));

					if (NULL != pstTag)
					{
						pstTag->ucComponentTag	= pstStrmIdenDes->ucCompTag;
						pstTag->usPid			= pstPmtElem->usElemPid;

						pstTagList = HLIB_LIST_Append(pstTagList, (void *)pstTag);
					}

					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstStrmIdenDes);
				}
			}
		}
	}

	return pstTagList;
}

STATIC HxList_t *xsvc_si_GetAlternativeTunnelledData_UkFsat (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTable)
{
	HBOOL				 bFoundPrivate;
	HxList_t			*pstElemItem, *pstDesItem;
	HxList_t			*pstAltList = NULL;
	HERROR				 hResult = ERR_FAIL;

	for (pstElemItem = pstTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data(pstElemItem);
		SIxTable_PmtElement_t	*pstPmtElem;

		if (NULL == pst2ndItem)					{ continue; }

		pstPmtElem = &(pst2ndItem->unInfo.stPmt);
		if ((0x05 != pstPmtElem->ucStreamType) && (0x0B != pstPmtElem->ucStreamType))
		{
			continue;
		}

		bFoundPrivate = FALSE;
		for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data(pstDesItem);

			if (NULL != pucRawDes)
			{
				if ((eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA == *pucRawDes) && (TRUE == bFoundPrivate))
				{
					HUINT32						 ulCnt;
					SIxAltTunnelledDataDes_t	*pstAltTnDes = (SIxAltTunnelledDataDes_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, pucRawDes);

					if (NULL != pstAltTnDes)
					{
						if ((0 != pstAltTnDes->ulNumInfo) && (NULL != pstAltTnDes->astInfo))
						{
							for (ulCnt = 0; ulCnt < pstAltTnDes->ulNumInfo; ulCnt++)
							{
								XsvcSi_AltTunnelledData_t	*pstData = (XsvcSi_AltTunnelledData_t *)OxSI_Malloc(sizeof(XsvcSi_AltTunnelledData_t));

								if (NULL != pstData)
								{
									pstData->ucTunnelledDataType	= pstAltTnDes->astInfo[ulCnt].ucTunnelledDataType;
									pstData->usPid					= pstPmtElem->usElemPid;

									pstAltList = HLIB_LIST_Append(pstAltList, (void *)pstData);
								}
							}
						}

						PAL_SIRAWPOOL_FreeDes(pstAltTnDes);
					}
				}
				else if (eSIxDESCTAG_PRIVATE_DATA_SPECIFIER == *pucRawDes)
				{
					SIxPrivateDataSpecDes_t 	*pstSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, pucRawDes);

					if (NULL != pstSpecDes)
					{
						if (PRIVATE_DATA_SPECIFIER_FOR_UK_FSAT == pstSpecDes->ulPrivateDataSpec)
						{
							bFoundPrivate = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstSpecDes);
					}
				}
				else if (eSIxDESCTAG_PRIVATE_DATA_INDICATOR == *pucRawDes)
				{
					bFoundPrivate = FALSE;
				}
			}
		}
	}

	return pstAltList;
}

STATIC HBOOL xsvc_si_CbListFindTnDataDes_UkFsat (void *pvUserData, void *pvListData)
{
	HUINT8					*pucTnDataType = (HUINT8 *)pvUserData;
	SIxTunnelledDataInfo_t	*pstTnDataInfo = (SIxTunnelledDataInfo_t *)pvListData;

	if ((NULL != pucTnDataType) && (NULL != pstTnDataInfo))
	{
		if (pstTnDataInfo->ucTunnelledDataType == *pucTnDataType)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL xsvc_si_CbListFindComponent_UkFsat (void *pvUserData, void *pvListData)
{
	HUINT8					*pucCpntTag = (HUINT8 *)pvUserData;
	svcSi_BasicEs_t			*pstCpntTagData = (svcSi_BasicEs_t *)pvListData;

	if ((NULL != pucCpntTag) && (NULL != pstCpntTagData))
	{
		if (pstCpntTagData->ucComponentTag == *pucCpntTag)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL xsvc_si_CbListFindAltTnDataDes_UkFsat (void *pvUserData, void *pvListData)
{
	HUINT8						*pucTnDataType = (HUINT8 *)pvUserData;
	XsvcSi_AltTunnelledData_t	*pstAltData = (XsvcSi_AltTunnelledData_t *)pvListData;

	if ((NULL != pucTnDataType) && (NULL != pstAltData))
	{
		if (pstAltData->ucTunnelledDataType == *pucTnDataType)
		{
			return TRUE;
		}
	}

	return FALSE;
}

#define ___MEMBER_FUNCTIONS___
Handle_t xsvc_si_LibMakeTunnelledInfoHandle_UkFsat (HUINT32 ulActionId, HUINT32 ulProgNum)
{
	Handle_t					 hTunnelled = HANDLE_NULL;
	SIxTable_PsiTable_t			*pstTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Pmt, ulProgNum);
	XsvcSi_TunnelInfo_Fsat_t	*pstTunnelled;

	if (NULL == pstTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable PMT Err: ActionId(%d), ProgNum(0x%04x)\n", ulActionId, ulProgNum);
		goto END_FUNC;
	}

	pstTunnelled = (XsvcSi_TunnelInfo_Fsat_t *)OxSI_Calloc(sizeof(XsvcSi_TunnelInfo_Fsat_t));
	if (NULL == pstTunnelled)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_StrNCpy(pstTunnelled->szHeadItem, TUNNEL_INFO_HEAD, 12);

	pstTunnelled->pstTnDataDesList = xsvc_si_GetTunnelledDataDesList_UkFsat(ulActionId, pstTable);
	pstTunnelled->pstTnCpntTagList = xsvc_si_GetTunnelledComponentTagList_UkFsat(ulActionId, pstTable);
	pstTunnelled->pstAltTnDataList = xsvc_si_GetAlternativeTunnelledData_UkFsat(ulActionId, pstTable);

	hTunnelled = (Handle_t)pstTunnelled;

END_FUNC:
	if (NULL != pstTable)			{ PAL_SIRAWPOOL_FreeTable(pstTable); }
	return hTunnelled;
}

HERROR xsvc_si_LibFreeTunnelledInfoHandle_UkFsat (Handle_t hTunnelled)
{
	XsvcSi_TunnelInfo_Fsat_t	*pstTunnelled = xsvc_si_GetTunnelledInfo(hTunnelled);

	if (NULL == pstTunnelled)
	{
		return ERR_FAIL;
	}

	HLIB_LIST_RemoveAllFunc(pstTunnelled->pstTnDataDesList, xsvc_si_FreeFreeDes_UkFsat);
	HLIB_LIST_RemoveAllFunc(pstTunnelled->pstTnCpntTagList, OxSI_Free_CB);
	HLIB_LIST_RemoveAllFunc(pstTunnelled->pstAltTnDataList, OxSI_Free_CB);
	OxSI_Free(pstTunnelled);

	return ERR_OK;
}

HERROR xsvc_si_LibGetTunneledPid_UkFsat (Handle_t hTunnelled, HUINT8 ucTDataType, HUINT16 *pusPid)
{
	HxList_t					*pstTnDataItem, *pstCpntTagItem, *pstAltTnItem;
	XsvcSi_TunnelInfo_Fsat_t	*pstTunnelled = xsvc_si_GetTunnelledInfo(hTunnelled);

	if (NULL == pusPid)					{ return ERR_FAIL; }

	if (NULL == pstTunnelled)
	{
		HxLOG_Error("Tunnelled Info wrong... hTunnelled(0x%08x)\n", hTunnelled);
		return ERR_FAIL;
	}

	*pusPid = PID_NULL;

	pstTnDataItem = HLIB_LIST_FindEx(pstTunnelled->pstTnDataDesList, (const void *)&ucTDataType, xsvc_si_CbListFindTnDataDes_UkFsat);
	if (NULL != pstTnDataItem)
	{
		SIxTunnelledDataInfo_t	*pstTnDataInfo = (SIxTunnelledDataInfo_t *)HLIB_LIST_Data(pstTnDataItem);

		pstCpntTagItem = HLIB_LIST_FindEx(pstTunnelled->pstTnCpntTagList, (const void *)&(pstTnDataInfo->ucComponentTag), xsvc_si_CbListFindComponent_UkFsat);
		if (NULL != pstCpntTagItem)
		{
			svcSi_BasicEs_t			*pstCpntTag = (svcSi_BasicEs_t *)HLIB_LIST_Data(pstCpntTagItem);

			*pusPid = pstCpntTag->usPid;
		}
	}
	else
	{
		pstAltTnItem = HLIB_LIST_FindEx(pstTunnelled->pstAltTnDataList, (const void *)&ucTDataType, xsvc_si_CbListFindAltTnDataDes_UkFsat);
		if (NULL != pstAltTnItem)
		{
			XsvcSi_AltTunnelledData_t	*pstAltData = (XsvcSi_AltTunnelledData_t *)HLIB_LIST_Data(pstAltTnItem);

			*pusPid = pstAltData->usPid;
		}
	}

	return (PID_NULL != *pusPid) ? ERR_OK : ERR_FAIL;
}

HERROR xsvc_si_LibInitTablePidInfo_UkFsat (XsvcSi_TablePid_UkFsat_t *pstTablePid)
{
	HUINT32				 ulCnt;

	if (NULL == pstTablePid)				{ return ERR_FAIL; }

	HxSTD_MemSet(pstTablePid, 0, sizeof(XsvcSi_TablePid_UkFsat_t));

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		pstTablePid->aulTunnelledPid[ulCnt] = PID_NULL;
		pstTablePid->aulLastPid[ulCnt] = PID_NULL;
		pstTablePid->aulLastExtId[ulCnt] = SILIB_TABLE_EXT_ID_NOT;
	}

	return ERR_OK;
}

/* end of file */

