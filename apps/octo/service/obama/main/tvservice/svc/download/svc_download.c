/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  download_main.c
	@brief	  Filtering을 통해 File Download를 하는 기능을 모은 Task이다.

	Description:

	Module:

	Copyright (c) 2009 HUMAX Co., Ltd.			\n
	All rights reserved.						\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdio.h>
#include <octo_common.h>
#include <pal_pipe.h>
#include <isosvc.h>
#include <util.h>
#include <linkedlist.h>

#include <svc_download.h>
#include <svc_swup.h>

/** @brief local header file  */
#include <_svc_download.h>
#include <psvc_download.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief Context status */
typedef enum
{
	eDown_CntxStatus_None = 0,
	eDown_CntxStatus_Running,
	eDown_CntxStatus_Waiting
} SvcDownload_CntxStatus_e;

/** @brief Download service Context   */
typedef struct  tagDown_Context
{
	SvcDownload_ActionType_e	eActionType;						/** < action type을 나타낸다. */
	SvcDownload_CntxStatus_e	eStatus;							/** < none, running, waiting */
	Handle_t			hAct;								/** < action Handle */
	void				(*doAction)(SvcDownload_Msg_t *m,void *d);	/** < action 실행 함수.  */
	void				*pData;  							/** < action에서 사용될 private data structure  */
	unsigned long 		 ulSyncSemaphore;					/** < action stop API에서 app과의 sync를 위한 Mutex */
} SvcDownload_Context_t;


typedef	HERROR (*GetArg_t)(SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal);

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/** < Download Service 에서 사용 되는 Context */
static SvcDownload_Context_t		 s_astDown_Context[NUM_MAX_ACTION];
static unsigned long		 s_ulDown_SemForSync;
static unsigned long		 s_ulDown_SemForContents;

/** < Download Task에서 사용할 Queue ID와 Task ID */
static unsigned long		 s_ulDown_MainTaskMsgQId;
static unsigned long		 s_ulDown_MainTaskId;

static SvcDownload_SiGetTimeOutCb_t		s_fnTimeoutCb = NULL;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC INLINE void 				svc_download_SetActionHandle(SvcDownload_Context_t *pContext, Handle_t hAction);
STATIC INLINE void				svc_download_SetContextStatus(SvcDownload_Context_t *pContext, SvcDownload_CntxStatus_e eStatus);
STATIC INLINE SvcDownload_CntxStatus_e	svc_download_GetContextStatus(SvcDownload_Context_t *pContext);
STATIC INLINE SvcDownload_Context_t*	svc_download_GetContext (HUINT32 n);
STATIC INLINE void				svc_download_ExecuteDoAction(SvcDownload_Msg_t *pstMsg,  SvcDownload_Context_t *pContext);
STATIC INLINE HUINT32			svc_download_GetTaskMsgQId (void);
STATIC INLINE void				svc_download_GetSyncSem (void);
STATIC INLINE void				svc_download_ReleaseSyncSem (void);
STATIC HERROR					svc_download_ResetContext(SvcDownload_Context_t *pCt);
STATIC HERROR					svc_download_InitContext (SvcDownload_Context_t *pstCt, SvcDownload_Msg_t *pstMsg);
STATIC HERROR					svc_download_MakeStopMsg (SvcDownload_ActionType_e eActionType, Handle_t hActHandle, SvcDownload_Msg_t *pstStopMsg);

STATIC void						svc_download_MainTask (void *pvArg);


#define _______EXT2_FUNCTIONS_____________________________________________________________________
STATIC HERROR svc_download_AllocGroupInfo (SvcDownload_DnGrp_t **ppstGroupItem);
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
STATIC void svc_download_FreeGroupInfo (void *pvItem );
#else
STATIC HERROR svc_download_FreeGroupInfo (SvcDownload_DnGrp_t *pstGroupItem);
#endif

STATIC HERROR svc_download_CountGroup (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulGroupNum);

STATIC HERROR svc_download_GetGroupByGroupId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, SvcDownload_DnGrp_t **ppstGroupItem);

STATIC HERROR svc_download_CheckAllGroupInfoReceived (SvcDownload_DsmccContents_t *pstDownload);

STATIC HERROR svc_download_AllocModuleInfo (SvcDownload_DnMod_t **ppstModuleItem);
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
STATIC void svc_download_FreeModuleInfo (void *pvItem);
#else
STATIC HERROR svc_download_FreeModuleInfo (SvcDownload_DnMod_t *pstModuleItem);
#endif
STATIC HERROR svc_download_GetModuleByGroupModuleId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, HUINT32 ulModuleId, SvcDownload_DnMod_t **ppstModuleItem);

STATIC HERROR svc_download_PrepareModuleInfoToDownload (SvcDownload_DnMod_t *pstModuleItem);


STATIC HERROR svc_download_PrepareAllModuleInfoToDownload (SvcDownload_DsmccContents_t *pstDownload);

STATIC HERROR svc_download_CheckModuleInfoDownloaded (SvcDownload_DnMod_t *pstModuleItem);

STATIC HERROR svc_download_CheckModuleInfoInGroupDownloaded (SvcDownload_DnGrp_t *pstGroupItem);

STATIC HERROR svc_download_CheckAllModuleInfoDownloaded (SvcDownload_DsmccContents_t *pstDownload);

STATIC HERROR svc_download_GetDownloadProgress (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulReceivedBlocks, HUINT32 *pulTotalBlocks);

#define _____GROUP_INFO_____

STATIC HERROR svc_download_AllocGroupInfo (SvcDownload_DnGrp_t **ppstGroupItem)
{
	SvcDownload_DnGrp_t	*pstGroupItem;

	if (ppstGroupItem == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	pstGroupItem = (SvcDownload_DnGrp_t *)OxSI_Malloc (sizeof(SvcDownload_DnGrp_t));
	if (pstGroupItem == NULL)			{ return ERR_DOWN_MEM_ALLOC_FAIL; }

	HxSTD_memset (pstGroupItem, 0, sizeof(SvcDownload_DnGrp_t));

	*ppstGroupItem = pstGroupItem;
	return ERR_DOWN_RESULT_OK;
}

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
STATIC void svc_download_FreeGroupInfo (void *pvItem )
{
	SvcDownload_DnGrp_t *pstGroupItem;

	if (pvItem == NULL)			{ return ; }

	pstGroupItem = (SvcDownload_DnGrp_t *)pvItem;

	// Free the Group Lists
	if (pstGroupItem->pstModuleList != NULL)
	{
		pstGroupItem->pstModuleList = HLIB_LIST_RemoveAllFunc (pstGroupItem->pstModuleList, svc_download_FreeModuleInfo);

	}

	OxSI_Free (pstGroupItem);
	return;
}
#else
STATIC HERROR svc_download_FreeGroupInfo (SvcDownload_DnGrp_t *pstGroupItem)
{
	if (pstGroupItem == NULL)			{ return ERR_DOWN_SOURCE_NULL; }

	// Free the Group Lists
	if (pstGroupItem->pstModuleList != NULL)
	{
		pstGroupItem->pstModuleList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstGroupItem->pstModuleList, (FREE_FUNC)svc_download_FreeModuleInfo);
	}

	OxSI_Free (pstGroupItem);
	return ERR_OK;
}
#endif

STATIC HERROR svc_download_CountGroup (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulGroupNum)
{
	if (pstDownload == NULL)			{ return ERR_DOWN_SOURCE_NULL; }
	if (pulGroupNum == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	if (pstDownload->pstGroupList == NULL)
	{
		return ERR_DOWN_SOURCE_NULL;
	}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	*pulGroupNum = HLIB_LIST_Length(pstDownload->pstGroupList);
#else
	*pulGroupNum = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstDownload->pstGroupList, NULL, NULL);
#endif
	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR svc_download_GetGroupByGroupId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, SvcDownload_DnGrp_t **ppstGroupItem)
{
	SvcDownload_DnGrp_t			*pstGroupItem;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t				*pstItem = NULL;
#else
	POINTER_LIST_T		*pstItem;
#endif
	if (pstDownload == NULL)			{ return ERR_DOWN_SOURCE_NULL; }
	if (ppstGroupItem == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	pstItem = pstDownload->pstGroupList;

	while (pstItem != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupItem = (SvcDownload_DnGrp_t *)HLIB_LIST_Data(pstItem);
#else
		pstGroupItem = (SvcDownload_DnGrp_t *)pstItem->pvContents;
#endif
		if (pstGroupItem != NULL)
		{
			if (pstGroupItem->ulGroupId == ulGroupId)
			{
				*ppstGroupItem = pstGroupItem;
				return ERR_DOWN_RESULT_OK;
			}
		}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstItem = HLIB_LIST_Next(pstItem);
#else
		pstItem = pstItem->pNext;
		if (pstItem == pstDownload->pstGroupList)		{ break; }
#endif
	}

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_CheckAllGroupInfoReceived (SvcDownload_DsmccContents_t *pstDownload)
{
	HINT32				 nItemNum;
	SvcDownload_DnGrp_t			*pstGroupItem;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t				*pstItem = NULL;
#else
	POINTER_LIST_T		*pstItem;
#endif
	if (pstDownload == NULL)			{ return ERR_DOWN_SOURCE_NULL; }

	pstItem = pstDownload->pstGroupList;
	nItemNum = 0;

	while (pstItem != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupItem = (SvcDownload_DnGrp_t *)HLIB_LIST_Data(pstItem);
#else
		pstGroupItem = (SvcDownload_DnGrp_t *)pstItem->pvContents;
#endif
		if (pstGroupItem != NULL)
		{
			if (pstGroupItem->bGroupInfoReceived == FALSE)
			{
				return ERR_DOWN_RESULT_FAIL;
			}

			nItemNum ++;
		}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstItem = HLIB_LIST_Next(pstItem);
#else
		pstItem = pstItem->pNext;
		if (pstItem == pstDownload->pstGroupList)		{ break; }
#endif
	}

	return (nItemNum > 0) ? ERR_DOWN_RESULT_OK : ERR_DOWN_RESULT_FAIL;

}

#define _____MODULE_INFO_____

STATIC HERROR svc_download_AllocModuleInfo (SvcDownload_DnMod_t **ppstModuleItem)
{
	SvcDownload_DnMod_t	*pstModuleItem;

	if (ppstModuleItem == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	pstModuleItem = (SvcDownload_DnMod_t *)OxSI_Malloc (sizeof(SvcDownload_DnMod_t));
	if (pstModuleItem == NULL)			{ return ERR_DOWN_MEM_ALLOC_FAIL; }

	HxSTD_memset (pstModuleItem, 0, sizeof(SvcDownload_DnMod_t));

	*ppstModuleItem = pstModuleItem;
	return ERR_DOWN_RESULT_OK;
}

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
STATIC void svc_download_FreeModuleInfo (void *pvItem)
{
	HUINT32			 	ulIndex;
	SvcDownload_DnMod_t *pstModuleItem;

	if (pvItem == NULL)			{ return ; }

	pstModuleItem =  (SvcDownload_DnMod_t *)pvItem;

	// Free the Data
	if (pstModuleItem->pucBuffer != NULL)
	{
		OxSI_Free (pstModuleItem->pucBuffer);
		pstModuleItem->pucBuffer = NULL;
	}

	if (pstModuleItem->ppucBlockArray != NULL)
	{
		for (ulIndex = 0; ulIndex < pstModuleItem->ulBlockNum; ulIndex++)
		{
			if (pstModuleItem->ppucBlockArray[ulIndex] != NULL &&
				pstModuleItem->ppucBlockArray[ulIndex] != BLOCK_NOT_DOWNLOADED)
			{
				OxSI_Free (pstModuleItem->ppucBlockArray[ulIndex]);
			}
		}

		OxSI_Free (pstModuleItem->ppucBlockArray);
		pstModuleItem->pucBuffer = NULL;
	}

	OxSI_Free (pstModuleItem);
	return ;
}

#else
STATIC HERROR svc_download_FreeModuleInfo (SvcDownload_DnMod_t *pstModuleItem)
{
	HUINT32			 ulIndex;

	if (pstModuleItem == NULL)			{ return ERR_DOWN_SOURCE_NULL; }

	// Free the Data
	if (pstModuleItem->pucBuffer != NULL)
	{
		OxSI_Free (pstModuleItem->pucBuffer);
		pstModuleItem->pucBuffer = NULL;
	}

	if (pstModuleItem->ppucBlockArray != NULL)
	{
		for (ulIndex = 0; ulIndex < pstModuleItem->ulBlockNum; ulIndex++)
		{
			if (pstModuleItem->ppucBlockArray[ulIndex] != NULL &&
				pstModuleItem->ppucBlockArray[ulIndex] != BLOCK_NOT_DOWNLOADED)
			{
				OxSI_Free (pstModuleItem->ppucBlockArray[ulIndex]);
			}
		}

		OxSI_Free (pstModuleItem->ppucBlockArray);
		pstModuleItem->pucBuffer = NULL;
	}

	OxSI_Free (pstModuleItem);
	return ERR_OK;
}
#endif

STATIC HERROR svc_download_GetModuleByGroupModuleId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, HUINT32 ulModuleId, SvcDownload_DnMod_t **ppstModuleItem)
{
	SvcDownload_DnGrp_t			*pstGroupItem;
	SvcDownload_DnMod_t			*pstModuleItem;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t				*pstItem = NULL;
#else
	POINTER_LIST_T		*pstItem;
#endif
	HERROR				 hErr;

	hErr = svc_download_GetGroupByGroupId (pstDownload, ulGroupId, &pstGroupItem);
	if (hErr != ERR_OK)
	{
		return hErr;
	}

	pstItem = pstGroupItem->pstModuleList;

	while (pstItem != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstModuleItem = (SvcDownload_DnMod_t *)HLIB_LIST_Data(pstItem);
#else
		pstModuleItem = (SvcDownload_DnMod_t *)pstItem->pvContents;
#endif
		if (pstModuleItem != NULL)
		{
			if (pstModuleItem->ulModuleId == ulModuleId)
			{
				*ppstModuleItem = pstModuleItem;
				return ERR_DOWN_RESULT_OK;
			}
		}

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstItem = HLIB_LIST_Next(pstItem);
#else
		pstItem = pstItem->pNext;
		if (pstItem == pstGroupItem->pstModuleList)		{ break; }
#endif
	}

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_PrepareModuleInfoToDownload (SvcDownload_DnMod_t *pstModuleItem)
{
	HUINT32		  ulBlockNum;
	HUINT8		**ppucBlockArray;

	if (pstModuleItem == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	if (pstModuleItem->ulModuleSize == 0 || pstModuleItem->ulBlockSize == 0)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	ulBlockNum = ((pstModuleItem->ulModuleSize - 1) / pstModuleItem->ulBlockSize) + 1;

	ppucBlockArray = (HUINT8 **)OxSI_Malloc (sizeof(HUINT8 *) * ulBlockNum);
	if (ppucBlockArray == NULL)
	{
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	HxSTD_memset (ppucBlockArray, 0, sizeof(HUINT8 *) * ulBlockNum);

	pstModuleItem->ulBlockNum = ulBlockNum;
	pstModuleItem->ppucBlockArray = ppucBlockArray;

	return ERR_DOWN_RESULT_OK;
}



STATIC HERROR svc_download_PrepareAllModuleInfoToDownload (SvcDownload_DsmccContents_t *pstDownload)
{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t		*pstGroupList = NULL;
	HxList_t		*pstModuleList= NULL;
#else
	POINTER_LIST_T		*pstGroupList;
	POINTER_LIST_T		*pstModuleList;
#endif
	SvcDownload_DnGrp_t			*pstGroupItem;
	SvcDownload_DnMod_t			*pstModuleItem;

	if (pstDownload == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	pstGroupList = pstDownload->pstGroupList;
	while (pstGroupList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupItem = (SvcDownload_DnGrp_t *)HLIB_LIST_Data(pstGroupList);
#else
		pstGroupItem = (SvcDownload_DnGrp_t *)pstGroupList->pvContents;
#endif
		if (pstGroupItem != NULL)
		{
			pstModuleList = pstGroupItem->pstModuleList;
			while (pstModuleList != NULL)
			{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
				pstModuleItem = (SvcDownload_DnMod_t *)HLIB_LIST_Data(pstModuleList);
#else
				pstModuleItem = (SvcDownload_DnMod_t *)pstModuleList->pvContents;
#endif
				if (pstModuleItem != NULL)
				{
					svc_download_PrepareModuleInfoToDownload (pstModuleItem);
				}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
				pstModuleList = HLIB_LIST_Next(pstModuleList);
#else
				pstModuleList = pstModuleList->pNext;
				if (pstModuleList == pstGroupItem->pstModuleList)		{ break; }
#endif
			}
		}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupList = HLIB_LIST_Next(pstGroupList);
#else
		pstGroupList = pstGroupList->pNext;
		if (pstGroupList == pstDownload->pstGroupList)					{ break; }
#endif
	}

	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR svc_download_CheckModuleInfoDownloaded (SvcDownload_DnMod_t *pstModuleItem)
{
#if 0
	HUINT32		  ulBlockIdx;
	HUINT8		**ppucBlockArray;

	if (pstModuleItem == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	if (pstModuleItem->ulBlockNum == 0 || pstModuleItem->ppucBlockArray == NULL)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	ppucBlockArray = pstModuleItem->ppucBlockArray;
	for (ulBlockIdx = 0; ulBlockIdx < pstModuleItem->ulBlockNum; ulBlockIdx++)
	{
		if (ppucBlockArray[ulBlockIdx] == NULL)
		{
			return ERR_DOWN_RESULT_FAIL;
		}
	}

	// SKIP이건 실제 버퍼이건 다 받았다
	return ERR_DOWN_RESULT_OK;
#endif
	HUINT32			 ulTotalBlockNum;

	if (pstModuleItem == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	if (pstModuleItem->ulBlockNum == 0 || pstModuleItem->ppucBlockArray == NULL)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	ulTotalBlockNum = pstModuleItem->ulReceivedBlockNum;// + pstModuleItem->ulSkippedBlockNum;
	if (ulTotalBlockNum >= pstModuleItem->ulBlockNum)
	{
		return ERR_DOWN_RESULT_OK;
	}

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_CheckModuleInfoInGroupDownloaded (SvcDownload_DnGrp_t *pstGroupItem)
{
	HBOOL				 bChecked;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t				*pstModuleList = NULL;
#else
	POINTER_LIST_T		*pstModuleList;
#endif
	SvcDownload_DnMod_t			*pstModuleItem;
	HERROR				 hErr;

	if (pstGroupItem == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	bChecked = FALSE;

	pstModuleList = pstGroupItem->pstModuleList;
	while (pstModuleList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstModuleItem = (SvcDownload_DnMod_t *)HLIB_LIST_Data(pstModuleList);
#else
		pstModuleItem = (SvcDownload_DnMod_t *)pstModuleList->pvContents;
#endif
		if (pstModuleItem != NULL)
		{
			hErr = svc_download_CheckModuleInfoDownloaded (pstModuleItem);
			if (hErr != ERR_DOWN_RESULT_OK)
			{
				return hErr;
			}

			bChecked = TRUE;
		}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstModuleList = HLIB_LIST_Next(pstModuleList);
#else
		pstModuleList = pstModuleList->pNext;
		if (pstModuleList == pstGroupItem->pstModuleList)			{ break; }
#endif
	}

	return (bChecked == TRUE) ? ERR_DOWN_RESULT_OK : ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_CheckAllModuleInfoDownloaded (SvcDownload_DsmccContents_t *pstDownload)
{
	HBOOL				 bChecked;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t				*pstGroupList = NULL;
#else
	POINTER_LIST_T		*pstGroupList;
#endif
	SvcDownload_DnGrp_t			*pstGroupItem;
	HERROR				 hErr;

	if (pstDownload == NULL)					{ return ERR_DOWN_SOURCE_NULL; }

	bChecked = FALSE;

	pstGroupList = pstDownload->pstGroupList;
	while (pstGroupList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupItem = (SvcDownload_DnGrp_t *)HLIB_LIST_Data(pstGroupList);
#else
		pstGroupItem = (SvcDownload_DnGrp_t *)pstGroupList->pvContents;
#endif
		if (pstGroupItem != NULL)
		{
			hErr = svc_download_CheckModuleInfoInGroupDownloaded (pstGroupItem);
			if (hErr != ERR_DOWN_RESULT_OK)
			{
				return hErr;
			}

			bChecked = TRUE;
		}
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupList = HLIB_LIST_Next(pstGroupList);
#else
		pstGroupList = pstGroupList->pNext;
		if (pstGroupList == pstDownload->pstGroupList)			{ break; }
#endif
	}

	return (bChecked == TRUE) ? ERR_DOWN_RESULT_OK : ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_GetDownloadProgress (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulReceivedBlocks, HUINT32 *pulTotalBlocks)
{
	if (pstDownload == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulReceivedBlocks == NULL || pulTotalBlocks == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	*pulReceivedBlocks	= pstDownload->ulRcvBlocks;
	*pulTotalBlocks		= pstDownload->ulTotalBlocks;
	return ERR_DOWN_RESULT_OK;
}

// = Group Info = //

HERROR SVC_DOWNLOAD_AllocGroupInfo (SvcDownload_DnGrp_t **ppstGroupItem)
{
	return svc_download_AllocGroupInfo (ppstGroupItem);
}

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
void SVC_DOWNLOAD_FreeGroupInfo (void *pstGroupItem)
{
	svc_download_FreeGroupInfo (pstGroupItem);
}
#else
HERROR SVC_DOWNLOAD_FreeGroupInfo (SvcDownload_DnGrp_t *pstGroupItem)
{
	return svc_download_FreeGroupInfo (pstGroupItem);
}
#endif
HERROR SVC_DOWNLOAD_CountGroup (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulGroupNum)
{
	return svc_download_CountGroup (pstDownload, pulGroupNum);
}

HERROR SVC_DOWNLOAD_GetGroupByGroupId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, SvcDownload_DnGrp_t **ppstGroupItem)
{
	return svc_download_GetGroupByGroupId (pstDownload, ulGroupId, ppstGroupItem);
}

HERROR SVC_DOWNLOAD_CheckAllGroupInfoReceived (SvcDownload_DsmccContents_t *pstDownload)
{
	return svc_download_CheckAllGroupInfoReceived (pstDownload);
}


// = Module Info = //

HERROR SVC_DOWNLOAD_AllocModuleInfo (SvcDownload_DnMod_t **ppstModuleItem)
{
	return svc_download_AllocModuleInfo (ppstModuleItem);
}

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
void SVC_DOWNLOAD_FreeModuleInfo (void *pstModuleItem)
{
	svc_download_FreeModuleInfo (pstModuleItem);
}
#else
HERROR SVC_DOWNLOAD_FreeModuleInfo (SvcDownload_DnMod_t *pstModuleItem)
{
	return svc_download_FreeModuleInfo (pstModuleItem);
}
#endif
HERROR SVC_DOWNLOAD_GetModuleByGroupModuleId (SvcDownload_DsmccContents_t *pstDownload, HUINT32 ulGroupId, HUINT32 ulModuleId, SvcDownload_DnMod_t **ppstModuleItem)
{
	return svc_download_GetModuleByGroupModuleId (pstDownload, ulGroupId, ulModuleId, ppstModuleItem);
}

HERROR SVC_DOWNLOAD_PrepareModuleInfoToDownload (SvcDownload_DnMod_t *pstModuleItem)
{
	return svc_download_PrepareModuleInfoToDownload (pstModuleItem);
}

HERROR SVC_DOWNLOAD_PrepareAllModuleInfoToDownload (SvcDownload_DsmccContents_t *pstDownload)
{
	return svc_download_PrepareAllModuleInfoToDownload (pstDownload);
}

HERROR SVC_DOWNLOAD_CheckModuleInfoDownloaded (SvcDownload_DnMod_t *pstModuleItem)
{
	return svc_download_CheckModuleInfoDownloaded (pstModuleItem);
}

HERROR SVC_DOWNLOAD_CheckModuleInfoInGroupDownloaded (SvcDownload_DnGrp_t *pstGroupItem)
{
	return svc_download_CheckModuleInfoInGroupDownloaded (pstGroupItem);
}

HERROR SVC_DOWNLOAD_CheckAllModuleInfoDownloaded (SvcDownload_DsmccContents_t *pstDownload)
{
	return svc_download_CheckAllModuleInfoDownloaded (pstDownload);
}

HERROR SVC_DOWNLOAD_GetDownloadProgress (SvcDownload_DsmccContents_t *pstDownload, HUINT32 *pulReceivedBlocks, HUINT32 *pulTotalBlocks)
{
	return svc_download_GetDownloadProgress (pstDownload, pulReceivedBlocks, pulTotalBlocks);
}
#define _______EXT2_FUNCTIONS_____________________________________________________________________


#define ________Internal_Function________

STATIC INLINE void  svc_download_SetActionHandle(SvcDownload_Context_t *pContext, Handle_t hAction)
{
	pContext->hAct = hAction;
}

STATIC INLINE void svc_download_SetContextStatus(SvcDownload_Context_t *pContext, SvcDownload_CntxStatus_e eStatus)
{
	pContext->eStatus = eStatus;
}

STATIC INLINE SvcDownload_CntxStatus_e svc_download_GetContextStatus(SvcDownload_Context_t *pContext)
{
    return pContext->eStatus;
}

STATIC INLINE SvcDownload_Context_t * svc_download_GetContext (HUINT32 n)
{
	return &s_astDown_Context[n];
}

STATIC INLINE void svc_download_ExecuteDoAction(SvcDownload_Msg_t *pstMsg, SvcDownload_Context_t *pContext)
{
    pContext->doAction(pstMsg, (void *)pContext->pData);
}

STATIC INLINE HUINT32 svc_download_GetTaskMsgQId()
{
	return s_ulDown_MainTaskMsgQId;
}

typedef struct tagDownloadAction
{
	SvcDownload_CB_t						stCallbackList;
} SvcDownload_Action_t;

STATIC SvcDownload_Action_t	s_astDownloadActionList[eDOWN_ACTION_MAX];

STATIC INLINE SvcDownload_CB_t *svc_download_GetCbList(SvcDownload_ActionType_e eActType)
{
	return (eDOWN_ACTION_MAX <= eActType) ? NULL : &s_astDownloadActionList[eActType].stCallbackList;
}


STATIC HERROR svc_download_InitAction(void)
{
	HxSTD_memset(s_astDownloadActionList, 0, sizeof(SvcDownload_CB_t)*eDOWN_ACTION_MAX);

	return psvc_download_InitProduct();
}

HERROR svc_download_ActionRegister(SvcDownload_ActionType_e eActType, SvcDownload_CB_t *pstCbList)
{
	if (eDOWN_ACTION_MAX <= eActType)
		return ERR_DOWN_RESULT_FAIL;

	if (NULL == pstCbList)
		return ERR_DOWN_RESULT_FAIL;

	HxSTD_memcpy(&s_astDownloadActionList[eActType].stCallbackList, pstCbList, sizeof(SvcDownload_CB_t));

	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR svc_download_ExecuteInit(SvcDownload_ActionType_e eActType, void **ppvAction, void **ppvData, void *pvSiMsg)
{
	SvcDownload_CB_t *pstCbList;

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnInit)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnInit)(ppvAction, ppvData, pvSiMsg);
}

STATIC HERROR svc_download_ExecuteTerm(SvcDownload_ActionType_e eActType, void **ppvAction, void **ppvData)
{
	SvcDownload_CB_t *pstCbList;

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnTerm)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnTerm)(ppvAction, ppvData);
}

STATIC HERROR svc_download_ExecuteGetData(SvcDownload_ActionType_e eActType, void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	SvcDownload_CB_t *pstCbList;

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnGetData)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnGetData)(pvData, eDataType, pulDataNum, ppvDataArray);
}

STATIC HERROR svc_download_ExecuteFreeData(SvcDownload_ActionType_e eActType, SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray)
{
	SvcDownload_CB_t *pstCbList;

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnFreeData)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnFreeData)(eDataType, ulDataNum, pvDataArray);
}

STATIC HERROR svc_download_ExecuteCheckDownloadComplete(SvcDownload_ActionType_e eActType, HUINT32 ulSignalVersion, HBOOL *pbComplete)
{
	SvcDownload_CB_t *pstCbList;

	HxLOG_Assert(pbComplete);

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnCheckDonwloadComplete)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnCheckDonwloadComplete)(ulSignalVersion, pbComplete);
}

STATIC HERROR svc_download_ExecuteClearDownloadInfo(SvcDownload_ActionType_e eActType)
{
	SvcDownload_CB_t *pstCbList;

	pstCbList = svc_download_GetCbList(eActType);
	if (NULL == pstCbList)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	if (NULL == pstCbList->pfnClearDownloadInfo)
	{
		return ERR_DOWN_CALLBACK_NULL;
	}

	return (pstCbList->pfnClearDownloadInfo)();
}

STATIC HERROR svc_download_ExecuteInitMetaInfo(void)
{
	SvcDownload_ActionType_e	 eActType;
	SvcDownload_CB_t		*pstCbList;
	HUINT32				 ulCount;
	HERROR				 hErr;

	hErr = ERR_OK;
	for (ulCount=0; ulCount<eDOWN_ACTION_MAX; ulCount++)
	{
		eActType = (SvcDownload_ActionType_e)ulCount;
		pstCbList = svc_download_GetCbList(eActType);
		if (pstCbList && pstCbList->pfnInitMetaInfo)
		{
			hErr |= (pstCbList->pfnInitMetaInfo)();
		}
	}

	return hErr;
}

/** @brief Context의 내용을 초기화 하는 함수 */
STATIC HERROR svc_download_ResetContext(SvcDownload_Context_t *pCt)
{
    HUINT32     ulRet = ERR_DOWN_RESULT_OK;

	if ( pCt == NULL )
	{
		HxLOG_Critical("\n\n");
		return ERR_DOWN_SOURCE_NULL;
	}

	ulRet = svc_download_ExecuteTerm (pCt->eActionType, (void**)&pCt->doAction, (void**)&pCt->pData);

    pCt->eActionType	= eDOWN_ACTION_NONE;
	pCt->eStatus		= eDown_CntxStatus_None;
	pCt->hAct			= HANDLE_NULL;

	return ulRet;
}

/** @brief Download Context를 초기화 하는 함수. */
STATIC HERROR svc_download_InitContext (SvcDownload_Context_t *pstCt, SvcDownload_Msg_t *pstMsg)
{
	HUINT32		ulActType;
	HERROR		hErr;

	if (pstCt == NULL || pstMsg == NULL)
	{
		return ERR_DOWN_PARAM_NULL;
	}

	/* context reset! */
	svc_download_ResetContext (pstCt);

	hErr = svc_download_GetMsgArg (pstMsg, eDownArgType_ActionType, &ulActType);
	if (hErr != ERR_DOWN_RESULT_OK)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	pstCt->eActionType = (SvcDownload_ActionType_e)ulActType;

	hErr = svc_download_ExecuteInit(pstCt->eActionType, (void**)&pstCt->doAction, (void**)&pstCt->pData, (void *)pstMsg);

	return hErr;
}

STATIC HERROR svc_download_MakeStopMsg (SvcDownload_ActionType_e eActionType, Handle_t hActHandle, SvcDownload_Msg_t *pstStopMsg)
{
	if (pstStopMsg == NULL)						{ return ERR_DOWN_TARGET_NULL; }

	HxSTD_memset (pstStopMsg, 0, sizeof(SvcDownload_Msg_t));

	if (eDOWN_ACTION_MAX <= eActionType)
	{
		return ERR_DOWN_PARAM_INVALID;
	}

	pstStopMsg->ulMsgClass = CMD_DOWN_STOP;
	pstStopMsg->hActHandle = hActHandle;

	return ERR_DOWN_RESULT_OK;
}


STATIC INLINE void svc_download_GetSyncSem (void)
{
	VK_SEM_Get (s_ulDown_SemForSync);
}

STATIC INLINE void svc_download_ReleaseSyncSem (void)
{
	VK_SEM_Release (s_ulDown_SemForSync);
}


#define ________External_Function________

STATIC PostMsgToMgrCb_t		s_pfnSvcDownloadNotifyMsgToMgr = NULL;
STATIC PostDataToMgrCb_t	s_pfnSvcDownloadNotifyDataToMgr = NULL;

void svc_download_UtilPostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{

	if (s_pfnSvcDownloadNotifyMsgToMgr)
	{
		(void)(* s_pfnSvcDownloadNotifyMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return;
}

void svc_download_UtilPostDataToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize)
{
	if (s_pfnSvcDownloadNotifyDataToMgr)
	{
		(void)(* s_pfnSvcDownloadNotifyDataToMgr)(nMsg, hAction, nParam1, nParam2, pvData, ulSize);
	}

	return;
}

HERROR	svc_download_UtilRegisterNotifyCallback(PostMsgToMgrCb_t pfnMsgNotify, PostDataToMgrCb_t pfnDataNotify)
{
	s_pfnSvcDownloadNotifyMsgToMgr = pfnMsgNotify;
	s_pfnSvcDownloadNotifyDataToMgr = pfnDataNotify;

	return ERR_OK;
}


void svc_download_ContentsEnterCritical (void)
{
	VK_SEM_Get (s_ulDown_SemForContents);
}

void svc_download_ContentsLeaveCritical (void)
{
	VK_SEM_Release (s_ulDown_SemForContents);
}

HERROR svc_download_SendSectionMessage (SvcDownload_Message_e eMsgType, Handle_t hAction, HUINT32 ulTableStatus, HUINT32 ulFilterId)
{
	SvcDownload_Msg_t		stMsg;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_DOWN_PARAM_INVALID;
	}

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));

	stMsg.ulMsgClass = (HUINT32)eMsgType;
	stMsg.hActHandle = hAction;
	stMsg.unArg.stSection.ulTableStatus	= ulTableStatus;
	stMsg.unArg.stSection.ulFilterId	= ulFilterId;

	return (HERROR)VK_MSG_Send (svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
}

HERROR svc_download_GetMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal)
{
	GetArg_t	pfGetArg;

	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulArgVal == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	pfGetArg = NULL;

	// Download Section Callback Message :
	if (pstMsg->ulMsgClass > DOWNCB_TABLE_START && pstMsg->ulMsgClass < DOWNCB_TABLE_END)
	{
		pfGetArg = svc_download_GetSectionMsgArg;
	}
	// Download Section One-Table Completed Callback Message : (In All-retrieving Functions)
	else if (pstMsg->ulMsgClass > DOWNCB_ALL_RECEIVE_START && pstMsg->ulMsgClass < DOWNCB_ALL_RECEIVE_END)
	{
		pfGetArg = svc_download_GetTableCompleteMsgArg;
	}
	else
	{
		switch (pstMsg->ulMsgClass)
		{
		// One Message Type:
		case CMD_DOWN_START:
			pfGetArg = svc_download_GetStartMsgArg;
			break;
		case CMD_DOWN_STOP:
			pfGetArg = svc_download_GetStopMsgArg;
			break;
		default:
			pfGetArg = NULL;
			break;
		}
	}

	if (pfGetArg)
	{
		return pfGetArg (pstMsg, ulArgType, pulArgVal);
	}

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR svc_download_CopySetData (SvcDownload_SetData_t *pstDst, SvcDownload_SetData_t *pstSrc)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT32			 ulStrLen = 0;
	HCHAR			*pszSrcStr = NULL, *pszDstStr = NULL;
#endif

	if ((NULL == pstDst) || (NULL == pstSrc))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDst, pstSrc, sizeof(SvcDownload_SetData_t));
	switch (pstSrc->eSetType)
	{
#if defined(CONFIG_MW_SI_AIT)
	case eDOWN_SET_HBBTV_ADDPID:
		pszSrcStr = pstSrc->stHbbtvAddPid.pszCachePath;
		if (NULL != pszSrcStr)
		{
			ulStrLen = HLIB_STD_StrNLen(pszSrcStr, 2048);
			if (ulStrLen > 0)
			{
				pszDstStr = (HCHAR *)OxMW_Malloc(ulStrLen + 1);
				if (NULL != pszDstStr)
				{
					HxSTD_StrNCpy(pszDstStr, pszSrcStr, ulStrLen);
					pszDstStr[ulStrLen] = '\0';
				}
			}
		}

		pstDst->stHbbtvAddPid.pszCachePath = pszDstStr;
		break;
#endif

	default:
		break;
	}

	return ERR_OK;
}

STATIC HERROR svc_download_ClearSetData (SvcDownload_SetData_t *pstData)
{
	switch (pstData->eSetType)
	{
#if defined(CONFIG_MW_SI_AIT)
	case eDOWN_SET_HBBTV_ADDPID:
		if (NULL != pstData->stHbbtvAddPid.pszCachePath)
		{
			OxMW_Free(pstData->stHbbtvAddPid.pszCachePath);
		}

		break;
#endif

	default:
		break;
	}

	HxSTD_MemSet(pstData, 0, sizeof(SvcDownload_SetData_t));
	return ERR_OK;
}

// ==============================================================================
//     Download Main Task
// ==============================================================================
STATIC HERROR svc_download_CmdStop (SvcDownload_Context_t *pstContext, SvcDownload_Msg_t *pstMsg)
{
	Handle_t			 hAction = pstMsg->hActHandle;
	HERROR				 hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->ulMsgClass, pstContext->eStatus);

	switch (pstContext->eStatus)
	{
	case eDown_CntxStatus_None:
		pstContext->hAct = hAction;
		hResult = ERR_OK;
		break;

	case eDown_CntxStatus_Waiting:
		pstContext->hAct = hAction;
		if (NULL != pstContext->doAction)
		{
			svc_download_ExecuteDoAction(pstMsg, pstContext);
		}

		hResult = ERR_OK;
		break;

	case eDown_CntxStatus_Running:
		pstContext->hAct	= hAction;
		pstContext->eStatus	= eDown_CntxStatus_Waiting;

		if (NULL != pstContext->doAction)
		{
			svc_download_ExecuteDoAction(pstMsg, pstContext);
		}
		else
		{
			HxLOG_Critical ("!!! State Running but No Action !!!\n");
		}

		hResult = ERR_OK;
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	if(pstContext->eActionType == eDOWN_DOWNLOAD_OTA_IMAGE)
	{
		svc_download_ResetContext(pstContext);
	}

	svc_download_UtilPostMsgToMgr(eSEVT_DOWN_STOPPED, hAction, pstContext->eActionType, 0, 0);
	return hResult;
}

STATIC HERROR svc_download_CmdStart (SvcDownload_Context_t *pstContext, SvcDownload_Msg_t *pstMsg)
{
	Handle_t					 hAction = pstMsg->hActHandle;
	SvcDownload_ActionType_e	 eActType = (SvcDownload_ActionType_e)pstMsg->unArg.stStartDown.ulActType;
	SvcDownload_Msg_t 			*pstStopMsg = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->ulMsgClass, pstContext->eStatus);

	switch (pstContext->eStatus)
	{
	case eDown_CntxStatus_None:
		hErr = svc_download_InitContext(pstContext, pstMsg);
		if (ERR_OK == hErr)
		{
			pstContext->hAct = hAction;
			pstContext->eStatus = eDown_CntxStatus_Waiting;

			hResult = svc_download_CmdStart(pstContext, pstMsg);
		}
		break;

	case eDown_CntxStatus_Waiting:
		if (NULL == pstContext->doAction)
		{
			HxLOG_Critical("Action Function shall be set in the waiting state!!!\n");
			return ERR_FAIL;
		}

		if (eActType != pstContext->eActionType)
		{
			// 현재 data 모두 초기화 시키고
			svc_download_ResetContext(pstContext);
			pstContext->eActionType = eActType;

			// 새로운 action kind로 초기화
			hErr = svc_download_InitContext(pstContext, pstMsg);
			if (ERR_OK != hErr)
			{
				HxLOG_Critical("context init. failed !!\n");
				return ERR_FAIL;
			}

			hResult = svc_download_CmdStart(pstContext, pstMsg);
		}
		else
		{
			pstContext->hAct = hAction;
			pstContext->eStatus = eDown_CntxStatus_Running;
			svc_download_ExecuteDoAction(pstMsg, pstContext);
			hResult = ERR_OK;
		}

		break;

	case eDown_CntxStatus_Running:
		pstStopMsg = (SvcDownload_Msg_t *)OxSI_Calloc(sizeof(SvcDownload_Msg_t));
		if (NULL != pstStopMsg)
		{
			hErr = svc_download_MakeStopMsg(pstContext->eActionType, pstContext->hAct, pstStopMsg);
			if (ERR_OK == hErr)
			{
				svc_download_CmdStop(pstContext, pstStopMsg);
			}

			OxSI_Free(pstStopMsg);
		}

		if (eDown_CntxStatus_Running != pstContext->eStatus)
		{
			hResult = svc_download_CmdStart(pstContext, pstMsg);
		}
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	return hResult;
}

STATIC void svc_download_MainTask (void *pvArg)
{
	HERROR			hErr;
	SvcDownload_Msg_t			stMsg;
	SvcDownload_Context_t	*pContext = NULL;
	HUINT32			actionId;
	HUINT32			ulSyncMode;
	Handle_t		hAction = HANDLE_NULL;

	while (1)
	{
		hErr = VK_MSG_Receive (svc_download_GetTaskMsgQId(), &stMsg, sizeof (SvcDownload_Msg_t));
		if (hErr != ERR_OK)
		{
			continue;
		}

		hAction = stMsg.hActHandle;
		if (hAction == HANDLE_NULL)
		{
			// Handle 이 HANDLE_NULL 이면 Main Action ID를 Handle이라고 판단한다.
			actionId = eActionId_VIEW_0;
			hAction = PAL_PIPE_GetActionHandle (actionId);
		}

		actionId = PAL_PIPE_GetActionId(hAction);
		pContext = (SvcDownload_Context_t*)svc_download_GetContext(actionId);

		/*
		 * nMsg를 실행 시 실행 여부는 최신 버전 확인 후 처리 한다.
		 * 최신 버전이 아닌 경우 무시 한다. 즉 NONE인 상태에서
		 * 해당 action이 동작하려면 최신 버전인 경우만 실행한다.
		 */

        hErr = PAL_PIPE_IsActionHandleLatest(hAction);

		if ( (pContext == NULL)
		|| ((svc_download_GetContextStatus(pContext) == eDown_CntxStatus_None) && (hErr != ERR_OK)) )
		{
			// Sync 일 경우에는 Semaphore를 풀어 준다.
			hErr = svc_download_GetMsgArg (&stMsg, eDownArgType_SyncMode, &ulSyncMode);
			if (hErr == ERR_DOWN_RESULT_OK && ulSyncMode == eSyncMode)
			{
				svc_download_ReleaseSyncSem();
			}

			continue;
		}

		switch (stMsg.ulMsgClass)
		{
		case CMD_DOWN_START:
			hErr = svc_download_CmdStart(pContext, &stMsg);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("svc_download_CmdStop err:\n");
			}

			break;

		case	CMD_DOWN_STOP:
			hErr = svc_download_CmdStop(pContext, &stMsg);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("svc_download_CmdStop err:\n");
			}

			// Sync 일 경우에는 Semaphore를 풀어 준다.
			hErr = svc_download_GetMsgArg (&stMsg, eDownArgType_SyncMode, &ulSyncMode);
			if (hErr != ERR_DOWN_RESULT_OK)
			{
				ulSyncMode = eAsyncMode;
			}

			if (ulSyncMode == eSyncMode)
			{
				/* Sync Call 인경우 sema를 release 해 준다. */
				svc_download_ReleaseSyncSem();
			}
			else
			{
				/* app에게 eSEVT_DOWN_STOPPED message를 보낸다. */
				svc_download_UtilPostMsgToMgr(eSEVT_DOWN_STOPPED, hAction, pContext->eActionType, 0, 0);
			}

			break;

		case CMD_DOWN_SET:
			// Set CMD shall be executed only the case of the running state.
			if (svc_download_GetContextStatus(pContext) == eDown_CntxStatus_Running)
			{
				svc_download_ExecuteDoAction(&stMsg, pContext);
			}

			(void)svc_download_ClearSetData(&(stMsg.unArg.stSet));
			break;

		case CMD_DOWN_SHUTDOWN:
			svc_download_ReleaseSyncSem();
			break;

		/******************************************************
		*	Pal에서 올라오는 모든 event 처리는 아래(default)  *
		*	에서 처리한다.									  *
		******************************************************/
		default :
			if ( (svc_download_GetContextStatus(pContext) == eDown_CntxStatus_Running) && (pContext->hAct == hAction) )
			{
				if ( pContext->doAction == NULL )
				{
					HxLOG_Print(": Event process - Have no doAction.\n");
					HxLOG_Critical("\n\n");
					break;
				}
                svc_download_ExecuteDoAction(&stMsg, pContext);
			}
			else
			{
				HxLOG_Print("Not running, Message is ignored !, nMsg=0x%x, hAction=0x%x\n", stMsg.ulMsgClass, hAction);
			}
			break;
		}
	}

}

#define _______MSG_ARG_PARSERS_______

HERROR svc_download_GetActionHandleFromMsg (SvcDownload_Msg_t *pstMsg, Handle_t *phAction)
{
	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (phAction == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	*phAction = pstMsg->hActHandle;
	return ERR_DOWN_RESULT_OK;
}

HERROR svc_download_GetSectionMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal)
{
	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulArgVal == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	if (pstMsg->ulMsgClass > DOWNCB_TABLE_START && pstMsg->ulMsgClass < DOWNCB_TABLE_END)
	{
		switch (ulArgType)
		{
		case eDownArgType_TableStatus:
			*pulArgVal = pstMsg->unArg.stSection.ulTableStatus;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_FilterId:
			*pulArgVal = pstMsg->unArg.stSection.ulFilterId;
			return ERR_DOWN_RESULT_OK;
		}
	}

	return ERR_DOWN_RESULT_FAIL;
}

HERROR svc_download_GetTableCompleteMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal)
{
	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulArgVal == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	if (pstMsg->ulMsgClass > DOWNCB_ALL_RECEIVE_START && pstMsg->ulMsgClass < DOWNCB_ALL_RECEIVE_END)
	{
		switch (ulArgType)
		{
		case eDownArgType_TsUniqId:
			*pulArgVal = (HUINT32)pstMsg->unArg.stCompTable.usTsUniqId;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_FilterId:
			*pulArgVal = (HUINT32)pstMsg->unArg.stCompTable.usFilterId;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_ExtId1:
			*pulArgVal = (HUINT32)pstMsg->unArg.stCompTable.usExtId;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_TableStatus:
			*pulArgVal = (HUINT32)pstMsg->unArg.stCompTable.usStatus;
			return ERR_DOWN_RESULT_OK;
		}
	}

	return ERR_DOWN_RESULT_FAIL;
}


#define	_____DOWNLOAD_MSG_PARSER_____

HERROR svc_download_GetStartMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal)
{
	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulArgVal == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	if (pstMsg->ulMsgClass == CMD_DOWN_START)
	{
		switch (ulArgType)
		{
		case eDownArgType_TsUniqId:
			*pulArgVal = (HUINT32)pstMsg->unArg.stStartDown.usTsUniqId;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_ActionType:
			*pulArgVal = pstMsg->unArg.stStartDown.ulActType;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_Pid:
			*pulArgVal = (HUINT32)pstMsg->unArg.stStartDown.usPid;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_ExtId1:
			*pulArgVal = pstMsg->unArg.stStartDown.ulExtId1;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_ExtId2:
			*pulArgVal = pstMsg->unArg.stStartDown.ulExtId2;
			return ERR_DOWN_RESULT_OK;
		case eDownArgType_ExtId3:
			*pulArgVal = pstMsg->unArg.stStartDown.ulExtId3;
			return ERR_DOWN_RESULT_OK;
		}
	}

	return ERR_DOWN_RESULT_FAIL;
}

HERROR svc_download_GetStopMsgArg (SvcDownload_Msg_t *pstMsg, HUINT32 ulArgType, HUINT32 *pulArgVal)
{
	if (pstMsg == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pulArgVal == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	if (pstMsg->ulMsgClass == CMD_DOWN_STOP)
	{
		switch (ulArgType)
		{
		case eDownArgType_SyncMode:
			*pulArgVal = pstMsg->unArg.stStopDown.ulSyncMode;
			return ERR_DOWN_RESULT_OK;
		}
	}

	return ERR_DOWN_RESULT_FAIL;
}


#define	_____DOWNLOAD_CONTEXT_____

HERROR svc_download_GetContextData (Handle_t hActHandle, void **ppvContextData)
{
	HUINT32			 ulActionId;
	SvcDownload_Context_t	*pstContext;

	if (ppvContextData == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	ulActionId = PAL_PIPE_GetActionId (hActHandle);
	pstContext = (SvcDownload_Context_t *)svc_download_GetContext (ulActionId);

	if ( pstContext == NULL || pstContext->hAct != hActHandle)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	if (pstContext->pData == NULL)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	*ppvContextData = pstContext->pData;
	return ERR_DOWN_RESULT_OK;
}

#define	_____DOWNLOAD_DSMCC_CONTENTS_____

HERROR svc_download_AllocDsmccContents (SvcDownload_DsmccContents_t **ppstDownload)
{
	SvcDownload_DsmccContents_t	*pstDownload;

	if (ppstDownload == NULL)			{ return ERR_DOWN_TARGET_NULL; }

	pstDownload = (SvcDownload_DsmccContents_t *)OxSI_Malloc (sizeof(SvcDownload_DsmccContents_t));
	if (pstDownload == NULL)			{ return ERR_DOWN_MEM_ALLOC_FAIL; }

	HxSTD_memset (pstDownload, 0, sizeof(SvcDownload_DsmccContents_t));

	*ppstDownload = pstDownload;
	return ERR_DOWN_RESULT_OK;
}

HERROR svc_download_FreeDsmccContents (SvcDownload_DsmccContents_t *pstDownload)
{
	if (pstDownload == NULL)			{ return ERR_DOWN_SOURCE_NULL; }

	// Free the Group Lists
	// pstDownload->pstGroupList

	HxSTD_memset (pstDownload, 0, sizeof(SvcDownload_DsmccContents_t));
	OxSI_Free (pstDownload);
	return ERR_OK;
}


#define	___________________________________________________________________
#define _____API_FUNCTIONS_FOR_APPS_____

#define _____INITIALIZAtION_____

HERROR SVC_DOWNLOAD_Init(PostMsgToMgrCb_t pfnMsgNotify, PostDataToMgrCb_t pfnDataNotify)
{
	HERROR		ulRet;
	HUINT32 	nAct;

	HxLOG_Info("SVC_DOWNLOAD_Init() [+]\n");

	svc_download_UtilRegisterNotifyCallback(pfnMsgNotify, pfnDataNotify);

	/** < Download task의 Queue를 생성한다. */
	ulRet = VK_MSG_Create(MW_DOWN_MSG_SIZE, sizeof(SvcDownload_Msg_t), "DownloadTaskQ", &s_ulDown_MainTaskMsgQId, VK_SUSPENDTYPE_FIFO);
	if (ulRet != ERR_OK)
	{
		goto ulRet_blk;
	}

	ulRet = (HUINT32)VK_TASK_Create(svc_download_MainTask, MW_DOWN_TASK_PRIORITY, MW_DOWN_TASK_STACK_SIZE, "svc_download_MainTask", NULL, &s_ulDown_MainTaskId, 0);
	if (ulRet != ERR_OK)
	{
		goto ulRet_blk;
	}

	HxSTD_memset(s_astDown_Context, 0, NUM_MAX_ACTION*(sizeof(SvcDownload_Context_t)));

	/* initialize contexts */
	for ( nAct = 0; nAct < NUM_MAX_ACTION; nAct++ )
	{
		s_astDown_Context[nAct].eStatus	= eDown_CntxStatus_None;
		s_astDown_Context[nAct].hAct		= HANDLE_NULL;
		s_astDown_Context[nAct].doAction	= NULL;
		s_astDown_Context[nAct].pData		= NULL;
	}

	// Sync API 를 위한 Semaphore
	ulRet = VK_SEM_Create (&s_ulDown_SemForSync, "DownSyncSem", VK_SUSPENDTYPE_PRIORITY);
	if (ulRet == ERR_OK)
	{
		svc_download_GetSyncSem();
	}
	else
	{
		s_ulDown_SemForSync = 0;
	}

	// Contents Sync를 위한 Semaphore
	ulRet = VK_SEM_Create (&s_ulDown_SemForContents, "DownCntsSem", VK_SUSPENDTYPE_PRIORITY);
	if (ulRet != ERR_OK)
	{
		s_ulDown_SemForContents = 0;
	}

	svc_download_InitAction();

	svc_download_ExecuteInitMetaInfo();

	/* Start Download Task */
	VK_TASK_Start(s_ulDown_MainTaskId);

	HxLOG_Info("SVC_DOWNLOAD_Init() [-]\n");
	return ERR_DOWN_RESULT_OK;

ulRet_blk:
	HxLOG_Critical("\n\n");
	return ERR_DOWN_RESULT_FAIL;
}

HERROR SVC_DOWNLOAD_SiRegisterTimeoutCallback(SvcDownload_SiGetTimeOutCb_t fnSiTimeout)
{
	s_fnTimeoutCb = fnSiTimeout;

	return ERR_OK;
}

HERROR svc_download_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	if (NULL == pulTimeout)
		return ERR_FAIL;

	if (s_fnTimeoutCb)
	{
		return (* s_fnTimeoutCb)(hAction, eTableType, pulTimeout);
	}

	return ERR_FAIL;
}

#define _____DOWNLOAD_API_____

HERROR SVC_DOWNLOAD_Start (Handle_t hAction, HUINT16 usTsUniqId, SvcDownload_ActionType_e eActType,
							HUINT16 usPid, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	SvcDownload_Msg_t 	 stMsg;
	HERROR		 hErr;

	HxLOG_Info("SVC_DOWNLOAD_Start()\n");

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));
	stMsg.ulMsgClass = CMD_DOWN_START;
	stMsg.hActHandle = hAction;
	stMsg.unArg.stStartDown.usTsUniqId	= usTsUniqId;
	stMsg.unArg.stStartDown.usPid		= usPid;
	stMsg.unArg.stStartDown.ulActType	= (HUINT32)eActType;
	stMsg.unArg.stStartDown.ulExtId1	= ulArg1;
	stMsg.unArg.stStartDown.ulExtId2	= ulArg2;
	stMsg.unArg.stStartDown.ulExtId3	= ulArg3;

	hErr = VK_MSG_Send (svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
	return hErr;
}

HERROR SVC_DOWNLOAD_Stop (Handle_t hAction, ApiSyncMode_t syncMode)
{
	SvcDownload_Msg_t		 stMsg;
	HERROR		 hErr;

	HxLOG_Info("SVC_DOWNLOAD_Stop()\n");

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));
	stMsg.ulMsgClass = CMD_DOWN_STOP;
	stMsg.hActHandle = hAction;
	stMsg.unArg.stStopDown.ulSyncMode = (HUINT32)syncMode;

	stMsg.unArg.stStopDown.usPid		= 0xFFFF;

	hErr = VK_MSG_Send (svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_DOWN_RESULT_FAIL;
	}

	if (syncMode == eSyncMode)
	{
		svc_download_GetSyncSem();
	}

	return hErr;
}

HERROR SVC_DOWNLOAD_Set (Handle_t hAction, SvcDownload_SetType_e eSetType, SvcDownload_SetData_t *pstData)
{
	SvcDownload_Msg_t 	 stMsg;
	HERROR		 hErr;

	HxLOG_Info("SVC_DOWNLOAD_Start()\n");

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));
	stMsg.ulMsgClass = CMD_DOWN_SET;
	stMsg.hActHandle = hAction;

	svc_download_CopySetData(&(stMsg.unArg.stSet), pstData);
	stMsg.unArg.stSet.eSetType			= eSetType;

	hErr = VK_MSG_Send(svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
	return hErr;
}

HERROR SVC_DOWNLOAD_GetData (Handle_t hAction, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HUINT32			 ulActionId;
	SvcDownload_Context_t	*pstContext;
	HERROR			 hErr;

	if (pulDataNum == NULL || ppvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstContext = (SvcDownload_Context_t *)svc_download_GetContext (ulActionId);

	if (pstContext == NULL || pstContext->hAct != hAction)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get (s_ulDown_SemForContents);

	hErr = svc_download_ExecuteGetData(pstContext->eActionType, pstContext->pData, eDataType, pulDataNum, ppvDataArray);

	VK_SEM_Release (s_ulDown_SemForContents);

	return hErr;
}

HERROR SVC_DOWNLOAD_FreeData (Handle_t hAction, SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray)
{
	HUINT32			 ulActionId;
	SvcDownload_Context_t	*pstContext;
	HERROR			 hErr;

	if (ulDataNum == 0 || pvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstContext = (SvcDownload_Context_t *)svc_download_GetContext (ulActionId);

	if (pstContext == NULL || pstContext->hAct != hAction)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get (s_ulDown_SemForContents);
	switch (eDataType)
	{
	case eDOWN_DATA_HBBTV_EVENT:
		hErr = svc_download_ExecuteFreeData(pstContext->eActionType, eDataType, ulDataNum, pvDataArray);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}
	VK_SEM_Release (s_ulDown_SemForContents);

	return hErr;
}

HERROR SVC_DOWNLOAD_CheckDownloadComplete(Handle_t hAction, SvcDownload_ActionType_e eActType, HUINT32 ulSignalVersion, HBOOL *pbComplete)
{
	HUINT32			 ulActionId;
	SvcDownload_Context_t	*pstContext;
	HERROR			 hErr;

	if (NULL == pbComplete)
	{
		return ERR_FAIL;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstContext = (SvcDownload_Context_t *)svc_download_GetContext (ulActionId);
	if (pstContext == NULL /*|| pstContext->hAct != hAction*/)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get (s_ulDown_SemForContents);

	hErr = svc_download_ExecuteCheckDownloadComplete(eActType, ulSignalVersion, pbComplete);

	VK_SEM_Release (s_ulDown_SemForContents);

	return hErr;
}

HERROR SVC_DOWNLOAD_ClearDownloadInfo(Handle_t hAction, SvcDownload_ActionType_e eActType)
{
	HUINT32			 ulActionId;
	SvcDownload_Context_t	*pstContext;
	HERROR			 hErr;

	ulActionId = PAL_PIPE_GetActionId (hAction);
	pstContext = (SvcDownload_Context_t *)svc_download_GetContext (ulActionId);
	if (pstContext == NULL || pstContext->hAct != hAction)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get (s_ulDown_SemForContents);

	hErr = svc_download_ExecuteClearDownloadInfo(eActType);

	VK_SEM_Release (s_ulDown_SemForContents);

	return hErr;
}


#define _____SW_UPDATE_API_____
/*
 Functions for only Software Update Module.
 Use different event and function.
*/
Handle_t SVC_SWUPDATE_StartDownloadRf (SvcSwUpdate_DownloadSourceInfoRf_t *pstRfParam, HUINT8 *szTargetPath)
{
	SvcDownload_Msg_t 	 stMsg;
	HERROR		 hErr;

	HxLOG_Info("OxSWUP_StartDownloadRf() --- PID(0x%x), TargetPath(%s)\n", pstRfParam->usPid, szTargetPath);

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));
	stMsg.ulMsgClass = CMD_DOWN_START;
	stMsg.hActHandle = pstRfParam->hAction;
	stMsg.unArg.stStartDown.usTsUniqId	= pstRfParam->usTsUniqId;
	stMsg.unArg.stStartDown.ulActType	= eDOWN_DOWNLOAD_OTA_IMAGE;
	stMsg.unArg.stStartDown.usPid		= pstRfParam->usPid;
	stMsg.unArg.stStartDown.ulExtId1	= pstRfParam->ulSignalVersion;
	stMsg.unArg.stStartDown.ulExtId2	= 0;
	stMsg.unArg.stStartDown.ulExtId3	= 0;

	hErr = VK_MSG_Send (svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
	(void)(hErr);

	return pstRfParam->hAction;
}

HERROR SVC_SWUPDATE_StopDownloadRf (Handle_t hDownload)
{
	SvcDownload_Msg_t		 stMsg;
	HERROR		 hErr;

	HxLOG_Info("OxSWUP_StopDownload()\n");

	HxSTD_memset(&stMsg, 0, sizeof(SvcDownload_Msg_t));
	stMsg.ulMsgClass = CMD_DOWN_STOP;
	stMsg.hActHandle = hDownload;

	stMsg.unArg.stStopDown.usPid		= 0xFFFF;

	hErr = VK_MSG_Send (svc_download_GetTaskMsgQId(), &stMsg, sizeof(SvcDownload_Msg_t));
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_DOWN_RESULT_FAIL;
	}

	return hErr;
}


/* end of file */
