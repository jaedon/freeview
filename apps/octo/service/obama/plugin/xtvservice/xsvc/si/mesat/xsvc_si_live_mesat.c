/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_mesat.c
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
#include <db_param.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <_xsvc_si_lib_live_base.h>

#include "_xsvc_si_mesat.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	TIME_TP_NUM_ME		(6)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	SVC_SatType_t			 eSatType;
	HUINT32					 ulFreq;
	HUINT32					 ulSymbolRate;
} XsvcLive_MeTimeTpInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XsvcLive_MeTimeTpInfo_t	 s_astTimeTpInfo[TIME_TP_NUM_ME] =	// xmgr_time_me.c 에 있는 s_stClockRefTp_Me 참고
{
		// Sat name						Freq.	SR
		//---------------------------------------
		{ eSatType_NILESAT_101_102, 	11766, 	27500 },		// 30s period : OTA Home TP
		{ eSatType_NILESAT_101_102, 	12091, 	27500 },		// 1s period : AD Sports HD

		{ eSatType_BADR_4_6, 		11804, 	27500 },		// 30s period : AD Sports SD
		{ eSatType_BADR_4_6, 		12130, 	27500 },		// 1s period : Dubai One

		{ eSatType_HOTBIRD, 			11054, 	27500 },		// 30s period : ZDF
		{ eSatType_HOTBIRD, 			10949, 	27500 }		// 1s period : Al Fady
};


#define ___LOCAL_FUNCTIONS___

STATIC HERROR xsvc_si_LiveSetLiveData_MeSat (svcSi_LiveData_t *pstLiveData)
{
	if (pstLiveData == NULL)				{ return ERR_FAIL; }

	// BAT
	pstLiveData->usBatBouquetId = 0xFFFF;	// not use EXT id
#if defined(CONFIG_OTA_IRDETO)
	pstLiveData->usBatBouquetId = eBOUQUET_ID_IRDETO;
#endif

	return ERR_OK;
}

STATIC void xsvc_si_LiveMsgCmdStartLive_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);
	HERROR					 hErr = ERR_FAIL;

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_EIT_ACT_PF |
#if defined(CONFIG_OTA_IRDETO)
					bSI_FILTERING_BAT |
#endif
					bSI_FILTERING_TDT);

#if defined(CONFIG_MW_CAS_IRDETO)
	hErr = xsvc_si_FilterSetTimeout_MeSat(PAL_PIPE_GetActionId(pstLive->hAction));
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("xsvc_si_FilterSetTimeout_MeSat err:\n");
	}
#endif

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)xsvc_si_LiveSetLiveData_MeSat (pstLive);

	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);
	HERROR					 hErr = ERR_FAIL;

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF );

#if defined(CONFIG_MW_CAS_IRDETO)
	hErr = xsvc_si_FilterSetTimeout_MeSat(PAL_PIPE_GetActionId(pstLive->hAction));
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("xsvc_si_FilterSetTimeout_MeSat err:\n");
	}
#endif

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)xsvc_si_LiveSetLiveData_MeSat (pstLive);

	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartPb_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_PlaybackOption_t *pstPbOption = &(pstMsg->unArg.stStartPb.stOption);
	HERROR					 hErr = ERR_FAIL;


	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF);


	hErr = xsvc_si_FilterSetTimeout_MeSat(PAL_PIPE_GetActionId(pstLive->hAction));
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("xsvc_si_FilterSetTimeout_MeSat err:\n");
	}

	(void)svc_si_LiveSetPbData (pstLive, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPmtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnEitActPfReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtBatTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnBatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTdtTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTdtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtNitTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnNitActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_MeSat (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnSdtActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_MeSat (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		xsvc_si_LiveMsgCmdStartLive_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_LIVE:
		xsvc_si_LiveMsgCmdStopLive_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		xsvc_si_LiveMsgCmdStartRec_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		xsvc_si_LiveMsgCmdStopRec_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		xsvc_si_LiveMsgCmdStartPb_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		xsvc_si_LiveMsgCmdStopPb_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_LiveMsgEvtPatTable_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_LiveMsgEvtPmtTable_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		xsvc_si_LiveMsgEvtEitTable_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		xsvc_si_LiveMsgEvtTdtTable_MeSat (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_BAT_TABLE:
		xsvc_si_LiveMsgEvtBatTable_MeSat(pstLive, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_LiveMsgEvtNitTable_MeSat(pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_LiveMsgEvtSdtTable_MeSat(pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_MeSat (void)
{
	return xsvc_si_LiveProcessMsg_MeSat;
}

void *xsvc_si_LiveAllocLiveActionData_MeSat (void)
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

void xsvc_si_LiveFreeLiveActionData_MeSat (void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

// 중동에서는 특정 tp에서만 time update를 해야 한다
HBOOL xsvc_si_LiveIsTimeUpdateAvailable(Handle_t hService)
{
	HERROR				hErr;
	HUINT32				ulUpdateTime = 0, i;
	DbSvc_Info_t		stSvcInfo;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_AntInfo_t	stAntInfo;

	// check debug db param.
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP, (HUINT32 *)&ulUpdateTime, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP) Error!!\n");
	}

	HxLOG_Debug("ulUpdateTime: %d \n", ulUpdateTime);

	if (ulUpdateTime != 0)
	{
		return TRUE;
	}

	// check tp validity
	hErr = DB_SVC_GetServiceInfo (hService, &stSvcInfo);
	if (hErr != ERR_OK)		{ return FALSE; }

	hErr = DB_SVC_GetTsInfo (DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
	if (hErr != ERR_OK)		{ return FALSE; }

	hErr = DB_SVC_GetAntInfo ((HUINT16)stTsInfo.stTuningParam.sat.ucAntId, &stAntInfo);
	if (hErr != ERR_OK)		{ return FALSE; }

	for (i = 0; i < TIME_TP_NUM_ME; i++)
	{
		HxLOG_Debug("i(%d) SatType(%d, %d) Freq(%d, %d) SR(%d, %d) ",
						i,
						s_astTimeTpInfo[i].eSatType, stAntInfo.eSatType,
						s_astTimeTpInfo[i].ulFreq, stTsInfo.stTuningParam.sat.ulFrequency,
						s_astTimeTpInfo[i].ulSymbolRate, stTsInfo.stTuningParam.sat.ulSymbolRate);

		if((s_astTimeTpInfo[i].eSatType == stAntInfo.eSatType)
			&& (s_astTimeTpInfo[i].ulFreq == stTsInfo.stTuningParam.sat.ulFrequency)
			&& (s_astTimeTpInfo[i].ulSymbolRate == stTsInfo.stTuningParam.sat.ulSymbolRate))
		{
			HxLOG_Info("time update on this tp, return TRUE \n");
			return TRUE;
		}
	}

	return FALSE;
}


/* end of file */

