/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include <_svc_cas_mgr_nvram.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	BLOCK_UNIXTIME					(10 * 60L)
#define	MAX_RETRY_PININPUT				3
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define NUM_OF_CARD_SLOTS				1
#else
#define NUM_OF_CARD_SLOTS				3
#endif
#define MAX_SMC_ID_LENGTH				20
#define DEF_FAILURE_COUNTER_NULL		0xFF

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
#define CA_PIN_DEFAULT_INDEX			2
#endif

/*******************************************************************/
/********************      Structure       *************************/
/*******************************************************************/
typedef struct tagPCPinInfo
{
	UNIXTIME	retryTime;
	HUINT8		nRetryInput;
	HUINT8		fAccess;
	HUINT8		aucFutureReserv[2];
	HUINT8 		aucSMCId[MAX_SMC_ID_LENGTH];
} XmgrCas_NaUiPcPinInco_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaUiPcPinInco_t 		s_pinInfo[NUM_OF_CARD_SLOTS];


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PINCODE_LOCAL_FUNCTION______________________________________________________

STATIC HERROR				xmgr_cas_NaReadPincodefromEEP(void)
{
	HUINT32 					ulCnt = 0;
	HERROR						hReturnError = ERR_OK;

	hReturnError = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_PIN_INFORMATION_OFFSET,
										  sizeof(XmgrCas_NaUiPcPinInco_t) * NUM_OF_CARD_SLOTS, (HUINT8 *)&s_pinInfo[0]);

	if (hReturnError == ERR_FAIL)
	{
		HxLOG_Error("EEPROM Read Error \n");
	}
	else
	{
		HxLOG_Print("[********* \n");
		for(ulCnt = 0; ulCnt < NUM_OF_CARD_SLOTS; ulCnt++)
		{
			HxLOG_Print("%d, [%d][%x]\n", ulCnt, s_pinInfo[ulCnt].nRetryInput, s_pinInfo[ulCnt].retryTime);
		}
	}

	return hReturnError;
}

STATIC HERROR				xmgr_cas_NaWritePincodefromEEP(void)
{
	HERROR						hReturnError = ERR_OK;

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_PIN_INFORMATION_OFFSET,
										  sizeof(XmgrCas_NaUiPcPinInco_t) * NUM_OF_CARD_SLOTS, (HUINT8 *)&s_pinInfo[0]);

	if (hReturnError == ERR_FAIL)
	{
		HxLOG_Error("EEPROM Write Error \n");
	}

	return hReturnError;
}

STATIC HERROR				xmgr_cas_NaUpdatePinState(UNIXTIME *pCurTime)
{
	UNIXTIME 				curTime;
	HUINT32 				ulCnt;
	HBOOL 					fDataChanged = FALSE;
	HERROR					hError = ERR_OK;

	if (pCurTime == NULL)
	{
		return ERR_FAIL;
	}

	hError = VK_CLOCK_GetTime((unsigned long*)&curTime);
	if(hError != ERR_OK)
	{
		HxLOG_Error("get system time error[%x] \n", hError);
		return 	hError;
	}

	*pCurTime = curTime;

	//set valid state
	for(ulCnt = 0; ulCnt < NUM_OF_CARD_SLOTS; ulCnt++)
	{
		s_pinInfo[ulCnt].fAccess = TRUE;

		if (s_pinInfo[ulCnt].nRetryInput > 0)
		{
			HxLOG_Print("[%x], [%x], [%x] \n", s_pinInfo[ulCnt].retryTime, curTime, s_pinInfo[ulCnt].retryTime - curTime);

			if ((curTime >= s_pinInfo[ulCnt].retryTime) || ((s_pinInfo[ulCnt].retryTime - curTime) > BLOCK_UNIXTIME))
			{
				// reset pin input
				s_pinInfo[ulCnt].nRetryInput = 0;
				s_pinInfo[ulCnt].retryTime = 0;

				// reset smc ID
				if(s_pinInfo[ulCnt].nRetryInput == 0)
				{
					HxSTD_memset(s_pinInfo[ulCnt].aucSMCId, 0, sizeof(s_pinInfo[ulCnt].aucSMCId));
				}
				fDataChanged = TRUE; // data changed
			}
		}

		HxLOG_Print("nRetryInput[%x], retryTime[%x] \n", s_pinInfo[ulCnt].nRetryInput, s_pinInfo[ulCnt].retryTime);
	}


	//save to EEPROM
	if(fDataChanged == TRUE)
	{
		xmgr_cas_NaWritePincodefromEEP();
	}


	return hError;
}

STATIC HERROR				xmgr_cas_NaPINCode_Initialize(void)
{
	HUINT32 				ulCnt;
	UNIXTIME 				curTime;

	HxSTD_memset(&s_pinInfo[0], 0, sizeof(XmgrCas_NaUiPcPinInco_t) * NUM_OF_CARD_SLOTS);

	xmgr_cas_NaReadPincodefromEEP();

	for(ulCnt = 0; ulCnt < NUM_OF_CARD_SLOTS; ulCnt++)
	{
		if (s_pinInfo[ulCnt].nRetryInput > MAX_RETRY_PININPUT)
		{
			HxSTD_memset(&s_pinInfo[ulCnt], 0, sizeof(XmgrCas_NaUiPcPinInco_t));
		}

		s_pinInfo[ulCnt].fAccess = TRUE;
	}

	return xmgr_cas_NaUpdatePinState(&curTime);
}

STATIC HUINT32				xmgr_cas_NaGetPincodeFromString(HUINT8 *szPincode)
{
	HUINT8					*p;
	HUINT32 				nPinCode;

	for (nPinCode = 0, p = szPincode; *p ; p++)
	{
		nPinCode *= 10;
		nPinCode += ((*p)-'0');
	}

	return nPinCode;
}

STATIC HUINT32				xmgr_cas_NaGetAvailableTimeSlot(HUINT8 *aucCardSer)
{
	HUINT32 ulCounter = DEF_FAILURE_COUNTER_NULL;
	HUINT32 ulCnt;

	//smartcard가 하나일 경우 timeslot은 0으로 fix
	if (NUM_OF_CARD_SLOTS == 1)
	{
		HxLOG_Print("force slot(0)!\n");
		return 0;
	}

	for (ulCnt = 0; ulCnt <  NUM_OF_CARD_SLOTS; ulCnt++)
	{
		if (MWC_UTIL_DvbStrncmp(s_pinInfo[ulCnt].aucSMCId, aucCardSer, MAX_SMC_ID_LENGTH) == 0)
		{
			ulCounter = ulCnt;
		}
	}

	if (ulCounter == DEF_FAILURE_COUNTER_NULL)
	{
		for (ulCnt = 0; ulCnt <  NUM_OF_CARD_SLOTS; ulCnt++)
		{
			if (s_pinInfo[ulCnt].aucSMCId[0] == 0)
			{
				ulCounter = ulCnt;
				break;
			}
		}
	}

	HxLOG_Print("available slot is [%d]\n", ulCounter);

	return ulCounter;
}

STATIC eCAS_PIN_STATE		xmgr_cas_NaGetValidPinCounter(HUINT32 *pulCounter, HUINT8 *ret_aucCardSer)
{
	HUINT8 					aucCardSer[NA_UI_SC_NUMBER_STRING_LEN];
	HUINT32 				ulCounter = 0;

	HxSTD_memset(aucCardSer, 0, sizeof(aucCardSer));

	xmgr_cas_NaUpdateSystemInfo();

	//get smartcard serial number
	if (xmgr_cas_NaGetCardSerialNumber(aucCardSer) == ERR_FAIL)
	{
		return eCAS_PIN_STATE_NOACCESS;
	}

	//get valid failure counter
	ulCounter = xmgr_cas_NaGetAvailableTimeSlot(aucCardSer);
	if((ulCounter == DEF_FAILURE_COUNTER_NULL) || (ulCounter >= NUM_OF_CARD_SLOTS))
	{
		return eCAS_PIN_STATE_NORIGHT;
	}

	*pulCounter = ulCounter;
	if (ret_aucCardSer != NULL)
	{
		MWC_UTIL_DvbStrncpy(ret_aucCardSer, aucCardSer, NA_UI_SC_NUMBER_STRING_LEN);
	}

	return eCAS_PIN_STATE_OK;
}


STATIC eCAS_PIN_STATE		_xmgr_cas_NaGetPincodeAccessibleState(HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	HERROR					hError;
	eCAS_PIN_STATE			hReturnState;
	HUINT32 				ulCounter = 0;
	UNIXTIME 				curTime;
	XmgrCas_NaAlamType_e		eScAlarm;

	if (pulRemainTime == NULL || pnRetryCount == NULL)
	{
		HxLOG_Error("pulRemainTime or pnRetryCount is NULL\n");
		return eCAS_PIN_STATE_NOTIME;
	}

	// redundancy 지만 power off/on 시에 문제가 되서...
	(void)xmgr_cas_NaPincodeInit();

	if (xmgr_cas_NaUpdatePinState(&curTime) != ERR_OK)
	{
		HxLOG_Error("Get current time error\n");
		return eCAS_PIN_STATE_NOTIME;
	}

#if defined(CONFIG_DEBUG)
{
	HxDATETIME_t				stDateTime;
	HERROR					nErrCode;

	nErrCode = HLIB_DATETIME_ConvertUnixTimeToDateTime(curTime, (HxDATETIME_t*)&stDateTime);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime() : fail\n");
	}

	HxLOG_Debug("curTime = %d, %d/%d/%d, %dh:%dm:%ds\n", curTime,
				stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
				stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

}
#endif

	hError = xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	if (hError != ERR_OK)
	{
		HxLOG_Error("eCAS_PIN_STATE_NOACCESS \n");
		return eCAS_PIN_STATE_NOACCESS;
	}

	switch (eScAlarm)
	{
	case eCAS_NA_CA_SMARTCARD_COM_ERROR:
	case eCAS_NA_CA_SMARTCARD_MUTE:
	case eCAS_NA_CA_SMARTCARD_INVALID:
	case eCAS_NA_CA_SMARTCARD_SUSPENDED:
	case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:
	case eCAS_NA_CA_SMARTCARD_REMOVED:
		
		HxLOG_Error("eCAS_PIN_STATE_NOACCESS \n");
		return eCAS_PIN_STATE_NOACCESS;

	default :
		break;
	}

	hReturnState = xmgr_cas_NaGetValidPinCounter(&ulCounter, NULL);
	if (hReturnState != eCAS_PIN_STATE_OK)
	{
		HxLOG_Error("Error Get Valid Pin-counter \n");
		*pulRemainTime = BLOCK_UNIXTIME;
		return hReturnState;
	}

	*pulRemainTime = (s_pinInfo[ulCounter].retryTime - curTime);
	*pnRetryCount  = s_pinInfo[ulCounter].nRetryInput;

	if (s_pinInfo[ulCounter].nRetryInput >= MAX_RETRY_PININPUT)
	{
		if (curTime <= s_pinInfo[ulCounter].retryTime)
		{
			HxLOG_Error("eCAS_PIN_STATE_NORIGHT \n");
			return eCAS_PIN_STATE_NORIGHT;
		}
	}

	HxLOG_Error("eCAS_PIN_STATE_OK \n");

	return eCAS_PIN_STATE_OK;
}

STATIC HERROR				xmgr_cas_NaSetPincodeSuccess(void)
{
	HUINT32 				ulCounter = 0;
	eCAS_PIN_STATE			hReturnState;

	hReturnState = xmgr_cas_NaGetValidPinCounter(&ulCounter, NULL);
	if (hReturnState != eCAS_PIN_STATE_OK)
	{
		HxLOG_Error("Error Get Valid Pin-counter \n");
		return ERR_FAIL;
	}

	s_pinInfo[ulCounter].nRetryInput = 0;
	s_pinInfo[ulCounter].retryTime = 0;
	HxSTD_memset(s_pinInfo[ulCounter].aucSMCId, 0, sizeof(s_pinInfo[ulCounter].aucSMCId));

	HxLOG_Print("nRetryInput[%x], retryTime[%x] \n", s_pinInfo[0].nRetryInput, s_pinInfo[0].retryTime);

	//write eeprom
	xmgr_cas_NaWritePincodefromEEP();

	return ERR_OK;
}


STATIC HERROR				xmgr_cas_NaSetPincodeFailure(void)
{
	HUINT32 				ulCounter = 0;
	HUINT8 					aucCardSer[NA_UI_SC_NUMBER_STRING_LEN];
	eCAS_PIN_STATE			hReturnState;
	HERROR					hError = ERR_OK;

	hReturnState = xmgr_cas_NaGetValidPinCounter(&ulCounter, &aucCardSer[0]);
	if (hReturnState != eCAS_PIN_STATE_OK)
	{
		HxLOG_Error("Error Get Valid Pin-counter \n");
		return ERR_FAIL;
	}

	s_pinInfo[ulCounter].nRetryInput++;

	HxSTD_memset(s_pinInfo[ulCounter].aucSMCId, 0, MAX_SMC_ID_LENGTH);
	MWC_UTIL_DvbStrncat(s_pinInfo[ulCounter].aucSMCId, aucCardSer, MAX_SMC_ID_LENGTH);

	hError = VK_CLOCK_GetTime((unsigned long*)&s_pinInfo[ulCounter].retryTime);
	if(hError != ERR_OK)
	{
		HxLOG_Error("get system time error[%x] \n", hError);
		return 	hError;
	}

	s_pinInfo[ulCounter].retryTime += BLOCK_UNIXTIME;

	HxLOG_Print("retryTime[%x] \n", s_pinInfo[ulCounter].retryTime);

#if defined(CONFIG_DEBUG)
{
	HxDATETIME_t				stDateTime;
	HERROR					nErrCode;

	nErrCode = HLIB_DATETIME_ConvertUnixTimeToDateTime(s_pinInfo[ulCounter].retryTime, (HxDATETIME_t*)&stDateTime);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime() : fail\n");
	}

	HxLOG_Debug("retryTime = %d, %d/%d/%d, %dh:%dm:%ds\n", s_pinInfo[ulCounter].retryTime,
				stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
				stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

}
#endif

	//write eeprom
	xmgr_cas_NaWritePincodefromEEP();

	return ERR_OK;
}

STATIC HERROR				_xmgr_cas_NaCheckSmartCardPin(HUINT8* szPincode)
{
	HERROR					hError = ERR_OK;
	TPinCodeStatus 			xStatus = CA_PIN_CODE_ERROR;
	HUINT32 				nPinCode;

	if (szPincode == NULL)
		return ERR_FAIL;

	nPinCode = xmgr_cas_NaGetPincodeFromString(szPincode);
#ifndef WIN32
#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	hError = xmgr_cas_NaCheckPincode(CA_PIN_INDEX_2, nPinCode, szPincode, &xStatus);
#else
	hError = xmgr_cas_NaCheckPincode (CA_PIN_DEFAULT_INDEX, nPinCode, szPincode, &xStatus);
#endif
#else
	if (nPinCode == 1234)
		xStatus = CA_PIN_CODE_PIN_ACCEPTED;
	else
		xStatus = CA_PIN_CODE_PIN_REJECTED;
#endif
	HxLOG_Error("xStatus[%x] \n", xStatus);

	if (hError == ERR_OK && xStatus == CA_PIN_CODE_PIN_ACCEPTED)
	{
		xmgr_cas_NaSetPincodeSuccess();

		return ERR_OK;
	}

	xmgr_cas_NaSetPincodeFailure();

	return ERR_FAIL;
}

STATIC HERROR				_xmgr_cas_NaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	HERROR					hError = ERR_OK;
	TPinCodeStatus 			xStatus = CA_PIN_CODE_ERROR;
	HUINT32 				nNewPinCode, nOldPincode;

	if (szOldPincode == NULL || szNewPincode == NULL)
		return ERR_FAIL;

	nOldPincode = xmgr_cas_NaGetPincodeFromString(szOldPincode);
	nNewPinCode = xmgr_cas_NaGetPincodeFromString(szNewPincode);
#ifndef WIN32
#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	hError = xmgr_cas_NaChangePincode(CA_PIN_INDEX_2, nOldPincode, nNewPinCode, szOldPincode, szNewPincode, &xStatus);
#else
	hError = xmgr_cas_NaChangePincode(CA_PIN_DEFAULT_INDEX, nOldPincode, nNewPinCode, szOldPincode, szNewPincode, &xStatus);
#endif
#else
	xStatus = CA_PIN_CODE_PIN_ACCEPTED;
#endif
	HxLOG_Error("xStatus[%x] \n", xStatus);

	if (hError == ERR_OK && xStatus == CA_PIN_CODE_PIN_ACCEPTED)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PINCODE_PUBLIC_FUNCTION______________________________________________________

HERROR			xmgr_cas_NaPincodeInit(void)
{
	return xmgr_cas_NaPINCode_Initialize();
}

eCAS_PIN_STATE	xmgr_cas_NaGetPincodeAccessibleState(HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	return _xmgr_cas_NaGetPincodeAccessibleState(pulRemainTime, pnRetryCount);
}

HERROR			xmgr_cas_NaCheckSmartCardPin(HUINT8* szPicode)
{
	return _xmgr_cas_NaCheckSmartCardPin(szPicode);
}

HERROR			xmgr_cas_NaGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	HERROR				hError = ERR_OK;
	HUINT32 				ulRemainTime = 0;	// 10m = 600s 보다 작은 값.
	HUINT8				nRetryCount = 0;
	eCAS_PIN_STATE 		nAccessState = eCAS_PIN_STATE_OK;

	if (xmgr_cas_NaPincodeCheckSCRemoved())
	{
		*pnAccessState = eCAS_PIN_STATE_NOACCESS;
		*pulRemainTime = ulRemainTime;
		*pnRetryCount = nRetryCount;

		HxLOG_Error("Smartcard not inserted! \n");
		return ERR_FAIL;
	}

	nAccessState = xmgr_cas_NaGetPincodeAccessibleState(&ulRemainTime, &nRetryCount);
	if (nAccessState == eCAS_PIN_STATE_OK)
	{
		HxLOG_Error("eCAS_PIN_STATE_OK \n");
		// web app에서 처리
		hError = ERR_OK;
	}
	else if (nAccessState == eCAS_PIN_STATE_NORIGHT)
	{
		HxLOG_Error("eCAS_PIN_STATE_NORIGHT \n");
		hError = ERR_FAIL;
	}
	else if (nAccessState == eCAS_PIN_STATE_NOACCESS || nAccessState == eCAS_PIN_STATE_NOTIME)
	{
		HxLOG_Error("eCAS_PIN_STATE_NOACCESS or eCAS_PIN_STATE_NOTIME (%d)\n", nAccessState);
		hError = ERR_FAIL;
	}
	else
	{
		// 이런 경우는 없다.
		HxLOG_Error("else >>> \n");

		ulRemainTime = 0;
		hError = ERR_FAIL;
	}

	*pnAccessState = nAccessState;
	*pulRemainTime = ulRemainTime;
	*pnRetryCount = nRetryCount;

	return hError;
}

/*
  *	web app에서 nagra pincode 입력창에서 입력한 pincode로 verify하고, 실패시 eCAS_PIN_STATE, remaintime, retrycount값을 올려준다.
  */
HERROR			xmgr_cas_NaVerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8* szPincode)
{
	HERROR				hError = ERR_OK;
	HUINT32 				ulRemainTime = 0;	// 10m = 600s 보다 작은 값.
	HUINT8				nRetryCount = 0;
	eCAS_PIN_STATE 		nAccessState = eCAS_PIN_STATE_OK;

	if (xmgr_cas_NaPincodeCheckSCRemoved())
	{
		*pnAccessState = eCAS_PIN_STATE_NOACCESS;
		*pulRemainTime = ulRemainTime;
		*pnRetryCount = nRetryCount;

		HxLOG_Error("Smartcard not inserted! \n");
		return ERR_FAIL;
	}

	hError = xmgr_cas_NaCheckSmartCardPin(szPincode);
	if (hError == ERR_OK)
	{
		HxLOG_Error("[%s:%d] xmgr_cas_NaCheckSmartCardPin() success \n", __FUNCTION__, __HxLINE__);

		xmgr_cas_NaSetPincodeSuccess();

		*pnAccessState = eCAS_PIN_STATE_OK;
		*pulRemainTime = ulRemainTime;
		*pnRetryCount = nRetryCount;

		return ERR_OK;
	}
	else
	{
		HxLOG_Error("xmgr_cas_NaCheckSmartCardPin() fail \n");
	}

	nAccessState = xmgr_cas_NaGetPincodeAccessibleState(&ulRemainTime, &nRetryCount);
	if (nAccessState == eCAS_PIN_STATE_OK)
	{
		HxLOG_Error("eCAS_PIN_STATE_OK \n");
		// web app에서 처리
		hError = ERR_OK;
	}
	else if (nAccessState == eCAS_PIN_STATE_NORIGHT)
	{
		HxLOG_Error("eCAS_PIN_STATE_NORIGHT(%d)\n", nAccessState);
		hError = ERR_FAIL;

	#if defined (CONFIG_OP_SES)
		// 편법이지만, 일단 이렇게...
		nAccessState = eCAS_PIN_STATE_OK;
		hError = ERR_OK;
		HxLOG_Error("force to set eCAS_PIN_STATE_OK(%d) >>>\n", nAccessState);
	#endif
	}
	else if (nAccessState == eCAS_PIN_STATE_NOACCESS)
	{
		HxLOG_Error("eCAS_PIN_STATE_NOACCESS(%d)\n", nAccessState);
		hError = ERR_FAIL;
	}
	else if (nAccessState == eCAS_PIN_STATE_NOTIME)
	{
		HxLOG_Error("eCAS_PIN_STATE_NOTIME (%d)\n", nAccessState);
		hError = ERR_FAIL;
	}
	else
	{
		// 이런 경우는 없다.
		HxLOG_Error("else >>> \n");

		ulRemainTime = 0;
		hError = ERR_FAIL;
	}

	*pnAccessState = nAccessState;
	*pulRemainTime = ulRemainTime;
	*pnRetryCount = nRetryCount;

	return hError;
}

HERROR			xmgr_cas_NaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	return _xmgr_cas_NaChangeSmartCardPin(szOldPincode, szNewPincode);
}

HUINT8			xmgr_cas_NaPincodeCheckSCRemoved(void)
{
	XmgrCas_NaAlamType_e eScAlarm;

	if (xmgr_cas_NaGetSmartCardStatus(&eScAlarm) == ERR_OK)
	{
		if (eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			return TRUE;
		}
	}

	return FALSE;
}


