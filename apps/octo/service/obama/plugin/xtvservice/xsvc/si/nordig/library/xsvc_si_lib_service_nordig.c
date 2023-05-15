/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_service_nordig.c
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
#include <tapi.h>
#include <hapi.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_output.h>

#include <db_param.h>

#include <svc_si.h>

#include <_xsvc_si_lib.h>
#include <_xsvc_si_lib_service_nordig.h>


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

#define ___MEMBER_FUNCTIONS___


HERROR xsvc_si_LibSvcEsSelectVideo_Nordig (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo)
{
	HINT32			 nCount;
	HUINT32			 ulCurrPriority;
	HUINT32			 ulLastPriority;
	HxList_t		*pstItem;						/** < svcSi_VideoEs_t */

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedVideo))
	{
		return ERR_FAIL;
	}

	pstSelectedVideo->bFound = FALSE;
	pstSelectedVideo->nEsIdx = -1;

	ulLastPriority = SVCSI_VIDEO_PRIORITY_NONE;

	for (pstItem = pstPmtEsInfo->pstVideoTrack, nCount = 0; NULL != pstItem; pstItem = pstItem->next, nCount++)
	{
		svcSi_VideoEs_t			*pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstVideoEs) && (PID_NULL != pstVideoEs->usPid))
		{
			ulCurrPriority = SVCSI_VIDEO_PRIORITY_NORMAL;

			if (pstVideoEs->bIsHd)
			{
				// AVC HD Video
				ulCurrPriority = SVCSI_VIDEO_PRIORITY_HD;
			}

			if (ulCurrPriority > ulLastPriority)
			{
				pstSelectedVideo->bFound	= TRUE;
				pstSelectedVideo->nEsIdx	= nCount;
				ulLastPriority				= ulCurrPriority;
			}
		}
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsComposeServiceInfoBySdt_Nordig(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT32 				 ulSvcNum = 0, ulPrvNum = 0;
	HxList_t				*pstSdtSvcLoop, *pstSdtSvcDesLoop;
	SIxTable_PsiTable_t 	*pstCurrSdt = NULL;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_PrvInfo_t 		*pstPrvArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	DxOperator_e			 eSvcOpType = eDxOPERATOR_None;

	if (pstSdtTable == NULL)		{ return ERR_FAIL; }

	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					ulSvcNum ++;
				}
			}
		}
	}

	if (0 == ulSvcNum)
	{
		HxLOG_Error ("No Service found in SDT\n");
		goto END_FUNC;
	}

	pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulSvcNum);
	if ((NULL == pstSvcArray) || (NULL == pstPrvArray))
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulSvcNum = 0;
	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
				DbSvc_Info_t	*pstSvcInfo = &(pstSvcArray[ulSvcNum]);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					SIxSvcDes_t *pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstSvcDes)
					{
						DxSvcType_e 			eSvcType;
						DxVideoType_e			eVideoType;

						DB_SVC_InitServiceInfo (pstSvcInfo);

						DbSvc_SetOnId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usOrgNetId);
						DbSvc_SetTsId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pstSdtSvcItem->unInfo.stSdt.usSvcId);
						svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(pstSvcInfo), NULL);

						// Provider
						if ((pstSvcDes->ucPrvName > 0) && (NULL != pstSvcDes->pszProviderName))
						{
							HUINT32 		 ulPrvCnt, ulPrvLen = (pstSvcDes->ucPrvName < DxMAX_PRV_NAME_LEN) ? (HUINT32)pstSvcDes->ucPrvName : DxMAX_PRV_NAME_LEN;
							HCHAR			 szPrvName[DxMAX_PRV_NAME_LEN + 1];

							HxSTD_StrNCpy (szPrvName, pstSvcDes->pszProviderName, ulPrvLen);
							szPrvName[ulPrvLen] = '\0';

							for (ulPrvCnt = 0; ulPrvCnt < ulPrvNum; ulPrvCnt++)
							{
								if (HxSTD_StrNCmp ((HCHAR*)pstPrvArray[ulPrvCnt].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN) == 0)
								{
									break;
								}
							}

							if (ulPrvCnt < ulPrvNum)
							{
								DbSvc_SetPrvIdx(pstSvcInfo, ulPrvCnt);
							}
							else
							{
								DbSvc_SetPrvIdx(pstSvcInfo, ulPrvNum);
								pstPrvArray[ulPrvNum].usPrvIdx = (HUINT16)ulPrvNum;
								HxSTD_StrNCpy ((HCHAR*)pstPrvArray[ulPrvNum].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN);
								pstPrvArray[ulPrvNum].szPrvName[DxMAX_PRV_NAME_LEN - 1] = '\0';

								ulPrvNum ++;
							}
						}
#if 0
						// PIDs
						hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
						DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);

						(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

						hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
						if (ERR_OK != hErr)
						{
							HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
						}
#endif
						// Service type, Video Type
						eSvcType = eDxSVC_TYPE_All;
						eVideoType = eDxVIDEO_TYPE_ALL;
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
						}

						DbSvc_SetSvcType(pstSvcInfo, eSvcType);
						DbSvc_SetVideoType(pstSvcInfo, eVideoType);

						// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
						//DbSvc_SetOrgSvcType(pstSvcInfo, DbSvc_GetSvcType(pstSvcInfo));
						DbSvc_SetOrgSvcType(pstSvcInfo, pstSvcDes->ucServType); //??

						// op type
						hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
							eSvcOpType = eDxOPERATOR_None;
						}
						DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

						ulSvcNum++;
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}
				}

				if (eSIxDESCTAG_LINKAGE == *pucRawDes)
				{
					SIxLinkDes_t *pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (pstLinkDes != NULL)
					{
						if (pstLinkDes->ucLinkType == LINKTYPE_NORDIG_SIMULCAST)
						{
							DbSvc_SetSimulcastOnId(pstSvcArray, pstLinkDes->usOrgNetId);
							DbSvc_SetSimulcastTsId(pstSvcArray, pstLinkDes->usTsId);
							DbSvc_SetSimulcastSvcId(pstSvcArray, pstLinkDes->usSvcId);
						}
					}

					PAL_SIRAWPOOL_FreeDes ((SIxLinkDes_t *)pstLinkDes);
				}
			}
		}
	}

	if (NULL != pulSvcNum)						{ *pulSvcNum = ulSvcNum; }
	if (NULL != ppstSvcArray)
	{
		if (ulSvcNum > 0)
		{
			*ppstSvcArray = pstSvcArray;
			pstSvcArray = NULL;
		}
		else
		{
			*ppstSvcArray = NULL;
		}
	}

	if (NULL != pulPrvNum)						{ *pulPrvNum = ulPrvNum; }
	if (NULL != ppstPrvArray)
	{
		if (ulPrvNum > 0)
		{
			*ppstPrvArray = pstPrvArray;
			pstPrvArray = NULL;
		}
		else
		{
			*ppstPrvArray = NULL;
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }

	return hResult;
}


