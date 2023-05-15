/*
@file				ca_icc_ext.h
@brief				ca_icc_ext.h (Nagra ICC Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_ICC_EXT_H
#define _CA_ICC_EXT_H

#include "ca_icc.h"
//#include "pal_sc.h"
#include <_svc_cas_mgr_common.h>

/********************************************************************
  Structure and Type Definition
 ********************************************************************/
typedef enum tagCardStatus
{
	eSc_Out = 0,
	eSc_In,
	eSc_Mute,
	eSc_Invalid,
	eSc_CardStatus_Max
} CardStatus_t;

typedef enum tagScInstStatus
{
	eSc_Inserted = 0,
	eSc_ArtReceived,
	eSc_ValidityReceived,
	eSc_Extracted,
	eSc_ScInstStatus_Max
} ScInstStatus_t;

typedef struct tagScStatusItem
{
	TUnsignedInt8 		ucSlotId;
	CardStatus_t		eCardStatus;
} ScStatusItem_t;

typedef struct tagScMgrInstance
{
	SciHandle_t			hSmcHandle;
	SvcCas_SciCb_t 		pfnSmcCallback;
	TIccSessionId		ulSessionId;
	SvcCas_DevSciProtocol_e		eProtocol;
	TIccClockFrequency	ulFreq;
	TIccAccessMode		eMode;
	CardStatus_t		eCardStatus;
	ScInstStatus_t		eInstState;
	TUnsignedInt8		ucAtr[ICC_ATR_MAX_LEN+1];
	TUnsignedInt8		ucAtrLen;
} ScMgrIntance_t;


/********************************************************************
  Extern Function Declaration
 ********************************************************************/
extern HERROR CA_NA_IccGetCardStatus(ScStatusItem_t *pstScStatus);
extern HERROR CA_NA_IccGetNumberOfSlots(TUnsignedInt8 *pucSlots);
extern HERROR CA_NA_IccGetRegistrationStatus(void);

#endif /* _CA_ICC_EXT_H */


