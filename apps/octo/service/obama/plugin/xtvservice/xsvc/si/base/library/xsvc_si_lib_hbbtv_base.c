/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_hbbtv_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /HBBTV		\n

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
#include <tapi.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_hbbtv.h>
#include <_xsvc_si_hbbtv.h>
#include <_xsvc_si_lib_hbbtv_base.h>

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
STATIC HERROR xsvc_si_LibHbbtvStartAitFilter_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction)
{
	HUINT32			 ulActionId = PAL_PIPE_GetActionId(hAction);
	HxList_t		*pstAitList = NULL;
	HERROR			 hErr;

	hErr = svc_si_LibGetAllAitPidFromPmt(ulActionId, &pstAitList);
	if ((ERR_OK == hErr) && (pstAitList != NULL))
	{
		HxList_t	*pstAitItem = NULL;

		if (HLIB_LIST_Length(pstAitList) > 1)
		{
			HxLOG_Warning("More than 1 ait. aitNum=(%d) \n", HLIB_LIST_Length(pstAitList));
		}

		for (pstAitItem = pstAitList; NULL != pstAitItem; pstAitItem = HLIB_LIST_Next(pstAitItem))
		{
			HUINT32			*pulAitPid = (HUINT32 *)HLIB_LIST_Data(pstAitItem);

			if (NULL != pulAitPid)
			{
				if (*pulAitPid != (HUINT32)pstHbbtv->usAitPid)
				{
					if (PID_NULL != pstHbbtv->usAitPid)
					{
						hErr = svc_si_FilterStopTableSection(hAction, eSIxTABLETYPE_Ait, SVCSI_HBBTV_APPLICATION_TYPE);
						if (ERR_OK != hErr)
						{
							HxLOG_Error("svc_si_FilterStopTableSection err: hAction(0x%08x)\n", hAction);
						}
					}

					pstHbbtv->usAitPid = (HUINT16)(*pulAitPid);

					/* Application type이 0x10인 AIT만을 수신한다. */
					hErr = svc_si_FilterStartTableSection(pstHbbtv->hAction, pstHbbtv->ulDemuxId, eSIxTABLETYPE_Ait, SVCSI_HBBTV_APPLICATION_TYPE, *pulAitPid, eSI_TABLE_FILTER_MODE_Monitor, NULL);
					if (ERR_OK == hErr)
					{
						pstHbbtv->eTableFiltering |= bSI_FILTERING_AIT;
					}
					else
					{
						HxLOG_Error ("svc_si_FilterStartTableSection err: AIT PID(0x%X)\n", (HUINT32)pulAitPid);
					}
				}

				break;
			}
		}

	}
	else
	{
		HxLOG_Print ("[%s:%d] No AIT found.\n", __FUNCTION__, __LINE__);

		if (PID_NULL != pstHbbtv->usAitPid)
		{
			hErr = svc_si_FilterStopTableSection(hAction, eSIxTABLETYPE_Ait, SVCSI_HBBTV_APPLICATION_TYPE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("svc_si_FilterStopTableSection err: hAction(0x%08x)\n", hAction);
			}

			pstHbbtv->usAitPid = PID_NULL;
			pstHbbtv->eTableFiltering &= ~bSI_FILTERING_AIT;
		}

		svc_si_PostMsgToAp (eSEVT_SI_NONE_AIT, hAction, 0, 0, 0);
	}

	if (NULL != pstAitList)
	{
		HLIB_LIST_RemoveAllFunc(pstAitList, OxSI_Free_CB);
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LibHbbtvOnMainPmt_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction)
{
	HBOOL					 bFound = FALSE, bAppended = FALSE;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);
	HUINT32					 ulOldCnt, ulNewCnt;
	HUINT32					 ulFilterId;
	HUINT32					 ulDeferredNum = 0;
	SIxTable_PsiTable_t		*pstPmtHead = NULL, *pstPmt = NULL;
	svcSi_HbbtvDeferred_t	*pstDeferred, *pstDeferredArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	pstDeferredArray = (svcSi_HbbtvDeferred_t *)OxSI_Calloc(sizeof(svcSi_HbbtvDeferred_t) * MAX_DEFERRED_SVC);
	if (NULL == pstDeferredArray)
	{
		HxLOG_Error("Memory Allocation Failed:\n");
		goto END_FUNC;
	}

	pstPmtHead = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Pmt, (HUINT32)pstHbbtv->usPmtSvcId);
	for (pstPmt = pstPmtHead; NULL != pstPmt; pstPmt = pstPmt->pstNextTable)
	{
		HxList_t			*pstDesItem;

		for (pstDesItem = pstPmt->pstDesLoop; NULL != pstDesItem; pstDesItem = HLIB_LIST_Next(pstDesItem))
		{
			HUINT16					 usPmtPid = PID_NULL;
			HUINT8					*pucRawDesc = HLIB_LIST_Data(pstDesItem);
			SIxDeferAssocTagDes_t	*pstDeferAssocDes;

			if (pucRawDesc == NULL)
			{
				HxLOG_Print("pucRawDesc == NULL, continue \n");
				continue;
			}

			if ((eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG == *pucRawDesc) && (ulDeferredNum < MAX_DEFERRED_SVC))
			{
				pstDeferAssocDes = (SIxDeferAssocTagDes_t *)PAL_SIRAWPOOL_ParseDes(ulActionId, (HUINT8 *)pucRawDesc);
				if (NULL != pstDeferAssocDes)
				{
					hErr = svc_si_LibGetPmtPidFromPat (ulActionId, pstDeferAssocDes->usProgNum, &usPmtPid);
					if ((ERR_OK == hErr) && (PID_NULL != usPmtPid))
					{
						pstDeferred = &(pstDeferredArray[ulDeferredNum]);

						if ((pstDeferAssocDes->ucAssocTagSize > 0) && (NULL != pstDeferAssocDes->ausAssocTags))
						{
							pstDeferred->ulNumTags = (pstDeferAssocDes->ucAssocTagSize < MAX_DEFFERED_TAG) ? (HUINT32)pstDeferAssocDes->ucAssocTagSize : MAX_DEFFERED_TAG;
							HxSTD_MemCopy(pstDeferred->ausAssociationTags, pstDeferAssocDes->ausAssocTags, sizeof(HUINT16) * pstDeferred->ulNumTags);
						}

						pstDeferred->usOnId = pstDeferAssocDes->usOriginalNetworkId;
						pstDeferred->usTsId = pstDeferAssocDes->usTransportStreamId;
						pstDeferred->usSvcId = pstDeferAssocDes->usProgNum;
						pstDeferred->usPmtPid = usPmtPid;
					}

					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstDeferAssocDes);
					ulDeferredNum++;
				}
			}
		}
	}

	// Reset the filters that are removed:
	for (ulOldCnt = 0; ulOldCnt < pstHbbtv->ulDeferredNum; ulOldCnt++)
	{
		bFound = FALSE;

		for (ulNewCnt = 0; ulNewCnt < ulDeferredNum; ulNewCnt++)
		{
			if ((pstHbbtv->astDeffered[ulOldCnt].usOnId == pstDeferredArray[ulNewCnt].usOnId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usTsId == pstDeferredArray[ulNewCnt].usTsId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usSvcId == pstDeferredArray[ulNewCnt].usSvcId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usPmtPid == pstDeferredArray[ulNewCnt].usPmtPid))
			{
				bFound = TRUE;
				break;
			}
		}

		if (TRUE != bFound)
		{
			hErr = svc_si_FilterStopTableSection(hAction, eSIxTABLETYPE_Pmt, (HUINT32)pstHbbtv->astDeffered[ulOldCnt].usSvcId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("svc_si_FilterStopTableSection hAction(0x%08x), PMT, usSvcId(0x%04x)\n", hAction, pstHbbtv->astDeffered[ulOldCnt].usSvcId);
			}
		}
	}

	// Add the new filters:
	for (ulNewCnt = 0; ulNewCnt < ulDeferredNum; ulNewCnt++)
	{
		bFound = FALSE;
		for (ulOldCnt = 0; ulOldCnt < pstHbbtv->ulDeferredNum; ulOldCnt++)
		{
			if ((pstHbbtv->astDeffered[ulOldCnt].usOnId == pstDeferredArray[ulNewCnt].usOnId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usTsId == pstDeferredArray[ulNewCnt].usTsId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usSvcId == pstDeferredArray[ulNewCnt].usSvcId) &&
				(pstHbbtv->astDeffered[ulOldCnt].usPmtPid == pstDeferredArray[ulNewCnt].usPmtPid))
			{
				bFound = TRUE;
				pstDeferredArray[ulNewCnt].bReceived = pstHbbtv->astDeffered[ulOldCnt].bReceived;
				pstDeferredArray[ulNewCnt].ulFilterId = pstHbbtv->astDeffered[ulOldCnt].ulFilterId;
				break;
			}
		}

		if (TRUE != bFound)
		{
			hErr = svc_si_FilterStartTableSection(hAction, pstHbbtv->ulDemuxId, eSIxTABLETYPE_Pmt,
												(HUINT32)pstDeferredArray[ulNewCnt].usSvcId,
												(HUINT32)pstDeferredArray[ulNewCnt].usPmtPid,
												eSI_TABLE_FILTER_MODE_Monitor,
												&ulFilterId);
			if (ERR_OK == hErr)
			{
				pstDeferredArray[ulNewCnt].ulFilterId = ulFilterId;
				bAppended = TRUE;
			}
			else
			{
				pstDeferredArray[ulNewCnt].ulFilterId = FILTER_ID_NULL;
			}
		}
	}

	pstHbbtv->ulDeferredNum = ulDeferredNum;
	HxSTD_MemCopy(pstHbbtv->astDeffered, pstDeferredArray, sizeof(svcSi_HbbtvDeferred_t) * MAX_DEFERRED_SVC);

	if (TRUE != bAppended)
	{
		xsvc_si_LibHbbtvStartAitFilter_Base(pstHbbtv, hAction);
		svc_si_PostMsgToAp(eSEVT_SI_PMT, hAction, 0, 0, 0);
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstPmtHead)				{ PAL_SIRAWPOOL_FreeTable(pstPmtHead); }
	if (NULL != pstDeferredArray)		{ OxSI_Free(pstDeferredArray); }

	return hResult;
}

STATIC HERROR xsvc_si_LibHbbtvOnDeferredPmt_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, HUINT32 ulDeferIdx)
{
	if (ulDeferIdx < pstHbbtv->ulDeferredNum)
	{
		HBOOL					 bAllReceived = TRUE;
		HUINT32 				 ulCnt;
		svcSi_HbbtvDeferred_t	*pstDeferred = &(pstHbbtv->astDeffered[ulDeferIdx]);

		if (TRUE != pstDeferred->bReceived)
		{
			pstDeferred->bReceived = TRUE;

			for (ulCnt = 0; ulCnt < pstHbbtv->ulDeferredNum; ulCnt++)
			{
				if (TRUE != pstHbbtv->astDeffered[ulCnt].bReceived)
				{
					bAllReceived = FALSE;
					break;
				}
			}

			if (TRUE == bAllReceived)
			{
				xsvc_si_LibHbbtvStartAitFilter_Base(pstHbbtv, hAction);
				svc_si_PostMsgToAp(eSEVT_SI_PMT, hAction, 0, 0, 0);
			}
		}
	}

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_LibHbbtvOnPatReceived_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR					 hErr;

	if (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus)
	{
		if ((FILTER_ID_NULL == pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pat]) ||
			(pstSecCb->ulFilterId != pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pat]))
		{
			svc_si_PostMsgToAp(eSEVT_SI_PAT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pat] = pstSecCb->ulFilterId;
		return ERR_OK;
	}

	// 약 1000ms 는 이전 TP의 PAT가 올라올 수 있는 MAX time으로 잡는다.
	if (HLIB_STD_GetSystemTick() < (pstHbbtv->ulPatTimeTick + 1000))
	{
		HBOOL				 bFound = FALSE;
		HUINT16 			 usTsIdInPat = 0;
		SIxTable_PsiTable_t *pstTable, *pstTableHead;

		pstTableHead = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
		for (pstTable = pstTableHead; NULL != pstTable; pstTable = pstTable->pstNextTable)
		{
			usTsIdInPat = pstTable->unInfo.stPat.usTsId;
			if (pstTable->unInfo.stPat.usTsId == pstHbbtv->usTsId)
			{
				bFound = TRUE;
				break;
			}
		}

		if (NULL != pstTableHead)			{ PAL_SIRAWPOOL_FreeTable(pstTableHead); }

		if (TRUE != bFound)
		{
			// PAT Filter를 빼고 다시 걸도록 한다. PAL쪽에 monitor 모드가 monitor_by_version 방식이라 새 데이터가 들어오는 걸 인지하는
			// 것이 PAT 단위로는 매우 길어져 버리므로 여기서 다시 건다.
			HxLOG_Error("PAT with wrong TS ID: TsId(SvcTsId(0x%04x), InPat(0x%04x)\n", pstHbbtv->usTsId, usTsIdInPat);

			(void)svc_si_FilterStartTableSection(hAction, pstHbbtv->ulDemuxId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_NOT, PAT_PID, eSI_TABLE_FILTER_MODE_Monitor, NULL);
			return ERR_OK;
		}
	}

	hErr = svc_si_LibGetPmtPidFromPat (ulActionId, pstHbbtv->usPmtSvcId, &usPmtPid);
	if ((ERR_OK != hErr) || (PID_NULL == usPmtPid))
	{
		HxLOG_Info ("svc_si_LibGetPmtPidFromPat err\n");

		hErr = svc_si_FilterStopTableSection(hAction, eSIxTABLETYPE_Pmt, (HUINT32)pstHbbtv->usPmtSvcId);
		if (ERR_OK == hErr)
		{
			pstHbbtv->usPmtPid = PID_NULL;
			pstHbbtv->eTableFiltering &= ~(bSI_FILTERING_PMT);
		}

		svc_si_PostMsgToAp(eSEVT_SI_PMT_PIDREMOVED, hAction, 0, 0, 0);
		return ERR_OK;
	}

	if (PID_NULL == pstHbbtv->usPmtPid)
	{
		pstHbbtv->usPmtPid = usPmtPid;
		(void)svc_si_HbbtvStartSection(pstHbbtv, bSI_FILTERING_PMT);
	}
	else if (pstHbbtv->usPmtPid != usPmtPid)
	{
		// PMT Changed:
		(void)svc_si_FilterStopTableSection(hAction, eSIxTABLETYPE_Pmt, (HUINT32)pstHbbtv->usPmtSvcId);

		pstHbbtv->usPmtPid = usPmtPid;
		(void)svc_si_HbbtvStartSection(pstHbbtv, bSI_FILTERING_PMT);
	}

	return ERR_OK;
}

HERROR xsvc_si_LibHbbtvOnPmtReceived_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT16			 usPid = PID_NULL;
	HUINT32			 ulActionId = PAL_PIPE_GetActionId(hAction);
	HERROR			 hErr;

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]) ||
			(pstSecCb->ulFilterId != pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_PMT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Pmt] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		hErr = svc_si_FilterGetFilterPid(ulActionId, eSIxTABLETYPE_Pmt, (HUINT16)pstSecCb->ulFilterId, &usPid);
		if ((ERR_OK == hErr) && (PID_NULL != usPid))
		{
			// Main Service
			if (usPid == pstHbbtv->usPmtPid)
			{
				hErr = xsvc_si_LibHbbtvOnMainPmt_Base(pstHbbtv, hAction);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("xsvc_si_LibHbbtvOnMainPmt_Base hAction(0x%08x)\n", hAction);
				}
			}
			else
			{
				HUINT32			 ulCnt;

				for (ulCnt = 0; ulCnt < pstHbbtv->ulDeferredNum; ulCnt++)
				{
					if (pstHbbtv->astDeffered[ulCnt].usPmtPid == usPid)
					{
						hErr = xsvc_si_LibHbbtvOnDeferredPmt_Base(pstHbbtv, hAction, ulCnt);
						if (ERR_OK != hErr)
						{
							HxLOG_Error("xsvc_si_LibHbbtvOnMainPmt_Base hAction(0x%08x)\n", hAction);
						}
					}
				}
			}
		}

		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibHbbtvOnAitReceived_Base (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Ait]) ||
			(pstSecCb->ulFilterId != pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Ait]))
		{
			svc_si_PostMsgToAp(eSEVT_SI_AIT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstHbbtv->aulTimeoutFilterId[eSIxTABLETYPE_Ait] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp(eSEVT_SI_AIT, hAction, pstHbbtv->usAitPid, 0, 0);
		break;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___

/* end of file */

