/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_fan.h"

#include "drv_fan.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FAN_Open()
{
	DRV_FAN_SetTestMode(TRUE);
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FAN_SpeedCtrl(FDI_FAN_CTRL_e eCtrl)
{
	DRV_FAN_CTRL eFanCtrl;
	
	switch(eCtrl)
	{
		case FDI_FAN_CTRL_STOP:
			eFanCtrl = DRV_FAN_CTRL_STOP;
			break;
		case FDI_FAN_CTRL_LOW:
			eFanCtrl = DRV_FAN_CTRL_LOW;
			break;
		case FDI_FAN_CTRL_LOW_MIDDLE:
			eFanCtrl = DRV_FAN_CTRL_LOW_MIDDLE;
			break;
		case FDI_FAN_CTRL_MIDDLE:
			eFanCtrl = DRV_FAN_CTRL_MIDDLE;
			break;
		case FDI_FAN_CTRL_HIGH:
			eFanCtrl = DRV_FAN_CTRL_HIGH;
			break;
		default:
			eFanCtrl = DRV_FAN_CTRL_LOW;
			break;
	}
	
	DRV_FAN_SetCtrl(eFanCtrl);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_FAN_GetSpeed(unsigned long *ulFanSpeed)
{
	DRV_FAN_SETTING FanSetting;
	
	DRV_FAN_GetInfo(&FanSetting);
	*ulFanSpeed = FanSetting.ulFanSpeed_Freq;

	return FDI_NO_ERR;
}

