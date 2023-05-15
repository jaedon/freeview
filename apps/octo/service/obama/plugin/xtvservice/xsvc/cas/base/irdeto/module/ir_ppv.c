/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_ppv.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_svc.h"
#include "ir_mgr.h"

#include "vd_humax.h"
#include <s3_ippv_msg.h>
#include <s3_lppv_msg.h>
#include <s3_event_if_msg.h>
#include <s3_service.h>

#include <pal_pipe.h>

#include "ir_msg.h"

#include "s3_cadrv.h"
#include "s3_status.h"
#include "s3_smartcard.h"
#include "s3_service.h"
#include "s3_cadrv.h"
#include "s3_decoder_msg.h"
#include "s3_tm_msg.h"
#include "s3_prod_if_msg.h"

#include "ir_api.h"
#include "ir_nvr.h"
#include "ir_ppv.h"

#include "util.h"

#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_api.h>
/********************************************************************
*	Definitions (for this source file)
********************************************************************/






//#define	IR_DEBUG_SI
#ifdef	IR_DEBUG_SI
#define	PrintPMT	HxLOG_Print
#define	PrintCAT	HxLOG_Print
#else
#define	PrintPMT(...)
#define	PrintCAT(...)
#endif

//---------------------------- Struct Define ----------------------------
//used for both IPPV & LPPV
/********************************************************************
*	Type Definition
********************************************************************/

/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
extern void		CAS_IR_CATASK_Init(void);

//extern HERROR	HUMAX_VD_DEMUX_AllocDemuxInfo(Handle_t pipeHandle);


/********************************************************************
*	Global variables
********************************************************************/
extern IR_SERVICE_INFO	g_IrDrvSvcInfo[IR_HUMAX_MAX_SERVICE];
extern IR_TP_INFO 		g_IrDrvTpInfo[IR_HUMAX_MAX_TP];
extern IR_DRV_SC_INFO	g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];


/********************************************************************
*	Static variables
********************************************************************/
//static	HUINT8	*s_pucCurrentIPPVEvent = NULL;
//static	HUINT8	*s_pucCurrentLPPVEvent = NULL;

/********************************************************************
*	Static functions
********************************************************************/

/********************************************************************
*	Global functions
********************************************************************/
HERROR CAS_IR_IPPV_SendPPVReplyToAP(HUINT32 ulIRMsg, HUINT8 *pData);

/********************************************************************
*	function name :	 CAS_IR_IPPV_Event_Notify
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Event_Notify(CAS_IR_SOFTCELL_MSG msg)
{
	msg_ippv_event_notify_st *pNotify = (msg_ippv_event_notify_st*)msg.pvMessage;
	HUINT16 *pSubNotify = (HUINT16*)((HUINT8*)msg.pvMessage + sizeof(msg_ippv_event_notify_st));
	HUINT8 *pDataToSend;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_event_notify_st) + pNotify->wServiceCount * sizeof(ia_word16));
	if(pDataToSend == NULL)
		return;

	HxSTD_memcpy(pDataToSend, pNotify, sizeof(msg_ippv_event_notify_st));
	HxSTD_memcpy(pDataToSend + sizeof(msg_ippv_event_notify_st), pSubNotify, (sizeof(ia_word16)* pNotify->wServiceCount));

	// ewlee 20100902 - for test
	HxLOG_Info("MW IPPV Event Notify: Resource ID(%d), Sector(%d), Event ID(%d), Cost(%d)\n", (int)pNotify->wResourceId,
		(int)pNotify->bSector, (int)pNotify->wEventId, (int)pNotify->wEventCost);

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pDataToSend);
}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Event_Notify
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Buy_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	HUINT8 *pNotify;

	msg_ippv_buy_reply_st *pReply = (msg_ippv_buy_reply_st*)msg.pvMessage;
	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_buy_reply_st));

	if(pNotify)
	{
		HxSTD_memset(pNotify, 0, sizeof(msg_ippv_buy_reply_st));
		HxSTD_memcpy(pNotify, pReply, sizeof(msg_ippv_buy_reply_st));
		CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);
	}

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_IPPV_Buy_Query
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Buy_Query(msg_ippv_buy_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

//[to do] 현재 가용한 Event ID 인지 확인하는 루틴 필요
//	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_read_ticket_query_st));
//	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_read_ticket_query_st));

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_buy_query_st));
	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_buy_query_st));

	irReturn = SCELL_Message(MSG_IPPV_BUY_QUERY, 0, sizeof(msg_ippv_buy_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Sector_Info_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Sector_Info_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_ippv_sector_reply_st *pReply = (msg_ippv_sector_reply_st*)msg.pvMessage;
	HUINT8 *pNotify;

	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_sector_reply_st));
	if(pNotify == NULL)
		return;

	HxSTD_memset(pNotify, 0, sizeof(msg_ippv_sector_reply_st));
	HxSTD_memcpy(pNotify, pReply, sizeof(msg_ippv_sector_reply_st));
	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_IPPV_Sector_Info_Query
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Sector_Info_Query(msg_ippv_sector_query_st *pMsg)
{
	ia_result irResult;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_sector_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_sector_query_st));

	irResult = SCELL_Message(MSG_IPPV_SECTOR_INFORMATION_QUERY, 0, sizeof(msg_ippv_sector_query_st), pDataToSend);

	return irResult;
}

/********************************************************************
*	function name :	 XSVC_CAS_IR_IPPV_Debit_Threshold
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Debit_Threshold(msg_ippv_set_debit_threshold_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_set_debit_threshold_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_set_debit_threshold_query_st));
	irReturn = SCELL_Message(MSG_IPPV_SET_DEBIT_THRESHOLD, 0, sizeof(msg_ippv_set_debit_threshold_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Event_List_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Event_List_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_ippv_event_list_reply_st *pReply = (msg_ippv_event_list_reply_st*)msg.pvMessage;
	msg_ippv_event_list_reply_event_st *pSubReply = (msg_ippv_event_list_reply_event_st*)((HUINT8*)msg.pvMessage + sizeof(msg_ippv_event_list_reply_st));
	HUINT8 *pNotify;
//[to do] OCTO APP에 Reply ?

	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_event_list_reply_st) + sizeof(msg_ippv_event_list_reply_event_st) * pReply->bEventCount);
	if(pNotify == NULL)
		return;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_ippv_event_list_reply_st));
	HxSTD_memcpy(pNotify + sizeof(msg_ippv_event_list_reply_st), pSubReply, sizeof(msg_ippv_event_list_reply_event_st) * pReply->bEventCount);

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);
}


/********************************************************************
*	function name :	 CAS_IR_IPPV_Event_List_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Event_List_Query(msg_ippv_event_list_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_event_list_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_event_list_query_st));

	irReturn = SCELL_Message(MSG_IPPV_EVENT_LIST_QUERY, 0, sizeof(msg_ippv_event_list_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Event_Check_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Event_Check_Reply(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG) // remove warning
	msg_ippv_event_check_reply_st *pReply = (msg_ippv_event_check_reply_st*)msg.pvMessage;
	msg_ippv_event_check_reply_event_st *pReplyEvents = (msg_ippv_event_check_reply_event_st*)((HUINT8*)msg.pvMessage+ sizeof(msg_ippv_event_check_reply_st));

	HxLOG_Info("CAS_IR_IPPV_Event_Check_Reply is called (%ld) (%d) \n", pReply->wResourceId, pReplyEvents->wEventId);
#endif
//[to do] OCTO APP에 Reply ?

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_IPPV_Event_Check_Query
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Event_Check_Query(msg_ippv_event_check_query_st *pMsg, msg_ippv_event_check_query_event_st* pSubMsg)
{
	ia_result irReturn;
	HUINT8 *pMsgToSend = NULL;
	HUINT32	ulMsgSize = 0;

	if(pMsg == NULL || pSubMsg == NULL)
		return IA_FAIL;

	ulMsgSize = sizeof(msg_ippv_event_check_query_st) + (sizeof(msg_ippv_event_check_query_event_st) * pMsg->bEventCount);
	pMsgToSend = (HUINT8*)OxCAS_Malloc(ulMsgSize);
	if(pMsgToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pMsgToSend, pMsg, sizeof(msg_ippv_event_check_query_st));
	HxSTD_memcpy(pMsgToSend + sizeof(msg_ippv_event_check_query_st)
		, pSubMsg, (sizeof(msg_ippv_event_check_query_event_st) * pMsg->bEventCount));

	irReturn = SCELL_Message(MSG_IPPV_EVENT_CHECK_QUERY, 0, ulMsgSize, pMsgToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Callback_Request
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : IPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_IPPV_Callback_Request(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG) // remove warning
	msg_ippv_callback_request_st *pReply = (msg_ippv_callback_request_st*)msg.pvMessage;

	HxLOG_Info("CAS_IR_IPPV_Callback_Request is called (%ld) @@@@@@@@@@@@@@@@@@@@\n", pReply->wResourceId);
#endif
//[to do] OCTO APP에 Reply ?

}

/********************************************************************
*	function name :	 CAS_IR_IPPV_Set_Debit_Threshold
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result CAS_IR_IPPV_Set_Debit_Threshold(CAS_IR_SOFTCELL_MSG msg)
{
	ia_result irResult;
	HUINT8 *pDataToSend;

	msg_ippv_set_debit_threshold_query_st *pMsg = (msg_ippv_set_debit_threshold_query_st*)msg.pvMessage;

	if(pMsg == NULL)
		return FALSE;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_set_debit_threshold_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_set_debit_threshold_query_st));

	irResult = SCELL_Message(MSG_IPPV_SET_DEBIT_THRESHOLD, 0, sizeof(msg_ippv_set_debit_threshold_query_st), pDataToSend);

	return irResult;
}


/********************************************************************
*	function name :	 XSVC_CAS_IR_IPPV_Phone_Number_Query
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result XSVC_CAS_IR_IPPV_Phone_Number_Query(msg_ippv_phone_number_query_data_st *pMsg)
{
	ia_result irResult;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_ippv_phone_number_query_data_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_ippv_phone_number_query_data_st));

	irResult = SCELL_Message(MSG_IPPV_PHONE_NUMBER_QUERY, 0, sizeof(msg_ippv_phone_number_query_data_st), pDataToSend);

	return irResult;
}


/********************************************************************
*	function name :	 CAS_IR_IPPV_Phone_Number_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
void CAS_IR_IPPV_Phone_Number_Reply(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG)	// remove warning

	msg_ippv_phone_number_data_st *pMsg = (msg_ippv_phone_number_data_st*)msg.pvMessage;
	HUINT8*	pSubMsg = (HUINT8*)msg.pvMessage + sizeof(msg_ippv_phone_number_data_st);

	HxLOG_Info("CAS_IR_IPPV_Phone_Number_Reply is called (%ld) (%d) \n", pMsg->wResourceId, *pSubMsg);
#endif
//[to do] OCTO 로 Notify

}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Event_Notify
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Event 알림 from softcell
********************************************************************/
void CAS_IR_LPPV_Event_Notify(CAS_IR_SOFTCELL_MSG msg)
{
	msg_lppv_event_notify_st *pNotify = (msg_lppv_event_notify_st*)msg.pvMessage;
	msg_lppv_event_ticket_st *pSubNotify = (msg_lppv_event_ticket_st*)((HUINT8*)msg.pvMessage + sizeof(msg_lppv_event_notify_st));
	HUINT32 ulDataSize;
	HUINT8* pDataToSend;


//[to do] OCTO 로 Notify ?
	HxLOG_Print("##### %s %d - LPPV EVENT NOTI \n", __FILE__,__LINE__);

	ulDataSize = sizeof(msg_lppv_event_notify_st) + sizeof(msg_lppv_event_ticket_st) * pNotify->bTicketCount;
	pDataToSend = (HUINT8*)OxCAS_Malloc(ulDataSize);
	if(pDataToSend == NULL)
		return;

	HxSTD_memcpy(pDataToSend, pNotify, sizeof(msg_ippv_event_notify_st));
	HxSTD_memcpy(pDataToSend + sizeof(msg_ippv_event_notify_st), pSubNotify, (sizeof(msg_lppv_event_ticket_st) * pNotify->bTicketCount));

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pDataToSend);
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Aware_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Aware 알림 from softcell
********************************************************************/
void CAS_IR_LPPV_Aware_Reply(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG) // remove warning
	msg_lppv_aware_reply_st *pReply = (msg_lppv_aware_reply_st*)msg.pvMessage;
	msg_lppv_ppv_mode_st *pSubNotify = (msg_lppv_ppv_mode_st*)((HUINT8*)msg.pvMessage + sizeof(msg_lppv_aware_reply_st));

	HxLOG_Info("CAS_IR_LPPV_Aware_Reply is called (%ld) (%d) \n", pReply->wResourceId, pSubNotify->bSector);
#endif
//[to do] OCTO 로 Notify ?

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Aware_Query
*	input : Softcell의 msg_lppv_aware_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Aware Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Aware_Query(msg_lppv_aware_query_st *pMsg)
{
	ia_result irResult;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_aware_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_aware_query_st));

	irResult = SCELL_Message(MSG_LPPV_AWARE_QUERY, 0, sizeof(msg_lppv_aware_query_st), pDataToSend);

	return irResult;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Read_Ticket_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Read Ticket 알림 from softcell
********************************************************************/
ia_result CAS_IR_LPPV_Read_Ticket_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_lppv_read_ticket_reply_st *pReply = (msg_lppv_read_ticket_reply_st*)msg.pvMessage;
	msg_lppv_read_ticket_reply_ticket_st *pSubNotify = (msg_lppv_read_ticket_reply_ticket_st*)((HUINT8*)msg.pvMessage + sizeof(msg_lppv_read_ticket_reply_st));

	HUINT8 *pNotify;
	HUINT32 ulLenToSend;

//[to do] OCTO 로 Notify ?

	ulLenToSend = sizeof(msg_lppv_read_ticket_reply_st) + sizeof(msg_lppv_read_ticket_reply_ticket_st) * pReply->bTicketCount;
	pNotify = (HUINT8*)OxCAS_Malloc(ulLenToSend);
	if(pNotify == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_lppv_read_ticket_reply_st));
	HxSTD_memcpy(pNotify + sizeof(msg_lppv_read_ticket_reply_st), pSubNotify, sizeof(msg_lppv_read_ticket_reply_ticket_st) * pReply->bTicketCount);
//[to do] OCTO 로 Notify ?
	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Read_Ticket_Query
*	input : Softcell의 msg_lppv_read_ticket_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Read Tiecket Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Read_Ticket_Query(msg_lppv_read_ticket_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_read_ticket_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_read_ticket_query_st));

	irReturn = SCELL_Message(MSG_LPPV_READ_TICKET_QUERY, 0, sizeof(msg_lppv_read_ticket_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Delete_Ticket_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Delete Ticket 알림 from softcell
********************************************************************/
void CAS_IR_LPPV_Delete_Ticket_Reply(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG) // remove warning
	msg_lppv_delete_ticket_reply_st *pReply = (msg_lppv_delete_ticket_reply_st*)msg.pvMessage;

	HxLOG_Info("CAS_IR_LPPV_Delete_Ticket_Reply is called (%ld) \n", pReply->wResourceId);
#endif
//[to do] OCTO 로 Notify ?

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Delete_Ticket_Query
*	input : Softcell의 msg_lppv_delete_ticket_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Delete Tiecket Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Delete_Ticket_Query(msg_lppv_delete_ticket_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_delete_ticket_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_delete_ticket_query_st));

	irReturn = SCELL_Message(MSG_LPPV_DELETE_TICKET_QUERY, 0, sizeof(msg_lppv_delete_ticket_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Read_Credit_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Read Credit 알림 from softcell
********************************************************************/
void CAS_IR_LPPV_Read_Credit_Reply(CAS_IR_SOFTCELL_MSG msg)
{
#if defined(CONFIG_DEBUG) // remove warning
	msg_lppv_read_credit_reply_st *pReply = (msg_lppv_read_credit_reply_st*)msg.pvMessage;

	HxLOG_Info("CAS_IR_LPPV_Read_Credit_Reply is called (%ld) \n", pReply->wResourceId);
#endif
//[to do] OCTO 로 Notify ?

}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Read_Credit_Query
*	input : Softcell의 msg_lppv_read_credit_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Read Credit Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Read_Credit_Query(msg_lppv_read_credit_query_st *pMsg)
{
	ia_result irResult;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_read_credit_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_read_credit_query_st));

	irResult = SCELL_Message(MSG_LPPV_READ_CREDIT_QUERY, 0, sizeof(msg_lppv_read_credit_query_st), pDataToSend);

	return irResult;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Read_Order_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Read Order 알림 from softcell
********************************************************************/
ia_result CAS_IR_LPPV_Read_Order_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_lppv_read_order_reply_st *pReply = (msg_lppv_read_order_reply_st*)msg.pvMessage;
	msg_lppv_read_order_reply_order_st *pSubNotify = (msg_lppv_read_order_reply_order_st*)((HUINT8*)msg.pvMessage + sizeof(msg_lppv_read_order_reply_st));
	HUINT8 *pNotify;
	HUINT32 ulLenToSend;

//[to do] OCTO 로 Notify ?

	ulLenToSend = sizeof(msg_lppv_read_order_reply_st) + sizeof(msg_lppv_read_order_reply_order_st) * pReply->bOrderCount;
	pNotify = (HUINT8*)OxCAS_Malloc(ulLenToSend);
	if(pNotify == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_lppv_place_order_reply_st));
	HxSTD_memcpy(pNotify + sizeof(msg_lppv_place_order_reply_st), pSubNotify, sizeof(msg_lppv_read_order_reply_order_st) * pReply->bOrderCount);

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Read_Order_Query
*	input : Softcell의 msg_lppv_read_order_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Read Order Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Read_Order_Query(msg_lppv_read_order_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_read_order_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_read_order_query_st));

	irReturn = SCELL_Message(MSG_LPPV_READ_ORDER_QUERY, 0, sizeof(msg_lppv_read_order_query_st), pDataToSend);

	return irReturn;
}


/********************************************************************
*	function name :	 CAS_IR_LPPV_Place_Order_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Place Order 알림 from softcell
********************************************************************/
ia_result CAS_IR_LPPV_Place_Order_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_lppv_place_order_reply_st *pReply = (msg_lppv_place_order_reply_st*)msg.pvMessage;
	HUINT8 *pNotify;

	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_place_order_reply_st));
	if(pNotify == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_lppv_place_order_reply_st));

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Place_Order_Query
*	input : Softcell의 msg_lppv_place_order_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Place Order Query
********************************************************************/
ia_result XSVC_CAS_IR_LPPV_Place_Order_Query(msg_lppv_place_order_query_st *pMsg)
{
	ia_result irReturn;
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return IA_FAIL;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_place_order_query_st));
	if(pDataToSend == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_place_order_query_st));

	irReturn = SCELL_Message(MSG_LPPV_PLACE_ORDER_QUERY, 0, sizeof(msg_lppv_place_order_query_st), pDataToSend);

	return irReturn;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_Cancel_Order_Reply
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : LPPV의 Place Order 알림 from softcell
********************************************************************/
ia_result CAS_IR_LPPV_Cancel_Order_Reply(CAS_IR_SOFTCELL_MSG msg)
{
	msg_lppv_cancel_order_reply_st *pReply = (msg_lppv_cancel_order_reply_st*)msg.pvMessage;
	HUINT8 *pNotify;

	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_cancel_order_reply_st));
	if(pNotify == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_lppv_cancel_order_reply_st));

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 XSVC_CAS_IR_LPPV_Cancel_Order_Query
*	input : Softcell의 msg_lppv_place_order_query_st 구조체
*	output : HBOOL
*	description : LPPV의 Cancel Order Query
********************************************************************/
HBOOL XSVC_CAS_IR_LPPV_Cancel_Order_Query(msg_lppv_cancel_order_query_st *pMsg)
{
	HUINT8 *pDataToSend;

	if(pMsg == NULL)
		return FALSE;

	pDataToSend = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_cancel_order_query_st));
	if(pDataToSend == NULL)
		return FALSE;

	HxSTD_memcpy(pDataToSend, pMsg, sizeof(msg_lppv_cancel_order_query_st));

	SCELL_Message(MSG_LPPV_CANCEL_ORDER_QUERY, 0, sizeof(msg_lppv_cancel_order_query_st), pDataToSend);

	return TRUE;
}

/********************************************************************
*	function name :	 CAS_IR_LPPV_EMM_TLV_Notify
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description :
********************************************************************/
ia_result CAS_IR_LPPV_EMM_TLV_Notify(CAS_IR_SOFTCELL_MSG msg)
{
	msg_emm_tlv_notify_st *pReply = (msg_emm_tlv_notify_st*)msg.pvMessage;
	HUINT8 *pNotify;

	pNotify = (HUINT8*)OxCAS_Malloc(sizeof(msg_lppv_cancel_order_reply_st) + pReply->bLength);
	if(pNotify == NULL)
		return IA_FAIL;

	HxSTD_memcpy(pNotify, pReply, sizeof(msg_lppv_cancel_order_reply_st) + pReply->bLength);

	CAS_IR_IPPV_SendPPVReplyToAP(msg.eOpCode, pNotify);

	return IA_SUCCESS;
}


/********************************************************************
*	function name :	 CAS_IR_IPPV_SendPPVReplyToAP
*	input :
*	output : HBOOL
*	description : PPV의 Rely 메시지를 OCTO에 전송
********************************************************************/
HERROR CAS_IR_IPPV_SendPPVReplyToAP(HUINT32 ulIRMsg, HUINT8 *pData)
{
	return svc_cas_MgrSubSendScCmd2MgrTask(eDxCAS_GROUPID_IR, CMD_CAS_SC_PPVREPLY, ulIRMsg, pData);
}

// ewlee 20100917 - load and save product list
void CAS_IR_PPV_LoadProductList(CAS_IR_PPV_PRODUCT_LIST *pProductList)
{
	HIR_ERR		nRet = HIR_NOT_OK;
	if (pProductList == NULL)
		return;

	nRet = CAS_IR_NVR_FlashReadPPVList(0, sizeof(CAS_IR_PPV_PRODUCT_LIST), (HUINT8*)pProductList);
}

void CAS_IR_PPV_SaveProductList(CAS_IR_PPV_PRODUCT_LIST *pProductList)
{
	HIR_ERR		nRet = HIR_NOT_OK;
	if (pProductList == NULL)
		return;

	nRet = CAS_IR_NVR_FlashWritePPVList(0, sizeof(CAS_IR_PPV_PRODUCT_LIST), (HUINT8*)pProductList);
}


#define _______________MSG_FROM_SOFTCELL__________________

/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_LPPV
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API -Ch13.  IPPV Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_IPPV(CAS_IR_BASE_MSG msgIR)
{
	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case	MSG_IPPV_EVENT_NOTIFY:
			CAS_IR_IPPV_Event_Notify(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_BUY_REPLY:
			CAS_IR_IPPV_Buy_Reply(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_SECTOR_INFORMATION_REPLY:
			CAS_IR_IPPV_Sector_Info_Reply(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_SET_DEBIT_THRESHOLD:
			CAS_IR_IPPV_Set_Debit_Threshold(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_CALLBACK_REQUEST:
			CAS_IR_IPPV_Callback_Request(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_EVENT_LIST_REPLY:
			CAS_IR_IPPV_Event_List_Reply(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_EVENT_CHECK_REPLY:
			CAS_IR_IPPV_Event_Check_Reply(msgIR.msg.SOFTCELL);
			break;

		case	MSG_IPPV_PHONE_NUMBER_REPLY:
			CAS_IR_IPPV_Phone_Number_Reply(msgIR.msg.SOFTCELL);
			break;

	}
}

/********************************************************************
*	function name :	 CAS_IR_SoftcellMsgProc_LPPV
*	input : CAS_IR_BASE_MSG중에 SOFTCELL.
*	output : None
*	description : Based on 710475 SoftCell 3 CA Task Driver API -Ch15.  LPPV Message
********************************************************************/
void CAS_IR_SoftcellMsgProc_LPPV(CAS_IR_BASE_MSG msgIR)
{
	switch(msgIR.msg.SOFTCELL.eOpCode)
	{
		case MSG_LPPV_AWARE_REPLY:
			CAS_IR_LPPV_Aware_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_EVENT_NOTIFY:
			CAS_IR_LPPV_Event_Notify(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_READ_TICKET_REPLY:
			CAS_IR_LPPV_Read_Ticket_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_DELETE_TICKET_REPLY:
			CAS_IR_LPPV_Delete_Ticket_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_READ_CREDIT_REPLY:
			CAS_IR_LPPV_Read_Credit_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_READ_ORDER_REPLY:
			CAS_IR_LPPV_Read_Order_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_PLACE_ORDER_REPLY:
			CAS_IR_LPPV_Place_Order_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_LPPV_CANCEL_ORDER_REPLY:
			CAS_IR_LPPV_Cancel_Order_Reply(msgIR.msg.SOFTCELL);
			break;

		case MSG_EMM_TLV_NOTIFY:
			CAS_IR_LPPV_EMM_TLV_Notify(msgIR.msg.SOFTCELL);
			break;

	}
}


