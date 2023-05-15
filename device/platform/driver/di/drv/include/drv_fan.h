/* $Header $ */
/********************************************************************
* $Workfile $
* $Modtime $
* Author: Hyun Seok Choi & Kyu-Young Cho
*
* DRV FAN Header for kraken
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
********************************************************************/
#ifndef __DRV_FAN_H__
#define __DRV_FAN_H__

#include "htype.h"
#include "drv_err.h"

#if defined(CONFIG_NEW_FAN_SCENARIO)
#define FAN_VOLTAGE_MAX				(12) 	// about MAX Fan Control Voltage
#else
#define FAN_VOLTAGE_MAX				(11) 	// about MAX Fan Control Voltage
#endif

typedef enum eDRV_FAN_STATUS
{
	DRV_FAN_STATUS_INIT = 0,
	DRV_FAN_STATUS_NORMAL,
	DRV_FAN_STATUS_HWTEST
} DRV_FAN_STATUS;

typedef enum eDRV_FAN_CTRL
{
	DRV_FAN_CTRL_STOP 				= 0,		// DC 0V
	DRV_FAN_CTRL_LOW				= 7,		// DC 7V
	DRV_FAN_CTRL_LOW_MIDDLE			= 8,		// DC 8V
	DRV_FAN_CTRL_MIDDLE				= 9,		// DC 9V
	DRV_FAN_CTRL_MIDDLE_HIGH		= 10,		// DC 10V
	DRV_FAN_CTRL_HIGH				= 11,		// DC 11V
	DRV_FAN_CTRL_VOLTAGE
} DRV_FAN_CTRL;

typedef struct	sDRV_FAN_SETTING
{
	HUINT8					ucSmartTemp;
	HUINT32					ulVoltValue;	// Only using if nFanCtrlStatus == DRV_FAN_CTRL_VOLTAGE
	HUINT16					usPWM_cWord;
	HUINT16					usPWM_PeriodInterval;
	HUINT16					usPWM_OnInterval;
	HUINT32					ulFanSpeed_Freq;
	HUINT32					ulFanSpeed_AdjVolt;
	DRV_FAN_STATUS			nFanStatus;
	DRV_FAN_CTRL 			nFanCtrl;
} DRV_FAN_SETTING;

typedef	enum
{
	DRV_FAN_EVENT_CHECK_TEMP,			/**< smart temperature over shutdown threshold ( >68) */
	DRV_FAN_EVENT_FAN_LOCKED,
	DRV_FAN_EVENT_OVER_TEMPERATURE,
	DRV_FAN_EVENT_END
}DRV_FAN_EVENT; 

typedef void (*DRV_FAN_CALLBACK)(void*);


DRV_Error DRV_FAN_Init(void);
DRV_Error DRV_FAN_Uninit(void);
DRV_Error DRV_FAN_SetCtrl(DRV_FAN_CTRL fanCtrl);
DRV_Error DRV_FAN_SetCtrlDetail(HUINT32 ulVoltValue);
DRV_Error DRV_FAN_SetControlWord(HUINT16 usControlWord);
DRV_Error DRV_FAN_SetPeriodInterval(HUINT16 usPeriodInterval);
DRV_Error DRV_FAN_SetOnInterval(HUINT16 usOnInterval);
DRV_Error DRV_FAN_SetTestMode(HBOOL bTestMode);
DRV_Error DRV_FAN_DisplayInfo(char* pMsg);
DRV_Error DRV_FAN_GetInfo(DRV_FAN_SETTING *pFanSetting);
DRV_Error DRV_FAN_SetCtrlFactory(DRV_FAN_CTRL fanCtrl);
void DRV_FAN_RegisterEventNotify(DRV_FAN_EVENT eFanEvent, DRV_FAN_CALLBACK pfFanEventCallback);
#endif

