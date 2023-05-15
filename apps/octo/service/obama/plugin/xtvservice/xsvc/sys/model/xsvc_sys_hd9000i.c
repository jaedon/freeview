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

HERROR	xsvc_sys_GetSystemId_Hd9000i(HUINT32 *pulSystemId)
{
	*pulSystemId = 0x80508700;

	return ERR_OK;
}

HERROR	xsvc_sys_GetModelName_Hd9000i(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	HUINT8		szBuffer[16] = {0, };

	if(pszModelName == NULL)
	{
		HxLOG_Error("Target is NULL\n");
		return ERR_FAIL;
	}

	/* prefix */
	HxSTD_StrNCpy(pszModelName, SYSTEM_MODEL_NAME_PREFIX, ulLength);

	/* project CONFIG NAME */
	HxSTD_MemSet(szBuffer, 0, (sizeof(HUINT8) * 16));

	HxSTD_StrNCpy(szBuffer, SYSTEM_MODEL_NAME, 16-1);
	HxSTD_StrNCat(pszModelName, szBuffer, ulLength);

	return ERR_OK;
}

