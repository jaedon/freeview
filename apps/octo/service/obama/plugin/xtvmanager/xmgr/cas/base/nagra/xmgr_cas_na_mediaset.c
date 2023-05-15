#include <octo_common.h>

#include <mgr_action.h>
#include <mgr_live.h>

#include <mgr_cas.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <settings.h>

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

#define 	MAX_NUM_POPUP_RESERVED		50
#define 	CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH	1000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSTATE_PPV_INIT = 0,
	eSTATE_PPV_TS_CHECK,
 	eSTATE_PPV_EVENT_INFO_REQUEST,
 	eSTATE_PPV_EVENT_CHECK_PURCHASABLE,
 	eSTATE_PPV_EVENT_PURCHASE_REQUEST,
 	eSTATE_PPV_EVENT_PURCHASE,
 	eSTATE_PPV_SC_ERROR
} XmgrCas_NaPpvState_e;

typedef enum
{
	ePOPUP_TYPE_RECHARGE = 1,
	ePOPUP_TYPE_OPPV
} XmgrCas_NaPopupType_e;

typedef	struct tagPOPUP_RESERVE_LIST_ITEM
{
	XmgrCas_NaPopupType_e	nType;
	TNAGRA_OP_TYPE			nNagraOPType;
//	TCreditAmount			nCredit;
	TUnsignedInt8			pName[MAX_EVENT_ITEM_NAME_LENGTH+1];
	struct tagPOPUP_RESERVE_LIST_ITEM*	pNext;
} XmgrCas_NaPopupReserveListItem_t;

typedef struct tagPurchasableEvent
{
	TPurchasableEventType 	nPurchaseType;
	TCaEvent*				pEvent;
	PPV_EVENT_INFO			stPPVEventInfo;
} XmgrCas_NaPurchasableEvent_t;

typedef struct
{
	Handle_t					hSvcHandle;
	XmgrCas_NaPpvState_e 		ePpvCurState;
	TNAGRA_OP_TYPE				eNagraOpType;
	TCreditAmount				nRemainedCredit;
	TUnsignedInt16 				nLastPurchasedPPTEventId;

	TPurchasableEventType 		nPurchaseType;
	TCaEvent*					pEvent;
	PPV_EVENT_INFO				stPPVEventType;
	XmgrCas_NaPopupReserveListItem_t		*stPopUpItems;
	TUnsignedInt16				nNumRemainedPopup;
	TUnsignedInt32				nPopupCredit;

	TBoolean					bWaitingNewTsCheckStart;
	TBoolean					bPaused;
	TBoolean					bSignalOff;
} XmgrCas_NaContent_Mediaset_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaContent_Mediaset_t		s_cas_na_mediaset_content;
STATIC XmgrCas_NaPurchasableEvent_t		s_PurchasableEventInformation;
STATIC HBOOL							s_bEventnfoRequested = FALSE;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_MEDIASET_LOCAL_FUNCTION______________________________________________________

STATIC HERROR				xmgr_cas_NaRequestTsCheck_Mediaset(void);
STATIC HERROR				xmgr_cas_NaResponseTsCheck_Mediaset(Handle_t hSvcHandle, TS_PERIOD_TYPE eTsType, XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC XmgrCas_NaContent_Mediaset_t*	xmgr_cas_NaGetContents_Mediaset(void);

STATIC void 				xmgr_cas_NaRemoveFirstReservedPopup_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC void 				xmgr_cas_NaAddReservedPopupMsgItem_Mediaset(TUnsignedInt8* pName, TUnsignedInt8 nNagraCardType, TUnsignedInt8 nPopupType, XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC HERROR				xmgr_cas_NaGetNagraOpType_Mediaset(TUnsignedInt8 *pNagraCardType);
STATIC HBOOL 				xmgr_cas_NaCheckPpvStepConfirm_Mediaset(void);
STATIC void 				xmgr_cas_NaAddOppvReservation_Mediaset(TUnsignedInt16 nNumToReserve, OPPV_PURCHASE_INFO_ITEM* pOppvList, XmgrCas_NaContent_Mediaset_t* pstContents, TUnsignedInt8 nNagraCardType);
STATIC HERROR				xmgr_cas_NaResponseOppvCallback_Mediaset(TCaObjectStatus objStatus, TUnsignedInt32 ulNum, const OPPV_PURCHASE_INFO_ITEM *pOppvItem, XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC void 				xmgr_cas_NaAddRechargeReservation_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, TUnsignedInt8 nNagraCardType);
STATIC HERROR				xmgr_cas_NaResponseRechargeAlarm_Mediaset(TCaObjectStatus objStatus, XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC HERROR				xmgr_cas_NaCheckReservationPopup_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents);
STATIC HERROR				xmgr_cas_NaUpdateCasMessageFromAlarmData_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, void* pAlarm);
STATIC HBOOL 				xmgr_cas_NaIsPopupEnabled_Mediaset(void);
STATIC void 				xmgr_cas_NaDisplayPurchaseResult_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, HUINT32 nPurchaseType);

#define _____XMGR_CAS_NA_MEDIASET_LOCAL_FUNCTION______________________________________________________

STATIC XmgrCas_NaContent_Mediaset_t*	xmgr_cas_NaGetContents_Mediaset(void)
{
	return &s_cas_na_mediaset_content;
}


STATIC HERROR	xmgr_cas_NaRequestTsCheck_Mediaset(void)
{
	Handle_t				curSvcHandle;
	HERROR					hErr;
	XmgrCas_NaContent_Mediaset_t	*pstContents = xmgr_cas_NaGetContents_Mediaset();

	hErr = MGR_LIVE_CurrentSvcInfoGet(MGR_ACTION_GetMainActionId(), &curSvcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_LIVE_CurrentSvcInfoGet ERROR(%d)\n", hErr);
		return ERR_FAIL;
	}

	hErr = xmgr_cas_NaRequestTsCheck(curSvcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaRequestTsCheck ERROR(%d)\n", hErr);
		return ERR_FAIL;
	}

	pstContents->ePpvCurState = eSTATE_PPV_TS_CHECK;

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_NaResponseTsCheck_Mediaset(Handle_t hSvcHandle, TS_PERIOD_TYPE eTsType, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	HUINT8	nPrevNagraOPType = pstContents->eNagraOpType;
	HERROR					hErr = ERR_OK;
	Handle_t				curSvcHandle;
	XmgrCas_NaAlamType_e		eScAlarm;

	hErr = MGR_LIVE_CurrentSvcInfoGet(MGR_ACTION_GetMainActionId(), &curSvcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_LIVE_CurrentSvcInfoGet ERROR(%d)\n", hErr);
		return ERR_FAIL;
	}

	if (curSvcHandle != hSvcHandle)
	{
		HxLOG_Error("xmgr_cas_NaResponseTsCheck_Mediaset hSvcHandle(0x%x), hSvcHandle(0x%x)\n", pstContents->hSvcHandle, hSvcHandle);
		return ERR_FAIL;
	}
	HxLOG_Print("eTsType = %d\n", eTsType);

	pstContents->hSvcHandle = hSvcHandle;		// hSvcHandle 과 curSvcHandle 이 같음

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	HxLOG_Print("eScAlarm = %d\n", eScAlarm);

	if (eTsType == TS_NA_MEDIASET_PERIOD || eTsType == TS_CLEAR_NA_MEDIASET_PERIOD)
	{
		pstContents->eNagraOpType = eNAGRA_OP_TYPE_MEDIASET;

		if (eTsType == TS_CLEAR_NA_MEDIASET_PERIOD)
		{

			// IPPV 제거
			if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset))
			{
				BUS_MGR_Destroy(xmgr_cas_NaPpvBanner_Mediaset);
				xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
				pstContents->stPPVEventType.pCaRequest = NULL;
			}

			// CARD Error message 제거
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
			{
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);
			}
		}
		else
		{
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset) == NULL && BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea) == NULL)
			{
				if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_MEDIASET))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");

						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Mediaset,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										0,
										0,
										0);
					}
				}
			}
		}
	}
	else if (eTsType == TS_NA_OTHER_PERIOD || eTsType == TS_CLEAR_NA_OTHER_PERIOD)
	{
		pstContents->eNagraOpType = eNAGRA_OP_TYPE_PANGEA;

		if (nPrevNagraOPType != eNAGRA_OP_TYPE_PANGEA)
		{
			if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
			{
				if (xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
				{
					if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
					{
						BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);

						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Pangea,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										CA_EVENT_ERROR,			// expired를 없애기 위해 편법으로 CA_EVENT_ERROR를 보낸다.
										0,
										0);

						BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE, (Handle_t)NULL, eDxCAS_GROUPID_NA, eNAGRA_OP_TYPE_PANGEA, 0); // tscheck가 끝나기 전에 mediaset banner window 가 먼저 떠 있는 경우 바꿔치기
					}
				}
			}
		}

		if (eTsType == TS_CLEAR_NA_OTHER_PERIOD)
		{

			// IPPV 제거
			if (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
			{
				BUS_MGR_Destroy(xproc_cas_NaPpvBanner_Pangea);
				xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
				pstContents->stPPVEventType.pCaRequest = NULL;
			}

			// CARD Error message 제거
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
			{
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Pangea);
			}
		}
		else
		{
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset) == NULL && BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea) == NULL)
			{
				if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");

						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Mediaset,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										0,
										0,
										0);
					}
				}
			}
		}
	}
	else
	{
		pstContents->eNagraOpType = eNAGRA_OP_TYPE_UNDEFINED;
	}

	HxLOG_Print("s_cas_na_mediaset_content.eNagraOpType = %d\n", pstContents->eNagraOpType);

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_NaMediasetRequestEventInfo(TS_PERIOD_TYPE eTsType, TBoolean isSCMatchedWithPMT, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	HERROR 				hErr;
	XmgrCas_NaAlamType_e	eAlarmType;

	hErr = xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
	HxLOG_Print("eTsType(%d), isSCMatchedWithPMT(%d), eAlarmType(%d), hErr(%d)\n", eTsType, isSCMatchedWithPMT, eAlarmType, hErr);

	switch(eTsType)
	{
		case TS_NA_MEDIASET_PERIOD:
		case TS_CLEAR_NA_MEDIASET_PERIOD:
			{
				if ((eAlarmType == eCAS_NA_CA_SMARTCARD_OK) && isSCMatchedWithPMT)
				{
					if ((eTsType == TS_NA_MEDIASET_PERIOD)/* && (xmgr_cas_NaRequestEventInfo() == ERR_OK)*/)		// Mediaset IPPV등 인지 체크
					{
						pstContents->ePpvCurState = eSTATE_PPV_EVENT_INFO_REQUEST;
					}
					else
					{
						pstContents->ePpvCurState = eSTATE_PPV_INIT;
						BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
				}
				else
				{
					pstContents->ePpvCurState = eSTATE_PPV_SC_ERROR;
				}
			}
			break;

		case TS_NA_OTHER_PERIOD:
		case TS_CLEAR_NA_OTHER_PERIOD:
			{
				if ((eAlarmType == eCAS_NA_CA_SMARTCARD_OK) && isSCMatchedWithPMT)
				{
					if ((eTsType == TS_NA_OTHER_PERIOD)/* && (xmgr_cas_NaRequestEventInfo() == ERR_OK)*/) 	// Mediaset IPPV등 인지 체크
					{
						pstContents->ePpvCurState = eSTATE_PPV_EVENT_INFO_REQUEST;
					}
					else
					{
						pstContents->ePpvCurState = eSTATE_PPV_INIT;
						BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
				}
				else
				{
					pstContents->ePpvCurState = eSTATE_PPV_SC_ERROR;
				}
			}
			break;

		case TS_NOT_NA_PERIOD:
			{
				pstContents->ePpvCurState = eSTATE_PPV_INIT;
				BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
				xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
				pstContents->stPPVEventType.pCaRequest = NULL;
			}
			break;

		default :
			{
				pstContents->ePpvCurState = eSTATE_PPV_INIT;
			}
			break;
	};
	HxLOG_Print("s_cas_na_mediaset_content.ePpvCurState = %d\n", pstContents->ePpvCurState);

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_NaMediasetCheckSamePpvEvent(PPV_EVENT_INFO* prevEvent, PPV_EVENT_INFO* currEvent, HBOOL *bSame)
{
	if (!prevEvent || !currEvent || !bSame)
	{
		return ERR_FAIL;
	}

	*bSame = TRUE;

	if (prevEvent->duration != currEvent->duration || prevEvent->xCost != currEvent->xCost || prevEvent->xEventStartTime != currEvent->xEventStartTime)
	{
		*bSame = FALSE;
		HxLOG_Print("different 1\n");
	}

	if (prevEvent->xPurchaseStartTime != currEvent->xPurchaseStartTime || prevEvent->xPurchaseEndTime != currEvent->xPurchaseEndTime || prevEvent->xEventID != currEvent->xEventID)
	{
		*bSame = FALSE;
		HxLOG_Print("different 2\n");
	}

	if (prevEvent->xSliceTime != currEvent->xSliceTime || prevEvent->xProductId != currEvent->xProductId || prevEvent->xPurchaseType != currEvent->xPurchaseType)
	{
		*bSame = FALSE;
		HxLOG_Print("different 3\n");
	}

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_NaMediasetResponseEventInfo(TEventStatus xStatus, const TCaEvent *pEvent, const PPV_EVENT_INFO *pEventInfo, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	MgrAction_State_e					state;
	AP_UiState_t				eUiState;
	HERROR						hErr = ERR_OK;
	TUnsignedInt8				nCardOpType;

	HxLOG_Print("ENTER\n");

	if (xStatus == CA_EVENT_SUCCESS && pEventInfo != NULL)
	{
		state = MGR_ACTION_GetState();
		if (state == eAmState_ACTIVE_STANDBY)
		{
			HxLOG_Print("Active standby mode - exit, pEventInfo->pCaRequest = %d\n", pEventInfo->pCaRequest);
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest);

			return ERR_OK;
		}

		if (BUS_MGR_Get(AP_WtvPinCode_Proc_0))
		{
			// pincode 창이 떠 있는 경우
			HxLOG_Print("pincode mode - exit, pEventInfo->pCaRequest = %d\n", pEventInfo->pCaRequest);
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest);

			return ERR_OK;
		}

		if (xmgr_cas_NaIsPopupEnabled_Mediaset()) // popup이 있으면 popup이 닫힐 때까지 IPPV/PPV는 띄우지 않는다.
		{
			HxLOG_Print("xmgr_cas_NaIsPopupEnabled_Mediaset TRUE, pEventInfo->pCaRequest = %d\n", pEventInfo->pCaRequest);
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거

			return ERR_OK;
		}

		eUiState = AP_UiState_GetUiState(eUI_STATE_LV_TOP);
		if ((eUiState == UISTATE_MENU) ||		// Menu UI
			(eUiState == UISTATE_EPG))			// EPG UI
		{
			HxLOG_Print("MENU mode\n");
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거
			return ERR_OK;
		}
	}

	if (pEventInfo)
	{
		HxLOG_Print("pEventInfo->xPuchaseType = %d, pEventInfo->xEventID = %d\n",  pEventInfo->xPurchaseType, pEventInfo->xEventID);
	}

	switch (xStatus)
	{
		case CA_EVENT_SUCCESS:	// IPPV or PPT
			if (pEventInfo == NULL || pEvent == NULL)
			{
				HxLOG_Print("EXIT - pEventInfo or pEvent NULL\n");
				return ERR_FAIL;
			}
			if (pEventInfo->pCaRequest == NULL || pEventInfo->pCaProduct == NULL)
			{
				HxLOG_Print("EXIT - pEventInfo->pRequest or pEventInfo->pProduct NULL\n");
				return ERR_FAIL;
			}

			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))	// Scrambled & expired banner가 떠 있는 경우에는 expired 를 없애야 한다.
			{
				BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
								APP_CAS_PRIORITY,
								xproc_cas_NaBannerSmartCardError_Mediaset,
								SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
								CA_EVENT_ERROR,			// expired를 없애기 위해 편법으로 CA_EVENT_ERROR를 보낸다.
								0,
								0);
			}

			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))	// Scrambled & expired banner가 떠 있는 경우에는 expired 를 없애야 한다.
			{
				BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
								APP_CAS_PRIORITY,
								xproc_cas_NaBannerSmartCardError_Pangea,
								SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
								CA_EVENT_ERROR,			// expired를 없애기 위해 편법으로 CA_EVENT_ERROR를 보낸다.
								0,
								0);
			}

			HxLOG_Print("xStatus = CA_EVENT_SUCCESS, Event Name(%s), pRequest(%d)\n", pEventInfo->xEvtName, pEventInfo->pCaRequest);

			// 이미 떠 있는 경우는 정보만 갱신 - 특히 pCaRequest가 바뀌는 경우는 반드시 xmgr_cas_NaDisposeCaRequest로 이전 pCaRequest 제거할 것
			if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset) || BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
			{
				HERROR 	hErr = ERR_OK;
				HBOOL	bSame = FALSE;

				hErr = xmgr_cas_NaMediasetCheckSamePpvEvent(&pstContents->stPPVEventType, pEventInfo, &bSame);
				if (hErr == ERR_OK && bSame == TRUE)
				{
					if (pEventInfo->xPurchaseType == CA_EVENT_PPT)
					{
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
					else
					{
						//xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = pEventInfo->pCaRequest;
					}
					break;
				}
				else
				{
					if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset))
					{
						BUS_MGR_Destroy(xmgr_cas_NaPpvBanner_Mediaset);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
					else if (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
					{
						BUS_MGR_Destroy(xproc_cas_NaPpvBanner_Pangea);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
				}
			}

			HxLOG_Print("pEventInfo->xPurchaseType = %d, pstContents->stPPVEventType.xEventID = %d, pstContents->nLastPurchasedPPTEventId = %d, pstContents->bSignalOff = %d\n", pEventInfo->xPurchaseType, pstContents->stPPVEventType.xEventID, pstContents->nLastPurchasedPPTEventId, pstContents->bSignalOff);

			if ((pEventInfo->xPurchaseType == CA_EVENT_PPT) && (pstContents->nLastPurchasedPPTEventId != 0) && (pEventInfo->xEventID == pstContents->nLastPurchasedPPTEventId))
			{
				// 자동 구매해 버린다. <= 이 경우는 signal out->in
				if (pstContents->bSignalOff == TRUE)
				{
					xmgr_cas_NaRequestEventPurchase_Mediaset();
				}

				// Antenna in/out or Menu in/out 시 PPT window를 다시 띄우지 않는다.
				xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest);		// 이번 pCaRequest 제거

				pstContents->nPurchaseType = 0;
				pstContents->pEvent = NULL;
				HxSTD_memset(&pstContents->stPPVEventType, 0, sizeof(PPV_EVENT_INFO));

				BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);

				break;
			}

			// Multi operator 체크
			hErr = xmgr_cas_NaGetNagraOpType_Mediaset(&nCardOpType);
			if (hErr != ERR_OK)
			{
				nCardOpType = eNAGRA_OP_TYPE_MEDIASET;
			}
			if (pstContents->eNagraOpType == eNAGRA_OP_TYPE_PANGEA)
			{
				if (nCardOpType == eNAGRA_OP_TYPE_PANGEA)
				{
					HxSTD_memcpy(&pstContents->stPPVEventType, pEventInfo, sizeof(PPV_EVENT_INFO));

					// 이미 xmgr_cas_NaPpvPangea_Banner_Proc가 있더라도 갱신
					BUS_MGR_Create("xproc_cas_NaPpvBanner_Pangea",
									APP_CAS_PPV_PRIORITY,
									xproc_cas_NaPpvBanner_Pangea,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									pstContents->hSvcHandle,
									&pstContents->stPPVEventType,
									0);

					break;
				}
			}
			else
			{
				if (nCardOpType == eNAGRA_OP_TYPE_MEDIASET)
				{
					HxSTD_memcpy(&pstContents->stPPVEventType, pEventInfo, sizeof(PPV_EVENT_INFO));

					// 이미 xmgr_cas_NaPpvMediaset_Banner_Proc가 있더라도 갱신
					BUS_MGR_Create("xmgr_cas_NaPpvBanner_Mediaset",
									APP_CAS_PPV_PRIORITY,
									xmgr_cas_NaPpvBanner_Mediaset,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									pstContents->hSvcHandle,
									&pstContents->stPPVEventType,
									0);

					break;
				}
			}

			// Multi operator 체크가 실패한 경우
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거
			break;

		case CA_EVENT_ERROR:
			HxLOG_Print("CA_EVENT_ERROR :\n");
			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
			xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
			pstContents->stPPVEventType.pCaRequest = NULL;

			if (pEventInfo)
			{
				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset) && (pEventInfo->xEventID != 0))	// Scrambled & expired banner가 떠 있는 경우에는 expired 를 없애야 한다.
				{
					BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
									APP_CAS_PRIORITY,
									xproc_cas_NaBannerSmartCardError_Mediaset,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									CA_EVENT_ERROR,
									0,
									0);
				}

				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea) && (pEventInfo->xEventID != 0))	// Scrambled & expired banner가 떠 있는 경우에는 expired 를 없애야 한다.
				{
					BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
									APP_CAS_PRIORITY,
									xproc_cas_NaBannerSmartCardError_Pangea,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									CA_EVENT_ERROR,
									0,
									0);
				}
			}
			break;

		case CA_EVENT_PURCHASE_PERIOD_OVER:
			if (pEventInfo)
			{
				HxLOG_Print("CA_EVENT_PURCHASE_PERIOD_OVER : s_PurchasableEventInformation.stPPVEventInfo.xProductId = %d, pEventInfo->xProductId = %d\n", s_PurchasableEventInformation.stPPVEventInfo.xProductId, pEventInfo?pEventInfo->xProductId:0);
				if (s_PurchasableEventInformation.stPPVEventInfo.xProductId != pEventInfo->xProductId)
				{
					BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
					xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
					pstContents->stPPVEventType.pCaRequest = NULL;
				}

				if (pstContents->eNagraOpType == eNAGRA_OP_TYPE_PANGEA)
				{
					if (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
					{
						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Pangea,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										CA_EVENT_ERROR,
										0,
										0);
					}
					else
					{
						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Pangea,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										CA_EVENT_PURCHASE_PERIOD_OVER,
										0,
										0);
					}

					BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE, (Handle_t)NULL, eDxCAS_GROUPID_NA, eNAGRA_OP_TYPE_PANGEA, 0); // tscheck가 끝나기 전에 mediaset banner window 가 먼저 떠 있는 경우 바꿔치기
				}
				else
				{
					if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset))
					{
						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Mediaset,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										CA_EVENT_ERROR,
										0,
										0);
					}
					else
					{
						BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
										APP_CAS_PRIORITY,
										xproc_cas_NaBannerSmartCardError_Mediaset,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										CA_EVENT_PURCHASE_PERIOD_OVER,
										0,
										0);
					}
				}
			}
			break;
		default:
			break;
#if 0
		case CA_EVENT_TS_FAIL:
			NA_PPV_ClosePPVWindow();
			DCM_Print("[Ax_na_ppv_state.c] xStatus = CA_EVENT_TS_FAIL\n");

			AX_SM_SetCurrentState(FSM_PPV_GetFsmHandle(), STATE_PPV_INIT, 0, 0, 0);
			break;

		case CA_EVENT_NO_VALID_CREDIT:
			DCM_Print("[Ax_na_ppv_state.c] xStatus = CA_EVENT_NO_VALID_CREDIT\n");
#ifdef 	S_NAGRA_OTHER_OP
			if (s_nNagraOPType == NAGRA_OP_TYPE_OTHER)
			{
				APMD_UI_MGR_RequestDisplay(AXMD_NA_GetUIModuleHandle(NA_UI_BARKER_MSG_WND), (HUINT32)UI_MGR_BARKER_TYPE_ACCESS_ERROR,
					WM_CA_ACCESS_DENIED, RES_NAGRA_OTHER_PF|STR_ID_MSET_ERR_NAGRA_OTHER_INSUFFICIENT_CREDIT);			}
			else
#endif
			{
				APMD_UI_MGR_RequestDisplay(AXMD_NA_GetUIModuleHandle(NA_UI_BARKER_MSG_WND), (HUINT32)UI_MGR_BARKER_TYPE_ACCESS_ERROR,
					WM_CA_ACCESS_DENIED, RES_MEDIASET_PF|STR_ID_MSET_ERR_NOT_ENOUGH_CREDIT);
			}
			AX_SM_SetCurrentState(FSM_PPV_GetFsmHandle(), STATE_PPV_INIT, 0, 0, 0);
			break;
		default:
			DCM_Print("[Ax_na_ppv_state.c] xStatus = %d\n",  xStatus);
			AX_SM_SetCurrentState(FSM_PPV_GetFsmHandle(), STATE_PPV_INIT, 0, 0, 0);
			break;
#endif
	}

	return ERR_OK;
}

STATIC void 	xmgr_cas_NaRemoveFirstReservedPopup_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents)
{
	XmgrCas_NaPopupReserveListItem_t 	*pTempItem = NULL;

	if (pstContents->nNumRemainedPopup > 0 && pstContents->stPopUpItems != NULL)
	{
		pTempItem = pstContents->stPopUpItems;

		pstContents->stPopUpItems = pstContents->stPopUpItems->pNext;

		if (pTempItem)
		{
			OxCAS_Free(pTempItem);
		}

		pstContents->nNumRemainedPopup--;
		HxLOG_Print("pstContents->nNumRemainedPopup = %d\n", pstContents->nNumRemainedPopup);
		if (pstContents->stPopUpItems == NULL)
		{
			HxLOG_Print("stReservedPOPUPList = NULL\n");
		}
	}
}

STATIC void xmgr_cas_NaAddReservedPopupMsgItem_Mediaset(TUnsignedInt8* pName, TUnsignedInt8 nNagraCardType, TUnsignedInt8 nPopupType, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	XmgrCas_NaPopupReserveListItem_t		*pNewItem = NULL, *pTempItem = NULL;

	if (pName)
	{
		HxLOG_Print("pstContents->nNumRemainedPopup = %d, pName = %s\n", pstContents->nNumRemainedPopup, pName);
	}

	if (pstContents->nNumRemainedPopup >= MAX_NUM_POPUP_RESERVED)
	{
		xmgr_cas_NaRemoveFirstReservedPopup_Mediaset(pstContents);
	}

	pNewItem = (XmgrCas_NaPopupReserveListItem_t *)OxCAS_Malloc(sizeof(XmgrCas_NaPopupReserveListItem_t));
	if ( pNewItem == NULL )
	{
		HxLOG_Print("Can not allocate Memory\n");
		return;
	}

	pNewItem->nNagraOPType = nNagraCardType;
	pNewItem->nType = nPopupType;

	HxLOG_Print("pNewItem->nNagraOPType = %d\n", pNewItem->nNagraOPType);

	HxSTD_memset(pNewItem->pName, 0, MAX_EVENT_ITEM_NAME_LENGTH+1);
	if (pName)
	{
		MWC_UTIL_DvbStrncpy(pNewItem->pName, pName, MAX_EVENT_ITEM_NAME_LENGTH);
	}
	pNewItem->pNext = NULL;

	if (pstContents->stPopUpItems)
	{
		pTempItem = pstContents->stPopUpItems;
		if ((pNewItem->nType == ePOPUP_TYPE_RECHARGE) && (pTempItem->nType == ePOPUP_TYPE_RECHARGE))
		{
			// recharge resveration 창은 이미 예약된 것이 있으면 그걸로 띄운다.
			HxLOG_Print("Already another recharge popup reserved. pstContents->nNumRemainedPopup = %d\n", pstContents->nNumRemainedPopup);
			OxCAS_Free(pNewItem);
			pNewItem = NULL;
			return;
		}

		while(pTempItem->pNext)
		{
			if ((pNewItem->nType == ePOPUP_TYPE_RECHARGE) && (pTempItem->pNext->nType == ePOPUP_TYPE_RECHARGE))
			{
				// recharge resveration 창은 이미 예약된 것이 있으면 그걸로 띄운다.
				HxLOG_Print("Already another recharge popup reserved. pstContents->nNumRemainedPopup = %d\n", pstContents->nNumRemainedPopup);
				OxCAS_Free(pNewItem);
				pNewItem = NULL;
				return;
			}
			pTempItem = pTempItem->pNext;
		}
		pTempItem->pNext = pNewItem;
	}
	else
	{
		HxLOG_Print("pstContents->stPopUpItems = NULL\n");
		pstContents->stPopUpItems = pNewItem;
	}

	pstContents->nNumRemainedPopup++;
}


STATIC HERROR xmgr_cas_NaGetNagraOpType_Mediaset(TUnsignedInt8 *pNagraCardType)
{
	HERROR						hErr;
	TPid						nProviderId;

	hErr = xmgr_cas_NaCakGetProviderId(&nProviderId);
	if (hErr != ERR_OK) // FALSE가 올라오면 안된다. Card를 IN했을 경우에는 한번 올라올 수 있지만 RECHARGE나 OPPV가 왔을 경우에는 FALSE가 return 되면 안된다.
	{
		*pNagraCardType = eNAGRA_OP_TYPE_UNDEFINED;
		return ERR_FAIL;
	}
	else
	{
		if (nProviderId == NA_MEDIASET_PROVIDER_ID || nProviderId == 0x0101)		// 0x0101은 테스트용 스트림에만 있는 것이고 미디어셋용이 아니다.
		{
			*pNagraCardType = eNAGRA_OP_TYPE_MEDIASET;
		}
		else
		{
			*pNagraCardType = eNAGRA_OP_TYPE_PANGEA;
		}
	}

	return ERR_OK;
}

STATIC HBOOL xmgr_cas_NaCheckPpvStepConfirm_Mediaset(void)
{
	if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset))
	{
		if (xmgr_cas_NaPpvBannerCheckStepConfirm_Mediaset())
		{
			return TRUE;
		}
	}

	if (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
	{
		if (xmgr_cas_NaPpvPangeaBannerCheckStepConfirm())
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC void xmgr_cas_NaAddOppvReservation_Mediaset(TUnsignedInt16 nNumToReserve, OPPV_PURCHASE_INFO_ITEM* pOppvList, XmgrCas_NaContent_Mediaset_t* pstContents, TUnsignedInt8 nNagraCardType)
{
	TUnsignedInt16				index = 0;
	OPPV_PURCHASE_INFO_ITEM*	pLocalOppvItem = pOppvList;
	TUnsignedInt8 				nPopupType;

	if (!pOppvList || nNumToReserve == 0)
	{
		return;
	}

	nPopupType = ePOPUP_TYPE_OPPV;

	pLocalOppvItem = pOppvList;
	for (index = 0; index < nNumToReserve; index++)
	{
		if (pLocalOppvItem)
		{
			xmgr_cas_NaAddReservedPopupMsgItem_Mediaset(pLocalOppvItem->pName, nNagraCardType, nPopupType, pstContents);
			HxLOG_Print("index(%d), name(%s)\n", index, pLocalOppvItem->pName);
			pLocalOppvItem = pLocalOppvItem->pNext;
		}
	}

	pstContents->nPopupCredit = pstContents->nRemainedCredit;
}

STATIC HERROR	xmgr_cas_NaResponseOppvCallback_Mediaset(TCaObjectStatus objStatus, TUnsignedInt32 ulNum, const OPPV_PURCHASE_INFO_ITEM *pOppvItem, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	TUnsignedInt8				nNagraCardType;
	HERROR						hErr;
	AP_UiState_t				eUiState;
	MgrAction_State_e					state;

	HxLOG_Print("ENTER - objStatus(%d), ulNum(%d)\n", objStatus, ulNum);

	if (objStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Print("\n");
		return ERR_FAIL;
	}

	// Credit을 보여주는 window가 있는 경우 credit 갱신해 줌
	xmgr_cas_NaGetScCredit(&pstContents->nRemainedCredit);
	pstContents->nPopupCredit = pstContents->nRemainedCredit;	// 다음에  뜰 Recharge/OPPV popup credit 갱신
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)objStatus, (HINT32)pstContents->nRemainedCredit);

	hErr = xmgr_cas_NaGetNagraOpType_Mediaset(&nNagraCardType);
	if (hErr != ERR_OK)
	{
		nNagraCardType = eNAGRA_OP_TYPE_MEDIASET;	// default 는 Mediaset으로
	}

	state = MGR_ACTION_GetState();
	if (state == eAmState_ACTIVE_STANDBY)
	{
		HxLOG_Print("Active standby mode - AddReserve\n");
		xmgr_cas_NaAddOppvReservation_Mediaset(ulNum, pOppvItem, pstContents, nNagraCardType);
		return ERR_OK;
	}

	eUiState = AP_UiState_GetUiState(eUI_STATE_LV_TOP);
	if ((eUiState == UISTATE_MENU) ||			// Menu UI
		(eUiState == UISTATE_EPG))				// EPG UI
	{
		HxLOG_Print("Menu or EPG exists.... -> ADD reservation\n");
		xmgr_cas_NaAddOppvReservation_Mediaset(ulNum, pOppvItem, pstContents, nNagraCardType);
		return ERR_OK;
	}

	if (xmgr_cas_NaCheckPpvStepConfirm_Mediaset())
	{
		HxLOG_Print("xmgr_cas_NaPpvBannerCheckStepConfirm_Mediaset = TRUE\n");
		xmgr_cas_NaAddOppvReservation_Mediaset(ulNum, pOppvItem, pstContents, nNagraCardType);
		return ERR_OK;
	}

	if(nNagraCardType == eNAGRA_OP_TYPE_MEDIASET) //mediaset
	{
		if (!xmgr_cas_NaIsPopupEnabled_Mediaset())
		{
			// 모든 UI들을 닫는다.
			BUS_SendMessage(NULL, MSG_GWM_QUIT_UI, 0, 0, 0, 0);

			BUS_MGR_Create("xproc_cas_NaPpvPopupOppv_Mediaset",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupOppv_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nRemainedCredit,
							pOppvItem->nameLength,
							(HINT32)pOppvItem->pName);

			if ((ulNum-1) > 0 && pOppvItem->pNext)
			{
				xmgr_cas_NaAddOppvReservation_Mediaset(ulNum-1, pOppvItem->pNext, pstContents, nNagraCardType);
			}
		}
		else
		{
			xmgr_cas_NaAddOppvReservation_Mediaset(ulNum, pOppvItem, pstContents, nNagraCardType);
		}
	}
	else
	{
		if (!xmgr_cas_NaIsPopupEnabled_Mediaset())
		{
			// 모든 UI들을 닫는다.
			BUS_SendMessage(NULL, MSG_GWM_QUIT_UI, 0, 0, 0, 0);

			BUS_MGR_Create("xproc_cas_NaPpvPopupOppv_Pangea",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupOppv_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nRemainedCredit,
							pOppvItem->nameLength,
							(HINT32)pOppvItem->pName);

			if ((ulNum-1) > 0 && pOppvItem->pNext)
			{
				xmgr_cas_NaAddOppvReservation_Mediaset(ulNum-1, pOppvItem->pNext, pstContents, nNagraCardType);
			}
		}
		else
		{
			xmgr_cas_NaAddOppvReservation_Mediaset(ulNum, pOppvItem, pstContents, nNagraCardType);
		}
	}

	HxLOG_Print("EXIT - pstContents->nRemainedCredit = %d\n", pstContents->nRemainedCredit);
	return ERR_OK;
}

STATIC void xmgr_cas_NaAddRechargeReservation_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, TUnsignedInt8 nNagraCardType)
{
	TUnsignedInt8 				nPopupType;

	HxLOG_Print("ENTER - pstContents->nNumRemainedPopup= %d\n", pstContents->nNumRemainedPopup);

	nPopupType = ePOPUP_TYPE_RECHARGE;

	xmgr_cas_NaAddReservedPopupMsgItem_Mediaset(NULL, nNagraCardType, nPopupType, pstContents);

	pstContents->nPopupCredit = pstContents->nRemainedCredit;

	HxLOG_Print("EXIT - pstContents->nNumRemainedPopup= %d\n", pstContents->nNumRemainedPopup);
}

STATIC HERROR	xmgr_cas_NaResponseRechargeAlarm_Mediaset(TCaObjectStatus objStatus, XmgrCas_NaContent_Mediaset_t* pstContents)
{
	TUnsignedInt8				nNagraCardType;
	AP_UiState_t				eUiState;
	MgrAction_State_e					state;
	HERROR						hErr;

	HxLOG_Print("ENTER - objStatus(%d)\n", objStatus);

	if (objStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Print("objStatus error!!!\n");
		return ERR_FAIL;
	}
	HxLOG_Print("Settings_GetWindowDepth() = %d\n", Settings_GetWindowDepth());

	// Credit을 보여주는 window가 있는 경우 credit 갱신해 줌
	xmgr_cas_NaGetScCredit(&pstContents->nRemainedCredit);
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)objStatus, (HINT32)pstContents->nRemainedCredit);

	hErr = xmgr_cas_NaGetNagraOpType_Mediaset(&nNagraCardType);
	HxLOG_Print("nNagraCardType = %d\n", nNagraCardType);
	if (hErr != ERR_OK)
	{
		nNagraCardType = eNAGRA_OP_TYPE_MEDIASET;	// default 는 Mediaset으로
	}

	state = MGR_ACTION_GetState();
	if (state == eAmState_ACTIVE_STANDBY)
	{
		HxLOG_Print("Active standby mode - AddReserve\n");
		xmgr_cas_NaAddRechargeReservation_Mediaset(pstContents, nNagraCardType);
		return ERR_OK;
	}

	eUiState = AP_UiState_GetUiState(eUI_STATE_LV_TOP);
	if ((eUiState == UISTATE_MENU) ||			// Menu UI
		(eUiState == UISTATE_EPG))				// EPG UI
	{
		xmgr_cas_NaAddRechargeReservation_Mediaset(pstContents, nNagraCardType);
		return ERR_OK;
	}

	if (xmgr_cas_NaCheckPpvStepConfirm_Mediaset())
	{
		HxLOG_Print("xmgr_cas_NaPpvBannerCheckStepConfirm_Mediaset = TRUE\n");
		xmgr_cas_NaAddRechargeReservation_Mediaset(pstContents, nNagraCardType);
		return ERR_OK;
	}

	HxLOG_Print("nNagraCardType = %d\n", nNagraCardType);

	if(nNagraCardType == eNAGRA_OP_TYPE_MEDIASET) //mediaset
	{
		if (!xmgr_cas_NaIsPopupEnabled_Mediaset())
		{
			// 모든 UI들을 닫는다.
			BUS_SendMessage(NULL, MSG_GWM_QUIT_UI, 0, 0, 0, 0);

			BUS_MGR_Create("xproc_cas_NaPpvPopupRecharge_Mediaset",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupRecharge_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nRemainedCredit,
							0,
							NULL);
		}
		else
		{
			xmgr_cas_NaAddRechargeReservation_Mediaset(pstContents, nNagraCardType);
		}
	}
	else
	{
		if (!xmgr_cas_NaIsPopupEnabled_Mediaset())
		{
			// 모든 UI들을 닫는다.
			BUS_SendMessage(NULL, MSG_GWM_QUIT_UI, 0, 0, 0, 0);

			BUS_MGR_Create("xproc_cas_NaPpvPopupRecharge_Pangea",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupRecharge_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nRemainedCredit,
							0,
							NULL);
		}
		else
		{
			xmgr_cas_NaAddRechargeReservation_Mediaset(pstContents, nNagraCardType);
		}
	}

	HxLOG_Print("EXIT - pstContents->nRemainedCredit = %d\n", pstContents->nRemainedCredit);
	return ERR_OK;
}

STATIC HERROR	xmgr_cas_NaCheckReservationPopup_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents)
{
	AP_UiState_t				eUiState;
	MgrAction_State_e					state;

	if (pstContents->nNumRemainedPopup <= 0)
	{
		HxLOG_Print("EXIT - no popup remained\n");
		return ERR_FAIL;
	}

	if (pstContents->stPopUpItems == NULL)
	{
		HxLOG_Print("EXIT - popup list NULL, pstContents->nNumRemainedPopup = %d\n", pstContents->nNumRemainedPopup);
		return ERR_FAIL;
	}

	state = MGR_ACTION_GetState();
	if (state == eAmState_ACTIVE_STANDBY)
	{
		HxLOG_Print("Active standby mode - AddReserve\n");
		return ERR_FAIL;
	}

	eUiState = AP_UiState_GetUiState(eUI_STATE_LV_TOP);
	if ((eUiState == UISTATE_MENU) ||			// Menu UI
		(eUiState == UISTATE_EPG))					// EPG UI
	{
		HxLOG_Print("Menu or EPG exists.... -> ADD reservation\n");
		return ERR_FAIL;
	}

	if (xmgr_cas_NaCheckPpvStepConfirm_Mediaset())
	{
		HxLOG_Print("xmgr_cas_NaPpvBannerCheckStepConfirm_Mediaset = TRUE\n");
		return ERR_FAIL;
	}

	if (xmgr_cas_NaIsPopupEnabled_Mediaset())
	{
		HxLOG_Print("Return - Yet popup window exists, pstContents->nNumRemainedPopup = %d\n", pstContents->nNumRemainedPopup);
		return ERR_OK;
	}

	if ((BUS_MGR_Get(xproc_cas_NaPpvPopupIppvResult_Mediaset) != NULL) || (BUS_MGR_Get(xmgr_cas_NaPpvPopupIppvResult_Pangea) != NULL))
	{
		return ERR_OK;
	}

	// 모든 UI들을 닫는다.
	BUS_SendMessage(NULL, MSG_GWM_QUIT_UI, 0, 0, 0, 0);

	HxLOG_Print("stPopUpItems->nNagraOPType = %d, stPopUpItems->nType = %d", pstContents->stPopUpItems->nNagraOPType, pstContents->stPopUpItems->nType);
	if (pstContents->stPopUpItems->nNagraOPType == eNAGRA_OP_TYPE_PANGEA)
	{
		if (pstContents->stPopUpItems->nType == ePOPUP_TYPE_OPPV)
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupOppv_Pangea",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupOppv_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nPopupCredit,
							SvcCas_UtilStrLen(pstContents->stPopUpItems->pName),
							(HINT32)pstContents->stPopUpItems->pName);
		}
		else	// ePOPUP_TYPE_RECHARGE
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupRecharge_Pangea",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupRecharge_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nPopupCredit,
							0,
							0);
		}
	}
	else // eNAGRA_OP_TYPE_MEDIASET
	{
		if (pstContents->stPopUpItems->nType == ePOPUP_TYPE_OPPV)
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupOppv_Mediaset",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupOppv_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nPopupCredit,
							SvcCas_UtilStrLen(pstContents->stPopUpItems->pName),
							(HINT32)pstContents->stPopUpItems->pName);
		}
		else	// ePOPUP_TYPE_RECHARGE
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupRecharge_Mediaset",
							APP_CAS_HIGH_PRIORITY,
							xproc_cas_NaPpvPopupRecharge_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							pstContents->nPopupCredit,
							0,
							0);
		}
	}

	xmgr_cas_NaRemoveFirstReservedPopup_Mediaset(pstContents);
	HxLOG_Print("EXIT - pstContents->nNumRemainedPopup = %d, pstContents->nPopupCredit = %d\n", pstContents->nNumRemainedPopup, pstContents->nPopupCredit);
}

STATIC HERROR	xmgr_cas_NaUpdateCasMessageFromAlarmData_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, void* pAlarm)
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

			HxLOG_Print("na_alarm_msg_type = %d\n", na_alarm_msg_type);
#if 0	// Mediaset or Pangea check!!! local_caPpv_CheckGivenProviderIDInPMT를 변형하여 caSystem ID까지 가져와야 할듯
			if (na_msg_type == eCAS_NA_CA_SMARTCARD_OK)
			{
				HUINT8		nNagraCardType;
				HUINT32		errStatus;
				HUINT32		ret;
				TCaSystemId caSystemId;
				BOOL		bStatus;

				ret = MW_CA_NA_GetSCCasID(&errStatus, &caSystemId);
				PRINT_FUNC(("WM_CA_SC_OK : ret = %d, caSystemId = 0x%x\n", ret, caSystemId));
				if (ret == ERR_OK && errStatus == CA_REQUEST_NO_ERROR)
				{
					HUINT8	numOfCaSysId;
					ret = MW_CA_NA_CheckGivenCaSysIdInPMT(&bStatus, &numOfCaSysId, caSystemId);
					PRINT_FUNC(("bStatus = %d, numOfCaSysId = %d\n", bStatus, numOfCaSysId));
					if (ret == ERR_OK && numOfCaSysId != 0 && bStatus == FALSE)
					{
						nMsgType = WM_CA_SC_INVALID;
					}
				}
			}
#endif
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
				HxLOG_Print("na_access_msg_type = %d\n", na_access_msg_type);
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
	HxLOG_Print("bDisplayCardErr = 0x%x, na_alarm_msg_type = 0x%x\n", bDisplayCardErr, na_alarm_msg_type);

	if (pstContents->eNagraOpType == eNAGRA_OP_TYPE_PANGEA)
	{
		if (bDisplayCardErr)
		{
			BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Pangea",
							APP_CAS_PRIORITY,
							xproc_cas_NaBannerSmartCardError_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							na_alarm_msg_type,
							0,
							0);

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE, (Handle_t)NULL, eDxCAS_GROUPID_NA, eNAGRA_OP_TYPE_PANGEA, 0); // tscheck가 끝나기 전에 mediaset banner window 가 먼저 떠 있는 경우 바꿔치기
		}
		else
		{
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
			{
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Pangea);
			}
		}
	}
	else
	{
		if (bDisplayCardErr)
		{
			BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
							APP_CAS_PRIORITY,
							xproc_cas_NaBannerSmartCardError_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							na_alarm_msg_type,
							0,
							0);
		}
		else
		{
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
			{
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);
			}
		}
	}

	if (na_alarm_msg_type == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->nLastPurchasedPPTEventId = 0;
		pstContents->bSignalOff = FALSE;
	}
	if (na_access_msg_type == eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT || na_access_msg_type == eCAS_NA_CA_ACCESS_DENIED_BUT_PPT)
	{
		// 돈이 소진된 경우 다시 충전되더라도 자동구매되어서는 안된다.
		pstContents->nLastPurchasedPPTEventId = 0;
		pstContents->bSignalOff = FALSE;

		// PPT 보다 이 access alarm이 늦게 오기 때문에, nLastPurchasedPPTEventId로 인해 PPT를 띄우지 못했을 수 있다.
		// 따라서 PPT창을 다시 띄우도록 아래를 호출한다.
		xmgr_cas_NaRequestTsCheck_Mediaset();
	}
	return ERR_OK;
}


STATIC void xmgr_cas_NaMediasetCardCreditCallback(TUnsignedInt8 ucSlotId, TUnsignedInt8 eStatus, TCreditAmount nCredit)
{
	HxLOG_Print("eStatus = 0x%x, nCredit = %d \n", eStatus, nCredit);
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT, (Handle_t)NULL, eDxCAS_GROUPID_NA, (HINT32)eStatus, (HINT32)nCredit);
}

STATIC void xmgr_cas_NaMediasetTsCheckNotifyCallback(Handle_t svcHandle, TS_PERIOD_TYPE tsType, const TBoolean isSCMatchedWithPMT)
{
	XmgrCas_NaContent_Mediaset_t* pstContents = xmgr_cas_NaGetContents_Mediaset();

	pstContents->bWaitingNewTsCheckStart = TRUE;

	HxLOG_Print("svcHandle = 0x%x, tsType = %d, isSCMatchedWithPMT = %d\n", svcHandle, tsType, isSCMatchedWithPMT);
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_TSCHECK_RESULT, (Handle_t)svcHandle, eDxCAS_GROUPID_NA, (HINT32)tsType, (HINT32)isSCMatchedWithPMT);
}

STATIC void xmgr_cas_NaMediasetEventInfoNotifyCallback(TEventStatus xStatus, const TCaEvent *pEvent, const PPV_EVENT_INFO *pEventInfo)
{
	AP_UiState_t	 eUiState;

	HxLOG_Print("xStatus = 0x%x\n", xStatus);

	s_bEventnfoRequested = FALSE;

	HxSTD_memset(&s_PurchasableEventInformation, 0, sizeof(XmgrCas_NaPurchasableEvent_t));
	if (pEventInfo)
	{
		s_PurchasableEventInformation.nPurchaseType = pEventInfo->xPurchaseType;
		s_PurchasableEventInformation.pEvent = pEvent;

		HxSTD_memcpy(&s_PurchasableEventInformation.stPPVEventInfo, pEventInfo, sizeof(PPV_EVENT_INFO));
		HxLOG_Print("s_PurchasableEventInformation.stPPVEventInfo.pCaProduct = 0x%x\n", s_PurchasableEventInformation.stPPVEventInfo.pCaProduct);
	}

	if ((xStatus == CA_EVENT_SUCCESS) && pEventInfo != NULL)
	{
		eUiState = AP_UiState_GetUiState(eUI_STATE_LV_TOP);
		if ((eUiState == UISTATE_MENU) ||			// Menu UI
			(eUiState == UISTATE_EPG))				// EPG UI
		{
			HxLOG_Print("not display - menu or portal or epg\n");
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거
			return;		// Menu 상태에서는 IPPV를 보여주지 않는다.
		}

		if ((BUS_MGR_Get(AP_Option_Proc) != NULL) ||
			(BUS_MGR_Get(AP_ChList_Proc) != NULL)	// || MyEPG  || Media(Video, Music, Photo) || CAS 메뉴
		)
		{
			HxLOG_Print("not display - chlist or option\n");
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거
			return;		// chlist or option 상태에서는 IPPV를 보여주지 않는다.
		}


		if (xmgr_cas_NaIsPopupEnabled_Mediaset())
		{
			HxLOG_Print("not display - recharge or oppv\n");
			xmgr_cas_NaDisposeCaRequest(pEventInfo->pCaRequest); 	// 이번 pCaRequest 제거
			return;		// recharge/oppv popup 상태에서는 IPPV를 보여주지 않는다.
		}
	}

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT, (Handle_t)xStatus, eDxCAS_GROUPID_NA, (HINT32)pEvent, (HINT32)&s_PurchasableEventInformation.stPPVEventInfo);
}

STATIC void xmgr_cas_NaMediasetPurchaseHistoryNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList)
{
	HxLOG_Print("xStatus = 0x%x, xNumberOfItems = %d\n", xStatus, xNumberOfItems);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_HISTORY, (Handle_t)xStatus, eDxCAS_GROUPID_NA, (HINT32)xNumberOfItems, (HINT32)pxPurchaseList);
}

STATIC void xmgr_cas_NaMediasetRechargeListNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 xNumberOfItems, const RECHARGE_LIST_ITEM* pxRechargeList)
{
	HxLOG_Print("xStatus = 0x%x\n", xStatus);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_LIST, (Handle_t)xStatus, eDxCAS_GROUPID_NA, (HINT32)xNumberOfItems, (HINT32)pxRechargeList);
}

STATIC void xmgr_cas_NaMediasetOppvNotifyCallback(TCaObjectStatus objStatus, const OPPV_PURCHASE_INFO *pxOPPVPurchaseInfo)
{
	HxLOG_Print("objStatus = 0x%x\n", objStatus);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST, (Handle_t)objStatus, eDxCAS_GROUPID_NA, (HINT32)pxOPPVPurchaseInfo->xNbItems, (HINT32)pxOPPVPurchaseInfo->pOPPVItem);
}

STATIC void xmgr_cas_NaMediasetRechargeAlarmNotifyCallback(TCaObjectStatus objStatus, TUnsignedInt32 wParam, TUnsignedInt32 lParam)
{
	HxLOG_Print("objStatus = 0x%x\n", objStatus);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM, (Handle_t)objStatus, eDxCAS_GROUPID_NA, (HINT32)wParam, (HINT32)lParam);
}

STATIC void xmgr_cas_NaMediasetPackageListNotifyCallback(TUnsignedInt32 status, const TCaRequest* pRequest, TUnsignedInt32 nNumber, const void *aPackageList)
{
	HxLOG_Print("status = 0x%x, pRequest = 0x%x\n", status, pRequest);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PACKAGE_LIST, (Handle_t)status, eDxCAS_GROUPID_NA, (HINT32)nNumber, (HINT32)aPackageList);
}

STATIC void xmgr_cas_NaMediasetPurchaseEventNotifyCallback(TUnsignedInt32 xStatus, TUnsignedInt32 nPurchaseType)
{
	HxLOG_Print("xStatus = %d, nPurchaseType = %d\n", xStatus, nPurchaseType);

	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT, (Handle_t)xStatus, eDxCAS_GROUPID_NA, (HINT32)nPurchaseType, 0);
}

STATIC void xmgr_cas_NaMediasetPurchasePackageNotifyCallback(TCaProcessingStatus reqStatus)
{
	HxLOG_Print("reqStatus = %d\n", reqStatus);

	BUS_PostMessage(xproc_cas_NaManagement_Mediaset, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_PACKAGE_RESULT, (Handle_t)reqStatus, eDxCAS_GROUPID_NA, 0, 1);	// p3가 1이면 다시 보내기
}

STATIC HBOOL xmgr_cas_NaIsPopupEnabled_Mediaset(void)
{
	if ((BUS_MGR_Get(xproc_cas_NaPpvPopupOppv_Mediaset) == NULL) && (BUS_MGR_Get(xproc_cas_NaPpvPopupRecharge_Mediaset) == NULL)
		&& (BUS_MGR_Get(xproc_cas_NaPpvPopupOppv_Pangea) == NULL) && (BUS_MGR_Get(xproc_cas_NaPpvPopupRecharge_Pangea) == NULL)
	)
	{
		return FALSE;
	}

	return TRUE;
}

STATIC void xmgr_cas_NaDisplayPurchaseResult_Mediaset(XmgrCas_NaContent_Mediaset_t* pstContents, HUINT32 nPurchaseType)
{
	HERROR 			hErr = ERR_OK;
	TUnsignedInt8	nNagraCardType;

	hErr = xmgr_cas_NaGetNagraOpType_Mediaset(&nNagraCardType);
	if (hErr != ERR_OK)
	{
		nNagraCardType = eNAGRA_OP_TYPE_MEDIASET;	// default 는 Mediaset으로
	}

	HxLOG_Print("nPurchaseType = 0x%x\n", nPurchaseType);

	if (nPurchaseType == CA_EVENT_SPECIAL_EVENT)
	{
		if (nNagraCardType == eNAGRA_OP_TYPE_MEDIASET)
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupIppvResult_Mediaset",
				APP_CAS_HIGH_PRIORITY,
				xproc_cas_NaPpvPopupIppvResult_Mediaset,
				SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
				0,
				0,
				0);
		}
		else
		{
			BUS_MGR_Create("xmgr_cas_NaPpvPopupIppvResult_Pangea",
				APP_CAS_HIGH_PRIORITY,
				xmgr_cas_NaPpvPopupIppvResult_Pangea,
				SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
				0,
				0,
				0);
		}
	}
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_MEDIASET_PUBLIC_FUNCTION______________________________________________________


BUS_Result_t 	xproc_cas_NaManagement_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr;
	HxDATETIME_Date_t				stDate_t;
	HULONG					ulUnixTime = 0;
	XmgrCas_NaContent_Mediaset_t* pstContents = xmgr_cas_NaGetContents_Mediaset();

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			hErr = VK_CLOCK_GetTime(&ulUnixTime);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy(NULL);
				return MESSAGE_BREAK;
			}

			HxSTD_memset(pstContents, 0, sizeof(XmgrCas_NaContent_Mediaset_t));

			break;

		case eSEVT_SI_EIT :
		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENT_CHANGED :
			{
				HxLOG_Print("MESSAGE : eSEVT_SI_EIT \n");
	//			pstContents->nLastPurchasedPPTEventId = 0;

	//			xmgr_cas_NaRequestTsCheck_Mediaset();
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_TSCHECK_RESULT :

			pstContents->bWaitingNewTsCheckStart = FALSE;
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_TSCHECK_RESULT svcHandle(%d), CasGroupId(%d)TsType(%d)isPmtMatched(%d)\n", hAction, p1, p2, p3);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				if (xmgr_cas_NaResponseTsCheck_Mediaset(hAction, (TS_PERIOD_TYPE)p2, pstContents) == ERR_OK)
				{
	//				if ((BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset) == NULL) && (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea) == NULL))
					{
						HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_TSCHECK_RESULT s_bEventnfoRequested(%d)\n", s_bEventnfoRequested);
						if (s_bEventnfoRequested == FALSE)
						{
							xmgr_cas_NaRequestEventInfo();
							s_bEventnfoRequested = TRUE;
						}
					}
	//				xmgr_cas_NaMediasetRequestEventInfo((TS_PERIOD_TYPE)p2, (TBoolean)p3, pstContents);
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT :
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT TEventStatus(%d), CasGroupId(%d)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				if (pstContents->bWaitingNewTsCheckStart == FALSE)
				{
					xmgr_cas_NaMediasetResponseEventInfo((TEventStatus)hAction, (TCaEvent*)p2, (PPV_EVENT_INFO*)p3, pstContents);
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST:
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST objStatus(%d), CasGroupId(%d)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				VK_TASK_Sleep(CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH);
				hErr = xmgr_cas_NaResponseOppvCallback_Mediaset((TCaObjectStatus)hAction, (TUnsignedInt32)p2, (OPPV_PURCHASE_INFO_ITEM*)p3, pstContents);
				HxLOG_Print("xmgr_cas_NaResponseOppvCallback_Mediaset hErr = %d\n", hErr);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM:
			HxLOG_Print("MESSAGE : eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM objStatus(%d), CasGroupId(0x%x)\n", hAction, p1);
			if (p1 == eDxCAS_GROUPID_NA)
			{
				VK_TASK_Sleep(CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH);
				hErr = xmgr_cas_NaResponseRechargeAlarm_Mediaset(hAction, pstContents);
				HxLOG_Print("xmgr_cas_NaResponseRechargeAlarm_Mediaset hErr = %d\n", hErr);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT\n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				pstContents->nRemainedCredit = (TCreditAmount)p3;
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED...\n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaCheckPauseResume_Mediaset(FALSE);

				hErr = xmgr_cas_NaCheckReservationPopup_Mediaset(pstContents);
				// remained popup이 없으면 ERR_FAIL을 return...
				if (hErr == ERR_FAIL)
				{
					xmgr_cas_NaRequestTsCheck_Mediaset();		// IPPV 체크
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT...\n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				VK_TASK_Sleep(CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH);
				xmgr_cas_NaGetScCredit(&pstContents->nRemainedCredit);
				pstContents->nPopupCredit = pstContents->nRemainedCredit;	// IPPV confirm에서 예약된 Recharge가 있는 경우 credit을 갱신

				if ((PURCHASE_RESULT_TYPE)hAction == CA_PROCESSING_NO_ERROR)
				{
					VK_TASK_Sleep(CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH);
					xmgr_cas_NaDisplayPurchaseResult_Mediaset(pstContents, p2);
					HxLOG_Print("xmgr_cas_NaDisplayPurchaseResult_Mediaset\n");
				}
				else
				{
					//xmgr_cas_NaRequestEventInfo();
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_PACKAGE_RESULT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT...\n");
			if (p1 == eDxCAS_GROUPID_NA &&  p3 == 1)	// p3가 0이면 받지 말자
			{
				if ((PURCHASE_RESULT_TYPE)hAction == eNA_PPV_PURCHASE_RESULT_TYPE_SUCCESS)
				{
					VK_TASK_Sleep(CAS_NA_PPV_POPUP_WAIT_CREDIT_REFRESH);

					xmgr_cas_NaGetScCredit(&pstContents->nRemainedCredit);
					pstContents->nPopupCredit = pstContents->nRemainedCredit;	// Menu에서 예약된 Recharge가 있는 경우 credit을 갱신
				}

				BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_PACKAGE_RESULT, (Handle_t)hAction, eDxCAS_GROUPID_NA, 0, 0);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PPT_CLEAR:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PPT_CLEAR\n");
			if (p1 == eDxCAS_GROUPID_NA)
			{
				pstContents->nLastPurchasedPPTEventId = 0;
				pstContents->bSignalOff = FALSE;
				return MESSAGE_BREAK;
			}
			break;

		case eMEVT_CAS_CHECK:
			break;

		case eMEVT_CAS_OK :
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset) || BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
			{
				HxLOG_Print("force scramble banner to close\n");

				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
				{
					BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);
				}

				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
				{
					BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Pangea);
				}
			}
			break;

		case eMEVT_CAS_FAIL:
			if (p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaUpdateCasMessageFromAlarmData_Mediaset(pstContents, (void*)p2);
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print("eSEVT_CH_NO_SIGNAL...\n");

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
			xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
			pstContents->stPPVEventType.pCaRequest = NULL;
			pstContents->bSignalOff = TRUE;

			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED...\n");

			//xmgr_cas_NaRequestTsCheck_Mediaset();
			MW_CA_NA_NotifySvcChanged(p1, hAction);

			pstContents->eNagraOpType = eNAGRA_OP_TYPE_UNDEFINED;

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
			xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
			pstContents->stPPVEventType.pCaRequest = NULL;

			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset) || BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
			{
				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
				{
					BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);
				}

				if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
				{
					BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Pangea);
				}
			}

			pstContents->nLastPurchasedPPTEventId = 0;
			pstContents->bSignalOff = FALSE;
			s_bEventnfoRequested = FALSE;

			break;

		case eMEVT_BUS_READY_SHUTDOWN :
			HxLOG_Print(" <<< eMEVT_BUS_READY_SHUTDOWN >>>\n ");

			if (pstContents->stPPVEventType.pCaRequest)
			{
				HxSTD_memset(&pstContents->stPPVEventType, 0, sizeof(PPV_EVENT_INFO));
			}
			return MESSAGE_PASS;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
		case eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED:
			{
				if (BUS_MGR_Get(AP_WtvPinCode_Proc_0))
				{
					if (BUS_MGR_Get(xmgr_cas_NaPpvBanner_Mediaset))
					{
						BUS_MGR_Destroy(xmgr_cas_NaPpvBanner_Mediaset);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
					if (BUS_MGR_Get(xproc_cas_NaPpvBanner_Pangea))
					{
						BUS_MGR_Destroy(xproc_cas_NaPpvBanner_Pangea);
						xmgr_cas_NaDisposeCaRequest(pstContents->stPPVEventType.pCaRequest);
						pstContents->stPPVEventType.pCaRequest = NULL;
					}
				}
				return MESSAGE_PASS;
			}
			return MESSAGE_PASS;

		case eMEVT_BUS_TIMER :
			break;

		case MSG_GWM_QUIT_UI :
			return MESSAGE_PASS;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR			xmgr_cas_NaRegisterCardCreditCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterScCreditCallback(xmgr_cas_NaMediasetCardCreditCallback);
}

HERROR			xmgr_cas_NaRegisterTsCheckNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterTsCheckNotifyCallback(xmgr_cas_NaMediasetTsCheckNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterEventInfoNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterEventInfoCallback(xmgr_cas_NaMediasetEventInfoNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterPurchaseHistoryNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterPurchaseHistoryCallback(xmgr_cas_NaMediasetPurchaseHistoryNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterRechargeListNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterRechargeListCallback(xmgr_cas_NaMediasetRechargeListNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterOppvPurchaseListNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterOppvPurchaseListCallback(xmgr_cas_NaMediasetOppvNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterRechargeAlarmNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterRechargeAlarmCallback(xmgr_cas_NaMediasetRechargeAlarmNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterPackageListNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterPackageListCallback(xmgr_cas_NaMediasetPackageListNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterRequestPurchaseEventNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterRequestPurchaseEventCallback(xmgr_cas_NaMediasetPurchaseEventNotifyCallback);
}

HERROR			xmgr_cas_NaRegisterRequestPurchasePackageNotifyCallback_Mediaset(void)
{
	return MW_CA_NA_RegisterRequestPurchasePackageCallback(xmgr_cas_NaMediasetPurchasePackageNotifyCallback);
}

HERROR			xmgr_cas_NaRequestEventPurchase_Mediaset(void)
{
	XmgrCas_NaContent_Mediaset_t* pstContents = xmgr_cas_NaGetContents_Mediaset();

	if (s_PurchasableEventInformation.nPurchaseType == CA_EVENT_PPT)
	{
		pstContents->nLastPurchasedPPTEventId = s_PurchasableEventInformation.stPPVEventInfo.xEventID;
		pstContents->bSignalOff = FALSE;
		HxLOG_Print("pstContents->nLastPurchasedPPTEventId = %d\n", pstContents->nLastPurchasedPPTEventId);
	}

	return MW_CA_NA_RequestEventPurchase(s_PurchasableEventInformation.stPPVEventInfo.xPurchaseType, s_PurchasableEventInformation.stPPVEventInfo.pCaRequest, s_PurchasableEventInformation.pEvent, s_PurchasableEventInformation.stPPVEventInfo.pCaProduct);
}

HERROR			xmgr_cas_NaRequestPackagePurchase_Mediaset(HUINT32* pProduct)
{
	return MW_CA_NA_RequestPackagePurchase(pProduct);
}

HERROR			xmgr_cas_NaRequestPpvGetProductDescription_Mediaset(HUINT32* pStrLen, HUINT8** ppBuffer)
{
	if (s_PurchasableEventInformation.stPPVEventInfo.pCaProduct == NULL || pStrLen == NULL || ppBuffer == NULL)
	{
		return ERR_FAIL;
	}

	return MW_CA_NA_GetProductGetDescription(s_PurchasableEventInformation.stPPVEventInfo.pCaProduct, (TSize*)pStrLen, ppBuffer);
}

HERROR			xmgr_cas_NaRequestPpvGetProductDisclaimer_Mediaset(HUINT32* pStrLen, HUINT8** ppBuffer)
{
	if (s_PurchasableEventInformation.stPPVEventInfo.pCaProduct == NULL || pStrLen == NULL || ppBuffer == NULL)
	{
		return ERR_FAIL;
	}

	return MW_CA_NA_GetProductGetDisclaimer(s_PurchasableEventInformation.stPPVEventInfo.pCaProduct, (TSize*)pStrLen, ppBuffer);

}

HBOOL 			xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(HUINT8 eNaOpType)
{
	HBOOL					bCardMatched = TRUE;
	HUINT8					nNagraCardType;

	if (xmgr_cas_NaGetNagraOpType_Mediaset(&nNagraCardType) == ERR_OK)
	{
		if (nNagraCardType != eNaOpType)
		{
			bCardMatched = FALSE;
		}
		else
		{
			bCardMatched = TRUE;
		}
	}
	else
	{
		HxLOG_Print("xmgr_cas_NaGetNagraOpType_Mediaset ERROR\n");
		bCardMatched = TRUE;
	}

	return bCardMatched;
}

HBOOL xmgr_cas_NaIsPopupReserved_Mediaset(void)
{
	HERROR 					hErr = ERR_OK;
	XmgrCas_NaContent_Mediaset_t* pstContents = xmgr_cas_NaGetContents_Mediaset();

	hErr = xmgr_cas_NaCheckReservationPopup_Mediaset(pstContents);
	HxLOG_Print("hErr = %d\n", hErr);
	// remained popup이 없으면 ERR_FAIL을 return...
	if (hErr == ERR_FAIL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

HERROR xmgr_cas_NaCheckPauseResume_Mediaset(HBOOL bPause)
{
	HERROR 					hErr = ERR_OK;
	XmgrCas_NaContent_Mediaset_t* pstContents = xmgr_cas_NaGetContents_Mediaset();

#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA_XVID)
	HUINT32 			ulActionId = NUM_MAX_ACTION;
	MgrAction_Type_e 	eActionType = eActionType_NONE;

	ulActionId = MGR_ACTION_GetMainActionId();
	eActionType = MGR_ACTION_GetType(ulActionId);

	if((eActionType == eViewType_PB) || (eActionType == eViewType_MOVIE_PB))
	{
		return ERR_FAIL;
	}
#endif

	HxLOG_Print("bPause = %d, pstContents->bPause = %d\n", bPause, pstContents->bPaused);

	if (pstContents->bPaused != bPause)
	{
		if (bPause)
		{
			hErr = SVC_CAS_Pause(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()));

			pstContents->nLastPurchasedPPTEventId = 0;	// Mediaset PPV 메뉴에서 빠져 나갈 때, 자동 구매 안되도록 함
			pstContents->bSignalOff = FALSE;

			HxLOG_Print("SVC_CAS_Pause - hErr = %d\n", hErr);
		}
		else
		{
			hErr = SVC_CAS_Resume(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()));
			HxLOG_Print("SVC_CAS_Resume - hErr = %d\n", hErr);
		}

		pstContents->bPaused = bPause;
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

