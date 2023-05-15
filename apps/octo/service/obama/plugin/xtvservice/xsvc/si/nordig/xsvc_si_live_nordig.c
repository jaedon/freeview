/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_nordig.c
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
#include <pal_pipe.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <_xsvc_si_lib_live_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	xSvcSi_Nordig_SERVICE_REPLACEMENT_TYPE		(0x5)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC HBOOL							s_bNordigServiceReplacement = FALSE;
STATIC SvcSi_SdtRunningStatusType_e		s_eRunningStatus = eSdtRunningStatusType_RUNNING;
STATIC DbSvc_TripleId_t					s_stReplacementTripleId = {0, 0, 0};


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


STATIC HERROR xsvc_si_FindTripleIdFromLinkageDescInSdt_Nordig(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, DbSvc_TripleId_t *pstTripleId)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bFound = FALSE;
	HUINT32			ulActionId;
	DbSvc_Info_t	stSvcInfo;
	DbSvc_TsInfo_t	stTsInfo;

	SIxTable_PsiTable_t 	*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;


	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	hError = DB_SVC_GetServiceInfo(pstLive->hService , &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	// SDT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	else
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

				if (*pucRawDes == eSIxDESCTAG_LINKAGE)
				{
					SIxLinkDes_t *pstLinkDesc = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pstLinkDesc != NULL)
					{
						if((pstLinkDesc->ucLinkType == xSvcSi_Nordig_SERVICE_REPLACEMENT_TYPE) && (pstLinkDesc->usOrgNetId == DbSvc_GetOnId(&stSvcInfo)))
						{
							if((pstLinkDesc->usTsId != DbSvc_GetTsId(&stSvcInfo)) || (pstLinkDesc->usSvcId != DbSvc_GetSvcId(&stSvcInfo)))
							{
								pstTripleId->usOnId = pstLinkDesc->usOrgNetId;
								pstTripleId->usTsId = pstLinkDesc->usTsId;
								pstTripleId->usSvcId= pstLinkDesc->usSvcId;

								bFound = TRUE;
							}
						}

						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstLinkDesc);

						if( bFound == TRUE)
						{
							if(pstSdtTable != NULL)
								PAL_SIRAWPOOL_FreeTable(pstSdtTable);

							return ERR_OK;
						}
					}
				}
			}
		}
	}

Error:

	if(pstSdtTable != NULL)
		PAL_SIRAWPOOL_FreeTable(pstSdtTable);

	return hError;
}

STATIC HERROR xsvc_si_LiveProcessRunningStatusFromSdt_Nordig(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, HUINT8 ucRunningStatus)
{
	HERROR	hErr;


	if(ulHead == ACT_HEAD)
	{
		if(ucRunningStatus == eSdtRunningStatusType_NOT_RUNNING)
		{
			Handle_t		hSupSvcHandle;
			DbSvc_TripleId_t stTripleId;

			HxSTD_memset (&stTripleId, 0, sizeof(DbSvc_TripleId_t));

			hErr = xsvc_si_FindTripleIdFromLinkageDescInSdt_Nordig(pstLive, pstSecCb, ulHead, &stTripleId);

			if(hErr == ERR_OK)
			{
				if (ERR_OK == DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_CAB, stTripleId, &hSupSvcHandle))
				{
					pstLive->hSupService = hSupSvcHandle;

					svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hSupService, eSdtRunningStatusChanged_ByNordig, ucRunningStatus);
					s_bNordigServiceReplacement = TRUE;
					s_stReplacementTripleId = stTripleId;
				}
			}
		}
		else if(ucRunningStatus == eSdtRunningStatusType_RUNNING)
		{
			DbSvc_Info_t		stSvcInfo;

			hErr = DB_SVC_GetServiceInfo(pstLive->hService , &stSvcInfo);
			if(hErr == ERR_OK)
			{
				if(s_stReplacementTripleId.usOnId == DbSvc_GetOnId(&stSvcInfo) && s_stReplacementTripleId.usTsId == DbSvc_GetTsId(&stSvcInfo) && s_stReplacementTripleId.usSvcId == DbSvc_GetSvcId(&stSvcInfo))
				{
					// Nothing TODO
				}
				else
				{
					svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hService, eSdtRunningStatusChanged_ByNordig, ucRunningStatus);
					s_bNordigServiceReplacement = FALSE;
					HxSTD_memset (&s_stReplacementTripleId, 0, sizeof(DbSvc_TripleId_t));
				}
			}
		}
	}
	else
	{
		#if 0
		if(ucRunningStatus == eSdtRunningStatusType_RUNNING )
		{
			svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hService, eSdtRunningStatusChanged_ByNordig, ucRunningStatus);
			pstLive->hSupService = pstLive->hService;
			s_bNordigServiceReplacement = FALSE;
		}
		#endif
	}
	HxLOG_Print("[%s : %d]s_eRunningStatus (%d),ucRunningStatus (%d) \n",__FUNCTION__, __LINE__, s_eRunningStatus,ucRunningStatus);

	s_eRunningStatus = ucRunningStatus;
					
	return ERR_OK;
}

STATIC HERROR xsvc_si_LiveCheckRunningStatusFromSdt_Nordig(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, HUINT8 *pucRunningStatus)
{
	HERROR			hError;
	HUINT32			ulActionId;
	DbSvc_Info_t		stSvcInfo;

	SIxTable_PsiTable_t 	*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t				*pstSdtSvcLoop = NULL;


	hError = DB_SVC_GetServiceInfo(pstLive->hService , &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);

	if(ulHead == ACT_HEAD)
	{
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Warning("pstSdtSec is NULL \n");
			continue;
		}

		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = NULL;
			SIxTable_SdtService_t	*pstSdtSvc = NULL;

			pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);
			if(pstSdtSvcItem == NULL)
			{
				HxLOG_Warning("pstSdtSvcItem is NULL \n");
				continue;
			}

			pstSdtSvc = &pstSdtSvcItem->unInfo.stSdt;
			if(pstSdtSvc == NULL)
			{
				HxLOG_Warning("pstSdtSvc is NULL \n");
				continue;
			}

			if(pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(&stSvcInfo))
			{
				HxLOG_Debug("\tSvcId:0x%X bEitSched:%d bEitPreFollow:%d ucRunningStatus:0x%X bCaFreeL[%d\n\r",
					pstSdtSvc->usSvcId,pstSdtSvc->bEitSched,pstSdtSvc->bEitPreFollow,
					pstSdtSvc->ucRunningStatus,pstSdtSvc->bCaFree);

				*pucRunningStatus = pstSdtSvcItem->unInfo.stSdt.ucRunningStatus;

				if(pstSdtTable!= NULL)
					PAL_SIRAWPOOL_FreeTable(pstSdtTable);

				return ERR_OK;
			}
		}
	}
Error:

	if(pstSdtTable!= NULL)
		PAL_SIRAWPOOL_FreeTable(pstSdtTable);

	return ERR_FAIL;
}


STATIC HERROR xsvc_si_LiveProcessSdt_Nordig  (svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR	hError = ERR_FAIL;
	HUINT8	ucRunningStatus;

	if(pstLive ==NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL Or pstSecCb is NULL!! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
		case eSI_TABLE_TIMEOUT:
		case eSI_TABLE_FAIL:
			//HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
			return hError;
		default :
			break;
	}

	hError = xsvc_si_LiveCheckRunningStatusFromSdt_Nordig(pstLive, pstSecCb, ulHead, &ucRunningStatus);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckRunningStatusforCurServiceFromSdt_Nordig Error (0x%x) \n", hError);
	}

	if((hError == ERR_OK) && ucRunningStatus && (ucRunningStatus != s_eRunningStatus))
	{
		xsvc_si_LiveProcessRunningStatusFromSdt_Nordig(pstLive, pstSecCb, ulHead, ucRunningStatus);
	}
	
	return hError;
}

STATIC void xsvc_si_LiveMsgCmdStartLive_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_NORDIG);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

#if defined(CONFIG_SUPPORT_FCC)
	// FCC svc라면 반드시 필요한 table들만 걸자
	bFiltering = (bSI_FILTERING_PAT | bSI_FILTERING_PMT);

	if (pstStartLive->bFccSvc == FALSE)
	{
		/* main svc */
		bFiltering |= ( bSI_FILTERING_NIT_ACT |
						bSI_FILTERING_SDT_ACT |
						bSI_FILTERING_EIT_ACT_PF |
						bSI_FILTERING_TDT |
						bSI_FILTERING_TOT );
	}
#else
	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_EIT_ACT_PF |
					bSI_FILTERING_TDT |
					bSI_FILTERING_TOT );
#endif

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_NORDIG);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF );

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartPb_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_PlaybackOption_t *pstPbOption = &(pstMsg->unArg.stStartPb.stOption);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_NORDIG);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF );

	(void)svc_si_LiveSetPbData (pstLive, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPmtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnEitActPfReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtNitTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnNitActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnSdtActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LiveProcessSdt_Nordig(pstLive, &(pstMsg->unArg.stSection), ACT_HEAD);
}

STATIC void xsvc_si_LiveMsgEvtTdtTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTdtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTotTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTotReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtAitTable_Nordig (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnAitReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_Nordig (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		xsvc_si_LiveMsgCmdStartLive_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_LIVE:
		xsvc_si_LiveMsgCmdStopLive_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		xsvc_si_LiveMsgCmdStartRec_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		xsvc_si_LiveMsgCmdStopRec_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		xsvc_si_LiveMsgCmdStartPb_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		xsvc_si_LiveMsgCmdStopPb_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_LiveMsgEvtPatTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_LiveMsgEvtPmtTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		xsvc_si_LiveMsgEvtEitTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_LiveMsgEvtNitTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_LiveMsgEvtSdtTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		xsvc_si_LiveMsgEvtTdtTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TOT_TABLE:
		xsvc_si_LiveMsgEvtTotTable_Nordig (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_AIT_TABLE:
		xsvc_si_LiveMsgEvtAitTable_Nordig (pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_Nordig (void)
{
	return xsvc_si_LiveProcessMsg_Nordig;
}

void *xsvc_si_LiveAllocLiveActionData_Nordig (void)
{
	svcSi_LiveData_t	*pstLive;

	pstLive = (svcSi_LiveData_t *)OxSI_Calloc (sizeof(svcSi_LiveData_t));
	if (NULL != pstLive)
	{
		pstLive->eState = eSI_LIVESTATE_Init;
		pstLive->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstLive;
}

void xsvc_si_LiveFreeLiveActionData_Nordig (void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

/* end of file */

