/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_int.h
	@brief

	Description:  									\n
	Module: Umma Internal Header                    \n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

#ifndef __UMMA_INT_H__
#define __UMMA_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	<sys/stat.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define UMMA_CHECK_AVAILABLE_MOUNTPATH

#ifdef UMMA_CHECK_AVAILABLE_MOUNTPATH
#define	UMMA_FM_CHECK_TIMEOUT 2000
#define	UMMA_FS_CHECK_TIMEOUT 5000
#endif

#define	UMMA_UNDEFINED			0

#define	UMMA_RECYCLEBIN_NAME	".recycleBin"

#define	UMMA_ASSERT(expr)			\
	if (!(expr)) {\
		HxLOG_Error("UMMA_ASSERT : Argument is not valid!!! \n");\
		abort();\
	}

#define UMMA_Malloc			HLIB_STD_MemAlloc
#define UMMA_Calloc			HLIB_STD_MemCalloc
#define	UMMA_Realloc		HLIB_STD_MemRealloc
#define	UMMA_StrDup			HLIB_STD_StrDup
#define	UMMA_MemDup			HLIB_STD_MemDup
#define UMMA_Free			HLIB_STD_MemFree
#define	UMMA_Malloc_CB		HLIB_STD_MemAlloc_CB
#define	UMMA_Free_CB		HLIB_STD_MemFree_CB

#define ERR_NO_DEVICE		(ERR_FAIL + 1)

#define NEW_OPERATION_LIST

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eUMMA_EVT_STORAGE_ADDED = 0,
	eUMMA_EVT_STORAGE_REMOVED,
	eUMMA_EVT_STORAGE_FORMAT_START,
	eUMMA_EVT_STORAGE_FORMAT_DONE,
	eUMMA_EVT_LAST

}UMMA_EVENT_e;


typedef struct
{
	HINT32		nEvent;

	HINT32		nParam1;
	HINT32		nParam2;
	HINT32		nParam3;
//	HUINT32		ulDummySignal;
} UMMA_EVT_t;

typedef struct
{
	HUINT32		ulSignal;

	HUINT32		ulParam1;
	HUINT32		ulParam2;
	HUINT32		ulParam3;
	HUINT32		ulParam4;
	HINT32		nReqId;
} UMMA_SIGNAL_t;

#if defined(NEW_OPERATION_LIST)
typedef struct
{
	HUINT32			 ulCmd;

	HxList_t		*pstHead;
	HxList_t		*pstTail;
} UMMA_OperationQueue_t;

typedef struct
{
	HUINT32					 ulNumHierachy;
	UMMA_OperationQueue_t	*pstQueueArray;
} UMMA_OperationPriorQueue_t;
#endif

typedef struct
{
	HUINT32		ulSemId;

	HINT32		nServerHandle;
	HINT32		nRpcHandle;
	HUINT32		ulMsgQueue;

	HxCBUF_t		nCBuf;
#if defined(NEW_OPERATION_LIST)
	HBOOL						 bUseOprPriorQueue;
	UMMA_OperationPriorQueue_t	 stOperationQueue;
#endif
	HxList_t	*pstOperations;

	HxList_t	*pstSignalHandler;
	HxList_t	*pstMountInfo;

	HxList_t	*pstEventList;

	HxHASH_t	*pstRequestList;

	HCHAR		szFlushName[1024];
	HCHAR		szRpcTemp[1024];

	HBOOL		bTryFormat;
	HINT32 		nRequestId;
	HUINT32 	ulDevId;
	HUINT32 	ulVolLab;
} UMMA_t;

typedef struct
{
	HINT32		nMessage;

	HINT32		nParam1;
	HINT32		nParam2;
	HINT32		nParam3;
//	HUINT32		ulDummySignal;
} UMMA_MSG_t;


#endif
/* end of file */

void	UMMA_SendEvent(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3);
HERROR		UMMA_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle);

HBOOL		UMMA_OperationQueueEmpty (UMMA_t *pstUmma);
HERROR		UMMA_OperationQueueRemoveItem (UMMA_t *pstUmma, const void *pvOperation);
HERROR		UMMA_OperationQueueRemoveItemIf (UMMA_t *pstUmma, void *pvUser, HBOOL (*pfnCondition)(void *userdata, void *data), void (*pfnFree)(void *data));
HERROR		UMMA_OperationQueueRemoveAllFunc (UMMA_t *pstUmma, void (*pfnFree)(void *data));
HUINT32		UMMA_OperationQueueInsertWithCmdPriority (UMMA_t *pstUmma, void *pvOperation);
HUINT32		UMMA_OperationQueueLength (UMMA_t *pstUmma);
HERROR		UMMA_OperationQueueForEach (UMMA_t *pstUmma, void *pvUserData, void (*pfnEachJob)(UMMA_t *umma, void *operation, void *userdata));
void *		UMMA_OperationQueueGetCurrent (UMMA_t *pstUmma);


