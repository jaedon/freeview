/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Include_________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"
#include "na_mgr_cak_ext.h"
#include "util.h"
#include <svc_si.h>
#include <_svc_cas_mgr_nvram.h>
#include <_svc_cas_mgr_common.h>

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
/**************************************************************************************************/
#define ________CA_CAK_PPV_Golbal_Value___________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Define__________________________________________________
/**************************************************************************************************/





#define NASP_CA_DESC			0x86
#define NASP_PPV_DESC			0x87
#define NASP_IEMM_DESC			0x8E

#define MAX_EIT_DES_SIZE 		1024

#define QUEUE_SIZE_1						sizeof(NaCasMsg_t)
#define QUEUE_SIZE_2						(sizeof(TUnsignedInt32) * 4)

#define VK_CreateMessageQueue(a, b, c, d)	VK_MSG_Create(a, d, c, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b, c) 			VK_MSG_Send(a, b, c)
#define VK_GetMessage(a ,b, c)				VK_MSG_Receive(a, b, c)
#define VK_WaitGetMessage(a, b, c, d)		VK_MSG_ReceiveTimeout(a, b, d, c)

#define APP_EVT_INFO_COMPLETE			0
#define APP_EXPIRATION_COMPLETE			1

#define NA_MAX_CALLBACK_NUM 			2

typedef enum
{
	eEIT_PF_PRESENT = 0,
 	eEIT_PF_FOLLOWING,
	eEIT_END,
} EVENT_PF_INFO;

/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Type_Define____________________________________________
/**************************************************************************************************/
typedef enum
{
	CA_PPV_CALLBACK_CREDIT = 0,
	CA_PPV_CALLBACK_PPV_LIST,
} CA_PPV_CALLBACK_TYPE_T;

typedef struct PAY_PER_VIEW_LIST_STRUCT
{
	TUnsignedInt8		xNbItems;
	TEventPurchaseListItem *pList;
} PayPerViewList_t;

typedef struct CREDIT_LIST_STRUCT
{
	TUnsignedInt8		xNbItems;
	TCreditItem			*pList;
} CreditList_t;

typedef struct PPV_PURCHASE_INFO_STRUCT
{
	TUnsignedInt32		xSvcHandle;
	TUnsignedInt8		xIndex;
	TCaRequest			*xRequest;
	TCaProduct			*xProduct;
	PPV_EVENT_INFO 		xEvtInfo;
} PpvPurchaseInfo_t;

typedef struct EVT_PURCHASE_STRUCT
{
	TUnsignedInt8		fPresent;
	TUnsignedInt8		ucEvtName[MAX_STR_LEN];
	TUnsignedInt8		ucSvcName[MAX_STR_LEN];
	TUnsignedInt16		nEvtId;
	TUnsignedInt32		ulDesLength;
	TUnsignedInt8		ucDesInfo[MAX_EIT_DES_SIZE];
	TUnsignedInt8		ucRawEvtNameLength;
	TUnsignedInt8		*pucRawEvtName;
	TUnixDate			ulStartTime;
	TUnixDate			ulDuration;
	TCalendarTime		stCalendarTime;
} EventPurcahseInfo_t;


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Value____________________________________________
/**************************************************************************************************/
static TUnsignedInt32 			s_ulPpvTaskId;
static TUnsignedInt32 			s_ulPpvQueueId;

PayPerViewList_t				ppv_list;
CreditList_t					credit_list;

static TUnsignedInt32 			gCasSmcState = -1;
static TUnsignedInt32 			gPpvListReqFlag = FALSE;
static TUnsignedInt32 			gCreditListReqFlag = FALSE;

static TEventPurchaseListStatus	gPpvListStatus;
static TCreditStatus 			gCreditStatus;

static TUnsignedInt8			gPpvListItemNb = 0;
static TUnsignedInt8			gCreditItemNb = 0;

PPV_PURCHASE_LIST				purchase_list;
PpvPurchaseInfo_t				gPpvPurchaseInfo;
EventPurcahseInfo_t				gEvtPurchaseInfo[eEIT_END];

static TUnsignedInt32			s_ulEvQueueId;
static TUnsignedInt32			s_ulExpirationQueueId;

static TBoolean 				EIT_STATE = 0;
static TBoolean 				TDT_STATE = 0;
static TBoolean 				CREDIT_STATE = 0;
static TBoolean 				PPV_LIST_STATE = 0;
static TBoolean 				EPG_MODE_STATE = 0;

TCreditCallback					gCreditCallBackFn = NULL;
TEventPurchaseListCallback		gPpvListCallBackFn = NULL;
TRechargeNotifyCallback 		RechargeNotifyCallbackFn = NULL;
TPurchaseHistoryNotifyCallback 	PurchaseHistoryNotifyCallback = NULL;
TPurchaseEventNotifyCallback   	PurchaseEventNotifyCallback = NULL;
TRequestEventNotifyCallback    	RequestEventNotifyCallback = NULL;


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Prototype_________________________________________
/**************************************************************************************************/
static void local_caPpv_InitPpvList(void);
static void local_caPpv_FreePpvList(void);
static void local_caPpv_AllocatePpvArray(TUnsignedInt8 xNbItems);
static void local_caPpv_AdjustPpvListTime(TCalendarTime*pStartTime);
static void local_caPpv_AllocatePpvList(TEventPurchaseListItem*pListItem,TUnsignedInt8 index);
static void local_caPpv_InitCreditList(void);
static void local_caPpv_FreeCreditList(void);
static void local_caPpv_AllocateCreditArray(TUnsignedInt8 xNbItems);
static void local_caPpv_AllocateCreditList(TUnsignedInt8  index, TCreditItem* pListItem);

static void local_caPpv_RequestCmd(NaCasMsg_t *pstNaMsg);
static void local_caPpv_ProcessFilterCmd(NaCasMsg_t *pstNaMsg);
static void local_caPpv_ProcessingPpvCallback(NaCasMsg_t *pstNaMsg);
static void local_caPpv_Task(void* p);

static TCaStatus local_caPpv_PpvListNotification(void);
static TCaStatus local_caPpv_CreditNotification(void);
static TCaStatus local_caPpv_PpvListCallback(TEventPurchaseListStatus xStatus, TUnsignedInt8 xItemNb, TEventPurchaseListItem *pxEventPurchaseListArray);
static TCaStatus local_caPpv_CreditCallback(TCreditStatus xStatus, TUnsignedInt8 xNumberOfItems, TCreditItem *pxCreditArray);
static void local_caPpv_PurchaseHistoryCallback(TCaRequest*  pxRequest);
static void local_caPpv_initPpv(void);
static void local_caPpv_PrintPurchaseList(PPV_PURCHASE_LIST *purchaseList, char* file , int line);
static void local_caPpv_InitPurchaseHistory(PPV_PURCHASE_LIST* pList);
static void local_caPpv_ClearPurchaseInfo(void);
static TSignedInt32 local_caPpv_GetEventInfoWithEvtId(TSignedInt16 nEvtId);

static HERROR  local_caPpv_MakeIppvEventInfo(TUnsignedInt32 hSvchandle);
static HERROR  local_caPpv_MakeIppvEventInfoByEpg(TUnsignedInt32 hSvchandle, TUnsignedInt16 nEvtId);
static void local_caPpv_RequestPurchaseHistory(void);
static void local_caPpv_RequestPurchaseHistoryCallBack(TCaRequest*pxRequest);
static void local_caPpv_FlushMsgQueue(TUnsignedInt32 msgQueueId);
static HERROR	local_caPpv_DisposeRequest(TCaRequest* pRequest);
static HERROR	local_caPpv_AdjustTimeOffset(UNIXTIME *stCalendarTime, UNIXTIME *endTime);
static HERROR	local_caPpv_GetPurchaseWIndow(TCaProduct *pxProduct, UNIXTIME *pnStartTime, UNIXTIME *pnEndTime, UNIXTIME *pnExpiredTime);
static TEventStatus local_caPpv_ProcessSpecialEvent(TCaProduct *pxProduct, TPurchasableEventType *xPurchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime);
static HERROR	local_caPpv_RequestEventInfoCallBack(TCaRequest* pxRequest);
static HERROR	local_caPpv_RequestEventInfo(void);
static HERROR	local_caPpv_RequestPurchaseSpecialEvent(TCaProduct *pProduct, TCaProcessingStatus *pProcessingStatus);
static HERROR	local_caPpv_WriteNewCreditFlag(TUnsignedInt8 bFlag);
static HERROR	local_caPpv_ReadNewCreditFlag(TUnsignedInt8 *pbFlag);



/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Prototype_Body____________________________________
/**************************************************************************************************/
static void local_caPpv_InitPpvList(void)
{
	ppv_list.pList = NULL;
	ppv_list.xNbItems = 0;
}

static void local_caPpv_FreePpvList(void)
{
	int i;

	for (i=0; i<ppv_list.xNbItems; i++)
	{
		if (ppv_list.pList[i].pEventName) OxCAS_Free((void*) ppv_list.pList[i].pEventName);
		if (ppv_list.pList[i].pServiceName) OxCAS_Free( (void*) ppv_list.pList[i].pServiceName);
		if (ppv_list.pList[i].pStartTime) OxCAS_Free( (void*) ppv_list.pList[i].pStartTime);
	}

	if (ppv_list.pList != NULL)
		OxCAS_Free( ppv_list.pList);
}

static void local_caPpv_AllocatePpvArray(TUnsignedInt8 xNbItems)
{
	if (xNbItems > 0)
	{
		ppv_list.pList = (TEventPurchaseListItem *)OxCAS_Malloc(xNbItems*sizeof(TEventPurchaseListItem));
		ppv_list.xNbItems = xNbItems;
		memset(ppv_list.pList, 0x00, (xNbItems*sizeof(TEventPurchaseListItem)));
	}
}

static void local_caPpv_AdjustPpvListTime(TCalendarTime*pStartTime)
{
	UNIXTIME uxTime;
	HxDATETIME_t stDTime;

	if (pStartTime == NULL)
		return;

	stDTime.stDate.usYear = pStartTime->year + 1900;
	stDTime.stDate.ucMonth = pStartTime->month;
	stDTime.stDate.ucDay= pStartTime->day;
	stDTime.stTime.ucHour = pStartTime->hour;
	stDTime.stTime.ucMinute = pStartTime->minute;
	stDTime.stTime.ucSecond = pStartTime->second;

	HLIB_DATETIME_ConvertDateTimeToUnixTime((HxDATETIME_t*)&stDTime, (UNIXTIME*)&uxTime);
	local_caPpv_AdjustTimeOffset((UNIXTIME*)&uxTime, NULL);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(uxTime, (HxDATETIME_t*)&stDTime);

	pStartTime->year = stDTime.stDate.usYear - 1900;
	pStartTime->month = stDTime.stDate.ucMonth;
	pStartTime->day = stDTime.stDate.ucDay;
	pStartTime->hour = stDTime.stTime.ucHour;
	pStartTime->minute = stDTime.stTime.ucMinute;
	pStartTime->second = stDTime.stTime.ucSecond;
}


static void local_caPpv_AllocatePpvList(TEventPurchaseListItem*pListItem,TUnsignedInt8 index)
{
	if (index > ppv_list.xNbItems)
	{
		HxLOG_Print("[local_caPpv_uitAllocatePpvList] invalid input param\n");
		return;
	}

	ppv_list.pList[index].iccSessionId = pListItem->iccSessionId;
	ppv_list.pList[index].provider = pListItem->provider;

	ppv_list.pList[index].eventNameLength = pListItem->eventNameLength;
	if ( (pListItem != NULL) && (pListItem->eventNameLength > 0) && (pListItem->pEventName != NULL) )
	{
		ppv_list.pList[index].pEventName = (TUnsignedInt8*)OxCAS_Malloc(pListItem->eventNameLength + 1);
		if (ppv_list.pList[index].pEventName)
		{
			memset((TUnsignedInt8 *)(ppv_list.pList[index].pEventName), 0, pListItem->eventNameLength + 1);
			memcpy((TUnsignedInt8 *)(ppv_list.pList[index].pEventName), (TUnsignedInt8 *)(pListItem->pEventName), pListItem->eventNameLength);
		}
		else
		{
			ppv_list.pList[index].eventNameLength = 0;
		}
	}

	ppv_list.pList[index].serviceNameLength = pListItem->serviceNameLength;
	if ( (pListItem != NULL) && (pListItem->serviceNameLength > 0) && (pListItem->pServiceName != NULL) )
	{
		ppv_list.pList[index].pServiceName = OxCAS_Malloc(pListItem->serviceNameLength + 1);
		if (ppv_list.pList[index].pServiceName)
		{
			memset((TUnsignedInt8 *)(ppv_list.pList[index].pServiceName), 0, pListItem->serviceNameLength + 1);
			memcpy((TUnsignedInt8 *)(ppv_list.pList[index].pServiceName), (TUnsignedInt8 *)(pListItem->pServiceName), pListItem->serviceNameLength);
		}
		else
		{
			ppv_list.pList[index].serviceNameLength = 0;
		}
	}

	if (pListItem != NULL && pListItem->pStartTime != NULL)
	{
		ppv_list.pList[index].pStartTime = OxCAS_Malloc(sizeof(TCalendarTime));
		if (ppv_list.pList[index].pStartTime)
		{
			//uitAdjustPpvListTime((TCalendarTime*)pListItem->pStartTime);
			memcpy((TUnsignedInt8 *)(ppv_list.pList[index].pStartTime), (TUnsignedInt8 *)(pListItem->pStartTime), sizeof(TCalendarTime));
		}
	}

	ppv_list.pList[index].flags = pListItem->flags;
	ppv_list.pList[index].cost = pListItem->cost;
}

static void local_caPpv_InitCreditList(void)
{
	credit_list.pList = NULL;
	credit_list.xNbItems = 0;
}

static void local_caPpv_FreeCreditList(void)
{
	if (credit_list.pList != NULL)
		OxCAS_Free(credit_list.pList);
}

static void local_caPpv_AllocateCreditArray(TUnsignedInt8 xNbItems)
{
	if (xNbItems > 0)
	{
		credit_list.xNbItems = xNbItems;
		credit_list.pList = (TCreditItem *)OxCAS_Malloc(xNbItems*sizeof(TCreditItem));
		memset(credit_list.pList, 0x00, (xNbItems*sizeof(TCreditItem)));
	}
}

static void local_caPpv_AllocateCreditList(TUnsignedInt8  index, TCreditItem* pListItem)
{
	if (index > credit_list.xNbItems)
	{
		HxLOG_Print("[local_caPpv_uitAllocateCreditList] invalid input param\n");
		return;
	}

	credit_list.pList[index].provider = pListItem->provider;
	credit_list.pList[index].credit = pListItem->credit;
	credit_list.pList[index].flags = pListItem->flags;
	credit_list.pList[index].iccSessionId= pListItem->iccSessionId;
}

static TCaStatus local_caPpv_PpvListCallback(TEventPurchaseListStatus xStatus, TUnsignedInt8 xItemNb, TEventPurchaseListItem *pxEventPurchaseListArray)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("->[Enter] xStatus(0x%x) xItemNb(%c) \n", xStatus, xItemNb);

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_LIST_CALL_BACK;
	stMsg.lParam3 = (unsigned short)xItemNb;
	stMsg.hWnd = (TUnsignedInt32)pxEventPurchaseListArray;
	stMsg.info = xStatus;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if( nErrCode != ERR_OK )
	{
		HxLOG_Print("Q Send Fail in uitPpvListCallback 0x%x\n", nErrCode);
		return CA_ERROR;
	}

	HxLOG_Print("-> [Exit] \n");

	return CA_NO_ERROR;
}

static TCaStatus local_caPpv_CreditCallback(TCreditStatus xStatus, TUnsignedInt8 xNumberOfItems, TCreditItem *pxCreditArray)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("-> [Enter] xStatus(0x%x) xNumberOfItems(%d)\n", xStatus, xNumberOfItems);

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_CREDIT_CALL_BACK;
	stMsg.lParam3 = xNumberOfItems;
	stMsg.hWnd = pxCreditArray;
	stMsg.info = (TUnsignedInt32) xStatus;

	nErrCode = VK_PostMessage( s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1 );
	if( nErrCode != ERR_OK )
	{
		HxLOG_Print("Q Send Fail in uitCreditCallback 0x%x\n", nErrCode);
		return CA_ERROR;
	}

	HxLOG_Print("-> [Exit] \n");

	return CA_NO_ERROR;
}


static void local_caPpv_PurchaseHistoryCallback(TCaRequest*  pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("-> [Enter] pxRequest(0x%x)\n", pxRequest);

	// Send Msg
	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PURCHASE_HISTORY_CALL_BACK;
	stMsg.lParam1 = (unsigned int)pxRequest;

	nErrCode= VK_PostMessage( s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1 );
	if( nErrCode != ERR_OK )
	{
		HxLOG_Print("Q Send Fail in local_caPpv_PurchaseHistoryCallback 0x%x\n", nErrCode);
	}

	HxLOG_Print("-> [Exit] \n");
	return ;
}

static void local_caPpv_PresentEventInfoCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("->[Enter] pxRequest(0x%x) \n", pxRequest);

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam1 = (unsigned int)pxRequest;
	stMsg.lParam2 = CAK_REQUEST_PRESENT_EVENT_CALL_BACK;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("Q Send Fail in local_caPpv_PresentEventInfoCallback 0x%x\n", nErrCode);
	}

	HxLOG_Print("->[Exit]\n");
}

static void local_caPpv_FollowingEventInfoCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("-> [Enter] pxRequest(0x%x) \n", pxRequest);

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam1 = (unsigned int)pxRequest;
	stMsg.lParam2 = CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("Q Send Fail in local_caPpv_FollowingEventInfoCallback 0x%x\n", nErrCode);
	}

	HxLOG_Print("->[Exit]\n");
}

static void local_caPpv_initPpv(void)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	nErrCode = VK_CreateMessageQueue( 32, &s_ulEvQueueId, "EVTQ", 32);
	if( nErrCode != ERR_OK )
	{
		HxLOG_Info("Error in creating UIT EVTINFO message queue %ld\n", nErrCode );
	}

	nErrCode = VK_CreateMessageQueue( 32, &s_ulExpirationQueueId, "EXPQ", 32);
	if( nErrCode != ERR_OK )
	{
		HxLOG_Info("Error in creating UIT EXPIRATION message queue %ld\n", nErrCode );
	}
}


static void local_caPpv_PrintPurchaseList(PPV_PURCHASE_LIST *purchaseList, char* file , int line)
{
	TUnsignedInt32 index;
	PPV_PURCHASE_LIST_ITEM* item;

	if (purchaseList == NULL)
	{
		HxLOG_Print("\n\n\n[*****PURCHASE_LIST = NULL ][%d][%s]\n", line, file);
		return;
	}

	HxLOG_Print("\n\n\n[*****PURCHASE_LIST (item num:%ld) ][%d][%s]\n", purchaseList->xNbItems, line, file);
	item = purchaseList->pList;

	for (index = 0; index < purchaseList->xNbItems; index++)
	{
		HxLOG_Print("\t[%d] nameLeng(%d) : name : %s \n", index, item[index].EvtNameLength,item[index].EvtName);
		HxLOG_Print("\t         time (%04dy %02dm %02dd %02dh %02dm %02ds): cost(%ld) \n",
			item[index].PurchaseTime.year+1900, item[index].PurchaseTime.month, item[index].PurchaseTime.day, item[index].PurchaseTime.hour,
			 item[index].PurchaseTime.minute, item[index].PurchaseTime.second, item[index].Cost));

		if  (item[index].PurchaseFlag & CA_PURCHASE_WATCHED)// BIT0  /**<  The related product has been watched.. */
			HxLOG_Print("\t	 PurchaseFlag (%ld) : CA_PURCHASE_WATCHED\n", item[index].PurchaseFlag);

		if  (item[index].PurchaseFlag & CA_PURCHASE_REFUNDED)// BIT1  /**<  The purchase has been refunded.*/
			HxLOG_Print("\t	 PurchaseFlag (%ld) : CA_PURCHASE_REFUNDED\n", item[index].PurchaseFlag);

		if  (item[index].PurchaseFlag & CA_PURCHASE_SUSPENDED)// BIT2  /**< The related product is suspended.*/
			HxLOG_Print("\t	 PurchaseFlag (%ld): CA_PURCHASE_SUSPENDED\n", item[index].PurchaseFlag);

		if  (item[index].PurchaseFlag & CA_PURCHASE_PAY_PER_TIME)// BIT3  /**<  The related product is a pay-per-time.*/
			HxLOG_Print("\t	 PurchaseFlag(%ld) : CA_PURCHASE_PAY_PER_TIME\n", item[index].PurchaseFlag);

		if  (item[index].PurchaseFlag & CA_PURCHASE_IMPULSIVE)// BIT4  /**<  Flag to specify: product can be purchased impulsively.*/
			HxLOG_Print("\t	 PurchaseFlag(%ld) : CA_PURCHASE_IMPULSIVE\n", item[index].PurchaseFlag);
	}

	HxLOG_Print("[******************** END ********************]\n");
	return;
}

static void local_caPpv_InitPurchaseHistory(PPV_PURCHASE_LIST* pList)
{
	HxLOG_Print("[uiTask][local_caPpv_InitPurchaseHistory] --->ENTER!!\n");

	if (pList == NULL)
	{
		HxLOG_Print("[uiTask][local_caPpv_InitPurchaseHistory] (NULL) called!!!!!!\n");
		return;
	}

	if ( (pList->xNbItems > 0) && (pList->pList) )
	{
		HxLOG_Print("/t[local_caPpv_InitPurchaseHistory](AP) OxCAS_Free(PURCHASE_LIST.pList);\n");
		OxCAS_Free(pList->pList);
		pList->pList = NULL;
	}

	HxSTD_memset(pList, 0x00, sizeof(PPV_PURCHASE_LIST));
	HxLOG_Print("[uiTask][local_caPpv_InitPurchaseHistory] <----------EXIT!!\n");
	return;
}

static void local_caPpv_ClearPurchaseInfo(void)
{
	TUnsignedInt32 	i;
	TCaRequest		*xRequest;

	for (i=0; i<eEIT_END; i++)
	{
		if (gEvtPurchaseInfo[i].pucRawEvtName)
		{
			OxCAS_Free(gEvtPurchaseInfo[i].pucRawEvtName);
		}
		memset((char*)&gEvtPurchaseInfo[i], 0, sizeof(EventPurcahseInfo_t));
	}

	xRequest = gPpvPurchaseInfo.xRequest;
	HxSTD_memset((char*)&gPpvPurchaseInfo, 0x00, sizeof(PpvPurchaseInfo_t));
	gPpvPurchaseInfo.xRequest = xRequest;
}

static TSignedInt32 local_caPpv_GetEventInfoWithEvtId(TSignedInt16 nEvtId)
{
	TSignedInt32 i;

	for (i=0; i<2; i++)
	{
		if ( (gEvtPurchaseInfo[i].fPresent) && (gEvtPurchaseInfo[i].nEvtId == nEvtId) )
			return (i);
	}
	return (-1);
}

static HERROR  local_caPpv_MakeIppvEventInfo(TUnsignedInt32 hSvchandle)
{
	TUnsignedInt32 	hErr;

	local_caPpv_ClearPurchaseInfo();

	// Present
	hErr = CA_NA_PSI_siExt_GetEventInfo(eEIT_PF_PRESENT, &gEvtPurchaseInfo[eEIT_PF_PRESENT]);
	if(hErr != ERR_OK)	{	return CA_ERROR; }

	// Following
	hErr= CA_NA_PSI_siExt_GetEventInfo(eEIT_PF_FOLLOWING, &gEvtPurchaseInfo[eEIT_PF_FOLLOWING]);
	if(hErr != ERR_OK)	{	return CA_ERROR; }

	gPpvPurchaseInfo.xSvcHandle = hSvchandle;

	return ERR_OK;
}

static HERROR  local_caPpv_MakeIppvEventInfoByEpg(TUnsignedInt32 hSvchandle, TUnsignedInt16 nEvtId)
{
	unsigned int i, nRet, hErr;
	unsigned short tsId, onId, svcId;
	TUnsignedInt8 *pRawCaDesc;
	TUnsignedInt16 rawCaDescSize;
	EIT_EVENT_INFO stEvtInfo[2];

	local_caPpv_ClearPurchaseInfo();

#if defined(ysmoon_temp)
	nRet = SI_EIT_GetTripleId(svchandle, &tsId, &onId, &svcId);
	if (nRet)
	{
		HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> SI_EIT_GetTripleId Err(%x)\n ", nRet);
		return ERR_FAIL;
	}

	gPpvPurchaseInfo.xSvcHandle = svchandle;
	memset(stEvtInfo, 0, (sizeof(EIT_EVENT_INFO)*2));
	nRet = SI_EIT_GetEvenWithEventIdForNa(tsId, onId, svcId, nEvtId, &stEvtInfo[0]);
	if (nRet != ERR_OK)
		HxLOG_Print("[local_caPpv_MakeIppvEventInfoByEpg] Error in getting present event information\n");
	else
		gEvtPurchaseInfo[0].fPresent = 1;

	for (i=0; i<2; i++)
	{
		rawCaDescSize = 0;
		pRawCaDesc = NULL;

		if (!gEvtPurchaseInfo[i].fPresent)
			continue;

		// 1. EvtName copy
		if (stEvtInfo[i].szName)
		{
			unsigned int length ;

			length = svc_cas_MgrUtilStrLen((char*)stEvtInfo[i].szName);

			if (length < MAX_STR_LEN)
				memcpy(gEvtPurchaseInfo[i].ucEvtName, stEvtInfo[i].szName, length);
			else
				memcpy(gEvtPurchaseInfo[i].ucEvtName, stEvtInfo[i].szName, MAX_STR_LEN-1);
		}

		// 2. EvtId copy
		gEvtPurchaseInfo[i].nEvtId = stEvtInfo[i].wEvtId;
		// 3. StartTime copy
		memcpy(&gEvtPurchaseInfo[i].stCalendarTime, &(stEvtInfo[i].ucStartTime), sizeof (TCalendarTime));
		// 4. stCalendarTime copy(UNIXTIME)
		gEvtPurchaseInfo[i].ulStartTime = stEvtInfo[i].startTime;
		// 5. duration copy
		gEvtPurchaseInfo[i].ulDuration = stEvtInfo[i].duration;

		HxLOG_Print("\n\nlocal_caPpv_MakeIppvEventInfoByEpg->********************** NEW EVENT *************************\n");
		HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> Service Handle : 0x%x\n", svchandle);
		HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> EVT ID : 0x%x\n", gEvtPurchaseInfo[i].nEvtId);
		HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> EVT NAME = %s \n", gEvtPurchaseInfo[i].ucEvtName);
		HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> EVT TYPE : %s\n", (i == 0) ? "PRESENT" : "FOLLOWING");
		HxLOG_Print("sMakeIppvEventInfoByEpg-> START Time (Y=%d),(M=%d)(D=%d)(H=%d)(M=%d)(UNIXTIME=%d)\n", (gEvtPurchaseInfo[i].stCalendarTime.year+1900), gEvtPurchaseInfo[i].stCalendarTime.month, gEvtPurchaseInfo[i].stCalendarTime.day, gEvtPurchaseInfo[i].stCalendarTime.hour, gEvtPurchaseInfo[i].stCalendarTime.minute, gEvtPurchaseInfo[i].ulStartTime);
		HxLOG_Print("sMakeIppvEventInfoByEpg-> Duration (UNIXTIME=%d)\n", gEvtPurchaseInfo[i].ulDuration);
		HxLOG_Print("sMakeIppvEventInfoByEpg->********************************************************\n\n");

		nRet = MXI_SM_CreateEitEventRawDescriptor(svchandle, ACT_PF_HEAD, stEvtInfo[i].wEvtId, svcId, tsId, onId, &rawCaDescSize, (void **)&pRawCaDesc);
		if ((nRet == ERR_OK) && (pRawCaDesc != NULL))
		{
			// 6. DesLength copy
			gEvtPurchaseInfo[i].ulDesLength = rawCaDescSize;
			// 7. Des[] copy
			if  (gEvtPurchaseInfo[i].ulDesLength < MAX_EIT_DES_SIZE)
				memcpy(gEvtPurchaseInfo[i].ucDesInfo, pRawCaDesc, rawCaDescSize);
			else
				memcpy(gEvtPurchaseInfo[i].ucDesInfo, pRawCaDesc, MAX_EIT_DES_SIZE-1);// 제일 마지막에 NULL을 넣어 준다.

			HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg-> EIT DES Length = %d \n", gEvtPurchaseInfo[i].ulDesLength);

#if defined(_DEBUG_)
{
			unsigned int j ;

			HxLOG_Print("\t\t\n======> Start Desc LawData \n\n");
			for (j=0; j<gEvtPurchaseInfo[i].ulDesLength; j++)
				HxLOG_Print("0x%02x, ",  gEvtPurchaseInfo[i].ucDesInfo[j]);
			HxLOG_Print("\t\t\n======> End Desc LawData \n\n");
}
#endif

			// 8, 9. ucRawEvtNameLength && pucRawEvtName copy 아래서 rawEvtName의 mem alloc을 해 준다.
			SI_EIT_GetShortEventDescriptor(rawCaDescSize, pRawCaDesc, &gEvtPurchaseInfo[i].ucRawEvtNameLength, &gEvtPurchaseInfo[i].pucRawEvtName) ;
			HxLOG_Print("local_caPpv_MakeIppvEventInfoByEpg->  EVT Raw Name = %s \n", gEvtPurchaseInfo[i].pucRawEvtName);
		}
		else
		{
			HxLOG_Print("0. MXI_SM_CreateEitEventRawDescriptor return Err(%x)\n", nRet) );

			if (rawCaDescSize > 0 && pRawCaDesc != NULL)
				OxCAS_Free(pRawCaDesc);

			continue;
		}

		if (rawCaDescSize > 0 && pRawCaDesc != NULL)
			OxCAS_Free(pRawCaDesc);
	}
#else

	// Present
	hErr = CA_NA_PSI_siExt_GetEventInfo(0, &gEvtPurchaseInfo[0]);
	if(hErr != ERR_OK)	{	return CA_ERROR; }

	gEvtPurchaseInfo[0].fPresent = 1;

	// Following
	hErr= CA_NA_PSI_siExt_GetEventInfo(1, &gEvtPurchaseInfo[1]);
	if(hErr != ERR_OK)	{	return CA_ERROR; }

	gEvtPurchaseInfo[1].fPresent = 1;

	gPpvPurchaseInfo.xSvcHandle = hSvchandle;
#endif

	return ERR_OK;
}

static void local_caPpv_RequestPurchaseHistory(void)
{
	TCaRequestStatus status;
	TCaRequest* pRequest;

#if defined(ysmoon_temp)
	if (!IS_CAK_INIT)
		return;
#endif

	HxLOG_Print("[local_caPpv_RequestPurchaseHistory] -------> ENTER!!\n");

#if (IPPV_DBG)
	HxLOG_Print("[local_caPpv_RequestPurchaseHistory] <----------- OLD Purchase List -------------->\n");
 	local_caPpv_PrintPurchaseList(&purchase_list, __FILE__, __LINE__);
#endif

	// clear old purchase list
	local_caPpv_InitPurchaseHistory(&purchase_list);

	status = caRequestCreate(CA_REQUEST_TYPE_PURCHASE_HISTORY, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_PurchaseHistoryCallback);
		if(status == CA_REQUEST_NO_ERROR)
		{
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				HxLOG_Print("[local_caPpv_RequestPurchaseHistory] caRequestSend OKKK!! wait for call back..(%d) \n",status);
				HxLOG_Print("[local_caPpv_RequestPurchaseHistory]<---------EXIT! \n");
				return;
			}
			else
			{
				HxLOG_Print("[local_caPpv_RequestPurchaseHistory]caRequestSend ERR!!  call caRequestDispose.(%d) \n",status);
			}
		}
		else
		{
			HxLOG_Print("[local_caPpv_RequestPurchaseHistory] caRequestSetAsynchronousResponse err(%d) \n",status);
		}
	}
	else
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseHistory] caRequestCreate(CA_REQUEST_TYPE_PURCHASE_HISTORY err(%d) \n",status);
	}

	if (pRequest)
	{
		caRequestDispose(pRequest);
	}

	HxLOG_Print("[local_caPpv_RequestPurchaseHistory]caRequestDisposeEND.. send ERR noti to AP..(%d) \n",status);

	if (PurchaseHistoryNotifyCallback)
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseHistory] callback exit, send to call cack...(%d) \n",status);
		(*PurchaseHistoryNotifyCallback)(status, 0, NULL );
		HxLOG_Print("[local_caPpv_RequestPurchaseHistory] end to  callback END!!! (%d) \n",status);
	}
	else
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseHistory] caRequestCreate(CA_REQUEST_TYPE_PURCHASE_HISTORY err(%d) \n",status);
	}

	HxLOG_Print("[local_caPpv_RequestPurchaseHistory]<---------EXIT!!\n");
}

static void local_caPpv_RequestPurchaseHistoryCallBack(TCaRequest*pxRequest)
{
	TCaRequest* pRequest;
	TCaRequestStatus reqStatus;
	TCaObjectStatus objStatus;
	TCaProcessingStatus procStatus;
	TUnsignedInt32 index;
	TUnsignedInt32 purchaseEventNb;
	TCaPurchase **ppPurchase;
	TEventStatus result = CA_EVENT_SUCCESS;
	TCaProcessingStatus status;

	HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack]--------------->Enter\n");

	pRequest = pxRequest;
	reqStatus = caRequestGetProcessingStatus(pRequest, (TCaProcessingStatus*)&status);
	if ((reqStatus != CA_REQUEST_NO_ERROR) || (status != CA_PROCESSING_NO_ERROR))
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack]->Processing Status is not correct !!!(0x%x), dispose request!! \n", status);
		result = CA_EVENT_ERROR;
		goto notiToAP;
	}

	reqStatus = caRequestGetObjects(pRequest, (TUnsignedInt32*)&purchaseEventNb, (void***)&ppPurchase);
	HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] purchase number = %d, reqStatus(%d)\n", purchaseEventNb, reqStatus);

	if( (reqStatus == CA_REQUEST_NO_ERROR) && (purchaseEventNb > 0) )
	{
		purchase_list.pList = OxCAS_Malloc(purchaseEventNb*sizeof(PPV_PURCHASE_LIST));
		memset(purchase_list.pList, 0x00, purchaseEventNb*sizeof(PPV_PURCHASE_LIST));
		if  (purchase_list.pList == NULL)
		{
			result = CA_EVENT_ERROR;
			goto notiToAP;
		}

		purchase_list.xNbItems = purchaseEventNb;
		for(index=0; index<purchaseEventNb; index++)
		{
			TUnsignedInt8 *pName;
			TCaProduct *pProduct;
			TSize length;
			TCaProductId productId;

			objStatus = caPurchaseGetProduct((const TCaPurchase*)ppPurchase[index], (const TCaProduct**)&pProduct);
			if(objStatus == CA_OBJECT_NO_ERROR)
			{
				objStatus = caProductGetId((const TCaProduct*)pProduct, &productId);
				HxLOG_Print("\t[local_caPpv_RequestPurchaseHistoryCallBack] objStatus = %d, prodect id=%d\n", objStatus, productId);

				objStatus = caProductGetName((const TCaProduct*)pProduct, (TSize*)&length, (const TUnsignedInt8**)&pName);
				HxLOG_Print("\t[local_caPpv_RequestPurchaseHistoryCallBack] [%d] name(%s), length(%d), objStatus(%d)\n",index,pName,length, objStatus);

				if ( (objStatus== CA_OBJECT_NO_ERROR) && (length > 0) )
				{
					purchase_list.pList[index].EvtNameLength = length;
					if (purchase_list.pList[index].EvtNameLength  <= MAX_EVENT_ITEM_NAME_LENGTH)
					{
						MWC_UTIL_DvbStrncpy(purchase_list.pList[index].EvtName, (char*)pName, purchase_list.pList[index].EvtNameLength);
					}
					else
					{
						purchase_list.pList[index].EvtNameLength = MAX_EVENT_ITEM_NAME_LENGTH;
						MWC_UTIL_DvbStrncpy(purchase_list.pList[index].EvtName, (char*)pName, MAX_EVENT_ITEM_NAME_LENGTH);
					}
					HxLOG_Print("\t[local_caPpv_RequestPurchaseHistoryCallBack] purchase_list.pList[%d].EvtName(%s)\n",index, pName);
				}
				else
				{
					HxLOG_Print("\t[local_caPpv_RequestPurchaseHistoryCallBack][%d], getName ERROR! objStatus(%d), name length(%d)\n",
													index, objStatus, purchase_list.pList[index].EvtNameLength));

					purchase_list.pList[index].EvtNameLength = 0;
					memset(purchase_list.pList[index].EvtName, 0x00, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
				}

				caPurchaseGetDate(ppPurchase[index], &(purchase_list.pList[index].PurchaseTime));
				caPurchaseGetCost(ppPurchase[index], &(purchase_list.pList[index].Cost));
				caPurchaseGetFlags(ppPurchase[index], &(purchase_list.pList[index].PurchaseFlag));
				HxLOG_Print("\t[local_caPpv_RequestPurchaseHistoryCallBack] purchase_list.pList[%d]: time(%d), cost(%d), flag(%d)\n",index,
													purchase_list.pList[index].PurchaseTime, purchase_list.pList[index].Cost,purchase_list.pList[index].PurchaseFlag));
			}
			else
			{
				HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] [%d] caPurchaseGetProduct ERR!, set to NULL!! objStatus(%d)\n",index,objStatus);
			}
		}
	}
	else
	{
		result = CA_EVENT_ERROR;
		HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] caRequestGetObjects ERR!, purchaseEventNb(%d), reqStatus(%d), result(%d)\n", purchaseEventNb, reqStatus, result);
	}

notiToAP:
	caRequestDispose(pRequest);
	HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] caRequestDispose(pRequest); successed..., result(%d)\n", result);

#if defined(_DEBUG_)
 	local_caPpv_PrintPurchaseList(&purchase_list, __FILE__, __LINE__);
#endif

	if (PurchaseHistoryNotifyCallback)
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] try to give call back to noti .,  result(%d),  purchase_list.xNbItems[%d]\n", result,  purchase_list.xNbItems);

		(*PurchaseHistoryNotifyCallback)(result, purchase_list.xNbItems, purchase_list.pList);
		HxLOG_Print("[local_caPpv_RequestPurchaseHistoryCallBack] give call back to noti OKK..,  result(%d)\n", reqStatus);
	}
	else
	{
		HxLOG_Print("[sRequestPurchaseHistoryCallBack] no call back to give noti...,  result(%d)\n", reqStatus);
	}

	HxLOG_Print("[sRequestPurchaseHistoryCallBack]<--------------->EXIT!!\n");
}

static void local_caPpv_FlushMsgQueue(TUnsignedInt32 msgQueueId)
{
	int err;
	NaCasMsg_t msg;

	do
	{
		err = VK_WaitGetMessage(msgQueueId, (TUnsignedInt32 *)&msg, 0, QUEUE_SIZE_1);
		if (err != ERR_OK)
		{
			HxLOG_Print("Flushed message %d from %d\n", msg.lParam2, msg.wMsg);
		}
	}
	while (err == ERR_OK);
}

static HERROR  local_caPpv_DisposeRequest(TCaRequest* pRequest)
{
	if (pRequest)
	{
		TCaRequestStatus objStatus;

		HxLOG_Print("-> dispose request, pRequst = 0x%x \n", pRequest);
		objStatus = caRequestDispose(pRequest);
		HxLOG_Print("-> after caRequestDispose, objStatus = %d\n", objStatus);
		pRequest = NULL;
	}
	else
	{
		HxLOG_Print("-> dispose request, pRequst == NULL!!  \n");
	}

	HxLOG_Print("-> END!! \n");
	return CA_NO_ERROR;
}

static HERROR  local_caPpv_AdjustTimeOffset(UNIXTIME *stCalendarTime, UNIXTIME *endTime)
{
	TSignedInt32	nHour, nMin;
	HBOOL 			isPositive = FALSE;
	UNIXTIME unixtimeOffset = 0;
	UNIXTIME tStartTime = 0;
	UNIXTIME tEndTime = 0;
	TUnsignedInt32 	hRes = CA_EVENT_ERROR;

	if ( (stCalendarTime == NULL) && (endTime == NULL) )
		return CA_ERROR;

	if (stCalendarTime != NULL)
		tStartTime = *stCalendarTime;

	if (endTime != NULL)
		tEndTime = *endTime;

	hRes = HLIB_DATETIME_GetOffsetTime((TSignedInt32)&nHour, (TSignedInt32)&nMin);
	if ( hRes == CA_NO_ERROR)
	{
		// positive value 설정..
		if(nHour < 0)
		{
			isPositive = FALSE;
		}
		else if(nHour == 0)
		{
			if(nMin < 0 )
				isPositive = FALSE;
			else
				isPositive = TRUE;
		}
		else
		{
			isPositive = TRUE;
		}

		if ( isPositive )
		{
			tStartTime += unixtimeOffset;
			tEndTime += unixtimeOffset;
		}
		else
		{
			if ((tStartTime-unixtimeOffset) >= 0)
				tStartTime = tStartTime - unixtimeOffset;
			if ((tEndTime-unixtimeOffset) >= 0)
				tEndTime = tEndTime - unixtimeOffset;
		}

		if (stCalendarTime != NULL)
			*stCalendarTime = tStartTime;
		if (endTime != NULL)
			*endTime = tEndTime;

		hRes = CA_EVENT_SUCCESS;
	}
	else
	{
		HxLOG_Print("[local_caPpv_AdjustTimeOffset] :API_SYS_GetLocalTimeOffset error(%d), return FALSE!! \n", hRes);
		hRes = CA_EVENT_ERROR;
	}

	return hRes;
}

static HERROR  local_caPpv_GetPurchaseWIndow(TCaProduct *pxProduct, UNIXTIME *pnStartTime, UNIXTIME *pnEndTime, UNIXTIME *pnExpiredTime)
{
	HUINT32			hRes;
	TUnsignedInt8		index;
#if (0)
	DATE_TIME_T 	dateTime;
	TCalendarTime 	tStartTime, tEndTime;
	TCaObjectStatus	objStatus1, objStatus2;
#endif

	if ( (pnStartTime == NULL) || (pnEndTime == NULL) || (pnExpiredTime == NULL) )
	{
		if (pnStartTime != NULL) *pnStartTime = 0;
		if (pnEndTime != NULL) *pnEndTime = 0;
		if (pnExpiredTime != NULL) *pnExpiredTime = 0;
		return CA_EVENT_ERROR;
	}

#if (0)
	objStatus1 = caProductGetPurchaseStartTime(pxProduct, & tStartTime);
	objStatus2 = caProductGetPurchaseEndTime(pxProduct, &tEndTime);
	if ( (objStatus1 != CA_OBJECT_NO_ERROR) || (objStatus2 != CA_OBJECT_NO_ERROR) )
	{
		HxLOG_Print(objStatus1=%d, objStatus2=%d)\n", objStatus1, objStatus2);
		if (pnStartTime != NULL) *pnStartTime = 0;
		if (pnEndTime != NULL) *pnEndTime = 0;
		return CA_EVENT_ERROR;
	}

	if (pnStartTime != NULL)
	{
		dateTime.stDate.usYear = (unsigned short)tStartTime.year +1900;
		dateTime.stDate.ucMonth = (unsigned short)tStartTime.month;
		dateTime.stDate.ucDay= (unsigned short)tStartTime.day;
		dateTime.stTime.ucHour= (unsigned short)tStartTime.hour;
		dateTime.stTime.ucMinute= (unsigned short)tStartTime.minute;
		dateTime.stTime.ucSecond= (unsigned short)tStartTime.second;
		dateTime.stTime.usMillisecond = 0 ;
		TM_ConvertDateTimeToUnixTime(&dateTime, pnStartTime);
	}

	if (pnEndTime != NULL)
	{
		dateTime.stDate.usYear = (unsigned short)tEndTime.year +1900;
		dateTime.stDate.ucMonth = (unsigned short)tEndTime.month;
		dateTime.stDate.ucDay= (unsigned short)tEndTime.day;
		dateTime.stTime.ucHour= (unsigned short)tEndTime.hour;
		dateTime.stTime.ucMinute= (unsigned short)tEndTime.minute;
		dateTime.stTime.ucSecond= (unsigned short)tEndTime.second;
		dateTime.stTime.usMillisecond = 0 ;
		TM_ConvertDateTimeToUnixTime(&dateTime, pnEndTime);
	}
#else

	index = gPpvPurchaseInfo.xIndex;
	*pnStartTime = gEvtPurchaseInfo[index].ulStartTime;
	*pnEndTime = (gEvtPurchaseInfo[index].ulStartTime + gEvtPurchaseInfo[index].ulDuration);

	hRes = local_caPpv_AdjustTimeOffset(pnStartTime, pnEndTime);
	if (hRes == CA_EVENT_SUCCESS)
	{
		*pnEndTime = (*pnEndTime - EXPIRED_EVENT_TIME);
		*pnExpiredTime = EXPIRED_EVENT_TIME;
		if (*pnStartTime >= *pnEndTime)
		{
			*pnExpiredTime = 0;
			*pnEndTime = (gEvtPurchaseInfo[index].ulStartTime + gEvtPurchaseInfo[index].ulDuration);
		}
	}
	else
	{
		HxLOG_Print("[local_caPpv_GetPurchaseWIndow] :sAdjustTimeOffset is failed\n");
		return CA_EVENT_ERROR;
	}

#endif

	if (*pnStartTime >= *pnEndTime)
	{
		HxLOG_Print("[local_caPpv_GetPurchaseWIndow] :Start time is over End time\n");
		return CA_EVENT_ERROR;
	}

	return CA_EVENT_SUCCESS;
}

static TEventStatus local_caPpv_ProcessSpecialEvent(TCaProduct *pxProduct, TPurchasableEventType *xPurchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime)
{
	HUINT32	hRes;
	UNIXTIME nStartTime= 0, nEndTime = 0, nExpiredTime = 0, nCurrTime = 0;

	HxLOG_Print("\n\n[local_caPpv_ProcessSpecialEvent] : =========> ENTER!!\n");

	if (xPurchaseStartTime != NULL)
		*xPurchaseStartTime = 0;
	if (xPurchaseEndTime != NULL)
		*xPurchaseEndTime = 0;
	if (xPurchaseType != NULL)
		*xPurchaseType = 0;

	nCurrTime = VK_TIMER_GetSystemTick();
	if (hRes != ERR_OK)
	{
		HxLOG_Print("[local_caPpv_ProcessSpecialEvent] : API_SYS_GetSystemTime error(0x%x), EXIT!\n", hRes);
		return CA_EVENT_ERROR;
	}

	if (local_caPpv_GetPurchaseWIndow(pxProduct, &nStartTime, &nEndTime, &nExpiredTime) != CA_EVENT_SUCCESS)
	{
		HxLOG_Print("[local_caPpv_ProcessSpecialEvent] : Error in sGetPurchaseWIndow\n");
		return CA_EVENT_ERROR;
	}

	HxLOG_Print("[local_caPpv_ProcessSpecialEvent] : after sGetPurchaseWIndow nStartTime = %d, nEndTime = %d, nCurrTime = %d\n", nStartTime, nEndTime, nCurrTime);

	*xPurchaseStartTime = nStartTime;
	*xPurchaseEndTime = nEndTime;
	*xPurchaseType = CA_EVENT_SPECIAL_EVENT;

	if (nCurrTime < nStartTime)
	{
		HxLOG_Print("[local_caPpv_ProcessSpecialEvent] :: Error! CA_EVENT_PURCHASE_PERIOD_BEFORE, EXIT!\n");
		return CA_EVENT_PURCHASE_PERIOD_BEFORE;
	}
	else if (nCurrTime > (nEndTime + nExpiredTime))
	{
		HxLOG_Print("[local_caPpv_ProcessSpecialEvent] :: Error! CA_EVENT_PURCHASE_PERIOD_OVER, EXIT!\n");
		return CA_EVENT_PURCHASE_PERIOD_OVER;
	}
	else
	{
		HxLOG_Print("[local_caPpv_ProcessSpecialEvent] : In the middle of Purchase Period, EXIT!\n");
		return CA_EVENT_SUCCESS;
	}
}

static HERROR  local_caPpv_RequestEventInfoCallBack(TCaRequest* pxRequest)
{
	TCaEvent **ppEvent;
	TCaProcessingStatus status;
	TCaAccess pxAccess;
	TCaProduct **ppxProductArray;
	TEventStatus ReEventStatus = CA_EVENT_ERROR;
	TUnsignedInt32 eventNb;
	TCaObjectStatus objStatus;
	TCaRequestStatus reqStatus;

	TSize length = 0;
	TSignedInt32 nSelProduct = 0;
	TUnsignedInt8	*pName = NULL;
	TUnsignedInt32 	i;

	HxLOG_Print("--------------->Enter\n");
	HxLOG_Print("pxRequest = 0x%x\n", pxRequest);

	if (pxRequest == NULL)
	{
		HxLOG_Print("given pxRequest == NULL!!, return FALSE\n");
		ReEventStatus = CA_EVENT_ERROR;
		goto notiToAP;
	}

	memset(&gPpvPurchaseInfo.xEvtInfo, 0, sizeof(PPV_EVENT_INFO));
	reqStatus = caRequestGetProcessingStatus(pxRequest, &status);
	HxLOG_Print("reqStatus(%d), status(%d)\n", reqStatus, status);
	if ( (reqStatus != CA_REQUEST_NO_ERROR) || (status != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Print("->Processing Status is not correct !!!(0x%x), dispose request!!\n", status);
		ReEventStatus = CA_EVENT_ERROR;
		goto disposeRequest;
	}

	gPpvPurchaseInfo.xEvtInfo.xEventID = gEvtPurchaseInfo[eEIT_PF_PRESENT].nEvtId;
	gPpvPurchaseInfo.xEvtInfo.xEventStartTime = gEvtPurchaseInfo[eEIT_PF_PRESENT].ulStartTime;
	local_caPpv_AdjustTimeOffset(&gPpvPurchaseInfo.xEvtInfo.xEventStartTime, NULL);
	gPpvPurchaseInfo.xEvtInfo.duration = gEvtPurchaseInfo[eEIT_PF_PRESENT].ulDuration;
	HxLOG_Print("EvtId = 0x%x, Evt StartTime = %d, duration = %d\n", gPpvPurchaseInfo.xEvtInfo.xEventID, gPpvPurchaseInfo.xEvtInfo.xEventStartTime, gPpvPurchaseInfo.xEvtInfo.duration);

	reqStatus = caRequestGetObjects(pxRequest, &eventNb, (void***)&ppEvent);
	HxLOG_Print("eventNb = %d, reqStatus(%d)\n", eventNb, reqStatus);
	if( (reqStatus == CA_REQUEST_NO_ERROR) && (eventNb > 0) )
	{
		objStatus = caEventGetAccess(ppEvent[0], &pxAccess);
		HxLOG_Print("after caEventGetAccess, pxAccess (%d) objStatus(%d)!\n", pxAccess, objStatus);
		if (objStatus == CA_OBJECT_NO_ERROR)
		{
#if	defined(ysmoon_temp)
			if (pxAccess == CA_ACCESS_DENIED)
#else
			if (1)
#endif
			{
				TProductFlags pproductFlag;
				TUnsignedInt32 i, nPreviewTime = 0, pNumberOfProducts = 0;

				objStatus = caEventGetPreviewTime(ppEvent[0], &nPreviewTime);
				HxLOG_Print("caEventGetPreviewTime, objStatus(%d), Preview Time(%d)\n", objStatus, nPreviewTime);
				gPpvPurchaseInfo.xEvtInfo.xPreviewTime = (nPreviewTime * 1000);

				objStatus = caEventGetRelatedProducts((const TCaEvent*)ppEvent[0], (TUnsignedInt32*)&pNumberOfProducts, (const TCaProduct***)&ppxProductArray);
				HxLOG_Print("caEventGetRelatedProducts, objStatus(%d), pNumberOfProducts(%d)!\n", objStatus, pNumberOfProducts);

				for (i=0; i<pNumberOfProducts; i++)
				{
					if (ppxProductArray[i] == NULL)
					{
						HxLOG_Print("Can't HAPPEN!!, ppxProductArray[%dth] == NULL\n", i);
						continue;
					}
					objStatus = caProductGetFlags(ppxProductArray[i], &pproductFlag);
					HxLOG_Print("[%dth] after objStatus(%d), Product Flag(%d)\n", i, objStatus, pproductFlag);
					if (objStatus == CA_OBJECT_NO_ERROR)
					{
						if (pproductFlag & CA_PRODUCT_PURCHASABLE)
						{
							gPpvPurchaseInfo.xEvtInfo.xEventType = CA_EVENT_TYPE_SCRAMBLED_PPV;
							if (pproductFlag & CA_PRODUCT_IMPULSIVE)
							{
								gPpvPurchaseInfo.xEvtInfo.xEventType = CA_EVENT_TYPE_SCRAMBLED_IPPV;
							}
						}
						else
						{
							continue;
						}

						ReEventStatus = local_caPpv_ProcessSpecialEvent(ppxProductArray[i], &gPpvPurchaseInfo.xEvtInfo.xPurchaseType, &gPpvPurchaseInfo.xEvtInfo.xPurchaseStartTime, &gPpvPurchaseInfo.xEvtInfo.xPurchaseEndTime);
						if( (ReEventStatus == CA_EVENT_SUCCESS) || (ReEventStatus == CA_EVENT_PURCHASE_PERIOD_BEFORE))
						{
							HxLOG_Print("\n CA_EVENT_SPECIAL_EVENT FOUND\n");
							HxLOG_Print("\t IPPV_StartTime = %d, IPPV_EndTime = %d\n", gPpvPurchaseInfo.xEvtInfo.xPurchaseStartTime, gPpvPurchaseInfo.xEvtInfo.xPurchaseEndTime);
							HxLOG_Print("\t ppxProductArray[%d] = 0x%x\n", i, ppxProductArray[i]);

							objStatus = caProductGetCost((const TCaProduct*)ppxProductArray[i], (TCreditAmount*)&gPpvPurchaseInfo.xEvtInfo.xCost);
							HxLOG_Print("\t objStatus = %d, IPPV_Cost = %d\n", objStatus, gPpvPurchaseInfo.xEvtInfo.xCost);

							objStatus = caProductGetName((const TCaProduct*)ppxProductArray[i], (TSize*)&length, (const TUnsignedInt8**)&pName);
							if ( (objStatus == CA_OBJECT_NO_ERROR) && (pName) && (length > 0) )
							{
								HxLOG_Print("\t pName = %s, length = %d, objStatus = %d\n",pName,  length, objStatus);
								memset(gPpvPurchaseInfo.xEvtInfo.xEvtName, 0, (MAX_EVENT_ITEM_NAME_LENGTH+1));
								if ( (objStatus== CA_OBJECT_NO_ERROR) && (length > 0) )
								{
									if (length < MAX_EVENT_ITEM_NAME_LENGTH)
										memcpy((void*)gPpvPurchaseInfo.xEvtInfo.xEvtName, (void*)pName, length);
									else
										memcpy((void*)gPpvPurchaseInfo.xEvtInfo.xEvtName, (void*)pName, MAX_EVENT_ITEM_NAME_LENGTH);

									HxLOG_Print("\t IPPV_EvtName = %s\n", gPpvPurchaseInfo.xEvtInfo.xEvtName);
								}
							}
							else
							{
								HxLOG_Print("\t pName= NULL, length = %d, objStatus=%d\n", length, objStatus);
							}

							gPpvPurchaseInfo.xEvtInfo.xSliceTime = 0;
							nSelProduct = i;

							gPpvPurchaseInfo.xRequest = (TCaRequest*)pxRequest;
							gPpvPurchaseInfo.xProduct = (TCaProduct*)ppxProductArray[nSelProduct];

							if (gPpvPurchaseInfo.xEvtInfo.xEventType == CA_EVENT_TYPE_SCRAMBLED_IPPV)
								goto notiToAP;
							else
								goto disposeRequest;
						}
						else if (ReEventStatus == CA_EVENT_ERROR)
						{
							HxLOG_Print("[%dth] sProcessSpecialEvent has no vaild purchase window, look for NEXT!! \n", i);
						}
						else
						{
							HxLOG_Print("[%dth] sProcessSpecialEvent period not valid %d!!, dispose request!! \n", i, ReEventStatus);
							goto disposeRequest;
						}
						HxLOG_Print("in for (i=%d) no IPPV avail!!\n", i);
					}
				}//for (i=0;i < pNumberOfProducts; i++)
			}//pxAccess != CA_ACCESS_DENIED
			else
			{
				HxLOG_Print("caEventGetRelatedProducts ERROR!!(objStatus = %d),(pxAccess= %d)\n", objStatus, pxAccess);
			}
		}// caEventGetAccess
		else
		{
			HxLOG_Print("caEventGetAccess ERROR!!(objStatus=%d)\n",objStatus);
		}
	}//	if( (reqStatus == CA_REQUEST_NO_ERROR) && (eventNb > 0) )
	else
	{
		HxLOG_Print("[sRequestEventInfoCallBack] caRequestGetObjects ERROR!!(reqStatus=%d)\n",reqStatus);
	}

disposeRequest:
	local_caPpv_DisposeRequest(pxRequest);
	gPpvPurchaseInfo.xRequest = NULL;

notiToAP:

	if (RequestEventNotifyCallback)
	{
		HxLOG_Print("\nNotify to AP >>>>> \n");
		HxLOG_Print("\tServiceHandle = 0x%x\n", gPpvPurchaseInfo.xSvcHandle);
		HxLOG_Print("\tRequestId = 0x%x\n", gPpvPurchaseInfo.xRequest);
		HxLOG_Print("\tProduct = 0x%\n", gPpvPurchaseInfo.xProduct);
		HxLOG_Print("\tReturn Value = 0x%x\n\n",ReEventStatus);
		(*RequestEventNotifyCallback)((TUnsignedInt32)ReEventStatus, gPpvPurchaseInfo.xSvcHandle, gPpvPurchaseInfo.xRequest, &gPpvPurchaseInfo.xEvtInfo, gPpvPurchaseInfo.xProduct);
	}

	HxLOG_Print("<--------------->EXIT!!\n");
	return ERR_OK;
}

static HERROR  local_caPpv_RequestEventInfo(void)
{
	TCaRequestStatus	status;
	TCaRequest			*pRequest = NULL;
	TUnsignedInt32 		i;


	HxLOG_Print(" -------> ENTER!!\n");
	if (gPpvPurchaseInfo.xRequest)
	{
		local_caPpv_DisposeRequest(gPpvPurchaseInfo.xRequest);
		gPpvPurchaseInfo.xRequest = NULL;
	}

	status = caRequestCreate(CA_REQUEST_TYPE_EVENT, &pRequest);
	if (status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_PresentEventInfoCallback);
		if (status == CA_REQUEST_NO_ERROR)
		{
			HxLOG_Print("Event Start Time (Y=%d),(M=%d)(D=%d)(H=%d)(M=%d)\n",
				(gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime.year+1900), gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime.month, gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime.day, gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime.hour, gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime.minute));
			HxLOG_Print("Event Name (%s), Desc Length(%d)\n", gEvtPurchaseInfo[eEIT_PF_PRESENT].ucEvtName, gEvtPurchaseInfo[eEIT_PF_PRESENT].ulDesLength);

			status = caRequestSetStartTime(pRequest, &gEvtPurchaseInfo[eEIT_PF_PRESENT].stCalendarTime);
			HxLOG_Print("caRequestSetStartTime status = %d\n", status);

			status = caRequestSetDescriptors(pRequest, gEvtPurchaseInfo[eEIT_PF_PRESENT].ulDesLength, gEvtPurchaseInfo[eEIT_PF_PRESENT].ucDesInfo);
			HxLOG_Print("caRequestSetDescriptors status = %d\n", status);

			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				HxLOG_Print("caRequestSend OK : Request = 0x%x!!\n", pRequest);
				HxLOG_Print("<---------EXIT! \n");
				return status;
			}
			else
			{
				HxLOG_Print("caRequestSend ERR!!  call caRequestDispose.(%d) \n", status);
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse err(%d) \n", status);
		}
	}
	else
	{
		HxLOG_Print("caRequestCreate(CA_REQUEST_TYPE_EVENT) err(%d) \n", status);
	}

	local_caPpv_DisposeRequest(pRequest);
	HxLOG_Print("caRequestDisposeEND.. send ERR noti to AP..(%d) \n", status);

	//일단 여기까지 오면 ERROR 다.. callback이 있으면 AP에게 알려라...
	if (RequestEventNotifyCallback)
	{
		HxLOG_Print("[%d] exit, send to call back...ERR(%d) \n", i, status);
		(*RequestEventNotifyCallback)((TUnsignedInt32)status, NULL, NULL, NULL, NULL); //뭐 딴 것  줄 것 있나?
	}

	HxLOG_Info("<---------EXIT\n");
	return ERR_OK;
}

static HERROR  local_caPpv_RequestPurchaseSpecialEvent(TCaProduct *pProduct, TCaProcessingStatus *pProcessingStatus)
{
	TCaRequest*pRequest = NULL;
	TCaRequestStatus reqStatus = CA_REQUEST_ERROR;
	TCaProcessingStatus procStatus = CA_PROCESSING_ERROR;
	static TUnsignedInt8 sInPurchasing = 0;

	if (sInPurchasing)
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]-> Already Purchasing requested...\n");
		return ERR_FAIL;
	}

	sInPurchasing = 1;
	HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent] ------>Enter\n");

	reqStatus = caRequestCreate(CA_REQUEST_TYPE_PRODUCT_PURCHASE, &pRequest);
	if (reqStatus == CA_REQUEST_NO_ERROR)
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent] pProduct = 0x%x\n", pProduct);
		reqStatus = caRequestSetProduct(pRequest, pProduct);
		HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent] pProduct222 = 0x%x\n", pProduct);
		if (reqStatus == CA_REQUEST_NO_ERROR)
		{
			reqStatus = caRequestSend(pRequest);
			if (reqStatus == CA_REQUEST_NO_ERROR)
			{
				reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
				if ( (reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_NO_ERROR) )
				{
					HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]--!! caRequestGetProcessingStatus OKKK!!\n");
				}
				else
				{
					HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]--!! caRequestGetProcessingStatus ERR!! procStatus(%d), reqStatus(%d)\n", procStatus, reqStatus);
				}
			}
			else
			{
				HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]--caRequestSend ERROR (%d) \n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]--caRequestSetProduct ERROR (%d) \n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent]--caRequestCreate(CA_REQUEST_TYPE_PRODUCT_PURCHASE) ERROR (%d) \n", reqStatus);
	}

	if (reqStatus != CA_REQUEST_NO_ERROR)
		procStatus = CA_PROCESSING_ERROR;

	*pProcessingStatus = procStatus;
	local_caPpv_DisposeRequest(pRequest);
	sInPurchasing = 0;

	HxLOG_Print("[local_caPpv_RequestPurchaseSpecialEvent] <------Exit\n");

	return ERR_OK;
}

static HERROR local_caPpv_WriteNewCreditFlag(TUnsignedInt8 bFlag)
{
	HERROR hErr;

	HxLOG_Print("[local_caPpv_WriteNewCreditFlag] bFlage = (%d)\n", bFlag);

	hErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CREDIT_INFO_OFFSET, sizeof(TUnsignedInt8), (TUnsignedInt8*)&bFlag);
	if (hErr != ERR_OK )
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_caPpv_ReadNewCreditFlag(TUnsignedInt8 *pbFlag)
{
	HERROR hErr;
	TUnsignedInt8 bFlag;

	hErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CREDIT_INFO_OFFSET, sizeof(TUnsignedInt8), (TUnsignedInt8*)&bFlag) ;
	if (hErr != ERR_OK )
	{
		return ERR_FAIL;
	}

	*pbFlag = bFlag;
	HxLOG_Print("[local_caPpv_ReadNewCreditFlag] bFlage = (%d)\n", *pbFlag);

	return ERR_OK;
}

#define	______CA_CAK_PPV_Local_Function_Body_____

static void local_caPpv_SendCakMsg(TUnsignedInt8 ucMsg, TUnsignedInt16 param1, TUnsignedInt8 *param2, TUnsignedInt32 param3)
{
	switch(ucMsg)
	{
		case CA_PPV_CALLBACK_CREDIT:
			{
				if (gCreditCallBackFn != NULL)
					(*gCreditCallBackFn)(gCreditStatus, gCreditItemNb, credit_list.pList);

				HxLOG_Print("[local_caPpv_SendCakMsg] Sent CA_PPV_CALLBACK_CREDIT msg\n");
			}
			break;

		case CA_PPV_CALLBACK_PPV_LIST:
			{
				if (gPpvListCallBackFn != NULL)
					(*gPpvListCallBackFn)(gPpvListStatus, gPpvListItemNb, ppv_list.pList);

				HxLOG_Print("[local_caPpv_SendCakMsg] Sent CA_PPV_CALLBACK_PPV_LIST msg\n");
			}
			break;

		default:
			break;
	}
}

static void local_caPpv_RequestCmd(NaCasMsg_t *pstNaMsg)
{
	TUnsignedInt16	usReqId ;

	usReqId = pstNaMsg->lParam2;

	switch(usReqId)
	{
	case CAK_PPV_LIST_REQ_ID:
		HxLOG_Print("[local_caPpv_RequestCmd] -> CAK_PPV_LIST_REQ_ID\n");
		if( caEventPurchaseListRequest( (TEventPurchaseListCallback)local_caPpv_PpvListCallback ) != OS_NO_ERROR )
		{
			HxLOG_Print("[local_caPpv_RequestCmd] Error in requesting PPV list\n");
		}
		else
		{
			gPpvListReqFlag = TRUE;
		}
		break;

	case CAK_CREDIT_REQ_ID:
		HxLOG_Print("[local_caPpv_RequestCmd] -> CAK_CREDIT_REQ_ID\n");
		if( caCreditRequest( (TCreditCallback)local_caPpv_CreditCallback ) != OS_NO_ERROR )
		{
			HxLOG_Print("[local_caPpv_RequestCmd] Error in requesting Credit\n");
		}
		else
		{
			gCreditListReqFlag = TRUE;
		}
		break;
	}
}

static void local_caPpv_ProcessFilterCmd(NaCasMsg_t *pstNaMsg)
{
	TUnsignedInt16 	usFilterType;
	TUnsignedInt32	hSvchandle;
	HERROR 			hErr;

	usFilterType = pstNaMsg->lParam2;
	hSvchandle = pstNaMsg->lParam1;

	HxLOG_Print("usFilterType(%d) hSvchandle(0x%x) \n", usFilterType, hSvchandle);
	HxLOG_Print("EIT_STATE(%d) TDT_STATE(%d) EPG_MODE_STATE(%d)\n", EIT_STATE,TDT_STATE, EPG_MODE_STATE);

	switch(usFilterType)
	{
		case CAK_FILTER_EIT_PF_ARRIVAL:
			{
				EIT_STATE = 1;

				if ( (EPG_MODE_STATE == 1) || (TDT_STATE == 0) )
					break;

				hErr = local_caPpv_MakeIppvEventInfo(hSvchandle);
				if (hErr == ERR_OK)
				{
					HxLOG_Print("CAK_FILTER_EIT_PF_ARRIVAL : SUCCESS, call local_caPpv_RequestEventInfo!!\n");

					local_caPpv_RequestEventInfo();
				}
				else
				{
					HxLOG_Print("CAK_FILTER_EIT_PF_ARRIVAL : result(%d) FAILLL!!!\n", hErr);
				}
			}

			break;

		case CAK_FILTER_TDT_ARRIVAL:
			{
#if	1
				if (TDT_STATE == 0)
				{
					TDT_STATE = 1;
					CA_NA_PPV_NotifyEITpfArrived (hSvchandle);	// Fake Notify.
				}
#else
				TDT_STATE = 1;

				if ( (EPG_MODE_STATE == 1) || (EIT_STATE == 0) )
						break;

				hErr = local_caPpv_MakeIppvEventInfo(hSvchandle);
				if (hErr == ERR_OK)
				{
					HxLOG_Print("CAK_FILTER_TDT_ARRIVAL : SUCCESS, call AXMD_NA_OnEitPfUpdated!!\n");

					local_caPpv_RequestEventInfo();
				}
				else
				{
					HxLOG_Print("CAK_FILTER_TDT_ARRIVAL : result(%d) FAILLL!!!\n", hErr);
				}
#endif
			}
			break;

		default:
			break;
	}
}

static void local_caPpv_ProcessingPpvCallback(NaCasMsg_t *pstNaMsg)
{
	TUnsignedInt16	usCallback ;

	usCallback = pstNaMsg->lParam2;

	switch(usCallback)
	{
		case CAK_PPV_LIST_NOTIFICATION:
			HxLOG_Print("-> CAK_PPV_LIST_NOTIFICATION\n");
			if( caEventPurchaseListRequest( (TEventPurchaseListCallback)local_caPpv_PpvListCallback ) != OS_NO_ERROR )
			{
				HxLOG_Print(" Error in requesting PPV list\n");
			}
			break;

		case CAK_CREDIT_NOTIFICATION:
			HxLOG_Print("-> CAK_CREDIT_NOTIFICATION\n");
			if( caCreditRequest( (TCreditCallback)local_caPpv_CreditCallback ) != OS_NO_ERROR )
			{
				HxLOG_Print("Error in requesting Credit\n");
			}
			break;

		case CAK_PPV_LIST_CALL_BACK:
			HxLOG_Print("-> CAK_PPV_LIST_CALL_BACK\n");
			{
				TUnsignedInt8 index;
				TEventPurchaseListItem *pItem;
				TEventPurchaseListStatus status;
				TUnsignedInt8 ppvListItemNb;

				ppvListItemNb = gPpvListItemNb;
				gPpvListStatus = status = pstNaMsg->info;
				gPpvListItemNb = (TUnsignedInt8)pstNaMsg->lParam3;
				if (status == CA_EVENT_PURCHASE_LIST_SUCCESS)
				{
					local_caPpv_FreePpvList();
					local_caPpv_InitPpvList();
					if (pstNaMsg->lParam3)
					{
						local_caPpv_AllocatePpvArray((TUnsignedInt8)pstNaMsg->lParam3);
					}

					pItem = (TEventPurchaseListItem *)pstNaMsg->hWnd;
					for(index=0; index<pstNaMsg->lParam3; index++)
					{
						if (pItem == NULL)
						{
							HxLOG_Print("pItme is NULL, but this should be non null\n");
							break;
						}
						local_caPpv_AllocatePpvList(pItem, (TUnsignedInt8)index);
						pItem++ ;
					}
					caEventPurchaseListAcknowledgement((TEventPurchaseListItem *)pstNaMsg->hWnd);

					if ( (!PPV_LIST_STATE) || (gPpvListReqFlag) || (ppvListItemNb != gPpvListItemNb) )
					{
						local_caPpv_SendCakMsg(CA_PPV_CALLBACK_PPV_LIST, (TUnsignedInt16)status, NULL, 0);
					}

					if (gPpvListReqFlag == TRUE)
					{
						 gPpvListReqFlag = FALSE;
					}

					PPV_LIST_STATE = 1;
				}
				else
				{
					HxLOG_Print("Error in receiving PPV list\n");
					local_caPpv_SendCakMsg(CA_PPV_CALLBACK_PPV_LIST, (TUnsignedInt16)status, NULL, 0);
					gPpvListReqFlag = FALSE;
				}
			}
			break;

		case CAK_CREDIT_CALL_BACK:
			HxLOG_Print("-> CAK_CREDIT_CALL_BACK\n");
			{
				TCreditItem *pItem;
				TCreditStatus status;
				TUnsignedInt8 index;
				TCreditAmount credit;

				credit = 0;
				gCreditStatus = status = pstNaMsg->info;
				if (status == CA_CREDIT_SUCCESS)
				{
					if (CREDIT_STATE)
						credit = credit_list.pList[0].credit;

					local_caPpv_FreeCreditList();
					local_caPpv_InitCreditList();
					gCreditItemNb = (TUnsignedInt8)(pstNaMsg->lParam3 > 0) ? pstNaMsg->lParam3 : 1;
					local_caPpv_AllocateCreditArray(gCreditItemNb);

					if (pstNaMsg->lParam3 > 0)
					{
						pItem = (TCreditItem *)pstNaMsg->hWnd;
						for (index=0; index<pstNaMsg->lParam3; index++)
						{
							local_caPpv_AllocateCreditList((TUnsignedInt8)index, pItem);
							pItem++;
						}
					}
					else
					{
						pItem = (TCreditItem *)OxCAS_Malloc(sizeof(TCreditItem));
						if (pItem)
						{
							memset((char*)pItem, 0, sizeof(TCreditItem));
							local_caPpv_AllocateCreditList(0, pItem);
							OxCAS_Free(pItem);
						}
					}
					caCreditAcknowledgement((TCreditItem *)pstNaMsg->hWnd);

					if ( (!CREDIT_STATE) ||(gCreditListReqFlag) ||(credit != credit_list.pList[0].credit) )
					{
						local_caPpv_SendCakMsg(CA_PPV_CALLBACK_CREDIT, (TUnsignedInt16)status, NULL, 0);
					}

					if (gCreditListReqFlag == TRUE)
					{
						 gCreditListReqFlag = FALSE;
					}
					else
					{
						if ( (CREDIT_STATE) && (credit < credit_list.pList[0].credit) )
						{
							local_caPpv_WriteNewCreditFlag((TUnsignedInt8)TRUE);

							if (RechargeNotifyCallbackFn)
							{
								(*RechargeNotifyCallbackFn)(CA_REQUEST_NO_ERROR, credit_list.pList[0].credit, 0);
							}
							HxLOG_Print("->CAK_CREDIT_CALL_BACK : old Credit = %d, Recharge Credit = %d\n", credit, credit_list.pList[0].credit);
						}
					}
					CREDIT_STATE = 1;
				}
				else
				{
					HxLOG_Print("->CAK_CREDIT_CALL_BACK : Error in receiving Credit list\n");
					 gCreditListReqFlag = FALSE;
					 local_caPpv_SendCakMsg(CA_PPV_CALLBACK_CREDIT, (TUnsignedInt16)status, NULL, 0);
				}
			}
			break;

		case CAK_REQUEST_PRESENT_EVENT_CALL_BACK:
			HxLOG_Print("-> CAK_REQUEST_PRESENT_EVENT_CALL_BACK\n");
			local_caPpv_RequestEventInfoCallBack((TCaRequest*)pstNaMsg->lParam1);
			break;

		case CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK:
			HxLOG_Print("-> CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK\n");
			local_caPpv_RequestEventInfoCallBack((TCaRequest*)pstNaMsg->lParam1);
			break;

		case CAK_REQUEST_PURCHASE_HISTORY:
			HxLOG_Print("-> CAK_REQUEST_PURCHASE_HISTORY\n");
			local_caPpv_RequestPurchaseHistory();
			break;

		case CAK_PURCHASE_HISTORY_CALL_BACK:
			HxLOG_Print("-> CAK_PURCHASE_HISTORY_CALL_BACK\n");
			local_caPpv_RequestPurchaseHistoryCallBack((TCaRequest*) pstNaMsg->lParam1);
			break;
	}
}

static void local_caPpv_Task(void* p)
{
	NaCasMsg_t 	stMsg;
	HERROR		hErr;

	while(1)
	{
		if (VK_GetMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1) != ERR_OK)
		{
			continue;
		}

		switch (stMsg.wMsg)
		{
			HxLOG_Print("[local_caPpv_Task] stMsg.wMsg (%d) stMsg.lParam2(%d) \n", stMsg.wMsg, stMsg.lParam2);
			case NA_CAK_ID:
				switch (stMsg.lParam2)
				{
					case CAK_PPV_LIST_NOTIFICATION:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_CREDIT_NOTIFICATION:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_REQUEST_PRESENT_EVENT_CALL_BACK:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_PPV_LIST_CALL_BACK:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_CREDIT_CALL_BACK:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_PURCHASE_HISTORY_CALL_BACK:
						local_caPpv_ProcessingPpvCallback(&stMsg);
						break;

					case CAK_EXPIRATION_CALL_BACK:
						break;


				}
				break;

			case NA_FILTER_ID:
				switch (stMsg.lParam2)
				{
					case CAK_FILTER_EIT_PF_ARRIVAL:
						local_caPpv_ProcessFilterCmd(&stMsg);
						break;

					case CAK_FILTER_TDT_ARRIVAL:
						local_caPpv_ProcessFilterCmd(&stMsg);
						break;
				}
				break;

			case NA_SERVICE_ID:
				switch (stMsg.lParam2)
				{
				case CAK_SERVICE_CHANGE_TP:
					break;

				case CAK_SERVICE_CHANGE_CHANNEL:
					break;
				}
				break;

			case NA_APP_ID :
				local_caPpv_RequestCmd(&stMsg);
				break;

			default:
				break;
		}
	}
}


/**************************************************************************************************/
#define ________CA_CAK_PPV_Public_Functions_Body___________________________________________
/**************************************************************************************************/

HERROR CA_NA_PPV_Startup(void)
{
	TSignedInt32 nErrCode;

	nErrCode = VK_CreateMessageQueue(256, &s_ulPpvQueueId, NA_PPV_MSG_Q_NAME, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_CreateMessageQueue (%ld)\n", nErrCode);
		return CA_ERROR;
	}

	nErrCode = VK_TASK_Create(local_caPpv_Task, NA_CAK_TASK_PRIORITY, NA_CAK_TASK_STACK_SIZE, NA_PPV_TASK_NAME, NULL, &s_ulPpvTaskId, 0);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in creating PPV thread %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_TASK_Start(s_ulPpvTaskId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in starting UIT thread %ld\n", nErrCode);
		return ERR_FAIL;
	}

	local_caPpv_initPpv();

	return ERR_OK;
}

HERROR CA_NA_PPV_Request(TUnsignedInt32 ulReqId)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_APP_ID;
	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= (TUnsignedInt16)ulReqId;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR  CA_NA_PPV_SetEpgMode(TBoolean xEpgMode)
{
	EPG_MODE_STATE = (TBoolean)xEpgMode;

	if (EPG_MODE_STATE == 1)
		local_caPpv_ClearPurchaseInfo();

	return ERR_OK;
}

HERROR  CA_NA_PPV_ForceCreatEventInfo(TUnsignedInt32 hSvchandle)
{
	HUINT32 result;

	if (TDT_STATE == 0)
		return ERR_FAIL;

	result = local_caPpv_MakeIppvEventInfo(hSvchandle);

	if (result == ERR_OK)
		HxLOG_Print("[CA_NA_PPV_ForceCreatEventInfo] SUCCESS, call AXMD_NA_OnEitPfUpdated!!\n");
	else
		HxLOG_Print("[CA_NA_PPV_ForceCreatEventInfo] result(%d) FAILLL!!!\n", result);
	return ERR_OK;
}

HERROR  CA_NA_PPV_GetPpvList(TEventPurchaseListStatus* xStatus, TUnsignedInt8* xNumberOfItems, TEventPurchaseListItem** pxEventPurchaseListArray)
{
	if (PPV_LIST_STATE == 0)
		return ERR_FAIL;

	*xNumberOfItems = gPpvListItemNb;
	*pxEventPurchaseListArray = ppv_list.pList;
	*xStatus = gPpvListStatus;

	return ERR_OK;
}

HERROR  CA_NA_PPV_GetCredit(TCreditStatus* xStatus, TUnsignedInt8* xNumberOfItems, TCreditItem** pxCreditArray)
{
	if (CREDIT_STATE == 0)
		return ERR_FAIL;

	*xNumberOfItems = gCreditItemNb;
	*pxCreditArray = credit_list.pList;
	*xStatus = gCreditStatus;

	return ERR_OK;
}

HERROR 	CA_NA_PPV_GetPurchaseEvtInfo(TUnsignedInt32 svcHandle, TUnsignedInt32 currTime, TUnsignedInt16 *eventId)
{
	unsigned char* pDesRaw;
	unsigned char i, desTag, desLen;
	unsigned long endTime, des_total_len = 0;

	if (eventId == NULL)
	{
		HxLOG_Print("NULL event Id \n");
		return CA_ERROR;
	}

	if (gPpvPurchaseInfo.xSvcHandle != svcHandle)
	{
		HxLOG_Print("Wrong Service Handle (0x%x, 0x%x)\n", gPpvPurchaseInfo.xSvcHandle, svcHandle);
		return CA_ERROR;
	}

	for (i=0; i<2; i++)
	{
		if (gEvtPurchaseInfo[i].fPresent)
		{
			endTime = gEvtPurchaseInfo[i].ulStartTime + gEvtPurchaseInfo[i].ulDuration;
			local_caPpv_AdjustTimeOffset(NULL, (UNIXTIME*)&endTime);
			if ((UNIXTIME)endTime > (UNIXTIME)currTime)
				break;
		}
	}

	if (i >= 2)
	{
		HxLOG_Print("Not Found Event (Service Handle = 0x%x)\n", svcHandle);
		return CA_ERROR;
	}

	HxLOG_Print("EVT_ID (%d)\n", gEvtPurchaseInfo[i].nEvtId);
	HxLOG_Print("EVT_Des Langth (%d)\n", gEvtPurchaseInfo[i].ulDesLength);
	HxLOG_Print("EVT_NAME = %s\n", gEvtPurchaseInfo[i].ucEvtName);
	HxLOG_Print("EVT_START Time (Y=%d),(M=%d)(D=%d)(H=%d)(M=%d)\n", (gEvtPurchaseInfo[i].stCalendarTime.year+1900), gEvtPurchaseInfo[i].stCalendarTime.month, gEvtPurchaseInfo[i].stCalendarTime.day, gEvtPurchaseInfo[i].stCalendarTime.hour, gEvtPurchaseInfo[i].stCalendarTime.minute);

	*eventId = gEvtPurchaseInfo[i].nEvtId;
	des_total_len = gEvtPurchaseInfo[i].ulDesLength;
	pDesRaw = gEvtPurchaseInfo[i].ucDesInfo;

	HxLOG_Print("CA_NA_PPV_PPV_GetPurchaseEvtInfo-> Total length = %d\n", des_total_len);
	while(des_total_len > 0)
	{
		desTag = *pDesRaw++;
		HxLOG_Print("CA_NA_PPV_PPV_GetPurchaseEvtInfo-> ucDesInfo tag = %x\n", desTag);
		desLen = *pDesRaw++;
		HxLOG_Print("CA_NA_PPV_PPV_GetPurchaseEvtInfo-> ucDesInfo len = %x\n", desLen);

		if (desTag == 0x00 || desLen <= 0) //잘못된 eit data가 들어올 경우 처리
			break;

		if (desTag != NASP_PPV_DESC)
		{
			des_total_len -= (desLen+2);
			pDesRaw += desLen;
			continue;
		}
		HxLOG_Print("CA_NA_PPV_PPV_GetPurchaseEvtInfo-> event Id = %x\n", *eventId);
		return CA_NO_ERROR;
	}
	return CA_ERROR;
}

HERROR  CA_NA_PPV_RequestEventInfo(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId)
{
	NaCasMsg_t msg;
	int err, inx;

	HxLOG_Print("[CA_NA_PPV_RequestEventInfo]\n");

	if (gPpvPurchaseInfo.xSvcHandle != svcHandle)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestEventInfo] Wrong ServicecHandle(0x%x, 0x%x)\n", gPpvPurchaseInfo.xSvcHandle, svcHandle);
		return ERR_FAIL;
	}

	inx = local_caPpv_GetEventInfoWithEvtId(nEvtId);
	if (inx == -1)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestEventInfo] Not found event info(nEvtId = 0x%x)\n", nEvtId);
		return ERR_FAIL;
	}

	gPpvPurchaseInfo.xIndex = (TUnsignedInt8)inx;
	local_caPpv_FlushMsgQueue(s_ulEvQueueId);
	local_caPpv_RequestEventInfo();

	if (VK_WaitGetMessage(s_ulEvQueueId, (TUnsignedInt32 *)&msg, 5000, QUEUE_SIZE_1) == ERR_OK)
	{
		if (msg.lParam2 == APP_EVT_INFO_COMPLETE)
		{
			if (RequestEventNotifyCallback)
			{
				HxLOG_Print("\n\n\t[CA_NA_PPV_PPV_RequestEventInfo] ServiceHandle = 0x%x\n", gPpvPurchaseInfo.xSvcHandle);
				HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfo] RequestId = 0x%x\n", gPpvPurchaseInfo.xRequest);
				HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfo] Product = 0x%\nx", gPpvPurchaseInfo.xProduct);
				HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfo] Return Value = 0x%x\n\n", msg.lParam1);
				(*RequestEventNotifyCallback)((TUnsignedInt32)msg.lParam1, gPpvPurchaseInfo.xSvcHandle, gPpvPurchaseInfo.xRequest, &gPpvPurchaseInfo.xEvtInfo, gPpvPurchaseInfo.xProduct);
			}
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR  CA_NA_PPV_RequestEventInfoByEpg(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId,
															TUnsignedInt32 *xStatus, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct)
{
	NaCasMsg_t msg;
	int err, inx;
	UNIXTIME pnExpiredTime;

	HxLOG_Print("[CA_NA_PPV_PPV_RequestEventInfoByEpg]\n");

	local_caPpv_MakeIppvEventInfoByEpg(svcHandle, nEvtId);
	inx = local_caPpv_GetEventInfoWithEvtId(nEvtId);
	if (inx == -1)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestEventInfoByEpg] Not found event info(nEvtId = 0x%x)\n", nEvtId);
		return ERR_FAIL;
	}

	gPpvPurchaseInfo.xIndex = (TUnsignedInt8)inx;
	local_caPpv_FlushMsgQueue(s_ulEvQueueId);
	if (local_caPpv_GetPurchaseWIndow((TCaProduct*)gPpvPurchaseInfo.xProduct,
			(UNIXTIME*)&gPpvPurchaseInfo.xEvtInfo.xPurchaseStartTime, (UNIXTIME*)&gPpvPurchaseInfo.xEvtInfo.xPurchaseEndTime, (UNIXTIME*)&pnExpiredTime) != CA_EVENT_SUCCESS)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestEventInfoByEpg] sGetPurchaseWIndow is failed\n");
		return ERR_FAIL;
	}
	local_caPpv_RequestEventInfo();

	if (VK_WaitGetMessage(s_ulEvQueueId, (TUnsignedInt32 *)&msg, 5000, QUEUE_SIZE_1) == ERR_OK)
	{
		if (msg.lParam2 == APP_EVT_INFO_COMPLETE)
		{
			HxLOG_Print("\n\n\t[CA_NA_PPV_PPV_RequestEventInfoByEpg] ServiceHandle = 0x%x\n", gPpvPurchaseInfo.xSvcHandle);
			HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfoByEpg] RequestId = 0x%x\n", gPpvPurchaseInfo.xRequest);
			HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfoByEpg] Product = 0x%x\nx", gPpvPurchaseInfo.xProduct);
			HxLOG_Print("\t[CA_NA_PPV_PPV_RequestEventInfoByEpg] Return Value = 0x%x\n\n", msg.lParam1);

			*xStatus = (TUnsignedInt32)msg.lParam1;
			*pRequest =  gPpvPurchaseInfo.xRequest;
			*pEventInfo = &gPpvPurchaseInfo.xEvtInfo;
			*pProduct = gPpvPurchaseInfo.xProduct;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

HERROR  CA_NA_PPV_RequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct)
{
	TCaObjectStatus objStatus;
	TCaProcessingStatus procStatus = CA_PROCESSING_ERROR;

	if (pRequest == NULL)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_requestPurchaseEvent] pRequest is Null...\n");
		goto notiToAP;
	}

	if (pProduct == NULL)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_requestPurchaseEvent] pProduct is Null...\n");
		goto disposeRequest;
	}

	if ( (pRequest != gPpvPurchaseInfo.xRequest) || (pProduct != gPpvPurchaseInfo.xProduct) )
	{
		HxLOG_Print("[CA_NA_PPV_PPV_requestPurchaseEvent] Wrong Request or Product...\n");
		goto disposeRequest;
	}

	HxLOG_Print("\n\n[CA_NA_PPV_PPV_requestPurchaseEvent] (xType = %d, pProduct = 0x%x, pRequest = 0x%x)\n", xType, pProduct, pRequest);
	switch (xType)
	{
		case CA_EVENT_SPECIAL_EVENT:
			HxLOG_Print("[CA_NA_PPV_PPV_requestPurchaseEvent] xType = CA_EVENT_SPECIAL_EVENT(pProduct = 0x%x, pRequest = 0x%x)\n", pProduct, pRequest);
			if (local_caPpv_RequestPurchaseSpecialEvent(pProduct, &procStatus) != ERR_OK)
				return ERR_FAIL;
			break;

		case CA_EVENT_PPT:
		default:
			HxLOG_Print("[CA_NA_PPV_PPV_requestPurchaseEvent] xType = %d\n", xType);
			break;
	}

disposeRequest:
	local_caPpv_DisposeRequest(pRequest);
	gPpvPurchaseInfo.xRequest = NULL;

notiToAP:
	if (PurchaseEventNotifyCallback)
		(*PurchaseEventNotifyCallback)(procStatus, CA_EVENT_SPECIAL_EVENT);

	return ERR_OK;
}

HERROR  CA_NA_PPV_RequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus)
{
	TCaObjectStatus objStatus;
	TCaProcessingStatus procStatus = CA_PROCESSING_ERROR;

	if (pRequest == NULL)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] pRequest is Null...\n");
		goto notiToAP;
	}

	if (pProduct == NULL)
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] pProduct is Null...\n");
		goto disposeRequest;
	}

	if ( (pRequest != gPpvPurchaseInfo.xRequest) || (pProduct != gPpvPurchaseInfo.xProduct) )
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] Wrong Request or Product...\n");
		goto disposeRequest;
	}

	HxLOG_Print("\n\n[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] (xType = %d, pProduct = 0x%x, pRequest = 0x%x)\n", xType, pProduct, pRequest);
	switch (xType)
	{
		case CA_EVENT_SPECIAL_EVENT:
			HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] xType = CA_EVENT_SPECIAL_EVENT(pProduct = 0x%x, pRequest = 0x%x)\n", pProduct, pRequest);
			if (local_caPpv_RequestPurchaseSpecialEvent(pProduct, &procStatus) != ERR_OK)
				return ERR_FAIL;
			break;

		case CA_EVENT_PPT:
		default:
			HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseEventByEpg] xType = %d\n", xType);
			break;
	}

disposeRequest:
	local_caPpv_DisposeRequest(pRequest);
	gPpvPurchaseInfo.xRequest = NULL;

notiToAP:
	*pProStatus = procStatus;
	return ERR_OK;
}

HERROR  CA_NA_PPV_RequestPurchaseHistory(void)
{
	int err;
	NaCasMsg_t stMsg;

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_REQUEST_PURCHASE_HISTORY;
	stMsg.lParam1 = 0;

	err = VK_PostMessage( s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if( err != ERR_OK )
	{
		HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseHistory] Q Send Fail in CA_NA_PPV_PPV_RequestPurchaseHistory 0x%x\n", err);
		return (!ERR_OK);
	}
	HxLOG_Print("[CA_NA_PPV_PPV_RequestPurchaseHistory]-> EXIT\n");
	return err;
}

HERROR  CA_NA_PPV_CancelPurchaseEvent(TCaRequest* pRequest)
{
	return local_caPpv_DisposeRequest(pRequest);
}

//------------------------------- NOTIFICATION ---------------------------------

TCaStatus CA_NA_PPV_PpvListNotification(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("->[Enter] \n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_LIST_NOTIFICATION;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Print("Q Send Fail in CA_NA_PPV_PpvListNotification (0x%x) \n", nErrCode);
		return CA_ERROR;
	}

	HxLOG_Print("->[Exit]\n");

	return CA_NO_ERROR;
}

TCaStatus CA_NA_PPV_CreditNotification(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("->[Enter] \n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_CREDIT_NOTIFICATION;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Print("Q Send Fail in CA_NA_PPV_CreditNotification (0x%x) \n", nErrCode);
		return CA_ERROR;
	}

	HxLOG_Print("->[Exit] \n");

	return CA_NO_ERROR;
}

void CA_NA_PPV_NotifyTPChanged(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print(" ->[Enter] \n");

	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.lParam2 	= CAK_SERVICE_CHANGE_TP;
	stMsg.lParam1 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("Q Send Fail in CA_NA_PPV_NotifyTPChanged (0x%x) \n", nErrCode);
	}

	HxLOG_Print("->[Exit] \n");

	return ;
}

void CA_NA_PPV_NotifyServiceChanged(TUnsignedInt32 hSvcHandle)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("->[Enter] hSvcHandle=0x%x\n", hSvcHandle);
	stMsg.wMsg 		= NA_SERVICE_ID;
	stMsg.lParam2 	= CAK_SERVICE_CHANGE_CHANNEL;
	stMsg.lParam1 	= (TUnsignedInt32)hSvcHandle;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("Q Send Fail in CA_NA_PPV_NotifyServiceChanged (0x%x) \n", nErrCode);
	}

	HxLOG_Print("->[Exit] \n");

	return ;
}

void CA_NA_PPV_NotifyEITpfArrived(TUnsignedInt32 hSvcHandle)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("->[Enter] hSvchandle=0x%x \n", hSvcHandle);

	stMsg.wMsg 		= NA_FILTER_ID;
	stMsg.lParam2 	= CAK_FILTER_EIT_PF_ARRIVAL;
	stMsg.lParam1 	= (TUnsignedInt32)hSvcHandle;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Error("Q Send Fail in CA_NA_PPV_NotifyEITpfArrived (0x%x) \n", nErrCode);
	}

	HxLOG_Print("->[Exit] \n");

	return ;
}

void CA_NA_PPV_NotifyTDTArrived(TUnsignedInt32 hSvcHandle)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("->[Enter] hSvcHandle=0x%x \n", hSvcHandle);

	stMsg.wMsg 		= NA_FILTER_ID;
	stMsg.lParam2 	= CAK_FILTER_TDT_ARRIVAL;
	stMsg.lParam1	= (TUnsignedInt32)hSvcHandle;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Error("Q Send Fail in CA_NA_PPV_NotifyTDTArrived (0x%x) \n", nErrCode);
	}
	HxLOG_Print("->[Exit] \n");

	return ;
}


HERROR  CA_NA_PPV_RegisterCreditCallback(TCreditCallback callback)
{
	gCreditCallBackFn = callback;
	return ERR_OK;
}


HERROR  CA_NA_PPV_RegisterPpvListCallback(TEventPurchaseListCallback callback)
{
	gPpvListCallBackFn = callback;
	return ERR_OK;
}

HERROR  CA_NA_PPV_RegisterRechargeNotifyCallback(TRechargeNotifyCallback callback)
{
	RechargeNotifyCallbackFn = callback;
	return ERR_OK;
}

HERROR  CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback)
{
	PurchaseEventNotifyCallback = callback;
	return ERR_OK;
}

HERROR  CA_NA_PPV_RegisterRequestEventNotifyCallback(TRequestEventNotifyCallback callback)
{
	RequestEventNotifyCallback = callback;
	return ERR_OK;
}

HERROR  CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback)
{
	PurchaseHistoryNotifyCallback = callback;
	return ERR_OK;
}

#define ________CA_CAK_PPV_Command_Functions_Body______________

void  CA_NA_PPV_PrintPfEvent(void)
{
	TUnsignedInt32 ulIndex;

	for (ulIndex=0; ulIndex<2; ulIndex++)
	{
		if(ulIndex == 0)
			HxLOG_Print("==== Present Event =====\n");
		else
			HxLOG_Print("==== Following Event =====\n");

		HxLOG_Print("EvtId [0x%04] %s\n", gEvtPurchaseInfo[ulIndex].nEvtId, gEvtPurchaseInfo[ulIndex].ucEvtName);

		HxLOG_Print("startTime [0x%04] duration [0x%04x] s\n", gEvtPurchaseInfo[ulIndex].ulStartTime, gEvtPurchaseInfo[ulIndex].ulDuration);

		HxLOG_Print("%04d/%02d/02d - %02d : %02d, %02d \n",
			gEvtPurchaseInfo[ulIndex].stCalendarTime.year, gEvtPurchaseInfo[ulIndex].stCalendarTime.month, gEvtPurchaseInfo[ulIndex].stCalendarTime.day,
			gEvtPurchaseInfo[ulIndex].stCalendarTime.hour, gEvtPurchaseInfo[ulIndex].stCalendarTime.minute, gEvtPurchaseInfo[ulIndex].stCalendarTime.second);
	}

	return ERR_OK;
}

#endif


