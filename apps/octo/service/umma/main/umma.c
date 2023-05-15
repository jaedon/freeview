/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma.c
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
#include	"umma_int.h"
#include 	<umma_fm.h>
#include 	<umma_fs.h>
#include 	<umma_hotplug.h>
#include 	<umma_smbc.h>
#include 	<umma_smbs.h>
#include 	<umma_ftps.h>

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

#if defined(CONFIG_SUPPORT_UMMA_SECURE_STORAGE)
#include 	<di_crypt.h>
#endif
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
static UMMA_t	*s_pstUmma = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		umma_mainLoop(void *arg);

static UMMA_t*	umma_getInstance(void)
{
	if (s_pstUmma == NULL)
	{
		unsigned long	tid;

		s_pstUmma = (UMMA_t*)UMMA_Calloc(sizeof(UMMA_t));

		HxSEMT_Create(&s_pstUmma->ulSemId, "ummasem", 0);

		HxSEMT_Get(s_pstUmma->ulSemId);
		//	UMMA MainTask의 Wakeup을 위한 Event.
		HLIB_MSG_Create(128, sizeof(UMMA_EVT_t), "ummaMainMgrEvt", (unsigned long*)&s_pstUmma->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO);

		VK_TASK_Create(umma_mainLoop, 70, SIZE_64K, "ummaMainMgrTask", (void*)s_pstUmma, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}
	else
	{
		HxSEMT_Get(s_pstUmma->ulSemId);
	}

	return s_pstUmma;
}

static UMMA_t*	umma_getInstanceWO(void)
{
	return s_pstUmma;
}

static void		umma_releaseInstance(UMMA_t *pstUmma)
{
	HxSEMT_Release(pstUmma->ulSemId);
}


static DxStorageInterface_e umma_getInterface(DxStorage_DevType_e  eDevType)
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

static void		umma_eventProcess(UMMA_t *pstUmma, UMMA_EVT_t *pstEvent)
{
	HUINT32		ulDeviceId;
	HINT32		nEvt, nP1, nP2, nP3;
	HERROR		hErr;

	if (!pstEvent)
	{
		HxLOG_Error("pstEvent is null.. \n");
		return;
	}

	nEvt = pstEvent->nEvent;
	nP1 = pstEvent->nParam1;
	nP2 = pstEvent->nParam2;
	nP3 = pstEvent->nParam3;

	ulDeviceId = (HUINT32)nP1;
	switch (nEvt)
	{
		case eUMMA_EVT_STORAGE_ADDED:
			{
				DxStorage_Info_t stStorageInfo = {0, };

				hErr = UMMA_FS_GetStorageInfo(ulDeviceId, &stStorageInfo);
				if (hErr != ERR_OK)
				{
					break;
				}
				if (stStorageInfo.nPartitionNum >= 1)
				{
					HxLOG_Warning("[eUMMA_EVT_STORAGE_ADDED] [Device : %d] MountPath : %s, Interface : %d\n", ulDeviceId, stStorageInfo.szMountPath, umma_getInterface(stStorageInfo.eDevType));
					UMMA_FM_AddStorage(stStorageInfo.szMountPath, stStorageInfo.szUUID, umma_getInterface(stStorageInfo.eDevType), stStorageInfo.ullReservedSizeKb);
				}
			}
			break;

		case	eUMMA_EVT_STORAGE_REMOVED:
			{
				DxStorage_Info_t stStorageInfo = {0, };
				HCHAR *pszMountPath = (HCHAR *)nP2;

				hErr = UMMA_FS_GetStorageInfo(ulDeviceId, &stStorageInfo);
				if (hErr != ERR_OK)
				{
					HxLOG_Warning("Error, Fail to Get Storage Info.\n");
					if(pszMountPath == NULL)
					{
						break;
					}
					else
					{
						HxSTD_snprintf(stStorageInfo.szMountPath, DxSTORAGE_NAME_LEN, "%s", pszMountPath);
						UMMA_FM_RemoveStorage(stStorageInfo.szMountPath);
					}
				}
				else
				{
					HxLOG_Warning("[eUMMA_EVT_STORAGE_REMOVED] [Device : %d] MountPath : %s\n", ulDeviceId, stStorageInfo.szMountPath);

					if (stStorageInfo.nPartitionNum >= 1)
					{
						UMMA_FM_RemoveStorage(stStorageInfo.szMountPath);
					}
				}

				if(pszMountPath)
				{
					UMMA_Free(pszMountPath);
				}
			}
			break;

		case 	eUMMA_EVT_STORAGE_FORMAT_START:
			{
				UMMA_FM_ReportFormatStart(ulDeviceId);
			}
			break;

		case 	eUMMA_EVT_STORAGE_FORMAT_DONE:
			{
				UMMA_FM_ReportFormatDone();
			}
			break;

		default:
			break;
	}
}

static void		umma_mainLoop(void *arg)
{
	UMMA_t		*pstUmma;
	UMMA_EVT_t	stEvt;

	while (1)
	{
		pstUmma = umma_getInstanceWO();

		if (HLIB_MSG_Receive(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t)) == ERR_OK)
		{
			pstUmma = umma_getInstance();
			umma_eventProcess(pstUmma, &stEvt);
			umma_releaseInstance(pstUmma);
		}
	}
}



static void	umma_sendEvent(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	UMMA_t *pstUmma;

	pstUmma = umma_getInstanceWO();

	if (UMMA_OperationQueueEmpty(pstUmma))
	{
		UMMA_EVT_t	stEvt;
		HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

		stEvt.nEvent = nEvt;
		stEvt.nParam1 = nP1;
		stEvt.nParam2 = nP2;
		stEvt.nParam3 = nP3;

		HLIB_MSG_Send(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
	}
}

int umma_mount_secure_storage(char *strFolder, char *strPath)
{
#if defined(CONFIG_SUPPORT_UMMA_SECURE_STORAGE)
	char strPassphrase[16] = {0,};
	char strPassphraseToAscii[32+1] = {0,};
	char strShellCmd[1024] = {0,};
	int idx = 0;
	static int sOnceDiCryptInit = FALSE;
	static int sMount_cnt = 0;
	char strDirName[1024] = {0,};
	HBOOL	bFound = FALSE;

	/* Step0: Magic String */
	sprintf(strPassphrase, "HUMAX_%d", ++sMount_cnt);
	if(sMount_cnt > 0x0FFFFFFF)
		sMount_cnt = 0;

	memset(strPassphraseToAscii, 0x00, sizeof(strPassphraseToAscii));
	memset(strShellCmd, 0x00, sizeof(strShellCmd));

	/* Step1: Get PassPhase from Chip(OtpA) */
	{
		stCrypt_t	CryptInfo;
#if 0 /* CRYPT_Init called at UMMA_FM_Init() */
		if(sOnceDiCryptInit == FALSE)
		{
			if(DI_CRYPT_Init() == DI_ERR_OK)
			{
				sOnceDiCryptInit = TRUE;
			}
			else
			{
				HxLOG_Warning("Error DI_CRYPT_Init\n");
				return DI_ERR_ERROR;
			}
		}
#endif
		VK_MEM_Memset((void *)&CryptInfo, 0, sizeof(stCrypt_t));
		CryptInfo.etCryptMode = DI_CRYPT_AES;
		CryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
		CryptInfo.etCryptData = DI_CRYPT_BLOCK;
		CryptInfo.pKey = NULL;

		CryptInfo.ulKeySize = 16;
		CryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(sizeof(strPassphrase));

		if(CryptInfo.pSrcBuffer == NULL)
		{
			HxLOG_Warning("Error DI_CRYPT_MemAlloc strSrcBuffer NULL\n");
			return DI_ERR_ERROR;
		}

		VK_MEM_Memcpy((void *)CryptInfo.pSrcBuffer, (void *)strPassphrase, sizeof(strPassphrase));
		CryptInfo.ulBuffSize = sizeof(strPassphrase);

		CryptInfo.pDstBuffer = CryptInfo.pSrcBuffer;
		if(CryptInfo.pDstBuffer == NULL)
		{
			HxLOG_Warning("Error DI_CRYPT_MemAlloc pDstBuffer NULL\n");
			DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
			return DI_ERR_ERROR;
		}

		if(DI_CRYPT_Encrypt(&CryptInfo) != DI_ERR_OK)
		{
			HxLOG_Warning("Error DI_CRYPT_Encrypt\n");
			return DI_ERR_ERROR;
		}

		VK_MEM_Memset(strPassphrase, 0x0, sizeof(strPassphrase));
		VK_MEM_Memcpy(strPassphrase, (void *)CryptInfo.pSrcBuffer, sizeof(strPassphrase));

		DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
	}

	/* Step2: Register Kry to Keyring */
	{
		HxLOG_Warning("strPassphrase:");
		for(idx=0; idx < sizeof(strPassphrase); ++idx)
		{
			sprintf(&strPassphraseToAscii[idx*2], "%02X", (unsigned char)strPassphrase[idx]);
			fprintf(stderr, "%02X ", (unsigned char)strPassphrase[idx]);
		}
		HxLOG_Warning("\n");

		HxLOG_Warning("strPassphraseToAscii: [%s]\n", strPassphraseToAscii);


		memset(strDirName, 0x00, sizeof(strDirName));
		sprintf(strDirName, "%s/.ecryptfs", strPath);
		bFound = HLIB_DIR_IsExist(strDirName);
		if(bFound == FALSE)
		{
			memset(strShellCmd, 0x00, sizeof(strShellCmd));
			sprintf(strShellCmd, "mkdir %s/.ecryptfs", strPath);
			system(strShellCmd);
		}


		/* Get signiture to File */
		memset(strShellCmd, 0x00, sizeof(strShellCmd));
		sprintf(strShellCmd,
				"printf \"%%s\" \"%s\" | ecryptfs-add-passphrase "
				"| grep 'Inserted auth' | cut -d']' -f 1 | cut -d'[' -f2 > %s/.ecryptfs/%s.sig",
				strPassphraseToAscii, strPath, strFolder);
		system(strShellCmd);
	}


	/* Step3: Mount Storage */
	{

		memset(strDirName, 0x00, sizeof(strDirName));
		sprintf(strDirName, "%s/.%s", strPath, strFolder);
		bFound = HLIB_DIR_IsExist(strDirName);
		if(bFound == FALSE)
		{
			memset(strShellCmd, 0x00, sizeof(strShellCmd));
			sprintf(strShellCmd, "mkdir %s/.%s", strPath, strFolder);
			system(strShellCmd);

		}


		memset(strDirName, 0x00, sizeof(strDirName));
		sprintf(strDirName, "%s/%s", strPath, strFolder);
		bFound = HLIB_DIR_IsExist(strDirName);
		if(bFound == FALSE)
		{
			memset(strShellCmd, 0x00, sizeof(strShellCmd));
			sprintf(strShellCmd, "mkdir %s/%s", strPath, strFolder);
			system(strShellCmd);
		}

		memset(strShellCmd, 0x00, sizeof(strShellCmd));
		sprintf(strShellCmd, "mount.ecryptfs_private %s %s", strFolder, strPath);
		system(strShellCmd);
	}

	/* Check */
	{
		memset(strShellCmd, 0x00, sizeof(strShellCmd));
		sprintf(strShellCmd, "ls -al %s", strPath);
		system(strShellCmd);
		sprintf(strShellCmd, "ls -al %s/.ecryptfs", strPath);
		system(strShellCmd);
	}
#endif
	return 0;
}


int	umma_main(int argc, char **argv)
{
//	unsigned long	tid;

	VK_Init();

	HxLOG_Warning("------------- Start UMMA ---------------- \n");
	HxLOG_Warning(" 		 Usb & Mass storage MAnager \n");
	HxLOG_Warning("--------------------------------------- \n");

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("umma");

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	HLIB_DBG_Init();
	HLIB_DBG_SetupTrace("ummacmd", 5, "s:op", "s:src", "s:dst", "s:ft", "s:rec");

	umma_releaseInstance(umma_getInstance());

	UMMA_FS_Init();
	UMMA_FM_Init();
	UMMA_HOTPLUG_Init();

	HxLOG_Warning("------------- Start Mount drm ---------------- \n");
	umma_mount_secure_storage("drm", "/var/lib/humaxtv");
	HxLOG_Warning("------------- Start Mount browser ---------------- \n");
	umma_mount_secure_storage("browser", "/var/lib/humaxtv");

#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	UMMA_SMBC_Init();
#endif

#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	UMMA_SMBS_Init();
#endif

#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	UMMA_FTPS_Init();
#endif

	return 0;
}

int	main(int argc, char **argv)
{
	umma_main(argc, argv);

	while (1)
		VK_TASK_Sleep(100000);

	return 0;
}

void	UMMA_SendEvent(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	umma_sendEvent(nEvt, nP1, nP2, nP3);
}

static HINT32	umma_operation_CmpOperationPriority (void *a, void *b)
{
	UMMA_OPS_t	*pstSrc, *pstDst;

	pstSrc = (UMMA_OPS_t*)a;
	pstDst = (UMMA_OPS_t*)b;

	return pstDst->eCmd - pstSrc->eCmd;
}

HBOOL UMMA_OperationQueueEmpty (UMMA_t *pstUmma)
{
#if defined(NEW_OPERATION_LIST)
	// need sema......
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32			 ulCnt, ulNum = pstUmma->stOperationQueue.ulNumHierachy;

		for (ulCnt = 0; ulCnt < ulNum; ulCnt++)
		{
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			if (NULL != pstQueue->pstHead)
			{
				return FALSE;
			}
		}

		return TRUE;
	}
#endif

	return (NULL == pstUmma->pstOperations) ? TRUE : FALSE;
}

HERROR UMMA_OperationQueueRemoveItem (UMMA_t *pstUmma, const void *pvOperation)
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		UMMA_OPS_t		*pstOperation = (UMMA_OPS_t *)pvOperation;

		if (NULL != pstOperation)
		{
			HxList_t				*pstListItem;
			UMMA_OperationQueue_t	*pstQueue;

			if (pstOperation->eCmd >= eUMMA_CMD_MAX)
			{
				HxLOG_Error("Operator CMD value invalid!! (%d)\n", pstOperation->eCmd);
				return ERR_FAIL;
			}

			pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[pstOperation->eCmd]);

			pstListItem = HLIB_LIST_Find(pstQueue->pstHead, (const void *)pstOperation);
			if (NULL != pstListItem)
			{
				if (pstListItem == pstQueue->pstTail)
				{
					pstQueue->pstTail = pstListItem->prev;
				}

				if (pstListItem == pstQueue->pstHead)
				{
					pstQueue->pstHead = pstListItem->next;
				}

				if (pstListItem->prev)
				{
					pstListItem->prev->next = pstListItem->next;
				}

				if (pstListItem->next)
				{
					pstListItem->next->prev = pstListItem->prev;
				}

				HLIB_MEM_Free(pstListItem);
			}
		}

		return ERR_OK;
	}
#endif

	pstUmma->pstOperations = HLIB_LIST_Remove(pstUmma->pstOperations, pvOperation);
	return ERR_OK;
}

HERROR UMMA_OperationQueueRemoveItemIf (UMMA_t *pstUmma, void *pvUser, HBOOL (*pfnCondition)(void *userdata, void *data), void (*pfnFree)(void *data))
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32			 ulCnt;

		for (ulCnt = 0; ulCnt < pstUmma->stOperationQueue.ulNumHierachy; ulCnt++)
		{
			HBOOL					 bCondition;
			HBOOL					 bFirst;
			HxList_t				*pstCurrItem = NULL, *pstNextItem = NULL, *pstFirstItem = NULL, *pstLastItem = NULL;
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			bFirst = TRUE;
			pstCurrItem = pstQueue->pstHead;
			while (pstCurrItem)
			{
				bCondition = FALSE;
				pstNextItem = pstCurrItem->next;

				if (pfnCondition)
				{
					bCondition = pfnCondition(pvUser, pstCurrItem->data);
				}

				if (bCondition)
				{
					if (pstCurrItem->prev)
					{
						pstCurrItem->prev->next = pstCurrItem->next;
					}

					if (pstCurrItem->next)
					{
						pstCurrItem->next->prev = pstCurrItem->prev;
					}

					if (pfnFree)
					{
						pfnFree(pstCurrItem->data);
						pstCurrItem->data = NULL;
					}

					HLIB_MEM_Free(pstCurrItem);
				}
				else
				{
					if (bFirst)
					{
						pstFirstItem = pstCurrItem;
						bFirst = FALSE;
					}

					pstLastItem = pstCurrItem;
				}

				pstCurrItem = pstNextItem;
			}

			pstQueue->pstHead = pstFirstItem;
			pstQueue->pstTail = pstLastItem;
		}

		return ERR_OK;
	}
#endif

	pstUmma->pstOperations = HLIB_LIST_RemoveIf(pstUmma->pstOperations, pvUser, pfnCondition, pfnFree);
	return ERR_OK;
}

HERROR UMMA_OperationQueueRemoveAllFunc (UMMA_t *pstUmma, void (*pfnFree)(void *data))
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32			 ulCnt;

		for (ulCnt = 0; ulCnt < pstUmma->stOperationQueue.ulNumHierachy; ulCnt++)
		{
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			pstQueue->pstHead = HLIB_LIST_RemoveAllFunc(pstQueue->pstHead, pfnFree);
			pstQueue->pstTail = NULL;
		}

		return ERR_OK;
	}
#endif

	pstUmma->pstOperations = HLIB_LIST_RemoveAllFunc(pstUmma->pstOperations, pfnFree);
	return ERR_OK;
}

HUINT32 UMMA_OperationQueueInsertWithCmdPriority (UMMA_t *pstUmma, void *pvOperation)
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		UMMA_OPS_t		*pstOperation = (UMMA_OPS_t *)pvOperation;

		if (NULL != pstOperation)
		{
			HxList_t				*pstListItem;
			UMMA_OperationQueue_t	*pstQueue;

			if (pstOperation->eCmd >= eUMMA_CMD_MAX)
			{
				HxLOG_Error("Operator CMD value invalid!! (%d)\n", pstOperation->eCmd);
				return ERR_FAIL;
			}

			pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[pstOperation->eCmd]);

			if (NULL == pstQueue->pstHead)
			{
				pstQueue->pstHead = HLIB_LIST_Append(pstQueue->pstHead, pvOperation);
				pstQueue->pstTail = pstQueue->pstHead;
			}
			else
			{
				if (NULL == pstQueue->pstTail)
				{
					pstQueue->pstTail = HLIB_LIST_Last(pstQueue->pstHead);
				}

				pstListItem = (HxList_t *)HLIB_MEM_Calloc(sizeof(HxList_t));
				if (NULL != pstListItem)
				{
					pstListItem->data = pvOperation;
					pstQueue->pstTail->next = pstListItem;
					pstListItem->prev = pstQueue->pstTail;
					pstQueue->pstTail = pstListItem;
				}
			}
		}

		return ERR_OK;
	}
#endif

	pstUmma->pstOperations = HLIB_LIST_AppendSortEx(pstUmma->pstOperations, pvOperation, umma_operation_CmpOperationPriority);
	return ERR_OK;
}

HUINT32 UMMA_OperationQueueLength (UMMA_t *pstUmma)
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32			 ulCnt, ulNum = pstUmma->stOperationQueue.ulNumHierachy;
		HUINT32			 ulTotal = 0;

		for (ulCnt = 0; ulCnt < ulNum; ulCnt++)
		{
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			ulTotal += HLIB_LIST_Length(pstQueue->pstHead);
		}

		return ulTotal;
	}
#endif

	return HLIB_LIST_Length(pstUmma->pstOperations);
}

HERROR UMMA_OperationQueueForEach (UMMA_t *pstUmma, void *pvUserData, void (*pfnEachJob)(UMMA_t *umma, void *operation, void *userdata))
{
	HxList_t		*pstListItem;

#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32 		 ulCnt, ulNum = pstUmma->stOperationQueue.ulNumHierachy;
		HUINT32 		 ulTotal = 0;

		for (ulCnt = 0; ulCnt < ulNum; ulCnt++)
		{
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			for (pstListItem = pstQueue->pstHead; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
			{
				if (pfnEachJob)
				{
					pfnEachJob(pstUmma, pstListItem->data, pvUserData);
				}
			}
		}

		return ERR_OK;
	}
#endif

	for (pstListItem = pstUmma->pstOperations; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
	{
		if (pfnEachJob)
		{
			pfnEachJob(pstUmma, pstListItem->data, pvUserData);
		}
	}

	return ERR_OK;
}

void *UMMA_OperationQueueGetCurrent (UMMA_t *pstUmma)
{
#if defined(NEW_OPERATION_LIST)
	if (TRUE == pstUmma->bUseOprPriorQueue)
	{
		HUINT32 		 ulCnt, ulNum = pstUmma->stOperationQueue.ulNumHierachy;
		HUINT32 		 ulTotal = 0;

		for (ulCnt = 0; ulCnt < ulNum; ulCnt++)
		{
			UMMA_OperationQueue_t	*pstQueue = &(pstUmma->stOperationQueue.pstQueueArray[ulCnt]);

			if (NULL != HLIB_LIST_Data(pstQueue->pstHead))
			{
				return HLIB_LIST_Data(pstQueue->pstHead);
			}
		}

		return NULL;
	}
#endif

	return HLIB_LIST_Data(pstUmma->pstOperations);
}

