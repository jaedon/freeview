/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_CHANNEL_LA8304_H__
#define	__DRV_CHANNEL_LA8304_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
// End Including Headers


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition

// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
int LA8304_Init(int nUnitId);
int LA8304_SetOutput(int  nUnitId, HBOOL bPower);
int LA8304_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage);
int LA8304_CheckAntState(int nUnitId);
int LA8304_ReadStatus(int nUnitId, HUINT8 *pucData);
// End global function prototypes

#endif /* !__DRV_CHANNEL_LA8304_H__ */
