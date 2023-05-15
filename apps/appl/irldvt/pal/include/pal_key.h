/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_key.h
	@brief	  PAL key header

	Description: RCU and front key hander PAL 		\n
	Module: PAL/KEY			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_KEY_H__
#define	__PAL_KEY_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include "octo_common.h"
#include <hlib.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief DB event messages */

#if 0
typedef enum
{
	eKEY_CUSTOMCODE_1 = 0,
	eKEY_CUSTOMCODE_2 = 1,
	eKEY_CUSTOMCODE_3 = 2,
	eKEY_CUSTOMCODE_4 = 3,
	eKEY_CUSTOMCODE_5 = 4,
	eKEY_CUSTOMCODE_6 = 5,
	eKEY_CUSTOMCODE_MAX
} PalKey_CustomCode_t;

typedef void(*KEY_NotifyCallback_t)(HUINT32 ucKeyCode, KEY_PressMode_e pressMode, KEY_Device_t device);
typedef void (*KEY_PostEvtToMwCallback_t)(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark   PAL/KEY 초기화 API.
  @remark	type : Inline

  @return	NONE
*/
extern void		PAL_KEY_Init(void);

extern HERROR	PAL_KEY_GetCurrentCustomCode(HUINT16 *pusCustomCode);

#if 0
/**
  @remark   Key callback 등록.
  @remark	type : Inline

  @return	NONE
*/
extern HERROR PAL_KEY_RegisterKeyCallback(
				KEY_NotifyCallback_t pfnKeyNotifyCallback	/**< [in] Callback function pointer */
				);

/**
  @remark   Key 입력을 disable하는 API
  @remark	type : Inline

  @return	NONE
*/
extern void PAL_KEY_DisableKeyInput(void);

/**
  @remark   Key 입력을 enable하는 API
  @remark	type : Inline

  @return	NONE
*/
extern void PAL_KEY_EnableKeyInput(void);

extern KEY_NotifyCallback_t PAL_KEY_GetRegisterKeyCallback(void);

extern HUINT8 *PAL_KEY_GetKeyString(HUINT32 ulKeyCode);

#if defined(CONFIG_PROD_RCU_MULTI_CUSTOM_CODE) || defined(CONFIG_MW_SYS_HOTKEY)
extern HERROR	PAL_KEY_SetNotifyCallback(KEY_PostEvtToMwCallback_t pfncCallback);
#endif

extern HBOOL PAL_KEY_IsWakeUpKey(HUINT32 ulKeyCode);
extern void PAL_KEY_FrontLocked(HBOOL bIsLocked);
#endif

#endif /* !__PAL_KEY_H__ */

