/**
	@file     xsvc_sys_hms100s.c
	@brief    xsvc_sys_hms100s.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_sys.h>

#include "../local_include/_xsvc_sys.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TDC_RGBLED_BLINK_INTERVAL		(700)	//ms

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC SvcSys_RgbLedInfo_t		s_stRgbLedInfo[] = 
	{
			{eDxLED_ID_RGB_REC, {eDxRGBLED_COLOR_NONE, eDxRGBLED_DIMMLEVEL_SOLID}, FALSE},
			{eDxLED_ID_RGB_NET, {eDxRGBLED_COLOR_NONE, eDxRGBLED_DIMMLEVEL_SOLID}, FALSE}
	};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xsvc_sys_GetRgbLedInfo_Ysr2000(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum)
{
	*pRgbLedList = s_stRgbLedInfo;
	*pulNum = sizeof(s_stRgbLedInfo) / sizeof(SvcSys_RgbLedInfo_t);

	return ERR_OK;
}

HULONG xsvc_sys_GetRgbBlinkInterval_Ysr2000(void)
{
	return TDC_RGBLED_BLINK_INTERVAL;
}

