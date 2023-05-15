/**************************************************************************************/
/**
 * @file oplppvhistory.c
 *
 * Defines the ppvhistory structure.
 *
 * @author  LEE SUNG HOON(shlee3@humaxdigital.com)
 * @date    2012/10/11
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplppvhistory.h"

#include "oapi.h"
#include "apk.h"
#include "oplevent.h"
#include <hlib.h>

//#define	_DLOG					printf
#define	_DLOG					HxLOG_Debug
#define	ASSERT(expr)			HxLOG_Assert(expr)
#define	TRACE()					do{_DLOG("OPLFAV::TRACE [%s:%d]\n", __FUNCTION__, __LINE__);}while(0)
#define	RETURN_IF_VOID(expr)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

typedef struct
{
	HxVector_t	*list;		/// [ DxPPVHistory_t * ]
} PPVHistoryManager_t;

static OPL_UploadPPVViewlogDataListener_t s_onUploadPPVViewlogData = NULL;

static void 	opl_channel_UploadPPVViewlogDataListener (HUINT32 event, HUINT32 arg1, HUINT32 arg2);

static HERROR opl_ppvhistory_LoadList (PPVHistoryManager_t *manager)
{
	HxVector_t		*list;
	HERROR			hErr = ERR_FAIL;

	if (manager && manager->list)
		return ERR_OK;

	HxLOG_Trace();

	hErr = APK_METAPPVHISTORY_LoadFromOcto();
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[%s,%d]Load ppvhistory data\n", __FUNCTION__, __LINE__);
		return hErr;
	}

	list = APK_METAPPVHISTORY_GetPPVHistoryList();
	if (list == NULL)
	{
		HxLOG_Error("[%s,%d]Cannot get ppvhistory list from appkit\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if (manager->list)
		HLIB_VECTOR_Delete(manager->list);

	manager->list = list;
	return ERR_OK;
}

static void opl_ppvhistory_ReleaseList (PPVHistoryManager_t *manager)
{
	if( manager )
	{
		if (manager->list)
			HLIB_VECTOR_Delete(manager->list);
	}
	else
	{
		HxLOG_Error("[%s,%d]Argumetn ( manager) is NULL \n", __FUNCTION__, __LINE__);
	}
}

static void 	opl_channel_NotifyUploadPPVViewlogDataResult(HUINT32 P1, HUINT32 P2, HUINT32 P3)
{
	HINT32 result = -1;
	if (s_onUploadPPVViewlogData)
	{
		switch(P1)
		{
			case eSysCtrl_Event_UserReqCallFail:
				result = -1;
				break;
			case eSysCtrl_Event_UserReqCallOK:
				result = 0;
				break;
			case eSysCtrl_Event_UserReqCallCanceled:
				result = 1;
				break;
			default:
				return;
		}
		s_onUploadPPVViewlogData(result);
		APK_SQCSCENARIOJAPAN_UnRegisterNotifier(opl_channel_UploadPPVViewlogDataListener);
		s_onUploadPPVViewlogData = NULL;
	}
}

static void opl_channel_SyncedUploadPPVViewlogDataListener (int argc, void **argv)
{
	HxLOG_Assert(argc == 3);
	opl_channel_NotifyUploadPPVViewlogDataResult((HUINT32)argv[0], (HUINT32)argv[1], (HUINT32)argv[2]);
}

static void 	opl_channel_UploadPPVViewlogDataListener (HUINT32 event, HUINT32 arg1, HUINT32 arg2)
{
	if((event != eSysCtrl_Event_UserReqCallFail)
	&& (event != eSysCtrl_Event_UserReqCallOK)
	&& (event != eSysCtrl_Event_UserReqCallCanceled))
	{
		return;
	}
	opl_channel_NotifyUploadPPVViewlogDataResult(event, arg1, arg2);
}

void			OPL_PPVHistory_Delete (OPL_PPVHistoryHandle ppvhistory)
{
	APK_METAPPVHISTORY_Delete((void*)ppvhistory);
}

HERROR		OPL_PPVHistory_Remove( void  )
{
	return APK_METAPPVHISTORY_Remove( );
}

HERROR		OPL_PPVHistory_CancelPPVViewlogDataUpload( HINT32 * pnCancelResult  )
{
	return APK_SQCSCENARIOJAPAN_CancelPPVViewlogDataUpload(pnCancelResult );
}

void		OPL_PPVHistory_SetUploadPPVViewlogDataListener( OPL_UploadPPVViewlogDataListener_t listener)
{
	s_onUploadPPVViewlogData = listener;
	APK_SQCSCENARIOJAPAN_RegisterNotifier(opl_channel_UploadPPVViewlogDataListener);
}

HERROR		OPL_PPVHistory_UploadPPVViewlogData(HINT32 * pnUploadResult)
{
	return APK_SQCSCENARIOJAPAN_UploadPPVViewlogData(pnUploadResult);
}

HINT32		OPL_PPVHistory_GetPPVHistoryList ( OPL_PPVHistoryHandle **ppvhistoryList)
{
	DxPPVHistory_t			*ppvhistory;
	OPL_PPVHistoryHandle	*list;
	HINT32				i =0, n =0, count =0;
	PPVHistoryManager_t		manager;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();
	HxLOG_Assert(ppvhistoryList);

	memset( &manager, 0x00, sizeof( PPVHistoryManager_t ));
	hErr = opl_ppvhistory_LoadList( &manager );
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[%s,%d]load ppvhistory list error!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (manager.list == NULL
		|| (n = HLIB_VECTOR_Length(manager.list)) == 0)
	{
		HxLOG_Error("[%s,%d]ppvhistory manager does not initialized!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	HxLOG_Print("[%s,%d] count : %d \n", __FUNCTION__ , __LINE__,n );

	list = (OPL_PPVHistoryHandle *)HLIB_STD_MemAlloc(sizeof(DxPPVHistory_t*) * n);
	if (list == NULL)
	{
		HxLOG_Error("[%s,%d]Out of memory!!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	for (i = 0, count = 0 ; i < n ; i++)
	{
		ppvhistory = (DxPPVHistory_t *)HLIB_VECTOR_ItemAt(manager.list, i);
		if (ppvhistory == NULL)
			continue;


		HxLOG_Print("[%s,%d] nId : %d \n", __FUNCTION__ , __LINE__, ppvhistory->nId);

		list[count] = (OPL_PPVHistoryHandle)APK_METAPPVHISTORY_Clone(ppvhistory);
		if (list[count])
			count++;
	}

	if (count == 0)
	{
		HLIB_STD_MemFree(list);
		list = NULL;
	}

	opl_ppvhistory_ReleaseList( &manager );

	*ppvhistoryList = list;

	HxLOG_Print("[%s,%d] count : %d \n", __FUNCTION__ , __LINE__ , count );
	return count;
}

HINT32		OPL_PPVHistory_GetId ( OPL_PPVHistoryHandle self )
{
	DxPPVHistory_t		*thiz = (DxPPVHistory_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, -1);

	return thiz->nId;
}

HINT32		OPL_PPVHistory_GetPurchaseDate ( OPL_PPVHistoryHandle self )
{
	DxPPVHistory_t		*thiz = (DxPPVHistory_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, -1);

	return (HINT32)thiz->purchaseTime;
}

HUINT32		OPL_PPVHistory_GetPPVFee ( OPL_PPVHistoryHandle self )
{
	DxPPVHistory_t		*thiz = (DxPPVHistory_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, -1);

	return thiz->ppvFee;

}

HERROR		OPL_PPVHistory_GetSystemId ( OPL_PPVHistoryHandle self, HCHAR *pBuf, HUINT32 bufLen )
{
	DxPPVHistory_t		*thiz = (DxPPVHistory_t *)self;
	HCHAR *pCaSystemId = NULL;
	HERROR hErr = ERR_FAIL;
	HxLOG_Trace();
	RETURN_IF(!self, ERR_FAIL);
	hErr = APK_CAS_BC_MakeDrmSystemIdStr(&pCaSystemId, thiz->caSystemId);
	if((hErr != ERR_OK) || (pCaSystemId == NULL))
		return ERR_FAIL;
	HxSTD_StrNCpy(pBuf, pCaSystemId, bufLen);
	HLIB_STD_MemFree(pCaSystemId);
	return ERR_OK;
}

HERROR		OPL_PPVHistory_GetEventData ( OPL_PPVHistoryHandle self, HCHAR *pBuf, HUINT32 bufLen )
{
	HxLOG_Trace();
	RETURN_IF(!self, ERR_FAIL);
	return APK_METAPPVHISTORY_GetEventData(self, pBuf, bufLen);
}


