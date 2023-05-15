/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_ukfsat.c
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
#include <_xsvc_si_spec_ukfsat.h>
#include <_xsvc_si_lib_tunnel_ukfsat.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcSi_LiveData_t			 stLiveData;			// Default로 기본적인 Live Data:

	// 여기서부터는 Freesat 스펙만을 위한 추가 사양
	XsvcSi_TablePid_UkFsat_t	 stTablePidInfo;
} XsvcSi_LiveData_UkFsat_t;

typedef struct
{
	SIxTable_Type_e			 eSiTableType;
	svcSi_FilteringType_b	 eFilterType;
	HUINT32					 ulTunnelDataType;
} XsvcSi_TnType2SiType_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XsvcSi_TnType2SiType_t	 s_astUkFsat_TType2SiType_Live[] =
{
	{ eSIxTABLETYPE_NitOth,		bSI_FILTERING_NIT_OTH,		FSAT_T_DATA_TYPE_NIT		},
	{ eSIxTABLETYPE_Bat,		bSI_FILTERING_BAT,			FSAT_T_DATA_TYPE_BAT		},
	{ eSIxTABLETYPE_SdtOth, 	bSI_FILTERING_SDT_OTH,		FSAT_T_DATA_TYPE_SDT		},
	{ eSIxTABLETYPE_Tot,		bSI_FILTERING_TOT,			FSAT_T_DATA_TYPE_TOT		},
	{ eSIxTABLETYPE_Tdt,		bSI_FILTERING_TDT,			FSAT_T_DATA_TYPE_TDT		},
	{ eSIxTABLETYPE_EitOthSc,	bSI_FILTERING_EIT_OTH_SCH,	FSAT_T_DATA_TYPE_EITSC		},
	{ eSIxTABLETYPE_EitOthPf,	bSI_FILTERING_EIT_OTH_PF,	FSAT_T_DATA_TYPE_EITPFPP	}
};

#define ___LOCAL_FUNCTIONS___
STATIC HERROR xsvc_si_LiveConvertTableType2TnDataType (SIxTable_Type_e eTableType, svcSi_FilteringType_b *pbFilterType, HUINT32 *pulTnDataType)
{
	HUINT32			 ulCnt, ulMax = sizeof(s_astUkFsat_TType2SiType_Live) / sizeof(XsvcSi_TnType2SiType_t);

	for (ulCnt = 0; ulCnt < ulMax; ulCnt++)
	{
		if (s_astUkFsat_TType2SiType_Live[ulCnt].eSiTableType == eTableType)
		{
			if (NULL != pbFilterType)					{ *pbFilterType = s_astUkFsat_TType2SiType_Live[ulCnt].eFilterType; }
			if (NULL != pulTnDataType)					{ *pulTnDataType = s_astUkFsat_TType2SiType_Live[ulCnt].ulTunnelDataType; }

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____FILTERING_FUNCTIONS_____
STATIC HERROR xsvc_si_LiveConvertTableType2FilterTypeAndExtId_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	svcSi_LiveData_t			*pstLiveData = &(pstUkData->stLiveData);
	XsvcSi_TablePid_UkFsat_t	*pstTablePid = &(pstUkData->stTablePidInfo);

	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_Pmt:
		*peFilterType	= bSI_FILTERING_PMT;
		*pulExtId 		= (HUINT32)pstLiveData->usPmtSvcId;
		*pulPid			= (HUINT32)pstLiveData->usPmtPid;
		break;

	case eSIxTABLETYPE_EitActPf:
		*peFilterType	= bSI_FILTERING_EIT_ACT_PF;
		*pulExtId 		= (HUINT32)pstLiveData->usEitSvcId;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_EitActPf];
		break;

	case eSIxTABLETYPE_SdtAct:
		*peFilterType	= bSI_FILTERING_SDT_ACT;
		*pulExtId 		= (HUINT32)pstLiveData->usSdtTsId;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_SdtAct];
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType	= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_NitAct];
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstLiveData->usBatBouquetId;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_Bat];
		break;

	case eSIxTABLETYPE_Tdt:
		*peFilterType	= bSI_FILTERING_TDT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_Tdt];
		break;

	case eSIxTABLETYPE_Tot:
		*peFilterType	= bSI_FILTERING_TOT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= pstTablePid->aulTunnelledPid[eSIxTABLETYPE_Tot];
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_si_LiveStartSection_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, svcSi_FilteringType_b eFiltering)
{
	svcSi_LiveData_t			*pstLiveData = &(pstUkData->stLiveData);
	XsvcSi_TablePid_UkFsat_t	*pstTablePidInfo = &(pstUkData->stTablePidInfo);
	HBOOL						 bFiltered = FALSE;
	HUINT32						 ulActionId = PAL_PIPE_GetActionId(pstLiveData->hAction);
	HUINT32						 ulExtId = SILIB_TABLE_EXT_ID_NOT, ulPid = PID_NULL;
	SIxTable_Type_e				 eTableType;
	svcSi_FilteringType_b		 eFilterType;
	HERROR						 hErr;

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = xsvc_si_LiveConvertTableType2FilterTypeAndExtId_UkFsat(pstUkData, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (eFiltering & eFilterType) && (ulPid < PID_NULL))
		{
			if ((pstTablePidInfo->aulLastExtId[eTableType] != ulExtId) ||
				(pstTablePidInfo->aulLastPid[eTableType] != ulPid))
			{
				if (PID_NULL != pstTablePidInfo->aulLastPid[eTableType])
				{
					hErr = svc_si_FilterStopTableSection(pstLiveData->hAction, eTableType, pstTablePidInfo->aulLastExtId[eTableType]);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("Error in stopping filter\n");
					}
				}

				hErr = svc_si_FilterStartTableSection(pstLiveData->hAction, pstLiveData->ulDemuxId, eTableType, ulExtId, ulPid, eSI_TABLE_FILTER_MODE_AllMonitor, NULL);
				if (ERR_OK == hErr)
				{
					pstLiveData->eTableFiltering |= eFilterType;
					pstTablePidInfo->aulLastExtId[eTableType] = ulExtId;
					pstTablePidInfo->aulLastPid[eTableType] = ulPid;

					bFiltered = TRUE;
				}
				else
				{
					HxLOG_Error ("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstLiveData->hAction, eTableType);
				}
			}
		}
	}

	if (TRUE != bFiltered)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}



#define _____MSG_PROCESS_____
STATIC void xsvc_si_LiveMsgCmdStartLive_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(ulActionId, eSIxSPEC_FSAT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(ulActionId, eSIxTEXT_ENC_300468_UK);

	(void)xsvc_si_LibInitTablePidInfo_UkFsat(&(pstUkData->stTablePidInfo));

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT );

	(void)svc_si_LiveSetLiveData(pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);

	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pat] = PAT_PID;
	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pmt] = pstLive->usPmtPid;

	(void)svc_si_LiveStartSection(pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)svc_si_LiveStopSection(pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(ulActionId, eSIxSPEC_FSAT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(ulActionId, eSIxTEXT_ENC_300468_UK);

	(void)xsvc_si_LibInitTablePidInfo_UkFsat(&(pstUkData->stTablePidInfo));

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT );

	(void)svc_si_LiveSetLiveData(pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);

	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pat] = PAT_PID;
	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pmt] = pstLive->usPmtPid;

	(void)svc_si_LiveStartSection(pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)svc_si_LiveStopSection(pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartPb_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_PlaybackOption_t	*pstPbOption = &(pstMsg->unArg.stStartPb.stOption);
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(ulActionId, eSIxSPEC_FSAT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(ulActionId, eSIxTEXT_ENC_300468_UK);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT );

	(void)svc_si_LiveSetPbData(pstLive, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );

	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pat] = PAT_PID;
	pstUkData->stTablePidInfo.aulTunnelledPid[eSIxTABLETYPE_Pmt] = pstLive->usPmtPid;

	(void)svc_si_LiveStartSection(pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)svc_si_LiveStopSection(pstLive);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnPatReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t			*pstLive = &(pstUkData->stLiveData);
	XsvcSi_TablePid_UkFsat_t	*pstTablePidInfo = &(pstUkData->stTablePidInfo);
	HUINT32						 ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	Handle_t					 hTunnelled = HANDLE_NULL;

	if (TRUE == pstLive->bAbortPatPmt)
	{
		return ERR_OK;
	}

	switch (pstMsg->unArg.stSection.ulTableStatus)
	{
	case eSI_TABLE_RECEIVED:
		hTunnelled = xsvc_si_LibMakeTunnelledInfoHandle_UkFsat(ulActionId, (HUINT32)pstLive->usPmtSvcId);
		if (HANDLE_NULL != hTunnelled)
		{
			HUINT16					 usPid;
			HUINT32					 ulTnDataType;
			SIxTable_Type_e			 eTableType;
			svcSi_FilteringType_b	 eFilterType;
			HERROR					 hErr;

			for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
			{
				usPid = PID_NULL;
				pstTablePidInfo->aulTunnelledPid[eTableType] = PID_NULL;

				hErr = xsvc_si_LiveConvertTableType2TnDataType(eTableType, &eFilterType, &ulTnDataType);
				if (ERR_OK == hErr)
				{
					hErr = xsvc_si_LibGetTunneledPid_UkFsat(hTunnelled, (HUINT8)ulTnDataType, &usPid);
					if ((ERR_OK == hErr) && (PID_NULL != usPid))
					{
						pstTablePidInfo->aulTunnelledPid[eTableType] = usPid;
					}
				}
			}

			xsvc_si_LibFreeTunnelledInfoHandle_UkFsat(hTunnelled);
		}

		break;

	default:
		break;
	}

	(void)xsvc_si_LibLiveOnPmtReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnEitActPfReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtNitTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnNitActReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnSdtActReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTdtTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnTdtReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTotTable_UkFsat (XsvcSi_LiveData_UkFsat_t *pstUkData, SvcSi_Msg_t *pstMsg)
{
	svcSi_LiveData_t		*pstLive = &(pstUkData->stLiveData);

	(void)xsvc_si_LibLiveOnTotReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_UkFsat (SvcSi_Msg_t *pstMsg, void *pvData)
{
	XsvcSi_LiveData_UkFsat_t	*pstUkData = (XsvcSi_LiveData_UkFsat_t *)pvData;

	if (NULL == pstUkData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_START_LIVE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStartLive_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_LIVE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_STOP_LIVE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStopLive_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_START_REC \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStartRec_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_STOP_REC \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStopRec_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_START_PB \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStartPb_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_CMD_STOP_PB \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgCmdStopPb_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_PAT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtPatTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_PMT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtPmtTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_EIT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtEitTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_NIT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtNitTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_SDT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtSdtTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_TDT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtTdtTable_UkFsat(pstUkData, pstMsg);
		break;

	case eSIMSG_EVT_TOT_TABLE:
		HxLOG_Print("[%s:%d]  eMsgClass : eSIMSG_EVT_TOT_TABLE \n", __FUNCTION__, __LINE__);
		xsvc_si_LiveMsgEvtTotTable_UkFsat(pstUkData, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_UkFsat (void)
{
	return xsvc_si_LiveProcessMsg_UkFsat;
}

void *xsvc_si_LiveAllocLiveActionData_UkFsat (void)
{
	XsvcSi_LiveData_UkFsat_t	*pstUkData;

	pstUkData = (XsvcSi_LiveData_UkFsat_t *)OxSI_Calloc(sizeof(XsvcSi_LiveData_UkFsat_t));
	if (NULL != pstUkData)
	{
		pstUkData->stLiveData.eState			= eSI_LIVESTATE_Init;
		pstUkData->stLiveData.eTextEncType		= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstUkData;
}

void xsvc_si_LiveFreeLiveActionData_UkFsat (void *pvData)
{
	XsvcSi_LiveData_UkFsat_t	*pstUkData = (XsvcSi_LiveData_UkFsat_t *)pvData;

	if (NULL != pstUkData)
	{
		OxSI_Free(pstUkData);
	}

	return;
}

/* end of file */

