/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_irdeto.c
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


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define IRDETO_PMT_STREAMTYPE_80		0x80
#define IRDETO_PMT_STREAMTYPE_82		0x82
#define IRDETO_PMT_STREAMTYPE_84		0x84

#define SWUP_IRDETO_BAT_LINKAGE_82		0x82
#define SWUP_IRDETO_BAT_LINKAGE_80		0x80

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_SwupCheck_Base_t		 stBase;

	HUINT16						 usBatBouquetId;
	HUINT16						 usNetIdForNitOth;	// DVB-SSU OTA시 이 값이 0이 아니면 NIT OTH도 사용한다

	HBOOL						 bFound;
	XsvcSi_SwupSignal_Irdeto_t	 stSwupInfo;
} xsvcSi_SwupData_Irdeto_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC XsvcSi_SwupCheckParam_Irdeto_t	s_stSwupCheckPramIrdeto;


#define ___LOCAL_FUNCTIONS___
STATIC HERROR xsvc_si_SwupCheckInitSwupData_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck)
{
	HxLOG_Trace();

	if (pstSwupCheck == NULL)				{ return ERR_FAIL; }

	pstSwupCheck->usBatBouquetId 	= eBOUQUET_ID_IRDETO;

	HxLOG_Debug("pstSwupCheck->stBase.stOption.ulSwupMode: %d \n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);
	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
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

STATIC HERROR xsvc_si_SwupCheckGetLink80InfoFromBatLinkDes_Irdeto(SIxLinkDes_t *pstLinkDes, SIxLink80Info_t **ppstLink80Info)
{
	HUINT8			*pucRaw;
	SIxLink80Info_t	*pstLink80Info;

	if (pstLinkDes == NULL)
	{
		return ERR_FAIL;
	}

	if (pstLinkDes->ucLinkType != SWUP_IRDETO_BAT_LINKAGE_80)
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

	pstLink80Info = (SIxLink80Info_t *)OxSI_Malloc (sizeof(SIxLink80Info_t));
	if (pstLink80Info == NULL)				{ return ERR_FAIL; }
	HxSTD_memset (pstLink80Info, 0, sizeof(SIxLink80Info_t));

	pstLink80Info->ucManufacturerCode = HxMACRO_Get8Bit(pucRaw);
	pucRaw++;
	pstLink80Info->ucControlByte = HxMACRO_Get8Bit(pucRaw);

	*ppstLink80Info = pstLink80Info;

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckGetLink82InfoFromBatLinkDes_Irdeto(SIxLinkDes_t *pstLinkDes, SIxLink82Info_t **ppstLink82Info)
{
	HUINT8			*pucRaw;
	SIxLink82Info_t		*pstLink82Info;

	if (pstLinkDes == NULL)
	{
		return ERR_FAIL;
	}

	if (pstLinkDes->ucLinkType != SWUP_IRDETO_BAT_LINKAGE_82)
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

	pstLink82Info = (SIxLink82Info_t *)OxSI_Malloc (sizeof(SIxLink82Info_t));
	if (pstLink82Info == NULL)				{ return ERR_FAIL; }
	HxSTD_memset (pstLink82Info, 0, sizeof(SIxLink82Info_t));

	pucRaw++;
	pstLink82Info->ucManufacturerCode = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 2;
	pstLink82Info->ucHardwareCode = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 2;
	pstLink82Info->ucVariantHi = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 2;
	pstLink82Info->ucVariantLo = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 2;
	pstLink82Info->ucLoadSequenceNumber = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 2;
	pstLink82Info->ucControlByte = HxMACRO_Get8Bit(pucRaw);

	*ppstLink82Info = pstLink82Info;

	return ERR_OK;
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

STATIC HERROR xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte(Handle_t hAction, HUINT32 *pulNumInfo, void **ppvInfoArray, XsvcSi_SwupDataType_e eDataType)
{

	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulActionId;
	HUINT8					 ucManufacturerCode = s_stSwupCheckPramIrdeto.ucManufacturerCode;
	HUINT8					 ucHardwareCode = s_stSwupCheckPramIrdeto.ucHardwareCode;
	HUINT8					 ucLoadSequenceNumber = s_stSwupCheckPramIrdeto.ucLoadSequenceNumber;
	HUINT8					 ucVariantHi = s_stSwupCheckPramIrdeto.ucVariantHi;
	HUINT8					 ucVariantLo = s_stSwupCheckPramIrdeto.ucVariantLo;
	SIxTable_PsiTable_t 	*pstBatTable = NULL;
	HxList_t				*pstElemItem = NULL;
	HBOOL					 bFoundLinkDes = FALSE;
	SIxLink82Info_t			*pstLink82Info = NULL;
	SIxLink80Info_t			*pstLink80Info = NULL;
	SIxLinkDes_t			*pstLinkDes = NULL;

	HUINT8					 ucControlByte = 0;
	HBOOL					 bFoundFlag = FALSE;
	HBOOL					 bManufacturerFound = FALSE;
	HBOOL					 bHardwareAndVariantFound = FALSE;

	DbSvc_TripleId_t		*pstTripleIdArray = NULL;
	HUINT8					*pucCtrlByte = NULL;


	HxLOG_Trace();

	pucCtrlByte = NULL;
	pucCtrlByte = (HUINT8 *)OxSI_Malloc (sizeof(HUINT8));
	if (pucCtrlByte == NULL)
	{
		HxLOG_Error("Alloc fail \n");
		return ERR_FAIL;
	}

	pstTripleIdArray = NULL;
	pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t));
	if (pstTripleIdArray == NULL)
	{
		HxLOG_Error("Alloc fail \n");
		if(pucCtrlByte != NULL)
		{
			OxSI_Free(pucCtrlByte);
			pucCtrlByte = NULL;
		}
		return ERR_FAIL;
	}


	if ((pulNumInfo == NULL) || (ppvInfoArray == NULL))
	{
		HxLOG_Error("Invalid parameter \n");
		if(pucCtrlByte != NULL)
		{
			OxSI_Free(pucCtrlByte);
			pucCtrlByte = NULL;
		}

		if(pstTripleIdArray != NULL)
		{
			OxSI_Free(pstTripleIdArray);
			pstTripleIdArray = NULL;
		}
		return ERR_FAIL;
	}

	*pulNumInfo = 0;
	*ppvInfoArray = NULL;

	ulActionId = PAL_PIPE_GetActionId (hAction);


	pstBatTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Bat, eBOUQUET_ID_IRDETO);

	if (pstBatTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d) \n", ulActionId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if(pstBatTable->unInfo.stBat.usBqId != eBOUQUET_ID_IRDETO)
	{
		HxLOG_Info("pstBatTable->unInfo.stBat.usBqId: 0x%x is not a eBOUQUET_ID_IRDETO(0x%x)\n", pstBatTable->unInfo.stBat.usBqId, eBOUQUET_ID_IRDETO);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if(pstBatTable->pstDesLoop == NULL)
	{
		HxLOG_Error ("pstBatTable->pstDesLoop == NULL \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxLOG_Print("[System Information]\n");
	HxLOG_Print("manufacturer_code\t = 0x%x\n", ucManufacturerCode);
	HxLOG_Print("hardware_code\t\t = 0x%x\n", ucHardwareCode);
	HxLOG_Print("load_sequence_number\t = 0x%x\n", ucLoadSequenceNumber);
	HxLOG_Print("operator_variant_hi\t = 0x%x\n", ucVariantHi);
	HxLOG_Print("operator_variant_lo\t = 0x%x\n", ucVariantLo);
	HxLOG_Print("\t =================\n");


	// Should search 0x82 at first.
	for (pstElemItem = pstBatTable->pstDesLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		HUINT8		*pucDesRaw = HLIB_LIST_Data (pstElemItem);

		HxLOG_Print("start 1st loop desc parsing \n");

		if (NULL == pucDesRaw)
		{
			HxLOG_Print("NULL == pucDesRaw, continue \n");
			continue;
		}

		bFoundLinkDes = FALSE;

		if(pstLinkDes)
		{
			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
			pstLinkDes = NULL;
		}

#if defined(CONFIG_DEBUG)
		HxLOG_Print("tag: 0x%02x \n", *pucDesRaw);
#endif

		switch (*pucDesRaw)
		{
			case eSIxDESCTAG_LINKAGE:
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
				break;
		}

		if(pstLinkDes)
		{
			HxLOG_Print("pstLinkDes exists, pstLinkDes->ucLinkType: 0x%x \n", pstLinkDes->ucLinkType);

			/*Find linkage_descriptor with linkage_type 0x82*/
			if (pstLinkDes->ucLinkType == 0x82)
			{
				hErr = xsvc_si_SwupCheckGetLink82InfoFromBatLinkDes_Irdeto (pstLinkDes, &pstLink82Info);

				if (hErr != ERR_OK || pstLink82Info == NULL)
				{
					continue;
				}

				HxLOG_Print("[Descriptor Information]\n");
				HxLOG_Print("manufacturer_code\t = 0x%x\n", pstLink82Info->ucManufacturerCode);
				HxLOG_Print("hardware_code\t\t = 0x%x\n", pstLink82Info->ucHardwareCode);
				HxLOG_Print("load_sequence_number\t = 0x%x\n", pstLink82Info->ucLoadSequenceNumber);
				HxLOG_Print("operator_variant_hi\t = 0x%x\n", pstLink82Info->ucVariantHi);
				HxLOG_Print("operator_variant_lo\t = 0x%x\n", pstLink82Info->ucVariantLo);

				if (ucManufacturerCode == pstLink82Info->ucManufacturerCode)
				{
					bManufacturerFound = TRUE;

#if 0	// SWUP TEST Code
					bHardwareAndVariantFound = TRUE;
					bFoundFlag = TRUE;
					ucControlByte = pstLink82Info->ucControlByte;

					HxLOG_Print("[SI_LIVE_ProcessBat_CheckControlByte] LinkType (0x%x), ControlByte (0x%x)\n", pstLinkDes->ucLinkType, pstLink82Info->ucControlByte);
					break;

#else
					if ((ucHardwareCode == pstLink82Info->ucHardwareCode) &&
						(ucVariantHi == pstLink82Info->ucVariantHi) &&
						(ucVariantLo == pstLink82Info->ucVariantLo))
					{
						bHardwareAndVariantFound = TRUE;

						if ((pstLink82Info->ucLoadSequenceNumber == 0) ||
							(pstLink82Info->ucLoadSequenceNumber == 0xFF) ||
							(pstLink82Info->ucLoadSequenceNumber == ucLoadSequenceNumber))
						{
							HxLOG_Print("[1]\n");
							if (pstLink82Info)
							{
								OxSI_Free(pstLink82Info);
								pstLink82Info = NULL;
							}
							continue;
						}

						if (((HUINT32)ucLoadSequenceNumber + 1) >= 0xff)
						{
							if ((pstLink82Info->ucLoadSequenceNumber < 1) ||
								(pstLink82Info->ucLoadSequenceNumber > 99))
							{
								HxLOG_Print("[2]\n");
								if (pstLink82Info)
								{
									OxSI_Free(pstLink82Info);
									pstLink82Info = NULL;
								}
								continue;
							}
						}
						else
						{
							if (((HUINT32)ucLoadSequenceNumber + 99) > 0xff)
							{
								if ((pstLink82Info->ucLoadSequenceNumber > ((HUINT32)ucLoadSequenceNumber + 99 - 256)) &&
									(pstLink82Info->ucLoadSequenceNumber < ((HUINT32)ucLoadSequenceNumber + 1)))
								{
									HxLOG_Print("[3]\n");
									if (pstLink82Info)
									{
										OxSI_Free(pstLink82Info);
										pstLink82Info = NULL;
									}
									continue;
								}
							}
							else
							{
								if ((pstLink82Info->ucLoadSequenceNumber < (ucLoadSequenceNumber + 1)) ||
									(pstLink82Info->ucLoadSequenceNumber > (ucLoadSequenceNumber + 99)))
								{
									HxLOG_Print("[4]\n");
									if (pstLink82Info)
									{
										OxSI_Free(pstLink82Info);
										pstLink82Info = NULL;
									}
									continue;
								}
							}
						}

						bFoundFlag = TRUE;
						ucControlByte = pstLink82Info->ucControlByte;
						*pucCtrlByte = ucControlByte;

						pstTripleIdArray->usOnId	= pstLinkDes->usOrgNetId;
						pstTripleIdArray->usTsId		= pstLinkDes->usTsId;
						pstTripleIdArray->usSvcId	= pstLinkDes->usSvcId;

						//HxLOG_Print("LinkType (0x%x), ControlByte (0x%x), usOnId (0x%x), usTsId (0x%x), usSvcId (0x%x)\n", pstLinkDes->ucLinkType, pstLink80Info->ucControlByte, pstTripleIdArray->usOnId, pstTripleIdArray->usTsId, pstTripleIdArray->usSvcId);

						if (pstLink82Info)
						{
							OxSI_Free(pstLink82Info);
							pstLink82Info = NULL;
						}

						break;
					}
#endif
				}

				if (pstLink82Info)
				{
					OxSI_Free(pstLink82Info);
					pstLink82Info = NULL;
				}

				if (bFoundFlag == TRUE)
				{
					HxLOG_Print("SWUP flag exist in linkType 0x82. So break! \n");
					break;
				}
			}
			else
			{
				HxLOG_Print("LinkType is NOT 0x82\n");
				continue;
			}

		}
		else
		{
			HxLOG_Print(" pstLinkDes doesn't exists [%d]\n", __LINE__);
			continue;
		}

	}

	if (bFoundFlag == TRUE || bManufacturerFound == TRUE)
	{
		HxLOG_Error ("Control Byte Found in 0x82 type\n");
		hErr = ERR_OK;
		goto END_FUNC;
	}

	// Do search 0x80 if not exist 0x82.
	for (pstElemItem = pstBatTable->pstDesLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		HUINT8		*pucDesRaw = HLIB_LIST_Data (pstElemItem);

		HxLOG_Print("start 1st loop desc parsing \n");

		if (NULL == pucDesRaw)
		{
			HxLOG_Print("NULL == pucDesRaw, continue \n");
			continue;
		}

		bFoundLinkDes = FALSE;

		if(pstLinkDes)
		{
			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
			pstLinkDes = NULL;
		}

#if defined(CONFIG_DEBUG)
		HxLOG_Print("tag: 0x%02x \n", *pucDesRaw);
#endif

		switch (*pucDesRaw)
		{
			case eSIxDESCTAG_LINKAGE:
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
				break;
		}

		if(pstLinkDes)
		{
			HxLOG_Print("pstLinkDes exists, pstLinkDes->ucLinkType: 0x%x \n", pstLinkDes->ucLinkType);

			/*Find linkage_descriptor with linkage_type 0x82*/
			if (pstLinkDes->ucLinkType == 0x80)
			{

				// Spec 변경 0x82에서 Manufacture 를 찾았을 경우 0x80은 무시 한다.
				if ( bManufacturerFound == FALSE )
				{
					hErr = xsvc_si_SwupCheckGetLink80InfoFromBatLinkDes_Irdeto (pstLinkDes, &pstLink80Info);

					if (hErr != ERR_OK || pstLink80Info == NULL)
					{
						HxLOG_Print("[7]\n");
						continue;
					}

					HxLOG_Print(" link80-ManufactureCode (0x%x)\n", pstLink80Info->ucManufacturerCode);
					if (ucManufacturerCode == pstLink80Info->ucManufacturerCode)
					{
						bFoundFlag = TRUE;
						ucControlByte = pstLink80Info->ucControlByte;
						*pucCtrlByte = ucControlByte;

						pstTripleIdArray->usOnId	= pstLinkDes->usOrgNetId;
						pstTripleIdArray->usTsId	= pstLinkDes->usTsId;
						pstTripleIdArray->usSvcId	= pstLinkDes->usSvcId;

						HxLOG_Print("LinkType (0x%x), ControlByte (0x%x), usOnId (0x%x), usTsId (0x%x), usSvcId (0x%x)\n", pstLinkDes->ucLinkType, pstLink80Info->ucControlByte, pstTripleIdArray->usOnId, pstTripleIdArray->usTsId, pstTripleIdArray->usSvcId);

						if (pstLink80Info)
						{
							OxSI_Free(pstLink80Info);
							pstLink80Info = NULL;
						}
						break;

					}

					if (pstLink80Info)
					{
						OxSI_Free(pstLink80Info);
						pstLink80Info = NULL;
					}


				}
				else
				{
					HxLOG_Print("ignore linkType 0x80 because linkType 0x82 has higer priority \n");
				}
			}
			else
			{
				HxLOG_Print("LinkType is NOT 0x80\n");
				continue;
			}

		}
		else
		{
			HxLOG_Print(" pstLinkDes doesn't exists [%d]\n", __LINE__);
			continue;
		}

	}


	//여기 온 것은 OK로 간주. 즉 만약 BqId 0x3622 에서 못 찾은 경우에도, control byte 0으로 setting 한다는 의미.
	{
		hErr = ERR_OK;
	}

END_FUNC:


	if (pstLink80Info)
	{
		OxSI_Free(pstLink80Info);
		pstLink80Info = NULL;
	}

	if (pstLink82Info)
	{
		OxSI_Free(pstLink82Info);
		pstLink80Info = NULL;
	}

	if(pstLinkDes)
	{
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
		pstLinkDes = NULL;
	}

	if (NULL != pstBatTable)
	{
		PAL_SIRAWPOOL_FreeTable (pstBatTable);
		pstBatTable = NULL;
	}

	if (bFoundFlag)
	{
		if(eDataType == eSI_SWUP_DATA_SSU_LINK_TRIPLEID)
		{
			*ppvInfoArray = pstTripleIdArray;
			*pulNumInfo = 1;
			HxLOG_Debug("get Triple ID \n");

		}
		else if(eDataType == eSI_SWUP_DATA_IRDETO_CTRL_BYTE)
		{
			*ppvInfoArray = pucCtrlByte;
			*pulNumInfo = 1;
			HxLOG_Debug("Get Control byte\n");
		}
		else
		{
			HxLOG_Error("eDataType: %d is not appropriate \n", eDataType);
		}

		pstTripleIdArray = NULL;
		pucCtrlByte = NULL;

	}
	else
	{
		hErr = ERR_FAIL;
		HxLOG_Info("[%s] leave. hErr: %d, Cannot find any information. \n", __FUNCTION__, hErr);
	}

	// 여기서 bFoundFlag==1인 것들은 xsvc_si_SwupCheckFreeData_Irdeto 에서 free.
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


#define ___XXXXXXXXXXXX___________
STATIC void xsvc_si_SwupCheckFreePsiInfo (void *pvData)
{
	xsvcSi_SwupPsiInfo_t		*pstSwupPsiInfo = (xsvcSi_SwupPsiInfo_t *)pvData;

	if (NULL != pstSwupPsiInfo)
	{
		OxSI_Free(pstSwupPsiInfo);
	}

	return;
}

STATIC HERROR xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT32 *pulNumSsu, xsvcSi_SwupPsiInfo_t **apstSwupPsiInfo)
{
	HUINT32						 ulActionId;
	HERROR						 hResult = ERR_FAIL;
	SIxTable_PsiTable_t			*pstPmtTable = NULL;
	HxList_t					*pstPmt2ndLoop = NULL;

	HxList_t					*pstInfoList = NULL;
	HxList_t					*pstList = NULL;
	xsvcSi_SwupPsiInfo_t		*pstSwupInfo, *astSwupInfoArray;
	HUINT32						 ulNumSsuSvc = 0, i;

	if ((pstSwupCheck == NULL) || (pulNumSsu == NULL) || (apstSwupPsiInfo == NULL))
	{
		HxLOG_Error("invalid parametes..! \n");
		return ERR_FAIL;
	}

	*pulNumSsu = 0;
	*apstSwupPsiInfo = NULL;

	pstSwupInfo = NULL;
	astSwupInfoArray	= NULL;

	// PMT parsing해서
	ulActionId = SVC_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	// irdeto ota stream type을 가진 element들을 list에 넣고
	for (pstPmt2ndLoop = pstPmtTable->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
		SIxTable_PmtElement_t	*pstPmtElem;

		if (NULL == pst2ndItem) 				{ continue; }
		pstPmtElem = &(pst2ndItem->unInfo.stPmt);

		if ((pstPmtElem->ucStreamType== IRDETO_PMT_STREAMTYPE_80)
			|| (pstPmtElem->ucStreamType == IRDETO_PMT_STREAMTYPE_82)
			|| (pstPmtElem->ucStreamType >= IRDETO_PMT_STREAMTYPE_84))
		{
			pstSwupInfo = (xsvcSi_SwupPsiInfo_t *)OxSI_Calloc (sizeof(xsvcSi_SwupPsiInfo_t));
			if (pstSwupInfo != NULL)
			{
				pstSwupInfo->usSvcId	= pstPmtTable->unInfo.stPmt.usProgNum;
				pstSwupInfo->usPid	= pstPmtElem->usElemPid;

				// TODO: descriptor 확인 필요
				pstSwupInfo->bUseUnt 	= 0;
				pstSwupInfo->ucUpdateVer = 0;

				pstInfoList = HLIB_LIST_Append(pstInfoList, (void *)pstSwupInfo);
			}
		}
	}

	// list에 저장된 정보가 하나도 없다면 error~~
	ulNumSsuSvc = HLIB_LIST_Length(pstInfoList);
	if (ulNumSsuSvc == 0)
	{
		HxLOG_Critical("\n\n");
		goto Exit_Func;
	}

	HxLOG_Debug("ulNumSsuSvc: %d \n", ulNumSsuSvc);

	// list에 있는 elem. 들을 array로 만들어서 return
	astSwupInfoArray = (xsvcSi_SwupPsiInfo_t *)OxSI_Calloc (sizeof(xsvcSi_SwupPsiInfo_t) * ulNumSsuSvc);
	if (astSwupInfoArray == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Exit_Func;
	}

	for (i = 0; i < ulNumSsuSvc; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstInfoList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumSsuSvc, i);
			continue;
		}

		pstSwupInfo = (xsvcSi_SwupPsiInfo_t *)HLIB_LIST_Data(pstList);
		if (pstSwupInfo == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		HxSTD_MemCopy(&astSwupInfoArray[i], pstSwupInfo, sizeof(xsvcSi_SwupPsiInfo_t));
	}

	*pulNumSsu 			= ulNumSsuSvc;
	*apstSwupPsiInfo	= astSwupInfoArray;
	hResult 			= ERR_OK;

Exit_Func:

	if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }
	if (pstInfoList != NULL)		{ HLIB_LIST_RemoveAllFunc(pstInfoList, xsvc_si_SwupCheckFreePsiInfo); }

	return hResult;
}

STATIC HERROR xsvc_si_SwupCheckFindSwupInfoFromIct_Irdeto(xsvcSi_SwupData_Irdeto_t *pstSwupCheck, HUINT32 *pulNumSsu, XsvcSi_SwupSignal_Irdeto_t **apstSwupPsiInfo)
{
	HERROR						 hErr = ERR_FAIL;
	HERROR						 hResult = ERR_FAIL;
	HUINT32						 ulActionId = 0;
	HUINT32						 ulNumSsu = 0;
	xsvcSi_SwupPsiInfo_t		*pstSsuPsiInfo = NULL;
	HUINT8						 ucManufacturerCode = 0;
	HUINT8						 ucHardwareCode = 0;
	SIxTable_PsiTable_t			*pstIctTable = NULL;
	SIxTable_PsiTable_t			*pstCurTable = NULL;
	xsvcSi_IctSectionInfo_t		 stIctInfo;
	HUINT8						 ucLoadSequenceNumber;
	HUINT32 					 i, j;
	HUINT16 					 usTime;
	HUINT8						 ucManufacId = 0, ucHwCode = 0;
	HUINT8						 ucFindSeqNum = 0, ucTableId = ETC_TABLE_ID, hour = 0, min = 0, sec = 0;
	HUINT8						 bFoundFlag = FALSE;
	XsvcSi_SwupSignal_Irdeto_t 	 stFoundInfo;
	XsvcSi_SwupSignal_Irdeto_t	*astSwupInfoArray;


	if ((pstSwupCheck == NULL) || (pstSwupCheck == NULL) || (pstSwupCheck == NULL))
	{
		HxLOG_Error("Invalid parameter..! \n");
		return ERR_FAIL;
	}

	hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto(pstSwupCheck, &ulNumSsu, &pstSsuPsiInfo);
	if ((hErr != ERR_OK) || (ulNumSsu == 0) || (pstSsuPsiInfo == NULL))
	{
		HxLOG_Error("xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto() failed..!! \n");
		if (pstSsuPsiInfo != NULL)
		{
			OxSI_Free (pstSsuPsiInfo);
		}
		svc_si_PostMsgToAp(eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return ERR_FAIL;
	}

	ucLoadSequenceNumber 	= s_stSwupCheckPramIrdeto.ucLoadSequenceNumber;
	ucManufacturerCode		= s_stSwupCheckPramIrdeto.ucManufacturerCode;
	ucHardwareCode			= s_stSwupCheckPramIrdeto.ucHardwareCode;


	HxLOG_Print("[System Information]\n");
	HxLOG_Print("manufacturer_code\t = 0x%x\n", ucManufacturerCode);
	HxLOG_Print("hardware_code\t\t = 0x%x\n", ucHardwareCode);
	HxLOG_Print("load_sequence_number\t = 0x%x\n", ucLoadSequenceNumber);
	HxLOG_Print("\t =================\n");

	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);

	pstIctTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Ict, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstIctTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		if (pstSsuPsiInfo != NULL)
		{
			OxSI_Free (pstSsuPsiInfo);
		}

		return ERR_FAIL;
	}

	for (pstCurTable = pstIctTable; pstCurTable != NULL; pstCurTable = pstCurTable->pstNextTable)
	{
		stIctInfo.ucManufacturerCode = pstCurTable->unInfo.stIct.ucManufacturerCode;
		stIctInfo.ucHardwareCode = pstCurTable->unInfo.stIct.ucHardwareCode;
		HxSTD_MemCopy(stIctInfo.aucSeqNumber, 		pstCurTable->unInfo.stIct.aucSeqNumber, 	sizeof(stIctInfo.aucSeqNumber));
		HxSTD_MemCopy(stIctInfo.aucTableId, 		pstCurTable->unInfo.stIct.aucTableId, 		sizeof(stIctInfo.aucTableId));
		HxSTD_MemCopy(stIctInfo.ausDownloadTime, 	pstCurTable->unInfo.stIct.ausDownloadTime, 	sizeof(stIctInfo.ausDownloadTime));

		// Check Manufacture code && Hardware code
		if (!((ucManufacturerCode == stIctInfo.ucManufacturerCode) && (ucHardwareCode == stIctInfo.ucHardwareCode)))
		{
			HxLOG_Print("[%s:%d] ERROR !! ucManufacturerCode: (0x%x), ucHardwareCode :(0x%x)\n", __FUNCTION__, __LINE__, stIctInfo.ucManufacturerCode, stIctInfo.ucHardwareCode);
			continue;
		}

		// Find MAX Sequence Number
		for (i = 0, ucFindSeqNum = 0, j = 0; i < 255; i++)
		{
			HxLOG_Print("stIctInfo.aucSeqNumber[%d]: %d \n", i, stIctInfo.aucSeqNumber[i]);

			if (stIctInfo.aucSeqNumber[i] == 0 || stIctInfo.aucSeqNumber[i] == 0xff)		{ continue;	}
			if (stIctInfo.aucSeqNumber[i] == ucLoadSequenceNumber)						{ continue;	}

			if (((HUINT32)ucLoadSequenceNumber + 1) >= 0xff)
			{
				if(stIctInfo.aucSeqNumber[i] < 1 || stIctInfo.aucSeqNumber[i] > 99)		{ continue;	}
			}
			else
			{
				if (((HUINT32)ucLoadSequenceNumber + 99) > 0xff)
				{
					if (stIctInfo.aucSeqNumber[i] > ((HUINT32)ucLoadSequenceNumber + 99) &&
						stIctInfo.aucSeqNumber[i] < ((HUINT32)ucLoadSequenceNumber + 1))
					{
						continue;
					}
				}
				else
				{
					if (stIctInfo.aucSeqNumber[i] < (ucLoadSequenceNumber + 1) ||
						stIctInfo.aucSeqNumber[i] > (ucLoadSequenceNumber + 99))
					{
						continue;
					}
				}
			}

			if ((stIctInfo.aucSeqNumber[j] < stIctInfo.aucSeqNumber[i]) || (i == 0))
			{
				bFoundFlag = TRUE;
				ucManufacId = stIctInfo.ucManufacturerCode;
				ucHwCode = stIctInfo.ucHardwareCode;
				ucFindSeqNum = stIctInfo.aucSeqNumber[i];
				ucTableId = stIctInfo.aucTableId[i];
				usTime = stIctInfo.ausDownloadTime[i];
				hour = stIctInfo.ausDownloadTime[i] / 3600;
				min = (stIctInfo.ausDownloadTime[i] % 3600) / 60;
				sec = stIctInfo.ausDownloadTime[i] % 60;

				HxLOG_Print("\t[xsvc_si_SwupCheckFindSwupInfoFromIct_Irdeto] ++Irdeto OTA++ i = %d, SeqNum:0x%x TableID:0x%x Time:%d:%d:%d\n",
					i, ucFindSeqNum, ucTableId, hour, min, sec);

				j = i;
			}
		}

		if (bFoundFlag == TRUE)
		{
			// 기존 구현에서 1개의 ict info 만 처리하게 되어있는데 list로 구현되어 있다
			// max 1개이므로 list 처리 없이 처리하도록 구현함.
			HxSTD_MemSet(&stFoundInfo, 0x00, sizeof(XsvcSi_SwupSignal_Irdeto_t));
			stFoundInfo.ucManufacturerCode = ucManufacId;
			stFoundInfo.ucHardwareCode = ucHwCode;
			stFoundInfo.ucLoadSeqNumber = ucFindSeqNum;
			stFoundInfo.ucTableId = ucTableId;
			stFoundInfo.stDownloadTime.ucHour = hour;
			stFoundInfo.stDownloadTime.ucMinute = min;
			stFoundInfo.stDownloadTime.ucSecond = sec;
			stFoundInfo.stDownloadTime.usMillisecond = 0;
			stFoundInfo.usPid = pstSsuPsiInfo->usPid;

			HxLOG_Print("[%s:%d] loadSequenceNumber (0x%x), TableID (0x%x), Pid (0x%x)\n", __FUNCTION__, __LINE__, stFoundInfo.ucLoadSeqNumber, stFoundInfo.ucTableId, stFoundInfo.usPid);
		}
		else
		{
			HxLOG_Print("[%s:%d] No SWUP Information found..! \n", __FUNCTION__, __LINE__);
		}
	}

	if (bFoundFlag != TRUE)
	{
		HxLOG_Print("[%s:%d] No SWUP Information found..! \n", __FUNCTION__, __LINE__);
		goto Exit_Func;
	}

	astSwupInfoArray = (XsvcSi_SwupSignal_Irdeto_t *)OxSI_Calloc(sizeof(XsvcSi_SwupSignal_Irdeto_t));
	if (astSwupInfoArray == NULL)
	{
		HxLOG_Print("[%s:%d] No SWUP Information found..! \n", __FUNCTION__, __LINE__);
		goto Exit_Func;
	}
	HxSTD_MemCopy(astSwupInfoArray, &stFoundInfo, sizeof(XsvcSi_SwupSignal_Irdeto_t));

	*pulNumSsu 			= 1;
	*apstSwupPsiInfo	= astSwupInfoArray;
	hResult 			= ERR_OK;

Exit_Func:

	if (pstSsuPsiInfo != NULL) 		{ OxSI_Free (pstSsuPsiInfo); }
	if (pstIctTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstIctTable); }

	return hResult;
}


#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SwupCheckMsgCmdStart_Irdeto(xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_Irdeto(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_Irdeto(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Irdeto (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_Irdeto(xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode == eSI_SWUP_MODE_IRDETO_SSUSVC_TP)
	{
		(void)xsvc_si_LibSwupOnPatReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	}
}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR								 hErr = ERR_FAIL;
	xsvcSi_SwupPsiInfo_t				*pstPsiInfo = NULL;
	HUINT32 				 			 ulNumSsu = 0;
	HUINT16 							 usExtId = 0;
	HUINT8								 ucManufacturerCode;
	HUINT8								 ucHardwareCode;
	HUINT32								 i;

	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_IRDETO_SSUSVC_TP)
	{
		return;
	}

	hErr = xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto(pstSwupCheck, &ulNumSsu, &pstPsiInfo);
	if ((hErr != ERR_OK) || (ulNumSsu == 0) || (pstPsiInfo == NULL))
	{
		HxLOG_Error("xsvc_si_SwupCheckFindSsuInfoFromPmt_Irdeto() failed..!! \n");
		svc_si_PostMsgToAp(eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		if (pstPsiInfo != NULL) 	{ OxSI_Free (pstPsiInfo); }
		return;
	}

	ucManufacturerCode		= s_stSwupCheckPramIrdeto.ucManufacturerCode;
	ucHardwareCode			= s_stSwupCheckPramIrdeto.ucHardwareCode;

	usExtId = (HUINT16)(ucManufacturerCode<<8) & 0xff00;
	usExtId |= ucHardwareCode;

	for (i = 0; i < ulNumSsu; i++)
	{
		HxLOG_Print("[%s:%d] Retrieve ICT (PID 0x%04x)\n", __FUNCTION__, __LINE__, pstPsiInfo[i].usPid);

		hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Ict, (HUINT32)usExtId, (HUINT32)pstPsiInfo[i].usPid);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("svc_si_SrchStartFilter err: usExtId(%d), PID(%d)\n", usExtId, pstPsiInfo[i].usPid);
		}
	}

	if (pstPsiInfo != NULL) 	{ OxSI_Free (pstPsiInfo); }

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

STATIC void xsvc_si_SwupCheckMsgEvtIctTable_Irdeto (xsvcSi_SwupData_Irdeto_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HUINT32						 ulActionId = SVC_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	HERROR						 hErr = ERR_FAIL;
	HUINT32						 ulNumSsu = 0;
	XsvcSi_SwupSignal_Irdeto_t 	*astSwupSignal = NULL;

	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_IRDETO_SSUSVC_TP)
	{
		return;
	}

	// ICT timeout
	if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
	{
		if (PAL_SIRAWPOOL_CountRaws(ulActionId, eSIxTABLETYPE_Ict, FALSE, 0) == 0)
		{
			HxLOG_Print("[%s:%d] No ict table exist.. ! \n", __FUNCTION__, __LINE__);

			hErr = xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Ict, SILIB_TABLE_EXT_ID_ALL);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("stop ICT section failed..!! \n");
			}

			svc_si_PostMsgToAp(eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		}
		return;
	}

	// ICT received
	hErr = xsvc_si_SwupCheckFindSwupInfoFromIct_Irdeto(pstSwupCheck, &ulNumSsu, &astSwupSignal);
	if ((hErr != ERR_OK) || (ulNumSsu == 0) || (astSwupSignal == NULL))
	{
		HxLOG_Error("stop ICT section failed..!! \n");
		svc_si_PostMsgToAp(eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return;
	}

	// SWUP 정보를 기록해 놓는다.
	pstSwupCheck->bFound = TRUE;
	pstSwupCheck->stSwupInfo = astSwupSignal[0];

	OxSI_Free(astSwupSignal);

	// SWUP를 위해 받은 모든 ICT 를 Clear 한다.
	hErr = xsvc_si_SwupCheckStopSection_Irdeto(pstSwupCheck, eSIxTABLETYPE_Ict, SILIB_TABLE_EXT_ID_ALL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("stop ICT section failed..!! \n");
	}

	svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);

	return;
}

STATIC void xsvc_si_SwupCheckProcessMsg_Irdeto (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupData_Irdeto_t	*pstSwupData = (xsvcSi_SwupData_Irdeto_t *)pvData;

	if (NULL == pstSwupData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		xsvc_si_SwupCheckMsgCmdStart_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		xsvc_si_SwupCheckMsgCmdStop_Irdeto(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SwupCheckMsgEvtPatTable_Irdeto (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SwupCheckMsgEvtPmtTable_Irdeto (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
	case eSIMSG_EVT_NIT_OTH_TABLE:
		xsvc_si_SwupCheckMsgEvtNitTable_Irdeto (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		xsvc_si_SwupCheckMsgEvtBatTable_Irdeto (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_ICT_TABLE:
		xsvc_si_SwupCheckMsgEvtIctTable_Irdeto (pstSwupData, pstMsg);
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
	xsvcSi_SwupData_Irdeto_t		*pstSwupCheck;

	pstSwupCheck = (xsvcSi_SwupData_Irdeto_t *)OxSI_Calloc(sizeof(xsvcSi_SwupData_Irdeto_t));
	if (NULL != pstSwupCheck)
	{
		pstSwupCheck->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)pstSwupCheck;
}

void xsvc_si_SwupCheckFreeData_Irdeto (void *pvData)
{
	xsvcSi_SwupData_Irdeto_t		*pstSwupCheck = (xsvcSi_SwupData_Irdeto_t *)pvData;

	if (NULL != pstSwupCheck)
	{
		OxSI_Free(pstSwupCheck);
	}

	return;
}

HERROR xsvc_si_SwupCheckFlag_Irdeto (Handle_t hAction, HUINT32 *pulSwupCheckFlag)
{
	HUINT32 				ulNumInfo = 0;
	HERROR				hErr = ERR_FAIL;
	HUINT8				*pucControlByte;

	hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte (hAction, &ulNumInfo, (void **)&pucControlByte, eSI_SWUP_DATA_IRDETO_CTRL_BYTE);
	if(hErr == ERR_OK)
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

	if (pulDataNum == NULL)				{ return ERR_FAIL; }
	if (ppvDataArray == NULL)			{ return ERR_FAIL; }

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
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte (hAction, pulDataNum, ppvDataArray, eDataType);
		break;

	case eSI_SWUP_DATA_IRDETO_CTRL_BYTE:
		hErr = xsvc_si_SwupCheckGetSsuLinkTripleIdAndControlByte (hAction, pulDataNum, ppvDataArray, eDataType);
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


/* end of file */

