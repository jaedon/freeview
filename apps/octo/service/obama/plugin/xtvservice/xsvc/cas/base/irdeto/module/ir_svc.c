/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_svc.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/
#include <pal_sc.h>
#include <pal_pipe.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "s3_vd.h"
#include "s3_prod_if_msg.h"

#include "ir_svc.h"
#include "ir_msg_scell.h"
#include "ir_api.h"
#include "ir_evt.h"

#include "ir_util.h"
#include "ir_fta_block.h"

#include "ir_status.h"

#if	defined(IRPLAT_SYS_PVR)
	#include "ir_pvr.h"
	#include <svc_rec.h>
	#include <svc_pb.h>
	#include <svc_meta.h>
#endif
#if	defined(IRPLAT_SYS_EMM_BASED_ORGID)
#include <db_svc.h>
#endif
#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
/* 	IR_SVC_CLOSE_USE_NULL_PMT¿¡¼­´Â fake service close¸¦ »ç¿ëÇÏ±â¿¡, ÃÖÃÊ·Î service defineÀÌ µÉ ´ç½ÃÀÇ demux id°¡
	Áö¼ÓÀûÀ¸·Î »ç¿ëµÈ´Ù. ( ÀÌ¹Ì defineµÈ service¿¡ ´ëÇØ demux id¸¦ º¯°æÇÒ ¼ö°¡ ¾ø´Ù.)
	±×·¡¼­, descramble service¿Í demux id¸¦ °áÇÕÇÏ´Â ¹æ½ÄÀ» Ãß°¡ÇÑ´Ù. »õ·Î¿î actionÀÌ descramble service¸¦ ¿äÃ»ÇÒ °æ¿ì,
	demux id¸¦ key·Î findÇÏ¿© ÇØ´ç descramble service¸¦ »ç¿ëÇÏµµ·Ï ÇÑ´Ù. ÇØ´ç demux id¸¦ Áö¿øÇÏ´Â descramble service°¡ ¾øÀ¸¸é
	»õ·Î¿î descramble service¸¦ ¸¸µé°í ÇØ´ç demux id¸¦ ´ã´çÇÏµµ·Ï ÇÑ´Ù.

	(ÃÖ´ë descramble service´Â 7°³(actionÀÇ °³¼ö) ÀÌ¸ç, ÃÖ´ë demux °³¼öµµ 7°³ÀÌ´Ù.  ¸¸¾à, ³ªÁß¿¡ °³¼ö°¡ Æ²¾îÁö¸é ÀÌ¸¦ º¸¿ÏÇÒ ÄÚµå°¡ Ãß°¡µÇ¾î¾ß ÇÑ´Ù.)
*/
#define __COMBINE_DESCRAMBLE_SVC_WITH_DEMUX__
#endif

/********************************************************************
*	Definitions (for this source file)
********************************************************************/





/********************************************************************
*	Global variables
********************************************************************/
IR_SERVICE_INFO			g_IrSvcInfo[IR_HUMAX_MAX_SERVICE];
IR_TP_INFO				g_IrTpInfo[IR_HUMAX_MAX_TP];


/********************************************************************
*	External variables
********************************************************************/
extern	IR_DRV_SC_INFO		g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];


extern	AXI_STATUSCODE_IDX	CAS_IR_DRV_GetErrorCodeIndex(ia_status_st *pStatus);


/********************************************************************
*	local Functions
********************************************************************/

#define _____local_Functions___________________________________________________________

static	IR_TP_INFO			*local_cas_ir_Drv_FindIrTpInfo(Handle_t hAction)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		if (g_IrTpInfo[i].ucIsAllocated && ((g_IrTpInfo[i].hAction == hAction) || (svc_cas_SubSvcRmGetActionId(hAction) == svc_cas_SubSvcRmGetActionId(g_IrTpInfo[i].hAction))))
			return (IR_TP_INFO*)&(g_IrTpInfo[i]);
	}

	return (IR_TP_INFO*)NULL;
}

static	IR_TP_INFO			*local_cas_ir_Drv_FindIrTpInfoBySCELL(HUINT16 usSCELLHandle)
{
	HINT32		i, j;

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
			if (g_IrTpInfo[i].ucIsAllocated &&  g_IrTpInfo[i].ucIsSCELL_Ready && (g_IrTpInfo[i].usSCELL_SvcHandle[j] == usSCELLHandle))
				return (IR_TP_INFO*)&(g_IrTpInfo[i]);
		}
	}

	return NULL;
}

static	void				local_cas_ir_Drv_ClearTpInfo(IR_TP_INFO *pstIrTpInfo)
{
	HINT32 i=0;

	if (pstIrTpInfo->pCatRaw)
		OxCAS_Free(pstIrTpInfo->pCatRaw);
	HxSTD_memset(pstIrTpInfo, 0, sizeof(IR_TP_INFO));

#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
	pstIrTpInfo->pCatRaw = NULL;
#endif

	for ( i = 0; i < CAS_MAX_NUM_OF_SC; i++ )
	{
		pstIrTpInfo->usSCELL_SvcHandle[i] = INVALID_SOFTCELL_SVCHANDLE;
	}
}

static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfo(Handle_t hIrSvc)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated && (g_IrSvcInfo[i].hIrSvc == hIrSvc))
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
	}

	return (IR_SERVICE_INFO*)NULL;
}


#ifdef __COMBINE_DESCRAMBLE_SVC_WITH_DEMUX__
static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfoByDemuxId(Handle_t hIrSvc, HUINT8 ucDmxId)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated && (g_IrSvcInfo[i].hIrSvc == HANDLE_NULL))
		{
			if (g_IrSvcInfo[i].ucDemuxId == ucDmxId)
				return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
		}
	}

	return (IR_SERVICE_INFO*)NULL;
}
#else

#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindAllocIrSvcInfo(Handle_t hIrSvc)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated && (g_IrSvcInfo[i].hIrSvc == HANDLE_NULL))
		{
		#if 1
			// LIVE ÀÌ¿ÜÀÇ ActionÀÌ ÇÒ´çµÇ¸é LIVE ActionÀ¸·Î ÇÒ´çµÇ¾ú´ø SVC´Â  ºÙÀÌÁö ¾Ê´Â´Ù.
			// LIVE¿Í TSRÀÇ Softcell HandleÀÌ ÀÌÀü ¼­ºñ½º¿Í ´Ù¸£°Ô µÇ¸é DescrambleÀÌ µÇÁö ¾Ê´Â Áõ»ó¿¡ ´ëÇÑ WA
			if (hIrSvc & 0xFFFF0000)
			{
				if (g_IrSvcInfo[i].eCasActionType != eCAS_ActionType_Live)
				{
					g_IrSvcInfo[i].hIrSvc = hIrSvc;
					return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
				}
			}
			else
		#endif
			{
			g_IrSvcInfo[i].hIrSvc = hIrSvc;
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
		}
	}
	}

	return (IR_SERVICE_INFO*)NULL;
}
#endif
#endif


static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfoByContentsId(HUINT32 ulContentId)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsSCELLReady && (g_IrSvcInfo[i].ulContentsId == ulContentId))
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
	}

	return (IR_SERVICE_INFO*)NULL;
}

static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfoByPvrInfo(void *pvrInfo)
{
#if	defined(IRPLAT_SYS_PVR)
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsSCELLReady && ((void*)g_IrSvcInfo[i].pstPVRSvcInfo == pvrInfo))
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
	}
#endif

	return (IR_SERVICE_INFO*)NULL;
}

static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfoBySCELL(HUINT16 usSCELLHandle)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsSCELLReady && (g_IrSvcInfo[i].usSCELL_SvcHandle == usSCELLHandle))
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
	}

	return NULL;
}

#if defined(IRPLAT_PVR_DRM)
static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindIrSvcInfoByActionId(Handle_t hAction)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated && (svc_cas_SubSvcRmGetActionId(g_IrSvcInfo[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction)))
			return (IR_SERVICE_INFO*)&(g_IrSvcInfo[i]);
	}

	return (IR_SERVICE_INFO*)NULL;
}
#endif

static	IR_SERVICE_INFO		*local_cas_ir_Drv_FindEmptySvcInfo(void)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated == 0)
			return &(g_IrSvcInfo[i]);
	}

	return NULL;
}

#if 0
//	function for finding irsvcinfo with pvr record service by live irsvc info
static	IR_SERVICE_INFO		*local_cas_ir_Drv_GetIrRecSvcInfo(Handle_t hIrSvc, HUINT32 ulContentsId)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if ((g_IrSvcInfo[i].ucIsAllocated &&
			(g_IrSvcInfo[i].hIrSvc == hIrSvc) && (g_IrSvcInfo[i].ulContentsId == ulContentsId) &&
			(g_IrSvcInfo[i].eIrSvcActionType == IR_SVC_RECORD)))
			return &(g_IrSvcInfo[i]);
	}

	return NULL;
}
#endif

static	HINT32				local_cas_ir_Drv_GetSelectedIrSvcCount(void)
{
	HINT32		i, nCnt;

	nCnt = 0;
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated && g_IrSvcInfo[i].ucIsSelected)
			nCnt++;
	}

	return nCnt;
}

static	HINT32				local_cas_ir_Drv_GetIrSvcIndexFromPtr(IR_SERVICE_INFO *pstIrSvcInfo)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (&(g_IrSvcInfo[i]) == pstIrSvcInfo)
			return i;
	}

	return i;
}

static	void				local_cas_ir_Drv_ClearServiceInfo(IR_SERVICE_INFO *pstIrSvcInfo)
{
	HUINT16		i, j;

	if (pstIrSvcInfo->pucPMTRaw)
		OxCAS_Free(pstIrSvcInfo->pucPMTRaw);

#if	defined(IRPLAT_SYS_PVR)
	if (pstIrSvcInfo->pstPVRSvcInfo)
		OxCAS_Free(pstIrSvcInfo->pstPVRSvcInfo);
#endif

#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
		pstIrSvcInfo->hIrSvc = HANDLE_NULL;
		pstIrSvcInfo->ucStreamCount = 0;
		pstIrSvcInfo->ucIsValidPMT = 0;
		pstIrSvcInfo->pucPMTRaw = NULL;

		HxSTD_memset(&(pstIrSvcInfo->GlobalStatus), 0, sizeof(IR_STATUS_INFO));
		HxSTD_memset(&(pstIrSvcInfo->InternalStatus), 0, sizeof(IR_STATUS_INFO));
		HxSTD_memset(&(pstIrSvcInfo->DvbDescramblerRscStatus), 0, sizeof(IR_STATUS_INFO));
		HxSTD_memset(&(pstIrSvcInfo->EcmRscStatus), 0, sizeof(IR_STATUS_INFO));
		HxSTD_memset(&(pstIrSvcInfo->SmartcardRscStatus), 0, sizeof(IR_STATUS_INFO));
		HxSTD_memset(&(pstIrSvcInfo->MacrovisionRscStatus), 0, sizeof(IR_STATUS_INFO));
#else
		HxSTD_memset(pstIrSvcInfo, 0, sizeof(IR_SERVICE_INFO));
		pstIrSvcInfo->usSCELL_SvcHandle = INVALID_SOFTCELL_SVCHANDLE;
		pstIrSvcInfo->hIrSvc = HANDLE_NULL;
#endif

	for (i = 0; i < IR_SVC_MAX_PID; i++)
	{
		pstIrSvcInfo->astPidList[i].usPID = PID_NULL;
		pstIrSvcInfo->astPidList[i].IsLive = 0;
		pstIrSvcInfo->astPidList[i].IsRecord = 0;
		pstIrSvcInfo->astPidList[i].ucTypeFromPlay = eEsType_None;
		pstIrSvcInfo->astPidList[i].ucTypeFromPMT = eEsType_None;

		for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
		{
			pstIrSvcInfo->astPidList[i].ElemRsc[j].CharSeverity = 0;
			pstIrSvcInfo->astPidList[i].ElemRsc[j].usEcmPID = PID_NULL;
			pstIrSvcInfo->astPidList[i].ElemRsc[j].Valid = 0;
			pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.eSeverity = 0;
			pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.wStatus = 0;
			pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.eSource = 0;
		}
	}

	for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
	{
		for (i = 0; i < MAX_NUM_ECM_MONITOR_PID; i++)
		{
			pstIrSvcInfo->EcmMonitorInfo[j].ausEcmMonitorPID[i] = PID_NULL;
		}
	}

}


#define _____Public_Functions___________________________________________________________

#if	defined(IRPLAT_SYS_EMM_BASED_ORGID)

static	void			local_cas_ir_Drv_DumpTpList(void)
{
#ifdef	PRINT_EMM_FILTER_DEBUG
	HINT32		i, j;
	IR_TP_INFO	*pstIrTpInfo;

	HxLOG_Info("Dump TP List\n");
	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		pstIrTpInfo = &g_IrTpInfo[i];
		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
			HxLOG_Info("\t[%d](%d) action[%x] usUniqueId[%x] usOrgId[%x] state[%x] scellReady[%d] scellHandle[%d]\n", i, pstIrTpInfo->ucIsAllocated, pstIrTpInfo->hAction, pstIrTpInfo->usTsUniqueId, pstIrTpInfo->usOrgNetId, pstIrTpInfo->eState, pstIrTpInfo->ucIsSCELL_Ready, pstIrTpInfo->usSCELL_SvcHandle[j]);
		}
	}
#endif
}

static	IR_TP_INFO		*local_cas_ir_Drv_FindIrTpInfoByActionId(HUINT32 ulActionId)
{
	if (ulActionId < IR_HUMAX_MAX_TP)
		return &g_IrTpInfo[ulActionId];

	return NULL;

}

static	void	local_cas_ir_Drv_CAT_RequestEMMOpen(void)
{
	ia_result		result;
	msg_service_open_request_data_st	*pstSvcOpenReqData;

	pstSvcOpenReqData = (msg_service_open_request_data_st*)OxCAS_Malloc(sizeof(msg_service_open_request_data_st));
	if (pstSvcOpenReqData == NULL)
	{
		HxLOG_Print("Memory allocation error!!\n");
		return;
	}

	HxLOG_Info("------> Send EMM Open Request to SCELL\n");
	pstSvcOpenReqData->wServiceType = SERVICE_DVB_EMM;
	result = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, 0, sizeof(msg_service_open_request_data_st), (void*)pstSvcOpenReqData);
	if (result != IA_SUCCESS)
	{
		HxLOG_Print("Error : SCELL_Message (%x)", result);
	}
}

void	CAS_IR_SVC_UpdateIrTpInfo(void)
{
	HINT32		i,  j, k;
	IR_TP_INFO	*pstIrTpInfo;

	HxLOG_Info("++++++++++");
	HxLOG_Info("Update IR TP Info for Updating EMM\n");
	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		pstIrTpInfo = &g_IrTpInfo[i];
		if ((pstIrTpInfo->eState == eIRTP_State_Waiting) && (pstIrTpInfo->ucIsAllocated))
		{
			for (j = 0; j < IR_HUMAX_MAX_TP; j++)
			{
					if ((pstIrTpInfo->usTsId == g_IrTpInfo[j].usTsId)
					&& (pstIrTpInfo->usOrgNetId == g_IrTpInfo[j].usOrgNetId)
					&& (g_IrTpInfo[j].eState == eIRTP_State_Started))
					break;
			}
			if (j == IR_HUMAX_MAX_TP)
			{
				for( k = 0; k < CAS_MAX_NUM_OF_SC; k++ )
				{
					pstIrTpInfo->usSCELL_SvcType[k] = SERVICE_DVB_EMM;
					//	need NEW TS Start
					HxLOG_Info("---> Start EMM : slot[%d] TsId[0x%04x] usOrgId[0x%04x] actionHandle [0x%08x]\n", i, pstIrTpInfo->usTsId, pstIrTpInfo->usOrgNetId, pstIrTpInfo->hAction);
					pstIrTpInfo->eState = eIRTP_State_Started;
					HxLOG_Print("if (j==IR_HUMAX_MAX_TP)");
					local_cas_ir_Drv_CAT_RequestEMMOpen();
					HxLOG_Info("<--- Started EMM\n");
				}
			}
		}
	}

	local_cas_ir_Drv_DumpTpList();
}

HUINT32	CAS_IR_SVC_AddIrTpInfo(Handle_t hAction, HUINT16 usUniqueId, HUINT32 ulLength, HUINT8 *pucRawData)
{
	HBOOL			bRefreshData;
	HINT32			nDemuxId, i;
	HUINT8			ucActionId;
	IR_TP_INFO		*pstIrTpInfo;
	DbSvc_TsInfo_t		stTsInfo;

	ucActionId = svc_cas_SubSvcRmGetActionId(hAction);
	svc_cas_SubSvcRmGetResourceId(ucActionId, eRxRSCTYPE_DEMUX, &nDemuxId);
	DB_SVC_GetTsInfoByUniqueId(usUniqueId, &stTsInfo);

	pstIrTpInfo = local_cas_ir_Drv_FindIrTpInfoByActionId(ucActionId);
	if (pstIrTpInfo->ucIsAllocated)
	{
		//	if EMM is already open?
		if (pstIrTpInfo->ucIsSCELL_Ready)
		{
			if ((pstIrTpInfo->nCatLen == ulLength) && (memcmp(pstIrTpInfo->pCatRaw, pucRawData, ulLength) == 0))
			{
				HxLOG_Info("Same CAT encountered!!!\n");
				OxCAS_Free(pucRawData);
				return ERR_FAIL;
			}

			HxLOG_Info("EMM Updated actionId[%d]\n", ucActionId);
			CAS_IR_SVC_CLOSE_EMM(pstIrTpInfo);

			for( i = 0; i < CAS_MAX_NUM_OF_SC; i++ )
			{
				pstIrTpInfo->usSCELL_SvcHandle[i]	= INVALID_SOFTCELL_SVCHANDLE;
				pstIrTpInfo->usSCELL_SvcType[i]	= SERVICE_INVALID;
				pstIrTpInfo->ucIsSCELL_Ready	= 0;
			}
		}
	}

	pstIrTpInfo->eState				= eIRTP_State_Waiting;
	pstIrTpInfo->ucIsAllocated		= 1;
	pstIrTpInfo->hAction			= hAction;
	pstIrTpInfo->usTsUniqueId		= usUniqueId;
	pstIrTpInfo->nDmxId				= nDemuxId;
	pstIrTpInfo->usOrgNetId			= stTsInfo.usOnId;
	pstIrTpInfo->usTsId				= stTsInfo.usTsId;

	HxLOG_Info("hAction: 0x%x, usUniqueId: %d, nDemuxId: %d, stTsInfo.usOnId: 0x%x, stTsInfo.usTsId: 0x%x \n", hAction, usUniqueId, nDemuxId, stTsInfo.usOnId, stTsInfo.usTsId);
	for( i = 0; i < CAS_MAX_NUM_OF_SC; i++ )
	{
		pstIrTpInfo->usSCELL_SvcType[i]	= SERVICE_DVB_EMM;
	}

	pstIrTpInfo->IsValidCat			= TRUE;

	bRefreshData = TRUE;
	if (pstIrTpInfo->pCatRaw)
	{
		if (pstIrTpInfo->nCatLen == ulLength)
		{
			if (memcmp(pstIrTpInfo->pCatRaw, pucRawData, ulLength) == 0)
			{
				bRefreshData = FALSE;
			}
		}

		if (bRefreshData)
		{
			OxCAS_Free(pstIrTpInfo->pCatRaw);
			pstIrTpInfo->pCatRaw = NULL;
			pstIrTpInfo->nCatLen = 0;
		}
	}

	if (pstIrTpInfo->pCatRaw == NULL)
	{
		pstIrTpInfo->pCatRaw = pucRawData;
		pstIrTpInfo->nCatLen = ulLength;
	}
	else
	{
		OxCAS_Free(pucRawData);
	}

	HxLOG_Info("- Add New CAT for starting\n");

	local_cas_ir_Drv_DumpTpList();

	return ERR_OK;
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_StopTS(CAS_CAI_IF_msgTS *pMsgTS)
{
	HUINT8			ucActionId;
	IR_TP_INFO		*pstIrTpInfo;

	HxLOG_Info("-- Stop TS with action[%08x]\n", pMsgTS->hAction);

	ucActionId = svc_cas_SubSvcRmGetActionId(pMsgTS->hAction);
	pstIrTpInfo = local_cas_ir_Drv_FindIrTpInfoByActionId(ucActionId);
	if (pstIrTpInfo == NULL)
	{
		HxLOG_Print("not started TS info with Unique ID (%x)\n", pMsgTS->usTsUniqueId);
		return;
	}

	if (pstIrTpInfo->eState == eIRTP_State_Started)
	{
		//	pMsgTS->usTsUniqueID is next service's TS Unique Id.
		//	if same unique id with previous service...
		if (pstIrTpInfo->usTsUniqueId == pMsgTS->usTsUniqueId)
		{
			HxLOG_Info("Next TS UniqueID is SAME, then it must be continued\n");
			return;
		}
		else
		{
			HxLOG_Info("Current UniqueID (%x) Next UniqueId (%x)\n", pstIrTpInfo->usTsUniqueId, pMsgTS->usTsUniqueId);
		}

		HxLOG_Info("-- Started EMM <-- Close EMM\n");

		CAS_IR_SVC_CLOSE_EMM(pstIrTpInfo);
	}

	local_cas_ir_Drv_ClearTpInfo(pstIrTpInfo);

	local_cas_ir_Drv_DumpTpList();

	CAS_IR_SVC_UpdateIrTpInfo();
}

#else

/********************************************************************************
********************************************************************************/
static	IR_TP_INFO			*local_cas_ir_Drv_FindEmptyTpInfo(void)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		if (g_IrTpInfo[i].ucIsAllocated == 0)
			return &(g_IrTpInfo[i]);
	}

	return NULL;
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_StartTS(CAS_CAI_IF_msgTS *pMsgTS)
{
	HINT32			nDmxId;
	HERROR			hErr;
	IR_TP_INFO		*pstIrTpInfo;

	HxLOG_Info("\n");

	if (pMsgTS == NULL)
		return;

	pstIrTpInfo = local_cas_ir_Drv_FindIrTpInfo(pMsgTS->hAction);
	if (pstIrTpInfo != NULL)
	{
		HxLOG_Print("Already Started action [%x]\n", pMsgTS->hAction);
		return;
	}

	pstIrTpInfo = local_cas_ir_Drv_FindEmptyTpInfo();
	if (pstIrTpInfo == NULL)
	{
		HxLOG_Print("IR TP Info POOL is FULL\n");
		return;
	}

	pstIrTpInfo->ucIsAllocated	= 1;
	pstIrTpInfo->hAction		= pMsgTS->hAction;
	pstIrTpInfo->usTsUniqueId	= pMsgTS->usTsUniqueId;

	HxLOG_Info("pstIrTpInfo->hAction: 0x%x, pstIrTpInfo->usTsUniqueId: 0x%x \n", pstIrTpInfo->hAction, pstIrTpInfo->usTsUniqueId);

	hErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(pstIrTpInfo->hAction), eRxRSCTYPE_DEMUX, &nDmxId);
	if (hErr != HIR_OK)
	{
		HxLOG_Print("svc_cas_SubSvcRmGetResourceId error\n");
		return;
	}

	pstIrTpInfo->nDmxId			= nDmxId;

	HxLOG_Info("StartTS is OK!\n");
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_StopTS(CAS_CAI_IF_msgTS *pMsgTS)
{
	HIR_ERR			hErr;
	IR_TP_INFO		*pstIrTpInfo;
	//HUINT16			i;

	HxLOG_Info("hAction (%x) uniqutId (%x)\n", pMsgTS->hAction, pMsgTS->usTsUniqueId);
	pstIrTpInfo = local_cas_ir_Drv_FindIrTpInfo(pMsgTS->hAction);
	if (pstIrTpInfo == NULL)
	{
		HxLOG_Print("not started TS info with Unique ID (%x)\n", pMsgTS->usTsUniqueId);
		return;
	}

	//	pMsgTS->usTsUniqueID is next service's TS Unique Id.
	//	if same unique id with previous service...
	if (pstIrTpInfo->usTsUniqueId == pMsgTS->usTsUniqueId)
	{
		HxLOG_Print("Next TS UniqueID is SAME, then it must be continued\n");
		return;
	}
	else
	{
		HxLOG_Print("Current UniqueID (%x) Next UniqueId (%x)\n", pstIrTpInfo->usTsUniqueId, pMsgTS->usTsUniqueId);
	}

	HxLOG_Print("Close TP start\n");

	hErr = CAS_IR_SVC_CLOSE_EMM(pstIrTpInfo);
	if (hErr != HIR_OK)
	{
		HxLOG_Print("CAS_IR_SVC_CLOSE_EMM fail = %x\n", hErr);
		local_cas_ir_Drv_ClearTpInfo(pstIrTpInfo);
		return;
	}

	/*
		//	PVR moduleÀÏ‹š ÇÏ´Â ÀÏÀÌ ¸î°¡Áö ´õ ÀÖÀ½.
		//	±Ùµ¥ ³­ ÀÌÇØ°¡ ¾ÈµÊ... ³ªÁß¿¡ ´Ù½Ã »ý°¢ÇØº½...

			isNeedOpen = TRUE;
			openedTs = IR_HUMAX_MAX_TP;
			for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
			{
				if( g_IrTpInfo[i].hAction != HANDLE_NULL )
				{
					if (openedTs == IR_HUMAX_MAX_TP)
					{
						openedTs = i;
					}
					if (g_IrTpInfo[i].SCELL_LiveSvcType == SERVICE_DVB_EMM)
					{
						isNeedOpen = FALSE;
						break;
					}
				}
			}

			if (isNeedOpen && openedTs != IR_HUMAX_MAX_TP)
			{
				// CAT¸¦ ¹Þ¾ÒÀ» ¶§ EMM source service¸¦ open.
				pSvcOpenReqDataForLive = (msg_service_open_request_data_st *)OxCAS_Malloc(sizeof(msg_service_open_request_data_st)); // SOFTCELL$)C@L free.
				if (pSvcOpenReqDataForLive == NULL)
				{
					HxLOG_Print("[CAS_IR_SVC_StopTS] Error : Mem Alloc Error 3\n");
					return;
				}

				pSvcOpenReqDataForLive->wServiceType = SERVICE_DVB_EMM;
				g_IrTpInfo[openedTs].SCELL_LiveSvcType = SERVICE_DVB_EMM;
				HxLOG_Print("[SCELL_Message] MSG_SERVICE_OPEN_REQUEST: SERVICE_DVB_EMM\n");
				result = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, 0, sizeof(msg_service_open_request_data_st), (void *)pSvcOpenReqDataForLive);
				if( result != IA_SUCCESS )
				{
					// TODO
					HxLOG_Print("[CAT_UPDATE : Error From SCELL_Message for live]\n");
				}
			}

	*/

	local_cas_ir_Drv_ClearTpInfo(pstIrTpInfo);
}

/********************************************************************************
********************************************************************************/

#endif


/********************************************************************************
//	¼¼°¡Áö ¹æÇâ.
//	Live Only, Playback, Record
//	RecordÀÇ °æ¿ì Live Only·Î ¿ì¼± Ãâ¹ß --> PMT¹Þ°í °áÁ¤.
********************************************************************************/
void CAS_IR_SVC_StartService(CAS_CAI_IF_msgPID *pMsgPID)
{
	HINT32			nDmxId;

	HMSG			msg;
	Handle_t		hAction, hSvc, hIrSvc;
	HERROR			hErr;

	IR_SERVICE_INFO	*pstIrSvcInfo;

	HxLOG_Info("hIrSvc[%x]\n", pMsgPID->hIrSvc);

	hAction	= pMsgPID->hAct;
	hSvc	= pMsgPID->hSvc;
	hIrSvc	= pMsgPID->hIrSvc;

	//	find same service with given hIrSvc
	if ((pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc)))
	{
		//	if same service is existed, just go through
	}
	else
	{
		hErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hIrSvc), eRxRSCTYPE_DEMUX, &nDmxId);
		if(hErr != HIR_OK)
		{
			HxLOG_Print("svc_cas_SubSvcRmGetResourceId: Error: 0x%X\n", hErr);
			return;
		}

#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
#ifdef __COMBINE_DESCRAMBLE_SVC_WITH_DEMUX__
		pstIrSvcInfo = local_cas_ir_Drv_FindIrSvcInfoByDemuxId(hIrSvc, nDmxId);
#else
		pstIrSvcInfo = local_cas_ir_Drv_FindAllocIrSvcInfo(hIrSvc);
#endif

		if (pstIrSvcInfo == NULL)
		{
			//	if it is new service, going to make new IrSvc Info
			pstIrSvcInfo = local_cas_ir_Drv_FindEmptySvcInfo();
			if (pstIrSvcInfo == NULL)
			{
				//	slot is full.....
				HxLOG_Print("START SVC : service pool is full!!\n");
				HxLOG_Critical("\n\n");
				return;
			}
		}
#else
		//	if it is new service, going to make new IrSvc Info
		pstIrSvcInfo = CAS_IR_DRV_FindEmptySvcInfo();
		if (pstIrSvcInfo == NULL)
		{
			//	slot is full.....
			HxLOG_Print("START SVC : service pool is full!!\n");
			HxLOG_Critical("\n\n");
			return;
		}
#endif
		HxLOG_Print("New Ir Service \n");

		pstIrSvcInfo->hIrSvc		= hIrSvc;
		pstIrSvcInfo->hAction		= hAction;
		pstIrSvcInfo->hSvc			= hSvc;
		pstIrSvcInfo->ucDemuxId		= nDmxId;
		pstIrSvcInfo->eCasActionType= pMsgPID->eActType;

#if	defined(IRPLAT_SYS_PVR)
		if (SVC_META_GetContentIdByhAction(hAction, &pstIrSvcInfo->ulContentsId) != ERR_OK)
		{
			pstIrSvcInfo->ulContentsId = IR_RECORD_ID_INVALID;
			HxLOG_Print("Can't find ContentsID with given hAction[%x]\n", hAction);
		} else
			HxLOG_Print("ContentsId : [%x]\n", pstIrSvcInfo->ulContentsId);
#endif

		//	notification to ir event machine, event of starting service
		msg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_START;
		CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);		//	action??? How can i sent event to PVR Record Module??
	}

	pstIrSvcInfo->ucIsAllocated = 1;
	switch (pMsgPID->eActType)
	{
#if	defined(IRPLAT_SYS_PVR)
		case	eCAS_ActionType_Playback:
		case	eCAS_ActionType_Tsr_Play:
			{
	#if defined(IRPLAT_PVR_PHASE12)
				HUINT8	encryption;
	#endif
				//	Playback´Â Descramble Service¸¦ µ¿ÀÛ½ÃÅ°Áö ¾Ê´Â´Ù..
				pstIrSvcInfo->ucIsSCELLReady = 1;
				pstIrSvcInfo->eIrSvcActionType = IR_SVC_PLAYBACK;
				pstIrSvcInfo->ucIsSelected = 1;
				pstIrSvcInfo->usSCELL_SvcHandle = 0xFFFF;				//	add invalid scell handle
	#if defined(IRPLAT_PVR_PHASE12)
				pstIrSvcInfo->usSCELL_SvcType = SERVICE_PVR_PLAYBACK;
				CAS_IR_Drv_GetPlaybackEncryptionMode(pstIrSvcInfo->hIrSvc, &encryption);
				if (encryption)
					CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_CHECKMSK, 0, 0);
				else
					CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DESCRAMBLE, 0, 0);
	#endif
	#if	defined(IRPLAT_PVR_DRM)
				pstIrSvcInfo->usSCELL_SvcType = SERVICE_PVR_DRM;
				CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM, 0, 0);
	#endif
			}
			break;

		case	eCAS_ActionType_Tsr_Rec:
		case	eCAS_ActionType_Recording:
			pstIrSvcInfo->eIrSvcActionType = IR_SVC_RECORD_BASE;
				if (pstIrSvcInfo->ulContentsId == IR_RECORD_ID_INVALID)
			{
				HxLOG_Info("\n\n --> Contents id is not valid\n\n");
			}
			break;
#endif
		default:
			pstIrSvcInfo->eIrSvcActionType = IR_SVC_LIVE;
			break;
	}
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_IamSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect)
{
	HMSG		msg;
	char		cSeverity;

	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("hIrSvc = %x\n", pMsgSelect->hIrSvc);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(pMsgSelect->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("can't find irSvcHandle in IrSvcList\n");
		return;
	}

	if (pstIrSvcInfo->ucIsSelected)		//	card change or re-inserted...
	{
		HxLOG_Print("Already selected resource!!\n");
		cSeverity = SEVERITY_TO_CHAR(pstIrSvcInfo->GlobalStatus.stStatus.eSeverity);

		if (pstIrSvcInfo->GlobalStatus.Valid)
		{
			HxLOG_Print("source = %d, status = %d, severity = %d\n", pstIrSvcInfo->GlobalStatus.stStatus.eSource, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSeverity);
			if (Is_D29_4(pstIrSvcInfo->GlobalStatus.stStatus))
			{
				HxLOG_Error("\n\n Fix Unknown Message Error !!! ---> Please contact kimjh when you see this assert line!!!\n\n");

				msg.usMsgClass = IREVT_CLASS_CAS_DESCRAMBLED_WELL;
				msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&(pstIrSvcInfo->GlobalStatus.stStatus) );
				msg.ulParam= (HUINT32)MAKE_IR_STATUSCODE(cSeverity, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSource);
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
				if(pstIrSvcInfo->eIrSvcActionType == IR_SVC_LIVE)
				{// Start ECM Monitor for Preview
					CAS_IR_DRV_SendServiceDefineOfEcmMonitor(pstIrSvcInfo->usSCELL_SvcHandle, 1);
				}
#endif
			}
			else
			{
				if ((!Is_I34_9(pstIrSvcInfo->GlobalStatus.stStatus)) && (pstIrSvcInfo->GlobalStatus.stStatus.eSeverity != STATUS_INFO) && (pstIrSvcInfo->GlobalStatus.stStatus.eSeverity != STATUS_INFO_DND))
				{
					msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
					msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&(pstIrSvcInfo->GlobalStatus.stStatus) );
					msg.ulParam= (HUINT32)MAKE_IR_STATUSCODE(cSeverity, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSource);

					CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
				}
			}
		}
	}
	else
	{
		if (CAS_IR_DRV_GetSelectedIrSvcCount() < IR_SOFTCELL_MAX_SERVICE)
		{
			//	if service needs descramble, (LIVE, RECORD_BASE)
			if (pstIrSvcInfo->eIrSvcActionType == IR_SVC_LIVE || pstIrSvcInfo->eIrSvcActionType == IR_SVC_RECORD_BASE)
			{
				CAS_IR_SVC_OPEN_REQUEST_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo));
				pstIrSvcInfo->ucIsSelected = 1;
			}
		}
		else
		{
			HxLOG_Print("Error - Softcell service resource is FULL!!!\n");
			HxLOG_Critical("\n\n");
			return;
		}
	}
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_IamDeSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("hIrSvc : %x\n", pMsgSelect->hIrSvc);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(pMsgSelect->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("not registered service!!!\n");
		return;
	}

	if (pstIrSvcInfo->ucIsSelected)
	{
		switch (pstIrSvcInfo->eIrSvcActionType)
		{
			HxLOG_Print("pstIrSvcInfo->eIrSvcActionType: %d \n", pstIrSvcInfo->eIrSvcActionType);

			case	IR_SVC_LIVE:
				CAS_IR_SVC_CLOSE_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), pMsgSelect->CloseFlag);
				break;
#if	defined(IRPLAT_SYS_PVR)
			case	IR_SVC_RECORD_BASE:
				{
					//	stop PVR record
					if (pstIrSvcInfo->pstPVRSvcInfo)
					{
						CAS_IR_Drv_CloseRecord(pstIrSvcInfo->hIrSvc);
					}
					else
						HxLOG_Print("Record Service but, can't find Irdeto PVR service!!!\n");

					//	stop descramble
					CAS_IR_SVC_CLOSE_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), pMsgSelect->CloseFlag);
				}
				break;
			case	IR_SVC_PLAYBACK:
				CAS_IR_Drv_ClosePlayback(pstIrSvcInfo->hIrSvc);
				break;
			case	IR_SVC_RECORD:
				//	it will be deselected automatically when IR_SVC_RECORD_BASE is deselected
				break;
#else
			case	IR_SVC_RECORD_BASE:
			case	IR_SVC_PLAYBACK:
			case	IR_SVC_RECORD:
				HxLOG_Critical("\n\n");
				break;
#endif
		}
		pstIrSvcInfo->ucIsSelected = 0;
	}
	else
	{
		HxLOG_Print("Registered service BUT not SELECTED!!!\n");
	}
}

/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_StopService(CAS_CAI_IF_msgPID *pMsgPID)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Print("hIrSvc = %x\n", pMsgPID->hIrSvc);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(pMsgPID->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("not registered SERVICE!!!\n");
	}
	else
	{
		HxLOG_Print("Stop and Clear Service\n");
		if (pstIrSvcInfo->ucIsSelected)
		{
			switch (pstIrSvcInfo->eIrSvcActionType)
			{
				case	IR_SVC_LIVE:
					CAS_IR_SVC_CLOSE_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), FALSE);
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
					CAS_IR_DRV_SendServiceDefineOfEcmMonitor(pstIrSvcInfo->usSCELL_SvcHandle, 0);
#endif
					break;
#if	defined(IRPLAT_SYS_PVR)
				case	IR_SVC_RECORD_BASE:
					{
						//	stop PVR record
						CAS_IR_Drv_CloseRecord(pstIrSvcInfo->hIrSvc);

						//	stop descramble
						CAS_IR_SVC_CLOSE_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), FALSE);
					}
					break;
				case	IR_SVC_PLAYBACK:
					CAS_IR_Drv_ClosePlayback(pstIrSvcInfo->hIrSvc);
					break;
				case	IR_SVC_RECORD:
					//	it will be deselected automatically when IR_SVC_RECORD_BASE is deselected
					break;
#else
				case	IR_SVC_RECORD_BASE:
				case	IR_SVC_PLAYBACK:
				case	IR_SVC_RECORD:
					HxLOG_Critical("\n\n");
					break;
#endif
			}
			pstIrSvcInfo->ucIsSelected = 0;
		}
		local_cas_ir_Drv_ClearServiceInfo(pstIrSvcInfo);
	}
}


/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_SetPID(CAS_CAI_IF_msgPIDList *pMsgPIDList)
{
	HINT32				nNeedUpdate = 0, i = 0, j = 0;
	IR_SERVICE_INFO		*pstIrSvcInfo = NULL;

	HxLOG_Info("hIrSvc[%x]\n", pMsgPIDList->hIrSvc);

	pstIrSvcInfo = local_cas_ir_Drv_FindIrSvcInfo(pMsgPIDList->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("not registered service!! hIrSvc = %x\n", pMsgPIDList->hIrSvc);
		return;
	}

	//	check pid from MsgPIDList
	for (i = 0; i < pMsgPIDList->ulPidInfoCnt; i++)
	{
		if (pMsgPIDList->astPidInfo[i].usPID == IRDRV_NULL_PID)
			continue;

		for (j = 0; j < pstIrSvcInfo->ucStreamCount; j++)
		{
			if ((pMsgPIDList->astPidInfo[i].usPID == pstIrSvcInfo->astPidList[j].usPID) && (pMsgPIDList->astPidInfo[i].ucType == pstIrSvcInfo->astPidList[j].ucTypeFromPlay))
				break;
		}

		//	found same Pid
		if (j < pstIrSvcInfo->ucStreamCount)
			continue;

		//	new pid found
		pstIrSvcInfo->astPidList[pstIrSvcInfo->ucStreamCount].usPID = pMsgPIDList->astPidInfo[i].usPID;
		pstIrSvcInfo->astPidList[pstIrSvcInfo->ucStreamCount].ucTypeFromPlay = pMsgPIDList->astPidInfo[i].ucType;
		pstIrSvcInfo->ucStreamCount++;
		nNeedUpdate = TRUE;
	}

	//	if pid is updated, re-define DVB
	if (nNeedUpdate && pstIrSvcInfo->ucIsSCELLReady && pstIrSvcInfo->ucIsSelected)
	{
		CAS_IR_SVC_DEFINE_DVB(local_cas_ir_Drv_GetIrSvcIndexFromPtr(pstIrSvcInfo));
	}
}


/********************************************************************************
********************************************************************************/
void	CAS_IR_SVC_SetPIDforRecording(IR_SERVICE_INFO *pstIrSvcInfo)
{
	HINT32	i;

	SvcCas_PmtInfo_t		*pstPmtInfo;
	CAS_CAI_IF_msgPIDList	stMsgPidList;

	HxLOG_Info("Forced SetPID for recording service with IrSvc[%x]\n", pstIrSvcInfo->hIrSvc);

	memset(&stMsgPidList, 0, sizeof(CAS_CAI_IF_msgPIDList));
	stMsgPidList.hIrSvc = pstIrSvcInfo->hIrSvc;
	pstPmtInfo = svc_cas_MgrSubGetPmtInfo(pstIrSvcInfo->hAction);
	if (pstPmtInfo)
	{
		stMsgPidList.ulPidInfoCnt = 0;
		for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
		{
			switch (pstPmtInfo->stElementInfo[i].eEsType)
			{
				case	eEsType_Audio:
				case	eEsType_Data:
				case	eEsType_Video:
				case	eEsType_Subtitle:
				case	eEsType_Teletext:
					stMsgPidList.astPidInfo[stMsgPidList.ulPidInfoCnt].ucType	= pstPmtInfo->stElementInfo[i].eEsType;
					stMsgPidList.astPidInfo[stMsgPidList.ulPidInfoCnt].usPID	= pstPmtInfo->stElementInfo[i].usPid;
					stMsgPidList.ulPidInfoCnt++;
					break;
				default:
					break;
			}
		}
		if (stMsgPidList.ulPidInfoCnt)
		{
			CAS_IR_SVC_SetPID(&stMsgPidList);
		}
	}
}

#define _____External_Functions___________________________________________________________

/********************************************************************************
********************************************************************************/
IR_TP_INFO			*CAS_IR_DRV_FindIrTpInfo(Handle_t hAction)
{
	return local_cas_ir_Drv_FindIrTpInfo(hAction);
}

/********************************************************************************
********************************************************************************/
IR_TP_INFO			*CAS_IR_DRV_FindIrTpInfoBySCELL(HUINT16 usSCELLHandle)
{
	return local_cas_ir_Drv_FindIrTpInfoBySCELL(usSCELLHandle);

}

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfo(Handle_t hIrSvc)
{
	return local_cas_ir_Drv_FindIrSvcInfo(hIrSvc);
}

/********************************************************************************
********************************************************************************/
HINT32	CAS_IR_DRV_GetSelectedIrSvcCount(void)
{
	return local_cas_ir_Drv_GetSelectedIrSvcCount();
}

/********************************************************************************
********************************************************************************/
HINT32	CAS_IR_DRV_GetIrSvcIndexFromPtr(IR_SERVICE_INFO *pstIrSvcInfo)
{
	return local_cas_ir_Drv_GetIrSvcIndexFromPtr(pstIrSvcInfo);
}

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByContentsId(HUINT32 ulContentId)
{
	return local_cas_ir_Drv_FindIrSvcInfoByContentsId(ulContentId);
}

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByPvrInfo(void *pvrInfo)
{
	return local_cas_ir_Drv_FindIrSvcInfoByPvrInfo(pvrInfo);
}

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(HUINT16 usSCELLHandle)
{
	return local_cas_ir_Drv_FindIrSvcInfoBySCELL(usSCELLHandle);
}


#if defined(IRPLAT_PVR_DRM)
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByActionId(Handle_t hAction)
{
	return local_cas_ir_Drv_FindIrSvcInfoByActionId(hAction);
}
#endif

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO		*CAS_IR_DRV_FindEmptySvcInfo(void)
{
	return local_cas_ir_Drv_FindEmptySvcInfo();
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_GetSvcCasKind(Handle_t hAction, eIrDrvCaType *eSvcCasKind)
{
	HUINT32		i;

	HxLOG_Info("\n");

	for( i = 0; i  < IR_HUMAX_MAX_SERVICE; i++ )
	{
		if ((g_IrSvcInfo[i].hAction == hAction) || (svc_cas_SubSvcRmGetActionId(g_IrSvcInfo[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction)))
		{
			*eSvcCasKind = g_IrSvcInfo[i].eSvcCasKind;
			return HIR_OK;
		}
	}
	return HIR_NOT_OK;
}

/********************************************************************************
********************************************************************************/
HIR_ERR	CAS_IR_DRV_GetSvcRecordMode (Handle_t hAction, HUINT8 *pucRecordMode)
{
	HUINT32		i;

	HxLOG_Info("\n");

	for( i = 0; i  < IR_HUMAX_MAX_SERVICE; i++ )
	{
		if ((g_IrSvcInfo[i].hAction == hAction) || (svc_cas_SubSvcRmGetActionId(g_IrSvcInfo[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction)))
		{
			*pucRecordMode = g_IrSvcInfo[i].ucRecordMode;
			return HIR_OK;
		}
	}
	return HIR_NOT_OK;
}

/********************************************************************************
********************************************************************************/
HUINT32 XSVC_CAS_IR_DRV_GetScInfo(HUINT8 ucSlotID, IR_DRV_SC_INFO *IrScInfo)
{
	HxLOG_Info("\n");

	memcpy(IrScInfo, &g_IrDrvScInfo[ucSlotID], sizeof(IR_DRV_SC_INFO) );

#if defined(CONFIG_DEBUG)
	HxLOG_Info("Status: %s\n", IrScInfo->stIrScApiInfo.szStatus);

	HxLOG_Info("Status: %s\n", IrScInfo->stIrScApiInfo.szStatus);
	HxLOG_Info("Number: %s\n", IrScInfo->stIrScApiInfo.szNumber);
	HxLOG_Info("Type: %04X\n", IrScInfo->stIrScApiInfo.usType);
	HxLOG_Info("Version: %s\n", IrScInfo->stIrScApiInfo.szVersion);
	if( IrScInfo->stIrScApiInfo.szVersion[0] >= 0x35 )
	{
		HxLOG_Info("Build: %02d\n", IrScInfo->stIrScApiInfo.ucBuild);
		HxLOG_Info("Variant: %02X\n", IrScInfo->stIrScApiInfo.ucVariant);
		HxLOG_Info("Patch Level: %04X\n", IrScInfo->stIrScApiInfo.usPatchLevel);
		HxLOG_Info("Owner ID: %04X\n", IrScInfo->stIrScApiInfo.usOwnerID);
	}
	else
	{
		HxLOG_Info("Build:\n");
		HxLOG_Info("Variant:\n");
		HxLOG_Info("Patch Level:\n");
		HxLOG_Info("Owner ID:\n");
	}

	HxLOG_Info("Nationality: %s\n", IrScInfo->stIrScApiInfo.szNationality);
#endif

	return HIR_OK;
}

// ewlee 20100817
HUINT32 XSVC_CAS_IR_DRV_CopyScInfo(HUINT8 ucSlotID, HUINT8 *pData)
{
	IR_DRV_SC_INFO IrScInfo;

	XSVC_CAS_IR_DRV_GetScInfo(ucSlotID, &IrScInfo);
	HxSTD_memcpy(pData, &(IrScInfo.szScID[0]), IR_SC_INFO_ID_LENGTH);
	return HIR_OK;
}

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
HBOOL XSVC_CAS_IR_DRV_IsSameWithSavedSmartcardNumber()
{
	return HUMAX_VdSc_IsSameWithSavedSmartcardNumber();
}

HUINT32 XSVC_CAS_IR_DRV_GetSavedSmartcardNumber (HUINT8 *pData)
{
	return CAS_IR_NVR_ReadSmartcardNumber (pData, IR_SC_INFO_NUMBER_LENGTH);
}

HBOOL XSVC_CAS_IR_DRV_SavedNumberIsInitial ()
{
	return CAS_IR_NVR_SavedNumberIsInitial ();
}

#endif

/********************************************************************************
********************************************************************************/
HBOOL XSVC_CAS_IR_DRV_GetHWErrorOfSC(HUINT8 ucSlotID)
{
	if(g_IrDrvScInfo[ucSlotID].ErrorReason == VD_SC_HARDWARE_ERROR)
		return TRUE;
	else
		return FALSE;
}


/********************************************************************************
********************************************************************************/
HBOOL XSVC_CAS_IR_DRV_MakeNumericIRStatus(HUINT32 ulIRMessage, HUINT16 *pusStatus)
{
	HUINT8 ucSeverity;


	ucSeverity = (char)(ulIRMessage >> 24);
	if(ucSeverity == 'E' || ucSeverity == 'e')
	{
		*pusStatus = 0x0E00;
	}
	else
	if(ucSeverity == 'D' || ucSeverity == 'd')
	{
		*pusStatus = 0x0D00;
	}
	else
		return FALSE;

	*pusStatus |= ((ulIRMessage & 0x00FFFFFF) >> 8);

	return TRUE;
}

/********************************************************************************
********************************************************************************/
HBOOL XSVC_CAS_IR_DRV_GetGlobalStatusOfSC(HUINT8 ucSlotId, HUINT32 *pucStatus)
{
	if(ucSlotId >= HUMAX_VD_SC_SLOT_COUNT)
		return FALSE;

	if(g_IrDrvScInfo[ucSlotId].stIrScApiInfo.stStatus.eSeverity == 'E')
	{
		*pucStatus = 0x0E00;
	}
	else
	if(g_IrDrvScInfo[ucSlotId].stIrScApiInfo.stStatus.eSeverity == 'D')
	{
		*pucStatus = 0x0D00;
	}
	else
		return FALSE;

	*pucStatus |= g_IrDrvScInfo[ucSlotId].stIrScApiInfo.stStatus.usStatus;
	return TRUE;
}

/********************************************************************************
********************************************************************************/
ia_result XSVC_CAS_IR_DRV_PinCodeRequest(HUINT8 ucSlotId, ia_pin_action_e ePinAction, ia_pin_type_e ePinType,
	HUINT16 usOldPin, HUINT16 usNewPin)
{
	msg_pin_code_query_st *pstPinCode;
	ia_result irReturn;

	pstPinCode = (msg_pin_code_query_st*)OxCAS_Malloc(sizeof(msg_pin_code_query_st));
	if(pstPinCode == NULL)
		return IA_FAIL;

	HxSTD_memset(pstPinCode, 0, sizeof(msg_pin_code_query_st));

	pstPinCode->wResourceId = ucSlotId;
	pstPinCode->ePinAction = ePinAction;
	pstPinCode->ePinType = ePinType;

	HxSTD_memcpy(pstPinCode->abPinCode, &usOldPin, 2);
	HxSTD_memcpy(pstPinCode->abNewPinCode, &usNewPin, 2);

	irReturn = SCELL_Message(MSG_PIN_CODE_QUERY, 0, sizeof(msg_pin_code_query_st), pstPinCode);

	if(IA_SUCCESS != irReturn)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


HBOOL XSVC_CAS_IR_DRV_GetSCProductExistance(HUINT8	 ucSlotId, HUINT8 *bExist)
{
	if(ucSlotId >= HUMAX_VD_SC_SLOT_COUNT || bExist == NULL)
		return FALSE;

	if(g_IrDrvScInfo[ucSlotId].bProductInit == IA_FAIL)
		return FALSE;


	if(g_IrDrvScInfo[ucSlotId].bProductExistance == IA_SUCCESS)
		*bExist = TRUE;
	else
		*bExist = FALSE;
	return TRUE;
}



/********************************************************************************
********************************************************************************/
HUINT32	CAS_IR_DRV_GetIrSvcType(ia_word16 irSvcHandle)
{
	HINT32		i, j;

	HxLOG_Info("SCELL svcHandle (%x)\n", irSvcHandle);
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucIsAllocated)
		{
			//	PlaybackÀº PVR Service·Î¸¸ µî·ÏÀÌ µÈ´Ù.
			//	Playback½Ã BodyÀÇ °æ¿ì Service HandleÀÌ InvalidÇÑ °ªÀÌ µÊÀ¸·Î Slave¸¸À» ºÁ¾ß ÇÏ°í
			//	BodyÀÇ Service Handle´Â PLAYBACK½Ã¿¡´Â ¹«½ÃÇÑ´Ù.
			if ((g_IrSvcInfo[i].ucIsSCELLReady) && (g_IrSvcInfo[i].usSCELL_SvcHandle == irSvcHandle) && (g_IrSvcInfo[i].eIrSvcActionType != IR_SVC_PLAYBACK))
				break;
#if	defined(IRPLAT_SYS_PVR)
			if (g_IrSvcInfo[i].pstPVRSvcInfo)
			{
				if ((g_IrSvcInfo[i].pstPVRSvcInfo->ucIsSCELLReady) && (g_IrSvcInfo[i].pstPVRSvcInfo->usSCELL_SvcHandle == irSvcHandle))
					break;
			}
#endif
		}
	}

	if (i < IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Print("GetIRSvcType : [%x]\n", g_IrSvcInfo[i].eIrSvcActionType);
		if (g_IrSvcInfo[i].usSCELL_SvcHandle == irSvcHandle)
		{
			switch (g_IrSvcInfo[i].eIrSvcActionType)
			{
#if	defined(IRPLAT_SYS_PVR)
				case	IR_SVC_PLAYBACK:
#if	defined(IRPLAT_PVR_PHASE12)
					return SERVICE_PVR_PLAYBACK;
#endif
#if	defined(IRPLAT_PVR_DRM)
					return SERVICE_PVR_DRM;
#endif
#endif
				default:
					//	RECORD_BASE ---> same with DVB_DESCRAMBLE
					return SERVICE_DVB_DESCRAMBLE;
			}
		}
		else
		{
#if	defined(IRPLAT_SYS_PVR)
			//	pstPVRSvcInfo --> only for PVR service. (2types are existed!!!)
			switch (g_IrSvcInfo[i].eIrSvcActionType)
			{
				case	IR_SVC_PLAYBACK:
#if	defined(IRPLAT_PVR_PHASE12)
					return SERVICE_PVR_PLAYBACK;
#endif
#if	defined(IRPLAT_PVR_DRM)
					return SERVICE_PVR_DRM;
#endif
				default:
#if	defined(IRPLAT_PVR_PHASE12)
					return SERVICE_PVR_RECORD;
#endif
#if	defined(IRPLAT_PVR_DRM)
					return SERVICE_PVR_DRM;
#endif
			}
#endif
		}
	}

	HxLOG_Print("finding SCELL svcHandle in TP List\n");
	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
			if (g_IrTpInfo[i].ucIsAllocated && g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].usSCELL_SvcHandle[j] == irSvcHandle)
				return SERVICE_DVB_EMM;
		}
	}

	return SERVICE_INVALID;
}

/********************************************************************************
********************************************************************************/
HUINT8	CAS_IR_DRV_GetIrSvcInfoIDXbyActHandle(Handle_t hAction)
{
	HINT32	i;

	HxLOG_Info("hAction (%x)\n", hAction);
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if ((g_IrSvcInfo[i].hAction == hAction) || (svc_cas_SubSvcRmGetActionId(g_IrSvcInfo[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction)))
			return i;
	}

	return IR_HUMAX_MAX_SERVICE;
}

/********************************************************************************
********************************************************************************/
IR_SERVICE_INFO*	CAS_IR_DRV_GetIrSvcInfobyActHandle(Handle_t hAction)
{
	HINT32	i;

	HxLOG_Info("hAction (%x)\n", hAction);
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if ((g_IrSvcInfo[i].hAction == hAction) || (svc_cas_SubSvcRmGetActionId(g_IrSvcInfo[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction)))
			return &g_IrSvcInfo[i];
	}

	return NULL;
}

/********************************************************************************
********************************************************************************/

HIR_ERR	CAS_IR_DRV_GetActionHandleByDemuxId(HUINT8 in_ucDemuxId, Handle_t *out_hAct)
{
	HINT32		i;

	HxLOG_Info("demuxId(%x)\n", in_ucDemuxId);
	//initial output-pointers' value
	*out_hAct = HANDLE_NULL;
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].ucDemuxId == in_ucDemuxId && g_IrSvcInfo[i].ucIsAllocated)
		{
			*out_hAct = g_IrSvcInfo[i].hAction;
			return HIR_OK;
		}
	}

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		if (g_IrTpInfo[i].nDmxId == in_ucDemuxId && g_IrTpInfo[i].ucIsAllocated)
		{
			*out_hAct = g_IrTpInfo[i].hAction;
			return HIR_OK;
		}
	}

	HxLOG_Print("Error : Can't find DemuxID(%x) in Service, TP List\n", in_ucDemuxId);

	return HIR_NOT_OK;
}

/********************************************************************************
********************************************************************************/
HIR_ERR	CAS_IR_DRV_GetDemuxIdByActionHandle(Handle_t in_hAct, HUINT8 *out_ucDemuxId)
{
	HINT32		i;

	HxLOG_Info("handle (%x)\n", in_hAct);
	//initial output-pointers' value
	*out_ucDemuxId = 0xff;
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].hAction == in_hAct)
		{
			*out_ucDemuxId = g_IrSvcInfo[i].ucDemuxId;
			return HIR_OK;
		}
	}

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		if (g_IrTpInfo[i].hAction == in_hAct)
		{
			*out_ucDemuxId = g_IrTpInfo[i].nDmxId;
			return HIR_OK;
		}
	}

	HxLOG_Print("Error : Can't find Action Handle(%x) in Service, TP List\n", in_hAct);

	return HIR_NOT_OK;
}

IR_SERVICE_INFO*	CAS_IR_DRV_GetLiveSvcInfo (void)
{
	HINT32		i;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].eIrSvcActionType == IR_SVC_LIVE)
		{
			return &g_IrSvcInfo[i];
		}
	}

	return NULL;
}
