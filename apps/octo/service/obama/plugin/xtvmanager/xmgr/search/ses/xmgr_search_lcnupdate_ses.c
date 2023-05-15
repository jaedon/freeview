/**
	@file     xmgr_search_lcnupdate_ses.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <dlib.h>
#include <octo_common.h>

#include <mgr_action.h>
#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_svc.h>
#include <xdb_svc.h>
#include <db_param.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_epg.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <xmgr_search.h>
#include <xmgr_module_search.h>
#include "../local_include/_xmgr_search.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	LOG							HxLOG_Print
#define	ERR_LOG						HxLOG_Error
#define	MSG_LOG						HxLOG_Message
#else
#define	LOG							while (0) ((int (*)(char *, ...)) 0)
#define	ERR_LOG						while (0) ((int (*)(char *, ...)) 0)
#define MSG_LOG						while (0) ((int (*)(char *, ...)) 0)
#endif

typedef struct tagXmgrSearch_lcnupdate_SatContents_t
{
	HUINT32		ulSearchType;
} XmgrSearch_lcnupdate_Contents_t;

static XmgrSearch_lcnupdate_Contents_t		s_stSesLcn_Contents;

#define	_________________________Body___________________________________

// Local Functions
static XmgrSearch_lcnupdate_Contents_t *xmgr_search_lcnupdate_GetContents (void);
static HERROR		xmgr_search_lcnupdate_InitContents (XmgrSearch_lcnupdate_Contents_t *pstContents, HUINT32 ulSearchType);
static HERROR		xmgr_search_lcnupdate_MsgBusCreate(XmgrSearch_lcnupdate_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR		xmgr_search_lcnupdate_MsgBusDestroyed(XmgrSearch_lcnupdate_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR		xmgr_search_lcnupdate_MsgLcnUpdateStart (XmgrSearch_lcnupdate_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_MsgLcnUpdateSave(XmgrSearch_lcnupdate_Contents_t *pstContents, HUINT32 ulSearchType, HUINT32 ulSelectedIdx);
static HERROR		xmgr_search_lcnupdate_MsgLcnUpdateStop (XmgrSearch_lcnupdate_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_MsgSearchStarted (XmgrSearch_lcnupdate_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_MsgSearchFinished (XmgrSearch_lcnupdate_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_MsgSearchSaveFinished (XmgrSearch_lcnupdate_Contents_t *pstContents);
static HERROR		xmgr_search_lcnupdate_MakeSvcUid(HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HINT32 *ulSvcUid);

#define _________________________________________________________________________________

static XmgrSearch_lcnupdate_Contents_t *xmgr_search_lcnupdate_GetContents (void)
{
	return &s_stSesLcn_Contents;
}

static HERROR xmgr_search_lcnupdate_InitContents (XmgrSearch_lcnupdate_Contents_t *pstContents, HUINT32 ulSearchType)
{
	// Clear all the APS related information
	XMGR_SEARCH_INFO_InitNetworkInfo();
	XMGR_SEARCH_INFO_InitDeliveryTsInfo();
	XMGR_SEARCH_INFO_InitAstraReferenceTsInfo();
	XMGR_SEARCH_INFO_InitAstraSgtTsInfo();
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
	XMGR_SEARCH_INFO_InitAstraPortalReferenceTsInfo();
#endif

	pstContents->ulSearchType = ulSearchType;

	return ERR_OK;
}

// eMEVT_BUS_CREATE
static HERROR xmgr_search_lcnupdate_MsgBusCreate(XmgrSearch_lcnupdate_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr;

	hErr = xmgr_search_lcnupdate_InitContents(pstContents, (HUINT32)p1);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy(NULL);
		return MESSAGE_BREAK;
	}
	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY
static HERROR xmgr_search_lcnupdate_MsgBusDestroyed(XmgrSearch_lcnupdate_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return (HERROR)BUS_PostMessageToOM(NULL, eMEVT_LCNUPDATE_STOP_OK, HANDLE_NULL, 0, 0, 0);
}

static HERROR xmgr_search_lcnupdate_MsgLcnUpdateStart (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_SEARCH_StartLcnUpdate(pstContents->ulSearchType);

	return hErr;
}

static HERROR xmgr_search_lcnupdate_MsgLcnUpdateSave(XmgrSearch_lcnupdate_Contents_t *pstContents, HUINT32 ulSearchType, HUINT32 ulSelectedIdx)
{
	HUINT8			 ucSvcListVer, ucPrevSvcListVer;
	HUINT16			 usSvcListId, usPrevSvcListId;
	HUINT32			ulListIdx, ulListNum;
	HBOOL			bSvcListUpdated = FALSE;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if (BUS_MGR_Get(MGR_SEARCH_GetLcnUpdateSearchAction()) != NULL)
	{
		switch(ulSearchType)
		{
			case 0:		// Update
				hErr = XMGR_SEARCH_INFO_GetSesLcnServiceListIdFromDb (&usPrevSvcListId);
				if (hErr != ERR_OK)					{ usPrevSvcListId = 0; }

				hErr = XMGR_SEARCH_INFO_GetSesLcnServiceListVerFromDb (&ucPrevSvcListVer);
				if (hErr != ERR_OK)					{ ucPrevSvcListVer = 0; }

				hErr = XMGR_SEARCH_LCNUPDATE_CountLcnSvcList(&ulListNum);
				if(hErr == ERR_OK && ulListNum > 0)
				{
					for (ulListIdx = 0; ulListIdx < ulListNum; ulListIdx++)
					{
						hErr = XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo (ulListIdx, &usSvcListId , &ucSvcListVer, NULL);
						if (hErr == ERR_OK)
						{
							if (usSvcListId == usPrevSvcListId)
							{
								if (ucSvcListVer != ucPrevSvcListVer)
								{
									// Found the new version : Update
									//   --> eMEVT_LCNUPDATE_SAVE_OK message will be sent with save finished
									bSvcListUpdated = TRUE;
									BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchAction(), eMEVT_SEARCH_SAVE_RESULT, HANDLE_NULL, usSvcListId, 0, 0);
								}
							}
						}
					}
				}

				if(bSvcListUpdated == FALSE)
				{
					BUS_PostMessage(NULL, eMEVT_LCNUPDATE_SAVE_FAIL, HANDLE_NULL, 0, 0, 0);
				}
				break;

			case 1:		// Change
				hErr = XMGR_SEARCH_LCNUPDATE_CountLcnSvcList (&ulListNum);
				if (hErr == ERR_OK && ulListNum > 0)
				{
					hErr = XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo (ulSelectedIdx, &usSvcListId, NULL, NULL);
					if(hErr == ERR_OK)
					{
						// Change the current lcn service list : Change
						//   --> eMEVT_LCNUPDATE_SAVE_OK message will be sent with save finished
						bSvcListUpdated = TRUE;
						BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchAction(), eMEVT_SEARCH_SAVE_RESULT, HANDLE_NULL, usSvcListId, 0, 0);
					}
				}

				if(bSvcListUpdated == FALSE)
				{
					BUS_PostMessage(NULL, eMEVT_LCNUPDATE_SAVE_FAIL, HANDLE_NULL, 0, 0, 0);
				}
				break;

			default:
				BUS_PostMessage(NULL, eMEVT_LCNUPDATE_SAVE_FAIL, HANDLE_NULL, 0, 0, 0);
				break;
		}
	}

	return ERR_OK;
}

//
static HERROR xmgr_search_lcnupdate_MsgLcnUpdateStop (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_SEARCH_StopLcnUpdate(eDESTROY_BUS_MYSELF);

	return hErr;
}

// eMEVT_SEARCH_STARTED
static HERROR xmgr_search_lcnupdate_MsgSearchStarted (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	// Do nothing.
	return ERR_OK;
}

// eMEVT_SEARCH_FINISHED
static HERROR xmgr_search_lcnupdate_MsgSearchFinished (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	HERROR						hErr = ERR_FAIL;
	HUINT32						ulSvcListNum = 0;
	DxLcnUpdateStartData_t		*pstStartData = NULL;
	int							i;

	hErr = XMGR_SEARCH_LCNUPDATE_CountLcnSvcList (&ulSvcListNum);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[XMGR_SEARCH_LCNUPDATE_CountLcnSvcList] return fail!!\n");
		ulSvcListNum = 0;
	}

	pstStartData = (DxLcnUpdateStartData_t*)OxAP_Malloc(sizeof(DxLcnUpdateStartData_t));
	HxSTD_MemSet(pstStartData, 0x00, sizeof(DxLcnUpdateStartData_t));

	pstStartData->ulSvcListNum = ulSvcListNum;

	pstStartData->svcList = (DxLcnUpdateServiceList_t*)OxAP_Malloc(sizeof(DxLcnUpdateServiceList_t) * ulSvcListNum);
	HxSTD_MemSet(pstStartData->svcList, 0x00, sizeof(DxLcnUpdateServiceList_t) * ulSvcListNum);

	for(i = 0; i < ulSvcListNum; i++)
	{
		XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo(i, NULL, NULL, pstStartData->svcList[i].szSvcListName);
		HxLOG_Debug("Added [%d]  lcn svc list name : %s \n", i, pstStartData->svcList[i].szSvcListName);
	}

	return BUS_PostMessageToOM(NULL, eMEVT_LCNUPDATE_START_OK, HANDLE_NULL, (HINT32)pstStartData, 0, 0);
}

// eMEVT_SEARCH_SAVE_FINISHED
static HERROR xmgr_search_lcnupdate_MsgSearchSaveFinished (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	HERROR						hErr = ERR_FAIL;
	DxLcnUpdateSaveData_t		*pstSaveData = NULL;
	HUINT32						ulNumOfNewSvc = 0, ulNumOfReplacedSvc = 0;
	Handle_t					*pNewSvcHandle = NULL, *pReplacedSvcHandle = NULL;
	DbSvc_Info_t				*astNewSvcInfo = NULL, *astReplacedSvcInfo = NULL;
	int							i;

	if(ERR_OK != DB_PARAM_GetItem(eDB_PARAM_ITEM_NEWSVC_NUM, (HUINT32*)&ulNumOfNewSvc, 0))
		ulNumOfNewSvc = 0;
	HxLOG_Error("\t\t[%s:%d], num of new svc : %d\n", __FUNCTION__, __LINE__, ulNumOfNewSvc);

	if(ERR_OK != DB_PARAM_GetItem(eDB_PARAM_ITEM_REPLACESVC_NUM, (HUINT32*)&ulNumOfReplacedSvc, 0))
		ulNumOfReplacedSvc = 0;
	HxLOG_Error("\t\t[%s:%d], num of replaced svc : %d\n", __FUNCTION__, __LINE__, ulNumOfReplacedSvc);

	pstSaveData = (DxLcnUpdateSaveData_t*)OxAP_Malloc(sizeof(DxLcnUpdateSaveData_t));
	HxSTD_MemSet(pstSaveData, 0x00, sizeof(DxLcnUpdateSaveData_t));

	pstSaveData->ulNewSvcNum = ulNumOfNewSvc;
	pstSaveData->ulReplacedSvcNum = ulNumOfReplacedSvc;

	if(ulNumOfNewSvc > 0)
	{
		pstSaveData->newSvc = (DxLcnUpdateService_t*)OxAP_Malloc(sizeof(DxLcnUpdateService_t) * ulNumOfNewSvc);
		HxSTD_MemSet(pstSaveData->newSvc, 0x00, sizeof(DxLcnUpdateSaveData_t) * ulNumOfNewSvc);

		pNewSvcHandle = (Handle_t*)OxAP_Malloc(sizeof(Handle_t) * ulNumOfNewSvc);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_NEWSVC_ITEMS, (HUINT32*)pNewSvcHandle, sizeof(Handle_t) * ulNumOfNewSvc);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("DB_PARAM_GetItem (eDB_PARAM_ITEM_NEWSVC_ITEMS) err:\n");
			goto END_FUNC;
		}
		astNewSvcInfo = (DbSvc_Info_t*)OxAP_Malloc(sizeof(DbSvc_Info_t) * ulNumOfNewSvc);

		for(i = 0; i<ulNumOfNewSvc; i++)
		{
			hErr = DB_SVC_GetServiceInfo(pNewSvcHandle[i], &(astNewSvcInfo[i]));
			if(hErr == ERR_OK)
			{
				HINT32		ulSvcUid = 0;

				// save lcn & service name
				pstSaveData->newSvc[i].lcn = DbSvc_GetLcn(&astNewSvcInfo[i]);
				HxSTD_MemCopy(pstSaveData->newSvc[i].szSvcName, DbSvc_GetName(&astNewSvcInfo[i]), DbSvc_GetNameLength());

				// save triple id
				pstSaveData->newSvc[i].usOnId = DbSvc_GetOnId(&astNewSvcInfo[i]);
				pstSaveData->newSvc[i].usTsId = DbSvc_GetTsId(&astNewSvcInfo[i]);
				pstSaveData->newSvc[i].usSvcId = DbSvc_GetSvcId(&astNewSvcInfo[i]);

				// make & save service uid
				xmgr_search_lcnupdate_MakeSvcUid(DbSvc_GetOnId(&astNewSvcInfo[i]), DbSvc_GetTsId(&astNewSvcInfo[i]), DbSvc_GetSvcId(&astNewSvcInfo[i]), &ulSvcUid);
				pstSaveData->newSvc[i].ulSvcUid = ulSvcUid;

				HxLOG_Print("\t\t[%s:%d], [lcn : %d, name : %s] onid : 0x%x, tsid : 0x%x, svcid : 0x%x, svcuid : 0x%x\n", __FUNCTION__, __LINE__,
					DbSvc_GetLcn(&astNewSvcInfo[i]), DbSvc_GetName(&astNewSvcInfo[i]), DbSvc_GetOnId(&astNewSvcInfo[i]), DbSvc_GetTsId(&astNewSvcInfo[i]), DbSvc_GetSvcId(&astNewSvcInfo[i]), ulSvcUid);
			}
		}
	}

	if(ulNumOfReplacedSvc > 0)
	{
		pstSaveData->replacedSvc= (DxLcnUpdateService_t*)OxAP_Malloc(sizeof(DxLcnUpdateService_t) * ulNumOfReplacedSvc);
		HxSTD_MemSet(pstSaveData->replacedSvc, 0x00, sizeof(DxLcnUpdateSaveData_t) * ulNumOfReplacedSvc);

		pReplacedSvcHandle = (Handle_t*)OxAP_Malloc(sizeof(Handle_t) * ulNumOfReplacedSvc);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_REPLACESVC_ITEMS, (HUINT32*)pReplacedSvcHandle, (sizeof(Handle_t) * ulNumOfReplacedSvc));

		astReplacedSvcInfo = (DbSvc_Info_t*)OxAP_Malloc(sizeof(DbSvc_Info_t) * ulNumOfReplacedSvc);

		for(i = 0; i<ulNumOfReplacedSvc; i++)
		{
			hErr = DB_SVC_GetServiceInfo(pReplacedSvcHandle[i], &(astReplacedSvcInfo[i]));
			if(hErr == ERR_OK)
			{
				HINT32		ulSvcUid = 0;

				// save lcn & service name
				pstSaveData->replacedSvc[i].lcn = DbSvc_GetLcn(&astReplacedSvcInfo[i]);
				HxSTD_MemCopy(pstSaveData->replacedSvc[i].szSvcName, DbSvc_GetName(&astReplacedSvcInfo[i]), DbSvc_GetNameLength());

				// save triple id
				pstSaveData->replacedSvc[i].usOnId = DbSvc_GetOnId(&astReplacedSvcInfo[i]);
				pstSaveData->replacedSvc[i].usTsId = DbSvc_GetTsId(&astReplacedSvcInfo[i]);
				pstSaveData->replacedSvc[i].usSvcId = DbSvc_GetSvcId(&astReplacedSvcInfo[i]);

				// make & save service uid
				xmgr_search_lcnupdate_MakeSvcUid(DbSvc_GetOnId(&astReplacedSvcInfo[i]), DbSvc_GetTsId(&astReplacedSvcInfo[i]), DbSvc_GetSvcId(&astReplacedSvcInfo[i]), &ulSvcUid);
				pstSaveData->replacedSvc[i].ulSvcUid = ulSvcUid;

				HxLOG_Print("\t\t[%s:%d], [lcn : %d, name : %s] onid : 0x%x, tsid : 0x%x, svcid : 0x%x, svcuid : 0x%x\n", __FUNCTION__, __LINE__,
					DbSvc_GetLcn(&astReplacedSvcInfo[i]), DbSvc_GetName(&astReplacedSvcInfo[i]), DbSvc_GetOnId(&astReplacedSvcInfo[i]), DbSvc_GetTsId(&astReplacedSvcInfo[i]), DbSvc_GetSvcId(&astReplacedSvcInfo[i]), ulSvcUid);
			}
		}
	}

	// initialize
	DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_NUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACESVC_NUM, 0, 0);

END_FUNC:
	if(pNewSvcHandle)
	{
		HLIB_STD_MemFree(pNewSvcHandle);
	}

	if(astNewSvcInfo)
	{
		HLIB_STD_MemFree(astNewSvcInfo);
	}

	if(pReplacedSvcHandle)
	{
		HLIB_STD_MemFree(pReplacedSvcHandle);
	}

	if(astReplacedSvcInfo)
	{
		HLIB_STD_MemFree(astReplacedSvcInfo);
	}

	return (HERROR)BUS_PostMessageToOM(NULL, eMEVT_LCNUPDATE_SAVE_OK, HANDLE_NULL, (HINT32)pstSaveData, 0, 0);
}

// eMEVT_SEARCH_ACTION_DESTROYED
static HERROR xmgr_search_lcnupdate_MsgSearchActionDestroyed (XmgrSearch_lcnupdate_Contents_t *pstContents)
{
	BUS_MGR_Destroy(NULL);

	return ERR_OK;
}

static HERROR xmgr_search_lcnupdate_MakeSvcUid(HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HINT32 *ulSvcUid)
{
	DbSvc_Condition_t		cond;
	HINT32				svcuid[16];
	HINT32				count;

	DB_SVC_InitSvcFindCondition(&cond);

	cond.nOrgDeliType = (HINT32)eDxDELIVERY_TYPE_SAT;
	cond.nSvcId = (HINT32)usSvcId;
	cond.nTsId  = (HINT32)usTsId;
	cond.nOnId  = (HINT32)usOnId;

	count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);

	if(count == 0)
	{
		HxLOG_Error("err: cannot get svcuid\n");
		return ERR_FAIL;
	}

	*ulSvcUid = svcuid[0];

	return ERR_OK;
}

BUS_Result_t xproc_search_lcnupdate_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR							hErr = ERR_FAIL;
	XmgrSearch_lcnupdate_Contents_t	*pstContents;

	pstContents = xmgr_search_lcnupdate_GetContents();
	if (pstContents == NULL)			{ return MESSAGE_BREAK; }

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			MSG_LOG ("[eMEVT_BUS_CREATE] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgBusCreate(pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			MSG_LOG ("[eMEVT_BUS_DESTROY] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgBusDestroyed(pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_LCNUPDATE_CMD_START:
			MSG_LOG ("[eMEVT_LCNUPDATE_START] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgLcnUpdateStart(pstContents);
			break;

		case eMEVT_LCNUPDATE_CMD_SAVE:
			MSG_LOG ("[eMEVT_LCNUPDATE_SAVE] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgLcnUpdateSave(pstContents, p1, p2);
			break;

		case eMEVT_LCNUPDATE_CMD_STOP:
			MSG_LOG ("[eMEVT_LCNUPDATE_STOP] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgLcnUpdateStop(pstContents);
			break;

		case eMEVT_SEARCH_STARTED:
			MSG_LOG ("[eMEVT_SEARCH_STARTED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgSearchStarted(pstContents);
			break;

		case eMEVT_SEARCH_FINISHED:
			MSG_LOG ("[eMEVT_SEARCH_FINISHED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgSearchFinished(pstContents);
			break;

		case eMEVT_SEARCH_SAVE_FINISHED:
			MSG_LOG ("[eMEVT_SEARCH_SAVE_FINISHED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgSearchSaveFinished(pstContents);
			break;

		case eMEVT_SEARCH_ACTION_DESTROYED:
			MSG_LOG ("[eMEVT_SEARCH_ACTION_DESTROYED] 0x%08x, (%08x:%08x:%08x)\n", hAction, p1, p2, p3);
			hErr = xmgr_search_lcnupdate_MsgSearchActionDestroyed(pstContents);
			break;

		default:
			hErr = ERR_OK;
			break;
	}

	return (hErr == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}


/* End of File. ---------------------------------------------------------- */

