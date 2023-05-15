/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_nordig.c
	@brief

	Description:  									\n
	Module: plugin\xtvservice\xsvc\si\nordig		\n

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

#include <pal_pipe.h>

#include <isosvc.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_xsvc_si_lib_service_base.h>
#include <_xsvc_si_lib_search_base.h>
#include <_xsvc_si_lib_lcn_base.h>
#include <_xsvc_si_lib.h>

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
STATIC HUINT32			 s_ulNitCrc32ForDu;
STATIC HUINT32			 s_ulSdtCrc32ForDu;


#define ___LOCAL_FUNCTIONS___

#ifdef CONFIG_DEBUG
const HINT8* INT_UTIL_GETSTR_SvcSi_MsgType(svcSi_MsgType_e eMsgClass)
{
	switch (eMsgClass)
	{
	ENUM_TO_STR(eSIMSG_CMD_START_SESSION);
	ENUM_TO_STR(eSIMSG_CMD_END_SESSION);
	ENUM_TO_STR(eSIMSG_CMD_START_LIVE);
	ENUM_TO_STR(eSIMSG_CMD_STOP_LIVE);
	ENUM_TO_STR(eSIMSG_CMD_START_PB);
	ENUM_TO_STR(eSIMSG_CMD_STOP_PB);
	ENUM_TO_STR(eSIMSG_CMD_START_SEARCH);
	ENUM_TO_STR(eSIMSG_CMD_STOP_SEARCH);
	ENUM_TO_STR(eSIMSG_EVT_SEARCH_DONE);
	ENUM_TO_STR(eSIMSG_EVT_SEARCH_FAIL);
	ENUM_TO_STR(eSIMSG_CMD_START_EXTSEARCH);
	ENUM_TO_STR(eSIMSG_CMD_STOP_EXTSEARCH);
	ENUM_TO_STR(eSIMSG_CMD_START_OTACHECK);
	ENUM_TO_STR(eSIMSG_CMD_STOP_OTACHECK);
	ENUM_TO_STR(eSIMSG_CMD_SHUTDOWN);
	ENUM_TO_STR(eSIMSG_CMD_START_TIMEUPDATE);
	ENUM_TO_STR(eSIMSG_CMD_STOP_TIMEUPDATE);
	ENUM_TO_STR(eSIMSG_EVT_TABLE_START);
	ENUM_TO_STR(eSIMSG_EVT_PAT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_CAT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_PMT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_NIT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_NIT_OTH_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_SDT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_SDT_OTH_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_BAT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_EIT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_EIT_OTH_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_EITSCH_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_EITSCH_OTH_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_TDT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_TOT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_UNT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_DSI_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_ICT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_DDB_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_SGT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_AIT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_DIT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_RCT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_PMT_N_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_BIT_TABLE);
	ENUM_TO_STR(eSIMSG_EVT_TABLE_END);
	ENUM_TO_STR(eSIMSG_EVT_ALL_RECEIVE_START);
	ENUM_TO_STR(eSIMSG_EVT_ALL_RECEIVE_END);
	ENUM_TO_STR(eSIMSG_EVT_EXT_SECTION_START);
	ENUM_TO_STR(eSIMSG_EVT_EXT_SECTION_END);
	default:
		break;
	}

	return ("Unknown");
}

const HINT8* INT_UTIL_GETSTR_SvcSi_SrchNitOption(svcSi_SrchNitOption_e eNitOption)
{
	switch (eNitOption)
	{
	ENUM_TO_STR(eSI_SRCH_NITOPT_NoNit);
	ENUM_TO_STR(eSI_SRCH_NITOPT_NitAct);
	ENUM_TO_STR(eSI_SRCH_NITOPT_NitOth);
	ENUM_TO_STR(eSI_SRCH_NITOPT_EndOfCase);
	default:
		break;
	}

	return ("Unknown");


}
#endif

#define ___DERIVATION________________

STATIC HBOOL _xsvc_sisearch_IsUsePrivateDataSpecifierDescriptor(void)
{
#if defined(CONFIG_OP_TELENOR)
	return FALSE;
#elif defined(CONFIG_OP_TDC)
	return TRUE;
#else
	// default: CONFIG_OP_NORDIG -> Use
	return TRUE;
#endif
}

STATIC void _xsvc_sisearch_GetFilteringType_Nordig(svcSi_SrchData_t *pstSrch)
{
	if(NULL == pstSrch)
	{
		HxLOG_Error("param is null\n");
		return;
	}

	// 1) in different cases of Nordig basic
#if defined(CONFIG_OP_TELENOR) || defined(CONFIG_OP_TDC)
	switch(pstSrch->stOption.ulSearchType)
	{
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased:
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate:
		pstSrch->eFiltering = (bSI_FILTERING_NIT_ACT | bSI_FILTERING_SDT_ACT);
		return;

	default:
		break;
	}
#endif


	// 2) Nordig basic
	switch(pstSrch->stOption.ulSearchType)
	{
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased:
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate:
		pstSrch->eFiltering = (bSI_FILTERING_NIT_ACT | bSI_FILTERING_SDT_ACT | bSI_FILTERING_SDT_OTH);
		break;

	case eSVCSI_SEARCHTYPE_NORDIG_NitOthBased:
		pstSrch->eFiltering = (bSI_FILTERING_NIT_OTH | bSI_FILTERING_SDT_ACT | bSI_FILTERING_SDT_OTH);
		break;

	case eSVCSI_SEARCHTYPE_NORDIG_WithoutNit:
		pstSrch->eNitOption = eSI_SRCH_NITOPT_NoNit;
		(void)svc_si_SrchGetFilteringTypes (pstSrch);
		break;

	default:
		// For Nordig LCN, You should always receive NIT actual.
		pstSrch->eNitOption = eSI_SRCH_NITOPT_NitAct;
		(void)svc_si_SrchGetFilteringTypes (pstSrch);

		if (pstSrch->stOption.ulSearchType != 0)
		{
			pstSrch->eFiltering |= (bSI_FILTERING_SDT_OTH);
		}
		break;
	}
}


#define ___MEMBER_FUNCTIONS___

STATIC HBOOL	_xsvc_sisearch_IsFoundPrivateDataSpecifierDescriptor(HUINT32 ulActionId, XsvcSi_ActionSpec_e eSiActSpec, HUINT8 *pucDesRaw)
{
	HERROR		hErr = ERR_FAIL;
	HUINT32		ulPrivateSpecifyValue = 0;
	HBOOL		bFoundPrivate = FALSE;

	if(NULL == pucDesRaw)
	{
		HxLOG_Error ("pucDesRaw is NULL\n");
		return bFoundPrivate;
	}

	hErr = svc_si_GetPrivateSpecifyValue(eSiActSpec, &ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return bFoundPrivate;
	}

	if(eSIxDESCTAG_PRIVATE_DATA_SPECIFIER == *pucDesRaw)
	{
		SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

		if(NULL != pPrivDataSpecDes)
		{
			if (ulPrivateSpecifyValue == pPrivDataSpecDes->ulPrivateDataSpec)
			{
				bFoundPrivate = TRUE;
				HxLOG_Info(HxANSI_COLOR_YELLOW("eSiActSpec = %d, ulPrivateSpecifyValue = 0x%x\n"), eSiActSpec, ulPrivateSpecifyValue);
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
		}
	}

	return bFoundPrivate;
}

#ifdef CONFIG_OP_TDC
STATIC HERROR	_xsvc_sisearch_GetTdcLcn(HUINT32 ulActionId, HUINT16 usSvcId, HUINT8 *pucDesRaw, xsvcSi_LcnInfo_Nordig_t *pstLcnInfo)
{
	HBOOL	bLcnFound = FALSE;

	if((NULL == pucDesRaw) || (NULL == pstLcnInfo))
	{
		return ERR_FAIL;
	}

	HxLOG_Info("usSvcId = 0x%x, Tag: 0x%x\n", usSvcId, *pucDesRaw);
	// Service Info (Service Type & LCN)
	if(eSIxDESCTAG_TDC_CHANNEL == *pucDesRaw)
	{
		SIxTdcChannelDes_t	*pstTdcChDes = (SIxTdcChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
		if (NULL != pstTdcChDes)
		{
			SIxTdcChannelInfo_t *pChListItem = pstTdcChDes->pstInfo;

			while(pChListItem)
			{
				HxLOG_Info("usSvcId = 0x%x, pChListItem->usServiceId = 0x%x\n", usSvcId, pChListItem->usServiceId);
				if(usSvcId == pChListItem->usServiceId)
				{
					HxSTD_MemSet(pstLcnInfo, 0, sizeof(xsvcSi_LcnInfo_Nordig_t));

					pstLcnInfo->usServiceId = pChListItem->usServiceId;
					pstLcnInfo->ucVisibleServiceFlag = TRUE; /* always visible in TDC (YouSee) */
					pstLcnInfo->usLogicalChannelNumber = pChListItem->usChannelNumber;
					bLcnFound = TRUE;
					HxLOG_Info("\t[LCN] %d, svcid = 0x%x\n", pstLcnInfo->usLogicalChannelNumber, pstLcnInfo->usServiceId);
					break;
				}

				pChListItem = pChListItem->pstNext;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTdcChDes);

			if(TRUE == bLcnFound)
				return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif


STATIC void	_xsvc_sisearch_DeleteChannels(HxList_t *pChList)
{
	if (pChList)
	{
		HxList_t	*pstItem;

		HLIB_LIST_First(pChList);
		for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pChList);
	}
}

STATIC void	_xsvc_sisearch_DeleteLcnInfoList(HxList_t *pLcnInfoList)
{
	if (pLcnInfoList)
	{
		HxList_t	*pstItem;

		HLIB_LIST_First(pLcnInfoList);
		for (pstItem = pLcnInfoList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pLcnInfoList);
	}
}

STATIC HERROR _xsvc_sisearch_GetLcnInfoList(HUINT32 ulActionId, DbSvc_TsInfo_t *pstTsInfo, HxList_t *pst2ndDesLoop, HxList_t **ppLcnInfoList)
{
	HxList_t						*pstItem = NULL;
	HBOOL							bLcdV2Found = FALSE;
	HBOOL							bLcdV1Found = FALSE;
	HBOOL							bUsePds = TRUE;
	HBOOL							bFoundNordigPrivate = FALSE;
	HxList_t						*pLcnInfoList = NULL;
	HUINT8							*pucDesRaw = NULL;
	xsvcSi_LcnInfo_Nordig_t 		*pstLcnInfo;
	SIxNordigLogicalChannelv2Des_t	*pstLcdV2 = NULL;
	SIxNordigLogicalChannelDes_t	*pstLcdV1 = NULL;

	HxLOG_Debug(HxANSI_COLOR_YELLOW("Enter > ComposeLcdBasedServiceInfo_Nordig\n"));

	if ((NULL == pst2ndDesLoop) || (NULL == ppLcnInfoList) || (NULL == pstTsInfo))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	bUsePds = _xsvc_sisearch_IsUsePrivateDataSpecifierDescriptor();
	HxLOG_Info("Use Private Data Specifier? (%d)\n", bUsePds);

	if(TRUE == bUsePds)
	{ 	// PDS를 꼭 써야 되면
		bFoundNordigPrivate = FALSE;

		for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);

			bFoundNordigPrivate = _xsvc_sisearch_IsFoundPrivateDataSpecifierDescriptor(ulActionId, eSVCSI_ACT_SPEC_Nordig, pucDesRaw);

			if(TRUE == bFoundNordigPrivate)
			{
				pst2ndDesLoop = pstItem; // PDS 이후부터 Descriptor가 유효함.
				break;
			}
		}

		HxLOG_Info("bFoundNordigPrivate (%d)\n", bFoundNordigPrivate);

		if(TRUE != bFoundNordigPrivate)
		{
			return FALSE;
		}
	}
	else
	{ 	// PDS를 안 써도 되면 그냥LCD 만 처리
		HxLOG_Info("Not use Private Data Specifier \n");
	}


	pLcnInfoList = *ppLcnInfoList;


	// (1) Finding LCD V2
	for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);

		if (eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2 != *pucDesRaw)
		{
			continue;
		}

		pstLcdV2 = (SIxNordigLogicalChannelv2Des_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
		if (NULL != pstLcdV2)
		{
			SIxNordigLogicalChannelv2Info_t 	*pChSvcItem = pstLcdV2->pstInfo;
			SIxNordigLogicalChannelv2SvcInfo_t	*pChListItem = pChSvcItem->pstInfo;

			while(pChListItem)
			{
				pstLcnInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_Nordig_t));

				if(NULL != pstLcnInfo)
				{
					pstLcnInfo->usOnId = pstTsInfo->usOnId;
					pstLcnInfo->usTsId = pstTsInfo->usTsId;
					pstLcnInfo->usServiceId = pChListItem->usServiceId;
					pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
					pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

					pLcnInfoList = HLIB_LIST_Append(pLcnInfoList, (void *)pstLcnInfo);
					bLcdV2Found = TRUE;
				}

				pChListItem = pChListItem->pstNext;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLcdV2);
		}
	}

	if(bLcdV2Found == TRUE)
	{
		goto END_FUNC;
	}

	// (2) Finding LCD V1
	for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);

		if (eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL != *pucDesRaw)
		{
			continue;
		}

		pstLcdV1 = (SIxNordigLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
		if (NULL != pstLcdV1)
		{
			SIxNordigLogicalChannelInfo_t	*pChListItem = pstLcdV1->pstInfo;

			while(pChListItem)
			{
				pstLcnInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_Nordig_t));

				if(NULL != pstLcnInfo)
				{
					pstLcnInfo->usOnId = pstTsInfo->usOnId;
					pstLcnInfo->usTsId = pstTsInfo->usTsId;
					pstLcnInfo->usServiceId = pChListItem->usServiceId;
					pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
					pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

					pLcnInfoList = HLIB_LIST_Append(pLcnInfoList, (void *)pstLcnInfo);
					bLcdV1Found = TRUE;
				}

				pChListItem = pChListItem->pstNext;
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLcdV1);
		}
	}

END_FUNC:

	if((bLcdV2Found == TRUE) || (bLcdV1Found == TRUE))
	{
		*ppLcnInfoList = pLcnInfoList;
		HxLOG_Debug(HxANSI_COLOR_YELLOW("Exit - LCD found v2(%d), v1(%d)\n"), bLcdV2Found, bLcdV1Found);
		return ERR_OK;
	}
	else
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("Exit - LCD not found\n"));
		return ERR_FAIL;
	}
}

STATIC HERROR _xsvc_sisearch_SetLcnInfo(DbSvc_Info_t *pstSvcInfo, HxList_t *pLcnInfoList)
{
	HBOOL		 hErr = ERR_FAIL;
	HxList_t	*pstItem = NULL;

	HUINT16		 usSvcOnid = DbSvc_GetOnId(pstSvcInfo);
	HUINT16		 usSvcTsid = DbSvc_GetTsId(pstSvcInfo);
	HUINT16		 usSvcSid  = DbSvc_GetSvcId(pstSvcInfo);

	HxLOG_Debug(" [SET LCN INFO] LCN Info Num = [%d] \n", HLIB_LIST_Length(pLcnInfoList));

	DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
	DbSvc_SetLcn(pstSvcInfo, 0);
	DbSvc_SetVisibleFlag(pstSvcInfo, TRUE);

	HLIB_LIST_First(pLcnInfoList);

	for (pstItem = pLcnInfoList; NULL != pstItem; pstItem = pstItem->next)
	{
		xsvcSi_LcnInfo_Nordig_t 	*pstLcnInfo = (xsvcSi_LcnInfo_Nordig_t *)HLIB_LIST_Data(pstItem);

		if((usSvcOnid == pstLcnInfo->usOnId) && (usSvcTsid == pstLcnInfo->usTsId) && (usSvcSid == pstLcnInfo->usServiceId))
		{
			DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
			DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usLogicalChannelNumber);
			DbSvc_SetVisibleFlag(pstSvcInfo, pstLcnInfo->ucVisibleServiceFlag);
			HxLOG_Debug(" [SET LCN INFO]  SVC Name = [%s], LCN = [%d] \n", DbSvc_GetName(pstSvcInfo), pstLcnInfo->usLogicalChannelNumber);

			hErr = ERR_OK;
			break;
		}
	}

	if(hErr == ERR_FAIL)
	{
		HxLOG_Print(" [SET LCN INFO]   SVC Name = [%s] : Can not found a LCN \n", DbSvc_GetName(pstSvcInfo));
	}

	return hErr;
}

// SLD + (LCD or Channel Desc.)
STATIC HERROR _xsvc_sisearch_ComposeSldBasedServiceInfo_Nordig(HUINT32 ulActionId, DbSvc_TsInfo_t *pstTsInfo, HxList_t *pst2ndDesLoop, HxList_t **ppChList)
{
	HERROR			hErr = ERR_OK;
	HUINT32			ulSvcNum = 0;
	HxList_t		*pstItem = NULL;
	HxList_t 		*pChList = NULL;
	HUINT8 			*pucDesRaw = NULL;

	HxLOG_Info(HxANSI_COLOR_YELLOW("Enter > ComposeSldBasedServiceInfo_Nordig \n"));

	if ((NULL == pst2ndDesLoop) || (NULL == ppChList))
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pChList = *ppChList;

	for (pstItem = pst2ndDesLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
		if(eSIxDESCTAG_SERVICE_LIST == *pucDesRaw)
		{
			SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
			HUINT8			*pszUndefinedSvcName = NULL;
			HUINT32			 ulDvbSvcType;
			DxSvcType_e 	 eSvcType;
			DxVideoType_e	 eVideoType;
			DxOperator_e	 eSvcOpType = eDxOPERATOR_None;

			if (NULL != pstSvcListDes)
			{
				SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
				while(pSvcListItem)
				{
					DbSvc_Info_t				*pstSvcInfo = NULL;

					pstSvcInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(DbSvc_Info_t));
					if(NULL != pstSvcInfo)
					{
						DB_SVC_InitServiceInfo(pstSvcInfo);

						DbSvc_SetTsIdx(pstSvcInfo, pstTsInfo->usTsIdx);
						DbSvc_SetOnId(pstSvcInfo, pstTsInfo->usOnId);
						DbSvc_SetTsId(pstSvcInfo, pstTsInfo->usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pSvcListItem->usSvcId);

						// Service name
						hErr = svc_si_LibGetUndefinedServiceName(ulActionId, &pszUndefinedSvcName);
						if ((ERR_OK == hErr) && (NULL != pszUndefinedSvcName))
						{
							DbSvc_SetName(pstSvcInfo, pszUndefinedSvcName);
						}

						// guess service type - tv or radio
						ulDvbSvcType = pSvcListItem->ucServType;

						// Service type, Video Type
						eSvcType = eDxSVC_TYPE_All;
						eVideoType = eDxVIDEO_TYPE_ALL;
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, ulDvbSvcType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: fixed SvcType(%d)\n", ulDvbSvcType);
						}
						else
						{
							HxLOG_Info("\t[SVC] (0x%x:%s), SvcType(%d, %d)\n", DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo), ulDvbSvcType, eSvcType);

							DbSvc_SetSvcType(pstSvcInfo, eSvcType);
							DbSvc_SetVideoType(pstSvcInfo, eVideoType);

							// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
							DbSvc_SetOrgSvcType(pstSvcInfo, ulDvbSvcType);

							// op type
							hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
							if (hErr != ERR_OK)
							{
								HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
								eSvcOpType = eDxOPERATOR_None;
							}
							DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

							pChList = HLIB_LIST_Append(pChList, (void *)pstSvcInfo);
							ulSvcNum++;
						}
					}

					pSvcListItem = pSvcListItem->pstNext;
				}
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
			}
		}
	}

	if(0 == ulSvcNum)
	{
		return ERR_FAIL;
	}
	else
	{
		*ppChList = pChList;
		return ERR_OK;
	}
}

STATIC HERROR _xsvc_sisearch_ComposeLcdBasedServiceInfo_Nordig(HUINT32 ulActionId, DbSvc_TsInfo_t *pstTsInfo, HxList_t *pLcnInfoList, HxList_t **ppChList)
{
	HBOOL		 	 hErr = ERR_FAIL;
	HUINT32			 ulSvcNum = 0;
	HxList_t		*pstItem = NULL;
	HxList_t 		*pChList = NULL;
	HUINT8			*pszUndefinedSvcName = NULL;
	DxOperator_e	 eSvcOpType = eDxOPERATOR_None;

	if (NULL == ppChList)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pChList = *ppChList;
	HLIB_LIST_First(pLcnInfoList);

	for (pstItem = pLcnInfoList; NULL != pstItem; pstItem = pstItem->next)
	{
		xsvcSi_LcnInfo_Nordig_t 	*pstLcnInfo = (xsvcSi_LcnInfo_Nordig_t *)HLIB_LIST_Data(pstItem);
		DbSvc_Info_t				*pstSvcInfo = NULL;

		if((pstLcnInfo->usOnId == pstTsInfo->usOnId) && (pstLcnInfo->usTsId == pstTsInfo->usTsId))
		{
			pstSvcInfo = (DbSvc_Info_t *)OxSI_Malloc(sizeof(DbSvc_Info_t));
			if(NULL != pstSvcInfo)
			{
				DB_SVC_InitServiceInfo(pstSvcInfo);

				DbSvc_SetTsIdx(pstSvcInfo, pstTsInfo->usTsIdx);
				DbSvc_SetOnId(pstSvcInfo, pstTsInfo->usOnId);
				DbSvc_SetTsId(pstSvcInfo, pstTsInfo->usTsId);
				DbSvc_SetSvcId(pstSvcInfo, pstLcnInfo->usServiceId);

				// Service name
				pszUndefinedSvcName = NULL;
				hErr = svc_si_LibGetUndefinedServiceName(ulActionId, &pszUndefinedSvcName);
				if ((ERR_OK == hErr) && (NULL != pszUndefinedSvcName))
				{
					DbSvc_SetName(pstSvcInfo, pszUndefinedSvcName);
				}

				// guess service type - tv or radio
				// LCD 가지고는 Service Type 알수 없는데.....
				DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_TV);
				DbSvc_SetOrgSvcType(pstSvcInfo, eDxOrgSVC_TYPE_DIGITAL_TV);
				DbSvc_SetVideoType(pstSvcInfo, eDxVIDEO_TYPE_ALL);

				// op type
				hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
					eSvcOpType = eDxOPERATOR_None;
				}
				DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

				pChList = HLIB_LIST_Append(pChList, (void *)pstSvcInfo);
				ulSvcNum++;
			}
		}
	}

	if(0 == ulSvcNum)
	{
		return ERR_FAIL;
	}
	else
	{
		*ppChList = pChList;
		return ERR_OK;
	}
}

STATIC HERROR _xsvc_sisearch_checkComposeLcdBasedSVC(svcSi_SrchData_t *pstSrch, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR bComposeSvc = ERR_FAIL;

#if defined(CONFIG_OP_TELENOR)
	if(pstSrch->stOption.ulSearchType == eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate)
	{
			// Telenor - B2 Network 에서는 SLD가 없다. Telenor 요구로 SLD가 없는 B2 Network에서도 Dynamic Update로
			// 전체 채널에 대한 Dynamic Update가 가능 하도록 SLD가 없을 경우 LCD를 이용해 서비스 추가 될 수 있도록 한다.
			if(xsvc_si_isTelenorB2_Network(pstTsInfo->usOnId) == TRUE)
			{
				bComposeSvc = ERR_OK;
			}
	}
#endif

	return bComposeSvc;
}

STATIC HERROR _xsvc_sisearch_ComposeSdtBasedServiceInfo_Nordig(HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT32 				 ulSvcNum = 0, ulPrvNum = 0;
	HUINT32 				 ulActSvcNum = 0, ulActPrvNum = 0;
	HUINT32 				 ulOthSvcNum = 0, ulOthPrvNum = 0;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_Info_t			*pstActSvcArray = NULL;
	DbSvc_Info_t			*pstOthSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstActPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstOthPrvArray = NULL;

	SIxTable_PsiTable_t		*pstSdtActTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	HERROR					 hErr1 = ERR_OK, hErr2 = ERR_OK;
	HBOOL					bMemAlloc = TRUE;

	if (NULL != pstSdtActTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT ACT\n");
		hErr1 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtActTbl, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
		ulSvcNum = ulActSvcNum;
		ulPrvNum = ulActPrvNum;
	}

	if (NULL != pstSdtOthTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT OTH\n");
		hErr2 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtOthTbl, &ulOthSvcNum, &pstOthSvcArray, &ulOthPrvNum, &pstOthPrvArray);
		ulSvcNum += ulOthSvcNum;
		ulPrvNum += ulOthPrvNum;
	}

	if (ulSvcNum > 0)
	{
		HxLOG_Info("ulSvcNum: %d \n", ulSvcNum);

		pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
		if (NULL == pstSvcArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstSvcArray, pstActSvcArray, sizeof(DbSvc_Info_t) * ulActSvcNum);
		HxSTD_memcpy(&pstSvcArray[ulActSvcNum], pstOthSvcArray, sizeof(DbSvc_Info_t) * ulOthSvcNum);

		*ppstSvcArray = pstSvcArray;
		pstSvcArray = NULL;
		*pulSvcNum = ulSvcNum;
	}

	if (ulPrvNum > 0)
	{
		HxLOG_Info("ulPrvNum: %d \n", ulPrvNum);

		pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (NULL == pstPrvArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstPrvArray, pstActPrvArray, sizeof(DbSvc_PrvInfo_t) * ulActPrvNum);
		HxSTD_memcpy(&pstPrvArray[ulActPrvNum], pstOthPrvArray, sizeof(DbSvc_PrvInfo_t) * ulOthPrvNum);

		*ppstPrvArray = pstPrvArray;
		pstPrvArray = NULL;
		*pulPrvNum = ulPrvNum;

	}

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstActSvcArray)					{ OxSI_Free (pstActSvcArray); }
	if (NULL != pstOthSvcArray)					{ OxSI_Free (pstOthSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }
	if (NULL != pstActPrvArray)					{ OxSI_Free (pstActPrvArray); }
	if (NULL != pstOthPrvArray)					{ OxSI_Free (pstOthPrvArray); }
	if (NULL != pstSdtActTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtActTbl); }
	if (NULL != pstSdtOthTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTbl); }

	return (((hErr1 == ERR_OK || hErr2 == ERR_OK) && bMemAlloc == TRUE) ? ERR_OK : ERR_FAIL);
}

STATIC HERROR	_xsvc_sisearch_NORDIG_IsTestTransponder(HUINT16 usNetworkId, HUINT16 usOnId)
{
	if (0xff01 <= usNetworkId)
	{
		return TRUE;
	}

	if (0xff00 <= usOnId)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR	_xsvc_sisearch_GetResults_NORDIG_NitActSearch(svcSi_SrchData_t *pstSrch, HUINT32 ulActionId)
{
	HUINT32 				 ulCnt, ulSubCnt, ulTsNum = 0, ulNitSvcNum = 0, ulSdtSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes1stLoop;
	DbSvc_Info_t			*pstSdtSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL;
	SIxTable_PsiTable_t		*pstSdtTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	HxList_t				*pstItem;
	HxList_t				*pstItem1, *pstItem2;
	HxList_t 				*pChList = NULL;
	HxList_t 				*pLcnInfoList = NULL;
	HBOOL					 bDsdFound;
	HBOOL					 bNitCRC = TRUE;
	HBOOL					 bSdtCRC = TRUE;

	HxLOG_Print("++ NORDIG_NITSearch ++\n");

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstNitTable)
	{
		goto END_FUNC;
	}

	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);

	if(pstSrch->stOption.ulSearchType == eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate)
	{
		if (pstNitTable != NULL)
		{
			if(s_ulNitCrc32ForDu != pstNitTable->unInfo.stNit.ulCrc32)
			{
				s_ulNitCrc32ForDu = pstNitTable->unInfo.stNit.ulCrc32;
			}
			else
			{
				bNitCRC = FALSE;
				HxLOG_Info(" >>>>> This NIT CRC value was updated changes before..... [0x%X] \n", pstNitTable->unInfo.stNit.ulCrc32);
			}
		}

		if (pstSdtTable != NULL)
		{
			if  (s_ulSdtCrc32ForDu != pstSdtTable->unInfo.stSdt.ulCrc32)
			{
				s_ulSdtCrc32ForDu = pstSdtTable->unInfo.stSdt.ulCrc32;
			}
			else
			{
				bSdtCRC = FALSE;
				HxLOG_Info(" >>>>> This SDT CRC value was updated changes before..... [0x%X] \n", pstSdtTable->unInfo.stSdt.ulCrc32);
			}
		}

		if ( (bNitCRC == FALSE) && (bSdtCRC == FALSE) )
		{
			HxLOG_Error(" >>>>> This CRC value was updated changes before..... \n");
			goto END_FUNC;
		}

	}

	/* Network Info */
	HxSTD_MemSet(&stNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
	stNetInfo.usNetId = pstNitTable->unInfo.stNit.usNetId;

	pucRawDes1stLoop = SILIB_TABLE_Get1stLoopDescriptor (pstNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
	if (NULL != pucRawDes1stLoop)
	{
		SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes1stLoop);
		if (NULL != pstNameDes)
		{
			HxSTD_StrNCpy (stNetInfo.szNetName, pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
		}
	}
	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);
	HxLOG_Print("[NET] id = 0x%x, name = %s\n", stNetInfo.usNetId, stNetInfo.szNetName);

	// TS Info
	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	if (NULL != pstSdtTable)
	{
		stTsInfo.usOnId	= pstSdtTable->unInfo.stSdt.usOrgNetId;
		stTsInfo.usTsId	= pstSdtTable->unInfo.stSdt.usTsId;
	}
	else
	{
		HxLOG_Error ("No way to make the TS Info:\n");
		goto END_FUNC;
	}

	stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);

	//--------------------------------------------------------------------
	// 1. NIT에서 데이터 추출
	//--------------------------------------------------------------------
	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
		if (NULL != pst2ndLoop)
		{
			stTsInfo.usOnId	= pst2ndLoop->unInfo.stNit.usOrgNetId;
			stTsInfo.usTsId	= pst2ndLoop->unInfo.stNit.usTsId;
		}
		else
		{
			HxLOG_Error ("No way to make the TS Info:\n");
			goto END_FUNC;
		}
		stTsInfo.usNetIdx = (HUINT16)ulNetIdx;

		/* Ts Info */
		// (1) Update delivery system info
		bDsdFound = FALSE;
		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8		*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			hErr = svc_si_LibGetDeliveryTsInfoFromDsd(ulActionId, pucDesRaw, &stTsInfo);
			if(ERR_OK == hErr)
			{
				stTsInfo.usTsIdx = ulTsNum;
				(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Delivery, (void *)&stTsInfo, &ulTsIdx);
				ulTsNum++;
				HxLOG_Print("[TS](%d) onid/tsid, freq/sr/cons = 0x%x/0x%x/%d/%d/%d\n"
								, ulTsIdx, stTsInfo.usOnId, stTsInfo.usTsId
								, stTsInfo.stTuningParam.cab.ulFreq, stTsInfo.stTuningParam.cab.usSR, stTsInfo.stTuningParam.cab.ucCon);
				bDsdFound = TRUE;
				break;
			}
		}

		if(FALSE == bDsdFound)
			continue;

		// (2) Get LCN Info List from NIT
		_xsvc_sisearch_GetLcnInfoList(ulActionId, &stTsInfo, pst2ndLoop->pstDesLoop, &pLcnInfoList);

		// (3) Make Service List from SLD
		hErr = _xsvc_sisearch_ComposeSldBasedServiceInfo_Nordig(ulActionId, &stTsInfo, pst2ndLoop->pstDesLoop, &pChList);
		if(ERR_OK == hErr)
		{
			HxLOG_Debug("properly updated with SLD\n");
		}
		else
		{
			hErr = _xsvc_sisearch_checkComposeLcdBasedSVC(pstSrch, &stTsInfo);

			if(ERR_OK == hErr)
			{
				_xsvc_sisearch_ComposeLcdBasedServiceInfo_Nordig(ulActionId, &stTsInfo, pLcnInfoList, &pChList);
			}
		}
	}
	/////////////////////////////////////////////////////////////////


	//--------------------------------------------------------------------
	// 2. SDT에서 데이터 추출
	//--------------------------------------------------------------------
	hErr = _xsvc_sisearch_ComposeSdtBasedServiceInfo_Nordig(ulActionId, &ulSdtSvcNum, &pstSdtSvcArray, &ulPrvNum, &pstPrvArray);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make sdt base service info\n");
	}
	/////////////////////////////////////////////////////////////////


	// Provider Info
	if (NULL != pstPrvArray)
	{
		for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
		{
			(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
		}
	}

	ulNitSvcNum = HLIB_LIST_Length(pChList);
	HxLOG_Print(HxANSI_COLOR_YELLOW("ulNitSvcNum = %d, ulSdtSvcNum = %d\n"), ulNitSvcNum, ulSdtSvcNum);

	// Service Info
	ulCnt = 0;
	HLIB_LIST_First(pChList);
	for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
	{
		DbSvc_Info_t 	*pstNitSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstItem);
		HBOOL			bSameSvcFound = FALSE;

		// Update service name
		for (ulSubCnt = 0; ulSubCnt < ulSdtSvcNum; ulSubCnt++)
		{
			DbSvc_Info_t	*pstSdtSvcInfo = &(pstSdtSvcArray[ulSubCnt]);

			if (DbSvc_GetTsId(pstNitSvcInfo) == DbSvc_GetTsId(pstSdtSvcInfo)
				&& DbSvc_GetOnId(pstNitSvcInfo) == DbSvc_GetOnId(pstSdtSvcInfo)
				&& DbSvc_GetSvcId(pstNitSvcInfo) == DbSvc_GetSvcId(pstSdtSvcInfo))
			{
				DbSvc_SetName(pstNitSvcInfo, DbSvc_GetName(pstSdtSvcInfo));
				DbSvc_SetSvcType(pstNitSvcInfo, DbSvc_GetSvcType(pstSdtSvcInfo));

				// set simulcast id from SDT
				DbSvc_SetSimulcastOnId(pstNitSvcInfo, DbSvc_GetSimulcastOnId(pstSdtSvcInfo));
				DbSvc_SetSimulcastTsId(pstNitSvcInfo, DbSvc_GetSimulcastTsId(pstSdtSvcInfo));
				DbSvc_SetSimulcastSvcId(pstNitSvcInfo, DbSvc_GetSimulcastSvcId(pstSdtSvcInfo));

				bSameSvcFound = TRUE;

				HxLOG_Print("[SVC:%d] [%3d. %s] type:%d, id=(0x%x,0x%x), idx=(%d,%d,%d), simulcast id=(0x%x,0x%x,0x%x) \n"
					, ulCnt+1
					, DbSvc_GetLcn(pstNitSvcInfo)
					, DbSvc_GetName(pstNitSvcInfo)
					, DbSvc_GetSvcType(pstNitSvcInfo)
					, DbSvc_GetTsId(pstNitSvcInfo)
					, DbSvc_GetSvcId(pstNitSvcInfo)
					, DbSvc_GetTsIdx(pstNitSvcInfo)
					, DbSvc_GetSvcIdx(pstNitSvcInfo)
					, DbSvc_GetPrvIdx(pstNitSvcInfo)
					, DbSvc_GetSimulcastOnId(pstNitSvcInfo)
					, DbSvc_GetSimulcastTsId(pstNitSvcInfo)
					, DbSvc_GetSimulcastSvcId(pstNitSvcInfo)
					);

				if(_xsvc_sisearch_NORDIG_IsTestTransponder(stNetInfo.usNetId, DbSvc_GetOnId(pstNitSvcInfo)) == FALSE)
				{
					(void)_xsvc_sisearch_SetLcnInfo(pstNitSvcInfo, pLcnInfoList);
					(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstNitSvcInfo, NULL);
					ulCnt++;
				}
				else
				{
					HxLOG_Error ("This TP Is Test Stream for Nordig (Do Not ADD) - NID(%d), ONID(%d)  \n", stNetInfo.usNetId, DbSvc_GetOnId(pstNitSvcInfo));
				}
				break;
			}

		}

		if(TRUE != bSameSvcFound)
		{
			HxLOG_Print("[SVC] [%d. %s] id=(0x%x,0x%x,0x%x) no same svc in sdt\n"
				, DbSvc_GetLcn(pstNitSvcInfo)
				, DbSvc_GetName(pstNitSvcInfo)
				, DbSvc_GetOnId(pstNitSvcInfo)
				, DbSvc_GetTsId(pstNitSvcInfo)
				, DbSvc_GetSvcId(pstNitSvcInfo)
				);

			if(pstSrch->stOption.ulSearchType == eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate)
			{
				// Service가 NIT에만 있고 SDT에는 없다고 해도 추가 해준다.
				// Dynamic Update, Add TP를 위해 필요함.

				Handle_t			*phDbSvcList = NULL;
				DbSvc_Info_t		*pstDbSvcArray = NULL;
				DbSvc_Condition_t	 stSvcCond;
				HUINT32				 ulSvcNum = 0;


				DB_SVC_InitSvcFindCondition(&stSvcCond);
				stSvcCond.nOnId = DbSvc_GetOnId(pstNitSvcInfo);
				stSvcCond.nTsId= DbSvc_GetTsId(pstNitSvcInfo);
				stSvcCond.nSvcId= DbSvc_GetSvcId(pstNitSvcInfo);

				hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstDbSvcArray);
				hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phDbSvcList);

				if(hErr == ERR_OK)
				{
					DbSvc_Info_t	*pstDbSvcInfo = &(pstDbSvcArray[0]);

					DbSvc_SetName(pstNitSvcInfo, DbSvc_GetName(pstDbSvcInfo));
					DbSvc_SetSvcType(pstNitSvcInfo, DbSvc_GetSvcType(pstDbSvcInfo));

					DbSvc_SetSimulcastOnId(pstNitSvcInfo, DbSvc_GetSimulcastOnId(pstDbSvcInfo));
					DbSvc_SetSimulcastTsId(pstNitSvcInfo, DbSvc_GetSimulcastTsId(pstDbSvcInfo));
					DbSvc_SetSimulcastSvcId(pstNitSvcInfo, DbSvc_GetSimulcastSvcId(pstDbSvcInfo));

					DB_SVC_FreeServiceInfoList(NULL, phDbSvcList, pstDbSvcArray);
				}

				if(_xsvc_sisearch_NORDIG_IsTestTransponder(stNetInfo.usNetId, DbSvc_GetOnId(pstNitSvcInfo)) == FALSE)
				{
					(void)_xsvc_sisearch_SetLcnInfo(pstNitSvcInfo, pLcnInfoList);
					(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstNitSvcInfo, NULL);
					ulCnt++;
				}
				else
				{
					HxLOG_Error ("This TP Is Test Stream for Nordig (Do Not ADD) - NID(%d), ONID(%d)  \n", stNetInfo.usNetId, DbSvc_GetOnId(pstNitSvcInfo));
				}
			}
		}

	}

	if(pstSrch->stOption.ulSearchType != eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate)
	{
		// SLD, LCD에 없고 SDT에만 존재하는 Service도 Search 되어야 한다.
		// 이때, LCD처리는 Non LCD Service로 처리 한다.
		// Labwise Test : Task 8:10

		for (ulCnt = 0; ulCnt < ulSdtSvcNum; ulCnt++)
		{
			DbSvc_Info_t	*pstSdtSvcInfo = &(pstSdtSvcArray[ulCnt]);
			HBOOL			bSameSvcFound = FALSE;

			if((DbSvc_GetSvcType(pstSdtSvcInfo) == eDxSVC_TYPE_TV) || (DbSvc_GetSvcType(pstSdtSvcInfo) == eDxSVC_TYPE_RADIO))
			{
				for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
				{
					DbSvc_Info_t 	*pstNitSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstItem);

					if (DbSvc_GetTsId(pstNitSvcInfo) == DbSvc_GetTsId(pstSdtSvcInfo)
						&& DbSvc_GetOnId(pstNitSvcInfo) == DbSvc_GetOnId(pstSdtSvcInfo)
						&& DbSvc_GetSvcId(pstNitSvcInfo) == DbSvc_GetSvcId(pstSdtSvcInfo))
					{
						bSameSvcFound = TRUE;
						break;
					}
				}

				if(bSameSvcFound == FALSE)
				{
					HxLOG_Print("[SVC:--] [%3d. %s] type:%d, id=(0x%x,0x%x), idx=(%d,%d,%d)\n"
						, DbSvc_GetLcn(pstSdtSvcInfo)
						, DbSvc_GetName(pstSdtSvcInfo)
						, DbSvc_GetSvcType(pstSdtSvcInfo)
						, DbSvc_GetTsId(pstSdtSvcInfo)
						, DbSvc_GetSvcId(pstSdtSvcInfo)
						, DbSvc_GetTsIdx(pstSdtSvcInfo)
						, DbSvc_GetSvcIdx(pstSdtSvcInfo)
						, DbSvc_GetPrvIdx(pstSdtSvcInfo)
						);

					if(_xsvc_sisearch_NORDIG_IsTestTransponder(stNetInfo.usNetId, DbSvc_GetOnId(pstSdtSvcInfo)) == FALSE)
					{
						(void)_xsvc_sisearch_SetLcnInfo(pstSdtSvcInfo, pLcnInfoList);
						(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstSdtSvcInfo, NULL);
					}
					else
					{
						HxLOG_Error ("This TP Is Test Stream for Nordig (Do Not ADD) - NID(%d), ONID(%d)  \n", stNetInfo.usNetId, DbSvc_GetOnId(pstSdtSvcInfo));
					}
				}
			}
		}
	}

	_xsvc_sisearch_DeleteChannels(pChList);
	_xsvc_sisearch_DeleteLcnInfoList(pLcnInfoList);

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSdtSvcArray)				{ OxSI_Free (pstSdtSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }

	return hResult;
}

STATIC HERROR	_xsvc_sisearch_GetResults_NORDIG_NitOthSearch(svcSi_SrchData_t *pstSrch, HUINT32 ulActionId)
{
	HUINT32 				 ulCnt, ulSubCnt, ulTsNum = 0, ulNitSvcNum = 0, ulSdtSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes1stLoop;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t			*pstSdtSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstCurNitTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	HxList_t				*pstItem;
	HxList_t				*pstItem1, *pstItem2;
	HxList_t 				*pChList = NULL;
	HxList_t 				*pLcnInfoList = NULL;
	HBOOL					 bDsdFound;

	HxLOG_Print("++ NORDIG_NITSearch ++\n");

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstNitTable)
	{
		goto END_FUNC;
	}

	pstCurNitTable = pstNitTable;
	while(NULL != pstCurNitTable)
	{
		// Network Info
		HxSTD_MemSet(&stNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));
		stNetInfo.usNetId = pstCurNitTable->unInfo.stNit.usNetId;
		pucRawDes1stLoop = SILIB_TABLE_Get1stLoopDescriptor (pstCurNitTable, eSIxDESCTAG_NETWORK_NAME, 0);
		if (NULL != pucRawDes1stLoop)
		{
			SIxNetNameDes_t	*pstNameDes = (SIxNetNameDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes1stLoop);
			if (NULL != pstNameDes)
			{
				HxSTD_StrNCpy (stNetInfo.szNetName, pstNameDes->pszNetName, DxMAX_NET_NAME_LEN - 1);
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstNameDes);
			}
		}
		(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);
		HxLOG_Print("[NET] id = %d, name = %s, ulNetIdx = %d\n", stNetInfo.usNetId, stNetInfo.szNetName, ulNetIdx);

		// TS Info - NIT SLD가 SDT service_desc 보다 우선
		// And make service list
		for (pstItem1 = pstCurNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
		{
			SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

			HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
			if (NULL != pst2ndLoop)
			{
				stTsInfo.usOnId	= pst2ndLoop->unInfo.stNit.usOrgNetId;
				stTsInfo.usTsId	= pst2ndLoop->unInfo.stNit.usTsId;
			}
			else
			{
				HxLOG_Error ("No way to make the TS Info:\n");
				goto END_FUNC;
			}
			stTsInfo.usNetIdx = (HUINT16)ulNetIdx;

			// (1) Update delivery system info.
			//      - delivery -> transponder, 2015-07-18
			bDsdFound = FALSE;
			for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
			{
				HUINT8		*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

				hErr = svc_si_LibGetDeliveryTsInfoFromDsd(ulActionId, pucDesRaw, &stTsInfo);
				if(ERR_OK == hErr)
				{
					stTsInfo.usTsIdx = ulTsNum;
					(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);
					ulTsNum++;
					HxLOG_Print("[TS](%d) onid/tsid/freq/sr/cons = 0x%x/0x%x/%d/%d/%d\n"
									, ulTsIdx, stTsInfo.usOnId, stTsInfo.usTsId
									, stTsInfo.stTuningParam.cab.ulFreq, stTsInfo.stTuningParam.cab.usSR, stTsInfo.stTuningParam.cab.ucCon);
					bDsdFound = TRUE;
					break;
				}
			}

			if(FALSE == bDsdFound)
				continue;

			// (2) Get LCN Info List from NIT
			_xsvc_sisearch_GetLcnInfoList(ulActionId, &stTsInfo, pst2ndLoop->pstDesLoop, &pLcnInfoList);

			// (3) Make Service List from SLD
			hErr = _xsvc_sisearch_ComposeSldBasedServiceInfo_Nordig(ulActionId, &stTsInfo, pst2ndLoop->pstDesLoop, &pChList);
			if(ERR_OK != hErr)
			{
				HxLOG_Warning("no proper service in this ts loop\n");
			}
		}

		pstCurNitTable = pstCurNitTable->pstNextTable;
	}

	hErr = _xsvc_sisearch_ComposeSdtBasedServiceInfo_Nordig(ulActionId, &ulSdtSvcNum, &pstSdtSvcArray, &ulPrvNum, &pstPrvArray);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("can't make sdt base service info\n");
	}

	// Provider Info
	if (NULL != pstPrvArray)
	{
		for (ulCnt = 0; ulCnt < ulPrvNum; ulCnt++)
		{
			(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Provider, (void *)&(pstPrvArray[ulCnt]), NULL);
		}
	}

	ulNitSvcNum = HLIB_LIST_Length(pChList);
	HxLOG_Print(HxANSI_COLOR_YELLOW("ulNitSvcNum = %d, ulSdtSvcNum = %d\n"), ulNitSvcNum, ulSdtSvcNum);

	// Service Info
	ulCnt = 0;
	HLIB_LIST_First(pChList);
	for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next, ulCnt++)
	{
		DbSvc_Info_t 	*pstNitSvcInfo = (DbSvc_Info_t *)HLIB_LIST_Data(pstItem);
		HBOOL			bSameSvcFound = FALSE;

		// Update service name
		for (ulSubCnt = 0; ulSubCnt < ulSdtSvcNum; ulSubCnt++)
		{
			DbSvc_Info_t	*pstSdtSvcInfo = &(pstSdtSvcArray[ulSubCnt]);

			if (DbSvc_GetTsId(pstNitSvcInfo) == DbSvc_GetTsId(pstSdtSvcInfo)
				&& DbSvc_GetOnId(pstNitSvcInfo) == DbSvc_GetOnId(pstSdtSvcInfo)
				&& DbSvc_GetSvcId(pstNitSvcInfo) == DbSvc_GetSvcId(pstSdtSvcInfo))
			{
				DbSvc_SetName(pstNitSvcInfo, DbSvc_GetName(pstSdtSvcInfo));
				bSameSvcFound = TRUE;
				HxLOG_Info("[SVC](%03d) [%3d. %s] tsid:0x%x, svcid:0x%x, tsidx:%d, svcidx:%d, prvidx:%d\n", ulCnt
					, DbSvc_GetLcn(pstNitSvcInfo)
					, DbSvc_GetName(pstNitSvcInfo)
					, DbSvc_GetTsId(pstNitSvcInfo)
					, DbSvc_GetSvcId(pstNitSvcInfo)
					, DbSvc_GetTsIdx(pstNitSvcInfo)
					, DbSvc_GetSvcIdx(pstNitSvcInfo)
					, DbSvc_GetPrvIdx(pstNitSvcInfo)
					);
				break;
			}
		}
		if(TRUE != bSameSvcFound)
		{
			HxLOG_Print("[SVC](%03d) [%3d. %s] no same svc in sdt\n", ulSubCnt, DbSvc_GetLcn(pstNitSvcInfo), DbSvc_GetName(pstNitSvcInfo));
		}

		(void)_xsvc_sisearch_SetLcnInfo(pstNitSvcInfo, pLcnInfoList);
		(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Service, (void *)pstNitSvcInfo, NULL);
	}

	_xsvc_sisearch_DeleteLcnInfoList(pLcnInfoList);
	_xsvc_sisearch_DeleteChannels(pChList);

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTsArray)					{ OxSI_Free (pstTsArray); }
	if (NULL != pstSdtSvcArray)				{ OxSI_Free (pstSdtSvcArray); }
	if (NULL != pstPrvArray)				{ OxSI_Free (pstPrvArray); }
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	return hResult;
}

STATIC HERROR _xsvc_sisearch_GetResults_NORDIG_Default(svcSi_SrchData_t *pstSrch, HUINT32 ulActionId)
{
	HUINT32 				 ulCnt, ulTsNum = 0, ulSvcNum = 0, ulPrvNum = 0;
	HUINT32					 ulNetIdx = 0, ulTsIdx = 0;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_NetInfo_t			 stNetInfo;
	HUINT8					*pucRawDes;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstSdtTable = NULL, *pstPatTable = NULL;
	SIxTable_PsiTable_t		*pstSdtOthTbl = NULL;
	SIxTable_PsiTable_t		*pstTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	HxList_t				*pstItem;
    HBOOL					bTpFound = FALSE;
    HxList_t	            *pstNotAvailableTp = NULL;
    HINT32                   ulTpUid;

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	pstPatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
	pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	// Network
	HxSTD_memset(&stNetInfo, 0, sizeof(DbSvc_NetInfo_t));
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
	}
	else
	{
		// make dummy netInfo
		HUINT8		*pszNetName = NULL;
		HUINT32		 ulNameLen = 0;

		hErr = svc_si_LibGetUndefinedNetworkName(ulActionId, &pszNetName);
		if (hErr == ERR_OK)
		{
			ulNameLen = HxSTD_StrLen(pszNetName);
			if (ulNameLen > (sizeof(stNetInfo.szNetName) - 1))
			{
				ulNameLen = (sizeof(stNetInfo.szNetName) - 1);
			}
			HxSTD_StrNCpy(stNetInfo.szNetName, pszNetName, ulNameLen);
		}

		stNetInfo.szNetName[ulNameLen] = '\0';
	}

	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Network, (void *)&stNetInfo, &ulNetIdx);

	// TS Info
	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	if (NULL != pstSdtTable)
	{
		stTsInfo.usOnId	= pstSdtTable->unInfo.stSdt.usOrgNetId;
		stTsInfo.usTsId	= pstSdtTable->unInfo.stSdt.usTsId;
	}
	else if (NULL != pstPatTable)
	{
		stTsInfo.usTsId = pstPatTable->unInfo.stPat.usTsId;
	}
	else
	{
		HxLOG_Error ("No way to make the TS Info:\n");
		goto END_FUNC;
	}

	stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
	(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);

	if (NULL != pstSdtOthTbl && NULL != pstNitTable)
	{
		for (pstTable = pstSdtOthTbl; pstTable != NULL; pstTable = pstTable->pstNextTable)
		{
            bTpFound = FALSE;
			for (pstItem = pstNitTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
			{
				SIxTable_2ndLoop_t		*pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem);

				if ((pstTable->unInfo.stSdt.usOrgNetId == pst2ndLoop->unInfo.stNit.usOrgNetId)
				&&  (pstTable->unInfo.stSdt.usTsId == pst2ndLoop->unInfo.stNit.usTsId))
				{
					HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
					stTsInfo.usOnId	= pstTable->unInfo.stSdt.usOrgNetId;
					stTsInfo.usTsId	= pstTable->unInfo.stSdt.usTsId;
					stTsInfo.usNetIdx = (HUINT16)ulNetIdx;
					(void)svc_si_SrchAddSearchResultItem (pstSrch, eSI_SRCH_INFOTYPE_Transponder, (void *)&stTsInfo, &ulTsIdx);
                    bTpFound = TRUE;
				}
			}
            if(bTpFound == FALSE){
                ulTpUid = ((HUINT32)pstTable->unInfo.stSdt.usOrgNetId<<16) | ((HUINT32)pstTable->unInfo.stSdt.usTsId);
                HxLOG_Error("TP Not Found in NIT :: (0x%x)\n",ulTpUid);
                pstNotAvailableTp = HLIB_LIST_Append(pstNotAvailableTp , (void *)ulTpUid );
            }
		}
	}

	// Delivery TS Info :
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

	HxLOG_Info("pstSrch->stOption.ulSvcType: %d, pstSrch->stOption.ulCasType: %d \n", pstSrch->stOption.ulSvcType, pstSrch->stOption.ulCasType);
	// Service Info:
	hErr = svc_si_LibComposeServiceInfoInTp (ulActionId, &ulSvcNum, &pstSvcArray, &ulPrvNum, &pstPrvArray);
	if (ERR_OK == hErr)
	{
		if (NULL != pstSvcArray)
		{
			DbSvc_Info_t		*pstSvc = NULL;

			for (ulCnt = 0; ulCnt < ulSvcNum; ulCnt++)
			{
				pstSvc = &pstSvcArray[ulCnt];

				ulTpUid = ((HUINT32)DbSvc_GetOnId(pstSvc)<<16) | ((HUINT32)DbSvc_GetTsId(pstSvc));

                if(HLIB_LIST_Find(pstNotAvailableTp,ulTpUid) != NULL){
                    HxLOG_Error("InVaild Service:: NIT doesn't have this information for service TP (0x%x)\n",ulTpUid);
                    continue;
                }

				if (xsvc_si_LibSrchCompareSvcType(pstSrch->stOption.ulSvcType, DbSvc_GetSvcType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 svc type과 다르다
					HxLOG_Info("do not add svc type: %d \n", DbSvc_GetSvcType(pstSvc));
					continue;
				}

				if (xsvc_si_LibSrchCompareCasType(pstSrch->stOption.ulCasType, DbSvc_GetCasType(pstSvc)) != ERR_OK)
				{
					// search option에 설정된 cas type과 다르다
					HxLOG_Info("do not add cas type: %d \n", DbSvc_GetCasType(pstSvc));
					continue;
				}

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
	if (NULL != pstSdtOthTbl)				{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTbl); }
    if (NULL != pstNotAvailableTp)          { HLIB_LIST_RemoveAll(pstNotAvailableTp);}
	return hResult;
}

#define _____SI_MSG_PROCESS_FUNCTION_____

STATIC void xsvc_si_SrchMsgCmdStartSearch_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstMsg->hAction));

	(void)svc_si_SrchInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartSrch.unSpec), &(pstMsg->unArg.stStartSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_NORDIG);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	HxLOG_Print(HxANSI_COLOR_YELLOW("pstSrch->stOption.ulSearchType = 0x%x\n"), pstSrch->stOption.ulSearchType);
	HxLOG_Print(HxANSI_COLOR_YELLOW("pstSrch->eNitOption            = %d\n"), pstSrch->eNitOption);
	HxLOG_Print(HxANSI_COLOR_YELLOW("pstSrch->usNitNetId            = %d\n"), pstSrch->usNitNetId);

	_xsvc_sisearch_GetFilteringType_Nordig(pstSrch);
	HxLOG_Print("0x%08x\n", pstSrch->eFiltering);
	(void)svc_si_SrchStartSection (pstSrch);
}

STATIC void xsvc_si_SrchMsgCmdStopSearch_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtOthTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_Nordig (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}


STATIC void xsvc_si_SrchProcessMsg_Nordig (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

#ifdef CONFIG_DEBUG
	HxLOG_Print(HxANSI_COLOR_YELLOW("%s\n"), INT_UTIL_GETSTR_SvcSi_MsgType(pstMsg->eMsgClass));
#endif

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		xsvc_si_SrchMsgCmdStartSearch_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		xsvc_si_SrchMsgCmdStopSearch_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SrchMsgEvtPatTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SrchMsgEvtPmtTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_SrchMsgEvtSdtTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		xsvc_si_SrchMsgEvtSdtOthTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_SrchMsgEvtNitTable_Nordig (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		xsvc_si_SrchMsgEvtNitOthTable_Nordig (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_Nordig (void)
{
	return xsvc_si_SrchProcessMsg_Nordig;
}

void *xsvc_si_SrchAllocSearchActionData_Nordig (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_Nordig (void *pvData)
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

HERROR xsvc_si_SrchProcessForSimualcastService_Nordig(void)
{
	Handle_t			*phSvcList = NULL;
	DbSvc_Info_t		*pstSvcArray = NULL;
	HERROR				hErr;
	HUINT32 			i, j, ulSvcNum = 0;
	DbSvc_Condition_t	stSvcCond;

	// 모든 Service List 를 가져온다.
	DB_SVC_InitSvcFindCondition(&stSvcCond);

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstSvcArray);
	hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phSvcList);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	HxLOG_Info("ulSvcNum : %d \n", ulSvcNum);

	for (i = 0; i < ulSvcNum; i++)
	{
		// Simulcast Service가 없으면 Skip
		if (DbSvc_GetSimulcastOnId(&pstSvcArray[i]) == 0
			&& DbSvc_GetSimulcastTsId(&pstSvcArray[i]) == 0
			&& DbSvc_GetSimulcastSvcId(&pstSvcArray[i]) == 0)
		{
			continue;
		}

		for (j = 0; j < ulSvcNum; j++)
		{
			// 같은 Service는 Skip
			if (i == j)
			{
				continue;
			}

			// 다른 Service Type이면 Skip
			if (DbSvc_GetSvcType(&pstSvcArray[i]) != DbSvc_GetSvcType(&pstSvcArray[j]))
			{
				continue;
			}

			// Simulcast Service가 아니면 Skip
			if (DbSvc_GetSimulcastOnId(&pstSvcArray[i]) != DbSvc_GetOnId(&pstSvcArray[j])
				|| DbSvc_GetSimulcastTsId(&pstSvcArray[i]) != DbSvc_GetTsId(&pstSvcArray[j])
				|| DbSvc_GetSimulcastSvcId(&pstSvcArray[i]) != DbSvc_GetSvcId(&pstSvcArray[j]))
			{
				continue;
			}

			// Simulcast의 Target Service가 존재하면 Simulcast Linkage 를 갖고 있는 Source Service를 제거한다.
			//hErr = DB_SVC_DeleteServiceInfo(phSvcList[i]);
			//DbSvc_SetVisibleFlag(&pstSvcArray[i], FALSE);
			DbSvc_SetSvcType(&pstSvcArray[i], eDxSVC_TYPE_HIDDEN);
			DB_SVC_UpdateServiceInfo(phSvcList[i], &pstSvcArray[i]);
			break;
		}
	}

	DB_SVC_FreeServiceInfoList(NULL, phSvcList, pstSvcArray);

	return ERR_OK;
}

HERROR xsvc_si_SrchGetSearchedResultFromSi_Nordig (void *pvData)
{
	svcSi_SrchData_t 		*pstSrch = NULL;
	HUINT32					 ulActionId = 0;

	if (pvData == NULL)
	{
		HxLOG_Error("null parameter error ! \n");
		return ERR_FAIL;
	}
	pstSrch = (svcSi_SrchData_t *)pvData;
	ulActionId = PAL_PIPE_GetActionId (pstSrch->hAction);

	switch(pstSrch->stOption.ulSearchType)
	{
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased:
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate:
		return _xsvc_sisearch_GetResults_NORDIG_NitActSearch(pstSrch, ulActionId);

	case eSVCSI_SEARCHTYPE_NORDIG_NitOthBased:
		return _xsvc_sisearch_GetResults_NORDIG_NitOthSearch(pstSrch, ulActionId);

	case eSVCSI_SEARCHTYPE_NORDIG_WithoutNit:
		// TODO: 새로 작성
		return _xsvc_sisearch_GetResults_NORDIG_Default(pstSrch, ulActionId);

	default:
		return _xsvc_sisearch_GetResults_NORDIG_Default(pstSrch, ulActionId);
	}
}

HERROR xsvc_si_SrchSvcEsComposeServiceInfoInTp_Nordig (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT32 				 ulSvcNum = 0, ulPrvNum = 0;
	HUINT32 				 ulActSvcNum = 0, ulActPrvNum = 0;
	HUINT32 				 ulOthSvcNum = 0, ulOthPrvNum = 0;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_Info_t			*pstActSvcArray = NULL;
	DbSvc_Info_t			*pstOthSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstActPrvArray = NULL;
	DbSvc_PrvInfo_t			*pstOthPrvArray = NULL;

	SIxTable_PsiTable_t		*pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	HERROR					 hErr1 = ERR_OK, hErr2 = ERR_OK;
	HBOOL					bMemAlloc = TRUE;

	if (NULL != pstSdtTable)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT ACT\n");
		// default search 시에는 base로.
		hErr1 = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtTable, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
		ulSvcNum = ulActSvcNum;
		ulPrvNum = ulActPrvNum;
	}
	else if (NULL != pstPmtTable)
	{
		HxLOG_Info ("PMT Found: Composite the Services by PMT \n");
		hErr1 = xsvc_si_LibSvcEsComposeServiceInfoByPmt_Base (ulActionId, pstPmtTable, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
		ulSvcNum = ulActSvcNum;
		ulPrvNum = ulActPrvNum;
	}

	HxLOG_Info("hErr1: %d \n", hErr1);

	if (NULL != pstSdtOthTbl)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT OTH\n");
		// default search 시에는 base로.
		hErr2 = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtOthTbl, &ulOthSvcNum, &pstOthSvcArray, &ulOthPrvNum, &pstOthPrvArray);
		ulSvcNum += ulOthSvcNum;
		ulPrvNum += ulOthPrvNum;
	}

	HxLOG_Info("hErr2: %d \n", hErr2);

	if (ulSvcNum > 0)
	{
		HxLOG_Info("ulSvcNum: %d \n", ulSvcNum);

		pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
		if (NULL == pstSvcArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstSvcArray, pstActSvcArray, sizeof(DbSvc_Info_t) * ulActSvcNum);
		HxSTD_memcpy(&pstSvcArray[ulActSvcNum], pstOthSvcArray, sizeof(DbSvc_Info_t) * ulOthSvcNum);

		*ppstSvcArray = pstSvcArray;
		pstSvcArray = NULL;
		*pulSvcNum = ulSvcNum;
	}

	if (ulPrvNum > 0)
	{

		HxLOG_Info("ulPrvNum: %d \n", ulPrvNum);

		pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (NULL == pstPrvArray)
		{
			bMemAlloc = FALSE;
			HxLOG_Error ("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_memcpy(pstPrvArray, pstActPrvArray, sizeof(DbSvc_PrvInfo_t) * ulActPrvNum);
		HxSTD_memcpy(&pstPrvArray[ulActPrvNum], pstOthPrvArray, sizeof(DbSvc_PrvInfo_t) * ulOthPrvNum);

		*ppstPrvArray = pstPrvArray;
		pstPrvArray = NULL;
		*pulPrvNum = ulPrvNum;

	}

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstActSvcArray)					{ OxSI_Free (pstActSvcArray); }
	if (NULL != pstOthSvcArray)					{ OxSI_Free (pstOthSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }
	if (NULL != pstActPrvArray)					{ OxSI_Free (pstActPrvArray); }
	if (NULL != pstOthPrvArray)					{ OxSI_Free (pstOthPrvArray); }
	if (NULL != pstSdtTable)					{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }
	if (NULL != pstSdtOthTbl)					{ PAL_SIRAWPOOL_FreeTable (pstSdtOthTbl); }

	return (((hErr1 == ERR_OK || hErr2 == ERR_OK) && bMemAlloc == TRUE) ? ERR_OK : ERR_FAIL);
}

/* end of file */

