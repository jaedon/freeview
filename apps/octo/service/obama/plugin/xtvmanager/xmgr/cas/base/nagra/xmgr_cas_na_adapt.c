/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <namgr_main.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include <mgr_cas.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//STATIC XmgrCas_NaUiAlramData_t		s_stNaUiAlarmData;

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/
#if (1) // APS-Moon
#define LOC_CHIPSET_PAIRING_REQUIRED			"Chipset Parining Required..."
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_ADAPT_LOCAL_FUNCTION______________________________________________________

#if 1
STATIC XmgrCas_NaAlamType_e	_xmgr_cas_NaConvertAccessAlarmtoAPP(TCaAccess eAlarm)
{
	XmgrCas_NaAlamType_e eCasAlarmType = eCAS_NA_CA_ALARM_TYPE_ERR;

	switch (eAlarm)
	{
	case CA_ACCESS_CLEAR:
		eCasAlarmType = eCAS_NA_CA_ACCESS_FREE;
		break;

	case CA_ACCESS_GRANTED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED;
		break;

	case CA_ACCESS_FREE:
		eCasAlarmType = eCAS_NA_CA_ACCESS_FREE;
		break;

	case CA_ACCESS_GRANTED_PPT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED_PPT;
		break;

	case CA_ACCESS_DENIED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED;
		break;

	case CA_ACCESS_NO_VALID_SMARTCARD:
		eCasAlarmType = eCAS_NA_CA_ACCESS_NO_VALID_SMARTCARD;
		break;

	case CA_ACCESS_BLACKED_OUT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_BLACKOUT;
		break;

	case CA_ACCESS_DENIED_NO_VALID_CREDIT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT;
		break;

	case CA_ACCESS_DENIED_COPY_PROTECTED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED;
		break;

	case CA_ACCESS_DENIED_PARENTAL_CONTROL:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_PARENTAL_CONTROL;
		break;


	case CA_ACCESS_DENIED_DIALOG_REQUIRED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED;
		break;

	case CA_ACCESS_DENIED_PAIRING_REQUIRED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED;
		break;

	case CA_ACCESS_DENIED_CHIPSET_PAIRING_REQUIRED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED;
		break;

	case CA_ACCESS_SMARTCARD_BLACKLISTED:
	case CA_ACCESS_SMARTCARD_SUSPENDED:
	case CA_ACCESS_DENIED_FOR_PARENTAL_RATING:
	case CA_ACCESS_RIGHT_SUSPENDED:
	case CA_ACCESS_DENIED_BUT_PPT:
	default :
		//deprecated or don't enter here
		break;
	}

	return eCasAlarmType;
}
#else
STATIC XmgrCas_NaAlamType_e	_xmgr_cas_NaConvertAccessAlarmtoAPP(TAccessAlarm eAlarm)
{
	XmgrCas_NaAlamType_e eCasAlarmType = eCAS_NA_CA_ALARM_TYPE_ERR;

	switch (eAlarm)
	{
	case CA_ALARM_ACCESS_FREE:
		eCasAlarmType = eCAS_NA_CA_ACCESS_FREE;
		break;

	case CA_ALARM_ACCESS_GRANTED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED;
		break;

	case CA_ALARM_ACCESS_GRANTED_PPT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED_PPT;
		break;

	case CA_ALARM_ACCESS_GRANTED_FOR_PREVIEW:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED_FOR_PREVIEW;
		break;

	case CA_ALARM_ACCESS_GRANTED_BUT_WARNING_PREVIEW:
		eCasAlarmType = eCAS_NA_CA_ACCESS_GRANTED_BUT_WARNING_PREVIEW;
		break;

	case CA_ALARM_ACCESS_DENIED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED;
		break;

	case CA_ALARM_ACCESS_DENIED_BUT_PREVIEW:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_BUT_PREVIEW;
		break;

	case CA_ALARM_ACCESS_DENIED_FOR_PARENTAL_RATING:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_FOR_PARENTAL_RATING;
		break;

	case CA_ALARM_ACCESS_BLACKOUT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_BLACKOUT;
		break;

	case CA_ALARM_ACCESS_DENIED_BUT_PPT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_BUT_PPT;
		break;

	case CA_ALARM_ACCESS_DENIED_NO_VALID_CREDIT:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_NO_VALID_CREDIT;
		break;

	case CA_ALARM_ACCESS_DENIED_COPY_PROTECTED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_COPY_PROTECTED;
		break;

	case CA_ALARM_DENIED_PAIRING_REQUIRED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_PAIRING_REQUIRED;
		break;

	case CA_ALARM_DENIED_CS_PAIRING_REQUIRED:
		eCasAlarmType = eCAS_NA_CA_ACCESS_DENIED_CS_PAIRING_REQUIRED;
		break;

	default :
		//don't enter here
		break;
	}

	return eCasAlarmType;
}

#endif


#if 1
STATIC XmgrCas_NaAlamType_e	_xmgr_cas_NaConvertSmarcardAlarmtoAPP(TSmartcardState eAlarm)
{
	XmgrCas_NaAlamType_e eCasAlarmType = eCAS_NA_CA_ALARM_TYPE_ERR;

	switch (eAlarm)
	{
	case CA_SMARTCARD_STATE_OK:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_OK;
		break;

	case CA_SMARTCARD_STATE_ERROR:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_COM_ERROR;
		break;

	case CA_SMARTCARD_STATE_MUTE:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_MUTE;
		break;

	case CA_SMARTCARD_STATE_INVALID:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
		break;

	case CA_SMARTCARD_STATE_BLACKLISTED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_BLACKLISTED;
		break;

	case CA_SMARTCARD_STATE_SUSPENDED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_SUSPENDED;
		break;

	case CA_SMARTCARD_STATE_NEVER_PAIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NEVER_PAIRED;
		break;

	case CA_SMARTCARD_STATE_NOT_PAIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NOT_PAIRED;
		break;

	case CA_SMARTCARD_STATE_EXPIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_EXPIRED;
		break;

	case CA_SMARTCARD_STATE_NOT_CERTIFIED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED;
		break;

	case CA_ALARM_SMARTCARD_REMOVED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_REMOVED;
		break;

	default :
		//don't enter here
		break;
	}

	return eCasAlarmType;
}

#else
STATIC XmgrCas_NaAlamType_e	_xmgr_cas_NaConvertSmarcardAlarmtoAPP(TSmartcardAlarm eAlarm)
{
	XmgrCas_NaAlamType_e eCasAlarmType = eCAS_NA_CA_ALARM_TYPE_ERR;

	switch (eAlarm)
	{
	case CA_ALARM_SMARTCARD_OK:
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
		if (!xmgr_cas_NaCheckTivuCardSystemId() && xmgr_cas_NaCheckTivuService())
		{
			eCasAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
			break;
		}
#endif
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_OK;
		break;

	case CA_ALARM_SMARTCARD_COM_ERROR:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_COM_ERROR;
		break;

	case CA_ALARM_SMARTCARD_MUTE:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_MUTE;
		break;

	case CA_ALARM_SMARTCARD_INVALID:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
		break;

	case CA_ALARM_SMARTCARD_BLACKLISTED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_BLACKLISTED;
		break;

	case CA_ALARM_SMARTCARD_SUSPENDED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_SUSPENDED;
		break;

	case CA_ALARM_SMARTCARD_NEVER_PAIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NEVER_PAIRED;
		break;

	case CA_ALARM_SMARTCARD_NOT_PAIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NOT_PAIRED;
		break;

	case CA_ALARM_SMARTCARD_EXPIRED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_EXPIRED;
		break;

	case CA_ALARM_SMARTCARD_NOT_CERTIFIED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED;
		break;

	case CA_ALARM_SMARTCARD_REMOVED:
		eCasAlarmType = eCAS_NA_CA_SMARTCARD_REMOVED;
		break;

	default :
		//don't enter here
		break;
	}

	return eCasAlarmType;
}
#endif

STATIC XmgrCas_NaAlamType_e	_xmgr_cas_NaRetrieveAlarmMessage(NaAlarmData_t *pstNaAlarmData)
{
	XmgrCas_NaAlamType_e eCasAlarmType = eCAS_NA_CA_ALARM_TYPE_ERR;

	if (pstNaAlarmData == NULL)
	{
		return eCasAlarmType;
	}

	if (pstNaAlarmData->eAlarmType == eAlarm_Access)
	{
		eCasAlarmType = _xmgr_cas_NaConvertAccessAlarmtoAPP(pstNaAlarmData->pstAccessAlarmItems->eAlarm);
		HxLOG_Print("Access eAlarm[%x] \n", pstNaAlarmData->pstAccessAlarmItems->eAlarm);
	}
	else if (pstNaAlarmData->eAlarmType == eAlarm_Smartcard)
	{
		eCasAlarmType = _xmgr_cas_NaConvertSmarcardAlarmtoAPP(pstNaAlarmData->pstScAlarmItems->eAlarm);
		HxLOG_Print("Smartcard eAlarm[%x] \n", pstNaAlarmData->pstScAlarmItems->eAlarm);

	}
	else
	{
		HxLOG_Print("error[%x] \n", pstNaAlarmData->eAlarmType);
	}

	return eCasAlarmType;
}


//This function is processing only alarm message type
STATIC HERROR				_xmgr_cas_Naalarm_message_function(void* pData, XmgrCas_NaAlamType_e *peCasAlarmType)
{
	XmgrCas_NaAlamType_e	eCasAlarmType;
//	HINT32				nMessageId;
	NaAlarmData_t 		*pstNaAlarmData = NULL;

	if (peCasAlarmType == NULL)
	{
		HxLOG_Print("NULL\n");
		return ERR_FAIL;
	}

	pstNaAlarmData = (NaAlarmData_t *)pData;
	if (pstNaAlarmData == NULL)
	{
		HxLOG_Print("NaAlarmData_t error\n");
		return ERR_FAIL;
	}

	eCasAlarmType = _xmgr_cas_NaRetrieveAlarmMessage(pstNaAlarmData);
	if (eCasAlarmType == eCAS_NA_CA_ALARM_TYPE_ERR)
	{
		HxLOG_Print("_xmgr_cas_NaRetrieveAlarmMessage error[%d] \n", eCasAlarmType);
		return ERR_FAIL;
	}

	*peCasAlarmType = eCasAlarmType;
	HxLOG_Print("peCasAlarmType[%d] \n", *peCasAlarmType);

	return ERR_OK;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_ADAPT_PUBLIC_FUNCTION______________________________________________________

HERROR		xmgr_cas_NaAlarmDataProcessing(void *pDataNa, XmgrCas_NaAlamType_e *peCasAlarmType)
{
	return _xmgr_cas_Naalarm_message_function(pDataNa, peCasAlarmType);
}

HERROR		xmgr_cas_NaGetSmartCardStatus(XmgrCas_NaAlamType_e *peScAlarm)
{
	HERROR				hErrResult;
	HUINT32				eScAlarm = CA_ALARM_SMARTCARD_REMOVED;

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	HUINT32 i = 0;
	HUINT32 eTempAlarm = CA_ALARM_SMARTCARD_REMOVED;
#if defined(CONFIG_DEBUG)
	for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
	{
		hErrResult = xmgr_cas_NaGetCurrScStatus(i, &eScAlarm);
//		if (hErrResult == ERR_OK && eScAlarm == CA_ALARM_SMARTCARD_OK)
		{
			TPpid						providerId;
			SvcCas_CtrlParam_t 			stParam;
			NA_GetProviderIdInParam_t		stInParam;
			NA_GetProviderIdOutParam_t	stOutParam;

			stParam.ulControlType = eCACTRL_NA_GetProviderId;
			stInParam.ucSlotId = i;
			stParam.pvIn = (void *)&stInParam;
			stParam.pvOut = (void *)&stOutParam;
			hErrResult = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eCACTRL_NA_GetProviderId, &stParam);
			providerId = stOutParam.usProviderId;

			//hErrResult = MW_CA_NA_GetProviderId(i, &providerId);
			HxLOG_Error("hErrResult = %d, providerId = 0x%x\n", hErrResult, providerId);
//			continue;
		}
	}
#endif
	for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
	{
		hErrResult = xmgr_cas_NaGetCurrScStatus(i, &eScAlarm);
		HxLOG_Print("eScAlarm = 0x%x\n", eScAlarm);
		if (hErrResult != ERR_OK || eScAlarm == CA_ALARM_SMARTCARD_REMOVED || eScAlarm == CA_SMARTCARD_STATE_INCOMPATIBLE)
		{
			continue;
		}
		else
		{
			TPpid providerId;
			hErrResult = MW_CA_NA_GetProviderId(i, &providerId);
			HxLOG_Print("providerId = 0x%x\n", providerId);

			if (hErrResult == ERR_OK && (providerId == NA_CAS_TIVU_PROVIDER_ID || providerId == NA_CAS_TIVU_TEST_PROVIDER_ID))
			{
				HxLOG_Print("providerId is right!!!\n");
				break;
			}

			if (eTempAlarm == CA_ALARM_SMARTCARD_REMOVED)
			{
				eTempAlarm = eScAlarm;	// Tivu smartcard를 찾기 전 까지는 첫번째 카드 alarm을 저장하고 있자
			}
		}
	}

	if (i == CAS_MAX_NUM_OF_SC)		// Tivu smartcard를 찾지 못한 경우, 첫번째 카드 alarm을 display
	{
		eScAlarm = eTempAlarm;
	}
#else
	#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	hErrResult = xmgr_cas_NaGetCurrScStatus(CARDLESS_SLOT_ID, &eScAlarm);
	#else
	hErrResult = xmgr_cas_NaGetCurrScStatus(0, &eScAlarm);
	#endif

	if (hErrResult != ERR_OK)
	{
		*peScAlarm = eCAS_NA_CA_SMARTCARD_REMOVED;
		return ERR_FAIL;
	}
#endif

	*peScAlarm = _xmgr_cas_NaConvertSmarcardAlarmtoAPP(eScAlarm);
	return ERR_OK;
}

HBOOL xmgr_cas_NaIsSmartCardStatusOK(void)
{
	XmgrCas_NaAlamType_e 	peScAlarm = eCAS_NA_CA_ALARM_TYPE_ERR;
	HERROR				hErr = ERR_FAIL;

	hErr = xmgr_cas_NaGetSmartCardStatus(&peScAlarm);
	if(hErr == ERR_OK && peScAlarm == eCAS_NA_CA_SMARTCARD_OK)
	{
		return TRUE;
	}

	return FALSE;
}

HERROR		xmgr_cas_NaGetSmartCardStatusBySlotID(HUINT8 ucSlotId, XmgrCas_NaAlamType_e *peScAlarm)
{
	HERROR				hErrResult;
	HUINT32				eScAlarm = CA_ALARM_SMARTCARD_REMOVED;

	hErrResult = xmgr_cas_NaGetCurrScStatus(ucSlotId, &eScAlarm);
	if (hErrResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	*peScAlarm = _xmgr_cas_NaConvertSmarcardAlarmtoAPP(eScAlarm);
	return ERR_OK;
}

