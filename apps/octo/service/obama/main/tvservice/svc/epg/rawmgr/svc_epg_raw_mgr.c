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
#include <octo_common.h>
#include <vkernel.h>

#include <svc_epg.h>
#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_common_lib.h"
#include "../local_include/_svc_epg_raw_mgr.h"
#include "_svc_epg_raw_pool.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL_EPGRAW								\
{																\
	VK_SEM_Get(sulEpgRawSemaphoreId);						\
}

#define	LEAVE_CRITICAL_EPGRAW								\
{																\
	VK_SEM_Release(sulEpgRawSemaphoreId);					\
}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32				sulEpgRawSemaphoreId = 0;
static svcEpg_RawPool_t		*s_pstEpgRawDataPool = NULL;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR				svc_epgrawmgr_InitResource(void);
STATIC HUINT32				svc_epgrawmgr_GetSemaphoreId(void);
STATIC HERROR				svc_epgrawmgr_GetRawPool(svcEpg_RawPool_t **ppstRawPool);
STATIC HERROR				svc_epgrawmgr_ClearRawPool (void);
STATIC HERROR				svc_epgrawmgr_AddRawSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec);
STATIC HERROR				svc_epgrawmgr_FreeAllSection(HUINT16 usUniqueId, SiTableType_e eSiTableType);
STATIC HERROR				svc_epgrawmgr_FreeSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId);

#define ____GLOBAL_FUNC____
HERROR			SVC_EPGRAWMGR_InitResource(void)
{
	return svc_epgrawmgr_InitResource();
}

HUINT32			SVC_EPGRAWMGR_GetSemaphoreId(void)
{
	return svc_epgrawmgr_GetSemaphoreId();
}

HERROR			SVC_EPGRAWMGR_GetRawPool (svcEpg_RawPool_t **ppstRawPool)
{
	return svc_epgrawmgr_GetRawPool(ppstRawPool);
}

HERROR			SVC_EPGRAWMGR_ClearRawPool (void)
{
	return svc_epgrawmgr_ClearRawPool();
}

HERROR			SVC_EPGRAWMGR_AddRawSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	return svc_epgrawmgr_AddRawSection(usUniqueId, eSiTableType, pucRawSec);
}

HERROR			SVC_EPGRAWMGR_FreeAllSection(HUINT16 usUniqueId, SiTableType_e eSiTableType)
{
	return svc_epgrawmgr_FreeAllSection(usUniqueId, eSiTableType);
}


HERROR			SVC_EPGRAWMGR_FreeSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId)
{
	return svc_epgrawmgr_FreeSection(usUniqueId, eSiTableType, bUseExtId, usExtId);
}

#define ____LOCAL_FUNC____

STATIC HERROR				svc_epgrawmgr_InitResource(void)
{
	HINT32				lResult = 0;
	HERROR				hErr = ERR_EPG_FAIL;
	HUINT32				ulSemId = 0;
	svcEpg_RawPool_t			*pstRawPool = NULL;

	lResult = VK_SEM_Create((unsigned long*)&ulSemId, "Epg_Raw_Sem", VK_SUSPENDTYPE_PRIORITY);
	if(lResult != VK_OK)
	{
		HxLOG_Print("VK_SEM_Create fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}
	sulEpgRawSemaphoreId = ulSemId;

	pstRawPool = svc_epg_RawPoolAlloc(0, (HUINT8*)"EPGRAW");
	if(pstRawPool == NULL)
	{
		HxLOG_Print("AllocRawPool  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}
	s_pstEpgRawDataPool = pstRawPool;

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :

	return hErr;
}

STATIC HUINT32			svc_epgrawmgr_GetSemaphoreId(void)
{
	return sulEpgRawSemaphoreId;
}


STATIC HERROR				svc_epgrawmgr_GetRawPool(svcEpg_RawPool_t **ppstRawPool)
{
	HERROR				hErr = ERR_EPG_FAIL;

	if(ppstRawPool == NULL)
	{
		HxLOG_Print("param  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	*ppstRawPool = s_pstEpgRawDataPool;

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :

	return hErr;
}

STATIC HERROR				svc_epgrawmgr_ClearRawPool(void)
{
	HERROR				 hErr = ERR_EPG_FAIL;
	svcEpg_RawPool_t	*pstRawPool = NULL;

	ENTER_CRITICAL_EPGRAW;
	hErr = svc_epgrawmgr_GetRawPool(&pstRawPool);
	if(hErr != ERR_EPG_OK)
	{
		HxLOG_Print("GetRawPool  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	svc_epg_RawPoolClear(pstRawPool);

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :

	LEAVE_CRITICAL_EPGRAW;

	return hErr;
}



STATIC HERROR				svc_epgrawmgr_AddRawSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	HERROR				 hErr = ERR_EPG_FAIL;
	svcEpg_RawPool_t	*pstRawPool = NULL;

	ENTER_CRITICAL_EPGRAW;
	hErr = svc_epgrawmgr_GetRawPool(&pstRawPool);
	if(hErr != ERR_EPG_OK)
	{
		HxLOG_Print("GetRawPool  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	hErr = (HERROR)svc_epg_RawPoolAddSection(pstRawPool, usUniqueId, eSiTableType, pucRawSec);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("svc_epg_RawPoolAddSection  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :
	LEAVE_CRITICAL_EPGRAW;

	return hErr;
}

STATIC HERROR				svc_epgrawmgr_FreeAllSection(HUINT16 usUniqueId, SiTableType_e eSiTableType)
{
	HERROR				 hErr = ERR_EPG_FAIL;
	svcEpg_RawPool_t	*pstRawPool = NULL;

	ENTER_CRITICAL_EPGRAW;
	hErr = svc_epgrawmgr_GetRawPool(&pstRawPool);
	if(hErr != ERR_EPG_OK)
	{
		HxLOG_Print("GetRawPool  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	svc_epg_RawPoolFreeAllSection(pstRawPool, usUniqueId, eSiTableType);

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :
	LEAVE_CRITICAL_EPGRAW;

	return hErr;

}

STATIC HERROR				svc_epgrawmgr_FreeSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId)
{
	HERROR				 hErr = ERR_EPG_FAIL;
	svcEpg_RawPool_t	*pstRawPool = NULL;

	ENTER_CRITICAL_EPGRAW;
	hErr = svc_epgrawmgr_GetRawPool(&pstRawPool);
	if(hErr != ERR_EPG_OK)
	{
		HxLOG_Print("GetRawPool  fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	hErr = (HERROR)svc_epg_RawPoolFreeSection(pstRawPool, usUniqueId, eSiTableType, bUseExtId, usExtId);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("svc_epg_RawPoolFreeSection fail!!\n");
		goto EXIT_EPGRAW_FUNC;
	}

	hErr = ERR_EPG_OK;

EXIT_EPGRAW_FUNC :
	LEAVE_CRITICAL_EPGRAW;

	return hErr;
}

/*********************** End of File ******************************/
