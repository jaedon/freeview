/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:			drv_lnb.h
  Original Author: 	Alvin
  File Description:	LNB Device Module (A8290 for HD-S7000(iCon))
  Module:			DEV_LNB
  Remarks:
*/

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/
#ifndef	__DRV_CHANNEL_LA8286_H__
#define	__DRV_CHANNEL_LA8286_H__

#define SUPPORT_S_LNB_A8286

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

int LA8286_Init(int nUnitId);
int LA8286_SetOutput(int  nUnitId, HBOOL bPower);
int LA8286_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage);
int LA8286_CheckAntState(int nUnitId);
int LA8286_ReadStatus(int nUnitId, HUINT8 *pucData);
#endif

