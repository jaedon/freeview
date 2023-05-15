/********************************************************************
 * $Workfile:   va_sc_adapt.h  $
 * Project    :
 * Author     :
 * Description:
 *
 *                              Copyright (c) 2002 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _VA_SC_ADAPT_H
#define _VA_SC_ADAPT_H

/********************************************************************
 Header Files
 ********************************************************************/
#include	"htype.h"

/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
#define		T0_COMMAND_LEN			5
#define		VA_SC_INVALID_SLOT		-1
#define		VA_SC_INVALID_HANDLE	-1

/********************************************************************
 Structure and Type Definition
 ********************************************************************/

/********************************************************************
 Global Function Declaration
 ********************************************************************/
HERROR VA_SC_Adapt_Init();
HERROR VA_SC_Adapt_Reset(HINT32 nScSlot, HUINT8 *pucAtr, HUINT32 *pulAtrLen, HUINT8 *pucProtocol, HUINT8 ucPpsMax, HUINT8 *pucPpsData);
HERROR VA_SC_Adapt_Write(HINT32 nScSlot, HUINT8 *pucSndData, HUINT32 ulSndLen, HUINT8 *pucRcvData, HUINT32 *pulNumRead);
HERROR VA_SC_Adapt_Read(HINT32 nScSlot, HUINT8 *pucSndData, HUINT8 *pucRcvData, HUINT32 * pulNumRead);
HERROR VA_SC_Adapt_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession);
#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
int VA_SC_Adapt_Activate(HINT32 nScSlot);
#endif
HERROR VA_SC_Adapt_Deactivate(HINT32 nScSlot);

#endif

