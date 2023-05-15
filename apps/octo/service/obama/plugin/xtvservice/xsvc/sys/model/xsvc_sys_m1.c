/**
	@file     xsvc_sys_m1.c
	@brief    xsvc_sys_m1.c

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

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xsvc_sys_GetSystemId_M1(HUINT32 *pulSystemId)
{
	*pulSystemId = 0x90107D80;

	return ERR_OK;
}

HERROR	xsvc_sys_GetModelName_M1(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	HUINT8		szBuffer[16] = {0, };

	/* prefix */
//	MWC_UTIL_DvbStrcpy(pszModelName, SYSTEM_MODEL_NAME_PREFIX);
	HxSTD_StrNCpy((char*)pszModelName, SYSTEM_MODEL_NAME_PREFIX, ulLength);

	/* project CONFIG NAME */
	HxSTD_MemSet(szBuffer, 0, (sizeof(HUINT8) * 16));

//	MWC_UTIL_DvbStrcpy(szBuffer, SYSTEM_MODEL_NAME);
//	MWC_UTIL_DvbStrcat(pszModelName, szBuffer);
	HxSTD_StrNCpy((char*)szBuffer, SYSTEM_MODEL_NAME, 16-1);
	HxSTD_StrNCat((char*)pszModelName, (const char*)szBuffer, ulLength);

	return ERR_OK;
}

HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_M1(void)
{
	return 1024*1024*128;
}

