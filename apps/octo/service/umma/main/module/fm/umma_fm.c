/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_fm.c
	@brief

	Description:  									\n
	Module: Umma API Module			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<dirent.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<dlib.h>
#include	<uapi.h>
#include	<umma_int.h>
#include	<umma_fm.h>
#include	<umma_fs.h>
#include 	<papi.h>
#include	<hapi.h>
#include	<dapi.h>
#include	<oapi.h>
#if defined(CONFIG_FUNC_USE_DECRYPTCOPY) /* direct M2M decryption thru shared CDI(chip driver interface) */
#include "di_err.h"
#include "di_crypt.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	IS_NOT_DOT(PATH)		(PATH[0] != '.')
#define	SUPPORT_WEBAPP			//	현재 WebApp의 구현 상태에 맞게 umma동작 변경 --> 해당 디파인이 없으면 linux일반 모드와 동일하게 동작함.

#define	ENTER_OPERATIONS_SEM		HxSEMT_Get(s_ulOperationsSemId)
#define	LEAVE_OPERATIONS_SEM		HxSEMT_Release(s_ulOperationsSemId)
//#define	DUMP_STACK
#if defined(CONFIG_PROD_OS_EMULATOR)
#define stat emu_stat
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define		UMMA_FM_SIGNAL_MOUNT		1
#define		UMMA_FM_SIGNAL_UNMOUNT		2
#define		UMMA_FM_SIGNAL_SCAN			3
#define		UMMA_FM_SIGNAL_REMOVE		4
#define		UMMA_FM_SIGNAL_MOVE			5
#define		UMMA_FM_SIGNAL_COPY			6
#define		UMMA_FM_SIGNAL_CANCEL		7
#define		UMMA_FM_SIGNAL_MKDIR		8
#define		UMMA_FM_SIGNAL_STARTJOB		9

#define 	UMMA_FM_SIGNAL_SHUTDOWN  	10
#define 	UMMA_FM_SIGNAL_OPERATION 	11
#define 	UMMA_FM_SIGNAL_SHUTDOWNDONE 12

#define 	UMMA_FM_SIGNAL_OPER_GROUP 	13
#define		UMMA_FM_SIGNAL_FORMAT		14

#define 	UMMA_FM_SIGNAL_REBOOT		15

#define 	UMMA_FM_SIGNAL_FLUSH		16

#ifdef DUMP_STACK
struct vk_task_arg {
	pthread_mutex_t lock_;
	pthread_cond_t cond_;
	unsigned long priority_;
	unsigned long stackSize_;
	int IsJoinable;
	int start;
	unsigned long *pucTaskStack;
	unsigned long *pulStackStartAddress;
	void (*start_func_)(void *);
	void *arg_;
#if defined(CONFIG_DEBUG)
	pid_t pid;
	pid_t tid;
	int	thread_id;
#endif
	unsigned long task_id;
	char name[32];
};

struct vk_task {
	pthread_t task_;
	struct vk_task_arg targ_;
};
#endif

typedef struct {
	HINT32 nReqId;
	HUINT32 nHndId;
	UAPI_FM_GROUP_OPER_e eOper;

	HINT32 nDoneCnt;
	HINT32 nItemCnt;
	HUINT64 nTotalSize;
	HUINT64 nDoneSize;

	HCHAR szDestPath[512];

} UMMA_FM_OPER_GROUP_t;

typedef struct{
	DxStorageInterface_e eInterface;
	HUINT64	ullReservedSizeKb;
}UMMA_FM_StorageInfo_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static UMMA_t	*s_pstUmmaFm = NULL;
static HUINT32	s_ulOperationsSemId = 0;

// Mount시 scan할 확장자
static HCHAR	s_szFullExtList[] = {"3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb|smi|sami|smil|srt|sub|txt|ass|ssa|psb|jpg|jpeg|bmp|png|gif|tif|mp3|wav|f4a|m4a|flac"};
static HUINT32	s_unSignalSema = 0;
/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		umma_fm_mainLoop(void *arg);
static UMMA_OPS_t*	umma_fm_AddOperation(UMMA_t *pstUmma, UMMA_CMD_e eCmd, UMMA_CMD_e ePostCmd, HINT32 nRecursive, HINT32 nFlags, void *pvUser, const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nRequestId);
static void		umma_fm_FreeOperationCBCancel(void *pvData);
static UMMA_MOUNTINFO_t*	umma_fm_GetMountInfo(UMMA_t *pstUmma, const HCHAR *pszName);
static HERROR	umma_fm_operation_ScanResult(UMMA_t *pstUmma, void *pvUserData, const HCHAR *pszName, struct stat *fstat);

static HERROR	umma_fm_debug_PrintJobList(UMMA_t *pstUmma, UMMA_CMD_e eFilterCmd);
static HERROR	umma_fm_operation_MoveFileToRecycleBin(UMMA_t *pstUmma, const HCHAR *pszName);

#ifdef	DUMP_STACK
static HUINT32	s_tid;
#endif
static UMMA_t*	umma_fm_getInstance(void)
{
	if (s_pstUmmaFm == NULL)
	{
		HERROR hErr;
		unsigned long	tid;

		s_pstUmmaFm = (UMMA_t*)UMMA_Calloc(sizeof(UMMA_t));

#if defined(NEW_OPERATION_LIST)
{
		HUINT32 		 ulCnt;

		s_pstUmmaFm->bUseOprPriorQueue = TRUE;
		s_pstUmmaFm->stOperationQueue.ulNumHierachy = eUMMA_CMD_MAX;
		s_pstUmmaFm->stOperationQueue.pstQueueArray = (UMMA_OperationQueue_t *)UMMA_Calloc(sizeof(UMMA_OperationQueue_t) * eUMMA_CMD_MAX);

		for (ulCnt = 0; ulCnt < eUMMA_CMD_MAX; ulCnt++)
		{
			s_pstUmmaFm->stOperationQueue.pstQueueArray[ulCnt].ulCmd = ulCnt;
		}
}
#endif

		HxSEMT_Create(&s_pstUmmaFm->ulSemId, "ummafmsem", 0);
		HxSEMT_Get(s_pstUmmaFm->ulSemId);
		HxSEMT_Create(&s_ulOperationsSemId, "ummafmopersem", 0);

		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaFm->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaFm->nRpcHandle);
		}

		s_pstUmmaFm->nCBuf         = HLIB_CBUF_New(0x8000);
		s_pstUmmaFm->pstRequestList= HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UMMA_Free_CB);

		//	UMMA MainTask의 Wakeup을 위한 Event.
		HLIB_MSG_Create(128, sizeof(UMMA_EVT_t), "ummafmEvt", (unsigned long*)&s_pstUmmaFm->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO);


		VK_TASK_Create(umma_fm_mainLoop, 70, 1024 * 1024/*SIZE_64K*/, "ummafmMainHandler", (void*)s_pstUmmaFm, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
#ifdef	DUMP_STACK
		s_tid = tid;
		HxLOG_Print("---> tid : [%p]\n", tid);
#endif
	}
	else
	{
		HxSEMT_Get(s_pstUmmaFm->ulSemId);
	}

	return s_pstUmmaFm;
}

static UMMA_t*	umma_fm_getInstanceWO(void)
{
	return s_pstUmmaFm;
}

static void		umma_fm_releaseInstance(UMMA_t *pstUmma)
{
	HxSEMT_Release(pstUmma->ulSemId);
}

#ifdef UMMA_CHECK_AVAILABLE_MOUNTPATH
static DxStorageInterface_e umma_fm_getInterface(DxStorage_DevType_e  eDevType)
{
	switch (eDevType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:
		case eDxSTORAGE_DEVTYPE_DETACHABLE:
		case eDxSTORAGE_DEVTYPE_ESATAHDD:
			return eDxINTERFACE_SATA;


		case eDxSTORAGE_DEVTYPE_USBHDD:
		case eDxSTORAGE_DEVTYPE_USBMEM:
			return eDxINTERFACE_USB;

		case eDxSTORAGE_DEVTYPE_MMC:
			return eDxINTERFACE_SD;

		default:
			return eDxINTERFACE_UNKNOWN;
	}

}
#endif

static inline void umma_fm_safety_operation_remove( UMMA_t *pstUmma, void *pstOps )
{
	ENTER_OPERATIONS_SEM;
	(void)UMMA_OperationQueueRemoveItem(pstUmma, (const void *)pstOps);
	LEAVE_OPERATIONS_SEM;
}

static HINT32	umma_fm_CompareOperationPriority(void *a, void *b)
{
	UMMA_OPS_t	*pstSrc, *pstDst;

	pstSrc = (UMMA_OPS_t*)a;
	pstDst = (UMMA_OPS_t*)b;

	return pstDst->eCmd - pstSrc->eCmd;
}

//	fire event
static void		umma_fm_requestcallback_fire(UMMA_t *pstUmma, UMMA_REQINFO_t *pstReqInfo)
{
#define	UMMA_CALLBACK_DURATION		1000
	HUINT32		ulSystemTick = HLIB_STD_GetSystemTick();

	if ((pstReqInfo->nOperationCount == pstReqInfo->nOperationDoneCount) ||
		((ulSystemTick - pstReqInfo->ulLastCallback) > UMMA_CALLBACK_DURATION))
	{
		UAPI_STATUS_t	stStatus;

		HxLOG_Debug("fire signal req[%d] [%d] [%d, %d]\n", pstReqInfo->nRequestId, pstReqInfo->nPreProcessDone, pstReqInfo->nOperationCount, pstReqInfo->nOperationDoneCount);
		stStatus.nTotalOperationNum = pstReqInfo->nOperationCount;
		stStatus.nDoneOperationNum  = pstReqInfo->nOperationDoneCount;
		stStatus.ulErr				= pstReqInfo->ulErr;
		stStatus.bDonePreProcess    = (pstReqInfo->nPreProcessDone ? TRUE : FALSE);

		//	if operation is finished, call dapi_lazyupdate for updating db.
		if (stStatus.nDoneOperationNum == stStatus.nTotalOperationNum)
		{
			DAPI_LazyUpdate(DxNAMEOF(DxFileInfo_t));
			DAPI_WaitServerSync();
		}

		HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onStatusChange, "ib", pstReqInfo->nRequestId, &stStatus, sizeof(UAPI_STATUS_t));

		pstReqInfo->ulLastCallback = ulSystemTick;
	}

	if (pstReqInfo->nOperationCount == pstReqInfo->nOperationDoneCount)
	{
		HxLOG_Debug("Job Finished! done count(%d)\n", pstReqInfo->nOperationDoneCount);
		HLIB_HASH_Remove(pstUmma->pstRequestList, (void*)pstReqInfo->nRequestId);
	}
}

static inline void	umma_fm_notify_operation_error(UMMA_t *pstUmma, HINT32 nRequestId, HUINT32 nReason )
{
	UAPI_STATUS_t	stStatus;
	HxSTD_MemSet( &stStatus, 0x00, sizeof(UAPI_STATUS_t) );

	stStatus.ulErr = nReason;
	if ( UMERR_OK == nReason )
	{
		stStatus.nTotalOperationNum = -1;
	}
	VK_TASK_Sleep(100);
	HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onStatusChange, "ib", nRequestId, &stStatus, sizeof(UAPI_STATUS_t));
}

static HERROR	umma_fm_operation_UpdateFileInfo(UMMA_t *pstUmma, const HCHAR *pszName, HBOOL bDir, UMMA_UPDATE_e eMode)
{
	HCHAR	*pszSQL;
	UMMA_MOUNTINFO_t *pMountInfo = NULL;

	pszSQL = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);
	switch (eMode)
	{
		case eUMMA_UPDATE_ADDED:
			pMountInfo = umma_fm_GetMountInfo(pstUmma,pszName);
			HxLOG_Print("\n[UMMA_FM] Add Path : %s\n", pszName );
			if(pMountInfo)
			{
#if defined(NEW_OPERATION_LIST)
				if ((TRUE == bDir) || (HxSTD_StrStr(pszName, "*") != NULL))
				{
					umma_fm_AddOperation(pstUmma, eUMMA_CMD_DIRECTDIR, eUMMA_CMD_GETINFO, FALSE, UMMA_UNDEFINED, (void*)pMountInfo, pszName, NULL, NULL, 0);
				}
				else
				{
					umma_fm_AddOperation(pstUmma, eUMMA_CMD_GETINFO, eUMMA_CMD_NONE, FALSE, UMMA_UNDEFINED, (void*)pMountInfo, pszName, NULL, NULL, 0);
				}
#else
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_DIRECTDIR, eUMMA_CMD_GETINFO, FALSE, UMMA_UNDEFINED, (void*)pMountInfo, pszName, NULL, NULL, 0);
#endif
			}

			break;
		case eUMMA_UPDATE_REMOVED:
			HxLOG_Print("\n[UMMA_FM] Rmove Path : %s\n", pszName );
			if (bDir)
			{
				HxSTD_snprintf(pszSQL, 1024, "delete from DxFileInfo_t where path = \"%s\" ", pszName);
			} else
			{
				HxSTD_snprintf(pszSQL, 1024, "delete from DxFileInfo_t where \"%s\" = path || \"/\"|| name", pszName);
			}
			HxLOG_Print("\n[UMMA_FM] Removed Path : %s, query :%s \n", pszName, pszSQL );
			DAPI_Query(pszSQL, NULL, NULL);
			break;
		case eUMMA_UPDATE_REMOVED_LIKE:
			if ( bDir == TRUE && !HxSTD_StrEmpty(pszName) )
			{
				HxLOG_Print("[UMMA_FM] delete DxFileInfo_t. pszName : %s \n", pszName );
				HxSTD_snprintf(pszSQL, 1024, "delete from DxFileInfo_t where path like \"%s%%\" ", pszName);
				DAPI_Query(pszSQL, NULL, NULL);
			} else
			{
				HxLOG_Error("Not supported removed like function with file.\n");
			}
			break;
		default:
			HxLOG_Error("Undefined update mode [%d]\n", eMode);
			break;
	}

	return ERR_OK;
}



static HBOOL	umma_fm_compare_SameSrcName(void *userdata, void *data)
{
	UMMA_OPS_t	*pstCmpOps = (UMMA_OPS_t*)userdata;
	UMMA_OPS_t	*pstListOps = (UMMA_OPS_t*)data;

	if( HxSTD_StrEmpty(pstCmpOps->pszSourceName)== TRUE || HxSTD_StrEmpty(pstListOps->pszSourceName)==TRUE)
		return FALSE;

	if (HxSTD_StrCmp(pstCmpOps->pszSourceName, pstListOps->pszSourceName) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL	umma_fm_compare_SameCmdNSrcName(void *userdata, void *data)
{
	UMMA_OPS_t	*pstCmpOps = (UMMA_OPS_t*)userdata;
	UMMA_OPS_t	*pstListOps = (UMMA_OPS_t*)data;

	if( HxSTD_StrEmpty(pstCmpOps->pszSourceName)== TRUE || HxSTD_StrEmpty(pstListOps->pszSourceName)==TRUE)
		return FALSE;

	if ( (pstCmpOps->eCmd == pstListOps->eCmd) &&
		(HxSTD_StrCmp(pstCmpOps->pszSourceName, pstListOps->pszSourceName) == 0))
	{
		return TRUE;
	}

	return FALSE;
}

//	add job to priority queue.
static UMMA_OPS_t*	umma_fm_AddOperation(UMMA_t *pstUmma, UMMA_CMD_e eCmd, UMMA_CMD_e ePostCmd, HINT32 nRecursive, HINT32 nFlags, void *pvUser, const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HINT32 nRequestId)
{
	UMMA_OPS_t	*pstOps;
	UMMA_EVT_t	stEvt;

	//	Operation 입력시 --> Main Task Wakeup용도.
	if (UMMA_OperationQueueEmpty(pstUmma))
	{
		HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
		HLIB_MSG_Send(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
	}

	pstOps = (UMMA_OPS_t*)UMMA_Calloc(sizeof(UMMA_OPS_t));
	pstOps->eCmd = eCmd;
	pstOps->ePostCmd = ePostCmd;
	pstOps->nRecursive = nRecursive;
	pstOps->nFlags = nFlags;
	pstOps->pszSourceName = HLIB_STD_StrDup(pszSource);
	pstOps->pszDestName = HLIB_STD_StrDup(pszDest);
	pstOps->pszExtFilter = HLIB_STD_StrDup(pszExtFilter);
	pstOps->nRequestId = nRequestId;
	pstOps->pvUser = pvUser;

	ENTER_OPERATIONS_SEM;

#if !defined(NEW_OPERATION_LIST)
	if (pstOps->eCmd == eUMMA_CMD_GETINFO_PRIO)
	{
		 //같은 pszSourceName으로 더 낮은 우선 순위의 Operation이 등록되어있다면 삭제
		pstUmma->pstOperations = HLIB_LIST_RemoveIf(pstUmma->pstOperations, (void*)pstOps, umma_fm_compare_SameSrcName, umma_fm_FreeOperationCBCancel);
	}
	else if (pstOps->eCmd == eUMMA_CMD_GETINFO)
	{
		// 같은 SourceName의 중복된 eUMMA_CMD_GETINFO라면 삭제 한다.
		pstUmma->pstOperations = HLIB_LIST_RemoveIf(pstUmma->pstOperations, (void*)pstOps, umma_fm_compare_SameCmdNSrcName, umma_fm_FreeOperationCBCancel);
	}
#endif

	(void)UMMA_OperationQueueInsertWithCmdPriority(pstUmma, (void *)pstOps);

	LEAVE_OPERATIONS_SEM;

	//	Update Request List with adding
	{
		UMMA_REQINFO_t	*pstReqInfo;

		pstReqInfo = (UMMA_REQINFO_t*)HLIB_HASH_Lookup(pstUmma->pstRequestList, (void*)nRequestId);
		if (pstReqInfo == NULL)
		{
			pstReqInfo = (UMMA_REQINFO_t*)UMMA_Calloc(sizeof(UMMA_REQINFO_t));
			pstReqInfo->nRequestId = nRequestId;
			HLIB_HASH_Insert(pstUmma->pstRequestList, (void*)nRequestId, (void*)pstReqInfo);
		}
		pstReqInfo->nOperationCount++;

		pstOps->pstReqInfo = pstReqInfo;
		umma_fm_requestcallback_fire(pstUmma, pstReqInfo);
	}

	return pstOps;
}

static void		umma_fm_FreeOperation(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	if (pstOps)
	{
		UMMA_REQINFO_t	*pstReqInfo;

		if (pstUmma == NULL)
			pstUmma = umma_fm_getInstanceWO();

		HxLOG_Print("[%s:%d]CMD[%d],src[%s],dest[%s]\n",__FUNCTION__,__LINE__,pstOps->eCmd,pstOps->pszSourceName,pstOps->pszDestName);
		if (pstOps->ulErr != UMERR_REASON_CANCELED)
		{
			HBOOL bIsDirDest		= FALSE;
			HBOOL bIsDirSrc			= FALSE;
			HBOOL bRecordings		= FALSE;
			HBOOL bChunkedForMedia	= FALSE;

			switch (pstOps->eCmd)
			{
				case eUMMA_CMD_COPY:
					bRecordings			=	(pstOps->nFlags & UAPI_COPY_FLAG_RECORDINGS)?		TRUE:FALSE;
					bChunkedForMedia	=	(pstOps->nFlags & UAPI_COPY_FLAG_CHUNKFORMEDIA)?	TRUE:FALSE;

					bIsDirDest = HLIB_DIR_IsDirPath(pstOps->pszDestName);
					if (bRecordings == FALSE && bIsDirDest == TRUE && bChunkedForMedia == FALSE)
					{
						break;
					}
					umma_fm_operation_UpdateFileInfo(pstUmma, pstOps->pszDestName, bIsDirDest, eUMMA_UPDATE_ADDED);
					break;
				case eUMMA_CMD_MOVE:
					bRecordings			=	(pstOps->nFlags & UAPI_COPY_FLAG_RECORDINGS)?		TRUE:FALSE;
					bChunkedForMedia	=	(pstOps->nFlags & UAPI_COPY_FLAG_CHUNKFORMEDIA)?	TRUE:FALSE;
					if ( pstOps->pszSourceName){
						bIsDirSrc = HLIB_DIR_IsDirPath(pstOps->pszSourceName);
					}

					if ( pstOps->pszSourceName && bIsDirSrc )
					{
						UMMA_CMD_e eCmdDir = (bRecordings == TRUE || bChunkedForMedia == TRUE)? eUMMA_CMD_REMOVEDIR : eUMMA_CMD_REMOVEDIR_CHKSUB;
						umma_fm_AddOperation(pstUmma, eCmdDir, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0,
								(const HCHAR*)pstOps->pszSourceName, NULL, (const HCHAR*)pstOps->pszExtFilter, pstOps->nRequestId);
					}
					bIsDirDest = HLIB_DIR_IsDirPath(pstOps->pszDestName);
					if (bRecordings == FALSE && bIsDirDest == TRUE && bChunkedForMedia == FALSE)
					{
						break;
					}

					umma_fm_operation_UpdateFileInfo(pstUmma, pstOps->pszSourceName, bIsDirSrc, eUMMA_UPDATE_REMOVED);
					umma_fm_operation_UpdateFileInfo(pstUmma, pstOps->pszDestName, bIsDirDest, eUMMA_UPDATE_ADDED);
					break;
				case eUMMA_CMD_REMOVE:
					bIsDirSrc = HLIB_DIR_IsDirPath(pstOps->pszSourceName);
					umma_fm_operation_UpdateFileInfo(pstUmma, pstOps->pszSourceName, bIsDirSrc, eUMMA_UPDATE_REMOVED);
					break;
				case eUMMA_CMD_REMOVEDIR_CHKSUB :
					if ( TRUE == HLIB_DIR_IsDirPath(pstOps->pszSourceName))
					{
						umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVEDIR_CHKEND, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0,
								(const HCHAR*)pstOps->pszSourceName, NULL, (const HCHAR*)pstOps->pszExtFilter, pstOps->nRequestId);
					}
					break;
				default:
					break;
			}
		}

		pstReqInfo = (UMMA_REQINFO_t*)HLIB_HASH_Lookup(pstUmma->pstRequestList, (void*)pstOps->nRequestId);
		if (pstReqInfo)
		{
			pstReqInfo->nOperationDoneCount++;
			pstReqInfo->ulErr |= pstOps->ulErr;

			umma_fm_requestcallback_fire(pstUmma, pstReqInfo);
		}

		if (pstOps->pszSourceName)
			HLIB_STD_MemFree(pstOps->pszSourceName);
		if (pstOps->pszDestName)
			HLIB_STD_MemFree(pstOps->pszDestName);
		if (pstOps->pszExtFilter)
			HLIB_STD_MemFree(pstOps->pszExtFilter);

		UMMA_Free(pstOps);
	}
}

#if	0
static void		umma_fm_FreeOperationCB(void *pvData)
{
	umma_fm_FreeOperation(NULL, pvData);
}
#endif

static void		umma_fm_FreeOperationCBCancel(void *pvData)
{
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvData;

	pstOps->ulErr = UMERR_REASON_CANCELED;
	umma_fm_FreeOperation(NULL, pstOps);
}

//	Get file in folder - for flushing recycle bin.
static HCHAR*	umma_fm_GetFirstFileInDir(UMMA_t *pstUmma, const HCHAR *pszDir)
{
	HCHAR		*pszName;
	HxDIR_t		*dir;

	dir = HLIB_DIR_Open(pszDir);
	if (dir == NULL)
		return NULL;

	pszName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);
	do
	{
		HLIB_STD_StrNCpySafe(pszName, HLIB_DIR_GetName(dir), 1024);
		if ((HxSTD_StrCmp(pszName, ".") == 0) || (HxSTD_StrCmp(pszName, "..") == 0))
		{
		} else
		{
			HLIB_DIR_Close(dir);
			return pszName;
		}
	} while (HLIB_DIR_Next(dir) == ERR_OK);

	HLIB_DIR_Close(dir);

	return NULL;
}

//	check path1, path2 has same device
static HBOOL	umma_fm_IsSameMountPath(UMMA_t *pstUmma, const HCHAR *pszPath1, const HCHAR *pszPath2)
{
	HxList_t	*list;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	list = pstUmma->pstMountInfo;
	while (list)
	{
		pstMountInfo = (UMMA_MOUNTINFO_t*)HLIB_LIST_Data(list);
		if (HLIB_STD_StrStartWith(pszPath1, pstMountInfo->szMountPath) == TRUE)
		{
			if (HLIB_STD_StrStartWith(pszPath2, pstMountInfo->szMountPath) == TRUE)
				return TRUE;
			else
				return FALSE;
		}
		list = list->next;
	}

	return FALSE;
}

//	get mount path with give file or dir path.
static HCHAR*	umma_fm_GetMountPath(UMMA_t *pstUmma, const HCHAR *pszName, HCHAR *pszMountPath, HINT32 n)
{
	HxList_t	*list;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	list = pstUmma->pstMountInfo;
	while (list)
	{
		pstMountInfo = (UMMA_MOUNTINFO_t*)HLIB_LIST_Data(list);
		if (HLIB_STD_StrStartWith(pszName, pstMountInfo->szMountPath) == TRUE)
		{
			HLIB_STD_StrNCpySafe(pszMountPath, pstMountInfo->szMountPath, n);
			return pszMountPath;
		}
		list = list->next;
	}

	return NULL;
}

//	get mount info by given argument.
static UMMA_MOUNTINFO_t*	umma_fm_GetMountInfo(UMMA_t *pstUmma, const HCHAR *pszName)
{
	HxList_t	*list;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	list = pstUmma->pstMountInfo;
	while (list)
	{
		pstMountInfo = (UMMA_MOUNTINFO_t*)HLIB_LIST_Data(list);
		if (HLIB_STD_StrStartWith(pszName, pstMountInfo->szMountPath) == TRUE)
		{
			return pstMountInfo;
		}
		list = list->next;
	}

	return NULL;
}

//	get recycle bin path.
static HCHAR*	umma_fm_GetRecycleBinPath(UMMA_t *pstUmma, const HCHAR *pszName)
{
	HCHAR	*pszMountPath, *pszRecycleBinPath;

	pszMountPath = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 512);
	if (umma_fm_GetMountPath(pstUmma, pszName, pszMountPath, 512) == NULL)
		return NULL;

	pszRecycleBinPath = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 512);

	return HLIB_PATH_Join(pszMountPath, UMMA_RECYCLEBIN_NAME, pszRecycleBinPath, 512);
}

static HBOOL	umma_fm_compare_PathLike(void *userdata, void *data)
{
	HINT32		nLen;
	HCHAR		*pszPath = (HCHAR*)userdata;
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)data;

	if (pstOps->stDetail.bOnGoing)			//	OnGoing단계라면 해당 Proc에서 처리하자.
		return FALSE;

	nLen = HxSTD_StrLen(pszPath);
	if (!HxSTD_StrEmpty(pstOps->pszSourceName) && HxSTD_StrNCmp(pstOps->pszSourceName, pszPath, nLen) == 0)
		return TRUE;
	if (!HxSTD_StrEmpty(pstOps->pszDestName) && HxSTD_StrNCmp(pstOps->pszDestName, pszPath, nLen) == 0)
		return TRUE;

	return FALSE;
}

static HBOOL	umma_fm_compare_EqualRequestId(void *userdata, void *data)
{
	HINT32		nRequestId = (HINT32)userdata;
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)data;

	if (pstOps->stDetail.bOnGoing)			//	OnGoing단계라면 해당 Proc에서 처리하자.
		return FALSE;

	if (pstOps->nRequestId == nRequestId)
		return TRUE;

	return FALSE;
}

static HCHAR*	umma_fm_GetFlushInnerFilePath(const HCHAR *pszDir,const HCHAR *pszRoot)
{
	HxDIR_t		*dir;
	HCHAR		*pszFileName = NULL,*pszFilePath = NULL;
	HCHAR		*pszInnerPath = NULL;
	HUINT32 	ulItemCount=0;

	dir = HLIB_DIR_Open(pszDir);
	if (dir == NULL)
		return NULL;

	pszInnerPath = (HCHAR*)UMMA_Calloc(HxPATH_MAX);
	do
	{
		pszFileName = HLIB_DIR_GetName(dir);
		if ((HxSTD_StrCmp(pszFileName, ".") == 0) || (HxSTD_StrCmp(pszFileName, "..") == 0))
			continue;

		ulItemCount++;
		HLIB_PATH_Join(HLIB_DIR_GetPath(dir),pszFileName,pszInnerPath, HxPATH_MAX -1);

		if(HLIB_DIR_IsDir(dir) == TRUE) // find inner directory..
		{
			pszFilePath = umma_fm_GetFlushInnerFilePath(pszInnerPath,pszRoot); // search~ inner item
			if(pszFilePath)
			{
				HxLOG_Debug(HxANSI_COLOR_YELLOW("Find inner item [%s]\n"),pszFilePath);
				break;
			}
		}
		if(HLIB_DIR_IsFile(dir) == TRUE)
		{
			pszFilePath = UMMA_StrDup(pszInnerPath);
			break;
		}

	} while (HLIB_DIR_Next(dir) == ERR_OK);
	HLIB_DIR_Close(dir);

	if(ulItemCount == 0 && HxSTD_StrCmp(pszDir,pszRoot) !=0)  // delete empty folder and go to upper folder
	{
		HCHAR *p;

		HxSTD_StrNCpy(pszInnerPath,pszDir,HxPATH_MAX-1);
		HxLOG_Debug(HxANSI_COLOR_YELLOW("Directory Delete [%s]\n"),pszInnerPath);
		HLIB_DIR_Delete(pszInnerPath);

		p = HxSTD_StrRChar(pszInnerPath,'/');
		*p = '\0';
		HxLOG_Debug(HxANSI_COLOR_YELLOW("Go parent [%s]\n"),pszInnerPath);
		pszFilePath = umma_fm_GetFlushInnerFilePath(pszInnerPath,pszRoot); //go~ parent dir
	}
	UMMA_Free(pszInnerPath);
	return pszFilePath;
}


//	Get file in folder - for flushing recycle bin.
static HCHAR*	umma_fm_GetFlushFilePath(const HCHAR *pszDir)
{
	HxDIR_t		*dir;
	HCHAR		*pszInnerPath = NULL;
	HCHAR		*pszFileName = NULL,*pszFilePath = NULL;
	HBOOL		isDir = FALSE,IsFile = FALSE;

	dir = HLIB_DIR_Open(pszDir);
	if (dir == NULL)
		return NULL;

	pszInnerPath = (HCHAR*)UMMA_Calloc(HxPATH_MAX);
	do
	{
		pszFileName = HLIB_DIR_GetName(dir);
		if ((HxSTD_StrCmp(pszFileName, ".") == 0) || (HxSTD_StrCmp(pszFileName, "..") == 0))
			continue;

		//get one item
		HLIB_PATH_Join(HLIB_DIR_GetPath(dir),pszFileName,pszInnerPath, HxPATH_MAX -1);

		isDir = HLIB_DIR_IsDir(dir);
		IsFile = HLIB_DIR_IsFile(dir);

		break; //get one item..

	} while (HLIB_DIR_Next(dir) == ERR_OK);
	HLIB_DIR_Close(dir);

	if(isDir == TRUE) // search inner path
	{
		pszFilePath = umma_fm_GetFlushInnerFilePath(pszInnerPath,pszDir);
	}
	else if(IsFile == TRUE) // return file path
	{
		pszFilePath = UMMA_StrDup(pszInnerPath);
	}
	UMMA_Free(pszInnerPath);
	return pszFilePath;

}

static void umma_fm_checkRecodPath(UMMA_t *pstUmma,const HCHAR *pszSourceName)
{
	if(UMMA_FS_IsPvrFile(pszSourceName) == TRUE)
	{
		HCHAR *pszParent= NULL,*pszHJM = NULL;

		pszParent = (HCHAR*)UMMA_Calloc(HxPATH_MAX);
		pszHJM = (HCHAR*)UMMA_Calloc(HxPATH_MAX);

		HLIB_PATH_SplitExt(pszSourceName,pszParent,HxPATH_MAX -1);
		HxSTD_snprintf(pszHJM,HxPATH_MAX -1, "%s.hjm",pszParent);
		if(HLIB_FILE_Exist(pszHJM) == FALSE)
		{
			HxLOG_Warning("Can't find hjm(%s).So Delete[%s] :( \n",pszHJM,pszSourceName);
			umma_fm_operation_MoveFileToRecycleBin(pstUmma,pszSourceName);
		}
		UMMA_Free(pszParent);
		UMMA_Free(pszHJM);
	}
}

static HERROR	umma_fm_operation_SetPreProcessIsDone(UMMA_t *pstUmma, UMMA_REQINFO_t *pstReqInfo, HBOOL bSet)
{
	HINT32	nSet;

	nSet = (bSet ? 1 : 0);

	//	set flag to request info <-- bOpsReady == TRUE means, Preprocess is done!
	if (pstReqInfo == NULL)
	{
		HxLOG_Error("Can't find default request info in pstOPS\n");
		return ERR_FAIL;
	}

	if (pstReqInfo->nPreProcessDone == nSet)
		return ERR_OK;

	//	if nPreProcessDone is set, call lazy update for update DB.
	DAPI_LazyUpdate(DxNAMEOF(DxFileInfo_t));

	pstReqInfo->nPreProcessDone = nSet;
	umma_fm_requestcallback_fire(pstUmma, pstReqInfo);

	return ERR_OK;
}

static HERROR	umma_fm_operation_GetFileInfo(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	HERROR	ret;
	HBOOL	bThumbnailUrgent = (pstOps->eCmd == eUMMA_CMD_GETINFO_PRIO)? TRUE : FALSE;

	umma_fm_operation_SetPreProcessIsDone(pstUmma, pstOps->pstReqInfo, TRUE);
	ret = UMMA_FM_IFACE_AddFileInfo(pstOps->pvUser, pstOps->pszSourceName, bThumbnailUrgent);

	umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );
	umma_fm_FreeOperation(pstUmma, pstOps);

	return ret;
}


//	scan folder with given parameter.
static HERROR	umma_fm_operation_dir(UMMA_t *pstUmma, const HCHAR *pszSourceName, const HCHAR *pszExtFilter, HINT32 *pnRecursive, void *pvUserData, HERROR (*cbProcessDir)(UMMA_t *_pstUmma, void *_pvUserData, const HCHAR *_pszName, struct stat *_fstat))
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_OK;
#else
	HCHAR	*pszScanFolder, *pszScanFilter, *pszNewFullPath;
	HCHAR	*pszTempBuff1, *pszTempBuff2, *pszTempBuff3;
	HBOOL	bWildCard = FALSE;
	HBOOL	bCheckPoint = FALSE;

	HINT32	nItemCount;
	struct dirent	**ppstItemList;
	struct stat		fstat;

#ifdef	DUMP_STACK
	static FILE	*_file = NULL;
	struct vk_task	*pstVk = (struct vk_task*)s_tid;
#endif

	HxLOG_Print("<--- dir [%s]\n", pszSourceName);
	pszTempBuff1 = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);
	pszTempBuff2 = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 512);
	pszTempBuff3 = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);

	//	pszSourceName의 이름을 가진 Folder가 존재한다면...
	if (stat(pszSourceName, &fstat) == 0 && S_ISDIR(fstat.st_mode))
	{
		//	Source자체가 Folder
		pszScanFolder = (HCHAR*)pszSourceName;
		pszScanFilter = NULL;
		if ( umma_fm_operation_ScanResult == cbProcessDir)
		{
			UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvUserData;
			if ( 0 == pstOps->nRecursive )
				cbProcessDir(pstUmma, (void*)pvUserData, pszSourceName, &fstat);
		}
		bWildCard = TRUE;
		*pnRecursive = TRUE;
	} else
	{
		//	/AAA/BBB/CCC/DDD --> CCC를 ScanFolder로 지정.
		pszScanFolder = HLIB_PATH_DirName(pszSourceName, pszTempBuff1, 1024);
		pszScanFilter = HLIB_PATH_FileName(pszSourceName);
		HLIB_STD_StrNCpySafe(pszTempBuff2, pszScanFilter, 512);
		pszScanFilter = pszTempBuff2;

		if (HLIB_STD_StrEndWith(pszScanFilter, "*") == TRUE)
		{
			//	/*, /adssd*, ...
			pszScanFilter[HxSTD_StrLen(pszScanFilter) - 1] = '\0';
			bWildCard = TRUE;
			bCheckPoint = FALSE;
		} else
		if (HLIB_STD_StrStartWith(pszScanFilter, "*") == TRUE)
		{
			//	/*asdasdasd
			pszScanFilter++;
			bWildCard = TRUE;
			bCheckPoint = TRUE;
		}
	}

	HxLOG_Debug("pszScanFolder [%s] pszScanFilter [%s] bWildCard [%d]\n", pszScanFolder, pszScanFilter, bWildCard);

	if(bWildCard ) *pnRecursive = TRUE;

#ifdef	DUMP_STACK
	if (_file == NULL)
		_file = fopen("stack.log", "w");
	HLIB_LOG_DumpEx(pstVk->targ_.pulStackStartAddress, 1024 * 1024, 0, 0, _file);
#endif
	nItemCount = scandir((const char*)pszScanFolder, &ppstItemList, NULL, NULL);
#ifdef	DUMP_STACK
	HLIB_LOG_DumpEx(pstVk->targ_.pulStackStartAddress, 1024 * 1024, 0, 0, _file);
#endif
	if (nItemCount <= 0)
	{
		HxLOG_Error("fail to scan directory!\n");
		return ERR_FAIL;
	}

	while (nItemCount--)
	{
		if (!HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, ".") ||  /* Check "." or  ".." explicitly */
			!HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, ".."))
		{
			free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
			continue;
		}

		if (IS_NOT_DOT(ppstItemList[nItemCount]->d_name))
		{
			if (!HxSTD_StrEmpty(pszScanFilter))
			{
				if (bCheckPoint == FALSE &&
					!HLIB_STD_StrStartWith(ppstItemList[nItemCount]->d_name, pszScanFilter))
				{
					free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
					continue;
				}

				if (bCheckPoint == TRUE &&
					!HLIB_STD_StrEndWith(ppstItemList[nItemCount]->d_name, pszScanFilter))
				{
					free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
					continue;
				}
			}

			if (!HxSTD_StrEmpty(pszScanFilter) &&
				HLIB_STD_StrStartWith(ppstItemList[nItemCount]->d_name, pszScanFilter) && !bCheckPoint)
			{
				if (bWildCard == FALSE &&
					HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, pszScanFilter))
				{
					free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
					continue;
				}
			}

			if (!HxSTD_StrEmpty(pszScanFilter) &&
				HLIB_STD_StrEndWith(ppstItemList[nItemCount]->d_name, pszScanFilter) && bCheckPoint)
			{
				if (bWildCard == FALSE &&
					HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, pszScanFilter))
				{
					free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
					continue;
				}
			}

			pszNewFullPath = HLIB_PATH_Join(pszScanFolder, ppstItemList[nItemCount]->d_name, pszTempBuff3, 1024);
			if (stat(pszNewFullPath, &fstat))
			{
				HxLOG_Debug("fail to stat [%s]\n", pszNewFullPath);
				free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
				continue;
			}

			if (!S_ISREG(fstat.st_mode) && !S_ISDIR(fstat.st_mode))
			{
				HxLOG_Debug("not dir or file. [%s]\n", pszNewFullPath);
				free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
				continue;
			}

			if (!HxSTD_StrEmpty(pszExtFilter) && !S_ISDIR(fstat.st_mode))
			{
				HCHAR	*pszExtSource = HLIB_PATH_Extension(pszNewFullPath);

				if (HxSTD_StrEmpty(pszExtSource) == FALSE && HxSTD_StrLen(pszExtSource) > 1)
				{
					if (HLIB_STD_StrCaseStr(pszExtFilter, pszExtSource) == NULL)
					{
						free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
						continue;
					}
				}
				else if (!S_ISDIR(fstat.st_mode))	// 확장자 없는 파일 무시
				{
					free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
					continue;
				}
			}
			if (cbProcessDir)
			{
				cbProcessDir(pstUmma, (void*)pvUserData, pszNewFullPath, &fstat);
			}

		}
		free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
	}

	if (ppstItemList != NULL)
	{
		free(ppstItemList);
		ppstItemList = NULL;
	}
	return ERR_OK;
#endif /* CONFIG_PROD_OS_EMULATOR */
}

//	add operation for humax chunk based recording files or folder.
static HBOOL	umma_fm_operation_AddRecordingProcess(UMMA_t *pstUmma, UMMA_OPS_t *pstOps, const HCHAR *pszSource, const HCHAR *pszDest)
{
	HCHAR		*pszTestChunkFile;
	UMMA_OPS_t	*pstOpsBuf;
	HBOOL		bRecording;
	HBOOL		bChunkedFileForMedia = FALSE;

	bRecording				= pstOps->nFlags & UAPI_COPY_FLAG_RECORDINGS?		TRUE:FALSE;
	bChunkedFileForMedia	= pstOps->nFlags & UAPI_COPY_FLAG_CHUNKFORMEDIA?	TRUE:FALSE;

	if (bRecording == TRUE || bChunkedFileForMedia == TRUE)
	{
		pszTestChunkFile = umma_fm_GetFirstFileInDir(pstUmma, pszSource);
		if (HLIB_STD_StrStartWith(pszTestChunkFile, "chunk.") == TRUE)
		{
			HxLOG_Debug("[%s] includes recording chunk files!\n", pszSource);
			pstOpsBuf = umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszSource, pszDest, pstOps->pszExtFilter, pstOps->nRequestId);
			pstOpsBuf->stDetail.bOpsReady = TRUE;
			pstOpsBuf->stDetail.bChunked  = TRUE;

			return TRUE;
		}
	}

	return FALSE;
}

//      add operation for video(media) subtitle.
static HERROR   umma_fm_operation_AddSubtitleProcess( UMMA_t *pstUmma, UMMA_OPS_t *pstOps, const HCHAR *pszSource  )
{
	UMMA_OPS_t      *pstOpsBuf = NULL;
	const HCHAR pszSubtitleList[9][8] = {"smi", "sami", "smil", "srt", "sub", "txt", "ass", "ssa", "psb"};

	HCHAR *pszExt = NULL;
	HCHAR szSubtitle[512] = { 0 };
	HCHAR szBaseName[512] = { 0 };
	HCHAR szDestName[512] = { 0 };
	HINT32 i = 0;

	if(!pstOps)
		return ERR_FAIL;

	if ( HxSTD_StrEmpty(pszSource) )
		return ERR_FAIL;

	HxSTD_StrNCpy( szBaseName, pszSource, sizeof(szBaseName)-1 );
	pszExt = HLIB_PATH_Extension(szBaseName);
	if ( HxSTD_StrEmpty(pszExt) )
		return ERR_FAIL;

	if ( NULL == HxSTD_StrStr(UMMA_VIDEO_EXT, pszExt))
		return ERR_FAIL;

	*pszExt = '\0';
	for ( i = 0; i < 9; ++i )
	{
		HxSTD_snprintf( szSubtitle, sizeof(szSubtitle),"%s%s", szBaseName, pszSubtitleList[i]);
		if ( TRUE == HLIB_FILE_Exist(szSubtitle) )
		{
			if (pstOps->pszDestName)
			{
				if (HLIB_DIR_IsDirPath(pstOps->pszDestName) == TRUE)
					HLIB_PATH_Join(pstOps->pszDestName, HLIB_PATH_FileName(szSubtitle), szDestName, sizeof(szDestName));
				else
					HxSTD_StrNCpy(szDestName, pstOps->pszDestName, sizeof(szDestName)-1);
			}

			pstOpsBuf = NULL;
			pstOpsBuf = umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, FALSE, FALSE,
			pstOps->pvUser, szSubtitle, szDestName, pszSubtitleList[i], pstOps->nRequestId);
			if ( pstOpsBuf )
				pstOpsBuf->stDetail.bOpsReady = TRUE;
		}
	}

	return ERR_OK;
}

//	callback for umma_fm_operation_dir for folder or file move
static HERROR	umma_fm_operation_ScanMoveResult(UMMA_t *pstUmma, void *pvUserData, const HCHAR *pszName, struct stat *fstat)
{
	HERROR	hErr = ERR_OK;
	HCHAR	*pszDestName, *pszBaseName;
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvUserData;
	HINT32		nFlags = 0;

	if(!pstOps)
		return ERR_FAIL;

	pszDestName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);
	pszBaseName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 256);

	if (S_ISREG(fstat->st_mode))
	{
		UMMA_OPS_t	*pstOpsBuf;
		HBOOL		bRecording;
		HBOOL		bChunkedFileForMedia;

		bRecording				= pstOps->nFlags & UAPI_COPY_FLAG_RECORDINGS?		TRUE:FALSE;
		bChunkedFileForMedia	= pstOps->nFlags & UAPI_COPY_FLAG_CHUNKFORMEDIA?	TRUE:FALSE;
		if (bRecording == TRUE || bChunkedFileForMedia == TRUE)
		{
			if(HxSTD_StrStr(pszName, "chunk.0"))
			{
				HxLOG_Print("don't add dummy file.. (chunk.0)..\n");
				return hErr;
			}
		}

			if (HLIB_DIR_IsDirPath(pstOps->pszDestName) == TRUE)
				HLIB_PATH_Join(pstOps->pszDestName, HLIB_PATH_FileName(pszName), pszDestName, 1024);
			else
				HxSTD_StrNCpy(pszDestName, pstOps->pszDestName, 1024);

		nFlags = pstOps->nFlags;
		if (bRecording == TRUE || bChunkedFileForMedia == TRUE)
		{
			HCHAR **ppszMetaFilter = UMMA_FM_TOOLKIT_GetExtRec();
			HBOOL	bIsMetaFile = FALSE;

			bIsMetaFile = UMMA_FM_TOOLKIT_ExtSelector(pszDestName, ppszMetaFilter);
			if (bIsMetaFile == TRUE)
			{
				HxLOG_Print("don't decrypt copy meta file. name=(%s) \n",pszDestName);
				nFlags = (nFlags & ~UAPI_COPY_FLAG_DECRYPT);
				if(bChunkedFileForMedia == TRUE)
				{
					HxLOG_Print("don't add Meta File...\n");
					return hErr;
				}
			}
		}

		HxLOG_Print("REG)add Move operation : [%s] <-- [%s]\n", pszName, pszDestName);
		pstOpsBuf = umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, nFlags, pstOps->pvUser, pszName, pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
		pstOpsBuf->stDetail.bOpsReady = TRUE;

		umma_fm_operation_AddSubtitleProcess( pstUmma, pstOps, pszName );
	} else
	if (S_ISDIR(fstat->st_mode))
	{
#ifdef	SUPPORT_WEBAPP
		if (pstOps->nRecursive)
#endif
		{
			HLIB_PATH_BaseName(pszName, pszBaseName, 256);
			HLIB_PATH_Join(pstOps->pszDestName, pszBaseName, pszDestName, 1024);

			//	if nRecordings flag is ON, check pszName includes chunk files.
			//	if chunk files are existed, add chunk folder and bOpsReady flag is on
			if (umma_fm_operation_AddRecordingProcess(pstUmma, pstOps, pszName, pszDestName) == FALSE)
			{
				if (HLIB_DIR_IsExist(pszDestName) == FALSE)
					HLIB_DIR_Make(pszDestName);
				HxLOG_Print("DIR)add Move operation : [%s] <-- [%s]\n", pszName, pszDestName);
				umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszName, pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
			}
		}
	}
	return hErr;
}

//	callback for umma_fm_operation_dir for copy files or foler.
static HERROR	umma_fm_operation_ScanCopyResult(UMMA_t *pstUmma, void *pvUserData, const HCHAR *pszName, struct stat *fstat)
{
	HERROR	hErr = ERR_OK;
	HCHAR	*pszDestName, *pszBaseName;
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvUserData;
	HINT32		nFlags = 0;

	if(!pstOps)
		return ERR_FAIL;

	pszDestName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);
	pszBaseName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 256);

	if (S_ISREG(fstat->st_mode))
	{
		UMMA_OPS_t	*pstOpsBuf;
		HBOOL		bRecording;
		HBOOL		bChunkedFileForMedia = FALSE;

		bRecording				= pstOps->nFlags & UAPI_COPY_FLAG_RECORDINGS?		TRUE:FALSE;
		bChunkedFileForMedia	= pstOps->nFlags & UAPI_COPY_FLAG_CHUNKFORMEDIA?	TRUE:FALSE;
		if (bRecording == TRUE || bChunkedFileForMedia == TRUE)
		{
			if(HxSTD_StrStr(pszName, "chunk.0"))
			{
				HxLOG_Print("don't add dummy file.. (chunk.0)..\n");
				return hErr;
			}
		}

			if (HLIB_DIR_IsDirPath(pstOps->pszDestName) == TRUE)
				HLIB_PATH_Join(pstOps->pszDestName, HLIB_PATH_FileName(pszName), pszDestName, 1024);
			else
				HxSTD_StrNCpy(pszDestName, pstOps->pszDestName, 1024);

		nFlags = pstOps->nFlags;
		if (bRecording == TRUE || bChunkedFileForMedia == TRUE)
		{
			HCHAR **ppszMetaFilter = UMMA_FM_TOOLKIT_GetExtRec();
			HBOOL	bIsMetaFile = FALSE;

			bIsMetaFile = UMMA_FM_TOOLKIT_ExtSelector(pszDestName, ppszMetaFilter);
			if (bIsMetaFile == TRUE)
			{
				HxLOG_Print("don't decrypt copy meta file. name=(%s) \n",pszDestName);
				nFlags = (nFlags & ~UAPI_COPY_FLAG_DECRYPT);
				if(bChunkedFileForMedia == TRUE)
				{
					HxLOG_Print("don't add Meta File...\n");
					return hErr;
				}
			}
		}

		HxLOG_Print("REG)add Copy operation : [%s] <-- [%s]\n", pszName, pszDestName);
		pstOpsBuf = umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, nFlags, pstOps->pvUser, pszName, pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
		pstOpsBuf->stDetail.bOpsReady = TRUE;

		umma_fm_operation_AddSubtitleProcess( pstUmma, pstOps, pszName );
	}
	else if (S_ISDIR(fstat->st_mode))
	{
		//	if recursive is on, try sub folder.
#ifdef	SUPPORT_WEBAPP
		if (pstOps->nRecursive)
#endif
		{
			HLIB_PATH_BaseName(pszName, pszBaseName, 256);
			HLIB_PATH_Join(pstOps->pszDestName, pszBaseName, pszDestName, 1024);

			//	if nRecordings flag is ON, check pszName includes chunk files.
			//	if chunk files are existed, add chunk folder and bOpsReady flag is on
			if (umma_fm_operation_AddRecordingProcess(pstUmma, pstOps, pszName, pszDestName) == FALSE)
			{
				//	Folder Copy
				if (HLIB_DIR_IsExist(pszDestName) == FALSE)
				{
					HLIB_DIR_Make(pszDestName);
				}

				HxLOG_Print("DIR)add Copy operation : [%s] <-- [%s]\n", pszName, pszDestName);
				umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszName, pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
			}
		}
	}

	return hErr;
}

static HBOOL	umma_fm_operation_IsDeviceWritable(UMMA_t *pstUmma, const HCHAR *pszName)
{
	UMMA_MOUNTINFO_t	*pstMountInfo;

	pstMountInfo = umma_fm_GetMountInfo(pstUmma, pszName);
	if (pstMountInfo)
		return pstMountInfo->bIsWritable;

	if (0 == access( pszName, W_OK ))
		return TRUE;

	return FALSE;
}

//	move file to recycle bin. recyclebin has only files not folder.
static HERROR	umma_fm_operation_MoveFileToRecycleBin(UMMA_t *pstUmma, const HCHAR *pszName)
{
	HCHAR	*pszRecycleName, *pszNewName;
	HCHAR	*pszRecycleBinPath;

	struct stat	fstat;

	if (stat(pszName, &fstat))
		return ERR_FAIL;

	pszRecycleName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 512);
	pszNewName = HLIB_CBUF_GetBuffer(pstUmma->nCBuf, 1024);

	pszRecycleBinPath = umma_fm_GetRecycleBinPath(pstUmma, pszName);
	if (NULL == pszRecycleBinPath)
		return ERR_OK;

	if (HLIB_DIR_IsExist(pszRecycleBinPath) == FALSE)
	{
		HLIB_DIR_Make(pszRecycleBinPath);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(pszRecycleBinPath, UID_umma, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to HLIB_EXTRA_ChownWithParam (%s)\n", pszRecycleBinPath);
		}

		if (HLIB_DIR_Chmod(pszRecycleBinPath, 0770) != ERR_OK)
		{
			HxLOG_Error ("Fail to HLIB_DIR_Chmod (%s)\n", pszRecycleBinPath);
		}
#endif
	}

	HxSTD_snprintf(pszRecycleName, 512, "%lld%d%s", fstat.st_ino, HLIB_STD_GetSystemTick(), HLIB_PATH_FileName(pszName));
	if (HLIB_PATH_Join(pszRecycleBinPath, pszRecycleName, pszNewName, 1024) == NULL)
		return ERR_FAIL;

	HLIB_FILE_Move(pszName, pszNewName);

	//	Send global noti.
	HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", -1, pszName);

	return ERR_OK;
}

//	Entry point for RPC GroupOper Operation
static HERROR	umma_fm_operation_Group_oper(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	UMMA_FM_OPER_GROUP_t *pOperGroup = (UMMA_FM_OPER_GROUP_t *)pstOps->pvUser;
	UAPI_FM_GROUP_OPER_e eOper = eUAPI_FM_GROUP_OPER_UNKNOWN;
	UMMA_REQINFO_t	*pstReqInfo = NULL;
	struct stat	fstat;
	HUINT64 ullSize = 0;
	HINT32	nErr = 0;
	HCHAR	*pDestPath = NULL;
	HCHAR	*pSrcPath = NULL;
	HCHAR	*pExtFilter = NULL;
	HCHAR 	szTargetPath[512] = { 0 };
	HCHAR	*pszBasePath = NULL;
	HxList_t *pItem = NULL;
	UMMA_CMD_e eCmd = eUMMA_CMD_NONE;
	HINT32	nTmpReqId = 0;
	HCHAR szTmpPath[512] = { 0 };
	HCHAR szTmp2Path[512] = { 0 };
	HCHAR	*pszName = NULL;

	if(pOperGroup == NULL)
	{
		HxLOG_Print(" pOperGroup NULL...\n");
		goto OPER_GROUP_END;
	}

	eOper		= pOperGroup->eOper;
	if ( eUAPI_FM_GROUP_OPER_CANCEL == eOper )
	{
		UAPI_FM_GROUP_OPER_e eJobOper = eUAPI_FM_GROUP_OPER_UNKNOWN;
		UMMA_FM_GROUP_GetOperation( pOperGroup->nHndId, (HINT32 *)&eJobOper, &nTmpReqId );
		if ( eUAPI_FM_GROUP_OPER_UNKNOWN == eJobOper || 0 == nTmpReqId )
		{
			UMMA_FM_GROUP_SetOperation( pOperGroup->nHndId, eUAPI_FM_GROUP_OPER_CANCEL, pstOps->nRequestId );
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_INVALID_ARG );
		}
		else
		{
			HxLOG_Print("handle(%ld). cancel.nTmpReqId(%d), pstOps->nRequestId(%d)\n", pOperGroup->nHndId, nTmpReqId, pstOps->nRequestId);
			umma_fm_AddOperation(pstUmma, eUMMA_CMD_CANCEL, eUMMA_CMD_NONE, UMMA_UNDEFINED,
					pstOps->nFlags, (void *)nTmpReqId, NULL, NULL, NULL, pstOps->nRequestId);
		}
		goto OPER_GROUP_END;
	}

	if ( eUAPI_FM_GROUP_OPER_REMOVE != eOper )
	{
		if ( HxSTD_StrEmpty(pOperGroup->szDestPath) )
		{
			HxLOG_Error("dest path is invalid!!(%s)\n", pOperGroup->szDestPath);
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_FAIL_TO_STAT );
			goto OPER_GROUP_END;
		}
		if (TRUE != umma_fm_operation_IsDeviceWritable(pstUmma, pOperGroup->szDestPath) )
		{
			HxLOG_Error("device is not writable!!(%s)\n", pOperGroup->szDestPath);
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_FAIL_TO_WRITE );
			goto OPER_GROUP_END;
		}

		nErr = UMMA_FM_GROUP_GetTotalSize( pOperGroup->nHndId, &ullSize );
		if ( UMERR_OK != nErr )
		{
			HxLOG_Error(" hnadle(%ld). Get Total Size Fail [%d]\n", pOperGroup->nHndId, nErr);
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_INVALID_ARG );
			goto OPER_GROUP_END;
		}
	}

	if ( eUAPI_FM_GROUP_OPER_COPY == eOper || eUAPI_FM_GROUP_OPER_MOVE == eOper )
	{
		nErr = UMMA_FM_GROUP_CheckFreeSpace( pOperGroup->nHndId, pOperGroup->szDestPath );
		if ( UMERR_REASON_NOT_ENOUGH_SPACE == nErr )
		{
			HxLOG_Error(" hnadle(%ld). Check FreeSize Fail [%d]\n", pOperGroup->nHndId, nErr);
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_NOT_ENOUGH_SPACE );
			goto OPER_GROUP_END;

		}
		else if ( UMERR_REASON_SIZE_LIMIT_4G == nErr )
		{
			HxLOG_Error(" hnadle(%ld). Check FreeSize Fail [%d]\n", pOperGroup->nHndId, nErr);
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_SIZE_LIMIT_4G );
			goto OPER_GROUP_END;
		}
		else if ( UMERR_OK != nErr )
		{
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_INVALID_ARG );
			goto OPER_GROUP_END;
		}
	}

	switch ( eOper )
	{
		case eUAPI_FM_GROUP_OPER_COPY :
			eCmd = eUMMA_CMD_COPY;
			break;
		case eUAPI_FM_GROUP_OPER_MOVE :
			eCmd = eUMMA_CMD_MOVE;
			break;
		case eUAPI_FM_GROUP_OPER_REMOVE :
			eCmd = eUMMA_CMD_REMOVE;
			break;
		default :
			umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_INVALID_ARG );
			goto OPER_GROUP_END;
	}

	pstReqInfo = (UMMA_REQINFO_t*)HLIB_HASH_Lookup(pstUmma->pstRequestList, (void*)nTmpReqId);
	if (pstReqInfo )
		pstReqInfo->nOperationCount = 0; //ignore for group_oper job count.

	UMMA_FM_GROUP_SetOperation( pOperGroup->nHndId, eOper,  pstOps->nRequestId );

	pItem = UMMA_FM_GROUP_GetList( pOperGroup->nHndId );
	if (!pItem)
	{
		HxLOG_Error(" hnadle(%ld). Get list null !!!!.\n", pOperGroup->nHndId);
		umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_REASON_INVALID_ARG );
		goto OPER_GROUP_END;
	}

	pExtFilter = (HCHAR *)UMMA_FM_GROUP_GetExtFilter( pOperGroup->nHndId );

	umma_fm_notify_operation_error( pstUmma, pstOps->nRequestId, UMERR_OK );
	while ( pItem )
	{
		pSrcPath =  (HCHAR *)HLIB_LIST_Data( pItem );
		if ( 0 == stat(pSrcPath, &fstat))
		{
			if ( S_ISDIR(fstat.st_mode) ) // Folder
			{
				pszBasePath = HxSTD_StrRChar(pSrcPath, '/');
				if (pszBasePath)
				{
					HxSTD_snprintf(szTargetPath, 512, "%s/%s", pOperGroup->szDestPath, pszBasePath);
				}
				pDestPath = HLIB_PATH_Normalize(szTargetPath);
				HLIB_DIR_Make(pDestPath);
			}
			else
			{
				pDestPath = pOperGroup->szDestPath;
			}

			umma_fm_AddOperation(pstUmma, eCmd, eUMMA_CMD_NONE, UMMA_UNDEFINED, pstOps->nFlags,
					NULL, pSrcPath, pDestPath, pExtFilter, pstOps->nRequestId);
			if ( eUMMA_CMD_REMOVE == eCmd &&  S_ISDIR(fstat.st_mode))
			{
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVEDIR, eUMMA_CMD_NONE, UMMA_UNDEFINED,
						pstOps->nFlags, NULL, pSrcPath, pDestPath, pExtFilter, pstOps->nRequestId);
			}
		}
		else if (TRUE == UMMA_FM_GROUP_IsWildcardPath(pSrcPath, szTmpPath, sizeof(szTmpPath))
				&& HLIB_DIR_IsDirPath(szTmpPath))
		{
			/*waildcard path */
			HxDIR_t *pDir = NULL;
			pDir = HLIB_DIR_Open(szTmpPath);
			pDestPath = pOperGroup->szDestPath;
			do
			{
				/* exclude path */
				pszName = HLIB_DIR_GetName(pDir);
				if(pszName[0] == '.')
				{
					if(pszName[1] == 0)
						continue;
					else if(pszName[1] == '.' && pszName[2] == 0)
						continue;
					else if( HxSTD_StrStr(UMMA_FM_EXCLUDE_SYSPATH, pszName ))
						continue;
				}

				HxSTD_snprintf(szTmp2Path, sizeof(szTmp2Path), "%s/%s",
						szTmpPath, HLIB_DIR_GetName(pDir));
				pSrcPath = szTmp2Path;
				umma_fm_AddOperation(pstUmma, eCmd, eUMMA_CMD_NONE, UMMA_UNDEFINED, pstOps->nFlags,
						NULL, pSrcPath, pDestPath, pExtFilter, pstOps->nRequestId);
				if ( eUMMA_CMD_REMOVE == eCmd && HLIB_DIR_IsDir(pDir))
				{
					umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVEDIR, eUMMA_CMD_NONE, UMMA_UNDEFINED,
							pstOps->nFlags, NULL, pSrcPath, pDestPath, pExtFilter, pstOps->nRequestId);
				}
			}
			while(HLIB_DIR_Next(pDir) == ERR_OK);
			HLIB_DIR_Close(pDir);
		}

		pItem = HLIB_LIST_Next( pItem );
	}

	HxLOG_Print("[UMMA_FM] GROUP OPER START!!!!\n");

OPER_GROUP_END :
	nTmpReqId = pstOps->nRequestId;
	umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );

	if ( pOperGroup )
	{
		UMMA_Free(pOperGroup);
		pOperGroup = NULL;
		pstOps->pvUser = NULL;
	}
	umma_fm_FreeOperation(pstUmma, pstOps);

	return ERR_OK;
}

//	Entry point for RPC Move Operation
static HERROR	umma_fm_operation_Move(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	HERROR	err = ERR_OK;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	if (pstOps->stDetail.bOpsReady)
	{
		HUINT32	ulErr;
		pstMountInfo = umma_fm_GetMountInfo(pstUmma, pstOps->pszDestName);
		if(pstMountInfo != NULL)
		{
			umma_fm_operation_SetPreProcessIsDone(pstUmma, pstOps->pstReqInfo, TRUE);

			HxLOG_Debug("move [%s]\n -------> [%s]\n", pstOps->pszSourceName, pstOps->pszDestName);
			if (umma_fm_IsSameMountPath(pstUmma, pstOps->pszSourceName, pstOps->pszDestName) == TRUE)
			{
				if (HLIB_FILE_Move(pstOps->pszSourceName, pstOps->pszDestName) == ERR_OK)
				{
					//	Send global noti.
					HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", -1, pstOps->pszSourceName);
					HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is",  1, pstOps->pszDestName);

					pstOps->stDetail.bOpsReady = FALSE;
				}
				else
				{
					HxLOG_Error("Fail to MOVE reason [%x]\n", UMERR_REASON_FAIL_TO_STAT);
					pstOps->stDetail.bOpsReady = FALSE;
					pstOps->ulErr = UMERR_REASON_FAIL_TO_STAT;
				}
			}
			else
			{
				ulErr = UMMA_FM_TOOLKIT_Copy(pstOps, pstOps->nGiveUp ? FALSE : TRUE, pstMountInfo->ullReservedSizeKb);
				if (ulErr & UMERR_ERR)
				{
					if(ulErr & UMERR_REASON_CANCELED)
					{
						umma_fm_operation_MoveFileToRecycleBin(pstUmma, pstOps->pszDestName);
					}
					HxLOG_Error("Fail to TOOLKIT_Copy reason [%x]\n", ulErr);
					pstOps->stDetail.bOpsReady = FALSE;
					pstOps->ulErr = ulErr;
				}
				else if (ulErr & UMERR_ON_GOING)
				{
					pstOps->ulErr = UMERR_ON_GOING;
				}
				else
				{
					//	Send global noti.
					HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", -1, pstOps->pszSourceName);
					HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is",  1, pstOps->pszDestName);
					HxLOG_Print("Move Completed. src : %s, dest : %s\n", pstOps->pszSourceName, pstOps->pszDestName );

					pstOps->stDetail.bOpsReady = FALSE;
					umma_fm_operation_MoveFileToRecycleBin(pstUmma, pstOps->pszSourceName);
					pstOps->ulErr = UMERR_OK;
				}
			}
		}
		else
		{
			umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );
			umma_fm_FreeOperation(pstUmma, pstOps);
			return ERR_FAIL;
		}
	}
	else
	{
		//	Check destination fs policy first.
		if (umma_fm_operation_IsDeviceWritable(pstUmma, pstOps->pszDestName) == FALSE)
		{
			HxLOG_Debug("device is not writable!!\n");
			pstOps->ulErr = UMERR_REASON_FAIL_TO_WRITE;
		}
		else
		{
			HxLOG_Debug("src [%s] dst [%s]\n", pstOps->pszSourceName, pstOps->pszDestName);
			err = umma_fm_operation_dir(pstUmma, pstOps->pszSourceName, pstOps->pszExtFilter, &pstOps->nRecursive, (void*)pstOps, umma_fm_operation_ScanMoveResult);
		}
	}

	if (!pstOps->stDetail.bOpsReady)
	{
		umma_fm_safety_operation_remove(pstUmma, (void *)pstOps);
		umma_fm_FreeOperation(pstUmma, pstOps);
	}

	return err;
}

//	Entry point for RPC Copy Operation
static HERROR	umma_fm_operation_Copy(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	HERROR	err = ERR_OK;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	if (pstOps->stDetail.bOpsReady)
	{
		HUINT32	ulErr;
		pstMountInfo = umma_fm_GetMountInfo(pstUmma, pstOps->pszDestName);
		if(pstMountInfo != NULL)
		{
			umma_fm_operation_SetPreProcessIsDone(pstUmma, pstOps->pstReqInfo, TRUE);

			HxLOG_Debug("Copy id[%d]-[%s]\n -------> [%s]\n", pstOps->nRequestId,pstOps->pszSourceName, pstOps->pszDestName);
			ulErr = UMMA_FM_TOOLKIT_Copy(pstOps, pstOps->nGiveUp ? FALSE : TRUE, pstMountInfo->ullReservedSizeKb);
			if (ulErr & UMERR_ERR)
			{
				if(ulErr & UMERR_REASON_CANCELED)
				{
					umma_fm_operation_MoveFileToRecycleBin(pstUmma, pstOps->pszDestName);
				}
				HxLOG_Error("Fail to TOOLKIT_Copy reason [%x]\n", ulErr);
				pstOps->stDetail.bOpsReady = FALSE;
				pstOps->ulErr = ulErr;
			}
			else if (ulErr & UMERR_ON_GOING)
			{
				pstOps->ulErr = UMERR_ON_GOING;
			}
			else
			{
				//	Send global noti.
				HLIB_RPC_Notify(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", 1, pstOps->pszDestName);

				pstOps->stDetail.bOpsReady = FALSE;
				pstOps->ulErr = UMERR_OK;
			}
		}
		else
		{
			umma_fm_safety_operation_remove(pstUmma, (void *)pstOps);
    		umma_fm_FreeOperation(pstUmma, pstOps);
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Debug("src [%s] dst [%s]\n", pstOps->pszSourceName, pstOps->pszDestName);
		HLIB_PATH_Normalize(pstOps->pszSourceName);
		HLIB_PATH_Normalize(pstOps->pszDestName);
		if (HxSTD_StrCmp(pstOps->pszSourceName, pstOps->pszDestName) == 0)
		{
			HxLOG_Error("Error : Src and Dest is SAME! (%s : %s)\n", pstOps->pszSourceName, pstOps->pszDestName);
			err = ERR_FAIL;
		} else
		{
			//	Check destination fs policy first.
			if (umma_fm_operation_IsDeviceWritable(pstUmma, pstOps->pszDestName) == FALSE)
			{
				HxLOG_Debug("device is not writable!!(%s)\n", pstOps->pszDestName);
				pstOps->ulErr = UMERR_REASON_FAIL_TO_WRITE;
			} else
				err = umma_fm_operation_dir(pstUmma, pstOps->pszSourceName, pstOps->pszExtFilter, &pstOps->nRecursive, (void*)pstOps, umma_fm_operation_ScanCopyResult);
		}
	}

	if (!pstOps->stDetail.bOpsReady)
	{
		umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );
		umma_fm_FreeOperation(pstUmma, pstOps);
	}

	return err;
}

//	callback for umma_fm_operation_dir for file or foder remove.
static HERROR	umma_fm_operation_RemoveResult(UMMA_t *pstUmma, void *pvUserData, const HCHAR *pszName, struct stat *fstat)
{
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvUserData;

	HxLOG_Print("<--- remove result : [%s] - [%d]\n", pszName, S_ISREG(fstat->st_mode));
	if (S_ISREG(fstat->st_mode))
	{
		umma_fm_operation_SetPreProcessIsDone(pstUmma, pstOps->pstReqInfo, TRUE);
		umma_fm_operation_MoveFileToRecycleBin(pstUmma, pszName);
		umma_fm_operation_UpdateFileInfo(pstUmma, pszName, FALSE, eUMMA_UPDATE_REMOVED);

		umma_fm_operation_AddSubtitleProcess( pstUmma, pstOps, pszName );
	}
	else
	if (S_ISDIR(fstat->st_mode))
	{
#ifdef	SUPPORT_WEBAPP
		if (pstOps->nRecursive)
#endif
		{
			umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszName, pstOps->pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
			umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVEDIR, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0, (const HCHAR*)pszName, NULL, (const HCHAR*) pstOps->pszExtFilter, pstOps->nRequestId);
		}
	}

	return ERR_OK;
}

static HERROR	umma_fm_operation_Remove(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	HERROR	err = ERR_FAIL;

	pstOps->nRecursive = TRUE;
	//	Check destination fs policy first.
	if (umma_fm_operation_IsDeviceWritable(pstUmma, pstOps->pszSourceName) == FALSE)
	{
		HxLOG_Debug("device is not writable!!\n");
		pstOps->ulErr = UMERR_REASON_FAIL_TO_WRITE;
	} else
		err = umma_fm_operation_dir(pstUmma, pstOps->pszSourceName, pstOps->pszExtFilter, &pstOps->nRecursive, (void*)pstOps, umma_fm_operation_RemoveResult);

	umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );
	umma_fm_FreeOperation(pstUmma, pstOps);

	return err;
}

static HERROR	umma_fm_operation_ScanResult(UMMA_t *pstUmma, void *pvUserData, const HCHAR *pszName, struct stat *fstat)
{
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t*)pvUserData;
	HCHAR 		*pstExtention;

	if (S_ISREG(fstat->st_mode))
	{
		//	new path가 file이라면--> Next CMD 수행.
		if (pstOps->ePostCmd)
			umma_fm_AddOperation(pstUmma, pstOps->ePostCmd, eUMMA_CMD_NONE, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszName, pstOps->pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
	}
	else
	{
		if (S_ISDIR(fstat->st_mode) && pstOps->nRecursive) //	new path가 folder라면--> Scan Sub Folder수행.
		{
			pstExtention = HLIB_PATH_Extension(pszName);
			if( !(pstExtention && HxSTD_StrCmp(pstExtention,"ts")==0)) // delayed rec 의 ts 폴더는 수행하지 않는다.
				umma_fm_AddOperation(pstUmma, pstOps->eCmd, pstOps->ePostCmd, pstOps->nRecursive, pstOps->nFlags, pstOps->pvUser, pszName, pstOps->pszDestName, pstOps->pszExtFilter, pstOps->nRequestId);
		}
	}

	//	Add New Item.
	UMMA_FM_IFACE_AddFileInfoWoDetail(pstOps->pvUser, pszName, fstat);
	// check invaild pvr files..
	switch(pstOps->eCmd)
	{
		case eUMMA_CMD_DIRECTDIR:
		case eUMMA_CMD_COPY:
		case eUMMA_CMD_MOVE:
			HxLOG_Debug("Skip ... Check Record Path : %d\n", pstOps->eCmd);
			break;
		default:
			HxLOG_Debug("pstOps->eCmd : %d\n", pstOps->eCmd);
			umma_fm_checkRecodPath(pstUmma,pszName);
			break;
	}
	return ERR_OK;
}

static HERROR	umma_fm_operation_Scan(UMMA_t *pstUmma, UMMA_OPS_t *pstOps)
{
	HERROR	err;

	err = umma_fm_operation_dir(pstUmma, pstOps->pszSourceName, pstOps->pszExtFilter, &pstOps->nRecursive, (void*)pstOps, umma_fm_operation_ScanResult);

	//	Remove Current Operation in operation list.
	umma_fm_safety_operation_remove( pstUmma, (void *)pstOps );
	umma_fm_FreeOperation(pstUmma, pstOps);

	return err;
}

//	remove files in recycle bin.
static HBOOL	umma_fm_operation_FlushRecycleBin(UMMA_t *pstUmma)
{
	HCHAR				*pszFlushPath = NULL;
	HCHAR				pszBinPath[512] = {0,};
	HxList_t			*list;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	if (HxSTD_StrEmpty(pstUmma->szFlushName) == FALSE)
	{
		HUINT32 state;

		state = UMMA_FM_TOOLKIT_Delete(pstUmma->szFlushName);
		if (state == UMERR_ON_GOING)
		{
			return TRUE;
		}
		else
		{
			HxSTD_MemSet(pstUmma->szFlushName,0x0,sizeof(pstUmma->szFlushName));
		}
	}

	list = pstUmma->pstMountInfo;
	while (list)
	{
		pstMountInfo = (UMMA_MOUNTINFO_t*)HLIB_LIST_Data(list);
		HLIB_PATH_Join(pstMountInfo->szMountPath, UMMA_RECYCLEBIN_NAME, pszBinPath, 512);
		pszFlushPath = umma_fm_GetFlushFilePath(pszBinPath);
		if (pszFlushPath)
		{
			HxSTD_StrNCpy(pstUmma->szFlushName,pszFlushPath,sizeof(pstUmma->szFlushName));
			HxLOG_Debug(HxANSI_COLOR_YELLOW("->>Flush: %s \n"),pstUmma->szFlushName);
			UMMA_Free(pszFlushPath);
			return TRUE;
		}
		list = list->next;
	}
	return FALSE;
}

static void umma_fm_cbCancel (UMMA_t *umma, void *operation, void *userdata)
{
	UMMA_OPS_t	*pstOpInfo = (UMMA_OPS_t *)operation;
	UMMA_OPS_t	*pstOperation = (UMMA_OPS_t *)userdata;

	if (pstOpInfo->nRequestId == (HINT32)pstOperation->pvUser)
		pstOpInfo->nGiveUp = 1;
}

static HERROR	umma_fm_operation_CancelOperation(UMMA_t *pstUmma, UMMA_OPS_t *pstOperation)
{
	ENTER_OPERATIONS_SEM;
	(void)UMMA_OperationQueueRemoveItemIf(pstUmma, (void *)pstOperation->pvUser, umma_fm_compare_EqualRequestId, umma_fm_FreeOperationCBCancel);
	(void)UMMA_OperationQueueForEach(pstUmma, pstOperation, umma_fm_cbCancel);
	(void)UMMA_OperationQueueRemoveItem(pstUmma, (const void *)pstOperation);
	LEAVE_OPERATIONS_SEM;
	umma_fm_FreeOperation(pstUmma, pstOperation);

	return ERR_OK;
}

static void umma_fm_cbCancelWithPath (UMMA_t *umma, void *operation, void *userdata)
{
	HINT32		 nLen;
	UMMA_OPS_t	*pstOpInfo = (UMMA_OPS_t *)operation;
	HCHAR		*pszMountPath = (HCHAR *)userdata;

	nLen = HxSTD_StrLen(pszMountPath);

	if (!HxSTD_StrEmpty(pstOpInfo->pszSourceName) && HxSTD_StrNCmp(pstOpInfo->pszSourceName, pszMountPath, nLen) == 0)
	{
		pstOpInfo->nGiveUp = 1;
		pstOpInfo->ulErr = UMERR_REASON_CANCELED;
	}
	else if (!HxSTD_StrEmpty(pstOpInfo->pszDestName) && HxSTD_StrNCmp(pstOpInfo->pszDestName, pszMountPath, nLen) == 0)
	{
		pstOpInfo->nGiveUp = 1;
		pstOpInfo->ulErr = UMERR_REASON_CANCELED;
	}
}

static void		umma_fm_operation_CancelOperationWithPath(UMMA_t *pstUmma, const HCHAR *pszMountPath)
{
	ENTER_OPERATIONS_SEM;
	(void)UMMA_OperationQueueRemoveItemIf(pstUmma, (void *)pszMountPath, umma_fm_compare_PathLike, umma_fm_FreeOperationCBCancel);
	(void)UMMA_OperationQueueForEach(pstUmma, (void *)pszMountPath, umma_fm_cbCancelWithPath);
	LEAVE_OPERATIONS_SEM;
}

//	get mount info
static UMMA_MOUNTINFO_t	*umma_fm_operation_GetMountBasicInfo(const HCHAR *pszMountPath, const HCHAR *pszUUID)
{
	HCHAR	szMountOpt[80];
	HERROR	err = ERR_OK;
	UMMA_MOUNTINFO_t	*pstMountInfo;

	pstMountInfo = (UMMA_MOUNTINFO_t*)UMMA_Calloc(sizeof(UMMA_MOUNTINFO_t));
	HLIB_STD_StrNCpySafe(pstMountInfo->szMountPath, pszMountPath, 512);
	HLIB_STD_StrNCpySafe(pstMountInfo->szUUID, pszUUID, 512);

	err = HLIB_DIR_GetMountFstype(pszMountPath, pstMountInfo->szFileSystem, 64);
	err |= HLIB_DIR_GetMountOpts (pszMountPath, szMountOpt, 80);
	if (err != ERR_OK)
	{
		//	not defined in /proc/mounts --> we assumed it was network device like smb, ftp and etc.
		pstMountInfo->bIsNetworkDevice = TRUE;			//	if network device is on, umma support doesn't get fileinfo from data.
		pstMountInfo->bIsWritable = TRUE;				//	?? possible or not?
	}

	if (HxSTD_StrStr(szMountOpt, "rw"))
		pstMountInfo->bIsWritable = TRUE;

	return pstMountInfo;
}

static void		umma_fm_operation_MountDevice(UMMA_t *pstUmma, const HCHAR *pszMountPath, const HCHAR *pszUUID, const UMMA_FM_StorageInfo_t *pstInfo)
{
	UMMA_MOUNTINFO_t	*pstMountInfo;

	pstMountInfo = umma_fm_operation_GetMountBasicInfo(pszMountPath, pszUUID);
	if (pstMountInfo)
	{
		pstMountInfo->eInterface = pstInfo->eInterface;
		pstMountInfo->ullReservedSizeKb = pstInfo->ullReservedSizeKb;

		HxLOG_Debug("MountPath(%s), UUID(%s), eInterface(%d), ReservedSizeKb(%lld)\n\n", pszMountPath, pszUUID,  pstMountInfo->eInterface, pstMountInfo->ullReservedSizeKb);
		HAPI_BroadcastSignal("signal:onStorageMounted", 1, "ssi", pszMountPath, pszUUID, 0);
		UMMA_FM_CACHE_CheckVersion(pstMountInfo->szMountPath);
		pstUmma->pstMountInfo = HLIB_LIST_Append(pstUmma->pstMountInfo, (void*)pstMountInfo);
//		forced get fileinfo even if the device is network module.
//		if (pstMountInfo->bIsNetworkDevice)
//			umma_fm_AddOperation(pstUmma, eUMMA_CMD_LAZYDIR, eUMMA_CMD_NONE,    1, UMMA_UNDEFINED, (void*)pstMountInfo, pstMountInfo->szMountPath, NULL, NULL, 0);
//		else
		umma_fm_AddOperation(pstUmma, eUMMA_CMD_LAZYDIR, eUMMA_CMD_GETINFO, 1, UMMA_UNDEFINED, (void*)pstMountInfo, pstMountInfo->szMountPath, NULL, s_szFullExtList, 0);
	}
}

static HBOOL	umma_fm_compare_PathEqual(void *userdata, void *data)
{
	HCHAR	*pszName = (HCHAR*)userdata;
	UMMA_MOUNTINFO_t	*pstMountInfo = (UMMA_MOUNTINFO_t*)data;

	if (HxSTD_StrCmp(pszName, pstMountInfo->szMountPath) == 0)
		return TRUE;

	return FALSE;
}

static void		umma_fm_operation_UnMountDevice(UMMA_t *pstUmma, const HCHAR *pszMountPath)
{
	HCHAR	*pszName = HLIB_STD_StrDup(pszMountPath);

	HxLOG_Debug("MountPath(%s)\n\n", pszMountPath );
	HAPI_BroadcastSignal("signal:onStorageUnmounted", 1, "s", pszMountPath);

	HLIB_PATH_Normalize(pszName);
	umma_fm_operation_CancelOperationWithPath(pstUmma, pszName);
	umma_fm_operation_UpdateFileInfo(pstUmma, pszMountPath, TRUE, eUMMA_UPDATE_REMOVED_LIKE);
	pstUmma->pstMountInfo = HLIB_LIST_RemoveIf(pstUmma->pstMountInfo, pszName, umma_fm_compare_PathEqual, UMMA_Free_CB);

	HLIB_STD_MemFree(pszName);
}

static void		umma_fm_operation_FormatDevice(UMMA_t *pstUmma, const HUINT32 ulDeviceId)
{
	HERROR hErr;
	DxStorage_Info_t stStorageInfo;

	hErr = UMMA_FS_GetStorageInfo(ulDeviceId, &stStorageInfo);
	if (hErr == ERR_OK)
	{
		HxLOG_Warning("[umma_fm_operation_FormatDevice] DeviceId : %d, MountPath : %s\n", ulDeviceId, stStorageInfo.szMountPath);

		if (stStorageInfo.nPartitionNum >= 1)
		{
			umma_fm_operation_UnMountDevice(pstUmma, (const HCHAR*)stStorageInfo.szMountPath);
		}
	}

	UMMA_FS_FormatStroage();
}

static HERROR	umma_fm_operation_BrowseResult(void *pvUmma, HINT32 nReqId, void *pvResult)
{
	UMMA_t *pstUmma = (UMMA_t *)pvUmma;
	UAPI_BROWSE_RESULT_t *pBrowResult = (UAPI_BROWSE_RESULT_t *)pvResult;
	HINT32 itemSize = pBrowResult->nItemSize;

	HLIB_RPC_NotifyWithRequestId(pstUmma->nRpcHandle, RPC_UMMA_FM_BROWSE_onResult, nReqId, "ibb", pBrowResult->eEvent, pBrowResult, sizeof(UAPI_BROWSE_RESULT_t), pBrowResult->Itemdata, itemSize);
	return ERR_OK;
}

static void		umma_fm_signal_Process(UMMA_t *pstUmma, UMMA_SIGNAL_t *pstSignal)
{
	HCHAR	*pszSourceDir = NULL;
	HCHAR	*pszDestDir = NULL;
	HCHAR	*pszExtFilter = NULL;
	HINT32	nRecordings, nRequestId, nFlag;
	HBOOL	bChunkedFileForMedia = FALSE;
	nRequestId = pstSignal->nReqId;

	switch (pstSignal->ulSignal)
	{
		case UMMA_FM_SIGNAL_MOUNT:
			HxLOG_Debug("UMMA_FM_SIGNAL_MOUNT.. [%s]\n", pstSignal->ulParam1);
			HLIB_PATH_Normalize((HCHAR*)pstSignal->ulParam1);
			if (HLIB_DIR_IsExist((HCHAR*)pstSignal->ulParam1))
			{
				umma_fm_operation_MountDevice(pstUmma, (HCHAR*)pstSignal->ulParam1, (HCHAR*)pstSignal->ulParam2, (UMMA_FM_StorageInfo_t *)pstSignal->ulParam3);
			}

			if (pstSignal->ulParam1)
				UMMA_Free((void*)pstSignal->ulParam1);
			if (pstSignal->ulParam2)
				UMMA_Free((void*)pstSignal->ulParam2);
			if (pstSignal->ulParam3)
				UMMA_Free((void*)pstSignal->ulParam3);
			break;

		case UMMA_FM_SIGNAL_UNMOUNT:
			HxLOG_Debug("UMMA_FM_SIGNAL_UNMOUNT.. [%s]\n", pstSignal->ulParam1);
			umma_fm_operation_UnMountDevice(pstUmma, (const HCHAR *)pstSignal->ulParam1);
			if (pstSignal->ulParam1)
				UMMA_Free((void*)pstSignal->ulParam1);
			break;

		case UMMA_FM_SIGNAL_FORMAT:
			HxLOG_Debug("UMMA_FM_SIGNAL_FORMAT.. [%d]\n", pstSignal->ulParam1);
			umma_fm_operation_FormatDevice(pstUmma, (const HUINT32)pstSignal->ulParam1);
			break;

		////////////////////////////////////////////////////////////////////////////////////
		//	RPC SYNC SIGNAL.
		case UMMA_FM_SIGNAL_SCAN:
			{
				HINT32	nRecursive;
				UMMA_MOUNTINFO_t	*pstMountInfo;

				pszSourceDir = (HCHAR*)pstSignal->ulParam1;
				pszExtFilter = (HCHAR*)pstSignal->ulParam2;
				nRecursive   = (HINT32)pstSignal->ulParam3;
				pstMountInfo = umma_fm_GetMountInfo(pstUmma, pszSourceDir);
				if (pstMountInfo)
				{
					HxLOG_Debug("UMMA_FM_SIGNAL_SCAN.. [%s]\n", pszSourceDir);
			//		forced get fileinfo even if the device is network module.
			//		if (pstMountInfo->bIsNetworkDevice)
			//			umma_fm_AddOperation(pstUmma, eUMMA_CMD_DIR, eUMMA_CMD_NONE, nRecursive, UMMA_UNDEFINED, (void*)pstMountInfo, pszSourceDir, NULL, pszExtFilter, nRequestId);
			//		else
						umma_fm_AddOperation(pstUmma, eUMMA_CMD_DIR, eUMMA_CMD_GETINFO_PRIO, nRecursive, UMMA_UNDEFINED, (void*)pstMountInfo, (const HCHAR*)pszSourceDir, NULL, (const HCHAR*)pszExtFilter, nRequestId);
				}
#ifdef UMMA_CHECK_AVAILABLE_MOUNTPATH
				else
				{
					HERROR 					hErr;
					DxStorage_Info_t 		stStorageInfo;
					UMMA_FM_StorageInfo_t 	*pstInfo;
					HUINT32					ulCheckStartTick,ulTrialCnt;

					ulTrialCnt = 0;
					ulCheckStartTick = HLIB_STD_GetSystemTick();
					HLIB_PATH_Normalize(pszSourceDir);

					HxLOG_Error("Start Retry Path[%s]\n",pszSourceDir);
					while (HLIB_STD_GetSystemTick() < ulCheckStartTick + UMMA_FM_CHECK_TIMEOUT)
					{
						HxLOG_Error("Retry Count[%d] Path[%s]\n",ulTrialCnt,pszSourceDir);

						if (HLIB_DIR_IsExist(pszSourceDir) == TRUE)
						{
							hErr = UMMA_FS_GetStorageInfoByPath(pszSourceDir, &stStorageInfo);

							if (hErr == ERR_OK)
							{
								pstInfo = (UMMA_FM_StorageInfo_t*)UMMA_Calloc(sizeof(UMMA_FM_StorageInfo_t));
								pstInfo->eInterface = umma_fm_getInterface(stStorageInfo.eDevType);
								pstInfo->ullReservedSizeKb = stStorageInfo.ullReservedSizeKb;

								umma_fm_operation_MountDevice(pstUmma,pszSourceDir, stStorageInfo.szUUID, pstInfo);
								umma_fm_AddOperation(pstUmma, eUMMA_CMD_DIR, eUMMA_CMD_GETINFO_PRIO, nRecursive, UMMA_UNDEFINED, (void*)pstMountInfo, (const HCHAR*)pszSourceDir, NULL, (const HCHAR*)pszExtFilter, nRequestId);
								UMMA_Free(pstInfo);
							}

							HxLOG_Error("Retry Success = Path[%s]\n",pszSourceDir);
							break;
						}
						else
						{
							VK_TASK_Sleep(20);
							ulTrialCnt++;
						}
					} // end of while
				} // end of else
#endif
			}
			break;
		case UMMA_FM_SIGNAL_REMOVE:
			pszSourceDir = (HCHAR*)pstSignal->ulParam1;
			pszExtFilter = (HCHAR*)pstSignal->ulParam2;
			HxLOG_Debug("UMMA_FM_SIGNAL_REMOVE.. [%s]\n", pszSourceDir);

			umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVE, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0, (const HCHAR*)pszSourceDir, NULL, (const HCHAR*)pszExtFilter, nRequestId);
			umma_fm_AddOperation(pstUmma, eUMMA_CMD_REMOVEDIR, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0, (const HCHAR*)pszSourceDir, NULL, (const HCHAR*)pszExtFilter, nRequestId);
			break;

		case UMMA_FM_SIGNAL_MOVE:
			{
				pszSourceDir			= (HCHAR*)pstSignal->ulParam1;
				pszDestDir				= (HCHAR*)pstSignal->ulParam2;
				pszExtFilter			= (HCHAR*)pstSignal->ulParam3;
				nFlag					= pstSignal->ulParam4;
				nRecordings				= nFlag & UAPI_COPY_FLAG_RECORDINGS;
				bChunkedFileForMedia	= nFlag & UAPI_COPY_FLAG_CHUNKFORMEDIA ? TRUE : FALSE;

				if (HxSTD_StrEmpty(pszDestDir))
				{
					HxLOG_Error("RPC Copy [%s, %s, %s, %d]\n", pszSourceDir, pszDestDir, pszExtFilter, nRecordings);
					//umma_fm_notify_operation_error( pstUmma, nRequestId, UMERR_REASON_FAIL_TO_STAT );
					break;
				}
#ifdef	SUPPORT_WEBAPP
				if (HLIB_DIR_IsDirPath(pszSourceDir) == TRUE && nRecordings == FALSE && bChunkedFileForMedia == FALSE)
				{
					//	Change destination Folder
					HCHAR	*pszBasePath;

					HLIB_PATH_Normalize(pszSourceDir);
					pszBasePath = HxSTD_StrRChar(pszSourceDir, '/');
					if (pszBasePath)
					{
						HxSTD_snprintf(pstUmma->szRpcTemp, 1024, "%s/%s", pszDestDir, pszBasePath);
						pszDestDir = HLIB_PATH_Normalize(pstUmma->szRpcTemp);
						HLIB_DIR_Make(pszDestDir);
					}
				}
#endif
				HxLOG_Debug("RPC Move [%s, %s, %s, 0x%x]\n", pszSourceDir, pszDestDir, pszExtFilter, nFlag);
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_MOVE, eUMMA_CMD_NONE, UMMA_UNDEFINED, nFlag, 0, pszSourceDir, pszDestDir, pszExtFilter, nRequestId);
			}
			break;

		case UMMA_FM_SIGNAL_COPY:
			pszSourceDir			= (HCHAR*)pstSignal->ulParam1;
			pszDestDir				= (HCHAR*)pstSignal->ulParam2;
			pszExtFilter			= (HCHAR*)pstSignal->ulParam3;
			nFlag					= pstSignal->ulParam4;
			nRequestId				= pstSignal->nReqId;
			nRecordings				= nFlag & UAPI_COPY_FLAG_RECORDINGS;
			bChunkedFileForMedia	= nFlag & UAPI_COPY_FLAG_CHUNKFORMEDIA ? TRUE : FALSE;

			HxLOG_Debug("[COPY] pszSourceDir : %s, pszDestDir : %s\n", pszSourceDir, pszDestDir);
			if (HxSTD_StrEmpty(pszDestDir))
			{
				HxLOG_Error("RPC Copy [%s, %s, %s, 0x%x]\n", pszSourceDir, pszDestDir, pszExtFilter, nFlag);
				//umma_fm_notify_operation_error( pstUmma, nRequestId, UMERR_REASON_FAIL_TO_STAT );
				break;
			}
#ifdef	SUPPORT_WEBAPP
			if (HLIB_DIR_IsDirPath(pszSourceDir) == TRUE && nRecordings == FALSE && bChunkedFileForMedia == FALSE)
			{
				//	Change destination Folder
				HCHAR	*pszBasePath = NULL;
				HCHAR	*pszNomalizedPath = NULL;
				HLIB_PATH_Normalize(pszSourceDir);
				pszBasePath = HxSTD_StrRChar(pszSourceDir, '/');
				if (pszBasePath)
				{
					HxSTD_snprintf(pstUmma->szRpcTemp, 1024, "%s/%s", pszDestDir, pszBasePath);
					pszNomalizedPath = HLIB_PATH_Normalize(pstUmma->szRpcTemp);
					if(pszNomalizedPath == NULL)
					{
						HxLOG_Error("Error, There is no Nomalized Path.\n");
						break;
					}
					HLIB_DIR_Make(pszNomalizedPath);
				}
				HxLOG_Debug("RPC Copy [%s, %s, %s, 0x%x]\n", pszSourceDir, pszNomalizedPath, pszExtFilter, nFlag);
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_COPY, eUMMA_CMD_NONE, UMMA_UNDEFINED, nFlag, 0, pszSourceDir, pszNomalizedPath, pszExtFilter, nRequestId);
			}
			else
#endif
			{
				HxLOG_Debug("RPC Copy [%s, %s, %s, 0x%x]\n", pszSourceDir, pszDestDir, pszExtFilter, nFlag);
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_COPY, eUMMA_CMD_NONE, UMMA_UNDEFINED, nFlag, 0, pszSourceDir, pszDestDir, pszExtFilter, nRequestId);
			}

			break;

		case UMMA_FM_SIGNAL_CANCEL:
			{
				HINT32	_nRequestId = pstSignal->ulParam1;

				umma_fm_AddOperation(pstUmma, eUMMA_CMD_CANCEL, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, (void*)_nRequestId, NULL, NULL, NULL, nRequestId);
			}
			break;

		case UMMA_FM_SIGNAL_MKDIR:
			pszSourceDir = (HCHAR*)pstSignal->ulParam1;
			umma_fm_AddOperation(pstUmma, eUMMA_CMD_MAKEDIR, eUMMA_CMD_NONE, UMMA_UNDEFINED, UMMA_UNDEFINED, 0, pszSourceDir, NULL, NULL, nRequestId);
			break;

		case UMMA_FM_SIGNAL_STARTJOB:
			break;
		case UMMA_FM_SIGNAL_REBOOT:
			HxLOG_Warning("############ UMMA_FM_SIGNAL_REBOOT!!############\n");
			(void)UMMA_OperationQueueRemoveAllFunc(pstUmma, umma_fm_FreeOperationCBCancel);
			break;
		case UMMA_FM_SIGNAL_SHUTDOWN:
			HxLOG_Warning("############ UMMA_FM_SIGNAL_SHUTDOWN!!############\n");
			break;
		case UMMA_FM_SIGNAL_OPERATION:
			HxLOG_Warning("############ UMMA_FM_SIGNAL_OPERATION!!############\n");
			break;
		case UMMA_FM_SIGNAL_SHUTDOWNDONE:
			HxLOG_Warning("############ UMMA_FM_SIGNAL_SHUTDOWNDONE!!############\n");
			break;
		case UMMA_FM_SIGNAL_OPER_GROUP :
			{
				UMMA_FM_OPER_GROUP_t *pOperGroup = UMMA_Calloc(sizeof(UMMA_FM_OPER_GROUP_t));
				pszDestDir = (HCHAR *)pstSignal->ulParam3;

				pOperGroup->nReqId	= nRequestId;
				pOperGroup->nHndId	= pstSignal->ulParam1;
				pOperGroup->eOper	= (UAPI_FM_GROUP_OPER_e)pstSignal->ulParam2;
				HxLOG_Warning("############ UMMA_FM_SIGNAL_OPER_GROUP[%d]!!############\n", pOperGroup->eOper);
				HxSTD_StrNCpy( pOperGroup->szDestPath, pszDestDir, sizeof(pOperGroup->szDestPath)-1 );
				umma_fm_AddOperation(pstUmma, eUMMA_CMD_OPER_GROUP, eUMMA_CMD_NONE, UMMA_UNDEFINED,
						UMMA_UNDEFINED, (void *)pOperGroup, NULL, pszDestDir, NULL, nRequestId);
			}
			break;
		case UMMA_FM_SIGNAL_FLUSH:
			HxLOG_Debug("############ UMMA_FM_SIGNAL_FLUSH!!############\n");
			break;
		default:
			break;
	}

	if (pszSourceDir)
		UMMA_Free(pszSourceDir);
	if (pszDestDir)
		UMMA_Free(pszDestDir);
	if (pszExtFilter)
		UMMA_Free(pszExtFilter);
}

static HBOOL    umma_fm_findCancelOperation(void * user_data,void * list_data)
{
    UMMA_OPS_t *data = (UMMA_OPS_t*)list_data;

    if(data->eCmd == eUMMA_CMD_CANCEL)
    {
        return TRUE;
    }
    return FALSE;
}

#if !defined(NEW_OPERATION_LIST)
static UMMA_OPS_t    *umma_fm_sortingJobList(HxList_t *pOperations)
{
    HxList_t *pList = pOperations;

    pList = HLIB_LIST_FindEx(pList,NULL,umma_fm_findCancelOperation);

    return  (pList) ? (UMMA_OPS_t*)HLIB_LIST_Data(pList) : (UMMA_OPS_t*)HLIB_LIST_Data(pOperations);
}
#endif

static void		umma_fm_mainLoop(void *arg)
{
	HBOOL		bStartJOB = FALSE;
	UMMA_t		*pstUmma;
	HUINT32		ulOperationDuration, ulSleepDuration;
	HUINT32		ulUmmaIdleTime = 10;
	UMMA_EVT_t	stEvt;
#if defined(NEW_OPERATION_LIST)
	HBOOL		bSkipSleep = FALSE;
	HUINT32		ulSkipSleepNum = 0;
#endif

#ifdef	PRINT_JOB_OPERATION
	FILE		*file = NULL;
	file = fopen("/operation.job", "w");
#endif

	while (1)
	{
		pstUmma = umma_fm_getInstance();
#if defined(NEW_OPERATION_LIST)
		bSkipSleep = FALSE;
#endif
		//	will be removed when new hama api is defined.
		if (pstUmma->pstSignalHandler)
		{
			UMMA_SIGNAL_t	*pstSignal;

			HxSEMT_Get(s_unSignalSema);
			pstSignal = (UMMA_SIGNAL_t*)HLIB_LIST_Data(s_pstUmmaFm->pstSignalHandler);
			s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Remove (s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
			HxSEMT_Release(s_unSignalSema);

			if (pstSignal->ulSignal == UMMA_FM_SIGNAL_STARTJOB)
				bStartJOB = TRUE;
			else
				umma_fm_signal_Process(pstUmma, pstSignal);

			UMMA_Free(pstSignal);

			umma_fm_releaseInstance(pstUmma);
			continue;
		}

		if (bStartJOB == FALSE)
		{
			umma_fm_releaseInstance(pstUmma);
			HLIB_MSG_ReceiveTimeout(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t), 1000);
			continue;
		}

		ulOperationDuration = HLIB_STD_GetSystemTick();
		if (!UMMA_OperationQueueEmpty(pstUmma))
		{
#if defined(NEW_OPERATION_LIST)
			UMMA_OPS_t	*pstOperation = (UMMA_OPS_t *)UMMA_OperationQueueGetCurrent(pstUmma);
#else
			UMMA_OPS_t	*pstOperation = (UMMA_OPS_t *)umma_fm_sortingJobList(pstUmma->pstOperations);
#endif

#ifdef	PRINT_JOB_OPERATION
			fprintf(file, "[%d] [%s]\n", pstOperation->eCmd, pstOperation->pszSourceName);
			fflush(file);
#endif
			if (pstOperation->nGiveUp)
			{
				switch (pstOperation->eCmd)
				{
					case eUMMA_CMD_MOVE:
						if (umma_fm_operation_Move(pstUmma, pstOperation) != ERR_OK)
						{
							HxLOG_Error("Fail to canecl move [%s - %s]\n", pstOperation->pszSourceName, pstOperation->pszDestName);
						}
						break;
					case eUMMA_CMD_COPY:
						if (umma_fm_operation_Copy(pstUmma, pstOperation) != ERR_OK)
						{
							HxLOG_Error("Fail to cancel copy [%s - %s]\n", pstOperation->pszSourceName, pstOperation->pszDestName);
						}
						break;
					default :
						umma_fm_safety_operation_remove( pstUmma, (void *)pstOperation );
						umma_fm_FreeOperation(pstUmma, pstOperation);
						break;
				}
			} else
			{
				switch (pstOperation->eCmd)
				{
					case eUMMA_CMD_CANCEL:
						if (umma_fm_operation_CancelOperation(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to Cancel operation pvUser[%d][%d]\n", (HINT32)pstOperation->pvUser, pstOperation->nRequestId);
						break;
					case eUMMA_CMD_DIRECTDIR:
					case eUMMA_CMD_DIR:
					case eUMMA_CMD_LAZYDIR:
						if (umma_fm_operation_Scan(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to scanning folder named [%s:%d]\n", __FUNCTION__, __LINE__);
						break;
					case eUMMA_CMD_MOVE:
						if (umma_fm_operation_Move(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to Move files [%s - %s]\n", pstOperation->pszSourceName, pstOperation->pszDestName);
						break;
					case eUMMA_CMD_GETINFO:
					case eUMMA_CMD_GETINFO_PRIO:
						if (umma_fm_operation_GetFileInfo(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to retrieving file info [%s]\n", pstOperation->pszSourceName);
#if defined(NEW_OPERATION_LIST)
						bSkipSleep = TRUE;
#endif
						break;
					case eUMMA_CMD_OPER_GROUP :
						if (umma_fm_operation_Group_oper(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to group fileOperation destDir[%s]\n", pstOperation->pszDestName);
						break;
					case eUMMA_CMD_COPY:
						if (umma_fm_operation_Copy(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to Copy Files [%s - %s]\n", pstOperation->pszSourceName, pstOperation->pszDestName);
						break;
					case eUMMA_CMD_REMOVE:
						if (umma_fm_operation_Remove(pstUmma, pstOperation) != ERR_OK)
							HxLOG_Error("Fail to move files to recycleBin [%s]\n", pstOperation->pszSourceName);
						break;

					case eUMMA_CMD_REMOVEDIR:
						HxLOG_Print("----> removedir cmd : [%s]\n", pstOperation->pszSourceName);
						HLIB_DIR_Delete(pstOperation->pszSourceName);
						umma_fm_safety_operation_remove(pstUmma, (void *)pstOperation);
						umma_fm_FreeOperation(pstUmma, pstOperation);
						break;

					case eUMMA_CMD_REMOVEDIR_CHKSUB:
					case eUMMA_CMD_REMOVEDIR_CHKEND:
						if ( pstOperation->eCmd == eUMMA_CMD_REMOVEDIR_CHKSUB )
							HxLOG_Print("----> removedir_chksub cmd : [%s]\n", pstOperation->pszSourceName);
						else
							HxLOG_Print("----> removedir_chkend cmd : [%s]\n", pstOperation->pszSourceName);

						if ( HLIB_DIR_IsDirectoryEmpty(pstOperation->pszSourceName) )
						{
							HLIB_DIR_Delete(pstOperation->pszSourceName);
						}
						else
						{
							HxLOG_Print("----> has item : [%s]\n", pstOperation->pszSourceName);
						}
						umma_fm_safety_operation_remove(pstUmma, (void *)pstOperation);
						umma_fm_FreeOperation(pstUmma, pstOperation);
						break;

					case eUMMA_CMD_MAKEDIR:
						HLIB_DIR_Make(pstOperation->pszSourceName);
						umma_fm_safety_operation_remove(pstUmma, (void *)pstOperation);
						umma_fm_FreeOperation(pstUmma, pstOperation);
						break;
					default:
						break;
				}
			}
		}
		umma_fm_releaseInstance(pstUmma);

		if ( (UMMA_OperationQueueEmpty(pstUmma)) && (pstUmma->bTryFormat != TRUE) )
		{
			if (umma_fm_operation_FlushRecycleBin(pstUmma) == FALSE)
			{
				//HxLOG_Warning("<--- UMMA is now going to wait new operations.\n");

				//	Waiting for Adding New Operation --> Deep Sleep
				(void)HLIB_MSG_Receive(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
				while (HLIB_MSG_ReceiveTimeout(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t), 0) == ERR_OK);		//	Flush Event Buffer

				continue;
				//HxLOG_Warning("---> UMMA is now going to process operations.\n");
			}
		}

		ulOperationDuration = HLIB_STD_GetSystemTick() - ulOperationDuration;

		if (ulOperationDuration < ulUmmaIdleTime)
			ulOperationDuration = (ulUmmaIdleTime-ulOperationDuration);
		{
			ulSleepDuration = 10;

			if (ulOperationDuration > 100)
			{
				ulSleepDuration = (HUINT32)(ulOperationDuration / 10);
			}

#if defined(NEW_OPERATION_LIST)
			if (TRUE == bSkipSleep)
			{
				ulSkipSleepNum++;
				if (ulSkipSleepNum > 25)
				{
					bSkipSleep = FALSE;
					ulSkipSleepNum = 0;
				}
			}

			if (TRUE != bSkipSleep)
#endif
			{
				VK_TASK_Sleep(ulSleepDuration);
			}

			HxLOG_Debug("UMMA SLEEP (%d)\n", ulSleepDuration);
		}
	}
}

static HERROR	umma_fm_rpc_sendDummyEvent(UMMA_t *pstUmma)
{
	if (UMMA_OperationQueueEmpty(pstUmma))
	{
		UMMA_EVT_t	stEvt;
		HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
		HLIB_MSG_Send(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
	}

	return ERR_OK;
}

//	apArgv[0] = HCHAR*	pszScanDIR
//	apArgv[0] = HCHAR*	pszExtFilter
//	apArgv[2] = HINT32	bRecursive
static HERROR	umma_fm_rpc_Scan(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 3);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_SCAN;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));
	pstSignal->ulParam3 =                      HxOBJECT_INT(apArgv[2]);
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

//	apArgv[0] = HCHAR*	pszSourceDir
static HERROR	umma_fm_rpc_Remove(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 2);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_REMOVE;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

//	apArgv[0] = HCHAR*	pszSourceDir
//	apArgv[1] = HCHAR*	pszDestDir
//	apArgv[2] = HCHAR*	pszFilter
//	apArgv[3] = HINT32  nRecordings
static HERROR	umma_fm_rpc_Move(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 4);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_MOVE;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));
	pstSignal->ulParam3 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[2]));
	pstSignal->ulParam4 = HxOBJECT_INT(apArgv[3]);
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

//	apArgv[0] = HCHAR*	pszSourceDir
//	apArgv[1] = HCHAR*	pszDestDir
//	apArgv[2] = HCHAR*	pszFilter
//	apArgv[2] = HINT32	nRecordings
static HERROR	umma_fm_rpc_Copy(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 4);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_COPY;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[1]));
	pstSignal->ulParam3 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[2]));
	pstSignal->ulParam4 = HxOBJECT_INT(apArgv[3]);
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

//	apArgv[0] = HINT32 nRequestId
static HERROR	umma_fm_rpc_Cancel(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_CANCEL;
	pstSignal->ulParam1 = HxOBJECT_INT(apArgv[0]);
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

//	apArgv[0] = HCHAR* pszSource
static HERROR	umma_fm_rpc_MakeDir(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 1);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_MKDIR;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[0]));
	pstSignal->nReqId   = nRequestId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

static HERROR	umma_fm_rpc_Flush(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_FLUSH;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

/* To Do : Token Group File Operation */
static HERROR	umma_fm_rpc_group_create(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_t	*pstUmma = NULL;
	HUINT32 nHndId = 0;
	HINT32 ret = ERR_OK;
	HCHAR *pExtFilter = NULL;

	UMMA_ASSERT(nArgc == 1);
	pstUmma = umma_fm_getInstanceWO();
	pExtFilter = HxOBJECT_STR(apArgv[0]);

	nHndId = UMMA_FM_GROUP_CreateHandle( pExtFilter );

	HLIB_RPC_Response(pstUmma->nRpcHandle, nClientHandle, nRequestId, ret, "i", nHndId );
	return ERR_OK;
}

static HERROR	umma_fm_rpc_group_release(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 nHndId = 0;

	UMMA_ASSERT(nArgc == 1);

	nHndId		= (HUINT32)HxOBJECT_INT(apArgv[0]);
	UMMA_FM_GROUP_ReleaseHandle( nHndId );

	return ERR_OK;
}

static HERROR	umma_fm_rpc_group_operation(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SIGNAL_t	*pstSignal;

	UMMA_ASSERT(nArgc == 3);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_OPER_GROUP;
	pstSignal->ulParam1 = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstSignal->ulParam2 = (HUINT32)HxOBJECT_INT(apArgv[1]);
	pstSignal->ulParam3 = (HUINT32)UMMA_StrDup(HxOBJECT_STR(apArgv[2]));
	pstSignal->nReqId   = nRequestId;

	HxLOG_Print("############ UMMA_FM_RPC_OPER_GROUP!! nHandlId(%ld)############\n", (HUINT32)pstSignal->ulParam1);
	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

static HERROR	umma_fm_rpc_group_setPath(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 nHndId = 0;
	UAPI_FM_GROUP_SETPATH_e eSetPath = eUAPI_FM_GROUP_PATH_UNKNOWN;
	HCHAR pPath[512] = { 0 };

	UMMA_ASSERT(nArgc == 3);

	nHndId		= (HUINT32)HxOBJECT_INT(apArgv[0]);
	eSetPath	= (UAPI_FM_GROUP_SETPATH_e)HxOBJECT_INT(apArgv[1]);
	HxSTD_StrNCpy( pPath, HxOBJECT_STR(apArgv[2]), sizeof(pPath)-1);

	HLIB_PATH_Normalize(pPath);
	HxLOG_Print("[UMMA] hndId : %ld, set_path : %s\n", nHndId, pPath );
	switch ( eSetPath )
	{
		case eUAPI_FM_GROUP_PATH_ADD :
			UMMA_FM_GROUP_AddPath( nHndId, pPath );
			break;
		case eUAPI_FM_GROUP_PATH_REMOVE :
			UMMA_FM_GROUP_RemovePath( nHndId, pPath );
			break;
		case eUAPI_FM_GROUP_PATH_EXCLUDE :
		default :
			break;
	}
	return ERR_OK;
}

static HERROR	umma_fm_rpc_browse_Open(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_t	*pstUmma = NULL;
	UMMA_FM_BROWSE_OPEN_t  stOpt;
	HINT32 nHndId = 0;
	HINT32 ret = ERR_OK;
	HCHAR *pPath = NULL;

	UMMA_ASSERT(nArgc == 4);

	pstUmma = umma_fm_getInstanceWO();

	pPath = HxOBJECT_STR(apArgv[0]);
	stOpt.eBrowseType	= (HUINT32)HxOBJECT_INT(apArgv[1]);
	HxSTD_StrNCpy( stOpt.szFilter, HxOBJECT_STR(apArgv[2]), sizeof(stOpt.szFilter)-1 );
	stOpt.nOptFlags	= HxOBJECT_INT(apArgv[3]);
	stOpt.pvUmma		= (void *)pstUmma;
	stOpt.cbResult 		= umma_fm_operation_BrowseResult;

	HxLOG_Print("[UMMA_FM] RPC Call : BROWSE Open.requuestId : %d\n",  nRequestId);
	nHndId = UMMA_FM_BROWSE_Open( pPath, &stOpt, nRequestId );
	if( 0 ==  nHndId )
		ret = ERR_FAIL;

	HLIB_RPC_Response(pstUmma->nRpcHandle, nClientHandle, nRequestId, ret, "i", nHndId );
	return ERR_OK;
}

static HERROR	umma_fm_rpc_browse_Close(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nHndId = 0;

	UMMA_ASSERT(nArgc == 1);

	nHndId = HxOBJECT_INT(apArgv[0]);

	HxLOG_Print("[UMMA_FM] RPC Call : Browse(handle:%d), Close()\n", nHndId);
	return UMMA_FM_BROWSE_Close( nHndId );
}

static HERROR	umma_fm_rpc_browse_GetItems(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nHndId = 0;
	HINT32 nIdx = 0;
	HINT32 nReqCnt = 0;
	HINT32 nOpt = 0;

	UMMA_ASSERT(nArgc == 4);

	nHndId = HxOBJECT_INT(apArgv[0]);
	nIdx = HxOBJECT_INT(apArgv[1]);
	nReqCnt = HxOBJECT_INT(apArgv[2]);
	nOpt = HxOBJECT_INT(apArgv[3]);

	HxLOG_Print("[UMMA_FM] RPC Call : Browse(handle:%d), GetItems(%d, %d)\n", nHndId, nIdx, nReqCnt );
	return UMMA_FM_BROWSE_GetItems( nHndId, nIdx, nReqCnt, nOpt );
}

static HERROR	umma_fm_rpc_browse_ScanItems(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nHndId = 0;
	HINT32 nReqCnt = 0;
	HINT32 nOpt = 0;

	UMMA_ASSERT(nArgc == 3);

	nHndId = HxOBJECT_INT(apArgv[0]);
	nReqCnt = HxOBJECT_INT(apArgv[1]);
	nOpt = HxOBJECT_INT(apArgv[2]);

	HxLOG_Print("[UMMA_FM] RPC Call : Browse(handle:%d), ScanItems(%d)\n", nHndId, nReqCnt );
	return UMMA_FM_BROWSE_ScanItems( nHndId, nReqCnt, nOpt );
}
static HERROR	umma_fm_rpc_browse_Sort(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nHndId = 0;
	HINT32 eSort = 0;

	UMMA_ASSERT(nArgc == 2);

	nHndId = HxOBJECT_INT(apArgv[0]);
	eSort = HxOBJECT_INT(apArgv[1]);

	HxLOG_Print("[UMMA_FM] RPC Call : Browse(handle:%d), Sort(%d)\n", nHndId, eSort );
	return UMMA_FM_BROWSE_Sort( nHndId, eSort );
}

static HERROR	umma_fm_rpc_MainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	UMMA_SIGNAL_t	*pstSignal;

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_STARTJOB;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

static HERROR	umma_fm_rpc_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e		ePapiStatus;
	UMMA_t 		 	*pstUmma;
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));

	HxLOG_Warning("********************************************\n");
	HxLOG_Warning("ePapiStatus[%d]\n", ePapiStatus);
	HxLOG_Warning("********************************************\n");

	if (ePapiStatus == ePAMA_STATUS_Shutdown)
	{
#if defined(NEW_OPERATION_LIST)
		if(!(UMMA_OperationQueueEmpty(s_pstUmmaFm)))
#else
		if( s_pstUmmaFm->pstOperations)
#endif
		{
			HxLOG_Warning("############ UMMA FM Working.. ###########\n");
			PAPI_WaitForShutDown(3000);
		}
		else
		{
			HxLOG_Warning("############ UMMA_FM_SIGNAL_SHUTDOWN : Bye Bye...(T.T)############\n");
		}
		pstSignal->ulSignal = UMMA_FM_SIGNAL_SHUTDOWN;
	}
	else if (ePapiStatus == ePAMA_STATUS_Operation)
	{
		pstSignal->ulSignal = UMMA_FM_SIGNAL_OPERATION;
	}
	else if (ePapiStatus == ePAMA_STATUS_ShutdownDone)
	{
		pstSignal->ulSignal = UMMA_FM_SIGNAL_SHUTDOWNDONE;
	}
	else if(ePapiStatus == ePAMA_STATUS_Reboot)
	{
		pstSignal->ulSignal = UMMA_FM_SIGNAL_REBOOT;
	}
	else
	{
		UMMA_Free(pstSignal);
		return ERR_OK;
	}

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
static void umma_fm_onStbStatusChanged(void)
{
	/* noop */
	return;
}
#endif /* (CONFIG_FUNC_USE_DECRYPTCOPY)  */

#if defined(CONFIG_DEBUG)
static void	umma_fm_debug_InitCommandTools(void);
#endif


#define	______________API____________________

int	UMMA_FM_Init(void)
{
	UMMA_t		*pstUmma;
#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
	DI_ERR_CODE diErr = 0;
#endif
	HxSEMT_Create(&s_unSignalSema, "umma_fm_signal_sem", 0);

	pstUmma = umma_fm_getInstance();

	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Scan, "ssi", umma_fm_rpc_Scan,
		NULL, "scan folder: (HCHAR *pszFolder, HCHAR *pszExtFilter, HBOOL bRecursive)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Remove, "ss", umma_fm_rpc_Remove,
		NULL, "remove folder or file: (HCHAR *pszSource, HCHAR *pszExtFilter)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Move, "sssi", umma_fm_rpc_Move,
		NULL, "move folder or files: (HCHAR *pszSource, HCHAR *pszDest, HCHAR *pszExtFilter, HBOOL bRecordings)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Copy, "sssi", umma_fm_rpc_Copy,
		NULL, "copy folder or files: (HCHAR *pszSource, HCHAR *pszDest, HCHAR *pszExtFilter, HBOOL bRecordings)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Cancel, "i", umma_fm_rpc_Cancel,
		NULL, "cancel job with given id (HINT32 nRequestId)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Mkdir, "s", umma_fm_rpc_MakeDir,
		NULL, "make directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_Flush, "i", umma_fm_rpc_Flush,
		NULL, "Flush recycleBin folder (true)");
	HLIB_RPC_RegisterRequest(pstUmma->nRpcHandle, RPC_UAPI_FM_BROWSE_OPEN, "sisi", umma_fm_rpc_browse_Open,
		NULL, "open directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_BROWSE_CLOSE, "i", umma_fm_rpc_browse_Close,
		NULL, "close directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_BROWSE_GETITEMS, "iiii", umma_fm_rpc_browse_GetItems,
		NULL, "getitems directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_BROWSE_SCANITEMS, "iii", umma_fm_rpc_browse_ScanItems,
		NULL, "scanitems directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_BROWSE_SORT, "ii", umma_fm_rpc_browse_Sort,
		NULL, "sort directory (HCHAR *pszName)");
	/* file groupGrop operation */
	HLIB_RPC_RegisterRequest(pstUmma->nRpcHandle, RPC_UAPI_FM_GROUP_Create, "s", umma_fm_rpc_group_create,
		NULL, "group Group create (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_GROUP_Release, "i", umma_fm_rpc_group_release,
		NULL, "getitems directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_GROUP_Operation, "iis", umma_fm_rpc_group_operation,
		NULL, "getitems directory (HCHAR *pszName)");
	HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_FM_GROUP_SetPath, "iis", umma_fm_rpc_group_setPath,
		NULL, "getitems directory (HCHAR *pszName)");
	HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FM_GROUP_onUpdate, "ib", "notify event when status is chagned (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");

	HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FM_onStatusChange, "ib", "notify event when status is chagned (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");
	HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FM_onItemChanged, "is", "notify event when file or folder is changed (HINT32 nItemAddOrRemove(-1(removed), 0(modified), 1(added)), const HCHAR *pszName)");
	HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_FM_BROWSE_onResult, "ibb", "notify event when browse result Finished" );

	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, umma_fm_rpc_MainApplicationReady);
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, umma_fm_rpc_onSystemStateChanged);

	//HLIB_RPC_PrintHelp(pstUmma->nRpcHandle, NULL);

//	UMMA broadcast signal mount or unmount.
//	HAPI_ConnectSignal("signal:onStorageMounted",   NULL, umma_fm_system_onStorageMounted);
//	HAPI_ConnectSignal("signal:onStorageUnmounted", NULL, umma_fm_system_onStorageUnmounted);


#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
	/*
	 * For using the di_crypt.so (nexus client shared lib), check configs:
	 * - device/product_config/defconfigs/$(model)/rev0.x/platform_defconfig
	 *   CONFIG_DI_MODULE_SHARED_LIB=y
	 *   CONFIG_DI_MODULE_CRYPT_SHARED_LIB=y
	 *
	 * - apps/make/product_configs/$(model)_defconfig"
	 *   CONFIG_FUNC_USE_DECRYPTCOPY=y
	 */
	diErr = DI_CRYPT_Init();
	if (diErr != DI_ERR_OK)
	{
			HxLOG_Error("Error> Can't initialize the DI crypto\n");
			/* exit(-1);*/
	}

	(void)OAPI_SYSINFO_STB_Init(umma_fm_onStbStatusChanged);
#endif

	umma_fm_releaseInstance(pstUmma);

#if defined(CONFIG_DEBUG)
	umma_fm_debug_InitCommandTools();
#endif

	UMMA_FM_BROWSE_Init();
	return 0;
}

HERROR	UMMA_FM_AddStorage(HCHAR* pszMountPath, HCHAR* pszUUID, DxStorageInterface_e eInterface, HUINT64 ullReservedSizeKb)
{
	UMMA_SIGNAL_t	*pstSignal;
	UMMA_FM_StorageInfo_t *pstInfo;

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstInfo = (UMMA_FM_StorageInfo_t*)UMMA_Calloc(sizeof(UMMA_FM_StorageInfo_t));

	HxLOG_Assert(pstSignal);
	HxLOG_Assert(pstInfo);

	pstInfo->eInterface = eInterface;
	pstInfo->ullReservedSizeKb = ullReservedSizeKb;

	pstSignal->ulSignal = UMMA_FM_SIGNAL_MOUNT;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(pszMountPath);
	pstSignal->ulParam2 = (HUINT32)UMMA_StrDup(pszUUID);
	pstSignal->ulParam3 = (HUINT32)pstInfo;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

HERROR	UMMA_FM_RemoveStorage(HCHAR* pszMountPath)
{
	UMMA_SIGNAL_t	*pstSignal;

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_UNMOUNT;
	pstSignal->ulParam1 = (HUINT32)UMMA_StrDup(pszMountPath);

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

HERROR	UMMA_FM_ReportFormatStart(HUINT32 ulDeviceId)
{
	UMMA_SIGNAL_t *pstSignal;
	UMMA_t			*pstUmma;

	pstUmma = umma_fm_getInstance();
	pstUmma->bTryFormat = TRUE;
	umma_fm_releaseInstance(pstUmma);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = UMMA_FM_SIGNAL_FORMAT;
	pstSignal->ulParam1 = ulDeviceId;

	HxSEMT_Get(s_unSignalSema);
	s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
	HxSEMT_Release(s_unSignalSema);

	umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);

	return ERR_OK;
}

HERROR	UMMA_FM_ReportFormatDone(void)
{
	UMMA_t	*pstUmma;

	pstUmma = umma_fm_getInstance();
	pstUmma->bTryFormat = FALSE;
	umma_fm_releaseInstance(pstUmma);

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
static HCHAR*	umma_fm_debug_GetCmdName(UMMA_CMD_e eCmd)
{
	switch (eCmd)
	{
		ENUM_TO_STR(eUMMA_CMD_NONE);
		ENUM_TO_STR(eUMMA_CMD_CANCEL);
		ENUM_TO_STR(eUMMA_CMD_COPY);
		ENUM_TO_STR(eUMMA_CMD_DIR);
		ENUM_TO_STR(eUMMA_CMD_DIRECTDIR);
		ENUM_TO_STR(eUMMA_CMD_GETINFO);
		ENUM_TO_STR(eUMMA_CMD_GETINFO_PRIO);
		ENUM_TO_STR(eUMMA_CMD_MAKEDIR);
		ENUM_TO_STR(eUMMA_CMD_OPER_GROUP);
		ENUM_TO_STR(eUMMA_CMD_MOVE);
		ENUM_TO_STR(eUMMA_CMD_REMOVE);
		ENUM_TO_STR(eUMMA_CMD_REMOVEDIR);
		ENUM_TO_STR(eUMMA_CMD_REMOVEDIR_CHKSUB);
		ENUM_TO_STR(eUMMA_CMD_LAZYDIR);
		default:
			return "undefined";
	}

	return "undefined";
}

static void		umma_fm_debug_PrintOperationInfo(UMMA_OPS_t *pstOps)
{
	HLIB_CMD_Printf("%20s | %20s | %08x | %s | %s | %s\n", umma_fm_debug_GetCmdName(pstOps->eCmd), umma_fm_debug_GetCmdName(pstOps->ePostCmd),
		pstOps->nRequestId, (pstOps->nRecursive ? "RE" : "  "), (pstOps->nFlags ? "DE" : "  "), pstOps->pszSourceName);
}

static void		umma_fm_debug_PrintOperationDetailInfo(UMMA_OPS_t *pstOps)
{
	HLIB_CMD_Printf("%20s [%d] bChunked [%d] bOnGoing [%d] nChunkNo [%d]\n", "bOpsReady", pstOps->stDetail.bOpsReady, pstOps->stDetail.bChunked,
		pstOps->stDetail.bOnGoing, pstOps->stDetail.ex.nChunkNo);
}

static void umma_fm_debug_cbPrintJob (UMMA_t *umma, void *operation, void *userdata)
{
	UMMA_OPS_t	*pstOps = (UMMA_OPS_t *)operation;
	UMMA_CMD_e	 eFilterCmd = (UMMA_CMD_e)userdata;

	if (!eFilterCmd || (eFilterCmd && pstOps->eCmd == eFilterCmd))
		umma_fm_debug_PrintOperationInfo(pstOps);
}

static HERROR	umma_fm_debug_PrintJobList(UMMA_t *pstUmma, UMMA_CMD_e eFilterCmd)
{
	HLIB_CMD_Printf("\n************* UMMA JOB LIST *************\n");
	(void)UMMA_OperationQueueForEach(pstUmma, (void *)eFilterCmd, umma_fm_debug_cbPrintJob);
	HLIB_CMD_Printf("*****************************************\n");

	return ERR_OK;
}

static int	umma_fm_debug_cmdPrintJobList(void *szArgs)
{
	HINT8	*pszArg;
	UMMA_CMD_e	eFilterCmd;
	UMMA_t	*pstUmma;

	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (pszArg == NULL)
		eFilterCmd = eUMMA_CMD_NONE;
	else
		eFilterCmd = HLIB_STD_StrToINT32(pszArg);

	pstUmma = umma_fm_getInstance();
	umma_fm_debug_PrintJobList(pstUmma, eFilterCmd);
	umma_fm_releaseInstance(pstUmma);

	return HxCMD_OK;
}

static int	umma_fm_debug_cmdPrintCurrent(void *szArgs)
{
	UMMA_t	*pstUmma;
	UMMA_OPS_t	*pstOps;

	HLIB_CMD_Printf("\n************ Current JOB Info *************\n");
	pstUmma = umma_fm_getInstance();
	ENTER_OPERATIONS_SEM;
	pstOps = (UMMA_OPS_t*)UMMA_OperationQueueGetCurrent(pstUmma);
	if (pstOps)
	{
		umma_fm_debug_PrintOperationInfo(pstOps);
		umma_fm_debug_PrintOperationDetailInfo(pstOps);
	}
	LEAVE_OPERATIONS_SEM;
	umma_fm_releaseInstance(pstUmma);
	HLIB_CMD_Printf("*******************************************\n");

	return HxCMD_OK;
}

static int	umma_fm_debug_cmdGroupOper(void *szArgs)
{
	const HCHAR *pTestPath = "/mnt/hd2/Media/Video/FILE_OPER_TEST";
	const HCHAR *pExtFilter = "ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe";
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	static HUINT32 nHndId = 0;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("\n[UMMA_FM] group operation : %s\n", aucStr);

	if ( !HxSTD_StrCmp( "CREATE", aucStr ) )
	{
		if ( nHndId )
		{
			UMMA_FM_GROUP_ReleaseHandle( nHndId );
			nHndId = 0;
		}
		nHndId = UMMA_FM_GROUP_CreateHandle( pExtFilter );
		HLIB_CMD_Printf("\n[UMMA_FM] Create Handle : %ld\n", nHndId);
	}
	else if ( !HxSTD_StrCmp( "RELEASE", aucStr ) )
	{
		HUINT32 temp = 0;
		temp = nHndId;
		if ( nHndId )
		{
			UMMA_FM_GROUP_ReleaseHandle( nHndId );
			nHndId = 0;
		}
		HLIB_CMD_Printf("\n[UMMA_FM] Release Handle : %ld\n", temp);
	}
	else if ( !HxSTD_StrCmp( "ADD", aucStr ) || !HxSTD_StrCmp( "REMOVE", aucStr ) )
	{
		HUINT8		szFullPath[MAX_HXDIR_FILENAME_LENGTH] = { 0 };
		HUINT8		*pszName = NULL;
		HINT32 itemCnt = 0;
		HINT32 fileCnt = 0;
		HBOOL bAdd = FALSE;

		HxDIR_t *pDir = NULL;
		if ( TRUE != HLIB_DIR_IsExist(pTestPath) )
		{
			HLIB_CMD_Printf("\n[UMMA_FM] Error : not existed Path %s\n", pTestPath);
			return HxCMD_OK;
		}

		if ( 0 == nHndId )
			nHndId = UMMA_FM_GROUP_CreateHandle( pExtFilter );

		bAdd =  (!HxSTD_StrCmp( "ADD", aucStr )) ? TRUE : FALSE ;

		pDir = HLIB_DIR_Open(pTestPath);
		do
		{
			pszName = HLIB_DIR_GetName(pDir);
			if(pszName[0] == '.')
			{
				if(pszName[1] == 0)
					continue;
				else if(pszName[1] == '.' && pszName[2] == 0)
					continue;
			}

			snprintf(szFullPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", HLIB_DIR_GetPath(pDir), pszName);

			if ( bAdd )
				UMMA_FM_GROUP_AddPath( nHndId, szFullPath );
			else
				UMMA_FM_GROUP_RemovePath( nHndId, szFullPath );
			++fileCnt;
		}
		while(HLIB_DIR_Next(pDir) == ERR_OK);
		HLIB_DIR_Close(pDir);

		UMMA_FM_GROUP_GetItemCount( nHndId, &itemCnt );
		HLIB_CMD_Printf("\n[UMMA_FM] Handle : %ld, file count : %d, item count : %d\n", nHndId, fileCnt, itemCnt);
	}
	else if ( !HxSTD_StrCmp( "SIZE", aucStr ) )
	{
		HUINT64 ullSize = 0;
		if ( 0 == nHndId )
			return HxCMD_OK;
		UMMA_FM_GROUP_GetTotalSize( nHndId, &ullSize );
		HLIB_CMD_Printf("\n[UMMA_FM] Handle : %ld, total size : %lld KByte\n", nHndId, ullSize ? ullSize/1024 : 0);
	}
	else if ( !HxSTD_StrCmp( "COPY", aucStr ) )
	{
		UMMA_SIGNAL_t	*pstSignal;
		HCHAR szPath[512] = { 0 };
		HCHAR *pDest = "/mnt/hd2/Media/Download";
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( !aucArg )
			HxSTD_StrNCpy( szPath, pDest, sizeof(szPath)-1);
		else
			sscanf(aucArg, "%s", (char *)szPath);

		pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
		pstSignal->ulSignal = UMMA_FM_SIGNAL_OPER_GROUP;
		pstSignal->ulParam1 = nHndId;
		pstSignal->ulParam2 = eUAPI_FM_GROUP_OPER_COPY;
		pstSignal->ulParam3 = (HUINT32)UMMA_StrDup(szPath);
		pstSignal->nReqId   = 512;

		HLIB_CMD_Printf("\n[UMMA_FM] Handle : %ld, Copy Start!! dest : %s\n", nHndId,  szPath);
		HxSEMT_Get(s_unSignalSema);
		s_pstUmmaFm->pstSignalHandler = HLIB_LIST_Append(s_pstUmmaFm->pstSignalHandler, (void*)pstSignal);
		HxSEMT_Release(s_unSignalSema);

		umma_fm_rpc_sendDummyEvent(s_pstUmmaFm);
	}

	return HxCMD_OK;
}

static void		umma_fm_debug_InitCommandTools(void)
{
	HLIB_CMD_RegisterWord("main", umma_fm_debug_cmdPrintJobList,
						"printjob",
						"print umma job list",
						"printjob filter [eCmd]");
	HLIB_CMD_RegisterWord("main", umma_fm_debug_cmdPrintCurrent,
						"printcurrent",
						"print current job info",
						"printcurrent");
	HLIB_CMD_RegisterWord("fm", umma_fm_debug_cmdGroupOper,
						"group",
						"group operation",
						"group [create|release|add|copy|size]");
}
#endif

