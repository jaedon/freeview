/*******************************************************************/
/**
 * @file	di_fan.c
 *
 * Source Code for FAN Module
 *
 * @author	
 * @data	
 * @attention	Copyright (c) 2012 Humax - All rights reserved
 *
 **/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/

#include "vkernel.h"
#include "taskdef.h"
#include "drv_err.h"
#include "di_fan.h"
#include "drv_fan.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

//#define DI_FAN_DEBUG

#ifdef DI_FAN_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintExit()				VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


DI_ERR_CODE DI_FAN_SetMode(DI_FAN_MODE_e eMode)
{
	DRV_Error eDRV_Err;
	HBOOL bManualMode=FALSE;

	PrintEnter();

	if(eMode == DI_FAN_MODE_MANUAL)
	{
		bManualMode = TRUE;
	}
	else
	{
		bManualMode = FALSE;
	}
	
	eDRV_Err = DRV_FAN_SetTestMode(bManualMode);
	if(eDRV_Err != DRV_OK)
	{
		PrintError("[DI_FAN_SetMode] Error: DRV_FAN_SetTestMode() errCode(0x%x) \n", eDRV_Err);	
	}

	PrintExit();
	
	return DI_ERR_OK;
}


DI_ERR_CODE DI_FAN_Control(DI_FAN_CTRL_e eControl)
{
	DRV_Error eDRV_Err;
	DRV_FAN_CTRL eFanCtrl;
	
	PrintEnter();

	switch(eControl)
	{
		case DI_FAN_CTRL_STOP:
			eFanCtrl = DRV_FAN_CTRL_STOP;
			break;
		case DI_FAN_CTRL_LOW:
			eFanCtrl = DRV_FAN_CTRL_LOW;
			break;
		case DI_FAN_CTRL_LOW_MIDDLE:
			eFanCtrl = DRV_FAN_CTRL_LOW_MIDDLE;
			break;
		case DI_FAN_CTRL_MIDDLE:
			eFanCtrl = DRV_FAN_CTRL_MIDDLE;
			break;
		case DI_FAN_CTRL_MIDDLE_HIGH:
			eFanCtrl = DRV_FAN_CTRL_MIDDLE_HIGH;
			break;
		case DI_FAN_CTRL_HIGH:
			eFanCtrl = DRV_FAN_CTRL_HIGH;
			break;
		default:
			eFanCtrl = DRV_FAN_CTRL_LOW;
			break;
	}

	eDRV_Err = DRV_FAN_SetCtrl(eFanCtrl);
	if(eDRV_Err != DRV_OK)
	{
		PrintError("[DI_FAN_Control] Error: DRV_FAN_SetCtrl() errCode(0x%x) \n", eDRV_Err);	
	}

	PrintExit();
	
	return DI_ERR_OK;
}


DI_ERR_CODE DI_FAN_GetSpeed(HULONG *pulSpeed_Freq)
{
	DRV_Error eDRV_Err;
	DRV_FAN_SETTING FanSetting;

	if(!pulSpeed_Freq)
	{
		PrintError("[DI_FAN_GetSpeed] Error: Invalid Argument(pulSpeed_Freq = 0x%x)\n", pulSpeed_Freq);	
		return DI_ERR_ERROR;
	}
		
	eDRV_Err = DRV_FAN_GetInfo(&FanSetting);
	if(eDRV_Err != DRV_OK)
	{
		PrintError("[DI_FAN_GetSpeed] Error: DRV_FAN_GetInfo() errCode(0x%x) \n", eDRV_Err);	
		*pulSpeed_Freq = 0x0;
	}
	else
	{
		*pulSpeed_Freq = FanSetting.ulFanSpeed_Freq;
	}

	PrintExit();
	
	return DI_ERR_OK;
}

