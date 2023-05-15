/* $Header: */ 
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:			drv_lnb.h
// Original Author: 	Alvin
// File Description:	LNB Device Module (A8290 for HD-S7000(iCon))
// Module:			DEV_LNB
// Remarks:

/*******************************************************************/ 	
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/
#ifndef	__DRV_LNB_H__
#define	__DRV_LNB_H__

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
#define LNB_NOT_OVERLOADED		0
#define LNB_OVERLOADED			1


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
	SWITCH_OPTION_NONE,
	SWITCH_OPTION_A,
	SWITCH_OPTION_B,
	SWITCH_OPTION_MOTOR
} DRV_CH_SWITCH_OPTION;

typedef enum {
	SATELLITE_POSITION_NONE,
	SATELLITE_POSITION_A,
	SATELLITE_POSITION_B
} DRV_CH_SATELITE_POSITION;

typedef enum {
	LNB_POWER_ON,
	LNB_POWER_OFF,
	LNB_POWER_TEST,
	LNB_POWER_SHORT
} DRV_CH_LNB_POWER;

typedef enum {
	LNB_SELECT_LOW,
	LNB_SELECT_HIGH
} DRV_CH_LNB_SELECT;

typedef enum {
	LNB_VOLTAGE_STD,
	LNB_VOLTAGE_HIGH
}DRV_CH_LNB_VOLTAGE;

typedef enum
{
	LNB_POL_NOT_DEFINED = 0,
	LNB_POL_LINEAR_HOR,
	LNB_POL_LINEAR_VER,
	LNB_POL_CIRCUL_LEFT,
	LNB_POL_CIRCUL_RIGHT
} DRV_CH_LNB_POL;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_LNB_Init(void);
DRV_Error DRV_LNB_SetPolarization(DRV_CH_LNB_POL ucPolar, DRV_CH_LNB_VOLTAGE etVoltage);
DRV_Error DRV_LNB_SetLNBPower(DRV_CH_LNB_POWER etPower);
DRV_Error DRV_LNB_CheckOverload(HUINT8 *usReadReg);

#endif 

