/**
	@file     otl_ramdisk.c

	Description: MW와 AP가 공통으로 사용할 OCTO RAM Disk
	Module: MWC/RAMDISK			 	\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <linkedlist.h>

#include <otl.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define RAM_FILE_NAME_LEN			256
#define	RAM_DISK_CLUSTER_SIZE		1024
#define RAM_DISK_CLUSTER_BIT		10


#define	ENTER_CRITICAL_RAMFILE		VK_SEM_Get(s_ulRamDisk_FileSemId)
#define	LEAVE_CRITICAL_RAMFILE		VK_SEM_Release (s_ulRamDisk_FileSemId)
#define	ENTER_CRITICAL_RAMINST		VK_SEM_Get(s_ulRamDisk_FileInstSemId)
#define	LEAVE_CRITICAL_RAMINST		VK_SEM_Release (s_ulRamDisk_FileInstSemId)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eFileMode_ReadOnly,
	eFileMode_Write,
	eFileMode_Append

} otlRamdist_FileOpenMode_t;

typedef struct tagRAM_DATA_T
{
	HUINT8					 aucData[RAM_DISK_CLUSTER_SIZE];
	struct tagRAM_DATA_T	*pstNext;
} otlRamdist_Data_t;


typedef struct tagRAM_FILE_T
{
	HBOOL				 bFileAvailable;
	HBOOL				 bWriteLocked;

	HCHAR				 szFileName[RAM_FILE_NAME_LEN];

	otlRamdist_Data_t	*pstBuffer;
	HUINT32				 ulDataSize, ulBufferSize;

} otlRamdist_File_t;

typedef struct tagRAM_FILE_INST_T
{
	Handle_t					 hRamFileHandle;
	HBOOL						 bAvailable;

	otlRamdist_File_t			*pstFile;
	otlRamdist_FileOpenMode_t	 eFileMode;
	HUINT32						 ulPosition;

} otlRamdist_Inst_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC unsigned long	 s_ulRamDisk_FileSemId;
STATIC unsigned long	 s_ulRamDisk_FileInstSemId;

// File Objects
STATIC POINTER_LIST_T	*s_pstRamDisk_FilePool;

// File Instances
STATIC POINTER_LIST_T	*s_pstRamDisk_FileInstPool;
STATIC POINTER_LIST_T	*s_pstRamDisk_FileInstFreePool;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

STATIC otlRamdist_File_t*	otl_ramdisk_AllocFileObject (HUINT8 *szFileName);
STATIC void					otl_ramdisk_FreeFileObject (otlRamdist_File_t *pstFile);
STATIC void					otl_ramdisk_FreeAllFileObject (void);
STATIC HBOOL				otl_ramdisk_FilterFileName (void *pvItem, void *pvFilter);
STATIC otlRamdist_File_t*	otl_ramdisk_FindFileObjectByName (HCHAR *szFileName);
STATIC HERROR				otl_ramdisk_CountAllFiles (HUINT32 *pulFileNum);
STATIC HERROR				otl_ramdisk_GetFileName (HUINT32 ulIndex, HUINT8 *szFileName);
STATIC HERROR				otl_ramdisk_GetFileNameList (HUINT32 *pulFileNum, HUINT8 ***ppszFileNameArray);
STATIC HERROR				otl_ramdisk_FreeFileNameList (HUINT32 ulFileNum, HUINT8 **pszFileNameArray);

STATIC otlRamdist_Inst_t*	otl_ramdisk_AllocFileInstance (otlRamdist_File_t *pstFile);
STATIC void					otl_ramdisk_FreeFileInstance (otlRamdist_Inst_t *pstInst);
STATIC void					otl_ramdisk_FreeFileInstanceByFileObject (otlRamdist_File_t *pstFile);
STATIC void					otl_ramdisk_FreeAllFileInstance (void);
STATIC HBOOL				otl_ramdisk_IsFileInstanceAvailable (otlRamdist_Inst_t *pstInst);

STATIC HERROR				otl_ramdisk_OpenFile (HUINT8 *szFileName, otlRamdist_FileOpenMode_t eFileMode, otlRamdist_Inst_t **ppstFileInst);



/*******************************************************************/
/********************      Static Functions       *************************/
/*******************************************************************/


#define _____FILE_OBJECT_____

STATIC otlRamdist_File_t *otl_ramdisk_AllocFileObject (HUINT8 *szFileName)
{
	otlRamdist_File_t	*pstFile;

	pstFile = OxMW_Malloc (sizeof(otlRamdist_File_t));
	if (pstFile != NULL)
	{
		HxSTD_MemSet (pstFile, 0, sizeof(otlRamdist_File_t));
		pstFile->bFileAvailable = TRUE;
		HLIB_STD_StrUtf8NCpy (pstFile->szFileName, szFileName, RAM_FILE_NAME_LEN);

		s_pstRamDisk_FilePool = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstRamDisk_FilePool, (void *)pstFile);
		UTIL_LINKEDLIST_CircularizePointerList (s_pstRamDisk_FilePool);
	}

	return pstFile;
}

STATIC void otl_ramdisk_FreeFileObject (otlRamdist_File_t *pstFile)
{
	otlRamdist_Data_t	*pstBuffer, *pstNextBuffer;
	POINTER_LIST_T		*pstItem;

	if (pstFile != NULL)
	{
		pstBuffer = pstFile->pstBuffer;
		while (pstBuffer != NULL)
		{
			pstNextBuffer = pstBuffer->pstNext;
			OxMW_Free (pstBuffer);
			pstBuffer = pstNextBuffer;
		}

		pstItem = UTIL_LINKEDLIST_IsListItemInPointerList (s_pstRamDisk_FilePool, (void *)pstFile, NULL, NULL, NULL);
		if (pstItem != NULL)
		{
			s_pstRamDisk_FilePool = UTIL_LINKEDLIST_DeleteListItemFromPointerList (s_pstRamDisk_FilePool, pstItem, NULL);
		}

		OxMW_Free (pstFile);
	}
}

STATIC void otl_ramdisk_FreeAllFileObject (void)
{
	otlRamdist_File_t	*pstFile;
	otlRamdist_Data_t	*pstBuffer, *pstNextBuffer;

	while (s_pstRamDisk_FilePool != NULL)
	{
		pstFile = (otlRamdist_File_t *)UTIL_LINKEDLIST_GetContents(s_pstRamDisk_FilePool);
		if (pstFile != NULL)
		{
			pstBuffer = pstFile->pstBuffer;
			while (pstBuffer != NULL)
			{
				pstNextBuffer = pstBuffer->pstNext;
				OxMW_Free (pstBuffer);
				pstBuffer = pstNextBuffer;
			}

			OxMW_Free (pstFile);
		}

		s_pstRamDisk_FilePool = UTIL_LINKEDLIST_DeleteListItemFromPointerList (s_pstRamDisk_FilePool, s_pstRamDisk_FilePool, NULL);
	}
}


STATIC HBOOL otl_ramdisk_FilterFileName (void *pvItem, void *pvFilter)
{
	HCHAR		*szFileName;
	otlRamdist_File_t	*pstFile;

	szFileName = (HCHAR *)pvFilter;
	pstFile = (otlRamdist_File_t *)pvItem;

	if (szFileName != NULL && pstFile != NULL)
	{
		if (MWC_UTIL_DvbStrcmp (szFileName, pstFile->szFileName) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC otlRamdist_File_t *otl_ramdisk_FindFileObjectByName (HCHAR *szFileName)
{
	POINTER_LIST_T	*pstItem;
	otlRamdist_File_t		*pstFile;

	pstFile = NULL;

	if (szFileName != NULL && szFileName[0] != '\0')
	{
		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstRamDisk_FilePool, 0, otl_ramdisk_FilterFileName, (void *)szFileName);
		if (pstItem != NULL)
		{
			pstFile = (otlRamdist_File_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		}
	}

	return pstFile;
}

STATIC HERROR otl_ramdisk_CountAllFiles (HUINT32 *pulFileNum)
{
	if (pulFileNum == NULL)				{ return ERR_FAIL; }

	*pulFileNum = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstRamDisk_FilePool, NULL, NULL);

	return (*pulFileNum > 0) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR otl_ramdisk_GetFileName (HUINT32 ulIndex, HUINT8 *szFileName)
{
	POINTER_LIST_T	*pstItem;
	otlRamdist_File_t		*pstFile;

	if (szFileName == NULL)				{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstRamDisk_FilePool, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstFile = (otlRamdist_File_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstFile != NULL)
		{
			MWC_UTIL_DvbStrcpy (szFileName, pstFile->szFileName);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR otl_ramdisk_GetFileNameList (HUINT32 *pulFileNum, HUINT8 ***ppszFileNameArray)
{
	HUINT32				  ulFileIdx, ulFileNum;
	HUINT32				  ulFileNameLen;
	HUINT8				**pszFileNameArray;
	POINTER_LIST_T		 *pstItem;
	otlRamdist_File_t	 *pstFile;
	HERROR				  hErr;

	if (pulFileNum == NULL || ppszFileNameArray == NULL)
	{
		return ERR_FAIL;
	}

	ulFileNum = 0;
	pszFileNameArray = NULL;

	hErr = otl_ramdisk_CountAllFiles (&ulFileNum);
	if (hErr != ERR_OK || ulFileNum == 0)
	{
		return ERR_FAIL;
	}

	pszFileNameArray = (HUINT8 **)OxMW_Malloc (sizeof(HUINT8 *) * ulFileNum);
	if (pszFileNameArray == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (pszFileNameArray, 0, sizeof(HUINT8 *) * ulFileNum);

	pstItem = s_pstRamDisk_FilePool;
	for (ulFileIdx = 0; ulFileIdx < ulFileNum; ulFileIdx++)
	{
		pstFile = (otlRamdist_File_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstFile != NULL)
		{
			ulFileNameLen = MWC_UTIL_DvbStrlen (pstFile->szFileName);
			pszFileNameArray[ulFileIdx] = (HUINT8 *)OxMW_Malloc (ulFileNameLen + 3);
			if (pszFileNameArray[ulFileIdx] == NULL)
			{
				goto err_exit;
			}

			if (ulFileNameLen > 0)
			{
				MWC_UTIL_DvbStrcpy (pszFileNameArray[ulFileIdx], pstFile->szFileName);
			}

			pszFileNameArray[ulFileIdx][ulFileNameLen] = '\0';
			pszFileNameArray[ulFileIdx][ulFileNameLen + 1] = '\0';
			pszFileNameArray[ulFileIdx][ulFileNameLen + 2] = '\0';
		}

		pstItem = UTIL_LINKEDLIST_GetNextPointer (pstItem);
	}

	*pulFileNum = ulFileNum;
	*ppszFileNameArray = pszFileNameArray;

	return ERR_OK;

err_exit:
	otl_ramdisk_FreeFileNameList (ulFileNum, pszFileNameArray);
	return ERR_FAIL;
}

STATIC HERROR otl_ramdisk_FreeFileNameList (HUINT32 ulFileNum, HUINT8 **pszFileNameArray)
{
	HUINT32			 ulFileIdx;

	if (pszFileNameArray != NULL)
	{
		for (ulFileIdx = 0; ulFileIdx < ulFileNum; ulFileIdx++)
		{
			if (pszFileNameArray[ulFileIdx] != NULL)
			{
				OxMW_Free (pszFileNameArray[ulFileIdx]);
			}
		}

		OxMW_Free (pszFileNameArray);
	}

	return ERR_OK;
}



#define _____FILE_INSTANCES_____

STATIC otlRamdist_Inst_t *otl_ramdisk_AllocFileInstance (otlRamdist_File_t *pstFile)
{
	otlRamdist_Inst_t	*pstInst;

	if (pstFile == NULL)
	{
		return NULL;
	}

	if (s_pstRamDisk_FileInstFreePool == NULL)
	{
		pstInst = OxMW_Malloc (sizeof(otlRamdist_Inst_t));
	}
	else
	{
		pstInst = (otlRamdist_Inst_t *)UTIL_LINKEDLIST_GetContents (s_pstRamDisk_FileInstFreePool);
		s_pstRamDisk_FileInstFreePool = UTIL_LINKEDLIST_DeleteListItemFromPointerList (s_pstRamDisk_FileInstFreePool, s_pstRamDisk_FileInstFreePool, NULL);
	}

	if (pstInst != NULL)
	{
		HxSTD_MemSet (pstInst, 0, sizeof(otlRamdist_Inst_t));
		pstInst->hRamFileHandle	= (Handle_t)pstInst;
		pstInst->bAvailable		= TRUE;
		pstInst->pstFile		= pstFile;

		s_pstRamDisk_FileInstPool = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstRamDisk_FileInstPool, (void *)pstInst);
		UTIL_LINKEDLIST_CircularizePointerList (s_pstRamDisk_FileInstPool);
	}

	return pstInst;
}

STATIC void otl_ramdisk_FreeFileInstance (otlRamdist_Inst_t *pstInst)
{
	POINTER_LIST_T		*pstItem;

	if (pstInst != NULL)
	{
		HxSTD_MemSet (pstInst, 0, sizeof(otlRamdist_Inst_t));
		pstInst->hRamFileHandle	= (Handle_t)pstInst;
		pstInst->bAvailable = FALSE;

		pstItem = UTIL_LINKEDLIST_IsListItemInPointerList (s_pstRamDisk_FileInstPool, (void *)pstInst, NULL, NULL, NULL);
		if (pstItem != NULL)
		{
			s_pstRamDisk_FileInstPool = UTIL_LINKEDLIST_DeleteListItemFromPointerList (s_pstRamDisk_FileInstPool, pstItem, NULL);
		}

		s_pstRamDisk_FileInstFreePool = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstRamDisk_FileInstFreePool, (void *)pstInst);
		UTIL_LINKEDLIST_CircularizePointerList (s_pstRamDisk_FileInstFreePool);
	}
}

STATIC void otl_ramdisk_FreeFileInstanceByFileObject (otlRamdist_File_t *pstFile)
{
	POINTER_LIST_T		*pstItem, *pstNextItem;
	otlRamdist_Inst_t		*pstInst;

	if (pstFile != NULL)
	{
		pstItem = s_pstRamDisk_FileInstPool;

		while (pstItem != NULL)
		{
			pstNextItem = UTIL_LINKEDLIST_GetNextPointer (pstItem);

			// 같은 File Object를 가리키는가?
			pstInst = UTIL_LINKEDLIST_GetContents(pstItem);
			if (pstInst != NULL && pstInst->pstFile == pstFile)
			{
				otl_ramdisk_FreeFileInstance (pstInst);
			}

			pstItem = pstNextItem;
			if (pstItem == s_pstRamDisk_FileInstPool)
			{
				break;
			}
		}
	}
}

STATIC void otl_ramdisk_FreeAllFileInstance (void)
{
	POINTER_LIST_T		*pstItem, *pstNextItem;
	otlRamdist_Inst_t		*pstInst;

	pstItem = s_pstRamDisk_FileInstPool;

	while (pstItem != NULL)
	{
		pstNextItem = UTIL_LINKEDLIST_GetNextPointer (pstItem);

		// 같은 File Object를 가리키는가?
		pstInst = UTIL_LINKEDLIST_GetContents(pstItem);
		otl_ramdisk_FreeFileInstance (pstInst);

		pstItem = pstNextItem;
		if (pstItem == s_pstRamDisk_FileInstPool)
		{
			break;
		}
	}
}

STATIC HBOOL otl_ramdisk_IsFileInstanceAvailable (otlRamdist_Inst_t *pstInst)
{
	if (pstInst != NULL)
	{
		if ((pstInst->hRamFileHandle == (Handle_t)pstInst) && (pstInst->bAvailable == TRUE))
		{
			return TRUE;
		}
	}

	return FALSE;
}



#define _____FILE_ACCESS_____

STATIC HERROR otl_ramdisk_OpenFile (HUINT8 *szFileName, otlRamdist_FileOpenMode_t eFileMode, otlRamdist_Inst_t **ppstFileInst)
{
	HBOOL				 bIsNewFile;
	otlRamdist_File_t			*pstFile;
	otlRamdist_Inst_t		*pstInst;

	bIsNewFile = FALSE;
	pstInst = NULL;

	// 1. Find whether the same file name exists:
	pstFile = otl_ramdisk_FindFileObjectByName (szFileName);
	if (pstFile != NULL)
	{
		switch (eFileMode)
		{
		case eFileMode_ReadOnly:
			// Read mode : Use the file
			break;

		case eFileMode_Write:
			// Write mode : Remove the previous one and reallocate:
			bIsNewFile = TRUE;
			otl_ramdisk_FreeFileObject (pstFile);
			pstFile = otl_ramdisk_AllocFileObject (szFileName);
			break;

		case eFileMode_Append:
			// Use the existed file
			pstFile = otl_ramdisk_AllocFileObject (szFileName);
			break;
		}

	}
	else
	// Same File not found:
	{
		switch (eFileMode)
		{
		case eFileMode_ReadOnly:
			// Read mode : Fail because no file exists:
			goto err_exit;

		case eFileMode_Write:
		case eFileMode_Append:
			// Make the new file :
			bIsNewFile = TRUE;
			pstFile = otl_ramdisk_AllocFileObject (szFileName);
			break;
		}
	}

	if (pstFile == NULL)
	{
		goto err_exit;
	}

	// Make the file instance
	pstInst = otl_ramdisk_AllocFileInstance (pstFile);
	if (pstInst == NULL)
	{
		goto err_exit;
	}

	pstInst->eFileMode = eFileMode;

	switch (eFileMode)
	{
	case eFileMode_ReadOnly:
		pstInst->ulPosition = 0;
		break;

	case eFileMode_Write:
		pstInst->ulPosition = 0;
		break;

	case eFileMode_Append:
		pstInst->ulPosition = pstFile->ulDataSize;
		break;

	default:
		goto err_exit;
	}

	*ppstFileInst = pstInst;

	return ERR_OK;

err_exit:
	if (pstInst != NULL)
	{
		otl_ramdisk_FreeFileInstance (pstInst);
	}

	if (bIsNewFile == TRUE && pstFile != NULL)
	{
		otl_ramdisk_FreeFileObject (pstFile);
	}

	return ERR_FAIL;
}


#define _____EXTERNAL_FUNCTIONS_____

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR OTL_RAMDISK_Init (void)
{
	HERROR		 hErr;

	hErr = VK_SEM_Create (&s_ulRamDisk_FileSemId, "otlRamDiskSem", VK_SUSPENDTYPE_PRIORITY);
	if(hErr != VK_OK)			HxLOG_Critical("Critical Eror!!!\n");

	hErr = VK_SEM_Create (&s_ulRamDisk_FileInstSemId, "otlRamDiskInstSem", VK_SUSPENDTYPE_PRIORITY);
	if(hErr != VK_OK)			HxLOG_Critical("Critical Eror!!!\n");

	s_pstRamDisk_FilePool = NULL;

	s_pstRamDisk_FileInstPool = NULL;
	s_pstRamDisk_FileInstFreePool = NULL;

	return ERR_OK;
}

HERROR OTL_RAMDISK_OpenFile (HUINT8 *szFileName, HUINT8 *szMode, Handle_t *phRFileHandle)
{
	otlRamdist_FileOpenMode_t	 eFileMode;
	otlRamdist_Inst_t			*pstFileInst;
	HERROR						 hErr;

	if (szFileName == NULL || phRFileHandle == NULL)
	{
		return ERR_FAIL;
	}

	if (szFileName[0] == '\0')
	{
		return ERR_FAIL;
	}

	eFileMode = eFileMode_ReadOnly;
	if (szMode != NULL)
	{
		if (szMode[0] == 'r' || szMode[0] == 'R')
		{
			// Read only mode
			eFileMode = eFileMode_ReadOnly;
		}
		else if (szMode[0] == 'w' || szMode[0] == 'W')
		{
			// Make new file mode
			eFileMode = eFileMode_Write;
		}
		else if (szMode[0] == 'a' || szMode[0] == 'A')
		{
			// Append file mode
			eFileMode = eFileMode_Append;
		}
	}

	ENTER_CRITICAL_RAMINST;
	hErr = otl_ramdisk_OpenFile (szFileName, eFileMode, &pstFileInst);
	LEAVE_CRITICAL_RAMINST;
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	*phRFileHandle = (Handle_t)pstFileInst;

	return ERR_OK;
}

HERROR OTL_RAMDISK_CloseFile (Handle_t hRFileHandle)
{
	otlRamdist_Inst_t		*pstFileInst;

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		return ERR_FAIL;
	}

	ENTER_CRITICAL_RAMINST;
	otl_ramdisk_FreeFileInstance (pstFileInst);
	LEAVE_CRITICAL_RAMINST;

	return ERR_OK;
}

HERROR OTL_RAMDISK_GetFileSize (Handle_t hRFileHandle, HUINT32 *pulFileSize)
{
	otlRamdist_Inst_t	*pstFileInst;
	otlRamdist_File_t	*pstFile;

	if (pulFileSize == NULL)
	{
		return ERR_FAIL;
	}

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		return ERR_FAIL;
	}

	pstFile = pstFileInst->pstFile;
	if (pstFile == NULL)
	{
		return ERR_FAIL;
	}

	*pulFileSize = pstFile->ulDataSize;

	return ERR_OK;
}

HERROR OTL_RAMDISK_SeekFile (Handle_t hRFileHandle, HINT32 nOffset, OtlRamDisk_RamfileAccessOrigin_e eOrigin)
{
	HINT32				 nRealOffset;
	otlRamdist_Inst_t	*pstFileInst;
	otlRamdist_File_t	*pstFile;

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		return ERR_FAIL;
	}

	pstFile = pstFileInst->pstFile;
	if (pstFile == NULL)
	{
		return ERR_FAIL;
	}

	switch (eOrigin)
	{
	case eFileOrigin_Set:
		nRealOffset = nOffset;
		break;

	case eFileOrigin_End:
		nRealOffset = (HINT32)pstFile->ulDataSize - nOffset;
		break;

	case eFileOrigin_Cursor:
	default:
		nRealOffset = (HINT32)pstFileInst->ulPosition + nOffset;
		break;
	}

	if (nRealOffset < 0)
	{
		nRealOffset = 0;
	}
	else if (nRealOffset > (HINT32)pstFile->ulDataSize)
	{
		nRealOffset = (HINT32)pstFile->ulDataSize;
	}

	pstFileInst->ulPosition = (HUINT32)nRealOffset;

	return ERR_OK;
}

HERROR OTL_RAMDISK_TellFile (Handle_t hRFileHandle, HINT32 *pnOffset)
{
	otlRamdist_Inst_t		*pstFileInst;

	if (pnOffset == NULL)
	{
		return ERR_FAIL;
	}

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		return ERR_FAIL;
	}

	*pnOffset = (HINT32)pstFileInst->ulPosition;

	return ERR_OK;
}

HERROR OTL_RAMDISK_ReadFile (Handle_t hRFileHandle, HUINT8 *pucBuffer, HUINT32 ulSize, HUINT32 *pulRealReadSize)
{
	HINT32				 nRemainSize;
	HUINT32				 ulRealReadSize;
	HUINT32				 ulStartOffset, ulStartBlkIndex, ulStartBlkOffset;
	HUINT32				 ulEndOffset, ulEndBlkIndex, ulEndBlkOffset;
	HUINT32				 ulIndex;
	HUINT8				*pucWrite;
	otlRamdist_Inst_t	*pstFileInst;
	otlRamdist_File_t	*pstFile;
	otlRamdist_Data_t	*pstDataList;
	HERROR				 hResult;

	if (pucBuffer == NULL || ulSize == 0)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	pstFile = pstFileInst->pstFile;
	if (pstFile == NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	// File Position부터 ulSize 만큼 읽을 수 있는가를 체크한다.
	nRemainSize = (HINT32)pstFile->ulDataSize - (HINT32)pstFileInst->ulPosition;
	nRemainSize = (nRemainSize > 0) ? nRemainSize : 0;
	ulRealReadSize = ((HUINT32)nRemainSize > ulSize) ? ulSize : (HUINT32)nRemainSize;

	if (ulRealReadSize == 0)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d) : ulRealReadSize is 0\n", __LINE__);
		return ERR_FAIL;
	}

	ENTER_CRITICAL_RAMFILE;

	hResult = ERR_FAIL;

	ulStartOffset		= pstFileInst->ulPosition;
	ulStartBlkIndex		= ulStartOffset >> RAM_DISK_CLUSTER_BIT;
	ulStartBlkOffset	= ulStartOffset & (RAM_DISK_CLUSTER_SIZE - 1);

	ulEndOffset			= ulStartOffset + ulRealReadSize - 1;
	ulEndBlkIndex		= ulEndOffset >> RAM_DISK_CLUSTER_BIT;
	ulEndBlkOffset		= ulEndOffset & (RAM_DISK_CLUSTER_SIZE - 1);

	// 1. Get to the start offset block
	pstDataList = pstFile->pstBuffer;

	for (ulIndex = 0; ulIndex < ulStartBlkIndex; ulIndex++)
	{
		if (pstDataList == NULL)
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d) - ulIndex = %d, ulStartBlkIndex = %d\n", __LINE__, ulIndex, ulStartBlkIndex);
			goto err_exit;
		}

		pstDataList = pstDataList->pstNext;
	}

	if (pstDataList == NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
		goto err_exit;
	}

	// 2. Start Block과 End Block이 같은가?
	if (ulStartBlkIndex == ulEndBlkIndex)
	{
		// Read area : ulStartBlkOffset ~ ulEndBlkOffset
		HxSTD_MemCopy (pucBuffer, &(pstDataList->aucData[ulStartBlkOffset]), (ulEndBlkOffset - ulStartBlkOffset + 1));
	}
	// 블럭이 다르면 차례차례 copy 하도록 한다.
	else
	{
		pucWrite = pucBuffer;

		// 1. Start Block을 다 읽는다.
		HxSTD_MemCopy (pucWrite, &(pstDataList->aucData[ulStartBlkOffset]), (RAM_DISK_CLUSTER_SIZE - ulStartBlkOffset));
		pucWrite += (RAM_DISK_CLUSTER_SIZE - ulStartBlkOffset);
		pstDataList = pstDataList->pstNext;


		// 2. Start Block과 End Block 사이의 모든 Block을 다 Copy한다.
		for (ulIndex = ulStartBlkIndex + 1; ulIndex < ulEndBlkIndex; ulIndex++)
		{
			if (pstDataList == NULL)
			{
				HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d) - ulIndex = %d, ulEndBlkIndex = %d\n", __LINE__, ulIndex, ulEndBlkIndex);
				goto err_exit;
			}

			HxSTD_MemCopy (pucWrite, pstDataList->aucData, RAM_DISK_CLUSTER_SIZE);
			pucWrite += RAM_DISK_CLUSTER_SIZE;
			pstDataList = pstDataList->pstNext;
		}

		// 3. End Block을 Copy 한다.
		if (pstDataList == NULL)
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);
			goto err_exit;
		}

		HxSTD_MemCopy (pucWrite, pstDataList->aucData, ulEndBlkOffset + 1);
	}

	// 3. File Pointer를 옮겨놓아야 한다.
	pstFileInst->ulPosition += ulRealReadSize;
	if (pulRealReadSize != NULL)
	{
		*pulRealReadSize = ulRealReadSize;
	}

	LEAVE_CRITICAL_RAMFILE;
	return ERR_OK;

err_exit:
	LEAVE_CRITICAL_RAMFILE;
	HxLOG_Error("ERROR in OTL_RAMDISK_ReadFile (%d)\n", __LINE__);

	return hResult;
}

HERROR OTL_RAMDISK_WriteFile (Handle_t hRFileHandle, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	HUINT32				 ulStartOffset, ulStartBlkIndex, ulStartBlkOffset;
	HUINT32				 ulEndOffset, ulEndBlkIndex, ulEndBlkOffset;
	HUINT32				 ulIndex;
	HUINT8				*pucWrite;
	otlRamdist_Inst_t	*pstFileInst;
	otlRamdist_File_t	*pstFile;
	otlRamdist_Data_t	*pstDataList, *pstDataBlock;
	HERROR				 hResult;

	if (pucBuffer == NULL || ulSize == 0)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	if (hRFileHandle == (Handle_t)NULL || hRFileHandle == HANDLE_NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	pstFileInst = (otlRamdist_Inst_t *)hRFileHandle;
	if (otl_ramdisk_IsFileInstanceAvailable (pstFileInst) == FALSE)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	pstFile = pstFileInst->pstFile;
	if (pstFile == NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
		return ERR_FAIL;
	}

	ENTER_CRITICAL_RAMFILE;

	hResult = ERR_FAIL;

	// Write를 명령 내렸는데 실제 Buffer Size가 0이면 일단 Block 하나는 연결해 놓고 시작한다.
	if (pstFile->ulBufferSize == 0)
	{
		pstDataBlock = (otlRamdist_Data_t *)OxMW_Malloc (sizeof(otlRamdist_Data_t));
		if (pstDataBlock == NULL)
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
			goto err_exit;
		}

		pstDataBlock->pstNext	= NULL;
		pstFile->pstBuffer		= pstDataBlock;
		pstFile->ulBufferSize 	= RAM_DISK_CLUSTER_SIZE;
	}

	// 실제 File Buffer가 앞으로 쓸 만큼 할당되어 있는지 확인한다.
	// 할당되어 있지 않으면 필요한 Block만큼 더 추가한다.
	if (pstFile->ulBufferSize < (pstFileInst->ulPosition + ulSize))
	{
		ulStartBlkIndex	= (pstFile->ulBufferSize - 1) >> RAM_DISK_CLUSTER_BIT;
		ulEndBlkIndex	= (pstFileInst->ulPosition + ulSize - 1) >> RAM_DISK_CLUSTER_BIT;

		pstDataList = pstFile->pstBuffer;
		if (pstDataList == NULL)		// 여기서 pstDataList가 NULL 일 수 없다 : 위의 Logic 때문에
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
			goto err_exit;
		}

		while (pstDataList->pstNext != NULL)
		{
			pstDataList = pstDataList->pstNext;
		}

		// Write시 필요한 Block 갯수 만큼 추가
		for (ulIndex = ulStartBlkIndex; ulIndex < ulEndBlkIndex; ulIndex++)
		{
			pstDataBlock = (otlRamdist_Data_t *)OxMW_Malloc (sizeof(otlRamdist_Data_t));
			if (pstDataBlock == NULL)
			{
				HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d) - ulIndex = %d, ulEndBlkIndex = %d\n", __LINE__, ulIndex, ulEndBlkIndex);
				goto err_exit;
			}

			pstDataBlock->pstNext	= NULL;
			pstDataList->pstNext	= pstDataBlock;
			pstDataList				= pstDataList->pstNext;
			pstFile->ulBufferSize += RAM_DISK_CLUSTER_SIZE;
		}
	}

	// 시작 위치, 끝 위치 계산
	ulStartOffset		= pstFileInst->ulPosition;
	ulStartBlkIndex		= ulStartOffset >> RAM_DISK_CLUSTER_BIT;
	ulStartBlkOffset	= ulStartOffset & (RAM_DISK_CLUSTER_SIZE - 1);

	ulEndOffset			= ulStartOffset + ulSize - 1;
	ulEndBlkIndex		= ulEndOffset >> RAM_DISK_CLUSTER_BIT;
	ulEndBlkOffset		= ulEndOffset & (RAM_DISK_CLUSTER_SIZE - 1);


	// 1. Get to the start offset block
	pstDataList = pstFile->pstBuffer;

	for (ulIndex = 0; ulIndex < ulStartBlkIndex; ulIndex++)
	{
		if (pstDataList == NULL)
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d) - ulIndex = %d, ulStartBlkIndex = %d\n", __LINE__, ulIndex, ulStartBlkIndex);
			goto err_exit;
		}

		pstDataList = pstDataList->pstNext;
	}

	if (pstDataList == NULL)
	{
		HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
		goto err_exit;
	}

	// 2. Start Block과 End Block이 같은가?
	if (ulStartBlkIndex == ulEndBlkIndex)
	{
		// Write area : ulStartBlkOffset ~ ulEndBlkOffset
		HxSTD_MemCopy (&(pstDataList->aucData[ulStartBlkOffset]), pucBuffer, (ulEndBlkOffset - ulStartBlkOffset + 1));
	}
	// 블럭이 다르면 차례차례 copy 하도록 한다.
	else
	{
		pucWrite = pucBuffer;

		// 1. Start Block을 다 쓴다.
		HxSTD_MemCopy (&(pstDataList->aucData[ulStartBlkOffset]), pucWrite, (RAM_DISK_CLUSTER_SIZE - ulStartBlkOffset));
		pucWrite += (RAM_DISK_CLUSTER_SIZE - ulStartBlkOffset);
		pstDataList = pstDataList->pstNext;


		// 2. Start Block과 End Block 사이의 모든 Block을 다 Copy한다.
		for (ulIndex = ulStartBlkIndex + 1; ulIndex < ulEndBlkIndex; ulIndex++)
		{
			if (pstDataList == NULL)
			{
				HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d) - ulIndex = %d, ulEndBlkIndex = %d\n", __LINE__, ulIndex, ulEndBlkIndex);
				goto err_exit;
			}

			HxSTD_MemCopy (pstDataList->aucData, pucWrite, RAM_DISK_CLUSTER_SIZE);
			pucWrite += RAM_DISK_CLUSTER_SIZE;
			pstDataList = pstDataList->pstNext;
		}

		// 3. End Block을 Copy 한다.
		if (pstDataList == NULL)
		{
			HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);
			goto err_exit;
		}

		HxSTD_MemCopy (pstDataList->aucData, pucWrite, ulEndBlkOffset + 1);
	}

	// 3. File Pointer를 옮겨놓아야 한다.
	pstFileInst->ulPosition += ulSize;
	if (pstFileInst->ulPosition > pstFile->ulDataSize)
	{
		pstFile->ulDataSize = pstFileInst->ulPosition;

	}

	LEAVE_CRITICAL_RAMFILE;
	return ERR_OK;

err_exit:
	LEAVE_CRITICAL_RAMFILE;
	HxLOG_Error("ERROR in OTL_RAMDISK_WriteFile (%d)\n", __LINE__);

	return hResult;
}

HERROR OTL_RAMDISK_DeleteFile (HUINT8 *szFileName)
{
	otlRamdist_File_t			*pstFile;

	if (szFileName == NULL || szFileName[0] == '\0')
	{
		return ERR_FAIL;
	}

	ENTER_CRITICAL_RAMINST;
	ENTER_CRITICAL_RAMFILE;

	pstFile = otl_ramdisk_FindFileObjectByName (szFileName);
	if (pstFile != NULL)
	{
		otl_ramdisk_FreeFileInstanceByFileObject (pstFile);
		otl_ramdisk_FreeFileObject (pstFile);
	}

	LEAVE_CRITICAL_RAMFILE;
	LEAVE_CRITICAL_RAMINST;

	return ERR_OK;
}

HERROR OTL_RAMDISK_DeleteAllFiles (void)
{
	ENTER_CRITICAL_RAMINST;
	ENTER_CRITICAL_RAMFILE;

	otl_ramdisk_FreeAllFileObject();
	otl_ramdisk_FreeAllFileInstance();

	LEAVE_CRITICAL_RAMFILE;
	LEAVE_CRITICAL_RAMINST;

	return ERR_OK;
}

HERROR OTL_RAMDISK_CountAllFiles (HUINT32 *pulFileNum)
{
	HERROR		 hErr;

	ENTER_CRITICAL_RAMFILE;
	hErr = otl_ramdisk_CountAllFiles (pulFileNum);
	LEAVE_CRITICAL_RAMFILE;

	return hErr;
}


HERROR OTL_RAMDISK_GetFileName (HUINT32 ulIndex, HUINT8 *szFileName)
{
	HERROR		 hErr;

	ENTER_CRITICAL_RAMFILE;
	hErr = otl_ramdisk_GetFileName (ulIndex, szFileName);
	LEAVE_CRITICAL_RAMFILE;

	return hErr;
}

HERROR OTL_RAMDISK_GetFileNameList (HUINT32 *pulFileNum, HUINT8 ***ppszFileNameArray)
{
	HERROR		 hErr;

	ENTER_CRITICAL_RAMFILE;
	hErr = otl_ramdisk_GetFileNameList (pulFileNum, ppszFileNameArray);
	LEAVE_CRITICAL_RAMFILE;

	return hErr;
}

HERROR OTL_RAMDISK_FreeFileNameList (HUINT32 ulFileNum, HUINT8 **pszFileNameArray)
{
	return otl_ramdisk_FreeFileNameList (ulFileNum, pszFileNameArray);
}

/* end of file */

