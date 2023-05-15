
#ifndef __XSVC_SYS_INT_H__
#define __XSVC_SYS_INT_H__
/*******************************************************************************
* File Name    : _xsvc_sys.h
*
* Description  :
*
* Copyright (C) 2013, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                               Name
* ----        ------------                                             --------
*
*******************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_sys.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

extern HERROR	xsvc_sys_GetDeviceId_Base(HUINT8 *pszDeviceId, HUINT32 ulBufSize);
extern HERROR	xsvc_sys_GetDeviceId_FreeSat(HUINT8 *pszDeviceId, HUINT32 ulBufSize);
extern HERROR	xsvc_sys_GetDeviceId_HGS1000s(HUINT8 *pszDeviceId, HUINT32 ulBufSize);

extern HERROR	xsvc_sys_GetSystemId_Base(HUINT32 *pulSystemId);
extern HERROR	xsvc_sys_GetSystemId_Hms1000s(HUINT32 *pulSystemId);
extern HERROR	xsvc_sys_GetSystemId_Hd9000i(HUINT32 *pulSystemId);

extern HERROR	xsvc_sys_GetModelName_Base(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_Hms1000s(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_Hd9000i(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_iCordPro(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_HDNEO(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_HGS1000s(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_Cdig2000c(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_Ha7100s(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_Fvp4000t(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR	xsvc_sys_GetModelName_DerivBase(HUINT8 *pszModelName, HUINT32 ulLength);

extern HERROR	xsvc_sys_GetEeprom4LoaderFactoryReset_Base(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum);

extern HERROR	xsvc_sys_GetFusingInfo_Base(Sys_FusingInfo_t *pstFusingInfo);
extern HERROR	xsvc_sys_SetFusingInfoJTAG_Base(HUINT8 ucJTAG);
extern HERROR	xsvc_sys_SetFusingInfoCW_Base(HUINT8 ucCWE);
extern HERROR	xsvc_sys_SetFusingInfoSecureBoot_Base(HUINT8 ucSCS);

extern HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_Base(void);
extern HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_Hms1000s(void);
extern HUINT32	xsvc_sys_UpgradeGetMaxUpgradeSize_Hb1000s(void);

extern HERROR xsvc_sys_GetRgbLedInfo_Base(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum);
extern HERROR xsvc_sys_GetRgbLedInfo_Ysr2000(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum);
extern HERROR xsvc_sys_GetRgbLedInfo_Ys1000(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum);

extern HULONG xsvc_sys_GetRgbBlinkInterval_Base(void);
extern HULONG xsvc_sys_GetRgbBlinkInterval_Ysr2000(void);
extern HULONG xsvc_sys_GetRgbBlinkInterval_Ys1000(void);

#endif	//_LOG_CAPTURE_H_
