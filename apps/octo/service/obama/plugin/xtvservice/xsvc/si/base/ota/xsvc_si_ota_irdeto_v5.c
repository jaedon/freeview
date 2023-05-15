/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_irdeto_v5.c
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

#include <db_svc.h>
#include <db_param.h>

#include <svc_si.h>
#include <xsvc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>

//#include <si_ota.h>
//#include <si_service.h>

#include <svc_cas.h>
#include <svc_swup.h>

#include <svc_pipe.h>

#include <_svc_si_lib.h>
#include <_xsvc_si.h>
#include <_xsvc_si_lib_swup_base.h>
#include <_xsvc_si_swup_base.h>
#include <_xsvc_si_swup.h>

//#define ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE
#define ENABLE_IRDETO_SSU_OTA_TEST

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SWUP_IRDETO_LINKAGE_09	(0x09)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_SwupCheck_Base_t		 stBase;

	HUINT16						 usBatBouquetId;
	HUINT16						 usNetIdForNitOth;	// DVB-SSU OTA시 이 값이 0이 아니면 NIT OTH도 사용한다

	HBOOL						 bFound;

	XsvcSi_SwupSignal_Irdeto_t	 	stSwupInfo;
} xsvcSi_SwupData_Irdeto_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC XsvcSi_SwupCheckParam_Irdeto_t	s_stSwupCheckPramIrdeto = {0, };

#define ___LOCAL_FUNCTIONS___

#if defined(ENABLE_IRDETO_SSU_OTA_TEST)
STATIC HERROR xsvc_si_SwupSetDefaultData(XsvcSi_SwupCheckParam_Irdeto_t *pstSwCheckParam)
{
	if (pstSwCheckParam == NULL)
	{
		return ERR_FAIL;
	}

	pstSwCheckParam->usManufacturerId = 0x40;  // dec 64
	pstSwCheckParam->usHardwareVersion = 0x50; // dec 80
	pstSwCheckParam->usVariant = 0x64; // dec 100
	pstSwCheckParam->usSubVariant = 0x00;
	pstSwCheckParam->usDownloadSequenceNumber = 0x00;

	return ERR_OK;
}
#endif


STATIC HERROR xsvc_si_SwupCheckInitSwupData_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck)
{
	HxLOG_Trace();

	if (pstSwupCheck == NULL) { return ERR_FAIL; }

	pstSwupCheck->usBatBouquetId = eBOUQUET_ID_IRDETO;

	HxLOG_Debug("pstSwupCheck->stBase.stOption.ulSwupMode: %d \n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_IRDETO_HOME_TP:
		HxSTD_memset(&s_stSwupCheckPramIrdeto, 0x00, sizeof(XsvcSi_SwupCheckParam_Irdeto_t));
		break;

	case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
		HxLOG_Debug("eSI_SWUP_MODE_IRDETO_SSUSVC_TP, pstSwupCheck->stBase.stOption.usExtId1: 0x%x\n", pstSwupCheck->stBase.stSwupOption.usExtId1);
		pstSwupCheck->stBase.usPmtSvcId = pstSwupCheck->stBase.stSwupOption.usExtId1;
		break;

	default:
		break;
	}

	HxLOG_Debug("pstSwupCheck->usBatBouquetId: 0x%x, pstSwupCheck->usNetIdForNitOth: 0x%x, pstSwupCheck->stBase.usPmtSvcId: 0x%x\n", pstSwupCheck->usBatBouquetId, pstSwupCheck->stBase.usPmtSvcId);

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckGetFilteringTypes_Irdeto(xsvcSi_SwupData_Irdeto_t *pstSwupCheck)
{
	HxLOG_Trace();

	if (NULL == pstSwupCheck)
	{
		return ERR_FAIL;
	}

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_IRDETO_HOME_TP:
		HxLOG_Debug("eSI_SWUP_MODE_IRDETO_HOME_TP \n");
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT | bSI_FILTERING_BAT;

		if (pstSwupCheck->usNetIdForNitOth)
		{
			pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
		}
		break;

	case eSI_SWUP_MODE_IRDETO_SSUSCAN_TP:
		HxLOG_Debug("eSI_SWUP_MODE_IRDETO_SSUSCAN_TP \n");
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT | bSI_FILTERING_BAT;

		if (pstSwupCheck->usNetIdForNitOth)
		{
			pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
		}
		break;

	case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
		HxLOG_Debug("eSI_SWUP_MODE_IRDETO_SSUSVC_TP \n");
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
		break;

	default:
		HxLOG_Debug("not defined ulSwupMode Mode (%d)!  pstSwupCheck->stBase.eFiltering = 0 !!! \n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
		pstSwupCheck->stBase.eFiltering = 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId (xsvcSi_SwupData_Irdeto_t *pstSwupData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType	= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_NitOth:
		*peFilterType	= bSI_FILTERING_NIT_OTH;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstSwupData->usBatBouquetId;
		*pulPid			= BAT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckStartSection_Irdeto(xsvcSi_SwupData_Irdeto_t *pstSwupCheck)
{
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (NULL == pstSwupCheck)
	{
		return ERR_FAIL;
	}

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId (pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
		HxLOG_Debug("hErr: %d, pstSwupCheck->stBase.eFiltering (%d) & eFilterType(%d) : %d \n", hErr, pstSwupCheck->stBase.eFiltering, eFilterType, (pstSwupCheck->stBase.eFiltering & eFilterType));
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

HERROR xsvc_si_SwupCheckStopSection_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR			hErr = ERR_FAIL;

	hErr = svc_si_SwupCheckStopSection(&pstSwupCheck->stBase, eSiTableType, ulExtId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckStopSection() failed..! eSiTableType(%d) extId(0x%X) \n", eSiTableType, ulExtId);
	}

	return hErr;
}

/* please refer to "740992 5.0 Loader 5 Specification.pdf" page in 31 */
STATIC HERROR xsvc_si_SwupCheckGetLink09InfoFromLinkDes_Irdeto(SIxLinkDes_t *pstLinkDes, SIxLinkIr09Info_t **ppstLink09Info)
{
	HUINT8			*pucRaw;
	HUINT8			ucOuiLen;
	HUINT32			ulOui;
	HUINT8			ucSelLen;
	SIxLinkIr09Info_t	*pstLink09Info;

	if (pstLinkDes == NULL)
	{
		return ERR_FAIL;
	}

	if (pstLinkDes->ucLinkType != SWUP_IRDETO_LINKAGE_09)
	{
		return ERR_FAIL;
	}

	if (pstLinkDes->pstPrivateDataInfo == NULL)
	{
		return ERR_FAIL;
	}

	if (pstLinkDes->pstPrivateDataInfo->pucPrivateData == NULL)
	{
		return ERR_FAIL;
	}

	pucRaw = pstLinkDes->pstPrivateDataInfo->pucPrivateData;

	ucOuiLen = HxMACRO_Get8Bit(pucRaw); pucRaw++;
	while (ucOuiLen > 0)
	{
		ulOui = HxMACRO_Get24Bit(pucRaw);
		pucRaw += 3;
		if ( (ulOui == eSWUP_OUI_HUMAX) || (ulOui == eSWUP_OUI_DVB) )
		{
			ucSelLen = 	HxMACRO_Get8Bit(pucRaw); pucRaw++;
			if (ucSelLen >= 14)
			{
				pstLink09Info = (SIxLinkIr09Info_t *)OxSI_Malloc (sizeof(SIxLinkIr09Info_t));
				if (pstLink09Info == NULL)	{ return ERR_FAIL; }
				HxSTD_memset (pstLink09Info, 0, sizeof(SIxLinkIr09Info_t));

				pstLink09Info->usManufacturerId = HxMACRO_Get16Bit(pucRaw);
				pucRaw += 2;
				pstLink09Info->usHardwareVersion = HxMACRO_Get16Bit(pucRaw);
				pucRaw += 4; // HardwareVer(2) & reserved(2) field
				pstLink09Info->usVariant = HxMACRO_Get16Bit(pucRaw);
				pucRaw += 2;
				pstLink09Info->usSubVariant = HxMACRO_Get16Bit(pucRaw);
				pucRaw += 2;
				pstLink09Info->usDownloadSequenceNumber = HxMACRO_Get16Bit(pucRaw);
				pucRaw += 2;
				pstLink09Info->ucTiggerControlByte = HxMACRO_Get8Bit(pucRaw);
				pucRaw++;
				pstLink09Info->ucDownloadMode = HxMACRO_Get8Bit(pucRaw);

				*ppstLink09Info = pstLink09Info;
				return ERR_OK;
			}
			pucRaw += ucSelLen;
			ucOuiLen -= (3+1+ucSelLen);
		}
	}
	return ERR_FAIL;
}

STATIC void xsvc_sw_SwupCheckGetDefaultSwupData_Irdeto(XsvcSi_SwupCheckParam_Irdeto_t *pstDefSwupData)
{
	if (pstDefSwupData != NULL)
	{
		HxSTD_MemSet(pstDefSwupData, 0x00, sizeof(XsvcSi_SwupCheckParam_Irdeto_t));
		HxSTD_MemCopy(pstDefSwupData, &s_stSwupCheckPramIrdeto, sizeof(XsvcSi_SwupCheckParam_Irdeto_t));
	}
}

STATIC void xsvc_si_SwupCheckFreePsiInfo_Irdeto (void *pvData)
{
	xsvcSi_SwupPsiInfo_t	*pstSwupInfo = (xsvcSi_SwupPsiInfo_t *)pvData;

	if (NULL != pstSwupInfo)
	{
		OxSI_Free(pstSwupInfo);
	}

	return;
}

STATIC void	 xsvc_si_SwupCheckFreeListItem_Irdeto (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxSI_Free (pvItem);
	}
}

#define _____SWUP_PARSING_____
STATIC HERROR xsvc_si_SwupCheckFindProperOuiData_Irdeto (HUINT32 ulSelectorNum, svcSi_SwupCheck_SsuInfo_t *pstSelectors, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
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

STATIC HERROR xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_Irdeto (SIxDataBroadcastIdDes_t *pstDbcIdDes, xsvcSi_SwupPsiInfo_t *pstSwupInfo)
{
	HUINT32			 		ulSelectorNum =0 ;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;
	HERROR					 hErr;

	hErr = svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes(pstDbcIdDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes Error \n");
		return ERR_FAIL;
	}

	hErr = xsvc_si_SwupCheckFindProperOuiData_Irdeto(ulSelectorNum, pstOuiSelectors, pstSwupInfo);

	svc_si_SwupCheckFreeSsuInfo(pstOuiSelectors);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckParseCompability_Irdeto (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSvFound, HBOOL *pbSwFound, HUINT16 *pusDownloadSeqNum)
{
	XsvcSi_SwupCheckParam_Irdeto_t stDefSwupData;

	HUINT8		ucDesTag, ucDesLen;
	HUINT16		usSignalManufacturerId,	usSignalHardwareVersion;
	HUINT16		usSignalVariant, usSignalSubVariant;
	HUINT16		usSignalDownloadSequenceNumber;

	HUINT32		ulOui;
	HUINT8		*pucNextDesRaw;
	HERROR		hErr;

	xsvc_sw_SwupCheckGetDefaultSwupData_Irdeto(&stDefSwupData);

	ucDesTag = pucDesRaw[0];
	ucDesLen = pucDesRaw[1];
	pucDesRaw += 2;
	pucNextDesRaw = pucDesRaw + ucDesLen;

	switch (ucDesTag)
	{
	case 0x01:				// System Hardware Descriptor
		ulOui = HxMACRO_Get32Bit(pucDesRaw) & 0xffffff;			pucDesRaw += 4;
		usSignalManufacturerId = HxMACRO_Get16Bit(pucDesRaw);	pucDesRaw += 2;
		usSignalHardwareVersion = HxMACRO_Get16Bit(pucDesRaw);	pucDesRaw += 2;

		HxLOG_Print("[%s][%d] ulOui(0x%x) usManufacturerId(0x%x) usSignalHardwareVersion(0x%x) \n", __FUNCTION__, __LINE__, ulOui, usSignalManufacturerId, usSignalHardwareVersion);
		if ( (ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX) )
		{
			if ( (stDefSwupData.usManufacturerId == usSignalManufacturerId)
				&& (stDefSwupData.usHardwareVersion == usSignalHardwareVersion) )
			{
				if (ppucNextDesRaw != NULL)	{ *ppucNextDesRaw = pucNextDesRaw; }
				*pbHwFound = TRUE;
				return ERR_OK;
			}
		}
		break;

	case 0x02:				// System Software Descriptor
		ulOui = HxMACRO_Get32Bit (pucDesRaw) & 0xffffff;				pucDesRaw += 4;
		usSignalVariant = HxMACRO_Get16Bit(pucDesRaw);					pucDesRaw += 2;
		usSignalDownloadSequenceNumber = HxMACRO_Get16Bit(pucDesRaw);	pucDesRaw += 2;
		HxLOG_Print("[%s][%d] ulOui(0x%x) usSignalVariant(0x%x) usSignalDownloadSequenceNumber%x) \n", __FUNCTION__, __LINE__, ulOui, usSignalVariant, usSignalDownloadSequenceNumber);

		if ( (ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX) )
		{
			if (stDefSwupData.usVariant == usSignalVariant)
			{
				if ( (usSignalDownloadSequenceNumber != 0) &&
					 (usSignalDownloadSequenceNumber != 0xFFFF) &&
					 (usSignalDownloadSequenceNumber > stDefSwupData.usDownloadSequenceNumber) ) // higher version only
				{
					if (ppucNextDesRaw != NULL)	 { *ppucNextDesRaw = pucNextDesRaw; }
					*pbSwFound = TRUE;
					*pusDownloadSeqNum = usSignalDownloadSequenceNumber;
					return ERR_OK;
				}
			}
		}
		break;

	case 0x40:				// Pirvate Descriptor for IRDETO to get SubVariant
		ulOui = HxMACRO_Get32Bit (pucDesRaw) & 0xffffff;		pucDesRaw += 4;
		usSignalSubVariant = HxMACRO_Get16Bit(pucDesRaw);		pucDesRaw += 2;
		HxLOG_Print("[%s][%d] ulOui(0x%x) usSignalSubVariant(0x%x) \n", __FUNCTION__, __LINE__, ulOui, usSignalSubVariant);

		if ( (ulOui == eSWUP_OUI_DVB ) || (ulOui == eSWUP_OUI_HUMAX) )
		{
			if (stDefSwupData.usSubVariant == usSignalSubVariant)
			{
				if (ppucNextDesRaw != NULL)	 { *ppucNextDesRaw = pucNextDesRaw; }
				*pbSvFound = TRUE;
				return ERR_OK;
			}
		}
		break;
	}

	if (ppucNextDesRaw != NULL) 			{ *ppucNextDesRaw = pucNextDesRaw; }
	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT32 *pulNumSsu, xsvcSi_SwupPsiInfo_t **pastSwupSignals)
{
	HUINT32						ulActionId;
	HUINT32						ulIndex, ulCntSsuSvc, ulNumSsuSvc;
	HERROR						hErr = ERR_FAIL;
	SIxTable_PsiTable_t			*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pstPmt2ndLoop = NULL, *pst2ndDesLoop = NULL;
	SIxDataBroadcastIdDes_t		*pstDbcIdDes = NULL;
	xsvcSi_SwupPsiInfo_t		stIrdetoPsiInfo;
	xsvcSi_SwupPsiInfo_t		*pstIrdetoPsiInfo = NULL, *pstIrdetoPsiInfoArray = NULL;
	HxList_t					*pstList, *pstIrdetoPsiInfoList=NULL;

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
				HUINT8	*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

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
						hErr = xsvc_si_SwupCheckFindOuiDataFromDataBroadcastIdDes_Irdeto(pstDbcIdDes, &stIrdetoPsiInfo);
					#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
						if(hErr == ERR_OK)
					#else
						if(hErr == ERR_OK && stIrdetoPsiInfo.bUseUnt == FALSE) // only get simple profile ssu
					#endif
						{
							pstIrdetoPsiInfo = (xsvcSi_SwupPsiInfo_t *)OxSI_Malloc (sizeof(xsvcSi_SwupPsiInfo_t));
							if (pstIrdetoPsiInfo != NULL)
							{
								HxSTD_memset (pstIrdetoPsiInfo, 0, sizeof(xsvcSi_SwupPsiInfo_t));
								pstIrdetoPsiInfo->usSvcId		= pstPmtSec->usProgNum;
								pstIrdetoPsiInfo->usPid			= pstPmtElem->usElemPid;
								pstIrdetoPsiInfo->bUseUnt 		= stIrdetoPsiInfo.bUseUnt;
								pstIrdetoPsiInfo->ucUpdateVer 	= stIrdetoPsiInfo.ucUpdateVer;

								pstIrdetoPsiInfoList = HLIB_LIST_Append (pstIrdetoPsiInfoList, (void *)pstIrdetoPsiInfo);
							}
						}
					}
				}
			}
		}
	}

	ulNumSsuSvc = HLIB_LIST_Length(pstIrdetoPsiInfoList);
	if (ulNumSsuSvc == 0)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("ulNumSsuSvc is Zero \n");
		goto Error;
	}

	pstIrdetoPsiInfoArray = (xsvcSi_SwupPsiInfo_t *)OxSI_Malloc (sizeof(xsvcSi_SwupPsiInfo_t) * ulNumSsuSvc);
	if (pstIrdetoPsiInfoArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntSsuSvc = 0;
	for (ulIndex = 0; ulIndex < ulNumSsuSvc; ulIndex++)
	{
		pstList = HLIB_LIST_GetListItem(pstIrdetoPsiInfoList, ulIndex);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSsuSvc, ulIndex);
			continue;
		}

		pstIrdetoPsiInfo = (xsvcSi_SwupPsiInfo_t *)HLIB_LIST_Data(pstList);
		if (pstIrdetoPsiInfo == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", ulIndex);
			continue;
		}

		HxSTD_MemCopy(&pstIrdetoPsiInfoArray[ulCntSsuSvc], pstIrdetoPsiInfo, sizeof(xsvcSi_SwupPsiInfo_t));
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
		*pastSwupSignals = pstIrdetoPsiInfoArray;
		pstIrdetoPsiInfoArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (pstIrdetoPsiInfoArray != NULL)				{ OxSI_Free (pstIrdetoPsiInfoArray); }
	if (pstPmtTable != NULL)						{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if (pstIrdetoPsiInfoList != NULL)				{ HLIB_LIST_RemoveAllFunc (pstIrdetoPsiInfoList, xsvc_si_SwupCheckFreePsiInfo_Irdeto);}

	return hErr;
}

#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
STATIC HERROR xsvc_si_SwupCheckGetDsmccPidFromPmt_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT16 usAssociationTag, HUINT16 *pusPid)
{
	HUINT32					 ulActionId;
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL, *pstCurrPmt = NULL;
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

STATIC HERROR xsvc_si_SwupCheckSetDesInfoFromUnt_Irdeto (void  **ppvSsuDescriptorInfo, HUINT32 ulTag,  void **ppvSsuDescriptorRawInfo)
{
	// TODO: 구현 필요..
	HUINT16 					ulStrLen;
	HxDATETIME_t			stDateTime;

	MsgDes					*pstMessageDes;
	SchedulingDes				*pstScheduleDes;
	UpdateDes				*pstUpdateDes;
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

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckFindAssociationTagInOperationalDes(xsvcSi_SwupData_Irdeto_t *pstSwupCheck,HxList_t *pListOp, HUINT16 *pusAssociationTag)
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

STATIC HERROR xsvc_si_SwupCheckFindAssociationTagInCommonDes(xsvcSi_SwupData_Irdeto_t *pstSwupCheck,HxList_t *pListCom, HUINT16 *pusAssociationTag)
{
	HxList_t	*pstLoop = NULL;
	HUINT8 		*pucRawDes =  NULL;
	HUINT32		ulActionId;

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

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromUnt_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **pastSwupSignals)
{
	XsvcSi_SwupCheckParam_Irdeto_t stDefSwupData;

	HBOOL					bHwFound, bSvFound, bSwFound;
	HUINT16					usDownloadSeqNum;
	HUINT32					ulActionId, ulSystemId;
	SIxTable_PsiTable_t		*pstUntTable = NULL, *pstCurrUnt = NULL;
	SIxTable_UntSec_t		*pstUntSecInfo = NULL;
	HxList_t				*pst2ndLoop = NULL, *pHxList = NULL, *pstSwupList = NULL;
	HUINT32					ulIndex, ulNumSsu;
	HUINT8					*pucRaw = NULL;
	HUINT16					usPid;
	XsvcSi_SwupSignal_Irdeto_t	*pstSwupInfo = NULL, *astSwupArray = NULL;
	HERROR					hErr;

	xsvc_sw_SwupCheckGetDefaultSwupData_Irdeto(&stDefSwupData);

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
			SIxTable_UntInfo_t	*pstUntInfo = NULL;
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

				xsvc_si_SwupCheckParseCompability_Irdeto(pucRaw, NULL, &bHwFound, &bSvFound, &bSwFound, &usDownloadSeqNum);
			}

			if (bHwFound == TRUE && bSvFound == TRUE && bSwFound == TRUE)
			{
				hErr = xsvc_si_SwupCheckFindAssociationTagInOperationalDes(pstSwupCheck, pstUntInfo->pstOperationalDesLoop, &usAssociationTag);
				if (hErr == ERR_FAIL)
				{
					hErr = xsvc_si_SwupCheckFindAssociationTagInCommonDes(pstSwupCheck, pstUntInfo->pstCompDesLoop, &usAssociationTag);
				}

				if (hErr == ERR_OK)
				{
					hErr = xsvc_si_SwupCheckGetDsmccPidFromPmt_Irdeto(pstSwupCheck, usAssociationTag, &usPid);
					if (hErr == ERR_OK)
					{
						pstSwupInfo = (XsvcSi_SwupSignal_Irdeto_t *)OxSI_Malloc (sizeof(XsvcSi_SwupSignal_Irdeto_t));
						if (pstSwupInfo != NULL)
						{
							HxSTD_memset (pstSwupInfo, 0, sizeof(XsvcSi_SwupSignal_Irdeto_t));

							pstSwupInfo->usManufacturerId			= stDefSwupData.usManufacturerId;
							pstSwupInfo->usHardwareVersion			= stDefSwupData.usHardwareVersion;
							pstSwupInfo->usVariant 					= stDefSwupData.usVariant;
							pstSwupInfo->usSubVariant				= stDefSwupData.usSubVariant;
							pstSwupInfo->usDownloadSequenceNumber	= usDownloadSeqNum;
							pstSwupInfo->usPid						= usPid;

							xsvc_si_SwupCheckGetSwupDownType(&pstSwupInfo->ucSwupType);
							svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSwupInfo->usDataType, &(pstSwupInfo->eDataType));
							pstSwupInfo->ulDataVersion = usDownloadSeqNum;

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
	if (pstSwupList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstSwupList, xsvc_si_SwupCheckFreeListItem_Irdeto); }

	return hErr;
}
#endif

STATIC HERROR xsvc_si_SwupCheckFindSwupInfoFromDsi_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT32 *pulNumSsu, XsvcSi_SwupSignal_Irdeto_t **pastSwupSignals, HUINT16 *pusPid)
{
	XsvcSi_SwupCheckParam_Irdeto_t stDefSwupData;

	HBOOL						bHwFound, bSvFound, bSwFound;
	HUINT16						usDownloadSeqNum;
	HUINT32				 		ulNumSsu;

	XsvcSi_SwupSignal_Irdeto_t 	*pstSwupInfo;
	XsvcSi_SwupSignal_Irdeto_t 	*astSwupArray;
	HxList_t						*pstSwupList;

	HERROR				 		hErr;
	HUINT32				 		ulActionId;
	SIxTable_PsiTable_t			*pstDsiTable = NULL, *pstCurrDsi = NULL;
	HxList_t						*pstDsi2ndLoop = NULL;
	HUINT32				 		ulListLen, ulIndex;
	HUINT8						*pucRaw;
	HxList_t						*pHxList = NULL;

	xsvc_sw_SwupCheckGetDefaultSwupData_Irdeto(&stDefSwupData);

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
		SIxTable_DsiSec_t *pstDsiSec = &pstCurrDsi->unInfo.stDsi;

		for (pstDsi2ndLoop = pstCurrDsi->pst2ndLoop; NULL != pstDsi2ndLoop; pstDsi2ndLoop = pstDsi2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstDsi2ndLoop);
			SIxTable_DsiGroup_t		*pstDsiGroup;

			if (NULL == pst2ndItem) { continue; }

			pstDsiGroup = (SIxTable_DsiGroup_t*)&(pst2ndItem->unInfo.stDsi);

			ulListLen = HLIB_LIST_Length(pstDsiGroup->pstGroupCompList);

			pHxList = pstDsiGroup->pstGroupCompList;

			bHwFound = FALSE;
			bSvFound = FALSE;
			bSwFound = FALSE;

			HxLOG_Print("ulListLen : %d\n", ulListLen);

			for(ulIndex= 0 ; ulIndex<ulListLen; ulIndex++)
			{
				pucRaw = (HUINT8*)HLIB_LIST_Data(pHxList);
				if(pucRaw == NULL)
				{
					HxLOG_Error ("pucRaw is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}

				xsvc_si_SwupCheckParseCompability_Irdeto(pucRaw, NULL, &bHwFound, &bSvFound, &bSwFound, &usDownloadSeqNum);

				pHxList = HLIB_LIST_Next(pHxList);
				if(pHxList == NULL)
				{
					HxLOG_Error ("pHxList is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}
			}

			if (bHwFound == TRUE && bSvFound == TRUE && bSwFound == TRUE)
			{
				HxLOG_Print ("Found SWUP H/W Ver & S/W Ver.!! \n");

				pstSwupInfo = (XsvcSi_SwupSignal_Irdeto_t *)OxSI_Malloc (sizeof(XsvcSi_SwupSignal_Irdeto_t));
				if (pstSwupInfo != NULL)
				{
					HxSTD_memset (pstSwupInfo, 0, sizeof(XsvcSi_SwupSignal_Irdeto_t));

					pstSwupInfo->usManufacturerId			= stDefSwupData.usManufacturerId;
					pstSwupInfo->usHardwareVersion			= stDefSwupData.usHardwareVersion;
					pstSwupInfo->usVariant 					= stDefSwupData.usVariant;
					pstSwupInfo->usSubVariant				= stDefSwupData.usSubVariant;
					pstSwupInfo->usDownloadSequenceNumber	= usDownloadSeqNum;
					pstSwupInfo->usPid						= pstDsiSec->usExtId;

					HxLOG_Debug("Found SWUP [usManufacturerId : 0x%x, usHardwareVersion : 0x%x, usDownloadSequenceNumber : 0x%x, usPid : 0x%x] \n",
									pstSwupInfo->usManufacturerId, pstSwupInfo->usHardwareVersion, pstSwupInfo->usDownloadSequenceNumber, pstSwupInfo->usPid);

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
		astSwupArray = (XsvcSi_SwupSignal_Irdeto_t *)OxSI_Malloc (sizeof(XsvcSi_SwupSignal_Irdeto_t) * ulNumSsu);
		if (astSwupArray == NULL)
		{
			hErr = ERR_FAIL;
			goto Error;
		}

		for(ulIndex = 0; ulIndex < ulNumSsu; ulIndex++)
		{
			pHxList = HLIB_LIST_GetListItem(pstSwupList, ulIndex);

			pstSwupInfo = (XsvcSi_SwupSignal_Irdeto_t *)HLIB_LIST_Data(pHxList);
			if (pstSwupInfo == NULL)
			{
				HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", ulIndex);
				continue;
			}

			HxSTD_MemCopy(&astSwupArray[ulIndex], pstSwupInfo, sizeof(XsvcSi_SwupSignal_Irdeto_t));
		}
		*pastSwupSignals = astSwupArray;
	}

	*pulNumSsu = ulNumSsu;

	hErr = ERR_OK;

Error:

	if (pstDsiTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstDsiTable); }
	if (pstSwupList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstSwupList, xsvc_si_SwupCheckFreeListItem_Irdeto); }

	return hErr;
}


#define ___LOCAL_SWUP_INFO_FUNCTIONS___
// API를 통해 SWUP Signalling 결과를 알려주기 위한 함수들
// NIT를 통해 얻은 Delivery TP Info들
STATIC HERROR xsvc_si_SwupCheckGetDataTsDeliveryInfo_Irdeto(Handle_t hAction, HUINT32 *pulNumTs, void **ppvTsInfoArray)
{
	HUINT32			 ulActionId;
	HERROR			 hErr = ERR_FAIL;
	HERROR			 hResult = ERR_FAIL;
	HUINT32			 ulNitActTsNum = 0, ulNitOthTsNum = 0;
	DbSvc_TsInfo_t	*pstNitActTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitOthTsInfo = NULL;
	DbSvc_TsInfo_t	*pstNitTsInfo = NULL;


	if ((pulNumTs == NULL) || (ppvTsInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	*pulNumTs = 0;
	*ppvTsInfoArray = NULL;

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
	*ppvTsInfoArray = pstNitTsInfo;
	hResult = ERR_OK;

Exit_Func:
	if (pstNitActTsInfo != NULL)			{ OxSI_Free (pstNitActTsInfo); }
	if (pstNitOthTsInfo != NULL)			{ OxSI_Free (pstNitOthTsInfo); }

	return hResult;
}

STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(Handle_t hAction, SIxTable_Type_e eSiType, HUINT32 *pulNumInfo, void **ppvInfoArray, XsvcSi_SwupDataType_e eDataType)
{
	XsvcSi_SwupCheckParam_Irdeto_t stDefSwupData;

	SIxTable_PsiTable_t 	*pstTable = NULL;
	HxList_t				*pstElemItem = NULL;
	SIxLinkIr09Info_t		*pstLinkIr09Info = NULL;
	SIxLinkDes_t			*pstLinkDes = NULL;

	HBOOL					bFoundFlag = FALSE;
	HUINT8					*pucCtrlByte = NULL;
	DbSvc_TripleId_t		*pstTripleIdArray = NULL;

	HERROR					hErr = ERR_FAIL;
	HUINT32					ulActionId;

	HxLOG_Trace();

	if ((pulNumInfo == NULL) || (ppvInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	xsvc_sw_SwupCheckGetDefaultSwupData_Irdeto(&stDefSwupData);

	*pulNumInfo = 0;
	*ppvInfoArray = NULL;

	HxLOG_Print ("xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte : hAction(0x%x),  eSiType(%d)\n", hAction, eSiType);

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSiType, eBOUQUET_ID_IRDETO);
	if(pstTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d) \n", ulActionId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if(pstTable->pstDesLoop == NULL)
	{
		HxLOG_Error ("pstTable->pstDesLoop == NULL \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Info ("[System Information]\n");
	HxLOG_Info ("manufacturer_id\t = 0x%x\n", stDefSwupData.usManufacturerId);
	HxLOG_Info ("hardware_version\t\t = 0x%x\n", stDefSwupData.usHardwareVersion);
	HxLOG_Info ("variant\t = 0x%x\n", stDefSwupData.usVariant);
	HxLOG_Info ("Sub variant\t = 0x%x\n", stDefSwupData.usSubVariant);
	HxLOG_Info ("download_sequence_number\t = 0x%x\n", stDefSwupData.usDownloadSequenceNumber);
	HxLOG_Info ("\t =================\n");
#endif

	// Should search 0x90 .
	for (pstElemItem = pstTable->pstDesLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		HUINT8	*pucDesRaw = HLIB_LIST_Data (pstElemItem);

		HxLOG_Info ("start 1st loop desc parsing \n");

		if (NULL == pucDesRaw)
		{
			HxLOG_Error ("NULL == pucDesRaw, continue \n");
			continue;
		}

		if(pstLinkDes)
		{
			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
			pstLinkDes = NULL;
		}

		if(pstLinkIr09Info)
		{
			OxSI_Free(pstLinkIr09Info);
			pstLinkIr09Info = NULL;
		}

		switch (*pucDesRaw)
		{
			case eSIxDESCTAG_LINKAGE:
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
				break;
		}

		if(pstLinkDes)
		{
			HxLOG_Print("pstLinkDes exists, pstLinkDes->ucLinkType: 0x%x \n", pstLinkDes->ucLinkType);

			/*Find linkage_descriptor with linkage_type 0x09*/
			if (pstLinkDes->ucLinkType == 0x09)
			{
				hErr = xsvc_si_SwupCheckGetLink09InfoFromLinkDes_Irdeto (pstLinkDes, &pstLinkIr09Info);
				if (hErr != ERR_OK || pstLinkIr09Info == NULL)
				{
					HxLOG_Error("xsvc_si_SwupCheckGetLink09InfoFromLinkDes_Irdeto fail...\n");
					continue;
				}

		#if defined(CONFIG_DEBUG)
				HxLOG_Info ("[Descriptor Information]\n");
				HxLOG_Info ("manufacturer_id\t = 0x%x\n", pstLinkIr09Info->usManufacturerId);
				HxLOG_Info ("hardware_code\t\t = 0x%x\n", pstLinkIr09Info->usHardwareVersion);
				HxLOG_Info ("variant\t = 0x%x\n", pstLinkIr09Info->usVariant);
				HxLOG_Info ("sub_variant\t = 0x%x\n", pstLinkIr09Info->usSubVariant);
				HxLOG_Info ("download_sequence_number\t = 0x%x\n", pstLinkIr09Info->usDownloadSequenceNumber);
				HxLOG_Info ("tiggrt_control_byte\t = 0x%x\n", pstLinkIr09Info->ucTiggerControlByte);
				HxLOG_Info ("download_mode\t = 0x%x\n", pstLinkIr09Info->ucDownloadMode);
		#endif

				if (stDefSwupData.usManufacturerId == pstLinkIr09Info->usManufacturerId)
				{
					if ((stDefSwupData.usHardwareVersion == pstLinkIr09Info->usHardwareVersion) &&
						(stDefSwupData.usVariant == pstLinkIr09Info->usVariant) &&
						(stDefSwupData.usSubVariant == pstLinkIr09Info->usSubVariant))
					{
						if ((pstLinkIr09Info->usDownloadSequenceNumber == 0) ||
							(pstLinkIr09Info->usDownloadSequenceNumber == 0xFFFF) ||
							(pstLinkIr09Info->usDownloadSequenceNumber == stDefSwupData.usDownloadSequenceNumber))
						{
							OxSI_Free(pstLinkIr09Info);
							pstLinkIr09Info = NULL;
							continue;
						}

						if(eDataType == eSI_SWUP_DATA_SSU_LINK_TRIPLEID)
						{
							pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t));
							if (pstTripleIdArray == NULL)
							{
								HxLOG_Error("Alloc fail for pstTripleIdArray\n");
								hErr = ERR_FAIL;
								goto END_FUNC;
							}
							pstTripleIdArray->usOnId	= pstLinkDes->usOrgNetId;
							pstTripleIdArray->usTsId	= pstLinkDes->usTsId;
							pstTripleIdArray->usSvcId	= pstLinkDes->usSvcId;
						}
						else if(eDataType == eSI_SWUP_DATA_IRDETO_CTRL_BYTE)
						{
							pucCtrlByte = (HUINT8 *)OxSI_Malloc (sizeof(HUINT8));
							if (pucCtrlByte == NULL)
							{
								HxLOG_Error("Alloc fail for pucCtrlByte\n");
								hErr = ERR_FAIL;
								goto END_FUNC;
							}
							*pucCtrlByte = pstLinkIr09Info->ucTiggerControlByte;
						}

						bFoundFlag = TRUE;
						HxLOG_Info ("LinkType (0x%x), ControlByte (0x%x), DownloadMode (0x%x), usOnId (0x%x), usTsId (0x%x), usSvcId (0x%x)\n", \
										pstLinkDes->ucLinkType, pstLinkIr09Info->ucTiggerControlByte, pstLinkIr09Info->ucDownloadMode, 	\
										pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
					}
				}

				if(bFoundFlag == TRUE)
				{
					HxLOG_Error("SWUP flag exist in linkType 0x09. So break! \n");
					hErr = ERR_OK;
					break;
				}
			}
			else
			{
				HxLOG_Error("LinkType is NOT 0x09\n");
				continue;
			}
		}
		else
		{
			HxLOG_Error(" pstLinkDes doesn't exists [%d]\n", __LINE__);
			continue;
		}
	}

END_FUNC:

	if(NULL != pstLinkIr09Info)
	{
		OxSI_Free(pstLinkIr09Info);
		pstLinkIr09Info = NULL;
	}

	if(NULL != pstLinkDes)
	{
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
		pstLinkDes = NULL;
	}

	if(NULL != pstTable)
	{
		PAL_SIRAWPOOL_FreeTable (pstTable);
		pstTable = NULL;
	}

	if(bFoundFlag)
	{
		if(eDataType == eSI_SWUP_DATA_SSU_LINK_TRIPLEID)
		{
			HxLOG_Debug("Get Triple ID \n");
			*ppvInfoArray = pstTripleIdArray;
			*pulNumInfo = 1;
			pstTripleIdArray = NULL;
		}
		else if(eDataType == eSI_SWUP_DATA_IRDETO_CTRL_BYTE)
		{
			HxLOG_Debug("Get TiggerControlByte \n");
			*ppvInfoArray = pucCtrlByte;
			*pulNumInfo = 1;
			pucCtrlByte = NULL;
		}
	}
	else
	{
		HxLOG_Info("[%s] leave. hErr: %d, Cannot find any information. \n", __FUNCTION__);
		hErr = ERR_FAIL;
	}

	if(pstTripleIdArray != NULL)
	{
		OxSI_Free(pstTripleIdArray);
		pstTripleIdArray = NULL;
	}

	if(pucCtrlByte != NULL)
	{
		OxSI_Free(pucCtrlByte);
		pucCtrlByte = NULL;
	}
	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo(Handle_t hAction, HUINT32 *pulNumInfo , void **apvInfoArray)
{
	HUINT32						ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupData_Irdeto_t	*pstSwupSignalInfo = (xsvcSi_SwupData_Irdeto_t *)svc_si_GetSiData(ulActionId);
	XsvcSi_SwupSignal_Irdeto_t	*pstSwupInfo;

	if ((pulNumInfo == NULL) || (apvInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter \n");
		return ERR_FAIL;
	}

	*pulNumInfo = 0;
	*apvInfoArray = NULL;

	if(pstSwupSignalInfo->bFound != TRUE)
	{
		HxLOG_Error("pstSwupSignalInfo->bFound: %d \n", pstSwupSignalInfo->bFound);
		return ERR_FAIL;
	}

	*pulNumInfo = 1;

	pstSwupInfo = (XsvcSi_SwupSignal_Irdeto_t *)OxSI_Malloc (sizeof(XsvcSi_SwupSignal_Irdeto_t));
	if (pstSwupInfo == NULL)
	{
		HxLOG_Error("Alloc Fail \n");
		return ERR_FAIL;
	}

	*pstSwupInfo = pstSwupSignalInfo->stSwupInfo;
	*apvInfoArray = (void *)pstSwupInfo;

	return ERR_OK;
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SwupCheckMsgCmdStart_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_Irdeto(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_Irdeto(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Irdeto (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR		hErr;
	HUINT16		usPmtPid;
	HUINT32		ulActionId;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			ulActionId = PAL_PIPE_GetActionId (pstSwupCheck->stBase.hAction);

			hErr = svc_si_LibGetPmtPidFromPat(ulActionId, pstSwupCheck->stBase.usPmtSvcId, &usPmtPid);
			if(hErr == ERR_OK)
			{
				HxLOG_Print ("svc_si_SwupCheckStartFilter (SvcID : 0x%x, PmtPid : 0X%x) \n", pstSwupCheck->stBase.usPmtSvcId, usPmtPid);

				hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Pmt, (HUINT32)pstSwupCheck->stBase.usPmtSvcId, (HUINT32)usPmtPid);
				if (ERR_OK != hErr)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}
			}
			break;

		default:
			return;
	}
	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR				 hErr;
	HUINT32				 ulIndex, ulNumSsu = 0;
	xsvcSi_SwupPsiInfo_t	*pstSwupInfoArray = NULL;
	HUINT32 				 ulExtId = SILIB_TABLE_EXT_ID_NOT;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:		// Timeout Check
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto(pstSwupCheck, &ulNumSsu, &pstSwupInfoArray);

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
			#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
				if (pstSwupInfoArray[ulIndex].bUseUnt == TRUE)
				{

					(void)xsvc_si_SwupCheckMakeUntExtIdByOui((HUINT16*)&ulExtId);
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Unt, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
					HxLOG_Print("[IRDETO_SWUP] Retrieve UNT (PID 0x%04x), ulExtId(0x%08x) hErr (%d) \n", pstSwupInfoArray[ulIndex].usPid, ulExtId, hErr);
				}
				else
			#endif
				{
					hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Dsi, (HUINT32)ulExtId, (HUINT32)pstSwupInfoArray[ulIndex].usPid);
					HxLOG_Print("[IRDETO_SWUP] Retrieve DSI (PID 0x%04x)  hErr (%d) \n", pstSwupInfoArray[ulIndex].usPid, hErr);
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
			return;
	}
	return;
}
STATIC void xsvc_si_SwupCheckMsgEvtNitTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HBOOL 	bNitAct;
	HxLOG_Debug("+++ \n");

	bNitAct = (pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE) ? TRUE : FALSE;
	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch(pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_IRDETO_HOME_TP:
			{
				DxOperator_e		eOperatorType;
				HERROR				hErr;

				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
				if (ERR_OK != hErr)
				{
					HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
				}

				HxLOG_Print("[%s:%d] eOperatorType: %d \n", __FUNCTION__, __LINE__, eOperatorType);
				HxLOG_Print("[%s:%d] bNitAct(%d) pstMsg->unArg.stSection.ulTableStatus: %d \n", __FUNCTION__, __LINE__, bNitAct, pstMsg->unArg.stSection.ulTableStatus);

				// Timeout Check
				if (eOperatorType == eDxOPERATOR_Ziggo)
				{
					// Waithing All Recevied
					if ((bNitAct == FALSE) && (pstMsg->unArg.stSection.ulTableStatus != eSI_TABLE_TIMEOUT))
					{
						// NIT Other 를 받으면 NIT ACT는 무시
						xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
					}
				}
				else
				{
					if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
					{
						// BAT가 있건 없건 중요하지 않다. 그러므로 정지 시킨다.
						xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Bat, SILIB_TABLE_EXT_ID_ALL);
						svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
						return;
					}
				}

				// BAT 도 받았는지 확인
				if (svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase) == TRUE)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return;
				}
			}
			break;

		case eSI_SWUP_MODE_IRDETO_SSUSCAN_TP:	// NOT USED for Irdeto SWUP !!! - mhoh
			// BAT 받거나 타임아웃 후에 NIT도 Timeout 이라면 Fail 이어야 한다.
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				if (svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase) == TRUE)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return;
				}
			}
			svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			return;

		default:
			break;
	}

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtBatTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_IRDETO_HOME_TP)
	{
		return;
	}

	if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
	{
		// NIT가 있건 없건 중요하지 않다. 그러므로 정지 시킨다.
		xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);

		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return;
	}

	// NIT 도 받았는지 확인
	if (svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase) == TRUE)
	{
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
	}

	return;
}

#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
STATIC void xsvc_si_SwupCheckMsgEvtUntTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR				hErr;
	HUINT32				ulNum;
	SvcSi_SwupSignal_t	*astSwupArray = NULL;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			hErr = xsvc_si_SwupCheckFindSsuInfoFromUnt_Irdeto(pstSwupCheck, &ulNum, &astSwupArray);
			if(hErr == ERR_OK)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(SvcSi_SwupSignal_t));

				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}

			// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
			if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
			{
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		default:
			return;
	}

	return;
}
#endif

STATIC void xsvc_si_SwupCheckMsgEvtDsiTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;
	HUINT16	usPid;
	HUINT32 ulNum = 0;
	XsvcSi_SwupSignal_Irdeto_t	*astSwupArray = NULL;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("pstSwupCheck is NULL\n");
		return ;
	}

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
			{
				// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
				if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
				{
					xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
				#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
					xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
				#endif

					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return;
				}
			}

			hErr = xsvc_si_SwupCheckFindSwupInfoFromDsi_Irdeto(pstSwupCheck, &ulNum, &astSwupArray, &usPid);
			if (hErr == ERR_OK && ulNum > 0 && astSwupArray != NULL)
			{
				// SWUP 정보를 기록해 놓는다.
				pstSwupCheck->bFound = TRUE;

				HxSTD_memcpy(&pstSwupCheck->stSwupInfo, astSwupArray, sizeof(XsvcSi_SwupSignal_Irdeto_t));

				OxSI_Free (astSwupArray);

				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
			#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
			#endif

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return ;
			}

			// UNT, DSI가 여러개 걸릴수도 있어서 확인하는 부분이 필요하여 추가함.
			if (svc_si_SwupCheckCheckAllSiTableReceived (&pstSwupCheck->stBase) == TRUE)
			{
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
			#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
				xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Unt, SILIB_TABLE_EXT_ID_ALL);
			#endif

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		default:
			return;
	}

	return;
}

STATIC void xsvc_si_SwupCheckProcessMsg_Irdeto (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupData_Irdeto_t *pstSwupData = (xsvcSi_SwupData_Irdeto_t *)pvData;

	if (NULL == pstSwupData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		HxLOG_Print("eSIMSG_CMD_START_OTACHECK!\n");
		xsvc_si_SwupCheckMsgCmdStart_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("eSIMSG_CMD_STOP_OTACHECK!\n");
		xsvc_si_SwupCheckMsgCmdStop_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("eSIMSG_EVT_PAT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtPatTable_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("eSIMSG_EVT_PMT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtPmtTable_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		HxLOG_Print("eSIMSG_EVT_NIT_TABLE! (%s)\n", pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE? "NIT_ACT" : "NIT_OTH");
		xsvc_si_SwupCheckMsgEvtNitTable_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		HxLOG_Print("eSIMSG_EVT_BAT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtBatTable_Irdeto(pstSwupData, pstMsg);
		break;

#if defined(ENABLE_IRDETO_SSU_OTA_ENHANCED_PROFILE)
	case eSIMSG_EVT_UNT_TABLE:
		HxLOG_Print("eSIMSG_EVT_UNT_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtUntTable_Irdeto(pstSwupData, pstMsg);
		break;
#endif

	case eSIMSG_EVT_DSI_TABLE:
		HxLOG_Print("eSIMSG_EVT_DSI_TABLE!\n");
		xsvc_si_SwupCheckMsgEvtDsiTable_Irdeto(pstSwupData, pstMsg);
		break;

	default:
		HxLOG_Debug("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___


#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_Irdeto (void)
{
	return xsvc_si_SwupCheckProcessMsg_Irdeto;
}

void *xsvc_si_SwupCheckAllocData_Irdeto (void)
{
	xsvcSi_SwupData_Irdeto_t *pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_Irdeto_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_Irdeto_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_Irdeto (void *pvData)
{
	xsvcSi_SwupData_Irdeto_t *pstSwupCheck = (xsvcSi_SwupData_Irdeto_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
	}

	return;
}

HERROR xsvc_si_SwupCheckFlag_Irdeto (Handle_t hAction, HUINT32 *pulSwupCheckFlag)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 ulNumInfo = 0;
	HUINT8	*pucControlByte;

#if defined(ENABLE_IRDETO_SSU_OTA_TEST)
	xsvc_si_SwupSetDefaultData(&s_stSwupCheckPramIrdeto);
#endif

	hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(hAction, eSIxTABLETYPE_Bat, &ulNumInfo, (void **)&pucControlByte, eSI_SWUP_DATA_IRDETO_CTRL_BYTE);
	if (hErr != ERR_OK)
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(hAction, eSIxTABLETYPE_NitAct, &ulNumInfo, (void **)&pucControlByte, eSI_SWUP_DATA_IRDETO_CTRL_BYTE);
	if (hErr == ERR_OK)
	{
		HxLOG_Debug("*pucControlByte: %d \n", *pucControlByte);
		*pulSwupCheckFlag = (HUINT32)*pucControlByte;
		HxLOG_Debug("*pulSwupCheckFlag: %d \n", *pulSwupCheckFlag);
	}
	else
	{
		HxLOG_Error("cannot find Control byte. \n");
	}

	return hErr;
}

HERROR xsvc_si_SwupCheckSetSwupCheckParam_Irdeto (SvcSi_SwupFlag_t *pstSwupCheckParam)
{
	if (NULL == pstSwupCheckParam)
		return ERR_FAIL;

	if (0 == pstSwupCheckParam->ulSize || NULL == pstSwupCheckParam->pvData)
		return ERR_FAIL;

	HxSTD_MemCopy(&s_stSwupCheckPramIrdeto, pstSwupCheckParam->pvData, pstSwupCheckParam->ulSize);

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_Irdeto(Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e 		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL)		{ return ERR_FAIL; }
	if (ppvDataArray == NULL)	{ return ERR_FAIL; }

	HxLOG_Print("eDataType: %d \n", eDataType);
	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
	case eSI_SWUP_DATA_DELIVERY_TSINFO:
		hErr = xsvc_si_SwupCheckGetDataTsDeliveryInfo_Irdeto(hAction, pulDataNum, ppvDataArray);
		break;

	case eSI_SWUP_DATA_SSU_SCAN_TRIPLEID:
		// NOT USED 라고 되어 있어 구현하지 않음
		break;

	case eSI_SWUP_DATA_SSU_LINK_TRIPLEID:
	case eSI_SWUP_DATA_IRDETO_CTRL_BYTE:
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(hAction, eSIxTABLETYPE_Bat, pulDataNum, ppvDataArray, eDataType);
		if (hErr != ERR_OK)
			hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(hAction, eSIxTABLETYPE_NitAct, pulDataNum, ppvDataArray, eDataType);
		break;

	case eSI_SWUP_DATA_SSU_SIGNAL:
		hErr = xsvc_si_SwupCheckGetSwupSignalInfo(hAction, pulDataNum, ppvDataArray);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	HxLOG_Print("hErr: %d \n", hErr);

	return hErr;
}

HERROR xsvc_si_SwupCheckFreeSwupCheckData_Irdeto (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
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

HERROR xsvc_si_SwupCheckMakeUntExtIdByOui_Irdeto (HUINT16	*pusExtId)
{
	return xsvc_si_LibSwupMakeUntExtIdByOui_Base(eSWUP_OUI_HUMAX, 1, pusExtId);
}

/* end of file */

