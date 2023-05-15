/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#ifndef __APK_FILEMGR_H__
#define __APK_FILEMGR_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define AxFILEMGR_PATH_LEN 	512

typedef enum
{
	//0x00000000 ~ 0x0000FFFF 까지만 할당
	// Exclusive value
	eAxFILEMGR_MODE_FOLDERVIEW	= 0,
	eAxFILEMGR_MODE_ONEDEPTHVIEW = 1,
	eAxFILEMGR_MODE_KEYWORD = 2,		// keyword# field 기반 검색
	eAxFILEMGR_MODE_GROUP = 3,			// keyword# field 기반 검색, 같은 keyword에 대해서 한개만 올려준다.

} AxFileMgrBrowsingMode_e;

// 앞 2Byte에 할당 (0xFFFF0000)
#define	AxFILEMGR_BROWSE_DIR 	0x10000000
#define	AxFILEMGR_BROWSE_FILE	0x20000000
#define	AxFILEMGR_BROWSE_VIDEO	0x01000000 // subtype of AxFILEMGR_BROWSE_FILE.
#define	AxFILEMGR_BROWSE_MUSIC	0x02000000 // subtype of AxFILEMGR_BROWSE_FILE.
#define	AxFILEMGR_BROWSE_PHOTO	0x04000000 // subtype of AxFILEMGR_BROWSE_FILE.

#define AxFILEMGR_BROWSE_MASK_SUBTYPE	0x0F000000

typedef enum
{
	eAxFILEMGR_ERR_ONGOING		= -1,
	eAxFILEMGR_ERR_OK		= 0,
	eAxFILEMGR_ERR_ERR,
	eAxFILEMGR_ERR_NOT_ENOUGH_SPACE,
	eAxFILEMGR_ERR_CANCELED,
	eAxFILEMGR_ERR_INVALID_ARG,
	eAxFILEMGR_ERR_FAIL_READ,
	eAxFILEMGR_ERR_FAIL_WRITE,
	eAxFILEMGR_ERR_FAIL_STAT,
	eAxFILEMGR_ERR_4G_FAT,

	eAxFILEMGR_ERR_UNKNOWN,
	eAxFILEMGR_ERR_NONE
} AxFileMgrState_e;

typedef enum
{
	eAxFILEMGR_OP_COPY		= 0,
	eAxFILEMGR_OP_MOVE,
	eAxFILEMGR_OP_REMOVE
} AxFileMgrOperation_e;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	AxFileMgrState_e	eState;

	HINT32				nTotalOperationNum;
	HINT32				nDoneOperationNum;
} AxFileMgrCbInfo_t;

typedef void	(*APK_FILEMGR_Notifier_t)(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pstInfo);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	APK_FILEMGR_Init(void);
HERROR	APK_FILEMGR_BrowserFiles(const HCHAR *pszPath, const HCHAR *pszFilter, const HCHAR *pszKeyword, HUINT32 ulBrowsingMode, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnTicket);
HERROR	APK_FILEMGR_ScanDir(const HCHAR *pszSource, const HCHAR *pszExtFilter, HBOOL bRecursive, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_Remove(const HCHAR *pszSource, const HCHAR *pszExtFilter, HBOOL bRecursive, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_Copy(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HBOOL bDecryption, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_Move(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HBOOL bDecryption, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_MakeDir(const HCHAR *pszSource, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_RemoveDir(const HCHAR *pszSource, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_Cancel(HINT32 nRequestId, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId);
HERROR	APK_FILEMGR_GetSizeOfPath(const HCHAR *pszPath, const HCHAR *pszExtFilter, HUINT64 *pulSize );
HERROR	APK_FILEMGR_AddListener(HINT32 nRequestId, void *pvUser, APK_FILEMGR_Notifier_t fnNotifier);
HERROR	APK_FILEMGR_FreeResultList(HxList_t *pstResultList);

#endif
/* end of file */
