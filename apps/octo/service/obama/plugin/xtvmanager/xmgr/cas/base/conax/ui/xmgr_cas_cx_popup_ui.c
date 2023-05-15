/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <svc_cas.h>
#include <conax_ctrl.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>
#include <mgr_live.h>
#include <mgr_schedule.h>

//#include <xmgr_cas_util_dslib.h>

#include "../local_include/_xmgr_cas_cx.h"
#include "../local_include/_xmgr_cas_cx_util.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#define	CX_USER_TEXT_HOUR				3600		// 1 hour
#define	CX_USER_TEXT_MIN				60
#define	CX_USET_TEXT_TIME_LIMIT			60
#define	CX_USET_TEXT_WAITING_TDT_TIME	1000		// 1 Sec
#define	CX_USET_TEXT_WAITING_REPEAT_TIME	9000		// 5 Sec, User Text�� Repeat �Ǿ� ��µǴ� ��찡 �ִµ� �� Repeat�� ���� Interval�� �ּ� 10�� �̴�. ���� �뷫 5�� Ÿ�̸Ӹ� �ɰ�, ���� ��µǾ��� Ÿ�̹��� ����ϵ��� �Ѵ�.
#define CX_USER_TEXT_MAIL_INDICATOR_TIME	1000		// Mail Icon üũ�� ���ؼ� Current Time�� Valid���� ���� ��, 1�� �ֱ�� üũ�Ѵ�.

#define	CX_UI_FP_STARTTIMER_ID					1
#define	CX_UI_FP_GETTINGSITDTTIMER_ID			2
#define	CX_UI_UT_STARTTIMER_ID					3
#define	CX_UI_UT_DURATIONTIMER_ID				4
#define	CX_UI_UT_INTERVALTIMER_ID				5
#define	CX_UI_UT_GETTINGSITDTTIMER_ID			6
#define	CX_UI_UT_REPEATTIMER_ID					7
#define	CX_UI_FP_DURATIONTIMER_ID				8
#define CX_UI_UT_MAIL_INDICATOR_ID				9

#define CX_UI_UT_CHARCODE_ISO6937		0x00
#define CX_UI_UT_CHARCODE_TABLE_05		0x05
#define CX_UI_UT_CHARCODE_TABLE_0B		0x0B

#define CX_UI_UT_FIRSTCHAR_LOC_ISO6937	1

#define CX_UI_UT_MSGTYPE_GAP				0x100
#define CX_UI_UT_UIMSGINFO_DATA_POSITION	8
#define CX_UI_UT_MAIL_MESSAGE_VALUE			0	// Duration�� User Text�� ��µǾ�� �ϴ� �ð��� ��Ÿ������, 0�� ��쿡�� �ش� Message�� Mail Message���� ��Ÿ��.
#define CX_UI_UT_OFFSET_SEC_TIME			1	// 1�� Ÿ�̸Ӱ� random case�� delay�� �߻��Ͽ�, 1������ Ʋ������ ��츦 ���� "1�� ����"�� ����Ѵ�.

#define CX_UI_SECONDS_OF_DAY 			86400
#endif


/*******************************************************************/
/********************      Type define		    ********************/
/*******************************************************************/
typedef struct CxPopUpContext {
	HUINT32						ulSlot;
	OxMgrCasUiEvtSession_t		stSessionInfo;
	XmgrCas_CxUiSessionStatus_e	eSessionStatus;
	CxUiMsgInfo_t				stOldMsgInfo;
#if defined(CONFIG_SUPPORT_FCC)
	CxUiMsgInfo_t				astFccMsgInfo[NUM_VIEW_ACTION];
#endif
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	CxUiMsgInfo_t				stUserTextInfo;
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	CxUiMsgInfo_t				stFingerPrintInfo;
#endif
	HBOOL						bStandby;
	HBOOL						bMailIndicator;
} XmgrCas_CxUiPopUpContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_CxUiPopUpContext_t	s_stCxUiPopUpContext;

/*******************************************************************/
/*****************       Static Func Prototype  ********************/
/*******************************************************************/
STATIC BUS_Result_t xmgr_cas_CxUiPopUpMsgEvtCasAlarm(XmgrCas_CxUiPopUpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR	xmgr_cas_CxUiPopUpProcessUserText(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t*	pstMsgInfo, HUINT32 unTimerId);
STATIC HERROR xmgr_cas_CxPopupCheckReadAllMailMessage(HBOOL *pbShow);

STATIC XmgrCas_CxUiPopUpContext_t *xmgr_cas_CxUiPopUpGetContext(void)
{
	return &s_stCxUiPopUpContext;
}

STATIC void xmgr_cas_CxUiPopUpInitContext(XmgrCas_CxUiPopUpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/**
	  *	p1 : slot id
	  * p2 : sessionTYPE
	  *	p3 : data verison
	**/
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32 i = 0;
	for(i=0; i<NUM_VIEW_ACTION; i++)
	{
		pstContext->astFccMsgInfo[i].eUiMsgType = eCxUi_GroupAll_None;
	}
#endif

	pstContext->ulSlot = (HUINT32)p1;
	pstContext->stSessionInfo.eSessionType		= eDxCAS_SESSIONTYPE_BANNER;
	pstContext->stSessionInfo.hSession			= HANDLE_NULL;
	pstContext->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_CxUiPopUp;
	pstContext->eSessionStatus					= eCX_UI_SESSION_STATUS_Stopped;
	HxSTD_MemSet(&pstContext->stOldMsgInfo, 0x00, sizeof(CxUiMsgInfo_t));
	pstContext->stOldMsgInfo.eUiMsgType			= eCxUi_GroupAll_None;
	pstContext->bStandby						= FALSE;
	pstContext->bMailIndicator					= FALSE;

	return;
}

STATIC HERROR xmgr_cas_CxUiPopUpSetPinMmiDisplayed(HBOOL bDisplayed)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	CX_SetCxDisplayedInParam_t		stInParam;

	stInParam.bDisplayed = bDisplayed;
	stParam.pvIn = (void *)&stInParam;

	stParam.ulControlType = eCACTRL_CX_SetPinMmiDisplayed;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetPinMmiDisplayed, &stParam);

	return	hErr;
}

STATIC HCHAR *xmgr_cas_CxUiPopUpEncodeString(HCHAR *pszString)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(pszString) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,pszString,Len);

    return buf;
}

STATIC HERROR xmgr_cas_CxUiPopUpConvSiStringToUtf8(HUINT32 ulSrcStrLen, HUINT8 *szSrcStr, HUINT8 **pszDstStr)
{
	SiTxtEncodedType_t 	eEncType;

	if(szSrcStr[0] == CX_UI_UT_CHARCODE_ISO6937)
	{
		// ù byte�� 0�ΰ�� chartacter code table�� 00�� ��Ÿ�� ���� �̿� ���� ó�� ����
		MWC_UTIL_GetSiStringEncodedType(&eEncType);
		MWC_UTIL_SetSiStringEncodedType(eSITEXT_ENCODED_300468);
		MWC_UTIL_ConvSiStringToUtf8(NULL, CX_MAX_USER_TEXT_LEN , &szSrcStr[CX_UI_UT_FIRSTCHAR_LOC_ISO6937], pszDstStr);
		MWC_UTIL_SetSiStringEncodedType(eEncType);
	}
	else if(szSrcStr[0] == CX_UI_UT_CHARCODE_TABLE_0B)
	{
		szSrcStr[0] = CX_UI_UT_CHARCODE_TABLE_05;
		MWC_UTIL_ConvSiStringToUtf8(NULL, CX_MAX_USER_TEXT_LEN , szSrcStr, pszDstStr);
	}
	else
	{
		MWC_UTIL_ConvSiStringToUtf8(NULL, CX_MAX_USER_TEXT_LEN , szSrcStr, pszDstStr);
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiPopUpCompareUiEvent(CxUiMsgInfo_t *pstOldMsg, CxUiMsgInfo_t *pstNewMsg, HBOOL *pbIsSameMsg)
{
	HINT8	*pcOldMsg = (HINT8 *)pstOldMsg;
	HINT8	*pcNewMsg = (HINT8 *)pstNewMsg;

	if(pstOldMsg->eUiMsgType == pstNewMsg->eUiMsgType)
	{
		// UI Msg Type�� ������� ������ ���� ���ؼ� ������ �ִ��� Ȯ���Ͽ� ������ msg ���� �����Ѵ�.
		// ù 4byte�� eUiMsgType�� ��Ÿ����, ���� 4byte�� chipset pairing error code�� ��Ÿ����.
		// chipset pairing error code�� No Access�� ����ؾ� �� ��츸 ���ǰ�, No Access�� ������ UI Msg�� ���޹޾Ƶ� ������� ���� Web���� �����ϹǷ� memcmp�� ���� �ʴ´�.
		// cpm_nErrCode�� �����ϴ� ������ �ٸ� UI���� ��� UI data�� ���������� cpm_nErrCode�� �޶� ������ϴ� ��찡 �ֱ� �����̴�.
		if (HxSTD_memcmp(&pcOldMsg[CX_UI_UT_UIMSGINFO_DATA_POSITION], &pcNewMsg[CX_UI_UT_UIMSGINFO_DATA_POSITION], sizeof(CxUiMsgInfo_t) - CX_UI_UT_UIMSGINFO_DATA_POSITION) == 0)
		{
			HxLOG_Info("same UI...\n");
			*pbIsSameMsg = TRUE;
		}
		else
		{
			HxLOG_Info("different UI...\n");
			*pbIsSameMsg = FALSE;
		}
	}
	else
	{
		// Type �� �ٸ����� �߰� �� ���� ������ ���� �ٸ� Msg�� ��Ÿ��.
		*pbIsSameMsg = FALSE;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiPopUpPostMessageSessionEventToOM(XmgrCas_CxUiPopUpContext_t *pstContext, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	HERROR								hError = ERR_OK;
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));

	pstEvent->eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	HxLOG_Info("Close Session: hSession[0x%08x]...\n", pstContext->stSessionInfo.hSession);
	pstEvent->hSession = pstContext->stSessionInfo.hSession;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_CX, ulSlotNumber, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		hError = ERR_FAIL;
	}

	return hError;
}

STATIC BUS_Result_t xmgr_cas_CxUiPopUpOpenSession(XmgrCas_CxUiPopUpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t		*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->eSessionStatus = eCX_UI_SESSION_STATUS_Started;
	pstContext->stSessionInfo.hSession = pstSession->hSession;
	HxLOG_Info("Web->Obama Open Session: hSession[0x%08x]...\n", pstSession->hSession);

	xmgr_cas_CxUiPopUpPostMessageSessionEventToOM(pstContext, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if(pstContext->stOldMsgInfo.eUiMsgType != eCxUi_GroupAll_None)
	{
		HxLOG_Info("UI data was received before session is open.\n");
		xmgr_cas_CxUiPopUpMsgEvtCasAlarm(pstContext, (HINT32)NULL, (Handle_t)NULL, (HINT32)NULL, (HINT32)&pstContext->stOldMsgInfo, (HINT32)NULL);
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CxUiPopUpReqCloseSession(XmgrCas_CxUiPopUpContext_t *pstContext, Handle_t hAction)
{
	pstContext->eSessionStatus = eCX_UI_SESSION_STATUS_Stopped;

	xmgr_cas_CxUiPopUpPostMessageSessionEventToOM(pstContext, CX_SC_SLOT_0, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_cas_CxUiPopUpSetTokenPpvString(HCHAR *pucTokenPpvStr, CxUiMsgInfo_t* pstMsgInfo, CxStatusList_t *pstStatusList)
{
	HUINT32 ulLength = 0;
	HUINT32 ulLengthSum = 0;

	// 1st line: '<program_label>'
	ulLength = HxSTD_sprintf(pucTokenPpvStr, "{\"str\":\"%s\"}", pstMsgInfo->td_szLabel);
	ulLengthSum = ulLengthSum+ulLength;

	// 2nd line: '<event_cost> token(s) per minute' or '<event_cost> token(s)
	if (pstMsgInfo->td_ucTokenMode == CX_TOKENS_MODE_EVENT)
	{
		if(pstMsgInfo->td_ulEventCost == 1)
		{
			ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"str\":\"%d \",\"id\":\"LOC_TOKEN_ID\"}", pstMsgInfo->td_ulEventCost);
		}
		else
		{
			ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"str\":\"%d \",\"id\":\"LOC_TOKENS_ID\"}", pstMsgInfo->td_ulEventCost);
		}
		ulLengthSum = ulLengthSum+ulLength;
	}
	else	// (pstMsgInfo->td_ucTokenMode = CX_TOKENS_MODE_TIME)
	{
		if(pstMsgInfo->td_ulEventCost == 1)
		{
			// TODO: [kkk] STR ID & LOC ID ��û �� ����
			ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"str\":\"%d \",\"id\":\"LOC_TOKEN_PER_MIN_ID\"}", pstMsgInfo->td_ulEventCost);
		}
		else
		{
			// TODO: [kkk] STR ID & LOC ID ��û �� ����
			ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"str\":\"%d \",\"id\":\"LOC_TOKEN_PER_MIN_ID\"}", pstMsgInfo->td_ulEventCost);
		}
		ulLengthSum = ulLengthSum+ulLength;
	}

	// 3rd line: 'Purse balance: <purse_label> <purse_balance> tokens
	if (pstStatusList->stStatus.stPurse.ulBalance == 1)
	{
		ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"id\":\"LOC_PURSE_BALANCE_TOKEN_ID\",\"put\":[\"%s - %d\"]}"
				,pstStatusList->stStatus.stPurse.szLabel, pstStatusList->stStatus.stPurse.ulBalance);
	}
	else
	{
		ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"id\":\"LOC_PURSE_BALANCE_TOKENS_ID\",\"put\":[\"%s - %d\"]}"
				,pstStatusList->stStatus.stPurse.szLabel, pstStatusList->stStatus.stPurse.ulBalance);
	}
	ulLengthSum = ulLengthSum+ulLength;

	// 4th line: 'Enter CA PIN to accept'
	if (pstMsgInfo->td_ulEventCost < pstStatusList->stStatus.stPurse.ulBalance)
	{
		ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"id\":\"LOC_ENTER_CAPIN_TO_ACCEPT_ID\"}");
	}
	else
	{
		ulLength = HxSTD_sprintf(pucTokenPpvStr + ulLengthSum, ",{\"id\":\"LOC_NOT_ENOUGH_TOKENS_ID\"}");
	}
	ulLengthSum = ulLengthSum+ulLength;

	*(pucTokenPpvStr + ulLengthSum) = '\0';

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendResponseTextboxMessage(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgType_t	eReturnMsgType)
{
	HINT32					slStream = 0;
	HCHAR					*pzProperties = NULL;
	OxMgrCasUiEvtTextbox_t	*pstEvent = NULL;
	HUINT32					ulStreamSize = 0;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HERROR					hError = ERR_OK;

	HxLOG_Trace();
	pstEvent = (OxMgrCasUiEvtTextbox_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtTextbox_t));
	if(pstEvent == (OxMgrCasUiEvtTextbox_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	// Set Text
	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");
	switch(eReturnMsgType)
	{
		case eCxUi_GroupMr_WrongPin:
		case eCxUi_GroupMr_PinLocked:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT2_ID\"}");
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Highlight;
			pstEvent->stTextbox.stHeader.ulPosition = eMgrCasUiPosition_Top;
			break;

		default:
			HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", eReturnMsgType);
			break;
	}
	HLIB_RWSTREAM_Print(slStream, "]");


	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstEvent->stTextbox.ulStringLength = ulStreamSize;

	pstEvent->stTextbox.pszString = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	MWC_UTIL_DvbStrncpy(pstEvent->stTextbox.pszString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_TEXTBOX_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendTextboxMessage(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstMsgInfo, CxStatusList_t *pstStatusList)
{
	HINT32					slStream = 0;
	HCHAR					*pzProperties = NULL;
	OxMgrCasUiEvtTextbox_t	*pstEvent = NULL;
	HUINT32					ulStreamSize = 0;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HUINT8					*pszText = NULL;
	HCHAR					*pzEncodedStr = NULL;
	HERROR					hError = ERR_OK;

	HxLOG_Trace();
	pstEvent = (OxMgrCasUiEvtTextbox_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtTextbox_t));
	if(pstEvent == (OxMgrCasUiEvtTextbox_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	// Set Title & attribute
	switch(pstMsgInfo->eUiMsgType)
	{
		case eCxUi_GroupPpv_TokenPpv:
			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_TOKENS_ACCESS_ID\"}]");
			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
			MWC_UTIL_DvbStrncpy(pstEvent->stTextbox.stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			HLIB_STD_MemFree(pzProperties);
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Title;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Highlight;
			pstEvent->stTextbox.stHeader.ulPosition = eMgrCasUiPosition_Top;
			break;
		case eCxUi_GroupPpv_OrderInfo:
			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_ORDERING_ID\"}]");
			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
			MWC_UTIL_DvbStrncpy(pstEvent->stTextbox.stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			HLIB_STD_MemFree(pzProperties);
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Title;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Highlight;
			pstEvent->stTextbox.stHeader.ulPosition = eMgrCasUiPosition_Top;
			break;
		case eCxUi_GroupMessage_Display:
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
			pstEvent->stTextbox.stHeader.ulPosition = eMgrCasUiPosition_Top;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Timeout;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Tenable;
			pstEvent->stTextbox.stHeader.ulTimeoutMilliSecond = pstMsgInfo->ut_usDuration*1000;
			break;
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		case eCxUi_GroupOther_Fingerprint:
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_FingerPrint;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPrintTime;
			//pstEvent->stTextbox.stHeader.ulPrintTime = pstMsgInfo->fp_ulTime;
			// Schedule ������ Obama���� �����ϰ�, Native APP������ ������ Immediate �� ó���ϵ��� �Ѵ�.
			pstEvent->stTextbox.stHeader.ulPrintTime = CX_FINGER_PRINT_IMMEDIATELY_TIME;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_Timeout;
			pstEvent->stTextbox.stHeader.ulTimeoutMilliSecond = pstMsgInfo->fp_usDuration*10;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
			pstEvent->stTextbox.stHeader.ulPosition = eMgrCasUiPosition_Coordinate;
			pstEvent->stTextbox.stHeader.ulPosX = pstMsgInfo->fp_usPosX;
			pstEvent->stTextbox.stHeader.ulPosY = pstMsgInfo->fp_usPosY;
			pstEvent->stTextbox.stHeader.ulAttr |= eMgrCasUiAttr_SetSize;
			pstEvent->stTextbox.stHeader.ulHeight = pstMsgInfo->fp_usHeight;

			break;
#endif
		default:
			HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", pstMsgInfo->eUiMsgType);
			break;
	}

	// Set Text

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");
	switch(pstMsgInfo->eUiMsgType)
	{
		case eCxUi_GroupPpv_TokenPpv:
		{
			HCHAR szTokenPpvStr[MGR_CAS_STR_LENGTH_LONG];
			if(xmgr_cas_CxUiPopUpSetTokenPpvString(szTokenPpvStr, pstMsgInfo, pstStatusList)==ERR_OK)
			{
				HLIB_RWSTREAM_Print(slStream, "%s", szTokenPpvStr);
			}
			else
			{
				HxLOG_Error("ERROR! xmgr_cas_CxUiPopUpSetTokenPpvString()...\n");
			}
			break;
		}
		case eCxUi_GroupPpv_OrderInfo:
			HLIB_RWSTREAM_Print(slStream, "{\"str\":\"%s / %d.%02d %s\"}"
				, pstMsgInfo->or_szLabel, pstMsgInfo->or_usPriceUnit, pstMsgInfo->or_ucPriceDecimal, pstMsgInfo->or_aucCurrency);
			break;
		case eCxUi_GroupMessage_Display:
		{
			xmgr_cas_CxUiPopUpConvSiStringToUtf8(CX_MAX_USER_TEXT_LEN, pstMsgInfo->ut_szText, &pszText);

			if(pszText != NULL)
			{
				pzEncodedStr = xmgr_cas_CxUiPopUpEncodeString(pszText);
				HxLOG_Info("pzEncodedStr:[%s]...\n", pzEncodedStr);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "{\"str\":\"%s\"}", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}
				HLIB_STD_MemFree(pszText);
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
		}
			break;
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		case eCxUi_GroupOther_Fingerprint:
			HLIB_RWSTREAM_Print(slStream, "{\"str\":\"%s\",\"Displayed\":%d,\"ProtectedData\":%d}",
						pstMsgInfo->fp_szText,
						pstMsgInfo->fp_ucDisplayed,
						pstMsgInfo->fp_bFPFromProtectedData);
			break;
#endif
		default:
			HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", pstMsgInfo->eUiMsgType);
			break;
	}
	HLIB_RWSTREAM_Print(slStream, "]");


	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstEvent->stTextbox.ulStringLength = ulStreamSize;

	pstEvent->stTextbox.pszString = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	MWC_UTIL_DvbStrncpy(pstEvent->stTextbox.pszString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_TEXTBOX_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendEnqMessage(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstMsgInfo, CxStatusList_t *pstStatusList)
{
	HERROR					hError = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtInput_t	*pstInputEvent = NULL;
	OxMgrCasUiInputNumber_t	*pstNumber = NULL;
	HINT32					slStream;
	HCHAR					*pzProperties = NULL;

	HxLOG_Info("xmgr_cas_CxUiPopUpSendEnqMessage()\n");

	pstInputEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstInputEvent == NULL)
	{
		return ERR_FAIL;
	}

	pstInputEvent ->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstInputEvent ->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstInputEvent ->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	// Set Title
	if(pstMsgInfo->eUiMsgType == eCxUi_GroupPpv_TokenPpv || pstMsgInfo->eUiMsgType == eCxUi_GroupPpv_AcceptViewing
		|| pstMsgInfo->eUiMsgType == eCxUi_GroupMr_Ask || pstMsgInfo->eUiMsgType == eCxUi_GroupOther_CachedPinWithErr)
	{
		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

		switch(pstMsgInfo->eUiMsgType)
		{
			case eCxUi_GroupPpv_TokenPpv:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_TOKENS_ACCESS_ID\"}]");
				break;
			case eCxUi_GroupPpv_AcceptViewing:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_START_VIEWING_ID\"}]");
				break;
			case eCxUi_GroupMr_Ask:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_MATURITY_LOCK_ID\"}]");
				break;
			case eCxUi_GroupOther_CachedPinWithErr:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\" \"}]");
				break;
			default:
				HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", pstMsgInfo->eUiMsgType);
				break;
		}

		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pstInputEvent->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		HLIB_STD_MemFree(pzProperties);
		pstInputEvent->stHeader.ulAttr |= eMgrCasUiAttr_Title;
	}

	// Set Sub-Title
	{
		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		switch(pstMsgInfo->eUiMsgType)
		{
			case eCxUi_GroupPpv_TokenPpv:
			{
				HCHAR szSubTitleStr[MGR_CAS_STR_LENGTH_LONG];

				if(xmgr_cas_CxUiPopUpSetTokenPpvString(szSubTitleStr, pstMsgInfo, pstStatusList)==ERR_OK)
				{
					HLIB_RWSTREAM_Print(slStream, "[%s]", szSubTitleStr);
				}
				else
				{
					HxLOG_Error("ERROR! xmgr_cas_CxUiPopUpSetTokenPpvString()...\n");
				}
				break;
			}
			case eCxUi_GroupPpv_AcceptViewing:
			{
				HLIB_RWSTREAM_Print(slStream, "[{\"str\":\"%s\"},{\"id\":\"LOC_VIEWING_TIME_LEFT_ID\",\"str\":\" : %d\"},{\"id\":\"LOC_ENTER_CAPIN_TO_VIEW_ID\"}]"
						, pstMsgInfo->av_szLabel, pstMsgInfo->av_usMinuteLeft);
				break;
			}
			case eCxUi_GroupMr_Ask:
			{
				HUINT8 szTemp1[TEXT_SIZE128] = {0,};
				HUINT8 szTemp2[TEXT_SIZE128] = {0,};
				switch(pstMsgInfo->mr_ucProgram)
				{
					case 0x01: // G - General audience
						HxSTD_snprintf((char*)szTemp1, TEXT_SIZE128, (char*)"{\"id\":\"LOC_PROGRAM_RATING_G_ID\"}");
						break;
					case 0x02: // PG - With parental guidance
						HxSTD_snprintf((char*)szTemp1, TEXT_SIZE128, (char*)"{\"id\":\"LOC_PROGRAM_RATING_PG_ID\"}");
						break;
					case 0x04: // A - Over 18
						HxSTD_snprintf((char*)szTemp1, TEXT_SIZE128, (char*)"{\"id\":\"LOC_PROGRAM_RATING_A_ID\"}");
						break;
					case 0x08: // X - Erotiv
						HxSTD_snprintf((char*)szTemp1, TEXT_SIZE128, (char*)"{\"id\":\"LOC_PROGRAM_RATING_X_ID\"}");
						break;
					case 0x09: // XXX
						HxSTD_snprintf((char*)szTemp1, TEXT_SIZE128, (char*)"{\"id\":\"LOC_PROGRAM_RATING_XXX_ID\"}");
						break;
					default:
						HxLOG_Error("ERROR... mr_ucProgram: [0x%02x]\n", pstMsgInfo->mr_ucProgram);
						break;
				}
				switch(pstMsgInfo->mr_ucSmartcard)
				{
					case 0x01: // G - General audience
						HxSTD_snprintf((char*)szTemp2, TEXT_SIZE128, (char*)"{\"id\":\"LOC_CARD_LEVEL_G_ID\"}");
						break;
					case 0x02: // PG - With parental guidance
						HxSTD_snprintf((char*)szTemp2, TEXT_SIZE128, (char*)"{\"id\":\"LOC_CARD_LEVEL_PG_ID\"}");
						break;
					case 0x04: // A - Over 18
						HxSTD_snprintf((char*)szTemp2, TEXT_SIZE128, (char*)"{\"id\":\"LOC_CARD_LEVEL_A_ID\"}");
						break;
					case 0x08: // X - Erotiv
						HxSTD_snprintf((char*)szTemp2, TEXT_SIZE128, (char*)"{\"id\":\"LOC_CARD_LEVEL_X_ID\"}");
						break;
					case 0x09: // XXX
						HxSTD_snprintf((char*)szTemp2, TEXT_SIZE128, (char*)"{\"id\":\"LOC_CARD_LEVEL_XXX_ID\"}");
						break;
					default:
						HxLOG_Error("ERROR... mr_ucSmartcard: [0x%02x]\n", pstMsgInfo->mr_ucSmartcard);
						break;
				}
				HLIB_RWSTREAM_Print(slStream, "[%s,%s,{\"id\":\"LOC_ENTER_CAPIN_TO_UNLOCK_ID\"}]", szTemp1, szTemp2);
				break;
			}
			case eCxUi_GroupOther_CachedPin:
			{
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CACHING_CA_PIN_ID\"}]");
				break;
			}
			case eCxUi_GroupOther_CachedPinWithErr:
			{
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CIPLUS_PIN_TEXT2_ID\"},{\"id\":\"LOC_CACHING_CA_PIN_ID\"}]");
				break;
			}
			default:
				HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", pstMsgInfo->eUiMsgType);
				break;
		}

		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pstInputEvent->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		HLIB_STD_MemFree(pzProperties);
		pstInputEvent->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
	}

	pstInputEvent->stInputData.eInputType = eMgrCasInputType_Number;

	pstNumber = (OxMgrCasUiInputNumber_t *)&pstInputEvent->stInputData.utInputData;
	pstNumber->ulAttr = eMgrCasUiAttr_HiddenInput;
	pstNumber->ulNumberLength = 4;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstInputEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	hError = CXAPI_UiPinMmiDisplayed(TRUE);
	if(hError != ERR_OK)
	{
		HxLOG_Error("\n\n");
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstInputEvent != NULL)
		{
			HLIB_STD_MemFree(pstInputEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendMailIconMessage(XmgrCas_CxUiPopUpContext_t *pstContext, HBOOL bShow)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMail_t		*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMail_t));

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	/* Display�� String length�� 0���� �� ��� Banner �� hide��Ų��. */
	pstEvent->stMail.ulTextLength = 0;
	pstEvent->stMail.stHeader.ulMailIconType = (HUINT32)bShow;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MAIL_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendHideMessage(XmgrCas_CxUiPopUpContext_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	/* Display�� String length�� 0���� �� ��� Banner �� hide��Ų��. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_cas_CxUiPopUpSendBannerMessage(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstMsgInfo) //, CxStatusList_t *pstStatusList)
{
	HINT32					slStream = 0;
	HCHAR					*pzProperties = NULL;
	OxMgrCasUiEvtBanner_t 	*pstEvent = NULL;
	HUINT32					ulStreamSize = 0;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HERROR					hError = ERR_OK;

	HxLOG_Trace();
	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == (OxMgrCasUiEvtBanner_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiPopUp;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	switch(pstMsgInfo->eUiMsgType)
	{
		case eCxUi_GroupBasic_NoAccess:
#if defined(CONFIG_MW_CAS_CONAX_CPM)
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_NO_ACCESS_ID\",\"str\":\" (%d)\"}]", pstMsgInfo->cpm_nErrCode);	//STR_NO_ACCESS_ID
#else
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_NO_ACCESS_ID\"}]");	//STR_NO_ACCESS_ID
#endif
			break;
		case eCxUi_GroupBasic_NoAccessBlackOut:
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_NO_ACCESS_GEOGRAPHICAL_ID\"}]");	//STR_MESG_377_ID
			break;
		case eCxUi_GroupBasic_NoAccessNetwork:
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_NO_ACCESS_NETWORK_ID\"}]");	//STR_MESG_378_ID
			break;
		case eCxUi_GroupSc_NoCard:
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_NO_CARD_INSERTED_ID\"}]");	//STR_NO_CARD_INSERTED_01_ID
			break;
		case eCxUi_GroupSc_Incorrect:
		case eCxUi_GroupSc_IncorrectConax:
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_INCORRECT_CARD_ID\"}]");	//STR_NO_CARD_INSERTED_01_ID
			break;
		case eCxUi_GroupSc_CommFail:
			HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CARD_PROBLEM_ID\"}]");	//STR_MESG_5683_ID
			break;
		default:
			HxLOG_Assert(0);
			break;
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstEvent->stBanner.ulAttr = 0;
	pstEvent->stBanner.stString.ulStringLength = ulStreamSize;
	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstEvent);
		}
	}

	return hError;
}

STATIC HERROR	xmgr_cas_CxUiPopUpOperateCachedPin(XmgrCas_CxUiPopUpContext_t *pstContext)
{
	CxUiMsgInfo_t	stMsgInfo;
	HERROR 			hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = xmgr_cas_CxGetSCState(CX_SC_SLOT_0);
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("S/C is not inserted or not appropriate!!\n");
		// SmartCard�� ���ԵǾ����� �ʰų� ������ �ִ� ��� Cached PIN �ó������� �������� �ʴ´�. �������̹Ƿ� OK�� return.
		return ERR_OK;
	}

	// 1. cached pin ���� ��û
	hErr = xmgr_cas_CxOperateCachedPin(NULL);
	if (hErr != ERR_OK)
	{
		// Cache �Ǿ��ִ� pin �� default pin�� ���� �������
		stMsgInfo.eUiMsgType = eCxUi_GroupOther_CachedPin;
		hErr = xmgr_cas_CxUiPopUpSendEnqMessage(pstContext, &stMsgInfo, NULL);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("ERROR!! xmgr_cas_CxUiPopUpSendEnqMessage()...\n");
			return hErr;
		}
		pstContext->stOldMsgInfo.eUiMsgType= eCxUi_GroupOther_CachedPin;
	}

	return ERR_OK;
}

STATIC BUS_Result_t xmgr_cas_CxUiPopUpMsgEvtCasAlarm(XmgrCas_CxUiPopUpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CxUiMsgInfo_t*		pstMsgInfo		= (CxUiMsgInfo_t*)p2;
	HERROR				hErr = ERR_FAIL;

	if(pstContext->eSessionStatus != eCX_UI_SESSION_STATUS_Started)
	{
		HxLOG_Warning("Session is not started...\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	HxLOG_Info("eUiMsgType:[%08x]...\n", pstMsgInfo->eUiMsgType);

	switch(pstMsgInfo->eUiMsgType)
	{
		case eCxUi_GroupBasic_NoAccess:
		case eCxUi_GroupBasic_NoAccessBlackOut:
		case eCxUi_GroupBasic_NoAccessNetwork:
			switch(pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP)
			{
				case eCxUi_GroupPpv:
				case eCxUi_GroupMr:
					xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
					break;

				default:
					break;
			}

			if(xmgr_cas_CxUiPopUpSendBannerMessage(pstContext, pstMsgInfo) != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiPopUpSendBannerMessage() error\n");
				return ERR_BUS_MESSAGE_BREAK;
			}
			break;
		case eCxUi_GroupBasic_NoAccessPullEmm:
			break;
		case eCxUi_GroupMr_Ask:
			if((pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			}
			hErr = xmgr_cas_CxUiPopUpSendEnqMessage(pstContext, pstMsgInfo, NULL);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("ERROR!! xmgr_cas_CxUiPopUpSendEnqMessage()...\n");
			}
			break;

		case eCxUi_GroupSc_NoCard:
		case eCxUi_GroupSc_Incorrect:
		case eCxUi_GroupSc_IncorrectConax:
		case eCxUi_GroupSc_CommFail:
			if(xmgr_cas_CxUiPopUpSendBannerMessage(pstContext, pstMsgInfo) != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiPopUpSendBannerMessage() error\n");
				return ERR_BUS_MESSAGE_BREAK;
			}
			break;

		case eCxUi_GroupSc_Inserted:
			break;

		case eCxUi_GroupPpv_TokenPpv:
		{
			CxStatusList_t 	*pstStatusList = NULL;

			pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Purse, pstMsgInfo->td_usPurseRef);
			if (pstStatusList == NULL)
			{
				HxLOG_Critical("\n\n");
				break;
			}

			if((pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			}

			if (pstMsgInfo->td_ulEventCost < pstStatusList->stStatus.stPurse.ulBalance)
			{
				hErr = xmgr_cas_CxUiPopUpSendEnqMessage(pstContext, pstMsgInfo, pstStatusList);
			}
			else
			{
				hErr = xmgr_cas_CxUiPopUpSendTextboxMessage(pstContext, pstMsgInfo, pstStatusList);
			}
			break;
		}
		case eCxUi_GroupPpv_OrderInfo:
			if((pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			}
			hErr = xmgr_cas_CxUiPopUpSendTextboxMessage(pstContext, pstMsgInfo, NULL);
			break;
		case eCxUi_GroupPpv_AcceptViewing:
			if((pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			}
			hErr = xmgr_cas_CxUiPopUpSendEnqMessage(pstContext, pstMsgInfo, NULL);
			break;
		case eCxUi_GroupMessage_Display:
			if((pstContext->stOldMsgInfo.eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			}
			hErr = xmgr_cas_CxUiPopUpProcessUserText(pstContext, pstMsgInfo, 0);
			break;
		default:
			HxLOG_Critical("Wrong UI Msg Type:[%08x]... Check it!! \n", pstMsgInfo->eUiMsgType);
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC HERROR	xmgr_cas_CxUiPopUpSetScPinForTokenPpv(Handle_t hAction, HUINT8* szPincode, CxUiMsgType_t *eReturnMsg)
{
	HERROR			hErr = ERR_OK;
	SvcCas_CtrlParam_t			stParam;
	CX_SetScPinInParam_t		stInParam;
	CX_SetScPinOutParam_t		stOutParam;

	if (szPincode == NULL)
	{
		return	ERR_FAIL;
	}

	stParam.ulControlType = eCACTRL_CX_SetScPinForTokenPpv;
	stInParam.hHandle = hAction;
	stInParam.pucPin = szPincode;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetScPinForTokenPpv, &stParam);

	return	hErr;
}

STATIC HERROR	xmgr_cas_CxUiPopUpSetScPin(Handle_t hAction, HUINT8* szPincode, CxUiMsgType_t eCurMsgType, CxUiMsgType_t *eReturnMsgType)
{
	HERROR			hErr = ERR_OK;
	SvcCas_CtrlParam_t			stParam;
	CX_SetScPinInParam_t		stInParam;
	CX_SetScPinOutParam_t		stOutParam;

	if (szPincode == NULL)
	{
		return	ERR_FAIL;
	}

	stParam.ulControlType = eCACTRL_CX_SetScPin;
	stInParam.hHandle = hAction;
	stInParam.pucPin = szPincode;
	stInParam.eMsgType = eCurMsgType;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetScPin, &stParam);

	*eReturnMsgType = stOutParam.eReturnMsg;

	return	hErr;
}

STATIC BUS_Result_t xmgr_cas_CxUiPopUpInputNotify(XmgrCas_CxUiPopUpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 							hError = ERR_FAIL;
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;

	HxLOG_Trace();

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	if(eSessionType != eDxCAS_SESSIONTYPE_BANNER)
	{
		HxLOG_Error("eSessionType should be eDxCAS_SESSIONTYPE_BANNER!! Check it!!\n");
	}

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Etc:
			switch(pstUserInputNotify->utData.stEtc.eMgrCasEtcType)
			{
				case eMgrCasEtc_Previous:
					HxLOG_Info("eMgrCasEtc_Previous\n");
					pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
					break;

				case eMgrCasEtc_Exit:
					HxLOG_Info("eMgrCasEtc_Previous\n");
					pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
					break;

				default:
					HxLOG_Critical("ERROR!! Check it...\n");
					break;
			}
			hError = CXAPI_UiPinMmiDisplayed(FALSE);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			break;

		case eMgrCasInputType_Number:
		{
			switch(pstContext->stOldMsgInfo.eUiMsgType)
			{
				case eCxUi_GroupPpv_TokenPpv:
				case eCxUi_GroupPpv_AcceptViewing:
				case eCxUi_GroupMr_Ask:
				{
					CxUiMsgType_t	eReturnMsgType;

					if(pstUserInputNotify->utData.stNumber.ulNumberLength == 4)
					{
						HUINT32 		ulMainActionId = MGR_ACTION_GetMainActionId();
						Handle_t		hMainAction;

						hMainAction = SVC_PIPE_GetActionHandle(ulMainActionId);

						if(hAction != hMainAction)
						{
							HxLOG_Warning("hAction error... check it... hAction:[0x%08x]\n", hAction);
							hError = xmgr_cas_CxUiPopUpSetScPin(hMainAction, pstUserInputNotify->utData.stNumber.pszNumber, pstContext->stOldMsgInfo.eUiMsgType, &eReturnMsgType);
							if(hError != ERR_OK)
							{
								HxLOG_Error("xmgr_cas_CxUiPopUpSetScPin() error\n");
							}
						}
						else
						{
							hError = xmgr_cas_CxUiPopUpSetScPin(hAction, pstUserInputNotify->utData.stNumber.pszNumber, pstContext->stOldMsgInfo.eUiMsgType, &eReturnMsgType);
							if(hError != ERR_OK)
							{
								HxLOG_Error("xmgr_cas_CxUiPopUpSetScPin() error\n");
							}
						}

						hError = xmgr_cas_CxUiPopUpSendResponseTextboxMessage(pstContext, eReturnMsgType);
						if(hError != ERR_OK)
						{
							HxLOG_Error("xmgr_cas_CxUiPopUpSendResponseTextboxMessage() error\n");
							return ERR_BUS_MESSAGE_BREAK;
						}
						else
						{
							pstContext->stOldMsgInfo.eUiMsgType = eReturnMsgType;
						}

						xmgr_cas_CxUiPopUpMsgEvtCasAlarm(pstContext, (HINT32)NULL, (Handle_t)NULL, (HINT32)NULL, (HINT32)&pstContext->stOldMsgInfo, (HINT32)NULL);
					}
					break;
				}
#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
				case eCxUi_GroupOther_CachedPin:
				case eCxUi_GroupOther_CachedPinWithErr:
				{
					CxUiMsgType_t	eReturnMsgType;

					hError = xmgr_cas_CxUiPopUpSetScPin(HANDLE_NULL, pstUserInputNotify->utData.stNumber.pszNumber, pstContext->stOldMsgInfo.eUiMsgType, &eReturnMsgType);
					if (hError == ERR_OK && eReturnMsgType == eCxUi_GroupAll_None)
					{
						HxLOG_Info("Pin is correct...\n");
						hError = xmgr_cas_CxOperateCachedPin(pstUserInputNotify->utData.stNumber.pszNumber);
						if(hError != ERR_OK)
						{
							HxLOG_Error("xmgr_cas_CxOperateCachedPin()...\n");
							break;
						}
					}
					else
					{
						CxUiMsgInfo_t	stMsgInfo;

						HxLOG_Info("Pin is NOT correct...\n");
						stMsgInfo.eUiMsgType = eCxUi_GroupOther_CachedPinWithErr;
						hError = xmgr_cas_CxUiPopUpSendEnqMessage(pstContext, &stMsgInfo, NULL);
						if(hError != ERR_OK)
						{
							HxLOG_Error("ERROR!! xmgr_cas_CxUiPopUpSendEnqMessage()...\n");
						}
						pstContext->stOldMsgInfo.eUiMsgType= eCxUi_GroupOther_CachedPinWithErr;
					}
					break;
				}
#endif
				default:
					break;
			}

			hError = CXAPI_UiPinMmiDisplayed(FALSE);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			break;
		}

		case eMgrCasInputType_Selection:
			break;

		default:
			break;
	}

	NOT_USED_PARAM(hError);
	NOT_USED_PARAM(ulSlotNumber);

	return ERR_BUS_MESSAGE_BREAK;
}

#define ________USER_TEXT________________________________
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC HERROR		xmgr_cas_CxUiPopUpGetUserTextPrintingTime(CxUiMsgInfo_t* pstCxUiInfo, HBOOL *pbUserTextTime, HULONG *pulTime, HUINT32 ulCurrentTime, UNIXTIME ulUTTime)
{
	int 			i = 0;
	HUINT32 		ulDiffTime = 0, ulSec = 0;
	HxDATETIME_t	stDateTime;
	HUINT32			ulMaxOffsetTime = 0;
	UNIXTIME 		ulUxTime = 0;	// ���� Ƚ�� �ݺ��� �Ǿ��ٰ� �Ǵ��ϰ� Time���.

	HxLOG_Print("[UT] Displayed(%d), Duration(%d), Repeat(%d)\n", pstCxUiInfo->ut_ucDisplayed, pstCxUiInfo->ut_usDuration, pstCxUiInfo->ut_ucRepeat);

	if(pstCxUiInfo->ut_ucDisplayed == 0 || pstCxUiInfo->ut_usDuration == CX_UI_UT_MAIL_MESSAGE_VALUE)
	{
		// Mail Message�� ����
		// (�ݺ� User Text�� ���)�ѹ��� Display ���� ���� ����, ���� �ð��� �Ѹ����� �׳� ������.
		return ERR_FAIL;
	}

	// �ݺ��� �����Ͱ� �ִ� ���
	if((pstCxUiInfo->ut_ucDisplayed > 0) && (pstCxUiInfo->ut_ucDisplayed < pstCxUiInfo->ut_ucRepeat))
	{
		ulMaxOffsetTime = ((pstCxUiInfo->ut_ucRepeat - 1) * pstCxUiInfo->ut_usInterval);

		// User Text�� Display�ϴ� �ð� ���� - �귯�� �ð���ŭ ����.
		for(i = 0; i<pstCxUiInfo->ut_ucDisplayed; i++)
		{
			// Interval�� ���� User Text�� Display time�� �ǹ��Ѵ�.
			ulUxTime = ulUTTime + pstCxUiInfo->ut_usInterval;
		}

		if(ulCurrentTime < ulUxTime)
		{
			ulDiffTime = (ulUTTime - ulCurrentTime);

			HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
			ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;

			HxLOG_Print("[1]ADJUST- OFS: start:%d/%d/%d %d:%02d:%02d\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay
				, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
			HxLOG_Print("[1]ADJUST- Display in %d seconds!\n", ulSec);

			*pulTime = (ulSec*CAS_CX_SECONDS);

			*pbUserTextTime = TRUE;
		}
		else if(ulCurrentTime > ulUxTime)
		{
			if(ulCurrentTime < (ulUTTime + ulMaxOffsetTime))
			{
				ulDiffTime = ((ulUTTime + ulMaxOffsetTime) - ulCurrentTime);

				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
				ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;

				HxLOG_Print("[2]ADJUST- OFS: start:%d/%d/%d %d:%02d:%02d\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay
					, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
				HxLOG_Print("[2]ADJUST- Display in %d seconds!\n", ulSec);

				*pulTime = (ulSec*CAS_CX_SECONDS);

				*pbUserTextTime = TRUE;
			}
		}
		else
		{
			return ERR_FAIL;
		}

		return ERR_OK;
	}
	else
	{
			return ERR_FAIL;
	}

	return ERR_FAIL;
}

STATIC	HERROR		xmgr_cas_CxUiPopUpCalculateUserTextTime(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstCxUiInfo, HBOOL *pbUserTextTime, HULONG *pulTime)
{
	HUINT32			ulError;
	HULONG			ulCurrentTime = 0, ulDiffTime = 0, ulSec = 0;
	UNIXTIME		ulUTTime=0, ulUTOffsetTime=0;
	HxDATETIME_t	stCurrentDateTime;
	HxDATETIME_t	stDateTime;
	UNIXTIME 		ulUTTimeTemp = 0;
	HINT32			i = 0;
//	HUINT32 		ulNeedStopHere = 0;

	HxLOG_Info("[ENTER]\r\n");

	*pbUserTextTime = FALSE;
	*pulTime = 0;

	// Ui Data
	if (pstCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	if (pstCxUiInfo->ut_usDuration != CX_UI_UT_MAIL_MESSAGE_VALUE)
	{
		if(((pstCxUiInfo->ut_stStartDataTime.stDate.usYear == 0xFF)&&(pstCxUiInfo->ut_stStartDataTime.stDate.ucMonth == 0xFF)&&(pstCxUiInfo->ut_stStartDataTime.stDate.ucDay == 0xFF))
			|| ((pstCxUiInfo->ut_stStartDataTime.stDate.usYear == 0x0)&&(pstCxUiInfo->ut_stStartDataTime.stDate.ucMonth == 0x0)&&(pstCxUiInfo->ut_stStartDataTime.stDate.ucDay == 0x0)))
		{
			// Immediate Message �̸� current �ð��� ��� ���� �ٷ� ��� ��.
			// ut_usDuration�� 0�̶�� Mail Message�� ��Ÿ���µ�, Immediate Mail message�� ���� ���� �ð��� ��ϵǾ�� �ϹǷ� �Ʒ� CurrentTime�� ���������� ������ �;����� ó���ȴ�.
			HxLOG_Print("- User Text Display Immediately!\n");

			*pbUserTextTime = FALSE;
			return ERR_OK;
		}
	}

	// Get Current Time
	ulError = VK_CLOCK_GetTime(&ulCurrentTime);
	if (ulError != VK_OK)
	{
		HxLOG_Critical("\n\n");
		pstContext->stUserTextInfo.eUiMsgType = pstCxUiInfo->eUiMsgType;
		pstContext->stUserTextInfo.ut_ucDisplayed = pstCxUiInfo->ut_ucDisplayed;
		pstContext->stUserTextInfo.ut_usDuration = pstCxUiInfo->ut_usDuration;		// �� ���� 0�̸� �޽����� ǥ�������� �ʰ� �Դٰ� �˷��ش� (mailbox message). ���� second.
		pstContext->stUserTextInfo.ut_ucSeqNo = pstCxUiInfo->ut_ucSeqNo;
		pstContext->stUserTextInfo.ut_ucRepeat = pstCxUiInfo->ut_ucRepeat;			// ������ 1ȸ�� ���
		pstContext->stUserTextInfo.ut_usInterval = pstCxUiInfo->ut_usInterval;		// �޽����� ǥ���ϴ� ���� (10�ʴ����� ������ *10�Ͽ� second������ �����ϰ� ����)
		HxSTD_memcpy(pstContext->stUserTextInfo.ut_szText, pstCxUiInfo->ut_szText, CX_MAX_USER_TEXT_LEN + 1);
		HxSTD_memcpy(&pstContext->stUserTextInfo.ut_stStartDataTime, &pstCxUiInfo->ut_stStartDataTime, sizeof(HxDATETIME_t));

		BUS_KillTimer(CX_UI_UT_GETTINGSITDTTIMER_ID);
		BUS_SetTimer(CX_UI_UT_GETTINGSITDTTIMER_ID, CX_USET_TEXT_WAITING_TDT_TIME);
		return	ERR_FAIL;
	}
	else
	{
		BUS_KillTimer(CX_UI_UT_GETTINGSITDTTIMER_ID);
	}

	// Conv Current time to date time with GMT Offset
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrentTime, &stCurrentDateTime);

	HxLOG_Info("- seq[%d], Text[%s], Duration[%d], Repetition[%d], Interval[%d]\n",
		pstCxUiInfo->ut_ucSeqNo, pstCxUiInfo->ut_szText, pstCxUiInfo->ut_usDuration, pstCxUiInfo->ut_ucRepeat, pstCxUiInfo->ut_usInterval);
	HxLOG_Info("- Current Time:%d/%d/%d %d:%02d:%02d\n", stCurrentDateTime.stDate.usYear, stCurrentDateTime.stDate.ucMonth,
		stCurrentDateTime.stDate.ucDay, stCurrentDateTime.stTime.ucHour, stCurrentDateTime.stTime.ucMinute, stCurrentDateTime.stTime.ucSecond);
	HxLOG_Info("- UT Time:%d/%d/%d %d:%02d:%02d\n", pstCxUiInfo->ut_stStartDataTime.stDate.usYear, pstCxUiInfo->ut_stStartDataTime.stDate.ucMonth,
		pstCxUiInfo->ut_stStartDataTime.stDate.ucDay, pstCxUiInfo->ut_stStartDataTime.stTime.ucHour, pstCxUiInfo->ut_stStartDataTime.stTime.ucMinute,
		pstCxUiInfo->ut_stStartDataTime.stTime.ucSecond);

	// Conv user text start date time to unixtime
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstCxUiInfo->ut_stStartDataTime, &ulUTTime);

/*
	ulNeedStopHere = xmgr_cas_CxUiPopUpGetUserTextPrintingTime(pstCxUiInfo, pbUserTextTime, pulTime, ulCurrentTime, ulUTTime);

	if(ulNeedStopHere == ERR_OK)
	{
		HxLOG_Error("After Boot, adjust UT Time with Current Time - timer(%d)\n",pulTime);
		return	ERR_OK;
	}
*/
	// ���Ĺ����� ��� ( Mail Msg ���� )
	// �ݺ��� �����Ͱ� �ִ� ��� �� ����.
	if((pstCxUiInfo->ut_ucDisplayed > 0) && (pstCxUiInfo->ut_ucDisplayed < pstCxUiInfo->ut_ucRepeat) && (pstCxUiInfo->ut_usDuration != CX_UI_UT_MAIL_MESSAGE_VALUE))
	{
		// Standby �� ��쵵, Display �ð��� �Ǹ�, �ð� ������� �ʴ´�.
		if ((ulCurrentTime == ulUTTime) || ((ulCurrentTime-CX_UI_UT_OFFSET_SEC_TIME) == ulUTTime))
		{
			*pbUserTextTime = FALSE;
			return	ERR_OK;
		}

		i=0;
		ulUTOffsetTime = 0;
		ulUTTimeTemp = ulUTTime;

		while(ulUTTimeTemp < ulCurrentTime)
		{
			ulUTOffsetTime = (ulUTOffsetTime + pstCxUiInfo->ut_usInterval);
			ulUTTimeTemp = (ulUTTimeTemp + pstCxUiInfo->ut_usInterval);

			i++;

			// User Text�� Display�ϴ� Ƚ�� ���� - �귯�� �ð���ŭ ����.
			pstCxUiInfo->ut_ucDisplayed = i;

			// debugging �� ulUTTimeTemp�� 0 �̶� while loop�� ���� ���� ������ ����. CT�� ���� ���� ������ ����� �־��� ��츸 �����Ѵ�.
			if (i > 200)
			{
				break;
			}

		}

		// User Text�� Display�ϴ� �ð� ���� - �귯�� �ð���ŭ ����.
		ulUTTime = (ulUTTime + ulUTOffsetTime);

		HxLOG_Info("recalculated ucDisplayed(%d)\n", pstCxUiInfo->ut_ucDisplayed);
		HxLOG_Info("add Offset time(%d) to UserText time (%d)\n", ulUTOffsetTime, ulUTTime);
	}

	// 0st case : start_time �ְ�, ���� �׷��� �ϴ� ���
	if ((ulCurrentTime == ulUTTime) || ((ulCurrentTime-CX_UI_UT_OFFSET_SEC_TIME) == ulUTTime))
	{
		HxLOG_Info("Case0 - User Text Display Immediately!\n");

		*pbUserTextTime = FALSE;
	}
	// 1st case : start_time ����, ���� �׷��� �ϴ� ��� ( TS�� ���� Msg ��찡 �ش�ȴ� )
	else if (ulUTTime == 0 && (pstCxUiInfo->ut_ulImmediateStartTime == 0))
	{
		HxLOG_Info("Case1 - User Text Display Immediately\n");

		*pbUserTextTime = FALSE;
	}
	// 2nd case : start_time�� �����ϰ� �̷��� ��� - Ÿ�̸� ����
	else if (ulUTTime > ulCurrentTime)
	{
		HxLOG_Info("Case2 - User Text Display Timer!\n");

		*pbUserTextTime = TRUE;
		ulDiffTime = (ulUTTime - ulCurrentTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
		ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;

		HxLOG_Print("- OFS: start:%d/%d/%d %d:%02d:%02d\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay
			, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
		HxLOG_Print("- Display in %d seconds!\n", ulSec);

		*pulTime = (ulSec*CAS_CX_SECONDS);
	}
	else
	{
		HxLOG_Info("Case3 - Is this needed?\n");
		HxLOG_Error("Past Time- UT Time:%d/%d/%d %d:%02d:%02d\n", pstCxUiInfo->ut_stStartDataTime.stDate.usYear, pstCxUiInfo->ut_stStartDataTime.stDate.ucMonth,
														pstCxUiInfo->ut_stStartDataTime.stDate.ucDay, pstCxUiInfo->ut_stStartDataTime.stTime.ucHour,
														pstCxUiInfo->ut_stStartDataTime.stTime.ucMinute, pstCxUiInfo->ut_stStartDataTime.stTime.ucSecond);

// CXHD-5100C 2011-08 1st Conax PT. USER_TEXT ���� �ð��� ��쿡�� ���� ��� ǥ�ø� �ؾ��Ѵٰ� ��. by Teppo in Labwise
// ��, Mailbox message�� ��쿡�� menu�� ǥ���ϵ� mail indication�� ���� �ʴ´�. (������ ���)
#if 1
		*pbUserTextTime = FALSE;
#else
		// Immediate Msg�� �ش�
		if(pstCxUiInfo->ut_usDuration != CX_UI_UT_MAIL_MESSAGE_VALUE)
		{
			// ���� �ð��� msg�� Display���� �ʵ��� �ϱ�����
			xmgr_cas_CxSetUserTextDisplayed(pstCxUiInfo->ut_ucSeqNo, eCxUTDisplayCntMax);
		}

		HxLOG_Critical("\n\n");

		return	ERR_FAIL;
#endif
	}

	HxLOG_Print("[EXIT]\r\n");

	return	ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiPopUpProcessUserText(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t*	pstMsgInfo, HUINT32 unTimerId)
{
	HBOOL		bUserTextTime = FALSE;
	HERROR		hError = ERR_OK;
	HULONG		ulTime = 0;
	HULONG		ulCurrentTime = 0;
	HBOOL		bMailIndicator = FALSE;

	if (unTimerId == CX_UI_UT_REPEATTIMER_ID)
	{
		if(((pstMsgInfo->ut_stStartDataTime.stDate.usYear == 0xFF)&&(pstMsgInfo->ut_stStartDataTime.stDate.ucMonth == 0xFF)&&(pstMsgInfo->ut_stStartDataTime.stDate.ucDay == 0xFF))
			|| ((pstMsgInfo->ut_stStartDataTime.stDate.usYear == 0x0)&&(pstMsgInfo->ut_stStartDataTime.stDate.ucMonth == 0x0)&&(pstMsgInfo->ut_stStartDataTime.stDate.ucDay == 0x0)))
		{
			hError = VK_CLOCK_GetTime(&ulCurrentTime);
			if (hError != VK_OK)
			{
				HxLOG_Critical("CRITICAL ERROR!! CHECK IT!!\n\n");
				return	ERR_FAIL;
			}

			HLIB_DATETIME_ConvertUnixTimeToDateTime((ulCurrentTime - (CX_USET_TEXT_WAITING_REPEAT_TIME/1000)), &pstMsgInfo->ut_stStartDataTime);
		}
	}

	hError = xmgr_cas_CxUiPopUpCalculateUserTextTime(pstContext, pstMsgInfo, &bUserTextTime, &ulTime);

	if (hError == ERR_OK)
	{
		// User Text Immediate Message
		// ó�� ���� User Text�� ��츸 setting. Timer�� ������ ���� ����.
		if (unTimerId != CX_UI_UT_STARTTIMER_ID)
		{
			pstContext->stUserTextInfo.eUiMsgType = pstMsgInfo->eUiMsgType;
			pstContext->stUserTextInfo.ut_ucDisplayed = pstMsgInfo->ut_ucDisplayed;
			pstContext->stUserTextInfo.ut_usDuration = pstMsgInfo->ut_usDuration;		// �� ���� 0�̸� �޽����� ǥ�������� �ʰ� �Դٰ� �˷��ش� (mailbox message). ���� second.
			pstContext->stUserTextInfo.ut_ucSeqNo = pstMsgInfo->ut_ucSeqNo;
			pstContext->stUserTextInfo.ut_ucRepeat = pstMsgInfo->ut_ucRepeat;			// ������ 1ȸ�� ���
			pstContext->stUserTextInfo.ut_usInterval = pstMsgInfo->ut_usInterval;		// �޽����� ǥ���ϴ� ���� (10�ʴ����� ������ *10�Ͽ� second������ �����ϰ� ����)
			HxSTD_memcpy(pstContext->stUserTextInfo.ut_szText, pstMsgInfo->ut_szText, CX_MAX_USER_TEXT_LEN + 1);
			HxSTD_memcpy(&pstContext->stUserTextInfo.ut_stStartDataTime, &pstMsgInfo->ut_stStartDataTime, sizeof(HxDATETIME_t));
		}

		if (bUserTextTime == TRUE)
		{
			HxLOG_Print("Set Uset Text Timer : CX_UI_UT_STARTTIMER_ID (%d)!!\n", ulTime);
			BUS_KillTimer(CX_UI_UT_STARTTIMER_ID);
			BUS_KillTimer(CX_UI_UT_REPEATTIMER_ID);
			BUS_SetTimer(CX_UI_UT_STARTTIMER_ID, ulTime);
		}
		else	// bUserTextTime �� False�� ����̰� �� ���� immediate message�� ��Ÿ����.
		{
			// Mail Message Notify
			if (xmgr_cas_CxPopupCheckReadAllMailMessage(&bMailIndicator) == ERR_OK)
			{
				if (bMailIndicator != pstContext->bMailIndicator)
				{
					pstContext->bMailIndicator = bMailIndicator;
					if (xmgr_cas_CxUiPopUpSendMailIconMessage(pstContext, pstContext->bMailIndicator) != ERR_OK)
					{
						HxLOG_Error("\n\n");
					}
				}
			}

			if (pstMsgInfo->ut_usDuration != CX_UI_UT_MAIL_MESSAGE_VALUE)
			{
				HxLOG_Info("- MSG Type[%08x], seq[%d], Text[%s], Duration[%d], Repetition[%d], Interval[%d]\n",
					pstMsgInfo->eUiMsgType, pstMsgInfo->ut_ucSeqNo, pstMsgInfo->ut_szText, pstMsgInfo->ut_usDuration, pstMsgInfo->ut_ucRepeat, pstMsgInfo->ut_usInterval);
				// Immediate Message Process
				hError = xmgr_cas_CxUiPopUpSendTextboxMessage(pstContext, pstMsgInfo, NULL);
				hError = xmgr_cas_CxSetUserTextDisplayed(pstMsgInfo->ut_ucSeqNo, eCxUTDisplayCntOnce);
				if(hError != ERR_OK)
				{
					HxLOG_Error("xmgr_cas_CxSetUserTextDisplayed() FUNCTION ERROR!!\n");
				}
				BUS_KillTimer(CX_UI_UT_STARTTIMER_ID);
				BUS_KillTimer(CX_UI_UT_REPEATTIMER_ID);

				pstContext->stUserTextInfo.ut_ucDisplayed++;

				if(pstContext->stUserTextInfo.ut_ucDisplayed < pstContext->stUserTextInfo.ut_ucRepeat)
				{
					HxLOG_Print("Set Uset Text Timer : CX_UI_UT_REPEATTIMER_ID!!\n");
					BUS_SetTimer(CX_UI_UT_REPEATTIMER_ID, CX_USET_TEXT_WAITING_REPEAT_TIME);
				}
			}
		}
	}
	else
	{
		HxLOG_Print("On User Text Error!! \n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

STATIC HERROR xmgr_cas_CxPopupCheckReadAllMailMessage(HBOOL *pbShow)
{
	HERROR				hError = ERR_FAIL;
	HINT32 				i;
	HUINT32 			ulCnt = 0, ulNumMsg;
	CxUserTextList_t 	pstList;
	UNIXTIME			utCurrUnixTime;
	UNIXTIME			utMailUnixtime;

	HxLOG_Info("xmgr_cas_CxPopupCheckReadAllMailMessage() +\n");
	*pbShow = FALSE;

	if(VK_CLOCK_GetTime(&utCurrUnixTime) != VK_OK)
	{
		utCurrUnixTime = 0;
		BUS_KillTimer(CX_UI_UT_MAIL_INDICATOR_ID);
		BUS_SetTimer(CX_UI_UT_MAIL_INDICATOR_ID, CX_USER_TEXT_MAIL_INDICATOR_TIME);
		return ERR_FAIL;
	}
	else
	{
		BUS_KillTimer(CX_UI_UT_MAIL_INDICATOR_ID);
	}

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i = 0 ; i < ulNumMsg ; i++)
	{
		if (pstList.pstUserText[i].usDuration == 0)
		{
			if ((pstList.pstUserText[i].ucDisplayed == 0) && (HxSTD_StrLen(pstList.pstUserText[i].szText) > 0))
			{
				HxLOG_Print("utCurrUnixTime : 0x%08x\r\n", utCurrUnixTime);
				HxLOG_Print("utMailUnixtime : 0x%08x\r\n", utMailUnixtime);
				HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstList.pstUserText[i].stStartDataTime, &utMailUnixtime);
				if (utCurrUnixTime >= utMailUnixtime)
				{
					ulCnt++;
				}
			}
		}
	}

	HxLOG_Print("Not read Mail Count = %d\n", ulCnt);
	if (ulCnt > 0)
	{
		*pbShow = TRUE;
	}

	return ERR_OK;
}

#endif

#define _______Finger_Print____________________________________
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
STATIC	HERROR		xmgr_cas_CxUiPopUpCalculateFingerPrintTime(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstCxUiInfo, HBOOL *pbIsScheduledFp, HULONG *pulTime)
{
	HINT32			nDiffDisplayTime = 0;
	HUINT32			ulError;
	UNIXTIME		ulCurrentTime = 0, ulFPUnixTime=0;
	HxDATETIME_t	stFPDateTime;
	HUINT8			*pucFPTime;

	HxLOG_Info("[ENTER]\r\n");

	*pbIsScheduledFp = FALSE;
	*pulTime = 0;

	// Ui Data
	if (pstCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	/* display immediately */
	if (pstCxUiInfo->fp_ulTime == CX_FINGER_PRINT_IMMEDIATELY_TIME)
	{
		*pbIsScheduledFp = FALSE;
		HxLOG_Info(" - Immediately fingerprint!! \n");
		return	ERR_OK;
	}

	// Get Current Time
	ulError = VK_CLOCK_GetTime((HULONG*)&ulCurrentTime);
	if (ulError != VK_OK)
	{
		HxLOG_Critical("\n\n");
		pstContext->stFingerPrintInfo.eUiMsgType = pstCxUiInfo->eUiMsgType;
		pstContext->stFingerPrintInfo.fp_ulTime = pstCxUiInfo->fp_ulTime;
		HxSTD_memcpy(pstContext->stFingerPrintInfo.fp_ucCentTime, pstCxUiInfo->fp_ucCentTime, sizeof(pstCxUiInfo->fp_ucCentTime));
		pstContext->stFingerPrintInfo.fp_usDuration = pstCxUiInfo->fp_usDuration;
		pstContext->stFingerPrintInfo.fp_usPosX = pstCxUiInfo->fp_usPosX;
		pstContext->stFingerPrintInfo.fp_usPosY = pstCxUiInfo->fp_usPosY;
		pstContext->stFingerPrintInfo.fp_usHeight = pstCxUiInfo->fp_usHeight;
		HxSTD_memcpy(pstContext->stFingerPrintInfo.fp_szText, pstCxUiInfo->fp_szText, sizeof(pstCxUiInfo->fp_szText));
		pstContext->stFingerPrintInfo.fp_ucDisplayed = pstCxUiInfo->fp_ucDisplayed;
		pstContext->stFingerPrintInfo.fp_bFPFromProtectedData = pstCxUiInfo->fp_bFPFromProtectedData;

		BUS_KillTimer(CX_UI_FP_GETTINGSITDTTIMER_ID);
		BUS_SetTimer(CX_UI_FP_GETTINGSITDTTIMER_ID, CX_USET_TEXT_WAITING_TDT_TIME);
		return	ERR_FAIL;
	}

	pucFPTime = pstCxUiInfo->fp_ucCentTime;
	if(pucFPTime == NULL)
	{
		HxLOG_Error("FP Time is null....\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stFPDateTime, 0, sizeof(HxDATETIME_t));

	// FP�� ����� ������ current�� �޴´�.
	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stFPDateTime);

	// �ð������� ���� �����ͷ� set
	stFPDateTime.stTime.ucHour = pucFPTime[0];
	stFPDateTime.stTime.ucMinute = pucFPTime[1];
	stFPDateTime.stTime.ucSecond = pucFPTime[2];
	stFPDateTime.stTime.usMillisecond = (HUINT16)pucFPTime[3];

	HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&stFPDateTime, &ulFPUnixTime);

	HxLOG_Info(" == Fingerprint Time Year(%d), Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d)\n",
		stFPDateTime.stDate.usYear, stFPDateTime.stDate.ucMonth, stFPDateTime.stDate.ucDay,
		stFPDateTime.stTime.ucHour, stFPDateTime.stTime.ucMinute, stFPDateTime.stTime.ucSecond);

	// �̷��� �ð�
	if (ulCurrentTime < ulFPUnixTime)
	{
		nDiffDisplayTime = ulFPUnixTime - ulCurrentTime;
		*pulTime = (nDiffDisplayTime*CAS_CX_SECONDS);
		*pbIsScheduledFp = TRUE;

		HxLOG_Info("- Current Time : %d, Finger Time : %d, Diff : %d\n", ulCurrentTime, ulFPUnixTime, (ulFPUnixTime - ulCurrentTime));
		HxLOG_Info("- FingerPrintDisplay Timer %d\n", nDiffDisplayTime);
	}
	else if(ulCurrentTime == ulFPUnixTime)
	{
		// ��� ���
		HxLOG_Info("- Current Time : %d, Finger Time : %d, Diff : %d\n", ulCurrentTime, ulFPUnixTime, (ulFPUnixTime - ulCurrentTime));
		*pbIsScheduledFp = FALSE;
	}
	else // the Finger Print message is for past or current...
	{
		HxLOG_Info("This Fingerprint will print one day later...\n");
		ulFPUnixTime = ulFPUnixTime + CX_UI_SECONDS_OF_DAY;
		nDiffDisplayTime = ulFPUnixTime - ulCurrentTime;
		*pulTime = (nDiffDisplayTime*CAS_CX_SECONDS);
		*pbIsScheduledFp = TRUE;
		HxLOG_Info("- Current Time : %d, Finger Time : %d, Diff : %d\n", ulCurrentTime, ulFPUnixTime, (ulFPUnixTime - ulCurrentTime));
	}

	HxLOG_Info("- FingerPrintDisplay X:%d, Y:%d - Msg %s\n", pstCxUiInfo->fp_usPosX, pstCxUiInfo->fp_usPosY, pstCxUiInfo->fp_szText);

	HxLOG_Info("[EXIT]\r\n");

	return	ERR_OK;
}

STATIC HERROR	xmgr_cas_CxUiPopUpProcessFingerPrint(XmgrCas_CxUiPopUpContext_t *pstContext, CxUiMsgInfo_t* pstMsgInfo, HUINT32 unTimerId)
{
	HBOOL		bIsScheduledFp;
	HERROR		hError = ERR_OK;
	HULONG		ulTime = 0;

	hError = xmgr_cas_CxUiPopUpCalculateFingerPrintTime(pstContext, pstMsgInfo, &bIsScheduledFp, &ulTime);

	if (hError == ERR_OK)
	{
		/* MSG_CREATE �� ��쿡�� parameter���� �Ѿ�� structure�� static variable�� ���� */
		// if (p3 == CX_UI_FP_FPPROC_CREATE)
		if (unTimerId != CX_UI_FP_STARTTIMER_ID)
		{
			if((pstContext->stFingerPrintInfo.fp_bFPFromProtectedData == TRUE) && (pstMsgInfo->fp_bFPFromProtectedData == FALSE))
			{
				HxLOG_Error("Finger Print Error... Check it...\n");
				return ERR_FAIL;
			}

			pstContext->stFingerPrintInfo.eUiMsgType = pstMsgInfo->eUiMsgType;
			pstContext->stFingerPrintInfo.fp_ulTime = pstMsgInfo->fp_ulTime;
			HxSTD_memcpy(pstContext->stFingerPrintInfo.fp_ucCentTime, pstMsgInfo->fp_ucCentTime, sizeof(pstMsgInfo->fp_ucCentTime));
			pstContext->stFingerPrintInfo.fp_usDuration = pstMsgInfo->fp_usDuration;
			pstContext->stFingerPrintInfo.fp_usPosX = pstMsgInfo->fp_usPosX;
			pstContext->stFingerPrintInfo.fp_usPosY = pstMsgInfo->fp_usPosY;
			pstContext->stFingerPrintInfo.fp_usHeight = pstMsgInfo->fp_usHeight;
			HxSTD_memcpy(pstContext->stFingerPrintInfo.fp_szText, pstMsgInfo->fp_szText, sizeof(pstMsgInfo->fp_szText));
			pstContext->stFingerPrintInfo.fp_ucDisplayed = pstMsgInfo->fp_ucDisplayed;
			pstContext->stFingerPrintInfo.fp_bFPFromProtectedData = pstMsgInfo->fp_bFPFromProtectedData;
		}

		if (bIsScheduledFp == TRUE)
		{
			HxLOG_Print("Set fingerPint Timer : CAS_CX_UI_FPTIMER_ID!! Time(%ld)\n", ulTime);
			BUS_KillTimer(CX_UI_FP_STARTTIMER_ID);
			BUS_KillTimer(CX_UI_FP_GETTINGSITDTTIMER_ID);
			BUS_SetTimer(CX_UI_FP_STARTTIMER_ID, ulTime);
		}
		else	// ���� or ���
		{
			hError = xmgr_cas_CxUiPopUpSendTextboxMessage(pstContext, pstMsgInfo, NULL);

			if (pstMsgInfo->fp_ulTime != CX_FINGER_PRINT_IMMEDIATELY_TIME)
			{
				// ���� ����� FP�� schedule�� ���� ��µ� ���̸� ȭ�鿡 �ѷ���ٴ� ������ �Ʒ��� �˷���� �Ѵ�.
				hError = xmgr_cas_CxSetFingerPrintDisplayed();
				if(hError != ERR_OK)
				{
					HxLOG_Error("xmgr_cas_CxSetFingerPrintDisplayed() FUNCTION ERROR!!\n");
				}
			}

			BUS_KillTimer(CX_UI_FP_STARTTIMER_ID);
			BUS_KillTimer(CX_UI_FP_GETTINGSITDTTIMER_ID);
			BUS_KillTimer(CX_UI_FP_DURATIONTIMER_ID);
			BUS_SetTimer(CX_UI_FP_DURATIONTIMER_ID, pstMsgInfo->fp_usDuration*10);	// fp_usDuration�� centi-second�̱� ������ 10�� ���Ͽ� milli-second�� timer ����.
		}
	}
	else
	{
		HxLOG_Error("On fingerPint Error! \n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}
#endif

BUS_Result_t xproc_cas_CxUiPopUp(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				hBusResult = MESSAGE_PASS;
	XmgrCas_CxUiPopUpContext_t	*pstContext = xmgr_cas_CxUiPopUpGetContext();
	OxMgrCasSessionEvent_t		*pstSessionEvent = NULL;
	HUINT32						ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR						hErr = ERR_FAIL;
	HBOOL						bMailIndicator = FALSE;

	if(ulActionId < NUM_VIEW_ACTION)
	{
		// View �� ���� ���븸 popup UI�� ó���ϰ� �� �̿ܿ� Recording���� �߻��ϴ� Conax PopUp UI event�� �����ϵ��� �Ѵ�.
	}
	else
	{
		return ERR_BUS_MESSAGE_PASS;
	}

#if defined(CONFIG_DEBUG)
{
	HCHAR *pszMessage = xmgr_cas_CxMessageDebugString(message);
	if(pszMessage != NULL)
	{
		HxLOG_Print("message(%s), hAction(0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", pszMessage, hAction, p1, p2, p3);
	}
}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("Message : eMEVT_BUS_CREATE Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);

			xmgr_cas_CxUiPopUpInitContext(pstContext, message, hAction, p1, p2, p3);

			hBusResult = MESSAGE_BREAK;
			break;

		/**
		  * 		MMI Data message
		**/
		case eMEVT_CAS_SC_INSERTED:
			HxLOG_Info("eMEVT_CAS_SC_INSERTED...\n");
			hErr = xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
			if((p2 & 0xff00) == eDxCAS_GROUPID_CX)
			{
				if(xmgr_cas_CxGetRunningInStandby() != TRUE)
				{
					CxScReset_Reason_t eResetReason;

					xmgr_cas_CxGetScResetReason(p1, &eResetReason);
					if(eResetReason == eScReset_By_ISR)
					{
						if(pstContext->eSessionStatus == eCX_UI_SESSION_STATUS_Started && pstContext->bStandby == FALSE)
						{
							// UI Session�� ���� �־�� Cached PIN â�� ��� �� �� �ִ�.
							hErr = xmgr_cas_CxUiPopUpOperateCachedPin(pstContext);
							if(hErr != ERR_OK)
							{
								HxLOG_Error("\n\n");
							}
						}
					}
				}
			}
#endif
			break;

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
		case eMEVT_BUS_GO_OPERATION:
			pstContext->bStandby = FALSE;
			HxLOG_Info("eMEVT_BUS_GO_OPERATION...\n");
			if(pstContext->eSessionStatus == eCX_UI_SESSION_STATUS_Started)
			{
				hErr = xmgr_cas_CxUiPopUpOperateCachedPin(pstContext);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n\n");
				}

				if (xmgr_cas_CxPopupCheckReadAllMailMessage(&bMailIndicator) == ERR_OK)
				{
					if (bMailIndicator != pstContext->bMailIndicator)
					{
						pstContext->bMailIndicator = bMailIndicator;
						if (xmgr_cas_CxUiPopUpSendMailIconMessage(pstContext, pstContext->bMailIndicator) != ERR_OK)
						{
							HxLOG_Error("\n\n");
						}
					}
				}
			}
			break;
#endif

		case eMEVT_CAS_SC_REMOVED:
			HxLOG_Info("eMEVT_CAS_SC_REMOVED...\n");
			pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
#if defined(CONFIG_SUPPORT_FCC)
{
			HUINT32 i = 0;
			for(i=0; i<NUM_VIEW_ACTION; i++)
			{
				// No Card�� ���� UI �� Card�� ������ �� UI�� ���� ������ ������ �ʰ� �ش� FCC Service�� Main view�� ���� �Ǿ��� �� No Card UI�� ����ؾ� �Ѵ�.
				// Card Problem(CommFail)�� �Ͼ���� svc cas mgr�ʿ��� eSEVT_CAS_SC_REMOVED �̺�Ʈ�� �ø��� �ִ�.
				// �Ʒ��ʿ��� �̷��� ��� �ش� event�� �ٺ������� �ø��� �ʾƾ� ������ ������ ���� �ʾ� �ϴ� ����ó�� �ϵ��� �Ѵ�.
				// ���Ŀ� �ش� ������ ���� �Ǹ� if(pstContext->astFccMsgInfo[i].eUiMsgType != eCxUi_GroupSc_CommFail) �κ��� �����ϵ��� �Ѵ�.
				if(pstContext->astFccMsgInfo[i].eUiMsgType != eCxUi_GroupSc_NoCard && pstContext->astFccMsgInfo[i].eUiMsgType != eCxUi_GroupSc_CommFail)
				{
					HxSTD_MemSet(&pstContext->astFccMsgInfo[i], 0x00, sizeof(CxUiMsgInfo_t));
					pstContext->astFccMsgInfo[i].eUiMsgType = eCxUi_GroupAll_None;
				}
			}
}
#endif
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("Slot Num: [%d]...\n", pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				hBusResult = xmgr_cas_CxUiPopUpOpenSession(pstContext, message, hAction, p1, p2, p3);
			}
#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
			hErr = xmgr_cas_CxUiPopUpOperateCachedPin(pstContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
#endif

			if (xmgr_cas_CxPopupCheckReadAllMailMessage(&bMailIndicator) == ERR_OK)
			{
				if (bMailIndicator != pstContext->bMailIndicator)
				{
					pstContext->bMailIndicator = bMailIndicator;
					if (xmgr_cas_CxUiPopUpSendMailIconMessage(pstContext, pstContext->bMailIndicator) != ERR_OK)
					{
						HxLOG_Error("\n\n");
					}
				}
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			// not use yet...
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			hBusResult = xmgr_cas_CxUiPopUpInputNotify(pstContext, message, hAction, p1, p2, p3);
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			break;

		case eMEVT_CAS_OK:
#if defined(CONFIG_SUPPORT_FCC)
			if(pstContext->astFccMsgInfo[ulActionId].eUiMsgType != eCxUi_GroupAll_None)
			{
				HxLOG_Info("Stored UI is not effective anymore...\n");
				pstContext->astFccMsgInfo[ulActionId].eUiMsgType = eCxUi_GroupAll_None;
			}

			if(ulActionId != MGR_ACTION_GetMainActionId())
			{
				return ERR_BUS_MESSAGE_BREAK;
			}
#endif
			HxLOG_Info("eMEVT_CAS_OK...\n");

			if(pstContext->stOldMsgInfo.eUiMsgType == eCxUi_GroupPpv_TokenPpv
				|| pstContext->stOldMsgInfo.eUiMsgType == eCxUi_GroupPpv_AcceptViewing
				|| pstContext->stOldMsgInfo.eUiMsgType == eCxUi_GroupMr_Ask
				|| pstContext->stOldMsgInfo.eUiMsgType == eCxUi_GroupBasic_NoAccess)
			{
				hErr = xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n\n");
				}

				pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
			}
			break;

#if defined(CONFIG_SUPPORT_FCC)
		case eSEVT_SI_STOPPED:	// Signal�� ���̻� ���� �� FCC������ �����س��� UI �̹��� ����� ���� �ʾƵ� �Ǿ� UI ������ ����
			if(ulActionId != MGR_ACTION_GetMainActionId())
			{
				if(pstContext->astFccMsgInfo[ulActionId].eUiMsgType != eCxUi_GroupAll_None)
				{
					HxLOG_Info("Stored UI is not effective anymore...\n");
					pstContext->astFccMsgInfo[ulActionId].eUiMsgType = eCxUi_GroupAll_None;
				}
			}
			break;

	#if 0
		case eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION:
		{
			HUINT32		ulOldMainActionId = (HUINT32)p1;

			HxLOG_Info("eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION...\n");
			HxLOG_Info("new Main ActionID:[%d], old main ActionID:[%d]...\n", ulActionId, ulOldMainActionId);
			HxLOG_Info("UI MSG Type that FCC service contained: [%08x]...\n", pstContext->astFccMsgInfo[ulActionId].eUiMsgType);

			pstContext->astFccMsgInfo[ulOldMainActionId].eUiMsgType = eCxUi_GroupAll_None;
			xmgr_cas_CxUiPopUpSendHideMessage(pstContext);

			if(pstContext->astFccMsgInfo[ulActionId].eUiMsgType != eCxUi_GroupAll_None)
			{
				xmgr_cas_CxUiPopUpMsgEvtCasAlarm(pstContext, message, (HINT32)NULL, p1, (HINT32)&pstContext->astFccMsgInfo[ulActionId], p3);
			}
			HxSTD_memcpy(&pstContext->stOldMsgInfo, &pstContext->astFccMsgInfo[ulActionId], sizeof(CxUiMsgInfo_t));
			pstContext->astFccMsgInfo[ulActionId].eUiMsgType = eCxUi_GroupAll_None;

			break;
		}
	#endif
#endif
		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
			pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
			xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
			// TODO: �Ʒ� �������� ���� cached pin � ������ �޴µ� ���� �� �ҷ��� UI�� ���� ������ �ְ�...
#else
			pstContext->stOldMsgInfo.eUiMsgType = eCxUi_GroupAll_None;
			xmgr_cas_CxUiPopUpSendHideMessage(pstContext);
	#if defined(CONFIG_SUPPORT_FCC)
			if(pstContext->astFccMsgInfo[ulActionId].eUiMsgType != eCxUi_GroupAll_None)
			{
				xmgr_cas_CxUiPopUpMsgEvtCasAlarm(pstContext, message, (HINT32)NULL, p1, (HINT32)&pstContext->astFccMsgInfo[ulActionId], p3);
			}
			HxSTD_memcpy(&pstContext->stOldMsgInfo, &pstContext->astFccMsgInfo[ulActionId], sizeof(CxUiMsgInfo_t));
	#endif
#endif
			break;

		case eMEVT_CAS_FAIL :
		case eMEVT_CAS_CMD :
		case eMEVT_CAS_CHECK :
		{
			CxUiMsgInfo_t*	pstMsgInfo = (CxUiMsgInfo_t*)p2;
			HBOOL			bIsSameMsg = FALSE;
#if defined(CONFIG_SUPPORT_FCC)
			HUINT32 		ulMainActionId = MGR_ACTION_GetMainActionId();

			if(hAction != (HINT32)HANDLE_NULL)
			{
				// hAction �� HANDLE_NULL ���� FCC service���� ����ִ� UI�� �׷��ִ� ���� ������ �׷��� �Ǵ� data �̴�.

				if(ulActionId != ulMainActionId)
				{
					HxLOG_Info("MSG is for FCC svc(%d)... [%08x]\n", ulActionId, pstMsgInfo->eUiMsgType);
					HxSTD_memcpy(&pstContext->astFccMsgInfo[ulActionId], pstMsgInfo, sizeof(CxUiMsgInfo_t));
					return ERR_BUS_MESSAGE_BREAK;
				}
			}
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			if(pstMsgInfo->eUiMsgType == eCxUi_GroupOther_Fingerprint)
			{
				// finger print�� priority ��� ���� �ٸ� UI�� �Բ� ��� �Ǿ�� �Ѵ�.
				hErr = xmgr_cas_CxUiPopUpProcessFingerPrint(pstContext, pstMsgInfo, 0);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n\n");
				}
				break;
			}
#endif
			HxLOG_Info("MSG(%d)... [%08x]\n", ulActionId, pstMsgInfo->eUiMsgType);
			if((pstMsgInfo->eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic)
			{
				// No Access ���� message�� priority�� ���� ���⶧���� �ٸ� UI�� ��µǾ� �ִ� ���¸� No Access UI Event�� �ø��� �ʴ´�.
				switch(pstContext->stOldMsgInfo.eUiMsgType)
				{
					/*
						comment ó���� ������ service ������ UI�� �߻��ϴ� No Access ��� �縳�� �� �� ���� ������ �Ǿ�� �ϴµ�..
						PPV�� ��� SMARTCARD���� PPV + AccessStatus (NoAccess)�� ���� �����ϳ�...
						-> �ش� case�� No Access�� �߻����� �ʵ��� ���Ƶΰ� MGR���� ������ ���ϰ� �ǵ��� �����Ͽ���.
					*/
				#if 0
					case eCxUi_GroupMr_Ask:
					case eCxUi_GroupPpv_TokenPpv:
					case eCxUi_GroupPpv_AcceptViewing:
					case eCxUi_GroupPpv_OrderInfo:
				#endif
					case eCxUi_GroupMessage_Display:
					case eCxUi_GroupOther_CachedPin:
						// �׸��� �ʴ´�.
						HxLOG_Info("Low priority UI... Do not draw...\n");
						return ERR_OK;
					default:
						break;
				}
			}

			if((pstMsgInfo->eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupBasic || (pstMsgInfo->eUiMsgType / CX_UI_UT_MSGTYPE_GAP) == eCxUi_GroupSc)
			{
				// System Message ��� UI �� �ݺ����� �÷��� ��.
				// Msg Compare ���� ����.
				HxLOG_Info("basic or sc... re-draw...\n");
				bIsSameMsg = FALSE;
			}
			else
			{
				HxLOG_Info("Old MSG... [%08x]\n", pstContext->stOldMsgInfo.eUiMsgType);
				xmgr_cas_CxUiPopUpCompareUiEvent(&pstContext->stOldMsgInfo, pstMsgInfo, &bIsSameMsg);
			}

			if(bIsSameMsg == TRUE)
			{
				HxLOG_Info("same UI... Do NOT print this Msg...\n");
				break;
			}

			xmgr_cas_CxUiPopUpMsgEvtCasAlarm(pstContext, message, hAction, p1, p2, p3);

			HxSTD_memcpy(&pstContext->stOldMsgInfo, pstMsgInfo, sizeof(CxUiMsgInfo_t));

			HxLOG_Info("Message : eMEVT_CAS_FAIL, Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", hAction, p1, p2, p3);
			break;
		}
		case eMEVT_BUS_TIMER:
			HxLOG_Info("eMEVT_BUS_TIMER... p1[%d]...\n", p1);
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
			if (p1 == CX_UI_UT_STARTTIMER_ID || p1 == CX_UI_UT_GETTINGSITDTTIMER_ID || p1 == CX_UI_UT_REPEATTIMER_ID)
			{
				hErr = xmgr_cas_CxUiPopUpProcessUserText(pstContext, &pstContext->stUserTextInfo, p1);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n\n");
				}
			}

			if (p1 == CX_UI_UT_MAIL_INDICATOR_ID)
			{
				if (xmgr_cas_CxPopupCheckReadAllMailMessage(&bMailIndicator) == ERR_OK)
				{
					if (bMailIndicator != pstContext->bMailIndicator)
					{
						pstContext->bMailIndicator = bMailIndicator;
						if (xmgr_cas_CxUiPopUpSendMailIconMessage(pstContext, pstContext->bMailIndicator) != ERR_OK)
						{
							HxLOG_Error("\n\n");
						}
					}
				}
			}
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			if (p1 == CX_UI_FP_STARTTIMER_ID || p1 == CX_UI_FP_GETTINGSITDTTIMER_ID)
			{
				hErr = xmgr_cas_CxUiPopUpProcessFingerPrint(pstContext, &pstContext->stFingerPrintInfo, p1);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n\n");
				}
			}
			else if(p1 == CX_UI_FP_DURATIONTIMER_ID)
			{
				BUS_KillTimer(CX_UI_FP_DURATIONTIMER_ID);
				HxSTD_MemSet(&pstContext->stFingerPrintInfo, 0x00, sizeof(CxUiMsgInfo_t));
			}
#endif
			break;
		case eMEVT_SCHEDULE_NEW_REC_ADDED:
			hErr = xmgr_cas_CxUiPopUpOperateCachedPin(pstContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Info("Message : eMEVT_BUS_DESTROY, Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			pstContext->bStandby = TRUE;
			return ERR_BUS_MESSAGE_PASS;

		case eSEVT_SI_TDT :
		case eSEVT_SI_TOT :
			if (xmgr_cas_CxPopupCheckReadAllMailMessage(&bMailIndicator) == ERR_OK)
			{
				if (bMailIndicator != pstContext->bMailIndicator)
				{
					pstContext->bMailIndicator = bMailIndicator;
					if (xmgr_cas_CxUiPopUpSendMailIconMessage(pstContext, pstContext->bMailIndicator) != ERR_OK)
					{
						HxLOG_Error("\n\n");
					}
				}
			}
			break;

		default:
			break;
	}

	if(hBusResult != MESSAGE_BREAK)
	{
		hBusResult = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return hBusResult;
}

#define _______DEBUG____________________________________
#if defined(CONFIG_DEBUG)
HCHAR *xmgr_cas_CxMessageDebugString(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_BUS_CREATE);
		ENUM_TO_STR(eMEVT_CAS_SC_INSERTED);
		ENUM_TO_STR(eMEVT_BUS_GO_OPERATION);
		ENUM_TO_STR(eMEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_OPEN_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_CLOSE_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_USERINPUTDATA);
		ENUM_TO_STR(eMEVT_CAS_OK);
		ENUM_TO_STR(eSEVT_SI_STOPPED);
		ENUM_TO_STR(eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_SVC_STOPPED);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_SVC_CHANGED);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_CHECK);
		ENUM_TO_STR(eMEVT_BUS_TIMER);
		ENUM_TO_STR(eMEVT_SCHEDULE_NEW_REC_ADDED);
		ENUM_TO_STR(eMEVT_BUS_DESTROY);
		ENUM_TO_STR(eMEVT_BUS_READY_SHUTDOWN);
	}

	return NULL;
}
#endif

// End of file
