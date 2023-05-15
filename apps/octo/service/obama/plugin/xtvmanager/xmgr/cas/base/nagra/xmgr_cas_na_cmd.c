/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>

#include <isosvc.h>
#include <db_param.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <svc_sys.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaUiPopupData_t		s_stNaUiPopupData;


/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/





/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#define _____XMGR_CAS_NA_CMD_LOCAL_FUNCTION_PROTOTYPE___________________________________________
STATIC HERROR _xmgr_cas_Nacmd_get_PopupData(XmgrCas_NaUiPopupCmd_e *popup_cmd, XmgrCas_NaUiPopupMode_e *popup_mode, HUINT8 **pszMsg);
STATIC HERROR _xmgr_cas_Nacmd_makeLowerCountryCode(HUINT8* szDest, HUINT8* szSrc);
STATIC HERROR _xmgr_cas_Nacmd_process_Popup(NaCmdData_t *pstNaCmdData);
STATIC HERROR _xmgr_cas_Nacmd_process_bdc(NaCmdData_t *pstNaCmdData);
STATIC HERROR _xmgr_cas_Nacmd_resetPinCode(void);
STATIC HERROR _xmgr_cas_Nacmd_setPinCode(HUINT8 *pucPinCode, HUINT8 ucPinLength);
STATIC HERROR _xmgr_cas_Nacmd_fingerPrint(HUINT32 nMode);
STATIC HERROR _xmgr_cas_Nacmd_forceTune(Handle_t hSvc);
STATIC HERROR _xmgr_cas_Nacmd_defaultSetting(void);
STATIC HERROR _xmgr_cas_Nacmd_changeUsage(HUINT8 ucValue);
STATIC HERROR _xmgr_cas_Nacmd_rebootStb(void);
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
STATIC HERROR _xmgr_cas_Nacmd_goStandby(void);
#endif
STATIC HERROR _xmgr_cas_Nacmd_displayMailMsg(HUINT16 usMailId, HUINT16 usDeletedMailId);
STATIC HERROR _xmgr_cas_Nacmd_displayPopupMsg(void);
STATIC HERROR _xmgr_cas_Nacmd_logicalDiskFormat(void);
STATIC HERROR _xmgr_cas_Nacmd_process_ird(NaCmdData_t *pstNaCmdData);
STATIC HERROR _xmgr_cas_Nacmd_message_function(void *pNA_data, XmgrCas_NaUiCmdType_e *pCmdType);



#define _____XMGR_CAS_NA_CMD_LOCAL_FUNCTION_BODY_________________________________________________

STATIC HERROR _xmgr_cas_Nacmd_get_PopupData(XmgrCas_NaUiPopupCmd_e *popup_cmd, XmgrCas_NaUiPopupMode_e *popup_mode, HUINT8 **pszMsg)
{
#if 0		// not used
	HUINT8 *tempstr = NULL;
#endif

	if (s_stNaUiPopupData.popup_cmd == eCAS_NA_POPUP_DISPLAY)
	{
		(*popup_cmd)	= s_stNaUiPopupData.popup_cmd;
		(*popup_mode)	= s_stNaUiPopupData.popup_mode;
		(*pszMsg)		= s_stNaUiPopupData.pszPopup_msg;
	}
	else if (s_stNaUiPopupData.popup_cmd == eCAS_NA_POPUP_CLOSE)
	{
		(*popup_cmd)	= s_stNaUiPopupData.popup_cmd;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // BDC ADD
		(*popup_mode)	= s_stNaUiPopupData.popup_mode;
#endif
	}
	else
	{
		return eCAS_NA_CMD_ERR_BDC_NULL;
	}
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_makeLowerCountryCode(HUINT8* szDest, HUINT8* szSrc)
{
	HxSTD_memcpy((HUINT8 *)szDest, (HUINT8 *)szSrc, 3);
	MWC_UTIL_ConvertCharLower(&szDest[0]);
	MWC_UTIL_ConvertCharLower(&szDest[1]);
	MWC_UTIL_ConvertCharLower(&szDest[2]);
	return ERR_OK;
}

#if 0
STATIC HERROR _xmgr_cas_Nacmd_process_Popup(NaCmdData_t *pstNaCmdData)
{
	NaBdcItem_t			*pstBdcItem = NULL;
	XmgrCas_NaUiPopupMode_e 	popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
	HINT32				idx;
	HUINT32				nStrLen = 0;
	HUINT8				szCurLang1[4] = {0, };
	HUINT8				szCurLang2[4] = {0, };
	HUINT8				szBdcLang[4] = {0, };
	HINT8				*pszPopUpStr = NULL;

	if (pstNaCmdData == NULL)
		return eCAS_NA_CMD_ERR_NULL;

	pstBdcItem = (NaBdcItem_t *)(pstNaCmdData->item.pstBdcItem);
	if (pstBdcItem == NULL)
	{
		HxLOG_Error("pstBdcItem == NULL\n");
		return eCAS_NA_CMD_ERR_BDC_NULL;
	}

	//initializing bdc global variable
	HxSTD_memset(&s_stNaUiPopupData, 0x00, sizeof(XmgrCas_NaUiPopupData_t));

	HxLOG_Print("eBdcCmd[%x], eBdcType[%x], ucNumOfLang[%x]\n", pstBdcItem->eBdcCmd, pstBdcItem->eBdcType, pstBdcItem->ucNumOfLang);

	if (pstBdcItem->eBdcCmd == eBdc_Display)
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_DISPLAY;
	}
	else if (pstBdcItem->eBdcCmd == eBdc_Remove)
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_CLOSE;
		return eCAS_NA_CMD_ERR_OK;
	}

	switch (pstBdcItem->eBdcType)
	{
		case eBdc_Always :
			popup_mode = eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP;
			break;

		case eBdc_User :
			popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
			break;

		case eBdc_Timeout :
			popup_mode = eCAS_NA_POPUP_MODE_TIMEOUT;
			break;

		default :
			break;
	}

	// get language code from menu
	if (xmgr_cas_NaGetCountrycode(szCurLang1, szCurLang2) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetCountrycode ERROR\n");
		return eCAS_NA_CMD_ERR_BDC_LOADING_CC;
	}

	HxLOG_Print("szCurLang1[%s], szCurLang2[%s]\n", szCurLang1, szCurLang2);

	for ( idx = 0 ; idx < pstBdcItem->ucNumOfLang ; idx++ )
	{
		HxLOG_Print("idx[%d], stBdcContent[idx].xLang[%s]\n", idx, pstBdcItem->stBdcContent[idx].xLang);

		_xmgr_cas_Nacmd_makeLowerCountryCode(szBdcLang, pstBdcItem->stBdcContent[idx].xLang);

		if ( MWC_UTIL_DvbStrcmp((HUINT8 *)szCurLang1, (HUINT8 *)szBdcLang) == 0 )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
			nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
			break;
		}

		if ( MWC_UTIL_DvbStrcmp((HUINT8 *)szCurLang2, (HUINT8 *)szBdcLang) == 0 )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
			nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
			break;
		}
	}

	if ( pszPopUpStr == NULL )
	{
		for ( idx = 0 ; idx < pstBdcItem->ucNumOfLang ; idx++ )
		{
			_xmgr_cas_Nacmd_makeLowerCountryCode(szBdcLang, pstBdcItem->stBdcContent[idx].xLang);

			if (MWC_UTIL_DvbStrcmp((HUINT8 *)"eng", (HUINT8 *)szBdcLang) == 0)
			{
				pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
				nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
				break;
			}
		}

		if ( pszPopUpStr == NULL )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[0].ucText;
		}
	}

	s_stNaUiPopupData.popup_mode 	= popup_mode;
	s_stNaUiPopupData.pszPopup_msg	= pszPopUpStr;

	HxLOG_Print("s_stNaUiPopupData.pszPopup_msg[%s], s_stNaUiPopupData.popup_mode[%d]\n", s_stNaUiPopupData.pszPopup_msg, s_stNaUiPopupData.popup_mode);
	return eCAS_NA_CMD_ERR_OK;
}
#else
STATIC HERROR _xmgr_cas_Nacmd_process_Popup(NaCmdData_t *pstNaCmdData)
{
	NaPopupItem_t		*pstPopupItem = NULL;
	XmgrCas_NaUiPopupMode_e 	popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
	HINT8				*pszPopUpStr = NULL;

	if (pstNaCmdData == NULL) {
		HxLOG_Error("pstPopupItem == NULL\n");
		return eCAS_NA_CMD_ERR_NULL;
	}

	pstPopupItem = (NaPopupItem_t *)(pstNaCmdData->item.pstPopupItem);
	if (pstPopupItem == NULL)
	{
		HxLOG_Error("pstPopupItem == NULL\n");
		return eCAS_NA_CMD_ERR_BDC_NULL;
	}
	HxLOG_Error("eType[%x], ucPersistence[%x], ulPeriod[%x], Msg[0x%x]\n", pstPopupItem->eType, pstPopupItem->ucPersistence, pstPopupItem->ulPeriod, pstPopupItem->pucMsg);
	HxLOG_Error("pstPopupItem->pucMsg = [%s] \n", pstPopupItem->pucMsg);

	//initializing bdc global variable
	VK_memset(&s_stNaUiPopupData, 0x00, sizeof(XmgrCas_NaUiPopupData_t));

	if (pstPopupItem->eType == ePopup_Message)
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_DISPLAY;
		HxLOG_Error("[MJAHN] 2 \n");
	}
	else
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_CLOSE;
		HxLOG_Error("[MJAHN] ---3 \n");
		return eCAS_NA_CMD_ERR_OK;
  	}

	if (pstPopupItem->ucPersistence == POPUP_PERSISTENCE_NORMAL)
		popup_mode = eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP;
	else if (pstPopupItem->ucPersistence == POPUP_PERSISTENCE_TIMEOUT)
		popup_mode = eCAS_NA_POPUP_MODE_TIMEOUT;
	else if (pstPopupItem->ucPersistence == POPUP_PERSISTENCE_USER)
		popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
	else
		popup_mode = 0;


	pszPopUpStr = (HINT8 *)pstPopupItem->pucMsg;

	s_stNaUiPopupData.popup_mode	 = popup_mode;
	s_stNaUiPopupData.pszPopup_msg = pszPopUpStr;

	HxLOG_Error("s_stNaUiPopupData.pszPopup_msg[%s], s_stNaUiPopupData.popup_mode[%d]\n", s_stNaUiPopupData.pszPopup_msg, s_stNaUiPopupData.popup_mode);
	return eCAS_NA_CMD_ERR_OK;
}

#endif


static HERROR _xmgr_cas_Nacmd_process_bdc(NaCmdData_t *pstNaCmdData)
{
	NaBdcItem_t			*pstBdcItem = NULL;
	XmgrCas_NaUiPopupMode_e 	popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
	HINT32				idx;
	HUINT32				nStrLen = 0;
	HUINT8				szCurLang1[4] = {0, };
	HUINT8				szCurLang2[4] = {0, };
	HUINT8				szBdcLang[4] = {0, };
	HINT8				*pszPopUpStr = NULL;

	if (pstNaCmdData == NULL)
		return eCAS_NA_CMD_ERR_NULL;

	pstBdcItem = (NaBdcItem_t *)(pstNaCmdData->item.pstBdcItem);
	if (pstBdcItem == NULL)
	{
		HxLOG_Error("pstBdcItem == NULL\n");
		return eCAS_NA_CMD_ERR_BDC_NULL;
	}

	//initializing bdc global variable
	VK_memset(&s_stNaUiPopupData, 0x00, sizeof(XmgrCas_NaUiPopupData_t));

	HxLOG_Print("eBdcCmd[%x], eBdcType[%x], ucNumOfLang[%x]\n", pstBdcItem->eBdcCmd, pstBdcItem->eBdcType, pstBdcItem->ucNumOfLang);

	if (pstBdcItem->eBdcCmd == eBdc_Display)
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_DISPLAY;
	}
	else if (pstBdcItem->eBdcCmd == eBdc_Remove)
	{
		s_stNaUiPopupData.popup_cmd = eCAS_NA_POPUP_CLOSE;
		return eCAS_NA_CMD_ERR_OK;
	}

	switch (pstBdcItem->eBdcType)
	{
		case eBdc_Always :
			popup_mode = eCAS_NA_POPUP_MODE_ALWAYS_ON_TOP;
			break;

		case eBdc_User :
			popup_mode = eCAS_NA_POPUP_MODE_USER_EXIT;
			break;

		case eBdc_Timeout :
			popup_mode = eCAS_NA_POPUP_MODE_TIMEOUT;
			break;

		default :
			break;
	}

	// get language code from menu
	if (xmgr_cas_NaGetCountrycode(szCurLang1, szCurLang2) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetCountrycode ERROR\n");
		return eCAS_NA_CMD_ERR_BDC_LOADING_CC;
	}

	HxLOG_Print("szCurLang1[%s], szCurLang2[%s]\n", szCurLang1, szCurLang2);

	for ( idx = 0 ; idx < pstBdcItem->ucNumOfLang ; idx++ )
	{
		HxLOG_Print("idx[%d], stBdcContent[idx].xLang[%s]\n", idx, pstBdcItem->stBdcContent[idx].xLang);

		_xmgr_cas_Nacmd_makeLowerCountryCode(szBdcLang, pstBdcItem->stBdcContent[idx].xLang);

		if ( MWC_UTIL_DvbStrcmp((HUINT8 *)szCurLang1, (HUINT8 *)szBdcLang) == 0 )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
			nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
			break;
		}

		if ( MWC_UTIL_DvbStrcmp((HUINT8 *)szCurLang2, (HUINT8 *)szBdcLang) == 0 )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
			nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
			break;
		}
	}

	if ( pszPopUpStr == NULL )
	{
		for ( idx = 0 ; idx < pstBdcItem->ucNumOfLang ; idx++ )
		{
			_xmgr_cas_Nacmd_makeLowerCountryCode(szBdcLang, pstBdcItem->stBdcContent[idx].xLang);

			if (MWC_UTIL_DvbStrcmp((HUINT8 *)"eng", (HUINT8 *)szBdcLang) == 0)
			{
				pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[idx].ucText;
				nStrLen = pstBdcItem->stBdcContent[idx].ulNumBytes;
				break;
			}
		}

		if ( pszPopUpStr == NULL )
		{
			pszPopUpStr = (HINT8 *)pstBdcItem->stBdcContent[0].ucText;
		}
	}

	s_stNaUiPopupData.popup_mode 	= popup_mode;
	s_stNaUiPopupData.pszPopup_msg	= pszPopUpStr;

	HxLOG_Error("s_stNaUiPopupData.pszPopup_msg[%s], s_stNaUiPopupData.popup_mode[%d], nStrLen[%d]\n", s_stNaUiPopupData.pszPopup_msg, s_stNaUiPopupData.popup_mode, nStrLen);
	
	return eCAS_NA_CMD_ERR_OK;
}

STATIC HERROR _xmgr_cas_Nacmd_resetPinCode(void)
{
	HxLOG_Print("_xmgr_cas_Nacmd_resetPinCode()...\n");

	DB_PARAM_SetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32)"0000", 0);

	xmgr_cas_NaCmd_DisplayResetPinCodeMessage();

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_Nacmd_setPinCode(HUINT8 *pucPinCode, HUINT8 ucPinLength)
{
	HERROR err;
	HUINT8 szPincode[5];

	if ( (pucPinCode == NULL) || (ucPinLength < 4) )
	{
		HxLOG_Error("Tagret is null...");
		return eCAS_NA_CMD_ERR_FAIL;
	}

	HxLOG_Print("Change Pin Code... (%4s)(%d)\n", pucPinCode, ucPinLength);

	ucPinLength = (ucPinLength <= 5-1) ? ucPinLength : 5-1;
	HxSTD_memcpy(szPincode, pucPinCode, ucPinLength);
	szPincode[ucPinLength] = '\0';

//	MWC_PARAM_SetPinCode(szPincode);
	err = DB_PARAM_SetItem(eDB_PARAM_ITEM_PINCODE, (HUINT32)szPincode, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_PINCODE) Error\n");
	}

	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_fingerPrint(HUINT32 nMode)
{
	HxLOG_Print("_xmgr_cas_Nacmd_fingerPrint\n");

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER)
#if 0 //1 To do 준비되면 열자
	if(BUS_MGR_Get(xmgr_cas_NaIRD_FingerPrint_Proc) != NULL)
	{
		HxLOG_Print("[_xmgr_cas_Nacmd_fingerPrint] destroy previous finger print window \n");
		BUS_MGR_Destroy(xmgr_cas_NaIRD_FingerPrint_Proc);
	}
	BUS_MGR_Create("xmgr_cas_NaIRD_FingerPrint_Proc", APP_CAS_HIGH_PRIORITY, xmgr_cas_NaIRD_FingerPrint_Proc, (HINT32)0, nMode, 0, 0);
#endif
#endif
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_forceTune(Handle_t hSvc)
{
	HERROR err;
	Handle_t hCurSvc;

	if (hSvc == HANDLE_NULL)
	{
		HxLOG_Error("Null Svc Handle...");
		return eCAS_NA_CMD_ERR_FAIL;
	}

	HxLOG_Print("Force Tune... (0x%x)\n", hSvc);

	err = MGR_LIVE_CurrentSvcInfoGet(MGR_ACTION_GetMainActionId(), &hCurSvc);
	if (err != ERR_OK)
	{
		return eCAS_NA_CMD_ERR_FAIL;
	}

	if (hCurSvc != hSvc)
	{
		MGR_LIVE_CmdPostServiceStartMainView(hSvc, eChListUiGroup_CURRENT, 0);
	}
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_defaultSetting(void)
{
	HxLOG_Print("Force DefultSetting...\n");
	/* IRD-CMD로 defaultsetting시에는 HDD format및 CAS관련 value는 reset하지 않는다. */

	/* micom 시간을 invalid 값으로 변경. 시간도 default set하기 위함. */
	SVC_SYS_SetRtcTime(0);

	/* loader용 default를 불러 준다. */
	{
		HERROR							 hErr;
		SvcSys_FactoryDefaultItem_t		*astDefaultItem = NULL;
		HUINT32 						 ulItemNum = 0;

		hErr = SVC_SYS_GetEeprom4LoaderDefaultItemList(&astDefaultItem, &ulItemNum);
		if (ERR_OK == hErr)
		{
			if (NULL != astDefaultItem && 0 < ulItemNum)
			{
				SVC_SYS_SetEeprom4LoaderFactoryReset(astDefaultItem, ulItemNum);
			}
		}
	}

	/* set default value */
	//DB_DEFAULT_Load();

	BUS_PostMessage(NULL, eSEVT_DB_CHECK_SVC_DB, 0, (eCheckDbType_SvcAdded | eCheckDbType_SvcDeleted), 0, 0);

#if 0 //1 To do 준비되면 열자
	/* restarting... message를 3초 동안 출력 후 Reboot. */
	BUS_MGR_Create("local_feature_ResponseDialogProc",
					APP_DEFAULT_PRIORITY,
					AP_ResponseMessage_ProtoProc,
					0, (HINT32)GetStrRsc(LOC_SYS_HIDDEN_SET_DEFAULTSETHDD_RESPONSE_MSG_ID), TRUE, 624);
#endif

	VK_TASK_Sleep(3*1000);
	MGR_ACTION_RebootSystem();
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_changeUsage(HUINT8 ucValue)
{
#if 0		// not used
	HERROR err;
#endif

	HxLOG_Print("Change Usage...\n");

#if 0
#if defined(CONFIG_PROD_UDHDR)
	err = DB_PARAM_SetItem(MENUCFG_ITEMNAME_OTA_USAGE, (HUINT32)ucValue, 0);
	if (err != ERR_OK)
	{
		HxLOG_Error("Cannot store OTA Usage in DB...");
		return eCAS_NA_CMD_ERR_FAIL;
	}
	DB_PARAM_Sync();

	err = MW_SYS_SetUsageId(ucValue);
	if (err != ERR_OK)
	{
		HxLOG_Error("Cannot store OTA Usage in eeprom...");
		return eCAS_NA_CMD_ERR_FAIL;
	}
#endif
#endif
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_rebootStb(void)
{
	HxLOG_Print("Reboot STB...\n");

	// CAS가 Reboot 시킨 경우, Loader의 AV가 가려져서는 안된다.
	SVC_SYS_SetColdBootPanelCondition(TRUE, TRUE);

	VK_TASK_Sleep(100);
	MGR_ACTION_RebootSystem();

	return eCAS_NA_CMD_ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
STATIC HERROR _xmgr_cas_Nacmd_goStandby(void)
{
	HxLOG_Print("Force Standby...\n");

#if 0
	SVC_SYS_OnOffKeyInput(FALSE); /* 전원키가 또 들어와서 꼬이는 것 방지 */
#endif

	MGR_ACTION_GoStandby(eKeyDevice_RCU, FALSE);

	return eCAS_NA_CMD_ERR_OK;
}
#endif


STATIC HERROR _xmgr_cas_Nacmd_displayMailMsg(HUINT16 usMailId, HUINT16 usDeletedMailId)
{
	BUS_Callback_t pfnMailMessageUiProc;

	HxLOG_Print("Mail Message... (0x%x)(0x%x)\n", usMailId, usDeletedMailId);

	pfnMailMessageUiProc = xmgr_cas_Na_GetUiProc(eNaCasUiType_MailMessage);
	if(pfnMailMessageUiProc == NULL)
	{
		return ERR_FAIL;
	}

	if(BUS_MGR_Get(pfnMailMessageUiProc) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnMailMessageUiProc, HANDLE_NULL, 0, 0, 0);

	BUS_SendMessage(pfnMailMessageUiProc, eMEVT_CAS_CMD, HANDLE_NULL, usMailId, usDeletedMailId, 0);

#if 0 //1 To do 준비되면 열자
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	if(BUS_MGR_Get(xmgr_cas_NaRCS_MailList_Proc) != NULL)
	{
		xmgr_cas_NaIrdCmdMailUpdateReadFlag(usMailId, FALSE);
		// update mail list
		BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_IRD_MAIL_UPDATE, (Handle_t)NULL, 0, 0, 0);
		xmgr_cas_NaMAIL_ICON_Check();
	}
	else
	{
		NA_MAIL_PRIORITY nPriority = eMailPriortyInvalid;

		xmgr_cas_NaIrdCmdMailGetMailPriority(usMailId, &nPriority);
		HxLOG_Print("[%s] mail priority[%d]", __FUNCTION__, nPriority);

		switch ( nPriority )
		{
			case eMailPriortyNormal:
			case eMailPriortyHigh:
				HxLOG_Print("[%s] eMailPriortyNormal/eMailPriortyHigh [%d]", __FUNCTION__, nPriority);
				// Display Mail Icon
				xmgr_cas_NaIrdCmdMailUpdateReadFlag(usMailId, FALSE);
				// check mail icon
				xmgr_cas_NaMAIL_ICON_Check();
				break;
			case eMailPriortyEmergency:
				HxLOG_Print("[%s] eMailPriortyEmergency [%d]", __FUNCTION__, nPriority);

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
				// 아래 윈도우들인 경우는 pop up을 피한다.
				if((BUS_MGR_Get(AP_EPGMAIN_Proc) == NULL)	// EPG
					// avoid ppv dialog windows
					&& (BUS_MGR_Get(xmgr_cas_NaIppvBuyingPopup_Rcs) == NULL)	// ippv popup window
					&& (BUS_MGR_Get(xproc_cas_NaPpvInfoPopup_Rcs) == NULL)		// ppv info popup window
					&& (BUS_MGR_Get(xmgr_cas_NaRCS_PPVCreditPopup_Proc) == NULL)	// ppv credit popup window
					&& (BUS_MGR_GetPriority(BUS_MGR_GetThis())!= APP_DIALOGBOX_PRIORITY) // avoid dialog box
					)
				{
					// set mail status
					xmgr_cas_NaIrdCmdMailUpdateReadFlag(usMailId, FALSE);
					// 받은 Mail을	대기 리스트에 넣는다.
					HxLOG_Print("[%s] Put Emergency Mail List! Mail Id [%d]", __FUNCTION__, nPriority);
					xmgr_cas_NaRCS_MailMsgArrayPush(usMailId, 1);

					// 이미 text window가 실행된 상태가 아니면 실행 시킨다.
					if ( BUS_MGR_Get(xmgr_cas_NaRCS_MailMsgTextForced_Proc) == NULL)
					{
						// start forced text window
						BUS_MGR_Create("xmgr_cas_NaRCS_MailMsgTextForced_Proc", APP_DEFAULT_PRIORITY, (BUS_Callback_t)xmgr_cas_NaRCS_MailMsgTextForced_Proc, 0, (HINT32)0, 0, 0);
					}
				}
#endif
				// 읽지 않은 메시지가 하나라도 있으면 Mail Icon을 Display한다.
				// 읽지 않은 메시지가 없으면 Mail Icon을 Close한다.
				xmgr_cas_NaMAIL_ICON_Check();
				break;
			case eMailPriortyInvalid:
				HxLOG_Print("[%s] Invalid Priority Value [%d]", __FUNCTION__, nPriority);
				return eCAS_NA_CMD_ERR_FAIL;
			default:
				HxLOG_Print("[%s] Invalid Priority Value [%d]", __FUNCTION__, nPriority);
				return eCAS_NA_CMD_ERR_FAIL;
		}
	}
#endif
#endif
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_displayPopupMsg(void)
{
	HxLOG_Error("Popup Message...>>> NOTHING TO DO\n");

	/* Noting to do */
	return eCAS_NA_CMD_ERR_OK;
}


STATIC HERROR _xmgr_cas_Nacmd_logicalDiskFormat(void)
{
#if 0 //1 To do 준비되면 열자
	HBOOL				bFormat = FALSE;
	HINT32				nIsExternal = 0;
	HUINT32				nActionId = MGR_ACTION_GetMainActionId();
	MgrAction_Type_e		nActionType = MGR_ACTION_GetType(nActionId);
#endif

	HxLOG_Print("Logical Disk Format...\n");

	// Format 명령과 함께 PVR Manager에서 PVR Stop시키므로 여기 있던 PVR Stop 명령들은 삭제한다.

#if 0 //1 To do 준비되면 열자
#if defined(CONFIG_MW_MM_INTERNAL_SATA)
	nIsExternal = 0;
	bFormat = MGR_MEDIA_CheckFormatHddWithoutPartition();

	if(bFormat == TRUE)
	{
		// Spawn Format App
		BUS_MGR_Create ("AP_Media_FormatProc", APP_DEFAULT_PRIORITY,
						AP_Media_FormatProc,
						HANDLE_NULL,
						(HINT32)nIsExternal,
						(HINT32)0,
						(HINT32)0);
	}
#endif
#endif

	return eCAS_NA_CMD_ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
#define MAX_ZONE_ID_LENGTH		6
#define MAX_COUNTRY_CODE_LENGTH		3
static HERROR _xmgr_cas_Nacmd_ZoneId(NA_IRDCMD_OPCODE_t eIrdOpCode)
{
	HERROR nErr = ERR_OK;
	HUINT8 aucZoneId[MAX_ZONE_ID_LENGTH];
	HUINT8 aucCountryCode[MAX_COUNTRY_CODE_LENGTH];
	
	if(eIrdOpCode == eOpCodeInit)
	{
		// received ZoneID
		HxLOG_Print("Received ZoneID\n");

		nErr = xmgr_cas_NaGetZoneId(aucZoneId, MAX_ZONE_ID_LENGTH);
		if(nErr != ERR_OK)
		{
			HxLOG_Error("Can't get ZoneID\n");
			return ERR_FAIL;
		}
	}
	else if(eIrdOpCode == eOpCodeCountryCode)
	{
		// received Country Code
		nErr = xmgr_cas_NaGetCountryCode(aucCountryCode, MAX_COUNTRY_CODE_LENGTH);
		if(nErr != ERR_OK)
		{
			HxLOG_Error("Can't get ZoneID\n");
			return ERR_FAIL;
		}
		HxLOG_Print("CountryCode[%02x %02x %02x]\n", aucCountryCode[0], aucCountryCode[1], aucCountryCode[2]);
	}
	else
	{
		// Cancel ZoneID
		HxLOG_Print("Set ZoneID\n");
	}

	return ERR_OK;
}
#endif

STATIC HERROR _xmgr_cas_Nacmd_process_ird(NaCmdData_t *pstNaCmdData)
{
	NaIrdCmdItem_t *pstIrdCmdItem = NULL;

	if (pstNaCmdData == NULL)
		return eCAS_NA_CMD_ERR_NULL;

	pstIrdCmdItem = (NaIrdCmdItem_t *)(pstNaCmdData->item.pstIrcCmdItem);
	if (pstIrdCmdItem == NULL)
	{
		HxLOG_Error("pstIrdCmdItem == NULL\n");
		return eCAS_NA_CMD_ERR_IRD_NULL;
	}
	switch (pstIrdCmdItem->eIrdCmd)
	{
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
		case eComIDResetPincode :
			#if defined(CONFIG_MW_CAS_NAGRA_OP_SWISS_CANAL)
				xmgr_cas_NaPIN_CODE_Display();
			#endif
			_xmgr_cas_Nacmd_resetPinCode();
			break;

		case eComIDSetPincode :
			_xmgr_cas_Nacmd_setPinCode((HUINT8*)pstIrdCmdItem->ulParam1, (HUINT8)pstIrdCmdItem->ulParam2);
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER)
		case eComIDForceIdentification :
			_xmgr_cas_Nacmd_fingerPrint((HUINT32*)pstIrdCmdItem->ulParam1);
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_TUNE)
		case eComIDForceTune :
			_xmgr_cas_Nacmd_forceTune((Handle_t)pstIrdCmdItem->ulParam1);
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_DEFSETTING)
		case eComIDSetForceDefSetting :
			_xmgr_cas_Nacmd_defaultSetting();
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_USAGE)
		case eComIDChangeUsageID :
			_xmgr_cas_Nacmd_changeUsage((HUINT8)pstIrdCmdItem->ulParam1);
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_REBOOT)
		case eComIDRebootStb :
			_xmgr_cas_Nacmd_rebootStb();
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
		case eComIDSetForceStandby :
			_xmgr_cas_Nacmd_goStandby();
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
		case eComIDMail :
			_xmgr_cas_Nacmd_displayMailMsg((HUINT16)pstIrdCmdItem->ulParam1, (HUINT16)pstIrdCmdItem->ulParam2);
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
		case eComIDDisplayPopUp :
			_xmgr_cas_Nacmd_displayPopupMsg();
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
		case eComIDLogicalDiskFormat :
			_xmgr_cas_Nacmd_logicalDiskFormat();
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eComIDApsHdPlus :
			xmgr_cas_NaIrdCommandUpdated_Aps(pstIrdCmdItem->eIrdOpCode);
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
		case eComIDZoneID:
			_xmgr_cas_Nacmd_ZoneId(pstIrdCmdItem->eIrdOpCode);
#endif
		default :
			break;
	}
	return eCAS_NA_CMD_ERR_OK;
}


//This function is processing only popup or ird message type
STATIC HERROR _xmgr_cas_Nacmd_message_function(void *pNA_data, XmgrCas_NaUiCmdType_e *pCmdType)
{
	HERROR		nReturnError = eCAS_NA_CMD_ERR_OK;
	NaCmdData_t	*pstNaCmdData = NULL;

	pstNaCmdData = (NaCmdData_t *)pNA_data;
	if (pstNaCmdData == NULL)
	{
		HxLOG_Error("pstNaCmdData == NULL\n");
		return eCAS_NA_CMD_ERR_NULL;
	}

	(*pCmdType) = (XmgrCas_NaUiCmdType_e)pstNaCmdData->eCmdType;

	switch (pstNaCmdData->eCmdType)
	{
		case eCmd_PopUp :
			nReturnError = _xmgr_cas_Nacmd_process_Popup(pstNaCmdData);
			break;
		case eCmd_Bdc:
			nReturnError = _xmgr_cas_Nacmd_process_bdc(pstNaCmdData);
			break;
		case eCmd_IrdCmd :
			nReturnError = _xmgr_cas_Nacmd_process_ird(pstNaCmdData);
			break;

		case eCmd_Cop :
			break;

		default :
			break;
	}
	return nReturnError;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_CMD_PUBLIC_FUNCTION______________________________________________________

HERROR xmgr_cas_NaGetPopupData(XmgrCas_NaUiPopupCmd_e *popup_cmd, XmgrCas_NaUiPopupMode_e *popup_mode, HUINT8 **pszMsg)
{
	return _xmgr_cas_Nacmd_get_PopupData(popup_cmd, popup_mode, pszMsg);
}


HERROR xmgr_cas_NaCommandProcessing(void *pNA_data, XmgrCas_NaUiCmdType_e *pCmdType)
{
	return _xmgr_cas_Nacmd_message_function(pNA_data, pCmdType);
}


/*******************************************************************/
/********************      For debug     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
void	xmgr_cas_NaDbgCmdIrdCmdMail(void)
{
	_xmgr_cas_Nacmd_displayMailMsg(0, 0);
	_xmgr_cas_Nacmd_displayMailMsg(1, 0);
	_xmgr_cas_Nacmd_displayMailMsg(2, 0);
	_xmgr_cas_Nacmd_displayMailMsg(3, 0);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
void	xmgr_cas_NaDbgCmdIrdCmdFormatDisk(void)
{
	_xmgr_cas_Nacmd_logicalDiskFormat();
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
void xmgr_cas_NaDbgCmd_IrdCmdPincode(void)
{
	xmgr_cas_NaPIN_CODE_Display();
}
#endif

#endif
