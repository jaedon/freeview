/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_ukdtt.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

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

#include <xdb_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_xsvc_si_lib_search_base.h>
#include <_xsvc_si_lib.h>
#include <_xsvc_si.h>


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

STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInNetwork_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, DbSvc_NetInfo_t *pstNetInfo)
{
	HUINT8					*pucRawDes;

	if ((pstNitTable != NULL) && (pstNetInfo != NULL))
	{
		pstNetInfo->utExt.stTvAnyTime.ulFTAContentMgr = 0;

		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
		if (NULL != pucRawDes)
		{
			SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstDADes)
			{
				(void)DB_SVC_SetNetworkDefaultAuthority(pstNetInfo, (const HUINT8 *)pstDADes->pszDefaultAuthority);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInTS_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, SIxTable_PsiTable_t *pstSdtTable, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT8 *pucRawDes;

	if ((pstNitTable != NULL) && (pstSdtTable != NULL) && (pstTsInfo != NULL))
	{
		HxList_t *pstItem;

		(void)DB_SVC_ResetTsDefaultAuthority(pstTsInfo);

		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == pstTsInfo->usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == pstTsInfo->usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
				if (NULL != pucRawDes)
				{
					SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstDADes)
					{
						(void)DB_SVC_SetTsDefaultAuthority(pstTsInfo, (const HUINT8 *)pstDADes->pszDefaultAuthority);
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
						break;
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SrchSetDefaultAuthoriyInSvc_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc)
{
	HUINT8 				*pucRawDes = NULL;
	HxList_t				*pstSdtSvcLoop = NULL;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;

	if ((pstSdtTable != NULL) && (pstSvc != NULL))
	{
		(void)DB_SVC_ResetServiceDefaultAuthority(pstSvc);
		for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
		{
			if (pstCurrSdt->unInfo.stSdt.usTsId == DbSvc_GetTsId(pstSvc) && pstCurrSdt->unInfo.stSdt.usOrgNetId == DbSvc_GetOnId(pstSvc))
			{
				for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
				{
					SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

					if (pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
					{
						pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pstSdtSvcItem, eSIxDESCTAG_DEFAULT_AUTHORITY, 0);
						if (NULL != pucRawDes)
						{
							SIxDefaultAuthorityDes_t *pstDADes = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
							if (NULL != pstDADes)
							{
								(void)DB_SVC_SetServiceDefaultAuthority(pstSvc, (const HUINT8 *)pstDADes->pszDefaultAuthority);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDADes);
								break;
							}
						}
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SrchSetFTAContentsMgrInNetwork_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, DbSvc_NetInfo_t *pstNetInfo)
{
	HUINT8					*pucRawDes;
	HUINT32					ulFtaContentValue = 0;

	if ((pstNitTable != NULL) && (pstNetInfo != NULL))
	{
		(void)DB_SVC_ResetNetworkDefaultAuthority(pstNetInfo);

		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_FTA_CONTENT_MANAGEMENT, 0);
		if (NULL != pucRawDes)
		{
			SIxFtaContentManagementDes_t	*pstFtaContentManaementDes = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstFtaContentManaementDes)
			{
				HxLOG_Print("\t Found FTA Contents Management Descript, Network Level in NIT\n\t\t-- DoNotScramble:0x%X CtrlRAccOverINet:0x%X, DoNotApplyRevocation:0x%X\n",
							pstFtaContentManaementDes->ucDoNotScramble,pstFtaContentManaementDes->ucCtrlRAccOverINet,
							pstFtaContentManaementDes->ucDoNotApplyRevocation);

				ulFtaContentValue = (HUINT32)(((HUINT32)eFTA_CONTENT_NET_LEVEL << 24) & 0xFF000000);
				ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucDoNotScramble << 16) & 0x00FF0000);
				ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucCtrlRAccOverINet << 8) & 0x0000FF00);
				ulFtaContentValue |= (HUINT32)( (HUINT32)pstFtaContentManaementDes->ucDoNotApplyRevocation & 0x000000FF);

				HxLOG_Print("\t Found FTA Contents Management Descript, Network Level in NIT\n\t\t-- ulFtaContentMessage:0x%X\n",ulFtaContentValue);

				if(pstNetInfo->utExt.stTvAnyTime.ulFTAContentMgr != ulFtaContentValue)
				{
					/* FTA Contents Management changed in NIT first loop */
					(void)DB_SVC_SetNetworkFTAContentMgr(pstNetInfo, pstNetInfo->utExt.stTvAnyTime.ulFTAContentMgr);
				}
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstFtaContentManaementDes);
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR xsvc_si_SrchSetFTAContentsMgrInTS_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, SIxTable_PsiTable_t *pstSdtTable, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT8 *pucRawDes;
	HUINT32					ulFtaContentValue = 0;

	if ((pstNitTable != NULL) && (pstSdtTable != NULL) && (pstTsInfo != NULL))
	{
		HxList_t *pstItem;

		pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr = 0;

		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == pstTsInfo->usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == pstTsInfo->usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_FTA_CONTENT_MANAGEMENT, 0);
				if (NULL != pucRawDes)
				{
					SIxFtaContentManagementDes_t *pstFtaContentManaementDes = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstFtaContentManaementDes)
					{
						ulFtaContentValue = (HUINT32)(((HUINT32)eFTA_CONTENT_TS_LEVEL << 24) & 0xFF000000);
						ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucDoNotScramble << 16) & 0x00FF0000);
						ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucCtrlRAccOverINet << 8) & 0x0000FF00);
						ulFtaContentValue |= (HUINT32)( (HUINT32)pstFtaContentManaementDes->ucDoNotApplyRevocation & 0x000000FF);


						(void)DB_SVC_SetTsFTAContentMgr(pstTsInfo,ulFtaContentValue);
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstFtaContentManaementDes);
						break;
					}

				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SrchSetFTAContentsMgrInSvc_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc)
{
	HUINT8 				*pucRawDes = NULL;
	HxList_t				*pstSdtSvcLoop = NULL;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;
	HUINT32					ulFtaContentValue = 0;

	if ((pstSdtTable != NULL) && (pstSvc != NULL))
	{
		XdbSvc_TvAnyTimeSetContentMgr(pstSvc, 0);
		for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
		{
			if (pstCurrSdt->unInfo.stSdt.usTsId == DbSvc_GetTsId(pstSvc) && pstCurrSdt->unInfo.stSdt.usOrgNetId == DbSvc_GetOnId(pstSvc))
			{
				for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
				{
					SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

					if (pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
					{
						pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pstSdtSvcItem, eSIxDESCTAG_FTA_CONTENT_MANAGEMENT, 0);
						if (NULL != pucRawDes)
						{
							SIxFtaContentManagementDes_t *pstFtaContentManaementDes = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
							if (NULL != pstFtaContentManaementDes)
							{
								ulFtaContentValue = (HUINT32)(((HUINT32)eFTA_CONTENT_SVC_LEVEL << 24) & 0xFF000000);
								ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucDoNotScramble << 16) & 0x00FF0000);
								ulFtaContentValue |= (HUINT32)( (HUINT32)(pstFtaContentManaementDes->ucCtrlRAccOverINet << 8) & 0x0000FF00);
								ulFtaContentValue |= (HUINT32)( (HUINT32)pstFtaContentManaementDes->ucDoNotApplyRevocation & 0x000000FF);

								(void)DB_SVC_SetServiceFTAContentMgr(pstSvc,ulFtaContentValue);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstFtaContentManaementDes);
								break;
							}
						}
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_GetTRgNameInNit_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, HUINT32 *pulCnt ,DbSvc_T_RegionNameInfo_t **ppstTRgNameInfo)
{
	SIxTable_PsiTable_t 		*pstCurrNit = NULL;
	HUINT32 					ulTRCnt=0,idx;
	HUINT8						szCountryCode[4];
	DbSvc_T_RegionNameInfo_t	*pstPtr;
	HUINT8						*pszTmpString;
	SIxTargetRegionNameInfo_t	*pstTRegionNameDesInfo;
	HxList_t 	*pstList = NULL;


	if ((pstNitTable != NULL) )
	{
		for (pstCurrNit = pstNitTable; NULL != pstCurrNit; pstCurrNit = pstCurrNit->pstNextTable)
		{
			for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
			{
				HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
				if (*pucRawDes == eSIxDESCTAG_EXTENSION)
				{
					SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstExtensionDes)
					{
						if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_TARGET_REG_NAME)
						{
							SIxTargetRegionNameDes_t *pstTRegionNameDes = (SIxTargetRegionNameDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
							if(NULL != pstTRegionNameDes)
							{
								pstTRegionNameDesInfo = pstTRegionNameDes->pstInfo;
								while(pstTRegionNameDesInfo)
								{
									ulTRCnt++;
									pstTRegionNameDesInfo = pstTRegionNameDesInfo->pstNext;
								}

								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTRegionNameDes);
							}
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
					}
				}
			}
		}
	}

	if ( 0 == ulTRCnt)
	{
		HxLOG_Print("[%s:%d] Exit -- ulTRCnt:%d\n", __FUNCTION__,__LINE__,ulTRCnt);
		return ERR_FAIL;
	}

	pstPtr = (DbSvc_T_RegionNameInfo_t *)OxSI_Malloc(sizeof(DbSvc_T_RegionNameInfo_t)*ulTRCnt);
	if ( NULL == pstPtr)
	{
		HxLOG_Print("[%s:%d] Exit -- ppstTRgNameInfo:0x%X\n", __FUNCTION__,__LINE__, (unsigned int)pstPtr);
		return ERR_FAIL;
	}
	HxSTD_memset(pstPtr, 0 , ( sizeof(DbSvc_T_RegionNameInfo_t) * ulTRCnt ) );
	idx = 0;

	for (pstCurrNit = pstNitTable; NULL != pstCurrNit; pstCurrNit = pstCurrNit->pstNextTable)
	{
		for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
			if (*pucRawDes == eSIxDESCTAG_EXTENSION)
			{
				SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
				if (NULL != pstExtensionDes)
				{
					if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_TARGET_REG_NAME)
					{

						SIxTargetRegionNameDes_t *pstTRegionNameDes = (SIxTargetRegionNameDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData );
						if(NULL != pstTRegionNameDes)
						{
							pstTRegionNameDesInfo = pstTRegionNameDes->pstInfo;

							HxSTD_memcpy(szCountryCode,pstTRegionNameDes->ucCountryCode,3);
							while(pstTRegionNameDesInfo)
							{
								HxSTD_memcpy(	(void *)pstPtr[idx].szCountryCode, (void *)szCountryCode , 3 );

								pszTmpString = NULL;
								pszTmpString = SILIB_TEXT_MakeUtf8 (eSIxTEXT_ENC_300468_UK, pstTRegionNameDes->ucIso639LangCode, pstTRegionNameDesInfo->pszRegionName, pstTRegionNameDesInfo->ucRegionNameLen);
								if ( NULL != pszTmpString )
								{
									HxSTD_StrNCpy( (void *)pstPtr[idx].szTRegionName , (const void *)pszTmpString , DxMAX_T_REGION_NAME_LEN-1);
									SILIB_TEXT_Free(pszTmpString);
								}

								pstPtr[idx].ucRegionDepth = pstTRegionNameDesInfo->ucRegionDepth;
								pstPtr[idx].ulRegionCode = 0xFFFFFFFF;
								switch ( pstPtr[idx].ucRegionDepth )
								{
								case 0:
									pstPtr[idx].ulRegionCode = 0xFFFFFFFF;
									break;
								case 1:
									pstPtr[idx].ulRegionCode = (HUINT32)((pstTRegionNameDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
									break;
								case 2:
									pstPtr[idx].ulRegionCode = (HUINT32)((pstTRegionNameDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
									pstPtr[idx].ulRegionCode |= (HUINT32)((pstTRegionNameDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
									break;
								case 3:
									pstPtr[idx].ulRegionCode = (HUINT32)((pstTRegionNameDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
									pstPtr[idx].ulRegionCode |= (HUINT32)((pstTRegionNameDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
									pstPtr[idx].ulRegionCode |= (HUINT32)(( pstTRegionNameDesInfo->stTargetRegionInfo.usTertiaryRegionCode) & 0x0000FFFF );
									break;
								}

								idx++;
								pstTRegionNameDesInfo = pstTRegionNameDesInfo->pstNext;
							}
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTRegionNameDes);
					}

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
				}
			}
		}
	}
	*pulCnt = ulTRCnt;
	*ppstTRgNameInfo = pstPtr;

	return ERR_OK;

}
STATIC HERROR xsvc_si_SrchSetUriLinkageInNetwork_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, DbSvc_NetInfo_t *pstNetInfo)
{
	SIxTable_PsiTable_t 		*pstCurrNit = NULL;
	HxList_t 	*pstList = NULL;

	if ((pstNitTable != NULL) )
	{
		for (pstCurrNit = pstNitTable; NULL != pstCurrNit; pstCurrNit = pstCurrNit->pstNextTable)
		{
			for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
			{
				HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
				if (*pucRawDes == eSIxDESCTAG_EXTENSION)
				{
					SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstExtensionDes)
					{
						if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_URI_LINKAGE)
						{
							SIxUriLinkageDes_t *pstUriLinkageDes = (SIxUriLinkageDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
							if(NULL != pstUriLinkageDes)
							{
								(void)DB_SVC_SetNetworkUriPrefix(pstNetInfo, (const HUINT8 *)pstUriLinkageDes->pszUriChar);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUriLinkageDes);
							}
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SrchCollectTRgNameInfoFromSi_UkDtt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable)
{
	HUINT32						ulTRgNameCnt=0;
	HERROR						hError = ERR_OK;
	DbSvc_T_RegionNameInfo_t	*pstTRgNameInfo = NULL,*pstNewTRgNameInfo = NULL,*pstDbTRgNameInfo = NULL,*pstPtr = NULL,*pstDbPtr = NULL;
	HUINT32						ulDbTRgNameCnt = 0,i,j,ulNewDataCnt = 0;
	HBOOL						bFound = FALSE;

	xsvc_si_GetTRgNameInNit_UkDtt(ulActionId, pstNitTable, &ulTRgNameCnt ,&pstTRgNameInfo);
	if(0!= ulTRgNameCnt)
	{
		hError = DB_PARAM_GetItem (eDB_PARAM_ITEM_REGION_NAMEINFO_CNT, &ulDbTRgNameCnt, 0);
		if(hError != ERR_OK)
		{
			ulDbTRgNameCnt = 0;
		}

		pstNewTRgNameInfo = (DbSvc_T_RegionNameInfo_t *)OxSI_Malloc(sizeof(DbSvc_T_RegionNameInfo_t)*(ulTRgNameCnt + ulDbTRgNameCnt));
		if(NULL == pstNewTRgNameInfo)
		{
			HxLOG_Print("[xsvc_si_SrchCollectTRgNameInfoFromSi_UkDtt] Memory Alloc Fail!\n");
			goto END_FUNC;
		}
		HxSTD_memset(pstNewTRgNameInfo,0,(sizeof(DbSvc_T_RegionNameInfo_t) * (ulTRgNameCnt + ulDbTRgNameCnt)));

		HxLOG_Print("\tulTRgNameCnt:%d ulDbTRgNameCnt:%d pstNewTRgNameInfo:0x%X\n", ulTRgNameCnt, ulDbTRgNameCnt, pstNewTRgNameInfo);

		HxLOG_Print("\n\n\n=========== NIT's T-REGION NAME INFO ===========\n");

		if ( 0 != ulDbTRgNameCnt )
		{
			pstDbTRgNameInfo = (DbSvc_T_RegionNameInfo_t *)OxSI_Malloc(sizeof(DbSvc_T_RegionNameInfo_t) * ulDbTRgNameCnt);
			HxSTD_memset(pstDbTRgNameInfo,0,(sizeof(DbSvc_T_RegionNameInfo_t) * ulDbTRgNameCnt));

			HxLOG_Print("\tpstDbTRgNameInfo:0x%X\n", pstDbTRgNameInfo);

			(void)DB_PARAM_GetItem(eDB_PARAM_ITEM_REGION_NAMEINFO,
													(HUINT32 *)pstDbTRgNameInfo,
													sizeof(DbSvc_T_RegionNameInfo_t)* ulDbTRgNameCnt);

			HxLOG_Print("\t nTRgCnt:%d\n",ulDbTRgNameCnt);
			for ( i = 0 ; i < ulTRgNameCnt; i++ )
			{
				if ( 0xFF != pstTRgNameInfo[i].ucRegionDepth)
				{
					HxLOG_Print("[%02d] [Country:%s][TRegionDepth:0x%X][TRgName:%s][RCode:0x%X]\n",i,
								pstTRgNameInfo[i].szCountryCode, pstTRgNameInfo[i].ucRegionDepth,
								pstTRgNameInfo[i].szTRegionName, pstTRgNameInfo[i].ulRegionCode);
				}
			}
		}

		HxLOG_Print("=========== NIT's T-REGION NAME INFO ===========\n\n\n");

		ulNewDataCnt = 0;
		for ( i = 0 ; i < ulTRgNameCnt ; i++ )
		{
			pstPtr = &pstTRgNameInfo[i];
			bFound = FALSE;

			if ( 0 != ulDbTRgNameCnt )
			{
				for ( j = 0 ; j < ulDbTRgNameCnt ; j++ )
				{
					pstDbPtr = &pstDbTRgNameInfo[j];

					if ( 0 == HxSTD_memcmp(pstPtr, pstDbPtr , sizeof(DbSvc_T_RegionNameInfo_t) ) )
						bFound = TRUE;
				}
			}

			if ( FALSE == bFound )
			{
				HxSTD_memcpy(&pstNewTRgNameInfo[ulNewDataCnt] , pstPtr , sizeof (DbSvc_T_RegionNameInfo_t) );
				HxLOG_Print("[%02d] [Country:%s][TRegionDepth:0x%X][TRgName:%s][RCode:0x%X]\n",ulNewDataCnt,
								pstTRgNameInfo[ulNewDataCnt].szCountryCode, pstTRgNameInfo[ulNewDataCnt].ucRegionDepth,
								pstTRgNameInfo[ulNewDataCnt].szTRegionName, pstTRgNameInfo[ulNewDataCnt].ulRegionCode);

				ulNewDataCnt++;
			}
		}

		HxLOG_Print("\tulNewDataCnt:%d ulDbTRgNameCnt:%d\n", ulNewDataCnt , ulDbTRgNameCnt);

		if ( 0 != ulDbTRgNameCnt )
			HxSTD_memcpy(&pstNewTRgNameInfo[ulNewDataCnt] , pstDbTRgNameInfo , (sizeof(DbSvc_T_RegionNameInfo_t) * ulDbTRgNameCnt) );


		ulNewDataCnt = (ulNewDataCnt + ulDbTRgNameCnt);
		HxLOG_Print("\tulNewDataCnt:%d\n",ulNewDataCnt);

		hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_REGION_NAMEINFO,(HUINT32) pstNewTRgNameInfo, (sizeof(DbSvc_T_RegionNameInfo_t) * ulNewDataCnt));
		hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_REGION_NAMEINFO_CNT,(HUINT32)ulNewDataCnt, 0);

		HxLOG_Print("\tpstDbTRgNameInfo:0x%X\n", pstDbTRgNameInfo);

	}
	END_FUNC:

	if ( NULL != pstTRgNameInfo )
		OxSI_Free(pstTRgNameInfo);
	if ( NULL != pstNewTRgNameInfo )
		OxSI_Free(pstNewTRgNameInfo);
	if ( NULL != pstDbTRgNameInfo )
		OxSI_Free(pstDbTRgNameInfo);

	return hError;

}

HERROR xsvc_si_GetTRegionInfoFromNitNetLevel_UkDtt( HUINT32 ulActionId, SIxTable_PsiTable_t *pstNitTable, HUINT32 ulMaxTRgNum, DbSvc_T_RegionInfo_t *pstTRegionInfo )
{
	HUINT32		ulTRgCnt=0,i;
	HERROR		hError = ERR_OK;
	HUINT8						szCountryCode[4];
	SIxTable_PsiTable_t 		*pstCurrNit = NULL;
	SIxTargetRegionInfo_t	*pstTRegionDesInfo;
	DbSvc_T_RegionInfo_t		*pstTmpTRegionInfo;
	HxList_t 	*pstList = NULL;

	if ( NULL == pstTRegionInfo)
	{
		return ERR_FAIL;
	}

	HxSTD_memset( pstTRegionInfo , 0 , ( sizeof(DbSvc_T_RegionInfo_t) * ulMaxTRgNum ) );

	for ( i = 0 ; i < ulMaxTRgNum ; i++ )
	{
		HxSTD_memset( pstTRegionInfo[i].szCountryCode, 0 , 4 );
		pstTRegionInfo[i].ucRegionDepth = 0xFF; // Region Level 0xFF is invalid.
	}

	if ((pstNitTable != NULL) )
	{
		for (pstCurrNit = pstNitTable; NULL != pstCurrNit; pstCurrNit = pstCurrNit->pstNextTable)
		{
			for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
			{
				HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
				if (*pucRawDes == eSIxDESCTAG_EXTENSION)
				{
					SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstExtensionDes)
					{
						if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_TARGET_REG)
						{
							SIxTargetRegionDes_t *pstTRegionDes = (SIxTargetRegionDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
							pstTRegionDesInfo = pstTRegionDes->pstInfo;

							HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
							if ( NULL == pstTRegionDesInfo)
							{
								if ( ulTRgCnt < ulMaxTRgNum )
								{
									// Only Exist Country Code.
									pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];
									HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
									pstTmpTRegionInfo->ucRegionDepth = 0;
									pstTmpTRegionInfo->ulRegionCode = 0;
									ulTRgCnt++;
								}
							}
							else
							{
								while(pstTRegionDesInfo)
								{
									if ( ulTRgCnt < ulMaxTRgNum )
									{
										pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];

										if ( 0 != pstTRegionDesInfo->ucCountryCodeFlag )
										{
											HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
											HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
										}
										else
											HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );

										pstTmpTRegionInfo->ucRegionDepth = pstTRegionDesInfo->ucRegionDepth;
										switch ( pstTmpTRegionInfo->ucRegionDepth )
										{
										case 0:
											pstTmpTRegionInfo->ulRegionCode = 0;
											break;
										case 1:
											pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
											break;
										case 2:
											pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
											pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
											break;
										case 3:
											pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
											pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
											pstTmpTRegionInfo->ulRegionCode |= (HUINT32)(( pstTRegionDesInfo->stTargetRegionInfo.usTertiaryRegionCode ) & 0x0000FFFF );
											break;
										default:
											pstTmpTRegionInfo->ulRegionCode = 0xFFFFFFFF;
											break;
										}
									}
									else
									{
										HxLOG_Print("[%s:%d] -- ulMaxTRgNum:%ulTRgCnt:%d\n", __FUNCTION__,__LINE__,ulMaxTRgNum,ulTRgCnt);
									}

									ulTRgCnt++;
									pstTRegionDesInfo = pstTRegionDesInfo->pstNext;
								}
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTRegionDes);
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
					}
				}
			}
		}
	}

	return hError;
}


HERROR xsvc_si_GetTRegionInfoFromNitTsLevel_UkDtt( HUINT32 ulActionId,SIxTable_PsiTable_t *pstNitTable, DbSvc_TsInfo_t *pstTsInfo , HUINT32 ulMaxTRgNum, DbSvc_T_RegionInfo_t *pstTRegionInfo )
{
	HUINT32						ulTRgCnt=0,i;
	HERROR						hError = ERR_OK;
	SIxTargetRegionInfo_t			*pstTRegionDesInfo;
	DbSvc_T_RegionInfo_t			*pstTmpTRegionInfo;
	HUINT8						szCountryCode[4];
	HxList_t 	*pstList = NULL;


	if ( NULL == pstTRegionInfo)
	{
		return ERR_FAIL;
	}
	HxSTD_memset( pstTRegionInfo , 0 , ( sizeof(DbSvc_T_RegionInfo_t) * ulMaxTRgNum ) );
	for ( i = 0 ; i < ulMaxTRgNum ; i++ )
	{
		HxSTD_memset( pstTRegionInfo[i].szCountryCode, 0 , 4 );
		pstTRegionInfo[i].ucRegionDepth = 0xFF; // Region Level 0xFF is invalid.
	}

	if ((pstNitTable != NULL) && (pstTsInfo != NULL))
	{
		HxList_t *pstItem;

		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == pstTsInfo->usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == pstTsInfo->usOnId) )
			{

				for (pstList = pst2ndLoop->pstDesLoop; pstList != NULL; pstList = pstList->next)
				{
					HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
					if (*pucRawDes == eSIxDESCTAG_EXTENSION)
					{
						SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
						if (NULL != pstExtensionDes)
						{
							if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_TARGET_REG)
							{
								SIxTargetRegionDes_t *pstTRegionDes = (SIxTargetRegionDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
								if(NULL != pstTRegionDes)
								{
									pstTRegionDesInfo = pstTRegionDes->pstInfo;
									HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
									if ( NULL == pstTRegionDesInfo)
									{
										if ( ulTRgCnt < ulMaxTRgNum )
										{
											// Only Exist Country Code.
											pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];
											HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
											pstTmpTRegionInfo->ucRegionDepth = 0;
											pstTmpTRegionInfo->ulRegionCode = 0;
											ulTRgCnt++;
										}
									}
									else
									{
										while(pstTRegionDesInfo)
										{
											if ( ulTRgCnt < ulMaxTRgNum )
											{
												pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];

												if ( 0 != pstTRegionDesInfo->ucCountryCodeFlag )
												{
													HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
													HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
												}
												else
													HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );

												pstTmpTRegionInfo->ucRegionDepth = pstTRegionDesInfo->ucRegionDepth;
												switch ( pstTmpTRegionInfo->ucRegionDepth )
												{
												case 0:
													pstTmpTRegionInfo->ulRegionCode = 0;
													break;
												case 1:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													break;
												case 2:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
													break;
												case 3:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)(( pstTRegionDesInfo->stTargetRegionInfo.usTertiaryRegionCode ) & 0x0000FFFF );
													break;
												default:
													pstTmpTRegionInfo->ulRegionCode = 0xFFFFFFFF;
													break;
												}
											}
											else
											{
												HxLOG_Print("[%s:%d] -- ulMaxTRgNum:%ulTRgCnt:%d\n", __FUNCTION__,__LINE__,ulMaxTRgNum,ulTRgCnt);
											}

											ulTRgCnt++;
											pstTRegionDesInfo = pstTRegionDesInfo->pstNext;
										}
									}

									PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstTRegionDes);
								}
							}
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);

						}
					}
				}
			}
		}
	}

	return hError;
}

HERROR xsvc_si_GetTRegionInfoFromSdtSvcLevel (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, DbSvc_TsInfo_t *pstTsInfo , HUINT16 usSvcId,HUINT32 ulMaxTRgNum, DbSvc_T_RegionInfo_t *pstTRegionInfo )
{
	HUINT32						ulTRgCnt=0,i;
	HERROR						hError = ERR_OK;
	DbSvc_T_RegionInfo_t		*pstTmpTRegionInfo;
	HUINT8						szCountryCode[4];
	SIxTargetRegionInfo_t		*pstTRegionDesInfo;
	HxList_t 	*pstList = NULL;


	if ( NULL == pstTRegionInfo)
	{
		HxLOG_Error("Invalid Parameters (pstTRegionInfo is NULL) \n");

		return ERR_FAIL;
	}

	HxSTD_memset( pstTRegionInfo , 0 , ( sizeof(DbSvc_T_RegionInfo_t) * ulMaxTRgNum ) );
	for ( i = 0 ; i < ulMaxTRgNum ; i++ )
	{
		HxSTD_memset( pstTRegionInfo[i].szCountryCode, 0 , 4 );
		pstTRegionInfo[i].ucRegionDepth = 0xFF; // Region Level 0xFF is invalid.
	}

	if ((pstSdtTable != NULL) && (pstTsInfo != NULL))
	{
		HxList_t *pstItem;

		for (pstItem = pstSdtTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if (pst2ndLoop->unInfo.stSdt.usSvcId == usSvcId)
			{

				for (pstList = pst2ndLoop->pstDesLoop; pstList != NULL; pstList = pstList->next)
				{
					HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);
					if (*pucRawDes == eSIxDESCTAG_EXTENSION)
					{
						SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
						if (NULL != pstExtensionDes)
						{
							if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_TARGET_REG)
							{
								SIxTargetRegionDes_t *pstTRegionDes = (SIxTargetRegionDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);

								if(NULL != pstTRegionDes)
								{
									pstTRegionDesInfo = pstTRegionDes->pstInfo;
									HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
									if ( NULL == pstTRegionDesInfo)
									{
										if ( ulTRgCnt < ulMaxTRgNum )
										{
											// Only Exist Country Code.
											pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];
											HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
											pstTmpTRegionInfo->ucRegionDepth = 0;
											pstTmpTRegionInfo->ulRegionCode = 0;
											ulTRgCnt++;
										}
									}
									else
									{
										while(pstTRegionDesInfo)
										{
											if ( ulTRgCnt < ulMaxTRgNum )
											{
												pstTmpTRegionInfo = &pstTRegionInfo[ulTRgCnt];

												if ( 0 != pstTRegionDesInfo->ucCountryCodeFlag )
												{
													HxSTD_memcpy(	(void *)szCountryCode, (void *)pstTRegionDes->ucCountryCode , 4 );
													HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );
												}
												else
													HxSTD_memcpy(	(void *)pstTmpTRegionInfo->szCountryCode, (void *)szCountryCode , 4 );

												pstTmpTRegionInfo->ucRegionDepth = pstTRegionDesInfo->ucRegionDepth;
												switch ( pstTmpTRegionInfo->ucRegionDepth )
												{
												case 0:
													pstTmpTRegionInfo->ulRegionCode = 0;
													break;
												case 1:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													break;
												case 2:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
													break;
												case 3:
													pstTmpTRegionInfo->ulRegionCode = (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucPrimaryRegionCode << 24) & 0xFF000000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)((pstTRegionDesInfo->stTargetRegionInfo.ucSecondaryRegionCode << 16) & 0x00FF0000);
													pstTmpTRegionInfo->ulRegionCode |= (HUINT32)(( pstTRegionDesInfo->stTargetRegionInfo.usTertiaryRegionCode ) & 0x0000FFFF );
													break;
												default:
													pstTmpTRegionInfo->ulRegionCode = 0xFFFFFFFF;
													break;
												}
											}
											else
											{
												HxLOG_Print("[%s:%d] -- ulMaxTRgNum:%ulTRgCnt:%d\n", __FUNCTION__,__LINE__,ulMaxTRgNum,ulTRgCnt);
											}

											ulTRgCnt++;
											pstTRegionDesInfo = pstTRegionDesInfo->pstNext;
										}
									}

									PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstTRegionDes);
								}
							}
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);

						}
					}
				}
			}

		}
	}


	return hError;
}


STATIC HERROR xsvc_si_SrchSetGuidanceInfoFromSdt_UkDtt(HUINT32 ulActionId,SIxTable_PsiTable_t *pstSdtTable, DbSvc_Info_t *pstSvc,HUINT32 ulSrchActionSpec)
{
	HERROR				hErr;
	HUINT16 			usRealTsId, usRealOnId;
	SIxTable_PsiTable_t		*pstCurrSdt;
	HBOOL				bFoundPrivate = FALSE;
	HUINT32 			ulPrivateSpecifyValue;
	HxLANG_Id_e			eMenuLangId, eCurLangId, eUndLangId;
	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;
	SIxUkdttGuidanceDes_t 	*pstUkdttSelGuidanceDesc = NULL;

	if ((pstSdtTable == NULL) || (pstSvc == NULL))
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetPrivateSpecifyValue(ulSrchActionSpec,&ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	eMenuLangId = eLangID_MAX;
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HINT32 *)&eMenuLangId,0);
	if ((hErr != ERR_OK) || (eMenuLangId == eLangID_MAX))
	{
		return ERR_FAIL;
	}
	eUndLangId = eLangID_Undefined;

	// SDT section Loop :
	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{

		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Print("[%s:%d] pstSdtSec is NULL \n", __FUNCTION__, __LINE__);
			continue;
		}
		if (pstCurrSdt->unInfo.stSdt.usTsId == DbSvc_GetTsId(pstSvc) && pstCurrSdt->unInfo.stSdt.usOrgNetId == DbSvc_GetOnId(pstSvc))
		{
			/* Get Service Information from db */
			usRealTsId = pstSdtSec->usTsId;
			usRealOnId = pstSdtSec->usOrgNetId;

			for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
			{
				SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

				if (pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
				{
					bFoundPrivate = FALSE;
					pstUkdttSelGuidanceDesc = NULL;

					for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
					{
						HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

						if ((*pucRawDes == eSIxDESCTAG_UKDTT_GUIDANCE) && (bFoundPrivate == TRUE))
						{
							SIxUkdttGuidanceDes_t *pstUkdttGuidanceDesc = (SIxUkdttGuidanceDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

//							HLIB_CMD_Printf("## ysmoon ## pstUkdttGuidanceDesc (%p) \n\n", pstUkdttGuidanceDesc);

							if (NULL != pstUkdttGuidanceDesc)
							{
								if((pstUkdttGuidanceDesc->ucGuidanceType == 0) || (pstUkdttGuidanceDesc->ucGuidanceType == 1))
								{
									if (pstUkdttSelGuidanceDesc == NULL)
									{
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
//										HLIB_CMD_Printf("## ysmoon ## 1. pstUkdttSelGuidanceDesc (%p) (%p) \n\n", pstUkdttSelGuidanceDesc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo);
									}

									eCurLangId = eLangID_MAX;
									eCurLangId = HLIB_LANG_639ToId(pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->szIso639LangCode);

//									HLIB_CMD_Printf("eCurLangId (0x%x) eMenuLangId(0x%x) eUndLangId(0x%x) \n", eCurLangId, eMenuLangId, eUndLangId);

									if (eCurLangId == eMenuLangId)
									{
										/* Desc Free는 아래에서 사용 후에 */
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
										break;
									}
									else if ((eUndLangId != eLangID_MAX) && (eCurLangId == eUndLangId))
									{
										pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
										break;
									}

									if(pstUkdttSelGuidanceDesc != pstUkdttGuidanceDesc)
									{
										PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttGuidanceDesc);
										pstUkdttGuidanceDesc = NULL;
									}
								}
								else
								{
									if(NULL != pstUkdttGuidanceDesc)
									{
										PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttGuidanceDesc);
										pstUkdttGuidanceDesc = NULL;
									}
								}

							}

						}
						else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
						{
							SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

							if(pPrivDataSpecDes != NULL)
							{
								if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
								{
									bFoundPrivate = TRUE;
								}

								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
							}
						}
						else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
						{
							bFoundPrivate = FALSE;
						}
					}

					if(pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(pstSvc))
					{
						if(pstUkdttSelGuidanceDesc)
						{
//							HLIB_CMD_Printf(" ## ysmoon ## [xsvc_si_search_ukdtt] pstUkdttSelGuidanceDesc(%p) pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo (%p) pstUkdttSelGuidanceDesc->ucGuidanceType(%d) \n\n",
//								pstUkdttSelGuidanceDesc,  pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo, pstUkdttSelGuidanceDesc->ucGuidanceType);

								XdbSvc_TvAnyTimeSetGuidanceType(pstSvc, pstUkdttSelGuidanceDesc->ucGuidanceType);

							if(pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01)
								XdbSvc_TvAnyTimeSetGuidanceMode(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode);
							else
								XdbSvc_TvAnyTimeSetGuidanceMode(pstSvc, 0);

							XdbSvc_TvAnyTimeSetGuidanceStr(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);
						}
						else
						{
							XdbSvc_TvAnyTimeSetGuidanceType(pstSvc, 0xff);
						}
						break;
					}

#if 0

						// Update Guidance Info
						if(pstUkdttSelGuidanceDesc->ucGuidanceType != XdbSvc_TvAnyTimeGetGuidanceType(pstSvc)
							|| (pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01 && pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode != XdbSvc_TvAnyTimeGetGuidanceMode(pstSvc))
							|| 0 != HxSTD_StrCmp(pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance, XdbSvc_TvAnyTimeGetGuidanceStr(pstSvc)))
						{
							bUpdateInform = TRUE;

							XdbSvc_TvAnyTimeSetGuidanceType(pstSvc, pstUkdttSelGuidanceDesc->ucGuidanceType);
							if(pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01)
							{
								XdbSvc_TvAnyTimeSetGuidanceMode(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode);
							}
							else
							{
								XdbSvc_TvAnyTimeSetGuidanceMode(pstSvc, 0);
							}

							XdbSvc_TvAnyTimeSetGuidanceStr(pstSvc, pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);
						}
					}
					else if ((HxSTD_StrLen(XdbSvc_TvAnyTimeGetGuidanceStr(pstSvc)) > 0)
						|| (XdbSvc_TvAnyTimeGetGuidanceType(pstSvc) != 0xFF))
					{
						/* Clear Default Authority */
						bUpdateInform = TRUE;

						XdbSvc_TvAnyTimeSetGuidanceType(pstSvc, 0xFF);
						XdbSvc_TvAnyTimeSetGuidanceMode(pstSvc, 0);
						XdbSvc_TvAnyTimeResetGuidanceStr(pstSvc);
					}
					break;
#endif
					if (pstUkdttSelGuidanceDesc)
					{
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttSelGuidanceDesc);
						pstUkdttSelGuidanceDesc = NULL;
					}
				}
			}
		}
	}

	return ERR_OK;
}



#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SrchMsgCmdStartSearch_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	/* NETWORK search 여부와 상관없이 UK향 지상파 경우는 LCN할당을 위해 NIT를 걸어야 함 */
	if (pstMsg == NULL)
	{
		HxLOG_Error("Error !!  null parameter !! \n");
		return;
	}

	pstMsg->unArg.stStartSrch.stOption.ulNetId = SvcSi_NETID_SEARCH_NITACT;

	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstMsg->hAction));

	(void)svc_si_SrchInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartSrch.unSpec), &(pstMsg->unArg.stStartSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_UKDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_UK);

	// For UKDTT LCN, You should always receive NIT actual.
	pstSrch->eNitOption = eSI_SRCH_NITOPT_NitAct;
	(void)svc_si_SrchGetFilteringTypes (pstSrch);
	(void)svc_si_SrchStartSection (pstSrch);

}

STATIC void xsvc_si_SrchMsgCmdStopSearch_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_UkDtt (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}


STATIC void xsvc_si_SrchProcessMsg_UkDtt (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		HxLOG_Debug("eSIMSG_CMD_START_SEARCH \n");
		xsvc_si_SrchMsgCmdStartSearch_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		HxLOG_Debug("eSIMSG_CMD_STOP_SEARCH \n");
		xsvc_si_SrchMsgCmdStopSearch_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_PAT_TABLE \n");
		xsvc_si_SrchMsgEvtPatTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_PMT_TABLE \n");
		xsvc_si_SrchMsgEvtPmtTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_SDT_TABLE \n");
		xsvc_si_SrchMsgEvtSdtTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Debug("eSIMSG_EVT_NIT_TABLE \n");
		xsvc_si_SrchMsgEvtNitTable_UkDtt (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Debug("eSIMSG_EVT_NIT_OTH_TABLE \n");
		xsvc_si_SrchMsgEvtNitOthTable_UkDtt (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

HERROR xsvc_si_SrchCollectServiceDataFromSi_UkDtt (void *pvData)
{
	svcSi_SrchData_t 		*pstSrch = NULL;
	HUINT32					 ulActionId = 0;
	HUINT32 				 ulCnt, ulTsNum = 0, ulSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstSdtTable = NULL, *pstPatTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	DbSvc_T_RegionInfo_t 	*astTsRegionInfo,*astNetRegionInfo;
#if defined(CONFIG_DEBUG)
	HUINT32					i;
#endif

	if (pvData == NULL)
	{
		HxLOG_Error("null parameter error ! \n");
		return ERR_FAIL;
	}
	pstSrch = (svcSi_SrchData_t *)pvData;
	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);

	// Network
	if (NULL != pstNitTable)
	{
		stNetInfo.usNetId = pstNitTable->unInfo.stNit.usNetId;
		pucRawDes = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes)
		{
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy (stNetInfo.szNetName, pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}

		xsvc_si_SrchSetDefaultAuthoriyInNetwork_UkDtt(ulActionId, pstNitTable, &stNetInfo);
		xsvc_si_SrchSetFTAContentsMgrInNetwork_UkDtt(ulActionId, pstNitTable, &stNetInfo);
		xsvc_si_SrchSetUriLinkageInNetwork_UkDtt(ulActionId, pstNitTable, &stNetInfo);

		// target region
		xsvc_si_SrchCollectTRgNameInfoFromSi_UkDtt( ulActionId , pstNitTable);
		astNetRegionInfo = XdbSvc_TvAnyTimeNetGetRegionInfo(&stNetInfo, 0);
		xsvc_si_GetTRegionInfoFromNitNetLevel_UkDtt( ulActionId, pstNitTable, DxMAX_T_REGION_NUM, astNetRegionInfo);
	}


	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);

	// TS Info
	if (NULL != pstSdtTable)
	{
		stTsInfo.usOnId	= pstSdtTable->unInfo.stSdt.usOrgNetId;
		stTsInfo.usTsId	= pstSdtTable->unInfo.stSdt.usTsId;
	}
	else if (NULL != pstPatTable)
	{
		stTsInfo.usOnId = 0; //onid 0으로 set
		stTsInfo.usTsId = pstPatTable->unInfo.stPat.usTsId;
	}
	else
	{
		HxLOG_Error ("No way to make the TS Info:\n");
		goto END_FUNC;
	}

	stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
	if ( (NULL != pstNitTable) && (NULL != pstSdtTable) )
	{
		HxList_t *pstItem;
		for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

			if ( (pst2ndLoop->unInfo.stNit.usTsId == stTsInfo.usTsId)
				&& (pst2ndLoop->unInfo.stNit.usOrgNetId == stTsInfo.usOnId) )
			{
				pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndLoop, eSIxDESCTAG_FREQUENCY_LIST, 0);
				if (pucRawDes != NULL)
				{
					stTsInfo.utExt.stTvAnyTime.bFreqListed = TRUE;
					break;
				}
			}
		}
		xsvc_si_SrchSetDefaultAuthoriyInTS_UkDtt(ulActionId, pstNitTable, pstSdtTable, &stTsInfo);
		xsvc_si_SrchSetFTAContentsMgrInTS_UkDtt(ulActionId, pstNitTable, pstSdtTable, &stTsInfo);
		// target region
		astTsRegionInfo = XdbSvc_TvAnyTimeTsGetRegionInfo(&stTsInfo, 0);
		xsvc_si_GetTRegionInfoFromNitTsLevel_UkDtt(ulActionId, pstNitTable, &stTsInfo, DxMAX_T_REGION_NUM, astTsRegionInfo);

	}


	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);

	// Delivery TS Info : Network Search 라면.. 아래의 구문 추가
	if (eSI_SRCH_NITOPT_NitAct == pstSrch->eNitOption)
	{
		hErr = svc_si_LibGetAllDeliveryTsInfoFromNit (ulActionId, TRUE, TRUE, &ulTsNum, &pstTsArray);
		if ((ERR_OK == hErr) && (0 != ulTsNum) && (NULL != pstTsArray))
		{
			for (ulCnt = 0; ulCnt < ulTsNum; ulCnt++)
			{
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&(pstTsArray[ulCnt]), NULL);
			}
		}
	}

	//Target Region Descriptor


#if defined(CONFIG_DEBUG)
{
	DbSvc_T_RegionInfo_t *pstTRgInfo;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM; i++ )
	{
		pstTRgInfo = &stNetInfo.utExt.stTvAnyTime.stRegionInfo[i];
		if ( 0xFF != pstTRgInfo->ucRegionDepth)
		{
			HxLOG_Print("NIT Network Level's Region -- [%02d][Country:%s][RgDepth:0x%08X][TRCode:0x%X]\n",
				i,pstTRgInfo->szCountryCode, pstTRgInfo->ucRegionDepth, pstTRgInfo->ulRegionCode);
		}
	}

	for ( i = 0 ; i < DxMAX_T_REGION_NUM; i++ )
	{
		pstTRgInfo = &stTsInfo.utExt.stTvAnyTime.stRegionInfo[i];
		if ( 0xFF != pstTRgInfo->ucRegionDepth)
		{
			HxLOG_Print("NIT TS Level's Region -- [%02d][Country:%s][RgDepth:0x%08X][TRCode:0x%X]\n",
				i,pstTRgInfo->szCountryCode, pstTRgInfo->ucRegionDepth, pstTRgInfo->ulRegionCode);
		}
	}
}
#endif


	// Service Info:
	//SIPC_ComposeSvcInfoFromSI same
	hErr = svc_si_LibComposeServiceInfoInTp (ulActionId, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
	if (ERR_OK == hErr)
	{
		if (NULL != pstSvcArray)
		{
			DbSvc_Info_t		*pstSvc = NULL;

			for (ulCnt = 0; ulCnt < ulSvcNum; ulCnt++)
			{
				DbSvc_T_RegionInfo_t *astRegionInfo;

				pstSvc = &pstSvcArray[ulCnt];
				if (xsvc_si_LibSrchCompareSvcType(pstSrch->stOption.ulSvcType, DbSvc_GetSvcType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 svc type과 다르다
					continue;
				}

				if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 cas type과 다르다
					continue;
				}

				// UK Terrestrial 에서는 MHEG 때문에 Data Channel이 TV Channel List에 끼어야 한다.
				// Data Type -> TV Type로 변환
				if(DbSvc_GetSvcType(pstSvc) == eDxSVC_TYPE_DATA)
				{
					DbSvc_SetSvcType(pstSvc, eDxSVC_TYPE_TV);
				}

//Add to TRD Info
#if defined(CONFIG_DB_SVC_SAVE_ORG_LCN)
				DbSvc_SetOrgLcn(pstSvc, DbSvc_GetLcn(pstSvc));
#endif

				astRegionInfo = XdbSvc_TvAnyTimeGetRegionInfo(pstSvc, 0);
				xsvc_si_GetTRegionInfoFromSdtSvcLevel (ulActionId , pstSdtTable, &stTsInfo, DbSvc_GetSvcId(pstSvc), DxMAX_T_REGION_NUM, astRegionInfo);
#if defined(CONFIG_DEBUG)
				for ( i = 0 ; i < DxMAX_T_REGION_NUM; i++ )
				{
					if (0xFF != astRegionInfo[i].ucRegionDepth)
					{
						HxLOG_Print("SDT SVC Level's Region -- [%d][Country:%s][TRegion:%d][RCode:0x%08X]\n",i, astRegionInfo[i].szCountryCode,
									astRegionInfo[i].ucRegionDepth, astRegionInfo[i].ulRegionCode);
					}
				}
#endif

				xsvc_si_SrchSetDefaultAuthoriyInSvc_UkDtt(ulActionId, pstSdtTable, pstSvc);
				xsvc_si_SrchSetFTAContentsMgrInSvc_UkDtt(ulActionId, pstSdtTable, pstSvc);
				xsvc_si_SrchSetGuidanceInfoFromSdt_UkDtt(ulActionId,pstSdtTable, pstSvc,pstSrch->unSiSpec.stSrchSpec.ulActionSpec);

				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstSvc, NULL);
			}
		}

		if (NULL != pstPrvArray)
		{
			for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
			{
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
			}
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTsArray)					{ OxSI_Free (pstTsArray); }
	if (NULL != pstSvcArray)				{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPatTable)				{ PAL_SIRAWPOOL_FreeTable (pstPatTable); }

	return hResult;
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_UkDtt (void)
{
	return xsvc_si_SrchProcessMsg_UkDtt;
}

void *xsvc_si_SrchAllocSearchActionData_UkDtt (void)
{
	svcSi_SrchData_t	*pstSrch;


	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_UK;
	}
	(void)SVC_SI_InitTrdConflicteSvcHandleTable();
	(void)SVC_SI_ClearNcdNotifyData(); // search  시  NCD Info모두 clear

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_UkDtt (void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL != pstSrch)
	{
		svc_si_SrchFreeSearchData (pstSrch);
		OxSI_Free (pstSrch);
		pstSrch = NULL;
	}

	return;
}
/* end of file */

