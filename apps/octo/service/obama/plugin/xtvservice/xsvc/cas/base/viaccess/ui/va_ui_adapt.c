/*
	Viaccess UI
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_ui.h"
#include "va_ui_api.h"
#include "va_ui_adapt.h"

#include "va_main.h"
#include "va_util.h"
#include "va_pi_api.h"

#include "vkernel.h"

#include <string.h>

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
#include "va_pvr_adapt.h"
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





#define GET_ACTION_ID(c) 					((c & 0xFF000000) >> 24)
#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)

#define VA_UI_EVENT_TIMEOUT		1000 // Á¶±Ý ´Ã·Á¾ß ÇÒµí.. 500

#define VA_UI_EVENT_BUFFER_SIZE	20 // 100

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static HUINT32 sVaUiAdaptEventQid;
static HUINT8 *s_pszStbId;
static HUINT8 *s_pszUa;
static HUINT8 *s_pszVersion;
static HUINT8 *s_pszCamlock;
static HUINT8 *s_pszStbSerial;
static HUINT8 *s_pszCurrency;
static VA_UI_OP_MESSAGE *s_pOpMessage;
static VA_UI_OP_DATA *s_pOpData;

static VA_UI_EVENT s_stLastEvent;

static HINT32 s_nSecurity; /* TRUE or FALSE */
static HINT32 s_nCamLockCtrlLevel; /* VA_CAMLOCK_CONTROL_NO, VA_CAMLOCK_CONTROL_LOW, VA_CAMLOCK_CONTROL_MEDIUM, VA_CAMLOCK_CONTROL_HIGH */
static HINT32 s_nCamlock; /* VA_UI_CAMLOCK_ACTIVATED_OK, ..., VA_UI_CAMLOCK_DISABLED */
static HINT32 s_nScState; /* VA_UI_SC_INSERTED, VA_UI_SC_EXTRACTED, VA_UI_SC_READY, VA_UI_SC_FAILED, VA_UI_SC_NOT_ACCEPTED, VA_UI_SC_INVALIDATED */

static VA_UI_EVENT s_stUiEventBuf[VA_UI_EVENT_BUFFER_SIZE];
static HUINT32 s_ulUiEventBufIdx;

static HUINT32 s_ulVaUiSemId;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_UI_INT_ConvertStr2VaPin(HUINT8 *pVaPin, HUINT8 *pStr);
static void VA_UI_INT_ConvertHex2Str(HUINT8 *pStr, HUINT8 *pHex, HUINT32 ulCount);
static void VA_UI_INT_ConvertUA2Str(HUINT8* pStr, HUINT8 *pUA, HUINT32 ulCount);
static void VA_UI_INT_ConvertDate2Str(HUINT8 *pStr, tVA_UI_Date date);
static void VA_UI_INT_ConvertPrice2Str(HUINT8 *pStr, HINT32 nInteger, HINT32 nFraction);

#ifdef CONFIG_DEBUG
HUINT8 *VA_UI_Debug_GetEventName(VA_UI_EVENT_TYPE eEventType)
{
	switch (eEventType)
	{
	case VA_UI_DESCRAMBLING_OK:									return "VA_UI_DESCRAMBLING_OK";
	case VA_UI_DESCRAMBLING_OK_RIGHT_OK: 						return "VA_UI_DESCRAMBLING_OK_RIGHT_OK";
	case VA_UI_FREE_TO_AIR:										return "VA_UI_FREE_TO_AIR";
	case VA_UI_SMARTCARD_INSERTED:								return "VA_UI_SMARTCARD_INSERTED";
	case VA_UI_SMARTCARD_READY:									return "VA_UI_SMARTCARD_READY";
	case VA_UI_STOP_SECURITY_RESTRICTION:						return "VA_UI_STOP_SECURITY_RESTRICTION";
	case VA_UI_CAMLOCK_NOT_ACTIVATED:							return "VA_UI_CAMLOCK_NOT_ACTIVATED";
	case VA_UI_CAMLOCK_DISABLED:								return "VA_UI_CAMLOCK_DISABLED";
	case VA_UI_CAMLOCK_ACTIVATED_OK:							return "VA_UI_CAMLOCK_ACTIVATED_OK";
	case VA_UI_DESCRAMBLING_KO: 								return "VA_UI_DESCRAMBLING_KO";
	case VA_UI_DESCRAMBLING_KO_NO_RIGHT:						return "VA_UI_DESCRAMBLING_KO_NO_RIGHT";
	case VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT:			return "VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT";
	case VA_UI_DESCRAMBLING_KO_MISSING_KEY:						return "VA_UI_DESCRAMBLING_KO_MISSING_KEY";
	case VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE:					return "VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE";
	case VA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH:			return "VA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH";
	case VA_UI_VIACCESS_NOT_APPLICABLE:							return "VA_UI_VIACCESS_NOT_APPLICABLE";
	case VA_UI_VIACCESS_SMARTCARD_NOT_FOUND:					return "VA_UI_VIACCESS_SMARTCARD_NOT_FOUND";
	case VA_UI_SMARTCARD_EXTRACTED:								return "VA_UI_SMARTCARD_EXTRACTED";
	case VA_UI_SMARTCARD_NEEDED:								return "VA_UI_SMARTCARD_NEEDED";
	case VA_UI_SMARTCARD_FAILED:								return "VA_UI_SMARTCARD_FAILED";
	case VA_UI_SMARTCARD_NOT_ACCEPTED:							return "VA_UI_SMARTCARD_NOT_ACCEPTED";
	case VA_UI_SMARTCARD_INSUFFICIENT_MEMORY:					return "VA_UI_SMARTCARD_INSUFFICIENT_MEMORY";
	case VA_UI_SMARTCARD_INVALIDATED:							return "VA_UI_SMARTCARD_INVALIDATED";
	case VA_UI_CAMLOCK_ACTIVATED_NO_CARD:						return "VA_UI_CAMLOCK_ACTIVATED_NO_CARD";
	case VA_UI_PAIRING_MISMATCH:								return "VA_UI_PAIRING_MISMATCH";
	case VA_UI_NEW_OPERATOR_MESSAGE:							return "VA_UI_NEW_OPERATOR_MESSAGE";
	case VA_UI_CAMLOCK_ACTIVATED_KO:							return "VA_UI_CAMLOCK_ACTIVATED_KO";
	case VA_UI_START_SECURITY_RESTRICTION:						return "VA_UI_START_SECURITY_RESTRICTION";
	case VA_UI_DESCRAMBLING_KO_MATURITY_RATING:					return "VA_UI_DESCRAMBLING_KO_MATURITY_RATING";
	case VA_UI_DESCRAMBLING_OK_PREVIEW:							return "VA_UI_DESCRAMBLING_OK_PREVIEW";
	case VA_UI_NEW_OPERATOR_DATA:								return "VA_UI_NEW_OPERATOR_DATA";
	case VA_UI_DESCRAMBLING_KO_IPPV_OFFER:						return "VA_UI_DESCRAMBLING_KO_IPPV_OFFER";
	case VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT:				return "VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT";
	case VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER:			return "VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER";
	case VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED:				return "VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED";
	case VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER:	return "VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER";
	case VA_UI_OVERDRAFT_REACHED:								return "VA_UI_OVERDRAFT_REACHED";
	case VA_UI_THRESHOLD_REACHED:								return "VA_UI_THRESHOLD_REACHED";
	case VA_UI_RECORD_RIGHT_OK:									return "VA_UI_RECORD_RIGHT_OK";
	case VA_UI_TIMESHIFT_RIGHT_OK:								return "VA_UI_TIMESHIFT_RIGHT_OK";
	case VA_UI_RECORD_RIGHT_KO:									return "VA_UI_RECORD_RIGHT_KO";
	case VA_UI_TIMESHIFT_RIGHT_KO:								return "VA_UI_TIMESHIFT_RIGHT_KO";
	case VA_UI_RECORD_INFO_CHANGE:								return "VA_UI_RECORD_INFO_CHANGE";
	case VA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED:					return "VA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED";
	case VA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED:					return "VA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED";
	case VA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED:					return "VA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED";
	case VA_UI_ORM_CONNECTION_OK:								return "VA_UI_ORM_CONNECTION_OK";
	case VA_UI_ORM_CONNECTION_KO:								return "VA_UI_ORM_CONNECTION_KO";
	case VA_UI_CW_SERVER_CONNECTION_OK:							return "VA_UI_CW_SERVER_CONNECTION_OK";
	case VA_UI_CW_SERVER_CONNECTION_KO:							return "VA_UI_CW_SERVER_CONNECTION_KO";
	case VA_UI_TERMINAL_NOT_ACTIVATED:							return "VA_UI_TERMINAL_NOT_ACTIVATED";
	case VA_UI_TERMINAL_READY:									return "VA_UI_TERMINAL_READY";
	case VA_UI_TERMINAL_INSUFFICIENT_MEMORY:					return "VA_UI_TERMINAL_INSUFFICIENT_MEMORY";
	case VA_UI_TERMINAL_CONFIGURATION_ERROR:					return "VA_UI_TERMINAL_CONFIGURATION_ERROR";
	case VA_UI_TERMINAL_INVALIDATED:							return "VA_UI_TERMINAL_INVALIDATED";
	case VA_UI_TERMINAL_PERSONALIZATION_OK:						return "VA_UI_TERMINAL_PERSONALIZATION_OK";
	case VA_UI_TERMINAL_PERSONALIZATION_KO:						return "VA_UI_TERMINAL_PERSONALIZATION_KO";
	case VA_UI_FREE_TO_AIR_SERVICE:								return "VA_UI_FREE_TO_AIR_SERVICE";
	case VA_UI_DESCRAMBLE_SERVICE:								return "VA_UI_DESCRAMBLE_SERVICE";
	case VA_UI_OTHER_DESCRAMBLE_SERVICE:						return "VA_UI_OTHER_DESCRAMBLE_SERVICE";
	case VA_UI_START_SERVICE:									return "VA_UI_START_SERVICE";
	case VA_UI_STOP_SERVICE:									return "VA_UI_STOP_SERVICE";

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
	case VA_UI_SHOW_FINGERPRINT:								return "VA_UI_SHOW_FINGERPRINT";
	case VA_UI_HIDE_FINGERPRINT:								return "VA_UI_HIDE_FINGERPRINT";
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
	case VA_UI_FREE_CHANNEL_BLOCK:								return "VA_UI_FREE_CHANNEL_BLOCK";
	case VA_UI_FREE_CHANNEL_BLOCK_STOP:							return "VA_UI_FREE_CHANNEL_BLOCK_STOP";
#endif

	default:													return "Unknown";
	}
}

HUINT8 *VA_UI_Debug_GetMsgTypeName(SvcCas_MsgType_e eMsgType) // svc_cas_DbgMakeStrCasMsgType()
{
	switch (eMsgType)
	{
	case eCAS_MSG_TYPE_None: 		return "None";
	case eCAS_MSG_TYPE_Ok:		return "Ok";
	case eCAS_MSG_TYPE_Fail:		return "Fail";
	case eCAS_MSG_TYPE_Command:	return "Command";
	case eCAS_MSG_TYPE_Check: 	return "Check";
	default:					return "Unknown";
	}
}

HUINT8 *VA_UI_Debug_GetShowTypeName(SvcCas_ShowType_e eShowType) // svc_cas_DbgMakeStrCasShowType()
{
	switch (eShowType)
	{
	case eCasShowType_None: 		return "None";
	case eCasShowType_AudioOnly:	return "AudioOnly";
	case eCasShowType_VideoOnly:	return "VideoOnly";
	case eCasShowType_Both: 		return "Both";
	default:						return "Unknown";
	}
}

HUINT8 *VA_UI_Debug_GetCamlockLevel(HINT32 nCamlockCtrlLevel)
{
	switch(nCamlockCtrlLevel)
	{
		case VA_CAMLOCK_CONTROL_HIGH:	return "High";
		case VA_CAMLOCK_CONTROL_MEDIUM:	return "Medium";
		case VA_CAMLOCK_CONTROL_LOW :	return "Low";
		case VA_CAMLOCK_CONTROL_NO:		return "NoCtrl";
		default:						return "Unknown";
	}
}

HUINT8 *VA_UI_Debug_GetCamlockState(HINT32 nCamlock)
{
	switch(nCamlock)
	{
		case VA_UI_CAMLOCK_ACTIVATED_OK:		return "OK";
		case VA_UI_CAMLOCK_ACTIVATED_KO:		return "KO";
		case VA_UI_CAMLOCK_ACTIVATED_NO_CARD :	return "NoCard";
		case VA_UI_CAMLOCK_NOT_ACTIVATED:		return "NotActivated";
		case VA_UI_CAMLOCK_DISABLED:			return "Disabled";
		default:								return "Unknown";
	}
}

HUINT8 *VA_UI_Debug_GetScState(HINT32 nScState)
{
	switch(nScState)
	{
		case VA_UI_SMARTCARD_INSERTED:		return "Inserted";
		case VA_UI_SMARTCARD_EXTRACTED:	return "Extracted";
		case VA_UI_SMARTCARD_READY :		return "Ready";
		case VA_UI_SMARTCARD_FAILED:		return "Failed";
		case VA_UI_SMARTCARD_NOT_ACCEPTED:	return "NotAccepted";
		case VA_UI_SMARTCARD_INVALIDATED:	return "Invalidated";
		default:					return "Unknown";
	}
}
#endif

static void VA_UI_INT_ConvertStr2VaPin(HUINT8 *pVaPin, HUINT8 *pStr)
{
	VA_memset(pVaPin, 0, 8);
	pVaPin[6] = ((pStr[0] - '0') << 4) | ((pStr[1] - '0') & 0xf);
	pVaPin[7] = ((pStr[2] - '0') << 4) | ((pStr[3] - '0') & 0xf);

	HxLOG_Info("%s (%x %x %x %x) %x %x %x %x %x %x %x %x\n", pStr, pStr[0], pStr[1], pStr[2], pStr[3],
	pVaPin[0], pVaPin[1], pVaPin[2], pVaPin[3], pVaPin[4], pVaPin[5], pVaPin[6], pVaPin[7]);
}

static void VA_UI_INT_ConvertDate2Str(HUINT8 *pStr, tVA_UI_Date date)
{
	// it might be changed
	snprintf(pStr, 11, "%02d/%02d/%04d", date.uiMonth, date.uiDay, date.uiYear);
}

static void VA_UI_INT_ConvertDateEx2Str(HUINT8 *pStr, tVA_UI_DateEx date)
{
	// it might be changed
	// format YYYY/MM/DD HH:MM:SS
	snprintf(pStr, 20, "%04d/%02d/%02d %02d:%02d:%02d", date.uiYear, date.uiMonth, date.uiDay, date.uiHour, date.uiMinute, date.uiSecond);
}

static void VA_UI_INT_ConvertHex2Str(HUINT8 *pStr, HUINT8 *pHex, HUINT32 ulCount)
{
	HINT32 i;
	HINT32 digit;

	for (i = 0; i < ulCount; i++)
	{
		digit = pHex[i] >> 4;
		if (digit > 9)
			pStr[i*2] = (digit - 10) + 'A';
		else
			pStr[i*2] = digit + '0';

		digit = pHex[i] &0xF;
		if (digit > 9)
			pStr[i*2 + 1] = (digit - 10) + 'A';
		else
			pStr[i*2 + 1] = digit + '0';
	}
	pStr[i*2] = 0;
}

/* Unique address is 5byte integer. so it can't be displayed by "%ld" */
static void VA_UI_INT_ConvertUA2Str(HUINT8* pStr, HUINT8 *pUA, HUINT32 ulCount)
{
	HUINT32 base, thousand, million, billion;
	HUINT32 ulValue;

	ulValue = get32bit(pUA + 1);

	base = ulValue % 1000;
	ulValue /= 1000;

	thousand = ulValue % 1000;
	ulValue /= 1000;

	million = ulValue % 1000;
	ulValue /= 1000;

	billion = ulValue % 1000;

	// 0x100000000 = 4,294,967,296

	base += (HUINT32)pUA[0] * 296;
	thousand += (base / 1000);
	base = (base % 1000);

	thousand += (HUINT32)pUA[0] * 967;
	million += (thousand / 1000);
	thousand = (thousand % 1000);

	million += (HUINT32)pUA[0] * 294;
	billion += (million / 1000);
	million = (million % 1000);

	billion += (HUINT32)pUA[0] * 4;

	// i.e. 000 00 70 526, 319 30 00 686 (3, 193, 000, 686)
	snprintf(pStr, VA_UI_UA_SIZE*4, "%d%02d %01d%01d %02d %03d", billion, million/10, million%10, thousand/100, thousand%100, base);
 }

static void VA_UI_INT_ConvertPrice2Str(HUINT8 *pTarget, HINT32 nInteger, HINT32 nFraction)
{
	if (nFraction)
		sprintf(pTarget, "%d.%d", nInteger, nFraction);
	else
		sprintf(pTarget, "%d", nInteger);
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
static void VA_UI_INT_CheckEventForRecord(SvcCas_MsgType_e eMsgType, VA_UI_EVENT *pEvent)
{
	HBOOL bBlock;

	if (eMsgType == eCAS_MSG_TYPE_Command || eMsgType == eCAS_MSG_TYPE_Fail)
	{
		switch(pEvent->eEventType)
		{
		case VA_UI_NEW_OPERATOR_MESSAGE:
		case VA_UI_NEW_OPERATOR_DATA:
		case VA_UI_SMARTCARD_READY:
		case VA_UI_STOP_SECURITY_RESTRICTION:
		case VA_UI_CAMLOCK_NOT_ACTIVATED:
		case VA_UI_CAMLOCK_DISABLED:
		case VA_UI_CAMLOCK_ACTIVATED_OK:
#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
		case VA_UI_SHOW_FINGERPRINT:
		case VA_UI_HIDE_FINGERPRINT:
#endif
// hjlee3, for viaccess evaluation step
#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
		case VA_UI_FREE_CHANNEL_BLOCK:
		case VA_UI_FREE_CHANNEL_BLOCK_STOP:
#endif
			bBlock = FALSE;
			break;

		default:
			bBlock = TRUE;
			break;
		}

		if (bBlock)
		{
			VA_PVR_Rec_DeleteEcm(pEvent->ulAcsId, bBlock);
		}
	}
	else
	{
		if (eMsgType == eCAS_MSG_TYPE_Ok)
			VA_PVR_Rec_DeleteEcm(pEvent->ulAcsId, FALSE);
	}
}
#endif

void VA_UI_Adapt_Init()
{
	HERROR nErr;

	HxLOG_Print("[Enter]\n");

	nErr = VA_SEM_Create(VA_UI_ADAPT_SEM_NAME, &s_ulVaUiSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	sVaUiAdaptEventQid = 0;

	s_pszStbId = NULL;
	s_pszUa = NULL;
	s_pszVersion = NULL;
	s_pszCamlock = NULL;
	s_pszStbSerial = NULL;
	s_pszCurrency = NULL;
	s_pOpMessage = NULL;
	s_pOpData = NULL;

	s_nSecurity = FALSE;

#if 1 // for camlock control
	s_nCamLockCtrlLevel =
	#if defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_HIGH)
		VA_CAMLOCK_CONTROL_HIGH;
	#elif defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_MEDIUM)
		VA_CAMLOCK_CONTROL_MEDIUM;
	#elif defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_LOW)
		VA_CAMLOCK_CONTROL_LOW;
	#else
		VA_CAMLOCK_CONTROL_NO;
	#endif
#endif

#if 0 // org
	s_nCamlock = VA_UI_CAMLOCK_NOT_ACTIVATED;
#else // modified
	s_nCamlock =
	#if defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_HIGH) || defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_MEDIUM) /* || defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_LOW) */
		#if defined(CONFIG_PROD_VAHD3100S)
			VA_UI_CAMLOCK_NOT_ACTIVATED;
		#else
		VA_UI_CAMLOCK_ACTIVATED_NO_CARD;
		#endif
	#else
		VA_UI_CAMLOCK_NOT_ACTIVATED;
	#endif
#endif

	s_nScState = VA_UI_SMARTCARD_EXTRACTED;

	s_ulUiEventBufIdx = 0;

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_Exit()
{
	VA_SEM_Destroy(s_ulVaUiSemId);
}

void VA_UI_Adapt_SetMsgQid(HUINT32 ulMsgQId)
{
	HxLOG_Print("[Enter]\n");

	sVaUiAdaptEventQid = ulMsgQId;

	HxLOG_Print("[Exit]\n");
}

static HBOOL s_bCamlockMsgReceived = FALSE;
void VA_UI_Adapt_SendEvent(VA_UI_EVENT *pEvent, SvcCas_MsgType_e eMsgType, SvcCas_ShowType_e eShowType)
{
	Handle_t hAction;
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	int i, cnt;
	Handle_t hVaAction, hMainAction;
#endif

	HxLOG_Print("[Enter]\n");

	/* VHDR-3000S ÀÇ °æ¿ì ºÎÆÃÈÄ insert smartcard ¸Þ½ÃÁö ³ª¿À´Â ¹®Á¦ ¼öÁ¤À» À§ÇØ ¾Æ·¡¿Í °°Àº Ã³¸®°¡ Ãß°¡ µÇ¾î ÀÖÀ¸³ª
	    º£ÀÌ½º ÄÚµåÀÇ °æ¿ì¿£ local_cas_va_adapt_message_function() ³»¿¡¼­ Ã³¸®ÇÑ´Ù (VA_UI_VIACCESS_SMARTCARD_NOT_FOUND). */
#if 0 // #ifdef CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_HIGH
	if (VA_GetCamLockControlLevel())
	{
		if (s_bCamlockMsgReceived == FALSE)
		{
			switch (pEvent->eEventType)
			{
			case eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT:
			case eVA_UI_CAMLOCK_DISABLED_EVENT:
			case eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT:
			case eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT:
			case eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT:
				s_bCamlockMsgReceived = TRUE;
#ifdef VA_BBX_INIT_CHECK // BBX data update°¡ µÇ¾ú´ÂÁö È®ÀÎÇÏ±â À§ÇÑ ÀÓ½Ã Á¶Ä¡..
			VA_UTIL_SetBbxInit();
#endif
				break;

			default:
				HxLOG_Info("CAMLOCK msg is not received yet. ignore other messages [%x]\n", pEvent->eEventType);
				return;
			}
		}
	}
#endif

	VA_ASSERT(s_ulVaUiSemId);
	VA_SEM_Get(s_ulVaUiSemId);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(pEvent->ulAcsId);
	hAction = VAMGR_GetActionHandle(hVaAction);

	HxLOG_Print("[action:%x,evt:%x,msg:%x,show:%x. eDscrErrEsType:%x]\n", hAction, pEvent->eEventType, eMsgType, eShowType, pEvent->eDscrErrEsType);

	if (hAction != HANDLE_NULL)
	{
#ifdef CONFIG_DEBUG
VA_OS_Printf("**** SEND EVENT:(act:%x,%s,%x).msg:%s, show:%x\n", hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), eShowType);
#endif

		VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
		VA_UI_INT_CheckEventForRecord(eMsgType, &s_stUiEventBuf[s_ulUiEventBufIdx]);

		svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);

		s_ulUiEventBufIdx++;
		if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
			s_ulUiEventBufIdx = 0;
	}
	else
	{
		/*
			¿øÀÎÆÄÀÍ½Ã±îÁö ÀÓ½Ã·Î... ½ÇÁ¦ ¾Æ·¡ ÁÖ¼®Ã³·³ µ¿ÀÛÇØ¾ß ¸Â´Âµ¥...
			industrial test½Ã streamÀÌ ³¡³­ ÈÄ main view handleÀÌ »ç¶óÁö´Â °æ¿ì°¡ ÀÖ¾î
			¾Æ·¡ VA_PI_Adapt_IsProgramSwitchOn ÀÌÇÏ¿¡¼­ event message¸¦ ¿Ã¸®Áö ¸øÇÏ´Â °æ¿ì°¡ ÀÖ´Ù.
			main actionÀ¸·Î´Â ¹«Á¶°Ç º¸³»°í ³ª¸ÓÁö action¿¡ ´ëÇØ¼­´Â ¼±ÅÃÀûÀ¸·Î º¸³»ÀÚ.
		*/
		hMainAction = hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());

#ifdef CONFIG_DEBUG
VA_OS_Printf("**** !!SEND EVENT:(act:%x,%s,%x).msg:%s, show:%x\n", hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), eShowType);
#endif
		VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
		VA_UI_INT_CheckEventForRecord(eMsgType, &s_stUiEventBuf[s_ulUiEventBufIdx]);

		svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);

		s_ulUiEventBufIdx++;
		if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
			s_ulUiEventBufIdx = 0;

		// ¸ðµç actionÀ¸·Î º¸³½´Ù..
		for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
		{
			hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(i);
			if (VA_PI_Adapt_IsProgramSwitchOn(hVaAction))
			{
				hAction = VAMGR_GetActionHandle(hVaAction);

				// Main Action¿¡´Â ÀÌ¹Ì º¸³Â´Ù.
				if(hMainAction == hAction)
					continue;

#ifdef CONFIG_DEBUG
VA_OS_Printf("**** [%d]SEND EVENT:(act:%x,%s,%x).msg:%s, show:%x\n", i, hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), eShowType);
#endif

				VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
				VA_UI_INT_CheckEventForRecord(eMsgType, &s_stUiEventBuf[s_ulUiEventBufIdx]);

				svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);
				s_ulUiEventBufIdx++;
				if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
					s_ulUiEventBufIdx = 0;
			}
		}
/*
		// ¸ðµç actionÀ¸·Î º¸³½´Ù..
		for (i = 0, cnt = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
		{
			hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(i);
			if (VA_PI_Adapt_IsProgramSwitchOn(hVaAction))
			{
				hAction = VAMGR_GetActionHandle(hVaAction);
#ifdef CONFIG_DEBUG
VA_OS_Printf("**** [%d]SEND EVENT:(act:%x,%s,%x).msg:%s, show:%x\n", i, hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), eShowType);
#endif

				VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
				VA_UI_INT_CheckEventForRecord(eMsgType, &s_stUiEventBuf[s_ulUiEventBufIdx]);
				svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);
				cnt++;

				s_ulUiEventBufIdx++;
				if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
					s_ulUiEventBufIdx = 0;
			}
		}
		// ¾Æ¹«°Íµµ program switch on µÈ°Ô ¾øÀ» °æ¿ì, live¸¦ Ã£¾Æ¼­ º¸³½´Ù.
		if (cnt == 0)
		{
			hAction = VAMGR_GetActionHandle(HANDLE_NULL);
			if(hAction == HANDLE_NULL)
			{
				hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
			}
#ifdef CONFIG_DEBUG
VA_OS_Printf("**** !!SEND EVENT:(act:%x,%s,%x).msg:%s, show:%x\n", hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), eShowType);
#endif

			VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
			VA_UI_INT_CheckEventForRecord(eMsgType, &s_stUiEventBuf[s_ulUiEventBufIdx]);
			svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);

			s_ulUiEventBufIdx++;
			if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
				s_ulUiEventBufIdx = 0;
		}
*/
	}
#else
	hAction = VAMGR_GetActionHandle(VA_PI_Adapt_GetVaActionHandleByAcsId(pEvent->ulAcsId));
	if (hAction == HANDLE_NULL)
	{
		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
	}
#ifdef CONFIG_DEBUG
	HxLOG_Info("[Action:%x][Event:%s(%x)][Msg:%s][Show:%s][Type:%d]\n",
		hAction, VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), VA_UI_Debug_GetShowTypeName(eShowType), pEvent->eDscrErrEsType);
#endif
	VA_memcpy(&s_stUiEventBuf[s_ulUiEventBufIdx], pEvent, sizeof(VA_UI_EVENT));
	svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_VA, (void *)&s_stUiEventBuf[s_ulUiEventBufIdx], eShowType);
	s_ulUiEventBufIdx++;
	if (s_ulUiEventBufIdx == VA_UI_EVENT_BUFFER_SIZE)
		s_ulUiEventBufIdx = 0;
#endif

	VA_SEM_Release(s_ulVaUiSemId);

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_SendDescramblingEvent(VA_UI_EVENT *pEvent)
{
	SvcCas_MsgType_e eMsgType;
	SvcCas_ShowType_e eShowType;
	VA_UI_INFO *pUiInfo;

	HxLOG_Print("[Enter]\n");

	pUiInfo = VA_PI_GetUiInfoByAcsId(pEvent->ulAcsId);
	if (pUiInfo == NULL)
		return;

	//## (audio change µîÀ¸·Î) È¤½Ã ok/ko°¡ ¹Ù²ð¼öµµ ÀÖÀ¸´Ï  ´Ù½ÃÇÑ¹ø È®ÀÎÇØ¼­ ¹Ù²î¾úÀ¸¸é Àû¿ëÇØ¼­ º¸³»ÀÚ..
	pUiInfo->stLastEvent.eDscrErrEsType = VA_PI_GetStreamStateForUi(pEvent->ulAcsId);

	if (pUiInfo->stLastEvent.eDscrErrEsType == VA_ES_NONE)
	{
		pUiInfo->stLastEvent.eEventType = pUiInfo->eEventOk;
//ÀÌ°É·Î´Â preview Ã³¸®°¡ ¾ÈµÇ..		eMsgType = eCAS_MSG_TYPE_Ok;
		eMsgType = pUiInfo->eMsgTypeOk;
		eShowType = eCasShowType_Both;
	}
	else
	{
		pUiInfo->stLastEvent.eEventType = pUiInfo->eEventKo;
		// ko´Â failÀÏ¼öµµ, commandÀÏ¼öµµ ÀÖ´Â°Å ¾Æ´Ñ°¡??
		eMsgType = pUiInfo->eMsgTypeKo;
		if (pUiInfo->stLastEvent.eDscrErrEsType == VA_ES_ALL)
			eShowType = eCasShowType_None;
		else if (pUiInfo->stLastEvent.eDscrErrEsType == VA_ES_AUDIO)
			eShowType = eCasShowType_VideoOnly;
		else if (pUiInfo->stLastEvent.eDscrErrEsType == VA_ES_VIDEO)
			eShowType = eCasShowType_AudioOnly;
		else
			eShowType = eCasShowType_Both;
	}
#ifdef CONFIG_DEBUG
	HxLOG_Info("[Event:%s(%x)][Msg:%s][Show:%s]\n",
		VA_UI_Debug_GetEventName(pUiInfo->stLastEvent.eEventType), pUiInfo->stLastEvent.eEventType, VA_UI_Debug_GetMsgTypeName(eMsgType), VA_UI_Debug_GetShowTypeName(eShowType));
#endif
	//## timeoutµÇ¾úÀ¸´Ï ÀÌº¥Æ® Àü´Þ
	VA_UI_Adapt_SendEvent(&pUiInfo->stLastEvent, eMsgType, eShowType);

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	if (pUiInfo->stLastEvent.eEventType == eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT)
	{
		VA_PVR_AutoPinForRecord(pEvent->ulAcsId, pEvent->ulScSlot);
	}
#if defined(CONFIG_OP_NTVPLUS)
	// NTV¿¡¼­´Â ½ÃÃ»±ÇÇÑÀÌ ¾øÀ» ¶§µµ recording Á¦ÇÑ ÇÏ±â¸¦ ¿øÇÔ
	// ´Ü maturity ratingÀÏ °æ¿ì´Â º°µµ·Î Ã³¸®ÇÔ. pin ÀÔ·ÂÀü¿¡ recording failµÇ´Â ÀÌ½´ ÀÖÀ½.
	else
	{
		if (eShowType != eCasShowType_Both)
		{
			// timeshift´Â º°°³ÀÓ..
			VA_PI_Adapt_SetRecordForbidden(pEvent->ulAcsId);
		}
	}
#endif
#endif

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_cbEventTimer(unsigned long ulParam, void *pvParam)
{
	VA_UI_INFO *pUiInfo;
	HUINT32 ulAcsId;

	HxLOG_Print("[Enter]\n");

	ulAcsId = *((HUINT32*)pvParam);

	pUiInfo = VA_PI_GetUiInfoByAcsId(ulAcsId);
	if (pUiInfo == NULL)
		return;

	if (ulParam != pUiInfo->ulTimerId)
		return;

	pUiInfo->ulTimerId = 0;

	//## timeoutµÇ¾úÀ¸´Ï ÀÌº¥Æ® Àü´Þ
	VA_UI_Adapt_SendDescramblingEvent(&pUiInfo->stLastEvent);
	pUiInfo->nReceivedEvtNum = 0;

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_SendLastEvent(int nAcsId)
{
	VA_UI_INFO *pUiInfo;

	HxLOG_Print("[Enter]\n");

	pUiInfo = VA_PI_GetUiInfoByAcsId(nAcsId);
	if (pUiInfo == NULL)
		return;

	//## ¸¶Áö¸· ÀÌº¥Æ® ´Ù½Ã
	if (pUiInfo->stLastEvent.ulAcsId < VA_PI_MAX_ACS_INSTANCE) // ¸¶Áö¸· ÀÌº¥Æ®°¡ À¯È¿ÇÑ °æ¿ì¿¡¸¸ Ã³¸®ÇÏµµ·Ï Á¶°Ç Ãß°¡ÇÔ.
		VA_UI_Adapt_SendDescramblingEvent(&pUiInfo->stLastEvent);

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_MsgHandler(VA_UI_EVENT *pEvent)
{
	VA_UI_INFO *pUiInfo = NULL;
	SvcCas_MsgType_e eMsgType = eCAS_MSG_TYPE_None;
	SvcCas_ShowType_e eShowType = eCasShowType_None;
	HBOOL	bGlobalEvent = TRUE;		// ½ºÆ®¸²ÀÌ³ª acs instance¿Í ¿¬°ü¾ø´Â ÀÌº¥Æ®,, s/c µîµî..

	HxLOG_Print("[Enter]\n");

#ifdef CONFIG_DEBUG
	HxLOG_Print("[EVENT][%s(%x)][acs:%d][Type:%d][Number:%d][Handle:%x]\n", VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType,
		pEvent->ulAcsId, pEvent->nStreamType, pEvent->nStreamNumber, pEvent->ulStbStreamHandle);
#endif

	if (pEvent == NULL)
	{
		HxLOG_Error("pEvent is NULL\n");
		return;
	}

//## ÀÌº¥Æ® º°·Î ¾î¶² Ã³¸®¸¦ ÇÒÁö °áÁ¤ÇÑ´Ù. (ACS3.1 ¹öÀü)
//## s/c, camlock, security °ü·Ã --> ¹Ù·Î ¿Ã·ÁÁØ´Ù..
//## camlock, security restrictionÀÌ top level, priority event°¡ ±×´ÙÀ½ level
	switch (pEvent->eEventType)
	{
// UC01 priority event
	case eVA_UI_SMARTCARD_NOT_ACCEPTED_EVENT: // "Invalid smartcard." / Indicates that Viaccess detects that the inserted smartcard is not a Viaccess smartcard.
	case eVA_UI_SMARTCARD_FAILED_EVENT: // "Smartcard error." / Indicates that a communication problem has occurred with the Viaccess smartcard.
	case eVA_UI_SMARTCARD_INVALIDATED_EVENT: // "The smartcard has been invalidated by the operator. Please contact the operator for further information." / Indicates that the smartcard has been invalidated.
		if (pEvent->eEventType == eVA_UI_SMARTCARD_NOT_ACCEPTED_EVENT)
			s_nScState = VA_UI_SMARTCARD_NOT_ACCEPTED;
		else if (pEvent->eEventType == eVA_UI_SMARTCARD_FAILED_EVENT)
			s_nScState = VA_UI_SMARTCARD_FAILED;
		else
			s_nScState = VA_UI_SMARTCARD_INVALIDATED;
//## ¹«Á¶°Ç È­¸é¿¡ Ç¥½ÃÇØ¾ß ÇÔ!!
//## show = none
//## type = Æ¯º°ÇÑ°É ½á¾ßµÇ..

// card invalid¸¦ ÇÏ´Ï, sc removed·Î Ã³¸®ÇØ¹ö¸®³× -_-;
//		VA_SC_Adapt_ValidCard(pEvent->ulScSlot, FALSE, eDxCAS_GROUPID_VA, VA_MAIN_ACS_NUM);
		if (VA_GetCamLockControlLevel())
			eMsgType = eCAS_MSG_TYPE_Command;
		else
			eMsgType = eCAS_MSG_TYPE_Fail;
		break;

// UC02 simple notify (SC0201/SC0202: with/without video displayed) [PVR]
	case eVA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED_EVENT: // "You cannot view this <specific text> because the visualization date has expired."
	case eVA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED_EVENT: // "You cannot view this <specific text> because the maximum number of playbacks has been reached."

// UC02 simple notify (SC0201/SC0202: with/ithout video displayed)
	case eVA_UI_DESCRAMBLING_KO_NO_RIGHT_EVENT: // "You do not have the rights to access the <specific text>."
	case eVA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT_EVENT: // "Geographical blackout for the <specific text>."
	case eVA_UI_DESCRAMBLING_KO_MISSING_KEY_EVENT: // "Access not granted because the key is not present. If this <specific text> is in your subscription please contact your operator."
	case eVA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE_EVENT: // "Access to this <specific text> has been forbidden by the operator."
	// event for schip only
	case eVA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH_EVENT: // "Terminal configuration is not accurate. Please contact your operator for further information."
//## show = audio/video °æ¿ì¿¡ µû¶ó¼­..
		bGlobalEvent = FALSE;
		eMsgType = eCAS_MSG_TYPE_Fail;
		break;

// UC02 simple notify (SC0201: without video displayed)
	case eVA_UI_SMARTCARD_EXTRACTED_EVENT: // "Please insert your smartcard." (Note: Displayed only when the program is scrambled.) / Indicates that the smartcard has been extracted.
		s_nScState = VA_UI_SMARTCARD_EXTRACTED;
		#if 0 // Ç×»ó wrong handle ·Î ¿¡·¯ ¸®ÅÏµÇ°í ¾Æ¹«·± ÀÏµµ ÇÏÁö ¾ÊÀ¸¹Ç·Î ¸·À½.
		VA_SC_Adapt_ValidCard(pEvent->ulScSlot, FALSE, eDxCAS_GROUPID_VA, VA_MAIN_ACS_NUM);
		#endif
//## show = none
		if (VA_GetCamLockControlLevel())
			eMsgType = eCAS_MSG_TYPE_Command;
		else
			eMsgType = eCAS_MSG_TYPE_Fail;
		break;

	case eVA_UI_VIACCESS_NOT_APPLICABLE_EVENT: // "Not a Viaccess scrambled program."
	case eVA_UI_VIACCESS_SMARTCARD_NOT_FOUND_EVENT: // "Please insert the smartcard to access the program." / Indicates that Viaccess cannot unscramble the stream because the smartcard inserted in the STB is not ready (the stream is scrambled by Viaccess).
		bGlobalEvent = FALSE;

	case eVA_UI_SMARTCARD_NEEDED_EVENT: // "Please insert your smartcard." / Indicates that Viaccess requires the smartcard to be present in the smartcard reader in order to carry out its functions.
	case eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_EVENT: // "Smartcard memory is full." / Indicates that there is not enough memory on the smartcard.
	case eVA_UI_PAIRING_MISMATCH_EVENT: // "Wrong Card inserted. Please contact the operator for further information."
//## show = none
		eMsgType = eCAS_MSG_TYPE_Fail;
		break;

// UC02 simple notify (SC0203: without banner)
	case eVA_UI_SMARTCARD_READY_EVENT: // Indicates that the smartcard is ready to be used.
		s_nScState = VA_UI_SMARTCARD_READY;
		VA_SC_Adapt_ValidCard(pEvent->ulScSlot, TRUE, eDxCAS_GROUPID_VA, VA_MAIN_ACS_NUM);
		eShowType = eCasShowType_Both;
		if (VA_GetCamLockControlLevel())
			eMsgType = eCAS_MSG_TYPE_Command;
		else
			eMsgType = eCAS_MSG_TYPE_Ok;
		break;

	case eVA_UI_DESCRAMBLING_OK_RIGHT_OK_EVENT: // only when both audio and video streams are not scrambled / Indicates that the access conditions have changed and that the descrambling process is successful.
	case eVA_UI_FREE_TO_AIR_EVENT: // this event cancels messages about the smartcard status, until  the next right-restricting message
		bGlobalEvent = FALSE;
		eShowType = eCasShowType_Both;
		eMsgType = eCAS_MSG_TYPE_Ok;
		break;

	case eVA_UI_STOP_SECURITY_RESTRICTION_EVENT:
		s_nSecurity = 0;
	case eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT: // Indicates that the CAMLOCK feature is deactivated. This feature can be activated by air.
	case eVA_UI_CAMLOCK_DISABLED_EVENT: // Indicates that the CAMLOCK feature is not available in the STB. In this case, any Viaccess smartcard is accepted.
	case eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT: // Indicates that the CAMLOCK feature is activated and that the STB and the smartcard belong to the same operator. with dwScSlot.
		if (pEvent->eEventType == eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT)
			s_nCamlock = VA_UI_CAMLOCK_NOT_ACTIVATED;
		else if (pEvent->eEventType == eVA_UI_CAMLOCK_DISABLED_EVENT)
			s_nCamlock = VA_UI_CAMLOCK_DISABLED;
		else if (pEvent->eEventType == eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT)
			s_nCamlock = VA_UI_CAMLOCK_ACTIVATED_OK;

//ÀÌ°Ç ÇÊ¿ä¾øÁö ¾Ê³ª..	case eVA_UI_SMARTCARD_INSERTED_EVENT:
//## show = °æ¿ì¿¡ µû¶ó¼­...
		eShowType = eCasShowType_Both;
		eMsgType = eCAS_MSG_TYPE_Command; // cmd´Â ÇØÁ¦ÇÒ¶§µµ cmd¸¦ º¸³»¾ßÇÏ³×.. eCAS_MSG_TYPE_Ok;
		break;

	case eVA_UI_SMARTCARD_INSERTED_EVENT: // default case ¿¡¼­ ¿¡·¯ ÂïÈ÷Áö ¾Êµµ·Ï Ãß°¡ÇÔ + s_nScState °ª º¯°æ Ãß°¡ÇÔ.
		s_nScState = VA_UI_SMARTCARD_INSERTED;
		break;

// UC03 operator message
	case eVA_UI_NEW_OPERATOR_MESSAGE_EVENT:
	case eVA_UI_NEW_OPERATOR_DATA_EVENT:
//## show = both
		eShowType = eCasShowType_Both;
		eMsgType = eCAS_MSG_TYPE_Command;
		break;

// UC04 camlock
	case eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT: // Indicates that the CAMLOCK feature is activated and that the STB and the smartcard do not belong to the same operator. with dwScSlot.
	case eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT: // Indicates that the CAMLOCK feature is active, and that no smartcard is inserted in the STB
		if (pEvent->eEventType == eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT)
			s_nCamlock = VA_UI_CAMLOCK_ACTIVATED_KO;
		else
			s_nCamlock = eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT;
//## ¹«Á¶°Ç È­¸é¿¡ Ç¥½ÃÇØ¾ß ÇÔ!!
//## show = none
//## type = Æ¯º°ÇÑ°É ½á¾ßµÇ..
		eMsgType = eCAS_MSG_TYPE_Command; //eCAS_MSG_TYPE_Fail;
		break;

// UC05 security restriction
	case eVA_UI_START_SECURITY_RESTRICTION_EVENT: // "A security restriction has been activated on this STB!Please contact your operator."
//## eVA_UI_STOP_SECURITY_RESTRICTION_EVENT ¹ß»ý ¶§±îÁö´Â ¹«Á¶°Ç black screen & banner À¯Áö. ÀÌ »óÅÂ¿¡¼­ Ã¤³Î ÀüÈ¯ °¡´ÉÇØ¾ß ÇÏ°í ¸Þ´ºµµ Á¢±Ù °¡´ÉÇØ¾ß ÇÔ.
//## ¹«Á¶°Ç È­¸é¿¡ Ç¥½ÃÇØ¾ß ÇÔ!!
//## show = none
//## type = Æ¯º°ÇÑ°É ½á¾ßµÇ..
		s_nSecurity = 1;
		eMsgType = eCAS_MSG_TYPE_Command; //eCAS_MSG_TYPE_Fail;
		break;

// UC06 maturity rating agreement
	case eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT:
		bGlobalEvent = FALSE;
//## show = none
		eMsgType = eCAS_MSG_TYPE_Check;
		break;

// UC07 preview
	case eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT:
		bGlobalEvent = FALSE;
		eShowType = eCasShowType_Both;
		eMsgType = eCAS_MSG_TYPE_Check;
		break;

//## ÀÌ°Ç ±âÁ¸ ÀÌº¥Æ®°¡ À¯ÁöµÇ°í ÀÖ´Ù´Â °Å...
	case eVA_UI_DESCRAMBLING_OK_EVENT: // only when both audio and video streams are not scrambled / Indicates that the descrambling process is successful.
		eMsgType = eCAS_MSG_TYPE_Ok;
		bGlobalEvent = FALSE;
		break;

	case eVA_UI_DESCRAMBLING_KO_EVENT: // Indicates that the unscrambling process has failed.
		eMsgType = eCAS_MSG_TYPE_Fail;
		bGlobalEvent = FALSE;
		break;

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
	// event for ippv only

	// UC02 simple notify (SC0201/SC0202: with/without video displayed)
	case eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED_EVENT:
	case eVA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT_EVENT:

	// UC02 simple notify (SC0201: without video displayed)
	case eVA_UI_OVERDRAFT_REACHED_EVENT:
	case eVA_UI_THRESHOLD_REACHED_EVENT:

	case eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EVENT:							// UC07
	case eVA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER_EVENT:			// UC07
	case eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER_EVENT:	// UC07
		HxLOG_Info("[VA_UI_Task] event (%d) is for IPPV only: not implemented\n", pEvent->eEventType);
		break;
#endif

	// event for pvr only

// UC02 simple notify (SC0201: without video displayed)
	case eVA_UI_RECORD_RIGHT_KO_EVENT: // "You do not have the rights to record this <specific text>."
	case eVA_UI_TIMESHIFT_RIGHT_KO_EVENT: // "You do not have the rights to record this <specific text>."
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		// VA_PI_NotifyStreamRecordPolicy()¿¡¼­ Ã³¸®ÇÔ.
		// do nothing
		break;
#endif

// UC02 simple notify (SC0203: without banner)
	case eVA_UI_RECORD_RIGHT_OK_EVENT:
	case eVA_UI_TIMESHIFT_RIGHT_OK_EVENT:
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		// VA_PI_NotifyStreamRecordPolicy()¿¡¼­ Ã³¸®ÇÔ.
		// do nothing
		break;
#endif

	case eVA_UI_RECORD_INFO_CHANGE_EVENT:
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		VA_PVR_SetRecordInfo(pEvent->ulAcsId, pEvent->ulStbStreamHandle);
#else
		HxLOG_Error("event (%x) is for PVR only: not implemented\n", pEvent->eEventType);
#endif
		break;

#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
	// event for purple box or IP box only (must not be implemented)

	// UC02 simple notify (SC0201: without video displayed)
	case eVA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED_EVENT: // "Please activate the terminal to access the program."
	case eVA_UI_TERMINAL_CONFIGURATION_ERROR_EVENT: // "The terminal configuration is corrupted. Please contact the operator for further information."
	case eVA_UI_TERMINAL_INSUFFICIENT_MEMORY_EVENT: // "The terminal configuration is full. Please contact the operator for further information."
	case eVA_UI_TERMINAL_INVALIDATED_EVENT: // "The terminal has been invalidated by the operator. Please contact the operator for further information."
	case eVA_UI_TERMINAL_PERSONALIZATION_KO_EVENT: // "Personalization of the terminal failed. Please contact your operator."

	case eVA_UI_ORM_CONNECTION_OK_EVENT:
	case eVA_UI_ORM_CONNECTION_KO_EVENT:
	case eVA_UI_CW_SERVER_CONNECTION_OK_EVENT:
	case eVA_UI_CW_SERVER_CONNECTION_KO_EVENT:
	case eVA_UI_TERMINAL_NOT_ACTIVATED_EVENT:
	case eVA_UI_TERMINAL_READY_EVENT:
	case eVA_UI_TERMINAL_PERSONALIZATION_OK_EVENT:
		HxLOG_Error("event (%x) is for Purple box only: not implemented\n", pEvent->eEventType);
		break;
#endif

	default:
		HxLOG_Error("event (%x) is unknown\n", pEvent->eEventType);
		break;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Info("\n[01;34m[Event:%s(%x)][Slot:%d][AcsId:%d][Handle:%x][%d][Num:%d][Type:%d]->[Msg:%s][Show:%s][%s][00m\n",
		VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType,
		pEvent->ulScSlot, pEvent->ulAcsId,
		pEvent->ulStbStreamHandle,
		pEvent->bIsScrambledByProgram,
		pEvent->nStreamNumber,
		pEvent->nStreamType,
		VA_UI_Debug_GetMsgTypeName(eMsgType), VA_UI_Debug_GetShowTypeName(eShowType), bGlobalEvent == TRUE ? "Global" : "Local");
#endif
//## global event¶ó¸é º°µµ·Î ÀúÀå, acsId°¡ ÀÖ´Ù¸é ÇØ´ç acs instance¿¡ ÀúÀå
	if (eMsgType == eCAS_MSG_TYPE_None)
	{
		// AP·Î Àü´ÞÇÒ ÇÊ¿ä ¾ø´Â ÀÌº¥Æ®..
	}
	else if (bGlobalEvent)
	{
		VA_memcpy(&s_stLastEvent, pEvent, sizeof(VA_UI_EVENT));
		//## ¹Ù·Î Àü´Þ..
		VA_UI_Adapt_SendEvent(&s_stLastEvent, eMsgType, eShowType);
	}
	else
	{
		pUiInfo = VA_PI_GetUiInfoByAcsId(pEvent->ulAcsId);

		if (pUiInfo != NULL)
		{
			//## ¿©±â¼­ global event´Â Áö¿ö¾ß ÇÏ·Á³ª?? ¾Æ¸®±î¸®ÇÏ³×¿ä..¤¾¤¾;;

			if (pEvent->eEventType == eVA_UI_DESCRAMBLING_KO_EVENT || pEvent->eEventType == eVA_UI_DESCRAMBLING_OK_EVENT)
			{
				//## eVA_UI_DESCRAMBLING_KO_EVENT/eVA_UI_DESCRAMBLING_OK_EVENT ´Â ÀÌÀü eventÀÇ ¹Ýº¹ÀÓ.. ±âÁ¸ event À¯Áö..
				//## È¤½Ã ´Ù¸¥ Ã³¸®°¡ ÇÊ¿äÇÑ °æ¿ì°¡ ÀÖÀ» Áöµµ ¸ð¸§...preview¶óµç°¡.. È®ÀÎÇØºÁ¾ß..
				if (pUiInfo->eEventOk == eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT ||
					pUiInfo->eEventKo == eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT)
				{
					VA_UI_Adapt_SendLastEvent(pEvent->ulAcsId);
				}
			}
			else
			{
				//## ÀÌº¥Æ® Àü´Þ Á¶°ÇÀº...
				//## 1) nStreamNumber¸¸Å­ µ¿ÀÏ ÀÌº¥Æ®¸¦ ¹ÞÀ½
				//## 2) timeout ¹ß»ý..

				//## ÀÏ´Ü µ¹°íÀÖ´ø timer °¡ ÀÖÀ¸¸é Á¤Áö.. ÇÊ¿äÇÏ¸é ´Ù½Ã ¼¼ÆÃÇÔ..
				if (pUiInfo->ulTimerId)
					VK_TIMER_Cancel(pUiInfo->ulTimerId);
				pUiInfo->ulTimerId = 0;

				//## °°Àº ÀÌº¥Æ®ÀÎÁö È®ÀÎ
				//## nReceivedEvtNum = 0ÀÌ¸é ºó»óÅÂÀÌ°Å³ª event¸¦ º¸³»°í ³­ ÈÄÀÓ.. (È¤½Ã³ª µ¿ÀÏÇÑ ÀÌº¥Æ®°¡ °è¼Ó ¿À´Â °æ¿ì°¡ ÀÖÀ»±îºÁ..)
				if (pEvent->ulAcsId == pUiInfo->stLastEvent.ulAcsId && pUiInfo->nReceivedEvtNum > 0)
				{
					//## °°Àº ¼­ºñ½º¿¡ ´ëÇÑ ÀÌº¥Æ®°¡ ¶Ç ¿Ô³×.. (´Ù¸¥ es¿¡ ´ëÇÑ..)
					pUiInfo->nReceivedEvtNum++;

					//## a/v block¿¡ °ü¿©ÇÏ´Â show type Ã³¸®
					if (eShowType == eCasShowType_Both)
					{
						// OK event
						if (pEvent->nStreamType == eEsType_Video)
							pUiInfo->eShowType |= eCasShowType_VideoOnly;
						else if (pEvent->nStreamType == eEsType_Audio)
							pUiInfo->eShowType |= eCasShowType_AudioOnly;

						pUiInfo->eEventOk = pEvent->eEventType;
						pUiInfo->eMsgTypeOk = eMsgType;
					}
					else
					{
						// KO event
						if (pEvent->nStreamType == eEsType_Video)
							pUiInfo->eShowType &= ~eCasShowType_VideoOnly;
						else if (pEvent->nStreamType == eEsType_Audio)
							pUiInfo->eShowType &= ~eCasShowType_AudioOnly;

						pUiInfo->eEventKo = pEvent->eEventType;
						// KOÀÇ °ÍÀ» ±â·ÏÇØµÐ´Ù. OK´Â OKÀÎ°Ô »·ÇÏÀÚ³ª.. failÀº fail/command..
						pUiInfo->eMsgTypeKo = eMsgType;
					}
				}
				else
				{
					// »õ·Î¿î ÀÌº¥Æ®³×..
					//## ¹Ù²ï ÀÌº¥Æ® ÀúÀå
					VA_memcpy(&pUiInfo->stLastEvent, pEvent, sizeof(VA_UI_EVENT));
					pUiInfo->nReceivedEvtNum = 1;

					//## a/v block¿¡ °ü¿©ÇÏ´Â show type Ã³¸®
					if (eShowType == eCasShowType_Both)
					{
						// OK event
						if (pEvent->nStreamType == eEsType_Video)
							pUiInfo->eShowType = eCasShowType_VideoOnly;
						else if (pEvent->nStreamType == eEsType_Audio)
							pUiInfo->eShowType = eCasShowType_AudioOnly;
						else
							pUiInfo->eShowType = eCasShowType_None;

						pUiInfo->eEventOk = pEvent->eEventType;
//ok,ko°¡ ¼¯ÀÏ°æ¿ì...	pUiInfo->eEventKo = eVA_UI_LAST_EVENT;
						pUiInfo->eMsgTypeOk = eMsgType;
					}
					else
					{
						// KO event
						pUiInfo->eShowType = eCasShowType_None;

//ok,ko°¡ ¼¯ÀÏ°æ¿ì...	pUiInfo->eEventOk = eVA_UI_LAST_EVENT;
						pUiInfo->eEventKo = pEvent->eEventType;
						pUiInfo->eMsgTypeKo = eMsgType;
					}

				}

				// ¿øÇÏ´Â °³¼ö¸¸Å­ event°¡ ¿ÔÀ¸¸é ¹Ù·Î Àü´Þ.. ¾Æ´Ï¸é timer ¼¼ÆÃ
				if (pUiInfo->nReceivedEvtNum >= pEvent->nStreamNumber)
				{
					//## ÀÌº¥Æ® Àü´Þ
					VA_UI_Adapt_SendDescramblingEvent(&pUiInfo->stLastEvent);
					pUiInfo->nReceivedEvtNum = 0;
				}
				else
				{
					if(VK_TIMER_EventAfter(VA_UI_EVENT_TIMEOUT, VA_UI_Adapt_cbEventTimer, (void*)&pEvent->ulAcsId, sizeof(HUINT32), &pUiInfo->ulTimerId) != VK_OK)
					{
						//## ¸¸¾à timer ¼³Á¤¿¡ ½ÇÆÐÇÏ¸é Áö±Ý °Å¶óµµ ±×³É º¸³¾±î? ¾Æ´Ô assert¸¦ ÇØ¹ö¸±±î?
						HxLOG_Info("UI event receive timer state : set\n");
					}
				}
			}
		}
	}

	HxLOG_Print("[Exit]\n");
}

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
void VA_UI_Adapt_ShowFingerprint(HUINT8 *pszFingerprintStr)
{
	static VA_UI_EVENT stEvent;

	HxLOG_Print("[Enter]\n");

	VA_memset(&stEvent, 0, sizeof(VA_UI_EVENT));
	stEvent.eEventType = VA_UI_SHOW_FINGERPRINT;
	stEvent.ulScSlot = (HUINT32)pszFingerprintStr;
	stEvent.ulStbStreamHandle = 0;
	VA_UI_Adapt_SendEvent(&stEvent, eCAS_MSG_TYPE_Command, eCasShowType_Both);

	HxLOG_Print("[Exit]\n");
}

void VA_UI_Adapt_HideFingerprint()
{
	static VA_UI_EVENT stEvent;

	HxLOG_Print("[Enter]\n");

	VA_memset(&stEvent, 0, sizeof(VA_UI_EVENT));
	stEvent.eEventType = VA_UI_HIDE_FINGERPRINT;
	stEvent.ulStbStreamHandle = 0;
	VA_UI_Adapt_SendEvent(&stEvent, eCAS_MSG_TYPE_Command, eCasShowType_Both);

	HxLOG_Print("[Exit]\n");
}
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
void VA_UI_Adapt_FreeChannelBlock(int fBlock)
{
	static VA_UI_EVENT stEvent;

	HxLOG_Print("[Enter(block:%d)]\n", fBlock);

	VA_memset(&stEvent, 0, sizeof(VA_UI_EVENT));
	if (fBlock)
	{
		stEvent.eEventType = VA_UI_FREE_CHANNEL_BLOCK;
		VA_UI_Adapt_SendEvent(&stEvent, eCAS_MSG_TYPE_Fail, eCasShowType_None);
	}
	else
	{
		stEvent.eEventType = VA_UI_FREE_CHANNEL_BLOCK_STOP;
		VA_UI_Adapt_SendEvent(&stEvent, eCAS_MSG_TYPE_Ok, eCasShowType_Both);
	}

	HxLOG_Print("[Exit]\n");
}
#endif

// do not free return value
HUINT8 *VA_UI_Adapt_GetStbId()
{
	HUINT8 aucStbId[kVA_UI_STB_ID_SIZE];

	HxLOG_Print("[Enter]\n");

	if (s_pszStbId == NULL)
	{
		s_pszStbId = (HUINT8 *)VA_MEM_Alloc(kVA_UI_STB_ID_SIZE*2+1);
		if (s_pszStbId == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	if (VA_UI_GetStbId(aucStbId, NULL, NULL) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetStbId() failed\n");
		return NULL;
	}

	VA_UI_INT_ConvertHex2Str(s_pszStbId, aucStbId, kVA_UI_STB_ID_SIZE);

	HxLOG_Print("[Exit]\n");
	return s_pszStbId;
}

// do not free return value
HUINT8 *VA_UI_Adapt_GetUA(HINT32 nScSlot)
{
	HUINT8 aucUA[kVA_UI_UA_SIZE];

	HxLOG_Print("[Enter]\n");

	if (s_pszUa == NULL)
	{
//		s_pszUa = (HUINT8 *)OxCAS_Malloc(VA_UI_UA_SIZE*2+1);
		s_pszUa = (HUINT8 *)OxCAS_Malloc(VA_UI_UA_SIZE*4);
		if (s_pszUa == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	if (VA_UI_GetUA(nScSlot, (tVA_UI_Ua *)&aucUA) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetUA() failed\n");
		return NULL;
	}

	VA_UI_INT_ConvertUA2Str(s_pszUa, aucUA, kVA_UI_UA_SIZE);

	HxLOG_Print("[Exit]\n");
	return s_pszUa;
}

// do not free return value
HUINT8 *VA_UI_Adapt_GetVersionName()
{
	HxLOG_Print("[Enter]\n");

	if (s_pszVersion == NULL)
	{
		s_pszVersion = (HUINT8*)VA_MEM_Alloc(kVA_UI_MAX_VERSION_NAME_SIZE+1);
		if (s_pszVersion == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	if (VA_UI_GetVersionName(s_pszVersion) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetVersionName() failed\n");
		return NULL;
	}

	HxLOG_Print("[Exit]\n");
	return s_pszVersion;
}

// do not free return value
HUINT8 *VA_UI_Adapt_GetCamlockStr()
{
	HxLOG_Print("[Enter]\n");

	if (s_pszCamlock == NULL)
	{
		s_pszCamlock = (HUINT8*)VA_MEM_Alloc(kVA_UI_MAX_CAMLOCK_SIZE+1);
		if (s_pszCamlock == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	if (VA_UI_GetCamlockInfo(NULL, s_pszCamlock) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetCamlockInfo() failed\n");
		return NULL;
	}

	HxLOG_Print("[Exit]\n");
	return s_pszCamlock;
}

HINT32 VA_UI_Adapt_GetPairingMode()
{
	HUINT32 ulMode;

	HxLOG_Print("[Enter]\n");

	if (VA_UI_GetPairingMode(&ulMode) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetPairingMode() failed\n");
		return VA_UI_ERR;
	}

	HxLOG_Print("[Exit]\n");
	return ulMode;
}

HINT32 VA_UI_Adapt_GetPairingSC()
{
	HUINT32 ulMax, ulNum;

	HxLOG_Print("[Enter]\n");

	if (VA_UI_GetPairingSmartcard(&ulMax, &ulNum) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetPairingSmartcard() failed\n");
		return 0; // VA_UI_ERR;
	}

	HxLOG_Print("[Exit]\n");
	return ulMax;
}

// the return value must be freed by caller function
VA_UI_UA *VA_UI_Adapt_GetPairingUaList(HUINT32 *pulNumUa)
{
	HUINT32 ulNum = 0;
	tVA_UI_Ua *pUa = NULL;
	VA_UI_UA *pUaList = NULL;
	HINT32 i;

	HxLOG_Print("[Enter]\n");

	if (pulNumUa == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	pUa = VA_UI_GetPairingUaList(&ulNum);

	if (pUa == NULL || ulNum == 0)
	{
		if (pUa != NULL)
		{
			VA_MEM_Free(pUa);
		}
		*pulNumUa = 0;
		return NULL;
	}

	pUaList = (VA_UI_UA *)VA_MEM_Alloc(sizeof(VA_UI_UA) * ulNum);
	if (pUaList == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		VA_MEM_Free(pUa);
		*pulNumUa = 0;
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		VA_UI_INT_ConvertHex2Str(pUaList[i].aucUa, pUa[i], kVA_UI_UA_SIZE);
	}

	VA_MEM_Free(pUa);

	*pulNumUa = ulNum;

	HxLOG_Print("[Exit]\n");
	return pUaList;
}

// length of <pszLabel> must be greater than <VA_UI_MAX_SO_LABLE+1>
HINT32 VA_UI_Adapt_GetLabelFromSoid(HINT32 nScSlot, HUINT32 ulSoid, HUINT8 *pszLabel)
{
	HxLOG_Print("[Enter]\n");

	if (pszLabel == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	if (VA_UI_GetLabel(nScSlot, ulSoid, pszLabel) == VA_ERR)
	{
		HxLOG_Error("VA_UI_GetLabel() failed\n");
		return VA_UI_ERR;
	}

	HxLOG_Print("[Exit]\n");
	return VA_UI_ERR_OK;
}

// the return value must be freed by caller function
VA_UI_OPERATOR_LIST *VA_UI_Adapt_GetSoidList(HINT32 nScSlot, HUINT32 *pulNumSoid)
{
	HUINT32 ulNum = 0;
	tVA_UI_Soid *pSoidList = NULL;
	VA_UI_OPERATOR_LIST *pOpList = NULL;
	HINT32 i;

	HxLOG_Print("[Enter]\n");

	if (pulNumSoid == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	pSoidList = VA_UI_GetSoidList((UINT32)nScSlot, &ulNum);

	if (pSoidList == NULL || ulNum == 0)
	{
		*pulNumSoid = 0;
		return NULL;
	}

	pOpList = (VA_UI_OPERATOR_LIST *)VA_MEM_Alloc(sizeof(VA_UI_OPERATOR_LIST) * ulNum);
	if (pOpList == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		VA_MEM_Free(pSoidList);
		*pulNumSoid = 0;
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		pOpList[i].ulSoid = pSoidList[i];
		if (VA_UI_GetLabel(nScSlot, pOpList[i].ulSoid, pOpList[i].szLable) == VA_ERR)
		{
			HxLOG_Error("VA_UI_GetLabel() failed\n");
			VA_MEM_Free(pSoidList);
			VA_MEM_Free(pOpList);
			*pulNumSoid = 0;
			return NULL;
		}
	}

	VA_MEM_Free(pSoidList);

	*pulNumSoid = ulNum;

	HxLOG_Info("[Exit][%d]\n", ulNum);
	return pOpList;
}

// <pClassList> must be freed by caller function
VA_UI_CLASS_LIST *VA_UI_Adapt_GetClassList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue)
{
	HUINT32 ulNum = 0;
	tVA_UI_ClassItem *pClass = NULL;
	VA_UI_CLASS_LIST *pClassList = NULL;
	HINT32 nRet, i;

	HxLOG_Print("[Enter]\n");

	if (pulNumList == NULL || pnReturnValue == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	nRet = VA_UI_GetClassListAll((UINT32)nScSlot, (UINT32)ulSoid, &ulNum, &pClass);
	if (nRet == VA_ERR_UI_CONSULTATION_LOCKED)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR_CONSULTATION_LOCKED;
		return NULL;
	}
	else if (nRet != VA_ERR_OK)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	if (pClass == NULL || ulNum == 0)
	{
		if (pClass)
			VA_MEM_Free(pClass);
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	pClassList = (VA_UI_CLASS_LIST *)VA_MEM_Alloc(sizeof(VA_UI_CLASS_LIST) * ulNum);
	if (pClassList == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		VA_MEM_Free(pClass);
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		VA_UI_INT_ConvertDate2Str(pClassList[i].szStartDate, pClass[i].stDateRange.stMin);
		VA_UI_INT_ConvertDate2Str(pClassList[i].szEndDate, pClass[i].stDateRange.stMax);
		VA_memcpy(pClassList[i].aucClassMark, pClass[i].pClassMaskBuffer, pClass[i].uiClassMaskSize);
		pClassList[i].ucSize = pClass[i].uiClassMaskSize;
	}

	VA_MEM_Free(pClass);

	*pulNumList = ulNum;
	*pnReturnValue = VA_UI_ERR_OK;

	HxLOG_Info("[Exit][%d]\n", ulNum);
	return pClassList;
}

// <pThLevelList> must be freed by caller function
VA_UI_THEME_LEVEL_LIST *VA_UI_Adapt_GetThemeLevelList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue)
{
	HUINT32 ulNum = 0;
	tVA_UI_ThemeLevelItem *pThLevel = NULL;
	VA_UI_THEME_LEVEL_LIST *pThLevelList = NULL;
	HINT32 nRet, i;

	HxLOG_Print("[Enter]\n");

	if (pulNumList == NULL || pnReturnValue == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	nRet = VA_UI_GetThemeLevelListAll(nScSlot, ulSoid, &ulNum, &pThLevel);
	if (nRet == VA_ERR_UI_CONSULTATION_LOCKED)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR_CONSULTATION_LOCKED;
		return NULL;
	}
	else if (nRet != VA_ERR_OK)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	if (pThLevel == NULL || ulNum == 0)
	{
		if (pThLevel)
			VA_MEM_Free(pThLevel);
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	pThLevelList = (VA_UI_THEME_LEVEL_LIST *)VA_MEM_Alloc(sizeof(VA_UI_THEME_LEVEL_LIST) * ulNum);
	if (pThLevelList == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		VA_MEM_Free(pThLevel);
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		VA_UI_INT_ConvertDate2Str(pThLevelList[i].szStartDate, pThLevel[i].stDateRange.stMin);
		VA_UI_INT_ConvertDate2Str(pThLevelList[i].szEndDate, pThLevel[i].stDateRange.stMax);
		pThLevelList[i].ucTheme = pThLevel[i].uiTheme;
		pThLevelList[i].ucLevel = pThLevel[i].uiLevel;
	}

	VA_MEM_Free(pThLevel);

	*pulNumList = ulNum;
	*pnReturnValue = VA_UI_ERR_OK;

	HxLOG_Info("[Exit][%d]\n", ulNum);
	return pThLevelList;
}

// <pPrebookedList> must be freed by caller function
VA_UI_PREBOOKED_LIST *VA_UI_Adapt_GetPrebookedList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue)
{
	HUINT32 ulNum = 0;
	tVA_UI_ProgramNumberRange *pPrebooked = NULL;
	VA_UI_PREBOOKED_LIST *pPrebookedList = NULL;
	HINT32 nRet, i;

	HxLOG_Print("[Enter]\n");

	if (pulNumList == NULL || pnReturnValue == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	nRet = VA_UI_GetPrebookedListAll(nScSlot, ulSoid, &ulNum, &pPrebooked);
	if (nRet == VA_ERR_UI_CONSULTATION_LOCKED)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR_CONSULTATION_LOCKED;
		return NULL;
	}
	else if (nRet != VA_ERR_OK)
	{
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	if (pPrebooked == NULL || ulNum == 0)
	{
		if (pPrebooked)
			VA_MEM_Free(pPrebooked);
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	pPrebookedList = (VA_UI_PREBOOKED_LIST *)VA_MEM_Alloc(sizeof(VA_UI_PREBOOKED_LIST) * ulNum);
	if (pPrebookedList == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		VA_MEM_Free(pPrebooked);
		*pulNumList = 0;
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		pPrebookedList[i].ulMin = pPrebooked[i].uiMin;
		pPrebookedList[i].ulMax = pPrebooked[i].uiMax;
	}

	VA_MEM_Free(pPrebooked);

	*pulNumList = ulNum;
	*pnReturnValue = VA_UI_ERR_OK;

	HxLOG_Info("[Exit][%d]\n", ulNum);
	return pPrebookedList;
}

/*
	pPinCode: 4 digits characters ("0" ~ "9")
*/
HINT32 VA_UI_Adapt_UnlockConsultation(HINT32 nScSlot, HUINT8 *pPinCode)
{
	HUINT8 aucVaPin[8];
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);

	nRet = VA_UI_UnlockConsultation(nScSlot, aucVaPin);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

HINT32 VA_UI_Adapt_GetPinCodeAttempts(HINT32 nScSlot, HUINT8 *pucRemainingCount, HUINT8 *pucMaxCount)
{
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pucRemainingCount == NULL || pucMaxCount == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	nRet = VA_UI_GetPinCodeAttempts(nScSlot, pucRemainingCount, pucMaxCount);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

/*
	pOldPin: 4 digits characters ("0" ~ "9")
	pNewPin: 4 digits characters ("0" ~ "9")
*/
HINT32 VA_UI_Adapt_ModifyPinCode(HINT32 nScSlot, HUINT8 *pOldPin, HUINT8 *pNewPin)
{
	HUINT8 aucVaOldPin[8];
	HUINT8 aucVaNewPin[8];
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pOldPin == NULL || pNewPin == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaOldPin, pOldPin);
	VA_UI_INT_ConvertStr2VaPin(aucVaNewPin, pNewPin);

	nRet = VA_UI_ModifyPinCode(nScSlot, aucVaOldPin, aucVaNewPin);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pNewPin);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

HINT32 VA_UI_Adapt_GetMaturityRating(HINT32 nScSlot, HUINT8 *pucMaturityRating, HUINT8 *pucMaxMaturityRating)
{
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pucMaturityRating == NULL ||pucMaxMaturityRating == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	nRet = VA_UI_GetMaturityRating(nScSlot, pucMaturityRating, pucMaxMaturityRating);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

/*
	pPinCode: 4 digits characters ("0" ~ "9")
*/
HINT32 VA_UI_Adapt_ModifyMaturityRating(HINT32 nScSlot, HUINT8 *pPinCode, HUINT8 ucMaturityRating)
{
	HUINT8 aucVaPin[8];
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);
	nRet = VA_UI_ModifyMaturityRating(nScSlot, aucVaPin, ucMaturityRating);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

/*
	pPinCode: 4 digits characters ("0" ~ "9")
*/
HINT32 VA_UI_Adapt_AcceptMaturityRating(HINT32 nAcsId, HINT32 nScSlot, HUINT8 *pPinCode)
{
	HUINT8 aucVaPin[8];
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);
	nRet = VA_UI_AcceptMaturityRating(nAcsId, nScSlot, aucVaPin);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
	{
		HINT32 nRecAcsId;

		VA_UTIL_SetPinCode(pPinCode);

		if (VA_PI_Adapt_IsLive(nAcsId) == TRUE)
		{
			// ÇöÀç liveÀÌ¹È¼­ REC³ª TSRÀÌ ÀÖÀ¸¸é °Å±âµµ PINÀ» ³Ñ°ÜÁà¾ßÇÏ´Âµð..
			nRecAcsId = VA_PI_Adapt_GetRecordModeAcs(nAcsId);
			if (nRecAcsId >= 0)
			{
				VA_UI_INFO *pUiInfo;

				pUiInfo = VA_PI_GetUiInfoByAcsId(nRecAcsId);
				if (pUiInfo != NULL && pUiInfo->stLastEvent.eEventType == eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT)
				{
					VA_UI_AcceptMaturityRating(nRecAcsId, nScSlot, aucVaPin);
				}
			}
		}
		else if (VA_PI_Adapt_IsPlayback(nAcsId))
		{
			VA_SvcPb_ForceSetTimecode(nAcsId);
		}
	}
#endif


	HxLOG_Print("[Exit]\n");
	return nRet;
}

// this function returns tVA_UI_ReferenceItem structure.
// contents of this data can be parsed by VA_UI_AdaptGetStringPreselectionItem()
void *VA_UI_Adapt_GetPreselection(HINT32 nScSlot, HINT32 *pulCount, HINT32 *pnReturnValue)
{
	HUINT32 ulNum = 0;
	tVA_UI_ReferenceItem *pReference = NULL;
	HINT32 nRet, i;

	HxLOG_Print("[Enter]\n");

	if (pulCount == NULL || pnReturnValue == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	nRet = VA_UI_GetPreselection(nScSlot, &ulNum, &pReference);
	*pnReturnValue = nRet;

	if (nRet != VA_ERR_OK)
	{
		*pulCount = 0;
		return NULL;
	}

	*pulCount = ulNum;

	HxLOG_Print("[Exit]\n");
	return (void *)pReference;
}

// this function returns tVA_UI_ReferenceItem structure.
// contents of this data can be parsed by VA_UI_AdaptGetStringPreselectionItem()
void *VA_UI_Adapt_GetOpPreselection(HINT32 nScSlot, HUINT32 ulSoid,  HINT32 *pulCount, HINT32 *pnReturnValue)
{
	HUINT32 ulNum = 0;
	tVA_UI_ReferenceItem *pReference = NULL;
	HINT32 nRet, i;

	HxLOG_Print("[Enter]\n");

	if (pulCount == NULL || pnReturnValue == NULL)
	{
		HxLOG_Error("wrong param\n");
		return NULL;
	}

	nRet = VA_UI_GetOperatorPreselection(nScSlot, ulSoid, &ulNum, &pReference);
	*pnReturnValue = nRet;

	if (nRet != VA_ERR_OK)
	{
		*pulCount = 0;
		return NULL;
	}

	*pulCount = ulNum;

	HxLOG_Print("[Exit]\n");
	return (void *)pReference;
}

void VA_UI_Adapt_GetPreselectionItemData(void *pPreselection,
												HINT32 nIndex,
												HUINT32 *pulType,
												HUINT32 *pulSoid,
												HUINT32 *pulValue1,
												HUINT32 *pulValue2,
												HUINT8 *pszStr)
{
	tVA_UI_ReferenceItem *pReference;

	HxLOG_Print("[Enter]\n");

	if (pPreselection == NULL || pulType == NULL || pulSoid == NULL || pulValue1 == NULL)
	{
		HxLOG_Error("wrong param\n");
		return;
	}

	pReference = (tVA_UI_ReferenceItem *)pPreselection;

	*pulType = (HUINT32)pReference[nIndex].eType;
	*pulSoid = pReference[nIndex].uiSoid;

	switch (pReference[nIndex].eType)
	{
	case eVA_UI_THEME_LEVEL_REFERENCE:
		if (pulValue2 == NULL)
			return;

		*pulValue1 = (HUINT32)pReference[nIndex].uInfo.stThemeLevelReference.uiTheme;
		*pulValue2 = (HUINT32)pReference[nIndex].uInfo.stThemeLevelReference.uiLevel;
		VA_UI_INT_ConvertDate2Str(pszStr, pReference[nIndex].uInfo.stThemeLevelReference.stDate);
		break;

	case eVA_UI_CLASS_REFERENCE:
		*pulValue1 = (HUINT32)pReference[nIndex].uInfo.stClassReference.uiClass;
		VA_UI_INT_ConvertDate2Str(pszStr, pReference[nIndex].uInfo.stClassReference.stDate);
		break;

	case eVA_UI_PREBOOKED_REFERENCE:
		*pulValue1 = (HUINT32)pReference[nIndex].uInfo.uiPrebookedReference;
		break;

	case eVA_UI_IPPVP_REFERENCE:
		*pulValue1 = (HUINT32)pReference[nIndex].uInfo.stIppvPReference.uiProgramNumber;
		VA_UI_INT_ConvertPrice2Str(pszStr, pReference[nIndex].uInfo.stIppvPReference.stPrice.iInteger, pReference[nIndex].uInfo.stIppvPReference.stPrice.uiFraction);
		break;

	case eVA_UI_IPPVT_REFERENCE:
		*pulValue1 = (HUINT32)pReference[nIndex].uInfo.stIppvTReference.uiProgramNumber;
		VA_UI_INT_ConvertPrice2Str(pszStr, pReference[nIndex].uInfo.stIppvTReference.stCeiling.iInteger, pReference[nIndex].uInfo.stIppvTReference.stCeiling.uiFraction);
		break;
	}

	HxLOG_Print("[Exit]\n");
}

/*
	pPinCode: 4 digits characters ("0" ~ "9")
*/
HINT32 VA_UI_Adapt_DeletePreselection(HINT32 nScSlot, HUINT8 *pPinCode, void *pInfo, HINT32 nIndex)
{
	HINT32 nRet;
	tVA_UI_ReferenceItem *pRef = (tVA_UI_ReferenceItem *)pInfo;
	HUINT8 aucVaPin[8];

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	if (pInfo == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);

	nRet = VA_UI_ModifyPreselection(nScSlot, pRef[nIndex].uiSoid, aucVaPin, eVA_UI_DELETE_ACTION, pRef[nIndex].eType, &pRef[nIndex].uInfo);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

/*
	pPinCode: 4 digits characters ("0" ~ "9")
	in case of <theme_level>
		ulValue1: theme
		lValue2: level
		usValue3: year
		usValue4: month
		usValue5: day
	in case of <class>
		ulValue1: class
		usValue3: year
		usValue4: month
		usValue5: day
	in case of <prebooked>
		ulValue1: program number
	in case of <ippv-p>
		ulValue1: program number
		lValue2: price - integer
		usValue3: price - fraction
	in case of <ippv-t>
		ulValue1: program number
		lValue2: ceiling - integer
		usValue3: ceiling -fraction
*/
HINT32 VA_UI_Adapt_AddPreselection(HINT32 nScSlot,
							HUINT8 *pPinCode,
							HUINT32 ulType,
							HUINT32 ulSoid,
							HUINT32 ulValue1,
							HINT32 lValue2,
							HUINT16 usValue3,
							HUINT16 usValue4,
							HUINT16 usValue5)
{
	tVA_UI_ReferenceInfo *pInfo;
	HINT32 nRet;
	HUINT32 ulNum;
	HUINT8 aucVaPin[8];
	tVA_UI_ReferenceType eType = eVA_UI_THEME_LEVEL_REFERENCE;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
	{
		HxLOG_Error("wrong param\n");
		return VA_UI_ERR;
	}

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);

	pInfo = (tVA_UI_ReferenceInfo *)VA_MEM_Alloc(sizeof(tVA_UI_ReferenceInfo));
	if (pInfo == NULL)
	{
		HxLOG_Error("can't allocate memory\n");
		return VA_UI_ERR;
	}

	switch (ulType)
	{
	case VA_UI_THEME_LEVEL_REFERENCE:
		eType = eVA_UI_THEME_LEVEL_REFERENCE;
		pInfo->stThemeLevelReference.uiTheme = (HUINT8)ulValue1;
		pInfo->stThemeLevelReference.uiLevel = (HUINT8)lValue2;
		pInfo->stThemeLevelReference.stDate.uiYear = usValue3;
		pInfo->stThemeLevelReference.stDate.uiMonth = usValue4;
		pInfo->stThemeLevelReference.stDate.uiDay = usValue5;
		break;

	case VA_UI_CLASS_REFERENCE:
		eType = eVA_UI_CLASS_REFERENCE;
		pInfo->stClassReference.uiClass = (HUINT8)ulValue1;
		pInfo->stClassReference.stDate.uiYear = usValue3;
		pInfo->stClassReference.stDate.uiMonth = usValue4;
		pInfo->stClassReference.stDate.uiDay =usValue5;
		break;

	case VA_UI_PREBOOKED_REFERENCE:
		eType = eVA_UI_PREBOOKED_REFERENCE;
		pInfo->uiPrebookedReference = (tVA_UI_ProgramNumber)ulValue1;
		break;

	case VA_UI_IPPVP_REFERENCE:
		eType = eVA_UI_IPPVP_REFERENCE;
		pInfo->stIppvPReference.uiProgramNumber = (tVA_UI_ProgramNumber)ulValue1;
		pInfo->stIppvPReference.stPrice.iInteger = (HINT32)lValue2;
		pInfo->stIppvPReference.stPrice.uiFraction = (HUINT16)usValue3;
		break;

	case VA_UI_IPPVT_REFERENCE:
		eType = eVA_UI_IPPVT_REFERENCE;
		pInfo->stIppvTReference.uiProgramNumber = (tVA_UI_ProgramNumber)ulValue1;
		pInfo->stIppvTReference.stCeiling.iInteger = (HINT32)lValue2;
		pInfo->stIppvTReference.stCeiling.uiFraction = (HUINT16)usValue3;
		break;
	}

	nRet = VA_UI_ModifyPreselection(nScSlot, ulSoid, aucVaPin, eVA_UI_WRITE_ACTION, eType, pInfo);
	VA_MEM_Free(pInfo);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

HINT32 VA_UI_Adapt_GetSecurityState(HINT32 nScSlot)
{
	// card´Â 1Àå¸¸ °í·ÁµÇ¾îÀÖÀ½...
	if (nScSlot != 0)
	{
		HxLOG_Error("wrong sc slot:%d\n", nScSlot);
		return VA_UI_SMARTCARD_FAILED;
	}
	HxLOG_Print("[state:%d]\n", s_nSecurity);
	return s_nSecurity;
}

HINT32 VA_UI_Adapt_GetCamlockLevel(HINT32 nScSlot)
{
	// card´Â 1Àå¸¸ °í·ÁµÇ¾îÀÖÀ½...
	if (nScSlot != 0)
	{
		HxLOG_Error("wrong sc slot:%d\n", nScSlot);
		return VA_UI_SMARTCARD_FAILED;
	}
	HxLOG_Print("[state:%x]\n", s_nCamLockCtrlLevel);
	return s_nCamLockCtrlLevel;
}

HINT32 VA_UI_Adapt_GetCamlockState(HINT32 nScSlot)
{
	// card´Â 1Àå¸¸ °í·ÁµÇ¾îÀÖÀ½...
	if (nScSlot != 0)
	{
		HxLOG_Error("wrong sc slot:%d\n", nScSlot);
		return VA_UI_SMARTCARD_FAILED;
	}
	HxLOG_Print("[state:%x]\n", s_nCamlock);
	return s_nCamlock;
}

HINT32 VA_UI_Adapt_GetSmartcardState(HINT32 nScSlot)
{
	// card´Â 1Àå¸¸ °í·ÁµÇ¾îÀÖÀ½...
	if (nScSlot != 0)
	{
		HxLOG_Error("wrong sc slot:%d\n", nScSlot);
		return VA_UI_SMARTCARD_FAILED;
	}
	HxLOG_Print("[state:%d]\n", s_nScState);
	return s_nScState;
}

#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
// do not free return value
// message will be stored and then delivered
VA_UI_OP_MESSAGE *VA_UI_Adapt_GetNewOpMessage(HINT32 nScSlot)
{
	HUINT32 ulSoid, ulEncode, ulCount;

	VaPrint(DBG_PRINT, ("[Enter]\n"));

	if (s_pOpMessage == NULL)
	{
		s_pOpMessage = (VA_UI_OP_MESSAGE *)VA_MEM_Alloc(sizeof(VA_UI_OP_MESSAGE));
		if (s_pOpMessage == NULL)
			return NULL;
	}

	ulSoid = kVA_UI_NOT_USED;

#ifdef TEST_OP_MSG
{
	UNIXTIME stUnixtime;
	HxDATETIME_t stDateTime;
	static int test_cnt = 0;

	VK_CLOCK_GetTime(&stUnixtime);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(stUnixtime, &stDateTime);

	snprintf(s_pOpMessage->szMsg, VA_UI_OP_MSG_SIZE, "<OP MSG TEST %d> : %d/%d %d:%d:%d Received - 123456789A123456789B123456789C 123456789D123456789E123456789F", test_cnt++, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

	ulSoid = 0;
	ulEncode = 0;
	ulCount = strlen(s_pOpMessage->szMsg);

}
#else
	if (VA_UI_GetOperatorMessage(nScSlot, &ulSoid, eVA_UI_ALL_PRIORITY, &ulEncode, &ulCount, s_pOpMessage->szMsg) != VA_ERR_OK)
	{
		return NULL;
	}
#endif

	if (ulCount > VA_UI_OP_MSG_SIZE)
		ulCount = VA_UI_OP_MSG_SIZE;

	if (ulCount < VA_UI_OP_MSG_SIZE)
		s_pOpMessage->szMsg[ulCount] = 0;

	s_pOpMessage->ulSoid = ulSoid;
	s_pOpMessage->ulEncode = ulEncode;
	s_pOpMessage->ulCount = ulCount;
	s_pOpMessage->ucRead = FALSE;
	VK_CLOCK_GetTime(&s_pOpMessage->stUnixTime);

	// store
	VA_SetOpMessage(s_pOpMessage);

	VaPrint(DBG_PRINT, ("[Exit]\n"));
	return s_pOpMessage;
}

HINT32 VA_UI_Adapt_GetOpMessage(VA_UI_OP_MESSAGE *pOpMsg, HUINT32 ulMsgSlot)
{
	HERROR nErr;

	VaPrint(DBG_PRINT, ("[Enter]\n"));

	nErr = VA_GetOpMessage(pOpMsg, ulMsgSlot);

	VaPrint(DBG_PRINT, ("[Exit]\n"));
	return nErr;
}

HINT32 VA_UI_Adapt_GetOpMessageCount()
{
	return VA_GetOpMessageCount();
}

HINT32 VA_UI_Adapt_UpdateOpMessage(VA_UI_OP_MESSAGE *pOpMsg, HUINT32 ulMsgSlot)
{
	return VA_UpdateOpMessage(pOpMsg, ulMsgSlot);
}

HINT32 VA_UI_Adapt_DeleteOpMessage(HUINT32 ulMsgSlot)
{
	return VA_DeleteOpMessage(ulMsgSlot);
}
#else
// do not free return value
// message will be stored and then delivered
VA_UI_OP_MESSAGE *VA_UI_Adapt_GetOpMessage(HINT32 nScSlot)
{
	HUINT32 ulSoid, ulEncode, ulCount;

	HxLOG_Print("[Enter]\n");

	if (s_pOpMessage == NULL)
	{
		s_pOpMessage = (VA_UI_OP_MESSAGE *)VA_MEM_Alloc(sizeof(VA_UI_OP_MESSAGE));
		if (s_pOpMessage == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	ulSoid = kVA_UI_NOT_USED;

	if (VA_UI_GetOperatorMessage(nScSlot, &ulSoid, eVA_UI_ALL_PRIORITY, &ulEncode, &ulCount, s_pOpMessage->szMsg) != VA_ERR_OK)
	{
		return NULL;
	}

	if (ulCount > VA_UI_OP_MSG_SIZE)
		ulCount = VA_UI_OP_MSG_SIZE;

	if (ulCount < VA_UI_OP_MSG_SIZE)
		s_pOpMessage->szMsg[ulCount] = 0;

	s_pOpMessage->ulSoid = ulSoid;
	s_pOpMessage->ulEncode = ulEncode;
	s_pOpMessage->ulCount = ulCount;

	// store
	VA_SetOpMessage(s_pOpMessage, 0);
	VA_FlashFlush();

	HxLOG_Print("[Exit]\n");
	return s_pOpMessage;
}
#endif

VA_UI_OP_MESSAGE *VA_UI_Adapt_GetLastOpMessage()
{
	HxLOG_Print("[Enter]\n");

	if (s_pOpMessage == NULL)
	{
		s_pOpMessage = (VA_UI_OP_MESSAGE *)VA_MEM_Alloc(sizeof(VA_UI_OP_MESSAGE));
		if (s_pOpMessage == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	VA_GetOpMessage(s_pOpMessage, 0);

	HxLOG_Print("[Exit]\n");
	return s_pOpMessage;
}

// do not free return value
// data will be stored and then delivered
VA_UI_OP_DATA *VA_UI_Adapt_GetOpData(HINT32 nScSlot)
{
	HUINT32 ulSoid, ulCount;

	HxLOG_Print("[Enter]\n");

	if (s_pOpData == NULL)
	{
		s_pOpData = (VA_UI_OP_DATA *)VA_MEM_Alloc(sizeof(VA_UI_OP_DATA));
		if (s_pOpData == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	ulSoid = kVA_UI_NOT_USED;

	if (VA_UI_GetOperatorData(nScSlot, &ulSoid, eVA_UI_ALL_PRIORITY, &ulCount, s_pOpData->aucData) != VA_ERR_OK)
	{
		return NULL;
	}

	s_pOpData->ulSoid = ulSoid;
	s_pOpData->ulCount = ulCount;

	// store
	VA_SetOpData(s_pOpData, 0);

	HxLOG_Print("[Exit]\n");
	return s_pOpData;
}

HINT32 VA_UI_Adapt_GetManufacturerId()
{
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	nRet = VA_GetManufacturerId();

	HxLOG_Print("[Exit]\n");
	return nRet;
}

HINT32 VA_UI_Adapt_GetModelId()
{
	HINT32 nRet;

	HxLOG_Print("[Enter]\n");

	nRet = VA_GetModelId();

	HxLOG_Print("[Exit]\n");
	return nRet;
}

// do not free return value
HUINT8 *VA_UI_Adapt_GetStbSerialStr()
{
	HUINT8 aucSerialNumber[VA_STB_SERIAL_SIZE];
	HINT32	i;

	HxLOG_Print("[Enter]\n");

	if (s_pszStbSerial == NULL)
	{
		s_pszStbSerial = (HUINT8*)VA_MEM_Alloc(VA_STB_SERIAL_SIZE*2 + 1);
		if (s_pszStbSerial == NULL)
		{
			HxLOG_Error("can't allocate memory\n");
			return NULL;
		}
	}

	if (VA_GetSTBSerialNumber(aucSerialNumber) != VA_ERR_OK)
		return NULL;

	VA_UI_INT_ConvertHex2Str(s_pszStbSerial, aucSerialNumber, 12);

	HxLOG_Print("[Exit]\n");

	// ¾Õ¿¡ 0À» Á¦¿ÜÇÏ°í º¸³»±â À§ÇÑ Ã³¸®
	for(i=0 ; i < VA_STB_SERIAL_SIZE*2 ; i++)
	{
		if(s_pszStbSerial[i] != '0')
			break;
	}

	// '0' ÀÌ ¾ø´Â String ½ÃÀÛÁ¡ ÁÖ¼Ò¸¦ º¸³½´Ù.
	return s_pszStbSerial + i;
}

HUINT8 *VA_UI_Adapt_GetHwVersionStr()
{
	return (HUINT8 *)VA_GetHardwareVer();
}

HINT32 VA_UI_Adapt_GetMaturityRatingValue(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle, HUINT8 *pucMaturityRatingValue)
{
	HINT32 nRet;
	tVA_UI_GetAccessConditionRequest infoRequest;

	HxLOG_Print("[Enter]\n");

	nRet = VA_UI_GetAccessCondition(ulAcsId, ulStbStreamHandle, 0, &infoRequest);
	if (nRet != VA_ERR_OK)
	{
		return nRet;
	}

	*pucMaturityRatingValue = infoRequest.stMaturityRatingInfo.uiMaturityRatingValue;

	HxLOG_Print("[Exit(MR:%d)]\n", *pucMaturityRatingValue);

	return VA_UI_ERR_OK;
}

HINT32 VA_UI_Adapt_GetPreviewInfo(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle, HUINT8* pucUsedCwNumber, HUINT8* pucMaxCwNumber)
{
	HINT32 nRet;
	HUINT8 ucUsedCwNumber;
	tVA_UI_GetAccessConditionRequest infoRequest;

	HxLOG_Print("[Enter]\n");

	nRet = VA_UI_GetAccessCondition(ulAcsId, ulStbStreamHandle, 0, &infoRequest);
	if (nRet != VA_ERR_OK)
	{
		return nRet;
	}

	nRet = VA_UI_GetPreviewInfo(0,
				infoRequest.uiSoid,
				infoRequest.stPreviewInfo.uiPreviewIndex,
				infoRequest.stPreviewInfo.uiPreviewNumber,
				&ucUsedCwNumber);

	if (nRet == VA_ERR_UI_CONSULTATION_LOCKED)
	{
		return nRet;
	}

	if (nRet != VA_ERR_OK)
	{
		return nRet;
	}

	*pucUsedCwNumber = ucUsedCwNumber;
	*pucMaxCwNumber = infoRequest.stPreviewInfo.uiMaxCwNumber;

	HxLOG_Print("[Exit]\n");

	return VA_UI_ERR_OK;
}

#if 1 // for camlock control

// TODO: AP_CAS_VA_GetCamlockState() ¿Í ÇÔ²² ÅëÀÏ½ÃÅ°´Â °ÍÀÌ ÁÁ°ÚÀ½.
int VA_GetCamLockControlLevel(void)
{
#ifdef CONFIG_DEBUG
	HxLOG_Info("Security(%d) CamLockCtrlLevel(%s) Camlock(%s) ScState(%s)\n",
		s_nSecurity, VA_UI_Debug_GetCamlockLevel(s_nCamLockCtrlLevel), VA_UI_Debug_GetCamlockState(s_nCamlock), VA_UI_Debug_GetScState(s_nScState));
#endif
	if (s_nCamlock == VA_UI_CAMLOCK_NOT_ACTIVATED)
		return VA_CAMLOCK_CONTROL_NO;
	else
		return s_nCamLockCtrlLevel;
}

void VA_SetCamLockControlLevel(int nCamLockCtrlLevel) // for test only
{
	s_nCamLockCtrlLevel = nCamLockCtrlLevel;
}

#endif

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
// <pItemList> must be freed by caller function
static VA_UI_IPPV_ITEM *VA_UI_Adapt_GetIPPVList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 ulIppvType, HUINT32 *pulNumList, HINT32 *pnReturnValue)
{
	tVA_UI_IppvItem *pItem;
	HUINT32 ulNum;
	HINT32 i;
	HINT32 nRet;
	VA_UI_IPPV_ITEM *pItemList;

	HxLOG_Print("[Enter(IPPV/%c)]\n", ulIppvType == IPPV_T?'T': 'P');

	if (pulNumList == NULL ||pnReturnValue == NULL)
		return NULL;

	nRet = VA_UI_GetIPPVListAll(nScSlot, ulSoid, ulIppvType, &ulNum, &pItem);
	if (nRet == VA_ERR_UI_CONSULTATION_LOCKED)
	{
		*pnReturnValue = VA_UI_ERR_CONSULTATION_LOCKED;
		return NULL;
	}
	else if (nRet != VA_ERR_OK)
	{
		*pnReturnValue = VA_UI_ERR;
		return NULL;
	}

	pItemList = (VA_UI_IPPV_ITEM *)OxCAS_Malloc(sizeof(VA_UI_IPPV_ITEM) * ulNum);
	if (pItemList == NULL)
	{
		*pnReturnValue = VA_UI_ERR;
		OxCAS_Free(pItem);
		return NULL;
	}

	for (i = 0; i < ulNum; i++)
	{
		pItemList[i].ulProgNum = pItem[i].uiProgramNumber;
		pItemList[i].stCost.ulInteger = pItem[i].stCost.iInteger;
		pItemList[i].stCost.usFraction = pItem[i].stCost.uiFraction;
	}

	*pulNumList = ulNum;

	OxCAS_Free(pItem);

	*pnReturnValue = VA_UI_ERR_OK;

	HxLOG_Print("[Exit]\n");
	return pItemList;
}

// <return value> must be freed by caller function
VA_UI_IPPV_ITEM *VA_UI_Adapt_GetIPPVTList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *ulNumList, HINT32 *nReturnValue)
{
	return VA_UI_Adapt_GetIPPVList(nScSlot, ulSoid, IPPV_T, ulNumList, nReturnValue);
}

// <return value> must be freed by caller function
VA_UI_IPPV_ITEM *VA_UI_Adapt_GetIPPVPList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *ulNumList, HINT32 *nReturnValue)
{
	return VA_UI_Adapt_GetIPPVList(nScSlot, ulSoid, IPPV_P, ulNumList, nReturnValue);
}

HUINT8 *VA_UI_Adapt_GetCurrencyStr()
{
	VA_UI_OPERATOR_LIST *pSoidList;
	HUINT32 ulNumSoid;
	VA_UI_PRICE stPrice;

	HxLOG_Print("[Enter]\n");

	if (s_pszCurrency == NULL)
	{
		s_pszCurrency = (HUINT8*)OxCAS_Malloc(VA_UI_CURRENCY_LABEL_SIZE + 1);
		if (s_pszCurrency == NULL)
			return NULL;
	}

	pSoidList = VA_UI_Adapt_GetSoidList(0, &ulNumSoid);
	if (pSoidList == NULL)
		return NULL;

	if (VA_UI_Adapt_GetCreditInfo(0, pSoidList[0].ulSoid, s_pszCurrency, &stPrice) != VA_UI_ERR_OK)
		return NULL;

	HxLOG_Info("[currency:%s, price:%d.%d]\n", s_pszCurrency, stPrice.ulInteger, stPrice.usFraction);

	HxLOG_Print("[Exit]\n");
	return s_pszCurrency;
}

// size of <pszCurrency> must be larger than kVA_UI_MAX_CURRENCY_LABEL_SIZE+1 (16+1)
int VA_UI_Adapt_GetCreditInfo(HINT32 nScSlot, HUINT32 ulSoid, HUINT8 *pszCurrency, VA_UI_PRICE *pstPrice)
{
	HINT32 nRet;
	tVA_UI_GetCreditInfoRequest stCreditInfo;

	HxLOG_Print("[Enter]\n");

	if (pszCurrency == NULL || pstPrice == NULL)
		return VA_UI_ERR;

	nRet = VA_UI_GetCreditInfo(nScSlot, ulSoid, &stCreditInfo);
	if (nRet != VA_ERR_OK)
	{
		return VA_UI_ERR;
	}

	strcpy(pszCurrency, stCreditInfo.pCurrency);
	pstPrice->ulInteger = stCreditInfo.stRemainingCredit.iInteger;
	pstPrice->usFraction = stCreditInfo.stRemainingCredit.uiFraction;

	HxLOG_Print("[Exit]\n");
	return VA_UI_ERR_OK;
}

int VA_UI_Adapt_GetIPPVInfo(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, VA_UI_IPPV_INFO *pInfo)
{
	tVA_UI_GetIppvInfoRequest stInfo;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pInfo == NULL)
		return VA_UI_ERR;

	nRet = VA_UI_GetIPPVInfo(nAcsId, nScSlot, ulStbStreamHandle, &stInfo);

	if (nRet == VA_ERR_OK)
	{
		pInfo->ulAcsId = stInfo.dwAcsId;
		pInfo->ulStbStreamHandle = stInfo.dwStbStreamHandle;
		pInfo->ulSoid = stInfo.uiSoid;
		pInfo->ulPaymentMode = stInfo.ePaymentMode;
		strcpy(pInfo->szCurrency, stInfo.pCurrency);
		pInfo->stRemainingCredit.ulInteger = stInfo.stRemainingCredit.iInteger;
		pInfo->stRemainingCredit.usFraction = stInfo.stRemainingCredit.uiFraction;
		pInfo->bIsIPPVPAgreementNeeded = stInfo.bIsIppvPAgreementNeeded;
		pInfo->bIsIPPVPPresent = stInfo.bIsIppvPPresent;
		pInfo->ulProgNumIPPVP = stInfo.uiPnumIppvP;
		pInfo->stCostIPPVP.ulInteger = stInfo.stCostIppvP.iInteger;
		pInfo->stCostIPPVP.usFraction = stInfo.stCostIppvP.uiFraction;
		pInfo->bIsIPPVTAgreementNeeded = stInfo.bIsIppvTAgreementNeeded;
		pInfo->bIsIPPVTPresent = stInfo.bIsIppvTPresent;
		pInfo->ulProgNumIPPVT = stInfo.uiPnumIppvT;
		pInfo->stCostIPPVT.ulInteger = stInfo.stCostIppvT.iInteger;
		pInfo->stCostIPPVT.usFraction = stInfo.stCostIppvT.uiFraction;
		pInfo->stCeiling.ulInteger = stInfo.stCeiling.iInteger;
		pInfo->stCeiling.usFraction = stInfo.stCeiling.uiFraction;
	}

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_AcceptIPPVP(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, HUINT8 *pPinCode, HUINT32 ulProgNum)
{
	HUINT8 aucVaPin[8];
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
		return VA_UI_ERR;

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);

	nRet = VA_UI_AcceptIPPVP(nAcsId, nScSlot, ulStbStreamHandle, aucVaPin, ulProgNum);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_AcceptIPPVT(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, HUINT8 *pPinCode, HUINT32 ulProgNum, VA_UI_PRICE *pstCeiling)
{
	HUINT8 aucVaPin[8];
	tVA_UI_Price stVaPrice;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
		return VA_UI_ERR;

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);
	stVaPrice.iInteger = pstCeiling->ulInteger;
	stVaPrice.uiFraction = pstCeiling->usFraction;

	nRet = VA_UI_AcceptIPPVT(nAcsId, nScSlot, ulStbStreamHandle, aucVaPin, ulProgNum, stVaPrice);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_GetThreshold(HINT32 nScSlot, VA_UI_PRICE *pstThreshold)
{
	tVA_UI_Price stVaPrice;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pstThreshold == NULL)
		return VA_UI_ERR;

	nRet = VA_UI_GetThreshold(nScSlot, &stVaPrice);

	if (nRet == VA_ERR_OK)
	{
		pstThreshold->ulInteger = stVaPrice.iInteger;
		pstThreshold->usFraction = stVaPrice.uiFraction;
	}

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_ModifyThreshold(HINT32 nScSlot, HUINT8 *pPinCode, VA_UI_PRICE *pstThreshold)
{
	HUINT8 aucVaPin[8];
	tVA_UI_Price stVaPrice;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pPinCode == NULL)
		return VA_UI_ERR;

	VA_UI_INT_ConvertStr2VaPin(aucVaPin, pPinCode);
	stVaPrice.iInteger = pstThreshold->ulInteger;
	stVaPrice.uiFraction = pstThreshold->usFraction;

	nRet = VA_UI_ModifyThreshold(nScSlot, aucVaPin, stVaPrice);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (nRet == VA_ERR_OK)
		VA_UTIL_SetPinCode(pPinCode);
#endif

	HxLOG_Print("[Exit]\n");
	return nRet;
}

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
int VA_UI_Adapt_StartIPPVSurvey(HINT32 nScSlot, HUINT32 ulSoid)
{
	int nRet;
	HxLOG_Print("[Enter]\n");

	nRet = VA_UI_StartIPPVSurvey(nScSlot, ulSoid);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_CancelIPPVSurvey()
{
	int nRet;
	HxLOG_Print("[Enter]\n");

	nRet = VA_UI_CancelIPPVSurvey();

	HxLOG_Print("[Exit]\n");
	return nRet;
}
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
int VA_UI_Adapt_GetRecordInfo(HINT32 nAcsId, HUINT32 ulStbStreamHandle, VA_UI_RECORD_INFO *pstInfo)
{
	tVA_UI_Date stDate;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pstInfo == NULL)
		return VA_UI_ERR;

	nRet = VA_UI_GetRecordInfo(nAcsId, ulStbStreamHandle, &stDate, &pstInfo->ulPlaybackDuration, &pstInfo->ulMaxNumberOfPlayback);

	if (stDate.uiYear == 0 && stDate.uiMonth == 0 && stDate.uiDay == 0)
		pstInfo->szExpireDate[0] = 0;
	else
	VA_UI_INT_ConvertDate2Str(pstInfo->szExpireDate, stDate);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_GetPlaybackInfoNow(VA_UI_PLAYBACK_INFO *pstInfo)
{
	HINT32 nAcsId;

	nAcsId = VA_PI_Adapt_GetPlaybackAcsId();
	if (nAcsId == -1)
		return VA_UI_ERR;

	return VA_UI_Adapt_GetPlaybackInfo(nAcsId, pstInfo);
}

int VA_UI_Adapt_GetPlaybackInfo(HINT32 nAcsId, VA_UI_PLAYBACK_INFO *pstInfo)
{
	tVA_UI_DateEx stDateEx, stDateEx2;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (pstInfo == NULL)
		return VA_UI_ERR;

	nRet = VA_UI_GetPlaybackInfo(nAcsId, &pstInfo->ulPlaybackCounter, &stDateEx, &stDateEx2);

	VA_UI_INT_ConvertDateEx2Str(pstInfo->szRecordDate, stDateEx);
	VA_UI_INT_ConvertDateEx2Str(pstInfo->szFirstPlaybackDate, stDateEx2);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UI_Adapt_IncrementPlaybackCount(HINT32 nAcsId)
{
	int nRet;

	HxLOG_Print("[Enter]\n");

	nRet = VA_UI_IncrementPlaybackCount(nAcsId);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

void VA_UI_Adapt_AutoPinForRecord(HUINT32 ulAcsId, HUINT32 ulScSlot)
{
	HUINT8 aucPin[5];
	HUINT8 aucVaPin[8];

	HxLOG_Print("[Enter]\n");

	if (VA_UTIL_GetPinCode(aucPin) == VA_ERR_OK)
	{
		aucPin[4] = 0;
		if (VA_PI_Adapt_IsRecord(ulAcsId))
		{
			VA_UI_INT_ConvertStr2VaPin(aucVaPin, aucPin);
			if (VA_UI_AcceptMaturityRating(ulAcsId, ulScSlot, aucVaPin) != ERR_OK)
			{
				VA_UTIL_InvalidatePincode();
			}
		}
	}

	HxLOG_Print("[Exit]\n");
}

#endif
