/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ca_drv.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/
#include <pal_sc.h>

#include <db_svc.h>

#include <_svc_cas_sub_dev_api.h>

#include "ir_platform.h"

#include "s3_cadrv.h"
#include "s3_decoder_msg.h"
#include "s3_tm_msg.h"
#include "s3_vd_macrovision.h"
#include "s3_vd_copycontrol.h"
#include "s3_smartcard.h"
#include "s3_prod_if_msg.h"
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
#include "s3_maturity_rating.h"
#endif

#include "vd_humax.h"

#include "os_drv.h"
#include "ir_svc.h"
#include "ir_evt.h"
#include "ir_msg.h"
#include "ir_main.h"
#include "ir_si.h"
#include "ir_status.h"
#include "ir_util.h"

#include "ir_msg_scell.h"

#if defined(CONFIG_MW_CAS_IRDETO_PPV)
//#include "ir_ppv.h"
#endif
#include "ir_fta_block.h"

#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_api.h>



	#define	DESC_RESOURCE		1

#define	DEBUG_DRMINFO

#if defined(CONFIG_MW_CAS_IRDETO_SOFTCELL_3_11)
 #define	RESOURCE_ECM_MONITOR		2
#else
 #define	RESOURCE_ECM_MONITOR		1
#endif

SOFTCELL_VERSION_DATA	ClientVersionData[1];
SOFTCELL_VERSION_DATA	ComponentsVersionData[32];

#if defined(CONFIG_DEBUG)
#define DBG_IRCADRV_PRINT_OPCODE
#define DBG_IRCADRV_SVC_RSC_STATUS_TRACE
#endif

extern	IR_SERVICE_INFO		g_IrSvcInfo[];
extern	IR_TP_INFO			g_IrTpInfo[];
extern	IR_DRV_SC_INFO		g_IrDrvScInfo[];

#if defined(IRPLAT_SYS_PVR)
HUINT32	CAS_IR_SVC_OPEN_REPLY_PVR(HUINT16 handle, HUINT16 ir_service_type, HUINT16 wOpSeqNo);
HUINT32	CAS_IR_SVC_DEFINE_PVR(HUINT16 wIrServiceHandle, HUINT16 ir_service_type, void *pvrInfo);
#endif

void CAS_IR_SoftcellMsgProc_Version(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Error(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Service(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_ServiceStatus(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Monitor(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Smartcard(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Decoder(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_TMInfo(CAS_IR_BASE_MSG msgIR);
void CAS_IR_SoftcellMsgProc_Product(CAS_IR_BASE_MSG msgIR);
void CAS_IR_DRV_ProductReply(CAS_IR_BASE_MSG msgIR);
#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
void CAS_IR_DRV_ExtendedProductListReply(CAS_IR_BASE_MSG msgIR);
void CAS_IR_DRV_SetSCExtendedProductList(msg_extended_product_list_reply_st *pstAnyProductReply);
#endif

/********************************************************************
*	External function prototypes
********************************************************************/
extern void CAS_IR_Dbg_ConvertResourceToString(HUINT32 source);
#if defined(CONFIG_MW_CAS_IRDETO_IPPV_CB) && defined(CONFIG_OP_DIGITURK)
extern HUINT8 XSVC_CAS_IR_SetRemoteChannelMessage(HUINT8* pucBuf, HUINT8 ucLength);
#endif

#ifdef _____ECM_EMM_MONITORING_____
extern HUINT16	s_usSCELLforMonitor;
extern HUINT8	s_ucTypeForMonitor;
	#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
		extern HUINT32	s_EmmMonitorCount[CAS_MAX_NUM_OF_SC];
	#endif
#endif

/********************************************************************
*	Global variables
********************************************************************/
// TODO: make API. jichoi.
// TODO: check it whether it's needed.
//HUINT8 g_ucScHardreset[HUMAX_VD_SC_SLOT_COUNT];


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
void local_cas_ir_CA_DRV_DBG_PrintOpCode(HUINT32 eOpCode);



#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
static void local_cas_ir_Drv_ClearPidListInfo(HUINT32 mainIdx, HUINT32 subIdx)
{
	HUINT16		i;
	if(mainIdx >= IR_HUMAX_MAX_SERVICE || subIdx >= IR_SVC_MAX_PID)
	{
		HxLOG_Assert(0);
		return;
	}

	g_IrSvcInfo[mainIdx].astPidList[subIdx].usPID = PID_NULL;
	g_IrSvcInfo[mainIdx].astPidList[subIdx].IsLive = 0;
	g_IrSvcInfo[mainIdx].astPidList[subIdx].IsRecord = 0;
	g_IrSvcInfo[mainIdx].astPidList[subIdx].ucTypeFromPlay = eEsType_None;
	g_IrSvcInfo[mainIdx].astPidList[subIdx].ucTypeFromPMT = eEsType_None;

	for (i = 0; i < CAS_MAX_NUM_OF_SC; i++)
	{
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].CharSeverity = 0;
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].usEcmPID = PID_NULL;
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].Valid = 0;
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].stStatus.eSeverity = 0;
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].stStatus.wStatus = 0;
		g_IrSvcInfo[mainIdx].astPidList[subIdx].ElemRsc[i].stStatus.eSource = 0;
	}

	return;
}
#endif

#define _______________API_TO_SOFTCELL___________________
/********************************************************************
*	function name :	 CR_DRV_Message
*	input :
*	output :
*	description : SoftCell¿Ã manufactur¿« CA task∑Œ message∏¶ ∫∏≥ª±‚ ¿ß«ÿ ªÁøÎ«œ¥¬ «‘ºˆ.
********************************************************************/
ia_result CA_DRV_Message( ca_drv_message_type_e eOpcode, ia_word16 wOpSeqNo, ia_word16 wLength, void *pvMessage )
{

	CAS_IR_BASE_MSG 	*pMsg = NULL;

	HxLOG_Info("eOpCode(0x%X), wOpSeqNo(0x%X), wLength(0x%X), pvMessage@(0x%X)\n", eOpcode, wOpSeqNo, wLength, (HUINT32) pvMessage);

	pMsg = (CAS_IR_BASE_MSG *)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
	if (pMsg == NULL)
	{
		return IA_FAIL;
	}
	HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
	pMsg->msg.SOFTCELL.eOpCode = (HUINT32)eOpcode;
	pMsg->msg.SOFTCELL.wOpSeqNo = (HUINT16)wOpSeqNo;
	pMsg->msg.SOFTCELL.wLength = (HUINT16)wLength;
	if( wLength != 0 )
	{
		pMsg->msg.SOFTCELL.pvMessage = (HUINT8 *)OxCAS_Malloc(wLength);
		if (pMsg->msg.SOFTCELL.pvMessage == NULL)
		{
			OxCAS_Free(pMsg);
			return IA_FAIL;
		}

		if( pvMessage == NULL )
		{
			if (pMsg->msg.SOFTCELL.pvMessage != NULL)
			{
				OxCAS_Free(pMsg->msg.SOFTCELL.pvMessage);
			}
			if (pMsg != NULL)
			{
				OxCAS_Free(pMsg);
			}
			return IA_FAIL;
		}

		HxSTD_memcpy(pMsg->msg.SOFTCELL.pvMessage, (HUINT8 *)pvMessage, wLength);
		pMsg->msg.SOFTCELL.ucFreeFlag = TRUE;
		if( pvMessage != NULL )
		{
			OxCAS_Free(pvMessage);
		}
	}
	else
	{
		pMsg->msg.SOFTCELL.ucFreeFlag = FALSE;
	}

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_FROM_SOFTCELL, (void *)pMsg);

	return IA_SUCCESS;

}

/********************************************************************
*	function name :	 CA_DRV_FatalError
*	input : void(None)
*	output : None
*	description : Fatal errorπﬂª˝ Ω√ø° softcell¿Ã call«œ¥¬ «‘ºˆ.
********************************************************************/
void CA_DRV_FatalError( ia_word16 wError )
{
	HxLOG_Error("FATAL ERROR FROM SOFTCELL : 0x%X ###\n", wError);

	switch(wError)
	{
	case IA_ERROR_MEMORY:
		HxLOG_Error(" == IA_ERROR_MEMORY ==\n");
		break;

	case IA_ERROR_TASK:
		HxLOG_Error(" == IA_ERROR_TASK ==\n");
		break;

	case IA_ERROR_RESOURCE:
		HxLOG_Error(" == IA_ERROR_RESOURCE ==\n");
		break;

	case IA_ERROR_MESSAGE:
		HxLOG_Error(" == IA_ERROR_MESSAGE ==\n");
		break;

	case IA_ERROR_ASSERT:
		HxLOG_Error(" == IA_ERROR_ASSERT ==\n");
		break;

	case IA_ERROR_FATAL:
		HxLOG_Error(" == IA_ERROR_FATAL ==\n");
		break;

	case IA_ERROR_RULE:
		HxLOG_Error(" == IA_ERROR_RULE ==\n");
		break;

	default:
		HxLOG_Error(" == UNKNOWN FATAL ERROR ==\n");
		break;
	}
}

#define _______________MSG_TO_SOFTCELL__________________


/********************************************************************
*	function name :	 CAS_IR_SVC_OPEN_REQUEST_DVB
*	input : Service Info Index
*	output : void
*	description :
********************************************************************/
void CAS_IR_SVC_OPEN_REQUEST_DVB(HUINT32 SvcIndex)
{
	msg_service_open_request_data_st	*pSvcOpenReqDataForLive;

	ia_result	result;
	IR_SERVICE_INFO *pIrSvcInfo = NULL;

	if (SvcIndex >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("Invalid svc index!!\n");
		return;
	}

	pIrSvcInfo = &(g_IrSvcInfo[SvcIndex]);

	HxLOG_Print("\n ### CAS_IR_SVC_OPEN_REQUEST_DVB ### \n\n");
	HxLOG_Print("- SvcIndex(%d) - hIrSvc(%x)\n", SvcIndex, pIrSvcInfo->hIrSvc);

	if (pIrSvcInfo->usSCELL_SvcType == SERVICE_DVB_DESCRAMBLE &&
		pIrSvcInfo->usSCELL_SvcHandle != INVALID_SOFTCELL_SVCHANDLE)
	{
		HxLOG_Print("DVB DESCRAMBLE is already opened!!\n");
		return;
	}

	pSvcOpenReqDataForLive = (msg_service_open_request_data_st *)OxCAS_Malloc(sizeof(msg_service_open_request_data_st)); // SOFTCELL$)C@L free.
	if (pSvcOpenReqDataForLive == NULL)
	{
		HxLOG_Print(" Mem Alloc Error\n");
		return;
	}


	// View Kind∞° live¿Ã∞≈≥™ record¿Ã∞≈≥™ SERVICE_DVB_DESCRAMBLE¿∫ «œ≥™ open«ÿæﬂ «‘.
	pSvcOpenReqDataForLive->wServiceType = SERVICE_DVB_DESCRAMBLE;
	pIrSvcInfo->usSCELL_SvcType = SERVICE_DVB_DESCRAMBLE;

	//identify multiple softcell message with the same OpCode,
	//reqply_OpSeqNo is the same with the query_OpSeqNo
	HxLOG_Print("Send MSG_SERVICE_OPEN_REQUEST, wServiceType: %d, pIrSvcInfo->usSCELL_SvcType: %d \n",  pSvcOpenReqDataForLive->wServiceType, pIrSvcInfo->usSCELL_SvcType);
	result = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, (ia_word16)SvcIndex, sizeof(msg_service_open_request_data_st), pSvcOpenReqDataForLive);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("Error From SCELL_Message for live.[0x%X]\n", result);
	}
}


HIR_ERR CAS_IR_SVC_CLOSE_EMM(IR_TP_INFO *pIrTpInfo)
{
	msg_service_close_data_st	*pSvcCloseDataForLive;
	ia_result	result;
	HUINT32		i;
	HIR_ERR		nRet = HIR_OK;

	HxLOG_Print("\n >>> CAS_IR_SVC_CLOSE_EMM >>> \n\n");

	for( i = 0; i < CAS_MAX_NUM_OF_SC; i++ )
	{
		if (pIrTpInfo->usSCELL_SvcType[i] != SERVICE_DVB_EMM)
		{
			HxLOG_Error("  SVC_CLOSE_EMM ERROR \n");
			nRet = HIR_INVALID_PARAMETER;
			continue;
		}

		pSvcCloseDataForLive = (msg_service_close_data_st *)OxCAS_Malloc(sizeof(msg_service_close_data_st)); // SOFTCELL$)C@L free.
		if (pSvcCloseDataForLive == NULL)
		{
			HxLOG_Error("  Mem Alloc Error 3\n");
			nRet = HIR_INVALID_MEMORY;
			continue;
		}
		pSvcCloseDataForLive->wServiceHandle = pIrTpInfo->usSCELL_SvcHandle[i];
		HxLOG_Print("[SCELL_Message] MSG_SERVICE_CLOSE (%d)\n", pIrTpInfo->usSCELL_SvcHandle[i]);
		result = SCELL_Message(MSG_SERVICE_CLOSE, 0, sizeof(msg_service_close_data_st), (void *)pSvcCloseDataForLive);
		if( result != IA_SUCCESS )
		{
			// TODO
			HxLOG_Error("Error From SCELL_Message for EMM service close]\n");
			nRet = HIR_ERR_SCELL_INTERNAL;
		}
	}
	return nRet;
}

/********************************************************************
*	function name :	 CAS_IR_SVC_DEFINE_DVB_FAKE_CLOSE
*	input : Service Index of Irdeto CAS MW
*	output : void
*	description : Ω«¡¶ Close∞° æ∆¥— NULL ¡§∫∏∏¶ ¿¸¥ﬁ«œπ«∑Œº≠ √ ±‚»≠ »ø∞˙
********************************************************************/
HUINT32 CAS_IR_SVC_DEFINE_DVB_FAKE_CLOSE(HUINT32 idx)
{
	msg_service_define_data_st	SvcDefineDataForLive;
	ia_result	result;
	HUINT8		*p = NULL, index;
	HUINT8		*pServiceDefineData = NULL;
	HUINT8		*pECMRscData = NULL;
	HUINT8		*pDESRscData = NULL;
	HUINT8		*pSCRscData = NULL;

	HUINT16		ServiceDefineLength = 0;
	HUINT16		ECMRscLength = 0;
	HUINT16		DESRscLength = 0;
	HUINT16		SCRscLength = 0;
	HUINT16		MacroVRscLength = 0;

	msg_resource_st		MsgDescRsc[DESC_RESOURCE];
	msg_resource_st		MsgSCRsc[CAS_MAX_NUM_OF_SC];

	HUINT16		MRDefineLength=0;
	HUINT16		MRRscLength =0;
//	HMSG	hMsg;

#if defined(IRPLAT_FN_COPY_CONTROL)
	HUINT8				*pMacroVRscData = NULL;
	msg_resource_st		MsgMacroVisionRsc;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	HUINT8		*pMRRscData=NULL;
	msg_resource_st		MsgMRRsc;
	msg_mr_define_st 	st_mr_define;
#endif

	if (idx >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("Invalid svc index!!\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("\n ### CAS_IR_SVC_DEFINE_DVB_FAKE_CLOSE ### \n\n");
	HxLOG_Print("svcIndex(%d) DemuxId(%x) SCELL(%x)\n", idx, g_IrSvcInfo[idx].ucDemuxId, g_IrSvcInfo[idx].usSCELL_SvcHandle);

	HxSTD_memset(&SvcDefineDataForLive, 0, sizeof(msg_service_define_data_st));
	HxSTD_memset(&MsgDescRsc, 0, (sizeof(msg_resource_st) * DESC_RESOURCE));
	HxSTD_memset(&MsgSCRsc, 0, (sizeof(msg_resource_st) * CAS_MAX_NUM_OF_SC));

	SvcDefineDataForLive.wServiceHandle = g_IrSvcInfo[idx].usSCELL_SvcHandle;

	if(CAS_MAX_NUM_OF_SC > 1)
		SvcDefineDataForLive.bResourceCount = 4;	// 2 smartcard¿Œ ∞ÊøÏ Resource ¡ı∞° « ø‰.
	else
		SvcDefineDataForLive.bResourceCount = 3;	// resource : ECM/Descrambler/Smartcard

#if defined(IRPLAT_FN_COPY_CONTROL)
	SvcDefineDataForLive.bResourceCount++;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	SvcDefineDataForLive.bResourceCount++;
#endif


	// ECM RESOURCE
	pECMRscData = CAS_IR_SI_ParseNullPMT((HUINT8)idx, g_IrSvcInfo[idx].ucDemuxId, &ECMRscLength);
	if( pECMRscData == (HUINT8 *)NULL )
	{
		HxLOG_Print(" : ParsePMT\n");
		return HIR_NOT_OK;
	}

	// Descrambler RESOURCE
	for (index = 0; index < DESC_RESOURCE; index++)
	{
		MsgDescRsc[index].wResourceType = RESOURCE_DVB_DESCRAMBLER;

		MsgDescRsc[index].wResourceId = g_IrSvcInfo[idx].ucDemuxId;
		if( g_IrSvcInfo[idx].ucDemuxId == HMX_VD_DMX_MAX_NUM) //HUMAX_VD_DEMUX_COUNT
		{
			HxLOG_Print(": no empty g_IrSvcInfo[%d].DescInfoIndex\n", idx);
		}
		MsgDescRsc[index].wResourceMgnt = MANAGE_UPDATE;
	}
	pDESRscData = (HUINT8 *)&MsgDescRsc;
	DESRscLength = (sizeof(msg_resource_st) * DESC_RESOURCE);

	// Smartcard RESOURCE
	for (index = 0; index < CAS_MAX_NUM_OF_SC; index++)
	{
		MsgSCRsc[index].wResourceType = RESOURCE_SMARTCARD;
		MsgSCRsc[index].wResourceId = index;
		MsgSCRsc[index].wResourceMgnt = MANAGE_UPDATE;
	}
	pSCRscData = (HUINT8 *)MsgSCRsc;
	SCRscLength = (sizeof(msg_resource_st) * CAS_MAX_NUM_OF_SC);

#if defined(IRPLAT_FN_COPY_CONTROL) // defs. name is changed from "defined(SUPPORT_MACROVISION)".
	/* macro vision(copy contorl) resource */
	MsgMacroVisionRsc.wResourceType = RESOURCE_COPY_CONTROL;
	MsgMacroVisionRsc.wResourceId = idx;
	MsgMacroVisionRsc.wResourceMgnt = MANAGE_UPDATE;
	pMacroVRscData = (HUINT8 *)&MsgMacroVisionRsc;
	MacroVRscLength = sizeof(msg_resource_st);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	/* Maturity rating resource */
	MsgMRRsc.wResourceType = RESOURCE_MATURITY_RATING;
	MsgMRRsc.wResourceId = idx;
	MsgMRRsc.wResourceMgnt =  MANAGE_UPDATE;

	pMRRscData = (HUINT8 *)&MsgMRRsc;
	MRRscLength = sizeof(msg_resource_st);

	/* when the resource is initially defined, the value shall be 0xFFFF */
	st_mr_define.abMRPin[0]= 0xFF;
	st_mr_define.abMRPin[1]= 0xFF;

	MRDefineLength = sizeof(msg_mr_define_st);
#endif


	ServiceDefineLength = sizeof(msg_service_define_data_st) + ECMRscLength + DESRscLength + SCRscLength + MacroVRscLength \
	+MRRscLength	+MRDefineLength;

	pServiceDefineData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
	if (pServiceDefineData == NULL)
	{
		HxLOG_Print("[CAS_IR_SVC_DEFINE_DVB] Error : Mem Alloc Error 3\n");
		if( pECMRscData != NULL )
			OxCAS_Free(pECMRscData);
		return HIR_NOT_OK;
	}
	p = pServiceDefineData;

	HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForLive, sizeof(msg_service_define_data_st));
	p += sizeof(msg_service_define_data_st);

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
{
	// Maturiry rating RESOURCE
	HxSTD_memcpy(p, pMRRscData, MRRscLength);
	p += MRRscLength;

	HxSTD_memcpy(p, (HUINT8*)&st_mr_define, MRDefineLength );
	p += MRDefineLength;
}
#endif


#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
	HxSTD_memcpy(p, pSCRscData, SCRscLength);
	p += SCRscLength;
	HxSTD_memcpy(p, pDESRscData, DESRscLength);
	p += DESRscLength;
	HxSTD_memcpy(p, pECMRscData, ECMRscLength);
	p += ECMRscLength;
#else
	HxSTD_memcpy(p, pECMRscData, ECMRscLength);
	p += ECMRscLength;
	HxSTD_memcpy(p, pDESRscData, DESRscLength);
	p += DESRscLength;
	HxSTD_memcpy(p, pSCRscData, SCRscLength);
	p += SCRscLength;
#endif

#if defined(IRPLAT_FN_COPY_CONTROL)
	HxSTD_memcpy(p, pMacroVRscData, MacroVRscLength);
	p += MacroVRscLength;
#endif
	if( pECMRscData != NULL )
		OxCAS_Free(pECMRscData);

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE\n");
#if 0
{
	int xx;
	printf("jichoi: ServiceDefineLength = %d\n", ServiceDefineLength);
	for(xx=0; xx<ServiceDefineLength; xx++)
	{
		printf("%02X ", pServiceDefineData[xx] );
		if(xx % 8 ==7 )
			printf("\n");
	}
	printf("\n");
}
#endif
	result = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefineData);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("[START SERVICE : Error From SCELL_Message for record]\n");
	}

	//anyways report app.
	//hMsg.usMsgClass = IREVTBW_SVC_CLOSED;
	//CAS_IR_EVT_SendEvent(g_IrSvcInfo[idx].hAction, &hMsg);

	return HIR_OK;
}

void CAS_IR_SVC_CLOSE_DVB(HUINT32 SvcIndex, HBOOL bInstOff)
{
#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
	HUINT16 	i;
#endif
	msg_service_close_data_st	*pSvcCloseDataForLive;
	ia_result	result;
	IR_SERVICE_INFO *pIrSvcInfo = NULL;

	if (SvcIndex >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Print("SvcIndex >= IR_HUMAX_MAX_SERVICE\n");
		return;
	}
	pIrSvcInfo = &(g_IrSvcInfo[SvcIndex]);

	HxLOG_Print("\n >>> CAS_IR_SVC_CLOSE_DVB >>> \n\n");
	HxLOG_Info("SvcIndex(%d) hIrSvc(%x)\n", SvcIndex, pIrSvcInfo->hIrSvc);

	if (pIrSvcInfo->ucIsSCELLReady == 0)
	{
		HxLOG_Print("ucIsSCELLReady is FALSE!!\n");
	}

	if(pIrSvcInfo->usSCELL_SvcHandle == INVALID_SOFTCELL_SVCHANDLE)
	{
		HxLOG_Print("\nWARNING  CAS_IR_SVC_CLOSE_DVB Invalid Servicehandle - cannot close\n");
		return;
	}
	if(pIrSvcInfo->usSCELL_SvcType == SERVICE_INVALID)
	{
		HxLOG_Print("\nWARNING  service is not open state\n");
		return;
	}

#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
	if(bInstOff == FALSE)
	{
	CAS_IR_SVC_DEFINE_DVB_FAKE_CLOSE(SvcIndex);

	pIrSvcInfo->hSvc = HANDLE_NULL;	// hPlay?
	pIrSvcInfo->ucIsSelected = 0;	// IsRunning

	for( i = 0; i < IR_SVC_MAX_PID; i++ )
	{
		local_cas_ir_Drv_ClearPidListInfo(SvcIndex, i);
	}
	}
	else
	{
		HxLOG_Print("***** SEND SVC CLOSE - SvcIndex(%d)\n", SvcIndex);
		pSvcCloseDataForLive = (msg_service_close_data_st *)OxCAS_Malloc(sizeof(msg_service_close_data_st));
		if (pSvcCloseDataForLive == NULL)
		{
			HxLOG_Print("[CAS_IR_SVC_CLOSE_DVB] Error : Mem Alloc Error\n");
			return;
		}
		pSvcCloseDataForLive->wServiceHandle = pIrSvcInfo->usSCELL_SvcHandle;

		HxLOG_Print("[SCELL_Message] MSG_SERVICE_CLOSE(%d)\n", pSvcCloseDataForLive->wServiceHandle);
		result = SCELL_Message(MSG_SERVICE_CLOSE, 0, sizeof(msg_service_close_data_st), (void *)pSvcCloseDataForLive);
		if( result != IA_SUCCESS )
		{
			// TODO
			HxLOG_Print("[STOP SERVICE : Error From SCELL_Message]\n");
		}

		pIrSvcInfo->ucIsSCELLReady = 0;
		pIrSvcInfo->usSCELL_SvcType = SERVICE_INVALID;

		// SoftCell¿Ã ªÁøÎ«œ¥¬ handle¿∫ 0∫Œ≈Õ Ω√¿€«œπ«∑Œ √ ±‚»≠∏¶ FFFF∑Œ «‘.
		pIrSvcInfo->usSCELL_SvcHandle = INVALID_SOFTCELL_SVCHANDLE;
	}
#else
	HxLOG_Print("***** SEND SVC CLOSE - SvcIndex(%d)\n", SvcIndex);
	pSvcCloseDataForLive = (msg_service_close_data_st *)OxCAS_Malloc(sizeof(msg_service_close_data_st));
	if (pSvcCloseDataForLive == NULL)
	{
		HxLOG_Print("[CAS_IR_SVC_CLOSE_DVB] Error : Mem Alloc Error\n");
		return;
	}
	pSvcCloseDataForLive->wServiceHandle = pIrSvcInfo->usSCELL_SvcHandle;

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_CLOSE(%d)\n", pSvcCloseDataForLive->wServiceHandle);
	result = SCELL_Message(MSG_SERVICE_CLOSE, 0, sizeof(msg_service_close_data_st), (void *)pSvcCloseDataForLive);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("[STOP SERVICE : Error From SCELL_Message]\n");
	}

	pIrSvcInfo->ucIsSCELLReady = 0;
	pIrSvcInfo->usSCELL_SvcType = SERVICE_INVALID;

	// SoftCell¿Ã ªÁøÎ«œ¥¬ handle¿∫ 0∫Œ≈Õ Ω√¿€«œπ«∑Œ √ ±‚»≠∏¶ FFFF∑Œ «‘.
	pIrSvcInfo->usSCELL_SvcHandle = INVALID_SOFTCELL_SVCHANDLE;
#endif

	return;
}


HUINT32 CAS_IR_SVC_DEFINE_EMM(HUINT16 irServiceHandle)
{
	HUINT8						*pEMMRscData = NULL;
	HUINT16						EMMRscLength = 0;
	msg_resource_st					MsgSCRsc;
	HUINT8						*pSCRscData = NULL;
	HUINT16						SCRscLength = 0;
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	msg_resource_st				MsgDORsc;
	HUINT8						*pDORscData = NULL;
	HUINT16						DORscLength = 0;
#endif
	HUINT16						ServiceDefineLength = 0;
	HUINT8						*pServiceDefinetData = NULL;
	HUINT8						*pTemp;
	msg_service_define_data_st	SvcDefineDataForLive;
	ia_result					result;
	HINT32 						i, j;

	HxLOG_Print("\n ### CAS_IR_SVC_DEFINE_EMM ### \n\n");

	for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
	{
		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
#ifdef	IRPLAT_SYS_EMM_BASED_ORGID
			if (g_IrTpInfo[i].ucIsAllocated && (g_IrTpInfo[i].eState == eIRTP_State_Started) && (g_IrTpInfo[i].usSCELL_SvcHandle[j] == INVALID_SOFTCELL_SVCHANDLE))
#else
			if (g_IrTpInfo[i].ucIsAllocated && g_IrTpInfo[i].usSCELL_SvcHandle[j] == INVALID_SOFTCELL_SVCHANDLE)//2SCø°º≠ EMM define¿Ã 2 2π¯ µ… ∂ß, Ready∏¶ check«ÿπˆ∏Æ∏È µŒ π¯¬∞ EMM¿Ã define æ»µ«æÓπˆ∏≤
#endif
			{
				g_IrTpInfo[i].usSCELL_SvcHandle[j] = irServiceHandle;
				g_IrTpInfo[i].ucIsSCELL_Ready = 1;
				break;
			}
		}

		if ( j != CAS_MAX_NUM_OF_SC)
			break;
	}

	if( i == IR_HUMAX_MAX_TP )
	{
		HxLOG_Print("- no empty g_IrTpInfo\n");
		return 1;
	}

	HxSTD_memset(&SvcDefineDataForLive, 0, sizeof(msg_service_define_data_st));
	HxSTD_memset(&MsgSCRsc, 0, sizeof(msg_resource_st));
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	HxSTD_memset(&MsgDORsc, 0, sizeof(msg_resource_st));
#endif

	SvcDefineDataForLive.wServiceHandle = g_IrTpInfo[i].usSCELL_SvcHandle[j];
	HxLOG_Info("[CAS_IR_SVC_DEFINE_EMM] g_IrTpInfo[%d].usSCELL_SvcHandle[%d] : (%d) \n", i,j,g_IrTpInfo[i].usSCELL_SvcHandle[j]);
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	SvcDefineDataForLive.bResourceCount = 3;	// resource : EMM/Smartcard/OOB_EMM
#else
	SvcDefineDataForLive.bResourceCount = 2;	// resource : EMM/Smartcard
#endif


	// EMM RESOURCE
	if( g_IrTpInfo[i].IsValidCat == FALSE)
	{
		HxLOG_Print(": CAT is not valid yet.\n");

		return 1;
	}

	pEMMRscData = CAS_IR_SI_ParseCAT( g_IrTpInfo[i].pCatRaw, g_IrTpInfo[i].nDmxId, &EMMRscLength);

	if( pEMMRscData == (HUINT8 *)NULL )
	{
		HxLOG_Print(" : ParseCAT\n");
		return 1;
	}

	// Smartcard RESOURCE
	MsgSCRsc.wResourceType = RESOURCE_SMARTCARD;
	MsgSCRsc.wResourceId = j;
	MsgSCRsc.wResourceMgnt = MANAGE_UPDATE;
	pSCRscData = (HUINT8 *)&MsgSCRsc;
	SCRscLength = sizeof(msg_resource_st);

#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	// OOB EMM
	MsgDORsc.wResourceType = RESOURCE_DVB_OOB;
	MsgDORsc.wResourceId = 0;
	MsgDORsc.wResourceMgnt = MANAGE_UPDATE;
	pDORscData = (HUINT8 *)&MsgDORsc;
	DORscLength = sizeof(msg_resource_st);
#endif

#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	ServiceDefineLength = sizeof(msg_service_define_data_st) + EMMRscLength + SCRscLength + DORscLength;
#else
	ServiceDefineLength = sizeof(msg_service_define_data_st) + EMMRscLength + SCRscLength;
#endif

	pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
	if (pServiceDefinetData == NULL)
	{
		if( pEMMRscData != NULL )
			OxCAS_Free(pEMMRscData);
		HxLOG_Print("[CAS_IR_SVC_DEFINE_EMM] Error : Mem Alloc Error 2\n");
		return 2;
	}
	pTemp = pServiceDefinetData;

	HxSTD_memcpy(pTemp, (HUINT8 *)&SvcDefineDataForLive, sizeof(msg_service_define_data_st));
	pTemp += sizeof(msg_service_define_data_st);
	HxSTD_memcpy(pTemp, pEMMRscData, EMMRscLength);
	pTemp += EMMRscLength;
	HxSTD_memcpy(pTemp, pSCRscData, SCRscLength);
	pTemp += SCRscLength;
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	HxSTD_memcpy(pTemp, pDORscData, DORscLength);
	pTemp += DORscLength;
#endif

	if( pEMMRscData != NULL )
	{
		OxCAS_Free(pEMMRscData);
	}

#if 0 //DEBUG
{
HUINT32	i;
HxLOG_Print("[MSG_SERVICE_DEFINE : ServiceDefineLength(0x%X)]\n", ServiceDefineLength);
for( i = 0; i < ServiceDefineLength; i++ )
{
HxLOG_Print("%02X ", pServiceDefinetData[i]);
if( i % 16 == 15 )
HxLOG_Print("\n");
}
HxLOG_Print("\n");
}
#endif

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE ==> SVC_DEFINE EMM \n");
	result = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefinetData);
	if( result != IA_SUCCESS )
	{
		HxLOG_Print(" : SERVICE_DEFINE for EMM\n");
	}

	#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
	if( s_ucTypeForMonitor == IR_MONITOR_EMM )
	{
		CAS_IR_SVC_DEFINE_EMMMONITOR(s_usSCELLforMonitor, 1);
	}
	#endif

	return result;
}


HUINT32 CAS_IR_SVC_DEFINE_DVB(HUINT32 idx)
{
	msg_service_define_data_st	SvcDefineDataForLive;
	ia_result	result;
	HUINT8		*p = NULL, index;
	HUINT8		*pServiceDefineData = NULL;
	HUINT8		*pECMRscData = NULL;
	HUINT8		*pDESRscData = NULL;
	HUINT8		*pSCRscData = NULL;

	HUINT16		ServiceDefineLength = 0;
	HUINT16		ECMRscLength = 0;
	HUINT16		DESRscLength = 0;
	HUINT16		SCRscLength = 0;
	HUINT16		MacroVRscLength = 0;

	msg_resource_st		MsgDescRsc[DESC_RESOURCE];
	msg_resource_st		MsgSCRsc[CAS_MAX_NUM_OF_SC];

	HUINT16		MRDefineLength=0;
	HUINT16		MRRscLength =0;
	HMSG	hMsg;

#if defined(IRPLAT_FN_COPY_CONTROL)
	HUINT8				*pMacroVRscData = NULL;
	msg_resource_st		MsgMacroVisionRsc;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	HUINT8		*pMRRscData=NULL;
	msg_resource_st		MsgMRRsc;
	msg_mr_define_st 	st_mr_define;
#endif

	if (idx >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("SvcIndex >= IR_HUMAX_MAX_SERVICE\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("\n ### CAS_IR_SVC_DEFINE_DVB ### \n\n");
	HxLOG_Print("svcIndex(%d) hIrSvc(%x) SCELL(%x)\n", idx, g_IrSvcInfo[idx].hIrSvc, g_IrSvcInfo[idx].usSCELL_SvcHandle);

	HxSTD_memset(&SvcDefineDataForLive, 0, sizeof(msg_service_define_data_st));
	HxSTD_memset(&MsgDescRsc, 0, (sizeof(msg_resource_st) * DESC_RESOURCE));
	HxSTD_memset(&MsgSCRsc, 0, (sizeof(msg_resource_st) * CAS_MAX_NUM_OF_SC));

	SvcDefineDataForLive.wServiceHandle = g_IrSvcInfo[idx].usSCELL_SvcHandle;

	if(CAS_MAX_NUM_OF_SC > 1)
		SvcDefineDataForLive.bResourceCount = 4;	// 2 smartcard¿Œ ∞ÊøÏ Resource ¡ı∞° « ø‰.
	else
		SvcDefineDataForLive.bResourceCount = 3;	// resource : ECM/Descrambler/Smartcard

#if defined(IRPLAT_FN_COPY_CONTROL)
	SvcDefineDataForLive.bResourceCount++;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	SvcDefineDataForLive.bResourceCount++;
#endif


	// ECM RESOURCE
	pECMRscData = CAS_IR_SI_ParsePMT((HUINT8)idx, g_IrSvcInfo[idx].pucPMTRaw, g_IrSvcInfo[idx].hIrSvc, &ECMRscLength);
	if( pECMRscData == (HUINT8 *)NULL )
	{
		HxLOG_Print(" : ParsePMT\n");
		return HIR_NOT_OK;
	}

	// Descrambler RESOURCE
	for (index = 0; index < DESC_RESOURCE; index++)
	{
		MsgDescRsc[index].wResourceType = RESOURCE_DVB_DESCRAMBLER;
		MsgDescRsc[index].wResourceId = g_IrSvcInfo[idx].ucDemuxId;
		if( g_IrSvcInfo[idx].ucDemuxId == HMX_VD_DMX_MAX_NUM) //HUMAX_VD_DEMUX_COUNT
		{
			HxLOG_Print(": no empty g_IrSvcInfo[%d].DescInfoIndex\n", idx);
		}
		MsgDescRsc[index].wResourceMgnt = MANAGE_UPDATE;
	}
	pDESRscData = (HUINT8 *)&MsgDescRsc;
	DESRscLength = (sizeof(msg_resource_st) * DESC_RESOURCE);

	// Smartcard RESOURCE
	for (index = 0; index < CAS_MAX_NUM_OF_SC; index++)
	{
		MsgSCRsc[index].wResourceType = RESOURCE_SMARTCARD;
		MsgSCRsc[index].wResourceId = index;
		MsgSCRsc[index].wResourceMgnt = MANAGE_UPDATE;
	}
	pSCRscData = (HUINT8 *)MsgSCRsc;
	SCRscLength = (sizeof(msg_resource_st) * CAS_MAX_NUM_OF_SC);

#if defined(IRPLAT_FN_COPY_CONTROL) // defs. name is changed from "defined(SUPPORT_MACROVISION)".
	/* macro vision(copy contorl) resource */
	MsgMacroVisionRsc.wResourceType = RESOURCE_COPY_CONTROL;
	MsgMacroVisionRsc.wResourceId = idx;
	MsgMacroVisionRsc.wResourceMgnt = MANAGE_UPDATE;
	pMacroVRscData = (HUINT8 *)&MsgMacroVisionRsc;
	MacroVRscLength = sizeof(msg_resource_st);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	/* Maturity rating resource */
	MsgMRRsc.wResourceType = RESOURCE_MATURITY_RATING;
	MsgMRRsc.wResourceId = idx;
	MsgMRRsc.wResourceMgnt =  MANAGE_UPDATE;

	pMRRscData = (HUINT8 *)&MsgMRRsc;
	MRRscLength = sizeof(msg_resource_st);

	/* when the resource is initially defined, the value shall be 0xFFFF */
	st_mr_define.abMRPin[0]= 0xFF;
	st_mr_define.abMRPin[1]= 0xFF;

	MRDefineLength = sizeof(msg_mr_define_st);
#endif


	ServiceDefineLength = sizeof(msg_service_define_data_st) + ECMRscLength + DESRscLength + SCRscLength + MacroVRscLength \
	+MRRscLength	+MRDefineLength;

	pServiceDefineData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
	if (pServiceDefineData == NULL)
	{
		HxLOG_Print("[CAS_IR_SVC_DEFINE_DVB] Error : Mem Alloc Error 3\n");
		if( pECMRscData != NULL )
			OxCAS_Free(pECMRscData);
		return HIR_NOT_OK;
	}
	p = pServiceDefineData;

	HxSTD_memset(p, 0, sizeof(ServiceDefineLength));

	HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForLive, sizeof(msg_service_define_data_st));
	p += sizeof(msg_service_define_data_st);

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
{
	// Maturiry rating RESOURCE
	HxSTD_memcpy(p, pMRRscData, MRRscLength);
	p += MRRscLength;

	HxSTD_memcpy(p, (HUINT8*)&st_mr_define, MRDefineLength );
	p += MRDefineLength;
}
#endif


#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
	HxSTD_memcpy(p, pSCRscData, SCRscLength);
	p += SCRscLength;
	HxSTD_memcpy(p, pDESRscData, DESRscLength);
	p += DESRscLength;
	HxSTD_memcpy(p, pECMRscData, ECMRscLength);
	p += ECMRscLength;
#else
	HxSTD_memcpy(p, pECMRscData, ECMRscLength);
	p += ECMRscLength;
	HxSTD_memcpy(p, pDESRscData, DESRscLength);
	p += DESRscLength;
	HxSTD_memcpy(p, pSCRscData, SCRscLength);
	p += SCRscLength;
#endif
#if defined(IRPLAT_FN_COPY_CONTROL)
	HxSTD_memcpy(p, pMacroVRscData, MacroVRscLength);
	p += MacroVRscLength;
#endif
	if( pECMRscData != NULL )
		OxCAS_Free(pECMRscData);

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE ==> SVC_DEFINE_DVB\n");
#if 0
{
	int xx;
	printf("jichoi: ServiceDefineLength = %d\n", ServiceDefineLength);
	for(xx=0; xx<ServiceDefineLength; xx++)
	{
		printf("%02X ", pServiceDefineData[xx] );
		if(xx % 8 ==7 )
			printf("\n");
	}
	printf("\n");
}
#endif
	result = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefineData);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("[START SERVICE : Error From SCELL_Message for record]\n");
	}

	//anyways report app.
	hMsg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_DEFINED;
	CAS_IR_EVT_SendEvent(g_IrSvcInfo[idx].hAction, &hMsg);

	return HIR_OK;
}


HIR_ERR CAS_IR_SVC_DEFINE_ECMMONITOR(HUINT16 ushSCELL, HUINT8 bStart)
{
	msg_service_define_data_st	SvcDefineDataForEcmMonitor;
	ia_result					result;
	HUINT8						*p = NULL;
	HUINT8						*pServiceDefinetData = NULL;
	HUINT8						*pMonitorRscData = NULL;
	HUINT8						*pMonitoring = NULL;
	HUINT16						ServiceDefineLength = 0;
	HUINT16						MonitorRscLength = 0;
	HUINT16						MonitoringLength = 0;
	msg_resource_st				MsgMonitorRsc;
	msg_monitoring_st				MsgMonitoring;
	ca_drv_message_type_e 		*peMonitorMsg = NULL;

	HxLOG_Print("\n ### CAS_IR_SVC_DEFINE_ECMMONITOR ### \n\n");
	HxLOG_Info("ushSCELL= 0x%X, Type = 0x%X\n", ushSCELL, bStart);

	HxSTD_memset(&SvcDefineDataForEcmMonitor, 0, sizeof(msg_service_define_data_st));
	HxSTD_memset(&MsgMonitorRsc, 0, sizeof(msg_resource_st));
	HxSTD_memset(&MsgMonitoring, 0, sizeof(msg_monitoring_st));

	SvcDefineDataForEcmMonitor.wServiceHandle = ushSCELL;
	SvcDefineDataForEcmMonitor.bResourceCount = RESOURCE_ECM_MONITOR;	// resource : Monitoring

	// Monitoring RESOURCE
	MsgMonitorRsc.wResourceType = RESOURCE_MONITORING;
	MsgMonitorRsc.wResourceId = 0;

	if( bStart == 1 )
	{
		MsgMonitorRsc.wResourceMgnt = MANAGE_UPDATE;
		pMonitorRscData = (HUINT8 *)&MsgMonitorRsc;
		MonitorRscLength = sizeof(msg_resource_st);

		MsgMonitoring.wNumberOfMonitors = 1;
		pMonitoring = (HUINT8 *)&MsgMonitoring;
		MonitoringLength = sizeof(msg_monitoring_st);

		peMonitorMsg = (ca_drv_message_type_e *)OxCAS_Malloc(sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors);
		if (peMonitorMsg == NULL)
		{
			HxLOG_Print(": Mem Alloc Error 2\n");
			return HIR_NOT_OK;
		}
		peMonitorMsg[0] = MSG_DVB_ECM_MONITOR;

		ServiceDefineLength = sizeof(msg_service_define_data_st) + MonitorRscLength + MonitoringLength + sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors;

		pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
		if (pServiceDefinetData == NULL)
		{
			if( peMonitorMsg != NULL )
				OxCAS_Free(peMonitorMsg);
			HxLOG_Print(" : Mem Alloc Error 3\n");
			return HIR_NOT_OK;
		}
		p = pServiceDefinetData;

		HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForEcmMonitor, sizeof(msg_service_define_data_st));
		p += sizeof(msg_service_define_data_st);

		HxSTD_memcpy(p, pMonitorRscData, MonitorRscLength);
		p += MonitorRscLength;

		HxSTD_memcpy(p, pMonitoring, MonitoringLength);
		p += MonitoringLength;

		HxSTD_memcpy(p, (HUINT8 *)peMonitorMsg, sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors);
		p += sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors;

		if( peMonitorMsg != NULL )
			OxCAS_Free(peMonitorMsg);

	}
	else
	{
		MsgMonitorRsc.wResourceMgnt = MANAGE_CLOSE;
		pMonitorRscData = (HUINT8 *)&MsgMonitorRsc;
		MonitorRscLength = sizeof(msg_resource_st);

		ServiceDefineLength = sizeof(msg_service_define_data_st) + MonitorRscLength;

		pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
		if (pServiceDefinetData == NULL)
		{
			HxLOG_Print(": Mem Alloc Error 4\n");
			return HIR_NOT_OK;
		}
		p = pServiceDefinetData;

		HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForEcmMonitor, sizeof(msg_service_define_data_st));
		p += sizeof(msg_service_define_data_st);

		HxSTD_memcpy(p, pMonitorRscData, MonitorRscLength);
		p += MonitorRscLength;
	}

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE: ECM MONITOR\n");
	result = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefinetData);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE: ECM MONITOR, FAIL\n");
	}

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_DEFINE: ECM MONITOR, OK\n");

	return HIR_OK;
}

/********************************************************************
*	function name :	 CAS_IR_SVC_DEFINE_EMMMONITOR
*	input : TS Index of Irdeto CAS MW
*	output : void
*	description : EMM Monitoring¿ª ON.
********************************************************************/
HUINT32 CAS_IR_SVC_DEFINE_EMMMONITOR(HUINT16 ushSCELL, HUINT8 bStart)
{
	msg_service_define_data_st	SvcDefineDataForEmmMonitor;
	ia_result	result;
	HUINT8		*p = NULL;
	HUINT8		*pServiceDefinetData = NULL;
	HUINT8		*pMonitorRscData = NULL;
	HUINT8		*pMonitoring = NULL;
	HUINT16		ServiceDefineLength = 0;
	HUINT16		MonitorRscLength = 0;
	HUINT16		MonitoringLength = 0;
	msg_resource_st		MsgMonitorRsc;
	msg_monitoring_st		MsgMonitoring;
	ca_drv_message_type_e 	*peMonitorMsg = NULL;

	HxLOG_Print("\n ### CAS_IR_SVC_DEFINE_EMMMONITOR ### \n\n");

	HxSTD_memset(&SvcDefineDataForEmmMonitor, 0, sizeof(msg_service_define_data_st));
	HxSTD_memset(&MsgMonitorRsc, 0, sizeof(msg_resource_st));
	HxSTD_memset(&MsgMonitoring, 0, sizeof(msg_monitoring_st));

	SvcDefineDataForEmmMonitor.wServiceHandle = ushSCELL;
	SvcDefineDataForEmmMonitor.bResourceCount = 1;	// resource : Monitoring

	// Monitoring RESOURCE
	MsgMonitorRsc.wResourceType = RESOURCE_MONITORING;
	MsgMonitorRsc.wResourceId = 0;
	if( bStart == 1 )
	{
		MsgMonitorRsc.wResourceMgnt = MANAGE_ADD;
		pMonitorRscData = (HUINT8 *)&MsgMonitorRsc;
		MonitorRscLength = sizeof(msg_resource_st);

		MsgMonitoring.wNumberOfMonitors = 1;
		pMonitoring = (HUINT8 *)&MsgMonitoring;
		MonitoringLength = sizeof(msg_monitoring_st);

		peMonitorMsg = (ca_drv_message_type_e *)OxCAS_Malloc(sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors);
		if (peMonitorMsg == NULL)
		{
			HxLOG_Print(": Mem Alloc Error 1\n");
			return HIR_NOT_OK;
		}
		peMonitorMsg[0] = MSG_DVB_EMM_MONITOR;

		ServiceDefineLength = sizeof(msg_service_define_data_st) + MonitorRscLength + MonitoringLength + sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors;

		pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
		if (pServiceDefinetData == NULL)
		{
			HxLOG_Print(" : Mem Alloc Error 2\n");
			if (peMonitorMsg != NULL)
			{
				OxCAS_Free(peMonitorMsg);
			}
			return HIR_NOT_OK;
		}
		p = pServiceDefinetData;

		HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForEmmMonitor, sizeof(msg_service_define_data_st));
		p += sizeof(msg_service_define_data_st);

		HxSTD_memcpy(p, pMonitorRscData, MonitorRscLength);
		p += MonitorRscLength;

		HxSTD_memcpy(p, pMonitoring, MonitoringLength);
		p += MonitoringLength;

		HxSTD_memcpy(p, (HUINT8 *)peMonitorMsg, sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors);
		p += sizeof(ca_drv_message_type_e) * MsgMonitoring.wNumberOfMonitors;

		if( peMonitorMsg != NULL )
			OxCAS_Free(peMonitorMsg);
	}
	else
	{
		MsgMonitorRsc.wResourceMgnt = MANAGE_CLOSE;
		pMonitorRscData = (HUINT8 *)&MsgMonitorRsc;
		MonitorRscLength = sizeof(msg_resource_st);

		ServiceDefineLength = sizeof(msg_service_define_data_st) + MonitorRscLength;

		pServiceDefinetData = (HUINT8 *)OxCAS_Malloc(ServiceDefineLength);
		if (pServiceDefinetData == NULL)
		{
			HxLOG_Print(" : Mem Alloc Error 3\n");
			return HIR_NOT_OK;
		}
		p = pServiceDefinetData;

		HxSTD_memcpy(p, (HUINT8 *)&SvcDefineDataForEmmMonitor, sizeof(msg_service_define_data_st));
		p += sizeof(msg_service_define_data_st);

		HxSTD_memcpy(p, pMonitorRscData, MonitorRscLength);
		p += MonitorRscLength;
	}

	HxLOG_Print("%s\n",(MsgMonitorRsc.wResourceMgnt == MANAGE_CLOSE)? "CLOSE" : "OPEN"  );
	result = SCELL_Message(MSG_SERVICE_DEFINE, 0, ServiceDefineLength, (void *)pServiceDefinetData);
	if( result != IA_SUCCESS )
	{
		// TODO
		HxLOG_Print("ERROR From SCELL_Message ]\n");
	}

	return HIR_OK;
}

#if defined(IRPLAT_SYS_PVR)
// type: SERVICE_PVR_RECORD, SERVICE_PVR_PLAYBACK
HUINT32	CAS_IR_SVC_OPEN_REQUEST_PVR(Handle_t hIrSvc, HUINT16 ir_service_type)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	msg_service_open_request_data_st	*ptr = NULL;
	ia_result 							ret;

	HxLOG_Info(":type=%d svcIndex=%d\n", ir_service_type, 0);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);//local_cas_ir_Drv_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Error : Can't find hIrSvc[%x] in IrSvc List\n", hIrSvc);
		return HIR_NOT_OK;
	}
	HxLOG_Print("svcIndex(%d) hIrSvc(%x) irServiceType(%d)\n", CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), hIrSvc, ir_service_type);

	ptr = (msg_service_open_request_data_st*)OxCAS_Malloc(sizeof(msg_service_open_request_data_st));
	if (ptr == NULL)
	{
		HxLOG_Print(": Mem Alloc Error 3\n");
		return HIR_NOT_OK;
	}
	ptr->wServiceType = ir_service_type;

	//Get Service handle in the CAS_IR_SVC_OPEN_REPLY_PVR.
	switch (ir_service_type)
	{
#if	defined(IRPLAT_PVR_PHASE12)
		case	SERVICE_PVR_RECORD:
			pstIrSvcInfo->pstPVRSvcInfo->usSCELL_SvcType = SERVICE_PVR_RECORD;
			break;
		case	SERVICE_PVR_PLAYBACK:
			pstIrSvcInfo->pstPVRSvcInfo->usSCELL_SvcType = SERVICE_PVR_PLAYBACK;
			break;
#endif

#if	defined(IRPLAT_PVR_DRM)
		case	SERVICE_PVR_DRM:
			pstIrSvcInfo->pstPVRSvcInfo->usSCELL_SvcType = SERVICE_PVR_DRM;
			break;
#endif
		default:
			HxLOG_Print("Error : Invalid PVR Service Type!!! [%x]\n", ir_service_type);
#if defined(CONFIG_OP_DIGITURK)
			if (ptr != NULL)
			{
				CA_MEM_Free(ptr);
			}
			return HIR_NOT_OK;
#else
			break;
#endif
	}

	HxLOG_Print("[SCELL_Message] MSG_SERVICE_OPEN_REQUEST: SERVICE_PVR_RECORD - opSeqNo[%d]\n", CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo));
	//	ret = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, CAS_IR_DRV_MakePVRSvcIndex(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo)), (ia_word16)sizeof(msg_service_open_request_data_st), ptr);
	ret = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), (ia_word16)sizeof(msg_service_open_request_data_st), ptr);
	if(ret != IA_SUCCESS)
	{
		HxLOG_Print("type=%d, ret=0x%x\n", ir_service_type, ret);
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


HUINT32	CAS_IR_SVC_CLOSE_PVR(Handle_t hIrSvc, HUINT16 ir_service_type)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	ia_result					ret;
	msg_service_close_data_st	*ptr = NULL;

	HxLOG_Info("%s\n", (ir_service_type==SERVICE_PVR_RECORD)?"RECORD":"PLAYBACK");
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Error : Can't find hIrSvc[%x] in IrSvc List\n", hIrSvc);
		return HIR_NOT_OK;
	}
	if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
	{
		HxLOG_Print("Error : PVR Service is not opened!!!\n");
		return HIR_NOT_OK;
	}
	if (pstIrSvcInfo->pstPVRSvcInfo->ucIsSCELLReady == 0)
	{
		HxLOG_Print("Error : PVR Service is not opened!!!\n");
		return HIR_NOT_OK;
	}

	ptr = (msg_service_close_data_st *)OxCAS_Malloc(sizeof(msg_service_close_data_st));
	if (ptr == NULL)
	{
		HxLOG_Print(" Error : Mem Alloc Error 3\n");
		return HIR_NOT_OK;
	}


	switch (ir_service_type)
	{
		case	SERVICE_PVR_RECORD:
		case	SERVICE_PVR_PLAYBACK:
		case	SERVICE_PVR_DRM:
			ptr->wServiceHandle = pstIrSvcInfo->pstPVRSvcInfo->usSCELL_SvcHandle;
			HxLOG_Print("svcIndex(%d) hIrSvc(%x) ir_service_type [%x] SCELL_svcHandle [%x]\n", CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), hIrSvc, ir_service_type, ptr->wServiceHandle);
			ret = SCELL_Message(MSG_SERVICE_CLOSE, 0, (ia_word16)sizeof(msg_service_close_data_st), ptr);
			if( ret != IA_SUCCESS )
			{
				HxLOG_Print(" Error From SCELL_Message\n");
				return HIR_NOT_OK;
			}
			//	pvr service structure will be initialized in CAS_IR_SVC_CLOSE_PVR caller
			break;

		default:
			HxLOG_Print("Invalid Service Type : (%x)\n", ir_service_type);
			if (ptr != NULL)
			{
				OxCAS_Free(ptr);
			}
			return HIR_NOT_OK;
	}

	return HIR_OK;
}



HUINT32 CAS_IR_SVC_OPEN_REPLY_PVR(HUINT16 handle, HUINT16 ir_service_type, HUINT16 wOpSeqNo)
{
	HUINT16 irSvcInfoIndex;

	HxLOG_Print("handle[%x] svcType[%x] wOpSeqNo[%x]\n", handle, ir_service_type, wOpSeqNo);
	//sent in the service open request.
//	irSvcInfoIndex = CAS_IR_DRV_GetBaseIrSvcIndexFromPVRIndex(wOpSeqNo);

	if(wOpSeqNo >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Critical("\n\n");
		return HIR_NOT_OK;
	}

	irSvcInfoIndex = wOpSeqNo;
	HxLOG_Print("svcIndex(%d) hIrSvc(%x)\n", irSvcInfoIndex, g_IrSvcInfo[irSvcInfoIndex].hIrSvc);

	if(g_IrSvcInfo[irSvcInfoIndex].hIrSvc == HANDLE_NULL)
	{
		HxLOG_Critical("\n\n");
		return HIR_NOT_OK;
	}
	switch(ir_service_type)
	{
#if	defined(IRPLAT_PVR_PHASE12)
		case SERVICE_PVR_RECORD:
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->ucIsSCELLReady = 1;
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->usSCELL_SvcHandle = handle;
			CAS_IR_SVC_DEFINE_PVR(handle, SERVICE_PVR_RECORD, (void*)g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo);
			break;

		case SERVICE_PVR_PLAYBACK:
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->ucIsSCELLReady = 1;
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->usSCELL_SvcHandle = handle;
			CAS_IR_SVC_DEFINE_PVR(handle, SERVICE_PVR_PLAYBACK, (void*)g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo);
			break;
#endif

#if	defined(IRPLAT_PVR_DRM)
		case SERVICE_PVR_DRM:
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->ucIsSCELLReady = 1;
			g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo->usSCELL_SvcHandle = handle;
			CAS_IR_SVC_DEFINE_PVR(handle, SERVICE_PVR_DRM, (void*)g_IrSvcInfo[irSvcInfoIndex].pstPVRSvcInfo);
			break;
#endif
		default:
			break;
	}

	return HIR_OK;
}

HUINT32 CAS_IR_SVC_DEFINE_PVR(HUINT16 wIrServiceHandle, HUINT16 ir_service_type, void *pvrInfo)
{
	HMSG						msg;
	HUINT16						len;
	ia_result					ret;

	IR_SERVICE_INFO				*pstIrSvcInfo = NULL;

	//recId ∏‚πˆ∞° ¡∏¿Á«œ¡ˆ æ ¿Ω, warning¡¶∞≈ ∏Ò¿˚¿∏∑Œ ContentId∑Œ ºˆ¡§.
#if 0
	HxLOG_Print("svcHandle[%x] svcType[%x] recId[%x]\n", wIrServiceHandle, ir_service_type, pvrInfo);
	if(ir_service_type == SERVICE_PVR_RECORD)
	{
		HxLOG_Print("SERVICE DEFINE PVR_RECORD : wIrServiceHandle=%d, recid=%d\n", wIrServiceHandle, pvrInfo);
	}
	else if(ir_service_type == SERVICE_PVR_PLAYBACK)
	{
		HxLOG_Print("SERVICE DEFINE PVR_PLAYBACK : wIrServiceHandle=%d, recid=%d\n", wIrServiceHandle, pvrInfo);
	}
	else if (ir_service_type == SERVICE_PVR_DRM)
	{
		HxLOG_Print("SERVICE DEFINE PVR_DRM : wIrServiceHandle=%d, recid=%d\n", wIrServiceHandle, pvrInfo);
	}
#else
	HxLOG_Print("svcHandle[%x] svcType[%x] ulContentId[%x]\n", wIrServiceHandle, ir_service_type, ((IR_PVRSERVICE_INFO *)(pvrInfo))->ulContentId));
	if(ir_service_type == SERVICE_PVR_RECORD)
	{
		HxLOG_Print("SERVICE DEFINE PVR_RECORD : wIrServiceHandle=%d, ulContentId=%d\n", wIrServiceHandle, ((IR_PVRSERVICE_INFO *)(pvrInfo))->ulContentId));
	}
	else if(ir_service_type == SERVICE_PVR_PLAYBACK)
	{
		HxLOG_Print("SERVICE DEFINE PVR_PLAYBACK : wIrServiceHandle=%d, ulContentId=%d\n", wIrServiceHandle, ((IR_PVRSERVICE_INFO *)(pvrInfo))->ulContentId));
	}
	else if (ir_service_type == SERVICE_PVR_DRM)
	{
		HxLOG_Print("SERVICE DEFINE PVR_DRM : wIrServiceHandle=%d, ulContentId=%d\n", wIrServiceHandle, ((IR_PVRSERVICE_INFO *)(pvrInfo))->ulContentId));
	}

#endif
//	recid = ??-----------------------MINI/-0---

#if	0
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByContentsId(recid);
#else
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByPvrInfo(pvrInfo);
#endif
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Error : Can't find same contentsid (%x) in ca service list!\n", pvrInfo);
		return 0;
	}

#if	defined(IRPLAT_PVR_PHASE12)
	{
		msg_pvr_service_define_st	*ptr;

		len = sizeof(msg_pvr_service_define_st);
		ptr = (msg_pvr_service_define_st*)OxCAS_Malloc(len);
		if(ptr == NULL)
		{
			HxLOG_Print("Error memalloc\n");
			return HIR_NOT_OK;
		}

		msg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_DEFINED;
		CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

		ptr->data.wServiceHandle = wIrServiceHandle;
		ptr->data.bResourceCount = 3;

		ptr->session_management.wResourceType =		RESOURCE_SESSION_MANAGEMENT;
		ptr->session_management.wResourceId = 		pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
		ptr->session_management.wResourceMgnt = 	MANAGE_ADD;

		ptr->crypto_operation.wResourceType =		RESOURCE_CRYPTO_OPERATION;
		ptr->crypto_operation.wResourceId =			pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
		ptr->crypto_operation.wResourceMgnt =		MANAGE_ADD;
		ptr->payload_crypto_operation.eAlgorithm =	VD_CRYPTO_ALGORITHM_3DES;

		ptr->smartcard.wResourceType =				RESOURCE_SMARTCARD;
		ptr->smartcard.wResourceId = 				0;	// TODO.. smartcard∞° 2∞≥∞° µ«∏È ¡ª ¥ı ∞Ì∑¡«ÿæﬂ«“ ∞Õ....
		ptr->smartcard.wResourceMgnt =				MANAGE_ADD;

		HxLOG_Print("MSG_SERVICE_DEFINE\n");
		ret = SCELL_Message(MSG_SERVICE_DEFINE, 0, len, ptr);
		if( ret != IA_SUCCESS )
		{
			HxLOG_Print("MSG_SERVICE_DEFINE: Error, ret = 0x%X\n",(HUINT32) ret);
			return HIR_NOT_OK;
		}
	}
	return HIR_OK;
#endif

#if	defined(IRPLAT_PVR_DRM)
	switch (pstIrSvcInfo->eCasActionType)
	{
		case	eCAS_ActionType_Playback:
		case	eCAS_ActionType_Tsr_Play:
			{
				msg_pvr_pbservice_define_st		*ptr;

				len = sizeof(msg_pvr_pbservice_define_st);
				ptr = (msg_pvr_pbservice_define_st*)OxCAS_Malloc(len);
				if(ptr == NULL)
				{
					HxLOG_Print("Error memalloc\n");
					return HIR_NOT_OK;
				}

				msg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_DEFINED;
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

				ptr->data.wServiceHandle = wIrServiceHandle;
				ptr->data.bResourceCount = 3;//4;
#if	defined(IRPLAT_FN_COPY_CONTROL)
				ptr->data.bResourceCount++;
#endif

				ptr->session_management.wResourceType =		RESOURCE_SESSION_MANAGEMENT_DRM;
				ptr->session_management.wResourceId = 		pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
				ptr->session_management.wResourceMgnt = 	MANAGE_ADD;

				ptr->drmdefine.eSessionSubtype =			MSG_SM_DRM_SESSION_SUBTYPE_PVR_PHASE_3;
				if (pstIrSvcInfo->eCasActionType == eCAS_ActionType_Playback)
					ptr->drmdefine.ePvrPurpose =			MSG_SM_DRM_PVR_PURPOSE_PERMANENT;
				else
					ptr->drmdefine.ePvrPurpose =			MSG_SM_DRM_PVR_PURPOSE_TIMESHIFT;
				ptr->drmdefine.ePvrAction =					MSG_SM_DRM_PVR_ACTION_PLAYBACK;

				ptr->drmplayback.wRightsRecordId =			pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId;
				ptr->drmplayback.wSequenceRecordId =		pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId;
				ptr->drmplayback.fPeekRightsOnly =			IA_FALSE;
				ptr->drmplayback.ePlaybackDirection =		MSG_SM_PLAYBACK_DIRECTION_FORWARD;

				/*
				ptr->drmplayback.wRightsRecordId = stCaDrmPvrPlayback.wRightsRecordId;
				ptr->drmplayback.wSequenceRecordId = stCaDrmPvrPlayback.wSequenceRecordId;
				ptr->drmplayback.fPeekRightsOnly = stCaDrmPvrPlayback.fPeekRightsOnly;
				ptr->drmplayback.ePlaybackDirection = (msg_sm_drm_playback_direction_e)stCaDrmPvrPlayback.ePlaybackDirection;
				*/

				ptr->crypto_operation.wResourceType =		RESOURCE_CRYPTO_OPERATION;
				ptr->crypto_operation.wResourceId =			pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
				ptr->crypto_operation.wResourceMgnt =		MANAGE_ADD;
				ptr->payload_crypto_operation.eAlgorithm =	VD_CRYPTO_ALGORITHM_AES;

				ptr->smartcard.wResourceType =				RESOURCE_SMARTCARD;
				ptr->smartcard.wResourceId = 				0;	// TODO.. smartcard∞° 2∞≥∞° µ«∏È ¡ª ¥ı ∞Ì∑¡«ÿæﬂ«“ ∞Õ....
				ptr->smartcard.wResourceMgnt =				MANAGE_ADD;

#if	defined(IRPLAT_FN_COPY_CONTROL)
				ptr->copycontrol.wResourceType =			RESOURCE_COPY_CONTROL;
				ptr->copycontrol.wResourceId =				0;
				ptr->copycontrol.wResourceMgnt =			MANAGE_ADD;
#endif
				HxLOG_Print("MSG_SERVICE_DEFINE\n");
				ret = SCELL_Message(MSG_SERVICE_DEFINE, 0, len, ptr);
				if( ret != IA_SUCCESS )
				{
					HxLOG_Print("MSG_SERVICE_DEFINE: Error, ret = 0x%X\n",(HUINT32) ret);
					return HIR_NOT_OK;
				}
			}
			break;
		case	eCAS_ActionType_Recording:
		case	eCAS_ActionType_Tsr_Rec:
			{
				msg_xsvc_rec_service_define_st	*ptr;

				len =  sizeof(msg_xsvc_rec_service_define_st);
				ptr = (msg_xsvc_rec_service_define_st *)OxCAS_Malloc(len);
				if (ptr == NULL)
				{
					HxLOG_Print("Error memalloc\n");
					return HIR_NOT_OK;
				}

				msg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_DEFINED;
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

				ptr->data.wServiceHandle = wIrServiceHandle;
				ptr->data.bResourceCount = 3;

				ptr->session_management.wResourceType =		RESOURCE_SESSION_MANAGEMENT_DRM;
				ptr->session_management.wResourceId = 		pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
				ptr->session_management.wResourceMgnt = 	MANAGE_ADD;

				ptr->crypto_operation.wResourceType =		RESOURCE_CRYPTO_OPERATION;
				ptr->crypto_operation.wResourceId =			pstIrSvcInfo->usSCELL_SvcHandle;//recid;	//use recid as Resource ID - Integration guide 4.3.2.1
				ptr->crypto_operation.wResourceMgnt =		MANAGE_ADD;
				ptr->payload_crypto_operation.eAlgorithm =	VD_CRYPTO_ALGORITHM_AES;

				ptr->smartcard.wResourceType =				RESOURCE_SMARTCARD;
				ptr->smartcard.wResourceId = 				0;	// TODO.. smartcard∞° 2∞≥∞° µ«∏È ¡ª ¥ı ∞Ì∑¡«ÿæﬂ«“ ∞Õ....
				ptr->smartcard.wResourceMgnt =				MANAGE_ADD;

				ptr->drmdefine.eSessionSubtype =			MSG_SM_DRM_SESSION_SUBTYPE_PVR_PHASE_3;
				if (pstIrSvcInfo->eCasActionType == eCAS_ActionType_Tsr_Rec)
					ptr->drmdefine.ePvrPurpose =			MSG_SM_DRM_PVR_PURPOSE_TIMESHIFT;
				else
					ptr->drmdefine.ePvrPurpose =			MSG_SM_DRM_PVR_PURPOSE_PERMANENT;
				ptr->drmdefine.ePvrAction =					MSG_SM_DRM_PVR_ACTION_RECORD;

				ptr->drmrecord.wDescrambleServiceHandle =	pstIrSvcInfo->usSCELL_SvcHandle;
				ptr->drmrecord.fFtaCheckAnySector =			IA_FALSE;
				ptr->drmrecord.bFtaCheckSectorNr =			0;

				//	Delayed Recording¿Ã ¡ˆø¯µ«∏È ª˝∞¢«ÿ∫º πÆ¡¶...
				ptr->drmrecord.fChainRecordings =			IA_FALSE;
				/*
				ptr->drmrecord.fChainRecordings =			IA_TRUE;
				ptr->drmrecord.wChainingSessionResourceId =	previous session id
				ptr->drmrecord.wChainingSequenceRecordId =	previous seq id
				ptr->drmrecord.wChainingRightRecordId =		previous
				*/

				HxLOG_Print("MSG_SERVICE_DEFINE\n");
				ret = SCELL_Message(MSG_SERVICE_DEFINE, 0, len, ptr);
				if( ret != IA_SUCCESS )
				{
					HxLOG_Print("MSG_SERVICE_DEFINE: Error, ret = 0x%X\n",(HUINT32) ret);
					return HIR_NOT_OK;
				}
			}
			break;
		default:
			break;
	}

	return HIR_OK;
#endif

}
#endif


HUINT32 CAS_IR_SVC_STATUS(HUINT8* data, HUINT16 len)
{
	msg_service_status_data_st		SvcStatusData;
	msg_rsc_status_st				RscStatus;
	HINT32							ResourceCount;
	msg_ecm_source_status_st		EcmSrcStatus;
	msg_emm_source_status_st		EmmSrcStatus;
	msg_session_source_substatus_st	SessionSrcSubStatus;

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	msg_gmss_stream_status_st		GmssStreamStatus;
#else
	msg_stream_status_st			StreamStatus;
#endif

#if	defined(IRPLAT_PVR_DRM)
	msg_sm_drm_status_st			drmStatus;
	msg_sm_drm_record_flags_st		drmRecordFlag;
	msg_sm_drm_rights_record_st		drmRightsRecord;

#if	defined(IRPLAT_FN_COPY_CONTROL)
	msg_copycontrol_substatus_st	copyControlStatus;
	vd_macrovision_mode_st			ccMacroVision;
	vd_copycontrol_digital_cci_st	ccDigitalcci;
	vd_copycontrol_analog_cci_st	ccAnalogcci;
#endif

#endif

	HINT8							StreamCount;
	HUINT32							eSvcType;

	HINT32							i;
	HUINT8 							*p;

#if defined(IRPLAT_FN_COPY_CONTROL)
	msg_copycontrol_substatus_st	CopyControlSubStatus;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	HUINT16		wSCFlag=0;
	HUINT16		wMRFlag=0;
	HMSG		MsgMR;
#endif

#if	defined(IRPLAT_PVR_DRM)
#if	defined(IRPLAT_FN_COPY_CONTROL)
	HxSTD_memset(&copyControlStatus, 0x00, sizeof(msg_copycontrol_substatus_st));
	HxSTD_memset(&ccMacroVision, 0x00, sizeof(vd_macrovision_mode_st));
	HxSTD_memset(&ccDigitalcci, 0x00, sizeof(vd_copycontrol_digital_cci_st));
	HxSTD_memset(&ccAnalogcci, 0x00, sizeof(vd_copycontrol_analog_cci_st));
#endif
#endif

	if (data == NULL)
	{
		HxLOG_Error("data is null.\n");
		return HIR_NOT_OK;
	}

	p = data;
	HxSTD_memcpy((HUINT8 *)&SvcStatusData, (HUINT8 *)p, sizeof(msg_service_status_data_st));
	p += sizeof(msg_service_status_data_st);

	HxLOG_Print("\n ### CAS_IR_SVC_STATUS ### \n\n");
	HxLOG_Info("wSvcHandle(0x%X), bRscCount(0x%X)]\n", SvcStatusData.wServiceHandle, SvcStatusData.bResourceCount);

	eSvcType = CAS_IR_DRV_GetIrSvcType(SvcStatusData.wServiceHandle);

	if( eSvcType == SERVICE_INVALID )
	{
		HxLOG_Print(" - INVALID Service Type for service status reply : wServiceHandle(0x%X)\n", SvcStatusData.wServiceHandle);
		return HIR_NOT_OK;
	}

#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
	//ycgo status∏¶ ECM±Ó¡ˆ ∫∏±‚ ¿ß«ÿº≠ global¿ª µ⁄ø°º≠ update«œµµ∑œ «ﬂ¥Ÿ.
#else
	CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_GLOBAL_SOURCE, 0, 0, SvcStatusData.stStatus, 0);
#endif

	CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_INTERNAL_SOURCE, 0, 0, SvcStatusData.stServiceStatus, 0);

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	/* check whether global status equals MP pin request status */
	if ( Is_E136orE137_4 (SvcStatusData.stStatus) )
	{
		wSCFlag = SvcStatusData.stStatus.wStatus;
	}
#endif
	ResourceCount = (HINT32)(SvcStatusData.bResourceCount);
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1) && !defined (CONFIG_MW_CAS_IRDETO_SOFTCELL_3_11)
	ResourceCount++;
#endif
	HxLOG_Info(" ResourceCount \t\t (0x%x)\n", ResourceCount);

	for( ; ResourceCount > 0; ResourceCount-- )
	{
		HxSTD_memcpy((HUINT8 *)&RscStatus, (HUINT8 *)p, sizeof(msg_rsc_status_st));
		p += sizeof(msg_rsc_status_st);

#ifdef DBG_IRCADRV_SVC_RSC_STATUS_TRACE
		HxLOG_Debug("[DBG_IRCADRV_SVC_RSC_STATUS_TRACE]\n");
		CAS_IR_Dbg_ConvertResourceToString(RscStatus.wType);
		HxLOG_Debug("\n");
#endif
		switch(RscStatus.wType)
		{
		case RESOURCE_ECM_SOURCE:				// 6.2
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_ECM_SOURCE, RscStatus.wId, 0, RscStatus.stStatus, 0);
			HxSTD_memcpy((HUINT8 *)&EcmSrcStatus, (HUINT8 *)p, sizeof(msg_ecm_source_status_st));
			p += sizeof(msg_ecm_source_status_st);
			StreamCount = (HINT8)(EcmSrcStatus.bStreamCount);

			for( i = 0; StreamCount > 0; StreamCount--, i++ )
			{
			  #if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
				HxSTD_memcpy((HUINT8 *)&GmssStreamStatus, (HUINT8 *)p, sizeof(msg_gmss_stream_status_st));
				p += sizeof(msg_gmss_stream_status_st);
				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_ELEM_STREAM_SOURCE, GmssStreamStatus.wEsPid, GmssStreamStatus.wEcmPid, GmssStreamStatus.stStatus, GmssStreamStatus.wResourceId);

				HxLOG_Info(" wEsPid \t\t (0x%x)\n", GmssStreamStatus.wEsPid);
				HxLOG_Info(" wEcmPid \t\t (0x%x)\n", GmssStreamStatus.wEcmPid);
				HxLOG_Info(" stStatus.eSource \t\t (0x%x)\n", GmssStreamStatus.stStatus.eSource);
				HxLOG_Info(" stStatus.wStatus \t\t (0x%x)\n", GmssStreamStatus.stStatus.wStatus);
				HxLOG_Info(" stStatus.eSeverity \t\t (0x%x)\n", GmssStreamStatus.stStatus.eSeverity);
				HxLOG_Info(" wResourceId \t (0x%x)\n", GmssStreamStatus.wResourceId);
				HxLOG_Info(" wMatchedSCount \t (0x%x)\n", GmssStreamStatus.wMatchedSCount);
			  #else
				HxSTD_memcpy((HUINT8 *)&StreamStatus, (HUINT8 *)p, sizeof(msg_stream_status_st));
				p += sizeof(msg_stream_status_st);
				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_ELEM_STREAM_SOURCE, StreamStatus.wEsPid, StreamStatus.wEcmPid, StreamStatus.stStatus, 0);

				HxLOG_Info(" wEsPid \t\t (0x%x)\n", StreamStatus.wEsPid);
				HxLOG_Info(" wEcmPid \t\t (0x%x)\n", StreamStatus.wEcmPid);
				HxLOG_Info(" stStatus.eSource \t\t (0x%x)\n", StreamStatus.stStatus.eSource);
				HxLOG_Info(" stStatus.wStatus \t\t (0x%x)\n", StreamStatus.stStatus.wStatus);
				HxLOG_Info(" stStatus \t\t (0x%x)\n", StreamStatus.stStatus);
			  #endif

			}
			break;

		case RESOURCE_EMM_SOURCE:				// 6.3
			HxSTD_memcpy((HUINT8 *)&EmmSrcStatus, (HUINT8 *)p, sizeof(msg_emm_source_status_st));
			p += sizeof(msg_emm_source_status_st);
			HxLOG_Print(" wEmmPid = 0x%X\n", EmmSrcStatus.wEmmPid);
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_EMM_SOURCE, RscStatus.wId, EmmSrcStatus.wEmmPid, RscStatus.stStatus, 0);
			break;

		case RESOURCE_DVB_DESCRAMBLER:			// 6.4
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_DVB_DESCRAMBLER, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_SMARTCARD:				// 6.5
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_SMARTCARD, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_IPTV_STB_SOCKET:			// 6.6
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_IPTV_STB_SOCKET, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_IPTV_HEADEND_SOCKET:		// 6.7
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_IPTV_HEADEND_SOCKET, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_IPTV_MROUTER_ADDRESS: 	// 6.8
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_IPTV_MROUTER_ADDRESS, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_MONITORING:				// 6.9
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_MONITORING, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_CC_SOURCE:				// 6.10
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_CC_SOURCE, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_CC_DECRYPTOR: 			// 6.11
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_CC_DECRYPTOR, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_REGISTRATION_HANDLE:		// 6.12
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_REGISTRATION_HANDLE, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_CRYPTO_OPERATION: 		// 6.14
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_CRYPTO_OPERATION, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
#if defined(IRPLAT_FN_COPY_CONTROL)
		case RESOURCE_MACROVISION:				// 6.15
		{
				vd_macrovision_mode_st stMacrovisionMode;
				vd_copycontrol_digital_cci_st stDigitalCCi;
				vd_copycontrol_analog_cci_st stAnalogCCi;

				HxSTD_memset(&stMacrovisionMode, 0, sizeof(vd_macrovision_mode_st));
				HxSTD_memset(&stDigitalCCi, 0, sizeof(vd_copycontrol_digital_cci_st));
				HxSTD_memset(&stAnalogCCi, 0, sizeof(vd_copycontrol_analog_cci_st));

			HxSTD_memcpy((HUINT8 *)&CopyControlSubStatus, (HUINT8 *)p, sizeof(msg_copycontrol_substatus_st) );
			p += sizeof(msg_copycontrol_substatus_st);

			// TODO: See followings comments.
			/* If you need to use macrovision, you MUST integrate followings contional codes */
			/* Followings are just basic calculation for referencing pointer 					*/
			HxLOG_Info("\t===>[COPYCONTROL][%d] status : 0x%02x\n", eSvcType, CopyControlSubStatus.wTypeMask));

			if( CopyControlSubStatus.wTypeMask & MSG_COPYCONTROL_MACROVISION )
			{
				HxSTD_memcpy((HUINT8 *)&stMacrovisionMode, (HUINT8 *)p, sizeof(vd_macrovision_mode_st));
				HxLOG_Info("\t===>[MSG_COPYCONTROL_MACROVISION] mode : 0x%02x\n", stMacrovisionMode.bMode);
				p += sizeof(vd_macrovision_mode_st);
			}
			if( CopyControlSubStatus.wTypeMask & MSG_COPYCONTROL_DIGITAL )
			{
				HxSTD_memcpy((HUINT8 *)&stDigitalCCi, (HUINT8 *)p, sizeof(vd_copycontrol_digital_cci_st));
				HxLOG_Info("\t===>[MSG_COPYCONTROL_DIGITAL] mode : %d\n", stDigitalCCi.eCopyControlInfo));
				p += sizeof(vd_copycontrol_digital_cci_st);
			}
			if( CopyControlSubStatus.wTypeMask & MSG_COPYCONTROL_ANALOG )
			{
				HxSTD_memcpy((HUINT8 *)&stAnalogCCi, (HUINT8 *)p, sizeof(vd_copycontrol_analog_cci_st));
				HxLOG_Info("\t===>[MSG_COPYCONTROL_ANALOG] mode : %d\n", stAnalogCCi.eCopyControlInfo));
				p += sizeof(vd_copycontrol_analog_cci_st);
			}

#if !defined(CONFIG_OP_DIGITURK)
			if(eSvcType == SERVICE_PVR_DRM)
#endif
			{
				HUINT32 ulCcValue = 0;
				HUINT8 ucType = (HUINT8)CopyControlSubStatus.wTypeMask;

				ulCcValue = ((HUINT32)ucType << 24) | ((HUINT32)stMacrovisionMode.bMode << 16) | ((HUINT32)stDigitalCCi.eCopyControlInfo << 8) | (HUINT32)stAnalogCCi.eCopyControlInfo;

				HxLOG_Info("\t===>[SERVICE_PVR_DRM][RESOURCE_MACROVISION] CC Value[0x%08x]\n", ulCcValue));

				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_MACROVISION, RscStatus.wId, ulCcValue, RscStatus.stStatus, 0);
			}
#if !defined(CONFIG_OP_DIGITURK)
			else
			{
				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_MACROVISION, RscStatus.wId, 0, RscStatus.stStatus, 0);
			}
#endif
		}
			break;

#endif
		case RESOURCE_STB_MAC_ADDRESS:			// 6.16
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_STB_MAC_ADDRESS, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_DVB_OOB:					// 6.17
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_DVB_OOB, RscStatus.wId, 0, RscStatus.stStatus, 0);
			break;
		case RESOURCE_SESSION_MANAGEMENT:		// 6.13
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_SESSION_MANAGEMENT, RscStatus.wId, 0, RscStatus.stStatus, 0);
			HxSTD_memcpy((HUINT8 *)&SessionSrcSubStatus, (HUINT8 *)p, sizeof(msg_session_source_substatus_st));
			p += sizeof(msg_session_source_substatus_st);
//subStatus
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_SESSION_SUB, RscStatus.wId, 0, SessionSrcSubStatus.stStatus, 0);
			break;


#if	defined(IRPLAT_PVR_DRM) && defined(IRPLAT_SYS_PVR)

		case RESOURCE_SESSION_MANAGEMENT_DRM:
			{
				IR_PVRSERVICE_INFO	*pstPVRSvcInfo = NULL;

				pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(SvcStatusData.wServiceHandle);

				if (pstPVRSvcInfo == NULL)
				{
					HxLOG_Error("pstPVRSvcInfo is null.\n");
				}

				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_SESSION_MANAGEMENT_DRM, RscStatus.wId, 0, RscStatus.stStatus, 0);

				//	substatus
				HxSTD_memcpy((HUINT8*)&SessionSrcSubStatus, (HUINT8*)p, sizeof(msg_session_source_substatus_st));
				p += sizeof(msg_session_source_substatus_st);

				CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_SESSION_SUB, RscStatus.wId, 0, SessionSrcSubStatus.stStatus, 0);

				//	drmStatus
				HxSTD_memcpy((HUINT8*)&drmStatus, (HUINT8*)p, sizeof(msg_sm_drm_status_st));

				p += sizeof(msg_sm_drm_status_st);

				if (drmStatus.fRecordFlagsUpdated == IA_TRUE)
				{
					HxSTD_memcpy((HUINT8*)&drmRecordFlag, (HUINT8*)p, sizeof(msg_sm_drm_record_flags_st));
					p += sizeof(msg_sm_drm_record_flags_st);

					//	TODO: need to noti.
				}
				if (drmStatus.fRightsRecordUpdated == IA_TRUE)
				{
					HxSTD_memcpy((HUINT8*)&drmRightsRecord, (HUINT8*)p, sizeof(msg_sm_drm_rights_record_st));
					p += sizeof(msg_sm_drm_rights_record_st);

					//	TODO: need to noti.
				}

#if	1
				if (pstPVRSvcInfo)
				{
					if (drmStatus.fRightsRecordUpdated == IA_TRUE)
					{
						pstPVRSvcInfo->stPVRDrmInfo.bReady = TRUE;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId = drmStatus.wRightsRecordId;

						pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID		= drmRightsRecord.wCRID;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.eCopyControl	= drmRightsRecord.eCopyControl;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackStartTime	= drmRightsRecord.wPlaybackWindowStartTime;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackDuration	= drmRightsRecord.wPlaybackWindowDuration;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.NrOfPlayback	= drmRightsRecord.bNrOfPlaybacks;
					}
					if (drmStatus.fSequenceRecordUpdated == IA_TRUE)
					{
						pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId = drmStatus.wSequenceRecordId;
					}

					if (drmStatus.fRecordFlagsUpdated == IA_TRUE)
					{
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRecFlagInfo.bReady = TRUE;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRecFlagInfo.fNonPVR = drmRecordFlag.fNonPVRable;
						pstPVRSvcInfo->stPVRDrmInfo.stDrmRecFlagInfo.fMustExpired = drmRecordFlag.fMustExpire;
					}
				}
#else
				if (drmStatus.fRightsRecordUpdated == IA_TRUE)
					CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_DRM_RIGHT, drmStatus.wRightsRecordId, drmStatus.wSequenceRecordId, SessionSrcSubStatus.stStatus, drmRightsRecord.wCRID);
#endif
#if	defined(DEBUG_DRMINFO)
				HxLOG_Print("\tDrmStatus : RightRecId[0x%08X] wSeqRecId[0x%04X] fRecFlagUpdate[%d] fRightRecUpdate[%d], fSequenceRecUpdate[%d]\n",
							drmStatus.wRightsRecordId, drmStatus.wSequenceRecordId, drmStatus.fRecordFlagsUpdated, drmStatus.fRightsRecordUpdated, drmStatus.fSequenceRecordUpdated);
				if (drmStatus.fRecordFlagsUpdated == IA_TRUE)
					HxLOG_Print("\t\tdrmRecFlag : nonPVRable[%d] fMustExpire[%d]\n", drmRecordFlag.fNonPVRable, drmRecordFlag.fMustExpire);
				if (drmStatus.fRightsRecordUpdated == IA_TRUE)
					HxLOG_Print("\t\tdrmRightRec : crId[0x%08X] eCopyControl[%d], wPlaybackStart[%d] wPlaybackDuration[%d] noOfPb[%d]\n",
								drmRightsRecord.wCRID, drmRightsRecord.eCopyControl, drmRightsRecord.wPlaybackWindowStartTime, drmRightsRecord.wPlaybackWindowDuration, drmRightsRecord.bNrOfPlaybacks);
#endif
			}

			break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case RESOURCE_MATURITY_RATING:		// 6.13, referenced on 733999.
			CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_MATURITY_RATING, RscStatus.wId, 0, RscStatus.stStatus, 0);
			/* Set wMRFlag when SOURCE_MOD_MATURITY_RATING:D100/D101 */
			if ((RscStatus.stStatus.eSource == SOURCE_MOD_MATURITY_RATING)\
				&& (RscStatus.stStatus.eSeverity == STATUS_INFO_DND))
			{
				wMRFlag = RscStatus.stStatus.wStatus;
			}
			break;
#endif

		default:
			HxLOG_Print("What CAS_IR_SVC_STATUS ??? Chek it!(rsc type = %d )\n", RscStatus.wType);
			break;

		}
	}

#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
	// ECM status∏¶ ∫∏±‚ ¿ß«ÿº≠ ECM¿Ã ≥°≥≠µ⁄ global¿ª ∫∏µµ∑œ status«‘ºˆ∏¶ ¿Ã∞˜¿∏∑Œ ø≈±Ë.
	CAS_IR_DRV_UpdateStatus(eSvcType, SvcStatusData.wServiceHandle, RESOURCE_GLOBAL_SOURCE, 0, 0, SvcStatusData.stStatus, 0);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	/* Trigger MR PIN code in following cases: */
	/* case: 1.SOURCE_MOD_MATURITY_RATING:D100 && SOURCE_SMARTCARD:E136/E137*/
	/* case: 2.SOURCE_SMARTCARD:E136 */

	/* ONE IMPORTANT Statement */
	/* SOURCE_MOD_MATURITY_RATING is not in Global state in current version's Softcell. 2009.3.4*/

	/*following condition statement is for making just once nofification to extapp layer, after define service */
	if(SvcStatusData.stStatus.eSource == SOURCE_SMARTCARD)
	{
		MsgMR.usMsgClass = IREVT_CLASS_MR_PIN_STAUS_UPDATE;

		/*The PIN window shall be triggered! */
		if (((wSCFlag == 136)&&(wMRFlag != 101)) || ((wSCFlag == 137)&&(wMRFlag == 100)))
		{
			if(wSCFlag == 136)
				MsgMR.usParam = IR_MR_PIN_NOTIFY_E136 ;
			else
				MsgMR.usParam = IR_MR_PIN_NOTIFY_E137 ;

			HxLOG_Print("[IR_MR_PIN_NOTIFY][REQUEST][E%d]\n", wSCFlag);
		}
		else
		{
			if(wSCFlag != 138) /* high possiblity than below */
				MsgMR.usParam = IR_MR_PIN_NOTIFY_NOT_NEED;
			else
				MsgMR.usParam = IR_MR_PIN_NOTIFY_E138 ;

			HxLOG_Print("[IR_MR_PIN_NOTIFY][NOT_NEED]%s\n",(wSCFlag == 138)? "E138": "" );
		}

		for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
		{
			if( (g_IrSvcInfo[i].hIrSvc != HANDLE_NULL) && ( g_IrSvcInfo[i].usSCELL_SvcHandle == SvcStatusData.wServiceHandle ) )
			{
				CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &MsgMR);
			}
		}
	}
#endif

	return 0;
}
#define _______________MSG_FROM_SOFTCELL__________________
/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Base on 710475 SoftCell 3 CA Task Driver API.
********************************************************************/
void CAS_IR_SoftcellMsgProc(CAS_IR_BASE_MSG *pMsgIR)
{
	CAS_IR_BASE_MSG	msgIR;

	if (pMsgIR == NULL)
		return;
	HxSTD_memcpy(&msgIR, pMsgIR, sizeof(CAS_IR_BASE_MSG));

	#ifdef DBG_IRCADRV_PRINT_OPCODE
	local_cas_ir_CA_DRV_DBG_PrintOpCode(msgIR.msg.SOFTCELL.eOpCode);
	#endif

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
	// Chap. 3. General Messages
	case	MSG_CLIENT_VERSION_QUERY:
	case	MSG_COMPONENTS_VERSION_QUERY:
	case	MSG_VERSION_REPLY:
		CAS_IR_SoftcellMsgProc_Version(msgIR);
		break;
	case	MSG_ERROR:	// ∞≥πﬂ ¡ﬂø° « ø‰«— ∞Õ.. Ω«¡¶ µø¿€Ω√ø°¥¬ ≥™ø¿∏È æ»µ .
		CAS_IR_SoftcellMsgProc_Error(msgIR);
		break;

	// Chap. 4. Service Messages
	case	MSG_SERVICE_OPEN_REQUEST:
	case	MSG_SERVICE_OPEN_REPLY:
	case	MSG_SERVICE_CLOSE:
	case	MSG_SERVICE_DEFINE:
		CAS_IR_SoftcellMsgProc_Service(msgIR);
		break;
	case	MSG_SERVICE_STATUS_REQUEST:
	case	MSG_SERVICE_STATUS:
		CAS_IR_SoftcellMsgProc_ServiceStatus(msgIR);
		break;

	// Chap. 7. DVB Client Monitoring
	case	MSG_DVB_EMM_MONITOR:
	case	MSG_DVB_ECM_MONITOR:
		CAS_IR_SoftcellMsgProc_Monitor(msgIR);
		break;

	// Chap. 9. Smartcard Message.
	case	MSG_SMARTCARD_STATUS:				// 9.2
	case	MSG_NUMBER_OF_SECTORS_NOTIFY:		// 9.3
	case	MSG_NATIONALITY_QUERY:				// 9.4
	case	MSG_NATIONALITY:					// 9.5
	case	MSG_SMARTCARD_ID:					// 9.6
	case	MSG_SMARTCARD_DATA_QUERY:			// 9.7
	case	MSG_SMARTCARD_DATA_REPLY:			// 9.8
	case	MSG_SMARTCARD_HOMING_CH_QUERY:		// 9.9
	case	MSG_SMARTCARD_HOMING_CH_REPLY:		// 9.10
	case	MSG_PPT_INFO_QUERY:					// 9.11
	case	MSG_PPT_INFO_REPLY:					// 9.12
	case	MSG_SURF_LOCK_QUERY:				// 9.13
	case	MSG_SURF_LOCK_REPLY:				// 9.14
	case	MSG_REGION_QUERY:					// 9.15
	case	MSG_REGION_REPLY:					// 9.16
	case	MSG_PIN_CODE_QUERY:					// 9.17
	case	MSG_PIN_CODE_REPLY:					// 9.18
	case	MSG_REMOTE_MESSAGE_QUERY:			// 9.19
	case	MSG_REMOTE_MESSAGE_REPLY:			// 9.20
	case	MSG_SMARTCARD_USER_DATA_QUERY:		// 9.21
	case	MSG_SMARTCARD_USER_DATA_REPLY:		// 9.22
	case	MSG_SMARTCARD_VENDOR_ID_QUERY:
	case	MSG_SMARTCARD_VENDOR_ID_REPLY:
		CAS_IR_SoftcellMsgProc_Smartcard(msgIR);
		break;

	// Chap. 11. Decoder Messages
	case	MSG_DECODER:
		CAS_IR_SoftcellMsgProc_Decoder(msgIR);
		break;

	case	MSG_IPTV_EMM_MONITOR:
		break;

	case	MSG_IPTV_REGISTRATION_ATTEMPTS:
		break;

	case	MSG_SERVICE_INFO_QUERY:
	case	MSG_TRANSPORT_INFO_QUERY:
	case	MSG_NETWORK_INFO_QUERY:
		CAS_IR_SoftcellMsgProc_TMInfo(msgIR);
		break;	// prevent fix : missing 'break' statement

#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
	case	MSG_EXTENDED_PRODUCT_LIST_REPLY:
#endif
	case	MSG_ANY_PRODUCT_REPLY:
		CAS_IR_SoftcellMsgProc_Product(msgIR);
		break;

	case	MSG_SERVICE_CLOSE_REPLY:
		CAS_IR_SoftcellMsgProc_Service(msgIR);
		break;

#if defined(CONFIG_MW_CAS_IRDETO_PPV)
	// Chap. 13. IPPV
	case	MSG_IPPV_EVENT_NOTIFY:
	case	MSG_IPPV_BUY_REPLY:
	case	MSG_IPPV_SECTOR_INFORMATION_REPLY:
	case	MSG_IPPV_SET_DEBIT_THRESHOLD:
	case	MSG_IPPV_CALLBACK_REQUEST:
	case	MSG_IPPV_EVENT_LIST_REPLY:
	case	MSG_IPPV_EVENT_CHECK_REPLY:
	case	MSG_IPPV_PHONE_NUMBER_REPLY:
		CAS_IR_SoftcellMsgProc_IPPV(msgIR);
		break;

	// Chap. 15. LPPV
	case MSG_LPPV_AWARE_REPLY:
	case MSG_LPPV_EVENT_NOTIFY:
	case MSG_LPPV_READ_TICKET_REPLY:
	case MSG_LPPV_DELETE_TICKET_REPLY:
	case MSG_LPPV_READ_CREDIT_REPLY:
	case MSG_LPPV_READ_ORDER_REPLY:
	case MSG_LPPV_PLACE_ORDER_REPLY:
	case MSG_LPPV_CANCEL_ORDER_REPLY:
	case MSG_EMM_TLV_NOTIFY:
		CAS_IR_SoftcellMsgProc_LPPV(msgIR);
		break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	/* Maturity Rating. */
	case	MSG_MATURITY_RATING_CAPABILITY_QUERY://   = 137,
	case	MSG_MATURITY_RATING_CAPABILITY_REPLY://   = 138,
	case	MSG_MATURITY_RATING_CONFIG_QUERY: //       = 139,
	case	MSG_MATURITY_RATING_CONFIG_REPLY : //= 140,
	case	MSG_MATURITY_RATING_USER_PROFILE_QUERY ://= 141,
	case	MSG_MATURITY_RATING_USER_PROFILE_REPLY: //= 142,
		CAS_IR_SoftcellMsgProc_Smartcard(msgIR);
	break;
#endif
	//following messages are printed while default card consultration.
#if 0 // currently not supported
	case	MSG_IPPV_EVENT_NOTIFY:
	case	MSG_IPPV_BUY_QUERY:
	case	MSG_IPPV_BUY_REPLY:
	case	MSG_IPPV_SECTOR_INFORMATION_QUERY:
	case	MSG_IPPV_SET_DEBIT_THRESHOLD:
	case	MSG_IPPV_CALLBACK_REQUEST:
	case	MSG_IPPV_EVENT_LIST_QUERY:
	case	MSG_IPPV_EVENT_LIST_REPLY:
	case	MSG_IPPV_EVENT_CHECK_QUERY:
	case	MSG_IPPV_EVENT_CHECK_REPLY:
	case	MSG_RFU1:
	case	MSG_REGISTRATION_USE_NEW_HEADEND:
	case	MSG_SMARTCARD_RSA_SIGN_REQUEST:
	case	MSG_SMARTCARD_RSA_SIGN_REPLY:
	case	MSG_SMARTCARD_RSA_VERIFY_REQUEST:
	case	MSG_SMARTCARD_RSA_VERIFY_REPLY:
	case	MSG_SMARTCARD_RSA_ENCRYPT_REQUEST:
	case	MSG_SMARTCARD_RSA_ENCRYPT_REPLY:
	case	MSG_SMARTCARD_RSA_DECRYPT_REQUEST:
	case	MSG_SMARTCARD_RSA_DECRYPT_REPLY:
	case	MSG_PRODUCT_CHECK_QUERY:
	case	MSG_PRODUCT_CHECK_REPLY:
	case	MSG_ANY_PRODUCT_QUERY:
	case	MSG_PRODUCT_LIST_QUERY:
	case	MSG_PRODUCT_LIST_REPLY:
	case	MSG_CLIENT_ID_QUERY:
	case	MSG_CLIENT_ID_REPLY:
	case	MSG_CC_SERVICE_ADD:
	case	MSG_CC_SERVICE_DELETE:
	case	MSG_CC_SERVICE_UPDATE:
	case	MSG_EXTENDED_PRODUCT_LIST_QUERY:
	case	MSG_EXTENDED_PRODUCT_LIST_REPLY:
	case	MSG_EXPIRED_PRODUCT_QUERY:
	case	MSG_EXPIRED_PRODUCT_REPLY:
		break;
#endif
	default :
		HxLOG_Print(": NOT Processed.(opCode = %d)\n", msgIR.msg.SOFTCELL.eOpCode);
		break;
	}

	if( msgIR.msg.SOFTCELL.ucFreeFlag == TRUE )
	{
		if (msgIR.msg.SOFTCELL.pvMessage != NULL)
		{
			OxCAS_Free(msgIR.msg.SOFTCELL.pvMessage);
		}
	}
}

/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Version
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Base on 710475 SoftCell 3 CA Task Driver API - Chap.3. General Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_Version(CAS_IR_BASE_MSG msgIR)
{
	msg_version_reply_data_st	VersionReplyData;
	HUINT8						*p;
	HUINT8						i;
	SOFTCELL_VERSION_DATA		*pVersionData;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		// Chap. 3. General Message... SOFTCELL => CAS TASK
		case	MSG_VERSION_REPLY:
				p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
				HxSTD_memcpy((HUINT8 *)&VersionReplyData, (HUINT8 *)p, sizeof(msg_version_reply_data_st));
				p += sizeof(msg_version_reply_data_st);
				HxLOG_Print("[MSG_VERSION_REPLY] wVersionCount = %d\n", VersionReplyData.wVersionCount);

				if( VersionReplyData.wVersionCount == 0 )
					return;


				if( VersionReplyData.wVersionCount == 1 )	// ClientVersion
				{
					pVersionData = (SOFTCELL_VERSION_DATA *)ClientVersionData;

					if( pVersionData[0].pucNameString!= NULL )
					{
						OxCAS_Free(pVersionData[0].pucNameString);
						pVersionData[0].pucNameString = NULL;
					}
					if( pVersionData[0].pucBuildInfoString!= NULL )
					{
						OxCAS_Free(pVersionData[0].pucBuildInfoString);
						pVersionData[0].pucBuildInfoString = NULL;
					}
				}
				else // ComponentsVersion
				{
					pVersionData = (SOFTCELL_VERSION_DATA *)ComponentsVersionData;

					HxLOG_Print("[MSG_VERSION_REPLY] usCount = %d\n", pVersionData[0].usCount);

					for( i = 0; i < pVersionData[0].usCount; i++ )
					{
						if( pVersionData[i].pucNameString!= NULL )
						{
							OxCAS_Free(pVersionData[i].pucNameString);
							pVersionData[i].pucNameString = NULL;
						}
						if( pVersionData[i].pucBuildInfoString!= NULL )
						{
							OxCAS_Free(pVersionData[i].pucBuildInfoString);
							pVersionData[i].pucBuildInfoString = NULL;
						}
					}
				}

				HxLOG_Print(" wVersionCount = %d\n", VersionReplyData.wVersionCount);
				for( i = 0; i < VersionReplyData.wVersionCount; i++ )
				{
					pVersionData[i].usCount = VersionReplyData.wVersionCount;
					HxSTD_memcpy((HUINT8 *)&(pVersionData[i].VersionData), (HUINT8 *)p, sizeof(msg_version_data_st));
					p += sizeof(msg_version_data_st);

					HxLOG_Print(" [%d].bMajor = %d\n", i, pVersionData[i].VersionData.bMajor);
					HxLOG_Print(" [%d].bMinor = %d\n", i, pVersionData[i].VersionData.bMinor);
					HxLOG_Print(" [%d].wSubVersion = 0x%X\n", i, (HUINT32)pVersionData[i].VersionData.wSubVersion);
					HxLOG_Print(" [%d].wNameLength = 0x%X\n", i, pVersionData[i].VersionData.wNameLength);
					HxLOG_Print(" [%d].wBuildInfoLength = 0x%X\n", i, pVersionData[i].VersionData.wBuildInfoLength);

					if( pVersionData[i].VersionData.wNameLength)
					{
						pVersionData[i].pucNameString = (HUINT8 *)OxCAS_Malloc(pVersionData[i].VersionData.wNameLength + 1);
						if (pVersionData[i].pucNameString == NULL)
						{
							HxLOG_Print("[5] Error : Mem Alloc Error 3\n");
							return;
						}
						HxSTD_memset(pVersionData[i].pucNameString, 0, pVersionData[i].VersionData.wNameLength + 1);
					}

					if( pVersionData[i].pucNameString != NULL )
					{
						HxSTD_memcpy(pVersionData[i].pucNameString, (HUINT8 *)p, pVersionData[i].VersionData.wNameLength);
						pVersionData[i].pucNameString[pVersionData[i].VersionData.wNameLength] = 0;
					}
					p += pVersionData[i].VersionData.wNameLength;

					HxLOG_Print("[%d].NameString = [%s]\n", i, pVersionData[i].pucNameString);

					if( pVersionData[i].VersionData.wBuildInfoLength)
					{
						pVersionData[i].pucBuildInfoString = (HUINT8 *)OxCAS_Malloc(pVersionData[i].VersionData.wBuildInfoLength + 1);
						if (pVersionData[i].pucBuildInfoString == NULL)
						{
							HxLOG_Print("[6] Error : Mem Alloc Error 3\n");
							return;
						}

						HxSTD_memcpy(pVersionData[i].pucBuildInfoString, (HUINT8 *)p, pVersionData[i].VersionData.wBuildInfoLength);
						pVersionData[i].pucBuildInfoString[pVersionData[i].VersionData.wBuildInfoLength] = 0;
						HxLOG_Print("[%d].pucBuildInfoString = [%s]\n", i, pVersionData[i].pucBuildInfoString);
					}
					p += pVersionData[i].VersionData.wBuildInfoLength;


				}
				// TODO: ¡¶¥Î∑Œ µ» path ∑Œ ø¨∞·«ÿ¡÷¿⁄. cas mgr ø°º≠ ∫∏≥ªµµ∑œ.
				HxLOG_Print("Send Menu Update msg \n");
				svc_cas_MgrSubUpdateCasEtcEvent(HANDLE_NULL, HANDLE_NULL, eDxCAS_GROUPID_IR,
											eSEVT_CAS_NOTIFICATION, eDxCAS_GROUPID_IR, 0, 0);
				break;

		// Chap. 3. General Message... CAS TASK => SOFTCELL.
		// ¿Ã messageµÈ¿∫ SCELL_Message∏¶ ≈Î«ÿ SOFTCELLø° ¿¸¥ﬁµ«æÓæﬂ «œπ«∑Œ ¿Ã «‘ºˆø°º≠¥¬ √≥∏Æµ«¡ˆ æ ¿Ω.
		case	MSG_CLIENT_VERSION_QUERY:
		case	MSG_COMPONENTS_VERSION_QUERY:
				break;
	}
}


/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Error
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Base on 710475 SoftCell 3 CA Task Driver API - Chap.3. General Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_Error(CAS_IR_BASE_MSG msgIR)
{
	msg_error_data_st			ErrorData;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case	MSG_ERROR:	// ∞≥πﬂ ¡ﬂø° « ø‰«— ∞Õ.. Ω«¡¶ µø¿€Ω√ø°¥¬ ≥™ø¿∏È æ»µ .
		HxSTD_memcpy((HUINT8 *)&ErrorData, (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage), sizeof(msg_error_data_st));

		HxLOG_Debug("eOpCode(0x%X), wOpSeqNo(0x%X), eSource(0x%X), eSeverity(0x%X), wStatus(0x%X)\n", \
			ErrorData.eOpcode, ErrorData.wOpSeqNo, ErrorData.stStatus.eSource, ErrorData.stStatus.eSeverity, ErrorData.stStatus.wStatus);

		#ifdef DBG_IRCADRV_PRINT_OPCODE
		local_cas_ir_CA_DRV_DBG_PrintOpCode(ErrorData.eOpcode);
		#endif

		CAS_IR_DRV_UpdateStatus(0, INVALID_SOFTCELL_SVCHANDLE, RESOURCE_ERROR, ErrorData.eOpcode, ErrorData.wOpSeqNo, ErrorData.stStatus, 0);
		HxLOG_Critical("\n\n");
		break;
	}
}



/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Service
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Base on 710475 SoftCell 3 CA Task Driver API - Chap.4. Service Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_Service(CAS_IR_BASE_MSG msgIR)
{
	HUINT8		*p = NULL;
	msg_service_open_reply_data_st	SvcOpenReplyData;
	msg_service_close_data_st		SvcCloseData;
	HMSG	hMsg;

	//identify multiple softcell message with the same OpCode,
	//reqply_OpSeqNo is the same with the query_OpSeqNo
	HUINT32				SvcIndex = msgIR.msg.SOFTCELL.wOpSeqNo;
	HUINT32				OpCode = msgIR.msg.SOFTCELL.eOpCode;

	if (SvcIndex >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("SvcIndex >= IR_HUMAX_MAX_SERVICE\n");
		return;
	}

	switch(OpCode)
	{
	// Chap. 4. Service Message... SOFTCELL => CAS TASK
	case	MSG_SERVICE_OPEN_REPLY:
		if( msgIR.msg.SOFTCELL.pvMessage != (void *)NULL )
		{
			p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
			if (p == NULL)
			{
				HxLOG_Print("[CAS_IR_SoftcellMsgProc_Service] p == NULL\n");
				return;
			}
			HxSTD_memcpy((HUINT8 *)&SvcOpenReplyData, (HUINT8 *)p, sizeof(msg_service_open_reply_data_st));
			HxLOG_Print(" OPEN_REPLY : Type(0x%04X), Handle(0x%04X)\n", SvcOpenReplyData.wServiceType, SvcOpenReplyData.wServiceHandle);

			switch(SvcOpenReplyData.wServiceType)
			{
				case SERVICE_DVB_DESCRAMBLE:
					{
						IR_SERVICE_INFO 	*pIrSvcInfo = &(g_IrSvcInfo[SvcIndex]);

						HxLOG_Print("\t SERVICE_DVB_DESCRAMBLE:REPLY : descramble service hanle(0x%04X) to SvcIdx(%d)\n", SvcOpenReplyData.wServiceHandle, SvcIndex);
						HxLOG_Print("\t SERVICE_DVB_DESCRAMBLE:REPLY : pIrSvcInfo->hIrSvc = 0x%X\n", pIrSvcInfo->hIrSvc);

						if( pIrSvcInfo->hIrSvc != HANDLE_NULL )
						{
							if (!pIrSvcInfo->ucIsSCELLReady)
							{
								pIrSvcInfo->usSCELL_SvcHandle = SvcOpenReplyData.wServiceHandle;
								pIrSvcInfo->ucIsSCELLReady = 1;
								hMsg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_OPEN;
								CAS_IR_EVT_SendEvent(pIrSvcInfo->hAction, &hMsg);

								//	open reply¿¸ø°¥¬ svc_define¿Ã æ»µ«π«∑Œ ø©±‚º≠ pid∏¶ πﬁæ∆ ≥ı¿∫∞Õ¿Ã ¿÷¿∏∏È defineΩ√≈≤¥Ÿ.
								if (pIrSvcInfo->ucStreamCount)
								{
									CAS_IR_SVC_DEFINE_DVB(SvcIndex);
								}
								else
								{
									HxLOG_Print("\t No PID was set yet!!\n");
								}
							}
							else
							{
								HxLOG_Print("\t CAS_IR_SVC_OPEN_REPLY_PVR: Invalid service handle - Send Close Service!!\n");
								//if stop service is called before service_open_reply, close service which is open.
								pIrSvcInfo->usSCELL_SvcHandle = SvcOpenReplyData.wServiceHandle;
								pIrSvcInfo->usSCELL_SvcType = SvcOpenReplyData.wServiceType;
								CAS_IR_SVC_CLOSE_DVB(SvcIndex, FALSE);
							}
						}
						else
						{
							HxLOG_Print("ERROR: SERVICE_OPEN_REPLY - hAction = NULL!!\n");
						}
					}
					break;

				case SERVICE_DVB_EMM:
					HxLOG_Print("\t SERVICE_DVB_EMM:REPLY : Emm service hanle(0x%04X) to SvcIdx(%d)\n", SvcOpenReplyData.wServiceHandle, SvcIndex);
					CAS_IR_SVC_DEFINE_EMM(SvcOpenReplyData.wServiceHandle);
					break;

#if defined(IRPLAT_SYS_PVR)
#if	defined(IRPLAT_PVR_PHASE12)
				case SERVICE_PVR_RECORD:
				case SERVICE_PVR_PLAYBACK:
#endif
#if	defined(IRPLAT_PVR_DRM)
				case SERVICE_PVR_DRM:
#endif
					CAS_IR_SVC_OPEN_REPLY_PVR(SvcOpenReplyData.wServiceHandle, SvcOpenReplyData.wServiceType, msgIR.msg.SOFTCELL.wOpSeqNo);
					break;
#endif

				default:
					HxLOG_Print(" CHAP. 4. Unknown Service Type(0x%X)\n", SvcOpenReplyData.wServiceType);
					break;
			}
		}
		else
		{
			HxLOG_Print(" MSG_SERVICE_OPEN_REPLY ERROR:pvMessage is NULL\n");
		}
		break;

	// Chap. 4. Service Message... CAS TASK => SOFTCELL.
	// ¿Ã messageµÈ¿∫ SCELL_Message∏¶ ≈Î«ÿ SOFTCELLø° ¿¸¥ﬁµ«æÓæﬂ «œπ«∑Œ ¿Ã «‘ºˆø°º≠¥¬ √≥∏Æµ«¡ˆ æ ¿Ω.
	// INT_IR_DRV_ProcessMsgFromCAI ¬¸¡∂.
	case	MSG_SERVICE_OPEN_REQUEST:
	case	MSG_SERVICE_CLOSE:
	case	MSG_SERVICE_DEFINE:

	/* extension part */
	case	MSG_SERVICE_CLOSE_REPLY:
		if( msgIR.msg.SOFTCELL.pvMessage != (void *)NULL )
		{
			p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
			if (p == NULL)
			{
				HxLOG_Print("[SoftcellMsgProc_Service] p == NULL\n");
				return;
			}
			HxSTD_memcpy((HUINT8 *)&SvcCloseData, (HUINT8 *)p, sizeof(msg_service_close_data_st));
		}
		break;
	default :
		// TODO: Check the case of being occured.
		HxLOG_Print("[CAS_IR_SoftcellMsgProc_Service] eOpCode(0x%X))\n", OpCode);
		break;

	}
}


	/********************************************************************
	*	function name :  CAS_IR_SoftcellMsgProc_Service
	*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
	*	output : None
	*	description : Base on 710475 SoftCell 3 CA Task Driver API - Chap.4. Service Message
	********************************************************************/
void CAS_IR_SoftcellMsgProc_ServiceStatus(CAS_IR_BASE_MSG msgIR)
{
	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
	// Chap. 4. Service Message... SOFTCELL => CAS TASK

		case	MSG_SERVICE_STATUS:
			if( msgIR.msg.SOFTCELL.pvMessage != (void *)NULL )
			{
				CAS_IR_SVC_STATUS((HUINT8 *)msgIR.msg.SOFTCELL.pvMessage, msgIR.msg.SOFTCELL.wLength);
			}
			else
			{
				HxLOG_Print(" MSG_SERVICE_STATUS ERROR:pvMessage is NULL\n");
			}
			break;

		case	MSG_SERVICE_STATUS_REQUEST:
			break;
	}
}


/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Monitor
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API - Chap.7. DVB Client Monitoring
********************************************************************/
void CAS_IR_SoftcellMsgProc_Monitor(CAS_IR_BASE_MSG msgIR)
{
	HMSG msg;
	msg_dvb_ecm_monitor_data_st	ecm_monitoring_st;
	msg_dvb_emm_monitor_data_st	emm_monitoring_st;
	HUINT8	*p;
	HUINT8	i, j;

	HxSTD_memset(&msg, 0, sizeof(HMSG) );

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		// Chap. 7. DVB Client Monitoring... SOFTCELL => CAS TASK
		case	MSG_DVB_EMM_MONITOR:				// 7.2
			HxLOG_Print("MSG_DVB_EMM_MONITOR");

			p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
			HxSTD_memcpy((HUINT8 *)&emm_monitoring_st, (HUINT8 *)p, sizeof(msg_dvb_emm_monitor_data_st));
			p += sizeof(msg_dvb_emm_monitor_data_st);


			for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
			{
				for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
				{
					if (g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].usSCELL_SvcHandle[j] == emm_monitoring_st.wServiceHandle)
					{
						HxSTD_memcpy((HUINT8 *)&(g_IrTpInfo[i].EmmMonitoring[j]), (HUINT8 *)&emm_monitoring_st, sizeof(msg_dvb_emm_monitor_data_st));

					#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
						g_IrTpInfo[i].EmmMonitoring[j].wEmmCount += s_EmmMonitorCount[j];
					#endif

						HxLOG_Print("EMM : %02X %02X %02X %02x %02x %02x (%d)",
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].bAddressControl,
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].bScStatus1,
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].bScStatus2,
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[0],
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[1],
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[2],
							(HINT32) g_IrTpInfo[i].EmmMonitoring[j].wEmmCount);
						break;
					}
				}
			}

			if(i == IR_HUMAX_MAX_TP)
			{
				HxLOG_Critical("\n\n");
				i = IR_HUMAX_MAX_TP - 1;
			}

			msg.usMsgClass = IREVT_CLASS_EMM_MONITOR_UPDATE;
			CAS_IR_EVT_SendEvent(g_IrTpInfo[i].hAction, &msg);
			break;

		case	MSG_DVB_ECM_MONITOR:				// 7.3
			HxLOG_Print("MSG_DVB_ECM_MONITOR");

			p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
			HxSTD_memcpy((HUINT8 *)&ecm_monitoring_st, (HUINT8 *)p, sizeof(msg_dvb_ecm_monitor_data_st));
			p += sizeof(msg_dvb_ecm_monitor_data_st);

			for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
			{
				if( g_IrSvcInfo[i].usSCELL_SvcHandle == ecm_monitoring_st.wServiceHandle )
				{
					HxLOG_Info("ecm_monitoring_st.bScStatus1 : 0x%x\n", ecm_monitoring_st.bScStatus1);
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
					if(ecm_monitoring_st.bScStatus1 == 0x9D)	// Preview
					{
						HxLOG_Info("ecm_monitoring_st.abScReply[5] : 0x%x\n", ecm_monitoring_st.abScReply[5]);
						if(ecm_monitoring_st.abScReply[5] >= 0x01 && ecm_monitoring_st.abScReply[5] <= 0xFE)
						{
							// Display ECM Preview Time
							msg.usMsgClass = IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME;
							HxLOG_Error("________________Preview Left Time - %d\n", ecm_monitoring_st.abScReply[5]);
							msg.usParam = ecm_monitoring_st.abScReply[5];
							CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
							break;
						}
					}
#endif

					for (j = 0; j < MAX_NUM_ECM_MONITOR_PID; j++)
					{
						if (g_IrSvcInfo[i].EcmMonitorInfo[ecm_monitoring_st.wResourceId].ausEcmMonitorPID[j] == IRDRV_NULL_PID)
						{
							g_IrSvcInfo[i].EcmMonitorInfo[ecm_monitoring_st.wResourceId].ausEcmMonitorPID[j] = ecm_monitoring_st.wEcmPid;
							break;
						}

						if (g_IrSvcInfo[i].EcmMonitorInfo[ecm_monitoring_st.wResourceId].ausEcmMonitorPID[j] == ecm_monitoring_st.wEcmPid)
						{
							break;
						}
					}

					if (j < MAX_NUM_ECM_MONITOR_PID)
					{
						g_IrSvcInfo[i].EcmMonitorInfo[ecm_monitoring_st.wResourceId].astEcmMonitoring[j] = ecm_monitoring_st;
					}
					break;
				}
			}

			if(i == IR_HUMAX_MAX_SERVICE)
			{
				HxLOG_Critical("\n\n");
				i = IR_HUMAX_MAX_SERVICE - 1;
			}

			msg.usMsgClass = IREVT_CLASS_ECM_MONITOR_UPDATE;
			CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
			break;

		default:
			HxLOG_Print("unkown op code (0x%X)\n", msgIR.msg.SOFTCELL.eOpCode);
			break;
	}

}

/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Smartcard
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Base on 710475 SoftCell 3 CA Task Driver API - Chap.9. Smartcard Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_Smartcard(CAS_IR_BASE_MSG msgIR)
{
	HUINT32								i, ulS3MsgLength;
	HUINT8								*pucS3MsgPayload;
	msg_smartcard_status_data_st		smartcard_status;
	msg_nationality_data_st				nationality_data;
	msg_smartcard_id_data_st			smartcard_id_data;
	msg_smartcard_data_reply_data_st	smartcard_data_reply_data;
#if defined(IRPLAT_SC_QUERY_NUMBER_OF_SECTORS_ID)
	msg_number_of_sectors_reply_data_st	number_of_sectors_reply_data;
	msg_number_of_sectors_sector_st		number_of_sectors[MAX_SC_SECTOR_COUNT];
#endif

#if defined(IRPLAT_FN_QUERY_VENDOR_ID)
	msg_vendor_id_reply_st				smartcard_vendor_id_reply;
	msg_vendor_id_st					smartcard_vendor_id_data[MAX_SC_SECTOR_COUNT];
	HUINT16								ausCaSysId[MAX_SC_SECTOR_COUNT]={0,};
	HUINT16								ucNumCaSysId=0;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	msg_mr_capability_reply_st			mr_capability_reply;
	msg_mr_config_reply_st				mr_config_reply;
	msg_mr_config_data_st				mr_config_data;
	msg_mr_user_profile_reply_st		mr_user_profile_reply;
	msg_mr_user_profile_data_st 		*pstTempUserPF;
	HUINT8	ReplyResult;
#endif


	vd_rsc_id							ResourceID;
	HMSG								msg;

	/*Initialize local function member  */
	HxSTD_memset(&msg, 0, sizeof(HMSG));

	/*Followings are used for reducing complexity of referencing pointers */
	pucS3MsgPayload = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
	ulS3MsgLength = (HUINT32) msgIR.msg.SOFTCELL.wLength;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{

#if defined(IRPLAT_FN_QUERY_VENDOR_ID)
		// Chap. 9. Smartcard Message... SOFTCELL => CAS TASK
		//in octo system. this has top priority.
		case	MSG_SMARTCARD_VENDOR_ID_REPLY:
				HxLOG_Print("Vendor ID reply(Nums = %d)\n",smartcard_vendor_id_reply.bNumOfVendorId );

				HxSTD_memcpy((HUINT8*)&smartcard_vendor_id_reply, pucS3MsgPayload, sizeof(msg_vendor_id_reply_st) );
				pucS3MsgPayload+=sizeof(msg_vendor_id_reply_st);

				if(smartcard_vendor_id_reply.bNumOfVendorId > MAX_SC_SECTOR_COUNT)
				{
					smartcard_vendor_id_reply.bNumOfVendorId = MAX_SC_SECTOR_COUNT;
				}

				HxSTD_memcpy((HUINT8*)smartcard_vendor_id_data,
										pucS3MsgPayload,
										smartcard_vendor_id_reply.bNumOfVendorId * sizeof(msg_vendor_id_st));

				for(i=0; i< smartcard_vendor_id_reply.bNumOfVendorId; i++)
				{
					if(IA_TRUE == smartcard_vendor_id_data[i].fActive)
					{
						ausCaSysId[ucNumCaSysId] = smartcard_vendor_id_data[i].wVendorId ;
						ucNumCaSysId++;
					}
				}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
				svc_cas_DevScNotifyScValidity(g_IrDrvScInfo[smartcard_vendor_id_reply.wResourceId].hIRSci, eCAS_SCM_FLAG_ENABLE);
				g_IrDrvScInfo[smartcard_vendor_id_reply.wResourceId].CardPhysicalStatus = eCAS_SCI_SC_PRESENT;
#endif

				CAS_IR_MGR_Notify_InstanceChanged((HUINT16)smartcard_vendor_id_reply.wResourceId, ausCaSysId, ucNumCaSysId, TRUE);
				break;
#endif
		case	MSG_SMARTCARD_STATUS:				// 9.2
				HxLOG_Print("wLength(0x%X), pvMessage@(0x%X)\n", ulS3MsgLength, (HUINT32) pucS3MsgPayload);
				HxSTD_memcpy((HUINT8*)&smartcard_status, pucS3MsgPayload, sizeof(msg_smartcard_status_data_st));
				ResourceID = smartcard_status.wResourceId;
				HxLOG_Print("resourceID : 0x%X\n", (HUINT16)ResourceID);
				HxLOG_Print("eSource : 0x%X, wStatus : 0x%X, eSeverity : 0x%X\n", smartcard_status.stStatus.eSource, smartcard_status.stStatus.wStatus, smartcard_status.stStatus.eSeverity);
				CAS_IR_DRV_UpdateStatus(0, INVALID_SOFTCELL_SVCHANDLE, RESOURCE_SMARTCARD, ResourceID, 0, smartcard_status.stStatus, 0);
				break;

		case	MSG_NUMBER_OF_SECTORS_NOTIFY:		// 9.3
#if defined(IRPLAT_SC_QUERY_NUMBER_OF_SECTORS_ID)
#if 1	// not used... moreover, memory leak...	// NEOø°º≠ ∞°¡Æø». 3.10ø°∏∏ ¿˚øÎµ«¥¬ ∫Œ∫–¿Œ¡ˆ¥¬ »Æ¿Œ « ø‰.
				HxSTD_memcpy((HUINT8 *)&number_of_sectors_reply_data, pucS3MsgPayload, sizeof(msg_number_of_sectors_reply_data_st));
				pucS3MsgPayload += sizeof(msg_number_of_sectors_reply_data_st);
				ResourceID = number_of_sectors_reply_data.wResourceId;
				if (ResourceID >= HUMAX_VD_SC_SLOT_COUNT)
				{
					break;
				}
				HxLOG_Print("resourceID : 0x%X\n", (HUINT32) ResourceID);
				HxLOG_Print("bSectors : 0x%X\n", number_of_sectors_reply_data.bSectors);
				g_IrDrvScInfo[ResourceID].bSectors = number_of_sectors_reply_data.bSectors;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.nNumberOfSecotrs = number_of_sectors_reply_data.bSectors;
				if( number_of_sectors_reply_data.bSectors != 0 )
				{
					if (g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st)
					{
						OxCAS_Free(g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st);
					}
					g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st = CA_MEM_Alloc(sizeof(msg_number_of_sectors_sector_st) * number_of_sectors_reply_data.bSectors);
					for( i = 0; i < number_of_sectors_reply_data.bSectors; i++ )
					{
						HxSTD_memcpy((HUINT8 *)&number_of_sectors[i], (HUINT8 *)pucS3MsgPayload, sizeof(msg_number_of_sectors_sector_st));
						pucS3MsgPayload += sizeof(msg_number_of_sectors_sector_st);
						HxSTD_memcpy(&(g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st[i]), &(number_of_sectors[i]), sizeof(msg_number_of_sectors_sector_st));

						if( i > MAX_SC_SECTOR_COUNT )
							break;
					}
#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
					CAS_IR_DRV_QueryExtendedProductList(ResourceID, IA_TRUE, g_IrDrvScInfo[ResourceID].bSectors, g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st);
#endif
				}

#if 0
{
	for( i = 0; i < g_IrDrvScInfo[ResourceID].bSectors; i++ )
	{
		HxLOG_Print("[Sector #%d]\n", i);
		HxLOG_Print("bSector : 0x%X\n", g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st[i].bSector);
		HxLOG_Print("fActive : 0x%X\n", g_IrDrvScInfo[ResourceID].p_number_of_sectors_sector_st[i].fActive);
		if( i > MAX_SC_SECTOR_COUNT )
			break;
	}
}
#endif
#endif
#endif
				break;

		case	MSG_NATIONALITY:					// 9.5
#if defined (CONFIG_OP_ALMAJD)
				if (g_IrDrvScInfo[0].stIrScApiInfo.ucValid == 0)	// Al Majd ¥¬ nationality ¿¸ø° smart card number ∏¶ æÀæ∆æﬂ «œ¥¬ ¿Ã¿Ø∞° ¿÷¥Ÿ.
				{
					break;
				}
#endif
				HxSTD_memcpy((HUINT8 *)&nationality_data, pucS3MsgPayload, sizeof(msg_nationality_data_st));
				HxLOG_Print("resourceID : 0x%X\n", (HUINT32) nationality_data.wResourceId);
				HxLOG_Print("nationality : %c%c%c\n", nationality_data.acNationality[0], nationality_data.acNationality[1], nationality_data.acNationality[2] );

				if (nationality_data.wResourceId >= HUMAX_VD_SC_SLOT_COUNT)
				{
					break;
				}

				HxSTD_memcpy(g_IrDrvScInfo[nationality_data.wResourceId].stIrScApiInfo.szNationality, nationality_data.acNationality, 3);

				msg.usMsgClass = IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM;

				for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
				{
					if( g_IrSvcInfo[i].hIrSvc != HANDLE_NULL )
					{
						CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
					}
				}

				break;

		case	MSG_SMARTCARD_ID:					// 9.6
				HxSTD_memcpy((HUINT8 *)&smartcard_id_data, pucS3MsgPayload, sizeof(msg_smartcard_id_data_st));
				pucS3MsgPayload += sizeof(msg_smartcard_id_data_st);
				HxLOG_Print("resourceID : 0x%X\n", (HUINT32)smartcard_id_data.wResourceId);
				HxLOG_Print("service handle : 0x%X\n", smartcard_id_data.wServiceHandle);
				HxLOG_Print("bIdStringLength : 0x%X\n", smartcard_id_data.bIdStringLength);
				for( i = 0; i < smartcard_id_data.bIdStringLength; i++ )
				{
					HxLOG_Print("%02X ", pucS3MsgPayload[i]);
				}
				HxLOG_Print("\n");

				if (smartcard_id_data.wResourceId >= HUMAX_VD_SC_SLOT_COUNT)
				{
					break;
				}

				HxSTD_memcpy(g_IrDrvScInfo[smartcard_id_data.wResourceId].szScID, pucS3MsgPayload, smartcard_id_data.bIdStringLength);

				HxLOG_Print("g_IrDrvScInfo[%d].szScID: %s \n", smartcard_id_data.wResourceId, g_IrDrvScInfo[smartcard_id_data.wResourceId].szScID);

				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT;
				msg.usParam = smartcard_id_data.wResourceId ;	//smartcard slot id

				for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
				{
					if( g_IrSvcInfo[i].hIrSvc != HANDLE_NULL )
					{
						CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
					}
				}
				break;

		case	MSG_SMARTCARD_DATA_REPLY:			// 9.8
				HxSTD_memcpy((HUINT8 *)&smartcard_data_reply_data, pucS3MsgPayload, sizeof(msg_smartcard_data_reply_data_st));
				pucS3MsgPayload += sizeof(msg_smartcard_data_reply_data_st);
				ResourceID = smartcard_data_reply_data.wResourceId;
				HxLOG_Print("resourceID : 0x%X\n", (HUINT32)smartcard_data_reply_data.wResourceId);
				HxLOG_Print("bSerialNoLength : 0x%X\n", smartcard_data_reply_data.bSerialNoLength);
				HxLOG_Print("bCheckDigit : 0x%X\n", smartcard_data_reply_data.bCheckDigit);
				HxLOG_Print("wSmartcardType : 0x%X\n", smartcard_data_reply_data.wSmartcardType);
				HxLOG_Print("bMajorVersion : 0x%X\n", smartcard_data_reply_data.bMajorVersion);
				HxLOG_Print("bMinorVersion : 0x%X\n", smartcard_data_reply_data.bMinorVersion);
				HxLOG_Print("bBuild : 0x%X\n", smartcard_data_reply_data.bBuild);
				HxLOG_Print("bVariant : 0x%X\n", smartcard_data_reply_data.bVariant);
				HxLOG_Print("wPatchLevel : 0x%X\n", smartcard_data_reply_data.wPatchLevel);
				HxLOG_Print("wOwnerId : 0x%X\n", smartcard_data_reply_data.wOwnerId);
				for( i = 0; i < smartcard_data_reply_data.bSerialNoLength; i++ )
				{
					HxLOG_Print("%02X ", pucS3MsgPayload[i]);
				}
				HxLOG_Print("\n");

				if (ResourceID >= HUMAX_VD_SC_SLOT_COUNT)
				{
					break;
				}

				g_IrDrvScInfo[ResourceID].stIrScApiInfo.usType = smartcard_data_reply_data.wSmartcardType;

				snprintf( (char*)g_IrDrvScInfo[ResourceID].stIrScApiInfo.szVersion, IR_SC_INFO_VERSION_LENGTH, (char *)"%d.%d", smartcard_data_reply_data.bMajorVersion, smartcard_data_reply_data.bMinorVersion);

				g_IrDrvScInfo[ResourceID].stIrScApiInfo.ucBuild = smartcard_data_reply_data.bBuild;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.ucVariant = smartcard_data_reply_data.bVariant;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.usPatchLevel = smartcard_data_reply_data.wPatchLevel;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.usOwnerID = smartcard_data_reply_data.wOwnerId;
				HxSTD_memcpy(g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber, pucS3MsgPayload, smartcard_data_reply_data.bSerialNoLength);

				g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber[smartcard_data_reply_data.bSerialNoLength-1] = ' ';
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber[smartcard_data_reply_data.bSerialNoLength] = '(';
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber[smartcard_data_reply_data.bSerialNoLength+1] = smartcard_data_reply_data.bCheckDigit;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber[smartcard_data_reply_data.bSerialNoLength+2] = ')';

				g_IrDrvScInfo[ResourceID].stIrScApiInfo.ucValid = 1;
				HxLOG_Print("Number : %s\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNumber);
				HxLOG_Print("Type : %X\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.usType);
				HxLOG_Print("Version : %s\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.szVersion);
				HxLOG_Print("Build : %02d\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.ucBuild);
				HxLOG_Print("Variant : %02X\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.ucVariant);
				HxLOG_Print("Patch Level : %04X\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.usPatchLevel);
				HxLOG_Print("Owner ID : 0x%X\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.usOwnerID);
				HxLOG_Print("Nationality : %s\n", g_IrDrvScInfo[ResourceID].stIrScApiInfo.szNationality);

#if defined (CONFIG_OP_ALMAJD)
				CAS_IR_DRV_QueryScNationality (0); // Al Majd ¥¬ nationality ¿¸ø° smart card number ∏¶ æÀæ∆æﬂ «œ¥¬ ¿Ã¿Ø∞° ¿÷¥Ÿ.
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
				CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_SMARTCARD_NUMBER);
#endif
#endif

				break;

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
		case	MSG_SMARTCARD_HOMING_CH_REPLY:		// 9.10
			{
				HUINT8 								ucTemp;
				HUINT8 								ucVal;
				HUINT32 							dwTemp;
				IR_SC_HOME_CHANNEL_INFO 			deli;
				msg_smartcard_homing_reply_data_st	homing_ch_reply;
#if defined (CONFIG_OP_ALMAJD)
				HUINT8								ucSlot;
#endif

				HxSTD_memcpy((HUINT8 *)&homing_ch_reply,pucS3MsgPayload, sizeof(msg_smartcard_homing_reply_data_st));
				ResourceID = homing_ch_reply.wResourceId;
				pucS3MsgPayload = homing_ch_reply.abHomingChannel;

#if defined (CONFIG_OP_ALMAJD)
				ucSlot = (pucS3MsgPayload[3] & 0x02) >> 1;
				HxLOG_Print("\t\t(slot = %d)\n", ucSlot);

				CAS_IR_NVR_SaveHomingChannel (ucSlot, &pucS3MsgPayload, IR_PARTION_CUSTOM_HOME_TP0_SIZE);
				break;

#endif

				// frequency(4byte)
				deli.dwFreq =(HUINT32)CAS_IR_DRV_Bcd2Dec(CAS_IR_DRV_Get32Bits(pucS3MsgPayload));
				pucS3MsgPayload += 4;
				HxLOG_Print("\t\t(frequency = %d)\n", deli.dwFreq);

				// orbital position(2byte)
				deli.wOrbitPos =(HUINT16)CAS_IR_DRV_Bcd2Dec(CAS_IR_DRV_Get16Bits(pucS3MsgPayload));
				pucS3MsgPayload+= 2;
				HxLOG_Print("\t\t(orbital position = 0x%x)\n", deli.wOrbitPos);

				// symbol rate (28bits)
				dwTemp = CAS_IR_DRV_Get32Bits(pucS3MsgPayload);
				deli.dwSymbolRate =(HUINT32)CAS_IR_DRV_Bcd2Dec((dwTemp & 0xfffffff0) >> 4);

				pucS3MsgPayload+= 4;
				HxLOG_Print("\t\t(symbol rate = %d)\n", deli.dwSymbolRate);


				ucTemp = *pucS3MsgPayload++;
				// Mux Verifier Mode(4bits)
				deli.eMuxVerifierMode = ((ucTemp & 0xf0) >> 4);
				HxLOG_Print("\t\t(Mux Verifier Mode = 0x%x)\n", deli.eMuxVerifierMode);
				// FEC inner(4bits)
				deli.eFECInner = (ucTemp & 0x0f);
				HxLOG_Print("\t\t(FEC inner = 0x%x)\n", deli.eFECInner);

				ucTemp = *pucS3MsgPayload++;
				// west east flag(1bit)
				ucVal =(ucTemp & 0x80) >> 7;
				deli.eWestEastFlag = ucVal;
				HxLOG_Print("\t\t(west east flag = 0x%x)\n", deli.eWestEastFlag);
				// polarization (2bits)
				deli.ePolar =(ucTemp & 0x60) >> 5;
				HxLOG_Print("\t\t(polarization = 0x%x)\n", deli.ePolar);
				// Roll-Off (2bits)
				deli.eRollOff =(ucTemp & 0x18) >> 3;
				HxLOG_Print("\t\t(roll off = 0x%x)\n", deli.eRollOff);
				// Modulation System (1bit)
				deli.eModulSystem=(ucTemp & 0x04) >> 2;
				HxLOG_Print("\t\t(module system = 0x%x)\n", deli.eModulSystem);
				// Modulation System (2bit)
				deli.eModulType =(ucTemp & 0x03);
				HxLOG_Print("\t\t(module type = 0x%x)\n", deli.eModulType);

		#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.stHomingInfo = deli;
				VK_memset32(&msg, 0, sizeof(HMSG));
				msg.usMsgClass = IREVT_CLASS_HOMING_CH_UPDATE;
				CAS_IR_EVT_SendEvent(0, &msg);
		#endif
			}
				break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case MSG_MATURITY_RATING_CAPABILITY_REPLY:
				HxSTD_memcpy((HUINT8 *)&mr_capability_reply, pucS3MsgPayload, sizeof(msg_mr_capability_reply_st) );
				ResourceID = mr_capability_reply.wResourceId;

				/*it shall be translated flag into humax type */
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.bSCSupportMR = (mr_capability_reply.fMRcapable== IA_TRUE)? 1: 0;

				HxLOG_Print("MSG_MATURITY_RATING_CAPABILITY_REPLY:SC:  %s\n",(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.bSCSupportMR==1)?"SUPPORT": "NOT-SUPPORT" );

				msg.usMsgClass = IREVT_CLASS_MR_SC_CAPABLITY_UPDATE;
				CAS_IR_EVT_SendEvent(0, &msg);

				break;

		case MSG_MATURITY_RATING_CONFIG_REPLY:
				HxSTD_memcpy((HUINT8 *)&mr_config_reply, pucS3MsgPayload, sizeof(msg_mr_config_reply_st) );

				ResourceID = mr_config_reply.wResourceId;
				g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult = mr_config_reply.eResult;

				if(mr_config_reply.eAction== IA_MR_CONFIG_READ)
				{
					if(mr_config_reply.eResult == IR_MR_CONFIG_OK)
					{
						/* adjust source offset */
						pucS3MsgPayload += sizeof(msg_mr_config_reply_st);
						/* read data from next pointer on config_reply structure */
						HxSTD_memcpy((HUINT8 *)&mr_config_data, pucS3MsgPayload, sizeof(msg_mr_config_data_st) );

						g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.bEnabledMREval = (mr_config_data.fEnable==IA_TRUE)?1:0;
						HxLOG_Print("MSG_MATURITY_RATING_CONFIG_REPLY: READ:OK:Enabled=%s\n", \
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.bEnabledMREval==1)?"Yes":"No");
					}
					else
					{
						/*error occured in trying read. */
						HxLOG_Print("MSG_MATURITY_RATING_CONFIG_REPLY: READ:ERROR:%s\n",\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_CARD_ERROR)?"IR_MR_CONFIG_CARD_ERROR":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_CARD_MR_INCAPABLE)?"IR_MR_CONFIG_CARD_MR_INCAPABLE":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_PIN_WRONG)?"IR_MR_CONFIG_PIN_WRONG":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_PIN_BLOCK)?"IR_MR_CONFIG_PIN_BLOCK":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_DISABLE_MR_NOT_ALLOWED)?"IR_MR_CONFIG_DISABLE_MR_NOT_ALLOWED":"Undefined");
					}
				}
				else if(mr_config_reply.eAction== IA_MR_CONFIG_UPDATE)
				{
					if(mr_config_reply.eResult == IR_MR_CONFIG_OK)
					{
						HxLOG_Print("MSG_MATURITY_RATING_CONFIG_REPLY: UPDATE:OK\n");
					}
					else
					{
						/*error occured in trying update. */
						HxLOG_Print("MSG_MATURITY_RATING_CONFIG_REPLY: UPDATE:ERROR:%s\n",\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_CARD_ERROR)?"IR_MR_CONFIG_CARD_ERROR":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_CARD_MR_INCAPABLE)?"IR_MR_CONFIG_CARD_MR_INCAPABLE":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_PIN_WRONG)?"IR_MR_CONFIG_PIN_WRONG":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_PIN_BLOCK)?"IR_MR_CONFIG_PIN_BLOCK":\
							(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.eConfigResult==IR_MR_CONFIG_DISABLE_MR_NOT_ALLOWED)?"IR_MR_CONFIG_DISABLE_MR_NOT_ALLOWED":"Undefined");
					}
				}
				else
				{
					HxLOG_Print("MSG_MATURITY_RATING_CONFIG_REPLY: ERROR:Action is not defined\n");
					break;
				}

				msg.usMsgClass = IREVT_CLASS_MR_CONFIG_UPDATE;
				msg.usParam= mr_config_reply.eResult;

				CAS_IR_EVT_SendEvent(0, &msg);

				break;

		case MSG_MATURITY_RATING_USER_PROFILE_REPLY:
				HxSTD_memcpy((HUINT8 *)&mr_user_profile_reply, (HUINT8*)(msgIR.msg.SOFTCELL.pvMessage), sizeof(msg_mr_user_profile_reply_st) );
				ResourceID = mr_user_profile_reply.wResourceId;

				HxLOG_Print("MSG_MATURITY_RATING_USER_PROFILE_REPLY: eAction= %d, eResult = %s(%d)\n", \
					mr_user_profile_reply.eAction, (mr_user_profile_reply.eResult==MR_USER_PROFILE_OK )?"OK": "ERROR",mr_user_profile_reply.eResult);

				ReplyResult = g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.ucUserProfileResult = mr_user_profile_reply.eResult;

				#if defined(CONFIG_DEBUG)
				/*Print out result of query */
				INT_IR_ParseMRMessage_DBG(&mr_user_profile_reply);
				#endif

				 /* -START-  store user pf info into static value */
				/* Set target pointer once*/
				pstTempUserPF = &(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stMRInfo.ast_mr_user_profile[mr_user_profile_reply.stUserProfile.bProfileID]);

				if(MR_USER_PROFILE_OK == ReplyResult)
				{
					/*when do delete action, the parameters were filled with dummy value, except for profile ID. */
					HxSTD_memset(pstTempUserPF, 0x00, sizeof(msg_mr_user_profile_data_st) );
					pstTempUserPF->bProfileID = mr_user_profile_reply.stUserProfile.bProfileID;
					if(mr_user_profile_reply.eAction !=	IA_MR_USER_PROFILE_DELETE)
					{
						pstTempUserPF->abUserPin[0] = mr_user_profile_reply.stUserProfile.abUserPin[0];
						pstTempUserPF->abUserPin[1] = mr_user_profile_reply.stUserProfile.abUserPin[1];
						pstTempUserPF->bUserAge = mr_user_profile_reply.stUserProfile.bUserAge;
					}

					HxLOG_Print("\t:USER MR INFO: PF_ID=%d, UserPin=%02X%02X, User Age = %d\n", \
						pstTempUserPF->bProfileID, pstTempUserPF->abUserPin[0], pstTempUserPF->abUserPin[1] , pstTempUserPF->bUserAge );
				}
				else if(MR_USER_PROFILE_USER_NOT_EXIST == ReplyResult)
				{
					HxSTD_memset(	pstTempUserPF, 0x00, sizeof(msg_mr_user_profile_data_st) );
					pstTempUserPF->bProfileID	= mr_user_profile_reply.stUserProfile.bProfileID;
				} /* store user pf info into static value - END- */

				/*notify to ext application as each action & result  */
				if(mr_user_profile_reply.eAction == IA_MR_USER_PROFILE_READ)
				{
					/* this condition case is to read whole users profile at once */
					if( (ReplyResult == MR_USER_PROFILE_OK) ||(ReplyResult == MR_USER_PROFILE_USER_NOT_EXIST) )
					{
						/* this condifitoin means Subscriber pin from APP is ok.*/
						if(mr_user_profile_reply.stUserProfile.bProfileID == 0)
						{
							/*from pf id 1*/
							for(i=1 ; i < IRDETO_MR_MAX_NUMBERS_OF_USER; i++)
							{
								msg_mr_user_profile_data_st  stRequestUserPF;

								stRequestUserPF.bProfileID = i;
								/* using backed up subscriber key which used for index 0 */
								XSVC_CAS_IR_DRV_MR_UserProfileQuery(ResourceID, g_IrDrvScInfo[0].stIrScApiInfo.stMRInfo.abSubscriberPin, IA_MR_USER_PROFILE_READ, &stRequestUserPF);
							}
						}
						/*the time when all pf is read and there's no errors. */
						if(mr_user_profile_reply.stUserProfile.bProfileID == (IRDETO_MR_MAX_NUMBERS_OF_USER -1 ) )
						{
							/* All pfs were received well */
							msg.usMsgClass = IREVT_CLASS_MR_PF_STAUS_UPDATE;
							/* send a result value with using  usParam */
							msg.usParam = ReplyResult;
							CAS_IR_EVT_SendEvent(0, &msg);
						}
					}
					else/* this case means showing error occured when trying read all user pf. */
					{
						msg.usMsgClass = IREVT_CLASS_MR_PF_STAUS_UPDATE;
						/* send a result value with using  usParam*/
						msg.usParam = ReplyResult;
						CAS_IR_EVT_SendEvent(0, &msg);
					}
				}
				else if((mr_user_profile_reply.eAction == IA_MR_USER_PROFILE_SET) ||(mr_user_profile_reply.eAction == IA_MR_USER_PROFILE_DELETE) )
				{
					msg.usMsgClass = IREVT_CLASS_MR_PF_STAUS_UPDATE;
					/* send a result value with using  usParam */
					msg.usParam = ReplyResult;
					CAS_IR_EVT_SendEvent(0, &msg);
				}
				else
				{
					HxLOG_Print("MSG_MATURITY_RATING_USER_PROFILE_REPLY: ERROR: Not defined Action type:(%d)\n", mr_user_profile_reply.eAction);
				}

				break;
#endif  // }}} defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)

		case	MSG_PPT_INFO_REPLY:					// 9.12
				break;

		case	MSG_SURF_LOCK_REPLY:				// 9.14
				break;

		case	MSG_REGION_REPLY:					// 9.16
			{
				msg_region_reply_st 	region_reply;
				HUINT16				u16RegionValue;

				HxSTD_memcpy((HUINT8 *)&region_reply, pucS3MsgPayload, sizeof(msg_region_reply_st) );
				HxLOG_Print("wResourceId : %d\n", region_reply.wResourceId);
				HxLOG_Print("bSector : %d\n", region_reply.bSector);
				HxLOG_Print("fSectorValid : %d\n", region_reply.fSectorValid);
				HxLOG_Print("bRegion : 0x%02X\n", region_reply.bRegion);
				HxLOG_Print("bSubRegion : 0x%02X\n", region_reply.bSubRegion);
				u16RegionValue = region_reply.bRegion << 8 | region_reply.bSubRegion;

#if defined(CONFIG_OP_ALMAJD)
				if (CAS_IR_NVR_SavedNumberIsInitial() == TRUE || HUMAX_VdSc_IsSameWithSavedSmartcardNumber() == TRUE)
				{
					CAS_IR_NVR_SaveRegionCode (&u16RegionValue, IR_PARTION_CUSTOM_REGION_CODE_SIZE);

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
					CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_REGION_CODE);
#endif
				}
#endif
			}
				break;

		case	MSG_PIN_CODE_REPLY:					// 9.18
			{
				msg_pin_code_reply_st *pstPinReply;
				pstPinReply = (msg_pin_code_reply_st*)OxCAS_Malloc(sizeof(msg_pin_code_reply_st));
				if (pstPinReply != NULL)
				{
					HxSTD_memcpy((HUINT8 *)pstPinReply, pucS3MsgPayload, sizeof(msg_pin_code_reply_st));
					svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_PINCODEREPLY, MSG_PIN_CODE_REPLY, (HUINT8 *)pstPinReply);
				}
			}
				break;

		case	MSG_REMOTE_MESSAGE_REPLY:			// 9.20
#if defined(CONFIG_MW_CAS_IRDETO_IPPV_CB) && defined(CONFIG_OP_DIGITURK)
			{
				msg_remote_channel_message_st*		pstRemoteChannelMessage;
				pstRemoteChannelMessage = (msg_remote_channel_message_st*)CA_MEM_Alloc(sizeof(msg_remote_channel_message_st));

				if (pstRemoteChannelMessage != NULL)
				{
					HxSTD_memcpy(pstRemoteChannelMessage, pucS3MsgPayload, sizeof(msg_remote_channel_message_st));
					pucS3MsgPayload += sizeof(msg_remote_channel_message_st);

					XSVC_CAS_IR_SetRemoteChannelMessage(pucS3MsgPayload, pstRemoteChannelMessage->bLength);
					svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_RCMSGREPLY, MSG_REMOTE_MESSAGE_REPLY, (HUINT8*)pstRemoteChannelMessage);
				}
			}
#endif
				break;

		case	MSG_SMARTCARD_USER_DATA_REPLY:		// 9.22
			{
				msg_smartcard_user_data_reply_data_st *pstUDReply;
				pstUDReply = (msg_smartcard_user_data_reply_data_st*)OxCAS_Malloc(sizeof(msg_smartcard_user_data_reply_data_st));

				if(pstUDReply == NULL)
					break;

				HxSTD_memset((void*)pstUDReply, 0x00, sizeof(msg_smartcard_user_data_reply_data_st));
				HxSTD_memcpy((void *)pstUDReply, pucS3MsgPayload, ulS3MsgLength);
				ResourceID = pstUDReply->wResourceId;
				if(ulS3MsgLength > sizeof(vd_rsc_id))
					HxSTD_memcpy((void*)g_IrDrvScInfo[ResourceID].stIrScApiInfo.aucUserData, pstUDReply->abUserData, (ulS3MsgLength - sizeof(vd_rsc_id)));

				svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_UDATAREADREPLY, MSG_SMARTCARD_USER_DATA_REPLY, (HUINT8*)pstUDReply);
			}
				break;

		// Chap. 9. Smartcard Message... CAS TASK => SOFTCELL.
		// ¿Ã messageµÈ¿∫ SCELL_Message∏¶ ≈Î«ÿ SOFTCELLø° ¿¸¥ﬁµ«æÓæﬂ «œπ«∑Œ ¿Ã «‘ºˆø°º≠¥¬ √≥∏Æµ«¡ˆ æ ¿Ω.
		case	MSG_NATIONALITY_QUERY:				// 9.4
		case	MSG_SMARTCARD_DATA_QUERY:			// 9.7
		case	MSG_SMARTCARD_HOMING_CH_QUERY:		// 9.9
		case	MSG_PPT_INFO_QUERY:					// 9.11
		case	MSG_SURF_LOCK_QUERY:				// 9.13
		case	MSG_REGION_QUERY:					// 9.15
		case	MSG_PIN_CODE_QUERY:					// 9.17
		case	MSG_REMOTE_MESSAGE_QUERY:			// 9.19
		case	MSG_SMARTCARD_USER_DATA_QUERY:		// 9.21
		default:
				break;

	}
}

/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_Decoder
*	input : CAS_IR_BASE_MSG¡ﬂø° SOFTCELL.
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API - Chap.11. Decoder Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_Decoder(CAS_IR_BASE_MSG msgIR)
{
	msg_decoder_data_st	decoder_data_st;
	HUINT8		*p;
	HUINT16		ServiceHandle;
	HUINT16		wMsgLen;
	HUINT32 	i, j;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		// Chap. 11. Decoder Message... SOFTCELL => CAS TASK
		case	MSG_DECODER:				// 11.2

			// Based on 750410 CCP IRD Messages (Decoder Messages) rev. 1.12.
			p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
			HxSTD_memcpy((HUINT8 *)&decoder_data_st, (HUINT8 *)p, sizeof(msg_decoder_data_st));
			p += sizeof(msg_decoder_data_st);

			ServiceHandle = decoder_data_st.wServiceHandle;	// Open Request«ﬂ¿ª ∂ß πﬁ¿∫ service handle¿Œµ•.. EMM open requestø° ¥Î«— ∞Õ¿Ã∞⁄¡ˆ..?..
			wMsgLen = decoder_data_st.wMessageLength;

			HxLOG_Print("ServiceHandle : 0x%X\n", decoder_data_st.wServiceHandle);
			HxLOG_Print("wMsgLen : %d\n", wMsgLen);

#if 0 // for dumping msg.
			{
				HUINT32		i;

				HxLOG_Print("[MSG_DECODER DATA]\n");
				for( i = 0; i < wMsgLen; i++ )
				{
					HxLOG_Print("%02X ", p[i]);
					if( i % 16 == 15 )
						HxLOG_Print("\n");
				}
				HxLOG_Print("\n");
			}
#endif

			for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
			{
				for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
				{
					if( (g_IrTpInfo[i].usTsUniqueId != INVALID_TS_UNIQUE_ID)
						&& (g_IrTpInfo[i].usSCELL_SvcHandle[j] == ServiceHandle)
						&& (g_IrTpInfo[i].ucIsSCELL_Ready))
					{
						CAS_IR_MSG_DRV_ProcessDecoderDataEMM(g_IrTpInfo[i].hAction, p);
						break;
					}
				}
			}

			if( i == IR_HUMAX_MAX_TP)
			{
				HxLOG_Print("can not find matched s3cell handle with decoder data handle\n");
			}

			break;

		default:
			break;
	}

}


/********************************************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_TMInfo
*	input : CAS_IR_BASE_MSG
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API - Chap.14. TM Info messages
********************************************************************************************/
void CAS_IR_SoftcellMsgProc_TMInfo(CAS_IR_BASE_MSG msgIR)
{
	msg_service_info_data_st	stSvcInfoData;
	msg_service_info_st			stSvcInfo[MAX_TMS_SVC];

	msg_transport_info_data_st	stTSInfoData;
	msg_transport_info_st 		stTSInfo[MAX_TMS_SVC];

	msg_network_info_data_st	stNetInfoData;
	msg_network_info_st			stNetInfo[MAX_TMS_SVC];

	HUINT8 *pucPayLoad = NULL;
	HUINT16 usPayLoadLength=0;

	HUINT8	*p;

	HUINT32	i;

	DbSvc_TsInfo_t		tsInfo;
	DbSvc_Info_t		svcInfo;
//	TS_INFO_T	tsInfo;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case	MSG_SERVICE_INFO_QUERY:

			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_SERVICE_INFO_QUERY===\n");

			/* Fill current service information & initialze struct */
			stSvcInfoData.bServiceCount = MAX_TMS_SVC;
			HxSTD_memset( stSvcInfo, 0, MAX_TMS_SVC * sizeof(msg_service_info_st) );

			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(DB_SVC_GetServiceInfo(g_IrSvcInfo[i].hSvc, &svcInfo) != ERR_OK )
					break;

				if(DB_SVC_GetTsInfoByServiceHandle(g_IrSvcInfo[i].hSvc, &tsInfo) != ERR_OK)
					break;

				stSvcInfo[i].wNetworkId	= tsInfo.usOnId;
				stSvcInfo[i].wTransportId = DbSvc_GetTsId(&tsInfo);
				stSvcInfo[i].wServiceId = DbSvc_GetSvcId(&svcInfo);

				HxLOG_Print("[%d] : ONID= 0x%04X,\tTSID=0x%04X,\tSVCID=0x%04X\n", i, stSvcInfo[i].wNetworkId, stSvcInfo[i].wTransportId, stSvcInfo[i].wServiceId);
			}
			stSvcInfoData.bServiceCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_service_info_data_st) + ( stSvcInfoData.bServiceCount * sizeof(msg_service_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load service info data (service count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stSvcInfoData, sizeof(msg_service_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_service_info_data_st);

			/* Load service info (ONID, TSID, SVCID) */
			for( i=0; i< stSvcInfoData.bServiceCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stSvcInfo[i]), sizeof(msg_service_info_st));
				p += sizeof(msg_service_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_SERVICE_INFO_REPLY\n");
			SCELL_Message(MSG_SERVICE_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

		case	MSG_TRANSPORT_INFO_QUERY:

			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_TRANSPORT_INFO_QUERY===\n");

			/* Fill current Trnasport information & initialze struct  */
			stTSInfoData.bTransportCount= MAX_TMS_SVC;
			HxSTD_memset( stTSInfo, 0, MAX_TMS_SVC * sizeof(msg_transport_info_data_st) );


			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(DB_SVC_GetTsInfoByServiceHandle(g_IrSvcInfo[i].hSvc, &tsInfo) != ERR_OK)
					break;

				stTSInfo[i].wNetworkId	= tsInfo.usOnId;
				stTSInfo[i].wTransportId = tsInfo.usTsId;

				HxLOG_Print("[%d] : ONID= 0x%04X,\tTSID=0x%04X\n", i, stTSInfo[i].wNetworkId, stTSInfo[i].wTransportId);
			}
			stTSInfoData.bTransportCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_transport_info_data_st) + ( stTSInfoData.bTransportCount * sizeof(msg_transport_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load TS info data (TS count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stTSInfoData, sizeof(msg_transport_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_transport_info_data_st);

			/* Load TS info (ONID, TSID ) */
			for( i=0; i< stTSInfoData.bTransportCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stTSInfo[i]), sizeof(msg_transport_info_st));
				p += sizeof(msg_transport_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_TRANSPORT_INFO_REPLY\n");
			SCELL_Message(MSG_TRANSPORT_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

		case	MSG_NETWORK_INFO_QUERY:
			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_NETWORK_INFO_QUERY===\n");

			/* Fill current Network information & initialze struct */
			stNetInfoData.bNetworkCount = MAX_TMS_SVC;
			HxSTD_memset( stNetInfo, 0, MAX_TMS_SVC * sizeof(msg_network_info_data_st) );

			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(DB_SVC_GetTsInfoByServiceHandle(g_IrSvcInfo[i].hSvc, &tsInfo) != ERR_OK)
					break;

				stNetInfo[i].wNetworkId	= tsInfo.usOnId;

				HxLOG_Print("[%d] : ONID= 0x%04X\n", i, stNetInfo[i].wNetworkId);
			}
			stNetInfoData.bNetworkCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_network_info_data_st) + ( stNetInfoData.bNetworkCount * sizeof(msg_network_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load ONID info data (ONID count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stNetInfoData, sizeof(msg_network_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_network_info_data_st);

			/* Load NET info (ONID) */
			for( i=0; i< stNetInfoData.bNetworkCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stNetInfo[i]), sizeof(msg_network_info_st));
				p += sizeof(msg_network_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_NETWORK_INFO_REPLY\n");
			SCELL_Message(MSG_NETWORK_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

#if defined(CONFIG_OP_DIGITURK)
		case 	MSG_SMARTCARD_TMS_DATA_REPLY:
			{
				HUINT8*								p  = NULL;
				msg_smartcard_tms_reply_data_st*	pstTmsReply;
				pstTmsReply = (msg_smartcard_tms_reply_data_st *)CA_MEM_Alloc(sizeof(msg_smartcard_tms_reply_data_st));

				if (pstTmsReply == NULL)
				{
					break;
				}

				p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);

				HxSTD_memset((void *)pstTmsReply, 0x00, sizeof(msg_smartcard_tms_reply_data_st));
				HxSTD_memcpy((void *)pstTmsReply, p, sizeof(msg_smartcard_tms_reply_data_st));

				if (pstTmsReply->wResourceId < HUMAX_VD_SC_SLOT_COUNT )
				{
					HxSTD_memset((void *)g_IrDrvScInfo[pstTmsReply->wResourceId].aucTMSUserData, 0x00,  sizeof(HUINT8) * TMS_DATA_LENGTH);
					HxSTD_memcpy((void *)g_IrDrvScInfo[pstTmsReply->wResourceId].aucTMSUserData, pstTmsReply->abTMSUserData, sizeof(HUINT8) * TMS_DATA_LENGTH);
					svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_UDATAREADREPLY, MSG_SMARTCARD_TMS_DATA_REPLY, (HUINT8*)pstTmsReply);
				}
			}
			break;
#endif

	}

}

void CAS_IR_SoftcellMsgProc_Product(CAS_IR_BASE_MSG msgIR)
{
	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case	MSG_ANY_PRODUCT_REPLY:
			CAS_IR_DRV_ProductReply(msgIR);
			break;

#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
		case MSG_EXTENDED_PRODUCT_LIST_REPLY:
			CAS_IR_DRV_ExtendedProductListReply(msgIR);
			break;
#endif
	}
}


void CAS_IR_DRV_SetSCProductExistance(HUINT8	 ucSlotId, ia_result bExist)
{
	if(ucSlotId >= CAS_MAX_NUM_OF_SC)
		return;

	HxLOG_Print(" Any Product was %d in slot %d\n", bExist, ucSlotId);
	g_IrDrvScInfo[ucSlotId].bProductInit = IA_SUCCESS;
	g_IrDrvScInfo[ucSlotId].bProductExistance = bExist;

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS);
#endif

}


void CAS_IR_DRV_ProductReply(CAS_IR_BASE_MSG msgIR)
{
	HUINT8	*p;
	msg_any_product_reply_st *pstAnyProductReply;
#if defined(CONFIG_PROD_UDFOXHD)
	HUINT8		i;
	HUINT16		*pusProducts = NULL;
	msg_any_product_reply_sector_st	*pstAnyProductReplySector;
#endif

	p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
	pstAnyProductReply = (msg_any_product_reply_st *) p;

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	/* Caller's mother is CAS_BASE_TASK */
	(void)CAS_IR_SOL_update_SC_Status( SOL_SC_UPDATE_PRODUCT_STATUS, (pstAnyProductReply->bSectorCount != 0 )? TRUE:FALSE );
#endif
	CAS_IR_DRV_SetSCProductExistance(pstAnyProductReply->wResourceId, (pstAnyProductReply->bSectorCount != 0 )? IA_SUCCESS : IA_FAIL );

#if defined(CONFIG_PROD_UDFOXHD)
	pstAnyProductReplySector = NULL;

	if(pstAnyProductReply->bSectorCount > 0)
	{
		HUINT32	ulSize;

		ulSize = (pstAnyProductReply->bSectorCount+1) * sizeof(HUINT16);
		pusProducts = (HUINT16*)CA_MEM_Alloc(ulSize);
		if(pusProducts != NULL)
		{
			p += sizeof(msg_any_product_reply_st);
			pstAnyProductReplySector = (msg_any_product_reply_sector_st *)p;

			for(i = 0; i < pstAnyProductReply->bSectorCount; i++)
			{
				HxLOG_Info("bSector: %d, fSectorValid: %d, fProductValid: %d, wProduct: 0x%04x\n", \
										pstAnyProductReplySector[i].bSector, \
										pstAnyProductReplySector[i].fSectorValid, \
										pstAnyProductReplySector[i].fProductValid, \
										pstAnyProductReplySector[i].wProduct );

				if((pstAnyProductReplySector[i].fSectorValid == IA_TRUE) && (pstAnyProductReplySector[i].fProductValid == IA_TRUE))
				{
					pusProducts[i] = pstAnyProductReplySector[i].wProduct;
				}
				else
				{
					pusProducts[i] = 0;
				}
			}
			pusProducts[i] = 0xFFFF;
}

	}
	svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_ANYPRODUCTREPLY, MSG_ANY_PRODUCT_REPLY, (HUINT8 *)pusProducts);
#endif
}


#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
void CAS_IR_DRV_ExtendedProductListReply(CAS_IR_BASE_MSG msgIR)
{
	HUINT8	*p;
	msg_extended_product_list_reply_st *pstAnyProductReply;

	p = (HUINT8 *)(msgIR.msg.SOFTCELL.pvMessage);
	pstAnyProductReply = (msg_extended_product_list_reply_st *) p;

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	/* Caller's mother is CAS_BASE_TASK */
	(void)CAS_IR_SOL_update_SC_Status( SOL_SC_UPDATE_PRODUCT_STATUS, (pstAnyProductReply->bSectorCount != 0 )? TRUE:FALSE );
#endif
	CAS_IR_DRV_SetSCProductExistance(pstAnyProductReply->wResourceId, (pstAnyProductReply->bSectorCount != 0 )? IA_SUCCESS : IA_FAIL );

	if(pstAnyProductReply->bSectorCount > 0)
	{
		CAS_IR_DRV_SetSCExtendedProductList(pstAnyProductReply);

		svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_EXTENDEDPRODUCTLISTREPLY, MSG_EXTENDED_PRODUCT_LIST_REPLY, 0);
	}


}

void CAS_IR_DRV_SetSCExtendedProductList(msg_extended_product_list_reply_st *pstAnyProductReply)
{

	HUINT32							i, j;
	vd_rsc_id							ResourceID;
	msg_extended_product_list_reply_st		extended_product_list_reply_data;
	HUINT8							*pucS3MsgPayload;

	if (pstAnyProductReply == NULL)
	{
		return;
	}

	pucS3MsgPayload = pstAnyProductReply;
	HxSTD_memcpy((HUINT8 *)&extended_product_list_reply_data, pstAnyProductReply, sizeof(msg_extended_product_list_reply_st));



	if(extended_product_list_reply_data.bSectorCount != 0)
	{
		pucS3MsgPayload += sizeof(msg_extended_product_list_reply_st);
		ResourceID = extended_product_list_reply_data.wResourceId;

		if (ResourceID >= HUMAX_VD_SC_SLOT_COUNT)
		{
			return;
		}

		HxLogInfo("resourceID : 0x%X\n", (HUINT32) ResourceID);
		HxLogInfo("bSectors : 0x%X\n", extended_product_list_reply_data.bSectorCount);

		// TODO:  µø¿œ«ÿæﬂ «‘.
		// g_IrDrvScInfo[ResourceID].bSectors == extended_product_list_reply_data.bSectorCount

		HxSTD_memset(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData, 0x00, sizeof(IR_EXTENDED_PRODUCT_LIST_INFO)*MAX_SC_SECTOR_COUNT);

		for(i = 0; i < extended_product_list_reply_data.bSectorCount; i++)
		{
			HxSTD_memcpy(&(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].stExtProductListSector), (HUINT8 *)pucS3MsgPayload, sizeof(msg_extended_product_list_sector_st));
			pucS3MsgPayload += sizeof(msg_extended_product_list_sector_st);

			if (g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct)
			{
				CA_MEM_Free(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct);
			}
			g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct = CA_MEM_Alloc(sizeof(msg_extended_product_list_product_st) * g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bProductCount);


			for(j = 0; j < g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bProductCount; j++)
			{
				HxSTD_memcpy(&(g_IrDrvScInfo[ResourceID].stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct[j]), (HUINT8 *)pucS3MsgPayload, sizeof(msg_extended_product_list_product_st));
				pucS3MsgPayload += sizeof(msg_extended_product_list_product_st);
			}


			if( i > MAX_SC_SECTOR_COUNT )
			{
				break;
			}


		}


	}



}


#endif


// TODO: must be implemented. !!!. jichoi.
#if 0
/********************************************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_TMInfo
*	input : CAS_IR_BASE_MSG
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API - Chap.14. TM Info messages
********************************************************************************************/
void CAS_IR_SoftcellMsgProc_TMInfo(CAS_IR_BASE_MSG msgIR)
{
	msg_service_info_data_st	stSvcInfoData;
	msg_service_info_st			stSvcInfo[MAX_TMS_SVC];

	msg_transport_info_data_st	stTSInfoData;
	msg_transport_info_st 		stTSInfo[MAX_TMS_SVC];

	msg_network_info_data_st	stNetInfoData;
	msg_network_info_st			stNetInfo[MAX_TMS_SVC];

	HUINT8 *pucPayLoad = NULL;
	HUINT16 usPayLoadLength=0;

	HUINT8	*pServiceInfoReply = NULL;
	HUINT8	*p;

	HUINT32	i;

	TS_INFO_T	tsInfo;
	HUINT16 	wSvcId;
	Handle_t	hSvc;

	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case	MSG_SERVICE_INFO_QUERY:

			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_SERVICE_INFO_QUERY===\n");

			/* Fill current service information & initialze struct */
			stSvcInfoData.bServiceCount = MAX_TMS_SVC;
			HxSTD_memset( stSvcInfo, 0, MAX_TMS_SVC * sizeof(msg_service_info_st) );

			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(MXI_SM_GetServiceHandleByPlayHandle(g_IrSvcInfo[i].hIrSvc, &hSvc) != ERR_OK )
					break;

				if(SLM_GetTransportStreamInfo(hSvc, &tsInfo) != RESULT_OK)
					break;

				if(SLM_GetServiceId(hSvc, &wSvcId ) != RESULT_OK )
					break;

				stSvcInfo[i].wNetworkId	= tsInfo.orgNetId;
				stSvcInfo[i].wTransportId = tsInfo.tsId;
				stSvcInfo[i].wServiceId = wSvcId;

				HxLOG_Print("[%d] : ONID= 0x%04X,\tTSID=0x%04X,\tSVCID=0x%04X\n", i, stSvcInfo[i].wNetworkId, stSvcInfo[i].wTransportId, stSvcInfo[i].wServiceId);
			}
			stSvcInfoData.bServiceCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_service_info_data_st) + ( stSvcInfoData.bServiceCount * sizeof(msg_service_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load service info data (service count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stSvcInfoData, sizeof(msg_service_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_service_info_data_st);

			/* Load service info (ONID, TSID, SVCID) */
			for( i=0; i< stSvcInfoData.bServiceCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stSvcInfo[i]), sizeof(msg_service_info_st));
				p += sizeof(msg_service_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_SERVICE_INFO_REPLY\n");
			SCELL_Message(MSG_SERVICE_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

		case	MSG_TRANSPORT_INFO_QUERY:

			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_TRANSPORT_INFO_QUERY===\n");

			/* Fill current Trnasport information & initialze struct  */
			stTSInfoData.bTransportCount= MAX_TMS_SVC;
			HxSTD_memset( stTSInfo, 0, MAX_TMS_SVC * sizeof(msg_transport_info_data_st) );

			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(MXI_SM_GetServiceHandleByPlayHandle(g_IrSvcInfo[i].hIrSvc, &hSvc) != ERR_OK )
					break;

				if(SLM_GetTransportStreamInfo(hSvc, &tsInfo) != RESULT_OK)
					break;

				stTSInfo[i].wNetworkId	= tsInfo.orgNetId;
				stTSInfo[i].wTransportId = tsInfo.tsId;

				HxLOG_Print("[%d] : ONID= 0x%04X,\tTSID=0x%04X\n", i, stTSInfo[i].wNetworkId, stTSInfo[i].wTransportId);
			}
			stTSInfoData.bTransportCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_transport_info_data_st) + ( stTSInfoData.bTransportCount * sizeof(msg_transport_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load TS info data (TS count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stTSInfoData, sizeof(msg_transport_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_transport_info_data_st);

			/* Load TS info (ONID, TSID ) */
			for( i=0; i< stTSInfoData.bTransportCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stTSInfo[i]), sizeof(msg_transport_info_st));
				p += sizeof(msg_transport_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_TRANSPORT_INFO_REPLY\n");
			SCELL_Message(MSG_TRANSPORT_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

		case	MSG_NETWORK_INFO_QUERY:
			HxLOG_Print("===CAS_IR_SoftcellMsgProc_TMInfo : MSG_NETWORK_INFO_QUERY===\n");

			/* Fill current Network information & initialze struct */
			stNetInfoData.bNetworkCount = MAX_TMS_SVC;
			HxSTD_memset( stNetInfo, 0, MAX_TMS_SVC * sizeof(msg_network_info_data_st) );

			for(i=0; i< MAX_TMS_SVC ; i++ )
			{
				if(g_IrSvcInfo[i].hIrSvc == HANDLE_NULL)
					break;

				if(MXI_SM_GetServiceHandleByPlayHandle(g_IrSvcInfo[i].hIrSvc, &hSvc) != ERR_OK )
					break;

				if(SLM_GetTransportStreamInfo(hSvc, &tsInfo) != RESULT_OK)
					break;

				stNetInfo[i].wNetworkId	= tsInfo.orgNetId;

				HxLOG_Print("[%d] : ONID= 0x%04X,\tTSID=0x%04X\n", i, stNetInfo[i].wNetworkId);
			}
			stNetInfoData.bNetworkCount = (HUINT8)i;
			usPayLoadLength = sizeof(msg_network_info_data_st) + ( stNetInfoData.bNetworkCount * sizeof(msg_network_info_st)) ;

			pucPayLoad = (HUINT8 *)OxCAS_Malloc(usPayLoadLength);
			if(NULL == pucPayLoad)
			{
				HxLOG_Print("CAS_IR_SoftcellMsgProc_TMInfo: mem alloc error \n");
				break;
			}

			/* Load ONID info data (ONID count) */
			HxSTD_memcpy(pucPayLoad, (HUINT8 *)&stNetInfoData, sizeof(msg_network_info_data_st));

			p = pucPayLoad;
			p += sizeof(msg_network_info_data_st);

			/* Load NET info (ONID) */
			for( i=0; i< stNetInfoData.bNetworkCount ;i++)
			{
				HxSTD_memcpy(p, (HUINT8 *)&(stNetInfo[i]), sizeof(msg_network_info_st));
				p += sizeof(msg_network_info_st);
			}

			HxLOG_Print("[SCELL_Message] MSG_NETWORK_INFO_REPLY\n");
			SCELL_Message(MSG_NETWORK_INFO_REPLY, 0, usPayLoadLength, (void *)pucPayLoad);

			break;

	}

}
#endif

#define _____DEBUG_API_________________________________________________________________

void local_cas_ir_CA_DRV_DBG_PrintOpCode(HUINT32 eOpCode)
{
	HxLOG_Debug("\n________________________________________________________\n");
	switch(eOpCode)
	{
	case	MSG_CLIENT_VERSION_QUERY:		HxLOG_Debug("[S3->CA] MSG_CLIENT_VERSION_QUERY\n");	break;
	case	MSG_COMPONENTS_VERSION_QUERY:	HxLOG_Debug("[S3->CA] MSG_COMPONENTS_VERSION_QUERY\n");	break;
	case	MSG_VERSION_REPLY:				HxLOG_Debug("[S3->CA] MSG_VERSION_REPLY\n");	break;
	case	MSG_ERROR:						HxLOG_Debug("[S3->CA] MSG_ERROR\n");	break;
	case	MSG_SERVICE_OPEN_REQUEST:		HxLOG_Debug("[S3->CA] MSG_SERVICE_OPEN_REQUEST\n");	break;
	case	MSG_SERVICE_OPEN_REPLY:			HxLOG_Debug("[S3->CA] MSG_SERVICE_OPEN_REPLY\n");	break;
	case	MSG_SERVICE_CLOSE:				HxLOG_Debug("[S3->CA] MSG_SERVICE_CLOSE\n");	break;
	case	MSG_SERVICE_CLOSE_REPLY:				HxLOG_Debug("[S3->CA] MSG_SERVICE_CLOSE_REPLY\n");	break;
	case	MSG_SMARTCARD_MULTICARD_STATE_NOTIFY:				HxLOG_Debug("[S3->CA] MSG_SMARTCARD_MULTICARD_STATE_NOTIFY\n");	break;
	case	MSG_SERVICE_DEFINE:				HxLOG_Debug("[S3->CA] MSG_SERVICE_DEFINE\n");	break;
	case	MSG_SERVICE_STATUS_REQUEST:		HxLOG_Debug("[S3->CA] MSG_SERVICE_STATUS_REQUEST\n");	break;
	case	MSG_SERVICE_STATUS:				HxLOG_Debug("[S3->CA] MSG_SERVICE_STATUS\n");	break;
	case	MSG_SMARTCARD_STATUS:			HxLOG_Debug("[S3->CA] MSG_SMARTCARD_STATUS\n");	break;
	case	MSG_NATIONALITY_QUERY:			HxLOG_Debug("[S3->CA] MSG_NATIONALITY_QUERY\n");	break;
	case	MSG_NATIONALITY:				HxLOG_Debug("[S3->CA] MSG_NATIONALITY\n");	break;
	case	MSG_SMARTCARD_ID:				HxLOG_Debug("[S3->CA] MSG_SMARTCARD_ID\n");	break;
	case	MSG_SMARTCARD_DATA_QUERY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_DATA_QUERY\n");	break;
	case	MSG_SMARTCARD_DATA_REPLY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_DATA_REPLY\n");	break;
	case	MSG_SMARTCARD_HOMING_CH_QUERY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_HOMING_CH_QUERY\n");	break;
	case	MSG_SMARTCARD_HOMING_CH_REPLY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_HOMING_CH_REPLY\n");	break;
	case	MSG_DECODER:					HxLOG_Debug("[S3->CA] MSG_DECODER\n");	break;
	case	MSG_DVB_EMM_MONITOR:			HxLOG_Debug("[S3->CA] MSG_DVB_EMM_MONITOR\n");	break;
	case	MSG_DVB_ECM_MONITOR:			HxLOG_Debug("[S3->CA] MSG_DVB_ECM_MONITOR\n");	break;
	case	MSG_IPTV_EMM_MONITOR:			HxLOG_Debug("[S3->CA] MSG_IPTV_EMM_MONITOR\n");	break;
	case	MSG_IPTV_REGISTRATION_ATTEMPTS:	HxLOG_Debug("[S3->CA] MSG_IPTV_REGISTRATION_ATTEMPTS\n");	break;
	case	MSG_SERVICE_INFO_QUERY:			HxLOG_Debug("[S3->CA] MSG_SERVICE_INFO_QUERY\n");	break;
	case	MSG_SERVICE_INFO_REPLY:			HxLOG_Debug("[S3->CA] MSG_SERVICE_INFO_REPLY\n");	break;
	case	MSG_TRANSPORT_INFO_QUERY:		HxLOG_Debug("[S3->CA] MSG_TRANSPORT_INFO_QUERY\n");	break;
	case	MSG_TRANSPORT_INFO_REPLY:		HxLOG_Debug("[S3->CA] MSG_TRANSPORT_INFO_REPLY\n");	break;
	case	MSG_NETWORK_INFO_QUERY:			HxLOG_Debug("[S3->CA] MSG_NETWORK_INFO_QUERY\n");	break;
	case	MSG_NETWORK_INFO_REPLY:			HxLOG_Debug("[S3->CA] MSG_NETWORK_INFO_REPLY\n");	break;
	case	MSG_IPPV_EVENT_NOTIFY:			HxLOG_Debug("[S3->CA] MSG_IPPV_EVENT_NOTIFY\n");	break;
	case	MSG_IPPV_BUY_QUERY:				HxLOG_Debug("[S3->CA] MSG_IPPV_BUY_QUERY\n");	break;
	case	MSG_IPPV_BUY_REPLY:				HxLOG_Debug("[S3->CA] MSG_IPPV_BUY_REPLY\n");	break;
	case	MSG_IPPV_SECTOR_INFORMATION_QUERY:	HxLOG_Debug("[S3->CA] MSG_IPPV_SECTOR_INFORMATION_QUERY\n");	break;
	case	MSG_IPPV_SECTOR_INFORMATION_REPLY:	HxLOG_Debug("[S3->CA] MSG_IPPV_SECTOR_INFORMATION_REPLY\n");	break;
	case	MSG_IPPV_SET_DEBIT_THRESHOLD:	HxLOG_Debug("[S3->CA] MSG_IPPV_SET_DEBIT_THRESHOLD\n");	break;
	case	MSG_IPPV_CALLBACK_REQUEST:		HxLOG_Debug("[S3->CA] MSG_IPPV_CALLBACK_REQUEST\n");	break;
	case	MSG_IPPV_EVENT_LIST_QUERY:		HxLOG_Debug("[S3->CA] MSG_IPPV_EVENT_LIST_QUERY\n");	break;
	case	MSG_IPPV_EVENT_LIST_REPLY:		HxLOG_Debug("[S3->CA] MSG_IPPV_EVENT_LIST_REPLY\n");	break;
	case	MSG_IPPV_EVENT_CHECK_QUERY:		HxLOG_Debug("[S3->CA] MSG_IPPV_EVENT_CHECK_QUERY\n");	break;
	case	MSG_IPPV_EVENT_CHECK_REPLY:		HxLOG_Debug("[S3->CA] MSG_IPPV_EVENT_CHECK_REPLY\n");	break;
	case	MSG_PIN_CODE_QUERY:				HxLOG_Debug("[S3->CA] MSG_PIN_CODE_QUERY,\n");	break;
	case	MSG_PIN_CODE_REPLY:				HxLOG_Debug("[S3->CA] MSG_PIN_CODE_REPLY\n");	break;
	case	MSG_NUMBER_OF_SECTORS_NOTIFY:	HxLOG_Debug("[S3->CA] MSG_NUMBER_OF_SECTORS_NOTIFY\n");	break;
	case	MSG_REMOTE_MESSAGE_QUERY:		HxLOG_Debug("[S3->CA] MSG_REMOTE_MESSAGE_QUERY,\n");	break;
	case	MSG_REMOTE_MESSAGE_REPLY:		HxLOG_Debug("[S3->CA] MSG_REMOTE_MESSAGE_REPLY\n");	break;
	case	MSG_SMARTCARD_USER_DATA_QUERY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_USER_DATA_QUERY\n");	break;
	case	MSG_SMARTCARD_USER_DATA_REPLY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_USER_DATA_REPLY\n");	break;
	case	MSG_RFU1:						HxLOG_Debug("[S3->CA] MSG_RFU1\n");	break;
	case	MSG_REGISTRATION_USE_NEW_HEADEND:	HxLOG_Debug("[S3->CA] MSG_REGISTRATION_USE_NEW_HEADEND\n");	break;
	case	MSG_SMARTCARD_RSA_SIGN_REQUEST:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_SIGN_REQUEST\n");	break;
	case	MSG_SMARTCARD_RSA_SIGN_REPLY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_SIGN_REPLY\n");	break;
	case	MSG_SMARTCARD_RSA_VERIFY_REQUEST:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_VERIFY_REQUEST\n");	break;
	case	MSG_SMARTCARD_RSA_VERIFY_REPLY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_VERIFY_REPLY\n");	break;
	case	MSG_SMARTCARD_RSA_ENCRYPT_REQUEST:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_ENCRYPT_REQUEST\n");	break;
	case	MSG_SMARTCARD_RSA_ENCRYPT_REPLY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_ENCRYPT_REPLY\n");	break;
	case	MSG_SMARTCARD_RSA_DECRYPT_REQUEST:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_DECRYPT_REQUEST\n");	break;
	case	MSG_SMARTCARD_RSA_DECRYPT_REPLY:	HxLOG_Debug("[S3->CA] MSG_SMARTCARD_RSA_DECRYPT_REPLY\n");	break;
	case	MSG_PRODUCT_CHECK_QUERY:		HxLOG_Debug("[S3->CA] MSG_PRODUCT_CHECK_QUERY\n");	break;
	case	MSG_PRODUCT_CHECK_REPLY:		HxLOG_Debug("[S3->CA] MSG_PRODUCT_CHECK_REPLY\n");	break;
	case	MSG_ANY_PRODUCT_QUERY:			HxLOG_Debug("[S3->CA] MSG_ANY_PRODUCT_QUERY\n");	break;
	case	MSG_ANY_PRODUCT_REPLY:			HxLOG_Debug("[S3->CA] MSG_ANY_PRODUCT_REPLY\n");	break;
	case	MSG_PRODUCT_LIST_QUERY:			HxLOG_Debug("[S3->CA] MSG_PRODUCT_LIST_QUERY\n");	break;
	case	MSG_PRODUCT_LIST_REPLY:			HxLOG_Debug("[S3->CA] MSG_PRODUCT_LIST_REPLY\n");	break;
	case	MSG_CLIENT_ID_QUERY:			HxLOG_Debug("[S3->CA] MSG_CLIENT_ID_QUERY\n");	break;
	case	MSG_CLIENT_ID_REPLY:			HxLOG_Debug("[S3->CA] MSG_CLIENT_ID_REPLY\n");	break;
	case	MSG_CC_SERVICE_ADD:				HxLOG_Debug("[S3->CA] MSG_CC_SERVICE_ADD\n");	break;
	case	MSG_CC_SERVICE_DELETE:			HxLOG_Debug("[S3->CA] MSG_CC_SERVICE_DELETE\n");	break;
	case	MSG_CC_SERVICE_UPDATE:			HxLOG_Debug("[S3->CA] MSG_CC_SERVICE_UPDATE\n");	break;
	case	MSG_EXTENDED_PRODUCT_LIST_QUERY:	HxLOG_Debug("[S3->CA] MSG_EXTENDED_PRODUCT_LIST_QUERY\n");	break;
	case	MSG_EXTENDED_PRODUCT_LIST_REPLY:	HxLOG_Debug("[S3->CA] MSG_EXTENDED_PRODUCT_LIST_REPLY\n");	break;
	case	MSG_PPT_INFO_QUERY:				HxLOG_Debug("[S3->CA] MSG_PPT_INFO_QUERY\n");	break;
	case	MSG_PPT_INFO_REPLY:				HxLOG_Debug("[S3->CA] MSG_PPT_INFO_REPLY\n");	break;
	case	MSG_SURF_LOCK_QUERY:			HxLOG_Debug("[S3->CA] MSG_SURF_LOCK_QUERY\n");	break;
	case	MSG_SURF_LOCK_REPLY:			HxLOG_Debug("[S3->CA] MSG_SURF_LOCK_REPLY\n");	break;
	case	MSG_REGION_QUERY:				HxLOG_Debug("[S3->CA] MSG_REGION_QUERY\n");	break;
	case	MSG_REGION_REPLY:				HxLOG_Debug("[S3->CA] MSG_REGION_REPLY\n");	break;
	case	MSG_EXPIRED_PRODUCT_QUERY:		HxLOG_Debug("[S3->CA] MSG_EXPIRED_PRODUCT_QUERY\n");	break;
	case	MSG_EXPIRED_PRODUCT_REPLY:		HxLOG_Debug("[S3->CA] MSG_EXPIRED_PRODUCT_REPLY\n");	break;
	case	MSG_SMARTCARD_VENDOR_ID_QUERY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_VENDOR_ID_QUERY\n");	break;
	case	MSG_SMARTCARD_VENDOR_ID_REPLY:		HxLOG_Debug("[S3->CA] MSG_SMARTCARD_VENDOR_ID_REPLY\n");	break;
	case	MSG_MATURITY_RATING_CAPABILITY_QUERY:	HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_CAPABILITY_QUERY\n");	break;
	case	MSG_MATURITY_RATING_CAPABILITY_REPLY:	HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_CAPABILITY_REPLY\n");	break;
	case	MSG_MATURITY_RATING_CONFIG_QUERY:		HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_CONFIG_QUERY\n");	break;
	case	MSG_MATURITY_RATING_CONFIG_REPLY:		HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_CONFIG_REPLY\n");	break;
	case	MSG_MATURITY_RATING_USER_PROFILE_QUERY:		HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_USER_PROFILE_QUERY\n");	break;
	case	MSG_MATURITY_RATING_USER_PROFILE_REPLY:		HxLOG_Debug("[S3->CA] MSG_MATURITY_RATING_USER_PROFILE_REPLY\n");	break;


	default:
		HxLOG_Debug("[S3->CA] UNKNOWN OPCODE[%d]\n", eOpCode);
		break;
	}
	HxLOG_Debug("________________________________________________________\n\n");
}


