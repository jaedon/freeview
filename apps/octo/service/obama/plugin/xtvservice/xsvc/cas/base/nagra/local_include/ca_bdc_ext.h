/*
@file				ca_bdc_ext.h
@brief				ca_bdc_ext.h (Nagra BDC Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef CA_BDC_EXT_H
#define CA_BDC_EXT_H

#include "ca_bdc.h"

typedef TBdcStatus (* TBdcDisplayPopUp)(TBdcPopUpPersistence xPersistence, TUnsignedInt8 xNumLanguages, const TBdcPopUpContent *pxContent);
typedef TBdcStatus (* TBdcRemovePopUp)(void);


/********************************************************************
  Extern Function declaration.
 ********************************************************************/
extern HERROR CA_NA_BdcRegisterPopUpFunc(TBdcDisplayPopUp pfnDisplayPopUp, TBdcRemovePopUp pfnRemovePopUp);
extern HERROR CA_NA_BdcUnregisterPopUpFunc(void);

#endif /* CA_BDC_EXT_H */

