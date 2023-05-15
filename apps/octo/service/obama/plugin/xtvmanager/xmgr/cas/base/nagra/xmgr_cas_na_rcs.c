/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <mgr_cas.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>


#include <svc_epg.h>
//#include <ap_epg_main.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/

typedef 	enum
{
	ePOPUP_TYPE_RECHARGE = 1,
	ePOPUP_TYPE_PPV
} XmgrCas_NaPopupType_Rcs_e;

typedef enum
{
	eSTATE_PPV_INIT = 0,
//	eSTATE_PPV_TS_CHECK,
// 	eSTATE_PPV_EVENT_INFO_REQUEST,
// 	eSTATE_PPV_EVENT_CHECK_PURCHASABLE,
// 	eSTATE_PPV_EVENT_PURCHASE_REQUEST,
// 	eSTATE_PPV_EVENT_PURCHASE,
 	eSTATE_PPV_SC_ERROR
} XmgrCas_NaPpvState_Rcs_e;

typedef	struct tagPOPUP_RCS_RESERVE_LIST_ITEM
{
	XmgrCas_NaPopupType_Rcs_e		nType;
//	TCreditAmount	nCredit;
	TUnsignedInt8		pName[MAX_EVENT_ITEM_NAME_LENGTH+1];
	struct tagPOPUP_RCS_RESERVE_LIST_ITEM*	pNext;
} XmgrCas_NaPopupReserveListItem_Rcs_t;

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
typedef struct tagPurchasableEvent
{
	TEventStatus				xStatus;
	Handle_t					hSvc;
	TPurchasableEventType 	nPurchaseType;
	TCaProduct				*pProduct;
	TCaRequest				*pRequest;
	PPV_EVENT_INFO			stPPVEventInfo;
} XmgrCas_NaPurchasableEvent_Rcs_t;

typedef struct tagCreditList
{
	TCreditStatus 		status;
	TUnsignedInt8		nNumberOfItems;
	TCreditItem			*aCreditArray;
} XmgrCas_NaCreditListData_t;
#endif

typedef struct
{
	Handle_t								hSvcHandle;

	XmgrCas_NaPpvState_Rcs_e 				ePpvCurState;
	TCreditAmount							nRemainedCredit;

	TPurchasableEventType 					nPurchaseType;
	TCaEvent*								pEvent;
	PPV_EVENT_INFO							stPPVEventType;
	XmgrCas_NaPopupReserveListItem_Rcs_t	*stPopUpItems;
	TUnsignedInt16							nNumRemainedPopup;
} XmgrCas_NaContent_Rcs_t;

#define NA_TEXT_SIZE512 512
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaContent_Rcs_t			s_stCasNaRcsContent;
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
STATIC XmgrCas_NaPurchasableEvent_Rcs_t	s_RCSPurchasableEventInformation;
STATIC XmgrCas_NaCreditListData_t		s_RCSCreditListDataCB;
STATIC XmgrCas_NaCreditListData_t		s_RCSCreditListData;
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_RCS_LOCAL_FUNCTION______________________________________________________

STATIC XmgrCas_NaContent_Rcs_t *xmgr_cas_NaRcsGetContents(void)
{
	return &s_stCasNaRcsContent;
}

#if 0
STATIC HERROR	xmgr_cas_NaRcsUpdateCasMessageFromAlarmData(XmgrCas_NaContent_Rcs_t* pstContents, void* pAlarm)
{
	HUINT16 			i = 0;
	XmgrCas_NaAlamType_e 	na_alarm_msg_type = eCAS_NA_CA_ALARM_TYPE_ERR;
	XmgrCas_NaAlamType_e 	na_access_msg_type = eCAS_NA_CA_ALARM_TYPE_ERR;
	HBOOL				bDisplayCardErr = FALSE;
	NaAlarmData_t 		*pstNaAlarmData = NULL;

	pstNaAlarmData = (NaAlarmData_t *)pAlarm;

	if (!pstContents || !pstNaAlarmData)
	{
		return ERR_FAIL;
	}

	// Smartcard alarm check
	HxLOG_Print("pstNaAlarmData->eAlarmType = %d, pAlarm->ucNumOfScAlarms[%d], pAlarm->ucNumOfAccessAlarms[%d]\n", pstNaAlarmData->eAlarmType, pstNaAlarmData->ucNumOfScAlarms, pstNaAlarmData->ucNumOfAccessAlarms);
	if (pstNaAlarmData->ucNumOfScAlarms > 0 && pstNaAlarmData->pstScAlarmItems != NULL )
	{
		for (i = 0 ; i < pstNaAlarmData->ucNumOfScAlarms; i++)
		{
			HxLOG_Print("pstNaAlarmData->pstScAlarmItems[%d].eAlarm[%d]\n", i, pstNaAlarmData->pstScAlarmItems[i].eAlarm);
			switch (pstNaAlarmData->pstScAlarmItems[i].eAlarm)
			{
				case CA_ALARM_SMARTCARD_OK:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_OK;
					break;
				case CA_ALARM_SMARTCARD_COM_ERROR:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_COM_ERROR;
					break;
				case CA_ALARM_SMARTCARD_MUTE:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_MUTE;
					break;
				case CA_ALARM_SMARTCARD_REMOVED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_REMOVED;
					break;
				case CA_ALARM_SMARTCARD_INVALID:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_INVALID;
					break;
				case CA_ALARM_SMARTCARD_BLACKLISTED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_BLACKLISTED;
					break;
				case CA_ALARM_SMARTCARD_SUSPENDED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_SUSPENDED;
					break;
				case CA_ALARM_SMARTCARD_NEVER_PAIRED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_NEVER_PAIRED;
					break;
				case CA_ALARM_SMARTCARD_NOT_PAIRED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_NOT_PAIRED;
					break;
				case CA_ALARM_SMARTCARD_EXPIRED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_EXPIRED;
					break;
				case CA_ALARM_SMARTCARD_NOT_CERTIFIED:
					na_alarm_msg_type = eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED;
					break;
				default:
					HxLOG_Print("Unknown Alarm Value [%d]\n", pstNaAlarmData->pstScAlarmItems[i].eAlarm);
					na_alarm_msg_type = eCAS_NA_CA_ALARM_TYPE_ERR;
					break;
			}
			// WM_CA_SC_OK 일 경우 Access Alamr 결과를 보고
			// 어떤 메시지를 선택할지 결정한다.
		}
	}

	if (na_alarm_msg_type == eCAS_NA_CA_SMARTCARD_OK || na_alarm_msg_type == eCAS_NA_CA_ALARM_TYPE_ERR)
	{
		// Access Alarm check
		if (pstNaAlarmData->ucNumOfAccessAlarms > 0 && pstNaAlarmData->pstAccessAlarmItems)
		{
			for (i = 0; i < pstNaAlarmData->ucNumOfAccessAlarms; i++)
			{
				HxLOG_Print("pstNaAlarmData->pstAccessAlarmItems[%d].eAlarm[%d]\n", i, pstNaAlarmData->pstAccessAlarmItems[i].eAlarm);
				switch(pstNaAlarmData->pstAccessAlarmItems[i].eAlarm)
				{
					case CA_ALARM_ACCESS_FREE:
						na_access_msg_type = eCAS_NA_CA_ACCESS_FREE;
						break;

					case CA_ALARM_ACCESS_GRANTED:
						na_access_msg_type = eCAS_NA_CA_ACCESS_GRANTED;
						break;

					case CA_ALARM_ACCESS_GRANTED_PPT:
						na_access_msg_type = eCAS_NA_CA_ACCESS_GRANTED_PPT;
						break;

					case CA_ALARM_ACCESS_GRANTED_FOR_PREVIEW:
						na_access_msg_type = eCAS_NA_CA_ACCESS_GRANTED_FOR_PREVIEW;
						break;

					case CA_ALARM_ACCESS_GRANTED_BUT_WARNING_PREVIEW:
						na_access_msg_type = eCAS_NA_CA_ACCESS_GRANTED_BUT_WARNING_PREVIEW;
						break;

					case CA_ALARM_ACCESS_DENIED:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED;
						break;

					case CA_ALARM_ACCESS_DENIED_BUT_PREVIEW:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED_BUT_PREVIEW;
						break;

					case CA_ALARM_ACCESS_DENIED_FOR_PARENTAL_RATING:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED_FOR_PARENTAL_RATING;
						break;

					case CA_ALARM_ACCESS_BLACKOUT:
						na_access_msg_type = eCAS_NA_CA_ACCESS_BLACKOUT;
						break;

					case CA_ALARM_ACCESS_DENIED_BUT_PPT:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED_BUT_PPT;
						break;

					case CA_ALARM_ACCESS_DENIED_NO_VALID_CREDIT:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT;
						break;

					case CA_ALARM_ACCESS_DENIED_COPY_PROTECTED:
						na_access_msg_type = eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED;
						break;

					default :
						//don't enter here
						break;
				}
			}
		}
	}

	if (na_alarm_msg_type != eCAS_NA_CA_ALARM_TYPE_ERR && na_alarm_msg_type != eCAS_NA_CA_SMARTCARD_OK)
	{
		bDisplayCardErr = TRUE;
	}
	else
	{
		// current event가 expired인지는 Card error UI에서  체크해야 함 -

		switch(na_access_msg_type)
		{
			case eCAS_NA_CA_ACCESS_DENIED:
			case eCAS_NA_CA_ACCESS_DENIED_BUT_PREVIEW:
			case eCAS_NA_CA_ACCESS_BLACKOUT:
			case eCAS_NA_CA_ACCESS_DENIED_FOR_PARENTAL_RATING:
			case eCAS_NA_CA_ACCESS_DENIED_BUT_PPT:
			case eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT:
				bDisplayCardErr = TRUE;
				break;
		}
	}

	if (bDisplayCardErr)
	{
		BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Rcs",
						APP_CAS_PRIORITY,
						xproc_cas_NaBannerSmartCardError_Rcs,
						SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
						na_alarm_msg_type,
						0,
						0);
	}
	else
	{
		if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Rcs))
		{
			BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Rcs);
		}
	}

	if (na_alarm_msg_type == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->nLastPurchasedPPTEventId = 0;
	}
}
#endif

STATIC HERROR xmgr_cas_NaRcsConvertCalTimeToUnixTime(const TCalendarTime *pCalTime, UNIXTIME *pUnixTime)
{
	HERROR			hRes;
	HxDATETIME_t	dateTime;

	if ( pCalTime == NULL || pUnixTime == NULL )
	{
		return ERR_FAIL;
	}

	dateTime.stDate.usYear		= (HUINT16)(pCalTime->year + 1900);
	dateTime.stDate.ucMonth		= (HUINT8)pCalTime->month;
	dateTime.stDate.ucDay		= (HUINT8)pCalTime->day;
	dateTime.stTime.ucHour		= (HUINT8)pCalTime->hour;
	dateTime.stTime.ucMinute	= (HUINT8)pCalTime->minute;
	dateTime.stTime.ucSecond	= (HUINT8)pCalTime->second;

	return HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&dateTime,  pUnixTime);
}

STATIC HBOOL xmgr_cas_NaRcsCreditAvailable(void)
{
	if ( s_RCSCreditListData.status == CA_CREDIT_SUCCESS )
	{
		if ( s_RCSCreditListData.nNumberOfItems > 0 && s_RCSCreditListData.aCreditArray != NULL)
		{
			return TRUE;
		}
	}
	return FALSE;
}

STATIC TCaStatus	xmgr_cas_NaRcsCreditCallback(TCreditStatus  xStatus, TUnsignedInt8  xNumberOfItems, TCreditItem *pxCreditArray)
{
	HxLOG_Print("xStatus = 0x%x, xNumberOfItems = %d \n", xStatus, xNumberOfItems);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	s_RCSCreditListDataCB.status = xStatus;
	s_RCSCreditListDataCB.nNumberOfItems = xNumberOfItems;
	if(s_RCSCreditListDataCB.aCreditArray)
	{
		OxAP_Free(s_RCSCreditListDataCB.aCreditArray);
		s_RCSCreditListDataCB.aCreditArray = NULL;
	}

	if(pxCreditArray != NULL && xNumberOfItems > 0)
	{
		s_RCSCreditListDataCB.aCreditArray = (TCreditItem *)OxAP_Malloc(sizeof(TCreditItem)*xNumberOfItems);
		if(s_RCSCreditListDataCB.aCreditArray)
		{
			HxSTD_memcpy(s_RCSCreditListDataCB.aCreditArray, pxCreditArray, sizeof(TCreditItem)*xNumberOfItems );
		}
	}

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_CREDIT, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)&s_RCSCreditListDataCB);
#endif

	return CA_NO_ERROR;
}

STATIC TCaStatus	xmgr_cas_NaRcsPpvListCallback(TEventPurchaseListStatus xStatus, TUnsignedInt8 xNumberOfItems, TEventPurchaseListItem *pxEventPurchaseListArray)
{
	HxLOG_Print("xStatus = 0x%x, xNumberOfItems = %d \n", xStatus, xNumberOfItems);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PPV_LIST, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)xNumberOfItems);
#endif

	return CA_NO_ERROR;
}

STATIC TCaStatus	xmgr_cas_NaRcsRechargeNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 wParam, TUnsignedInt32 lParam)
{
	HxLOG_Print("xStatus = 0x%x, xNumberOfItems = %d \n", xStatus, wParam);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_RECHARGE, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)wParam);
#endif

	return CA_NO_ERROR;
}

STATIC TCaStatus	xmgr_cas_NaRcsPurchaseEventNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 xPurchaseType)
{
	HxLOG_Print("xStatus = 0x%x, xPurchaseType = %d \n", xStatus, xPurchaseType);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)xPurchaseType);
#endif

	return CA_NO_ERROR;
}

STATIC TCaStatus	xmgr_cas_NaRcsEventNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 hSvcHandle, TCaRequest *pRequest, PPV_EVENT_INFO *pEventInfo, TCaProduct *pProduct)
{
	HxLOG_Print("xStatus = 0x%x, hSvcHandle = %08x \n", xStatus, hSvcHandle);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	HxSTD_memset(&s_RCSPurchasableEventInformation, 0, sizeof(XmgrCas_NaPurchasableEvent_Rcs_t));

	if (pEventInfo == NULL || pProduct == NULL || pRequest == NULL)
	{
		BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)CA_EVENT_ERROR, (HINT32)0);
		return CA_ERROR;
	}

	if (xStatus == CA_EVENT_SUCCESS)
	{
		s_RCSPurchasableEventInformation.xStatus = (TEventStatus)xStatus;
		s_RCSPurchasableEventInformation.hSvc = (Handle_t)hSvcHandle;
		s_RCSPurchasableEventInformation.nPurchaseType = pEventInfo->xPurchaseType;
		s_RCSPurchasableEventInformation.pProduct = pProduct;
		s_RCSPurchasableEventInformation.pRequest = pRequest;
		HxSTD_memcpy(&s_RCSPurchasableEventInformation.stPPVEventInfo, pEventInfo, sizeof(PPV_EVENT_INFO));
		HxLOG_Print("xmgr_cas_NaRcsEventNotifyCallback = 0x%x\n", s_RCSPurchasableEventInformation.stPPVEventInfo.pCaProduct);
	}

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)&s_RCSPurchasableEventInformation);
#endif

	return CA_NO_ERROR;

}

STATIC TCaStatus	xmgr_cas_NaRcsPurchaseHistoryNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 xNumberOfItems, PPV_PURCHASE_LIST *pxPurchaseList)
{
	// not used

	HxLOG_Print("xStatus = 0x%x, xNumberOfItems = %d \n", xStatus, xNumberOfItems);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_HISTORY, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)xStatus, (HINT32)xNumberOfItems);
#endif

	return CA_NO_ERROR;
}

STATIC HERROR xmgr_cas_NaRcsGetEvent(Handle_t hSvc, HINT32 nEventId, SvcEpg_Event_t *pEPGEvt)
{
	SvcEpg_FindParam_t			stParam = {0, };
	POINTER_LIST_T				*pList = NULL;
	HERROR						hErr = ERR_FAIL;
	SvcEpg_Event_t 					*pEvent = NULL;

	if(pEPGEvt == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(&stParam, 0, sizeof(SvcEpg_FindParam_t));

	stParam.ulFindMask = (EPG_FIND_MASK_PF | EPG_FIND_MASK_SCH | EPG_FIND_MASK_EVENTID);
	stParam.hSvcHandle = hSvc;
	stParam.nEvtId = nEventId;

	// TODO: EPG 소스 정리로 인해 새로 구현하세요.
//	hErr = SVC_EPG_GetEventList(&stParam, &pList);
	if(pList != NULL )
	{
		// get pEvent
		pEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pList);
		if(pEvent != NULL)
		{
			HxSTD_memcpy(pEPGEvt, pEvent, sizeof(SvcEpg_Event_t));
		}
		else
		{
			HxLOG_Print("[%s] UTIL_LINKEDLIST_GetContents fail!!\n", __FUNCTION__);
		}

		// release
		hErr = SVC_EPG_ReleaseEventList(pList);
		if(hErr != ERR_OK)
		{
			HxLOG_Print("[%s] SVC_EPG_ReleaseEventList fail!!\n", __FUNCTION__);
		}
	}
	else
	{
		HxLOG_Print("[%s] failure to get EPG \n", __FUNCTION__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void xmgr_cas_NaRcsClosePPVIPPVDlg(void)
{
#if 0 //To comile for Octo2
	// close IPPV
	if(BUS_MGR_Get(xmgr_cas_NaIppvBuyingPopup_Rcs) != NULL)
	{
		BUS_MGR_Destroy(xmgr_cas_NaIppvBuyingPopup_Rcs);
	}

	// cloe PPV
	if(BUS_MGR_Get(xproc_cas_NaPpvInfoPopup_Rcs) != NULL)
	{
		BUS_MGR_Destroy(xproc_cas_NaPpvInfoPopup_Rcs);
	}
#endif
}

/*******************************************************************/
/********************      Public Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_RCS_LOCAL_FUNCTION______________________________________________________

BUS_Result_t 	xproc_cas_NaManagement_Rcs(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr;
	HxDATETIME_Date_t			stDate_t;
	HULONG				ulUnixTime = 0;
	XmgrCas_NaContent_Rcs_t	*pstContents = xmgr_cas_NaRcsGetContents();
	XmgrCas_NaPurchasableEvent_Rcs_t	*pPEvt = NULL;
	XmgrCas_NaCreditListData_t	*pCreditData = NULL;
	STATIC SvcEpg_Event_t	stEPGEvt;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			hErr = VK_CLOCK_GetTime(&ulUnixTime);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy(NULL);
				return MESSAGE_BREAK;
			}
			//init data
			VK_MEM_Memset(pstContents, 0, sizeof(XmgrCas_NaContent_Rcs_t));
			break;

		case eSEVT_SI_EIT :
			HxLOG_Print("MESSAGE : eSEVT_SI_EIT \n");
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_CREDIT:
			//1. credit 을 local에 보관. 다른 작없 없음
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY \n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				break;
			}

			if(p2 == CA_CREDIT_SUCCESS)
			{
				pCreditData = (XmgrCas_NaCreditListData_t*)p3;
				s_RCSCreditListData.status = pCreditData->status;
				s_RCSCreditListData.nNumberOfItems = pCreditData->nNumberOfItems;
				if(s_RCSCreditListData.aCreditArray)
				{
					OxAP_Free(s_RCSCreditListData.aCreditArray);
					s_RCSCreditListData.aCreditArray = NULL;
				}

				if(pCreditData->aCreditArray != NULL && pCreditData->nNumberOfItems > 0)
				{
					s_RCSCreditListData.aCreditArray = (TCreditItem *)OxAP_Malloc(sizeof(TCreditItem)*pCreditData->nNumberOfItems);
					if(s_RCSCreditListData.aCreditArray)
					{
						HxSTD_memcpy(s_RCSCreditListData.aCreditArray, pCreditData->aCreditArray, sizeof(TCreditItem)*pCreditData->nNumberOfItems );
					}
				}

			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PPV_LIST:
			// 1. PPV/IPPV 구매 popup 이 존재하는경우 구매한 이벤트에 포커스 된거면 close 한다.
			// 2. purchased list update -> 윈도우가 실행중이면 자동으로 업데이트 됨
			// purchased list updated
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PPV_LIST \n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				break;
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_RECHARGE:
			// 1. credit 충전 된 정보 표시
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_RECHARGE \n");
			if (p1 != eDxCAS_GROUPID_NA)
			{
				break;
			}

			if(p2 != CA_REQUEST_NO_ERROR)
			{
				break;
			}

#if 0 //To comile for Octo2
			// 이미 열린경우 닫는다.
			if(BUS_MGR_Get(xproc_cas_NaPpvCreditPopupSimple_Rcs) != NULL)
			{
				BUS_MGR_Destroy(xproc_cas_NaPpvCreditPopupSimple_Rcs);
			}
			// show rechaged window
			BUS_MGR_Create( "xproc_cas_NaPpvCreditPopupSimple_Rcs",
								APP_DIALOGBOX_PRIORITY,
								xproc_cas_NaPpvCreditPopupSimple_Rcs,
								HANDLE_NULL,
								(HINT32)1,
								p3,
								0);
#endif
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT:
			// 1. 구매 성공 여부 메시지와 credit 표시
			// 2. 활성화 된 PPV IPPV 윈도우중 현재 구매 된 이벤트와 같은 것 들은 종료를 한다. <- 각자 윈도우들이 destroy 되도록 하엿음
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT \n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				break;
			}
			// EPG에서만 동작을 한다.?
//			if (AP_UiState_GetUiState(eUI_STATE_LV_TOP) == UISTATE_EPG)
			{
#if 0 //To comile for Octo2
				// 이미 열린경우 닫는다.
				if(BUS_MGR_Get(xmgr_cas_NaRCS_PPVCreditPopup_Proc) != NULL)
				{
					BUS_MGR_Destroy(xmgr_cas_NaRCS_PPVCreditPopup_Proc);
				}
				// show rechaged window
				BUS_MGR_Create( "xmgr_cas_NaRCS_PPVCreditPopup_Proc",
									APP_DIALOGBOX_PRIORITY,
									xmgr_cas_NaRCS_PPVCreditPopup_Proc,
									HANDLE_NULL,
									(HINT32)CA_PROCESSING_NO_ERROR,
									0,
									0);
#endif
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY:
			// 1. event 구매 하라는 창이 open 되어야 한다.
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY \n");
			// is nagra?
			if (p1 != eDxCAS_GROUPID_NA)
			{
				break;
			}

			// check credit available
			if(xmgr_cas_NaRcsCreditAvailable() == TRUE)
			{
				break;
			}

			// check data
			pPEvt = (XmgrCas_NaPurchasableEvent_Rcs_t*)p3;
			if((TEventStatus)p2 != CA_EVENT_SUCCESS ||  pPEvt == NULL )
			{
				break;
			}

			hErr = xmgr_cas_NaRcsGetEvent(pPEvt->hSvc, pPEvt->stPPVEventInfo.xEventID, &stEPGEvt);
			if(hErr == ERR_OK)
			{
				break;
			}

			if(pPEvt->stPPVEventInfo.xPurchaseType == CA_EVENT_TYPE_SCRAMBLED_IPPV)
			{
			// IPPV window
				// 이미 열린경우 닫는다.
				xmgr_cas_NaRcsClosePPVIPPVDlg();
#if 0 //To comile for Octo2
				BUS_MGR_Create( "xmgr_cas_NaRCS_PPVBuyingPopup_Proc",
									APP_DIALOGBOX_PRIORITY,
									xmgr_cas_NaIppvBuyingPopup_Rcs,
									HANDLE_NULL,
									0,
									(HINT32)&stEPGEvt,
									(HINT32)pPEvt->hSvc);
#endif
			}
			else if(pPEvt->stPPVEventInfo.xPurchaseType == CA_EVENT_TYPE_SCRAMBLED_PPV)
			{
			// PPV	window
				// 이미 열린경우 닫는다.
				xmgr_cas_NaRcsClosePPVIPPVDlg();
#if 0 //To comile for Octo2
				BUS_MGR_Create( "xproc_cas_NaPpvInfoPopup_Rcs",
									APP_DIALOGBOX_PRIORITY,
									xproc_cas_NaPpvInfoPopup_Rcs,
									HANDLE_NULL,
									0,
									(HINT32)&stEPGEvt,
									(HINT32)pPEvt->hSvc);

#endif
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_HISTORY:
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_HISTORY \n");
			// not used
			break;

		case eMEVT_CAS_CHECK:
			break;

		case eMEVT_CAS_FAIL:
			if (p1 == eDxCAS_GROUPID_NA)
			{
				// not used
#if 0
				xmgr_cas_NaRcsUpdateCasMessageFromAlarmData(pstContents, (void*)p2);
#endif
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED...\n");
			break;

		case eMEVT_BUS_TIMER :
			break;

#if 0 //To comile for Octo2
		case MSG_GWM_QUIT_UI :
			return MESSAGE_PASS;
#endif

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;


#if 0

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT :
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT TEventStatus(%d), CasGroupId(%d)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				if (xmgr_cas_Namediaset_ResponseEventInfo((TEventStatus)hAction, (TCaEvent*)p2, (PPV_EVENT_INFO*)p3, pstContents) == ERR_OK)
				{

				}
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST:
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST objStatus(%d), CasGroupId(%d)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				hErr = xmgr_cas_Namediaset_ResponseOppvCallback((TCaObjectStatus)hAction, (TUnsignedInt32)p2, (OPPV_PURCHASE_INFO_ITEM*)p3, pstContents);
				HxLOG_Print("xmgr_cas_Namediaset_ResponseOppvCallback hErr = %d", hErr);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM:
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM objStatus(%d), CasGroupId(0x%x)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				hErr = xmgr_cas_Namediaset_ResponseRechargeAlarm(hAction, pstContents);
				HxLOG_Print("xmgr_cas_Namediaset_ResponseRechargeAlarm hErr = %d", hErr);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				pstContents->nRemainedCredit = (TCreditAmount)p3;
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED...\n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_Namediaset_CheckReservationPopup(pstContents);
			}
			return MESSAGE_BREAK;

#endif
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR	xmgr_cas_NaPpvSetEpgMode(HBOOL xEpgMode)
{
	return MW_CA_NA_PPV_SetEpgMode(xEpgMode);
}

HERROR  xmgr_cas_NaPpvForceCreatEventInfo(Handle_t hSvcHandle)
{
	return MW_CA_NA_PPV_ForceCreatEventInfo(hSvcHandle);
}

HERROR xmgr_cas_NaPpvGetPpvList(TEventPurchaseListStatus *xStatus, HUINT8 *xNumberOfItems, TEventPurchaseListItem **pxEventPurchaseListArray)
{

	return MW_CA_NA_PPV_GetPpvList(xStatus, (TUnsignedInt8*)xNumberOfItems, pxEventPurchaseListArray);
}

HERROR  xmgr_cas_NaPpvGetCredit(TCreditStatus *xStatus, HUINT8 *xNumberOfItems, TCreditItem **pxCreditArray)
{
	return MW_CA_NA_PPV_GetCredit(xStatus, (TUnsignedInt8*)xNumberOfItems, pxCreditArray);
}

HERROR  xmgr_cas_NaPpvGetCreditSimple(TCreditAmount	*pCreditAmount, TCreditStatus *pxStatus)
{
	HERROR			res = ERR_FAIL;
	TCreditStatus 		xStatus = CA_CREDIT_ERROR;
	TCreditItem 		*pxCreditArray = NULL;
	HUINT8		       xNumberOfItems = 0;

	if(pCreditAmount == NULL || pxStatus == NULL)
	{
		return ERR_FAIL;
	}

	if(xmgr_cas_NaIsSmartCardStatusOK() == TRUE)
	{
		res = xmgr_cas_NaPpvGetCredit(&xStatus, &xNumberOfItems, &pxCreditArray);
	}
	else
	{
		HxLOG_Info("Smartcard error \n");
	}

	if(res  == ERR_OK && xStatus == CA_CREDIT_SUCCESS && xNumberOfItems > 0 && pxCreditArray != NULL)
	{
		*pCreditAmount = pxCreditArray[0].credit;
		*pxStatus = xStatus;
		HxLOG_Print("current credit = %d\n", *pCreditAmount);
	}
	else
	{
		*pCreditAmount = 0;
		*pxStatus = xStatus;
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR  xmgr_cas_NaPpvGetCreditString(HUINT8* pStr)
{
	HERROR			res = ERR_FAIL;
	TCreditStatus 		xStatus = CA_CREDIT_ERROR;
	TCreditItem 		*pxCreditArray = NULL;
	HUINT8		       xNumberOfItems = 0;
	TCreditAmount	CreditAmount = 0;

	if(pStr == NULL)
	{
		return ERR_FAIL;
	}

	if(xmgr_cas_NaIsSmartCardStatusOK() == TRUE)
	{
		res = xmgr_cas_NaPpvGetCredit(&xStatus, &xNumberOfItems, &pxCreditArray);
	}
	else
	{
		HxLOG_Info("Smartcard error \n");
	}

	if(res  == ERR_OK && xStatus == CA_CREDIT_SUCCESS && xNumberOfItems > 0 && pxCreditArray != NULL)
	{
		CreditAmount = pxCreditArray[0].credit;
		HxLOG_Print("current credit = %d\n", CreditAmount);
		sprintf((char*)pStr, "%d.%02d", CreditAmount/100, CreditAmount%100);
	}
	else
	{
		sprintf((char*)pStr, "-.--");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR 	xmgr_cas_NaPpvIsPurchasedEvent(void *pData, HUINT8 *pEvtName, HBOOL *pbPurchased)
{
	HERROR 					hErr = ERR_FAIL;
	TEventPurchaseListStatus 	status = CA_EVENT_PURCHASE_LIST_ERROR;
	HUINT8		 			nNumberOfItems = 0;
	TEventPurchaseListItem 	*aEventPurchaseListArray = NULL;
	HINT32					nCnt = 0;
	HUINT8					*pucBuf = NULL;
	HUINT8					*pucBufEvt = NULL;
	HUINT8					*pucBufHistory = NULL;
	HINT32					nStrEvtLen = 0;
	HINT32					nStrHistoryLen = 0;
	UNIXTIME				utStartTime = 0;
	UNIXTIME				ulTimeOffset = 0;
	SvcEpg_Event_t			*pEvt = NULL;

	if(pData == NULL || pEvtName == NULL || pbPurchased == NULL)
	{
		HxLOG_Info("[xmgr_cas_NaPpvIsPurchasedEvent] null param. !!\n");
		return ERR_FAIL;
	}

	*pbPurchased = FALSE;

	pEvt = (SvcEpg_Event_t*)pData;


	if(xmgr_cas_NaIsSmartCardStatusOK() == TRUE)
	{
		hErr = xmgr_cas_NaPpvGetPpvList(&status, &nNumberOfItems, &aEventPurchaseListArray);
	}
	else
	{
		HxLOG_Info("[xmgr_cas_NaPpvIsPurchasedEvent]Smartcard error !!\n");
		return ERR_OK;
	}

	// ppv list가 없으면 purchased list가 없는 것으로 간주
	if(hErr != ERR_OK || status != CA_EVENT_PURCHASE_LIST_SUCCESS || nNumberOfItems == 0 || aEventPurchaseListArray == NULL)
	{
		HxLOG_Info("[xmgr_cas_NaPpvIsPurchasedEvent]cannot get ppv list !!\n");
		return ERR_OK;
	}

	// wizard에서 설정된 GMT offset을 알아온다
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_GMT_OFFSET_MINUTES, (HUINT32 *)&ulTimeOffset, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Info("[xmgr_cas_NaPpvIsPurchasedEvent]cannot get time offset !!\n");
		return ERR_OK;
	}

	// alloc memory
	pucBuf = OxAP_Malloc(NA_TEXT_SIZE512);
	pucBufEvt = OxAP_Malloc(NA_TEXT_SIZE512);
	pucBufHistory = OxAP_Malloc(NA_TEXT_SIZE512);
	if(pucBufEvt == NULL || pucBufEvt == NULL || pucBufHistory == NULL)
	{
		HxLOG_Info("[xmgr_cas_NaPpvIsPurchasedEvent]cant alloc memory !!\n");
		return ERR_OK;
	}
	HxSTD_memset(pucBuf, 0, NA_TEXT_SIZE512);
	HxSTD_memset(pucBufEvt, 0, NA_TEXT_SIZE512);
	HxSTD_memset(pucBufHistory, 0, NA_TEXT_SIZE512);

	// make event name string
	MWC_UTIL_DvbStrcpyStrOnly(pucBuf, pEvtName);
	nStrEvtLen = SvcCas_UtilStrLen(pucBuf);
	MWC_UTIL_BothTrimString(pucBuf, nStrEvtLen, pucBufEvt);
	nStrEvtLen = SvcCas_UtilStrLen(pucBufEvt);

	// check
	for (nCnt=0 ; nCnt < nNumberOfItems ; nCnt++)
	{
		if(aEventPurchaseListArray[nCnt].eventNameLength == 0)
		{
			continue;
		}

		HxSTD_memset(pucBuf, 0, NA_TEXT_SIZE512);
		HxSTD_memset(pucBufHistory, 0, NA_TEXT_SIZE512);

		// check time
		hErr = xmgr_cas_NaRcsConvertCalTimeToUnixTime(aEventPurchaseListArray[nCnt].pStartTime, &utStartTime);
		if(hErr != ERR_OK)
		{
			continue;
		}
		utStartTime += (UNIXTIME)(ulTimeOffset * 60);

		// 분단위 스타트 시간 비교에서 시간이 다르면
		if ((pEvt->ulStartTime - (pEvt->ulStartTime % 60)) != (utStartTime - (utStartTime % 60)))
		{
			continue;
		}

		// make history event name string
		nStrHistoryLen = aEventPurchaseListArray[nCnt].eventNameLength;
		MWC_UTIL_DvbStrncpyStrOnly(pucBuf, aEventPurchaseListArray[nCnt].pEventName, nStrHistoryLen);
		nStrHistoryLen = SvcCas_UtilStrLen(pucBuf);
		MWC_UTIL_BothTrimString(pucBuf, nStrHistoryLen, pucBufHistory);
		nStrHistoryLen = SvcCas_UtilStrLen(pucBufHistory);

		if ( nStrEvtLen < nStrHistoryLen )
		{
			nStrHistoryLen = nStrEvtLen;
		}

		// is same event ?
		if(MWC_UTIL_DvbStrncmp((HUINT8 *)pucBufEvt, (HUINT8 *)pucBufHistory, nStrHistoryLen) == 0)
		{
			*pbPurchased = TRUE;
			break;
		}
	}

	// free memory
	OxAP_Free(pucBufHistory);
	OxAP_Free(pucBufEvt);
	OxAP_Free(pucBuf);

	return ERR_OK;
}

HERROR xmgr_cas_NaPpvCheckPPVRCS(void *pData)
{
	BUS_Result_t				 lResult = ERR_FAIL;
	SvcEpg_Event_t					*pFocusEvent = NULL;
	XmgrCas_NaEpgEvent_t		*pFocusAPEvent = NULL;
	UNIXTIME					ulEventStartTime = 0;
	UNIXTIME					ulEventEndTime = 0;
	UNIXTIME					ulSysTime = 0;
	HBOOL						bPurchased = FALSE;
	HERROR						hErr = ERR_FAIL;
	XmgrCas_NaEpgEvent_t 		*pApEvt	 =  (XmgrCas_NaEpgEvent_t*)pData;



	if(pApEvt == NULL)
	{
		HxLOG_Print("[xmgr_cas_NaPpvCheckPPVRCS] null pointer!!\n");
		return ERR_FAIL;
	}

	pFocusEvent = (SvcEpg_Event_t*)&pApEvt->stEventInfo;
	pFocusAPEvent = (XmgrCas_NaEpgEvent_t*)pApEvt;
	ulEventStartTime = pFocusEvent->ulStartTime;
	ulEventEndTime = pFocusEvent->ulStartTime + pFocusEvent->ulDuration;
	hErr= VK_CLOCK_GetTime(&ulSysTime);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("[%s] time is not valid !\n", __FUNCTION__);
		return ERR_FAIL;
	}

	// check param.
	if(pFocusEvent == NULL || pFocusAPEvent == NULL)
	{
		HxLOG_Print("[%s] null !\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if(pFocusAPEvent->ulEventState != eNA_MODE_EPG_EVENT_VALID)
	{
		HxLOG_Print("[%s] ulEventState is not valid  !\n", __FUNCTION__);
		return ERR_FAIL;
	}

	// check purchased event
	if(pFocusEvent->u8bPpvFlag == 1 || pFocusEvent->u8bPpvFlag == 2)
	{
		hErr = xmgr_cas_NaPpvIsPurchasedEvent((void*)pFocusEvent, pFocusAPEvent->aucShortEventName, &bPurchased);
		if(hErr != ERR_OK)
		{
			bPurchased = FALSE;
		}

		if(bPurchased == TRUE)
		{
			HxLOG_Print("[%s] purchaed event !\n", __FUNCTION__);
			return ERR_FAIL;
		}
	}

	/* PPV */
#if 1
	if(pFocusEvent->u8bPpvFlag == 1) // PPV
#else // test
	if(pFocusEvent->u8bPpvFlag == 2) // PPV
#endif
	{
		if(ulEventEndTime <= ulSysTime)
		{
			HxLOG_Print("[%s] time is over !\n", __FUNCTION__);
			return ERR_FAIL;
		}

		// show PPV info window
#if 0 //To comile for Octo2
		BUS_MGR_Create( "xproc_cas_NaPpvInfoPopup_Rcs",
							APP_DIALOGBOX_PRIORITY,
							xproc_cas_NaPpvInfoPopup_Rcs,
							HANDLE_NULL,
							(HINT32)pFocusAPEvent,
							0,
							0);
#endif
		return ERR_OK;
	}
	/* IPPV */
#if 1
	else if(pFocusEvent->u8bPpvFlag == 2) // IPPV
#else // test
	else if(pFocusEvent->u8bPpvFlag == 1) // IPPV
#endif
	{
/*
		// 5분 이상 남은 이벤트만 구입 가능
		if(ulEventEndTime < ulSysTime + (5*SECONDS_PER_MIN))
		{
			HxLOG_Print("[%s] time + 5min is over  !\n", __FUNCTION__);
			return ERR_FAIL;
		}
*/
		// show PPV buying window
#if 0 //To comile for Octo2
		BUS_MGR_Create( "xmgr_cas_NaRCS_PPVBuyingPopup_Proc",
							APP_DIALOGBOX_PRIORITY,
							xmgr_cas_NaIppvBuyingPopup_Rcs,
							HANDLE_NULL,
							(HINT32)pFocusAPEvent,
							0,
							0);
#endif
		return ERR_OK;
	}

	return ERR_FAIL;
}


HERROR 	xmgr_cas_NaPpvGetPurchaseEvtInfo(Handle_t hSvcHandle, HUINT32 currTime, HINT32 *eventId)
{
	return MW_CA_NA_PPV_GetPurchaseEvtInfo(hSvcHandle, currTime, eventId);
}

HERROR xmgr_cas_NaRequestEventInfo(Handle_t hSvcHandle, HINT32 nEvtId)
{
	return MW_CA_NA_PPV_RequestEventInfo(hSvcHandle, nEvtId);
}

HERROR xmgr_cas_NaPpvRequestEventInfoByEpg(Handle_t hSvcHandle,  HINT32 nEvtId, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct)
{
#if 1
	HERROR	 		nRes = ERR_FAIL;
	TUnsignedInt32	xStatus = CA_EVENT_ERROR;

	if (pRequest == NULL || pEventInfo == NULL || pProduct == NULL)
	{
		return ERR_FAIL;
	}

	nRes = MW_CA_NA_PPV_RequestEventInfoByEpg(hSvcHandle, nEvtId, &xStatus, pRequest, pEventInfo, pProduct);
	if ( nRes == ERR_OK )
	{
		if ( xStatus == CA_EVENT_SUCCESS || xStatus == CA_EVENT_PURCHASE_PERIOD_BEFORE )
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
#else
	return MW_CA_NA_PPV_RequestEventInfoByEpg((TUnsignedInt32)hSvcHandle, (TUnsignedInt16)nEvtId, (TUnsignedInt32*)xStatus, pRequest, pEventInfo, pProduct);
#endif
}

HERROR  xmgr_cas_NaPpvRequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct)
{
	return MW_CA_NA_PPV_RequestPurchaseEvent(xType, pRequest, pProduct);
}

HERROR  xmgr_cas_NaPpvRequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus)
{
	return MW_CA_NA_PPV_RequestPurchaseEventByEpg(xType, pRequest, pProduct, pProStatus);
}

// callback
HERROR  xmgr_cas_NaPpvInitialize(void)
{
#if	defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	HxSTD_memset(&s_RCSPurchasableEventInformation, 0, sizeof(XmgrCas_NaPurchasableEvent_Rcs_t));
	s_RCSPurchasableEventInformation.xStatus = CA_EVENT_ERROR;

	HxSTD_memset(&s_RCSCreditListDataCB, 0, sizeof(XmgrCas_NaCreditListData_t));
	s_RCSCreditListDataCB.status = CA_CREDIT_ERROR;
	HxSTD_memset(&s_RCSCreditListData, 0, sizeof(XmgrCas_NaCreditListData_t));
	s_RCSCreditListData.status = CA_CREDIT_ERROR;

#endif
	return ERR_OK;
}

HERROR  xmgr_cas_NaPpvRegisterCreditCallback(void)
{
	return MW_CA_NA_PPV_RegisterCreditCallback(xmgr_cas_NaRcsCreditCallback);
}

HERROR  xmgr_cas_NaPpvRegisterPpvListCallback(void)
{
	return MW_CA_NA_PPV_RegisterPpvListCallback(xmgr_cas_NaRcsPpvListCallback);
}

HERROR  xmgr_cas_NaPpvRegisterRechargeNotifyCallback(void)
{
	return MW_CA_NA_PPV_RegisterRechargeNotifyCallback(xmgr_cas_NaRcsRechargeNotifyCallback);
}

HERROR  xmgr_cas_NaPpvRegisterRequestPurchaseEventNotifyCallback(void)
{
	return MW_CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(xmgr_cas_NaRcsPurchaseEventNotifyCallback);
}

HERROR  xmgr_cas_NaPpvRegisterRequestEventNotifyCallback(void)
{
	return MW_CA_NA_PPV_RegisterRequestEventNotifyCallback(xmgr_cas_NaRcsEventNotifyCallback);
}

HERROR  xmgr_cas_NaPpvRegisterPurchaseHistoryNotifyCallback(void)
{
	return MW_CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(xmgr_cas_NaRcsPurchaseHistoryNotifyCallback);
}

/* end of file */
