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
 * @file	  		nx_prism_recordings.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#ifndef __NX_PRISM_RECORDINGS_H__
#define __NX_PRISM_RECORDINGS_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxRecGroup_PROGRAMME,
	eNxRecGroup_CHANNEL,
	eNxRecGroup_DOW,		// Day Of Week


} Nx_RecordGroupType_e;

typedef struct tagRecordItem
{
	HUINT32		ulContentId;
	HUINT32		ulSvcUid;
	HUINT32		ulRecStartTime;
	HBOOL		bIsExistThumbnail;					// thma �� ���� thumbnail �� �����ϴ���..?
	HCHAR		szName[DxRECLIST_NAME_LEN+1];
	HCHAR		szThumbnailPath[DxRECLIST_URL+1];
} Nx_RecordItem_t;

typedef struct tagRecGroupByChannel
{
	HUINT32		ulSvcUid;
	HCHAR		szChannelNo[5];
	HCHAR		szChannelName[64];
	HxVector_t	*recItemList;
} Nx_RecGroupByChannel_t;

typedef struct tagRecGroupByDOW		// Day Of Week
{
	HINT32		nDayOfWeek;

} Nx_RecGroupByDOW_t;

/*******************************************************************/
/********************	Private Functions		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Functions		********************/
/*******************************************************************/

#endif	//__NX_PRISM_RECORDINGS_H__

