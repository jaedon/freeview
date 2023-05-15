/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PSVC_SYS_H__
#define	__PSVC_SYS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



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
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	psvc_sys_GetDeviceId(HUINT8 *pszDeviceId, HUINT32 ulBufSize);
HERROR	psvc_sys_GetSystemId(HUINT32 *pulSystemId);
HERROR	psvc_sys_GetModelName(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
HERROR	psvc_sys_GetEeprom4LoaderDefaultItemList(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum);
HERROR	psvc_sys_GetFusingInfo(Sys_FusingInfo_t *pstFusingInfo);
HERROR	psvc_sys_SetFusingInfoJTAG(HUINT8 ucJTAG);
HERROR	psvc_sys_SetFusingInfoCW(HUINT8 ucCWE);
HERROR	psvc_sys_SetFusingInfoSecureBoot(HUINT8 ucSCS);
HUINT32	psvc_sys_UpgradeGetMaxUpgradeSize(void);
HERROR	psvc_sys_GetRgbLedInfo(SvcSys_RgbLedInfo_t **pRgbLedList, HUINT32 *pulNum);
HULONG	psvc_sys_GetRgbBlinkInterval(void);


#endif /* !__PSVC_SYS_H__ */
