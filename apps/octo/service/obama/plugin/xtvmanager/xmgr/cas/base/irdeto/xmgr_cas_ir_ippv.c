/********************************************************************
 Header Files
 ********************************************************************/
#include <bus.h>
//#include <vd_humax.h>
//#include <ir_svc.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <util.h>
#include <mgr_action.h>

#include <s3_ippv_msg.h>
#include <s3_lppv_msg.h>
#include <s3_event_if_msg.h>

#include "local_include/_xmgr_cas_ir_ppv.h"

#include "ir_ctrl.h"


/********************************************************************
 Macro Definition
 ********************************************************************/




#define 	IS_IR_IPPV_ACTIVATED(debit, limit)	( ( ((limit - debit - 1) >= 0) & ((limit - debit - 1)  != 0xFFFF) ) ? TRUE : FALSE)
#define		INVALID_SECTOR_ID		0xFF


/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
STATIC XmgrCas_IrPpvControl_t	s_Ippv_Control;
STATIC XmgrCas_IrSectorInfo_t	s_Sector_Info[MAX_PPV_SECTOR_NUM];
STATIC XmgrCas_IrIppvEitInfo_t	s_EIT_Info;
STATIC CAS_IR_PPV_PRODUCT_STRUCT	s_Cur_Product_Info;
//STATIC CAS_IR_PPV_PRODUCT_STRUCT	s_Purchased_Product[MAX_PPV_PRODUCT_NUM];
STATIC CAS_IR_PPV_PRODUCT_LIST		s_Purchased_ProductList;
// user 설정을 즉각 반영하며, commit하여 s_Purchased_ProductList.bIsHide[]에 적용
STATIC HUINT8	s_arrHiddenStatusForUser[MAX_PPV_PRODUCT_NUM];
STATIC HUINT8	s_bIsLoadProductList = FALSE;
STATIC HUINT8	s_bIsInitialized = FALSE;
HUINT32 g_ulCAS_IR_PPVSem = 0;

/********************************************************************
*   Global Function Prototypes definitions
*	The following functions will be used as outer functions
********************************************************************/

#define LOCK_SEM_IR_PPV()     			VK_SEM_Get(g_ulCAS_IR_PPVSem)
#define UNLOCK_SEM_IR_PPV()   		VK_SEM_Release(g_ulCAS_IR_PPVSem)


/********************************************************************
 External Variable Declaration
 ********************************************************************/


/********************************************************************
 Static Function
 ********************************************************************/
 #define _____CAS_IR_IPPV_LOCAL_FUNCTIONS_____________________
 STATIC void xmgr_cas_ir_ppv_RequestPurchasedList(HUINT8 ucSlotId, ia_byte bSector, ia_byte bPeriod)
{
	msg_ippv_event_list_query_st stEventList;

	stEventList.wResourceId = ucSlotId;
	stEventList.bSector = bSector;
	stEventList.bPeriod = bPeriod;

	XSVC_CAS_IR_IPPV_Event_List_Query(&stEventList);
}

 STATIC void xmgr_cas_ir_ppv_InitCurrentInfo()
{
	HUINT32	i;

	LOCK_SEM_IR_PPV();

	HxSTD_memset(&s_Ippv_Control, 0, sizeof(XmgrCas_IrPpvControl_t));
	HxSTD_memset(&s_EIT_Info, 0, sizeof(XmgrCas_IrIppvEitInfo_t));
	HxSTD_memset(&s_Cur_Product_Info, 0, sizeof(CAS_IR_PPV_PRODUCT_STRUCT));

	for (i = 0; i < MAX_PPV_SECTOR_NUM; i++)
	{
		HxSTD_memset(&s_Sector_Info[i], 0, sizeof(XmgrCas_IrSectorInfo_t));
		s_Sector_Info[i].sector = INVALID_SECTOR_ID;
	}

	if (s_bIsLoadProductList == FALSE)
	{
		memset(&s_Purchased_ProductList, 0, sizeof(CAS_IR_PPV_PRODUCT_LIST));
		CAS_IR_PPV_LoadProductList(&s_Purchased_ProductList);
		s_bIsLoadProductList = TRUE;

		s_Purchased_ProductList.usProductNum = 0;
		for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
		{
			if (s_Purchased_ProductList.poduct_list[i].evtId != 0)
				s_Purchased_ProductList.usProductNum++;
		}

		// 20100927 tsjung
		HxSTD_memcpy(s_arrHiddenStatusForUser ,s_Purchased_ProductList.bIsHide, sizeof(HUINT8) * MAX_PPV_PRODUCT_NUM);
	}

	UNLOCK_SEM_IR_PPV();
}

 STATIC void xmgr_cas_ir_ppv_Init()
{
	HUINT8	szSemName[10];
	HINT32	nRet;

	/* Semaphore Creation */
	sprintf(szSemName, "IRPPVSEM");
	nRet = VK_SEM_Create(&g_ulCAS_IR_PPVSem,szSemName, VK_SUSPEND_FIFO);
	if(nRet == VK_ERROR)
	{
		return;
	}

	xmgr_cas_ir_ppv_InitCurrentInfo();

//	xmgr_cas_ir_ppv_RequestPurchasedList(0, 0, IPPV_PURCHASED_LIST_ALL);

	s_bIsInitialized = TRUE;
}

STATIC HBOOL xmgr_cas_ir_ppv_CheckCardStatus(HUINT8 ucSlotId, CAS_IR_SC_RESULT* pReqResult)
{
	HBOOL isFound;
	HUINT32 ulSCStatus;
	SvcCas_CtrlParam_t					stIrCtrlParam;
	IR_GetGlobalStatusOfScOutParam_t	stGetStatusParam;

	stIrCtrlParam.ulControlType = eCACTRL_IR_DRV_GetGlobalStatusOfSC;
	stIrCtrlParam.pvOut		= &stGetStatusParam;
	stGetStatusParam.ucSlotId	= ucSlotId;
	svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_DRV_GetGlobalStatusOfSC, &stIrCtrlParam);
	ulSCStatus	= stGetStatusParam.ulStatus;
	isFound		= stGetStatusParam.bGetStatus;

	if(isFound == FALSE)
		return FALSE;

	HxLOG_Info("Card Status: 0x%X\n", ulSCStatus);

	switch(ulSCStatus)
	{
		case IR_EVT_AppStatus_CardOut:
			*pReqResult = CAS_IR_SC_RESULT_CARD_REMOVED;
			break;

		case IR_EVT_AppStatus_CardInvalid:
			*pReqResult = CAS_IR_SC_RESULT_CARD_ERROR;
			break;

		case IR_EVT_AppStatus_CardFault:
			*pReqResult = CAS_IR_SC_RESULT_CARD_MUTED;
			break;

		case IR_EVT_SvcStatus_ProductExpired:
			*pReqResult = CAS_IR_SC_RESULT_CARD_EXPIRED;
			break;

		case IR_EVT_SvcStatus_CreditLimit:
			*pReqResult = CAS_IR_SC_RESULT_MAXIMUM_CREDIT;
			break;

		case IR_EVT_SvcStatus_FTAService:
			*pReqResult = CAS_IR_SC_RESULT_PURCHASE_TIME_ENDED;
			break;

		case IR_EVT_SvcStatus_Descrambed:
		case IR_EVT_SvcStatus_Purchased:
		case IR_EVT_SvcStatus_AlreadyPurchase:
		case IR_EVT_SvcStatus_EventRecorded:
			*pReqResult = CAS_IR_SC_RESULT_ALREADY_PURCHASED;
			break;

		case IR_EVT_SvcStatus_PreviewTime:
		case IR_EVT_SvcStatus_NoView:
		case IR_EVT_SvcStatus_NoStreamMatch:
		case IR_EVT_SvcStatus_UnknownStatus:
			*pReqResult = CAS_IR_SC_RESULT_GENERIC_ERROR;
			break;

		default:
			isFound = FALSE;
	}

/*	if (isFound == FALSE)
	{
		HUINT8 window;
		window = AP_MHP_IRPPV_GetWindow();

		if (window & PPV_WINDOW_PPV_EVENT)
		{
			CA_IPPV_PROCESS_INFO_STRUCT stProcessInfo;
			AP_MHP_IRPPV_GetPpvEventInfo(TYPE_MGR_PROCESS, &stProcessInfo);
			if (stProcessInfo.isBought)
			{
				isFound = TRUE;
				*pReqResult = CAS_IR_SC_RESULT_ALREADY_PURCHASED;
			}
			else if (!(window & PPV_WINDOW_PURCHASE))
			{
				isFound = TRUE;
				*pReqResult = CAS_IR_SC_RESULT_PURCHASE_TIME_ENDED;
			}
		}
	}*/

	return isFound;
}

// new EIT를 얻을 경우 return ERR_OK, 아니면 return ERR_FAIL
STATIC HERROR 	xmgr_cas_ir_ppv_GetEITInfo()
{
	HUINT32 						ulActionId;
	HUINT16 						usStringLen;
	Handle_t 						hService;
	HERROR 							hError;
	SvcEpg_FindParam_t				stParam = {0, };
	POINTER_LIST_T					*pEventList = NULL;
	POINTER_LIST_T					*pList = NULL;
	SvcEpg_Event_t						*pMwEvent = NULL;
	UNIXTIME						currTime, endTime;
	XmgrCas_IrIppvEitInfo_t 	stNowEitInfo;
	HUINT8							*pucConvertString = NULL;
	DbSvc_Info_t						stSvcInfo;
	HUINT8							bEITRecieve = FALSE;


	ulActionId = MGR_ACTION_GetMainActionId();
	hError = MGR_ACTION_GetMasterSvcHandle(ulActionId, &hService);

	if(hError != ERR_OK)
		return hError;

	hError = VK_CLOCK_GetTime((unsigned long*) &currTime);
	if(hError != ERR_OK)
		return hError;

	stParam.ulFindMask = (EPG_FIND_MASK_PF | EPG_FIND_MASK_TIME);
	stParam.hSvcHandle = hService;
	stParam.ulStartTime = (HUINT32)currTime;
	stParam.ulEndTime = 0xffffffff;
	// TODO: EPG 소스 정리로 인해 새로 구현하세요.
	//hError = SVC_EPG_GetEventList(&stParam, &pEventList);
	if(hError != ERR_OK || pEventList == NULL)
		return ERR_FAIL;

	pList = pEventList;
	while(pList)
	{
		pMwEvent = NULL;
		pMwEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pList);
		if(pMwEvent == NULL)
		{
			pList = UTIL_LINKEDLIST_GetNextPointer(pList);
			continue;
		}

		endTime = pMwEvent->ulStartTime + pMwEvent->ulDuration;
	HxLOG_Info("Start Time (%ld) Current time (%ld) Event End Time (%ld) $$$$$$$$$$\n", pMwEvent->ulStartTime, currTime, endTime);
		if (endTime > currTime)
		{
			stNowEitInfo.evtId = pMwEvent->stEventInfo.stEitEventInfo.nEventId;
			stNowEitInfo.startTime = pMwEvent->ulStartTime;
			stNowEitInfo.endTime = endTime;

			if (pMwEvent->astShortText[0].pEventName)
			{
				// TODO: EPG 소스 정리로 인해 새로 구현하세요.
				//hError = SVC_EPG_ConvertEpgTextToString(pMwEvent->astShortText[0].pEventName, &pucConvertString);
				if (hError != ERR_OK)
					break;
				usStringLen = SvcCas_UtilStrLen(pucConvertString);
				if(usStringLen > SvcSi_MAX_SHORT_EVENT_NAME)
					usStringLen = SvcSi_MAX_SHORT_EVENT_NAME;

				stNowEitInfo.evtNameSize = usStringLen;
				memcpy(stNowEitInfo.evtName, pucConvertString, usStringLen);
				stNowEitInfo.evtName[usStringLen] = 0;

			HxLOG_Info("Event ID: 0x%X Event Name:%s $$$$$$$$$$\n", stNowEitInfo.evtId, stNowEitInfo.evtName);
			}

			if (pMwEvent->astShortText[0].pEventText)
			{
				// TODO: EPG 소스 정리로 인해 새로 구현하세요.
			//	hError = SVC_EPG_ConvertEpgTextToString(pMwEvent->astShortText[0].pEventText, &pucConvertString);
				if (hError != ERR_OK)
					break;
				usStringLen = SvcCas_UtilStrLen(pucConvertString);
				if(usStringLen > SvcSi_MAX_EVENT_TEXT)
					usStringLen = SvcSi_MAX_EVENT_TEXT;

				stNowEitInfo.evtTextSize = usStringLen;
				memcpy(stNowEitInfo.evtText, pucConvertString, usStringLen);
				stNowEitInfo.evtText[usStringLen] = 0;
			}

			HxSTD_memcpy(&s_EIT_Info, &stNowEitInfo, sizeof(XmgrCas_IrIppvEitInfo_t));
			bEITRecieve = TRUE;
			//s_Ippv_Control.eitReceived = TRUE;
			break;
		}

		pList = UTIL_LINKEDLIST_GetNextPointer(pList);
	}

	if(pEventList)
	{
		// TODO: EPG 소스 정리로 인해 새로 구현하세요.
		//hError = SVC_EPG_ReleaseEventList(pEventList);
	}

	if (bEITRecieve == TRUE)
	{
		if (DB_SVC_GetServiceInfo(hService, &stSvcInfo) == ERR_OK)
		{
			memcpy(s_EIT_Info.szSvcName, DbSvc_GetName(&stSvcInfo), DbSvc_GetNameLength());
			s_EIT_Info.usLcn = DbSvc_GetLcn(&stSvcInfo);

		HxLOG_Info("Service No: %d Service Name:%s $$$$$$$$$$\n", s_EIT_Info.usLcn, s_EIT_Info.szSvcName);
		}
		return ERR_OK;
	}

	return ERR_FAIL;	// EIT를 구하지 못한 경우 ERR_FAIL;

}

STATIC HBOOL xmgr_cas_ir_ppv_IsLeapYear(HUINT16 usYear)
{
	if (!(usYear % 400))
		return TRUE;
	if (!(usYear % 100))
		return FALSE;
	if (!(usYear % 4))
		return TRUE;
	return FALSE;
}

STATIC void xmgr_cas_ir_ppv_ConvertDateCodeToDate(HUINT16 dcode, HxDATETIME_Date_t *date)
{
	HINT16 Year, Mnth, Day;
	HUINT8 MDA[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	Year = 2000;
	Mnth = 1;
	Day = 1;
	Day += (dcode - 1);

	while( (Day <= 0) || (Day > MDA[Mnth-1]) )
	{
		if (Day <= 0)
		{
			Mnth -= 1;
			if (Mnth == 0)
			{
				Mnth = 12;
				Year--;
				MDA[1] = xmgr_cas_ir_ppv_IsLeapYear(Year) ? 29 : 28;
			}
			else if ( (Mnth == 2) && (xmgr_cas_ir_ppv_IsLeapYear(Year)) )
			{
				MDA[1] = 29;
			}
			Day += MDA[Mnth-1];
		}
		else if (Day > MDA[Mnth-1])
		{
			if ( (Mnth == 2) && (xmgr_cas_ir_ppv_IsLeapYear(Year)) )
			{
				MDA[1] = 29;
			}
			Day -= MDA[Mnth-1];
			Mnth += 1;
			if (Mnth == 13)
			{
				Mnth = 1;
				Year++;
				MDA[1] = xmgr_cas_ir_ppv_IsLeapYear(Year) ? 29 : 28;
			}
		}
	}

	date->usYear = Year;
	date->ucMonth = Mnth;
	date->ucDay = Day;
}


HUINT8 xmgr_cas_ir_ppv_ConvertYear4to2(HUINT16 usYear)
{
	HUINT16 c1000;
	HUINT16 c100;
	HUINT8 ucYear;


	c1000 = (HUINT16)(usYear / 1000);
	c100 = (HUINT16)((usYear - (c1000 * 1000)) / 100);
	ucYear = (HUINT8)(usYear - ((c1000 * 1000) + (c100 * 100)));


	return ucYear;
}

STATIC HERROR xmgr_cas_ir_ppv_MakeIppvProduct(HUINT16 usCaEventId)
{
	HxDATETIME_Time_t time;
	HxDATETIME_Date_t date;

	if (xmgr_cas_ir_ppv_GetEITInfo() != ERR_OK)
		return ERR_FAIL;

	s_Ippv_Control.eitReceived == TRUE;

	if (s_EIT_Info.evtTextSize > 0)
	{
		HxSTD_memcpy(s_Cur_Product_Info.ProductDesc, s_EIT_Info.evtText, s_EIT_Info.evtTextSize);
		s_Cur_Product_Info.ProductDesc[s_EIT_Info.evtTextSize] = 0;
	}

	if (s_EIT_Info.evtNameSize> 0)
	{
		HxSTD_memcpy(s_Cur_Product_Info.ProductName, s_EIT_Info.evtName, s_EIT_Info.evtNameSize);
		s_Cur_Product_Info.ProductName[s_EIT_Info.evtNameSize] = 0;
	}

	HxLOG_Info("########## Event Name: %s \n", s_EIT_Info.evtName);

	//Purchase Window Start time(Present Eit)
	HLIB_DATETIME_ConvertUnixTimeToDate(s_EIT_Info.startTime, &date);
	HLIB_DATETIME_ConvertUnixTimeToTime(s_EIT_Info.startTime, &time);
	s_Cur_Product_Info.StartTime.year = xmgr_cas_ir_ppv_ConvertYear4to2(date.usYear);
	s_Cur_Product_Info.StartTime.month = date.ucMonth;
	s_Cur_Product_Info.StartTime.day = date.ucDay;
	s_Cur_Product_Info.StartTime.hour = time.ucHour;
	s_Cur_Product_Info.StartTime.minute = time.ucMinute;
	s_Cur_Product_Info.StartTime.second = time.ucSecond;

	//Purchase Window End time(Present Eit)
	HLIB_DATETIME_ConvertUnixTimeToDate(s_EIT_Info.endTime, &date);
	HLIB_DATETIME_ConvertUnixTimeToTime(s_EIT_Info.endTime, &time);
	s_Cur_Product_Info.EndTime.year = xmgr_cas_ir_ppv_ConvertYear4to2(date.usYear);
	s_Cur_Product_Info.EndTime.month = date.ucMonth;
	s_Cur_Product_Info.EndTime.day = date.ucDay;
	s_Cur_Product_Info.EndTime.hour = time.ucHour;
	s_Cur_Product_Info.EndTime.minute = time.ucMinute;
	s_Cur_Product_Info.EndTime.second = time.ucSecond;

	s_Cur_Product_Info.usLcn = s_EIT_Info.usLcn;
	HxSTD_memcpy(s_Cur_Product_Info.szSvcName, s_EIT_Info.szSvcName, /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength());

	return ERR_OK;
}

STATIC CAS_IR_PPV_PRODUCT_STRUCT* xmgr_cas_ir_ppv_GetEmptyProductInfo()
{
	HUINT32	i;

	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.poduct_list[i].evtId == 0)
			return &s_Purchased_ProductList.poduct_list[i];
	}

	return NULL;
}

STATIC HUINT16 xmgr_cas_ir_ppv_ConvertPincodeStringToHex(HUINT8 *pucPincode)
{
	HUINT8 ucPincodeLen = 0;
	HUINT8 nNum;
	HUINT16	usReturn = 0;

	if(pucPincode == NULL)
		return 0;

	for(ucPincodeLen = 0; ucPincodeLen < MAX_PPV_PINCODE_SIZE; ucPincodeLen++)
	{
		if(pucPincode[ucPincodeLen] > 0x39 || pucPincode[ucPincodeLen] < 0x30)
			return 0;

		nNum = (HUINT8)pucPincode[ucPincodeLen] - 0x30;
		usReturn |= (nNum << (4 * (MAX_PPV_PINCODE_SIZE - ucPincodeLen - 1)));
	}

	return usReturn;
}

STATIC HUINT8 xmgr_cas_ir_ppv_IsPurchasedEvent(HINT32	eventId)
{
	HUINT32	i;

	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.poduct_list[i].evtId == eventId)
			return TRUE;
	}

	return FALSE;
}

// 20100915, tsjung
#define _____CAS_IR_PPV_PurchasedProduct_Array_LOCAL_FUNCTION___________________________________

STATIC HUINT32 xmgr_cas_ir_ppv_PurchasedProductArr_GetItemCount(void)
{
	HUINT32	i;

	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.poduct_list[i].evtId == 0)
			break;	// i는 empty product의 첫 인덱스이므로 current item count와 같다.
	}

	return i;
}

STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_ShiftArray(HUINT32 nShiftCount)
{
	CAS_IR_PPV_PRODUCT_STRUCT	temp_purchased_product[MAX_PPV_PRODUCT_NUM];
	HUINT8						temp_bIsHide[MAX_PPV_PRODUCT_NUM];
	HUINT8						temp_hiddenStatusForUser[MAX_PPV_PRODUCT_NUM];


	if (nShiftCount <= 0 || nShiftCount > MAX_PPV_PRODUCT_NUM)
	{
		HxLOG_Print("Shift count (%d)is invalid...\n", nShiftCount);
		return ERR_FAIL;
	}

	HxLOG_Print("Enter the xmgr_cas_ir_ppv_PurchasedProductArr_ShiftArray()\n");


	if (nShiftCount == MAX_PPV_PRODUCT_NUM)
	{
		// All clear
		HxSTD_memset(s_Purchased_ProductList.poduct_list, 0, sizeof(CAS_IR_PPV_PRODUCT_STRUCT) * MAX_PPV_PRODUCT_NUM);
		HxSTD_memset(s_Purchased_ProductList.bIsHide, 0, sizeof(HUINT8) * MAX_PPV_PRODUCT_NUM);
		HxSTD_memset(s_arrHiddenStatusForUser, 0, sizeof(HUINT8) * MAX_PPV_PRODUCT_NUM);
	}
	else	// The other case...
	{
		// Step1. copy original to temp
		HxSTD_memcpy(temp_purchased_product, &s_Purchased_ProductList.poduct_list[nShiftCount],
		sizeof(CAS_IR_PPV_PRODUCT_STRUCT) * (MAX_PPV_PRODUCT_NUM - nShiftCount));
		HxSTD_memcpy(temp_bIsHide, &s_Purchased_ProductList.bIsHide[nShiftCount],sizeof(HUINT8) * (MAX_PPV_PRODUCT_NUM - nShiftCount));
		HxSTD_memcpy(temp_hiddenStatusForUser, &s_arrHiddenStatusForUser[nShiftCount],sizeof(HUINT8) * (MAX_PPV_PRODUCT_NUM - nShiftCount));

		//Step2.  copy temp to original
		HxSTD_memcpy(s_Purchased_ProductList.poduct_list, temp_purchased_product,
		sizeof(CAS_IR_PPV_PRODUCT_STRUCT) * (MAX_PPV_PRODUCT_NUM - nShiftCount));
		HxSTD_memcpy(s_Purchased_ProductList.bIsHide, temp_bIsHide,sizeof(HUINT8) * (MAX_PPV_PRODUCT_NUM - nShiftCount));
		HxSTD_memcpy(s_arrHiddenStatusForUser, temp_hiddenStatusForUser,sizeof(HUINT8) * (MAX_PPV_PRODUCT_NUM - nShiftCount));

		// Step3. clear the rear empty area
		HxSTD_memset(&s_Purchased_ProductList.poduct_list[MAX_PPV_PRODUCT_NUM - nShiftCount], 0, sizeof(CAS_IR_PPV_PRODUCT_STRUCT) * nShiftCount);
		HxSTD_memset(&s_Purchased_ProductList.bIsHide[MAX_PPV_PRODUCT_NUM - nShiftCount], 0, sizeof(HUINT8) * nShiftCount);
		HxSTD_memset(&s_arrHiddenStatusForUser[MAX_PPV_PRODUCT_NUM - nShiftCount], 0, sizeof(HUINT8) * nShiftCount);
	}

	return ERR_OK;
}

/*  	shift array는 shift count만큼 무조건 shift하지만, smart shift array는 현재 아이템 개수를 고려해 필요한 만큼 shift한다.
	shift를 여러 번에 걸쳐 호출하면 overhead가 많으므로, purchased product array에 insert하고자 하는 아이템 개수만큼
	한 번에 shift하고자 한다. (보통 2개 이상의 item insert 시 미리 호출하도록 한다.)
	warning: 파라미터인 nWillBeInsertedCount는 update item을 고려하지 않고, add item count라고 보고 구현된 상태이다.
*/
STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_SmartShiftArray(HUINT32 nWillBeInsertedCount)
{
	HUINT32 nAvailableCount = 0;

	nAvailableCount = MAX_PPV_PRODUCT_NUM - xmgr_cas_ir_ppv_PurchasedProductArr_GetItemCount();

	// 넉넉하면 pass
	if (nWillBeInsertedCount <= nAvailableCount)
	{
		return ERR_OK;
	}

	// 현재 수용공간이 적으면 shift
	return xmgr_cas_ir_ppv_PurchasedProductArr_ShiftArray(nWillBeInsertedCount - nAvailableCount);
}

STATIC HUINT32 xmgr_cas_ir_ppv_PurchasedProductArr_FindItemByEventID(HINT32	evtId)
{
	HUINT32 i;

	if (evtId == 0)
	{
		HxLOG_Print("param event id[%d] is invalid", evtId);
		return -1;
	}

	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.poduct_list[i].evtId == evtId)
			return i;
		else if (s_Purchased_ProductList.poduct_list[i].evtId == 0)
			break;
	}

	return -1;
}


STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_SetItem(HUINT32 index,CAS_IR_PPV_PRODUCT_STRUCT* stNewItem)
{
	if (index >= MAX_PPV_PRODUCT_NUM)
	{
		HxLOG_Print("param index[%d] is invalid", index);
		return ERR_FAIL;
	}

	if (stNewItem == NULL)
	{
		HxLOG_Print("param stNewItem is NULL");
		return ERR_FAIL;
	}

	// copy
	HxSTD_memcpy(&s_Purchased_ProductList.poduct_list[index], stNewItem, sizeof(CAS_IR_PPV_PRODUCT_STRUCT));

	return ERR_OK;
}


STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_UpdateItem(HINT32	evtId, CAS_IR_PPV_PRODUCT_STRUCT* stNewItem)
{
	HUINT32 nTargetIndex = 0;

	if (evtId == 0 || stNewItem == NULL)
	{
		HxLOG_Print("param is invalid");
		return ERR_FAIL;
	}

	// Step1. Check for new item
	if (evtId != stNewItem->evtId)
	{
		HxLOG_Print("event id is not equal to new item's event id...");
		return ERR_FAIL;
	}

	// Step2. Search by event id
	if ((nTargetIndex = xmgr_cas_ir_ppv_PurchasedProductArr_FindItemByEventID(evtId)) == -1)
	{
		HxLOG_Print("Can not find item (event id: %d)", evtId);
		return ERR_FAIL;
	}

	// Step3. update item
	if (xmgr_cas_ir_ppv_PurchasedProductArr_SetItem(nTargetIndex,stNewItem) != ERR_OK)
	{
		HxLOG_Print("Failed to xmgr_cas_ir_ppv_PurchasedProductArr_SetItem()");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_UpdateTime(HINT32	evtId, CAS_IR_PPV_PRODUCT_STRUCT* stNewItem)
{
	HUINT32 nTargetIndex = 0;
	CAS_IR_PPV_PRODUCT_STRUCT		*pSavedProduct = NULL;

	if (evtId == 0 || stNewItem == NULL)
	{
		HxLOG_Print("param is invalid");
		return ERR_FAIL;
	}

	// Step1. Check for new item
	if (evtId != stNewItem->evtId)
	{
		HxLOG_Print("event id is not equal to new item's event id...");
		return ERR_FAIL;
	}

	// Step2. Search by event id
	if ((nTargetIndex = xmgr_cas_ir_ppv_PurchasedProductArr_FindItemByEventID(evtId)) == -1)
	{
		HxLOG_Print("Can not find item (event id: %d)", evtId);
		return ERR_FAIL;
	}

	// ewlee 20100920 - check purchasing time and update only time information
	pSavedProduct = &s_Purchased_ProductList.poduct_list[nTargetIndex];
	if (pSavedProduct->PurchaseTime.year > stNewItem->PurchaseTime.year)
		return ERR_OK;
	if (pSavedProduct->PurchaseTime.month > stNewItem->PurchaseTime.month)
		return ERR_OK;
	if (pSavedProduct->PurchaseTime.day > stNewItem->PurchaseTime.day)
		return ERR_OK;
	if (pSavedProduct->PurchaseTime.hour > stNewItem->PurchaseTime.hour)
		return ERR_OK;
	if (pSavedProduct->PurchaseTime.minute > stNewItem->PurchaseTime.minute)
		return ERR_OK;
	if (pSavedProduct->PurchaseTime.second > stNewItem->PurchaseTime.second)
		return ERR_OK;

	HxSTD_memcpy(&pSavedProduct->PurchaseTime, &stNewItem->PurchaseTime, sizeof(CAS_IR_DATE));

	return ERR_OK;
}

STATIC HERROR xmgr_cas_ir_ppv_PurchasedProductArr_AddItem(CAS_IR_PPV_PRODUCT_STRUCT* stNewItem)
{
	CAS_IR_PPV_PRODUCT_STRUCT *pProduct = NULL;

	if (stNewItem == NULL)
	{
		HxLOG_Print("PARAM is invalid...\n");
		return ERR_FAIL;
	}

	// Step1. Get an empty product pointer
	pProduct = xmgr_cas_ir_ppv_GetEmptyProductInfo();

	// purchased product가 full인 경우...
	if (pProduct == NULL)
	{
		HxLOG_Print("Purchased product info is full...I'll try to shift array...\n");

		if (xmgr_cas_ir_ppv_PurchasedProductArr_ShiftArray(1) != ERR_OK)
		{
			HxLOG_Print("Failed to xmgr_cas_ir_ppv_PurchasedProductArr_ShiftArray()\n");
			return ERR_FAIL;
		}

		pProduct = xmgr_cas_ir_ppv_GetEmptyProductInfo();
		if (pProduct == NULL)	// 이럴 경우는 없을 것이다...
		{
			HxLOG_Print("Failed to get empty product info\n");
			return ERR_FAIL;
		}
	}
	// empty product info 확보 OK!

	// Step 2. Add item to array
	HxSTD_memcpy(pProduct, stNewItem, sizeof(CAS_IR_PPV_PRODUCT_STRUCT));
	s_Purchased_ProductList.usProductNum++;

	return ERR_OK;
}



/********************************************************************
 Global Function
 ********************************************************************/

#define _____CAS_IR_IPPV_PUBLIC_API____________________________________

 void xmgr_cas_IrIppvInitialize()
{
	if (s_bIsInitialized == FALSE)
		xmgr_cas_ir_ppv_Init();
}

void xmgr_cas_IrIppvDestroy()
{
	xmgr_cas_ir_ppv_InitCurrentInfo();
	if (g_ulCAS_IR_PPVSem != 0)
	{
		VK_SEM_Destroy(g_ulCAS_IR_PPVSem);
		g_ulCAS_IR_PPVSem = 0;
	}

	s_bIsInitialized = FALSE;
}

HUINT8 xmgr_cas_IrIppvSetPreviewEvent(msg_ippv_event_notify_st *pNotify, HUINT8 *pSubNotify)
{
	HUINT8	bIsPurchased = FALSE;
	HUINT8 	bIsPreviewPeriod;
	UNIXTIME currTime;
	HUINT16 handler, caEvtId, caEvtCost;
	HUINT8 ippvClock, ippvPreview;
	CAS_IR_SC_RESULT reqResult;


	if (s_bIsInitialized == FALSE)
		xmgr_cas_ir_ppv_Init();

	if (pNotify == NULL)
		return TRUE;

	currTime		= 0;
	bIsPreviewPeriod= FALSE;

	handler 		= 0; // not usable in Softcell 3.x
	ippvClock 		= pNotify->bClockValue;
	ippvPreview 	= pNotify->bPreviewValue;
	caEvtId 		= pNotify->wEventId;
	caEvtCost 		= pNotify->wEventCost;
//	caEvtClock 		= *(pbData + 8); Not Used in Softcell 3.x

	LOCK_SEM_IR_PPV();

	if (s_Ippv_Control.caEvtId == caEvtId)
	{
		if (s_Ippv_Control.Purchased == TRUE)
		{
			UNLOCK_SEM_IR_PPV();
			return TRUE;
		}
		else
		{
			UNLOCK_SEM_IR_PPV();
		HxLOG_Info("########## Same Event ID: %d \n", caEvtId);
			return FALSE;
		}
	}
/*	else
	{
		xmgr_cas_ir_ppv_CheckCardStatus(pNotify->wResourceId, &reqResult);
		if(reqResult == CAS_IR_SC_RESULT_ALREADY_PURCHASED && xmgr_cas_ir_ppv_IsPurchasedEvent(caEvtId) == TRUE)
		{
			HxLOG_Info("########## This is already purchased: %d \n", caEvtId);
			UNLOCK_SEM_IR_PPV();
			return TRUE;
		}
	}
*/
	if (ippvClock < ippvPreview)
		bIsPreviewPeriod = TRUE;

	s_Ippv_Control.caEvtId = caEvtId;
	if (bIsPreviewPeriod == TRUE)
		s_Ippv_Control.curWindow = IR_PPV_WINDOW_PREVIEW;
	else
		s_Ippv_Control.curWindow = IR_PPV_WINDOW_NOVIEW;
	s_Ippv_Control.Purchased = FALSE;
	s_Ippv_Control.eitReceived = FALSE;

	s_Cur_Product_Info.Price = caEvtCost;
	s_Cur_Product_Info.evtId = caEvtId;
	s_Cur_Product_Info.sector = pNotify->bSector;

	/* 	make product가 실패할 경우, UI에서 xmgr_cas_IrPpvUpdateIPPVInfo()를 호출하여
		재구성하도록 한다.
	*/
	xmgr_cas_ir_ppv_MakeIppvProduct(caEvtId);

	UNLOCK_SEM_IR_PPV();

	return bIsPurchased;
}

void xmgr_cas_IrIppvSetSectorInfo( msg_ippv_sector_reply_st *pstSectorReply )
{
	HUINT8 sector;
	HUINT16 debit, limit, threshold, oldToken;
	HINT16 token;

	if (pstSectorReply == NULL)
		return;

	/* DEBIT => 1 Token unit */
	sector = pstSectorReply->bSector;
	debit = pstSectorReply->wCustomerPurse - pstSectorReply->wOperatorPurse;
	limit = pstSectorReply->wDebitLimit;
	threshold = pstSectorReply->wDebitThreshold;

	if (sector >= MAX_PPV_SECTOR_NUM)
		return;

	LOCK_SEM_IR_PPV();


	token = (HINT16)(limit - debit - 1);
	if (token < 0)
		token = 0;
	s_Sector_Info[sector].sector = sector;
	s_Sector_Info[sector].debit = debit;
	s_Sector_Info[sector].debit_limit = limit;
	oldToken = s_Sector_Info[sector].token;
	s_Sector_Info[sector].token = (HUINT16) token;

	if ( sector == 0 )
	{
		if( IS_IR_IPPV_ACTIVATED(debit, limit) )
		{
		//	_ca_sector_info[sector].ppvActivationStatus = TRUE;

			if(token != oldToken)
			{
			//	HxLOG_Print("[SCELL_API_Reply_DebitLimitAndThreshold] IR_SCEVT_DEBIT_CHANGED\n");
			//	AP_MHP_IRPPV_NotifySmartCardEvent(pstSectorReply->wResourceId, AC_ITCA_SCEVT_CREDIT_CHANGED);

				if (token == 0)
					s_Sector_Info[sector].rechargeToken = 0;
				else
					s_Sector_Info[sector].rechargeToken = token - oldToken;

			//	HxLOG_Print("[SCELL_API_Reply_DebitLimitAndThreshold] IR_CAEVT_REQUESTED_RECHARGE (%d)\n",
			//				_ca_sector_info[sector].rechargeToken);
			//	AP_MHP_IRPPV_NotifyCAEvent(pstSectorReply->wResourceId, AC_ITCA_CAEVT_REQUESTED_RECHARGE, (void *)AP_MHP_IRPPV_GetRechargeAmoutEuro100());
			}
		}

	}

	UNLOCK_SEM_IR_PPV();

}

void xmgr_cas_IrPppvReceiveBuyReply(msg_ippv_buy_reply_st *pBuyReply)
{
	UNIXTIME currTime;
	HxDATETIME_Time_t time;
	HxDATETIME_Date_t date;


	if (pBuyReply == NULL)
		return;

	HxLOG_Info("##########Buy Reply Result: 0x%02X", pBuyReply->eResult);

	if (pBuyReply->eResult != IPPV_BUY_SUCCESS)
			return;

	LOCK_SEM_IR_PPV();

	if (pBuyReply->wEventId != s_Ippv_Control.caEvtId)
	{
		UNLOCK_SEM_IR_PPV();
		return;
	}

	HxLOG_Info("##########Buy Reply EventID: %d", pBuyReply->wEventId);

	VK_CLOCK_GetTime(&currTime);

	HLIB_DATETIME_ConvertUnixTimeToDate(currTime, &date);
	HLIB_DATETIME_ConvertUnixTimeToTime(currTime, &time);
	s_Cur_Product_Info.PurchaseTime.year = xmgr_cas_ir_ppv_ConvertYear4to2(date.usYear);
	s_Cur_Product_Info.PurchaseTime.month = date.ucMonth;
	s_Cur_Product_Info.PurchaseTime.day = date.ucDay;
	s_Cur_Product_Info.PurchaseTime.hour = time.ucHour;
	s_Cur_Product_Info.PurchaseTime.minute = time.ucMinute;
	s_Cur_Product_Info.PurchaseTime.second = time.ucSecond;

	// 같은 event id를 갖는 product가 존재하면 update
	if (xmgr_cas_ir_ppv_PurchasedProductArr_UpdateItem(s_Cur_Product_Info.evtId, &s_Cur_Product_Info) != ERR_OK)
	{
		// 없다면 새로 추가...
		xmgr_cas_ir_ppv_PurchasedProductArr_AddItem(&s_Cur_Product_Info);
	}

	s_Ippv_Control.Purchased = TRUE;

	CAS_IR_PPV_SaveProductList(&s_Purchased_ProductList);

	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvCopyEITInfo(XmgrCas_IrIppvEitInfo_t *pDest)
{
	if (pDest == NULL)
		return;

	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(pDest, &s_EIT_Info, sizeof(XmgrCas_IrIppvEitInfo_t));
	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvCopySectorInfo(XmgrCas_IrSectorInfo_t *pDest, HUINT8 ucSector)
{
	if (pDest == NULL || ucSector >= MAX_PPV_SECTOR_NUM)
		return;

	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(pDest, &s_Sector_Info[ucSector], sizeof(XmgrCas_IrSectorInfo_t));
	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvCopyProductIInfo(CAS_IR_PPV_PRODUCT_STRUCT *pDest)
{
	if (pDest == NULL)
		return;

	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(pDest, &s_Cur_Product_Info, sizeof(CAS_IR_PPV_PRODUCT_STRUCT));
	UNLOCK_SEM_IR_PPV();
}

HUINT32 xmgr_cas_IrPpvGetPurchasedProductNum(void)
{
	HUINT32	i, nCnt = 0;

	LOCK_SEM_IR_PPV();
/*	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.poduct_list[i].evtId == 0)
			break;
		nCnt++;
	}
*/
	if (s_bIsLoadProductList == FALSE)
	{
		memset(&s_Purchased_ProductList, 0, sizeof(CAS_IR_PPV_PRODUCT_LIST));
		CAS_IR_PPV_LoadProductList(&s_Purchased_ProductList);
		s_bIsLoadProductList = TRUE;

		s_Purchased_ProductList.usProductNum = 0;
		for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
		{
			if (s_Purchased_ProductList.poduct_list[i].evtId != 0)
				s_Purchased_ProductList.usProductNum++;
		}
	}

	nCnt = s_Purchased_ProductList.usProductNum;
	UNLOCK_SEM_IR_PPV();

	return nCnt;
}

void xmgr_cas_IrPpvCopyPurchasedProductInfo(CAS_IR_PPV_PRODUCT_STRUCT *pProduct, HUINT32 nIndex)
{
	if (pProduct == NULL || nIndex >= MAX_PPV_PRODUCT_NUM)
		return;

	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(pProduct, &s_Purchased_ProductList.poduct_list[nIndex], sizeof(CAS_IR_PPV_PRODUCT_STRUCT));
	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvReceiveEventListReply(msg_ippv_event_list_reply_st *pNotify, msg_ippv_event_list_reply_event_st *pEvtList)
{
	HUINT32		i = 0, nNum = 0, nWillBeInsertedCount = 0;
	HUINT16		usDate;
	HxDATETIME_Date_t	date;
	CAS_IR_PPV_PRODUCT_STRUCT		stProduct;


	if (pNotify == NULL || pEvtList == NULL)
		return;

	nNum = (HUINT32) pNotify->bEventCount;
	if (nNum <= 0)
		return;

	HxLOG_Print("xmgr_cas_IrPpvReceiveEventListReply ##########\n");
	nWillBeInsertedCount = nNum;

	// 어차피 shift 될 data들은 넣지 않도록 한다.
	if (nNum > MAX_PPV_PRODUCT_NUM)
	{
		i = nNum - MAX_PPV_PRODUCT_NUM;
		nWillBeInsertedCount = MAX_PPV_PRODUCT_NUM;
	}

	// Smart shift
	if (xmgr_cas_ir_ppv_PurchasedProductArr_SmartShiftArray(nWillBeInsertedCount) != ERR_OK)
	{
		HxLOG_Print("Failed to xmgr_cas_ir_ppv_PurchasedProductArr_SmartShiftArray()\n");
	}

	LOCK_SEM_IR_PPV();
	for (; i < nNum; i++)
	{
		// Step1. make one product
		HxSTD_memset(&stProduct, 0, sizeof(CAS_IR_PPV_PRODUCT_STRUCT));

		stProduct.evtId = pEvtList[i].wEventId;
		stProduct.Price = pEvtList[i].wEventCost;
		stProduct.sector = pNotify->bSector;
		usDate = pEvtList[i].wDateCode;

		xmgr_cas_ir_ppv_ConvertDateCodeToDate(usDate, &date);
		stProduct.PurchaseTime.year = xmgr_cas_ir_ppv_ConvertYear4to2(date.usYear);
		stProduct.PurchaseTime.month = date.ucMonth;
		stProduct.PurchaseTime.day = date.ucDay;
		// Set event ID instead of event name because there is no event name
		sprintf(stProduct.ProductName, "Unknown Event (0x%04X)", stProduct.evtId);

		// Step2. Try to update
		// ewlee 20100920 - update only purchasing time due to insufficient information.
		if (xmgr_cas_ir_ppv_PurchasedProductArr_UpdateTime(stProduct.evtId, &stProduct) == ERR_OK)
		{
			HxLOG_Print("Succees to xmgr_cas_ir_ppv_PurchasedProductArr_UpdateItem()\n");
			continue;
		}

		// Step3. Add item to array
		if (xmgr_cas_ir_ppv_PurchasedProductArr_AddItem(&stProduct) != ERR_OK)
		{
			HxLOG_Print("Failed to xmgr_cas_ir_ppv_PurchasedProductArr_AddItem()\n");
			continue;
		}
	}

	CAS_IR_PPV_SaveProductList(&s_Purchased_ProductList);

	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvUnhideAllProducts()
{
	HUINT32	i;
	HUINT8  bIsSave = FALSE;

	LOCK_SEM_IR_PPV();
	for (i = 0; i <MAX_PPV_PRODUCT_NUM; i++)
	{
		s_arrHiddenStatusForUser[i] = FALSE;
	}
	UNLOCK_SEM_IR_PPV();
}

HUINT8 xmgr_cas_IrPpvIsHideProduct(HUINT32 nIndex)
{
	HUINT8	bIsHide = FALSE;
	if (nIndex >= MAX_PPV_PRODUCT_NUM)
		return FALSE;

	LOCK_SEM_IR_PPV();
	bIsHide = s_arrHiddenStatusForUser[nIndex];
	UNLOCK_SEM_IR_PPV();

	return bIsHide;
}

void xmgr_cas_IrPpvSetHideStatus(HUINT32 nIndex)
{
	HUINT32	i, nShowIdx;

	if (nIndex >= MAX_PPV_PRODUCT_NUM)
		return;

	LOCK_SEM_IR_PPV();
	nShowIdx = 0;
	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_arrHiddenStatusForUser[i] == TRUE)
			continue;
		if (nShowIdx == nIndex)
		{
			s_arrHiddenStatusForUser[i] = TRUE;
			break;
		}

		nShowIdx++;
	}

	UNLOCK_SEM_IR_PPV();
}

// 20100927 tsjung
HUINT8 xmgr_cas_IrPpvIsHiddenStatusChanged(void)
{
	HUINT32 	i = 0;
	HUINT8 		bResult = FALSE;

	LOCK_SEM_IR_PPV();
	for (i = 0; i < MAX_PPV_PRODUCT_NUM; i++)
	{
		if (s_Purchased_ProductList.bIsHide[i] != s_arrHiddenStatusForUser[i])
			break;
	}
	UNLOCK_SEM_IR_PPV();

	return (i < MAX_PPV_PRODUCT_NUM)? TRUE : FALSE;
}

void xmgr_cas_IrPpvCommitHiddenStatus(void)
{
	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(s_Purchased_ProductList.bIsHide, s_arrHiddenStatusForUser, sizeof(HUINT8) * MAX_PPV_PRODUCT_NUM);
	CAS_IR_PPV_SaveProductList(&s_Purchased_ProductList);
	UNLOCK_SEM_IR_PPV();
}

void xmgr_cas_IrPpvRollbackHiddenStatus(void)
{
	LOCK_SEM_IR_PPV();
	HxSTD_memcpy(s_arrHiddenStatusForUser, s_Purchased_ProductList.bIsHide, sizeof(HUINT8) * MAX_PPV_PRODUCT_NUM);
	UNLOCK_SEM_IR_PPV();
}

// 20100929 tsjung
HERROR xmgr_cas_IrPpvUpdateIPPVInfo(void)
{
	HERROR hResult = ERR_FAIL;

	LOCK_SEM_IR_PPV();
	hResult = xmgr_cas_ir_ppv_MakeIppvProduct(0);
	UNLOCK_SEM_IR_PPV();

	return hResult;
}

/* end of file */
