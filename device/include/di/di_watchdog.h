/* $Header:   //DTV/////.di_watchdog.h   1.0   July 28 2008 13:25:52      jhson$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_watchdog.h
// Original Author: Son Jung Hee
// File Description: Watchdog Header File.
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

#ifndef __DI_WATCHDOG_H__
#define __DI_WATCHDOG_H__

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files

#include "di_err.h"
#include "htype.h"

// End Including Headers

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

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

DI_ERR_CODE DI_WATCHDOG_OnOff(HBOOL onOff);
DI_ERR_CODE DI_WATCHDOG_GetUseStatus(HBOOL *useStatus);
DI_ERR_CODE DI_WATCHDOG_SetTimeOut(HUINT16 sec);
DI_ERR_CODE DI_WATCHDOG_Reset(void);

// End Function Prototype


#endif /* __DI_WATCHDOG_H__ */
