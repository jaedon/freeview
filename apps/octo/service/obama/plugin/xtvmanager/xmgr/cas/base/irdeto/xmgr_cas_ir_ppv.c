/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

//#include <vd_humax.h>
#include <s3_vd.h>
#include <s3_ippv_msg.h>
#include <s3_lppv_msg.h>
#include <s3_service.h>
#include <s3_event_if_msg.h>
#include <ir_platform.h>
#include <ir_ppv.h>

#include <octo_common.h>
#include "local_include/_xmgr_cas_ir.h"
#include "local_include/_xmgr_cas_ir_adapt.h"
#include "local_include/_xmgr_cas_ir_util.h"
#include "local_include/_xmgr_cas_ir_ui.h"
#include <db_param.h>
#include <stb_params.h>
#include <svc_si.h>

#if defined (CONFIG_PROD_HM9502HD)
#include "local_include/_xmgr_cas_ir_ppv.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define		IPPV_PURCHASED_LIST_MONTH			0x1E	// Get purchased event list for last 30 days
#define		IPPV_PURCHASED_LIST_ALL				0xFF


/*******************************************************************/
/********************      Structure       *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_PROD_HM9502HD)
STATIC	HUINT8	s_bIsCardInserted = TRUE;	// ewlee 20100914 - Event list query should be sent at first time.
#endif

/*******************************************************************/
/********************      STATIC Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PPV_LOCAL_FUNCTION______________________________________________________


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PPV_PUBLIC_FUNCTION______________________________________________________

void XMGR_CAS_IR_IPPV_EventNotify(HUINT8 *pData)
{
	msg_ippv_event_notify_st *pNotify;
	HUINT16 *pSubNotify;

	pNotify = (msg_ippv_event_notify_st*)pData;
	pSubNotify = (HUINT16*)(pData + sizeof(msg_ippv_event_notify_st));

	HxLOG_Info("IPPV Event Notify: Resource ID(%d), Sector(%d), Event ID(%d), Cost(%d)\n", (int)pNotify->wResourceId,
		(int)pNotify->bSector, (int)pNotify->wEventId, (int)pNotify->wEventCost);

#if defined (CONFIG_PROD_HM9502HD)
	if (xmgr_cas_IrIppvSetPreviewEvent(pNotify, pSubNotify) == FALSE)
	{
		// ewlee 20100927 - show purchase dialog right here
		XmgrCas_IrSectorInfo_t	SectorInfo;
		CAS_IR_PPV_PRODUCT_STRUCT	ProductInfo;

		xmgr_cas_IrPpvCopySectorInfo(&SectorInfo, pNotify->bSector);
		xmgr_cas_IrPpvCopyProductIInfo(&ProductInfo);
		// Show Dialog
		xmgr_cas_IrCreateUI("IPPV Purchase Dialog", APP_CAS_PPV_PRIORITY, eCasIr_UI_IppvEventNotify,
			0,&SectorInfo, &ProductInfo,0);
	}
#endif
}

void XMGR_CAS_IR_IPPV_SectorInfoQuery(HUINT8 ucSlotId, ia_byte bSector)
{
	msg_ippv_sector_query_st stSectorQuery;

	stSectorQuery.wResourceId = ucSlotId;
	stSectorQuery.bSector = bSector;

	XSVC_CAS_IR_IPPV_Sector_Info_Query(&stSectorQuery);
}

void XMGR_CAS_IR_IPPV_SectorInfoReply(HUINT8 *pData)
{
	msg_ippv_sector_reply_st *pNotify = (msg_ippv_sector_reply_st*)pData;

	HxLOG_Info("IPPV Sector Info Reply: Resource ID(%d), Sector(%d), Operator Purse(%d), Customer Purse(%d), Debit Limit(%d), Debit Thres(%d)\n",
			(int)pNotify->wResourceId, (int)pNotify->bSector, (int)pNotify->wOperatorPurse, (int)pNotify->wCustomerPurse,
			(int)pNotify->wDebitLimit, (int)pNotify->wDebitThreshold);

#if defined (CONFIG_PROD_HM9502HD)
	xmgr_cas_IrIppvSetSectorInfo(pNotify);

	// ewlee 20100914
	if (s_bIsCardInserted == TRUE)
	{
		XMGR_CAS_IR_IPPV_RequestEventList((HUINT8)pNotify->wResourceId, pNotify->bSector, (ia_byte)IPPV_PURCHASED_LIST_ALL);
	//	s_bIsCardInserted = FALSE;
	}
#endif
}

void XMGR_CAS_IR_IPPV_IPPVBuyQuery(HUINT8 ucSlotId, ia_byte bSector, ia_word16 nEventId, ia_byte *pIppvPin)
{
	msg_ippv_buy_query_st stIPPVQuery;

	stIPPVQuery.wResourceId = ucSlotId;
	stIPPVQuery.bSector = bSector;
	stIPPVQuery.wEventId = nEventId;

	stIPPVQuery.abIppvPin[0] = *(pIppvPin);
	stIPPVQuery.abIppvPin[1] = *(pIppvPin + 1);

	XSVC_CAS_IR_IPPV_Buy_Query(&stIPPVQuery);
}

void XMGR_CAS_IR_IPPV_IPPVBuyReply(HUINT8 *pData)
{
	msg_ippv_buy_reply_st *pNotify;
	HUINT16 *pSubNotify;

	pNotify = (msg_ippv_buy_reply_st*)pData;
	pSubNotify = (HUINT16*)(pData + sizeof(msg_ippv_buy_reply_st));

#if defined(CONFIG_PROD_HM9502HD)
	xmgr_cas_IrPppvReceiveBuyReply(pNotify);
#endif
}


void XMGR_CAS_IR_IPPV_RequestEventList(HUINT8 ucSlotId, ia_byte bSector, ia_byte bPeriod)
{
	msg_ippv_event_list_query_st stEventList;
	ia_result	nRet = IA_FAIL;

	stEventList.wResourceId = ucSlotId;
	stEventList.bSector = bSector;
	stEventList.bPeriod = bPeriod;

	nRet = XSVC_CAS_IR_IPPV_Event_List_Query(&stEventList);
}

void XMGR_CAS_IR_IPPV_RequestEventListReply(HUINT8 *pData)
{
	msg_ippv_event_list_reply_st *pNotify;
	HUINT16 *pSubNotify;

	pNotify = (msg_ippv_event_list_reply_st*)pData;
	pSubNotify = (HUINT16*)(pData + sizeof(msg_ippv_event_list_reply_st));

#if defined(CONFIG_PROD_HM9502HD)
	if (s_bIsCardInserted == TRUE)
	{
		s_bIsCardInserted = FALSE;
		xmgr_cas_IrPpvReceiveEventListReply(pNotify, (msg_ippv_event_list_reply_event_st*)pSubNotify);
		BUS_SendMessage(NULL, eMEVT_XMGR_CAS_IR_PPV_UPDATE_HISTORY_UI, HANDLE_NULL, 0, 0, 0);
	}
#endif
}

void XMGR_CAS_IR_IPPV_SetDebitThreshold(HUINT8 ucSlotId, ia_byte bSector, ia_word16 wDebitThreshold)
{
	msg_ippv_set_debit_threshold_query_st	stDebitThreshold;

	stDebitThreshold.wResourceId = ucSlotId;
	stDebitThreshold.bSector = bSector;
	stDebitThreshold.wDebitThreshold = wDebitThreshold;

	XSVC_CAS_IR_IPPV_Debit_Threshold(&stDebitThreshold);
}

// ewlee 20100914
void XMGR_CAS_IR_PPV_SC_Inserted(HUINT8 ucSlotId)
{
#if defined(CONFIG_PROD_HM9502HD)
	xmgr_cas_IrIppvInitialize();
	XMGR_CAS_IR_IPPV_SectorInfoQuery(ucSlotId, 0);	// ewlee 20100915 - how to get sector number?
	s_bIsCardInserted = TRUE;
#endif
}

void XMGR_CAS_IR_PPV_SC_Removed(HUINT8 ucSlotId)
{
#if defined(CONFIG_PROD_HM9502HD)
	INT_CAS_IR_DestroyUI(eCasIr_UI_IppvEventNotify);
	xmgr_cas_IrIppvDestroy();
	BUS_SendMessage(NULL, eMEVT_XMGR_CAS_IR_PPV_UPDATE_HISTORY_UI, HANDLE_NULL, 0, 0, 0);
#endif
}


void XMGR_CAS_IR_LPPV_EventNotify(HUINT8 *pData)
{
	msg_lppv_event_notify_st *pNotify = (msg_lppv_event_notify_st*)pData;
	msg_lppv_event_ticket_st *pSubNotify = (msg_lppv_event_ticket_st*)(pData + sizeof(msg_lppv_event_notify_st));

	HxLOG_Info("LPPV Event Notify: Resource ID(%d), Sector(%d)\n", (int)pNotify->wResourceId, (int)pNotify->bSector);
	HxLOG_Info("LPPV Event Ticket: Ticket ID(%d), Cost(%d)\n", (int)pSubNotify->wTicketId, (int)pSubNotify->wCost);

	return;
}

void XMGR_CAS_IR_LPPV_PlaceOrder(HUINT8 ucSlotId, ia_byte bSector, ia_word16
	usTicketId, ia_word16 usMaxCost, ia_word16 wExpiryDateCode, ia_bool bIgnoreBuyWnd, ia_byte *pPin)
{
	msg_lppv_place_order_query_st stPlaceOrder;

	stPlaceOrder.wResourceId = ucSlotId;
	stPlaceOrder.bSector = bSector;
	stPlaceOrder.wTicketId = usTicketId;
	stPlaceOrder.wMaxCost = usMaxCost;
	stPlaceOrder.wExpiryDataCode = wExpiryDateCode;
	stPlaceOrder.fIgnoreBuyWindow = bIgnoreBuyWnd;
	HxSTD_memcpy(stPlaceOrder.abPIN, pPin, sizeof(ia_byte) * 2);

	XSVC_CAS_IR_LPPV_Place_Order_Query(&stPlaceOrder);
}

void XMGR_CAS_IR_LPPV_PlaceOrderReply(HUINT8 *pData)
{
	msg_lppv_place_order_reply_st *pNotify;

	pNotify = (msg_lppv_place_order_reply_st*)pData;
}

void XMGR_CAS_IR_LPPV_CancelOrder(HUINT8 ucSlotId, ia_byte bSector, ia_word16 usTicketId, ia_byte *pPin)
{
	msg_lppv_cancel_order_query_st stCancelOrder;

	stCancelOrder.wResourceId = ucSlotId;
	stCancelOrder.bSector = bSector;
	stCancelOrder.wTicketId = usTicketId;
	HxSTD_memcpy(stCancelOrder.abPIN, pPin, sizeof(ia_byte) * 2);

	XSVC_CAS_IR_LPPV_Cancel_Order_Query(&stCancelOrder);
}

void XMGR_CAS_IR_LPPV_CancelOrderReply(HUINT8 *pData)
{
	msg_lppv_cancel_order_reply_st *pNotify;

	pNotify = (msg_lppv_cancel_order_reply_st*)pData;
}

void XMGR_CAS_IR_LPPV_ReadOrder(HUINT8 ucSlotId, ia_byte bSector)
{
	msg_lppv_read_order_query_st stReadOrder;

	stReadOrder.wResourceId = ucSlotId;
	stReadOrder.bSector = bSector;

	XSVC_CAS_IR_LPPV_Read_Order_Query(&stReadOrder);
}

void XMGR_CAS_IR_LPPV_ReadOrderReply(HUINT8 *pData)
{
	msg_lppv_read_order_reply_st *pNotify;
	msg_lppv_read_order_reply_order_st *pSubNotify;

	pNotify = (msg_lppv_read_order_reply_st*)pData;
	pSubNotify = (msg_lppv_read_order_reply_order_st*)(pData + sizeof(msg_lppv_read_order_reply_st));
}

void XMGR_CAS_IR_LPPV_ReadTicket(HUINT8 ucSlotId, ia_byte bSector, ia_byte bTicketMode)
{
	msg_lppv_read_ticket_query_st stReadTicket;

	stReadTicket.wResourceId = ucSlotId;
	stReadTicket.bSector = bSector;
	stReadTicket.bTicketMode = bTicketMode;

	XSVC_CAS_IR_LPPV_Read_Ticket_Query(&stReadTicket);
}

void XMGR_CAS_IR_LPPV_ReadTicketReply(HUINT8 *pData)
{
	msg_lppv_read_ticket_reply_st *pNotify;
	msg_lppv_read_ticket_reply_ticket_st *pSubNotify;

	pNotify = (msg_lppv_read_ticket_reply_st*)pData;
	pSubNotify = (msg_lppv_read_ticket_reply_ticket_st*)(pData + sizeof(msg_lppv_read_ticket_reply_st));
}

void XMGR_CAS_IR_LPPV_ReadCredit(HUINT8 ucSlotId, ia_byte bSector)
{
	msg_lppv_read_credit_query_st stReadCredit;

	stReadCredit.wResourceId = ucSlotId;
	stReadCredit.bSector = bSector;

	XSVC_CAS_IR_LPPV_Read_Credit_Query(&stReadCredit);
}

void XMGR_CAS_IR_LPPV_EMMTLV_Notify(HUINT8 *pData)
{
	msg_emm_tlv_notify_st *pNotify;
	ia_byte *pSubNotify;

	pNotify = (msg_emm_tlv_notify_st*)pData;
	pSubNotify = (ia_byte*)(pData + sizeof(msg_emm_tlv_notify_st));
}
