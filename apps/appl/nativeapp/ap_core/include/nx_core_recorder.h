/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_core_recorder.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_RECORDER_H__
#define __NX_CORE_RECORDER_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <thapi.h>
#include <oapi.h>
#include <apk.h>
#include <clib.h>

#include <nx_common.h>
#include <nx_port.h>

#include <nx_core_channel.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define NX_RECORDER_DEFAULT_REC_DURATION	(2*60*60)		// 2hr

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxBufferingType_NONE			= 0,
	eNxBufferingType_TSR,
	eNxBufferingType_USR,
	eNxBufferingType_ENDOFCASE,
}NX_BufferingType_e;
/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulContentId;		// record cid
}NxRecItem_t;

typedef struct
{
	HxVector_t	*recItemList;
}NxRecMgrContext_t;


typedef struct
{
	HUINT32		ulSvcUid;
	HUINT32		ulSessionId;
	OxMediaRec_StartInfo_t	stStartInfo;
}NxRecInstItem_t;

typedef struct
{
	HxVector_t	*recInstanceList;
}NxRecorderInst_t;

HERROR NX_RECORDER_Init(void);
NxRecorderInst_t	*NX_RECORDER_GetRecordingInst(void);
HINT32	NX_RECORDER_GetRecordingInstCount(void);
NxRecInstItem_t	*NX_RECORDER_GetRecordingInstItem(HINT32 nIndex);
HBOOL NX_RECORDER_FindRecordingInstBySvcUid(HUINT32 ulSvcUid);
HERROR NX_RECORDER_StartRec(NX_Channel_t *pstChannel, HINT32 nStartOffset, HUINT32 ulDuration, OxMediaRec_TargetStorage_e eStorageType, HUINT8 *szStorageId);
HERROR NX_RECORDER_RecordNow(NX_Channel_t *pstChannel, HUINT32 ulDurationSec);
HERROR NX_RECORDER_RecordNowAt(NX_Channel_t *pstChannel, HUINT32 ulDurationSec, HUINT32 ulTarget);
HERROR NX_RECORDER_StopRecording(HUINT32 ulSessionId);

#define	___________RELATED_RECORDING_ITEM_____________
HERROR	NX_RECORDER_ReloadRecItem(void);
HxVector_t *NX_RECORDER_GetRecItemList(void);
HERROR	NX_RECORDER_GetRecItems(HxVector_t **ddstList, HINT32 *nListNum);
DxRecListData_t* NX_RECORDER_GetByCID(const HUINT32 cid);
HCHAR	*NX_RECORDER_GetThumbnail(HCHAR *szUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL);


#endif // __NX_CORE_RECORDER_H__



