/**************************************************************************************/
/**
 * @file oplnagrapcmanager.h
 *
 * Parental Control Manager Interfaces
 *
 * @author  humax
 * @date    2012/05/15
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef	__OPLNAGRAPCMANAGER_H__
#define	__OPLNAGRAPCMANAGER_H__

#include	"opltype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	OPL_NAGRA_SC_PIN_CORRECT			0
#define	OPL_NAGRA_SC_PIN_INCORRECT_1st		1
#define	OPL_NAGRA_SC_PIN_INCORRECT_2nd		2
#define	OPL_NAGRA_SC_PIN_BLOCKED			3

OPL_BOOL	OPL_NAGRA_PCMAN_IsSmartCardInserted(void);
OPL_BOOL	OPL_NAGRA_PCMAN_SetSmartCardPIN(const char *oldPinCode, const char *newPinCode);
OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINAccessibleState(int *pnAccessibleState);
OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINRetryCount(int *pnRetryCount);
OPL_BOOL	OPL_NAGRA_PCMAN_GetSmartCardPINRemainTime(int *pnRemainTime);
OPL_BOOL	OPL_NAGRA_PCMAN_VerifySmartCardPIN(const char *pinCode, int *pnRetryCount);
OPL_BOOL	OPL_NAGRA_PCMAN_UnlockAV(void);

#ifdef __cplusplus
};
#endif

#endif
