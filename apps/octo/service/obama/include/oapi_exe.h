/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_exe.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

#ifndef __OAPI_EXE_H__
#define __OAPI_EXE_H__

#define RPC_OAPI_EXE_START			"oapi.exe.start"
#define RPC_OAPI_EXE_STOP			"oapi.exe.stop"
#define RPC_OAPI_EXE_SET			"oapi.exe.set"
#define RPC_OAPI_EXE_SETBIN			"oapi.exe.setbin"
#define RPC_OAPI_EXE_NOTI			"oapi.exe.noti"



#define OxEXE_PROC_BATCH 			"batch"
#if 0	// warning 제거 freesat 이 삭제 됨.
#define OxEXE_PROC_FREESATEPG 		"freesatEpg"
#endif
#define OxEXE_PROC_FACTORYDEFAULT	"factoryDefault"
#define OxEXE_PROC_IMEXPORTDB		"imexportDB"
#define OxEXE_PROC_EXPORTDB			"exportDB"
#define OxEXE_PROC_IMPORTDB			"importDB"
#define OxEXE_PROC_MHEG				"mheg"
#define OxEXE_PROC_MHEG_KEY			"mheg.key"
#define OxEXE_PROC_MHEG_DEEPLINK	"mheg.deeplink"
#define OxEXE_PROC_MHEG_OPCTRL		"mheg.opctrl"

#define OxEXE_PROC_MHEGIB_FILEREQ	"mhegib.filereq"
#define OxEXE_PROC_MHEGIB_DATEREQ	"mhegib.datereq"
#define OxEXE_PROC_MHEGIB_ABORTBYUSER		"mhegib.abortuser"
#define OxEXE_PROC_MHEGIB_ABORTBYSYSTEM		"mhegib.abortsystem"

#define OxEXE_PROC_LCNUPDATE				"lcnupdate"
#define OxEXE_PROC_STARTLCNUPDATE			"startlcnupdate"
#define OxEXE_PROC_STOPLCNUPDATE			"stoplcnupdate"
#define OxEXE_PROC_SAVELCNUPDATE			"savelcnupdate"
#define OxEXE_PROC_GETLCNUPDATEINFO		"getlcnupdateinfo"
#define OxEXE_PROC_REMOVELCNUPDATEINFO		"removelcnupdateinfo"

#define OxEXE_PROC_TVTVEPGREMOVEINFODIRECTORY		"tvtvepgremoveinfodirectory"
#define OxEXE_PROC_TVTVEPGINITDIRECTORY		"tvtvepginitdirectory"
#define OxEXE_PROC_LOADBASEEPG				"loadbaseepg"
#define OxEXE_PROC_LOADTVTVEPG				"loadtvtvepg"

#define	OxEXE_PROC_STARTSATRECORD			"startsatrecord"

#define OxEXE_PROC_TRDCONFLICT			"trdconflict"
#define OxEXE_PROC_GETTRDCONFLICTINFO		"gettrdconflictinfo"
#define OxEXE_PROC_SETTRDCONFLICTINFO		"settrdconflictinfo"

#define OxEXE_PROC_SETNCDNOTIFYDATA		"setncdnotifydata"
#define OxEXE_PROC_NCDINFO				"ncdinfo"
#define OxEXE_PROC_GETNCDINFO			"getncdinfo"


#define OxEXE_PROCNAME_LENGTH 		30

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR	OAPI_EXE_Start(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3);
HERROR	OAPI_EXE_Stop(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3);
HERROR	OAPI_EXE_Set(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3);
HERROR	OAPI_EXE_SetByBin(const HCHAR *szProcName, void *pvBin, HUINT32 ulSize);
HERROR	OAPI_EXE_Notify(const HCHAR *szProcName, HxRPC_Noti_t pfnCallback);

#endif // __OAPI_EXE_H__
