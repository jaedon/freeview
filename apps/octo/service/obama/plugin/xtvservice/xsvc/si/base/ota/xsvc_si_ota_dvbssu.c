/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_dvbssu.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /SWUP CHECK	\n

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



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static variable     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Functions     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xsvc_si_SwupCheckMakeStrMode_Dvbssu(XsvcSi_SwupMode_e eSwupMode)
{
	switch (eSwupMode)
	{
		ENUM_TO_STR(eSI_SWUP_MODE_DVBSSU_HOME_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_DVBSSU_SSUSVC_TP);
		default:
			break;
	}

	return ("Unknown");
}

STATIC const HINT8 * xsvc_si_SwupCheckMakeStrType_Dvbssu(XsvcSi_SwupDataType_e eSwupDataType)
{
	switch (eSwupDataType)
	{
		ENUM_TO_STR(eSI_SWUP_DATA_DELIVERY_TSINFO);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_SCAN_TRIPLEID);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_LINK_TRIPLEID);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_SIGNAL);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_UPDATE_INFO);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_SCHEDULE_INFO);
		ENUM_TO_STR(eSI_SWUP_DATA_SSU_MESSAGE_INFO);
		default:
			break;
	}

	return ("Unknown");
}

#endif

STATIC HERROR xsvc_si_SwupCheckInitSwupData_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck)
{
	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}
	xsvc_si_SwupCheckSetSwupDownType(SvcSi_SWUP_TYPE_USER_SELECT);

	pstSwupCheck->usBatBouquetId 	= SSU_BOUQUET_ID;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("ulSwupMode(%s) usBatBouquetId(0x%x), %d \n"
		, xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode), pstSwupCheck->usBatBouquetId,pstSwupCheck->stBase.stSwupOption.ulSwupMode);
#endif

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
		pstSwupCheck->usPmtSvcId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;

	case eSI_SWUP_MODE_DVBSSU_HOME_TP:
		pstSwupCheck->usNetIdForNitOth = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;

	default:
		break;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
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

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstSwupCheck->usBatBouquetId;
		*pulPid			= BAT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HUINT8 *xsvc_si_SwupGetSiTableTypeStr(SIxTable_Type_e eType)
{
#ifdef CONFIG_DEBUG
	switch(eType)
	{
		ENUM_TO_STR(eSIxTABLETYPE_Pat);
		ENUM_TO_STR(eSIxTABLETYPE_Pmt);
		ENUM_TO_STR(eSIxTABLETYPE_NitAct);
		ENUM_TO_STR(eSIxTABLETYPE_NitOth);
		ENUM_TO_STR(eSIxTABLETYPE_Bat);
		ENUM_TO_STR(eSIxTABLETYPE_SdtAct);
		ENUM_TO_STR(eSIxTABLETYPE_SdtOth);
		ENUM_TO_STR(eSIxTABLETYPE_EitActPf);
		ENUM_TO_STR(eSIxTABLETYPE_EitOthPf);
		ENUM_TO_STR(eSIxTABLETYPE_EitActSc);
		ENUM_TO_STR(eSIxTABLETYPE_EitOthSc);
		ENUM_TO_STR(eSIxTABLETYPE_Tdt);
		ENUM_TO_STR(eSIxTABLETYPE_Tot);
		ENUM_TO_STR(eSIxTABLETYPE_Cat);
		default:
			return "Unknown";
	}
#else
	return (HUINT8 *)("");
#endif
}


STATIC HERROR xsvc_si_SwupCheckStartSection_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck)
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
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Dvbssu(pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSwupCheck->stBase.eFiltering & eFilterType))
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("%s start...\n"), xsvc_si_SwupGetSiTableTypeStr(eTableType));
			hErr = svc_si_SwupCheckStartFilter (&pstSwupCheck->stBase, eTableType, ulExtId, ulPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_SwupCheckStartFilter err: eTableType(%d)\n", eTableType);
			}
		}
		else
		{
			//HxLOG_Error ("xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Dvbssu err: hErr(%d) eFiltering(%d) eTableType(%d)\n", hErr, pstSwupCheck->stBase.eFiltering, eFilterType);
		}
	}

	if (NULL == pstSwupCheck->stBase.pstFilterings)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStopSection_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
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

STATIC void xsvc_si_SwupCheckFreePsiInfo_Dvbssu (void *pvData)
{
	xsvcSi_SwupPsiInfo_t	*pstSwupInfo = (xsvcSi_SwupPsiInfo_t *)pvData;

	if (NULL != pstSwupInfo)
	{
		OxSI_Free(pstSwupInfo);
	}

	return;
}

STATIC void	 xsvc_si_SwupCheckFreeListItem_Dvbssu (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}


#define _____SWUP_PARSING_____

STATIC HERROR xsvc_si_SwupCheckFindProperOuiData_Dvbssu (HUINT32 ulSelectorNum, svcSi_SwupCheck_SsuInfo_t *pstSelectors, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
{
	HUINT32			 ulIndex;

	if (0 == ulSelectorNum || pstSelectors == NULL|| pstSwupInfo == NULL)
	{
		HxLOG_Error ("invalid parameter \n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < ulSelectorNum; ulIndex++)
	{
		if (pstSelectors[ulIndex].ulOui == eSWUP_OUI_HUMAX
			|| pstSelectors[ulIndex].ulOui == eSWUP_OUI_DVB)
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

STATIC HERROR xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_Dvbssu (SIxDataBroadcastIdDes_t *pstDbcIdDes, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
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

	hErr = xsvc_si_SwupCheckFindProperOuiData_Dvbssu (ulSelectorNum, pstOuiSelectors, pstSwupInfo);

	svc_si_SwupCheckFreeSsuInfo(pstOuiSelectors);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckParseCompability_Dvbssu (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound,
																					HUINT32 *pulSystemVer, SvcSi_OtaFailReason_e *peFailReason)
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
		HxLOG_Print("[%s][%d] ________________________ ulOui(0x%x) ulSignalSystemVer(0x%x) ulSystemVer(0x%x) \n", __FUNCTION__, __LINE__, ulOui, ulSignalSystemVer, ulSystemVer);

		if (((ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX)) && (ulSignalSystemVer > ulSystemVer))
		{
			if (ppucNextDesRaw != NULL)				{ *ppucNextDesRaw = pucNextDesRaw; }
			*pbSwFound = TRUE;
			*pulSystemVer = ulSignalSystemVer;
			return ERR_OK;
		}

		if(peFailReason) {	*peFailReason = eSvcSi_OTA_FAILREASON_Unknown;	}

#if defined(CONFIG_OP_TDC)
		if (((ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX)) && (ulSignalSystemVer <= ulSystemVer))
		{
			if(peFailReason) {	*peFailReason = eSvcSi_OTA_FAILREASON_Latest_Version;	}
		}
#endif

		break;
	}

	if (ppucNextDesRaw != NULL) 			{ *ppucNextDesRaw = pucNextDesRaw; }
	return ERR_OK;

}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, xsvcSi_SwupPsiInfo_t **pastSwupSignals)
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
						hErr = xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_Dvbssu(pstDbcIdDes,&stDvbssuPsiInfo);
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
		HxLOG_Critical("ulNumSsuSvc is Zero \n");
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
	if(pstDvbssuPsiInfoList != NULL)					{ HLIB_LIST_RemoveAllFunc (pstDvbssuPsiInfoList, xsvc_si_SwupCheckFreePsiInfo_Dvbssu);}

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckGetDsmccPidFromPmt_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck, HUINT16 usAssociationTag, HUINT16 *pusPid)
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

					pstStmIdentDes = (SIxStmIdentDes_t*)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if(pstStmIdentDes!= NULL)
					{
						if(pstStmIdentDes->ucCompTag == (HUINT8)usAssociationTag)
						{
							// DSM-CC Pid Found!!
							*pusPid = pstPmtElem->usElemPid;

							if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
							if(pstStmIdentDes!= NULL)	{PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *) pstStmIdentDes);}

							return ERR_OK;
						}

						if(pstStmIdentDes!= NULL)	{PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *) pstStmIdentDes);}
					}
				}
			}
		}
	}

	if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckSetDesInfoFromUnt_Dvbssu(void  **ppvSsuDescriptorInfo, HUINT32 ulTag ,  void **ppvSsuDescriptorRawInfo)
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

STATIC HERROR xsvc_si_SwupCheckFindAssociationTagInOperationalDes(xsvcSi_SwupData_t *pstSwupCheck,HxList_t *pListOp, HUINT16 *pusAssociationTag)
{
	HxList_t	*pstLoop = NULL;
	HUINT8 	*pucRawDes =  NULL;
	HUINT32	ulActionId;

	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);

	for(pstLoop = pListOp; NULL != pstLoop; pstLoop = pstLoop->next)
	{
		pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstLoop);

		switch(*pucRawDes)
		{
			case eSIxDESCTAG_UNT_SSU_LOCATION:
			{
				SIxSsuLocationDes_t	*pstLocDes = (SIxSsuLocationDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);
				if (NULL != pstLocDes)
				{
					HxLOG_Debug("\t\t usDataBroadcastId(0x%X) usDataBroadcastId(0x%X) \n", pstLocDes->usDataBroadcastId, pstLocDes->usAssociationTag);

					*pusAssociationTag = pstLocDes->usAssociationTag;
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstLocDes);

					return ERR_OK;
				}
			}
				break;
			default:
				HxLOG_Debug("\t\t DesTag(0x%X) ", *pucRawDes);
				break;

		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckFindAssociationTagInCommonDes(xsvcSi_SwupData_t *pstSwupCheck,HxList_t *pListCom, HUINT16 *pusAssociationTag)
{
	HxList_t	*pstLoop = NULL;
	HUINT8 	*pucRawDes =  NULL;
	HUINT32	ulActionId;

	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);

	for(pstLoop = pListCom; NULL != pstLoop; pstLoop = pstLoop->next)
	{
		pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstLoop);

		switch(*pucRawDes)
		{
			case eSIxDESCTAG_UNT_SSU_LOCATION:
			{
				SIxSsuLocationDes_t	*pstLocDes = (SIxSsuLocationDes_t *)PAL_SIRAWPOOL_ParseSsuUntDes (ulActionId, pucRawDes);
				if (NULL != pstLocDes)
				{
					HxLOG_Debug("\t\t usDataBroadcastId(0x%X) usDataBroadcastId(0x%X) \n", pstLocDes->usDataBroadcastId, pstLocDes->usAssociationTag);

					*pusAssociationTag = pstLocDes->usAssociationTag;
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstLocDes);

					return ERR_OK;
				}
			}
				break;
			default:
				HxLOG_Debug("\t\t DesTag(0x%X) ", *pucRawDes);
				break;

		}
	}

	return ERR_FAIL;

}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromUnt_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals,
																					 SvcSi_OtaFailReason_e *peFailReason)
{
	HERROR				hErr;
	HUINT32				ulActionId, ulSystemId, ulSwVersion, ulSignalSwVer = 0;
	SIxTable_PsiTable_t		*pstUntTable = NULL, *pstCurrUnt = NULL;
	SIxTable_UntSec_t		*pstUntSecInfo = NULL;
	HxList_t				*pst2ndLoop = NULL, *pHxList = NULL, *pstSwupList = NULL;
	HUINT32				 ulIndex, ulNumSsu;
	HUINT8				*pucRaw = NULL;
	HUINT16				usPid;
	SvcSi_SwupSignal_t		*pstSwupInfo = NULL, *astSwupArray = NULL;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
		hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSwVersion, sizeof(HUINT32));
	}
	else
	{
		hErr  = PAL_SYS_GetSystemId (&ulSystemId);
		hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSwVersion);
	}
	if (hErr != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstUntTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
	if (pstUntTable == NULL)
	{
		HxLOG_Error("eSIxTABLETYPE_Unt parse failed..!!... ");
		hErr = ERR_FAIL;
		goto Error;
	}

	for (pstCurrUnt = pstUntTable; pstCurrUnt != NULL; pstCurrUnt = pstCurrUnt->pstNextTable)
	{
		pstUntSecInfo = &pstCurrUnt->unInfo.stUnt;

		if((pstUntSecInfo->ulOui != eSWUP_OUI_DVB)
			&& (pstUntSecInfo->ulOui  != eSWUP_OUI_HUMAX))
		{
			continue;
		}

		// 2st Descriptor loop
		for (pst2ndLoop = pstCurrUnt->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t	*pst2ndItem  = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pst2ndLoop);
			SIxTable_UntInfo_t		*pstUntInfo = NULL;
			HBOOL				bHwFound = FALSE,bSwFound = FALSE;
			HUINT16				usAssociationTag;

			if(pst2ndItem == NULL)	{continue;}

			pstUntInfo = &pst2ndItem->unInfo.stUnt;

			// Check Compatibility Descriptor
			for(pHxList = pstUntInfo->pstCompDesLoop; NULL != pHxList; pHxList = pHxList->next)
			{
				pucRaw = (HUINT8*)HLIB_LIST_Data(pHxList);
				if(pucRaw == NULL)
				{
					HxLOG_Error ("pucRaw is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}

				xsvc_si_SwupCheckParseCompability_Dvbssu(pucRaw, NULL, &bHwFound, &bSwFound, &ulSignalSwVer, peFailReason);
			}

			if(bHwFound == TRUE && bSwFound == TRUE)
			{
				hErr = xsvc_si_SwupCheckFindAssociationTagInOperationalDes(pstSwupCheck, pstUntInfo->pstOperationalDesLoop, &usAssociationTag);
				if(hErr == ERR_FAIL)
				{
					hErr = xsvc_si_SwupCheckFindAssociationTagInCommonDes(pstSwupCheck, pstUntInfo->pstCompDesLoop, &usAssociationTag);
				}

				if(hErr == ERR_OK)
				{
					hErr = xsvc_si_SwupCheckGetDsmccPidFromPmt_Dvbssu( pstSwupCheck, usAssociationTag, &usPid);
					if(hErr == ERR_OK)
					{
						pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
						if (pstSwupInfo != NULL)
						{
							HxSTD_memset (pstSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

							pstSwupInfo->ulStartSystemId	= ulSystemId;
							pstSwupInfo->ulEndSystemId		= ulSystemId;
							pstSwupInfo->usPid			= usPid;
							pstSwupInfo->usDataType 		= eSWUP_DATATYPE_APPL;

							xsvc_si_SwupCheckGetSwupDownType(&pstSwupInfo->ucSwupType);
							svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSwupInfo->usDataType, &(pstSwupInfo->eDataType));
							pstSwupInfo->ulDataVersion = ulSignalSwVer;

							HxLOG_Debug("Found SWUP [ ulSystemId : 0x%x  , usPid : 0x%x , ulSignalVersion : 0x%x] \n", pstSwupInfo->ulStartSystemId, pstSwupInfo->usPid, pstSwupInfo->ulDataVersion);

							pstSwupList = HLIB_LIST_Append (pstSwupList, (void *)pstSwupInfo);
						}
					}
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
			HxLOG_Error("Error! OxSI_Malloc() failed! \n");
			hErr = ERR_FAIL;
			goto Error;
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

	if (pstUntTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstUntTable); }
	if (pstSwupList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstSwupList, xsvc_si_SwupCheckFreeListItem_Dvbssu); }

	return hErr;

}

STATIC HERROR xsvc_si_SwupCheckFindSwupInfoFromDsi_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, HUINT32 *pulNumSsu,
																							SvcSi_SwupSignal_t **pastSwupSignals, HUINT16 *pusPid, SvcSi_OtaFailReason_e *peFailReason)
{
	HBOOL				 bHwFound, bSwFound;
	HUINT32				 ulSignalVersion = 0;
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

	// PMT parsing해서
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

			HxLOG_Print("ulListLen : %d\n", ulListLen);

			for(ulIndex= 0 ; ulIndex< ulListLen; ulIndex++)
			{
				pucRaw = (HUINT8*)HLIB_LIST_Data(pHxList);
				if(pucRaw == NULL)
				{
					HxLOG_Error ("pucRaw is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}

				xsvc_si_SwupCheckParseCompability_Dvbssu(pucRaw, NULL, &bHwFound, &bSwFound, &ulSignalVersion, peFailReason);

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

					xsvc_si_SwupCheckGetSwupDownType(&pstSwupInfo->ucSwupType);
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
			hErr = ERR_FAIL;
			goto Error;
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
	if (pstSwupList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstSwupList, xsvc_si_SwupCheckFreeListItem_Dvbssu); }

	return hErr;
}

#define _____SWUP_INFO_TO_API_____

STATIC HERROR xsvc_si_SwupCheckGetDataDeliveryTsInfo_Dvbssu (Handle_t hAction, HUINT32 *pulNumTs, void **pavTsInfoArray)
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
		HxLOG_Print("svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitActTsNum == 0\n");
	}

	hErr = svc_si_LibGetAllDeliveryTsInfoFromNit(ulActionId, FALSE, TRUE, &ulNitOthTsNum, &pstNitOthTsInfo);
	if ((hErr != ERR_OK) || (ulNitOthTsNum == 0))
	{
		HxLOG_Print("svc_si_LibGetAllDeliveryTsInfoFromNit() fail or ulNitOthTsNum == 0\n");
	}

	if (ulNitActTsNum + ulNitOthTsNum == 0)
	{
		HxLOG_Print("no ts delivery infomation \n");
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

// NIT를 통해 얻은 SSU SCAN Linkage 정보 (SSU-BAT를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuScanTripleId_Dvbssu (Handle_t hAction, HUINT32 *pulNumSsuScan, void **ppvTripleIdArray)
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
		if (hErr != ERR_OK || *pulNumSsuScan == 0)
		{
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_SSU_SCAN, pulNumSsuScan, (DbSvc_TripleId_t **)ppvTripleIdArray);
		}
	}

	return hErr;
}

// BAT를 통해 얻은 SSU Linkage 정보 (SSU-SVC를 가리키는 Triple ID List)
STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleId_Dvbssu  (Handle_t hAction,  HUINT32 *pulNumSsu, void **ppvTripleIdArray)
{
	HERROR				 hErr;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_t	*pstSwupCheck = (xsvcSi_SwupData_t *)svc_si_GetSiData(ulActionId);

	if ((pulNumSsu == NULL) || (ppvTripleIdArray == NULL))
	{
		HxLOG_Error("Invalid parameter (pulNumSsu : %p, ppvTripleIdInfoArray : %p)\n",pulNumSsu, ppvTripleIdArray );
		return ERR_FAIL;
	}

	hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_SSU, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
	if (hErr != ERR_OK || *pulNumSsu == 0)
	{
		hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitAct, eLINKAGE_SSU, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
		if (hErr != ERR_OK || *pulNumSsu == 0)
		{
			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_NitOth, eLINKAGE_SSU, pulNumSsu, (DbSvc_TripleId_t **)ppvTripleIdArray);
		}
	}

	return hErr;
}

// PMT를 통해 얻은 SWUP Signalling Information
STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo_Dvbssu  (Handle_t hAction, HUINT32 *pulNumInfo, void **pavSwupInfoArray)
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

STATIC HERROR xsvc_si_SwupCheckGetDesInfoFromUnt_Dvbssu (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
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
	HxLOG_Print("(-)local_dvbssuota_GetDesInfoFromUnt =========================\n\n");
	return ERR_OK;
}


#define _____SI_MSG_PROCESS_FUNCTION_____

STATIC void xsvc_si_SwupCheckMsgCmdStart_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_Dvbssu(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Dvbssu (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR		hErr;
	HUINT16		usPmtPid;
	HUINT32		ulActionId;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);

			hErr = svc_si_LibGetPmtPidFromPat(ulActionId, pstSwupCheck->usPmtSvcId,  &usPmtPid);
			if(hErr == ERR_OK)
			{
				HxLOG_Print ("svc_si_SwupCheckStartFilter (SvcID : 0x%x, PmtPid : 0X%x) \n", pstSwupCheck->usPmtSvcId, usPmtPid);

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
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR					 hErr;
	HUINT32					 ulIndex, ulNumSsu = 0;
	xsvcSi_SwupPsiInfo_t	*pstSwupInfoArray = NULL;
	HUINT32 				 ulExtId = SILIB_TABLE_EXT_ID_NOT;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:		// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Dvbssu(pstSwupCheck, &ulNumSsu, &pstSwupInfoArray);

			//HxLOG_Print("hErr(0x%x), ulNumSsu(0x%x), pstSwupInfoArray(0x%x)\n\n", hErr, ulNumSsu, pstSwupInfoArray);

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
					(void)xsvc_si_SwupCheckMakeUntExtIdByOui((HUINT16*)&ulExtId);
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Unt, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
					HxLOG_Print("[DVBSSU_SWUP] Retrieve UNT (PID 0x%04x), ulExtId(0x%08x) hErr (%d) \n", pstSwupInfoArray[ulIndex].usPid, ulExtId, hErr);
				}
				else
				{
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Dsi, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
					HxLOG_Print("[DVBSSU_SWUP] Retrieve DSI (PID 0x%04x)  hErr (%d) \n", pstSwupInfoArray[ulIndex].usPid, hErr);
				}
			}

			if(pstSwupInfoArray != NULL)
			{
				OxSI_Free (pstSwupInfoArray);
				pstSwupInfoArray = NULL;
			}

#if 0//학인 필요..
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
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtBatTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_HOME_TP:
			// NIT를 아직 받은 것이 아니라면 skip한다.
			if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) != TRUE)
			{
				return;
			}

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				// 현재 이 코드로 인해서 NIT에는 SWUP정보가 있더라도 BAT Table이 존재하지않으면 무조건 SWUP Fail남.  why?
				HxLOG_Info("BAT Table Time Out -- SWUP is Fail\n\r");
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, NULL, NULL);
			if (hErr != ERR_OK)
			{
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_SSU_SCAN, NULL, NULL);
				if (hErr != ERR_OK)
				{
					// 아무 정보도 없다 : SWUP Fail
					HxLOG_Info("BAT Table has no SWUP Information -- SWUP is Fail\n\r");
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}
			}

			// BAT 에 필요한 정보가 있다.
			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			break;

		case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtUntTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR				hErr;
	HUINT32				 ulNum;
	SvcSi_SwupSignal_t		*astSwupArray = NULL;
	SvcSi_OtaFailReason_e eFailReason = eSvcSi_OTA_FAILREASON_Unknown;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromUnt_Dvbssu(pstSwupCheck, &ulNum, &astSwupArray, &eFailReason);
			if(hErr == ERR_OK)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(SvcSi_SwupSignal_t));

				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

				return;
			}

			if (eFailReason != eSvcSi_OTA_FAILREASON_Unknown)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, eFailReason);
			}
			// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
			else if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, eFailReason);
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtDsiTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR					 hErr;
	HUINT16					 usPid;
	HUINT32 				 ulNum = 0;
	SvcSi_SwupSignal_t		*astSwupArray = NULL;
	SvcSi_OtaFailReason_e eFailReason = eSvcSi_OTA_FAILREASON_Unknown;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
#if 1//defined(USE_UNT)
		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
				if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
				{
					xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
					xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

					return;
				}
			}

			hErr = xsvc_si_SwupCheckFindSwupInfoFromDsi_Dvbssu(pstSwupCheck, &ulNum, &astSwupArray, &usPid, &eFailReason);
			if (hErr == ERR_OK && ulNum > 0 && astSwupArray != NULL)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(SvcSi_SwupSignal_t));

				OxSI_Free (astSwupArray);

				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			if (eFailReason != eSvcSi_OTA_FAILREASON_Unknown)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, eFailReason);
			}
			// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
			else if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, eFailReason);
			}
			break;


#else
		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = xsvc_si_SwupCheckFindSwupInfoFromDsi_Dvbssu(pstSwupCheck, &ulNum, &astSwupArray, &usPid);
			if (hErr == ERR_OK && ulNum > 0 && astSwupArray != NULL)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(SvcSi_SwupSignal_t));

				OxSI_Free (astSwupArray);

				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}
			else
			{
				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}
			break;
#endif

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}

	return;
}

STATIC void xsvc_si_SwupCheckProcessMsg_Dvbssu (SvcSi_Msg_t *pstMsg, void *pvData)
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
		HxLOG_Print("eSIMSG_CMD_START_OTACHECK!\n");
		xsvc_si_SwupCheckMsgCmdStart_Dvbssu(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("eSIMSG_CMD_STOP_OTACHECK!\n");
		xsvc_si_SwupCheckMsgCmdStop_Dvbssu(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("eSIMSG_EVT_PAT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtPatTable_Dvbssu (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("eSIMSG_EVT_PMT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtPmtTable_Dvbssu (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("eSIMSG_EVT_NIT_TABLE! (%s)\n", pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE? "NIT_ACT" : "NIT_OTH");
		xsvc_si_SwupCheckMsgEvtNitTable(pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		HxLOG_Print("eSIMSG_EVT_BAT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtBatTable_Dvbssu (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_UNT_TABLE:
		HxLOG_Print("eSIMSG_EVT_UNT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtUntTable_Dvbssu (pstSwupCheck, pstMsg);
		break;

	case eSIMSG_EVT_DSI_TABLE:
		HxLOG_Print("eSIMSG_EVT_DSI_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtDsiTable_Dvbssu (pstSwupCheck, pstMsg);
		break;

	default:
		HxLOG_Debug("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_Dvbssu (void)
{
	return xsvc_si_SwupCheckProcessMsg_Dvbssu;
}

void *xsvc_si_SwupCheckAllocData_Dvbssu (void)
{
	xsvcSi_SwupData_t		*pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_Dvbssu (void *pvData)
{
	xsvcSi_SwupData_t		*pstSwupCheck = (xsvcSi_SwupData_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
	}

	return;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_Dvbssu(Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e 		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL || ppvDataArray == NULL)
	{
		HxLOG_Error ("invalide parameter\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("eDataType: %s \n", xsvc_si_SwupCheckMakeStrType_Dvbssu(eDataType));
#endif

	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
		case eSI_SWUP_DATA_DELIVERY_TSINFO:
			hErr = xsvc_si_SwupCheckGetDataDeliveryTsInfo_Dvbssu(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_SCAN_TRIPLEID:
			hErr = xsvc_si_SwupCheckGetSsuScanTripleId_Dvbssu(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_LINK_TRIPLEID:
			hErr = xsvc_si_SwupCheckGetSsuLinkTripleId_Dvbssu(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_SIGNAL:
			hErr = xsvc_si_SwupCheckGetSwupSignalInfo_Dvbssu(hAction, pulDataNum, ppvDataArray);
			break;

		case eSI_SWUP_DATA_SSU_UPDATE_INFO:
		case eSI_SWUP_DATA_SSU_SCHEDULE_INFO:
		case eSI_SWUP_DATA_SSU_MESSAGE_INFO:
			hErr = xsvc_si_SwupCheckGetDesInfoFromUnt_Dvbssu(hAction, eDataType, pulDataNum, ppvDataArray);
			break;

		default:
			hErr = ERR_FAIL;
			break;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("%s -> hErr: %d \n", xsvc_si_SwupCheckMakeStrType_Dvbssu(eDataType) , hErr);
#endif

	return hErr;

}

HERROR xsvc_si_SwupCheckFreeSwupCheckData_Dvbssu (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
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

HERROR xsvc_si_SwupCheckGetFilteringTypes_Dvbssu(xsvcSi_SwupData_t *pstSwupCheck)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("ulSwupMode(%d) \n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	if(pstSwupCheck->stBase.bLiveSwupCheck== TRUE)
	{
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
	}
	else
	{
		switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
		{
		case eSI_SWUP_MODE_DVBSSU_HOME_TP:
		case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_BAT;
			if (pstSwupCheck->usNetIdForNitOth)
			{
				pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
			}
			else
			{
				pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_ACT;
			}
			break;

		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
			break;

		default:
			pstSwupCheck->stBase.eFiltering = 0;
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}

void xsvc_si_SwupCheckMsgEvtNitTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;
	SIxTable_Type_e	eSTableType;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("ulSwupMode (%s) \n" , xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if(pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE)
		eSTableType = eSIxTABLETYPE_NitAct;
	else
		eSTableType = eSIxTABLETYPE_NitOth;

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_HOME_TP:
			{
#if 0
				hErr = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
				if(hErr != ERR_OK)
				{
					xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

					return ;
				}
#endif
				// SSU-SCAN Linkage 정보를 찾는다.
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSTableType, eLINKAGE_SSU_SCAN, NULL, NULL);
				if (hErr != ERR_OK)
				{
					// SSU-SCAN Linkage 정보는 없지만 SSU-SVC 정보가 직접 있는가?
					hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSTableType, eLINKAGE_ALL, NULL, NULL);
					if (hErr != ERR_OK)
					{
						// BAT 를 받았는가? : 받지 않았다면 BAT 받을 때 까지 기다려야 한다.
						if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) != TRUE)
						{
							return ;
						}

						hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_ALL, NULL, NULL);
						if (hErr != ERR_OK)
						{
							hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSIxTABLETYPE_Bat, eLINKAGE_SSU_SCAN, NULL, NULL);
							if (hErr != ERR_OK)
							{
								// 아무 정보도 없다 : SWUP Fail
								svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
								return ;
							}
						}
					}
					// BAT 에 필요한 정보가 있다.
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}

				xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
			{
				// BAT 받거나 타임아웃 후에 NIT도 Timeout 이라면 Fail 이어야 한다.
				if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
				{
					if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
					{
						svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
						return ;
					}
				}
				// SSU-SVC 정보를 찾아야 한다.
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSTableType, eLINKAGE_ALL, NULL, NULL);
				if (hErr == ERR_OK)
				{
					// NIT 에서 SSU-SVC 를 찾았다면
					// BAT가 있건 없건 중요하지 않다. 그러므로 정지 시킨다.
					xsvc_si_SwupCheckStopSection_Dvbssu(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);

					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
					break;
				}

				// NIT에 SSU-SVC 정보가 없다 :
				// BAT를 이미 받은 상태라면 FAIL, 그렇지 않으면 BAT가 올 때까지 기다린다.
				if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_Dvbssu(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
			return;
	}

	return ;
}

HERROR 	xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Dvbssu(SIxLinkDes_t *pstLinkDes)
{
	return svc_si_SwupCheckFindProperOuiDataFromLinkDes(pstLinkDes);
}

HERROR xsvc_si_SwupCheckMakeUntExtIdByOui_Dvbssu(HUINT16	*pusExtId)
{
	return xsvc_si_LibSwupMakeUntExtIdByOui_Base(eSWUP_OUI_HUMAX, 1, pusExtId);
}

/* end of file */

