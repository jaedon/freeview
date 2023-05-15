/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_cb.c
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

#include <pal_sirawpool.h>
#include <pal_sef.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CUSTOMCB_FUNCNAME_LEN			63

#define SVCSI_SAME_TABLE_MAX_COUNT		(8)
#define SVCSI_SAME_TABLE_MAX_COUNT_MONITOR	(1)	// filter mode를 continue로 걸고 monitor할 때 사용


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32				 ulTableIdAndResult;
	HUINT32				 ulFilterAndUniqId;
	HUINT32				 ulParam;

	SIxTable_Type_e		 eTable;
	svcSi_MsgType_e		 eMsgType;
	HBOOL				 bUseExtId;
	HUINT32				 ulExtId;
} svcSi_Cb_Parse_Arg_t;

typedef struct
{
	HUINT32				 ulUniqId;
	HUINT32				 ulTableId;
	HUINT32				 ulExtVal;
	HUINT32				 ulParam2;

	SIxTable_Type_e		 eTable;
	HBOOL				 bUseExtId;
} svcSi_Cb_Free_Arg_t;


typedef struct
{
	SIxTable_Type_e		 eTable;

	HCHAR				 szCustomParseName[CUSTOMCB_FUNCNAME_LEN + 1];
	HCHAR				 szCustomFreeName[CUSTOMCB_FUNCNAME_LEN + 1];
	PalSef_Callback_t		 pfCustomParse;
	PalSef_Callback_t		 pfCustomFree;
} svcSi_CustomCb_t;

typedef struct
{
	HxList_t			*pstCustomCbList;			// svcSi_CustomCb_t
} svcSi_CustomCbContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC svcSi_CustomCbContext_t		 s_astSvcSi_CustomCbContext[eActionId_MAX];

#define ___LOCAL_FUNCTIONS___
INLINE STATIC svcSi_CustomCbContext_t *svc_si_CbGetContext (HUINT32 ulActionId)
{
	return (ulActionId < eActionId_MAX) ? &(s_astSvcSi_CustomCbContext[ulActionId]) : NULL;
}

STATIC svcSi_CustomCb_t *svc_si_CbGetCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	HxList_t				*pstListItem;
	svcSi_CustomCbContext_t	*pstContext = svc_si_CbGetContext(ulActionId);

	for (pstListItem = pstContext->pstCustomCbList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcSi_CustomCb_t	*pstCustomCb = (svcSi_CustomCb_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstCustomCb)
		{
			if (pstCustomCb->eTable == eTable)
			{
				return pstCustomCb;
			}
		}
	}

	return NULL;
}

STATIC HERROR svc_si_CbInsertCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable, PalSef_Callback_t pfParse, PalSef_Callback_t pfFree, HCHAR *szParseName, HCHAR *szFreeName)
{
	svcSi_CustomCb_t		*pstCustomCb = NULL;
	svcSi_CustomCbContext_t	*pstContext = svc_si_CbGetContext(ulActionId);

	pstCustomCb = (svcSi_CustomCb_t *)OxSI_Calloc (sizeof(svcSi_CustomCb_t));
	if (NULL == pstCustomCb)
	{
		return ERR_FAIL;
	}

	pstCustomCb->pfCustomParse		= pfParse;
	pstCustomCb->pfCustomFree		= pfFree;
	if ((NULL != szParseName) && ('\0' != szParseName[0]))
	{
		HxSTD_StrNCpy (pstCustomCb->szCustomParseName, szParseName, CUSTOMCB_FUNCNAME_LEN);
	}
	else
	{
		HxSTD_StrNCpy (pstCustomCb->szCustomParseName, "Unknown", CUSTOMCB_FUNCNAME_LEN);
	}

	if ((NULL != szFreeName) && ('\0' != szFreeName[0]))
	{
		HxSTD_StrNCpy (pstCustomCb->szCustomFreeName, szFreeName, CUSTOMCB_FUNCNAME_LEN);
	}
	else
	{
		HxSTD_StrNCpy (pstCustomCb->szCustomFreeName, "Unknown", CUSTOMCB_FUNCNAME_LEN);
	}

	pstContext->pstCustomCbList = HLIB_LIST_Append (pstContext->pstCustomCbList, (void *)pstCustomCb);

	return ERR_OK;
}


STATIC HERROR svc_si_CbDeleteCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	svcSi_CustomCb_t		*pstCustomCb = svc_si_CbGetCustomCallback (ulActionId, eTable);
	svcSi_CustomCbContext_t	*pstContext = svc_si_CbGetContext(ulActionId);

	if (NULL == pstCustomCb)
	{
		return ERR_FAIL;
	}

	pstContext->pstCustomCbList = HLIB_LIST_Remove (pstContext->pstCustomCbList, (const void *)pstCustomCb);
	OxSI_Free (pstCustomCb);

	return ERR_OK;
}

#define ____DEFAULT_TABLE_PARSING_CALLBACK_FUNCTIONS_____
STATIC HERROR svc_si_CbParseDefaultOneTable (Handle_t hAction, svcSi_Cb_Parse_Arg_t *pstArgs)
{
	HUINT32					 ulExtId;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32					 ulTableId, ulResult, ulFilterId, ulUniqueId;
	eFilterTableStatus_t	 eFilterState = eSI_TABLE_TIMEOUT;
	PalSef_RawData_t				*pstSiRawData = (PalSef_RawData_t *)pstArgs->ulParam;
	HERROR					 hErr, hResult = ERR_FAIL;

	ulResult	= (HUINT32)(pstArgs->ulTableIdAndResult & 0x0000ffff);
	ulTableId	= (HUINT32)((pstArgs->ulTableIdAndResult & 0xffff0000) >> 16);
	ulUniqueId	= (HUINT32)(pstArgs->ulFilterAndUniqId & 0x0000ffff);
	ulFilterId	= (HUINT32)((pstArgs->ulFilterAndUniqId & 0xffff0000) >> 16);

	if (eSiFilterResult_Complete != ulResult)
	{
		HxLOG_Debug ("Timeout: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if ((0 == pstSiRawData->num_sec) || (NULL == pstSiRawData->buf[0]))
	{
		HxLOG_Error ("No Section: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if (pstArgs->bUseExtId)
	{
		ulExtId = HxMACRO_Get16Bit (pstSiRawData->buf[0] + 3);
	}
	else
	{
		ulExtId = 0;
	}

	// DSI / UNT / DIT  같이 EXT ID 만으로 구분할 수 없는 Table 들은 PID를 ext ID 영역에 write 하기로 한다.
	if(pstArgs->eTable == eSIxTABLETYPE_Dsi || pstArgs->eTable == eSIxTABLETYPE_Unt || pstArgs->eTable == eSIxTABLETYPE_Dit)
	{
		HUINT16	usPid;

		hErr = svc_si_FilterGetFilterPid(ulActionId, pstArgs->eTable, ulFilterId, &usPid);
		if(hErr == ERR_OK)
		{
			pstSiRawData->buf[0][3] = (HUINT8)((usPid >> 8) & 0xff);
			pstSiRawData->buf[0][4] = (HUINT8)(usPid & 0xff);
		}
	}

	hErr = PAL_SIRAWPOOL_ReplaceMultiRawSection (ulActionId, pstArgs->eTable, pstArgs->bUseExtId, ulExtId,
										pstSiRawData->num_sec, pstSiRawData->buf, ulFilterId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ReplaceMultiRawSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	eFilterState = eSI_TABLE_RECEIVED;
	hResult = ERR_OK;

	NOT_USED_PARAM(ulTableId);
	NOT_USED_PARAM(ulUniqueId);

END_FUNC:
	svc_si_SendSectionMessage (pstArgs->eMsgType, hAction, eFilterState, ulFilterId);
	return hResult;
}

STATIC HERROR svc_si_CbParseDefaultAllTable (Handle_t hAction, svcSi_Cb_Parse_Arg_t *pstArgs)
{
	HUINT32					 ulExtId;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32					 ulTableId, ulResult, ulFilterId, ulUniqueId;
	eFilterTableStatus_t	 eFilterState = eSI_TABLE_TIMEOUT;
	PalSef_RawData_t		*pstSiRawData = (PalSef_RawData_t *)pstArgs->ulParam;
	HERROR					 hErr, hResult = ERR_FAIL;

	HUINT32			 		 ulSameCount = 0;

	ulResult	= (HUINT32)(pstArgs->ulTableIdAndResult & 0x0000ffff);
	ulTableId	= (HUINT32)((pstArgs->ulTableIdAndResult & 0xffff0000) >> 16);
	ulUniqueId	= (HUINT32)(pstArgs->ulFilterAndUniqId & 0x0000ffff);
	ulFilterId	= (HUINT32)((pstArgs->ulFilterAndUniqId & 0xffff0000) >> 16);

	if (eSiFilterResult_Complete != ulResult)
	{
		HxLOG_Debug ("Timeout: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if ((0 == pstSiRawData->num_sec) || (NULL == pstSiRawData->buf[0]))
	{
		HxLOG_Error ("No Section: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if (pstSiRawData->num_sec > 1)
	{
		// 이 함수로 multi section이 들어올 일이 있을까?? 이 에러가 찍히면 확인해 보자
		HxLOG_Error ("multi section...??? : Table(%d) num_sec(%d) \n", pstArgs->eTable, pstSiRawData->num_sec);
	}

	// all table retrieve 이므로 항상 extid 가져오자
	ulExtId = HxMACRO_Get16Bit (pstSiRawData->buf[0] + 3);

	if (PAL_SIRAWPOOL_FindSameRawSection(ulActionId, pstArgs->eTable, ulExtId, pstSiRawData->num_sec, pstSiRawData->buf) == ERR_OK)
	{
		// raw pool에서 동일한 section을 찾았다~! counter만 늘려주자
		hErr = PAL_SIRAWPOOL_IncreaseSameSectionCounter(ulActionId, &ulSameCount);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_IncreaseSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}
	}
	else
	{
		/* 여기서 초기화를 해주는 이유? 어쨌든 새로운 Section이 들어오면, count를 0부터 다시 시작 */
		// new raw section~ section counter를 초기화..
		ulSameCount = 0;
		hErr = PAL_SIRAWPOOL_ResetSameSectionCounter(ulActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ResetSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}

		// incomplete section이라고 raw pool에 넣어주자  : multi section를 여기서 add할 일은 없겠지만 대비 차원에서..
		hErr = PAL_SIRAWPOOL_AddMultiRawSection (ulActionId, pstArgs->eTable, pstSiRawData->num_sec, pstSiRawData->buf, ulFilterId, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ReplaceMultiRawSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
			eFilterState = eSI_TABLE_TIMEOUT;
			goto END_FUNC;
		}

		// 현재 table section을 다 받았으면 완료됐다고 적어주자
		hErr = PAL_SIRAWPOOL_FinalizeIncompleteSection(ulActionId, pstArgs->eTable, ulExtId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_FinalizeIncompleteSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
		}
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("TID = 0x%02x, EXT_ID = 0x%04x, ulSameCount = %d\n\n"), ulTableId, ulExtId, ulSameCount);

	if (ulSameCount >= SVCSI_SAME_TABLE_MAX_COUNT)
	{
		// raw pool에 저장된 table과 동일한 table이 연속으로 들어왔당. 이제 고만~
		eFilterState = eSI_TABLE_RECEIVED;
		hErr = PAL_SIRAWPOOL_ResetSameSectionCounter(ulActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ResetSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}

		// section filter는 stop 시켜준다
		PAL_SEF_StopSecFilter (ulFilterId);
	}
	else
	{
		eFilterState = eSI_TABLE_RUNING;
	}
	hResult = ERR_OK;

	NOT_USED_PARAM(ulUniqueId);

END_FUNC:


	// section 받을 때마다 날리지 말고 모든 table 받았을 때만 msg 날리자 (timeout일때는 항상)
	if ((eFilterState == eSI_TABLE_RECEIVED) || (eFilterState == eSI_TABLE_TIMEOUT))
	{
		svc_si_SendSectionMessage (pstArgs->eMsgType, hAction, eFilterState, ulFilterId);
	}

	return hResult;
}

STATIC HERROR svc_si_CbParseDefaultBatTable (Handle_t hAction, svcSi_Cb_Parse_Arg_t *pstArgs)
{
	HUINT32					 ulExtId;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32					 ulTableId, ulResult, ulFilterId, ulUniqueId;
	eFilterTableStatus_t	 eFilterState = eSI_TABLE_TIMEOUT;
	PalSef_RawData_t		*pstSiRawData = (PalSef_RawData_t *)pstArgs->ulParam;
	HERROR					 hErr, hResult = ERR_FAIL;
	HBOOL					 bFoundNewSection = FALSE;

	//HUINT32			 		 ulSameCount = 0;

	ulResult	= (HUINT32)(pstArgs->ulTableIdAndResult & 0x0000ffff);
	ulTableId	= (HUINT32)((pstArgs->ulTableIdAndResult & 0xffff0000) >> 16);
	ulUniqueId	= (HUINT32)(pstArgs->ulFilterAndUniqId & 0x0000ffff);
	ulFilterId	= (HUINT32)((pstArgs->ulFilterAndUniqId & 0xffff0000) >> 16);

	if (eSiFilterResult_Complete != ulResult)
	{
		HxLOG_Debug ("Timeout: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if ((0 == pstSiRawData->num_sec) || (NULL == pstSiRawData->buf[0]))
	{
		HxLOG_Error ("No Section: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if (pstSiRawData->num_sec > 1)
	{
		// 이 함수로 multi section이 들어올 일이 있을까?? 이 에러가 찍히면 확인해 보자
		HxLOG_Error ("multi section...??? : Table(%d) num_sec(%d) \n", pstArgs->eTable, pstSiRawData->num_sec);
	}

	// all table retrieve 이므로 항상 extid 가져오자
	ulExtId = HxMACRO_Get16Bit (pstSiRawData->buf[0] + 3);

	if (PAL_SIRAWPOOL_FindSameRawSection(ulActionId, pstArgs->eTable, ulExtId, pstSiRawData->num_sec, pstSiRawData->buf) == ERR_OK)
	{
		// raw pool에서 동일한 section을 찾았다~! counter만 늘려주자
#if 0
		hErr = PAL_SIRAWPOOL_IncreaseSameSectionCounter(ulActionId, &ulSameCount);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_IncreaseSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}
#endif
	}
	else
	{
		// 새로운 section을 찾았다
		bFoundNewSection = TRUE;

		// new raw section~ section counter를 초기화..
#if 0
		ulSameCount = 0;
		hErr = PAL_SIRAWPOOL_ResetSameSectionCounter(ulActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ResetSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}
#endif

		// incomplete section이라고 raw pool에 넣어주자  : multi section를 여기서 add할 일은 없겠지만 대비 차원에서..
		hErr = PAL_SIRAWPOOL_AddMultiRawSection (ulActionId, pstArgs->eTable, pstSiRawData->num_sec, pstSiRawData->buf, ulFilterId, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ReplaceMultiRawSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
			eFilterState = eSI_TABLE_TIMEOUT;
			goto END_FUNC;
		}

		// 현재 table section을 다 받았으면 완료됐다고 적어주자
		hErr = PAL_SIRAWPOOL_FinalizeIncompleteSection(ulActionId, pstArgs->eTable, ulExtId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_FinalizeIncompleteSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
		}
	}

#if 1
	/**
	  *		IR4000HD에서 BAT를 처리하기 위한 임시 코드
	  *		현재는 Live SI에서만 BAT를 ALL로 걸고 있어서 여기로는 Live SI만 온다고 가정한다
	  *		아니라면 수정 필요함
	  *
	  *		새로운 BAT section을 받을 때마다 rcv로 올려준다.
	  *		그리고 version check를 하기 위해 filter stop은 따로 하지 않는다
	**/
	eFilterState = (bFoundNewSection == TRUE) ? eSI_TABLE_RECEIVED : eSI_TABLE_RUNING;
#else
	if (ulSameCount >= SVCSI_SAME_TABLE_MAX_COUNT)
	{
		// raw pool에 저장된 table과 동일한 table이 연속으로 들어왔당. 이제 고만~
		eFilterState = eSI_TABLE_RECEIVED;
		hErr = PAL_SIRAWPOOL_ResetSameSectionCounter(ulActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ResetSameSectionCounter err: ulActionId(%d)\n", ulActionId);
		}

		// section filter는 stop 시켜준다
		PAL_SEF_StopSecFilter (ulFilterId);
	}
	else
	{
		eFilterState = eSI_TABLE_RUNING;
	}

#endif

	hResult = ERR_OK;

	NOT_USED_PARAM(ulTableId);
	NOT_USED_PARAM(ulUniqueId);

END_FUNC:

	// section 받을 때마다 날리지 말고 모든 table 받았을 때만 msg 날리자 (timeout일때는 항상)
	if ((eFilterState == eSI_TABLE_RECEIVED) || (eFilterState == eSI_TABLE_TIMEOUT))
	{
		svc_si_SendSectionMessage (pstArgs->eMsgType, hAction, eFilterState, ulFilterId);
	}

	return hResult;
}

/**
  *		filter mode가 continue이고 계속 monitoring을 해야 할 경우 이 함수를 사용
  *		svc_si_CbParseDefaultAllTable()와 유사하나 차이점은 아래와 같음
  *		 -same counter가 max에 도달해도 counter reset 및 filter stop을 하지 않음 (live si stop될 때 filter stop될 것임)
  *		 -동일한 section을 받았을 때 이미 same counter가 max라면 monitor 상태로 간주하고 table rcv msg를 보내지 않음
  *		   (새로운 section을 받았다면 same counter가 reset되면서 다시 동작할 것임)
**/
STATIC HERROR svc_si_CbParseDefaultAllMonitorTable (Handle_t hAction, svcSi_Cb_Parse_Arg_t *pstArgs)
{
	HBOOL					 bFoundSameSection = FALSE;
	HUINT32 				 ulExtId;
	HUINT32 				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32 				 ulTableId, ulResult, ulFilterId, ulUniqueId;
	eFilterTableStatus_t	 eFilterState = eSI_TABLE_TIMEOUT;
	PalSef_RawData_t		*pstSiRawData = (PalSef_RawData_t *)pstArgs->ulParam;
	HERROR					 hErr, hResult = ERR_FAIL;

	HUINT32 				 ulSameCount = 0;
	HBOOL					 bMonitorState = FALSE;

	ulResult	= (HUINT32)(pstArgs->ulTableIdAndResult & 0x0000ffff);
	ulTableId	= (HUINT32)((pstArgs->ulTableIdAndResult & 0xffff0000) >> 16);
	ulUniqueId	= (HUINT32)(pstArgs->ulFilterAndUniqId & 0x0000ffff);
	ulFilterId	= (HUINT32)((pstArgs->ulFilterAndUniqId & 0xffff0000) >> 16);

	if (eSiFilterResult_Complete != ulResult)
	{
		HxLOG_Debug ("Timeout: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if ((0 == pstSiRawData->num_sec) || (NULL == pstSiRawData->buf[0]))
	{
		HxLOG_Error ("No Section: Table(%d)\n", pstArgs->eTable);
		eFilterState = eSI_TABLE_TIMEOUT;
		goto END_FUNC;
	}

	if (pstSiRawData->num_sec > 1)
	{
		// 이 함수로 multi section이 들어올 일이 있을까?? 이 에러가 찍히면 확인해 보자
		HxLOG_Error ("multi section...??? : Table(%d) num_sec(%d) \n", pstArgs->eTable, pstSiRawData->num_sec);
	}

	// all table retrieve 이므로 항상 extid 가져오자
	ulExtId = HxMACRO_Get16Bit (pstSiRawData->buf[0] + 3);

	// Monitor State 여부를 확인해서 동작이 달라야 한다.
	hErr = svc_si_FilterGetFilterMonitorMode(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId, &bMonitorState);
		if (ERR_OK != hErr)
		{
		HxLOG_Error ("svc_si_FilterGetFilterMonitorMode err: ulActionId(%d)\n", ulActionId);
		}

	hErr = PAL_SIRAWPOOL_FindSameRawSection(ulActionId, pstArgs->eTable, ulExtId, pstSiRawData->num_sec, pstSiRawData->buf);
	bFoundSameSection = (ERR_OK == hErr);

	if (hErr == ERR_OK)
		{
		// 같은 Section이 존재 여부 : SameCount를 늘려주어서 Monitor mode 여부를 확인한다.
		//if (TRUE != bMonitorState)
		{
			// 아직 monitor 상태가 아니다. counter를 늘려준다
			hErr = svc_si_FilterIncreaseFilterSameCount(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId, &ulSameCount);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_FilterIncreaseFilterSameCount err: ulActionId(%d)\n", ulActionId);
			}
		}
	}
	else
	{
		/* 여기서 초기화를 해주는 이유? 어쨌든 새로운 Section이 들어오면, count를 0부터 다시 시작 */
		// new raw section~ section counter를 초기화..
		if (TRUE == bMonitorState)
		{
			PAL_SIRAWPOOL_FreeRawSection(ulActionId, pstArgs->eTable, pstArgs->bUseExtId, pstArgs->ulExtId);
			bMonitorState = FALSE;
			hErr = svc_si_FilterSetFilterMonitorMode(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId, bMonitorState);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_FilterSetFilterMonitorMode err: ulActionId(%d)\n", ulActionId);
			}
		}

		ulSameCount = 0;
		hErr = svc_si_FilterResetFilterSameCount(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("svc_si_FilterResetFilterSameCount err: ulActionId(%d)\n", ulActionId);
		}

		// incomplete section이라고 raw pool에 넣어주자  : multi section를 여기서 add할 일은 없겠지만 대비 차원에서..
		hErr = PAL_SIRAWPOOL_AddMultiRawSection (ulActionId, pstArgs->eTable, pstSiRawData->num_sec, pstSiRawData->buf, ulFilterId, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ReplaceMultiRawSection err: Table(%d), ExtID(%d)\n", pstArgs->eTable, ulExtId);
			eFilterState = eSI_TABLE_TIMEOUT;
			goto END_FUNC;
		}
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("TID = 0x%02x, EXT_ID = 0x%04x, ulSameCount = %d\n\n"), ulTableId, ulExtId, ulSameCount);

	hErr = svc_si_FilterGetFilterSameCount(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId, &ulSameCount);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_FilterGetFilterSameCount err: ulActionId(%d)\n", ulActionId);
	}

	eFilterState = eSI_TABLE_RUNING;

#if 0
		{
			HUINT32 		 ulIndex;
			HUINT32 		 ulSecLen, ulCrc32;
			HUINT32 		 ulVerNum, ulSecNum, ulLastSecNum;

			if (bMonitorState == FALSE)
			{
				for (ulIndex = 0; ulIndex < pstSiRawData->num_sec; ulIndex++)
				{
					HUINT8		*pucRawSec = pstSiRawData->buf[ulIndex];

					if (NULL != pucRawSec)
					{
						ulExtId = HxMACRO_Get16Bit(pucRawSec + 3);
						ulVerNum = (HUINT32)((pucRawSec[5] >> 1) & 0x1F);
						ulSecNum = (HUINT32)pucRawSec[6];
						ulLastSecNum = (HUINT32)pucRawSec[7];
						ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRawSec + 1);
						ulCrc32 = HxMACRO_Get32Bit(pucRawSec + ulSecLen - 1);

						HxLOG_Warning(">>> Sec: ExtId(0x%04x), ver=%d, secNum=%d, lastSec=%d, CRC32(0x%08x), mon(%d), sameCnt(%d), new(%d)\n", ulExtId, ulVerNum, ulSecNum, ulLastSecNum, ulCrc32, bMonitorState, ulSameCount, bFoundSameSection);
					}
				}
			}
		}
#endif
	if ((bMonitorState == FALSE)
		&& (ulSameCount >= SVCSI_SAME_TABLE_MAX_COUNT_MONITOR))
	{
		hErr = PAL_SIRAWPOOL_FinalizeIncompleteSection(ulActionId, pstArgs->eTable, SILIB_TABLE_EXT_ID_ALL);
		if (ERR_OK == hErr)
		{
			eFilterState = eSI_TABLE_RECEIVED;
			hErr = svc_si_FilterSetFilterMonitorMode(ulActionId, pstArgs->eTable, (HUINT16)ulFilterId, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("svc_si_FilterSetFilterMonitorMode err: ulActionId(%d)\n", ulActionId);
			}
		}
	}

	hResult = ERR_OK;

	NOT_USED_PARAM(ulUniqueId);
	NOT_USED_PARAM(bFoundSameSection);

END_FUNC:
	// section 받을 때마다 날리지 말고 모든 table 받았을 때만 msg 날리자 (timeout일때는 항상)
	if ((eFilterState == eSI_TABLE_RECEIVED) || (eFilterState == eSI_TABLE_TIMEOUT))
	{
		svc_si_SendSectionMessage (pstArgs->eMsgType, hAction, eFilterState, ulFilterId);
	}

	return hResult;
}

STATIC void svc_si_CbParsePatTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Pat;
	stArgs.eMsgType				= eSIMSG_EVT_PAT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParsePmtTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Pmt;
	stArgs.eMsgType				= eSIMSG_EVT_PMT_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseNitActTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_NitAct;
	stArgs.eMsgType				= eSIMSG_EVT_NIT_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseNitOthTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	HUINT16 				 usFilterId;
	SvcSi_TableFilterMode_e	 eFilterMode = 0xff;	// invalid value

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_NitOth;
	stArgs.eMsgType				= eSIMSG_EVT_NIT_OTH_TABLE;
	stArgs.bUseExtId			= TRUE;

	usFilterId = (HUINT16)((stArgs.ulFilterAndUniqId & 0xffff0000) >> 16);
	hErr = svc_si_FilterGetSiFilterMode(PAL_PIPE_GetActionId(hAction), stArgs.eTable, usFilterId, &eFilterMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_FilterGetFilterMode() failed.. hAction(0x%X) filterId(0x%X)\n", hAction, usFilterId);
		eFilterMode = eSI_TABLE_FILTER_MODE_Once;
	}

	switch (eFilterMode)
	{
	case eSI_TABLE_FILTER_MODE_All:
			hErr = svc_si_CbParseDefaultAllTable (hAction, &stArgs);
			break;

	case eSI_TABLE_FILTER_MODE_AllMonitor:
		hErr = svc_si_CbParseDefaultAllMonitorTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Once:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Monitor:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

		default:
			HxLOG_Error("invalid filter mode(%d) ", eFilterMode);
			hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseSdtActTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_SdtAct;
	stArgs.eMsgType				= eSIMSG_EVT_SDT_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseSdtOthTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;
	HUINT16					 usFilterId;
	SvcSi_TableFilterMode_e	 eFilterMode = 0xff;	// invalid value

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_SdtOth;
	stArgs.eMsgType				= eSIMSG_EVT_SDT_OTH_TABLE;
	stArgs.bUseExtId			= TRUE;

	usFilterId = (HUINT16)((stArgs.ulFilterAndUniqId & 0xffff0000) >> 16);
	hErr = svc_si_FilterGetSiFilterMode(PAL_PIPE_GetActionId(hAction), stArgs.eTable, usFilterId, &eFilterMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_FilterGetFilterMode() failed.. hAction(0x%X) filterId(0x%X)\n", hAction, usFilterId);
		eFilterMode = eSI_TABLE_FILTER_MODE_Once;
	}

	switch (eFilterMode)
	{
	case eSI_TABLE_FILTER_MODE_All:
			hErr = svc_si_CbParseDefaultAllTable (hAction, &stArgs);
			break;

	case eSI_TABLE_FILTER_MODE_AllMonitor:
		hErr = svc_si_CbParseDefaultAllMonitorTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Once:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Monitor:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

		default:
			HxLOG_Error("invalid filter mode(%d) ", eFilterMode);
			hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseEitActPfTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_EitActPf;
	stArgs.eMsgType				= eSIMSG_EVT_EIT_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseEitOthPfTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_EitOthPf;
	stArgs.eMsgType				= eSIMSG_EVT_EIT_OTH_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseEitActScTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_EitActSc;
	stArgs.eMsgType				= eSIMSG_EVT_EITSCH_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseEitOthScTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_EitOthSc;
	stArgs.eMsgType				= eSIMSG_EVT_EITSCH_OTH_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseBatTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;
	HUINT16					 usFilterId;
	SvcSi_TableFilterMode_e	 eFilterMode = 0xff;	// invalid value

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Bat;
	stArgs.eMsgType				= eSIMSG_EVT_BAT_TABLE;
	stArgs.bUseExtId			= FALSE;

	usFilterId = (HUINT16)((stArgs.ulFilterAndUniqId & 0xffff0000) >> 16);
	hErr = svc_si_FilterGetSiFilterMode(PAL_PIPE_GetActionId(hAction), stArgs.eTable, usFilterId, &eFilterMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_FilterGetFilterMode() failed.. hAction(0x%X) filterId(0x%X)\n", hAction, usFilterId);
		eFilterMode = eSI_TABLE_FILTER_MODE_Once;
	}

	switch (eFilterMode)
	{
	case eSI_TABLE_FILTER_MODE_All:
		hErr = svc_si_CbParseDefaultAllTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_AllMonitor:
		hErr = svc_si_CbParseDefaultAllMonitorTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Once:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Monitor:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;

		default:
			HxLOG_Error("invalid filter mode(%d) ", eFilterMode);
			hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseTdtTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Tdt;
	stArgs.eMsgType				= eSIMSG_EVT_TDT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseTotTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Tot;
	stArgs.eMsgType				= eSIMSG_EVT_TOT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseAitTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Ait;
	stArgs.eMsgType				= eSIMSG_EVT_AIT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseIctTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Ict;
	stArgs.eMsgType				= eSIMSG_EVT_ICT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseSgtTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;
	HUINT16					 usFilterId;
	SvcSi_TableFilterMode_e	 eFilterMode = 0xff;	// invalid value

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Sgt;
	stArgs.eMsgType				= eSIMSG_EVT_SGT_TABLE;
	stArgs.bUseExtId			= FALSE;

	usFilterId = (HUINT16)((stArgs.ulFilterAndUniqId & 0xffff0000) >> 16);
	hErr = svc_si_FilterGetSiFilterMode(PAL_PIPE_GetActionId(hAction), stArgs.eTable, usFilterId, &eFilterMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_FilterGetFilterMode() failed.. hAction(0x%X) filterId(0x%X)\n", hAction, usFilterId);
		eFilterMode = eSI_TABLE_FILTER_MODE_Once;
	}

	switch (eFilterMode)
	{
	case eSI_TABLE_FILTER_MODE_All:
			hErr = svc_si_CbParseDefaultAllTable (hAction, &stArgs);
			break;

	case eSI_TABLE_FILTER_MODE_AllMonitor:
		hErr = svc_si_CbParseDefaultAllMonitorTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Once:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Monitor:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

		default:
			HxLOG_Error("invalid filter mode(%d) ", eFilterMode);
			hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseDsiTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Dsi;
	stArgs.eMsgType				= eSIMSG_EVT_DSI_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseUntTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Unt;
	stArgs.eMsgType				= eSIMSG_EVT_UNT_TABLE;
	stArgs.bUseExtId			= FALSE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}

STATIC void svc_si_CbParseCatTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Cat;
	stArgs.eMsgType				= eSIMSG_EVT_CAT_TABLE;
	stArgs.bUseExtId			= TRUE;

	hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}
STATIC void svc_si_CbParseRctTable (HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	svcSi_Cb_Parse_Arg_t	 stArgs;
	HERROR					 hErr;
	HUINT16 				 usFilterId;
	SvcSi_TableFilterMode_e	 eFilterMode = 0xff;	// invalid value

	stArgs.ulTableIdAndResult	= ulTableIdAndResult;
	stArgs.ulFilterAndUniqId	= ulFilterAndUniqId;
	stArgs.ulParam				= ulParam;
	stArgs.eTable				= eSIxTABLETYPE_Rct;
	stArgs.eMsgType				= eSIMSG_EVT_RCT_TABLE;
	stArgs.bUseExtId			= FALSE;

	usFilterId = (HUINT16)((stArgs.ulFilterAndUniqId & 0xffff0000) >> 16);
	hErr = svc_si_FilterGetSiFilterMode(PAL_PIPE_GetActionId(hAction), stArgs.eTable, usFilterId, &eFilterMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_FilterGetFilterMode() failed.. hAction(0x%X) filterId(0x%X)\n", hAction, usFilterId);
		eFilterMode = eSI_TABLE_FILTER_MODE_Once;
	}

	switch (eFilterMode)
	{
	case eSI_TABLE_FILTER_MODE_All:
		hErr = svc_si_CbParseDefaultAllTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_AllMonitor:
			hErr = svc_si_CbParseDefaultAllMonitorTable (hAction, &stArgs);
			break;

	case eSI_TABLE_FILTER_MODE_Once:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

	case eSI_TABLE_FILTER_MODE_Monitor:
		hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
		break;

		default:
			HxLOG_Error("invalid filter mode(%d) ", eFilterMode);
			hErr = svc_si_CbParseDefaultOneTable (hAction, &stArgs);
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Warning ("Parse Error: (timeout?) - hAction(0x%X)\n", hAction);
	}
}


#define ____TABLE_CALLBACK_FUNCTIONS_____
STATIC PalSef_Callback_t svc_si_CbGetDefaultTableParseFunc (SIxTable_Type_e eTable)
{
	switch (eTable)
	{
	case eSIxTABLETYPE_Pat:
		return  svc_si_CbParsePatTable;

	case eSIxTABLETYPE_Pmt:
		return  svc_si_CbParsePmtTable;

	case eSIxTABLETYPE_NitAct:
		return  svc_si_CbParseNitActTable;

	case eSIxTABLETYPE_NitOth:
		return  svc_si_CbParseNitOthTable;

	case eSIxTABLETYPE_SdtAct:
		return  svc_si_CbParseSdtActTable;

	case eSIxTABLETYPE_SdtOth:
		return  svc_si_CbParseSdtOthTable;

	case eSIxTABLETYPE_EitActPf:
		return	svc_si_CbParseEitActPfTable;

	case eSIxTABLETYPE_EitOthPf:
		return	svc_si_CbParseEitOthPfTable;

	case eSIxTABLETYPE_EitActSc:
		return	svc_si_CbParseEitActScTable;

	case eSIxTABLETYPE_EitOthSc:
		return	svc_si_CbParseEitOthScTable;

	case eSIxTABLETYPE_Bat:
		return	svc_si_CbParseBatTable;

	case eSIxTABLETYPE_Tdt:
		return	svc_si_CbParseTdtTable;

	case eSIxTABLETYPE_Tot:
		return	svc_si_CbParseTotTable;

	case eSIxTABLETYPE_Ait:
		return	svc_si_CbParseAitTable;

	case eSIxTABLETYPE_Ict:
		return	svc_si_CbParseIctTable;

	case eSIxTABLETYPE_Sgt:
		return	svc_si_CbParseSgtTable;

	case eSIxTABLETYPE_Dsi:
		return	svc_si_CbParseDsiTable;

	case eSIxTABLETYPE_Unt:
		return	svc_si_CbParseUntTable;

	case eSIxTABLETYPE_Cat:
		return	svc_si_CbParseCatTable;

	case eSIxTABLETYPE_Rct:
		return	svc_si_CbParseRctTable;

	default:
		break;
	}

	return NULL;
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_si_CbInit (void)
{
	HxSTD_MemSet (s_astSvcSi_CustomCbContext, 0, sizeof(s_astSvcSi_CustomCbContext));

	return ERR_OK;
}

PalSef_Callback_t svc_si_CbGetTableParseFunc (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	svcSi_CustomCb_t		*pstCustomCb;

	if ((ulActionId >= eActionId_MAX) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return NULL;
	}

	pstCustomCb = svc_si_CbGetCustomCallback (ulActionId, eTable);
	if (NULL != pstCustomCb)
	{
		HxLOG_Debug ("Custom Callback : eTable=%d, name=\"%s\"\n", eTable, pstCustomCb->szCustomParseName);
		return pstCustomCb->pfCustomParse;
	}

	return svc_si_CbGetDefaultTableParseFunc (eTable);
}

PalSef_Callback_t svc_si_CbGetTableFreeFunc (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	svcSi_CustomCb_t		*pstCustomCb;

	if ((ulActionId >= eActionId_MAX) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return NULL;
	}

	pstCustomCb = svc_si_CbGetCustomCallback (ulActionId, eTable);
	if (NULL != pstCustomCb)
	{
		HxLOG_Debug ("Custom Free Callback : eTable=%d, name=\"%s\"\n", eTable, pstCustomCb->szCustomFreeName);
		return pstCustomCb->pfCustomFree;
	}

	// Custom FreeCallback 외의 Default Free Function은 제공하지 않는다.  Filter를 Stop하는 시점에서 자신이 직접 그 Filter로 들어간 RAW를 빼도록 한다.
	return NULL;
}

HERROR svc_si_CbRegisterCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable, PalSef_Callback_t pfParse, PalSef_Callback_t pfFree, HCHAR *szParseName, HCHAR *szFreeName)
{
	HERROR			 hErr;

	if ((ulActionId >= eActionId_MAX) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return ERR_FAIL;
	}

	if (NULL == pfParse)				{ pfParse	= svc_si_CbGetDefaultTableParseFunc (eTable); };

	(void)svc_si_CbDeleteCustomCallback (ulActionId, eTable);

	hErr = svc_si_CbInsertCustomCallback (ulActionId, eTable, pfParse, pfFree, szParseName, szFreeName);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_CbInsertCustomCallback err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_CbUnregisterCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable)
{
	HERROR			 hErr;

	if ((ulActionId >= eActionId_MAX) || (eTable >= eSIxTABLETYPE_MAX))
	{
		return ERR_FAIL;
	}

	hErr = svc_si_CbDeleteCustomCallback (ulActionId, eTable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_CbDeleteCustomCallback err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
/* end of file */

