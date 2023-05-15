/**
	@file     xmgr_live.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding 을 사용하는 source file 입니다.
  * MS949 encodingを使用して source fileです。
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <mgr_live.h>
#include <mgr_action.h>
#include <svc_av.h>
#include <svc_si.h>
#include <svc_transcoder.h>
#include <svc_cas.h>

#include <mgr_svclist.h>
#include <xmgr_live.h>

/*
 * live stream이 시작되면 rec처럼 보여주기 위해, recoding info를 채워야 한다.
 * 그래서 struct를 사용하기 위해 include 하였다.
 */
#include <mgr_pvr_action.h>

#include "./local_include/_xmgr_live.h"
#include "./local_include/_xmgr_live_view.h"
#include "./local_include/_xmgr_live_lock.h"
#include "./local_include/_xmgr_live_comp.h"


#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___API_GLOBAL_FUNCTIONS___


#define	_____________UTIL_FUNC________________

HINT8 *xmgr_live_GetSvcListGroupStr(ChListUiGroup_t svcListGroup)
{
	HINT8	*pStr;

	switch (svcListGroup)
	{
		case eChListUiGroup_All:
			pStr = "Service List Group : ALL";
			break;

		case eChListUiGroup_TV :
			pStr = "Service List Group : TV";
			break;

		case eChListUiGroup_RADIO :
			pStr = "Service List Group : RADIO";
			break;

		case eChListUiGroup_HDTV :
			pStr = "Service List Group : HDTV";
			break;

		case eChListUiGroup_FAV_1 :
			pStr = "Service List Group : Favorite1";
			break;

		case eChListUiGroup_FAV_2 :
			pStr = "Service List Group : Favorite2";
			break;

		case eChListUiGroup_FAV_3 :
			pStr = "Service List Group : Favorite3";
			break;

		case eChListUiGroup_FAV_4 :
			pStr = "Service List Group : Favorite4";
			break;

		case eChListUiGroup_FAV_5 :
			pStr = "Service List Group : Favorite5";
			break;

		case eChListUiGroup_RECENT :
			pStr = "Service List Group : RECENT";
			break;

		case eChListUiGroup_CURRENT :
			pStr = "Service List Group : Current in zapping info.";
			break;

		case eChListUiGroup_CAS :
			pStr = "Service List Group : CAS";
			break;

		default :
			pStr = "Service List Group : UNKNOWN";
			break;
	}

	return pStr;
}

HERROR xmgr_live_CheckValidSvcListGroup(ChListUiGroup_t svcListGroup)
{
	return MGR_SVCLIST_CheckValidSvcListGroup(svcListGroup);
}

#if defined(CONFIG_APUI_FAV_ORDERING_NUMBER)
STATIC DbSvc_FavGroup_e		local_ConvertGroupToSvcGroup(ChListUiGroup_t eChlistUiGroup)
{
	switch(eChlistUiGroup)
	{
		case eChListUiGroup_FAV_1:	return eFavGroup_1;
		case eChListUiGroup_FAV_2:	return eFavGroup_2;
		case eChListUiGroup_FAV_3:	return eFavGroup_3;
		case eChListUiGroup_FAV_4:	return eFavGroup_4;
		case eChListUiGroup_FAV_5:	return eFavGroup_5;
		default:
			HxLOG_Critical("\n\n");
			return eFavGroup_All;
	}
}

HERROR xmgr_live_GetFavouriteCHNumber(ChListUiGroup_t eChlistUiGroup, Handle_t hSvc, HINT32 *pnFavChNum)
{
	HINT32				nOrderNum = 0;
	HUINT32				ulValue = 0;
	HERROR				hError = ERR_FAIL;
	DbSvc_FavGroup_e		eSvcGroup = eFavGroup_All;
	HBOOL				bCurGroupIsFavGroup = FALSE;

	if (eChListUiGroup_FAV_1 <= eChlistUiGroup && eChlistUiGroup <= eChListUiGroup_FAV_5)
	{
		bCurGroupIsFavGroup = TRUE;
	}
	else if (eChListUiGroup_FAVOURITE == eChlistUiGroup)
	{
		bCurGroupIsFavGroup = TRUE;
	}

	if (FALSE == bCurGroupIsFavGroup )
	{
		HxLOG_Print("\n Only Favourite Group is Support - Current Group : (%d) \n", eChlistUiGroup);
		return	ERR_FAIL;
	}

	// Check Favourite channel number order
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_FAV_CHNUM_ORDER, &ulValue, 0);
	if ( hError != ERR_OK )
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	HxLOG_Print("\n Favourite Channel number order value : (%d) - Current Group : (%d) \n", ulValue);
	if (ulValue != eFavChNum_On)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	eSvcGroup = local_ConvertGroupToSvcGroup(eChlistUiGroup);
	if (eSvcGroup == eFavGroup_All)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	hError = DB_SVC_GetFavOrderNumber(eSvcGroup, hSvc, &nOrderNum);
	if ( hError== ERR_OK )
	{
		*pnFavChNum = nOrderNum;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}
#endif


#define _____________DEBUG_FUNC_______________

HUINT32 xmgr_live_PrintLiveLockContext(void)
{
#if defined(CONFIG_DEBUG)
	XMGR_LiveLock_BASE_PrintContext();
#endif

	return 0;
}

HUINT32 xmgr_live_PrintWholeLiveContext(void)
{
#if defined(CONFIG_DEBUG)
//	OxMGR_LIVE_PrintContext();
	XMGR_LiveView_BASE_DBG_PrintContext();
	xmgr_live_PrintLiveLockContext();
#endif

	return 0;
}

/* EOF */
