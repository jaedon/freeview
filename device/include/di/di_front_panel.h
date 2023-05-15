/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_front_panel.h
// Original Author: Kyu-young Cho
// File Description: Panel(LCD/VFD/7-Seg) Header File.
// Module:
// Remarks:
//
//     input:
//
//     output:
//
//
/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_FRONT_PANEL_H__
#define __DI_FRONT_PANEL_H__

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files

#include "htype.h"
#include "di_err.h"

// End Including Headers

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

#define	DIM_LEVEL_MAX				(100)

// End #define
/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee


// End global variable

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef enum PANEL_TYPE{
	DI_FRONT_PANEL_TYPE_7SEG = 0,
	DI_FRONT_PANEL_TYPE_11SEG,
	DI_FRONT_PANEL_TYPE_VFD,
	DI_FRONT_PANEL_TYPE_LED,
	DI_FRONT_PANEL_TYPE_LCD,
	DI_FRONT_PANEL_TYPE_STLED,
	DI_FRONT_PANEL_TYPE_NONE
}DI_FRONT_PANEL_TYPE;

typedef struct PANNEL_CAP{
	DI_FRONT_PANEL_TYPE 	type;
	HUINT8         			max_length;
	HBOOL					dimmingLevelOnOff;
	HBOOL					displayOnOff;
}DI_FRONT_PANEL_CAP;

// End typedef
/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

// End static variable

/*******************************************************************/
/*********************** Function Prototype ***********************/
/*******************************************************************/
// Start Function Prototype

DI_ERR_CODE DI_FRONT_PANEL_GetCapability(DI_FRONT_PANEL_CAP *pstCapability);
DI_ERR_CODE DI_FRONT_PANEL_Display (HUINT8 *pstString);
DI_ERR_CODE DI_FRONT_PANEL_SetTimeDisplay(HBOOL bOn_nOff);
DI_ERR_CODE DI_FRONT_PANEL_SetDimmer(HBOOL bOn_Off);
DI_ERR_CODE DI_FRONT_PANEL_SetPanelPower(HBOOL bOn_nOff);
DI_ERR_CODE DI_FRONT_PANEL_SetDimmingLevel(HUINT8 ucDimLevel);
DI_ERR_CODE DI_FRONT_PANEL_SetDisplayAni(HBOOL bOn_nOff);
DI_ERR_CODE DI_FRONT_PANEL_SetDisplayAninamtion(HBOOL offOn);

// End Function Prototype


#endif /* __DI_FRONT_PANEL_H__ */

