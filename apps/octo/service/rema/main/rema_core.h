/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  re_init.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
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



#ifndef	__REMA_CORE_H__
#define	__REMA_CORE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	enum
{
	eREMA_TR069CwmpCpeOpen,
	eREMA_TR069CwmpCpeClose,
	eREMA_TR069CpeGetSSIDNumberOfEntries,
	eREMA_TR069CpeGetWifiSSIDEnable,
	eREMA_TR069_UNKNOWN
} REMA_TR069MSG_e;

typedef struct
{
	REMA_TR069MSG_e	eMsg;
	HUINT32		ulParam1;
	HCHAR		*pszProcessName;
} REMA_MSG_t;

#define	REMA_MSGQ_SIZE						64

typedef struct
{
	HBOOL	bCwmpStbOpen;
	HBOOL	bStartSession;			// 키 오류 및 중복 요청을 막기 위한 flag
	HBOOL	bStartSessionByApp;		// Session Open Flag
	HBOOL	bStopSessionByApp;		// Session Close Flag
	HBOOL	bConfigUploadSession;	// config upload 여러번 하는걸 막기 위함, NEC가 수정하면 삭제 해야함 (M Upload - Multiple 가능)
	HBOOL	bRebootForUpdate;
	HBOOL	bCwmpSessionStatus;
	HBOOL	bCpeNotifyUpdated;
	HINT32	nRetryCountSession;

	UNIXTIME ConfigFileGenTime;
	UNIXTIME ApplicationLogFileGenTime;
	UNIXTIME DocsisLogFileGenTime;
	UNIXTIME LinuxLogFileGenTime;

	HBOOL	bAutonomousEvent;
	HUINT32	ulSetupLaunchMode;

//Rema new Data
	HUINT32 		ulMainMsgQueue;
	HUINT32 		ulSemId;
}Rema_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HBOOL rema_core_Init(void);
void rema_core_Deinit(void);

void rema_core_cpe_open(Rema_t *rema);
void rema_core_cpe_close(Rema_t *rema);

/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/

#ifdef __cplusplus
}
#endif

#endif //__REMA_CORE_H__
