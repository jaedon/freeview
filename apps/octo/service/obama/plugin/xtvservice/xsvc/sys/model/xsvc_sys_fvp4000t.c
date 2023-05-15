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

#include <pal_sys.h>
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

#define SYSTEM_ID_FVP5000T	0x90107d44
#define SYSTEM_MODEL_NAME_FVP5000T	"FVP-5000T"

HERROR	xsvc_sys_GetModelName_Fvp4000t(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	HUINT32 ulSystemId=0;

	if (pszModelName == NULL)
	{
		HxLOG_Error("Target is NULL\n");
		return ERR_FAIL;
	}

	if (ERR_OK == PAL_SYS_GetSystemId(&ulSystemId))
	{
		if (SYSTEM_ID_FVP5000T == ulSystemId)	// System ID of FVP-5000T : 9010.7D44
		{
			HxSTD_snprintf(pszModelName, ulLength, "%s%s", SYSTEM_MODEL_NAME_PREFIX, SYSTEM_MODEL_NAME_FVP5000T);
		}
		else
		{
			HxSTD_snprintf(pszModelName, ulLength, "%s%s", SYSTEM_MODEL_NAME_PREFIX, SYSTEM_MODEL_NAME);
		}
	}
	else
	{
		HxSTD_snprintf(pszModelName, ulLength, "%s%s", SYSTEM_MODEL_NAME_PREFIX, SYSTEM_MODEL_NAME);
	}
	
	return ERR_OK;
}
