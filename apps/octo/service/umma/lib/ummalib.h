/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_int.h
	@brief

	Description:  									\n
	Module: SE Umma Internal Header                    \n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __UMMALIB_H__
#define __UMMALIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>
#include	<sys/stat.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define UMMALIB_HP_EVT_NUM	16

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum {
	eUMMALIB_HP_HDD=0,
	eUMMALIB_HP_USB,
	eUMMALIB_HP_USB_STOR,
	eUMMALIB_HP_USB_INPUT,
	eUMMALIB_HP_USB_WIRELESS,
	eUMMALIB_HP_WIRELESS = eUMMALIB_HP_USB_WIRELESS,
	eUMMALIB_HP_HID,	// Keyboard, mouse, joystick...
	eUMMALIB_HP_SD,
	eUMMALIB_HP_MIC,
	eUMMALIB_HP_MAX = UMMALIB_HP_EVT_NUM
} UMMALIB_HP_DEV_EVT_e;


typedef enum {
	eUMMALIB_HP_CONN=0,
	eUMMALIB_HP_DISCONN,
	eUMMALIB_HP_CONN_BY_PWR,
	eUMMALIB_HP_DISCONN_BY_PWR
}UMMALIB_HP_CONNECT_EVT_e;

typedef struct {
#if 1
	UMMALIB_HP_CONNECT_EVT_e eConnEvt;
	HINT32 nLen;
	HCHAR szDevpath[1024];
	HUINT32 ulIdvendor;
	HUINT32 ulIdproduct;
	HINT32 nPortnum;
	HINT32 nBusnum;
#endif

} UMMALIB_HP_Info_t;

typedef HERROR	(*pfUmmaLibHp_NotifyFUNC)(HUINT32 ulSlotIdx, HUINT32 eDevType, void *pvEventInfo);

HERROR UMMALIB_Hotplug_Init(void);
HERROR UMMALIB_Hotplug_RegisterCallback(pfUmmaLibHp_NotifyFUNC pfnNotify);

#endif
/* end of file */



