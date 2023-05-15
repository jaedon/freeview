/* $Header:   //.di_rtc.h   1.0   July 28 2008 13:25:52      kycho$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_clock.h
// Original Author: Son Jung Hee
// File Description: Clock Header File.
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

#ifndef __DI_RTC_H__
#define __DI_RTC_H__

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

typedef enum
{
	DI_RTC_ON_TIMER= 0,
	DI_RTC_OFF_TIMER
} DI_RTC_TIMER ;

typedef void (* pfnDI_RTC_Notify) (DI_RTC_TIMER eTimer) ;

// End typedef

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

// End static variable


/*******************************************************************/
/*********************** Function Prototype*************************/
/*******************************************************************/
// Start Function Prototype

void DI_RTC_RegisterRTCEventCallback (pfnDI_RTC_Notify pfnNotify) ;
DI_ERR_CODE DI_RTC_SetCurrentTime (HUINT32 unixTime) ;
DI_ERR_CODE DI_RTC_SetOnTime (HUINT32 unixTime) ;
DI_ERR_CODE DI_RTC_OnTimerOnOff (HBOOL onOff) ;
DI_ERR_CODE DI_RTC_GetCurrentTime (HUINT32 *unixTime) ;
DI_ERR_CODE DI_RTC_GetOnTime (HUINT32 *unixTime) ;
DI_ERR_CODE DI_RTC_GetBootTime(HUINT16 *pusMJD, HUINT32 *pulBootTime);

// End Function Prototype

#endif /* __DI_RTC_H__ */
