/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_search_ertelecom.c
	@brief

	Description:  									\n
	Module: plugin\xtvservice\xsvc\si\ertelecom		\n

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
#include <_xsvc_si_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ERTEL_ORIGINAL_NETWORK_ID	0x0001

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

#if 0
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

STATIC HERROR xsvc_si_SrchCheckAndComposeServiceInfoBySdt_Ertelecom (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulSvcNum = 0, ulPrvNum = 0;
	HxList_t				*pstSdtSvcLoop, *pstSdtSvcDesLoop;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;
	DbSvc_Info_t				*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (pstSdtTable == NULL) 		{ return ERR_FAIL; }

	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		if (pstCurrSdt->unInfo.stSdt.usOrgNetId != ERTEL_ORIGINAL_NETWORK_ID)
		{
			continue;
		}

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
		if (pstCurrSdt->unInfo.stSdt.usOrgNetId != ERTEL_ORIGINAL_NETWORK_ID)
		{
			continue;
		}

		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
				DbSvc_Info_t		*pstSvcInfo = &(pstSvcArray[ulSvcNum]);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					SIxSvcDes_t	*pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstSvcDes)
					{
						DxSvcType_e		eSvcType;
						DxVideoType_e	eVideoType;

						DB_SVC_InitServiceInfo (pstSvcInfo);

						DbSvc_SetOnId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usOrgNetId);
						DbSvc_SetTsId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pstSdtSvcItem->unInfo.stSdt.usSvcId);
						svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(pstSvcInfo), NULL);

						// Provider
						if ((pstSvcDes->ucPrvName > 0) && (NULL != pstSvcDes->pszProviderName))
						{
							HUINT32			 ulPrvCnt, ulPrvLen = (pstSvcDes->ucPrvName < DxMAX_PRV_NAME_LEN) ? (HUINT32)pstSvcDes->ucPrvName : DxMAX_PRV_NAME_LEN;
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

						// PIDs
						hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
						DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);

						{
							SvcSi_UserSettings_t stUserSettings = { { 0, }, };

							(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

							hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
							if (ERR_OK != hErr)
							{
								HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
							}
						}

						// Service type, Video Type
						eSvcType = eDxSVC_TYPE_All;
						eVideoType = eDxVIDEO_TYPE_ALL;
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
						}

						DbSvc_SetSvcTypePtr(pstSvcInfo, eSvcType);
						DbSvc_SetVideoTypePtr(pstSvcInfo, eVideoType);
						// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
						//DbSvc_SetOrgSvcTypePtr(pstSvcInfo, eSvcType);
						DbSvc_SetOrgSvcTypePtr(pstSvcInfo, pstSvcDes->ucServType);

						ulSvcNum++;
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}
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

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_SrchMsgCmdStartSearch_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstMsg->hAction));

	(void)svc_si_SrchInitSearchData (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stStartSrch.unSpec), &(pstMsg->unArg.stStartSrch.stOption));
	(void)svc_si_SrchInitSearchResultTable (pstSrch);

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxSPEC_ERTELECOM);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (PAL_PIPE_GetActionId(pstSrch->hAction), eSIxTEXT_ENC_300468_8859_9_BASE);

	// For ERtelecom LCN, You should always receive NIT actual.
	pstSrch->eNitOption = eSI_SRCH_NITOPT_NitAct;
	(void)svc_si_SrchGetFilteringTypes (pstSrch);

	// In ERtelecom search, You need NIT ACT/SDT ACT/SDT OTH.
	if (pstSrch->stOption.ulSearchType != 0)
	{
		pstSrch->eFiltering |= (bSI_FILTERING_SDT_OTH);
	}

	//HxLOG_Error("%s\n", INT_UTIL_GETSTR_SvcSi_SrchNitOption(pstSrch->eNitOption));
	HxLOG_Error("0x%08x\n", pstSrch->eFiltering);
	(void)svc_si_SrchStartSection (pstSrch);
}

STATIC void xsvc_si_SrchMsgCmdStopSearch_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_SrchStopSection (pstSrch);
	(void)svc_si_SrchFreeSearchData (pstSrch);
	(void)PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(pstSrch->hAction));
}

STATIC void xsvc_si_SrchMsgEvtPatTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnPatReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtPmtTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtSdtOthTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}

STATIC void xsvc_si_SrchMsgEvtNitOthTable_Ertelecom (svcSi_SrchData_t *pstSrch, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibSrchOnSiTableReceived_Base (pstSrch, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LibSrchCheckAndComposeSearchData (pstSrch);
}


STATIC void xsvc_si_SrchProcessMsg_Ertelecom (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_SrchData_t	*pstSrch = (svcSi_SrchData_t *)pvData;

	if (NULL == pstSrch)				{ return; }

#if 0
	HxLOG_Print("[%s:%d]%s\n", __FUNCTION__, __LINE__, INT_UTIL_GETSTR_SvcSi_MsgType(pstMsg->eMsgClass));
#endif

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SEARCH:
		xsvc_si_SrchMsgCmdStartSearch_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SEARCH:
		xsvc_si_SrchMsgCmdStopSearch_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_SrchMsgEvtPatTable_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_SrchMsgEvtPmtTable_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_SrchMsgEvtSdtTable_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		xsvc_si_SrchMsgEvtSdtOthTable_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_SrchMsgEvtNitTable_Ertelecom (pstSrch, pstMsg);
		break;

	case eSIMSG_EVT_NIT_OTH_TABLE:
		xsvc_si_SrchMsgEvtNitOthTable_Ertelecom (pstSrch, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_SrchGetSearchedResultFromSi_Ertelecom (void *pvData)
{
	svcSi_SrchData_t 		*pstSrch = NULL;
	HUINT32					 ulActionId = 0;
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
	pstSdtOthTbl = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

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
//HxLOG_Error("###############################################\n");
//HxLOG_Error("ulTsNum : %d\n", ulTsNum);
//HxLOG_Error("###############################################\n");
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

HERROR xsvc_si_SrchSvcEsComposeServiceInfoInTp_Ertelecom (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
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
		hErr1 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtTable, &ulActSvcNum, &pstActSvcArray, &ulActPrvNum, &pstActPrvArray);
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
		hErr2 = svc_si_LibSvcEsComposeServiceInfoBySdt (ulActionId, pstSdtOthTbl, &ulOthSvcNum, &pstOthSvcArray, &ulOthPrvNum, &pstOthPrvArray);
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

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_SrchGetSearchActionFunc_Ertelecom (void)
{
	return xsvc_si_SrchProcessMsg_Ertelecom;
}

void *xsvc_si_SrchAllocSearchActionData_Ertelecom (void)
{
	svcSi_SrchData_t	*pstSrch;

	pstSrch = (svcSi_SrchData_t *)OxSI_Calloc (sizeof(svcSi_SrchData_t));
	if (NULL != pstSrch)
	{
		pstSrch->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstSrch;
}

void xsvc_si_SrchFreeSearchActionData_Ertelecom (void *pvData)
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

