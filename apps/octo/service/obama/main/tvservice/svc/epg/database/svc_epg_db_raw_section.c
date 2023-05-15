/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdlib.h>
#include <octo_common.h>

#include "linkedlist.h"

#include <db_svc.h>
#include <svc_epg.h>
#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_common_lib.h"




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define EPG_SVCSECTION_RAW_POOL				500
#define SVCSECTION_RAW_SEGMENT				32


#define MACRO_SETBITMASK(p, pos)			(p[(pos>>3)]  |= (0x01 <<(pos&7)))
#define MACRO_CLEARBITMASK(p, pos)			(p[(pos>>3)] &= (~(0x01 <<(pos&7))))
#define MACRO_CHECKBITMASK(p, pos)			(p[(pos>>3)] & (0x01 <<(pos&7)))
#define MACRO_SEGMENTBITMASK(p, pos)		( p[(pos>>3)] &= (  0xff >> (7 - (pos&7))  )  )





/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/** data structure for section check */
typedef enum enSectionBuildMode
{
	eEpgSectionBuild_invalid		= 0,
	eEpgSectionBuild_building,
	eEpgSectionBuild_complete,

	eEpgSectionBuild_max
}SvcEpg_SectionBuildModRaw_t;


typedef struct tagSectionBuildInfo
{
	SvcEpg_SectionBuildModRaw_t			eBuildMode;
	HUINT8					ucTableId;
	HUINT8					ucVersion;
	HUINT8					ucLastSectionNumber;

	/** 256 bit (8*32) */
	HUINT8					acSectionBuildMask[SVCSECTION_RAW_SEGMENT];
}SvcEpg_SectionBuildInfoRaw_t;

/** data structure for section check */
typedef struct tagSvcSectionNode
{
	HUINT16 					usActionType;
	HUINT16						usTsId;
	HUINT16						usOnId;
	HUINT16						usSvcId;

	/** all table id complete */
	SvcEpg_SectionBuildModRaw_t				eBuildMode;

	/** table_0 ~ table_15 */
	SvcEpg_SectionBuildInfoRaw_t		astSectionBuildInfo[16];

	struct tagSvcSectionNode		*pNext;
}SvcEpg_SvcSectionNodeRaw_t;


/** data structure for section check */
typedef struct tagSvcSectionMgr
{
	HULONG						ulSemId;

	HUINT16						usTsUniqueId;

	POINTER_LIST_T				*pPfActSvcList;
	POINTER_LIST_T				*pPfOthSvcList;
	POINTER_LIST_T				*pSchActSvcList;
	POINTER_LIST_T				*pSchOthSvcList;

#if 0
	HUINT32						ulPoolSize;
	HUINT32						ulFreeNodeCount;
	SvcEpg_SvcSectionNodeRaw_t		*pBuffer;
	SvcEpg_SvcSectionNodeRaw_t		*pFreeHead;
#endif
}SvcEpg_SvcSectionMgrRaw_t;

// Manager 가 Action ID 단위로 존재하므로 SvcEpg_SvcSectionNodeRaw_t 의 양을 줄이기 위해
// SvcEpg_SvcSectionNodeRaw_t Pool을 따로 만들도록 한다.?
typedef struct
{
	HULONG							 ulSemId;

	HUINT32							 ulPoolSize;
	HINT32							 nFreeNodeCount;
	SvcEpg_SvcSectionNodeRaw_t		*pBuffer;
	SvcEpg_SvcSectionNodeRaw_t		*pFreeHead;
}SvcEpg_SvcSectionNodePool_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/******************************************************************
  * service section manager for check section complete
  *****************************************************************/
STATIC SvcEpg_SvcSectionMgrRaw_t		s_stSvcRawSectionManager[eEPG_ACTTYPE_MAX] = {{0, },{0, },};
STATIC SvcEpg_SvcSectionNodePool_t		s_stSvcRawNodePool = { 0, };

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR			svc_epgsection_CreateSvcSectionManager(void);
STATIC HERROR			svc_epgsection_InitSvcSectionManager(Handle_t hAction, HUINT16 usTsUniqueId, HUINT8 ucTableId, HBOOL bIsEitPlus);
STATIC void				svc_epgsection_FreeSvcSection(void *pvContents);
STATIC HERROR			svc_epgsection_AllocSectionNode(HUINT16 usActionType, SvcEpg_SvcSectionNodeRaw_t **ppNode);
STATIC HERROR			svc_epgsection_FreeSectionNode(SvcEpg_SvcSectionNodeRaw_t *pNode);
STATIC HERROR			svc_epgsection_CheckSvcSection(Handle_t hAction, HUINT16 usTsUniqueId, svcEpg_ServiceInfo_t *pServiceInfo, HBOOL *pbConflict, HBOOL *pbVersionChange, HBOOL *pbIsFirstTime);
STATIC HERROR			svc_epgsection_CheckSectionComplete(Handle_t hAction, HUINT8 ucTableId, HBOOL *pbComplete, HBOOL bIsEitPlus);
STATIC HERROR			svc_epgsection_FindSvcSection(POINTER_LIST_T *pList, svcEpg_ServiceInfo_t *pServiceInfo, SvcEpg_SvcSectionNodeRaw_t **ppSvcSection);
STATIC HERROR			svc_epgsection_GetSvcSectionInfo(Handle_t hAction, svcEpg_ServiceInfo_t *pSrcInfo, svcEpg_ServiceInfo_t *pDstInfo, HBOOL bIsEitPlus);
STATIC HBOOL			svc_epgsection_FilterSvcSection(void *pvItem, void *pvFilter);
STATIC HBOOL			svc_epgsection_FilterBuildingTable(void *pvItem, void *pvFilter);
STATIC HUINT16			svc_epgsection_GetContextIdx(Handle_t hAction);



#define ____GLOBAL_FUNC____

HERROR					SVC_EPGSECTION_CreateSvcSectionManager(void)
{
	return svc_epgsection_CreateSvcSectionManager();
}

HERROR					SVC_EPGSECTION_CheckSvcSection(Handle_t hAction, HUINT16 usTsUniqueId, svcEpg_ServiceInfo_t *pServiceInfo, HBOOL *pbConflict, HBOOL *pbVersionChange, HBOOL *pbIsFirstTime)
{
	return svc_epgsection_CheckSvcSection(hAction, usTsUniqueId, pServiceInfo, pbConflict, pbVersionChange , pbIsFirstTime);
}

HERROR					SVC_EPGSECTION_InitSvcSectionManager(Handle_t hAction, HUINT16 usTsUniqueId, HUINT8 ucTableId)
{
	return svc_epgsection_InitSvcSectionManager(hAction,usTsUniqueId, ucTableId, FALSE);
}


HERROR					SVC_EPGSECTION_CheckSectionComplete(Handle_t hAction, HUINT8 ucTableId, HBOOL *pbComplete)
{
	return svc_epgsection_CheckSectionComplete(hAction,ucTableId, pbComplete, FALSE);
}


HERROR					SVC_EPGSECTION_GetSvcSectionInfo(Handle_t hAction, svcEpg_ServiceInfo_t *pSrcInfo, svcEpg_ServiceInfo_t *pDstInfo)
{
	return svc_epgsection_GetSvcSectionInfo(hAction, pSrcInfo, pDstInfo, FALSE);
}

#define ____LOCAL_FUNC____

STATIC HUINT16			svc_epgsection_GetContextIdx(Handle_t hAction)
{
	HERROR			hError = ERR_EPG_FAIL;
	HUINT16			usActionType = eEPG_ACTTYPE_MAX;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Print("param fail!!\n");
		return usActionType;
	}

	hError = SVC_EPG_GetEpgActionType(hAction, &usActionType);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPG_GetEpgActionType fail!!\n");
		return usActionType;
	}
	return usActionType;
}


STATIC HERROR			svc_epgsection_CreateSvcSectionManager(void)
{
	HUINT32						ulIndex = 0;
	SvcEpg_SvcSectionNodeRaw_t	*pSvcSectionPool = NULL;
	HINT32						 nRet;
	HINT32						i = 0;

	for(i = 0; i < eEPG_ACTTYPE_MAX; i++)
	{
		if(s_stSvcRawSectionManager[i].ulSemId == 0)
		{
			nRet = VK_SEM_Create((unsigned long*)&s_stSvcRawSectionManager[i].ulSemId, "epgdb_svcsectionmgr", VK_SUSPENDTYPE_PRIORITY);
			if(nRet != VK_OK)
			{
				HxLOG_Error("VK_SEM_Create fail!!\n");
			}

		}

#if 0
		nRet = VK_SEM_Get(s_stSvcRawSectionManager[i].ulSemId);

		pSvcSecionPool = (SvcEpg_SvcSectionNodeRaw_t*)OxMW_Malloc(sizeof(SvcEpg_SvcSectionNodeRaw_t) * EPG_SVCSECTION_RAW_POOL);
		if(pSvcSecionPool == NULL)
		{
			HxLOG_Print("[svc_epgsection_CreateSvcSectionManager] pSvcSecionPool fail!!\n");
			nRet = VK_SEM_Release(s_stSvcRawSectionManager[i].ulSemId);
			return ERR_EPG_FAIL;
		}
		VK_memset32(pSvcSecionPool, 0x00, sizeof(SvcEpg_SvcSectionNodeRaw_t) * EPG_SVCSECTION_RAW_POOL);
		for(ulIndex = 0; ulIndex < EPG_SVCSECTION_RAW_POOL - 1; ulIndex++)
		{
			pSvcSecionPool[ulIndex].pNext = &(pSvcSecionPool[ulIndex + 1]);
		}
		s_stSvcRawSectionManager[i].pBuffer = pSvcSecionPool;
		s_stSvcRawSectionManager[i].pFreeHead = pSvcSecionPool;
		s_stSvcRawSectionManager[i].ulFreeNodeCount = EPG_SVCSECTION_RAW_POOL;
		s_stSvcRawSectionManager[i].ulPoolSize = EPG_SVCSECTION_RAW_POOL;
		nRet = VK_SEM_Release(s_stSvcRawSectionManager[i].ulSemId);
#endif
	}

	if (0 == s_stSvcRawNodePool.ulSemId)
	{
		(void)VK_SEM_Create((unsigned long*)&s_stSvcRawNodePool.ulSemId, "epgdb_nodepool", VK_SUSPENDTYPE_PRIORITY);
		(void)VK_SEM_Get(s_stSvcRawNodePool.ulSemId);

		pSvcSectionPool = (SvcEpg_SvcSectionNodeRaw_t*)OxMW_Calloc(sizeof(SvcEpg_SvcSectionNodeRaw_t) * EPG_SVCSECTION_RAW_POOL);
		if (NULL != pSvcSectionPool)
		{
			for(ulIndex = 0; ulIndex < EPG_SVCSECTION_RAW_POOL - 1; ulIndex++)
			{
				pSvcSectionPool[ulIndex].pNext = &(pSvcSectionPool[ulIndex + 1]);
			}

			s_stSvcRawNodePool.pBuffer = pSvcSectionPool;
			s_stSvcRawNodePool.pFreeHead = pSvcSectionPool;
			s_stSvcRawNodePool.nFreeNodeCount = EPG_SVCSECTION_RAW_POOL;
			s_stSvcRawNodePool.ulPoolSize = EPG_SVCSECTION_RAW_POOL;
		}

		(void)VK_SEM_Release(s_stSvcRawNodePool.ulSemId);
	}


	return ERR_EPG_OK;
}


STATIC HERROR			svc_epgsection_InitSvcSectionManager(Handle_t hAction, HUINT16 usTsUniqueId, HUINT8 ucTableId, HBOOL bIsEitPlus)
{
	HERROR		hError 			= ERR_EPG_FAIL;
	HUINT16 	usActionType	= svc_epgsection_GetContextIdx(hAction);


	VK_SEM_Get(s_stSvcRawSectionManager[usActionType].ulSemId);

	HxLOG_Debug("s_stSvcRawSectionManager.usTsUniqueId(0x%X)  usTsUniqueId(0x%X) hAction[%x], usActionType[%x]\n",
				s_stSvcRawSectionManager[usActionType].usTsUniqueId,usTsUniqueId, hAction, usActionType);

	s_stSvcRawSectionManager[usActionType].usTsUniqueId = usTsUniqueId;

	switch(ucTableId)
	{
		case EIT_ACTUAL_TABLE_ID:
			s_stSvcRawSectionManager[usActionType].pPfActSvcList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	s_stSvcRawSectionManager[usActionType].pPfActSvcList,
																									svc_epgsection_FreeSvcSection);
			break;
		case EIT_OTHER_TABLE_ID:
			s_stSvcRawSectionManager[usActionType].pPfOthSvcList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	s_stSvcRawSectionManager[usActionType].pPfOthSvcList,
																									svc_epgsection_FreeSvcSection);
			break;
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_0:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_1:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_2:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_3:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_4:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_5:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_6:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_7:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_8:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_9:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_10:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_11:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_12:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_13:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_14:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_15:
			s_stSvcRawSectionManager[usActionType].pSchActSvcList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	s_stSvcRawSectionManager[usActionType].pSchActSvcList,
																										svc_epgsection_FreeSvcSection);
			break;
		case EIT_OTHER_SCHEDULE_TABLE_ID_0:		case EIT_OTHER_SCHEDULE_TABLE_ID_1:
		case EIT_OTHER_SCHEDULE_TABLE_ID_2:		case EIT_OTHER_SCHEDULE_TABLE_ID_3:
		case EIT_OTHER_SCHEDULE_TABLE_ID_4:		case EIT_OTHER_SCHEDULE_TABLE_ID_5:
		case EIT_OTHER_SCHEDULE_TABLE_ID_6:		case EIT_OTHER_SCHEDULE_TABLE_ID_7:
		case EIT_OTHER_SCHEDULE_TABLE_ID_8:		case EIT_OTHER_SCHEDULE_TABLE_ID_9:
		case EIT_OTHER_SCHEDULE_TABLE_ID_10:	case EIT_OTHER_SCHEDULE_TABLE_ID_11:
		case EIT_OTHER_SCHEDULE_TABLE_ID_12:	case EIT_OTHER_SCHEDULE_TABLE_ID_13:
		case EIT_OTHER_SCHEDULE_TABLE_ID_14:	case EIT_OTHER_SCHEDULE_TABLE_ID_15:
			s_stSvcRawSectionManager[usActionType].pSchOthSvcList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(	s_stSvcRawSectionManager[usActionType].pSchOthSvcList,
																										svc_epgsection_FreeSvcSection);
			break;
		default :
			goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :
	VK_SEM_Release(s_stSvcRawSectionManager[usActionType].ulSemId);

	return hError;
}


STATIC void				svc_epgsection_FreeSvcSection(void *pvContents)
{
	HERROR						hError = ERR_EPG_FAIL;
	SvcEpg_SvcSectionNodeRaw_t			*pTemp = NULL;

	if(pvContents == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}
	pTemp = (SvcEpg_SvcSectionNodeRaw_t*)pvContents;

	hError = svc_epgsection_FreeSectionNode(pTemp);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("svc_epgsection_FreeSectionNode fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

EXIT_EPGRAWDB_SECTION_FUNC :

	return;
}



STATIC HERROR				svc_epgsection_AllocSectionNode(HUINT16 usActionType, SvcEpg_SvcSectionNodeRaw_t **ppNode)
{
	HERROR						hError = ERR_EPG_FAIL;
	SvcEpg_SvcSectionNodeRaw_t			*pTemp = NULL;

	if (0 == s_stSvcRawNodePool.ulSemId)
	{
		HxLOG_Error("No NodePool Semaphore: Not Initialized:\n");
		return ERR_EPG_FAIL;
	}

	if(ppNode == NULL)
	{
		HxLOG_Error("ppNode NULL\n");
		return ERR_EPG_FAIL;
	}

	*ppNode = NULL;

	(void)VK_SEM_Get(s_stSvcRawNodePool.ulSemId);

	if (NULL == s_stSvcRawNodePool.pFreeHead)
	{
		HUINT32							 ulIndex;
		SvcEpg_SvcSectionNodeRaw_t		*pstSvcSectionPool;

		pstSvcSectionPool = (SvcEpg_SvcSectionNodeRaw_t *)OxMW_Calloc(sizeof(SvcEpg_SvcSectionNodeRaw_t) * EPG_SVCSECTION_RAW_POOL);
		if (NULL == pstSvcSectionPool)
		{
			HxLOG_Error("Memory allocation failed:\n");
			goto EXIT_EPGRAWDB_SECTION_FUNC;
		}

		for(ulIndex = 0; ulIndex < EPG_SVCSECTION_RAW_POOL - 1; ulIndex++)
		{
			pstSvcSectionPool[ulIndex].pNext = &(pstSvcSectionPool[ulIndex + 1]);
		}

		s_stSvcRawNodePool.pFreeHead = pstSvcSectionPool;
		s_stSvcRawNodePool.nFreeNodeCount += EPG_SVCSECTION_RAW_POOL;
		s_stSvcRawNodePool.ulPoolSize += EPG_SVCSECTION_RAW_POOL;
	}

	pTemp = s_stSvcRawNodePool.pFreeHead;
	s_stSvcRawNodePool.pFreeHead = pTemp->pNext;
	s_stSvcRawNodePool.nFreeNodeCount--;
	HxSTD_MemSet(pTemp, 0, sizeof(SvcEpg_SvcSectionNodeRaw_t));

	*ppNode = pTemp;

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :
	(void)VK_SEM_Release(s_stSvcRawNodePool.ulSemId);

	return hError;
}


STATIC HERROR				svc_epgsection_FreeSectionNode(SvcEpg_SvcSectionNodeRaw_t *pNode)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pNode == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	(void)VK_SEM_Get(s_stSvcRawNodePool.ulSemId);

	pNode->pNext	= s_stSvcRawNodePool.pFreeHead;
	s_stSvcRawNodePool.pFreeHead	= pNode;
	s_stSvcRawNodePool.nFreeNodeCount++;

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :
	(void)VK_SEM_Release(s_stSvcRawNodePool.ulSemId);

	return hError;
}


STATIC HERROR			svc_epgsection_CheckSvcSection(Handle_t hAction, HUINT16 usTsUniqueId, svcEpg_ServiceInfo_t *pServiceInfo, HBOOL *pbConflict, HBOOL *pbVersionChange, HBOOL *pbIsFirstTime)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pList = NULL;
	SvcEpg_SvcSectionNodeRaw_t			*pSvcSection = NULL;
	HUINT8						ucTableSlot = 0;
	HUINT8						ucLastTableSlot = 0;
	HUINT8						acSectionBuildMask[SVCSECTION_RAW_SEGMENT] = {0, };
	HUINT8						ucIndex = 0;
	SvcEpg_SectionBuildInfoRaw_t		*pstSectionBuildInfo;
	HUINT16 		usActionType	=	svc_epgsection_GetContextIdx(hAction);

	//HxLOG_Warning("usActionType[%d]\n", usActionType);
	VK_SEM_Get(s_stSvcRawSectionManager[usActionType].ulSemId);
	if(pServiceInfo == NULL || pbConflict == NULL || pbVersionChange == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}
	*pbConflict = FALSE;
	*pbVersionChange = FALSE;
	*pbIsFirstTime = FALSE;
	// CONFIG_MW_MM_REC_WITH_EPG 일 경우 n 군데서 건 아이들이 다 올라온다.
	// 보다 멋지게 수정하려면 s_stSvcRawSectionManager 혹은 usTsUniqueId 를 array나 list형태로 해야겠지만,
	// 일단은 n군데서 올라온 data를 다 달아놔도 상관은 없다.
#if !defined(CONFIG_MW_MM_REC_WITH_EPG)
	if(s_stSvcRawSectionManager[usActionType].usTsUniqueId != usTsUniqueId)
	{
		HxLOG_Print("s_stSvcRawSectionManager.usTsUniqueId(0x%X) != usTsUniqueId(0x%X) !!\n",
					s_stSvcRawSectionManager[usActionType].usTsUniqueId,usTsUniqueId);
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}
#endif


	switch(pServiceInfo->ucTableId)
	{
		case EIT_ACTUAL_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfActSvcList;
			ucTableSlot = 0;
			break;
		case EIT_OTHER_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfOthSvcList;
			ucTableSlot = 0;
			break;
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_0:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_1:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_2:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_3:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_4:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_5:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_6:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_7:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_8:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_9:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_10:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_11:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_12:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_13:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_14:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchActSvcList;
			ucTableSlot = pServiceInfo->ucTableId - EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
			ucLastTableSlot = pServiceInfo->ucLastTableId - EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
			break;
		case EIT_OTHER_SCHEDULE_TABLE_ID_0:		case EIT_OTHER_SCHEDULE_TABLE_ID_1:
		case EIT_OTHER_SCHEDULE_TABLE_ID_2:		case EIT_OTHER_SCHEDULE_TABLE_ID_3:
		case EIT_OTHER_SCHEDULE_TABLE_ID_4:		case EIT_OTHER_SCHEDULE_TABLE_ID_5:
		case EIT_OTHER_SCHEDULE_TABLE_ID_6:		case EIT_OTHER_SCHEDULE_TABLE_ID_7:
		case EIT_OTHER_SCHEDULE_TABLE_ID_8:		case EIT_OTHER_SCHEDULE_TABLE_ID_9:
		case EIT_OTHER_SCHEDULE_TABLE_ID_10:	case EIT_OTHER_SCHEDULE_TABLE_ID_11:
		case EIT_OTHER_SCHEDULE_TABLE_ID_12:	case EIT_OTHER_SCHEDULE_TABLE_ID_13:
		case EIT_OTHER_SCHEDULE_TABLE_ID_14:	case EIT_OTHER_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchOthSvcList;
			ucTableSlot = pServiceInfo->ucTableId - EIT_OTHER_SCHEDULE_TABLE_ID_0;
			ucLastTableSlot = pServiceInfo->ucLastTableId - EIT_OTHER_SCHEDULE_TABLE_ID_0;
			break;
	}

	hError = svc_epgsection_FindSvcSection(pList, pServiceInfo, &pSvcSection);
	if(hError != ERR_OK)
	{
		HxLOG_Print("svc_epgsection_FindSvcSection fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

/***************************************************************************
*
* section masking area
*
***************************************************************************/
	if(pSvcSection == NULL)
	{
		// 이 Triple Id에 대한 섹션은 처음 받아 보았다.
		*pbIsFirstTime = TRUE;

		svc_epgsection_AllocSectionNode(usActionType, &pSvcSection);
		if(hError != ERR_OK || pSvcSection == NULL)
		{
			HxLOG_Print("svc_epgsection_AllocSectionNode fail!!\n");
			goto EXIT_EPGRAWDB_SECTION_FUNC;
		}
		pstSectionBuildInfo = pSvcSection->astSectionBuildInfo;
		pSvcSection->usActionType = usActionType;
		pSvcSection->usTsId = pServiceInfo->usTsId;
		pSvcSection->usOnId = pServiceInfo->usOnId;
		pSvcSection->usSvcId = pServiceInfo->usSvcId;
		pSvcSection->eBuildMode = eEpgSectionBuild_building;

		pstSectionBuildInfo[ucTableSlot].ucTableId = pServiceInfo->ucTableId;
		pstSectionBuildInfo[ucTableSlot].ucVersion = pServiceInfo->ucVersion;
		pstSectionBuildInfo[ucTableSlot].ucLastSectionNumber = pServiceInfo->ucLastSectionNumber;
		pstSectionBuildInfo[ucTableSlot].eBuildMode = eEpgSectionBuild_building;

		VK_memset32(	pstSectionBuildInfo[ucTableSlot].acSectionBuildMask,
						0xff,
						(pServiceInfo->ucLastSectionNumber/8 + 1)*sizeof(HUINT8));
		MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucLastSectionNumber);
		MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSegmentLastSectionNumber);
		MACRO_CLEARBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSectionNumber);

		pList = UTIL_LINKEDLIST_AppendListItemToPointerList(pList, (void*)pSvcSection);
		switch(pServiceInfo->ucTableId)
		{
			case EIT_ACTUAL_TABLE_ID:
				s_stSvcRawSectionManager[usActionType].pPfActSvcList = pList;
				break;
			case EIT_OTHER_TABLE_ID:
				pList = s_stSvcRawSectionManager[usActionType].pPfOthSvcList = pList;
				break;
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_0:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_1:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_2:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_3:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_4:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_5:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_6:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_7:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_8:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_9:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_10:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_11:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_12:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_13:
			case EIT_ACTUAL_SCHEDULE_TABLE_ID_14:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_15:
				s_stSvcRawSectionManager[usActionType].pSchActSvcList = pList;
				break;
			case EIT_OTHER_SCHEDULE_TABLE_ID_0:		case EIT_OTHER_SCHEDULE_TABLE_ID_1:
			case EIT_OTHER_SCHEDULE_TABLE_ID_2:		case EIT_OTHER_SCHEDULE_TABLE_ID_3:
			case EIT_OTHER_SCHEDULE_TABLE_ID_4:		case EIT_OTHER_SCHEDULE_TABLE_ID_5:
			case EIT_OTHER_SCHEDULE_TABLE_ID_6:		case EIT_OTHER_SCHEDULE_TABLE_ID_7:
			case EIT_OTHER_SCHEDULE_TABLE_ID_8:		case EIT_OTHER_SCHEDULE_TABLE_ID_9:
			case EIT_OTHER_SCHEDULE_TABLE_ID_10:	case EIT_OTHER_SCHEDULE_TABLE_ID_11:
			case EIT_OTHER_SCHEDULE_TABLE_ID_12:	case EIT_OTHER_SCHEDULE_TABLE_ID_13:
			case EIT_OTHER_SCHEDULE_TABLE_ID_14:	case EIT_OTHER_SCHEDULE_TABLE_ID_15:
				s_stSvcRawSectionManager[usActionType].pSchOthSvcList = pList;
				break;
		}

	}
	else
	{
		// 이 Triple Id에 대해서 받은 적이 있다.
		pstSectionBuildInfo = pSvcSection->astSectionBuildInfo;

		// 섹션을 받고 있거나 완성됐는데 받아놓은 섹션과 버전이 틀리다.
		if(	pstSectionBuildInfo[ucTableSlot].eBuildMode != eEpgSectionBuild_invalid
			&& pstSectionBuildInfo[ucTableSlot].ucVersion != pServiceInfo->ucVersion)
		{
			pSvcSection->eBuildMode = eEpgSectionBuild_building;
			HxLOG_Info("vesion change!! ucTableId(%x) old[%x] new[%x]\n\n", pServiceInfo->ucTableId, pstSectionBuildInfo[ucTableSlot].ucVersion, pServiceInfo->ucVersion);

			pstSectionBuildInfo[ucTableSlot].ucTableId = pServiceInfo->ucTableId;
			pstSectionBuildInfo[ucTableSlot].ucVersion = pServiceInfo->ucVersion;
			pstSectionBuildInfo[ucTableSlot].ucLastSectionNumber = pServiceInfo->ucLastSectionNumber;
			pstSectionBuildInfo[ucTableSlot].eBuildMode = eEpgSectionBuild_building;

			VK_memset32(	pstSectionBuildInfo[ucTableSlot].acSectionBuildMask,
							0xff,
							(pServiceInfo->ucLastSectionNumber/8 + 1)*sizeof(HUINT8));
			MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucLastSectionNumber);
			MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSegmentLastSectionNumber);
			MACRO_CLEARBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSectionNumber);

			*pbVersionChange = TRUE;
			hError = ERR_OK;
			HxLOG_Info("vesion change!! ucTableId(%x)\n", pServiceInfo->ucTableId);
			goto EXIT_EPGRAWDB_SECTION_FUNC;
		}

		// 이미 완성이 되었다.
		if(pstSectionBuildInfo[ucTableSlot].eBuildMode == eEpgSectionBuild_complete)
		{
			*pbConflict= TRUE;
			goto EXIT_EPGRAWDB_SECTION_FUNC;
		}

		// Invalid하다. 흠 Invalid로 셋팅한적 없는데? 뭐지?
		if(pstSectionBuildInfo[ucTableSlot].eBuildMode == eEpgSectionBuild_invalid)
		{
			pstSectionBuildInfo[ucTableSlot].ucTableId = pServiceInfo->ucTableId;
			pstSectionBuildInfo[ucTableSlot].ucVersion = pServiceInfo->ucVersion;
			pstSectionBuildInfo[ucTableSlot].ucLastSectionNumber = pServiceInfo->ucLastSectionNumber;
			pstSectionBuildInfo[ucTableSlot].eBuildMode = eEpgSectionBuild_building;

			VK_memset32(	pstSectionBuildInfo[ucTableSlot].acSectionBuildMask,
							0xff,
							(pServiceInfo->ucLastSectionNumber/8 + 1)*sizeof(HUINT8));
			MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucLastSectionNumber);
		}

		// 받았던 섹션이다.
		if(MACRO_CHECKBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSectionNumber) == 0)
		{
			*pbConflict= TRUE;
			// I think that we need double check "Table Done"..... and comment skip.
			//goto EXIT_EPGRAWDB_SECTION_FUNC;
		}
		MACRO_SEGMENTBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSegmentLastSectionNumber);
		MACRO_CLEARBITMASK(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, pServiceInfo->ucSectionNumber);
	}

/***************************************************************************
*
* check section complete
*
***************************************************************************/
	if( memcmp(pstSectionBuildInfo[ucTableSlot].acSectionBuildMask, acSectionBuildMask, (sizeof(HUINT8)*((pServiceInfo->ucLastSectionNumber >> 3) + 1))) == 0)
	{
		HxLOG_Info("\t OnId:0x%X TsId:0x%X SvcId:0x%X TableId:0x%X -- COMPLETE\n",
				pServiceInfo->usOnId,pServiceInfo->usTsId,pServiceInfo->usSvcId,pServiceInfo->ucTableId);

		pstSectionBuildInfo[ucTableSlot].eBuildMode = eEpgSectionBuild_complete;
		pSvcSection->eBuildMode = eEpgSectionBuild_complete;
		for(ucIndex = 0; ucIndex <= ucLastTableSlot; ucIndex++)
		{
			if(pstSectionBuildInfo[ucIndex].eBuildMode == eEpgSectionBuild_building)
			{
				pSvcSection->eBuildMode = eEpgSectionBuild_building;
				break;
			}
		}
	}


	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :

	if ( ERR_EPG_OK != hError )
	{
		if(pServiceInfo != NULL)
		{
			HxLOG_Error("[EPGDBRAW] TsUniqId:0x%X\n\tTblId:0x%X OnId:0x%X TsId:0x%X SvcId:0x%X SecNum:%d\n",
				usTsUniqueId,pServiceInfo->ucTableId,pServiceInfo->usOnId,pServiceInfo->usTsId,pServiceInfo->usSvcId,pServiceInfo->ucSectionNumber);
		}
		else
		{
			HxLOG_Error("[EPGDBRAW] TsUniqId:0x%X pServiceInfo == NULL\n", usTsUniqueId);
		}
	}


	VK_SEM_Release(s_stSvcRawSectionManager[usActionType].ulSemId);

	return hError;
}

STATIC HERROR			svc_epgsection_CheckSectionComplete(Handle_t hAction, HUINT8 ucTableId, HBOOL *pbComplete, HBOOL bIsEitPlus)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pList = NULL;
	POINTER_LIST_T				*pFound = NULL;
	HUINT16 	usActionType	= svc_epgsection_GetContextIdx(hAction);

	VK_SEM_Get(s_stSvcRawSectionManager[usActionType].ulSemId);

	switch(ucTableId)
	{
		case EIT_ACTUAL_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfActSvcList;
			break;
		case EIT_OTHER_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfOthSvcList;
			break;
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_0:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_1:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_2:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_3:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_4:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_5:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_6:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_7:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_8:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_9:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_10:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_11:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_12:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_13:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_14:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchActSvcList;
			break;
		case EIT_OTHER_SCHEDULE_TABLE_ID_0:		case EIT_OTHER_SCHEDULE_TABLE_ID_1:
		case EIT_OTHER_SCHEDULE_TABLE_ID_2:		case EIT_OTHER_SCHEDULE_TABLE_ID_3:
		case EIT_OTHER_SCHEDULE_TABLE_ID_4:		case EIT_OTHER_SCHEDULE_TABLE_ID_5:
		case EIT_OTHER_SCHEDULE_TABLE_ID_6:		case EIT_OTHER_SCHEDULE_TABLE_ID_7:
		case EIT_OTHER_SCHEDULE_TABLE_ID_8:		case EIT_OTHER_SCHEDULE_TABLE_ID_9:
		case EIT_OTHER_SCHEDULE_TABLE_ID_10:	case EIT_OTHER_SCHEDULE_TABLE_ID_11:
		case EIT_OTHER_SCHEDULE_TABLE_ID_12:	case EIT_OTHER_SCHEDULE_TABLE_ID_13:
		case EIT_OTHER_SCHEDULE_TABLE_ID_14:	case EIT_OTHER_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchOthSvcList;
			break;
		default :
			goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	pFound = UTIL_LINKEDLIST_IsListItemInPointerList(pList, NULL, NULL, (FILTER_FUNC)svc_epgsection_FilterBuildingTable, NULL);
	if(pFound == NULL)
	{
		*pbComplete = TRUE;
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :
	VK_SEM_Release(s_stSvcRawSectionManager[usActionType].ulSemId);

	return hError;

}

STATIC HERROR			svc_epgsection_FindSvcSection(POINTER_LIST_T *pList, svcEpg_ServiceInfo_t *pServiceInfo, SvcEpg_SvcSectionNodeRaw_t **ppSvcSection)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pFound = NULL;

	if(pServiceInfo == NULL || ppSvcSection == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	if(pList == NULL)
	{
		hError = ERR_EPG_OK;
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	*ppSvcSection = NULL;

	hError = ERR_EPG_OK;

	pFound = UTIL_LINKEDLIST_IsListItemInPointerList(pList, NULL, NULL, (FILTER_FUNC)svc_epgsection_FilterSvcSection, (void*)pServiceInfo);
	if(pFound == NULL)
	{
		HxLOG_Debug("UTIL_LINKEDLIST_IsListItemInPointerList fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	*ppSvcSection = UTIL_LINKEDLIST_GetContents(pFound);

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :

	return hError;

}


STATIC HERROR			svc_epgsection_GetSvcSectionInfo(Handle_t hAction, svcEpg_ServiceInfo_t *pSrcInfo, svcEpg_ServiceInfo_t *pDstInfo, HBOOL bIsEitPlus)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pList = NULL;
	SvcEpg_SvcSectionNodeRaw_t			*pSvcSection = NULL;
	HUINT8						ucTableSlot = 0;
	HUINT16 	usActionType	= svc_epgsection_GetContextIdx(hAction);

	VK_SEM_Get(s_stSvcRawSectionManager[usActionType].ulSemId);
	if(pSrcInfo == NULL || pDstInfo == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	switch(pSrcInfo->ucTableId)
	{
		case EIT_ACTUAL_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfActSvcList;
			ucTableSlot = 0;
			break;
		case EIT_OTHER_TABLE_ID:
			pList = s_stSvcRawSectionManager[usActionType].pPfOthSvcList;
			ucTableSlot = 0;
			break;
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_0:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_1:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_2:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_3:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_4:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_5:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_6:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_7:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_8:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_9:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_10:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_11:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_12:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_13:
		case EIT_ACTUAL_SCHEDULE_TABLE_ID_14:		case EIT_ACTUAL_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchActSvcList;
			ucTableSlot = pSrcInfo->ucTableId - EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
			break;
		case EIT_OTHER_SCHEDULE_TABLE_ID_0:		case EIT_OTHER_SCHEDULE_TABLE_ID_1:
		case EIT_OTHER_SCHEDULE_TABLE_ID_2:		case EIT_OTHER_SCHEDULE_TABLE_ID_3:
		case EIT_OTHER_SCHEDULE_TABLE_ID_4:		case EIT_OTHER_SCHEDULE_TABLE_ID_5:
		case EIT_OTHER_SCHEDULE_TABLE_ID_6:		case EIT_OTHER_SCHEDULE_TABLE_ID_7:
		case EIT_OTHER_SCHEDULE_TABLE_ID_8:		case EIT_OTHER_SCHEDULE_TABLE_ID_9:
		case EIT_OTHER_SCHEDULE_TABLE_ID_10:	case EIT_OTHER_SCHEDULE_TABLE_ID_11:
		case EIT_OTHER_SCHEDULE_TABLE_ID_12:	case EIT_OTHER_SCHEDULE_TABLE_ID_13:
		case EIT_OTHER_SCHEDULE_TABLE_ID_14:	case EIT_OTHER_SCHEDULE_TABLE_ID_15:
			pList = s_stSvcRawSectionManager[usActionType].pSchOthSvcList;
			ucTableSlot = pSrcInfo->ucTableId - EIT_OTHER_SCHEDULE_TABLE_ID_0;
			break;
		default :
			goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	hError = svc_epgsection_FindSvcSection(pList, pSrcInfo, &pSvcSection);
	if(hError != ERR_OK || pSvcSection == NULL)
	{
		HxLOG_Print("svc_epgsection_FindSvcSection fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_EPGRAWDB_SECTION_FUNC;
	}

	HxSTD_memcpy(pDstInfo, pSrcInfo, sizeof(svcEpg_ServiceInfo_t));
	pDstInfo->ucVersion = pSvcSection->astSectionBuildInfo[ucTableSlot].ucVersion;

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_SECTION_FUNC :
	VK_SEM_Release(s_stSvcRawSectionManager[usActionType].ulSemId);

	return hError;
}


STATIC HBOOL				svc_epgsection_FilterSvcSection(void *pvItem, void *pvFilter)
{
	SvcEpg_SvcSectionNodeRaw_t				*pTemp = (SvcEpg_SvcSectionNodeRaw_t*)pvItem;
	svcEpg_ServiceInfo_t 				*pFilter = (svcEpg_ServiceInfo_t*)pvFilter;

	if(pTemp == NULL || pFilter == NULL)
	{
		return FALSE;
	}

	if(	pTemp->usSvcId == pFilter->usSvcId
		&& pTemp->usTsId == pFilter->usTsId
		&& pTemp->usOnId == pFilter->usOnId)
	{
		return TRUE;
	}

	return FALSE;
}


STATIC HBOOL				svc_epgsection_FilterBuildingTable(void *pvItem, void *pvFilter)
{
	SvcEpg_SvcSectionNodeRaw_t				*pTemp = (SvcEpg_SvcSectionNodeRaw_t*)pvItem;

	HAPPY(pvFilter);

	if(pTemp == NULL )
	{
		return FALSE;
	}

	if(pTemp->eBuildMode == eEpgSectionBuild_building)
	{
		return TRUE;
	}

	return FALSE;
}
#if defined(CONFIG_DEBUG)
void SVC_EPGRAWSECTION_PrintContent(HUINT32 ulType)
{
	typedef struct tagSvcSectionNode
	{
		HUINT16 					usActionType;
		HUINT16 					usTsId;
		HUINT16 					usOnId;
		HUINT16 					usSvcId;

		/** all table id complete */
		SvcEpg_SectionBuildModRaw_t				eBuildMode;

		/** table_0 ~ table_15 */
		SvcEpg_SectionBuildInfoRaw_t		astSectionBuildInfo[16];

		struct tagSvcSectionNode		*pNext;
	}SVC_SECTION_NODE_Raw_t;

	POINTER_LIST_T	*pstPtrList, *pstPtrListHead;
	SVC_SECTION_NODE_Raw_t	*pstSvcSecNode;
	HINT32						i = 0;

	for(i = 0; i < eEPG_ACTTYPE_MAX; i++)
	{
		VK_SEM_Get(s_stSvcRawSectionManager[i].ulSemId);

		HLIB_CMD_Printf("\n(+)[%s:%d] Enter!!!!\n",__FUNCTION__,__LINE__);

		HLIB_CMD_Printf("== s_stSvcRawSectionManager Content ==\n");
		HLIB_CMD_Printf("  actionType:%d ulSemId:0x%x usTsUniqueId:0x%X \n",
			i,
			s_stSvcRawSectionManager[i].ulSemId,
			s_stSvcRawSectionManager[i].usTsUniqueId);

		if(1)//0 == ulType)
		{
			if ( NULL != s_stSvcRawSectionManager[i].pPfActSvcList)
			{
				HLIB_CMD_Printf("	  - PF ACT SVC LIST\n");
				pstPtrListHead = s_stSvcRawSectionManager[i].pPfActSvcList;
				pstPtrList = pstPtrListHead;
				while (pstPtrList)
				{
					HUINT32 j = 0;
					pstSvcSecNode = (SVC_SECTION_NODE_Raw_t *)pstPtrList->pvContents;
					HLIB_CMD_Printf("\tOnId:0x%X TsId:0x%X SvcId:0x%X eBuildMode:0x%X\n",
							pstSvcSecNode->usOnId, pstSvcSecNode->usTsId, pstSvcSecNode->usSvcId, pstSvcSecNode->eBuildMode);

					while(j < 16)
					{
						if(pstSvcSecNode->astSectionBuildInfo[j].eBuildMode != eEpgSectionBuild_invalid)
						{
							HLIB_CMD_Printf("\t\t TblId:0x%02X Ver:0x%02X LastSec:0x%02X eBuildMode:0x%X\n",
								pstSvcSecNode->astSectionBuildInfo[j].ucTableId, pstSvcSecNode->astSectionBuildInfo[j].ucVersion, pstSvcSecNode->astSectionBuildInfo[j].ucLastSectionNumber,
								pstSvcSecNode->astSectionBuildInfo[j].eBuildMode);
						}
						j++;
					}

					if (pstPtrList->pNext == pstPtrListHead)
						break;
					pstPtrList = pstPtrList->pNext;
				}
			}
			if ( NULL != s_stSvcRawSectionManager[i].pPfOthSvcList)
			{
				HLIB_CMD_Printf("	  - PF OTH SVC LIST\n");

				pstPtrListHead = s_stSvcRawSectionManager[i].pPfOthSvcList;
				pstPtrList = pstPtrListHead;
				while (pstPtrList)
				{
					HUINT32 j = 0;

					pstSvcSecNode = (SVC_SECTION_NODE_Raw_t *)pstPtrList->pvContents;
					HLIB_CMD_Printf("\tOnId:0x%X TsId:0x%X SvcId:0x%X eBuildMode:0x%X\n",
							pstSvcSecNode->usOnId, pstSvcSecNode->usTsId, pstSvcSecNode->usSvcId, pstSvcSecNode->eBuildMode);

					while(j < 16)
					{
						if(pstSvcSecNode->astSectionBuildInfo[j].eBuildMode != eEpgSectionBuild_invalid)
						{
							HLIB_CMD_Printf("\t\t TblId:0x%02X Ver:0x%02X LastSec:0x%02X eBuildMode:0x%X\n",
								pstSvcSecNode->astSectionBuildInfo[j].ucTableId, pstSvcSecNode->astSectionBuildInfo[j].ucVersion, pstSvcSecNode->astSectionBuildInfo[j].ucLastSectionNumber,
								pstSvcSecNode->astSectionBuildInfo[j].eBuildMode);
						}
						j++;

					}
					if (pstPtrList->pNext == pstPtrListHead)
						break;
					pstPtrList = pstPtrList->pNext;
				}
			}
		}
		else
		{
			if ( NULL != s_stSvcRawSectionManager[i].pSchActSvcList)
			{
				HLIB_CMD_Printf("	  - SC ACT SVC LIST\n");

				pstPtrListHead = s_stSvcRawSectionManager[i].pSchActSvcList;
				pstPtrList = pstPtrListHead;
				while (pstPtrList)
				{
					HUINT32 j = 0;

					pstSvcSecNode = (SVC_SECTION_NODE_Raw_t *)pstPtrList->pvContents;
					HLIB_CMD_Printf("\tOnId:0x%X TsId:0x%X SvcId:0x%X eBuildMode:0x%X\n",
							pstSvcSecNode->usOnId, pstSvcSecNode->usTsId, pstSvcSecNode->usSvcId, pstSvcSecNode->eBuildMode);

					while(j < 16)
					{
						if(pstSvcSecNode->astSectionBuildInfo[j].eBuildMode != eEpgSectionBuild_invalid)
						{
							HLIB_CMD_Printf("\t\t TblId:0x%02X Ver:0x%02X LastSec:0x%02X eBuildMode:0x%X\n",
								pstSvcSecNode->astSectionBuildInfo[j].ucTableId, pstSvcSecNode->astSectionBuildInfo[j].ucVersion, pstSvcSecNode->astSectionBuildInfo[j].ucLastSectionNumber,
								pstSvcSecNode->astSectionBuildInfo[j].eBuildMode);
						}
						j++;

					}

					if (pstPtrList->pNext == pstPtrListHead)
						break;
					pstPtrList = pstPtrList->pNext;
				}
			}
			if ( NULL != s_stSvcRawSectionManager[i].pSchOthSvcList)
			{
				HLIB_CMD_Printf("	  - SC OTH SVC LIST\n");

				pstPtrListHead = s_stSvcRawSectionManager[i].pSchOthSvcList;
				pstPtrList = pstPtrListHead;
				while (pstPtrList)
				{
					HUINT32 j = 0;

					pstSvcSecNode = (SVC_SECTION_NODE_Raw_t *)pstPtrList->pvContents;
					HLIB_CMD_Printf("\tOnId:0x%X TsId:0x%X SvcId:0x%X eBuildMode:0x%X\n",
							pstSvcSecNode->usOnId, pstSvcSecNode->usTsId, pstSvcSecNode->usSvcId, pstSvcSecNode->eBuildMode);
					while(j < 16)
					{
						if(pstSvcSecNode->astSectionBuildInfo[j].eBuildMode != eEpgSectionBuild_invalid)
						{
							HLIB_CMD_Printf("\t\t TblId:0x%02X Ver:0x%02X LastSec:0x%02X eBuildMode:0x%X\n",
								pstSvcSecNode->astSectionBuildInfo[j].ucTableId, pstSvcSecNode->astSectionBuildInfo[j].ucVersion, pstSvcSecNode->astSectionBuildInfo[j].ucLastSectionNumber,
								pstSvcSecNode->astSectionBuildInfo[j].eBuildMode);
						}
						j++;
					}

					if (pstPtrList->pNext == pstPtrListHead)
						break;
					pstPtrList = pstPtrList->pNext;
				}
			}
		}

		VK_SEM_Release(s_stSvcRawSectionManager[i].ulSemId);

	}
}
#endif

