
/**
	@file     ox_svc_ttx.c
	@brief    MW의 teletext service 모듈

	Module: MW/TLTXT			 	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>
#include <pal_pipe.h>
#include <util.h>
#include <pal_datastream.h>
#include <svc_si.h>
#include <pal_pes.h>

/** @brief local header file  */
#include "svc_ttx.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define DATA_STREAM_TTX_PES				"ttxpes"

#define DATA_STREAMER_EBU_SVCSTART		11111
#define DATA_STREAMER_EBU_SVCSTOP		22222
#define TTX_BACKGROUND_CACHING

#define PES_POOL_SIZE 		  10

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t		hAction;
	HUINT32			ulDemuxId;
	HUINT16			usPid;
	HBOOL			bAlreadyStopped;
} SvcTtx_Service_t;

typedef struct {
	HUINT32 len;
	HUINT8 *PES;
} SvcTtx_PesPump_t;

STATIC HINT32 			s_nPumpCount = 0;
STATIC SvcTtx_Service_t	s_sstTtxSvc;
STATIC HULONG			s_sstTtxSvcSemId;
SvcTtx_PesPump_t 		sPES_Pump[PES_POOL_SIZE];

STATIC PostMsgToMgrCb_t s_fnSvcTtx_PostMsgToAp = NULL;


STATIC void svc_ttx_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcTtx_PostMsgToAp)
	{
		 (* s_fnSvcTtx_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}
}

STATIC HERROR svc_ttx_PumpPES (Handle_t hAction, HUINT32 ulDemuxId, HUINT32 len, HUINT8 *PES)
{
	HINT32	i,size;
	HINT32			newLen;
	HUINT8			*pucBuffer, *p;
	if (len == 0 || PES == NULL)
	{
		return ERR_FAIL;
	}
#if defined(CONFIG_MW_TELETEXT)
	if(s_nPumpCount < PES_POOL_SIZE)
	{
		sPES_Pump[s_nPumpCount].PES = HLIB_STD_MemAlloc(len);
		sPES_Pump[s_nPumpCount].len = len;
		HxSTD_MemCopy(sPES_Pump[s_nPumpCount].PES ,PES,len);
		s_nPumpCount++;

	}
	if(s_nPumpCount== PES_POOL_SIZE)
	{
		for(i=0,size=0;i<PES_POOL_SIZE; i++)
			size += 	sizeof(sPES_Pump[i].len) + sPES_Pump[i].len;
		newLen = size + 8;
		p = pucBuffer = HLIB_STD_MemAlloc(newLen);
		HxMACRO_Put32Bit(p, PES_POOL_SIZE); 	p += 4;
		HxMACRO_Put32Bit(p, PES_POOL_SIZE);		p += 4;

		for(i=0;i<PES_POOL_SIZE; i++)
		{
			HxMACRO_Put32Bit(p,sPES_Pump[i].len);p+= 4;
			HxSTD_MemCopy(p,sPES_Pump[i].PES, sPES_Pump[i].len);
			p+= sPES_Pump[i].len;
		}
		HxLOG_Debug("TTX_PES pump! len[%d] \n",newLen);
		PAL_DSTREAM_Broadcast(DATA_STREAM_TTX_PES, newLen, (void*)pucBuffer);
		HLIB_STD_MemFree(pucBuffer);
		for(i=0;i< PES_POOL_SIZE;i++)
		{
			if(sPES_Pump[i].PES !=NULL)
				HLIB_STD_MemFree(sPES_Pump[i].PES);
		}

		s_nPumpCount=0;
	}
#endif
	return ERR_OK;
}

STATIC HERROR	svc_ttx_StopService (SvcTtx_Service_t *pstTtx)
{
	HUINT8	*pucBuffer, *p;

	HxLOG_Trace();

	if(NULL == pstTtx)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if(pstTtx->bAlreadyStopped == TRUE)
	{
		return ERR_OK;
	}

	(void)PAL_PES_DisableTtx(pstTtx->hAction, pstTtx->ulDemuxId);
	(void)PAL_PES_StopTtx(pstTtx->ulDemuxId);
	pstTtx->bAlreadyStopped = TRUE;

	HxLOG_Print(HxANSI_COLOR_YELLOW("hAction = 0x%x, ActionId:%d, ulDemuxId:%d, usPid = 0x%x\n")
			, pstTtx->hAction, PAL_PIPE_GetActionId(pstTtx->hAction), pstTtx->ulDemuxId, pstTtx->usPid);

#if defined(CONFIG_MW_TELETEXT)
	p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(10);
	if(p)
	{
		HxMACRO_Put32Bit(p,DATA_STREAMER_EBU_SVCSTOP);  p += 4;
		HxMACRO_Put32Bit(p,DATA_STREAMER_EBU_SVCSTOP);	p += 4;
		PAL_DSTREAM_Broadcast(DATA_STREAM_TTX_PES, 10, (void*)pucBuffer);

		HLIB_STD_MemFree(pucBuffer);
	}
#endif
	return ERR_OK;
}

STATIC HERROR	svc_ttx_StartService (SvcTtx_Service_t *pstTtx)
{
	HERROR		hErr = ERR_FAIL;
	HUINT8		*pucBuffer, *p;

	HxLOG_Trace();

	if(NULL == pstTtx)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if (PID_NULL == pstTtx->usPid)
	{
		HxLOG_Debug("NULL PID\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_TELETEXT)
	p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(10);
	if(NULL != p)
	{
		HxMACRO_Put32Bit(p,DATA_STREAMER_EBU_SVCSTART); p += 4;
		HxMACRO_Put32Bit(p,DATA_STREAMER_EBU_SVCSTART);	p += 4;
		PAL_DSTREAM_Broadcast(DATA_STREAM_TTX_PES, 10, (void*)pucBuffer);

		HLIB_STD_MemFree(pucBuffer);
	}
#endif

	hErr = PAL_PIPE_GetResourceId(PAL_PIPE_GetActionId(pstTtx->hAction), eRxRSCTYPE_DEMUX, &pstTtx->ulDemuxId);

	HxLOG_Print(HxANSI_COLOR_YELLOW("hAction = 0x%x, ActionId:%d, ulDemuxId:%d, usPid = 0x%x\n")
			, pstTtx->hAction, PAL_PIPE_GetActionId(pstTtx->hAction), pstTtx->ulDemuxId, pstTtx->usPid);

	if (hErr != ERR_OK)
	{
		HxLOG_Error("Cannot Get DEMUX id from Resource Manager\n");
		return ERR_FAIL;
	}

	hErr = PAL_PES_StartTtx (pstTtx->hAction, pstTtx->ulDemuxId, pstTtx->usPid);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Cannot Start pal.PES\n");
		return ERR_FAIL;
	}

#if defined(TTX_BACKGROUND_CACHING)
	(void) PAL_PES_EnableTtx(pstTtx->hAction, pstTtx->ulDemuxId);
#endif

	pstTtx->bAlreadyStopped = FALSE;
	return ERR_OK;
}

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
#define ____________API_FUNCTIONS_____________


void SVC_TTX_Init(PostMsgToMgrCb_t fnPostMsg)
{
	HERROR		hErr;

	HxLOG_Trace();

	s_sstTtxSvc.bAlreadyStopped = TRUE;
	s_sstTtxSvc.ulDemuxId = RxRSCID_NULL;
	s_sstTtxSvc.usPid = PID_NULL;
	s_sstTtxSvc.hAction = HANDLE_NULL;

	s_fnSvcTtx_PostMsgToAp = fnPostMsg;

	hErr = VK_SEM_Create((unsigned long *)&s_sstTtxSvcSemId, "TTX_SVC_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(hErr!=VK_OK)			HxLOG_Critical("Critical Eror!!!\n");

	/* Digital TTX : Callback을 등록 한다. */
	hErr = PAL_PES_RegisterTtxCallBack((PalPes_TTX_Notify_t)svc_ttx_PumpPES);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	hErr = PAL_PES_StartTtxTask();
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

}


HERROR SVC_TTX_StartService(Handle_t hAction, HUINT16 usPid)
{
	HERROR	hErr;

	HxLOG_Trace();
	HxLOG_Print("hAction:0x%X, usPid:0x%X\n", hAction, usPid);

	VK_SEM_Get(s_sstTtxSvcSemId);

	(void)svc_ttx_StopService(&s_sstTtxSvc);

	s_sstTtxSvc.hAction = hAction;
	s_sstTtxSvc.usPid = usPid;
	hErr = svc_ttx_StartService(&s_sstTtxSvc);

	VK_SEM_Release(s_sstTtxSvcSemId);
    return hErr;
}


HERROR SVC_TTX_StopService(Handle_t hAction, ApiSyncMode_t syncMode)
{
	HERROR	hErr;

	HxLOG_Trace();
	HxLOG_Print("hAction:0x%X, syncMode:%d\n", hAction, syncMode);

	VK_SEM_Get(s_sstTtxSvcSemId);

	hErr = svc_ttx_StopService(&s_sstTtxSvc);

	VK_SEM_Release(s_sstTtxSvcSemId);

    if (hErr == ERR_OK && syncMode == eAsyncMode)
	{
		svc_ttx_PostMsgToAp(eSEVT_TTX_STOPPED, hAction, 0, 0, 0);
	}
    return hErr;
}

