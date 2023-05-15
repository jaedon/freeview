/**
	@file     xmgr_svclist_arib.c
	@brief

	Description :				\n
	Module : 					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>

#include <isosvc.h>

#include <mgr_svclist.h>
//#include <xmgr_svclist.h>

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
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

void xmgr_svclist_InitSvcAttribute_Base (DbSvc_Attr_t * pstAttr)
{
	if(pstAttr == NULL)
		return;

	// DbSvc_Attr_t 는 항상, DB_SVC_InitSvcAttribute()를 가장 먼저 호출한다
	DB_SVC_InitSvcAttribute(pstAttr);

#if defined(CONFIG_DB_SVC_INVISIBLE_SKIP)
	{
		HERROR	hErr;
		HBOOL	bInvisibleSearch = FALSE;

//		bInvisibleSearch = MWC_PARAM_GetInvisibleSearch();
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_INVISIBLE_SEARCH, (HUINT32 *)&bInvisibleSearch, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_INVISIBLE_SEARCH) Error!!!\n");
		}
		if(bInvisibleSearch == TRUE)
		{
			pstAttr->eVisibleType = eLcnVisibleSvc_FindAll;
		}
	}
#endif  // TODO: 여기에 다른 사양을 추가할 때는 #elif defined(CONFIG_XXX) 와 같이 하세요
}


HERROR xmgr_svclist_GetSvcDbParamsFromGroup_Base(ChListUiGroup_t eSvcListGroup, DbSvc_Group_e *peSvcGroup, DbSvc_Attr_t *pstAttr)
{
	/* Attribute 초기화 */
	MGR_SVCLIST_InitSvcAttribute (pstAttr);

	switch (eSvcListGroup)
	{
	case eChListUiGroup_TV :
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_TV;
		break;

	case eChListUiGroup_RADIO :
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_RADIO;
		break;

	case eChListUiGroup_HDTV :
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_TV;
		pstAttr->eVideoType = eDxVIDEO_TYPE_HD;
		break;

	case eChListUiGroup_FAV_1 :
		*peSvcGroup = eSvcGroup_Fav_1;
		break;

	case eChListUiGroup_FAV_2 :
		*peSvcGroup = eSvcGroup_Fav_2;
		break;

	case eChListUiGroup_FAV_3 :
		*peSvcGroup = eSvcGroup_Fav_3;
		break;

	case eChListUiGroup_FAV_4 :
		*peSvcGroup = eSvcGroup_Fav_4;
		break;

	case eChListUiGroup_FAV_5 :
		*peSvcGroup = eSvcGroup_Fav_5;
		break;

	case eChListUiGroup_ProviderName :
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_All;
		pstAttr->eNotSvcOpType = eDxOPERATOR_NOT_DEFINED;
		break;

	case eChListUiGroup_RECENT :
		*peSvcGroup = eSvcGroup_Recent;
		break;

	case eChListUiGroup_CAS :
		{
			DxCasType_e		bwCasType;
			HERROR			hErr;

			*peSvcGroup = eSvcGroup_All;
			hErr = DB_SVC_GetAllCasType(eDxSVC_TYPE_All, eDxVIDEO_TYPE_ALL, &bwCasType);
			pstAttr->eCasType = (hErr == ERR_OK) ? bwCasType :
												(eDxCAS_TYPE_FTA
												| eDxCAS_TYPE_NAGRA
												| eDxCAS_TYPE_IRDETO
												| eDxCAS_TYPE_NDS
												| eDxCAS_TYPE_VIACCESS
												| eDxCAS_TYPE_CONAX
												| eDxCAS_TYPE_MEDIAGUARD
												| eDxCAS_TYPE_CRYPTOWORKS);
		}
		break;

	case eChListUiGroup_TRASH:
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_TV | eDxSVC_TYPE_RADIO;
//		pstAttr->eUserFlag1 |= eSvcUserFlag_03_SvcDeletedByUser;
//		pstAttr->eUserMask1 |= eSvcUserFlag_03_SvcDeletedByUser;
		DbSvc_SetUser1Flag(pstAttr->eUserFlag1, eSvcUserFlag_03_SvcDeletedByUser, TRUE);
		DbSvc_SetUser1Flag(pstAttr->eUserMask1, eSvcUserFlag_03_SvcDeletedByUser, TRUE);
		break;

	case eChListUiGroup_All: /* $주의 : TV|RADIO 로만 하고 data, suplementary등의 포함되지 않도록 한다. */
		*peSvcGroup = eSvcGroup_All;
		pstAttr->eSvcType = eDxSVC_TYPE_TV | eDxSVC_TYPE_RADIO;
		break;

	default :
		*peSvcGroup = eSvcGroup_All;
		break;
	}
	return ERR_OK;
}

HERROR xmgr_svclist_CheckValidGroup_Base(ChListUiGroup_t eSvcListGroup)
{
	switch(eSvcListGroup)
	{
	case eChListUiGroup_All :			return ERR_OK;
	case eChListUiGroup_TV :			return ERR_OK;
	case eChListUiGroup_RADIO :			return ERR_OK;
	case eChListUiGroup_HDTV :			return ERR_OK;
	case eChListUiGroup_FAV_1 :			return ERR_OK;
	case eChListUiGroup_FAV_2 :			return ERR_OK;
	case eChListUiGroup_FAV_3 :			return ERR_OK;
	case eChListUiGroup_FAV_4 :			return ERR_OK;
	case eChListUiGroup_FAV_5 :			return ERR_OK;
	case eChListUiGroup_RECENT :		return ERR_OK;
	case eChListUiGroup_CAS :			return ERR_OK;
	default :							return ERR_FAIL;
	}
}

