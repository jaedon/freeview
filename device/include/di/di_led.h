/* $Header:   //DTV/////.di_led.h   1.0   July 28 2008 13:25:52      jhson$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_led.h
// Original Author: Son Jung Hee
// File Description: Led Header File.
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

#ifndef __DI_LED_H__
#define __DI_LED_H__

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
#define	DI_LED_DIM_LEVEL_MAX		(100)

// End #define

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee

typedef enum
{
    DI_LED_NUM0     = 0x00000001,
    DI_LED_NUM1     = 0x00000002,
    DI_LED_NUM2     = 0x00000004,
    DI_LED_NUM3     = 0x00000008,
    DI_LED_NUM4     = 0x00000010,
    DI_LED_NUM5     = 0x00000020,
    DI_LED_NUM6     = 0x00000040,
    DI_LED_NUM7     = 0x00000080,
    DI_LED_NUM8     = 0x00000100,
    DI_LED_NUM9     = 0x00000200,
    DI_LED_NUM10    = 0x00000400,
    DI_LED_NUM11    = 0x00000800,
    DI_LED_NUM12    = 0x00001000,
    DI_LED_NUM13    = 0x00002000,
    DI_LED_NUM14    = 0x00004000,
    DI_LED_NUM15    = 0x00008000,
    DI_LED_NUM16    = 0x00010000,
    DI_LED_NUM17    = 0x00020000,
    DI_LED_NUM18    = 0x00040000,
    DI_LED_NUM19    = 0x00080000,
    DI_LED_NUM20    = 0x00100000,
    DI_LED_NUM21    = 0x00200000,
    DI_LED_NUM22    = 0x00400000,
    DI_LED_NUM23    = 0x00800000,
    DI_LED_NUM24    = 0x01000000,
    DI_LED_NUM25    = 0x02000000,
    DI_LED_NUM26    = 0x04000000,
    DI_LED_NUM27    = 0x08000000,
    DI_LED_NUM28    = 0x10000000,
    DI_LED_NUM29    = 0x20000000,
    DI_LED_NUM30    = 0x40000000
} DI_LED_ID ;

typedef enum
{
    DI_LED_OFF,
    DI_LED_WATCHING,
    DI_LED_STANDBY,
    DI_LED_STANDBY_LEDLOW,
    DI_LED_WATCHING_LEDLOW,
    DI_LED_RECORDING
} DI_LED_STATE ;

typedef enum
{
    DI_LED_DIMMING_HIGH = 0,
    DI_LED_DIMMING_MID  = 1,
    DI_LED_DIMMING_LOW  = 2
} DI_LED_DIMMING_LEVEL;

typedef enum
{
    DI_LED_RED = 0,
    DI_LED_GREEN,
    DI_LED_BLUE,
    DI_LED_MAX
} DI_LED_RGB ;


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

DI_ERR_CODE DI_LED_GetCapability (HUINT32 *pulLedCapaMask) ;
DI_ERR_CODE DI_LED_SetPowerLedUse (HBOOL onOff) ;
DI_ERR_CODE DI_LED_SetStandbyLedUse (HBOOL onOff);
DI_ERR_CODE DI_LED_SetAll (void);
DI_ERR_CODE DI_LED_ClearAll (void) ;
DI_ERR_CODE DI_LED_SetPowerLedState (DI_LED_STATE state) ;
DI_ERR_CODE DI_LED_SetOn (DI_LED_ID ledId) ;
DI_ERR_CODE DI_LED_SetOff (DI_LED_ID ledId) ;
DI_ERR_CODE DI_LED_MultiSetOnOff (HUINT32 ledMask, HUINT32 onOffMask) ;
DI_ERR_CODE DI_LED_SetBlinkOn (DI_LED_ID ledId, HUINT32 delay) ;
DI_ERR_CODE DI_LED_SetBlinkOff (void) ;
DI_ERR_CODE DI_LED_MultiSetBlinkOn (HUINT32 ledMask, HUINT32 onOffMask, HUINT32 delay) ;
DI_ERR_CODE DI_LED_SetPowerLedDimmingLevel (HUINT8 ucDimLevel);
DI_ERR_CODE DI_LED_SetStandbyLedDimmingLevel (HUINT8 ucDimLevel);
DI_ERR_CODE DI_LED_SetLedDimmingLevel (DI_LED_ID ledId, HUINT8 ucDimLevel);
DI_ERR_CODE DI_LED_GetPowerLedDimmingLevel (HUINT8 *pucDimLevel);
DI_ERR_CODE DI_LED_GetStandbyLedDimmingLevel (HUINT8 *pucDimLevel);
DI_ERR_CODE DI_LED_GetLedDimmingLevel (DI_LED_ID ledId, HUINT8 *pucDimLevel);
DI_ERR_CODE DI_LED_GetStandbyLedUse (HBOOL *onOff);
DI_ERR_CODE DI_LED_GetPowerLedUse (HBOOL *onOff);
DI_ERR_CODE DI_LED_SetRGBOn (DI_LED_ID eLedID, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue);
DI_ERR_CODE DI_LED_SetLedsDimmingLevel (DI_LED_ID eLedID, HUINT8 ucDimLevel);
DI_ERR_CODE DI_LED_Blink(void);


// End Function Prototype

#endif /* __DI_LED_H__ */
