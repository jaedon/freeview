#ifndef __IR_PPV_H__
#define	__IR_PPV_H__

/********************************************************************
 Header Files
 ********************************************************************/

#include		<db_svc.h>



/********************************************************************
 Constant/Macro Definition
 ********************************************************************/

#define		MAX_PPV_PRODUCT_NUM			50
#define		MAX_EVENT_NAME_LEN			255

/********************************************************************
*   type definition
********************************************************************/
typedef struct tag_CAS_IR_DATE
{
	HUINT8		year;  /**< The range of this is 0 to 99.
					 You shall subtract 2000. */
	HUINT8		month;
	HUINT8		day;
	HUINT8		hour;
	HUINT8		minute;
	HUINT8		second;
} CAS_IR_DATE;

typedef struct tag_CAS_IR_PPV_PRODUCT_STRUCT
{
	HINT32	evtId;
	HUINT8 		sector;
	HUINT8 		ProductName[MAX_EVENT_NAME_LEN];
	HUINT8 		ProductDesc[MAX_EVENT_NAME_LEN];

	CAS_IR_DATE 	StartTime;
	CAS_IR_DATE 	EndTime;
	CAS_IR_DATE		PurchaseTime;
	HUINT32		Price;

	HUINT8		szSvcName[/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()];
	HUINT16		usLcn;
} CAS_IR_PPV_PRODUCT_STRUCT;

typedef struct tag_CAS_IR_PPV_PRODUCT_LIST
{
	HUINT16		usProductNum;
	HUINT8		bIsHide[MAX_PPV_PRODUCT_NUM];
	CAS_IR_PPV_PRODUCT_STRUCT		poduct_list[MAX_PPV_PRODUCT_NUM];
}CAS_IR_PPV_PRODUCT_LIST;


ia_result XSVC_CAS_IR_IPPV_Buy_Query(msg_ippv_buy_query_st *pMsg);
ia_result XSVC_CAS_IR_IPPV_Sector_Info_Query(msg_ippv_sector_query_st *pMsg);
ia_result XSVC_CAS_IR_IPPV_Debit_Threshold(msg_ippv_set_debit_threshold_query_st *pMsg);
ia_result XSVC_CAS_IR_IPPV_Event_List_Query(msg_ippv_event_list_query_st *pMsg);

ia_result XSVC_CAS_IR_LPPV_Read_Ticket_Query(msg_lppv_read_ticket_query_st *pMsg);
ia_result XSVC_CAS_IR_LPPV_Read_Credit_Query(msg_lppv_read_credit_query_st *pMsg);
ia_result XSVC_CAS_IR_LPPV_Read_Order_Query(msg_lppv_read_order_query_st *pMsg);
ia_result XSVC_CAS_IR_LPPV_Place_Order_Query(msg_lppv_place_order_query_st *pMsg);
ia_result XSVC_CAS_IR_LPPV_Aware_Query(msg_lppv_aware_query_st *pMsg);
HBOOL XSVC_CAS_IR_LPPV_Cancel_Order_Query(msg_lppv_cancel_order_query_st *pMsg);

void 	CAS_IR_PPV_GetProductList(CAS_IR_PPV_PRODUCT_LIST *pProductList);
void 	CAS_IR_PPV_SaveProductList(CAS_IR_PPV_PRODUCT_LIST *pProductList);
void		CAS_IR_SoftcellMsgProc_IPPV(CAS_IR_BASE_MSG msgIR);
void		CAS_IR_SoftcellMsgProc_LPPV(CAS_IR_BASE_MSG msgIR);

#endif

/*	end of file	*/


