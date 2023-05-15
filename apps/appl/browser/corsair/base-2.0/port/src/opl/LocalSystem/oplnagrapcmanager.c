/**************************************************************************************/
/**
 * @file oplnagrapcmanager.c
 *
 * Defines the Parental Control Manager structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include	"oplnagrapcmanager.h"

#include	"hlib.h"
#include	"apk.h"

/**************************************************************
	internal structures and variables
 **************************************************************/
/**************************************************************
	Nagra ParentalControlManager functions
 **************************************************************/
#define	____PARENTAL_CONTROL_MANAGER_FUNCTIONS_______________

OPL_BOOL	OPL_NAGRA_PCMAN_IsSmartCardInserted(void)
{
	if (APK_CAS_NAGRA_IsSCInserted())
	{
		return OPL_TRUE;
	}
	return OPL_FALSE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_SetSmartCardPIN(const char *oldPinCode, const char *newPinCode)
{
	HxLOG_Trace();

	if (APK_CAS_NAGRA_PINCODE_Set(oldPinCode, newPinCode))
		return OPL_TRUE;

	return OPL_FALSE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINRetryCount(int *pnRetryCount)
{
	HxLOG_Trace();

	HUINT8	accessState = 0;
	HUINT32	remainTime = 0;
	HUINT8	retryCount = 0;

	if (APK_CAS_NAGRA_GetAccessibleState(&accessState, &remainTime, &retryCount))
	{
		*pnRetryCount = retryCount;
		return OPL_TRUE;
	}
	return OPL_FALSE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINAccessibleState(int *pnAccessibleState)
{
	HxLOG_Trace();

	HUINT8	accessState = 0;

//	if (APK_CAS_NAGRA_GetAccessibleState(&accessState, &remainTime, &retryCount))
	APK_CAS_NAGRA_IsAccessibleState(&accessState);

	*pnAccessibleState = accessState;
	return OPL_TRUE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINRemainTime(int *pnRemainTime)
{
	HxLOG_Trace();

	HUINT8	accessState;
	HUINT32	remainTime;
	HUINT8	retryCount;

	if (APK_CAS_NAGRA_GetAccessibleState(&accessState, &remainTime, &retryCount))
	{
		*pnRemainTime = remainTime;
		return OPL_TRUE;
	}
	return OPL_FALSE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_VerifySmartCardPIN(const char *pinCode, int *pnRetryCount)
{
	HxLOG_Trace();

	HUINT8	accessState;
	HUINT32	remainTime;
	HUINT8	retryCount;

	if (APK_CAS_NAGRA_VerifyPincodeGetAccessibleState(&accessState, &remainTime, &retryCount, pinCode))
	{
		switch(retryCount)
		{
			case 0:
				*pnRetryCount = OPL_NAGRA_SC_PIN_CORRECT;
				break;
			case 1:
				*pnRetryCount = OPL_NAGRA_SC_PIN_INCORRECT_1st;
				break;
			case 2:
				*pnRetryCount = OPL_NAGRA_SC_PIN_INCORRECT_2nd;
				break;
			case 3:
				*pnRetryCount = OPL_NAGRA_SC_PIN_BLOCKED;
				break;
		}
		return OPL_TRUE;
	}
	return OPL_FALSE;
}

OPL_BOOL	OPL_NAGRA_PCMAN_UnlockAV()
{
	HxLOG_Trace();

        
#if defined (CONFIG_OP_SES)
	if (APK_CAS_NAGRA_UnlockAV())
		return OPL_TRUE;
#endif        
	return OPL_FALSE;
}

