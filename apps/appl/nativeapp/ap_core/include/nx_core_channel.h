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
 * @file	  		nx_core_channel.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_CHANNEL_H__
#define __NX_CORE_CHANNEL_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>
#include <clib.h>

#include <nx_common.h>
#include <nx_port.h>

#include <nx_core_av.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_CORE_CHANNEL_SUPPORTED_FAV_NUM							CONFIG_APUI_SET_FAVOURITE_MAX_SLOT		// 지원되는 최대 favourite group num
#define	NX_CORE_CHANNEL_SUPPORTED_FAV_MAX_ITEM_NUM					CONFIG_APUI_SET_FAVOURITE_MAX_ITEM		// 지원되는 최대 favourite Item num
#define	NX_CORE_CHANNEL_MAKE_CCID_MAX_SIZE			128


typedef	enum
{
	eNxCh_State_Update = 0,
	eNxCh_State_Load,
	eNxCh_State_Max
} NX_CH_UpdateState_e;


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxChListGroup_ALL				= 0x0000,		// Not Used
	eNxChListGroup_TV				= 0x0001,		/**<< TV */
	eNxChListGroup_RADIO			= 0x0002,		/**<< Radio */
	eNxChListGroup_HDTV				= 0x0003,		/**<< HDTV */
	eNxChListGroup_FTA				= 0x0004,		/**<< FTA 그룹 */
	eNxChListGroup_CAS				= 0x0005,		/**<< CAS 그룹 */

	eNxChListGroup_ALPHABET			= 0x0006,		/** TV/RADIO ALL 채널을 보여준다. 사용자의 filter 변경으로 A, B, ..., Z, 0~9, Etc 로 변경*/

	eNxChListGroup_Favorite1		= 0x2000,
	eNxChListGroup_Favorite2		= 0x2001,
	eNxChListGroup_Favorite3		= 0x2002,
	eNxChListGroup_Favorite4		= 0x2003,
	eNxChListGroup_Favorite5		= 0x2004,

	eNxChListGroup_Satellite		= 0x4000,
	eNxChListGroup_MBC				= 0x3000,
	eNxChListGroup_News				= 0x3001,
	eNxChListGroup_Movie			= 0x3002,
	eNxChListGroup_Drama			= 0x3003,
	eNxChListGroup_Sports			= 0x3004,
	eNxChListGroup_Kids				= 0x3005,

	eNxChListGroup_AlphabetAll		= 0x1040,
	eNxChListGroup_AlphabetA		= 0x1041,
	eNxChListGroup_AlphabetB		= 0x1042,
	eNxChListGroup_AlphabetC		= 0x1043,
	eNxChListGroup_AlphabetD		= 0x1044,
	eNxChListGroup_AlphabetE		= 0x1045,
	eNxChListGroup_AlphabetF		= 0x1046,
	eNxChListGroup_AlphabetG		= 0x1047,
	eNxChListGroup_AlphabetH		= 0x1048,
	eNxChListGroup_AlphabetI		= 0x1049,
	eNxChListGroup_AlphabetJ		= 0x104A,
	eNxChListGroup_AlphabetK		= 0x104B,
	eNxChListGroup_AlphabetL		= 0x104C,
	eNxChListGroup_AlphabetM		= 0x104D,
	eNxChListGroup_AlphabetN		= 0x104E,
	eNxChListGroup_AlphabetO		= 0x104F,
	eNxChListGroup_AlphabetP		= 0x1050,
	eNxChListGroup_AlphabetQ		= 0x1051,
	eNxChListGroup_AlphabetR		= 0x1052,
	eNxChListGroup_AlphabetS		= 0x1053,
	eNxChListGroup_AlphabetT		= 0x1054,
	eNxChListGroup_AlphabetU		= 0x1055,
	eNxChListGroup_AlphabetV		= 0x1056,
	eNxChListGroup_AlphabetW		= 0x1057,
	eNxChListGroup_AlphabetX		= 0x1058,
	eNxChListGroup_AlphabetY		= 0x1059,
	eNxChListGroup_AlphabetZ		= 0x105A,
	eNxChListGroup_AlphabetDigit	= 0x105B,
	eNxChListGroup_AlphabetEtc		= 0x105C,

	eNxChListGroup_CURRENT			= -1	/* 현재의 채널 리스트 그룹으로 지정 */
}NX_ChListGroup_t;

typedef enum
{
	eNxFavGroupID_None		= 0,
	eNxFavGroupID_User		= 100,
}NX_FavGroupID_e;

typedef struct
{
	HUID		aUID;
	HxVector_t	*list;
}NX_FavouriteList_t;

typedef struct
{
	HUINT32		svcUid;
	HUINT32		lcn;
}NX_Channel_t;

typedef struct
{
	NX_Channel_t 	*pstChlist;
	HINT32			nNum;
}NX_GroupListData_t;

typedef struct
{
	HxVector_t	*list;	/// [ DxService_t * ]
	HxTREE_t	*map;	/// [ int uid, DxService_t * ]
	HINT32		lastVersion;
	HxCBUF_t	charBuf;
} NX_ChannelManager_t;

typedef enum
{
	eNxFindSvcDir_Current	= 1,
	eNxFindSvcDir_Next,
	eNxFindSvcDir_Prev,
	eNxFindSvcDir_First,
	eNxFindSvcDir_Last
} NX_FindSvcDir_e;

typedef struct
{
	NX_ChListGroup_t	eGroupType;
	HINT32				nSvcCnt;
} NX_GroupInfoList_t;

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/
typedef void *	NX_ChannelManagerHandle_t;

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR	NX_CHANNEL_Init(void);
void	NX_CHANNEL_LoadList(HBOOL forced);
HERROR	NX_CHANNEL_Update(HxVector_t *list);
HERROR	NX_CHANNEL_InitLastChannel(void);
HERROR	NX_CHANNEL_SaveLastChannel(HUINT32 svcUid);
DxService_t *NX_CHANNEL_GetLastService(void);
DxService_t *NX_CHANNEL_FindServiceBySvcUid(HUINT32 svcuid);
HERROR	NX_CHANNEL_GetLockState(OxMediaPlay_LiveLockState_e *peLockState);
HERROR	NX_CHANNEL_CheckPlayChangable(NX_Channel_t *pstChannel, HBOOL *pbChangable);
HERROR	NX_CHANNEL_SetChannel(NX_Channel_t *channel);
HERROR	NX_CHANNEL_StopChannel(void);
HERROR	NX_CHANNEL_SetFastChannelChange(NX_Channel_t *channel);
HERROR	NX_CHANNEL_UpdateService(DxService_t *pstService);
HERROR	NX_CHANNEL_RemoveService(DxService_t *pstService);
HERROR	NX_CHANNEL_RemoveChannelListAll(void);
HINT32	NX_CHANNEL_GetChanenlCountAll(void);
HERROR	NX_CHANNEL_GetServiceListBySatelliteName(HUINT8 *szSatName, HxVector_t *stResultList, HINT32 *lNum);
HERROR	NX_CHANNEL_DeleteServiceListBySatelliteName(HUINT8 *szSatName);
HERROR	NX_CHANNEL_GetLastServiceGroup(HINT32 *plLastSvcGroup);
HERROR	NX_CHANNEL_SetLastServiceGroup(HINT32 lLastSvcGroup);

HxList_t *	NX_CHANNEL_GROUP_SortNxChannelsByName(NX_Channel_t *dstChannelList, HINT32 nListNum);
HERROR	NX_CHANNEL_GROUP_SortNxChannelsByLcn(NX_Channel_t *dstChannelList, HINT32 nChNum);

HERROR	NX_CHANNEL_GROUP_GetIsGroupInfoList(HxVector_t	*dstList, HINT32 *nNum);
HERROR	NX_CHANNEL_GROUP_GetAvailableAlphabetGroupInfoList(HxVector_t	*dstList, HINT32 *nNum);
HINT32	NX_CHANNEL_GROUP_GetGroupCount(NX_ChListGroup_t eGroupType);
HUINT32	NX_CHANNEL_GROUP_GetExtraFilterByGroupType(NX_ChListGroup_t eChGroupType);
HERROR 	NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(NX_ChListGroup_t eChGroupType, HUINT32 svcUid, NX_Channel_t *retChannel);
DxService_t *NX_CHANNEL_GROUP_FindServiceInGroup(NX_ChListGroup_t eChGroupType, NX_FindSvcDir_e eFindDir, NX_Channel_t *pstCurrentChannel);
DxService_t *NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(NX_ChListGroup_t eChGroupType, NX_FindSvcDir_e eFindDir, HUINT32 ulSvcUid);
DxService_t *NX_CHANNEL_GROUP_FindAvailableServiceInGroupList(NX_ChListGroup_t eChGroupType, HUINT32 ulSvcUid, NX_ChListGroup_t *peFindChGroupType);
HBOOL	NX_CHANNEL_GROUP_GetGroupOfChannel(NX_ChListGroup_t *eRetGroup, NX_Channel_t *pstChannel);
HBOOL	NX_CHANNEL_GROUP_IsServiceInGroup(NX_ChListGroup_t eChGroupType, NX_Channel_t *pstChannel);
HBOOL	NX_CHANNEL_GROUP_IsFavouriteGroup(NX_ChListGroup_t eChGroupType);

HERROR	NX_CHANNEL_GROUP_MakeNxChannelListGroup(NX_Channel_t **ddstChannelList, HINT32 *nListNum, NX_ChListGroup_t eGroupType, HUINT32 ulExtraFilter);
HERROR	NX_CHANNEL_GROUP_MakeNxChannelListAll(NX_Channel_t **ddstChannelList, HINT32 *nListNum);
HERROR	NX_CHANNEL_GROUP_MakeNxChannelListByList(HxList_t * pServiceList, NX_Channel_t **ddstChannelList, HINT32 *nListNum);

HERROR	NX_CHANNEL_FAV_Initialize(void);
HERROR 	NX_CHANNEL_FAV_UpdateFavouriteGroup(NX_FavGroupID_e eFavID, HxVector_t *pstList);
HINT32	NX_CHANNEL_FAV_GetFavouriteNum(NX_FavGroupID_e eFavID);
HERROR	NX_CHANNEL_FAV_IsUpdateFavouriteName(NX_FavGroupID_e eFavID, HBOOL *pbIsUpdate);
HERROR	NX_CHANNEL_FAV_GetFavouriteName(NX_FavGroupID_e eFavID, HCHAR *pszFavName);
HERROR 	NX_CHANNEL_FAV_SetFavouriteName(NX_FavGroupID_e eFavID, HCHAR *pszName, HBOOL bUserUpdateFlag);
HERROR	NX_CHANNEL_FAV_LoadFavFromDB(NX_FavGroupID_e eFavID, NX_FavouriteList_t *pstFavList);
HERROR	NX_CHANNEL_FAV_AddFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstChannel);
HERROR	NX_CHANNEL_FAV_RemoveFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstChannel);
HERROR	NX_CHANNEL_FAV_FindChannelBySvcUidInFavGroup(NX_FavGroupID_e eFavID, HUINT32 svcUid, NX_Channel_t *retDstChannel);
DxService_t *NX_CHANNEL_FAV_FindServiceInFavGroup(NX_FavGroupID_e eFavID, NX_FindSvcDir_e eFindDir, NX_Channel_t *pstCurrentChannel);
DxService_t *NX_CHANNEL_FAV_FindServiceInFavGroupByServiceUid(NX_FavGroupID_e eFavID, NX_FindSvcDir_e eFindDir, HUINT32 ulSvcUid);
HBOOL	NX_CHANNEL_FAV_IsFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstChannel);
void	NX_CHANNEL_FAV_RemoveFavouriteGroup(NX_FavGroupID_e eFavID);
HERROR	NX_CHANNEL_FAV_MakeNxChannelListByFavGroup(NX_FavGroupID_e eFavID, NX_Channel_t **ddstChannelList, HINT32 *nListNum);
NX_FavGroupID_e		NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(NX_ChListGroup_t eChGroupType);
HERROR	NX_CHANNEL_FAV_RemoveServiceInAllFavouriteGroup(DxService_t *pstService);

HERROR	NX_CHANNEL_DCN_MakeNxChannelListByDCN(HUINT8 *pucDcn, NX_Channel_t **ddstChannelList, HINT32 *nListNum);
HERROR	NX_CHANNEL_DCN_MakeGroupNxChannelListByDCN(HUINT8 *pucDcn, NX_ChListGroup_t eChGroupType, NX_Channel_t **ddstChannelList, HINT32 *nListNum);

#endif // __NX_CORE_CHANNEL_H__

