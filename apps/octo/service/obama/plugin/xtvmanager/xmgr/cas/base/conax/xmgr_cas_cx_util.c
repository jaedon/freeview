/**
	@file     ap_cas_cx_uitl.c
	@brief    file_name & simple comment.

	Description: CONAX API  \n
	Module: Application(Layer), AP_CORE(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <hapi.h>
#include <papi.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>

#include "local_include/_xmgr_cas_cx_util.h"
//#include "local_include/_xmgr_cas_cx_adapt.h"
#include <xmgr_cas_res_str.h>

#include <conax.h>
// #include <cx_common.h>
#include "./local_include/_xmgr_cas_cx.h"

#include <conax_ctrl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ApCasCxUTAssert(c) 					((c) ? 0 : (HxLOG_Print("\==[AP:CAS_CX_UTIL]== assertion on %s: line%d\n\n", __FILE__, __LINE__)))

/* Month String Country Index */
typedef enum
{
	eCountryCode_UK = 0,
	eCountryCode_SWE,
	eCountryCode_DEN,
	eCountryCode_NOR,
	eCountryCode_FIN,
	eCountryCode_ITA,
	eCountryCode_FRA,
	eCountryCode_SPA,
	eCountryCode_GER,
} eCountryCode_Index;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
char	*szMonthString[9][12] =
{
	{ "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC", },	// UK
	{ "JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC", },	// SWE
	{ "JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC", },	// DEN
	{ "JAN", "FEB", "MAR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DES", },	// NOR
	{ "TAM", "HEL", "MAA", "HUH", "TOU", "KES", "HEI", "ELO", "SYY", "LOK", "MAR", "JOU", },	// FIN
	{ "GEN", "FEB", "MAR", "APR", "MAG", "GIU", "LUG", "AGO", "SET", "OTT", "NOV", "DIC", },	// ITA
	{ "JAN", "F\311V", "MAR", "AVR", "MAI", "JUN", "JUL", "AO\332", "SEP", "OCT", "NOV", "D\311C", },	//	FRA
	{ "ENE", "FEB", "MAR", "ABR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DIC", },	//SPA
	{ "JAN", "FEB", "MÄR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEZ", },	//GER
};

/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
STATIC HUINT32	xmgr_cas_CxUtilPower10(int n);


/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

// Basic Func
STATIC HUINT32	xmgr_cas_CxUtilPower10(int n)
{
	int i;
	HUINT32		ulPoweredNum = 1;

	if (n < 1)
		return	ulPoweredNum;

	for (i=0; i< n;i++)
		ulPoweredNum *=10;

	return	ulPoweredNum;
}

// MMI - Menu


#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
HBOOL		xmgr_cas_CxGetPowerSavingStandby(void)
{
	HBOOL bPowerSavingStandby = FALSE;

#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	bPowerSavingStandby = FALSE;
#else
	PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby);
#endif

	return bPowerSavingStandby;
}

HBOOL		xmgr_cas_CxGetRunningInStandby(void)
{
	HINT32 nStandby = 0;

	PAPI_GetLastStandby(&nStandby);

	return nStandby ? TRUE : FALSE;
}

// Basic Func

HUINT32		xmgr_cas_CxCalcDigit(HUINT8 ucNoDigit, HUINT32 numData)
{
	HUINT32 tmpInt = 0, tmp;
	int i;

	for (i=ucNoDigit;i>0;i--)
	{
		tmp = numData/xmgr_cas_CxUtilPower10(i-1)-(numData/xmgr_cas_CxUtilPower10(i))*10;
		if ((i%2) == 1)
			tmp *= 2;
		if (tmp > 9)
			tmp = (tmp/10) + (tmp%10);

		tmpInt += tmp;
	}

	return tmpInt;
}


void	xmgr_cas_CxGetWordFromString(HUINT8 *pszDest, HUINT8 *pszSrc, HUINT8 nIndex)
{
	HINT32		i, n, j;

	n = 0;
	for (i=0; i < (int)SvcCas_UtilStrLen(pszSrc) && n != nIndex; i++)
	{
		if (pszSrc[i] == '|')
			n++;
	}

	/* copy to output buffer */
	for (j = i; j < (int)SvcCas_UtilStrLen(pszSrc) && pszSrc[j] != '|'; j++)
		pszDest[j-i] = pszSrc[j];

	pszDest[j-i] = '\0';
}


HERROR 	xmgr_cas_CxGetMonthString(HUINT8 ucMonth, HUINT8 *szMonthString64)
{
	HERROR			hError = ERR_FAIL;
	HUINT8			*pucResult = NULL, *pStr = NULL;
	HUINT8			ucStrIdx = 0;
	HINT32			lCountryIdx = 0;

	if((ucMonth == 0) || (ucMonth > 12) || (szMonthString64 == NULL))
	{
		return	hError;
	}

	xmgr_cas_CxGetCountryIndicator(CX_SC_SLOT_0, &lCountryIdx);
	switch (lCountryIdx)
	{
		case	eITU_T_CountryCode_UK:		ucStrIdx = eCountryCode_UK;		break;
		case	eITU_T_CountryCode_SWE:		ucStrIdx = eCountryCode_SWE;	break;
		case	eITU_T_CountryCode_DEN:		ucStrIdx = eCountryCode_DEN;	break;
		case	eITU_T_CountryCode_NOR:		ucStrIdx = eCountryCode_NOR;	break;
		case	eITU_T_CountryCode_FIN:		ucStrIdx = eCountryCode_FIN;	break;
		case	eITU_T_CountryCode_ITA:		ucStrIdx = eCountryCode_ITA;	break;
		case	eITU_T_CountryCode_FRA:		ucStrIdx = eCountryCode_FRA;	break;
		case	eITU_T_CountryCode_SPA:		ucStrIdx = eCountryCode_SPA;	break;
		case	eITU_T_CountryCode_GER:		ucStrIdx = eCountryCode_GER;	break;
		default	: // UK
			ucStrIdx = eCountryCode_UK;
			break;
	}

	HxSTD_memset(szMonthString64, 0x00, sizeof(HUINT8)*TEXT_SIZE64);

	pStr = (HUINT8 *)szMonthString[ucStrIdx][ucMonth - 1];
	pucResult = MWC_UTIL_DvbStrncpy(szMonthString64, pStr, SvcCas_UtilStrLen(pStr));
	if (pucResult != NULL)
	{
		hError = ERR_OK;
	}
	else
	{
		hError = ERR_FAIL;
	}

	return hError;
}

/*
HERROR		xmgr_cas_CxGetMatutiryRateStr(HUINT8 *pszStr, HUINT8 ucCurMatRatLevel)
{
	switch (ucCurMatRatLevel)
	{
		case CX_MATURITY_RATE_G:
			sprintf(pszStr, (HUINT8*)"%s - %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_G_ID),
							(HUINT8*)AP_CasResStr_GetStrRsc(STR_GENERAL_AUDIENCE_ID));
			break;

		case CX_MATURITY_RATE_PG:
			sprintf(pszStr, (HUINT8*)"%s - %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_PG_ID),
							(HUINT8*)AP_CasResStr_GetStrRsc(STR_PARENTAL_GUIDANCE_ID));
			break;

		case CX_MATURITY_RATE_A:
			sprintf(pszStr, (HUINT8*)"%s - %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_A_ID),
							(HUINT8*)AP_CasResStr_GetStrRsc(STR_OVER_18_ID));
			break;

		case CX_MATURITY_RATE_X:
			sprintf(pszStr, (HUINT8*)"%s - %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_X_ID),
							(HUINT8*)AP_CasResStr_GetStrRsc(STR_EROTIC_ID));
			break;

		default:
			HxLOG_Critical("\n\n");
			return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR		xmgr_cas_CxGetMatutiryRateLevelStr(HUINT8 *pszStr, HUINT8 ucCurMatRatLevel)
{
	switch (ucCurMatRatLevel)
	{
		case CX_MATURITY_RATE_G:
			sprintf(pszStr, (HUINT8*)"%s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_G_ID));
			break;

		case CX_MATURITY_RATE_PG:
			sprintf(pszStr, (HUINT8*)"%s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_PG_ID));
			break;

		case CX_MATURITY_RATE_A:
			sprintf(pszStr, (HUINT8*)"%s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_A_ID));
			break;

		case CX_MATURITY_RATE_X:
			sprintf(pszStr, (HUINT8*)"%s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_X_ID));
			break;

		default:
			HxLOG_Critical("\n\n");
			return	ERR_FAIL;
	}

	return	ERR_OK;
}
*/

// MMI - MENU
HERROR		xmgr_cas_CxGetVersionInfo(HINT32 lScSlot, CxVersionInfo_t *pstInfo)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_GetVersionInfoInParam_t				stInParam;
	CX_GetVersionInfoOutParam_t				stOutParam;

	stParam.ulControlType = eCACTRL_CX_GetVersionInfo;
	stInParam.lScSlot = lScSlot;
	stOutParam.pstInfo = pstInfo;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, eCACTRL_CX_GetVersionInfo, &stParam);

	return	hErr;
}


HERROR		xmgr_cas_CxGetCurMatRatLevel(HINT32 lScSlot, HUINT8 *pucCurMatRatLevel)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_GetCurMatRatLevelInParam_t			stInParam;
	CX_GetCurMatRatLevelOutParam_t			stOutParam;

	stParam.ulControlType = eCACTRL_CX_GetCurMatRatLevel;
	stInParam.lScSlot = lScSlot;
	stOutParam.pucCurMatRatLevel = pucCurMatRatLevel;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, eCACTRL_CX_GetCurMatRatLevel, &stParam);

	return	hErr;
}


HERROR		xmgr_cas_CxChangeMaturityRating(HINT32 lScSlot, HUINT8 ucMaturityRatLevel, HUINT8 *pucPin)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_ChangeMaturityRatingInParam_t		stInParam;

	stParam.ulControlType = eCACTRL_CX_ChangeMaturityRating;
	stInParam.lScSlot = lScSlot;
	stInParam.ucMaturityRatLevel = ucMaturityRatLevel;
	stInParam.pucPin = pucPin;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CX, eCACTRL_CX_ChangeMaturityRating, &stParam);

	return hErr;
}


CxStatusList_t *xmgr_cas_CxReqStatusInfo(HINT32 lScSlot, CxStatusSelectType_t eType, HUINT32 ulRef)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_GetStatusInfoInParam_t		stInParam;
	CX_GetStatusInfoOutParam_t		stOutParam;
	CxStatusList_t					*pstList = NULL;

	stParam.ulControlType = eCACTRL_CX_ReqStatusInfo;
	stInParam.lScSlot = lScSlot;
	stInParam.eType = eType;
	stInParam.ulRef = ulRef;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_CX, eCACTRL_CX_ReqStatusInfo, &stParam);
	pstList = stOutParam.pstList;

	(void)(hErr);

	return (CxStatusList_t*)pstList;
}


HERROR		xmgr_cas_CxGetUserText(HUINT32 *pulNumMsg, CxUserTextList_t *pstList)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_GetUserTextOutParam_t				stOutParam;

	stParam.ulControlType = eCACTRL_CX_GetUserText;
	stOutParam.pulNumMsg = pulNumMsg;
	stOutParam.pstList = pstList;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, eCACTRL_CX_GetUserText, &stParam);

	return	hErr;
}


HERROR		xmgr_cas_CxGetCountryIndicator(HINT32 lScSlot, HINT32 *lCountryIdx)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CX_GetCountryIndicatorInParam_t				stInParam;
	CX_GetCountryIndicatorOutParam_t			stOutParam;

	stParam.ulControlType = eCACTRL_CX_GetCountryIndicator;
	stInParam.lScSlot = lScSlot;
	stOutParam.lCountryIdx = lCountryIdx;

	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, eCACTRL_CX_GetCountryIndicator, &stParam);

	return	hErr;
}


HERROR		xmgr_cas_CxGetSCState(HINT32 lScSlot)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	CX_GetSCStateInParam_t			stInParam;

	stParam.ulControlType = eCACTRL_CX_GetSCState;
	stInParam.lScSlot = lScSlot;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, eCACTRL_CX_GetSCState, &stParam);

	return	hErr;
}

HERROR		xmgr_cas_CxOperateCachedPin(HUINT8* pucInputPin)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	CX_SetScPinInParam_t		stInParam;

	stInParam.pucPin = pucInputPin;
	stParam.pvIn = (void *)&stInParam;

	stParam.ulControlType = eCACTRL_CX_OperateCachedPin;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_OperateCachedPin, &stParam);

	return	hErr;
}

HERROR	xmgr_cas_CxSetUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
	HERROR								hErr = ERR_OK;
	SvcCas_CtrlParam_t					stParam;
	CX_SetUserTextDisplayedInParam_t	stInParam;

	stParam.ulControlType = eCACTRL_CX_SetUserTextDisplayed;
	stInParam.ucSeqNo = ucSeqNo;
	stInParam.ulOpt = ulOpt;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetUserTextDisplayed, &stParam);

	return	hErr;
}

HERROR	xmgr_cas_CxSetFingerPrintDisplayed(void)
{
	HERROR								hErr = ERR_OK;
	SvcCas_CtrlParam_t					stParam;

	stParam.ulControlType = eCACTRL_CX_SetFingerPrintDisplayed;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetFingerPrintDisplayed, &stParam);

	return	hErr;
}

HERROR	xmgr_cas_CxGetScResetReason(HINT32 nScSlot, CxScReset_Reason_t *peResetReason)
{
	//HERROR								hErr = ERR_OK;
	SvcCas_CtrlParam_t					stParam;
	CX_GetScResetReasonInParam_t		stInParam;
	CX_GetScResetReasonOutParam_t		stOutParam;

	if(peResetReason == NULL)
	{
		return ERR_FAIL;
	}

	stInParam.nScSlot = nScSlot;

	stParam.ulControlType = eCACTRL_CX_GetScResetReason;
	stParam.pvIn = &stInParam;
	stParam.pvOut = &stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(GetScResetReason) error\n");
		return ERR_FAIL;
	}

	*peResetReason = stOutParam.eResetReason;

	return	ERR_OK;
}

/* Command msg Process */
void	xmgr_cas_CxCMD(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2)
{
//	xmgr_cas_CxCmdProcessing(pszCmd, pszMsgType, pszmsg1, pszmsg2);
}

#if defined(CONFIG_DEBUG)
HUINT8 *xmgr_cas_CxGetMsgTypeStr(CxUiMsgType_t eMsg)
{
	HUINT8 *pszName;

	switch (eMsg)
	{
	case eCxUi_GroupAll_None:						pszName = (HUINT8*)"eCxUi_GroupAll_None";				break;
	case eCxUi_GroupBasic_NoAccess:					pszName = (HUINT8*)"eCxUi_GroupBasic_NoAccess";			break;
	case eCxUi_GroupBasic_NoAccessBlackOut:			pszName = (HUINT8*)"eCxUi_GroupBasic_NoAccessBlackOut";	break;
	case eCxUi_GroupBasic_NoAccessNetwork:			pszName = (HUINT8*)"eCxUi_GroupBasic_NoAccessNetwork";	break;
	case eCxUi_GroupMessage_Display:				pszName = (HUINT8*)"eCxUi_GroupMessage_Display";		break;
	case eCxUi_GroupPpv_OrderInfo:					pszName = (HUINT8*)"eCxUi_GroupPpv_OrderInfo";			break;
	case eCxUi_GroupPpv_OrderConnect:				pszName = (HUINT8*)"eCxUi_GroupPpv_OrderConnect";		break;
	case eCxUi_GroupPpv_OrderFailed:				pszName = (HUINT8*)"eCxUi_GroupPpv_OrderFailed";		break;
	case eCxUi_GroupPpv_OrderAccepted:				pszName = (HUINT8*)"eCxUi_GroupPpv_OrderAccepted";		break;
	case eCxUi_GroupPpv_OrderNotAccepted:			pszName = (HUINT8*)"eCxUi_GroupPpv_OrderNotAccepted";	break;
	case eCxUi_GroupPpv_OrderClosed:				pszName = (HUINT8*)"eCxUi_GroupPpv_OrderClosed";		break;
	case eCxUi_GroupPpv_AcceptViewing:				pszName = (HUINT8*)"eCxUi_GroupPpv_AcceptViewing";		break;
	case eCxUi_GroupPpv_TokenPpv:					pszName = (HUINT8*)"eCxUi_GroupPpv_TokenPpv";			break;
	case eCxUi_GroupMr_Ask:							pszName = (HUINT8*)"eCxUi_GroupMr_Ask";					break;
	case eCxUi_GroupMr_WrongPin:					pszName = (HUINT8*)"eCxUi_GroupMr_WrongPin";			break;
	case eCxUi_GroupMr_PinLocked:					pszName = (HUINT8*)"eCxUi_GroupMr_PinLocked";			break;
	case eCxUi_GroupMenu_Subscription:				pszName = (HUINT8*)"eCxUi_GroupMenu_Subscription";		break;
	case eCxUi_GroupMenu_Event:						pszName = (HUINT8*)"eCxUi_GroupMenu_Event";				break;
	case eCxUi_GroupMenu_Purse:						pszName = (HUINT8*)"eCxUi_GroupMenu_Purse";				break;
	case eCxUi_GroupMenu_Debit:						pszName = (HUINT8*)"eCxUi_GroupMenu_Debit";				break;
	case eCxUi_GroupMenu_Credit:					pszName = (HUINT8*)"eCxUi_GroupMenu_Credit";			break;
	case eCxUi_GroupMenu_ChangePin:					pszName = (HUINT8*)"eCxUi_GroupMenu_ChangePin";			break;
	case eCxUi_GroupMenu_CheckNewPin:				pszName = (HUINT8*)"eCxUi_GroupMenu_CheckNewPin";		break;
	case eCxUi_GroupMenu_WrongPin:					pszName = (HUINT8*)"eCxUi_GroupMenu_WrongPin";			break;
	case eCxUi_GroupMenu_PinLocked:					pszName = (HUINT8*)"eCxUi_GroupMenu_PinLocked";			break;
	case eCxUi_GroupMenu_Mr:						pszName = (HUINT8*)"eCxUi_GroupMenu_Mr";				break;
	case eCxUi_GroupMenu_Modem:						pszName = (HUINT8*)"eCxUi_GroupMenu_Modem";				break;
	case eCxUi_GroupMenu_Message:					pszName = (HUINT8*)"eCxUi_GroupMenu_Message";			break;
	case eCxUi_GroupMenu_Info:						pszName = (HUINT8*)"eCxUi_GroupMenu_Info";				break;
	case eCxUi_GroupSc_NoCard:						pszName = (HUINT8*)"eCxUi_GroupSc_NoCard";				break;
	case eCxUi_GroupSc_Inserted:					pszName = (HUINT8*)"eCxUi_GroupSc_Inserted";			break;
	case eCxUi_GroupSc_Incorrect:					pszName = (HUINT8*)"eCxUi_GroupSc_Incorrect";			break;
	case eCxUi_GroupSc_IncorrectConax:				pszName = (HUINT8*)"eCxUi_GroupSc_IncorrectConax";		break;
	case eCxUi_GroupSc_CommFail:					pszName = (HUINT8*)"eCxUi_GroupSc_CommFail";			break;
	case eCxUi_GroupOther_Fingerprint:				pszName = (HUINT8*)"eCxUi_GroupOther_Fingerprint";		break;
//	case eCxUi_GroupOther_MessageReceived:			pszName = (HUINT8*)"eCxUi_GroupOther_MessageReceived";	break;
	default:										pszName = (HUINT8*)"UNKNOWN";							break;
	}

	return pszName;
}
#endif

// End of file

