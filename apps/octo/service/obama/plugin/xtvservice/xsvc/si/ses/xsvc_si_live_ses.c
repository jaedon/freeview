/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_ses.c
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
#include <_xsvc_si_lib.h>
#include <pmgr_copyrightcontrol.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSI_LIVEMODE_Live		= 0,
	eSI_LIVEMODE_Rec,
	eSI_LIVEMODE_Pb,
	eSI_LIVEMODE_Satip,

	eSI_LIVEMODE_EndOfCase
} svcSi_LiveMode_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC svcSi_LiveMode_e 	eCurLiveMode = eSI_LIVEMODE_Live;

#define PRIVATE_DATA_SPEC_APS_HD_DRM				0x00000001

#define ___LOCAL_FUNCTIONS___

STATIC HERROR xsvc_si_LiveGetTripleId_Ses(svcSi_LiveData_t *pstLive, DbSvc_TripleId_t *pstTripleId)
{
	HERROR					 hErr = ERR_FAIL;
	DbSvc_Info_t			*pstSvcInfo = NULL;

	if ((pstLive == NULL) || (pstTripleId == NULL))
	{
		HxLOG_Error("Invalid parameter !! \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstTripleId, 0x00, sizeof(DbSvc_TripleId_t));

	if (eCurLiveMode == eSI_LIVEMODE_Pb)
	{
		// Playback
		pstTripleId->usOnId 	= 0;	// 현재로서는 pb시 onId를 여기서 알 수가 없다.
		pstTripleId->usTsId 	= pstLive->usSdtTsId;
		pstTripleId->usSvcId 	= pstLive->usEitSvcId;
	}
	else
	{
		// Live, Rec
		pstSvcInfo = (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t));
		if (pstSvcInfo == NULL)
		{
			HxLOG_Error("not enough memory ! \n");
			return ERR_FAIL;
		}

		hErr = DB_SVC_GetServiceInfo (pstLive->hService, pstSvcInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("DB_SVC_GetServiceInfo hMasterSvc(0x%08x) Err\n", pstLive->hService);
			OxSI_Free (pstSvcInfo);
			return ERR_FAIL;
		}

		pstTripleId->usOnId 	= DbSvc_GetOnId(pstSvcInfo);
		pstTripleId->usTsId 	= DbSvc_GetTsId(pstSvcInfo);
		pstTripleId->usSvcId 	= DbSvc_GetSvcId(pstSvcInfo);

		OxSI_Free (pstSvcInfo);
	}

	return ERR_OK;
}

DxOperator_e xsvc_si_LIveGetServiceOptype_Ses(Handle_t  hAction)
{
	DxOperator_e eSvcOpType = eDxOPERATOR_None;
	HUINT32 ulActionId = PAL_PIPE_GetActionId(hAction);

	if(xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType) != ERR_OK)
	{
		HxLOG_Debug("xsvc_si_LibSrchGetServiceOptype() is error\n");
	}
	HxLOG_Debug("svcOpType is Ses(%s)\n", eSvcOpType==eDxOPERATOR_Ses ? "Yes":"No");

	return 	eSvcOpType;
//	return eDxOPERATOR_Ses;
}

STATIC HERROR xsvc_si_LiveCheckEitCopyControl_Ses(svcSi_LiveData_t *pstLive)
{
	HERROR					 hErr = ERR_FAIL;
	HERROR					 hResult = ERR_FAIL;
	HUINT32 				 ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	DbSvc_TripleId_t 		 stTripleId;
	SIxTable_PsiTable_t 	*pstEitTable = NULL, *pstCurrTable = NULL;
	HBOOL					bDrmFound = FALSE;
	HBOOL					bFoundPrivDes = FALSE;
	DxCopyrightControl_t	*pstCopyControl = NULL;
	HUINT8					ucCopyCtrlByteA = 0x00, ucCopyCtrlByteB = 0x00;

	pstEitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_EitActPf, SILIB_TABLE_EXT_ID_NOT);
	if (pstEitTable == NULL)
	{
		HxLOG_Debug ("Not exist EIT act pf \n");
		goto END_FUNC;
	}

	hErr = xsvc_si_LiveGetTripleId_Ses (pstLive, &stTripleId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("xsvc_si_LiveGetTripleId_Ses() failed..!! \n");
		goto END_FUNC;
	}

	// 오직 APS HD+ Service 일 경우에만 DRM 정보를 올려줘야 한다.
	if(xsvc_si_LIveGetServiceOptype_Ses(pstLive->hAction) != eDxOPERATOR_Ses)
	{
		HxLOG_Error ("Optype is not ses : error..!! \n");
		goto END_FUNC;
	}

	for (pstCurrTable = pstEitTable; pstCurrTable != NULL; pstCurrTable = pstCurrTable->pstNextTable)
	{
		HxList_t	*pstElemItem = NULL;

		/* PB인 경우 현재는 여기에서 녹화물의 onId를 알 방법이 없다.
		  그래서 PB일 때는 onId 검사는 생략하고 진행해보고 문제가 생기면 onId도 추가하자 */
		if (eCurLiveMode == eSI_LIVEMODE_Pb)
		{
			stTripleId.usOnId = pstEitTable->unInfo.stEit.usOrgNetId;
		}

		if ((pstEitTable->unInfo.stEit.usOrgNetId	!= stTripleId.usOnId)
			|| (pstEitTable->unInfo.stEit.usTsId	!= stTripleId.usTsId)
			|| (pstEitTable->unInfo.stEit.usSvcId	!= stTripleId.usSvcId))
		{
			continue;
		}

		/* first event를 present event로 간주한다
		for (pstElemItem = pstCurrTable->pst2ndLoop; pstElemItem != NULL; pstElemItem = pstElemItem->next)
		*/
		pstElemItem = pstCurrTable->pst2ndLoop;
		{
			SIxTable_2ndLoop_t		*pst2ndLoop = HLIB_LIST_Data (pstElemItem);
			HxList_t				*pstDescList = NULL;

			if (NULL == pst2ndLoop) 										{ continue; }

			for (pstDescList = pst2ndLoop->pstDesLoop; NULL != pstDescList; pstDescList = pstDescList->next)
			{
				HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescList);

				switch (*pucDesRaw)
				{
					case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
					{
						SIxPrivateDataSpecDes_t *pstPrivDataSpecDes = NULL;

						pstPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if(pstPrivDataSpecDes == NULL)
						{
							continue;
						}

						if(pstPrivDataSpecDes->ulPrivateDataSpec == PRIVATE_DATA_SPEC_APS_HD_DRM)
						{
							bFoundPrivDes = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t*)pstPrivDataSpecDes);
					}
					break;

					case eSIxDESCTAG_SES_COPY_CONTROL:
					{
						SIxCopyControlDes_t 	*pstDrmDes = NULL;
						SIxCopyControlInfo_t	*pstInfo = NULL;

						if(bFoundPrivDes == FALSE)
						{
							continue;
						}

						pstDrmDes = (SIxCopyControlDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if (pstDrmDes == NULL)
						{
							continue;
						}

						pstInfo = pstDrmDes->pstInfo;

						if(pstDrmDes->pstInfo)
						{
							bDrmFound 				= TRUE;
							ucCopyCtrlByteA			= pstInfo->ucCopyControl;
							pstInfo = pstInfo->pstNext;
							ucCopyCtrlByteB			= pstInfo->ucCopyControl;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t*)pstDrmDes);
					}
					break;

					default:
						// do nothing..
						break;
				}
			}
		}
	}

	pstCopyControl = (DxCopyrightControl_t *)OxSI_Calloc (sizeof(DxCopyrightControl_t));

	if(pstCopyControl)
	{
		pstCopyControl->bSiCc = TRUE;
		pstCopyControl->stSiCcInfo.eSiCcType = eDxCopyrightControl_SiCcType_Ses;

		if(bDrmFound == TRUE)
		{
			pstCopyControl->stSiCcInfo.info.stSesDrmInfo.ucControlByteA = ucCopyCtrlByteA;
			pstCopyControl->stSiCcInfo.info.stSesDrmInfo.ucControlByteB = ucCopyCtrlByteB;
		}
		else
		{
			pmgr_copyrightcontrol_InitSiCopyProtectInfo(pstCopyControl, NULL);
		}

		svc_si_PostDataToAp (eSEVT_SI_DRM_SIGNALLED, pstLive->hAction, 0, 0, (void *)pstCopyControl, sizeof(DxCopyrightControl_t));

		OxSI_Free (pstCopyControl);
	}

	hResult = ERR_OK;

END_FUNC:
	if (pstEitTable != NULL)				{ PAL_SIRAWPOOL_FreeTable (pstEitTable); }

	return hResult;
}

STATIC void xsvc_si_LiveMsgCmdStartLive_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
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

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

#if defined(CONFIG_SUPPORT_FCC)
		// FCC svc라면 fcc에 꼭 필요한 table만 걸자
		bFiltering = (bSI_FILTERING_PAT | bSI_FILTERING_PMT);

		if (pstStartLive->bFccSvc == FALSE)
		{
			/* main svc */
			bFiltering |= ( bSI_FILTERING_NIT_ACT |
							bSI_FILTERING_SDT_ACT |
							bSI_FILTERING_EIT_ACT_PF |
							bSI_FILTERING_TDT);
		}

		pstLive->bFccSvc = pstStartLive->bFccSvc;
#else
		bFiltering = (	bSI_FILTERING_PAT |
						bSI_FILTERING_PMT |
						bSI_FILTERING_NIT_ACT |
						bSI_FILTERING_SDT_ACT |
						bSI_FILTERING_EIT_ACT_PF |
						bSI_FILTERING_TDT );
#endif

	eCurLiveMode = eSI_LIVEMODE_Live;
	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
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

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
 					bSI_FILTERING_EIT_ACT_PF );

	eCurLiveMode = eSI_LIVEMODE_Rec;
	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartSatip_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
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

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT );

	eCurLiveMode = eSI_LIVEMODE_Satip;
	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopSatip_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}


STATIC void xsvc_si_LiveMsgCmdStartPb_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
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

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_SES);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF );

	eCurLiveMode = eSI_LIVEMODE_Pb;
	(void)svc_si_LiveSetPbData (pstLive, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPmtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnEitActPfReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LiveCheckEitCopyControl_Ses(pstLive);
}

STATIC void xsvc_si_LiveMsgEvtNitTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnNitActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnSdtActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTdtTable_Ses (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTdtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_Ses (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		xsvc_si_LiveMsgCmdStartLive_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_LIVE:
		xsvc_si_LiveMsgCmdStopLive_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		xsvc_si_LiveMsgCmdStartRec_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		xsvc_si_LiveMsgCmdStopRec_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_SATIP:
		xsvc_si_LiveMsgCmdStartSatip_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_SATIP:
		xsvc_si_LiveMsgCmdStopSatip_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		xsvc_si_LiveMsgCmdStartPb_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		xsvc_si_LiveMsgCmdStopPb_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_LiveMsgEvtPatTable_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_LiveMsgEvtPmtTable_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		xsvc_si_LiveMsgEvtEitTable_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_LiveMsgEvtNitTable_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_LiveMsgEvtSdtTable_Ses (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		xsvc_si_LiveMsgEvtTdtTable_Ses (pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_Ses (void)
{
	return xsvc_si_LiveProcessMsg_Ses;
}

void *xsvc_si_LiveAllocLiveActionData_Ses (void)
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

void xsvc_si_LiveFreeLiveActionData_Ses (void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

/* end of file */

