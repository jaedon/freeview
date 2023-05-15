/**
	@file     psvc_sys.c
	@brief    psvc_sys.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_sys.h>

#include "local_include/_xsvc_sys.h"

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

HERROR	psvc_sys_GetDeviceId(HUINT8 *pszDeviceId, HUINT32 ulBufSize)
{
#if defined(CONFIG_OP_FREESAT)
	return xsvc_sys_GetDeviceId_FreeSat(pszDeviceId, ulBufSize);
#elif defined(CONFIG_PROD_HGS1000S)
	return xsvc_sys_GetDeviceId_HGS1000s(pszDeviceId, ulBufSize);
#else
	return xsvc_sys_GetDeviceId_Base(pszDeviceId, ulBufSize);
#endif
}

HERROR	psvc_sys_GetSystemId(HUINT32 *pulSystemId)
{
#if 0
	
#elif defined(CONFIG_PROD_HMS1000S)
	return xsvc_sys_GetSystemId_Hms1000s(pulSystemId);
#else
	return xsvc_sys_GetSystemId_Base(pulSystemId);
#endif
}

HERROR	psvc_sys_GetModelName(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
#if 0

#elif defined(CONFIG_PROD_HMS1000S)
	return xsvc_sys_GetModelName_Hms1000s(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_HD9000I)
	return xsvc_sys_GetModelName_Hd9000i(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_ICORDPRO)
	return xsvc_sys_GetModelName_iCordPro(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_HDNEO)
	return xsvc_sys_GetModelName_HDNEO(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_HGS1000S)
	return xsvc_sys_GetModelName_HGS1000s(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_CDIG2000C)
	return xsvc_sys_GetModelName_Cdig2000c(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_YSR2000)
	return xsvc_sys_GetModelName_DerivBase(pszModelName, ulLength);
#elif defined(CONFIG_PROD_YSR1000)
	return xsvc_sys_GetModelName_DerivBase(pszModelName, ulLength);
#elif defined(CONFIG_PROD_HA7100S)
	return xsvc_sys_GetModelName_Ha7100s(pszModelName, ulLength, bPanelSizeOn);
#elif defined(CONFIG_PROD_FVP4000T)
	return xsvc_sys_GetModelName_Fvp4000t(pszModelName, ulLength, bPanelSizeOn);
#else
	return xsvc_sys_GetModelName_Base(pszModelName, ulLength, bPanelSizeOn);
#endif
}

HERROR	psvc_sys_GetEeprom4LoaderDefaultItemList(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum)
{
	return xsvc_sys_GetEeprom4LoaderFactoryReset_Base(pastDefaultItem, pulItemNum);
}

HERROR	psvc_sys_GetFusingInfo(Sys_FusingInfo_t *pstFusingInfo)
{
	return xsvc_sys_GetFusingInfo_Base(pstFusingInfo);
}

HERROR	psvc_sys_SetFusingInfoJTAG(HUINT8 ucJTAG)
{
	return xsvc_sys_SetFusingInfoJTAG_Base(ucJTAG);
}

HERROR	psvc_sys_SetFusingInfoCW(HUINT8 ucCWE)
{
	return xsvc_sys_SetFusingInfoCW_Base(ucCWE);
}

HERROR	psvc_sys_SetFusingInfoSecureBoot(HUINT8 ucSCS)
{
	return xsvc_sys_SetFusingInfoSecureBoot_Base(ucSCS);
}

HUINT32	psvc_sys_UpgradeGetMaxUpgradeSize(void)
{
#if defined(CONFIG_PROD_HB1000S)
	return xsvc_sys_UpgradeGetMaxUpgradeSize_Hb1000s();
#else
	return xsvc_sys_UpgradeGetMaxUpgradeSize_Base();
#endif
}

HERROR	psvc_sys_GetRgbLedInfo(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum)
{
#if defined(CONFIG_PROD_YSR2000)
	return xsvc_sys_GetRgbLedInfo_Ysr2000(pRgbLedList, pulNum);
#elif defined(CONFIG_PROD_YS1000)
	return xsvc_sys_GetRgbLedInfo_Ys1000(pRgbLedList, pulNum);
#else
	return xsvc_sys_GetRgbLedInfo_Base(pRgbLedList, pulNum);
#endif
}

HULONG	psvc_sys_GetRgbBlinkInterval(void)
{
#if defined(CONFIG_PROD_YSR2000)
	return xsvc_sys_GetRgbBlinkInterval_Ysr2000();
#elif defined(CONFIG_PROD_YS1000)
	return xsvc_sys_GetRgbBlinkInterval_Ys1000();
#else
	return xsvc_sys_GetRgbBlinkInterval_Base();
#endif
}


