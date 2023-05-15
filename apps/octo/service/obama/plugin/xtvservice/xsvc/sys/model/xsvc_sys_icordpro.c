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
HERROR	xsvc_sys_GetSystemId_iCordPro(HUINT32 *pulSystemId)
{
	*pulSystemId = 0x809C5020;

	return ERR_OK;
}

HERROR	xsvc_sys_GetModelName_iCordPro(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	HERROR		hErr;
	HUINT32 		ulHWRevision = 0;
	HUINT8		szBuffer[16] = {0, };

	/* prefix */
	HxSTD_StrNCpy(pszModelName, SYSTEM_MODEL_NAME_PREFIX, ulLength);

	/* project CONFIG NAME */
	HxSTD_MemSet(szBuffer, 0, (sizeof(HUINT8) * 16));

	// iCord Pro 와 iCord NEO 의 특수성으로 인해. 이것만 다르다.
	hErr = SVC_SYS_GetSysHWRevision(&ulHWRevision);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SYS_GetSysHWRevision. Error\n");
		return hErr;
	}
	HxLOG_Error("[%s:%d] ulHWRevision=%d\n", __FUNCTION__, __HxLINE__, ulHWRevision);
	//sprintf(szBuffer, "HW %d", ulHWRevision); 테스트용임.
	
	if (ulHWRevision == 0 || ulHWRevision == 4 || ulHWRevision == 8 || ulHWRevision == 16 || ulHWRevision == 28)
	{
		// iCord Pro HW rev 0.1 = 0(0x0)
		// iCord Pro HW rev 0.2 = 4(0x4=00100)
		// iCord Pro HW rev 0.3 = 8(0x8=01000)
		// iCord Pro HW rev 0.4 = 16(0x10=10000)
		// iCord Pro HW rev 1.0 = 28(0x1C=111000) 	// 양산박스
		
		// iCord NEO HW rev 0.1 = 28(0x1C=111000)	// 개발박스가 이걸로 되어 있다. 그래서 개발시료는 iCord Pro로 보인다.
		HxSTD_StrNCpy(szBuffer, SYSTEM_MODEL_NAME, 16-1);
		HxSTD_StrNCat(pszModelName, szBuffer, ulLength);
	}
	else if (ulHWRevision == 20 || ulHWRevision == 24)
	{	
		// iCord NEO HW rev  = 20(0x14=10100) 	// 양산박스
		// iCord NEO HW rev  = 24(0x18=11000) 	// 양산박스
		HxSTD_StrNCpy(szBuffer, SYSTEM_MODEL_NAME_NEO, 16-1);
		HxSTD_StrNCat(pszModelName, szBuffer, ulLength);
	}
	else
	{
		HxSTD_StrNCpy(szBuffer, SYSTEM_MODEL_NAME_NEO, 16-1);
		HxSTD_StrNCat(pszModelName, szBuffer, ulLength);
	}
	
	return ERR_OK;
}

HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_iCordPro(void)
{
	return 1024*1024*128;
}

