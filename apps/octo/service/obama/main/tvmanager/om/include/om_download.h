/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_download.h
	@brief

	Description:  									\n
	Module: TV MANAGER / OAPI					\n

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

#ifndef __MGR_OAPI_DOWNLOAD_H__
#define __MGR_OAPI_DOWNLOAD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL					bDlna;
	HBOOL					bAirplaying;
	HFLOAT32				fAirPlayingStartPercentage;
	HUINT64					ullBasePosition;


	HUINT8					*pCsoSettings; // SEAMLESS_SWITCHING
} OmDownload_IpMediaInfo_t;


typedef struct
{
	HBOOL					 	bAllocated;
	HBOOL						bDownloadAfterVerify;
	HUINT32					 	ulCsoSessionId;
	void						*pSession;
	OmDownload_IpMediaInfo_t	stMediaInfo;
	HBOOL						bDownloadFileCreated;
} OmDownload_IpMedia_t;

typedef struct
{
	DxDownload_Type_e		 eType;
	HUINT32					 ulDownloadId;
	DxDownload_Start_t		 unStartInfo;

	union
	{
		OmDownload_IpMedia_t	 stIpMedia;
	} unExtInfo;

	struct { 	
		OxMediaPlay_ErrorReason_e		eReason;
	} stError;

} OmDownload_Instance_t;


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
extern HERROR					OM_DOWNLOAD_Init (void);
extern OmDownload_Instance_t *	OM_DOWNLOAD_GetInstance (HUINT32 ulDownloadId);
extern OmDownload_Instance_t *	OM_DOWNLOAD_GetInstanceWithCsoId (HUINT32 ulCalypsoId);
extern OmDownload_IpMediaInfo_t* OM_DOWNLOAD_GetMediaInfoByCsoId (HUINT32 ulCalypsoId);
#endif // __MGR_OAPI_DOWNLOAD_H__
