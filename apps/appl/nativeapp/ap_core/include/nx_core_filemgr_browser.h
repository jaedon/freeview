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

/******************************************************************************/
/**
 * @file	  		nx_core_filemgr_browser.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_FILEMGR_BROWSER_H__
#define __NX_CORE_FILEMGR_BROWSER_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <dapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NXFILE_FILTER_MOVIE		"3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|pg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb|trp"
#define	NXFILE_FILTER_PHOTO		"jpg|jpeg|bmp|gif|tif|png"
/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef HINT32 Nxfile_Ticket_t;	/* handle */

typedef struct
{
	Nxfile_Ticket_t		ticket;
	HCHAR				szCurPath[DLIB_FILEINFO_NAME_LEN];
	HxVector_t			*itemList;
}NxFileBrowserHandle_t;

typedef struct
{
	HxVector_t		*hBrowserList;
}NxFileBrowserContext_t;
/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR	NX_FILEMGR_BROWSER_Init(void);
NxFileBrowserHandle_t *	NX_FILEMGR_BROWSER_CreateHandle(void);
HERROR	NX_FILEMGR_BROWSER_DestroyHandle(NxFileBrowserHandle_t *handle);
HERROR	NX_FILEMGR_BROWSER_SetPath(NxFileBrowserHandle_t *handle, HCHAR *szPath);
HCHAR	*NX_FILEMGR_BROWSER_GetCurPath(NxFileBrowserHandle_t *handle);
HERROR	NX_FILEMGR_BROWSER_MoveToUp(NxFileBrowserHandle_t *handle);
HERROR	NX_FILEMGR_BROWSER_AddFolderPath(NxFileBrowserHandle_t *handle, HCHAR *pszFolderName);
HERROR	NX_FILEMGR_BROWSER_BrowseByOneDepth(NxFileBrowserHandle_t *handle, HCHAR *pszFilter);
HERROR	NX_FILEMGR_BROWSER_BrowseByFolderView(NxFileBrowserHandle_t *handle, HCHAR *pszFilter);

#endif // __NX_CORE_FILEMGR_BROWSER_H__

