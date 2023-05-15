/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_power.h
	@brief	  Octo pal power header

	Description: Power on/off관련 PAL의 헤더 파일.\n
	Module: PAL/POWER			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_POWER_H__
#define	__PAL_POWER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

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

/* Public Function 마다 필히 기재하도록 함 */

extern void PAL_PWR_Init(void);
extern HERROR PAL_PWR_GetWakeUpReason(WakeUpReason_t *pWakeUpReason);
extern HERROR PAL_PWR_SetWakeUpReason(WakeUpReason_t wakeUpReason);
extern HERROR PAL_PWR_TurnOffMainBoard(HBOOL bPowerSaveMode);
extern HERROR PAL_PWR_SetRtcTime(HUINT32 ulUnixTime);
extern HERROR PAL_PWR_GetRtcTime(HUINT32 *pulUnixTime);
extern HERROR PAL_PWR_SetWakeUpTime(HUINT32 ulUnixTime);
extern HERROR PAL_PWR_EnableWakeUpTime(void);
extern HERROR PAL_PWR_DisableWakeUpTime(void);
extern HERROR PAL_PWR_Deactivate_SC(void);
extern HERROR PAL_PWR_WIFIPower(HBOOL bOnOff);
extern HERROR PAL_PWR_MountNandFlash(void);
extern HERROR PAL_PWR_UmountNandFlash(void);
extern HERROR PAL_PWR_Reboot(void);
extern HERROR PAL_PWR_SetWakeupRcuKey(WakeUpRcuKeyGroup keyGroup);
extern HERROR PAL_PWR_ClearWakeupRcuKey(void);
extern HERROR PAL_PWR_SetWakeOnLAN(HBOOL bOn_Off);

extern HERROR PAL_PWR_PrepareFastBootStandby(void);
extern HERROR PAL_PWR_SetFastBootStandby(HINT32 nSleepMode,HINT32 nBootMode,HBOOL bPowerSavingStandby);
extern HERROR PAL_PWR_CancelFastBootStandby(void);

#endif /* !__PAL_POWER_H__ */

