/* $Header:   //DTV/////.di_power.h   1.0   July 28 2008 13:25:52      jhson$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_power.h
// Original Author: Son Jung Hee
// File Description: Power Header File.
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

#ifndef __DI_POWER_H__
#define __DI_POWER_H__

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

typedef enum
{
           AC_ON                                  = 0x0001,
           FRONT_KEY                            = 0x0002,
           IR_NEC_TYPE                           = 0x0004,
           IR_JVC_TYPE                            = 0x0008,
           REAL_TIME_CLOCK         = 0x0010,
           WATCH_DOG                           = 0x0020,
           SERIAL_COMMAND                   = 0x0040,
           HDMI_CEC                              = 0x0080,
           SYSTEM_REBOOT           = 0x0100,
           RF4CE_RCU                             = 0x0200,
           FAN_LOCKED                          = 0x0400,
           OVER_TEMPERATURE      = 0x0800,
           WAKE_ON_LAN                             = 0x1000,
           RESERVED5                             = 0x2000,
           RESERVED6                             = 0x4000,
           RESERVED7                             = 0x8000
} DI_POWER_REASON ;

typedef enum
{
	DI_POWER_OFFMODE_STANDBY_NORMAL,
	DI_POWER_OFFMODE_STANDBY_POWERSAVING,
	DI_POWER_OFFMODE_POWERSAVING_READY
} DI_POWER_OFFMODE ;

typedef enum{
	DI_POWER_BOOTMODE_WARM,
	DI_POWER_BOOTMODE_COLD,
	DI_POWER_BOOTMODE_MAX
}DI_POWER_BOOTMODE;

typedef enum{
	DI_PM_POWERSTATE_IDLE,
	DI_PM_POWERSTATE_LIGHT_SLEEP,
	DI_PM_POWERSTATE_DEEP_SLEEP,
	DI_PM_POWERSTATE_MAX
}DI_PM_POWERSTATE;

typedef struct{
	HBOOL bIR;
	HBOOL bFrontKey;
	HBOOL bWOL;
	HBOOL bTimer;
	struct{
		struct{
			HUINT32 ulInterval;
		}stTimer;
	}stInfo;
}DI_PM_WAKEUP_SRC;


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

DI_ERR_CODE DI_POWER_SetPowerOff (DI_POWER_OFFMODE mode) ;
DI_ERR_CODE DI_POWER_SetPowerOffWaitDelay (HUINT8 delay) ;
DI_ERR_CODE DI_POWER_GetWakeupReason (DI_POWER_REASON *reason) ;
DI_ERR_CODE DI_POWER_SetMainChipReset (void) ;
DI_ERR_CODE DI_POWER_SetMainChipResetWaitDelay (HUINT8 delay) ;
DI_ERR_CODE DI_POWER_SetMicomReboot (void);

DI_ERR_CODE DI_POWER_SetDVDPowerOnOff (HBOOL onOff) ;

DI_ERR_CODE DI_POWER_PrepareStandby(DI_PM_POWERSTATE eState);
DI_ERR_CODE DI_POWER_CancelStandby(void);
DI_ERR_CODE DI_POWER_SetStandby(DI_PM_POWERSTATE eState, DI_PM_WAKEUP_SRC stSrc, DI_POWER_BOOTMODE eBootMode);



// End Function Prototype

#endif /* __DI_POWER_H__ */

