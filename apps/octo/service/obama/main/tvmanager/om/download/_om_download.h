/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _om_download.h
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

#ifndef __INT_OM_DOWNLOAD_H__
#define __INT_OM_DOWNLOAD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define OxDNLD_CACHE_NUM						10

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32					 nRpcHandle;
	HBOOL					 bSubModuleInited;

	HxList_t				*pstInstList;		// OmDownload_Instance_t
	HINT32					 anInstNum[eDxDNLD_TYPE_EndOfCase];

	OxDownload_Cache_t		stCache[OxDNLD_CACHE_NUM]; // download 갯수. define 해야 함. oapi_download.c : oapiDnldContext_t 와 동기화 시켜야 함.
} omDownload_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern omDownload_Context_t *	om_download_GetContext (void);
extern HERROR					om_download_RemoveInstance (HUINT32 ulDownloadId);

// om_download_ipmedia.c
extern BUS_Result_t				proc_om_download_ipmedia (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR					om_download_ipmedia_Init (void);
extern HERROR					om_download_ipmedia_InitRpcFunctions (HINT32 nRpcHandle);
extern HERROR 					om_download_ipmedia_CbProbe (OmDownload_Instance_t *pstInst, HUINT32 ulDownloadId, DxDownload_Start_t *punStart);
extern HERROR					om_download_ipmedia_CbStart (OmDownload_Instance_t *pstInst, HUINT32 ulDownloadId, DxDownload_Start_t *punStart);
extern HERROR					om_download_ipmedia_CbStop (HUINT32 ulDownloadId);
extern HERROR					om_download_ipmedia_CbPause (HUINT32 ulDownloadId);
extern HERROR					om_download_ipmedia_CbResume (HUINT32 ulDownloadId);

void om_download_CbOnComponentVideoChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_VideoCompTable_t *pstVideoCompTable);
void om_download_CbOnComponentAudioChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_AudioCompTable_t *pstAudioCompTable);
void om_download_CbOnComponentSubtitleChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_SubtitleCompTable_t *pstSubtitleCompTable);
void om_download_CbOnComponentDataChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_DataCompTable_t *pstDataCompTable);
void om_download_CbOnComponentSelected (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex);
void om_download_CbOnAvailabilityStartTimeChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, HCHAR *pString);

#endif // __INT_OM_DOWNLOAD_H__
