/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_action.h>

#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "../local_include/_xmgr_cas_na_util.h"
#include "../local_include/_xmgr_cas_na_adapt.h"
#include "../local_include/_xmgr_cas_na.h"

#include "_svc_cas_mgr_main.h"
#include "_svc_cas_mgr_instance.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define		NA_UI_BDC_MAX_STRING_LEN		500
#define		NA_UI_BDC_TIMEOUT_10SEC			0x0000000A
#define 	NA_UI_BDC_TIMER_ID				900

#define		NA_UI_POPUP_TIMER_ID			NA_UI_BDC_TIMER_ID

#if /*defined(CONFIG_MW_CAS_NAGRA_DALTEST) && */ defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define BDCACTION_TIMER_ID					0x1001
#define BDCACTION_TIMER_TIME				300
#define BDC_STORAGE_CNT						40	// 어떤 stress test가 올지 몰라 max 40개를 잡는다.
#define ENTITLEMENT_MESSAGE_STR_CMP
#endif

typedef struct
{
	XmgrCas_NaUiPopupMode_e	ePopupMode;
	Handle_t			hSession;
	HUINT32				ulMessageBackup;
	HINT32				nTimeout;
	HUINT8				szBdcMessage[NA_UI_BDC_MAX_STRING_LEN];
} NA_UI_BDC_Context_t;

typedef struct
{
	Handle_t		hSession;
	HUINT32			ulSlotNumber;
	NaPopupItem_t	*pstPopupItem;
} sNaUi_Popup_Context_t;

typedef struct
{
	Handle_t		hSession;
	HINT32			ulMessageBackup;
	HINT32			ulCasMsgIdBackup;
	HUINT8			*pszMessage;
	BUS_Callback_t	pfnOPMessageProc;		// TODO: 이거는 별도 proc으로 분리해야 할 것 같은데...?
	HUINT32			ulStandardErrorCode;
} sNaUi_Alarm_Context_t;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
typedef struct
{
	Handle_t		hSession;
	HINT32			ulMessageBackup;
	HINT32			ulCasMsgIdBackup;
	HUINT8			*pszMessage;
#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	HUINT8			szCmpMessage[MGR_CAS_STR_LENGTH_LONG];
#endif
	HUINT32			ulUiType;
} sNaUi_Entitlement_Context_t;

#if 1//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
typedef struct
{
	HBOOL 		actived;
	NA_UI_BDC_Context_t s_stCasNaBdcContext;
	HINT32 		message;
	Handle_t 	hAction;
	HINT32 		p1;
	HINT32 		p2;
	HINT32 		p3;
} sNaUi_Bdc_CasCmdStorage_t;

STATIC sNaUi_Bdc_CasCmdStorage_t		s_stBdcCasCmdStorage[BDC_STORAGE_CNT];
STATIC sNaUi_Bdc_CasCmdStorage_t 		stCasCmdStorageTemp[BDC_STORAGE_CNT];
STATIC HBOOL							bBdcFirstCasCmdMsgFlag = FALSE;
STATIC Handle_t hSessionClose; // TODO 위 구조체와 통합해야함.
#endif
#endif

/* Application에서는 AP_MEM_Free를 사용하여야 하나  MWC_UTIL_ConvSiStringToUtf8()함수에서  SI_MEM_Alloc을 사용하여 level을 맞추려 부듯이 하게 사용함. */
#define STR_MEM_FREE	OxSI_Free

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
STATIC sNaUi_Popup_Context_t	s_stCasNaPopupContext;
#endif
STATIC NA_UI_BDC_Context_t 		s_stCasNaBdcContext;

STATIC sNaUi_Alarm_Context_t	s_stCasNaAlramContext;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
STATIC sNaUi_Entitlement_Context_t	s_stCasNaEntitlementContext;
#endif

enum
{
	eDEVICE_NONE = 0,
	eDEVICE_REMOVED,
	eDEVICE_INSERTED
};

STATIC HBOOL _xgr_cas_NaUi_MsgOtherStausCheck(Handle_t hAction, XmgrCas_NaAlamType_e *eAlarmType);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
STATIC sNaUi_Popup_Context_t*	_xmgr_cas_NaUi_Popup_GetContext(void);
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UI_LOCAL_FUNCTION______________________________________________________

#define ______________SYSTEM_MESSAGE_______________
STATIC sNaUi_Alarm_Context_t *_xmgr_cas_NaUi_GetAlramContext(void)
{
	return &s_stCasNaAlramContext;
}

STATIC HERROR _xmgr_cas_NaUi_SendBannerHideMessage(sNaUi_Alarm_Context_t *pstContext)
{
	OxMgrCasUiEvtBanner_t	*pstEvent;

	HxLOG_Debug("\n");

	if(pstContext->hSession == HANDLE_NULL)
	{
		// 앞전에서 session이 다 열려야 한다.
		HxLOG_Error("SessionHandle (0x%x)\n", pstContext->hSession);
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_BANNER;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiSysMsg;

	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessage() error\n");
		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaUi_SendBannerMessage(sNaUi_Alarm_Context_t *pstContext)
{
	HINT32	slStream = 0;
	HCHAR	*pzProperties = NULL;
	OxMgrCasUiEvtBanner_t *pstEvent = NULL;

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		goto _ERROR;
	}

	if(pstContext->pszMessage == NULL)
	{
		HxLOG_Error("pstContext->pszMessage is NULL\n");
		goto _ERROR;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		goto _ERROR;
	}

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
	{
		HxLOG_Error("slStream is 0\n");
		goto _ERROR;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HLIB_RWSTREAM_Print(slStream, "\"%s\"", pstContext->pszMessage);
#else
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HLIB_RWSTREAM_Print(slStream, "[");
	HLIB_RWSTREAM_Print(slStream, "{\"id\":\"%s\",\"in\":[%d]}", pstContext->pszMessage, pstContext->ulStandardErrorCode);
	HLIB_RWSTREAM_Print(slStream, "]");
#else
	HLIB_RWSTREAM_Print(slStream, "[");
	HLIB_RWSTREAM_Print(slStream, "{\"id\":\"%s\"}", pstContext->pszMessage);
	HLIB_RWSTREAM_Print(slStream, "]");
#endif
#endif
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_BANNER;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiSysMsg;

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG-1);
	pstEvent->stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);

	HxLOG_Debug("System Message Str = %s\n", pstEvent->stBanner.stString.szString);

	HLIB_STD_MemFree(pzProperties);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() error\n");
		goto _ERROR;
	}

	return ERR_OK;

_ERROR:
	if(pstEvent != NULL)
		HLIB_STD_MemFree(pstEvent);

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SKYD)  || defined(CONFIG_MW_CAS_NAGRA_OP_KDG)
STATIC HUINT8*	_xmgr_cas_NaUi_GetSkydMessage(XmgrCas_NaAlamType_e nAlarmType)
{
	HUINT32 nLocId;

	switch (nAlarmType)
	{
		// smartcard related msgid
#if defined(CONFIG_PROD_NAHD2000C)

		case eCAS_NA_CA_SMARTCARD_REMOVED:			nLocId = LOC_CAS_NA_KDG_SMARTCARD_REMOVED;			break;
		case eCAS_NA_CA_SMARTCARD_COM_ERROR:		nLocId = LOC_CAS_NA_KDG_SMARTCARD_COM_ERROR;		break;
		case eCAS_NA_CA_SMARTCARD_BLACKLISTED:		nLocId = LOC_CAS_NA_KDG_SMARTCARD_BLACKLISTED;		break;
		case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED: 	nLocId = LOC_CAS_NA_KDG_SMARTCARD_NEVER_PAIRED;		break;
		case eCAS_NA_CA_SMARTCARD_NOT_PAIRED:		nLocId = LOC_CAS_NA_KDG_SMARTCARD_NOT_PAIRED;		break;
		case eCAS_NA_CA_SMARTCARD_MUTE: 			nLocId = LOC_CAS_NA_KDG_SMARTCARD_MUTE;				break;
		case eCAS_NA_CA_SMARTCARD_INVALID:			nLocId = LOC_CAS_NA_KDG_SMARTCARD_INVALID;			break;
		case eCAS_NA_CA_SMARTCARD_SUSPENDED:		nLocId = LOC_CAS_NA_KDG_SMARTCARD_SUSPENDED;		break;
		case eCAS_NA_CA_SMARTCARD_EXPIRED:			nLocId = LOC_CAS_NA_KDG_SMARTCARD_EXPIRED;			break;
		case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:	nLocId = LOC_CAS_NA_KDG_SMARTCARD_NOT_CERTIFIED;	break;

#else
		case eCAS_NA_CA_SMARTCARD_REMOVED:			nLocId =STR_MESG_301_ID/*LOC_CAS_NA_SKYD_SMARTCARD_REMOVED*/; 		break;
		case eCAS_NA_CA_SMARTCARD_COM_ERROR:		nLocId = STR_MESG_279_ID/*LOC_CAS_NA_SKYD_SMARTCARD_COM_ERROR*/;		break;
		case eCAS_NA_CA_SMARTCARD_BLACKLISTED:		nLocId = STR_MESG_282_ID/*LOC_CAS_NA_SKYD_SMARTCARD_BLACKLISTED*/;		break;
		case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED:		nLocId = STR_MESG_309_ID/*LOC_CAS_NA_SKYD_SMARTCARD_NEVER_PAIRED*/;	break;
		case eCAS_NA_CA_SMARTCARD_NOT_PAIRED:		nLocId = STR_MESG_318_ID/*LOC_CAS_NA_SKYD_SMARTCARD_NOT_PAIRED*/;		break;
		case eCAS_NA_CA_SMARTCARD_MUTE:				nLocId = STR_MESG_325_ID/*LOC_CAS_NA_SKYD_SMARTCARD_MUTE*/;			break;
		case eCAS_NA_CA_SMARTCARD_INVALID:			nLocId = STR_MESG_295_ID/*LOC_CAS_NA_SKYD_SMARTCARD_INVALID*/;			break;
		case eCAS_NA_CA_SMARTCARD_SUSPENDED:		nLocId = STR_MESG_283_ID/*LOC_CAS_NA_SKYD_SMARTCARD_SUSPENDED*/;		break;
		case eCAS_NA_CA_SMARTCARD_EXPIRED:			nLocId = STR_MESG_317_ID/*LOC_CAS_NA_SKYD_SMARTCARD_EXPIRED*/;			break;
		case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:	nLocId = STR_MESG_287_ID/*LOC_CAS_NA_SKYD_SMARTCARD_NOT_CERTIFIED*/;	break;
#endif
		// access related msgid

#if defined(CONFIG_PROD_NAHD2000C)
		case eCAS_NA_CA_ACCESS_DENIED:						nLocId = LOC_CAS_NA_KDG_ACCESS_DENIED;					break;
#else
		case eCAS_NA_CA_ACCESS_DENIED:						nLocId = STR_MESG_333_ID/*LOC_CAS_NA_SKYD_ACCESS_DENIED*/;					break;
#endif
		case eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD :			nLocId = STR_MESG_1760_ID/*LOC_CAS_NA_ACCESS_NO_VALID_SMARTCARD*/;			break;
		case eCAS_NA_CA_ACCESS_BLACKOUT :					nLocId = STR_ACCESS_BLACKOUT_ID/*LOC_CAS_NA_ACCESS_BLACKOUT*/;					break;
		case eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT :		nLocId = STR_MESG_1761_ID/*LOC_CAS_NA_ACCESS_DENIED_NO_VALID_CREDIT*/;		break;
		case eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED :		nLocId = STR_MESG_1762_ID/*LOC_CAS_NA_ACCESS_DENIED_COPY_PROTECTED*/;		break;
		case eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED :		nLocId = STR_MESG_1763_ID/*LOC_CAS_NA_ACCESS_DENIED_DIALOG_REQUIRED*/;		break;
		//UX DIC 추가 필요
		//case eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED :	nLocId = LOC_CAS_NA_ACCESS_DENIED_PAIRING_REQUIRED;		break;
		case eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED :	nLocId = STR_MESG_1765_ID/*LOC_CAS_NA_ACCESS_DENIED_CS_PAIRING_REQUIRED*/;	break;
		default :											nLocId = STR_UNKNOWN_ERROR_ID/*LOC_RESPONSE_MSG_ERR_UNKNOWN_ID*/;				break;
	}

	// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
	// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.

	// TODO: string을 web에서 처리하는 작업 필요
	return (HUINT8 *)NULL; //AP_CasResStr_GetStrRsc(nLocId);
}
#else

STATIC HUINT8*	_xmgr_cas_NaUi_GetCommonMessage(XmgrCas_NaAlamType_e nAlarmType)
{
#if 0
	HUINT32 nLocId;

	switch (nAlarmType)
	{
		// smartcard related msgid
		case eCAS_NA_CA_SMARTCARD_REMOVED:			nLocId = STR_MESG_212_ID/*LOC_CAS_NA_SMARTCARD_REMOVED*/; 			break;
		case eCAS_NA_CA_SMARTCARD_COM_ERROR:		nLocId = STR_SMARTCARD_COMMUNICATION_ERROR_ID/*LOC_CAS_NA_SMARTCARD_COM_ERROR*/;		break;
		case eCAS_NA_CA_SMARTCARD_BLACKLISTED:		nLocId = STR_MESG_996_ID/*LOC_CAS_NA_SMARTCARD_BLACKLISTED*/;		break;
		case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED:		nLocId = STR_MESG_1000_ID/*LOC_CAS_NA_SMARTCARD_NEVER_PAIRED*/;		break;
		case eCAS_NA_CA_SMARTCARD_NOT_PAIRED:		nLocId = STR_MESG_1001_ID/*LOC_CAS_NA_SMARTCARD_NOT_PAIRED*/;		break;
		case eCAS_NA_CA_SMARTCARD_MUTE:				nLocId = STR_MESG_994_ID/*LOC_CAS_NA_SMARTCARD_MUTE*/;				break;
		case eCAS_NA_CA_SMARTCARD_INVALID:			nLocId = STR_MESG_995_ID/*LOC_CAS_NA_SMARTCARD_INVALID*/;			break;
		case eCAS_NA_CA_SMARTCARD_SUSPENDED:		nLocId = STR_MESG_997_ID/*LOC_CAS_NA_SMARTCARD_SUSPENDED*/;		break;
		case eCAS_NA_CA_SMARTCARD_EXPIRED:			nLocId = STR_MESG_998_ID/*LOC_CAS_NA_SMARTCARD_EXPIRED*/;			break;
		case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:	nLocId = STR_MESG_999_ID/*LOC_CAS_NA_SMARTCARD_NOT_CERTIFIED*/;	break;
		// access related msgid
		case eCAS_NA_CA_ACCESS_DENIED:						nLocId = STR_MESG_1002_ID/*LOC_CAS_NA_ACCESS_DENIED*/;						break;
		case eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD :			nLocId = STR_MESG_1760_ID/*LOC_CAS_NA_ACCESS_NO_VALID_SMARTCARD*/;			break;
		case eCAS_NA_CA_ACCESS_BLACKOUT :					nLocId = STR_ACCESS_BLACKOUT_ID/*LOC_CAS_NA_ACCESS_BLACKOUT*/;					break;
		case eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT :		nLocId = STR_MESG_1761_ID/*LOC_CAS_NA_ACCESS_DENIED_NO_VALID_CREDIT*/;		break;
		case eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED :		nLocId = STR_MESG_1762_ID/*LOC_CAS_NA_ACCESS_DENIED_COPY_PROTECTED*/;		break;
		case eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED :		nLocId = STR_MESG_1763_ID/*LOC_CAS_NA_ACCESS_DENIED_DIALOG_REQUIRED*/;		break;
		//UX DIC 추가 필요
		//case eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED :	nLocId = LOC_CAS_NA_ACCESS_DENIED_PAIRING_REQUIRED;		break;
		case eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED :	nLocId = STR_MESG_1765_ID/*LOC_CAS_NA_ACCESS_DENIED_CS_PAIRING_REQUIRED*/;	break;
		default :											nLocId = STR_UNKNOWN_ERROR_ID/*LOC_RESPONSE_MSG_ERR_UNKNOWN_ID*/;				break;
	}
	return (HUINT8 *)AP_CasResStr_GetStrRsc(nLocId);
#else
	HCHAR *pszLocId = NULL;

	switch (nAlarmType)
	{
		// smartcard related msgid
		case eCAS_NA_CA_SMARTCARD_REMOVED:					pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_REMOVED_ID"; 					break;
		case eCAS_NA_CA_SMARTCARD_COM_ERROR:				pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_COM_ERROR_ID";					break;
		case eCAS_NA_CA_SMARTCARD_BLACKLISTED:				pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_BLACKLISTED_ID";				break;
		case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED:				pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_NEVER_PAIRED_ID";				break;
		case eCAS_NA_CA_SMARTCARD_NOT_PAIRED:				pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_NOT_PAIRED_ID";				break;
		case eCAS_NA_CA_SMARTCARD_MUTE:						pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_MUTE_ID";						break;
		case eCAS_NA_CA_SMARTCARD_INVALID:					pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_INVALID_ID";					break;
		case eCAS_NA_CA_SMARTCARD_SUSPENDED:				pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_SUSPENDED_ID";					break;
		case eCAS_NA_CA_SMARTCARD_EXPIRED:					pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_EXPIRED_ID";					break;
		case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:			pszLocId = "LOC_CAS_NA_MSG_SMARTCARD_NOT_CERTIFIED_ID";				break;
		// access related msgid
		case eCAS_NA_CA_ACCESS_DENIED:						pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_ID";						break;
		case eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD :			pszLocId = "LOC_CAS_NA_MSG_ACCESS_NO_VALID_SMARTCARD_ID";			break;
		case eCAS_NA_CA_ACCESS_BLACKOUT :					pszLocId = "LOC_CAS_NA_MSG_ACCESS_BLACKOUT_ID";						break;
		case eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT :		pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_NO_VALID_CREDIT_ID";		break;
		case eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED :		pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_COPY_PROTECTED_ID";		break;
		case eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED :		pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_DIALOG_REQUIRED_ID";		break;
		case eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED :	pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_PAIRING_REQUIRED_ID";		break;
		case eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED :	pszLocId = "LOC_CAS_NA_MSG_ACCESS_DENIED_CS_PAIRING_REQUIRED_ID";	break;
		default :											/*pszLocId = "LOC_CAS_NA_MSG_ERR_UNKNOWN_ID";*/						break;
	}

	return pszLocId;
#endif
}
#endif

STATIC HUINT8*	_xmgr_cas_NaUi_GetMessage(XmgrCas_NaAlamType_e nAlarmType)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	return xmgr_cas_NaGetMessage_Aps ((HINT32)nAlarmType);
#elif defined(CONFIG_MW_CAS_NAGRA_OP_SKYD)  || defined(CONFIG_MW_CAS_NAGRA_OP_KDG)
	return _xmgr_cas_NaUi_GetSkydMessage (nAlarmType);
#else
	return _xmgr_cas_NaUi_GetCommonMessage (nAlarmType);
#endif
}

STATIC HERROR	_xmgr_cas_NaUi_CheckMSGProcess(Handle_t hAction, HINT32 pData)
{
	HERROR				nErrorReturn = ERR_FAIL;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	nErrorReturn = xmgr_cas_NaCheckMsgProcessing_Aps(hAction, (void*)pData);
#endif

	if (nErrorReturn != ERR_OK)
	{
		return ERR_FAIL;
	}

	return nErrorReturn;
}

STATIC HERROR	_xmgr_cas_NaUi_AlarmMSGProcess(HINT32 pData, XmgrCas_NaAlamType_e *peCasAlarmType)
{
	if(xmgr_cas_NaAlarmDataProcessing((void*)pData, peCasAlarmType) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaAlarmDataProcessing error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgEvtCasUpdateAlarm(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaAlamType_e eCasAlarmType;

	eCasAlarmType = pstContext->ulCasMsgIdBackup;
	pstContext->pszMessage = _xmgr_cas_NaUi_GetMessage(eCasAlarmType);

	HxLOG_Print("[%s:%d] Nagra System message [%s]\n", __FUNCTION__, __HxLINE__, pstContext->pszMessage);

	if(_xmgr_cas_NaUi_SendBannerMessage(pstContext) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaUi_SendBannerMessage() error\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgCreate(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HERROR						hError = ERR_OK;
//	XmgrCas_NaAlamType_e 			eCasAlarmType;

	HxLOG_Print("[%s:%d] eMEVT_BUS_CREATE! \n", __FUNCTION__, __HxLINE__);

	HxSTD_memset(pstContext, 0, sizeof(sNaUi_Alarm_Context_t));

	pstContext->hSession = HANDLE_NULL;
//	pstContext->bSessionStarted = FALSE;
#if 0 // mslee defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	pstContext->pfnOPMessageProc	= xproc_cas_NaAps;
#else
	pstContext->pfnOPMessageProc	= NULL;
#endif

#if 0
	/* UI Data를 만들어 놓기만 하고 Application으로 보내지는 않는다. Session Start message가 날라오면 저장해 둔 UI Data를 보내자. */
	hError = _xmgr_cas_NaUi_AlarmMSGProcess(p2, &eCasAlarmType);

#if !(CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	if (hError != ERR_FAIL)
#endif
	{
		pstContext->ulCasMsgIdBackup = eCasAlarmType;

		{
			pstContext->pszMessage = _xmgr_cas_NaUi_GetMessage(eCasAlarmType);
		}

		return ERR_BUS_MESSAGE_BREAK;
	}

	return ERR_BUS_MESSAGE_PASS;
#else
	return ERR_BUS_MESSAGE_BREAK;
#endif

}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgEvtCasFail(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaAlamType_e eAlarmType;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	sNaUi_Popup_Context_t	*pstContext_popup = _xmgr_cas_NaUi_Popup_GetContext();
#endif

	if (_xmgr_cas_NaUi_AlarmMSGProcess(p2, &eAlarmType) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaUi_AlarmMSGProcess() error\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(xmgr_cas_NaGetStandardErrorCode(hAction, &pstContext->ulStandardErrorCode) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaUi_AlarmMSGProcess() error\n");
		return ERR_BUS_MESSAGE_BREAK;
	}
	
	if (pstContext_popup->pstPopupItem != NULL)
	{
		// Alarm type 을 저장해 놓는다. Popup Msg 가 disapper 되었을때, 보여주어야 한다.
		pstContext->pszMessage	= _xmgr_cas_NaUi_GetMessage(eAlarmType);
		HxLOG_Error("Nagra System message [%s]\n", pstContext->pszMessage);

		return ERR_BUS_MESSAGE_BREAK;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	// 예외 상황이 발생해서 여기서 임시로 remove에 한해서 alarm을 다시 check한다.
	if(eAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED) // sc를 제거한경우에 cam을 check해서
	{
		XmgrCas_NaAlamType_e eConvertAlarmType;
		HBOOL					bConvertMsgFlag = FALSE;

		if(_xgr_cas_NaUi_MsgOtherStausCheck(hAction, &eConvertAlarmType) == TRUE)
		{
			if(eConvertAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED)
			{
				eAlarmType = eCAS_NA_CA_SMARTCARD_REMOVED;
				bConvertMsgFlag = TRUE;
			}
			else if(eConvertAlarmType == eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD)
			{
				eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
				bConvertMsgFlag = TRUE;
			}
		}

		if(bConvertMsgFlag == FALSE)
		{
			return ERR_BUS_MESSAGE_BREAK;
		}
	}
#endif

	pstContext->ulCasMsgIdBackup		= eAlarmType;
	pstContext->pszMessage	= _xmgr_cas_NaUi_GetMessage(eAlarmType);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HxLOG_Error("Nagra System message [%s]\n", pstContext->pszMessage);

	if(pstContext->hSession != HANDLE_NULL)
	{
		HxLOG_Print("pstContext->hSession != HANDLE_NULL\n");
		if(_xmgr_cas_NaUi_SendBannerMessage(pstContext) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}
	// 항상 열려있으므로 여기는 들어오지 않을거라 예상됨
	else
	{
		HUINT32 ulSlotNumber = 0;
		HxLOG_Print("==> xproc_cas_NaUiSysMsg() \n");
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_BANNER, xproc_cas_NaUiSysMsg, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
	}
#else
	_xmgr_cas_NaUi_SendBannerMessage(pstContext);
#endif

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgEvtCasUpdateCheck(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;

	if (pstContext->pfnOPMessageProc != NULL)
	{
		eBusErr = pstContext->pfnOPMessageProc(message, hAction, p1, p2, p3);
	}

	return eBusErr;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define ______________ENTITLEMENT_SYSTEM_MESSAGE_______________

STATIC sNaUi_Entitlement_Context_t *_xmgr_cas_NaUi_GetEntitlementContext(void)
{
	return &s_stCasNaEntitlementContext;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgCreate(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("\\n");
	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	HxLOG_Debug("eMEVT_BUS_CREATE! \n");
	HxSTD_memset(pstContext, 0, sizeof(sNaUi_Entitlement_Context_t));
	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_SendHideBannerMessage(sNaUi_Entitlement_Context_t *pstContext)
{
	OxMgrCasUiEvtBanner_t *pstEvent;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	HxSTD_memset(&pstContext->szCmpMessage, 0, MGR_CAS_STR_LENGTH_LONG);
#endif

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	pstEvent->stBanner.stString.szString[0] = '\0';
	pstEvent->stBanner.stString.ulStringLength = 0;

	HxLOG_Debug("Session = 0x%08x, str = %s\n",pstEvent->stSessionInfo.hSession, pstEvent->stBanner.stString.szString);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n");

		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
		return ERR_BUS_MESSAGE_PASS;
	}


	return ERR_BUS_MESSAGE_BREAK;
}

STATIC HERROR _xmgr_cas_NaUi_Entitlement_SendBannerMessage(sNaUi_Entitlement_Context_t *pstContext)
{
	HINT32	slStream;
	HCHAR	*pzProperties = NULL;
	OxMgrCasUiEvtBanner_t *pstEvent;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		return ERR_FAIL;
	}

#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	if(strncmp(&pstContext->szCmpMessage[0], pstContext->pszMessage, SvcCas_UtilStrLen(pstContext->pszMessage)) == 0)
	{
		return ERR_OK;
	}
	else
	{
		HxSTD_memset(&pstContext->szCmpMessage, 0, MGR_CAS_STR_LENGTH_LONG);
	}
#endif

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	sprintf(pstContext->szCmpMessage, "%s", pstContext->pszMessage);
#endif

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	HLIB_RWSTREAM_Print(slStream, "\"%s\"", pstContext->pszMessage);
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiEntitlementMessage;

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG-1);
	pstEvent->stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);

	HxLOG_Debug("Session = 0x%08x, str = %s\n",pstEvent->stSessionInfo.hSession, pstEvent->stBanner.stString.szString);

	HLIB_STD_MemFree(pzProperties);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n");

		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
		return ERR_FAIL;
	}


	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaUi_Entitlement_SendUserInputMessage(sNaUi_Entitlement_Context_t *pstContext)
{
	OxMgrCasUiEvtInput_t *pstEvent;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HUINT32		slStream;
	HCHAR		*pszEncodedStr, *pzProperties;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		return ERR_FAIL;
	}

#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	if(strncmp(&pstContext->szCmpMessage[0], pstContext->pszMessage, SvcCas_UtilStrLen(pstContext->pszMessage)) == 0)
	{
		return ERR_OK;
	}
	else
	{
		HxSTD_memset(&pstContext->szCmpMessage, 0, MGR_CAS_STR_LENGTH_LONG);
	}
#endif

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));

	if(pstEvent == NULL)
	{
		HxLOG_Error("[%s:%d] pstEvent is NULL\n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

#if defined(ENTITLEMENT_MESSAGE_STR_CMP)
	sprintf(pstContext->szCmpMessage, "%s", pstContext->pszMessage);
#endif

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	pszEncodedStr = xmgr_cas_NaUtil_EncodeString(pstContext->pszMessage);

	HxLOG_Debug("SysMsg = %s, pszEncodedStr = %s\n",pstContext->pszMessage, pszEncodedStr);

	if(pszEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszEncodedStr);
		HLIB_STD_MemFree(pszEncodedStr);
		pszEncodedStr = NULL;
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiEntitlementMessage;

	pstEvent->stHeader.ulAttr = eMgrCasUiAttr_Subtitle;
	pstEvent->stInputData.eInputType = eMgrCasInputType_Button;
	pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_OK;
	MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "\"ok\"", MGR_CAS_STR_LENGTH_SHORT - 1);
	MWC_UTIL_DvbStrncpy(pstEvent->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG - 1);

	HxLOG_Debug("str = %s\n",pstEvent->stHeader.szSubTitle);

	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent);

	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessage() ERROR!!\n");
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree(pstEvent);
		}
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC BUS_Result_t	_xmgr_cas_NaUi_Entitlement_MsgLiveViewState(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR					hErr;
	HUINT32 				ulSlotNumber = 0;
	MgrLiveView_UiState_e	eViewState;

	if(pstContext == NULL)
	{
		HxLOG_Error ("pstContext is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	hErr  = MGR_LIVE_GetState (ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetState Error\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	HxLOG_Info("eViewState:%d --> close session = 0x%08x\n", eViewState, pstContext->hSession);

#if 0
	// 그래서 viewState가 init상태인경우에는 msg를 보내지 않는다.
	if(eViewState == eLiveViewUi_NONE)
	{
		HxLOG_Error("[%s:%d] eViewState is eLiveViewUi_NONE -> CasCheck is not process\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}
#endif

	if(pstContext->hSession != HANDLE_NULL)
	{
		if(eViewState != eLiveViewUi_CAS)
		{
			#if 1 // 채널 전환이 fast하게 일어나면 문제가 entitlement msg가 close와 쫑이 나기 때문에 banner를 close하지 않고 hide시킴
			if(pstContext->pszMessage)
			{
				_xmgr_cas_NaUi_Entitlement_SendHideBannerMessage(pstContext);
			}
			else
			{
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			}
			#else
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			#endif
		}
		else
		{
			XmgrCas_NaAlamType_e 		eScAlarm;
			// cas channel에서 entitle msg가 display된경우 card remove시 popup을 close한다.
			// 아래함수의 return value가 ERR_OK인지 ERR_FAIL인지 의미가 없다. 왜나하면 ERR_FAIL인경우에도 eCAS_NA_CA_SMARTCARD_REMOVED가 return되어 오기때문이다.
			(void)xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
			if(eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED)
			{
				#if 0 // 채널 전환이 fast하게 일어나면 문제가 entitlement msg가 close와 쫑이 나기 때문에 banner를 close하지 않고 hide시킴
				if(pstContext->pszMessage)
				{
					_xmgr_cas_NaUi_Entitlement_SendHideBannerMessage(pstContext);
				}
				else
				{
					xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
				}
				#else
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
				#endif
			}
		}
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgEvtCasCheck(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8 					messageType;
	HUINT8 					cas_na_entitle_message[512];
	HUINT32 				ulSlotNumber = 0;
	MgrLiveView_UiState_e	eViewState;
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	if(MGR_LIVE_GetState (ulActionId, eLiveProc_View, (HUINT32 *)&eViewState) != ERR_OK)
	{
		HxLOG_Error("[%s:%d] MGR_LIVE_GetState is fail\n", __FUNCTION__, __LINE__);
	}

	HxLOG_Info("eViewState:%d --> close session = 0x%08x\n", eViewState, pstContext->hSession);

	// cas 채널에서 entitle message가 display된경우 fta로 전환하면 이전 entitle message가 다시 display되는 문제가 발생함.
	// 그래서 viewState가 init상태인경우에는 msg를 보내지 않는다.
	if(eViewState == eLiveViewUi_NONE)
	{
		HxLOG_Error("[%s:%d] eViewState is eLiveViewUi_NONE -> CasCheck is not process\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	if(_xmgr_cas_NaUi_CheckMSGProcess(hAction, p2) == ERR_OK)
	{
		pstContext->ulMessageBackup = message;

		if(xmgr_cas_NaGetMessageTypeAndString_Aps(&messageType, &(cas_na_entitle_message[0])) == ERR_OK)
		{
			pstContext->pszMessage = &(cas_na_entitle_message[0]);

			HxLOG_Debug("msgType(%d), hSession(0x%08x), msg = %s\n",messageType, pstContext->hSession, pstContext->pszMessage);

			if(messageType == NA_CAS_APS_MSG_T_BLOCK)
			{
				/*
 				1. eCAS_APS_MSG_REF140
				2. eCAS_APS_MSG_REF133
				3. eCAS_APS_MSG_REF131
				4. eCAS_APS_MSG_REF133
				*/
				if(pstContext->hSession != HANDLE_NULL)
				{
					_xmgr_cas_NaUi_Entitlement_SendBannerMessage(pstContext);
				}
				else
				{
					pstContext->ulUiType = eMEVT_CASUI_TYPE_BANNER_DATA;
					xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
				}
			}
			else if(messageType == NA_CAS_APS_MSG_T_DIALOG)
			{
				/*
				1. eCAS_APS_MSG_REF136
				2. eCAS_APS_MSG_REF134
				3. eCAS_APS_MSG_REF130A
				4. eCAS_APS_MSG_REF132A
				*/
				if(pstContext->hSession != HANDLE_NULL)
				{
					_xmgr_cas_NaUi_Entitlement_SendUserInputMessage(pstContext);
				}
				else
				{
					pstContext->ulUiType = eMEVT_CASUI_TYPE_INPUT_DATA;
					xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
				}
			}
		}
	}

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgEvtCasUpdateCheck(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8 messageType;
	HUINT8 cas_na_entitle_message[512];

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	if(xmgr_cas_NaGetMessageTypeAndString_Aps(&messageType, &(cas_na_entitle_message[0])) == ERR_OK)
	{
		pstContext->pszMessage = &(cas_na_entitle_message[0]);

		HxLOG_Debug("messageType(%d), hSession(0x%08x) cas_na_entitle_message = %s\n",messageType, pstContext->hSession,  pstContext->pszMessage);

		if(messageType == NA_CAS_APS_MSG_T_BLOCK)
		{
			if(pstContext->hSession != HANDLE_NULL)
			{
				if(_xmgr_cas_NaUi_Entitlement_SendBannerMessage(pstContext) != ERR_OK)
				{
					HxLOG_Error("[%s:%d]\n",__FUNCTION__,__LINE__);
				}
	 		}
		}
		else if(messageType == NA_CAS_APS_MSG_T_DIALOG)
		{
			if(pstContext->hSession != HANDLE_NULL)
			{
				if(_xmgr_cas_NaUi_Entitlement_SendUserInputMessage(pstContext) != ERR_OK)
				{
					HxLOG_Error("[%s:%d]\n",__FUNCTION__,__LINE__);
				}
	 		}
		}

	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgOpenSession(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	BUS_Result_t eBusRes;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	switch(pstContext->ulMessageBackup)
	{
		//case eMEVT_CAS_FAIL:
		//	eBusRes = _xmgr_cas_NaUi_MsgEvtCasUpdateAlarm(pstContext, message, hAction, p1, p2, p3);
		//	break;
		case eMEVT_CAS_CHECK:
			pstContext->hSession = pstSession->hSession;
			HxLOG_Debug("pstSession->hSession = 0x%08x\n", pstSession->hSession);
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiEntitlementMessage, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgEvtCasUpdateCheck(pstContext, message, hAction, p1, p2, p3);
			break;
		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	return eBusRes;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgCloseSession(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	pstContext->hSession = HANDLE_NULL;
	pstContext->ulUiType = eMEVT_CAS_END;

	return BUS_MGR_Destroy(NULL);
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Entitlement_MsgEvtInputNotify(sNaUi_Entitlement_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulSlotNumber = (HUINT32)p1;
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Button:
			#if 0 // 채널 전환이 fast하게 일어나면 문제가 entitlement msg가 close와 쫑이 나기 때문에 banner를 close하지 않고 hide시킴
			if(pstContext->pszMessage)
			{
				_xmgr_cas_NaUi_Entitlement_SendHideBannerMessage(pstContext);
			}
			else
			{
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eSessionType, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			}
			#else
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eSessionType, xproc_cas_NaUiEntitlementMessage, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			#endif
			xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_OK);
			break;
		default:
			HxLOG_Debug("\n");
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC HBOOL _xgr_cas_NaUi_MsgOtherStausCheck(Handle_t hAction, XmgrCas_NaAlamType_e *eAlarmType)
{
	SvcCas_Context_t		*pstCasContext = NULL;
	SvcCas_DevInstance_t	*pstCasInstance = NULL;
	HBOOL					bMsgFlag = FALSE;
	HBOOL					bEcasDevState = eDEVICE_NONE;
	HBOOL					bCamDevState = eDEVICE_NONE;
	HUINT32					ulEcasInstanceId = HANDLE_NULL;
	HUINT32					ulCamInstanceId = HANDLE_NULL;
	HUINT32 				i = 0;
	HBOOL					bSysIdFound = FALSE;

	HxLOG_Debug("\n");

	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);

	if(pstCasContext == NULL)
	{
		HxLOG_Error("pstCasContext is NULL\n");
		return FALSE;
	}

	if(pstCasContext)
	{
		HxLOG_Debug("\t + pmtupdated(%d), eChType(%d)\n", pstCasContext->bPmtUpdated, pstCasContext->eChType);
		/* It's cas channel */
		if(pstCasContext->bPmtUpdated == TRUE && pstCasContext->eChType == eCasChannel_Scramble)
		{
			for(i=0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
			{
				pstCasInstance = svc_cas_InstanceGetCasDevInst(i);
				if(pstCasInstance == NULL)
				{
					continue;
				}

				if(pstCasInstance->eCasInstType == eCasInst_Embedded)
				{
					if(svc_cas_InstanceIsAdaptedSCInserted(i, 0) == FALSE)
					{
						bEcasDevState = eDEVICE_REMOVED;
					}
					else
					{
						bEcasDevState = eDEVICE_INSERTED;
						ulEcasInstanceId = i;
					}
					HxLOG_Debug("\t + [%d] eCasInst_Embedded : ulEcasInstanceId(0x%08x), state(%s)\n", i, ulEcasInstanceId, bEcasDevState == eDEVICE_REMOVED ? "eDEVICE_REMOVED":"eDEVICE_INSERTED");
				}
				else if(pstCasInstance->eCasInstType == eCasInst_CI)
				{
					if(pstCasInstance->eCamState == eCAS_CAM_STATE_Removed)
					{
						bCamDevState = eDEVICE_REMOVED;
					}
					else
					{
						bCamDevState = eDEVICE_INSERTED;
						ulCamInstanceId = i;
					}
					HxLOG_Debug("\t + [%d] eCasInst_CI : ulEcasInstanceId(0x%08x), state(%s)\n", i, ulCamInstanceId, bCamDevState == eDEVICE_REMOVED ? "eDEVICE_REMOVED":"eDEVICE_INSERTED");
				}
			}

			HxLOG_Debug("\t + CAS(%s), CAM(%s)\n", bEcasDevState == eDEVICE_REMOVED ? "eDEVICE_REMOVED":"eDEVICE_INSERTED", bCamDevState == eDEVICE_REMOVED ? "eDEVICE_REMOVED":"eDEVICE_INSERTED");

			if(bEcasDevState == eDEVICE_REMOVED || bEcasDevState == eDEVICE_NONE)
			{
				if(bCamDevState == eDEVICE_REMOVED || bCamDevState == eDEVICE_NONE)
				{
					* eAlarmType = eCAS_NA_CA_SMARTCARD_REMOVED;	// parse_E20a
					bMsgFlag = TRUE;
					HxLOG_Debug("\t\t + Msg : 130\n");
				}
				else
				{
					bSysIdFound = svc_cas_MgrGetSystemIdFoundByInstance(pstCasContext, ulCamInstanceId);

					if(bSysIdFound != TRUE)
					{
						* eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
						bMsgFlag = TRUE;
						HxLOG_Debug("\t\t + Msg : 144\n");
					}
					else
					{
						#if 0
						HBOOL   bVideoDecoding = FALSE;
						// 어떤 연유에의해 video가 문제가 발생하면
						svc_cas_DevVideoIsRunning(0, &bVideoDecoding);
						HxLOG_Print("####### bVideoDecoding = %d\n", bVideoDecoding);
						if(bVideoDecoding == FALSE)
						{
							* eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
							bMsgFlag = TRUE;
						}
						#endif
						HxLOG_Debug("\t\t + Msg : None\n");
					}
				}
			}
			else
			{
				if(bCamDevState == eDEVICE_REMOVED || bCamDevState == eDEVICE_NONE)
				{
					bSysIdFound = svc_cas_MgrGetSystemIdFoundByInstance(pstCasContext, ulEcasInstanceId);

					if(bSysIdFound != TRUE)
					{
						* eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
						bMsgFlag = TRUE;
						HxLOG_Debug("\t\t + Msg : 144\n");
					}
					else
					{
						#if 0
						HBOOL   bVideoDecoding = FALSE;
						// 어떤 연유에의해 video가 문제가 발생하면
						svc_cas_DevVideoIsRunning(0, &bVideoDecoding);
						HxLOG_Print("####### bVideoDecoding = %d\n", bVideoDecoding);
						if(bVideoDecoding == FALSE)
						{
							* eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
							bMsgFlag = TRUE;
						}
						#endif
						HxLOG_Debug("\t\t + Msg : None\n");
					}
				}
				else
				{
					bSysIdFound = svc_cas_MgrGetSystemIdFoundByInstance(pstCasContext, ulEcasInstanceId);

					if(bSysIdFound != TRUE)
					{
						* eAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
						bMsgFlag = TRUE;
						HxLOG_Debug("\t\t + Msg : 144\n");
					}
					else
					{
						HxLOG_Debug("\t\t + Msg : None\n");
					}
				}
			}
		}
	}

	return bMsgFlag;
}

STATIC BUS_Result_t	_xmgr_cas_NaUi_MsgCamStatus(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaAlamType_e eAlarmType;
	HBOOL					bMsgFlag = FALSE;

	HxLOG_Debug("message(%s)\n", message == eMEVT_CAS_CAM_REMOVED?"eMEVT_CAS_CAM_REMOVED":"None");

	if(message == eMEVT_CAS_CAM_REMOVED)
	{
		if(_xgr_cas_NaUi_MsgOtherStausCheck(hAction, &eAlarmType) == TRUE)
		{
			if(eAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED)
			{
				pstContext->pszMessage = "parse_E20a";
				bMsgFlag = TRUE;
			}
			else if(eAlarmType == eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD)
			{
				pstContext->pszMessage = "parse_E44";
				bMsgFlag = TRUE;
			}
		}
	}

	if(bMsgFlag == TRUE)
	{
		_xmgr_cas_NaUi_SendBannerMessage(pstContext);
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_PASS;
}

#endif


STATIC BUS_Result_t	_xmgr_cas_NaUi_MsgLiveViewState(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR					hErr;
	MgrLiveView_UiState_e		 eViewState;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HBOOL					bMsgFlag = FALSE;
#endif

	hErr  = MGR_LIVE_GetState (ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetState Error\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	HxLOG_Debug("eViewState:%d \n", eViewState);

	if(eViewState != eLiveViewUi_CAS)
	{
		_xmgr_cas_NaUi_SendBannerHideMessage(pstContext);
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(eViewState == eLiveViewUi_NO_SIGNAL)
	{
		pstContext->pszMessage = "parse_E1a"; // no signal인경우 e1a를 보낸다.
		bMsgFlag= TRUE;
	}
	else if(eViewState == eLiveViewUi_NO_SIGNAL_TIMEOUT)
	{
		pstContext->pszMessage = "parse_E1a"; // no signal인경우 e1a를 보낸다.
		bMsgFlag= TRUE;
	}
	else if(eViewState == eLiveViewUi_AV_UNDER_RUN)
	{
		XmgrCas_NaAlamType_e eAlarmType;

		if(_xgr_cas_NaUi_MsgOtherStausCheck(hAction, &eAlarmType) == TRUE)
		{
			if(eAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED)
			{
				pstContext->pszMessage = "parse_E20a";
				bMsgFlag = TRUE;
			}
			else if(eAlarmType == eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD)
			{
				pstContext->pszMessage = "parse_E44";
				bMsgFlag = TRUE;
			}
		}
	}

	if(bMsgFlag == FALSE)
	{
		return ERR_BUS_MESSAGE_PASS;
	}
#else
	switch(eViewState)
	{
		case eLiveViewUi_EMPTY_SVC:			pstContext->pszMessage = "LOC_CHANNEL_NOT_BROADCAST_ID";	break;
		case eLiveViewUi_NO_SIGNAL:			pstContext->pszMessage = "LOC_NO_SIGNAL_ID";				break;
		case eLiveViewUi_AV_UNDER_RUN:		pstContext->pszMessage = "LOC_CHANNEL_NOT_AVAILABLE_ID";	break;
	/*	case eLiveViewUi_NOT_RUNNING:		이 state일 경우가 발생했을 때 무엇이 표시되어야 할까?		break;	*/
		case eLiveViewUi_NO_SIGNAL_TIMEOUT:	pstContext->pszMessage = "LOC_NO_SIGNAL_ID";				break;
		default:	return ERR_BUS_MESSAGE_PASS;
	}
#endif

	_xmgr_cas_NaUi_SendBannerMessage(pstContext);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgEvtCasCheck(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hError;
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;

	hError = _xmgr_cas_NaUi_CheckMSGProcess(hAction, p2);

	pstContext->ulMessageBackup = message;

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)

#else
	if (hError != ERR_FAIL)
#endif
	{
		if (pstContext->pfnOPMessageProc != NULL)
		{
			eBusErr = pstContext->pfnOPMessageProc(message, hAction, p1, p2, p3);
		}
	}

	return eBusErr;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgOpenSession(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	BUS_Result_t eBusRes;

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_BANNER, xproc_cas_NaUiSysMsg, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	switch(pstContext->ulMessageBackup)
	{
		case eMEVT_CAS_FAIL:
			eBusRes = _xmgr_cas_NaUi_MsgEvtCasUpdateAlarm(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CHECK:
			eBusRes = _xmgr_cas_NaUi_MsgEvtCasUpdateCheck(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
			eBusRes = _xmgr_cas_NaUi_MsgLiveViewState(pstContext, message, hAction, p1, p2, p3);
			break;

		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	return eBusRes;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_MsgCloseSession(sNaUi_Alarm_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//1 TODO: System message session은 한번 열리면 절대로 닫히지 않는 Session으로 정의하였다.

	pstContext->hSession = HANDLE_NULL;

	// TODO: free할 것 하고...

	return BUS_MGR_Destroy(NULL);
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#define ______________CA_POPUP_______________
STATIC sNaUi_Popup_Context_t*	_xmgr_cas_NaUi_Popup_GetContext(void)
{
	return &s_stCasNaPopupContext;
}

STATIC void _xmgr_cas_NaUi_Popup_FreePopupItem(NaPopupItem_t *pstPopupItem)
{
	if(pstPopupItem != NULL)
	{
		if(pstPopupItem->ulPeriod)
			BUS_KillTimer(NA_UI_POPUP_TIMER_ID);

		if(pstPopupItem->pucMsg != NULL)
			OxCAS_Free(pstPopupItem->pucMsg);

		OxCAS_Free(pstPopupItem);
	}
}

STATIC HERROR _xmgr_cas_NaUi_Popup_SendPopupMessage(sNaUi_Popup_Context_t *pstContext)
{
	OxMgrCasUiEvtInput_t *pstEvent = NULL;
	HUINT32		slStream = 0;
	HUINT8		*pszPopupMsg = NULL;
	HCHAR		*pszEncodedStr = NULL, *pzProperties = NULL;

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("ulSessionHandle (0x%x)\n", pstContext->hSession);
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		goto _ERROR;
	}

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
	{
		HxLOG_Error("slStream is 0\n");
		goto _ERROR;
	}

	HLIB_RWSTREAM_Print(slStream, "[");

	switch(pstContext->pstPopupItem->eType)
	{
		case ePopup_Message:
			xmgr_cas_NaUtil_ConvertLanguageCode(pstContext->pstPopupItem->pucMsg);

			MWC_UTIL_ConvSiStringToUtf8(NULL, MWC_UTIL_DvbStrlen(pstContext->pstPopupItem->pucMsg), pstContext->pstPopupItem->pucMsg, &pszPopupMsg);
			if(pszPopupMsg == NULL)
			{
				HxLOG_Error("pszPopupMsg is NULL\n");
				goto _ERROR;
			}

			pszEncodedStr = xmgr_cas_NaUtil_EncodeString(pszPopupMsg);
			if(pszEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszEncodedStr);
				HLIB_STD_MemFree(pszEncodedStr);
				pszEncodedStr = NULL;
			}

			pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;
			MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "[{\"id\":\"LOC_CLOSE_ID\"}]", MGR_CAS_STR_LENGTH_SHORT-1);
			break;

		case ePopup_Catastrophic:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CAS_NA_CATASTROPHIC_FAILURE_ID\"}");

			pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;
			MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "[{\"id\":\"LOC_OK_ID\"}]", MGR_CAS_STR_LENGTH_SHORT-1);
			break;

		case ePopup_SWupgrade:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CAS_NA_SOFTWARE_UPGRADE_REQUIRED_ID\"}");

			pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;
			MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "[{\"id\":\"LOC_OK_ID\"}]", MGR_CAS_STR_LENGTH_SHORT-1);
			break;

		default:
			HxLOG_Error("Unknown popup type (%d)\n", pstContext->pstPopupItem->eType);
			goto _ERROR;
	}

	HLIB_RWSTREAM_Print(slStream, "]");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession = pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_NaUiPopup;

	pstEvent->stHeader.ulAttr = eMgrCasUiAttr_Subtitle;
	pstEvent->stInputData.eInputType = eMgrCasInputType_Button;
//	pstEvent->stInputData.utInputData.stBtn.ulAttr = 0;
	MWC_UTIL_DvbStrncpy(pstEvent->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG - 1);

	HLIB_STD_MemFree(pzProperties);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, eDxCAS_GROUPID_NA, pstContext->ulSlotNumber, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
		HxLOG_Error("\n");

	if(pstContext->pstPopupItem->ulPeriod)
		BUS_SetTimer(NA_UI_POPUP_TIMER_ID, pstContext->pstPopupItem->ulPeriod * 1000);

	return ERR_OK;

_ERROR:
	if(pstEvent)
		HLIB_STD_MemFree(pstEvent);

	if(pszEncodedStr != NULL)
		HLIB_STD_MemFree(pszEncodedStr);

	if(slStream != 0)
		HLIB_RWSTREAM_Close(slStream);

	return ERR_FAIL;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_UserInputData(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulSlotNumber = (HUINT32)p1;
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	if(pstContext->pstPopupItem == NULL)
	{
		HxLOG_Error("pstContext->pstPopupItem is NULL\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	if(pstUserInputNotify->hSession != pstContext->hSession)
	{
		HxLOG_Error("Invalid session handle (0x%x, 0x%x)\n", pstUserInputNotify->hSession, pstContext->hSession);
		return ERR_BUS_MESSAGE_BREAK;
	}

	if(ulSlotNumber != pstContext->ulSlotNumber)
	{
		HxLOG_Error("Invalid slot number (%d, %d)\n", ulSlotNumber, pstContext->ulSlotNumber);
		return ERR_BUS_MESSAGE_BREAK;
	}

	HxLOG_Print("PopupUI type (%d)\n", pstContext->pstPopupItem->eType);

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Button:
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eSessionType, xproc_cas_NaUiPopup, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);

			switch(pstContext->pstPopupItem->eType)
			{
				case ePopup_Message:	// 보여주거나 닫거나...다른 action 없음...
					break;

				case ePopup_Catastrophic:	// NAGRA File System formatting and System reboot...
					HxLOG_Warning("The file system will be formatted.\n");
					HLIB_DIR_Delete(CAS_JFFS2_PATH);

					HxLOG_Warning("The system will restart.\n");
					MGR_ACTION_RebootSystem();
					break;

				case ePopup_SWupgrade:	// OTA 화면이 표시되되록 하는 시나리오?
					HxLOG_Print("Select button (0x%x)\n", pstUserInputNotify->utData.stBtn.eMgrCasBtnType);
					break;

				default:
					HxLOG_Error("Unknown popup type (%d)\n", pstContext->pstPopupItem->eType);
					return ERR_BUS_MESSAGE_BREAK;
			}
			break;

		default:
			HxLOG_Error("eInputType(%d)\n", pstUserInputNotify->eInputType);
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_ServiceChanged(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->hSession != HANDLE_NULL)
	{
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPopup, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}

	_xmgr_cas_NaUi_Popup_FreePopupItem(pstContext->pstPopupItem);
	pstContext->pstPopupItem = NULL;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgCreate(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_memset(pstContext, 0x00, sizeof(sNaUi_Popup_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgDestroy(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgEvtCasCmd(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NaCmdData_t	*pstNaCmd;
	HBOOL		bDisplay = FALSE;

	pstNaCmd = (NaCmdData_t *)p2;	// NULL인지 이미 확인 완료.

	if(pstNaCmd->item.pstPopupItem == NULL)
	{
		return ERR_BUS_MESSAGE_PASS;
	}

	if(pstContext->pstPopupItem != NULL)
	{
		// 아래의 경우라면 모든 POPUP을 무시하자.. VFS가 오류나서 발생하는 경우로 VFS 영역 format 및 reboot 되어야 한다.
		if(pstContext->pstPopupItem->eType == ePopup_Catastrophic && pstNaCmd->item.pstPopupItem->eType != ePopup_Catastrophic)
		{
			_xmgr_cas_NaUi_Popup_FreePopupItem(pstNaCmd->item.pstPopupItem);
			return ERR_BUS_MESSAGE_PASS;
		}
	}

	_xmgr_cas_NaUi_Popup_FreePopupItem(pstContext->pstPopupItem);

	pstContext->pstPopupItem = pstNaCmd->item.pstPopupItem;

	switch(pstContext->pstPopupItem->eType)
	{
		case ePopup_Message:
			if(pstContext->pstPopupItem->pucMsg != NULL)
				bDisplay = TRUE;
			break;

		case ePopup_Catastrophic:	// LOC XXX string type으로 만들어야 할 것 같은데...?
		case ePopup_SWupgrade:
			bDisplay = TRUE;
			break;

		case ePopup_MessageRemove:
			bDisplay = FALSE;
			break;

		default:
			HxLOG_Error("[%s:%d] Unknown popup type (%d)\n", __FUNCTION__, __HxLINE__, pstContext->pstPopupItem->eType);
			return ERR_FAIL;
	}

	if(bDisplay == TRUE)
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			_xmgr_cas_NaUi_Popup_SendPopupMessage(pstContext);
		}
		else
		{
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPopup, HANDLE_NULL, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
		}
	}
	else
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPopup, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgOpenSession(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	if(pstSession->ulCasSlotNum != pstContext->ulSlotNumber)
	{
		HxLOG_Error("Invalid slot number (%d, %d)\n", pstSession->ulCasSlotNum, pstContext->ulSlotNumber);
		return ERR_BUS_MESSAGE_PASS;
	}

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPopup, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if(_xmgr_cas_NaUi_Popup_SendPopupMessage(pstContext) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaUi_Popup_SendPopupMessage() error\n");
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgCloseSession(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContext->hSession = HANDLE_NULL;

	if(pstContext->pstPopupItem != NULL)
	{
		switch(pstContext->pstPopupItem->eType)
		{
			case ePopup_SWupgrade:
				// 해당 CASE의 경우 USER가 CLOSE 시키더라도 Period값이 있으면 TIMER가 동작중이기에 다시 UI를 표시해 줘야 한다.
				if(pstContext->pstPopupItem->ulPeriod != 0)
					return ERR_BUS_MESSAGE_PASS;
				break;

			default:
				break;
		}

		_xmgr_cas_NaUi_Popup_FreePopupItem(pstContext->pstPopupItem);
		pstContext->pstPopupItem = NULL;

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
		{
			sNaUi_Alarm_Context_t *pstContext_Alarm = _xmgr_cas_NaUi_GetAlramContext();
			HUINT32 				ulActionId = SVC_PIPE_GetActionId(hAction);
			MgrLiveView_UiState_e	eViewState;
			HERROR				hErr;

			HxLOG_Error("CloseSession : Nagra System message [%s]\n", pstContext_Alarm->pszMessage);

			hErr  = MGR_LIVE_GetState (ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);
			if (ERR_OK == hErr)
			{
				HxLOG_Error("eViewState:%d \n", eViewState);

				if (eLiveViewUi_OK == eViewState)
				{
					HxLOG_Error ("eLiveViewUi_OK == eViewState\n");
					_xmgr_cas_NaUi_SendBannerHideMessage(pstContext_Alarm);
				}
				else
				{
					if(pstContext_Alarm->hSession != HANDLE_NULL)
					{
						HxLOG_Error("pstContext_Alarm->hSession != HANDLE_NULL\n");
						_xmgr_cas_NaUi_SendBannerMessage(pstContext_Alarm);
					}
				}
			}
			else
			{
				HxLOG_Error ("MGR_LIVE_GetState Error\n");
			}
		}
#endif

	}

	return BUS_MGR_Destroy(NULL);
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Popup_MsgBusTimer(sNaUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		case NA_UI_POPUP_TIMER_ID:
			if(pstContext->hSession != HANDLE_NULL)
				_xmgr_cas_NaUi_Popup_SendPopupMessage(pstContext);
			else
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPopup, HANDLE_NULL, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);

			return ERR_BUS_MESSAGE_BREAK;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}

#endif	//#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)

#define ______________BCD_______________
STATIC NA_UI_BDC_Context_t*	_xmgr_cas_NaUi_Bdc_GetContext(void)
{
	return &s_stCasNaBdcContext;
}

STATIC HERROR	_xmgr_cas_NaUi_Bdc_CopyStringToBuffer(HUINT8 *instr, HUINT8 *outstr, HUINT16 nlen)
{
	HUINT16 nidx = 0;

	if ((instr == NULL) || (outstr == NULL))
	{
		return ERR_FAIL;
	}

	while ((*instr != '\0') && (nlen > 0))
	{
		if ((nlen-1) < nidx)
		{
			break;
		}

		if (*instr == 0x0d)
		{
			*outstr = 0x0a;

			if (*(instr+1) == 0x0a)
			{
				instr++;
			}
		}
		else
		{
			*outstr = *instr;
		}

		outstr++;
		instr++;

		nidx++;
	}

	*outstr = '\0';

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaUi_SendBdcBannerMessage(NA_UI_BDC_Context_t *pstContext)
{
	HERROR hErr;
	OxMgrCasUiEvtBanner_t	*pstEvent;

	HUINT32		slStream;
	HCHAR		*pszEncodedStr, *pzProperties;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is null\n");
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	VK_memset(pstEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstContext->szBdcMessage);
	if(pstEvent->stBanner.stString.ulStringLength >= MGR_CAS_STR_LENGTH_LONG)
	{
		HxLOG_Warning("string length overflow %d\n", pstEvent->stBanner.stString.ulStringLength);
		pstEvent->stBanner.stString.ulStringLength = MGR_CAS_STR_LENGTH_LONG-1;
	}

	pszEncodedStr = xmgr_cas_NaUtil_EncodeString(pstContext->szBdcMessage);
	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(pszEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszEncodedStr);
		HLIB_STD_MemFree(pszEncodedStr);
		pszEncodedStr = NULL;
	}
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	//ulStreamSize = HxSTD_StrLen(pzProperties);

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
	HLIB_STD_MemFree(pzProperties);

	HxLOG_Debug("str = %s\n",pstEvent->stBanner.stString.szString);

	pstEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession = pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_NaUiBdc;

	if(pstContext->ePopupMode == eCAS_NA_POPUP_MODE_TIMEOUT)
	{
		pstEvent->stBanner.ulAttr = eMgrCasUiAttr_Timeout;
		pstEvent->stBanner.ulTimeoutSecond = pstContext->nTimeout;
	}
	else if(pstContext->ePopupMode == eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP)
	{
		pstEvent->stBanner.ulAttr = eMgrCasUiAttr_AlwaysOnTop | eMgrCasUiAttr_IgnoreKey;
	}

	hErr = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("Msg Error\n");
		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
	}

	return hErr;
}

STATIC HERROR _xmgr_cas_NaUi_SendBdcDialogMessage(NA_UI_BDC_Context_t *pstContext)
{
	HERROR hErr;
	OxMgrCasUiEvtInput_t	*pstEvent;
	OxMgrCasUiInputHeader_t	*pstHeader;
	OxMgrCasUiInput_t		*pstInputData;

	if(!(pstContext->ePopupMode == eCAS_NA_POPUP_MODE_USER_EXIT))
	{
		return ERR_FAIL;
	}

	pstEvent = OxAP_Malloc(sizeof(OxMgrCasUiEvtInput_t));
	if ( pstEvent == NULL )
	{
		HxLOG_Error("pstEvent Malloc Error\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstEvent, 0, sizeof(OxMgrCasUiEvtInput_t));

	pstEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession = pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_NaUiBdc;

	pstHeader = &pstEvent->stHeader;
	//pstHeader->ulUiHandle = (HUINT32)xproc_cas_NaUiPopup;
	pstInputData = &pstEvent->stInputData;
	MWC_UTIL_DvbStrncpy(pstHeader->szSubTitle, pstContext->szBdcMessage, MGR_CAS_STR_LENGTH_LONG - 1);

	if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_USER_EXIT)
	{
		pstHeader->ulAttr = eMgrCasUiAttr_IgnoreKey | eMgrCasUiAttr_Subtitle;
		pstInputData->eInputType = eMgrCasInputType_Button;
		pstInputData->utInputData.stBtn.ulAttr = 0;
		pstInputData->utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_OK;
		//OK button 1개와 Exit Key로 UI 사라지지 않는 Dialog box. Text는 pstContext->szBdcMessage가 Display되도록.
	}

	hErr = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("Msg Error\n");
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree(pstEvent);
			pstEvent  = NULL;
		}
	}

	return hErr;
}


STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgEvtCasCmd(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8	*pszText = NULL;
	HUINT32 ulSlotNumber = 0;
	XmgrCas_NaUiPopupCmd_e		popup_cmd = (XmgrCas_NaUiPopupCmd_e)p3;

	if ((HUINT8*)p2 != NULL)
	{
#if 0
// 우선 테스트를 위해 여기서는 szBdcMessage에 copy만하고 MWC_UTIL_ConvSiStringToUtf8()는 msg를 보내는곳에서 실행하는게 좋을거 같음.
		_xmgr_cas_NaUi_Bdc_CopyStringToBuffer((HUINT8*)p2, &(pstContext->szBdcMessage[0]), NA_UI_BDC_MAX_STRING_LEN-1);
#else
		MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen((HUINT8*)p2), (HUINT8*)p2, &pszText);
		_xmgr_cas_NaUi_Bdc_CopyStringToBuffer((HUINT8*)pszText, &(pstContext->szBdcMessage[0]), NA_UI_BDC_MAX_STRING_LEN-1);
		if(pszText)
		{
			STR_MEM_FREE(pszText);
		}
#endif
	}

	HxLOG_Debug("New PopupMode(%d), Old PopupMode(%d)\n",p1, pstContext->ePopupMode);

	if(pstContext->ePopupMode != p1)
	{
		pstContext->ePopupMode = p1;
	}

	if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_TIMEOUT)
	{
		pstContext->nTimeout = NA_UI_BDC_TIMEOUT_10SEC;
	}

	HxLOG_Debug("mode = %d, cmd = %d, hSession = 0x%08x, msg = %s\n",pstContext->ePopupMode, popup_cmd, pstContext->hSession, pstContext->szBdcMessage);

	if ((pstContext->ePopupMode == eCAS_NA_POPUP_MODE_TIMEOUT) || (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP))
	{
		if(popup_cmd == eCAS_NA_POPUP_DISPLAY)
		{
			if(pstContext->hSession != HANDLE_NULL)
			{
				HxLOG_Debug("\n");
				if(_xmgr_cas_NaUi_SendBdcBannerMessage(pstContext) != ERR_OK)
				{
					HxLOG_Critical("\n\n");
				}
			}
			else
			{
				// BDC가 여러 종류가 존재할수 있으므로 우선 EXTENSION으로 만든다.
				HxLOG_Debug("\n");
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
			}
		}
		else
		{
			HxLOG_Debug("pstContext->hSession = 0x%08x\n",pstContext->hSession);
			if(pstContext->hSession != HANDLE_NULL)
			{
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			}
		}
	}
	else if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_USER_EXIT)
	{
		if(popup_cmd == eCAS_NA_POPUP_DISPLAY)
		{
			if(pstContext->hSession != HANDLE_NULL)
			{
				if(_xmgr_cas_NaUi_SendBdcDialogMessage(pstContext) != ERR_OK)
				{
					HxLOG_Critical("\n\n");
				}
			}
			else
			{
				// BDC가 여러 종류가 존재할수 있으므로 우선 EXTENSION으로 만든다.
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);
			}
		}
		else
		{
			if(pstContext->hSession != HANDLE_NULL)
			{
				xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
			}
		}
		//OK button 1개와 Exit Key로 UI 사라지지 않는 Dialog box. Text는 pstContext->szBdcMessage가 Display되도록.
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgCreate(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	NaCmdData_t *pstNaCmd;
	HxSTD_memset(pstContext, 0x00, sizeof(NA_UI_BDC_Context_t));
//	pstNaCmd = (NaCmdData_t *)p2;

	pstContext->hSession = HANDLE_NULL;
	pstContext->ePopupMode = p1;
	pstContext->ulMessageBackup = message;
//	pstContext->pszPopupMsg = pstNaCmd->item.pstBdcItem;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgEvtCasFail(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	HUINT8	*pszText = NULL;

	if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_PPT)
	{
		if ((HUINT8*)p2 != NULL)
		{
#if 0
#if (0)
			pstContext->szBdcMessage[0] = eHxCP_UNICODE_UTF8;
#else
			pstContext->szBdcMessage[0] = 0x10;
			pstContext->szBdcMessage[1] = 0x00;
			pstContext->szBdcMessage[2] = 0x02;
#endif

#if (0)
			_xmgr_cas_NaUi_bdc_CopyStringToBuffer((HUINT8*)p2, &(pstContext->szBdcMessage[1]), NA_UI_BDC_MAX_STRING_LEN-1);
#else
			_xmgr_cas_NaUi_bdc_CopyStringToBuffer((HUINT8*)p2, &(pstContext->szBdcMessage[3]), NA_UI_BDC_MAX_STRING_LEN-4);
#endif
#else
			MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen((HUINT8*)p2), (HUINT8*)p2, &pszText);
			_xmgr_cas_NaUi_bdc_CopyStringToBuffer((HUINT8*)pszText, &(pstContext->szBdcMessage[0]), NA_UI_BDC_MAX_STRING_LEN-1);

			if(pszText)
			{
				STR_MEM_FREE(pszText);
			}
#endif
		}

		if(pstContext->bBdcSessionStarted == TRUE)
		{

			//UI를 다시 그리게 한다.
		}
	}
#endif
	return ERR_BUS_NO_ERROR;
}

#if /*defined(CONFIG_MW_CAS_NAGRA_DALTEST) && */ defined(CONFIG_MW_CAS_NAGRA_OP_SES)
STATIC void _xmgr_cas_NaUi_Bdc_InitCasCmdStorage(void)
{
	hSessionClose = HANDLE_NULL;
	HxSTD_memset(&s_stBdcCasCmdStorage, 0, sizeof(sNaUi_Bdc_CasCmdStorage_t)*BDC_STORAGE_CNT);
	BUS_SetTimer(BDCACTION_TIMER_ID, BDCACTION_TIMER_TIME);
}

STATIC void _xmgr_cas_NaUi_Bdc_ClearCasCmdStorageByIndex(HUINT8 ucIndex)
{
	HUINT8 i = 0;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;

	// ucIndex가 무조건 0이어야함.
	if(ucIndex != 0)
	{
		HxLOG_Error("ucIndex is error \n", ucIndex);
	}

	if(ucIndex < BDC_STORAGE_CNT)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[ucIndex];
		HxSTD_memset(pstBsdCasCmdStorage, 0, sizeof(sNaUi_Bdc_CasCmdStorage_t));
		HxSTD_memset(&stCasCmdStorageTemp, 0, sizeof(sNaUi_Bdc_CasCmdStorage_t)*BDC_STORAGE_CNT);
		for(i = 0; i < BDC_STORAGE_CNT; i++)
		{
			if(BDC_STORAGE_CNT == (i+1))
				break;
			HxSTD_memcpy(&stCasCmdStorageTemp[i], &s_stBdcCasCmdStorage[i+1], sizeof(sNaUi_Bdc_CasCmdStorage_t));
		}
		HxSTD_memset(&s_stBdcCasCmdStorage, 0, sizeof(sNaUi_Bdc_CasCmdStorage_t)*BDC_STORAGE_CNT);
		HxSTD_memcpy(&s_stBdcCasCmdStorage, &stCasCmdStorageTemp, sizeof(sNaUi_Bdc_CasCmdStorage_t)*BDC_STORAGE_CNT);
	}

	#if 0
	{
		HUINT8 ucActiveCnt = 0;
		for(i = 0; i < BDC_STORAGE_CNT; i ++)
		{
			pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
			if(pstBsdCasCmdStorage->actived == TRUE)
				ucActiveCnt++;
		}
	}
	#endif
}

STATIC void _xmgr_cas_NaUi_Bdc_PushCasCmdStorage(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8 i = 0;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return;
	}

	for(i = 0; i < BDC_STORAGE_CNT; i ++)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
		if(pstBsdCasCmdStorage->actived == FALSE)
		{
			pstBsdCasCmdStorage->actived = TRUE;
			HxSTD_memset(&pstBsdCasCmdStorage->s_stCasNaBdcContext, 0, sizeof(NA_UI_BDC_Context_t));
			pstBsdCasCmdStorage->s_stCasNaBdcContext.hSession = HANDLE_NULL;
			//HxSTD_memcpy(&pstBsdCasCmdStorage->s_stCasNaBdcContext, pstContext, sizeof(NA_UI_BDC_Context_t));
			pstBsdCasCmdStorage->message = message;
			pstBsdCasCmdStorage->hAction = hAction;
			pstBsdCasCmdStorage->p1 = p1;
			pstBsdCasCmdStorage->p2 = p2;
			pstBsdCasCmdStorage->p3 = p3;
			break;
		}
	}
}

STATIC void _xmgr_cas_NaUi_Bdc_PopCasCmdStorage(void)
{
	HUINT8 i = 0;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;
	XmgrCas_NaUiPopupCmd_e ePopupMode;

	HxLOG_Debug("\n");

	for(i = 0; i < BDC_STORAGE_CNT; i ++)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
		if(pstBsdCasCmdStorage->actived == TRUE)
		{
			break;
		}
	}

	if(pstBsdCasCmdStorage == NULL)
	{
		HxLOG_Error("pstBsdCasCmdStorage is NULL \n");
		return;
	}

	if(i >= BDC_STORAGE_CNT)
	{
		HxLOG_Debug("i = %dreturn\n",i);
		return;
	}

	pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
	ePopupMode = (XmgrCas_NaUiPopupCmd_e)pstBsdCasCmdStorage->p3;

	HxLOG_Debug("ePopupMode = %s\n",ePopupMode == eCAS_NA_POPUP_DISPLAY ? "BDC Display" : "BDC Remove");

	if(ePopupMode == eCAS_NA_POPUP_DISPLAY)
	{
		_xmgr_cas_NaUi_Bdc_MsgEvtCasCmd(&pstBsdCasCmdStorage->s_stCasNaBdcContext,
										pstBsdCasCmdStorage->message,
										pstBsdCasCmdStorage->hAction,
										pstBsdCasCmdStorage->p1,
										pstBsdCasCmdStorage->p2,
										pstBsdCasCmdStorage->p3);
		//HxSTD_memcpy(&s_stCasNaBdcContext, &pstBsdCasCmdStorage->s_stCasNaBdcContext, sizeof(NA_UI_BDC_Context_t));
	}
	else if(ePopupMode == eCAS_NA_POPUP_CLOSE)
	{
		pstBsdCasCmdStorage->s_stCasNaBdcContext.hSession =  hSessionClose; // session close시에는 항상 last로 open한 session을 가지고 close한다.
		_xmgr_cas_NaUi_Bdc_MsgEvtCasCmd(&pstBsdCasCmdStorage->s_stCasNaBdcContext,
										pstBsdCasCmdStorage->message,
										pstBsdCasCmdStorage->hAction,
										pstBsdCasCmdStorage->p1,
										pstBsdCasCmdStorage->p2,
										pstBsdCasCmdStorage->p3);
	}

	//_xmgr_cas_NaUi_Bdc_ClearCasCmdStorageByIndex(i); // session을 close시 or open시에 제거하도록 변경함.
}

STATIC void _xmgr_cas_NaUi_Bdc_ClearCasCmdStorage(void)
{
	HUINT8 i = 0;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;

	HxLOG_Debug("\n");

	for(i = 0; i < BDC_STORAGE_CNT; i ++)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
		if(pstBsdCasCmdStorage->actived == TRUE)
		{
			break;
		}
	}

	if(pstBsdCasCmdStorage == NULL)
	{
		HxLOG_Error("pstBsdCasCmdStorage is NULL \n");
		return;
	}

	if(i >= BDC_STORAGE_CNT)
	{
		HxLOG_Debug("i = %dreturn\n",i);
		return;
	}

	_xmgr_cas_NaUi_Bdc_ClearCasCmdStorageByIndex(i);
}

STATIC NA_UI_BDC_Context_t * _xmgr_cas_NaUi_Bdc_GetCasCmdStorageContext(void)
{
	HUINT8 i = 0;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;

	for(i = 0; i < BDC_STORAGE_CNT; i ++)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
		if(pstBsdCasCmdStorage->actived == TRUE)
		{
			break;
		}
	}

	if(pstBsdCasCmdStorage == NULL)
	{
		HxLOG_Error("pstBsdCasCmdStorage is NULL \n");

		#if defined (CONFIG_OP_SES)
		#else
		return;
		#endif
	}

	if(i >= BDC_STORAGE_CNT)
	{
		//HxLOG_Debug("i = %dreturn\n",i);
		i = 0; // actived 값이 없으면 default로 index 0의 bdc context를 보낸다.
	}

	return &s_stBdcCasCmdStorage[i].s_stCasNaBdcContext;
}

STATIC HERROR _xmgr_cas_NaUi_Bdc_IsValidCasCmdStorage(void)
{
	HUINT8 i = 0;
	HERROR ucValid = ERR_FAIL;
	sNaUi_Bdc_CasCmdStorage_t *pstBsdCasCmdStorage = NULL;

	for(i = 0; i < BDC_STORAGE_CNT; i ++)
	{
		pstBsdCasCmdStorage = &s_stBdcCasCmdStorage[i];
		if(pstBsdCasCmdStorage == NULL)
		{
			HxLOG_Error("pstBsdCasCmdStorage is NULL \n");
			continue;
		}
		if(pstBsdCasCmdStorage->actived == TRUE)
		{
			ucValid = ERR_OK;
			break;
		}
	}

	return ucValid;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_PushMsgEvtCasCmd(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext)
	{
		_xmgr_cas_NaUi_Bdc_PushCasCmdStorage(pstContext, message, hAction, p1, p2, p3);
	}
	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgEvtInputNotify(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;
	HUINT32 ulSlotNumber = 0;

	if(eInputType == eMgrCasInputType_Button)
	{
		if((pstContext->ePopupMode == eCAS_NA_POPUP_MODE_TIMEOUT) || (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP))
		{
			return ERR_BUS_MESSAGE_BREAK;
		}
		else if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_USER_EXIT)
		{
			if(pstCasInputData->stBtn.eMgrCasBtnType == eMgrCasBtn_OK)
			{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
				if(pstContext->hSession != HANDLE_NULL)
				{
					xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, pstContext->hSession, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
				}
#endif
			}
		}
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgReqSessionStart(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->hSession = pstSession->hSession;
	hSessionClose = pstSession->hSession; // sesssion을 close를 하기위해 사용되는 hSession임.

	HxLOG_Debug("hSession(0x%08x)\n", pstContext->hSession);

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiBdc, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if ((pstContext->ePopupMode == eCAS_NA_POPUP_MODE_TIMEOUT) || (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP))
	{
		if(_xmgr_cas_NaUi_SendBdcBannerMessage(pstContext) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}
	else if (pstContext->ePopupMode == eCAS_NA_POPUP_MODE_USER_EXIT)
	{
		if(_xmgr_cas_NaUi_SendBdcDialogMessage(pstContext) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}

	_xmgr_cas_NaUi_Bdc_ClearCasCmdStorage();

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_Bdc_MsgReqSessionStop(NA_UI_BDC_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	hSessionClose = HANDLE_NULL;
	_xmgr_cas_NaUi_Bdc_ClearCasCmdStorage();
	//pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_MESSAGE_PASS;
}
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UI_PUBLIC_FUNCTION______________________________________________________

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
BUS_Result_t	xproc_cas_NaUiPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusRes = ERR_BUS_NO_ERROR;
	sNaUi_Popup_Context_t	*pstContext = _xmgr_cas_NaUi_Popup_GetContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), hAction(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			eBusRes = _xmgr_cas_NaUi_Popup_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CMD:
			eBusRes = _xmgr_cas_NaUi_Popup_MsgEvtCasCmd(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_NaUi_Popup_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_NaUi_Popup_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			eBusRes = _xmgr_cas_NaUi_Popup_UserInputData(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusRes = _xmgr_cas_NaUi_Popup_ServiceChanged(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER :
			eBusRes = _xmgr_cas_NaUi_Popup_MsgBusTimer(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			eBusRes = _xmgr_cas_NaUi_Popup_MsgDestroy(pstContext, message, hAction, p1, p2, p3);
			break;

		default :
			break;
	}


	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)

BUS_Result_t	xproc_cas_NaUiBdc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusRes = ERR_BUS_NO_ERROR;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	NA_UI_BDC_Context_t 	*pstContext = _xmgr_cas_NaUi_Bdc_GetCasCmdStorageContext();
#else
	NA_UI_BDC_Context_t 	*pstContext = _xmgr_cas_NaUi_Bdc_GetContext();
#endif

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			eBusRes = _xmgr_cas_NaUi_Bdc_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_OP_SES)
		/* standby에서도 떠 있어야 하므로 그냥 PASS
		    stanby mode진입후 바로 live로 전환시 해당 proc이 distory되는 문제로인해 live 전환시 msg 처리 오류 발생.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
#endif

		case eMEVT_CAS_CMD:
#if /* defined(CONFIG_MW_CAS_NAGRA_DALTEST) && */ defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			if(bBdcFirstCasCmdMsgFlag == FALSE)
			{
				// 아래 함수가 eMEVT_BUS_CREATE에 존재해야하는데 eMEVT_BUS_CREATE가 들어오지 않아 여기에 임시 추가됨.
				bBdcFirstCasCmdMsgFlag = TRUE;
				_xmgr_cas_NaUi_Bdc_InitCasCmdStorage();
			}

			eBusRes = _xmgr_cas_NaUi_Bdc_PushMsgEvtCasCmd(pstContext, message, hAction, p1, p2, p3);
#else
			eBusRes = _xmgr_cas_NaUi_Bdc_MsgEvtCasCmd(pstContext, message, hAction, p1, p2, p3);
#endif
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			eBusRes = _xmgr_cas_NaUi_Bdc_MsgReqSessionStart(pstContext, message, hAction, p1, p2, p3);
#endif
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			eBusRes = _xmgr_cas_NaUi_Bdc_MsgReqSessionStop(pstContext, message, hAction, p1, p2, p3);
#endif
			break;

#if /* defined(CONFIG_MW_CAS_NAGRA_DALTEST) && */ defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eMEVT_BUS_TIMER :
			if(p1==BDCACTION_TIMER_ID)
			{
				if(_xmgr_cas_NaUi_Bdc_IsValidCasCmdStorage() == ERR_OK)
				{
					_xmgr_cas_NaUi_Bdc_PopCasCmdStorage();
				}
				eBusRes =  ERR_BUS_MESSAGE_BREAK;
			}
			break;
#endif

		case eMEVT_BUS_DESTROY :
			break;

		default :
			break;
	}


	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

BUS_Result_t	xproc_cas_NaUiSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sNaUi_Alarm_Context_t *pstContext = _xmgr_cas_NaUi_GetAlramContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), hAction(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaUiSysMsg");
			eBusRes = _xmgr_cas_NaUi_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_OP_SES)
		/* standby에서도 떠 있어야 하므로 그냥 PASS
		    stanby mode진입후 바로 live로 전환시 해당 proc이 distory되는 문제로인해 live 전환시 msg 처리 오류 발생.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
#endif

		case eMEVT_CAS_FAIL:
			eBusRes = _xmgr_cas_NaUi_MsgEvtCasFail(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CHECK:
			break;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eMEVT_CAS_CAM_REMOVED:
			eBusRes = _xmgr_cas_NaUi_MsgCamStatus(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED :
			if(pstContext->ulMessageBackup == eMEVT_CAS_FAIL)
			{
				eBusRes = _xmgr_cas_NaUi_MsgEvtCasUpdateAlarm(pstContext, message, hAction, p1, p2, p3);
			}
			break;
#endif

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		case eMEVT_CAS_OK:
			_xmgr_cas_NaUi_SendBannerHideMessage(pstContext);
			break;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
			eBusRes = _xmgr_cas_NaUi_MsgLiveViewState(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_NaUi_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_NaUi_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			break;
		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
BUS_Result_t	xproc_cas_NaUiEntitlementMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sNaUi_Entitlement_Context_t *pstContext = _xmgr_cas_NaUi_GetEntitlementContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), hAction(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif
	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaUiEntitlementMessage");
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_OP_SES)
		/* standby에서도 떠 있어야 하므로 그냥 PASS
		    stanby mode진입후 바로 live로 전환시 해당 proc이 distory되는 문제로인해 live 전환시 msg 처리 오류 발생.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
#endif

		case eMEVT_CAS_FAIL:
			_xmgr_cas_NaUi_Entitlement_SendHideBannerMessage(pstContext);
			break;

		case eMEVT_CAS_CHECK:
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgEvtCasCheck(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED :
			if(pstContext->ulMessageBackup == eMEVT_CAS_CHECK)
			{
				eBusRes = _xmgr_cas_NaUi_Entitlement_MsgEvtCasUpdateCheck(pstContext, message, hAction, p1, p2, p3);
			}
			break;
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		case eMEVT_CAS_OK:
			//_xmgr_cas_NaUi_SendBannerHideMessage(pstContext);
			break;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgLiveViewState(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA :
			eBusRes = _xmgr_cas_NaUi_Entitlement_MsgEvtInputNotify(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif


HUINT8*	xmgr_cas_NaGetMessage(HINT32 nAlarmType)
{
	return  _xmgr_cas_NaUi_GetMessage((XmgrCas_NaAlamType_e)nAlarmType);
}

