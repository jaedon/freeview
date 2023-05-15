/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ota_humax.c
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
#include <linkedlist.h>
#include <silib.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <db_svc.h>
#include <svc_swup.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>
#include <_xsvc_si_lib_swup_base.h>
#include <_xsvc_si_swup_base.h>
#include <_xsvc_si_swup.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_OP_MIDDLE_EAST)
#define	HUMAX_SSU_SWUP_CHECK 	// DIT 대신 DSI 를 사용하는 모델 또는 OP 에서는 enable 되어야 된다..
#endif

#if defined(HUMAX_SSU_SWUP_CHECK)
#define SWUP_SERVICE_NAME 		"HUMAX SSU"
#else
#define SWUP_SERVICE_NAME 		"HUMAX OTA"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(HUMAX_SSU_SWUP_CHECK)
typedef struct
{
	HUINT32			 ulStartSystemId;
	HUINT32			 ulEndSystemId;
	HUINT16			 usHdfVer;
	VersionType_t	 eDataType;			// 시그널링 온 Data Type : eVERSION_TYPE_LOADER, eVERSION_TYPE_APPL,  eVERSION_TYPE_EEPROM 등
	HUINT16			 usDataType;
	HUINT32			 ulDataVersion;
	HUINT8			 ucType;			// Forced, Normal
	HUINT16			 usSvcId;
	HUINT16			 usPid;
} xsvcSi_SwupCheckHumax_t;
#else
typedef XsvcSi_SwupSignal_Humax_t	xsvcSi_SwupCheckHumax_t;
#endif

typedef struct
{
	svcSi_SwupCheck_Base_t	 	 stBase;

	HBOOL						 bFound;

	SvcSi_SwupSignal_t			 stSwupInfo;

	xsvcSi_SwupCheckHumax_t		*astSwupInfo;
	HUINT32						 ulSwupNum;
} xsvcSi_SwupCheck_Humax_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC HERROR xsvc_si_SwupCheckInitSwupData_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck)
{
	if (pstSwupCheck == NULL)				{ return ERR_FAIL; }

	pstSwupCheck->bFound = FALSE;

	HxSTD_MemSet(&pstSwupCheck->stSwupInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

	return ERR_OK;
}

HERROR xsvc_si_SwupCheckGetFilteringTypes_Humax(xsvcSi_SwupCheck_Humax_t *pstSwupCheck)
{
	if (NULL == pstSwupCheck)
	{
		return ERR_FAIL;
	}

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_HUMAX_HOME_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT | bSI_FILTERING_SDT_ACT;
		break;

	default:
		pstSwupCheck->stBase.eFiltering = 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_SdtAct:
		*peFilterType	= bSI_FILTERING_SDT_ACT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid 		= SDT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckStartSection_Humax(xsvcSi_SwupCheck_Humax_t *pstSwupCheck)
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
		hErr = xsvc_si_SwupCheckConvertTableType2FilterTypeAndExtId_Humax (pstSwupCheck, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (pstSwupCheck->stBase.eFiltering & eFilterType))
		{
			hErr = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eTableType, ulExtId, ulPid);
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

STATIC HERROR xsvc_si_SwupCheckStopSection_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SIxTable_Type_e eSiTableType, HUINT32 ulExtId)
{
	HERROR			hErr = ERR_FAIL;

	hErr = svc_si_SwupCheckStopSection(&pstSwupCheck->stBase, eSiTableType, ulExtId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckStopSection() failed..! eSiTableType(%d) extId(0x%X) \n", eSiTableType, ulExtId);
	}

	return hErr;
}

STATIC void xsvc_si_SwupCheckFreeInfo_Humax (void *pvData)
{
	if (NULL != pvData)
	{
		OxSI_Free(pvData);
	}

	return;
}

STATIC HERROR xsvc_si_SwupCheckSaveSwupPid_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, HUINT16 usServiceId, HUINT16 usPid)
{
	xsvcSi_SwupCheckHumax_t		*astSwupInfo;
	HUINT32						 ulNum;
	HBOOL						 bFound ;
	HUINT32						 i;

	bFound = FALSE;

	astSwupInfo = pstSwupCheck->astSwupInfo;
	ulNum = pstSwupCheck->ulSwupNum;

	HxLOG_Print("[%s,%d] ulHumaxOtsNum (0x%x) \n", __FUNCTION__, __LINE__, ulNum);

	for(i=0; i< ulNum; i++)
	{
		if(astSwupInfo[i].usSvcId == usServiceId)
		{
			HxLOG_Print("[%s,%d]  Found = SW Update Service Id (0x%x) usPid (0x%x) \n", __FUNCTION__, __LINE__, usServiceId, usPid);
			astSwupInfo->usPid = usPid;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_SwupCheckFindSwupService_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck,  HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						 hErr;
	HUINT32						 ulNumHumaxSvc, ulCntHumaxSvc;
	POINTER_LIST_T				*pstInfoList;

	HUINT32						 ulActionId;
	SIxTable_PsiTable_t			*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t					*pstSdtSvcLoop, *pstSdtSvcDesLoop;
	HUINT8						 ucServName[256];
	xsvcSi_SwupCheckHumax_t		*pstSwupInfo, *astSwupInfoArray;

	// Initalize
	astSwupInfoArray	= NULL;
	pstInfoList			= NULL;
	ulNumHumaxSvc		= 0;
	ulCntHumaxSvc 		= 0;



	ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT\n", ulActionId);
		return ERR_FAIL;
	}

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
					SIxSvcDes_t *pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstSvcDes)
					{
						HxSTD_MemSet(ucServName, 0, sizeof(HUINT8) *256);
						MWC_UTIL_DvbStrcpyStrOnly(ucServName, pstSvcDes->pszServName);

						if (MWC_UTIL_DvbStrncmp(ucServName, SWUP_SERVICE_NAME, MWC_UTIL_DvbStrlen(SWUP_SERVICE_NAME) )== 0 )
						{
							pstSwupInfo = (xsvcSi_SwupCheckHumax_t *)OxSI_Malloc (sizeof(xsvcSi_SwupCheckHumax_t));
							if (pstSwupInfo != NULL)
							{
								HxSTD_MemSet (pstSwupInfo, 0, sizeof(xsvcSi_SwupCheckHumax_t));

								pstSwupInfo->usSvcId	=  pstSdtSvcItem->unInfo.stSdt.usSvcId;
								ulNumHumaxSvc++;
								pstInfoList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstInfoList, (void *)pstSwupInfo);
							}
						}
						else
						{
							HxLOG_Debug("not found humax ota service (%s) \n\n",ucServName);
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}
				}
			}
		}
	}

	HxLOG_Debug("PAL_SIRAWPOOL_ParseTable ulNumHumaxSvc : %d \n", ulNumHumaxSvc);

	if (ulNumHumaxSvc == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulDataNum == NULL && ppvDataArray == NULL)
	{
		hErr = ERR_OK;
		goto Error;
	}

	astSwupInfoArray = (xsvcSi_SwupCheckHumax_t *)OxSI_Malloc (sizeof(xsvcSi_SwupCheckHumax_t) * ulNumHumaxSvc);
	if (astSwupInfoArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	while (pstInfoList != NULL && ulCntHumaxSvc < ulNumHumaxSvc)
	{
		pstSwupInfo = (xsvcSi_SwupCheckHumax_t *)pstInfoList->pvContents;
		if (pstSwupInfo != NULL)
		{
			astSwupInfoArray[ulCntHumaxSvc] = *pstSwupInfo;
			ulCntHumaxSvc++;

			OxSI_Free (pstSwupInfo);
		}

		pstInfoList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstInfoList, pstInfoList, NULL);
	}

	if (ulCntHumaxSvc == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulDataNum != NULL)
	{
		*pulDataNum = ulCntHumaxSvc;
	}

	if (ppvDataArray != NULL)
	{
		*ppvDataArray = astSwupInfoArray;
		astSwupInfoArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (astSwupInfoArray != NULL)				{ OxSI_Free (astSwupInfoArray); }
	if (pstSdtTable != NULL)					{ PAL_SIRAWPOOL_FreeTable(pstSdtTable); }

	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstInfoList, (FREE_FUNC)xsvc_si_SwupCheckFreeInfo_Humax);

	return hErr;
}

STATIC HERROR xsvc_si_SwupCheckFindPidFromPmt_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck)
{
	HUINT32						 ulActionId;
	HERROR						 hResult = ERR_FAIL;
	HERROR						 hErr = ERR_FAIL;
	SIxTable_PsiTable_t			*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t					*pstPmt2ndLoop = NULL;

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

			hErr = xsvc_si_SwupCheckSaveSwupPid_Humax(pstSwupCheck, pstCurrPmt->unInfo.stPmt.usProgNum, pstPmtElem->usElemPid);
			if(hErr == ERR_OK)
			{
				HxLOG_Print("[%s:%d] Found SW Update Service (0x%x) \n", __FUNCTION__, __LINE__, pstCurrPmt->unInfo.stPmt.usProgNum);

				if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }

				return ERR_OK;
			}
			else
			{
				HxLOG_Print ("[%s:%d] Not Found SW Update Service (0x%x) \n", __FUNCTION__, __LINE__, pstCurrPmt->unInfo.stPmt.usProgNum);
			}
		}
	}

	if (pstPmtTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstPmtTable); }

	return hResult;
}

#if defined(HUMAX_SSU_SWUP_CHECK)
STATIC HERROR xsvc_si_SwupCheckParseCompability_Humax (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound, HUINT32 *pulSystemVer)
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

STATIC HERROR xsvc_si_SwupCheckFindInfoFromDsi_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, HUINT32 *pulNumSsu, SvcSi_SwupSignal_t **apstSwupSignals)
{
	HBOOL					 bHwFound = FALSE, bSwFound = FALSE;
	HUINT32					 ulSignalVersion = 0;
	HUINT32					 ulSystemId = 0, ulSwVersion = 0;
	HUINT32					 ulIdxSsu = 0, ulNumSsu = 0;
	SvcSi_SwupSignal_t		*pstSwupInfo;
	SvcSi_SwupSignal_t		*astSwupInfo;
	POINTER_LIST_T			*pstSwupList;

	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulActionId;
	SIxTable_PsiTable_t		*pstDsiTable = NULL, *pstCurrDsi = NULL;
	HxList_t				*pstDsi2ndLoop = NULL;
	HUINT32					 ulListLen = 0, ulIndex = 0;
	HUINT8					*pucRaw;
	HxList_t				*pHxList = NULL;

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

			HxLOG_Print("[%s:%d] ulListLen : %d) \n", __FUNCTION__, __LINE__, ulListLen);

			for(ulIndex= 0 ; ulIndex< ulListLen; ulIndex++)
			{
				pucRaw = (HUINT8*)HLIB_LIST_Data(pHxList);
				if(pucRaw == NULL)
				{
					HxLOG_Error ("pucRaw is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}

				xsvc_si_SwupCheckParseCompability_Humax(pucRaw, NULL, &bHwFound, &bSwFound, &ulSignalVersion);

				pHxList = HLIB_LIST_Next(pHxList);
				if(pHxList == NULL)
				{
					HxLOG_Error ("pHxList is NULL (ulActionId : %d) \n", ulActionId);
					break;
				}
			}

			if (bHwFound == TRUE && bSwFound == TRUE)
			{
				HxLOG_Print ("Found Sw Update H/W Ver & S/W Ver.!! [ bHwFound : %d  , bSwFound : %d ] \n", bHwFound, bSwFound);

				pstSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t));
				if (pstSwupInfo != NULL)
				{
					HxSTD_memset (pstSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

					pstSwupInfo->ulStartSystemId = ulSystemId;
					pstSwupInfo->ulEndSystemId	= ulSystemId;
					pstSwupInfo->usPid = pstDsiSec->usExtId;
					pstSwupInfo->usDataType = eSWUP_DATATYPE_APPL;
					//pstSwupInfo->ucOtaType	= Forced; // Forced 인지 처리하는 부분이 없네요..ㅜㅜㅜ

					svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSwupInfo->usDataType, &(pstSwupInfo->eDataType));
					pstSwupInfo->ulDataVersion = ulSignalVersion;

					HxLOG_Debug("Found OTA [ ulSystemId : 0x%x  , usPid : 0x%x , ulSignalVersion : 0x%x] \n", pstSwupInfo->ulStartSystemId, pstSwupInfo->usPid, pstSwupInfo->ulDataVersion);

					pstSwupList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSwupList, (void *)pstSwupInfo);
				}
			}
		}
	}

	if (pstSwupList == NULL)
	{
		HxLOG_Error("pstSwupList is NULL \n");
		if (pstDsiTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstDsiTable); }
		return ERR_FAIL;
	}

	if (pulNumSsu != NULL)
	{
		*pulNumSsu = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSwupList, NULL, NULL);
	}

	if (apstSwupSignals != NULL)
	{
		ulIdxSsu = 0;
		ulNumSsu = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSwupList, NULL, NULL);
		astSwupInfo = (SvcSi_SwupSignal_t *)OxSI_Malloc (sizeof(SvcSi_SwupSignal_t) * ulNumSsu);
		if (astSwupInfo == NULL)
		{
			UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSwupList, xsvc_si_SwupCheckFreeInfo_Humax);
			return ERR_FAIL;
		}

		while (pstSwupList)
		{
			pstSwupInfo = (SvcSi_SwupSignal_t *)pstSwupList->pvContents;

			if (pstSwupInfo != NULL)
			{
				astSwupInfo[ulIdxSsu] = *pstSwupInfo;
				OxSI_Free (pstSwupInfo);
				ulIdxSsu ++;
			}

			pstSwupList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSwupList, pstSwupList, NULL);
		}

		*apstSwupSignals = astSwupInfo;
	}

	if (pstDsiTable != NULL)		{ PAL_SIRAWPOOL_FreeTable(pstDsiTable); }

	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSwupList, xsvc_si_SwupCheckFreeInfo_Humax);

	return ERR_OK;
}
#endif

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SwupCheckMsgCmdStart_Humax(xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)svc_si_SwupCheckInitData(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stStartSwupCheck.unSpec), (void*)&(pstMsg->unArg.stStartSwupCheck.stOption), FALSE);
	(void)xsvc_si_SwupCheckInitSwupData_Humax(pstSwupCheck);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)xsvc_si_SwupCheckGetFilteringTypes_Humax(pstSwupCheck);
	(void)xsvc_si_SwupCheckStartSection_Humax (pstSwupCheck);
}

STATIC void xsvc_si_SwupCheckMsgCmdStop_Humax(xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Debug("+++ \n");

	(void)xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC HERROR xsvc_si_SwupCheckProcessAll_Humax(xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR						hError;
	HUINT32						i;
	HUINT32						 ulSwupNum;
	xsvcSi_SwupCheckHumax_t		*astSwupArray;

	HxLOG_Error("+++ \n");

	hError = xsvc_si_SwupCheckFindSwupService_Humax(pstSwupCheck, &ulSwupNum, (void **)&astSwupArray);
	if(hError != ERR_OK ||ulSwupNum == 0 || astSwupArray == NULL)
	{
		return ERR_FAIL;
	}
	else
	{
		for(i = 0 ; i<ulSwupNum; i++ )
		{
			HUINT16	usPmtPid;

			hError = SVC_SI_GetPmtPidBySvcId(pstSwupCheck->stBase.hAction, astSwupArray[i].usSvcId, &usPmtPid);
			if(hError == ERR_OK)
			{

				HxLOG_Print ("[%s:%d] svc_si_SwupCheckStartFilter (SvcID : 0x%x, PmtPid : 0X%x) \n", __FUNCTION__, __LINE__, astSwupArray[i].usSvcId, usPmtPid);

				hError = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Pmt, (HUINT32)astSwupArray[i].usSvcId, (HUINT32)usPmtPid);
				if (ERR_OK != hError)
				{
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				}
			}
		}
	}

	pstSwupCheck->ulSwupNum = ulSwupNum;
	pstSwupCheck->astSwupInfo = &astSwupArray[0];

	xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
	xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);

	return ERR_OK;
}

STATIC void xsvc_si_SwupCheckMsgEvtPatTable_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HBOOL	bReceived = FALSE;
	HERROR	hError	= ERR_FAIL;

	HxLOG_Debug("+++ \n");

	hError = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	if(hError != ERR_OK)
	{
		HxLOG_Error("xsvc_si_LibSwupOnSiTableReceived_Base Error  (hError : 0x%x_ \n",  hError);

		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);

		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return ;
	}

	bReceived = svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase);
	if(bReceived == TRUE)
	{
		if (pstSwupCheck->stBase.stSwupOption.ulSwupMode == eSI_SWUP_MODE_HUMAX_HOME_TP)
		{
			hError = xsvc_si_SwupCheckProcessAll_Humax(pstSwupCheck, pstMsg);

			HxLOG_Print("[%s:%d] xsvc_si_SwupCheckProcessAll_Humax (hError : 0x%x)  \n", __FUNCTION__, __LINE__, hError);

			if(hError != ERR_OK)
			{
			 	svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}
		}
	}
	else
	{
		HxLOG_Error("svc_si_SwupCheckCheckAllSiTableReceived  Error  (bReceived : %d \n",  bReceived);
	}

	return;

}

STATIC void xsvc_si_SwupCheckMsgEvtPmtTable_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR							 hError = ERR_FAIL;
	xsvcSi_SwupCheckHumax_t			*astSwupHumaxInfo = NULL;
	HUINT32 				 		 ulNum = 0;
	HUINT32 						 ulExtId = SILIB_TABLE_EXT_ID_NOT;
	HUINT32							 i;

	HxLOG_Debug("+++ \n");

	hError = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	if(hError != ERR_OK)
	{
		HxLOG_Error("xsvc_si_LibSwupOnSiTableReceived_Base Error (0x%x) ..!! \n", hError);

		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

		return ;
	}

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_HUMAX_HOME_TP)
	{
		HxLOG_Error("Invalide eOtaMode(0x%x) ..!! \n", pstSwupCheck->stBase.stSwupOption.ulSwupMode);

		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

		return;
	}

	hError = xsvc_si_SwupCheckFindPidFromPmt_Humax(pstSwupCheck);
	if (hError != ERR_OK)
	{
		HxLOG_Error("xsvc_si_SwupCheckFindSsuInfoFromPmt_Humax() failed..!! \n");

		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
		if(pstSwupCheck->astSwupInfo != NULL)	{OxSI_Free(pstSwupCheck->astSwupInfo);}

		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);

		return;
	}

	ulNum = pstSwupCheck->ulSwupNum;
	astSwupHumaxInfo = pstSwupCheck->astSwupInfo;

	HxLOG_Print("\n[%s:%d] ulNum (%d)  \n\n\n", __FUNCTION__, __LINE__, ulNum);

	hError = ERR_FAIL;

	for (i = 0; i < ulNum; i++)
	{
#if	defined(HUMAX_SSU_SWUP_CHECK)
		HxLOG_Print("[%s:%d] Retrieve DSI (PID 0x%04x)\n", __FUNCTION__, __LINE__, astSwupHumaxInfo[i].usPid);
		hError = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Dsi, (HUINT32)ulExtId, (HUINT32)astSwupHumaxInfo[i].usPid);
#else
		HxLOG_Print("[%s:%d] Retrieve DIT (PID 0x%04x)\n", __FUNCTION__, __LINE__, astSwupHumaxInfo[i].usPid);
		hError = svc_si_SwupCheckStartFilter(&pstSwupCheck->stBase, eSIxTABLETYPE_Dit, (HUINT32)ulExtId, (HUINT32)astSwupHumaxInfo[i].usPid);
#endif
		if (ERR_OK != hError)
		{
			HxLOG_Error ("svc_si_SwupCheckStartFilter err: usExtId(%d), PID(%d)\n", ulExtId, astSwupHumaxInfo[i].usPid);
			xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
		}
	}

	if (ERR_OK != hError)
	{
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
	}

	if (astSwupHumaxInfo != NULL) 	{ OxSI_Free (astSwupHumaxInfo); }

	xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);

	return;
}

STATIC void xsvc_si_SwupCheckMsgEvtSdtTable_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HBOOL	bReceived = FALSE;
	HERROR	hError;

	HxLOG_Debug("+++ \n");

	hError = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	if(hError != ERR_OK)
	{
		HxLOG_Error("xsvc_si_LibSwupOnSiTableReceived_Base Error  (hError : 0x%x_ \n",  hError);

		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);

		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return ;
	}

	bReceived = svc_si_SwupCheckCheckAllSiTableReceived(&pstSwupCheck->stBase);
	if(bReceived == TRUE)
	{
		if (pstSwupCheck->stBase.stSwupOption.ulSwupMode == eSI_SWUP_MODE_HUMAX_HOME_TP)
		{
			hError = xsvc_si_SwupCheckProcessAll_Humax(pstSwupCheck, pstMsg);

			HxLOG_Print("[%s:%d] xsvc_si_SwupCheckProcessAll_Humax (hError : 0x%x)  \n", __FUNCTION__, __LINE__, hError);

			if(hError != ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
				return;
			}
		}
	}
	else
	{
		HxLOG_Error("svc_si_SwupCheckCheckAllSiTableReceived  Error  (bReceived : %d \n",  bReceived);
	}

	return;
}

#if defined(HUMAX_SSU_SWUP_CHECK)
STATIC void xsvc_si_SwupCheckMsgEvtDsiTable_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HUINT32				 ulNum = 0;
	SvcSi_SwupSignal_t	*astSwupInfo;
	HERROR				 hError;

	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_HUMAX_HOME_TP)
	{
		HxLOG_Debug("eSI_SWUP_MODE_HUMAX_HOME_TP Not \n");
		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return;
	}

	if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
	{
		HxLOG_Debug("eSI_TABLE_TIMEOUT \n");
		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return;
	}

	hError = xsvc_si_SwupCheckFindInfoFromDsi_Humax (pstSwupCheck, &ulNum, &astSwupInfo);
	if(hError != ERR_OK || ulNum == 0 || astSwupInfo == NULL)
	{
		xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
		svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		return;
	}

	// OTA 정보를 기록해 놓는다.
	pstSwupCheck->bFound = TRUE;
	HxSTD_memcpy(&pstSwupCheck->stSwupInfo, &astSwupInfo[0], sizeof(SvcSi_SwupSignal_t));

	xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Dsi, SILIB_TABLE_EXT_ID_ALL);
	svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, TRUE, 0, 0);

	return;
}
#else

STATIC void xsvc_si_SwupCheckMsgEvtDitTable_Humax (xsvcSi_SwupCheck_Humax_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HUINT32						 ulActionId = PAL_PIPE_GetActionId(pstSwupCheck->stBase.hAction);
	HERROR						 hErr = ERR_FAIL;
	HUINT32						 ulNumSsu = 0;

	HxLOG_Debug("+++ \n");

	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));

	if (pstSwupCheck->stBase.stSwupOption.ulSwupMode != eSI_SWUP_MODE_HUMAX_SSUSVC_TP)
	{
		return;
	}

	// ICT timeout
	if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_TIMEOUT)
	{
		if (PAL_SIRAWPOOL_CountRaws(ulActionId, eSIxTABLETYPE_Ict, FALSE, 0) == 0)
		{
			HxLOG_Print("[%s:%d] No ict table exist.. ! \n", __FUNCTION__, __LINE__);

			hErr = xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Dit, SILIB_TABLE_EXT_ID_ALL);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("stop ICT section failed..!! \n");
			}

			svc_si_PostMsgToAp(eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
		}
		return;
	}


	// SWUP 정보를 기록해 놓는다.
	pstSwupCheck->bFound = TRUE;

	// SWUP를 위해 받은 모든 ICT 를 Clear 한다.
	hErr = xsvc_si_SwupCheckStopSection_Humax(pstSwupCheck, eSIxTABLETYPE_Ict, SILIB_TABLE_EXT_ID_ALL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("stop ICT section failed..!! \n");
	}

	svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, TRUE, 0, 0);

	return;
}
#endif

STATIC void xsvc_si_SwupCheckProcessMsg_Humax (SvcSi_Msg_t *pstMsg, void *pvData)
{
	xsvcSi_SwupCheck_Humax_t	*pstSwupData = (xsvcSi_SwupCheck_Humax_t *)pvData;

	if (NULL == pstSwupData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_START_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStart_Humax(pstSwupData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_OTACHECK:
		HxLOG_Print("[%s:%d]eSIMSG_CMD_STOP_OTACHECK!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgCmdStop_Humax(pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PAT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPatTable_Humax (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_PMT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtPmtTable_Humax (pstSwupData, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_SDT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtSdtTable_Humax (pstSwupData, pstMsg);
		break;

#if defined(HUMAX_SSU_SWUP_CHECK)
	case eSIMSG_EVT_DSI_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_DSI_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtDsiTable_Humax (pstSwupData, pstMsg);
		break;
#endif

#if !defined(HUMAX_SSU_SWUP_CHECK)
	case eSIMSG_EVT_DIT_TABLE:
		HxLOG_Print("[%s:%d]eSIMSG_EVT_DIT_TABLE!\n",__FUNCTION__,__LINE__);
		xsvc_si_SwupCheckMsgEvtDitTable_Humax (pstSwupData, pstMsg);
		break;
#endif
	default:
		HxLOG_Debug("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}


#define ___LOCAL_SWUP_INFO_FUNCTIONS___
// API를 통해 OTA Signalling 결과를 알려주기 위한 함수들

STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfo_Humax (Handle_t hAction, HUINT32 *pulNumInfo, void **pavInfoArray)
{
	HUINT32						 ulActionId = PAL_PIPE_GetActionId(hAction);
	xsvcSi_SwupCheck_Humax_t	*pstSwupCheck = (xsvcSi_SwupCheck_Humax_t *)svc_si_GetSiData(ulActionId);

	if (pstSwupCheck->bFound != TRUE)		{ return ERR_FAIL; }

	if (pulNumInfo != NULL)
	{
		*pulNumInfo = 1;
	}

	if (pavInfoArray != NULL)
	{
		*pavInfoArray = (void *)&pstSwupCheck->stSwupInfo;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_SwupCheckGetSwupSignalInfoFromSvcDesc_Humax (Handle_t hAction, HUINT32 *pulNumInfo, void **pavInfoArray)
{
#if 0
	XsvcSi_SwupSignal_Humax_t	*pstSwupInfo;

	if (pstSwupData == NULL)					{ return ERR_SI_SOURCE_NULL; }

	if (pstSwupData->bFound != TRUE)		{ return ERR_SI_RESULT_FAIL; }

	if (pulNumInfo != NULL)
	{
		*pulNumInfo = 1;
	}

	if (pavInfoArray != NULL)
	{
		pstSwupInfo = (XsvcSi_SwupSignal_Humax_t *)OxSI_Malloc (sizeof(XsvcSi_SwupSignal_Humax_t));
		if (pstSwupInfo == NULL)				{ return ERR_SI_MEM_ALLOC_FAIL; }

		*pstSwupInfo = pstSwupData->stHumaxOtaInfo;
		*pavInfoArray = (void *)pstSwupInfo;
	}
#endif
	return ERR_OK;
}

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SwupCheckGetActionFunc_Humax (void)
{
	return xsvc_si_SwupCheckProcessMsg_Humax;
}

void *xsvc_si_SwupCheckAllocData_Humax (void)
{
	xsvcSi_SwupCheck_Humax_t		*astSwupCheckInfo;

	astSwupCheckInfo = (xsvcSi_SwupCheck_Humax_t *)OxSI_Calloc(sizeof(xsvcSi_SwupCheck_Humax_t));
	if (NULL != astSwupCheckInfo)
	{
		astSwupCheckInfo->stBase.eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return (void *)astSwupCheckInfo;
}

void xsvc_si_SwupCheckFreeData_Humax (void *pvData)
{
	xsvcSi_SwupCheck_Humax_t	*pstSwupCheck ;

	pstSwupCheck  = (xsvcSi_SwupCheck_Humax_t	*)pvData;

	if (NULL != pstSwupCheck )
	{
		if(pstSwupCheck->astSwupInfo != NULL)
		{
			OxSI_Free(pstSwupCheck->astSwupInfo);
			pstSwupCheck->astSwupInfo = NULL;
		}

		OxSI_Free(pstSwupCheck );
		pstSwupCheck  = NULL;
	}

	return;
}

HERROR xsvc_si_SwupCheckGetSwupCheckData_Humax(Handle_t hAction, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR						hErr = ERR_FAIL;
	XsvcSi_SwupDataType_e 		eDataType = (XsvcSi_SwupDataType_e)ulDataType;

	if (pulDataNum == NULL)				{ return ERR_FAIL; }
	if (ppvDataArray == NULL)			{ return ERR_FAIL; }

	HxLOG_Print("[%s:%d] eDataType (%d) \n",__FUNCTION__,__LINE__, eDataType);
	*pulDataNum = 0;
	*ppvDataArray = NULL;

	switch (eDataType)
	{
	case eSI_SWUP_DATA_HUMAX_SVC:
		hErr = xsvc_si_SwupCheckGetSwupSignalInfo_Humax(hAction, pulDataNum, ppvDataArray);
		break;

	case eSI_SWUP_DATA_HUMAX_DESC:
		hErr = xsvc_si_SwupCheckGetSwupSignalInfoFromSvcDesc_Humax(hAction, pulDataNum, ppvDataArray);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	HxLOG_Print("[%s:%d] hErr (%d) \n",__FUNCTION__,__LINE__, hErr);

	return hErr;
}

HERROR xsvc_si_SwupCheckFreeSwupCheckData_Humax (Handle_t hAction, HUINT32 ulDataType, void *pvDataArray)
{
	(void)hAction;
	(void)ulDataType;

	if (pvDataArray != NULL)
	{
		OxSI_Free (pvDataArray);
	}

	return ERR_OK;
}

/* end of file */

