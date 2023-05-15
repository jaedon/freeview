/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif
#include <vkernel.h>
#include <svc_pipe.h>

#include <mgr_action.h>
#include <xmgr_cas_res_str.h>
#include <cx_common.h>
#include <conax.h>

#ifdef CONFIG_DEBUG
#include <cxcore_main.h>
#endif

#include "local_include/_xmgr_cas_cx_adapt.h"
#include "local_include/_xmgr_cas_cx_util.h"
#include "local_include/_xmgr_cas_cx.h"
#include "../../local_include/_xmgr_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/





/*******************************************************************/
/********************      Type Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#define	CX_USER_TEXT_HOUR				3600		// 1 hour
#define	CX_USER_TEXT_MIN				60
#define	CX_USET_TEXT_TIME_LIMIT			60
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT8		s_aulAcceptPin[5];


/************************** Order Info *************************************/
STATIC HUINT32		s_ulCasCxOrderProductID;				// 현재 보여주는 OrderProductID

/************************** Accept View ************************************/
// 사용자가 accept 한 PPV Event들을 저장하여, 채널 전환후 그 Event로 가도 메시지가 안 보이게 한다.
STATIC HUINT32		s_aulAcceptViewProductIDList[CAS_CX_MAX_SAVED_LIST];
STATIC HUINT8		s_ucAcceptViewProductIDCnt = 0;
STATIC Handle_t		s_hAcceptViewMainPlay;
STATIC HUINT32		s_ulAcceptViewProductID;										// 현재 보여주는 AcceptViewProductID

// 사용자가 accept 한 Conax7 PPV 한 Event들을 저장하여, 채널 전환후 그 Event로 가도 메시지가 안 보이게 한다.
STATIC HUINT16		s_ausAcceptDebitProductIDList[CAS_CX_MAX_SAVED_LIST];
STATIC HUINT32		s_aulAcceptDebitProgramLabel[CAS_CX_MAX_SAVED_LIST][CX_MAX_LABEL_LENGTH];
STATIC HUINT8		s_aucAcceptDebitInsuffientTokenList[CAS_CX_MAX_SAVED_LIST];
STATIC HUINT8		s_ucAcceptDebitProductIDCnt = 0;
STATIC Handle_t		s_hAcceptDebitMainPlay;

STATIC HUINT16		s_usAcceptDebitProductID;										// 현재 보여주는 DebitProductID
STATIC HUINT8		s_aucAcceptCurDebitProgramLabel[CX_MAX_LABEL_LENGTH] ;			// 현재 보여주는 Debit ProgramLabel
STATIC HUINT8		s_ucAcceptDebitInsuffientToken;								// 현재 보여주는 DebitProduct 의 InsuffientToken


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
// Error Message
STATIC HERROR		xmgr_cas_CxErrorMessageFunction(void* pCxData, HUINT8 *ucAlarm_type);
STATIC HERROR		xmgr_cas_CxGetErrorData(CxUiMsgInfo_t *pInfo, HUINT8 *pszMsg);

// check Message
STATIC HERROR		xmgr_cas_CxCheckMessageFunction(void* pCxData, HINT32 *lCheck_type);
// --> Get Info Message Data
STATIC HERROR		xmgr_cas_CxGetCheckInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize);
STATIC HERROR		xmgr_cas_CxGetOrderInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize);

// --> Get Check Message Data
STATIC HERROR		xmgr_cas_CxGetMaturityRateData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize);
STATIC HERROR		xmgr_cas_CxGetAcceptViewData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize);
STATIC HERROR		xmgr_cas_CxGetTokenDebitData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize);

// Command Message
STATIC HERROR		xmgr_cas_CxCmdMessageFunction(void* pCxData, HINT32 *lCmd_type);

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
// --> Get Finger Print Data
STATIC void			xmgr_cas_CxGetFingerPrintTime(HUINT32 ulTime, HxDATETIME_Time_t *pstTime);
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)


/*******************************************************************/
/********************      STATIC Functions     ********************/
/*******************************************************************/

#define _____LOCAL_FUNCTION___________________________________


//This function is processing only alarm message type
STATIC HERROR	xmgr_cas_CxErrorMessageFunction(void* pCxData, HUINT8 *ucAlarm_type)
{
	HERROR			hError = ERR_FAIL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[Enter]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)pCxData;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	HxLOG_Print("alarm Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);

	switch (stCxUiInfo->eUiMsgType)
	{
		/*  ------ eCxUi_GroupSc - Smart Card Driver MMI ---------------------- */
		case	eCxUi_GroupSc_Inserted:
			xmgr_cas_CxInitialize();
			*ucAlarm_type = eCxUi_GroupSc;
			hError = ERR_OK;
			break;

		case	eCxUi_GroupSc_NoCard:
		case	eCxUi_GroupSc_Incorrect:
		case	eCxUi_GroupSc_IncorrectConax:
		case	eCxUi_GroupSc_CommFail:
			*ucAlarm_type = eCxUi_GroupSc;
			hError = ERR_OK;
			break;

		/* ---- eCxUi_GroupBasic - Descramble MMI - ACCESS_STATUS -------------*/
		case	eCxUi_GroupBasic_NoAccess:
		case	eCxUi_GroupBasic_NoAccessBlackOut:
		case	eCxUi_GroupBasic_NoAccessNetwork:
		case	eCxUi_GroupBasic_NoAccessPairing:
			*ucAlarm_type = eCxUi_GroupBasic;
			hError = ERR_OK;
			break;

		default:
			HxLOG_Print("Invalid alarm Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);
			*ucAlarm_type = eCxUi_GroupBasic;
			hError = ERR_FAIL;
			break;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetErrorData(CxUiMsgInfo_t *pInfo, HUINT8 *pszMsg)
{
	HINT32		lMsgStrID = 0;
	HUINT8		*pszMsgRef;
	CxUiMsgType_t	eMsg;

	HxLOG_Print("[ENTER]\r\n");

	if (pszMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		//return	ERR_FAIL;
		return ERR_OK;
	}
	eMsg	= pInfo->eUiMsgType;
	switch (eMsg)
	{
		/*  ------ eCxUi_GroupSc - Smart Card Driver MMI ---------------------- */
		case	eCxUi_GroupSc_NoCard:
			lMsgStrID = STR_NO_CARD_INSERTED_ID;
			break;
		case	eCxUi_GroupSc_Incorrect:
			lMsgStrID = STR_INCORRECT_CARD_ID;
			break;
		case	eCxUi_GroupSc_IncorrectConax:
			lMsgStrID = STR_INCORRECT_CONAX_CARD_ID;
			break;
		case	eCxUi_GroupSc_CommFail:
			lMsgStrID = STR_MESG_372_ID;
			break;

		/* ---- eCxUi_GroupBasic - Descramble MMI - ACCESS_STATUS -------------*/
		case	eCxUi_GroupBasic_NoAccess:
			lMsgStrID = STR_NO_ACCESS_ID;
			break;
		case	eCxUi_GroupBasic_NoAccessBlackOut:
			lMsgStrID = STR_MESG_377_ID;
			break;
		case	eCxUi_GroupBasic_NoAccessNetwork:
			lMsgStrID = STR_MESG_378_ID;
			break;

#if defined(CONFIG_MW_CAS_CONAX_CPM)
		case	eCxUi_GroupBasic_NoAccessPairing:
			MWC_UTIL_DvbUtf8Sprintf(
							pszMsg,
							"%s (%d)",
							AP_CasResStr_GetStrRsc(STR_NO_ACCESS_ID),
							pInfo->cpm_nErrCode
						);
			return ERR_OK;
#endif
		// PT에서 SC가 insert 되었을 때 "no insert smartcard" message 남아 있는 것을 문제로 삼네...
		case eCxUi_GroupSc_Inserted:
			return ERR_FAIL;

		default:
			HxLOG_Print("Invalid Msg Type : (0x%x)!\n", eMsg);
			//return	ERR_FAIL;
			return ERR_OK;
	}


	HxLOG_Print("---Alarm Msg Type : (0x%x)!\n", eMsg);

	pszMsgRef = (HUINT8 *)AP_CasResStr_GetStrRsc(lMsgStrID);
	HxLOG_Print("[%s]\n", pszMsgRef);
	if (pszMsgRef != NULL)
	{
		HxLOG_Print("[%s]\n", pszMsgRef);

		MWC_UTIL_DvbStrncpy(pszMsg, pszMsgRef, TEXT_SIZE512 - 1);
		//return	ERR_OK;
	}

	//return	ERR_FAIL;
	return ERR_OK;
}


//This function is processing only Check message type
STATIC HERROR	xmgr_cas_CxCheckMessageFunction(void* pCxData, HINT32 *lCheck_type)
{
	HBOOL			bIsAccept = FALSE;
	HERROR			hError = ERR_FAIL, hErrorSvc = ERR_FAIL;
	HUINT32			ulActionId;
	Handle_t		hMasterSvcHandle = HANDLE_NULL, hCurSvcHandle = HANDLE_NULL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[ENTER]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)pCxData;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	ulActionId = MGR_ACTION_GetMainActionId();
	hErrorSvc = MGR_ACTION_GetMasterSvcHandle(ulActionId, &hCurSvcHandle);
	if (hErrorSvc != ERR_OK)
	{
		hCurSvcHandle = HANDLE_NULL;
		HxLOG_Critical("\n\n");
	}

	switch (stCxUiInfo->eUiMsgType)
	{
		/*  ------ eCxUi_GroupMr - Descramble MMI - ACCESS_STATUS ---------------------- */
		case	eCxUi_GroupMr_Ask:
			HxLOG_Print("---Check Msg Type : eCxUi_GroupMr_Ask\n");
			if (stCxUiInfo->mr_ucProgram > stCxUiInfo->mr_ucSmartcard)
			{
				*lCheck_type = eCAS_CX_POPUP_MODE_CHECK;
				hError = ERR_OK;
			}
			else
			{
				HxLOG_Print("\t -eCxUi_GroupMr_Ask : program level(%d) is smaller than smart card level(%d)!\n", stCxUiInfo->mr_ucProgram, stCxUiInfo->mr_ucSmartcard);
				hError = ERR_FAIL;
			}
			break;

		/*  ------ eCxUi_GroupPpv - Descramble MMI - Order Info ---------------------- */
		case	eCxUi_GroupPpv_OrderInfo:
			HxLOG_Print("---Check Msg Type : eCxUi_GroupPpv_OrderInfo\n");

			// Add Product ID to Product Id List
			xmgr_cas_CxSetPpvMgrProductID(stCxUiInfo->eUiMsgType, stCxUiInfo->av_ulProdRef);

			// Modem & Menu Modem Info 가 없어서 Info Msg 출력. 지원하면 변경되어야 함.
			*lCheck_type = eCAS_CX_POPUP_MODE_INFO;
			hError = ERR_OK;
			break;

		/*  ------ eCxUi_GroupPpv - Descramble MMI - Accept Viewing Info ---------------------- */
		case	eCxUi_GroupPpv_AcceptViewing:
			HxLOG_Print("---Check Msg Type : eCxUi_GroupPpv_AcceptViewing\n");
			// PT에서 AcceptViewing Pin 입력 후 다시 되돌아 올 때 pin을 다시 물어야 한답니다...
			// 따라서 기존정보들 보관할 필요없음...
#if 0
			// Add Product ID to Product Id List
			xmgr_cas_CxSetPpvMgrProductID(stCxUiInfo->eUiMsgType, stCxUiInfo->av_ulProdRef);

			// 1. Compare Product ID from Product List
			bIsAccept = xmgr_cas_CxCompareAcceptViewProductIDList(stCxUiInfo->av_ulProdRef);
			if (bIsAccept == TRUE)
			{
				HxLOG_Print("---Already Accept view current Product \n");
#if 0
				/*
					1. MW단에서는 채널전환을 하여 s/c의 session이 변경된 상태이다...
					2. AP에서는 채널전환을 하였더라도 pin accept를 하지 않는 이상 master service handle이 업데이트 되지 않는다.
					3. 이미 accept한 내역이 있는데도 불구하고 master service handle값이 같다고 판단하여 AP에서 s/c pin을 넣어 주지 않는다.
					4. channel이 변경되지 않으면 s/c에서 eCxUi_GroupPpv_AcceptViewing가 발생하지 않는다. 따라서 service handle을 체크 할 필요가 없을 듯 하다.
				*/

				// 1.1 Check Service Handle
				xmgr_cas_CxGetAcceptViewService(&hMasterSvcHandle);
				if (hMasterSvcHandle != hCurSvcHandle)
				{
					// 1.2 Save Current Service Handle
					xmgr_cas_CxSetAcceptViewService(hCurSvcHandle);

					// 1.3 Aleady Input Pin-code so input auto pin code
					xmgr_cas_CxPinCodeCheckAndAction(eCxUi_GroupPpv_AcceptViewing, hCurSvcHandle);
				}
#else
				xmgr_cas_CxPinCodeCheckAndAction(eCxUi_GroupPpv_AcceptViewing, hCurSvcHandle);
#endif
				// Set Ui Type
				*lCheck_type = (eCAS_CX_POPUP_MODE_INFO | eCAS_CX_POPUP_MODE_USEREXIT);
				hError = ERR_FAIL;
			}
			else
			{
				// 2. Set Ui Type
				*lCheck_type = eCAS_CX_POPUP_MODE_CHECK;
				hError = ERR_OK;
			}
#endif
			*lCheck_type = eCAS_CX_POPUP_MODE_CHECK;
			hError = ERR_OK;
			break;

		/*  ------ eCxUi_GroupPpv - Descramble MMI - Token Debit Info ---------------------- */
		case	eCxUi_GroupPpv_TokenPpv:
			HxLOG_Print("---Check Msg Type : eCxUi_GroupPpv_TokenPpv\n");

			// Add Product ID to Product Id List
			xmgr_cas_CxSetPpvMgrProductID(stCxUiInfo->eUiMsgType, stCxUiInfo->td_usPurseRef);
			// Add Program Lable
			xmgr_cas_CxSetAcceptTokensProgramLable(stCxUiInfo->td_szLabel);

			// 1. Compare Product ID from Product List
			bIsAccept = xmgr_cas_CxCompareAcceptTokensProductIDList(stCxUiInfo->td_usPurseRef, stCxUiInfo->td_szLabel, stCxUiInfo->td_ucInsufficientTokens);
			if (bIsAccept == TRUE)
			{
				HxLOG_Print("---Already Accept view current Product \n");
#if 0
				/*
					1. MW단에서는 채널전환을 하여 s/c의 session이 변경된 상태이다...
					2. AP에서는 채널전환을 하였더라도 pin accept를 하지 않는 이상 master service handle이 업데이트 되지 않는다.
					3. 이미 accept한 내역이 있는데도 불구하고 master service handle값이 같다고 판단하여 AP에서 s/c pin을 넣어 주지 않는다.
					4. channel이 변경되지 않으면 s/c에서 eCxUi_GroupPpv_TokenPpv가 발생하지 않는다. 따라서 service handle을 체크 할 필요가 없을 듯 하다.
				*/

				// 1.1 Check Service Handle
				xmgr_cas_CxGetAcceptTokensService(&hMasterSvcHandle);
				if (hMasterSvcHandle != hCurSvcHandle)
				{
					// 1.2 Save Current Service Handle
					xmgr_cas_CxSetAcceptTokensService(hCurSvcHandle);

					// 1.3 Aleady Input Pin-code so input auto pin code
					xmgr_cas_CxPinCodeCheckAndAction(eCxUi_GroupPpv_TokenPpv, hCurSvcHandle);
				}
#else
				xmgr_cas_CxPinCodeCheckAndAction(eCxUi_GroupPpv_TokenPpv, hCurSvcHandle);
#endif
				// Set Ui Type
				*lCheck_type = eCAS_CX_POPUP_MODE_INFO;
				hError = ERR_FAIL;
			}
			else
			{
				// 2. Set Ui Type
				if (stCxUiInfo->td_ucInsufficientTokens == CX_TOKENS_INSUFFICIENT_UNABLE)
				{
					*lCheck_type = eCAS_CX_POPUP_MODE_INFO;
					hError = ERR_OK;
				}
				else
				{
					*lCheck_type = eCAS_CX_POPUP_MODE_CHECK;
					hError = ERR_OK;
				}
			}
			break;

		default:
			HxLOG_Print("Invalid check Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);
			*lCheck_type =  eCAS_CX_POPUP_MODE_NONE;
			hError = ERR_FAIL;
			break;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	hError;
}


HERROR	xmgr_cas_CxGetCheckData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize)
{
	HERROR			hError = ERR_FAIL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[ENTER]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	switch (stCxUiInfo->eUiMsgType)
	{
		case	eCxUi_GroupMr_Ask:
			hError = xmgr_cas_CxGetMaturityRateData(lMsg, pszMsg, lData, ulMsgSize);
			break;

		case	eCxUi_GroupPpv_AcceptViewing:
			hError = xmgr_cas_CxGetAcceptViewData(lMsg, pszMsg, lData, ulMsgSize);
			break;

		case	eCxUi_GroupPpv_TokenPpv:
			hError = xmgr_cas_CxGetTokenDebitData(lMsg, pszMsg, lData, ulMsgSize);
			break;

		default:
			HxLOG_Print("Invalid check Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);
			hError = ERR_FAIL;
			break;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetCheckInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize)
{
	HERROR			hError = ERR_FAIL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[ENTER]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	switch (stCxUiInfo->eUiMsgType)
	{
		case	eCxUi_GroupPpv_OrderInfo:
			hError = xmgr_cas_CxGetOrderInfoData(lMsg, pszMsg, ulMsgSize);
			break;

		case	eCxUi_GroupPpv_AcceptViewing:
			hError = xmgr_cas_CxGetAcceptViewData(lMsg, pszMsg, (HINT32)eCX_PinCheckStep_InDlg_MAX, ulMsgSize);
			break;

		case	eCxUi_GroupPpv_TokenPpv:
			hError = xmgr_cas_CxGetTokenDebitData(lMsg, pszMsg, (HINT32)eCX_PinCheckStep_InDlg_MAX, ulMsgSize);
			break;

		default:
			HxLOG_Print("Invalid check Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);
			hError = ERR_FAIL;
			break;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetMaturityRateData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize)
{
	HERROR		hError = ERR_OK;
	HUINT8 		*szCurrentSetting = NULL, *szProgramLevel = NULL, *szCardLevel = NULL;
	HUINT8 		*szProgramLevelMat = NULL, *szMatStr = NULL, *szPinDesc = NULL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep;

	HxLOG_Print("[ENTER]\r\n");

	szProgramLevelMat = (HUINT8 *)OxAP_Malloc (64);
	szMatStr = (HUINT8 *)OxAP_Malloc (64);
	if (szProgramLevelMat == NULL || szMatStr == NULL)
	{
		hError = ERR_FAIL;
		goto FINISH;
	}

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	ePinCheckStep = (XmgrCas_CxPinCheckStepInDlg_e)lData;

	// Tilte String
	szCurrentSetting = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MATURITY_LOCK_ID);

	// Card MR Level
	szCardLevel = (HUINT8*)AP_CasResStr_GetStrRsc(STR_CARD_LEVEL_ID);
#if 1	// Show MR Level and context
	xmgr_cas_CxGetMatutiryRateStr(szMatStr, stCxUiInfo->mr_ucSmartcard);
#else
	xmgr_cas_CxGetMatutiryRateLevelStr(szMatStr, stCxUiInfo->mr_ucSmartcard);
#endif

	// Program MR Level
	szProgramLevel = (HUINT8*)AP_CasResStr_GetStrRsc(STR_PROGRAM_RATING_ID);
#if 1	// Show MR Level and context
	xmgr_cas_CxGetMatutiryRateStr(szProgramLevelMat, stCxUiInfo->mr_ucProgram);
#else
	xmgr_cas_CxGetMatutiryRateLevelStr(szProgramLevelMat, stCxUiInfo->mr_ucProgram);
#endif

	if (ePinCheckStep == eCX_PinCheckStep_InDlg_InputPincode)
	{
		szPinDesc = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_376_ID);
	}
	else if (ePinCheckStep == eCX_PinCheckStep_InDlg_WrongPincode)
	{
		szPinDesc = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_397_ID);
	}
	else
	{
		HxLOG_Critical("\n\n");

		hError = ERR_FAIL;
		goto FINISH;
	}

	// can't show string after the first newline character. adding space char. after newline char. it worked well.
	snprintf((char*)pszMsg, ulMsgSize, "%s\n %s : %s\n %s : %s\n %s", szCurrentSetting, szProgramLevel, szProgramLevelMat, szCardLevel, szMatStr, szPinDesc);

	HxLOG_Print("[EXIT]\r\n");

FINISH:
	if (szProgramLevelMat != NULL)			{ OxAP_Free (szProgramLevelMat); }
	if (szMatStr != NULL)					{ OxAP_Free (szMatStr); }

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetOrderInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize)
{
	HERROR			hError = ERR_OK;
	HUINT8 			*szCurrentSetting = NULL, *szStr = NULL;
	HUINT8 			*szLabelStr = NULL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[ENTER]\r\n");

	szLabelStr = (HUINT8 *)OxAP_Malloc (128);
	if (szLabelStr == NULL)					{ return ERR_FAIL; }

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;

	// Title
	szCurrentSetting = (HUINT8*)AP_CasResStr_GetStrRsc(STR_ORDERING_ID);
	// Content
	szStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_NO_MODEM_AVAILABLE_ID);

	// Msg
	snprintf((char*)pszMsg, ulMsgSize, "%s\n %s\n%X.%02X %s\n %s", szCurrentSetting, stCxUiInfo->or_szLabel,
				stCxUiInfo->or_usPriceUnit, stCxUiInfo->or_ucPriceDecimal, stCxUiInfo->or_aucCurrency, szStr);

	HxLOG_Print("[EXIT]\r\n");

	OxAP_Free (szLabelStr);

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetAcceptViewData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize)
{
	HERROR			hError = ERR_OK;
	HUINT8 			*szCurrentSetting = NULL, *szStr = NULL, *szPinStr = NULL;
	HUINT8 			*szLabelStr = NULL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep;

	HxLOG_Print("[ENTER]\r\n");

	szLabelStr = OxAP_Malloc (128);
	if (szLabelStr == NULL)				{ return ERR_FAIL; }

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	ePinCheckStep = (XmgrCas_CxPinCheckStepInDlg_e)lData;

	// Title
	szCurrentSetting = (HUINT8*)AP_CasResStr_GetStrRsc(STR_START_VIEWING_ID);
	// Content
	szStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_359_ID);

	if (ePinCheckStep == eCX_PinCheckStep_InDlg_InputPincode)
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_391_ID);
	}
	else if (ePinCheckStep == eCX_PinCheckStep_InDlg_WrongPincode)
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_397_ID);
	}
	else
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_396_ID);
	}

	// Msg
	snprintf((char*)pszMsg, ulMsgSize, "%s\n %s\n %s : %d\n %s", szCurrentSetting, stCxUiInfo->av_szLabel,
				szStr, stCxUiInfo->av_usMinuteLeft, szPinStr);

	HxLOG_Print("[EXIT]\r\n");

	OxAP_Free (szLabelStr);

	return	hError;
}


STATIC HERROR	xmgr_cas_CxGetTokenDebitData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize)
{
	HERROR			hError = ERR_OK;
	HUINT8 			*szCurrentSetting = NULL, *szStr = NULL, *szStr2 = NULL, *szTemp = NULL, *szPinStr = NULL;
	HUINT8 			*szLabelStr = NULL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	CxStatusList_t 	*pstStatusList = NULL, *pstStatusListTmp = NULL;
	XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep;
	CxStatusList_t 	stList;
	pstStatusList = &stList;

	HxLOG_Print("[ENTER]\r\n");

	szLabelStr = (HUINT8 *)OxAP_Malloc (128);
	szStr = (HUINT8 *)OxAP_Malloc (128);
	szStr2 = (HUINT8 *)OxAP_Malloc (128);
	if (szLabelStr == NULL || szStr == NULL || szStr2 == NULL)
	{
		hError = ERR_FAIL;
		goto FINISH;
	}

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	ePinCheckStep = (XmgrCas_CxPinCheckStepInDlg_e)lData;

	// Title
	szCurrentSetting = (HUINT8*)AP_CasResStr_GetStrRsc(STR_TOKENS_ACCESS_ID);

	// Get Status Info - Purse Status
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Purse, stCxUiInfo->td_usPurseRef);
	if (pstStatusList == NULL)
	{
		HxLOG_Critical("\n\n");

		hError = ERR_FAIL;
		goto FINISH;
	}

	pstStatusListTmp = pstStatusList;

	// Token / Time&Event
	if (stCxUiInfo->td_ucTokenMode == CX_TOKENS_MODE_EVENT)
	{
		if ((stCxUiInfo->td_ulEventCost > 1000) && (stCxUiInfo->td_ulEventCost < 1000000))
		{
			sprintf((char*)szStr, (char*)"%d,%03d %s", stCxUiInfo->td_ulEventCost/1000, stCxUiInfo->td_ulEventCost%1000, (HUINT8*)AP_CasResStr_GetStrRsc(STR_TOKENS_ID)) ;
		}
		else if (stCxUiInfo->td_ulEventCost > 1)
		{
			snprintf((char*)szStr, 128, (char*)"%d %s", stCxUiInfo->td_ulEventCost, (HUINT8*)AP_CasResStr_GetStrRsc(STR_TOKENS_ID)) ;
		}
		else
		{
			snprintf((char*)szStr, 128, (char*)"%d %s", stCxUiInfo->td_ulEventCost, (HUINT8*)AP_CasResStr_GetStrRsc(STR_TOKEN_ID)) ;
		}
	}
	else	// (stCxUiInfo->td_ucTokenMode = CX_TOKENS_MODE_TIME)
	{
		if (stCxUiInfo->td_ulEventCost == 1)
		{
			strncpy ((char *)szStr, (char *)AP_CasResStr_GetStrRsc(STR_MESG_346_ID), 127);
			szStr[127] = '\0';
		}
		else
		{
			snprintf((char*)szStr, 128, (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_345_ID), stCxUiInfo->td_ulEventCost) ;
		}
	}

	// Purse Balance
	szTemp = (HUINT8*)AP_CasResStr_GetStrRsc(STR_PURSE_BALANCE_ID);
	if ((pstStatusListTmp->stStatus.stPurse.ulBalance >1000) && (pstStatusListTmp->stStatus.stPurse.ulBalance < 1000000))
	{
		snprintf((char*)szStr2, 128, (char*)"%s : %s - %d,%03d %s", szTemp, pstStatusListTmp->stStatus.stPurse.szLabel,
			pstStatusListTmp->stStatus.stPurse.ulBalance/1000, pstStatusListTmp->stStatus.stPurse.ulBalance%1000,
			(stCxUiInfo->td_ulEventTag > 1 ) ? AP_CasResStr_GetStrRsc(STR_TOKENS_ID) : AP_CasResStr_GetStrRsc(STR_TOKEN_ID)) ;
	}
	else
	{
		snprintf((char*)szStr2, 128, (char*)"%s : %s - %d %s", szTemp, pstStatusListTmp->stStatus.stPurse.szLabel, pstStatusListTmp->stStatus.stPurse.ulBalance,
			(stCxUiInfo->td_ulEventTag > 1 ) ? AP_CasResStr_GetStrRsc(STR_TOKENS_ID) : AP_CasResStr_GetStrRsc(STR_TOKEN_ID)) ;
	}


	if (ePinCheckStep == eCX_PinCheckStep_InDlg_InputPincode)
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_374_ID);
	}
	else if (ePinCheckStep == eCX_PinCheckStep_InDlg_WrongPincode)
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_397_ID);
	}
	else
	{
		szPinStr = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_408_ID);
	}

	// Msg
	snprintf((char*)pszMsg, ulMsgSize, "%s\n %s\n %s\n %s\n %s", szCurrentSetting, stCxUiInfo->td_szLabel, szStr, szStr2, szPinStr);

FINISH:
	HxLOG_Print("[EXIT]\r\n");

	if (szLabelStr != NULL)				{ OxAP_Free (szLabelStr); }
	if (szStr != NULL)					{ OxAP_Free (szStr); }
	if (szStr2 != NULL)					{ OxAP_Free (szStr2); }

	return	hError;
}


STATIC HERROR	xmgr_cas_CxCmdMessageFunction(void* pCxData, HINT32 *lCmd_type)
{
	HERROR			hError = ERR_FAIL;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[Enter]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)pCxData;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	switch (stCxUiInfo->eUiMsgType)
	{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		case	eCxUi_GroupMessage_Display:
			HxLOG_Print("---Cmd Msg Type : eCxUi_GroupMessage_Display\n");
			*lCmd_type = eCAS_CX_POPUP_MODE_USERTEXT;
			hError = ERR_OK;
			break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		/*  ------ eCxUi_GroupOther - Finger Print ---------------------- */
		case	eCxUi_GroupOther_Fingerprint:
			HxLOG_Print("---Cmd Msg Type : eCxUi_GroupOther_Fingerprint\n");
			*lCmd_type = eCAS_CX_POPUP_MODE_FINGERPRINT;
			hError = ERR_OK;
			break;
#endif

		default:
			HxLOG_Print("Invalid & Not Support cmd Msg Type : (0x%x)!\n", stCxUiInfo->eUiMsgType);
			*lCmd_type = eCAS_CX_POPUP_MODE_NONE;
			hError = ERR_FAIL;
			break;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	hError;
}


#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
STATIC void	xmgr_cas_CxGetFingerPrintTime(HUINT32 ulTime, HxDATETIME_Time_t *pstTime)
{
	HUINT8		ucData, ucData1, ucData2;
	HUINT8 		ucHour, ucMinutes, ucSeconds;
	HUINT16		usCentiSeconds;

	ucData = (ulTime >> 16) & 0xff;
	ucData1 = (ulTime >> 8) & 0xff;
	ucData2= (ulTime & 0xff);

	if ((ucData == 0xFF) && (ucData1 == 0xFF) && (ucData2 == 0xFF))
	{
		pstTime->ucHour = 0xFF;
		pstTime->ucMinute = 0xFF;
		pstTime->ucSecond = 0xFF;
		pstTime->usMillisecond = 0xFF;
	}
	else
	{
		// Hour
		ucHour = ucData;
		ucHour = (ucHour & 0xF8) >> 3;
		pstTime->ucHour = ucHour;

		// Min
		ucHour = ucData;
		ucMinutes = ucData1;
		ucMinutes = (ucMinutes & 0xE0) >> 5;
		ucHour = (ucHour & 0x07) << 3;
		ucMinutes = ucMinutes | ucHour;
		pstTime->ucMinute = ucMinutes;

		// Seconds
		ucMinutes = ucData1;
		ucSeconds = ucData2;
		ucMinutes = (ucMinutes & 0x1F) << 1;
		ucSeconds = (ucSeconds & 0x80) >> 7;
		ucSeconds = ucMinutes | ucSeconds;
		pstTime->ucSecond = ucSeconds;

		// Centi
		ucSeconds = ucData2;
		ucSeconds = (ucSeconds & 0x7F);
		usCentiSeconds = ucSeconds * 10;
		pstTime->usMillisecond = usCentiSeconds;
	}
	HxLOG_Print(" Origianl FP Time :  Hour(%d), Min(%d), Sec(%d)\n", pstTime->ucHour, pstTime->ucMinute, pstTime->ucSecond);
}
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#define	____Global_Function____________________________________

/*******************************************************************/
/********************      Global Functions     ********************/
/*******************************************************************/

void	xmgr_cas_CxSetPpvMgrProductID(CxUiMsgType_t eUiMsgType, HUINT32 ulProductID)
{
	HxLOG_Print("[ENTER]\r\n");

	switch (eUiMsgType)
	{
		case	eCxUi_GroupPpv_OrderInfo:
			xmgr_cas_CxSetOrderProductId(ulProductID);
			break;

		case	eCxUi_GroupPpv_AcceptViewing:
			xmgr_cas_CxSetAcceptViewProductId(ulProductID);
			break;

		case	eCxUi_GroupPpv_TokenPpv:
			xmgr_cas_CxSetAcceptTokensProductId(ulProductID);
			break;

		default:
			HxLOG_Print("\t Invalid PPV MGR MsgType : 0x%x\r\n", eUiMsgType);
			HxLOG_Critical("\n\n");
			break;
	}

	HxLOG_Print("[EXIT]\r\n");
}


void	xmgr_cas_CxSetPpvMgrAction(CxUiMsgType_t eUiMsgType, HUINT8* szPin)
{
	HxLOG_Print("[ENTER]\r\n");

	switch (eUiMsgType)
	{
		case	eCxUi_GroupPpv_AcceptViewing:
			xmgr_cas_CxSetAcceptViewProductIDList(szPin);
			break;

		case	eCxUi_GroupPpv_TokenPpv:
			xmgr_cas_CxSetAcceptTokensProductIDList(szPin);
			break;

		default:
			HxLOG_Print("\t Invalid PPV MGR Action MsgType : 0x%x\r\n", eUiMsgType);
			HxLOG_Critical("\n\n");
			break;
	}
	HxLOG_Print("[EXIT]\r\n");
}


void	xmgr_cas_CxSetAcceptCAPincode(HUINT8 *szPin)
{
	HxLOG_Print("\t Set Accept CA Pin : Insert %s\r\n", szPin);
	memcpy(s_aulAcceptPin, szPin, sizeof(HUINT8)*5);
	s_aulAcceptPin[5-1] = '\0';
}


void	xmgr_cas_CxGetAcceptCAPincode(HUINT8 *szPin)
{
	memcpy(szPin , s_aulAcceptPin, sizeof(HUINT8)*5);
	szPin[5-1] = '\0';
}


#define	____MMI_ERROR_Message____________________________________

HERROR		xmgr_cas_CxErrorDataProcessing(void *pDataCx, HUINT8 *ucAlarm_type)
{
	return	xmgr_cas_CxErrorMessageFunction(pDataCx, ucAlarm_type);
}


HERROR		xmgr_cas_CxGetErrorData(HINT32 lMsg, HUINT8 *pszMsg)
{
	CxUiMsgInfo_t	*stCxUiInfo;

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;

	return	xmgr_cas_CxGetErrorData((CxUiMsgInfo_t *)lMsg, pszMsg);
}


HERROR		xmgr_cas_CxCheckDataProcessing(void *pDataCx, HINT32 *lCheck_type)
{
	return	xmgr_cas_CxCheckMessageFunction(pDataCx, lCheck_type);
}

HERROR		xmgr_cas_CxCommandProcessing(void *pDataCx, HINT32 *lCmd_type)
{
	return	xmgr_cas_CxCmdMessageFunction(pDataCx, lCmd_type);
}


HERROR		xmgr_cas_CxGetInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize)
{
	return	xmgr_cas_CxGetCheckInfoData(lMsg, pszMsg, ulMsgSize);
}


#define	_______Order_Info____________________________________

void	xmgr_cas_CxSetOrderProductId(HUINT32 ulOrderID)
{
	s_ulCasCxOrderProductID = ulOrderID;
}


#define	_______Accept_View____________________________________
/*  Accept Viewing List control */

void	xmgr_cas_CxSetAcceptViewProductId(HUINT32 ulAcceptViewID)
{
	s_ulAcceptViewProductID = ulAcceptViewID;
}


void	xmgr_cas_CxSetAcceptViewProductIDList(HUINT8* szPin)
{
	s_aulAcceptViewProductIDList[s_ucAcceptViewProductIDCnt] = s_ulAcceptViewProductID;
	memcpy(s_aulAcceptPin, szPin, sizeof(HUINT8)*5);
	s_aulAcceptPin[5 - 1] = '\0';
	HxLOG_Print(" Set Accept Product ID List : Insert s_aulAcceptViewProductIDList[%d]0x%x,\r\n", s_ucAcceptViewProductIDCnt, s_ulAcceptViewProductID);
	s_ucAcceptViewProductIDCnt = (s_ucAcceptViewProductIDCnt+1)%CAS_CX_MAX_SAVED_LIST;
}

HBOOL	xmgr_cas_CxCompareAcceptViewProductIDList(HUINT32 ulAcceptProductID)
{
	HUINT32	i;

	for (i=0; i < CAS_CX_MAX_SAVED_LIST; i++)
	{
		if (s_aulAcceptViewProductIDList[i] == ulAcceptProductID)
		{
			return	TRUE;
		}
	}

	return	FALSE;
}


void	xmgr_cas_CxSetAcceptViewService(Handle_t hSvcAccept)
{
	s_hAcceptViewMainPlay = hSvcAccept;
}


void	xmgr_cas_CxGetAcceptViewService(Handle_t *hSvcAccept)
{
	*hSvcAccept = s_hAcceptViewMainPlay;
}


void	xmgr_cas_CxClearAcceptViewProductIDList(void)
{
	s_ucAcceptViewProductIDCnt = 0;
	xmgr_cas_CxSetAcceptViewService(HANDLE_NULL);
	memset(s_aulAcceptViewProductIDList, 0, sizeof(s_aulAcceptViewProductIDList));
}


#define	_______Tokens_Debit____________________________________

// 사용자가 accept 한 Conax7 PPV 한 Event들을 저장하여, 채널 전환후 그 Event로 가도 메시지가 안 보이게 한다.

void	xmgr_cas_CxSetAcceptTokensProductId(HUINT32 ulAcceptTokensID)
{
	s_usAcceptDebitProductID = (HUINT16)ulAcceptTokensID;
}


void	xmgr_cas_CxSetAcceptTokensProgramLable(HUINT8 *szPrgramLable)
{
	memcpy(s_aucAcceptCurDebitProgramLabel, szPrgramLable, CX_MAX_LABEL_LENGTH);
}


void	xmgr_cas_CxSetAcceptTokensProductIDList(HUINT8 *szPin)
{
	s_ausAcceptDebitProductIDList[s_ucAcceptDebitProductIDCnt] = s_usAcceptDebitProductID;
	s_aucAcceptDebitInsuffientTokenList[s_ucAcceptDebitProductIDCnt] = FALSE;//TRUE; 이미 accept 했다면 token이 충분한거 아닌가?? 코드상으로는 FALSE가 되는게 맞을 듯 한데...
	memcpy(s_aulAcceptDebitProgramLabel[s_ucAcceptDebitProductIDCnt], s_aucAcceptCurDebitProgramLabel, CX_MAX_LABEL_LENGTH);
	memcpy(s_aulAcceptPin, szPin, sizeof(HUINT8)*5);
	s_aulAcceptPin[5 - 1] = '\0';
	HxLOG_Print(" Set Accept Product ID List : Insert s_AcceptTokensProductIDList[%d]0x%x,\r\n", s_ucAcceptDebitProductIDCnt, s_usAcceptDebitProductID);
	s_ucAcceptDebitProductIDCnt=(s_ucAcceptDebitProductIDCnt+1)%CAS_CX_MAX_SAVED_LIST;
}


HBOOL	xmgr_cas_CxCompareAcceptTokensProductIDList(HUINT16 nAcceptDebitProductID, HUINT8 *ucAcceptDebitProgramLabel, HUINT8 ucInsuffientToken)
{
	HUINT32		i;

	for (i=0; i < CAS_CX_MAX_SAVED_LIST; i++)
	{
		if(s_ausAcceptDebitProductIDList[i] == nAcceptDebitProductID && !memcmp(s_aulAcceptDebitProgramLabel[i], ucAcceptDebitProgramLabel, CX_MAX_LABEL_LENGTH) )
		{
			if( s_aucAcceptDebitInsuffientTokenList[i] == ucInsuffientToken && !ucInsuffientToken) // 이미 OK한 것이고, 살 token 도 존재
			{
				return	TRUE;
			}
			else // 이미 OK한 것이나, token 값이 변화되어 화면에 보여줘야 함.(토큰이 부족한 경우와, 부족한 상태에서 토큰을 recharge 한 경우)
			{
				s_aucAcceptDebitInsuffientTokenList[i] = ucInsuffientToken;
				return	FALSE;
			}
		}
	}

	return	FALSE;
}


void	xmgr_cas_CxSetAcceptTokensService(Handle_t hSvcAccept)
{
	s_hAcceptDebitMainPlay = hSvcAccept;
}


void	xmgr_cas_CxGetAcceptTokensService(Handle_t *hSvcAccept)
{
	*hSvcAccept = s_hAcceptDebitMainPlay;
}


void	xmgr_cas_CxClearAcceptTokensProductIDList(void)
{
	HUINT32		i;

	s_ucAcceptDebitProductIDCnt = 0;
	s_hAcceptDebitMainPlay = HANDLE_NULL;

	xmgr_cas_CxSetAcceptTokensService(HANDLE_NULL);
	memset(s_ausAcceptDebitProductIDList, 0, sizeof(s_ausAcceptDebitProductIDList));
	memset(s_aucAcceptDebitInsuffientTokenList, 0, sizeof(s_aucAcceptDebitInsuffientTokenList));
	for(i=0; i < CAS_CX_MAX_SAVED_LIST; i++)
	{
		memset(s_aulAcceptDebitProgramLabel[i], 0, CX_MAX_LABEL_LENGTH);
	}
}


#define	_______Finger_Print____________________________________
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
HERROR		xmgr_cas_CxOnFingerPrint(HINT32 lMsg, HBOOL *bTimePrint, HULONG *ulTime, HUINT16 *usDuration)
{
	HINT32			lDiffDisplayTime = 0;
	HUINT16			usFPDuration = 0, usTmpDuration;
	HUINT32			ulError;
	UNIXTIME		ulCurrentTime = 0, ulFPUnixTime=0;
	HxDATETIME_Time_t		stFingerPrintTime;
	HxDATETIME_t	stCurrentDateTime, stFPDateTime;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	HUINT8			*ucFPTime;

	HxLOG_Print("[ENTER]\r\n");

	*bTimePrint = FALSE;
	*usDuration = 0;
	*ulTime = 0;

	// Ui Data
	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	// Get Current Time
	ulError = VK_CLOCK_GetTime(&ulCurrentTime);
	if (ulError != VK_OK)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}
	ucFPTime = stCxUiInfo->fp_ucCentTime;
	if(ucFPTime == NULL)
	{
		HxLOG_Error("FP Time is null....\n");
		return ERR_FAIL;
	}
	memset(&stCurrentDateTime, 0, sizeof(HxDATETIME_t));
	memset(&stFPDateTime, 0, sizeof(HxDATETIME_t));

	// Conv Current time to date time - with out GMT Offset
	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stCurrentDateTime);

	// FP는 년월일 정보를 current로 받는다.
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrentTime, &stFPDateTime);

	// 시간정보는 실제 데이터로 set
	stFPDateTime.stTime.ucHour = ucFPTime[0];
	stFPDateTime.stTime.ucMinute = ucFPTime[1];
	stFPDateTime.stTime.ucSecond = ucFPTime[2];
	stFPDateTime.stTime.usMillisecond = (HUINT16)ucFPTime[3];

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurrentDateTime, &ulCurrentTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stFPDateTime, &ulFPUnixTime);

	HxLOG_Print(" == Current Time Year(%d), Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d)\n",
		stCurrentDateTime.stDate.usYear, stCurrentDateTime.stDate.ucMonth, stCurrentDateTime.stDate.ucDay,
		stCurrentDateTime.stTime.ucHour, stCurrentDateTime.stTime.ucMinute, stCurrentDateTime.stTime.ucSecond);

	HxLOG_Print(" == Fingerprint Time Year(%d), Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d)\n",
		stFPDateTime.stDate.usYear, stFPDateTime.stDate.ucMonth, stFPDateTime.stDate.ucDay,
		stFPDateTime.stTime.ucHour, stFPDateTime.stTime.ucMinute, stFPDateTime.stTime.ucSecond);

	/* display immediately */
	if (stCxUiInfo->fp_ulTime == CX_FINGER_PRINT_IMMEDIATELY_TIME)
	{
		*bTimePrint = FALSE;
		HxLOG_Print(" - Immediately fingerprint!! \n");
		return	ERR_OK;
		//UTIL_TCONV_ConvertDateTimeToUnixTimeWithOutOffset(&stCurrentDateTime, &ulFPTime);
	}

	// 미래의 시간
	if (ulCurrentTime < ulFPUnixTime)
	{
		lDiffDisplayTime = ulFPUnixTime - ulCurrentTime;
		*ulTime = (lDiffDisplayTime*CAS_CX_SECONDS);
		*bTimePrint = TRUE;

		HxLOG_Print("- Current Time : %d, Finger Time : %d, Diff : %d\n", ulCurrentTime, ulFPUnixTime, (ulFPUnixTime - ulCurrentTime));
		HxLOG_Print("- FingerPrintDisplay Timer %d\n", lDiffDisplayTime);
	}
	else // 현재의 시간
	{
		*bTimePrint = FALSE;
		usFPDuration = stCxUiInfo->fp_usDuration;
		usTmpDuration = (HUINT16)(ulCurrentTime - ulFPUnixTime)*100;

		// 현재(즉시) 시간 display하는 경우는 다른 처리하지 않고 그냥 진행 함.

		// 과거 시간
		if (ulFPUnixTime + (usFPDuration/100) < ulCurrentTime)
		{
			HxLOG_Error(" - Past fingerprint!! \n");
			return	ERR_FAIL;
		}

		if (usFPDuration >usTmpDuration)
		{
			usFPDuration -= usTmpDuration;
			*usDuration = usFPDuration;
		}
		HxLOG_Print("- Current Time : %d, Finger Time : %d, Diff : %d\n", ulCurrentTime, ulFPUnixTime, ulFPUnixTime - ulCurrentTime);
		HxLOG_Print("- FingerPrint Duration Time %d\n", usFPDuration);
	}

	HxLOG_Print("- FingerPrintDisplay X:%d, Y:%d - Msg %s\n", stCxUiInfo->fp_usPosX, stCxUiInfo->fp_usPosY, stCxUiInfo->fp_szText);

	HxLOG_Print("[EXIT]\r\n");

	return	ERR_OK;
}


void	xmgr_cas_CxSetFingerPrintDisplayed(void)
{
	CXAPI_UiFingerprintDisplayed();
}
#endif	// End of #if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)


#define	_______USER_TEXT____________________________________
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC HERROR		xmgr_cas_CxAdjustUTIntervalTimeOnBoot(CxUiMsgInfo_t* stCxUiInfo, HBOOL *bUserTextTime, HULONG *ulTime, HUINT32 ulCurrentTime, UNIXTIME ulUTTime)
{
	int 			i = 0;
	HUINT32 		ulDiffTime = 0, ulSec = 0;
	HxDATETIME_t	stDateTime;
	HUINT32			ulMaxOffsetTime = 0;
	UNIXTIME 		ulUxTime = 0;	// 일정 횟수 반복이 되었다고 판단하고 Time계산.

	HxLOG_Print("[UT] Displayed(%d), Duration(%d), Repeat(%d)\n",stCxUiInfo->ut_ucDisplayed,stCxUiInfo->ut_usDuration, stCxUiInfo->ut_ucRepeat);

	if(stCxUiInfo->ut_ucDisplayed == 0 || stCxUiInfo->ut_usDuration == 0)
	{
		// Mail Message의 경우와
		// (반복 User Text의 경우)한번도 Display 되지 않은 경우는, 지정 시간에 뿌리도록 그냥 보낸다.
		return ERR_FAIL;
	}

	// 반복할 데이터가 있는 경우
	if((stCxUiInfo->ut_ucDisplayed > 0) && (stCxUiInfo->ut_ucDisplayed < stCxUiInfo->ut_ucRepeat))
	{
		ulMaxOffsetTime = ((stCxUiInfo->ut_ucRepeat - 1) * stCxUiInfo->ut_usInterval);

		// User Text를 Display하는 시간 보정 - 흘러간 시간만큼 보정.
		for(i = 0; i<stCxUiInfo->ut_ucDisplayed; i++)
		{
			// Interval은 다음 User Text의 Display time을 의미한다.
			ulUxTime = ulUTTime + stCxUiInfo->ut_usInterval;
		}

		if(ulCurrentTime < ulUxTime)
		{
			ulDiffTime = (ulUTTime - ulCurrentTime);

			HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
			ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;

			HxLOG_Print("[1]ADJUST- OFS: start:%d/%d/%d %d:%02d:%02d\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay
				, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
			HxLOG_Print("[1]ADJUST- Display in %d seconds!\n", ulSec);

			*ulTime = (ulSec*CAS_CX_SECONDS);

			*bUserTextTime = TRUE;
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

				*ulTime = (ulSec*CAS_CX_SECONDS);

				*bUserTextTime = TRUE;
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
HERROR		xmgr_cas_CxOnUserText(HINT32 lMsg, CxUiMsgInfo_t	*stPrevCxUiUTInfo, HBOOL *bUserTextTime, HULONG *ulTime)
{
	HUINT32			ulError;
	HUINT32			ulCurrentTime = 0, ulDiffTime = 0, ulSec = 0;
	UNIXTIME		ulUTTime;
	HxDATETIME_t	stCurrentDateTime, stDateTime;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	HUINT32				ulNeedStopHere = 0;

	HxLOG_Print("[ENTER]\r\n");

	*bUserTextTime = FALSE;
	*ulTime = 0;

	// Ui Data
	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	// Check Same Msg
	if ((stCxUiInfo->ut_ucSeqNo == stPrevCxUiUTInfo->ut_ucSeqNo) &&
		(MWC_UTIL_DvbStrcmp(stCxUiInfo->ut_szText, stPrevCxUiUTInfo->ut_szText) == 0) &&
		(stCxUiInfo->ut_usDuration == stPrevCxUiUTInfo->ut_usDuration) &&
		(stCxUiInfo->ut_usInterval == stPrevCxUiUTInfo->ut_usInterval))
	{
		// it's same message
		HxLOG_Print("- User Text same message! ignore!\n");
		return	ERR_FAIL;
	}

	// Get Current Time
	ulError = VK_CLOCK_GetTime(&ulCurrentTime);
	if (ulError != VK_OK)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	// Conv Current time to date time with GMT Offset
	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stCurrentDateTime);

	HxLOG_Print("- seq[%d], Text[%s], Duration[%d], Repetition[%d], Interval[%d]\n",
		stCxUiInfo->ut_ucSeqNo, stCxUiInfo->ut_szText, stCxUiInfo->ut_usDuration, stCxUiInfo->ut_ucRepeat, stCxUiInfo->ut_usInterval);
	HxLOG_Print("- Current Time:%d/%d/%d %d:%02d:%02d\n", stCurrentDateTime.stDate.usYear, stCurrentDateTime.stDate.ucMonth,
		stCurrentDateTime.stDate.ucDay, stCurrentDateTime.stTime.ucHour, stCurrentDateTime.stTime.ucMinute, stCurrentDateTime.stTime.ucSecond);
	HxLOG_Print("- UT Time:%d/%d/%d %d:%02d:%02d\n", stCxUiInfo->ut_stStartDataTime.stDate.usYear, stCxUiInfo->ut_stStartDataTime.stDate.ucMonth,
		stCxUiInfo->ut_stStartDataTime.stDate.ucDay, stCxUiInfo->ut_stStartDataTime.stTime.ucHour, stCxUiInfo->ut_stStartDataTime.stTime.ucMinute, stCxUiInfo->ut_stStartDataTime.stTime.ucSecond);

	// Conv user text start date time to unixtime
	HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&stCxUiInfo->ut_stStartDataTime, &ulUTTime);

	ulNeedStopHere = xmgr_cas_CxAdjustUTIntervalTimeOnBoot(stCxUiInfo, bUserTextTime, ulTime, ulCurrentTime, ulUTTime);

	if(ulNeedStopHere == ERR_OK)
	{
		HxLOG_Error("After Boot, adjust UT Time with Current Time - timer(%d)\n",ulTime);
		return	ERR_OK;
	}

	if ((ulCurrentTime == ulUTTime)
		|| ((stCxUiInfo->ut_stStartDataTime.stDate.usYear == 0xFF)&&(stCxUiInfo->ut_stStartDataTime.stDate.ucMonth == 0xFF)&&(stCxUiInfo->ut_stStartDataTime.stDate.ucDay == 0xFF))
		|| ((stCxUiInfo->ut_stStartDataTime.stDate.usYear == 0x0)&&(stCxUiInfo->ut_stStartDataTime.stDate.ucMonth == 0x0)&&(stCxUiInfo->ut_stStartDataTime.stDate.ucDay == 0x0)))
	{
		HxLOG_Print("- User Text Display Immediately!\n");

		*bUserTextTime = FALSE;
	}
#if 0 // TODO: 나중에 이럴 경우가 생기면 처리
	else if (ulCurrentTime>ulUTTime)
	{
		/* 시간이 일치 할 수 없다. Uset Text 의 경우  Second Time이 0으로 처리 되어서 60이하 차이의 경우 출력 가능한 시간 이라고 본다. */
		ulDiffTime = (ulCurrentTime - ulUTTime);
		UTIL_TCONV_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
		ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;
		if (ulSec < CX_USET_TEXT_TIME_LIMIT)
		{
			*bUserTextTime = FALSE;
		}
		else
		{
			HxLOG_Print("- User Text Display past time !\n");
			return	ERR_FAIL;
		}
	}
#endif
	else if (ulUTTime > ulCurrentTime)
	{
		HxLOG_Print("- User Text Display Timer!\n");

		*bUserTextTime = TRUE;
		ulDiffTime = (ulUTTime - ulCurrentTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulDiffTime, &stDateTime);
		ulSec = (stDateTime.stTime.ucHour*CX_USER_TEXT_HOUR) + (stDateTime.stTime.ucMinute*CX_USER_TEXT_MIN) + stDateTime.stTime.ucSecond;

		HxLOG_Print("- OFS: start:%d/%d/%d %d:%02d:%02d\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay
			, stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
		HxLOG_Print("- Display in %d seconds!\n", ulSec);

		*ulTime = (ulSec*CAS_CX_SECONDS);
	}
	else
	{
		HxLOG_Error("Past Time- UT Time:%d/%d/%d %d:%02d:%02d\n", stCxUiInfo->ut_stStartDataTime.stDate.usYear, stCxUiInfo->ut_stStartDataTime.stDate.ucMonth,
														stCxUiInfo->ut_stStartDataTime.stDate.ucDay, stCxUiInfo->ut_stStartDataTime.stTime.ucHour,
														stCxUiInfo->ut_stStartDataTime.stTime.ucMinute, stCxUiInfo->ut_stStartDataTime.stTime.ucSecond);

		// Immediate Msg만 해당
		if(stCxUiInfo->ut_usDuration != 0)
		{
			// 과거 시간의 msg는 Display하지 않도록 하기위함
			xmgr_cas_CxSetUserTextDisplayed(stCxUiInfo->ut_ucSeqNo,eCxUTDisplayCntMax);
		}

		HxLOG_Critical("\n\n");

		return	ERR_FAIL;
	}

	HxLOG_Print("[EXIT]\r\n");

	return	ERR_OK;
}

/*
	Displayed setting은 하나의 User Text의 Duration이 끝나는 시점에 한다.
	이유는, Immediate Msg의 경우,
	Display 중에 Power Off 해버리면, 재부팅 후, Flash에 동일한 데이터가 있는것으로 간주하고,
	아무런 동작을 하지 않는다.
	아래 setting을 다음과 같이하면, 재부팅 후, Flash 데이터를 동작시키고, Remove함으로써, 정상동작한다.
*/
HERROR		xmgr_cas_CxSetUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
	return	CXAPI_UiUserTextDisplayed(ucSeqNo,ulOpt);
}
HERROR		xmgr_cas_CxRemoveUserText(HUINT8 ucSeqNo)
{
	return	CXAPI_NVMRemoveUserText(ucSeqNo);
}

#endif	// End of defined(CONFIG_MW_CAS_CONAX_TEXT)


#define	_______Debug_Command____________________________________
/* Debug Command msg Process */
void	xmgr_cas_CxCmdProcessing(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2)
{
#if defined(CONFIG_DEBUG)
    unsigned long   arg1 = 0, arg2 = 0;
	CxUiMsgType_t	eMsg;
	STATIC CxMaturityRatingValue_t 	stCxUiMrValue;
	STATIC CxOrderInfo_t 			stCxUiOrder;
	STATIC CxAcceptViewingInfo_t	stCxUiAcceptView;
	STATIC CxTokenDebitInfo_t		stCxUiTokenDebit;
	STATIC CxFingerprint_t			stCxUiFingerPrint;
	STATIC CxUserText_t				stCxUiUserText;

	//XMGR_CAS_CX_PARAM_UNUSED(pszmsg1);
	//XMGR_CAS_CX_PARAM_UNUSED(pszmsg2);

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"sc") == 0)
	{
		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"nocard") == 0)
		{
			eMsg = eCxUi_GroupSc_NoCard;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"incorrect") == 0)
		{
			eMsg = eCxUi_GroupSc_Incorrect;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"incorrectcx") == 0)
		{
			eMsg = eCxUi_GroupSc_IncorrectConax;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"fail") == 0)
		{
			eMsg = eCxUi_GroupSc_CommFail;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"allnone") == 0)
		{
			eMsg = eCxUi_GroupAll_None;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"insert") == 0)
		{
			eMsg = eCxUi_GroupSc_Inserted;
		}
		else
		{
			eMsg = eCAS_MSG_TYPE_None;
		}

		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, 0);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"access") == 0)
	{
		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"noaccess") == 0)
		{
			eMsg = eCxUi_GroupBasic_NoAccess;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"noaccess_gb") == 0)
		{
			eMsg = eCxUi_GroupBasic_NoAccessBlackOut;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"noaccess_nb") == 0)
		{
			eMsg = eCxUi_GroupBasic_NoAccessNetwork;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"noaccess_cpm") == 0)
		{
			eMsg = eCxUi_GroupBasic_NoAccessPairing;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"mrask") == 0)
		{
			if ((pszmsg1 == NULL) || (pszmsg2 == NULL))
			{
				HxLOG_Print("Check cmd : cas_cx access mrask [Program Level] [Smart Card Level]\n");
				return;
			}

	        arg1 = atoi(pszmsg1);
	        arg2 = atoi(pszmsg2);

			eMsg = eCxUi_GroupMr_Ask;
			stCxUiMrValue.ucProgram = (HUINT8)arg1;
			stCxUiMrValue.ucSmartcard = (HUINT8)arg2;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"mrwrong") == 0)
		{
			eMsg = eCxUi_GroupMr_Ask;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"mrpinlock") == 0)
		{
			eMsg = eCxUi_GroupMr_PinLocked;
		}
		else
		{
			eMsg = eCAS_MSG_TYPE_None;
		}

		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiMrValue);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"order") == 0)
	{
		eMsg = eCxUi_GroupPpv_OrderInfo;

		// Test Code
		stCxUiOrder.ulProdRef = 15896;
		MWC_UTIL_DvbStrcpy(stCxUiOrder.szLabel, (HUINT8*)"15896-NVOD 1");
		MWC_UTIL_DvbStrcpy(stCxUiOrder.stPrice.aucCurrency, (HUINT8*)"EUR");
		stCxUiOrder.stPrice.usPriceUnit = 3;
		stCxUiOrder.stPrice.ucPriceDecimal = 0;
		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiOrder);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"accept") == 0)
	{
		eMsg = eCxUi_GroupPpv_AcceptViewing;

		stCxUiAcceptView.ulProdRef = 15896;
		stCxUiAcceptView.usMinuteLeft = 141;
		MWC_UTIL_DvbStrcpy(stCxUiAcceptView.szLabel, (HUINT8*)"15896-NVOD 1");
		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiAcceptView);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"token") == 0)
	{
		eMsg = eCxUi_GroupPpv_TokenPpv;

		stCxUiTokenDebit.usPurseRef = 15896;
		stCxUiTokenDebit.ulEventTag = 1;			// 24 bits

		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"i0") == 0)
		{
			stCxUiTokenDebit.ucInsufficientTokens = CX_TOKENS_INSUFFICIENT_ENABLE;
		}
		else
		{
			stCxUiTokenDebit.ucInsufficientTokens = CX_TOKENS_INSUFFICIENT_UNABLE;
		}

		if (MWC_UTIL_DvbStrcmp(pszmsg1, (HUINT8*)"e0") == 0)
		{
			stCxUiTokenDebit.ucTokenMode = CX_TOKENS_MODE_EVENT;
			stCxUiTokenDebit.ulEventCost = 100;			// 24 bits
		}
		else
		{
			stCxUiTokenDebit.ucTokenMode = CX_TOKENS_MODE_TIME;
			stCxUiTokenDebit.ulEventCost = 5;			// 24 bits
		}

		if (pszmsg2 != NULL)
		{
	        arg2 = atoi(pszmsg2);
			stCxUiTokenDebit.ulEventCost = arg2;
		}

		MWC_UTIL_DvbStrcpy(stCxUiTokenDebit.szLabel, (HUINT8*)"Best of Pink Floyd");
		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiTokenDebit);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"fp") == 0)
	{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

		{
			UNIXTIME		ulCurrentTime = 0, ulFPTime = 0;
			HxDATETIME_t	stCurrentDateTime;


			VK_CLOCK_GetTime(&ulCurrentTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stCurrentDateTime);
			HxSTD_memset(&stCurrentDateTime.stDate, 0x0, sizeof(HxDATETIME_Date_t));
			stCurrentDateTime.stTime.ucMinute += 1;
			HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&stCurrentDateTime, &ulFPTime);

			eMsg = eCxUi_GroupOther_Fingerprint;

			if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"t") == 0)
			{
				stCxUiFingerPrint.ulTime = ulFPTime;	// After 1Minute
			}else
			{
				stCxUiFingerPrint.ulTime = 0xffffff;	// 0xffffff = immediately
			}
			stCxUiFingerPrint.usDuration = 0x0bbb;
			stCxUiFingerPrint.usPosX = 0x0014;
			stCxUiFingerPrint.usPosY = 0x0014;
			stCxUiFingerPrint.usHeight = 0x0010;
			MWC_UTIL_DvbStrcpy(stCxUiFingerPrint.szText, (HUINT8*)"TEST Finger Print");
			stCxUiFingerPrint.ucDisplayed = FALSE;

			CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiFingerPrint);
		}
#endif
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ut") == 0)
	{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		HUINT32			ulCurrentTime;
		HxDATETIME_t	stUserTextDateTime;

		eMsg = eCxUi_GroupMessage_Display;

		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"t") == 0)
		{
			VK_CLOCK_GetTime(&ulCurrentTime);
			ulCurrentTime += SECONDS_PER_MIN;
			HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stUserTextDateTime);
			HxSTD_memcpy(&stCxUiUserText.stStartDataTime, &stUserTextDateTime, sizeof(HxDATETIME_t));
		}
		else if(MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"i") == 0)
		{
			HxSTD_memset(&stUserTextDateTime, 0, sizeof(HxDATETIME_t));
			HxSTD_memcpy(&stCxUiUserText.stStartDataTime, &stUserTextDateTime, sizeof(HxDATETIME_t));
		}
		else
		{
			HxLOG_Print("Check cmd : cas_cx ut [Disp Mode(t(time), i(Immediately)] [Disp Type :m(Mail) ]\n");
			return;
		}

		// Example
		stCxUiUserText.ucSeqNo = 0x03;
		if (MWC_UTIL_DvbStrcmp(pszmsg1, (HUINT8*)"m") == 0)
		{
			stCxUiUserText.usDuration = 0x0;
		}
		else
		{
			stCxUiUserText.usDuration = 0x000a;	// 10 Seconds
		}
		stCxUiUserText.ucRepeat = 0x0a;		// 10 times
		stCxUiUserText.usInterval = 0x06;	// 1 Minute
		stCxUiUserText.ucDisplayed = FALSE;
		MWC_UTIL_DvbStrcpy(stCxUiUserText.szText, (HUINT8*)"Please update your subscription!");

		CXUI_Display_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsg, (HUINT32)&stCxUiUserText);
#endif
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"comm") == 0)
	{
		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"recovery") == 0)		// send recovery reset...
		{
			CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, 0, (HUINT32)0);
		}
		else if(MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"rev_cass") == 0)	// emulate receive cass version...
		{
			CXSC_DEBUG_ToggleReceiveCassVer();
		}
		else if(MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"cass_ver") == 0)	// emulate cass version...
		{
			CXSC_DEBUG_ToggleCassVersion();
		}
		else if(MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"ca_sysid") == 0)		// emulate ca sys id...
		{
			CXSC_DEBUG_ToggleCaSysId();
		}
		else if(MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"sw") == 0)			// emulate status word...
		{
			HUINT8 ucSW1, ucSW2;

			sscanf(pszmsg1, "%x", &ucSW1);
			sscanf(pszmsg2, "%x", &ucSW2);

			CXSC_Debug_SetStatusWord(ucSW1, ucSW2);
		}
	}
	else
	{
		HxLOG_Print("don't recognized[%s] message! \n", pszCmd);
	}
#endif	// End of defined(CONFIG_DEBUG)
}


// End of file

