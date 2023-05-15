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

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

// 사용하지는 않지만...
HERROR	xsvc_sys_GetSystemId_HGS1000s(HUINT32 *pulSystemId)
{
	*pulSystemId = 0x90107D73;

	return ERR_OK;
}

HERROR	xsvc_sys_GetModelName_HGS1000s(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	HUINT8		szBuffer[16] = {0, };

	/* prefix */
	HxSTD_StrNCpy(pszModelName, SYSTEM_MODEL_NAME_PREFIX, ulLength);

	/* project CONFIG NAME */
	HxSTD_MemSet(szBuffer, 0, (sizeof(HUINT8) * 16));

	HxSTD_StrNCpy(szBuffer, SYSTEM_MODEL_NAME, 16-1);
	HxSTD_StrNCat(pszModelName, szBuffer, ulLength);

	return ERR_OK;
}

HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_HGS1000s(void)
{
	return 1024*1024*128;
}

HERROR xsvc_sys_GetDeviceId_HGS1000s(HUINT8 *pszDeviceId, HUINT32 ulBufSize)
{
	HUINT32 ulChipId=0;

	if(SVC_SYS_GetChipId(&ulChipId) != ERR_OK)
		ulChipId = 0;
		
	HxSTD_snprintf(pszDeviceId, ulBufSize, "HMX-HGS-1000S-%x",ulChipId);

	return ERR_OK;
}
