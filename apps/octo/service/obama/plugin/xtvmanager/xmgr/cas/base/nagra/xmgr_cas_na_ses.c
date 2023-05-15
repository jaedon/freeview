/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include <mgr_action.h>
#include <xmgr_cas.h>
#include <svc_pipe.h>
#include <_svc_cas_mgr_nvram.h>
#include <cas_ses_mgr.h>

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

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define		APSHDPLUS_DEF_MAGICCODE		"HUMA"
#define		APSHDPLUS_DEF_HOTLINE		"0900-1904599"
#define		APSHDPLUS_DEF_CALLCHARGE	"0,49"

#define		APSHDPLUS_CHECK_TIMER				500
#define		APSHDPLUS_CHECK_TIMEOUT				(15 * 1000)	// 15 seconds
#define		APSHDPLUS_EEPROM_FILESIZE			2048

#define		TEXT_SIZE64			64
#define		TEXT_SIZE512		512

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC NaCasApsValidCheck_t		s_cas_na_aps_ValidContext;
STATIC HUINT8 					s_aucCasMsgTmp[TEXT_SIZE512];
STATIC HxDATETIME_Date_t					s_TimeToTodayCheck;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_APS_LOCAL_FUNCTION______________________________________________________

STATIC HERROR				xmgr_cas_NaDefaultSettingIndexDisplayProfile(void)
{
	HERROR					hReturnError = ERR_OK;
	NA_IRDCMD_IDX_PROFILE_t	stCurrentProfile[2];

	// to clear current profile data
	HxSTD_memset(&stCurrentProfile, 0, sizeof(NA_IRDCMD_IDX_PROFILE_t)*2);

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET,
											sizeof(NA_IRDCMD_IDX_PROFILE_t) * 2,
											(HUINT8 *)&stCurrentProfile);
	if (hReturnError != ERR_OK)
	{
		HxLOG_Print("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

STATIC HERROR				xmgr_cas_NaDefaultSettingDisplayProfile(void)
{
	HINT8					nIdx = 0, mIdx = 0;
	HERROR					hReturnError = ERR_OK;
	NA_IRDCMD_PROFILE_t 	stDisplayProfile[2];
	NA_IRDCMD_PROFILE_t 	*pstDisplayProfile;
	HUINT8					arrDefaultDisplayProfile[5][3] = {	{30,	15,		0},
																	{14,	8,		0},
																	{7,		4,		0},
																	{3,		3,		1},
																	{2,		0,		1}	};

	HxSTD_memset(stDisplayProfile, 0, sizeof(stDisplayProfile));

	// initialize Display Profile
	HxLOG_Print("initialize eProfileSmartcard\n");

	pstDisplayProfile = &(stDisplayProfile[0]);

	pstDisplayProfile->ucProfieType = eProfileSmartcard;
	for (nIdx = 0; nIdx < NA_IRDCMD_MAX_PROFILE; nIdx++)
	{
		pstDisplayProfile->ucPeroid[nIdx] 			= nIdx + 1;
		pstDisplayProfile->ucBeginOfPeroid[nIdx]	= arrDefaultDisplayProfile[nIdx][0];
		pstDisplayProfile->ucEndOfPeroid[nIdx]		= arrDefaultDisplayProfile[nIdx][1];
		pstDisplayProfile->ucFrequency[nIdx]		= arrDefaultDisplayProfile[nIdx][2];
	}

	// initialize Display Profile
	HxLOG_Print("initialize eProfileProducts\n");

	pstDisplayProfile = &(stDisplayProfile[1]);

	pstDisplayProfile->ucProfieType = eProfileProducts;
	for (nIdx = 0; nIdx < (NA_IRDCMD_MAX_PROFILE - 1); nIdx++)
	{
		mIdx = nIdx + 1;
		pstDisplayProfile->ucPeroid[nIdx] 			= nIdx + 1;
		pstDisplayProfile->ucBeginOfPeroid[nIdx]	= arrDefaultDisplayProfile[mIdx][0];
		pstDisplayProfile->ucEndOfPeroid[nIdx]		= arrDefaultDisplayProfile[mIdx][1];
		pstDisplayProfile->ucFrequency[nIdx]		= arrDefaultDisplayProfile[mIdx][2];
	}

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_PROFILE_OFFESET,
											sizeof(NA_IRDCMD_PROFILE_t) * 2,
											(HUINT8 *)&stDisplayProfile);
	if (hReturnError != ERR_OK)
	{
		HxLOG_Print("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

STATIC HERROR				xmgr_cas_NaDefaultSettingMessageVariable(void)
{
	HERROR					hReturnError = ERR_OK;
	NA_IRDCMD_MESSAGE_t 	stIrdMessage[2];

	HxSTD_memset(stIrdMessage, 0, sizeof(stIrdMessage));

	// initialize eMsgHotlineNum
	HxLOG_Print("initialize eMsgHotlineNum\n");

	stIrdMessage[0].ucMsgType = eMsgHotlineNum;
	MWC_UTIL_DvbStrncpy(stIrdMessage[0].ucMsgContent, APSHDPLUS_DEF_HOTLINE, NA_UI_IRDCMD_MAX_MSG_LEN - 1);


	// initialize eMsgCallCharge
	HxLOG_Print("initialize eMsgCallCharge\n");

	stIrdMessage[1].ucMsgType = eMsgCallCharge;
	MWC_UTIL_DvbStrncpy(stIrdMessage[1].ucMsgContent, APSHDPLUS_DEF_CALLCHARGE, NA_UI_IRDCMD_MAX_MSG_LEN - 1);

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MESSAGE_OFFSET,
											sizeof(NA_IRDCMD_MESSAGE_t) * 2,
											(HUINT8 *)&stIrdMessage);

	if (hReturnError != ERR_OK)
	{
		HxLOG_Print("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

#ifdef COMMAND_NAGRA_APS_TEST
STATIC HERROR				xmgr_cas_NaTestSettingMessageVariable(HUINT8 *msg1, HUINT8* msg2)
{
	HERROR					hReturnError = ERR_OK;
	NA_IRDCMD_MESSAGE_t 	stIrdMessage[2];

	HxSTD_memset(stIrdMessage, 0, sizeof(stIrdMessage));

	// initialize eMsgHotlineNum
	HxLOG_Print("initialize eMsgHotlineNum\n");

	stIrdMessage[0].ucMsgType = eMsgHotlineNum;
	MWC_UTIL_DvbStrncpy(stIrdMessage[0].ucMsgContent, msg1, NA_UI_IRDCMD_MAX_MSG_LEN - 1);


	// initialize eMsgCallCharge
	HxLOG_Print("initialize eMsgCallCharge\n");

	stIrdMessage[1].ucMsgType = eMsgCallCharge;
	MWC_UTIL_DvbStrncpy(stIrdMessage[1].ucMsgContent, msg2, NA_UI_IRDCMD_MAX_MSG_LEN - 1);

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MESSAGE_OFFSET,
											sizeof(NA_IRDCMD_MESSAGE_t) * 2,
											(HUINT8 *)&stIrdMessage);

	if (hReturnError != ERR_OK)
	{
		HxLOG_Print("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}
#endif

STATIC HERROR				xmgr_cas_NaCheckIrdCommand(void)
{
	HERROR					hReturnError = ERR_OK;
	HUINT8					szMagicCode[CA_NVRAM_IRDCMD_MAGICCODE_SIZE+1];

	HxSTD_memset(szMagicCode, 0x00, CA_NVRAM_IRDCMD_MAGICCODE_SIZE+1);

	hReturnError = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAGICCODE_OFFESET,
											CA_NVRAM_IRDCMD_MAGICCODE_SIZE, (HUINT8 *)&szMagicCode);
	if (hReturnError != ERR_OK)
	{
		HxLOG_Error("MagicCode read Error from NVRAM[%x]\n", hReturnError);
		return hReturnError;
	}
	HxLOG_Error("Getting MagicCode : [%s]\n", szMagicCode);

	if (MWC_UTIL_DvbStrncmp(szMagicCode, APSHDPLUS_DEF_MAGICCODE, CA_NVRAM_IRDCMD_MAGICCODE_SIZE) != 0)
	{
		HxLOG_Print("APS HD IrdCommand Initialize Process .....\n");

		// Setting Index Display Profile
		if (xmgr_cas_NaDefaultSettingIndexDisplayProfile() != ERR_OK)
		{
			return ERR_FAIL;
		}

		// Setting Display Profile from default data
		if (xmgr_cas_NaDefaultSettingDisplayProfile() != ERR_OK)
		{
			return ERR_FAIL;
		}

		// Setting Message Variable from default data
		if (xmgr_cas_NaDefaultSettingMessageVariable() != ERR_OK)
		{
			return ERR_FAIL;
		}

		// Setting Magic code
		HxLOG_Error("Setting MagicCode .....\n");
		hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAGICCODE_OFFESET,
												CA_NVRAM_IRDCMD_MAGICCODE_SIZE,
												(HUINT8 *)APSHDPLUS_DEF_MAGICCODE);
		if (hReturnError != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
			return hReturnError;
		}

	}

	return ERR_OK;
}

STATIC HERROR				xmgr_cas_NaGetIrdMessageVariable(HUINT8* paucHotline, HUINT8* paucCallCharge)
{
	HERROR					hReturnError = ERR_OK;
	NA_IRDCMD_MESSAGE_t 	stIrdMessage[2];

	HxSTD_memset(stIrdMessage, 0, sizeof(stIrdMessage));

	hReturnError = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MESSAGE_OFFSET,
											sizeof(NA_IRDCMD_MESSAGE_t) * 2, (HUINT8 *)&stIrdMessage);

	if (hReturnError != ERR_OK)
	{
		HxLOG_Print("Read IRD Message Variable read Error[%x]\n", hReturnError);
		return hReturnError;
	}

	if ((stIrdMessage[0].ucMsgType == eMsgHotlineNum) && (paucHotline != NULL))
	{
		MWC_UTIL_DvbStrncpy(paucHotline, stIrdMessage[0].ucMsgContent, NA_UI_IRDCMD_MAX_MSG_LEN - 1);
	}

	if ((stIrdMessage[1].ucMsgType == eMsgCallCharge) && (paucCallCharge != NULL))
	{
		MWC_UTIL_DvbStrncpy(paucCallCharge, stIrdMessage[1].ucMsgContent, NA_UI_IRDCMD_MAX_MSG_LEN - 1);
	}

	return ERR_OK;
}

HERROR					xmgr_cas_NaAPS_GetMessageTypeAndString(HUINT8 *pMessageType, HUINT8 *pszMessage)
{
	HxDATETIME_Date_t			*pExpiredDate;
	HUINT8 					aucDateTime[TEXT_SIZE64];
	HUINT8 					aucHotline[NA_UI_IRDCMD_MAX_MSG_LEN];
	HUINT8 					aucCallCharge[NA_UI_IRDCMD_MAX_MSG_LEN];
	eCAS_APS_VALID_MESSAGE	message_id;
	NaCasApsValidCheck_t	*pContext = NULL;
	TSystemInfoStatus		xStatus = CA_SYSTEM_INFO_ERROR;
	TIrdInfo				*pxIrdInfo = NULL;
	TUnsignedInt8			xNumberOfSmartcards = 0;
	TSmartcardInfoItem		*pxSmartcardInfoArray = NULL;
	TChar					SC_SerialNumber[64];

	pContext = &s_cas_na_aps_ValidContext;

	pExpiredDate = &(pContext->bExpiredDate.stDate);
	message_id = pContext->eCasApsValidMessage;

	HxSTD_memset(pszMessage, 0, TEXT_SIZE512);
	HxSTD_memset(aucHotline, 0, NA_UI_IRDCMD_MAX_MSG_LEN);
	HxSTD_memset(aucCallCharge, 0, NA_UI_IRDCMD_MAX_MSG_LEN);

	xmgr_cas_NaGetIrdMessageVariable(aucHotline, aucCallCharge);

	HxSTD_memset(SC_SerialNumber, 0, sizeof(HUINT8)*64);
	if(CA_NA_CakGetSystemInfo(&xStatus, &pxIrdInfo, &xNumberOfSmartcards, &pxSmartcardInfoArray) == ERR_OK)
	{
		MWC_UTIL_DvbStrncpy((HUINT8 *)SC_SerialNumber, (const HUINT8 *)pxSmartcardInfoArray[0].pSerialNumber, 64 -1);
	}

	HxLOG_Print("[%s:%d] message_id(0x%x), SC_SerialNumber(%s), aucHotline(%s), aucCallCharge(%s)\n", __FUNCTION__, __LINE__, message_id, SC_SerialNumber, aucHotline, aucCallCharge);

	switch(message_id)
	{
		case 	eCAS_APS_MSG_REF130A :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E30a_%d_%s_%s_%s", pContext->bRemainDay, SC_SerialNumber, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF130B :
			snprintf(aucDateTime, TEXT_SIZE64, "%02d-%02d-%04d", pExpiredDate->ucDay, pExpiredDate->ucMonth, pExpiredDate->usYear);
			snprintf(pszMessage, TEXT_SIZE512, "parse_E30b_%s_%s_%s_%s", aucDateTime, SC_SerialNumber, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF131 :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E31_%s_%s_%s", SC_SerialNumber, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF132A :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E32a_%d_%s_%s", pContext->bRemainDay, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF132B :
			snprintf(aucDateTime, TEXT_SIZE64, "%02d-%02d-%04d", pExpiredDate->ucDay, pExpiredDate->ucMonth, pExpiredDate->usYear);
			snprintf(pszMessage, TEXT_SIZE512, "parse_E32b_%s_%s_%s", aucDateTime, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF133 :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E33_%s_%s", aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF134 :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E34_%s_%s_%s", SC_SerialNumber, aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF135 :
			snprintf(pszMessage, TEXT_SIZE512, "parse_E35_%s_%s", aucHotline, aucCallCharge);
			break;
		case 	eCAS_APS_MSG_REF136 :
			snprintf(aucDateTime, TEXT_SIZE64, "%02d-%02d-%04d", pExpiredDate->ucDay, pExpiredDate->ucMonth, pExpiredDate->usYear);
			snprintf(pszMessage, TEXT_SIZE512, "parse_E36_%s_%s", SC_SerialNumber, aucDateTime);
			break;
		case 	eCAS_APS_MSG_REF140 :
			snprintf(pszMessage, TEXT_SIZE512, "%s", "parse_E26");
			break;
		default :
			return ERR_FAIL;
	}

	*pMessageType = (HUINT8)pContext->eCasApsValidType;

	return ERR_OK;
}

STATIC HERROR			xmgr_cas_Naaps_IrdCommandUpdated(NA_IRDCMD_OPCODE_t eOpcode)
{
	HxLOG_Print("op code:[%x] \n", eOpcode);

	switch (eOpcode)
	{
	case eOpCodeDisplayProfile :
		HxLOG_Print("update!\n");
		xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_PRV);
		break;

	case eOpCodeMessage :
		BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
		break;

#if defined(CONFIG_SUPPORT_NOVA_APP)
	case eOpCodeMultiscreen :
		//BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
		break;
#endif
	default :
		break;
	}

	return ERR_OK;
}

STATIC HERROR			xmgr_cas_Naaps_CheckMSG_Processing(Handle_t hAction, void *pMessage)
{
	NaCasApsCheck_t	*pstCasApsCheck = NULL;

	if (pMessage ==  NULL)
		return ERR_FAIL;

	pstCasApsCheck = (NaCasApsCheck_t*)pMessage;
	if(pstCasApsCheck != NULL)
	{
		HxSTD_memcpy(&s_cas_na_aps_ValidContext, &(pstCasApsCheck->context.stCasApsValidCheck), sizeof(NaCasApsValidCheck_t));
	}

	return ERR_OK;
}



#define _____SYSTEM_MESSAGE_RELATED_FUNCTION______________________________________________________

STATIC HUINT8			*xmgr_cas_NaGetSmartCardPairingMessage(HINT32 msgId)
{
	HUINT8 aucHotline[NA_UI_IRDCMD_MAX_MSG_LEN];
	HUINT8 aucCallCharge[NA_UI_IRDCMD_MAX_MSG_LEN];
	HUINT8 aucCardSer[NA_UI_SC_NUMBER_STRING_LEN];
	HUINT8 aucCaSer[NA_UI_CA_NUMBER_STRING_LEN];

	HxSTD_memset(s_aucCasMsgTmp, 0, sizeof(s_aucCasMsgTmp));
	HxSTD_memset(aucHotline, 0, sizeof(aucHotline));
	HxSTD_memset(aucCallCharge, 0, sizeof(aucCallCharge));
	HxSTD_memset(aucCardSer, 0, sizeof(aucCardSer));
	HxSTD_memset(aucCaSer, 0, sizeof(aucCaSer));

	xmgr_cas_NaUpdateSystemInfo();

	xmgr_cas_NaGetIrdMessageVariable(aucHotline, aucCallCharge);
	xmgr_cas_NaGetCardSerialNumber(aucCardSer);
	xmgr_cas_NaGetCANumber(aucCaSer);

	if(msgId == 1)
		snprintf(s_aucCasMsgTmp, TEXT_SIZE512, "parse_E17_%s_%s_%s_%s" /*GetStrRsc(msgId)*/, aucHotline, aucCallCharge, aucCardSer, aucCaSer);
	else if(msgId == 2)
		snprintf(s_aucCasMsgTmp, TEXT_SIZE512, "parse_E18_%s_%s_%s_%s" /*GetStrRsc(msgId)*/, aucHotline, aucCallCharge, aucCardSer, aucCaSer);

	return (HUINT8*)s_aucCasMsgTmp;
}

STATIC HUINT8			*xmgr_cas_NaGetSmartCardWithOPMessage(HINT32 msgId)
{
	HUINT8 aucHotline[NA_UI_IRDCMD_MAX_MSG_LEN];
	HUINT8 aucCallCharge[NA_UI_IRDCMD_MAX_MSG_LEN];

	HxSTD_memset(s_aucCasMsgTmp, 0, sizeof(s_aucCasMsgTmp));
	HxSTD_memset(aucHotline, 0, sizeof(aucHotline));
	HxSTD_memset(aucCallCharge, 0, sizeof(aucCallCharge));

	xmgr_cas_NaGetIrdMessageVariable(aucHotline, aucCallCharge);

	snprintf(s_aucCasMsgTmp, TEXT_SIZE512, "parse_E12_%s_%s"/*GetStrRsc(msgId)*/, aucHotline, aucCallCharge);
	return (HUINT8*)s_aucCasMsgTmp;
}

STATIC HUINT8			*xmgr_cas_NaGetAccessWithSmartCardNumber(HINT32 msgId)
{
	HUINT8 aucCardSer[NA_UI_SC_NUMBER_STRING_LEN];

	HxSTD_memset(s_aucCasMsgTmp, 0, sizeof(s_aucCasMsgTmp));
	HxSTD_memset(aucCardSer, 0, sizeof(aucCardSer));

	xmgr_cas_NaUpdateSystemInfo();

	xmgr_cas_NaGetCardSerialNumber(aucCardSer);

	snprintf(s_aucCasMsgTmp, TEXT_SIZE512, "parse_E15_%s" /*GetStrRsc(msgId)*/, aucCardSer);

	return (HUINT8*)s_aucCasMsgTmp;
}

STATIC HUINT8*	xmgr_cas_Naaps_GetMessage(XmgrCas_NaAlamType_e nAlarmType)
{
	HUINT8			*szErrCodeText;

	szErrCodeText = OxAP_Malloc (TEXT_SIZE512);
	if (szErrCodeText == NULL)
	{
		if (szErrCodeText != NULL)
			{ OxAP_Free (szErrCodeText); }
	}
	else
	{
		HxSTD_memset(szErrCodeText, 0x00, TEXT_SIZE512);
	}

	switch (nAlarmType)
	{
	// smartcard related msgid
	case eCAS_NA_CA_SMARTCARD_COM_ERROR :		//STR_MESG_4338_ID	E9
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E9");
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_BLACKLISTED: 		// STR_MESG_4341_ID	E12
		MWC_UTIL_DvbStrcpy(szErrCodeText, xmgr_cas_NaGetSmartCardWithOPMessage(1));
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED : 	// STR_MESG_4346_ID		E17
		MWC_UTIL_DvbStrcpy(szErrCodeText, xmgr_cas_NaGetSmartCardPairingMessage(1));
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_NOT_PAIRED :   	// STR_MESG_4347_ID		E18
		MWC_UTIL_DvbStrcpy(szErrCodeText, xmgr_cas_NaGetSmartCardPairingMessage(2));
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_MUTE :			// STR_MESG_4339_ID		E10A
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E10a");
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_REMOVED :			// STR_MESG_4359_ID		E106
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E20a");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED :				// STR_MESG_4344_ID			E15
		MWC_UTIL_DvbStrcpy(szErrCodeText, xmgr_cas_NaGetAccessWithSmartCardNumber(1));
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_INVALID :			// STR_MESG_4340_ID	E11
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E11");
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_SUSPENDED :		// STR_MESG_4342_ID	E12
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E13");
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_EXPIRED :			// STR_MESG_4343_ID	E13
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E14");
		return szErrCodeText;

	case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED :	// STR_MESG_4345_ID	E16
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E16");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_BLACKOUT :			//STR_ACCESS_BLACKOUT_ID	E101
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E101");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E44");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E102");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E103");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E104");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E105");
		return szErrCodeText;

	case eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E107");
		return szErrCodeText;

	default :
		MWC_UTIL_DvbStrcpy(szErrCodeText, (HUINT8 *)"parse_E0");
		return szErrCodeText;
	}

	return (HUINT8 *)NULL;
}

void			xmgr_cas_Naaps_Command(HUINT8 *pszCmd, HINT32 ulParam1, HINT32 ulParam2, HUINT8 *pszmsg)
{
	NaCasAPSCmdTest_t stCmdTest;

	if (MWC_UTIL_DvbStrcmp(pszCmd, "bdc") == 0)
	{
		stCmdTest.ucTestType = 0;

		stCmdTest.ucBdcType = ulParam1;
		stCmdTest.ucBdcCmd = ulParam2;
		if (pszmsg != NULL)
		{
			strncpy(stCmdTest.strBdcMessage, pszmsg, 128-1);
		}

		CASMGR_APS_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), &stCmdTest);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, "alarm") == 0)
	{
		stCmdTest.ucTestType = 1;

		stCmdTest.ucAlarmType = ulParam1;
		stCmdTest.ulAlarmMsg = ulParam2;

		CASMGR_APS_CMD_Test(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), &stCmdTest);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, "ses") == 0)
	{
		if (ulParam1 == 0)
		{
			xmgr_cas_NaDefaultSettingIndexDisplayProfile();
			xmgr_cas_NaDefaultSettingDisplayProfile();
			xmgr_cas_NaDefaultSettingMessageVariable();
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
			xmgr_cas_NaResetIRDCmdData();
#endif
		}
		else if (ulParam1 == 1)
		{
#ifdef COMMAND_NAGRA_APS_TEST
			xmgr_cas_NaTestSettingMessageVariable("010 6717 0412", "0, 12");
#endif
		}
		else if (ulParam1 == 2)
		{
			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);
		}
		else
		{
			HxLOG_Print("not support [%d] aps command! \n", ulParam1);
		}
	}
	else
	{
		HxLOG_Print("don't recognized[%s] message! \n", pszCmd);
	}
}




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_APS_PUBLIC_FUNCTION______________________________________________________

HUINT8*			xmgr_cas_NaGetMessage_Aps(HINT32 nAlarmType)
{
	return xmgr_cas_Naaps_GetMessage((XmgrCas_NaAlamType_e)nAlarmType);
}

HERROR			xmgr_cas_NaDefaultSetting_Aps(void)
{
	return xmgr_cas_NaDefaultSettingIndexDisplayProfile();
}

HERROR			xmgr_cas_NaCheckMsgProcessing_Aps(Handle_t hAction, void *pMessage)
{
	return xmgr_cas_Naaps_CheckMSG_Processing(hAction, pMessage);
}

HERROR			xmgr_cas_NaCheckIrdCommand_Aps(void)
{
	return xmgr_cas_NaCheckIrdCommand();
}

HERROR			xmgr_cas_NaIrdCommandUpdated_Aps(NA_IRDCMD_OPCODE_t eOpcode)
{
	return xmgr_cas_Naaps_IrdCommandUpdated(eOpcode);
}

HERROR			xmgr_cas_NaGetMessageTypeAndString_Aps(HUINT8 *pMessageType, HUINT8 *pszMessage)
{
	return xmgr_cas_NaAPS_GetMessageTypeAndString(pMessageType, pszMessage);
}

HERROR			xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APSUpdate type)
{
	return CASMGR_APS_UpdateCasFromAP((eCAS_APS_UPDATE_TYPE)type, MGR_ACTION_GetMainActionId());
}

void			xmgr_cas_NaCmd_Aps(HUINT8 *pszCmd, HINT32 ulParam1, HINT32 ulParam2, HUINT8 *pszmsg)
{
	xmgr_cas_Naaps_Command(pszCmd, ulParam1, ulParam2, pszmsg);
}

BUS_Result_t 	xproc_cas_NaManagement_Aps(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				err;
	HxDATETIME_Date_t			stDate_t;
	HULONG				ulUnixTime = 0;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxSTD_memset(&s_TimeToTodayCheck, 0x00, sizeof(HxDATETIME_Date_t));

			err = VK_CLOCK_GetTime(&ulUnixTime);
			if (err != ERR_OK)
			{
				BUS_MGR_Destroy(NULL);
				break;
			}

			HLIB_DATETIME_ConvertUnixTimeToDate(ulUnixTime, &s_TimeToTodayCheck);
			BUS_SetTimer(APSHDPLUS_CHECK_TIMER, APSHDPLUS_CHECK_TIMEOUT);

			HxLOG_Print("eMEVT_BUS_CREATE, [%d][%d][%d]\n", s_TimeToTodayCheck.usYear, s_TimeToTodayCheck.ucMonth, s_TimeToTodayCheck.ucDay);
			break;

		case eMEVT_BUS_TIMER :
			if (p1 == APSHDPLUS_CHECK_TIMER)
			{
				err = VK_CLOCK_GetTime(&ulUnixTime);
				if (err != ERR_OK)
				{
					break;
				}

				HxSTD_memset(&stDate_t, 0x00, sizeof(HxDATETIME_Date_t));
				HLIB_DATETIME_ConvertUnixTimeToDate(ulUnixTime, &stDate_t);

				if (HLIB_DATETIME_CompareDate(stDate_t, s_TimeToTodayCheck) != 0)
				{
					HxLOG_Print("eMEVT_BUS_TIMER, [%d][%d][%d]\n", stDate_t.usYear, stDate_t.ucMonth, stDate_t.ucDay);
					xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_PRV);

					HxSTD_memcpy(&s_TimeToTodayCheck, &stDate_t, sizeof(HxDATETIME_Date_t));
				}
				else
				{
					HxLOG_Print("eMEVT_BUS_TIMER, [%d][%d][%d] - today~!\n", stDate_t.usYear, stDate_t.ucMonth, stDate_t.ucDay);
				}
			}
			break;
#if 0
		case MSG_GWM_QUIT_UI :
			return MESSAGE_PASS;
#endif
		case eMEVT_BUS_DESTROY :
			BUS_KillTimer(APSHDPLUS_CHECK_TIMER);
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}



