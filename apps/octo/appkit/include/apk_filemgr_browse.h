/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_browse.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	NCopyright (c) 2013 HUMAX Co., Ltd.				\n
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

#ifndef __APK_FILEMGR_BROWSE_H__
#define __APK_FILEMGR_BROWSE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define APK_BROWSE_NAME_LEN		128
#define APK_BROWSE_PATH_LEN	 	512

#define AxFILEMGR_BROWSE_OPT_DIRTYPE_IGNORE		0x00000001
#define AxFILEMGR_BROWSE_OPT_HIDDEN_IGNORE	0x00000010
#define AxFILEMGR_BROWSE_OPT_META_IGNORE		0x00000100
#define AxFILEMGR_BROWSE_OPT_THUMB_WAIT		0x00001000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eAPK_FILEMGR_BROWSE_EVT_OPEN = 0,
	eAPK_FILEMGR_BROWSE_EVT_CLOSE,
	eAPK_FILEMGR_BROWSE_EVT_GETITEMS,
	eAPK_FILEMGR_BROWSE_EVT_SCANITEMS,
	eAPK_FILEMGR_BROWSE_EVT_SORT,
	eAPK_FILEMGR_BROWSE_EVT_THUMB,
	eAPK_FILEMGR_BROWSE_EVT_ADD,
	eAPK_FILEMGR_BROWSE_EVT_REMOVE,
	eAPK_FILEMGR_BROWSE_EVT_UPDATE,
	eAPK_FILEMGR_BROWSE_EVT_UNKNOWN,
	eAPK_FILEMGR_BROWSE_EVT_MAX
} APK_FILEMGR_BROWSE_EVT_e;

typedef struct
{
	HINT32		nTotalNum;
	HINT32		nReqIdx;
	HINT32		nItemCnt;
	HINT32		nSort;
	HxList_t	*pstFileList;
} APK_FILEMGR_BROWSE_RESULT_t;  
typedef	void	(*APK_FILEMGR_BROWSE_Notifier_t)(HUINT32 nHandleId, HINT32 eEvent, HINT32 result, void *pvData);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR	APK_FILEMGR_BROWSE_RegisterCallback(APK_FILEMGR_BROWSE_Notifier_t fnNotifier);
HERROR	APK_FILEMGR_BROWSE_UnregisterCallback(APK_FILEMGR_BROWSE_Notifier_t fnNotifier);

HUINT32	APK_FILEMGR_BROWSE_Open( HCHAR *pszPath, HUINT32 nCategory, HCHAR *pszExtFilter, HUINT32 nOptFlags );
HERROR	APK_FILEMGR_BROWSE_Close( HUINT32 nHandleId );
HERROR	APK_FILEMGR_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIndex, HUINT32 nReqCount, HINT32 nOption );
HERROR	APK_FILEMGR_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCount, HINT32 nOption );
HERROR	APK_FILEMGR_BROWSE_Sort( HUINT32 nHandleId, HUINT32 nSort );

#endif
/* end of file */
