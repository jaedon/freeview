/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_filtering.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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
#include <db_svc.h>

#include <pal_sirawpool.h>
#include <pal_sef.h>
#include <pal_pipe.h>

#include <svc_si.h>
#include <_svc_si_filtering.h>
#include "local_include/_svc_si_lib.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32			 ulActionId;
	HUINT32			 ulDemuxId;
	SIxTable_Type_e	 eTable;
	HUINT16			 usPid;
	HUINT16			 usExtId;
	HBOOL			 bUseExtId;

	HBOOL					 bMonitoring;
	PalSef_FilterMode_e		 ePalFilterMode;
	SvcSi_TableFilterMode_e	 eSiFilterMode;
	HUINT16			 usFilterId;

	HUINT32					 ulSameCount;
} svcSi_FilterTable_t;

typedef struct
{
	HBOOL			 bCustomed;
	HUINT32			 ulTimeout;
} svcSi_CustomTimeout_t;

typedef struct
{
	HxList_t				*apstFilterList[eSIxTABLETYPE_MAX];		// svcSi_FilterTable_t
	svcSi_CustomTimeout_t	 astCustomTimeout[eSIxTABLETYPE_MAX];

	unsigned long	 ulSemId;
} svcSi_FilterContext_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC svcSi_FilterContext_t		apst_sifilter_Contexts[eActionId_MAX];

#define ___LOCAL_FUNCTIONS___
INLINE STATIC svcSi_FilterContext_t *svc_si_FilterGetContext (HUINT32 ulActionId)
{
	return (ulActionId < eActionId_MAX) ? &(apst_sifilter_Contexts[ulActionId]) : NULL;
}

INLINE STATIC void svc_si_FilterEnterCritical (svcSi_FilterContext_t *pstContext)
{
	if (NULL != pstContext)
	{
		VK_SEM_Get (pstContext->ulSemId);
	}
}

INLINE STATIC void svc_si_FilterLeaveCritical (svcSi_FilterContext_t *pstContext)
{
	if (NULL != pstContext)
	{
		VK_SEM_Release (pstContext->ulSemId);
	}
}

STATIC svcSi_FilterTable_t *svc_si_FilterGetFilterTable (svcSi_FilterContext_t *pstContext, SIxTable_Type_e eTable, HBOOL bUseExtId, HUINT16 usExtId)
{
	HxList_t		*pstListItem;

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if ((bUseExtId == FALSE) ||
				((TRUE == pstFilter->bUseExtId) && (usExtId == pstFilter->usExtId)))
			{
				return pstFilter;
			}
		}
	}

	return NULL;
}

STATIC HERROR svc_si_FilterInsertFilterTable (svcSi_FilterContext_t *pstContext, SIxTable_Type_e eTable, svcSi_FilterTable_t *pstFilter)
{
	pstContext->apstFilterList[eTable] = HLIB_LIST_Append (pstContext->apstFilterList[eTable], (void *)pstFilter);
	return ERR_OK;
}

STATIC HERROR svc_si_FilterDeleteFilterTable (svcSi_FilterContext_t *pstContext, SIxTable_Type_e eTable, svcSi_FilterTable_t *pstFilter)
{
	pstContext->apstFilterList[eTable] = HLIB_LIST_Remove (pstContext->apstFilterList[eTable], (const void *)pstFilter);
	OxSI_Free (pstFilter);
	return ERR_OK;
}

STATIC HERROR svc_si_FilterStopFilter (HUINT32 ulActionId, SIxTable_Type_e eTable, HBOOL bUseExtId, HUINT16 usExtId)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	svcSi_FilterTable_t		*pstFilterTable = NULL;

	if (pstContext == NULL)
	{
		HxLOG_Error ("Invalid action id (%d) \n", ulActionId);
		return ERR_FAIL;
	}

	while (1)
	{
		pstFilterTable = svc_si_FilterGetFilterTable (pstContext, eTable, bUseExtId, usExtId);
		if (NULL == pstFilterTable)
		{
			HxLOG_Print ("[%s:%d] No Filter found: ActionId(%d), Table(%d), ExtID(%d)\n", __FUNCTION__, __LINE__, ulActionId, eTable, usExtId);
			break;
		}

		PAL_SEF_StopSecFilter (pstFilterTable->usFilterId);
		PAL_SIRAWPOOL_FreeRawSectionByUniqueId (ulActionId, eTable, pstFilterTable->usFilterId);

		svc_si_FilterDeleteFilterTable (pstContext, eTable, pstFilterTable);
	}

	return ERR_OK;
}

#define _____TABLE_RELATED_____


STATIC HBOOL svc_si_FilterIsMultiSection  (SIxTable_Type_e eTable)
{
	HBOOL					 bMultiSec = FALSE;

	switch (eTable)
	{
	case eSIxTABLETYPE_Pat:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_Pmt:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_NitAct:			bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_NitOth:			bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_Bat:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_SdtAct:			bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_SdtOth:			bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_EitActPf:		bMultiSec = TRUE;		break;			// 5 Sec
	case eSIxTABLETYPE_EitOthPf:		bMultiSec = TRUE;		break;			// 30 Sec
	case eSIxTABLETYPE_EitActSc:		bMultiSec = TRUE;		break;			// 15 min
	case eSIxTABLETYPE_EitOthSc:		bMultiSec = TRUE;		break;			// 60 min
	case eSIxTABLETYPE_Tdt:				bMultiSec = FALSE;		break;
	case eSIxTABLETYPE_Tot:				bMultiSec = FALSE;		break;
	case eSIxTABLETYPE_Cat:				bMultiSec = FALSE;		break;
	case eSIxTABLETYPE_Ait:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_Ict:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_Sgt:				bMultiSec = TRUE;		break;
	case eSIxTABLETYPE_Rct:				bMultiSec = TRUE;		break;
	default:
		break;
	}

	return bMultiSec;
}

#define ___MEMBER_FUNCTIONS___

// reference function - local_sidmx_GetDefaultTimeout()
HUINT32 svc_si_FilterGetDefaultTimeout  (SIxTable_Type_e eTable)
{
	HUINT32					 ulTimeout = 10000;					// Default Timeout:

	switch (eTable)
	{
	case eSIxTABLETYPE_Pat:				ulTimeout = 10000;		break;
	case eSIxTABLETYPE_Pmt:				ulTimeout = 10000;		break;
	case eSIxTABLETYPE_NitAct:			ulTimeout = 25000;		break;
	case eSIxTABLETYPE_NitOth:			ulTimeout = 25000;		break;
	case eSIxTABLETYPE_Bat:				ulTimeout = 35000;		break;
	case eSIxTABLETYPE_SdtAct:			ulTimeout = 35000;		break;
	case eSIxTABLETYPE_SdtOth:			ulTimeout = 35000;		break;
	case eSIxTABLETYPE_EitActPf:		ulTimeout = 5000;		break;			// 5 Sec
	case eSIxTABLETYPE_EitOthPf:		ulTimeout = 30000;		break;			// 30 Sec
	case eSIxTABLETYPE_EitActSc:		ulTimeout = 900000;		break;			// 15 min
	case eSIxTABLETYPE_EitOthSc:		ulTimeout = 3600000;	break;			// 60 min
	case eSIxTABLETYPE_Tdt:				ulTimeout = 30000;		break;
	case eSIxTABLETYPE_Tot:				ulTimeout = 30000;		break;
	case eSIxTABLETYPE_Cat:				ulTimeout = 10000;		break;
	case eSIxTABLETYPE_Ait:				ulTimeout = 5000;		break;
	case eSIxTABLETYPE_Ict:				ulTimeout = 30000;		break;
	case eSIxTABLETYPE_Sgt:				ulTimeout = 15000;		break;
	case eSIxTABLETYPE_Rct:				ulTimeout = 10000;		break;
	case eSIxTABLETYPE_Dsi:				ulTimeout = 10000;		break;	// ETSI TS 102 006 (V.1.2.1) 의 Page 34 기술 되어 있음.
	case eSIxTABLETYPE_Unt:
		// ETSI TS 102 006 (V.1.2.1) 의 Page 34 기술 되어 있음. 지상파와 그 외의 timeout이 다름. Combo 인 경우에는 개별 SI에서 Setting하도록 해 주세요..
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		ulTimeout = 60000;					
#else
		ulTimeout = 15000;
#endif	
		break;	
	default:
		break;
	}

	return ulTimeout;
}

HERROR svc_si_FilterInit (void)
{
	HUINT32				 ulActionId;
	HCHAR				 szSemName[32];
	int					 nRet;

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		svcSi_FilterContext_t	*pstContext = &(apst_sifilter_Contexts[ulActionId]);

		HxSTD_MemSet (pstContext->apstFilterList, 0, sizeof(HxList_t *) * eSIxTABLETYPE_MAX);
		HxSTD_PrintToStrN (szSemName, 31, "SvcSiFilt%02d", ulActionId);
		nRet = VK_SEM_Create (&(pstContext->ulSemId), (const char *)szSemName, VK_SUSPENDTYPE_FIFO);
		if (VK_OK != nRet)
		{
			HxLOG_Error ("VK_SEM_Create failed:\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HUINT32 svc_si_FilterGetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	svcSi_CustomTimeout_t	*pstTimeout;

	if ((NULL == pstContext) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return 0;
	}

	pstTimeout = &(pstContext->astCustomTimeout[eTable]);
	if (TRUE == pstTimeout->bCustomed)
	{
		return pstTimeout->ulTimeout;
	}

	return svc_si_FilterGetDefaultTimeout (eTable);
}

HERROR svc_si_FilterSetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT32 ulTimeout)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	svcSi_CustomTimeout_t	*pstTimeout;

	if ((NULL == pstContext) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return ERR_FAIL;
	}

	pstTimeout = &(pstContext->astCustomTimeout[eTable]);

	pstTimeout->bCustomed = TRUE;
	pstTimeout->ulTimeout = ulTimeout;

	return ERR_OK;
}

HERROR svc_si_FilterResetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);

	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	if (eSIxTABLETYPE_ALL == eTable)
	{
		for (eTable = 0; eTable < eSIxTABLETYPE_MAX; eTable++)
		{
			svc_si_FilterResetTimeout (ulActionId, eTable);
		}
	}
	else if (eTable < eSIxTABLETYPE_MAX)
	{
		svcSi_CustomTimeout_t	*pstTimeout = &(pstContext->astCustomTimeout[eTable]);

		pstTimeout->bCustomed = FALSE;
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_FilterStartTableSection (Handle_t hAction, HUINT32 ulDemuxId, SIxTable_Type_e eTable, HUINT32 ulExtId, HUINT32 ulPid, SvcSi_TableFilterMode_e eFilterMode, HUINT32 *pulFilterId)
{
	HBOOL					 bUseExtId;
	HBOOL					 bIsTable = svc_si_FilterIsMultiSection (eTable);
	HUINT8					 ucTableId = SILIB_TABLE_GetTableIdByTableType (eTable);
	HUINT16					 usExtId;
	HUINT32					 ulTimeout;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	PalSef_FilterParam_t	*pstFilterParam = NULL;
	PalSef_Callback_t		 pfParse = NULL, pfFree = NULL;
	svcSi_FilterTable_t		*pstFilterTable = NULL;
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HERROR					 hErr, hResult = ERR_FAIL;

	if (NULL != pulFilterId)					{ *pulFilterId = FILTER_ID_NULL; }

	if ((NULL == pstContext) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return ERR_FAIL;
	}

	pstFilterParam = (PalSef_FilterParam_t *)OxSI_Calloc (sizeof(PalSef_FilterParam_t));
	if (NULL == pstFilterParam)
	{
		return ERR_FAIL;
	}

	if (ulExtId < 0xFFFF)
	{
		bUseExtId = TRUE;
		usExtId = (HUINT16)ulExtId;
	}
	else
	{
		bUseExtId = FALSE;
		usExtId = 0;
	}

	pfParse	= svc_si_CbGetTableParseFunc (ulActionId, eTable);
	pfFree	= svc_si_CbGetTableFreeFunc (ulActionId, eTable);

	svc_si_FilterEnterCritical (pstContext);

	pstFilterTable = svc_si_FilterGetFilterTable (pstContext, eTable, bUseExtId, usExtId);
	if ((NULL != pstFilterTable) && (pstFilterTable->usPid == ulPid))	// 동일한 table type이라도 pid가 다를 수 있다
	{
		if (FILTER_ID_NULL != pstFilterTable->usFilterId)
		{
			PAL_SEF_StopSecFilter (pstFilterTable->usFilterId);
		}

		svc_si_FilterDeleteFilterTable (pstContext, eTable, pstFilterTable);
		pstFilterTable = NULL;
	}

	pstFilterTable = OxSI_Calloc (sizeof(svcSi_FilterTable_t));
	if (NULL == pstFilterTable)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto END_FUNC;
	}

	pstFilterTable->ulActionId	= ulActionId;
	pstFilterTable->ulDemuxId	= ulDemuxId;
	pstFilterTable->eTable		= eTable;
	pstFilterTable->usPid		= (HUINT16)ulPid;
	pstFilterTable->usExtId		= usExtId;
	pstFilterTable->bUseExtId	= bUseExtId;
	pstFilterTable->eSiFilterMode	= eFilterMode;

	switch (eFilterMode)
	{
		case eSI_TABLE_FILTER_MODE_Monitor:
			pstFilterTable->ePalFilterMode = eSiFilterMode_Monitor;
			break;

		case eSI_TABLE_FILTER_MODE_All:
			pstFilterTable->ePalFilterMode = eSiFilterMode_Continue;
			bIsTable = FALSE;
			break;

		case eSI_TABLE_FILTER_MODE_AllMonitor:
			pstFilterTable->ePalFilterMode = eSiFilterMode_Continue;
			bIsTable = FALSE;
			break;

		case eSI_TABLE_FILTER_MODE_Once:
			pstFilterTable->ePalFilterMode = eSiFilterMode_Once;
			break;

		default:
			pstFilterTable->ePalFilterMode = eSiFilterMode_Once;
			HxLOG_Error("invalid filter mode(%d) eTable(%d). set filter mode to ONCE ! \n", eFilterMode, eTable);
			break;
	}

	ulTimeout = svc_si_FilterGetTimeout (ulActionId, eTable);

	PAL_SEF_SiMakeDefaultFilterParam (pstFilterParam,
						ucTableId,								// TableID
						(HUINT8)pstFilterTable->ulDemuxId,		// DemuxId
						(HUINT16)pstFilterTable->ulActionId,	// UniqueID: ActionID로 해서 Callback에서 찾기 쉽게 하기 위함.
						pstFilterTable->usPid,					// PID
						bIsTable,								// Table Filter
						pstFilterTable->bUseExtId,				// UseExtId
						FALSE,									// UserVersionNum
						FALSE,									// CRC Check
						pstFilterTable->usExtId,				// ExtID
						0,										// VersionNum
						pstFilterTable->ePalFilterMode,							// PalSef_FilterMode_e
						eContinueTimeout_Once,
						ulTimeout,								// Timeout
						hAction,								// UserParam: hAction 을 통한 전체 Filter Stop용을 위함.
						pfParse,
						pfFree);


	if(eTable == eSIxTABLETYPE_Dsi)
	{
		// DSI 일 경우, 약간 달라져야 한다.
		pstFilterParam->eFilterLen = eSefFilterLen_16Bytes;
		pstFilterParam->pFilterValue[8] = 0x10;			pstFilterParam->pFilterMask[8] = 0xff;
		pstFilterParam->pFilterValue[9] = 0x06;			pstFilterParam->pFilterMask[9] = 0xff;
	}

	pstFilterTable->usFilterId = PAL_SEF_StartSecFilter (pstFilterParam);
	if (FILTER_ID_NULL == pstFilterTable->usFilterId)
	{
		HxLOG_Error ("FilterID=NULL in PAL_SEF_StartSecFilter: SiTable(%d), ExtID(%d)\n", eTable, usExtId);
		goto END_FUNC;
	}

	hErr = svc_si_FilterInsertFilterTable (pstContext, eTable, pstFilterTable);
	if (ERR_OK != hErr)
	{
		goto END_FUNC;
	}

	if (NULL != pulFilterId)					{ *pulFilterId = pstFilterTable->usFilterId; }

	pstFilterTable = NULL;
	hResult = ERR_OK;

END_FUNC:
	svc_si_FilterLeaveCritical (pstContext);

	if (NULL != pstFilterParam)					{ OxSI_Free (pstFilterParam); }
	if (NULL != pstFilterTable)					{ OxSI_Free (pstFilterTable); }
	return hResult;
}

HERROR svc_si_FilterStopTableSection (Handle_t hAction, SIxTable_Type_e eTable, HUINT32 ulExtId)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HERROR					 hResult = ERR_FAIL;

	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	if (eSIxTABLETYPE_ALL == eTable)
	{
		svc_si_FilterEnterCritical (pstContext);
		for (eTable = 0; eTable < eSIxTABLETYPE_MAX; eTable++)
		{
			svc_si_FilterStopFilter (ulActionId, eTable, FALSE, 0);
		}
		svc_si_FilterLeaveCritical (pstContext);

		hResult = ERR_OK;
	}
	else if (eTable < eSIxTABLETYPE_MAX)
	{
		HBOOL					 bUseExtId;
		HUINT16 				 usExtId;

		if (ulExtId < 0xFFFF)
		{
			bUseExtId = TRUE;
			usExtId = (HUINT16)ulExtId;
		}
		else
		{
			bUseExtId = FALSE;
			usExtId = 0;
		}

		svc_si_FilterEnterCritical (pstContext);
		hResult = svc_si_FilterStopFilter (ulActionId, eTable, bUseExtId, usExtId);
		svc_si_FilterLeaveCritical (pstContext);
	}
	else
	{
		hResult = ERR_FAIL;
	}

	return hResult;
}

HERROR svc_si_FilterGetPalFilterMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, PalSef_FilterMode_e *peFilterMode)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	if (peFilterMode == NULL)
	{
		goto END_FUNC;
	}

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				*peFilterMode = pstFilter->ePalFilterMode;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterGetSiFilterMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, SvcSi_TableFilterMode_e *peFilterMode)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				*peFilterMode = pstFilter->eSiFilterMode;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterGetFilterMonitorMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HBOOL *pbMonitor)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				if (NULL != pbMonitor)				{ *pbMonitor = pstFilter->bMonitoring; }
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterSetFilterMonitorMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HBOOL bMonitor)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				pstFilter->bMonitoring = bMonitor;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterGetFilterPid (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT16 *pusPid)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	if (pusPid == NULL)
	{
		goto END_FUNC;
	}

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				*pusPid = pstFilter->usPid;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterGetFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT32 *pulSameCount)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				if (NULL != pulSameCount)			{ *pulSameCount = pstFilter->ulSameCount; }
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterResetFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				pstFilter->ulSameCount = 0;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

HERROR svc_si_FilterIncreaseFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT32 *pulSameCount)
{
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	svc_si_FilterEnterCritical(pstContext);

	for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);

		if (NULL != pstFilter)
		{
			if (pstFilter->usFilterId == usFilterId)
			{
				pstFilter->ulSameCount ++;
				if (NULL != pulSameCount)			{ *pulSameCount = pstFilter->ulSameCount; }
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	svc_si_FilterLeaveCritical(pstContext);
	return hResult;
}

#define _____DEBUG_FUNCTIONS_____

#if defined(CONFIG_DEBUG)
HUINT32 svc_si_FilterPrintStatus(void)
{
	HUINT32					 ulActionId = eActionId_VIEW_FIRST;
	svcSi_FilterContext_t	*pstContext = svc_si_FilterGetContext (ulActionId);
	SIxTable_Type_e 		 eTable;
	HxList_t				*pstListItem;

	if (pstContext == NULL)
	{
		HLIB_CMD_Printf("invalid context - actionId(%d) ", ulActionId);
		return 1;
	}

	svc_si_FilterEnterCritical(pstContext);

	HLIB_CMD_Printf("\n");

	for (eTable = eSIxTABLETYPE_Pat; eTable < eSIxTABLETYPE_MAX; eTable++)
	{
		if (pstContext->apstFilterList[eTable] != NULL)
		{
			HLIB_CMD_Printf("[%s] \n", svc_si_LibGetTableTypeName(eTable));

			for (pstListItem = pstContext->apstFilterList[eTable]; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				svcSi_FilterTable_t			*pstFilter = HLIB_LIST_Data (pstListItem);
				if (NULL != pstFilter)
				{
					HLIB_CMD_Printf("\t actionId(%d) dmxId(%d) tableId(%d) pid(0x%X) \n", pstFilter->ulActionId, pstFilter->ulDemuxId, pstFilter->eTable, pstFilter->usPid);
					HLIB_CMD_Printf("\t usExtId(0x%X) bUseExtId(%d) eFilterMode(%d) usFilterId(0x%X)\n", pstFilter->usExtId, pstFilter->bUseExtId, pstFilter->ePalFilterMode, pstFilter->usFilterId);
				}
			}

			HLIB_CMD_Printf("\t Timeout(%d) ", svc_si_FilterGetTimeout(ulActionId, eTable));
			if (pstContext->astCustomTimeout[eTable].bCustomed == TRUE)
			{
				HLIB_CMD_Printf("(Custom)");
			}
			HLIB_CMD_Printf("\n");
		}
	}

	svc_si_FilterLeaveCritical(pstContext);

	return 0;
}
#endif

/* end of file */

