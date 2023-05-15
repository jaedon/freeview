/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_eudtt.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /OTACHECK	\n

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
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <db_svc.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_swup.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>

#include <_xsvc_si_lib_swup_base.h>
#include <_xsvc_si_swup_base.h>
#include <_xsvc_si_swup.h>
#include <db_param.h>

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xsvc_si_SwupCheckMakeStrMode_EuDtt(XsvcSi_SwupMode_e eSwupMode)
{
	switch (eSwupMode)
	{
		ENUM_TO_STR(eSI_SWUP_MODE_EUDTT_HOME_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_EUDTT_SSUSCAN_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_EUDTT_SSUSVC_TP);
		default:
			break;
	}
	return ("Unknown");

}
#endif
#if 0

STATIC HERROR xsvc_si_SwupCheckInitSwupData_EuDtt (xsvcSi_SwupData_t *pstSwupCheck)
{
	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] ulSwupMode(%s) usBatBouquetId(0x%x), %d \n"
		,__FUNCTION__,__LINE__, xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode), pstSwupCheck->usBatBouquetId,pstSwupCheck->stBase.stSwupOption.ulSwupMode);
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_EuDtt_SSUSVC_TP:
		pstSwupCheck->usPmtSvcId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;

	default:
		break;
	}

	return ERR_OK;
}



STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType		= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType		= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_NitOth:
		*peFilterType		= bSI_FILTERING_NIT_OTH;
		*pulExtId 		= pstSwupCheck->usNetIdForNitOth;
		*pulPid			= NIT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStartSection_EuDtt(xsvcSi_SwupData_t *pstSwupCheck)
{
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_EuDtt(pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSwupCheck->stBase.eFiltering & eFilterType))
		{
			hErr = svc_si_SwupCheckStartFilter (&pstSwupCheck->stBase, eTableType, ulExtId, ulPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SrchStartFilter err: eTableType(%d)\n", eTableType);
			}
		}
	}

	if (NULL == pstSwupCheck->stBase.pstFilterings)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStopSection_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR			hErr = ERR_FAIL;

	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckStopSection(&pstSwupCheck->stBase, eSiTableType, ulExtId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckStopSection() failed..! eSiTableType(%d) extId(0x%X) \n", eSiTableType, ulExtId);
	}

	return hErr;
}

STATIC void xsvc_si_SwupCheckFreePsiInfo_EuDtt (void *pvData)
{
	xsvcSi_SwupPsiInfo_t	*pstSwupInfo = (xsvcSi_SwupPsiInfo_t *)pvData;

	if (NULL != pstSwupInfo)
	{
		OxSI_Free(pstSwupInfo);
	}

	return;
}

STATIC void	 xsvc_si_SwupCheckFreeListItem_EuDtt (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}
#define _____SWUP_PARSING_____

STATIC HERROR xsvc_si_SwupCheckFindProperOuiData_EuDtt (HUINT32 ulSelectorNum, svcSi_SwupCheck_SsuInfo_t *pstSelectors, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
{
	HUINT32			 ulIndex;

	if (0 == ulSelectorNum || pstSelectors == NULL|| pstSwupInfo == NULL)
	{
		HxLOG_Error ("invalid parameter \n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < ulSelectorNum; ulIndex++)
	{
		if (pstSelectors[ulIndex].ulOui == eSWUP_OUI_HUMAX)
		{
			HxSTD_memset(pstSwupInfo, 0, sizeof(xsvcSi_SwupPsiInfo_t));

			if (pstSelectors[ulIndex].ucUpdateType == 0x02 || pstSelectors[ulIndex].ucUpdateType == 0x03)
			{
				pstSwupInfo->bUseUnt = TRUE;

				if (pstSelectors[ulIndex].bUpdateVerFlag != FALSE)
				{
					pstSwupInfo->ucUpdateVer = pstSelectors[ulIndex].ucUpdateVer;
				}
			}

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_EuDtt (SIxDataBroadcastIdDes_t *pstDbcIdDes, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
{
	HUINT32			 		ulSelectorNum =0 ;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;
	HERROR					 hErr;

	hErr = svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes (pstDbcIdDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes Error \n");
		return ERR_FAIL;
	}

	hErr = xsvc_si_SwupCheckFindProperOuiData_EuDtt (ulSelectorNum, pstOuiSelectors, pstSwupInfo);

	svc_si_SwupCheckFreeSsuInfo(pstOuiSelectors);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckParseCompability_EuDtt (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound, HUINT32 *pulSystemVer)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSignalSystemId, ulSignalSystemVer;
	HUINT32		 ulOui, ulSystemId, ulSystemVer;
	HUINT8		*pucNextDesRaw;
	HERROR		 hErr;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
		hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSystemVer, sizeof(HUINT32));
	}
	else
	{
		hErr  = PAL_SYS_GetSystemId (&ulSystemId);
		hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSystemVer);
	}
	if (hErr != ERR_OK)				{ return ERR_FAIL; }

	ucDesTag = pucDesRaw[0];
	ucDesLen = pucDesRaw[1];
	pucDesRaw += 2;
	pucNextDesRaw = pucDesRaw + ucDesLen;

	switch (ucDesTag)
	{
	case 0x01:				// System Hardware Descriptor
		ulOui = HxMACRO_Get32Bit(pucDesRaw) & 0xffffff;					pucDesRaw += 4;
		ulSignalSystemId = HxMACRO_Get32Bit (pucDesRaw);					pucDesRaw += 4;

		HxLOG_Print("[%s][%d] ________________________ ulOui(0x%x) ulSignalSystemId(0x%x) ulSystemId(0x%x) \n", __FUNCTION__, __LINE__, ulOui, ulSignalSystemId, ulSystemId);
		if ( ((ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX)) && (ulSignalSystemId == ulSystemId) )
		{
			if (ppucNextDesRaw != NULL)				{ *ppucNextDesRaw = pucNextDesRaw; }
			*pbHwFound = TRUE;
			return ERR_OK;
		}
		break;

	case 0x02:				// System Software Descriptor
		ulOui = HxMACRO_Get32Bit (pucDesRaw) & 0xffffff;					pucDesRaw += 4;
		ulSignalSystemVer = HxMACRO_Get32Bit (pucDesRaw);					pucDesRaw += 4;
		HxLOG_Print("[%s][%d] ________________________ ulOui(0x%x) ulSignalSystemVer(0x%x) ulSystemVer0x%x) \n", __FUNCTION__, __LINE__, ulOui, ulSignalSystemVer, ulSystemVer);

		if (((ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX)) && (ulSignalSystemVer > ulSystemVer))
		{
			if (ppucNextDesRaw != NULL)				{ *ppucNextDesRaw = pucNextDesRaw; }
			*pbSwFound = TRUE;
			*pulSystemVer = ulSignalSystemVer;
			return ERR_OK;
		}

		break;
	}

	if (ppucNextDesRaw != NULL) 			{ *ppucNextDesRaw = pucNextDesRaw; }
	return ERR_OK;

}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_EuDtt(xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, xsvcSi_SwupPsiInfo_t **pastSwupSignals)
{
	HUINT32						 ulActionId;
	HUINT32						 ulIndex, ulCntSsuSvc, ulNumSsuSvc;
	HERROR						 hErr = ERR_FAIL;
	SIxTable_PsiTable_t			*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pstPmt2ndLoop = NULL, *pst2ndDesLoop = NULL;
	SIxDataBroadcastIdDes_t		*pstDbcIdDes = NULL;
	xsvcSi_SwupPsiInfo_t		stDvbssuPsiInfo;
	xsvcSi_SwupPsiInfo_t		*pstDvbssuPsiInfo = NULL, *pstDvbssuPsiInfoArray = NULL;
	HxList_t					*pstList, *pstDvbssuPsiInfoList=NULL;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error("invalid parametes..! \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("+++ \n");

	// PMT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;

		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t		*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			hErr = svc_si_SwupCheckIsSsuStreamType(pstPmtElem->ucStreamType);
			if(hErr !=ERR_OK)			{ continue; }

			for (pst2ndDesLoop = pst2ndItem->pstDesLoop; pst2ndDesLoop != NULL; pst2ndDesLoop = pst2ndDesLoop->next)
			{
				HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

				if (pucRawDes == NULL)
				{
					HxLOG_Print("pucRawDes == NULL, continue \n");
					continue;
				}

				if (*pucRawDes == eSIxDESCTAG_DATA_BROADCAST_ID)
				{
					pstDbcIdDes = (SIxDataBroadcastIdDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if(pstDbcIdDes != NULL)
					{
						hErr = xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_EuDtt(pstDbcIdDes,&stDvbssuPsiInfo);
						if(hErr == ERR_OK)
						{
							pstDvbssuPsiInfo = (xsvcSi_SwupPsiInfo_t *)OxSI_Malloc (sizeof(xsvcSi_SwupPsiInfo_t));
							if (pstDvbssuPsiInfo != NULL)
							{
								HxSTD_memset (pstDvbssuPsiInfo, 0, sizeof(xsvcSi_SwupPsiInfo_t));
								pstDvbssuPsiInfo->usSvcId		= pstPmtSec->usProgNum;
								pstDvbssuPsiInfo->usPid			= pstPmtElem->usElemPid;
								pstDvbssuPsiInfo->bUseUnt 		= stDvbssuPsiInfo.bUseUnt;
								pstDvbssuPsiInfo->ucUpdateVer 	= stDvbssuPsiInfo.ucUpdateVer;

								pstDvbssuPsiInfoList = HLIB_LIST_Append (pstDvbssuPsiInfoList, (void *)pstDvbssuPsiInfo);
							}
						}
					}
				}
			}

		}
	}

	ulNumSsuSvc = HLIB_LIST_Length(pstDvbssuPsiInfoList);
	if (ulNumSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("ulNumLinkage is Zero \n");
		goto Error;
	}

	pstDvbssuPsiInfoArray = (xsvcSi_SwupPsiInfo_t *)OxSI_Malloc (sizeof(xsvcSi_SwupPsiInfo_t) * ulNumSsuSvc);
	if (pstDvbssuPsiInfoArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntSsuSvc = 0;
	for (ulIndex = 0; ulIndex < ulNumSsuSvc; ulIndex++)
	{
		pstList = HLIB_LIST_GetListItem(pstDvbssuPsiInfoList, ulIndex);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSsuSvc, ulIndex);
			continue;
		}

		pstDvbssuPsiInfo = (xsvcSi_SwupPsiInfo_t *)HLIB_LIST_Data(pstList);
		if (pstDvbssuPsiInfo == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", ulIndex);
			continue;
		}

		HxSTD_MemCopy(&pstDvbssuPsiInfoArray[ulCntSsuSvc], pstDvbssuPsiInfo, sizeof(xsvcSi_SwupPsiInfo_t));
		ulCntSsuSvc++;
	}

	if (ulCntSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumSsu != NULL)
	{
		*pulNumSsu = ulCntSsuSvc;
	}

	if (pastSwupSignals != NULL)
	{
		*pastSwupSignals = pstDvbssuPsiInfoArray;
		pstDvbssuPsiInfoArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (pstDvbssuPsiInfoArray != NULL)				{ OxSI_Free (pstDvbssuPsiInfoArray); }
	if (pstPmtTable != NULL)						{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if(pstDvbssuPsiInfoList != NULL)					{ HLIB_LIST_RemoveAllFunc (pstDvbssuPsiInfoList, xsvc_si_SwupCheckFreePsiInfo_EuDtt);}

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckGetDsmccPidFromPmt_EuDtt(xsvcSi_SwupData_t *pstSwupCheck, HUINT16 usAssociationTag, HUINT16 *pusPid)
{
	HUINT32						 ulActionId;
	HERROR						 hErr = ERR_FAIL;
	SIxTable_PsiTable_t			*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pstPmt2ndLoop = NULL, *pstPmtDesLoop = NULL;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error("invalid parametes..! \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("+++ \n");

	// PMT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t		*pstPmtElem;

			if (NULL == pst2ndItem) 				{ continue; }

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			hErr = svc_si_SwupCheckIsSsuStreamType(pstPmtElem->ucStreamType);
			if(hErr == ERR_OK)
			{
				for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
				{
					HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
					SIxStmIdentDes_t		*pstStmIdentDes = NULL;

					if (eSIxDESCTAG_STREAM_IDENTIFIER == *pucRawDes)
					{
						if(pstStmIdentDes->ucCompTag == (HUINT8)usAssociationTag)
							// DSM-CC Pid Found!!
							*pusPid = pstPmtElem->usElemPid;

							if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }

							return ERR_OK;
					}
				}
			}
		}
	}

	if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckSetDesInfoFromUnt_EuDtt(void  **ppvSsuDescriptorInfo, HUINT32 ulTag ,  void **ppvSsuDescriptorRawInfo)
{
	// TODO: 구현 필요..
#if 0
	HUINT16 		ulStrLen;
	HxDATETIME_t	stDateTime;

	MsgDes				*pstMessageDes;
	SchedulingDes		*pstScheduleDes;
	UpdateDes			*pstUpdateDes;
	SvcSi_SsuUpdateInfo_t 	*pstSsuUpdateInfo;
	SvcSi_SsuScheduleInfo_t	*pstSsuScheduleInfo;
	SvcSi_SsuMessageInfo_t 	*pstSsuMessageInfo;


	switch (ulTag)
	{
	case TAG_MESSAGE:
		{
			pstMessageDes = (MsgDes*)*ppvSsuDescriptorRawInfo;
			pstSsuMessageInfo = (SvcSi_SsuMessageInfo_t*)*ppvSsuDescriptorInfo;

			ulStrLen = 0;
			HxSTD_memcpy(pstSsuMessageInfo->ucLangCode, pstMessageDes->szIso639LangCode, 4);

			ulStrLen = MWC_UTIL_DvbStrlen(pstMessageDes->pszText);
			pstSsuMessageInfo->usTextLen = ulStrLen;

			pstSsuMessageInfo->pucText = (HUINT8 *)OxSI_Malloc(sizeof(HUINT8) * (ulStrLen +1));
			if(pstSsuMessageInfo->pucText != NULL)
			{
				HxSTD_memset(pstSsuMessageInfo->pucText, 0x00, sizeof(HUINT8) * ulStrLen +1);
				pstSsuMessageInfo->pucText[ulStrLen] = '\0';
				HLIB_STD_StrUtf8NCpy(pstSsuMessageInfo->pucText, pstMessageDes->pszText, ulStrLen);
			}
		}
		break;
	case TAG_SCHEDULING :
		{
			pstScheduleDes = (SchedulingDes*)*ppvSsuDescriptorRawInfo;
			pstSsuScheduleInfo = (SvcSi_SsuScheduleInfo_t*)*ppvSsuDescriptorInfo;

			memset(&stDateTime, 0, sizeof(HxDATETIME_t));
			HLIB_DATETIME_ConvertUnixTimeToDateTime(pstScheduleDes->startDateTime, &stDateTime);
			pstSsuScheduleInfo->startDateTime		= stDateTime;

			memset(&stDateTime, 0, sizeof(HxDATETIME_t));
			HLIB_DATETIME_ConvertUnixTimeToDateTime(pstScheduleDes->endDateTime, &stDateTime);
			pstSsuScheduleInfo->endDateTime 	= stDateTime;

			pstSsuScheduleInfo->ucIsFinalAvailable		= pstScheduleDes->ucIsFinalAvailable;
			pstSsuScheduleInfo->ucIsPeriodic			= pstScheduleDes->ucIsPeriodic;
			pstSsuScheduleInfo->ucPeriodUnit			= pstScheduleDes->ucPeriodUnit;
			pstSsuScheduleInfo->ucDurationUnit			= pstScheduleDes->ucDurationUnit;
			pstSsuScheduleInfo->ucEstimatedCycleUnit	= pstScheduleDes->ucEstimatedCycleUnit;
			pstSsuScheduleInfo->ucPeriod				= pstScheduleDes->ucPeriod;
			pstSsuScheduleInfo->ucDuration				= pstScheduleDes->ucDuration;
			pstSsuScheduleInfo->ucEstimatedCycle		= pstScheduleDes->ucEstimatedCycle;
		}
		break;
	case TAG_UPDATE:
		{
			pstUpdateDes = (UpdateDes*)*ppvSsuDescriptorRawInfo;
			pstSsuUpdateInfo = (SvcSi_SsuUpdateInfo_t*)*ppvSsuDescriptorInfo;

			pstSsuUpdateInfo->ucIsAuto		= pstUpdateDes->ucUpdateFlag;
			pstSsuUpdateInfo->ucMethod		= pstUpdateDes->ucUpdateMethod;
			pstSsuUpdateInfo->ucPriority	= pstUpdateDes->ucUpdatePriority;
		}
		break;
	default:
		return ERR_SI_PARAM_INVALID;
	}
#endif
	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromUnt_EuDtt(xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals)
{
#if 0
	HUINT16			 usPid;
	HBOOL			 bHwFound, bSwFound;
	HUINT32			 ulSignalVersion;
	HUINT32			 ulSystemId, ulSwupVersion;
	HUINT32			 ulIdxSsu, ulNumSsu;
	HUINT8			*pucDesRaw;
	SvcSi_SwupSignal_t	*pstSwupInfo;
	SvcSi_SwupSignal_t	*astSwupArray;
	POINTER_LIST_T	*pstSwupList;


	HERROR			 hErr;
	HBOOL			bOpFound = FALSE, bCommFound = FALSE;
	HUINT16			usAssociationTag;
	HUINT32			ulSecIdx, ulSecNum;
	HUINT32			ulCmpDesInfoNum, ulCmpDesInfoIndex;
	HUINT32			ulCmpDesNum, ulCmpDesIndex;
	HUINT32			ulDesNum, ulDesIndex;
	UntSectionInfo_t	stUntInfo;
	SvcSi_SsuDescriptorInfo_t *ppstSsuDescriptorInfo;

	usPid = 0;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
		hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSystemVer, sizeof(HUINT32));
	}
	else
	{
		hErr  = PAL_SYS_GetSystemId (&ulSystemId);
		hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSwupVersion);
	}
	if (hErr != ERR_OK)				{ return ERR_SI_RESULT_FAIL; }

	pstSwupList = NULL;

	ENTER_CRITICAL_SIRAW;

	hErr = SI_CountUntSection (pstSwupData->usTsUniqId, &ulSecNum);
	if (hErr != ERR_OK) 			{ ulSecNum = 0; }

	for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx++)
	{
		ulCmpDesInfoNum = 0;

		hErr = SI_GetUntSectionInfo (pstSwupData->usTsUniqId, ulSecIdx, &stUntInfo);
		if (hErr != ERR_OK) 			{ return ERR_SI_RESULT_FAIL; }

		if(stUntInfo.ulOui != HUMAX_OUI)	{	continue;	}

		// Compatibility descriptor Info count
		hErr = SI_CountUntCompatibilityDescriptorInfo (pstSwupData->usTsUniqId, ulSecIdx, &ulCmpDesInfoNum);
		if (hErr != ERR_OK) 			{ return ERR_SI_RESULT_FAIL; }

		for(ulCmpDesInfoIndex = 0; ulCmpDesInfoIndex < ulCmpDesInfoNum ; ulCmpDesInfoIndex++)
		{
			pucDesRaw = NULL;

			bHwFound = FALSE;
			bSwFound = FALSE;
			ulCmpDesNum = 0;

			// Compatibility descriptor Info count
			hErr = SI_CountUntCompatibilityDescriptor(pstSwupData->usTsUniqId, ulSecIdx, ulCmpDesInfoIndex, &ulCmpDesNum);
			if (hErr != ERR_OK) 			{ return ERR_SI_RESULT_FAIL; }

			for(ulCmpDesIndex = 0; ulCmpDesIndex < ulCmpDesNum; ulCmpDesIndex++)
			{
				hErr = SI_GetUntCompatibilityDescriptor(pstSwupData->usTsUniqId, ulSecIdx, ulCmpDesInfoIndex, ulCmpDesIndex, &pucDesRaw);
				if (hErr == ERR_OK && pucDesRaw != NULL)
				{
					local_dvbssuota_ParseCompability (pucDesRaw, NULL, &bHwFound, &bSwFound, &ulSignalVersion);
					SI_FreeDsiGroupComp (pucDesRaw);
				}

				HxLOG_Print("[local_dvbssuota_FindSsuInfoFromUnt] bHwFound(%d) bSwFound(%d) \n", bHwFound, bSwFound);

				if(bHwFound == TRUE && bSwFound== TRUE)
				{
					ulDesNum = 0;

					hErr = SI_CountUntOperationalDescriptor(pstSwupData->usTsUniqId, ulSecIdx, ulCmpDesInfoIndex, TAG_SSU_LOCATION, &ulDesNum);
					if (hErr != ERR_OK) 			{	continue;  }
					if(ulDesNum!=0)					{ 	bOpFound = TRUE;	}

					if(bOpFound == FALSE)
					{
						// Common descriptor
						hErr = SI_CountUntCommonDescriptor (pstSwupData->usTsUniqId, ulSecIdx, TAG_SSU_LOCATION, &ulDesNum);
						if (hErr != ERR_OK) 		{	 continue; 	}
						if(ulDesNum != 0)			{	bCommFound = TRUE;	}
					}

					HxLOG_Print("[local_dvbssuota_FindSsuInfoFromUnt] bOpFound(%d) bCommFound(%d) \n", bOpFound, bCommFound);

					for(ulDesIndex = 0; ulDesIndex< ulDesNum; ulDesIndex++)
					{
						if(bOpFound == TRUE)
						{
							hErr = SI_GetUntAssociationTagFromOperationalDescriptor(pstSwupData->usTsUniqId, ulSecIdx, ulCmpDesInfoIndex, ulDesIndex, TAG_SSU_LOCATION, &usAssociationTag);
						}
						else if(bOpFound == FALSE && bCommFound == TRUE)
						{
							hErr = SI_GetUntAssociationTagFromCommonDescriptor(pstSwupData->usTsUniqId, ulSecIdx, ulDesIndex, TAG_SSU_LOCATION, &usAssociationTag);
						}

						if(hErr == ERR_OK)
						{
							HxLOG_Print("[local_dvbssuota_FindSsuInfoFromUnt] usAssociationTag(0x%x) \n", usAssociationTag);

							// PMT에서 usAssociationTag 로 usPid 값 구하기..
							hErr =local_dvbssuota_GetDsmccPidFromPmt(pstSwupData, usAssociationTag, &usPid);
							if(hErr ==ERR_OK)
							{
								// SWUP info 만들기..
								pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
								if (pstSwupInfo != NULL)
								{
									HxSTD_memset (pstSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

									pstSwupInfo->ulStartSystemId = ulSystemId;
									pstSwupInfo->ulEndSystemId = ulSystemId;
									pstSwupInfo->usPid = usPid;
									pstSwupInfo->usDataType = eSWUP_DATATYPE_APPL;		// DVB-SSU Simple 로는 SWUP Type을 알 수 없다.
									SVC_SI_OTA_ConvertOtaSiDataTypeToVersionType ((SvcSi_SwupImageType_e)pstSwupInfo->usDataType, &(pstSwupInfo->eDataType));
									pstSwupInfo->ulDataVersion = ulSignalVersion;

									HxLOG_Print("[local_dvbssuota_FindSsuInfoFromUnt] ulSystemId(0x%x) usPid(0x%x) ulSignalVersion(0x%x)\n", ulSystemId, usPid, ulSignalVersion);

									pstSwupList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSwupList, (void *)pstSwupInfo);
								}
							}
						}
					}
				}
			}
		}
	}

	LEAVE_CRITICAL_SIRAW;

	if (pstSwupList == NULL)
	{
		return ERR_SI_RESULT_FAIL;
	}

	if (pulNumSsu != NULL)
	{
		*pulNumSsu = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSwupList, NULL, NULL);
	}

	if (pastSwupSignals != NULL)
	{
		ulIdxSsu = 0;
		ulNumSsu = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSwupList, NULL, NULL);
		astSwupArray = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t) * ulNumSsu);
		if (astSwupArray == NULL)
		{
			pstSwupList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSwupList, local_dvbssuota_FreeListItem);
			return ERR_SI_MEM_ALLOC_FAIL;
		}

		while (pstSwupList)
		{
			pstSwupInfo = (SvcSi_SwupSignal_t *)pstSwupList->pvContents;

			if (pstSwupInfo != NULL)
			{
				astSwupArray[ulIdxSsu] = *pstSwupInfo;
				OxSI_Free (pstSwupInfo);
				ulIdxSsu ++;
			}

			pstSwupList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSwupList, pstSwupList, NULL);
		}

		*pastSwupSignals = astSwupArray;

		// descriptor parsing
		hErr = local_dvbssuota_ParseDesInfoFromUnt(pstSwupData, (SvcSi_SsuDescriptorInfo_t **) &ppstSsuDescriptorInfo);
		if (hErr == ERR_OK)
		{
			SVC_SI_BusPostMessage (eSEVT_SWUP_DETECT_PRIVATE_DATA_FOUND, pstSwupData->hAction, 0, 0, 0);
		}
	}

	pstSwupList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSwupList, local_dvbssuota_FreeListItem);
	return ERR_SI_RESULT_OK;
#endif
	return ERR_OK;

}

STATIC HERROR xsvc_si_SwupCheckFindSwupInfoFromDsi_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals, HUINT16 *pusPid)
{
	HBOOL				 bHwFound, bSwFound;
	HUINT32				 ulSignalVersion;
	HUINT32				 ulSystemId, ulSwupVersion;
	HUINT32				 ulNumSsu;
	SvcSi_SwupSignal_t	*pstSwupInfo;
	SvcSi_SwupSignal_t	*astSwupArray;
	HxList_t			*pstSwupList;

	HERROR				 hErr;
	HUINT32				 ulActionId;
	SIxTable_PsiTable_t	*pstDsiTable = NULL, *pstCurrDsi = NULL;
	HxList_t			*pstDsi2ndLoop = NULL;
	HUINT32				 ulListLen, ulIndex;
	HUINT8				*pucRaw;
	HxList_t			*pHxList = NULL;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
		hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSwupVersion, sizeof(HUINT32));
	}
	else
	{
		hErr  = PAL_SYS_GetSystemId (&ulSystemId);
		hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSwupVersion);
	}
	if (hErr != ERR_OK)				{ return ERR_FAIL; }

	pstSwupList = NULL;

	// DSI parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstDsiTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstDsiTable)
	{
		HxLOG_Error ("pstDsiTable is NULL (ulActionId : %d) \n", ulActionId);
		return ERR_FAIL;
	}

	for (pstCurrDsi = pstDsiTable; pstCurrDsi != NULL; pstCurrDsi = pstCurrDsi->pstNextTable)
	{
		SIxTable_DsiSec_t		*pstDsiSec = &pstCurrDsi->unInfo.stDsi;

		for (pstDsi2ndLoop = pstCurrDsi->pst2ndLoop; NULL != pstDsi2ndLoop; pstDsi2ndLoop = pstDsi2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstDsi2ndLoop);
			SIxTable_DsiGroup_t		*pstDsiGroup;

			if (NULL == pst2ndItem) 				{ continue; }

			pstDsiGroup = (SIxTable_DsiGroup_t*)&(pst2ndItem->unInfo.stDsi);

			ulListLen = HLIB_LIST_Length(pstDsiGroup->pstGroupCompList);

			pHxList = pstDsiGroup->pstGroupCompList;

			bHwFound = FALSE;
			bSwFound = FALSE;

			HxLOG_Print("[%s:%d] ulListLen : %d) \n", __FUNCTION__, __LINE__, ulListLen);

			for(ulIndex= 0 ; ulIndex< ulListLen; ulIndex++)
			{
				pucRaw = (HUINT8*)HLIB_LIST_Data(pHxList);
				if(pucRaw == NULL)
				{
					HxLOG_Error ("pucRaw is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}

				xsvc_si_SwupCheckParseCompability_EuDtt(pucRaw, NULL, &bHwFound, &bSwFound, &ulSignalVersion);

				pHxList = HLIB_LIST_Next(pHxList);
				if(pHxList == NULL)
				{
					HxLOG_Error ("pHxList is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}
			}

			if (bHwFound == TRUE && bSwFound == TRUE)
			{
				HxLOG_Print ("Found SWUP H/W Ver & S/W Ver.!! [ bHwFound : %d  , bSwFound : %d ] \n", bHwFound, bSwFound);

				pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
				if (pstSwupInfo != NULL)
				{
					HxSTD_memset (pstSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

					pstSwupInfo->ulStartSystemId	= ulSystemId;
					pstSwupInfo->ulEndSystemId		= ulSystemId;
					pstSwupInfo->usPid 				= pstDsiSec->usExtId;
					pstSwupInfo->usDataType			= eSWUP_DATATYPE_APPL;

#if defined(SWUP_NEW_TESTED)
					xsvc_si_SwupCheckGetSwupDownType(&pstSwupInfo->ucSwupType);
#else
#if defined(CONFIG_OP_ERTELECOM)
					//pstSwupInfo->ucSwupType	= s_ucSwupDownType;
					xsvc_si_SwupCheckGetSwupDownType(&pstSwupInfo->ucSwupType);

#else
					//pstSwupInfo->ucOtaType	= Forced; // Forced 인지 처리하는 부분이 없네요..ㅜㅜㅜ
#endif
#endif
					svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSwupInfo->usDataType, &(pstSwupInfo->eDataType));
					pstSwupInfo->ulDataVersion = ulSignalVersion;

					HxLOG_Debug("Found SWUP [ ulSystemId : 0x%x  , usPid : 0x%x , ulSignalVersion : 0x%x] \n", pstSwupInfo->ulStartSystemId, pstSwupInfo->usPid, pstSwupInfo->ulDataVersion);

					pstSwupList = HLIB_LIST_Append (pstSwupList, (void *)pstSwupInfo);
				}
			}
		}
	}

	ulNumSsu = (HUINT32)HLIB_LIST_Length(pstSwupList);
	if(ulNumSsu == 0 )
	{
		HxLOG_Error("ulNumSsu is Zero \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pastSwupSignals != NULL)
	{
		ulIndex = 0;
		astSwupArray = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t) * ulNumSsu);
		if (astSwupArray == NULL)
		{
			HLIB_LIST_RemoveAllFunc (pstSwupList, xsvc_si_SwupCheckFreeListItem_EuDtt);
			return ERR_FAIL;
		}

		for(ulIndex = 0; ulIndex < ulNumSsu; ulIndex++)
		{
			pHxList = HLIB_LIST_GetListItem(pstSwupList, ulIndex);

			pstSwupInfo = (SvcSi_SwupSignal_t *)HLIB_LIST_Data(pHxList);
			if (pstSwupInfo == NULL)
			{
				HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", ulIndex);
				continue;
			}

			HxSTD_MemCopy(&astSwupArray[ulIndex], pstSwupInfo, sizeof(SvcSi_SwupSignal_t));
		}
		*pastSwupSignals = astSwupArray;
	}

	*pulNumSsu = ulNumSsu;

	hErr = ERR_OK;

Error:

	if (pstDsiTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstDsiTable); }
	if (pstSwupList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstSwupList, xsvc_si_SwupCheckFreeListItem_EuDtt); }

	return hErr;
}

#define _____SWUP_INFO_TO_API_____
STATIC HERROR xsvc_si_SwupCheckGetDataDeliveryTsInfo_EuDtt (Handle_t hAction, HUINT32 *pulNumTs, void **pavTsInfoArray)
{
	HUINT32 		 ulActionId;
	HERROR			 hErr = ERR_FAIL;
	HERROR			 hResult = ERR_FAIL;
	HUINT32 		 ulNitActTsNum = 0, ulNitOthTsNum = 0;
	DbSvc_TsInfo_t	*pstNitActTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitOthTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitTsInfo = NULL;


	if ((pulNumTs == NULL) || (pavTsInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumTs : %p, pavTsInfoArray : %p)\n",pulNumTs, pavTsInfoArray );
		return ERR_FAIL;
	}

	*pulNumTs = 0;
	*pavTsInfoArray = NULL;

	ulActionId = PAL_PIPE_GetActionId (hAction);
	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit(ulActionId, TRUE, TRUE, &ulNitActTsNum, &pstNitActTsInfo);
	if ((hErr != ERR_OK) || (ulNitActTsNum == 0))
	{
		HxLOG_Print("[%s:%d] svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitActTsNum == 0\n", __FUNCTION__, __LINE__);
	}

	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit(ulActionId, FALSE, TRUE, &ulNitOthTsNum, &pstNitOthTsInfo);
	if ((hErr != ERR_OK) || (ulNitOthTsNum == 0))
	{
		HxLOG_Print("[%s:%d] svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitOthTsNum == 0\n", __FUNCTION__, __LINE__);
	}

	if (ulNitActTsNum + ulNitOthTsNum == 0)
	{
		HxLOG_Print("[%s:%d] no ts delivery infomation \n", __FUNCTION__, __LINE__);
		goto Exit_Func;
	}

	pstNitTsInfo = (DbSvc_TsInfo_t *)OxSI_Malloc (sizeof(DbSvc_TsInfo_t) * (ulNitActTsNum + ulNitOthTsNum));
	if (NULL == pstNitTsInfo)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto Exit_Func;
	}

	if (pstNitActTsInfo > 0)
	{
		HxSTD_MemCopy(pstNitTsInfo, pstNitActTsInfo, sizeof(DbSvc_TsInfo_t) * ulNitActTsNum);
	}

	if (ulNitOthTsNum > 0)
	{
		HxSTD_MemCopy(&pstNitTsInfo[ulNitActTsNum], pstNitOthTsInfo, sizeof(DbSvc_TsInfo_t) * ulNitOthTsNum);
	}

	*pulNumTs = ulNitActTsNum + ulNitOthTsNum;
	*pavTsInfoArray = pstNitTsInfo;
	hResult = ERR_OK;

Exit_Func:
	if (pstNitActTsInfo != NULL)			{ OxSI_Free (pstNitActTsInfo); }
	if (pstNitOthTsInfo != NULL)			{ OxSI_Free (pstNitOthTsInfo); }

	return hResult;
}

// NIT를 통해 얻은 SSU SCAN Linkage 정보 (SSU-SCAN를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuScanTripleId_EuDtt (Handle_t hAction, HUINT32 *pulNumSsuScan, void **ppvTripleIdArray)
{
	HERROR				 hErr;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsuScan == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsuLink : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsuScan, ppvTripleIdArray );
		return ERR_FAIL;
	}

	*pulNumSsuScan = 0;
	*ppvTripleIdArray = NULL;

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU_SCAN, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdArray);
	if (hErr != ERR_OK || *pulNumSsuScan == 0)
	{
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitOth, eLINKAGE_SSU_SCAN, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdArray);
	}

	return hErr;
}

// NIT를 통해 얻은 SSU Linkage 정보 (SSU-SVC를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleId_EuDtt  (Handle_t hAction,  HUINT32 *pulNumSsu, void **ppvTripleIdArray)
{
	HERROR				 hErr;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsu == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsu : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsu, ppvTripleIdArray );
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_ALL, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
	if (hErr != ERR_OK || *pulNumSsu == 0)
	{
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitOth, eLINKAGE_ALL, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
	}

	return hErr;
}

// PMT를 통해 얻은 SWUP Signalling Information
STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo_EuDtt  (Handle_t hAction, HUINT32 *pulNumInfo, void **pavSwupInfoArray)
{
	SvcSi_SwupSignal_t	*pstSwupInfo;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumInfo == NULL) || (pavSwupInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumInfo : %p, pavSwupInfoArray : %p)\n",pulNumInfo, pavSwupInfoArray );
		return ERR_FAIL;
	}

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error("pstSwupCheck is NULL \n");
		return ERR_FAIL;
	}

	if (pstSwupCheck->bFound!= TRUE)
	{
		HxLOG_Error("bFound is FALE \n");
		return ERR_FAIL;
	}

	if (pulNumInfo != NULL)
	{
		*pulNumInfo = 1;
	}

	if (pavSwupInfoArray != NULL)
	{
		pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
		if (pstSwupInfo == NULL)
		{
			HxLOG_Error("pstSwupInfo Memalloc Error\n");
			return ERR_FAIL;
		}

		*pstSwupInfo = pstSwupCheck->stSwupInfo;
		*pavSwupInfoArray = (void *)pstSwupInfo;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckGetDesInfoFromUnt_EuDtt (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HUINT32				 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulDataNum == NULL) || (ppvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulDataNum : %p, ppvDataArray : %p)\n",pulDataNum, ppvDataArray );
		return ERR_FAIL;
	}

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error("pstSwupCheck is NULL \n");
		return ERR_FAIL;
	}

#if 0
	HUINT32 	ulTag, ulCount = 0;

	SvcSi_SsuMessageInfo_t 		*pstSsuMessageInfo = NULL;
	SvcSi_SsuScheduleInfo_t		*pstSsuScheduleInfo = NULL;
	SvcSi_SsuUpdateInfo_t		*pstSsuUpdateInfo = NULL;
	SvcSi_SsuDescriptorInfo_t	*pstSsuDescriptorInfo = NULL;

	HxLOG_Print("\n(+) local_dvbssuota_GetDesInfoFromUnt ========================= (%d) \n", eDataType);

	switch (eDataType)
	{
	case eSI_SWUP_DATA_SSU_UPDATE_INFO:
		ulTag = TAG_UPDATE;
		break;
	case eSI_SWUP_DATA_SSU_SCHEDULE_INFO:
		ulTag = TAG_SCHEDULING;
		break;
	case eSI_SWUP_DATA_SSU_MESSAGE_INFO:
		ulTag = TAG_MESSAGE;
		break;
	default :
		ulTag = TAG_END_RESERVED;
		break;
	}

	if(ulTag == TAG_END_RESERVED)	{ return ERR_FAIL;}

	if(ppvDataArray != NULL && ulTag == TAG_MESSAGE)
	{
		pstSsuDescriptorInfo = pstSwupData->pstSsuDescriptorInfo;

		if (pstSsuDescriptorInfo == NULL || pstSsuDescriptorInfo->pMessageInfo == NULL || pstSsuDescriptorInfo->usNumMessageInfo == 0)
		{
			return ERR_FAIL;
		}

		ulCount = pstSsuDescriptorInfo->usNumMessageInfo;
		pstSsuMessageInfo = (SvcSi_SsuMessageInfo_t *)OxSI_Malloc(sizeof(SvcSi_SsuMessageInfo_t)*ulCount );
		if (pstSsuMessageInfo == NULL)
		{
			return ERR_FAIL;
		}

		HxSTD_memcpy (pstSsuMessageInfo, pstSsuDescriptorInfo->pMessageInfo, sizeof(SvcSi_SsuMessageInfo_t)*ulCount);
		*ppvDataArray = (void *)pstSsuMessageInfo;
		*pulDataNum = ulCount;
	}
	else if(ppvDataArray != NULL && ulTag == TAG_UPDATE)
	{
		pstSsuDescriptorInfo = pstSwupData->pstSsuDescriptorInfo;

		if (pstSsuDescriptorInfo == NULL || pstSsuDescriptorInfo->pUpdateInfo == NULL || pstSsuDescriptorInfo->usNumUpdateInfo == 0)
		{
			return ERR_FAIL;
		}

		ulCount = pstSsuDescriptorInfo->usNumUpdateInfo;
		pstSsuUpdateInfo = (SvcSi_SsuUpdateInfo_t *)OxSI_Malloc(sizeof(SvcSi_SsuUpdateInfo_t) *ulCount);
		if (pstSsuUpdateInfo == NULL)
		{
			return ERR_FAIL;
		}

		HxSTD_memcpy (pstSsuUpdateInfo, pstSsuDescriptorInfo->pUpdateInfo, sizeof(SvcSi_SsuUpdateInfo_t) *ulCount);
		*ppvDataArray = (void *)pstSsuUpdateInfo;
		*pulDataNum = ulCount;
	}
	else if(ppvDataArray != NULL && ulTag == TAG_SCHEDULING)
	{
		pstSsuDescriptorInfo = pstSwupData->pstSsuDescriptorInfo;

		if (pstSsuDescriptorInfo == NULL || pstSsuDescriptorInfo->pScheduleInfo == NULL || pstSsuDescriptorInfo->usNumScheduleInfo == 0)
		{
			return ERR_FAIL;
		}

		ulCount = pstSsuDescriptorInfo->usNumScheduleInfo;
		pstSsuScheduleInfo = (SvcSi_SsuScheduleInfo_t *)OxSI_Malloc(sizeof(SvcSi_SsuScheduleInfo_t) *ulCount);
		if (pstSsuScheduleInfo == NULL)
		{
			return ERR_FAIL;
		}

		HxSTD_memcpy (pstSsuScheduleInfo, pstSsuDescriptorInfo->pScheduleInfo, sizeof(SvcSi_SsuScheduleInfo_t) *ulCount);
		*ppvDataArray = (void *)pstSsuScheduleInfo;
		*pulDataNum = ulCount;
	}
	else
	{
		return ERR_FAIL;
	}
#endif
	return ERR_OK;

}
#define _____SI_MSG_PROCESS_FUNCTION_____

STATIC void xsvc_si_SwupCheckMsgCmdStart_EuDtt(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_EuDtt(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_EuDtt(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_EuDtt (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_EuDtt(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_SwupCheckStopSection_EuDtt(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR		hErr;
	HUINT16		usPmtPid;
	HUINT32		ulActionId;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_EuDtt_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);

			hErr = svc_si_LibGetPmtPidFromPat(ulActionId, pstSwupCheck->usPmtSvcId,  &usPmtPid);
			if(hErr == ERR_OK)
			{
				HxLOG_Print ("[%s:%d] svc_si_SwupCheckStartFilter (SvcID : 0x%x, PmtPid : 0X%x) \n", __FUNCTION__, __LINE__, pstSwupCheck->usPmtSvcId, usPmtPid);

				hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Pmt, (HUINT32)pstSwupCheck->usPmtSvcId, (HUINT32)usPmtPid);
				if (ERR_OK != hErr)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR					 hErr;
	HUINT32					 ulIndex, ulNumSsu;
	xsvcSi_SwupPsiInfo_t	*pstSwupInfoArray;
	HUINT32 				 ulExtId = SILIB_TABLE_EXT_ID_NOT;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_EUDTT_SSUSVC_TP:		// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_EuDtt(pstSwupCheck, &ulNumSsu, &pstSwupInfoArray);

			//HxLOG_Print("[%s:%d] hErr(0x%x), ulNumSsu(0x%x), pstSwupInfoArray(0x%x)\n\n", __FUNCTION__, __LINE__, hErr, ulNumSsu, pstSwupInfoArray);

			if (hErr != ERR_OK || ulNumSsu == 0 || pstSwupInfoArray == NULL)
			{
				if(pstSwupInfoArray != NULL)
				{
					OxSI_Free (pstSwupInfoArray);
					pstSwupInfoArray = NULL;
				}
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			for (ulIndex = 0; ulIndex < ulNumSsu; ulIndex++)
			{
				if (pstSwupInfoArray[ulIndex].bUseUnt == TRUE)
				{
					HxLOG_Print("[EUDTT_SWUP] Retrieve UNT (PID 0x%04x)\n", pstSwupInfoArray[ulIndex].usPid);
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Unt, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
				}
				else
				{
					HxLOG_Print("[UKDTT_SWUP] Retrieve DSI (PID 0x%04x)\n", pstSwupInfoArray[ulIndex].usPid);
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Dsi, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
				}
			}

			if(pstSwupInfoArray != NULL)
			{
				OxSI_Free (pstSwupInfoArray);
				pstSwupInfoArray = NULL;
			}

// TODO: 확인 필요..
#if 0
			hErr = SVC_SI_DMX_CountSectionTable (pstSwupData->hAction, eSI_TableType_DSI, &ulDsiCount);
			if (hErr != ERR_OK)			{ ulDsiCount = 0; }

			hErr = SVC_SI_DMX_CountSectionTable (pstSwupData->hAction, eSI_TableType_UNT, &ulUntCount);
			if (hErr != ERR_OK)			{ ulUntCount = 0; }

			if ((ulDsiCount + ulUntCount) == 0)
			{
				HxLOG_Critical("\n\n");
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}
#endif
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtNitTable_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;
	SIxTable_Type_e	eSTableType;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	if(pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE)
		eSTableType = eSIxTABLETYPE_NitAct;
	else
		eSTableType = eSIxTABLETYPE_NitOth;

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_UKDTT_HOME_TP:
			{
				hErr = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
				if(hErr != ERR_OK)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

					return;
				}

				// SSU Linkage 정보를 찾는다.
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSTableType, eLINKAGE_ALL, NULL, NULL);
				if (hErr != ERR_OK)
				{				
					// 아무 정보도 없다 : SWUP Fail
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtUntTable_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_UKDTT_SSUSVC_TP:
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtDsiTable_EuDtt (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR					 hErr;
	HUINT16					 usPid;
	HUINT32 				 ulNum;
	SvcSi_SwupSignal_t		*astSwupArray;

	HxLOG_Debug("+++ \n");

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_UKDTT_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				xsvc_si_SwupCheckStopSection_EuDtt(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = xsvc_si_SwupCheckFindSwupInfoFromDsi_EuDtt(pstSwupCheck, &ulNum, &astSwupArray, &usPid);
			if (hErr == ERR_OK && ulNum > 0 && astSwupArray != NULL)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(SvcSi_SwupSignal_t));

				OxSI_Free (astSwupArray);

				xsvc_si_SwupCheckStopSection_EuDtt(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}
			else
			{
				xsvc_si_SwupCheckStopSection_EuDtt(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_EuDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}

	return;
}

STATIC void xsvc_si_SwupCheckProcessMsg_EuDtt (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)pvData;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_START_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStart_EuDtt(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_STOP_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStop_EuDtt(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPatTable_EuDtt (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PMT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPmtTable_EuDtt (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_NIT_TABLE! (%s)\n",__FUNCTION__,__LINE__, pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE? "NIT_ACT" : "NIT_OTH");
		xsvc_si_SwupCheckMsgEvtNitTable_EuDtt (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_UNT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_UNT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtUntTable_EuDtt (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_DSI_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_DSI_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtDsiTable_EuDtt (pstSwupCheck, pstMsg);
		break;

	default:
		HxLOG_Debug("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}
#endif

#define ___API_FUNCTIONS___

SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_EuDtt (void)
{
	//return xsvc_si_SwupCheckProcessMsg_EuDtt;
	return NULL;
}

void *xsvc_si_SwupCheckAllocData_EuDtt  (void)
{
	xsvcSi_SwupData_t		*pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_EuDtt  (void *pvData)
{
	xsvcSi_SwupData_t		*pstSwupCheck = (xsvcSi_SwupData_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
	}

	return;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_EuDtt (Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL || ppvDataArray == NULL)
	{
		HxLOG_Error ("invalide parameter\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%d] eDataType: %s \n", __FUNCTION__, __LINE__, xsvc_si_SwupCheckMakeStrMode_EuDtt(eDataType));
#endif

#if 0
	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
		case eSI_SWUP_DATA_DELIVERY_TSINFO:
			hErr = xsvc_si_SwupCheckGetDataDeliveryTsInfo_EuDtt(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_SCAN_TRIPLEID:
			hErr = xsvc_si_SwupCheckGetSsuScanTripleId_EuDtt(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_LINK_TRIPLEID:
			hErr = xsvc_si_SwupCheckGetSsuLinkTripleId_EuDtt(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_SIGNAL:
			hErr = xsvc_si_SwupCheckGetSwupSignalInfo_EuDtt(hAction, pulDataNum, ppvDataArray);
			break;

		default:
			hErr = ERR_FAIL;
			break;
	}
#endif
	HxLOG_Print("[%s:%d] hErr: %d \n",  __FUNCTION__, __LINE__,hErr);

	return hErr;

}


HERROR xsvc_si_SwupCheckFreeSwupCheckData_EuDtt (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
{
	(void)hAction;
	(void)ulDataType;

	if (pvDataArray != NULL)
	{
		OxSI_Free (pvDataArray);
		pvDataArray = NULL;
	}

	return ERR_OK;
}

HERROR 		xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_EuDtt(SIxLinkDes_t *pstLinkDes)
{
	return svc_si_SwupCheckFindProperOuiDataFromLinkDes(pstLinkDes);
}
/* end of file */

