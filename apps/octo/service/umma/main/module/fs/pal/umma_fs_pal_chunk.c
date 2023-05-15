/**
	@file     	pal_fs_chunk.c
	@brief    	Chunk File Emulator

	Description: 		\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include "umma_fs_pal_fs.h"
#include <dirent.h>
#include <sys/stat.h>
#include <umma_int.h>

#define ___INTERNAL_DECLARATION___

#define CHECK_INITIALIZED 			do { if (TRUE != s_bPalFsChunkInitiated) pal_fs_chunk_init(); } while (0)
#define ENTER_FSCHUNK				VK_SEM_Get (s_ullPalFsChunk_SemId)
#define LEAVE_FSCHUNK				VK_SEM_Release (s_ullPalFsChunk_SemId)

#define ___INTERNAL_STRUCTURES___
typedef struct
{
	Handle_t		 hChunk;
	HCHAR			 szPath[PAL_FS_MAX_NAME_LEN];

	HUINT32			 ulChunkSize;
	HUINT32			 ulChunkNum;
	HUINT64			 ullTotalSize;
	HUINT64			 ullCurrPosition;

	// File Open Cache¿ë:
	HINT32			 nOpenedChunkId;
	PAL_FS_UINT32	 ulOpenedChunkFile;

} palfschunk_Inst_t;

#define ___STATIC_VARIABLES___
STATIC HBOOL			 s_bPalFsChunkInitiated = FALSE;
STATIC HULONG			 s_ullPalFsChunk_SemId;
STATIC HxList_t			*s_pstPalFsChunkInstList = NULL;			// palfschunk_Inst_t

#define ___INTERNAL_FUNCTIONS___
STATIC HERROR pal_fs_chunk_init (void)
{
	HINT32				 nRet;

	nRet = VK_SEM_Create ((HULONG *)&s_ullPalFsChunk_SemId, "ChunkSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != nRet)
	{
		HxLOG_Error ("VK_SEM_Create failed:\n");
		return ERR_FAIL;
	}

	s_bPalFsChunkInitiated = TRUE;
	return ERR_OK;
}

STATIC palfschunk_Inst_t *pal_fs_chunk_GetInstByChunkHandle (Handle_t hChunk)
{
	HxList_t			*pstListItem;

	if (0 != hChunk)
	{
		for (pstListItem = s_pstPalFsChunkInstList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			palfschunk_Inst_t		*pstInst = (palfschunk_Inst_t *)HLIB_LIST_Data (pstListItem);

			if (hChunk == pstInst->hChunk)
			{
				return pstInst;
			}
		}
	}

	return NULL;
}

STATIC Handle_t pal_fs_chunk_AllocateUniqueChunkHandle (void)
{
	STATIC Handle_t		 s_hLastChunkHandle = 0x0001;
	Handle_t			 hChunk = s_hLastChunkHandle + 1;

	if (hChunk >= 0xFFFE)					{ hChunk = 0x0001; }

	while (hChunk != s_hLastChunkHandle)
	{
		palfschunk_Inst_t		*pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);

		if (NULL == pstInst)
		{
			s_hLastChunkHandle = hChunk;
			return hChunk;

		}

		hChunk ++;
		if (hChunk >= 0xFFFE)					{ hChunk = 0x0001; }
	}

	return HANDLE_NULL;
}

STATIC HBOOL pal_fs_chunk_cbCheckChunk (HxDIR_t *pstSelf, HCHAR *szPath, HCHAR *szName, HBOOL bIsDir, void *pvUserData)
{
	HBOOL				 bResult = FALSE;
	palfschunk_Inst_t	*pstInst = (palfschunk_Inst_t *)pvUserData;

	if ((TRUE != bIsDir) && (NULL != pstInst))
	{
		PAL_FS_UINT32	 ulFileDes = 0;
		PAL_FS_INT64	 nFileSize = 0;
		HINT32			 nRet;
		HCHAR			 szFullName[PAL_FS_MAX_NAME_LEN];

		HxSTD_PrintToStrN (szFullName, PAL_FS_MAX_NAME_LEN, "%s/%s", szPath, szName);

		nRet = PAL_FS_Secure_Open ((const char *)szFullName, PAL_FS_MODE_READ, &ulFileDes);
		if ((ERR_OK == nRet) && (0 != ulFileDes))
		{
			PAL_FS_Secure_Seek (ulFileDes, 0L, PAL_FS_POS_END);
			PAL_FS_Secure_Tell (ulFileDes, &nFileSize);

			if (nFileSize > 16)
			{
				if (pstInst->ulChunkSize < (HUINT32)nFileSize)
				{
					pstInst->ulChunkSize = (HUINT32)nFileSize;
				}

				pstInst->ullTotalSize += (HUINT64)nFileSize;
				pstInst->ulChunkNum ++;
			}

			PAL_FS_Secure_Close (ulFileDes);
		}

		bResult = (pstInst->ullTotalSize > 0) ? TRUE : FALSE;
	}

	return bResult;
}

STATIC HBOOL pal_fs_chunk_cbScanChunks (HxDIR_t *pstSelf, HCHAR *szPath, HCHAR *szName, HBOOL bIsDir, void *pvUserData)
{
	palfschunk_Inst_t	*pstInst = (palfschunk_Inst_t *)pvUserData;

	if ((TRUE != bIsDir) && (NULL != pstInst))
	{
		PAL_FS_UINT32	 ulFileDes = 0;
		PAL_FS_INT64	 nFileSize = 0;
		HINT32			 nRet;
		HCHAR			 szFullName[PAL_FS_MAX_NAME_LEN];

		HxSTD_PrintToStrN (szFullName, PAL_FS_MAX_NAME_LEN, "%s/%s", szPath, szName);

		nRet = PAL_FS_Secure_Open ((const char *)szFullName, PAL_FS_MODE_READ, &ulFileDes);
		if ((ERR_OK == nRet) && (0 != ulFileDes))
		{
			PAL_FS_Secure_Seek (ulFileDes, 0L, PAL_FS_POS_END);
			PAL_FS_Secure_Tell (ulFileDes, &nFileSize);

			if (nFileSize > 16)
			{
				if (pstInst->ulChunkSize < (HUINT32)nFileSize)
				{
					pstInst->ulChunkSize = (HUINT32)nFileSize;
				}

				pstInst->ullTotalSize += (HUINT64)nFileSize;
				pstInst->ulChunkNum ++;
			}

			PAL_FS_Secure_Close (ulFileDes);
		}
	}

	return FALSE;
}

STATIC HERROR pal_fs_readChunkBlocks (palfschunk_Inst_t *pstInst, HUINT32 ulChunkId, HUINT32 ulPos, HUINT32 ulReadSize, HUINT8 *pucBuf)
{
	PAL_FS_UINT32		 ulSizeP;
	HINT32				 nRet;

	if ((pstInst->nOpenedChunkId >= 0) && (pstInst->nOpenedChunkId != ulChunkId))
	{
		PAL_FS_Secure_Close (pstInst->ulOpenedChunkFile);
		pstInst->nOpenedChunkId = -1;
		pstInst->ulOpenedChunkFile = 0;
	}

	if (pstInst->nOpenedChunkId < 0)
	{
		PAL_FS_UINT32			 ulFileDes = 0;
		HCHAR					*szFileName = NULL;

		szFileName = UMMA_Malloc (PAL_FS_MAX_NAME_LEN);
		if (NULL == szFileName)
		{
			HxLOG_Error ("UMMA_Malloc failed\n");
			return ERR_FAIL;
		}

		HxSTD_PrintToStrN (szFileName, PAL_FS_MAX_NAME_LEN, "%s/chunk.%d", pstInst->szPath, ulChunkId + 1);
		nRet = PAL_FS_Secure_Open (szFileName, PAL_FS_MODE_READ, &ulFileDes);
		UMMA_Free (szFileName);

		if ((ERR_OK != nRet) || (0 == ulFileDes))
		{
			HxLOG_Error ("Error in PAL_FS_Open \n");
			return ERR_FAIL;
		}

		pstInst->nOpenedChunkId = (HINT32)ulChunkId;
		pstInst->ulOpenedChunkFile = ulFileDes;
	}

	PAL_FS_Secure_Seek (pstInst->ulOpenedChunkFile, ulPos, PAL_FS_POS_START);
	PAL_FS_Secure_Read (pstInst->ulOpenedChunkFile, (PAL_FS_UINT8 *)pucBuf, ulReadSize, &ulSizeP);

	return ERR_OK;
}


#define _____API_PROTOTYPE_INTERNAL_____
STATIC HBOOL pal_fs_chunk_IsChunkedFile (HCHAR *szPath)
{
	HCHAR				*szAllFileName = "*.*";
	palfschunk_Inst_t	*pstInst = NULL;


	pstInst = (palfschunk_Inst_t *)UMMA_Calloc (sizeof(palfschunk_Inst_t));
	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance allocation failed:\n");
		return FALSE;
	}

	HLIB_DIR_Find (szPath, szAllFileName, "", pal_fs_chunk_cbCheckChunk, (void *)pstInst);

	if ((0 == pstInst->ulChunkNum) || (0 == pstInst->ulChunkSize) || (0L == pstInst->ullTotalSize))
	{
		HxLOG_Error ("No proper chunked file inside:\n");
		UMMA_Free (pstInst);
		return FALSE;
	}

	UMMA_Free (pstInst);
	return TRUE;
}

STATIC Handle_t pal_fs_chunk_Open (HCHAR *szPath, HCHAR *szMode)
{
	Handle_t			 hChunk = pal_fs_chunk_AllocateUniqueChunkHandle();
	HCHAR				*szAllFileName = "*.*";
	palfschunk_Inst_t	*pstInst = NULL;

	if (HANDLE_NULL == hChunk)
	{
		HxLOG_Error ("pal_fs_chunk_AllocateUniqueChunkHandle failed:\n");
		goto ERROR;
	}

	pstInst = (palfschunk_Inst_t *)UMMA_Calloc (sizeof(palfschunk_Inst_t));
	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance allocation failed:\n");
		goto ERROR;
	}

	pstInst->hChunk			= hChunk;
	pstInst->ulChunkNum		= 0;
	pstInst->nOpenedChunkId	= -1;
	HLIB_STD_StrNCpySafe (pstInst->szPath, szPath, PAL_FS_MAX_NAME_LEN);

	HLIB_DIR_Find (szPath, szAllFileName, "", pal_fs_chunk_cbScanChunks, (void *)pstInst);

	if ((0 == pstInst->ulChunkNum) || (0 == pstInst->ulChunkSize) || (0L == pstInst->ullTotalSize))
	{
		HxLOG_Error ("No proper chunked file inside:\n");
		goto ERROR;
	}

	s_pstPalFsChunkInstList = HLIB_LIST_Append (s_pstPalFsChunkInstList, (void *)pstInst);

	return hChunk;

ERROR:
	if (NULL != pstInst)			{ UMMA_Free (pstInst); }

	return HANDLE_NULL;
}

STATIC HERROR pal_fs_chunk_Close (Handle_t hChunkFile)
{
	palfschunk_Inst_t	*pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunkFile);

	if (NULL == pstInst)			{ return ERR_FAIL; }

	s_pstPalFsChunkInstList = HLIB_LIST_Remove (s_pstPalFsChunkInstList, (void *)pstInst);

	if (pstInst->nOpenedChunkId >= 0)
	{
		PAL_FS_Secure_Close (pstInst->ulOpenedChunkFile);

		pstInst->nOpenedChunkId = -1;
		pstInst->ulOpenedChunkFile = 0;
	}

	UMMA_Free (pstInst);

	return ERR_OK;
}

STATIC HERROR pal_fs_chunk_Read (palfschunk_Inst_t *pstInst, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead)
{
	HUINT32			 ulReadSize;
	HUINT32			 ulStartChunkId, ulStartChunkPos;
	HUINT32			 ulEndChunkId, ulEndChunkPos;
	HINT64			 nEndPos;

	if ((pstInst->ullCurrPosition + ulSize) >= pstInst->ullTotalSize)
	{
		nEndPos = (HINT64)pstInst->ullTotalSize - 1;
	}
	else
	{
		nEndPos = (HINT64)(pstInst->ullCurrPosition + ulSize - 1);
	}

	if ((HINT64)pstInst->ullCurrPosition > nEndPos)
	{
		HxLOG_Error ("Current Position is at the end position or over\n");
		return ERR_FAIL;
	}

	ulStartChunkId	= (HUINT32)(pstInst->ullCurrPosition / (HUINT64)pstInst->ulChunkSize);
	ulStartChunkPos	= (HUINT32)(pstInst->ullCurrPosition % (HUINT64)pstInst->ulChunkSize);
	ulEndChunkId	= (HUINT32)(nEndPos / (HINT64)pstInst->ulChunkSize);
	ulEndChunkPos	= (HUINT32)(nEndPos % (HINT64)pstInst->ulChunkSize);

	if (ulStartChunkId == ulEndChunkId)
	{
		ulReadSize = (HUINT32)((HUINT64)nEndPos - pstInst->ullCurrPosition + 1);
		pal_fs_readChunkBlocks (pstInst, ulStartChunkId, ulStartChunkPos, ulReadSize, pucBuf);
	}
	else
	{
		HUINT32					 ulChunkId;

		// First Chunk:
		ulReadSize = (pstInst->ulChunkSize - ulStartChunkPos);
		pal_fs_readChunkBlocks (pstInst, ulStartChunkId, ulStartChunkPos, ulReadSize, pucBuf);
		pucBuf += ulReadSize;

		// Middle Chunks:
		for (ulChunkId = ulStartChunkId + 1; ulChunkId < ulEndChunkId; ulChunkId++)
		{
			pal_fs_readChunkBlocks (pstInst, ulChunkId, 0, pstInst->ulChunkSize, pucBuf);
			pucBuf += pstInst->ulChunkSize;
			ulReadSize += pstInst->ulChunkSize;
		}

		// Last Chunk:
		pal_fs_readChunkBlocks (pstInst, ulEndChunkId, 0, ulEndChunkPos + 1, pucBuf);
		ulReadSize += (ulEndChunkPos + 1);
	}

	pstInst->ullCurrPosition += (HUINT64)ulReadSize;
	if (NULL != pulSizeRead)				{ *pulSizeRead = ulReadSize; }

	return ERR_OK;
}

#define ___PAL_API_FUNCTIONS___
HBOOL PAL_FS_CHUNK_IsChunkedFile (HCHAR *szPath)
{
	CHECK_INITIALIZED;

	if (HLIB_DIR_IsDirPath(szPath) == FALSE)
		return FALSE;

	return pal_fs_chunk_IsChunkedFile(szPath);
}

Handle_t PAL_FS_CHUNK_Open (HCHAR *szPath, HCHAR *szMode)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
//TOOD;
	return 0;
#else
	HBOOL				 bIsDir = FALSE;
	Handle_t			 hChunk;
	HxDIR_t				*pstDir = NULL;
	struct 	stat			fstat;

	CHECK_INITIALIZED;

	pstDir = HLIB_DIR_Open (szPath);
	if (NULL == pstDir)
	{
		HxLOG_Error ("HLIB_DIR_Open error:\n");
		return HANDLE_NULL;
	}

	bIsDir = HLIB_DIR_IsDir (pstDir);
	HLIB_DIR_Close (pstDir);

	if (!bIsDir)
	{
		/* Check again */
		/* Sometimes, HLIB_DIR_IsDir() don't detect record directory */
		if (!lstat(szPath,&fstat))
		{
			bIsDir = S_ISDIR(fstat.st_mode);
			if (bIsDir == FALSE)
			{
				HxLOG_Error ("S_ISDIR error:\n");
				return HANDLE_NULL;
			}
		}
	}

	if (!bIsDir)
	{
		HxLOG_Error ("HLIB_DIR_IsDir error:\n");
		return HANDLE_NULL;
	}

	ENTER_FSCHUNK;
	hChunk = pal_fs_chunk_Open (szPath, szMode);
	LEAVE_FSCHUNK;

	return hChunk;
#endif /* CONIFG_PROD_OS_EMULATOR */	
}

HERROR PAL_FS_CHUNK_Close (Handle_t hChunk)
{
	HERROR			 hErr;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	hErr = pal_fs_chunk_Close (hChunk);
	LEAVE_FSCHUNK;

	return hErr;
}

HERROR PAL_FS_CHUNK_Read (Handle_t hChunk, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead)
{
	palfschunk_Inst_t	*pstInst;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	return pal_fs_chunk_Read (pstInst, pucBuf, ulSize, pulSizeRead);
}

HERROR PAL_FS_CHUNK_GetFileSize (Handle_t hChunk, HINT64 *pnSize)
{
	palfschunk_Inst_t	*pstInst;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	if (NULL != pnSize)				{ *pnSize = (HINT64)pstInst->ullTotalSize; }
	return ERR_OK;
}

HERROR PAL_FS_CHUNK_Seek (Handle_t hChunk, HINT64 nOffset, HULONG ulWhence, HINT64 *pnAbsOffset)
{
	HUINT64				 nAbsOffset;
	palfschunk_Inst_t	*pstInst;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	switch (ulWhence)
	{
	case PAL_FS_POS_CURRENT:
		nAbsOffset = (HINT64)pstInst->ullCurrPosition + nOffset;
		break;

	case PAL_FS_POS_END:
		nAbsOffset = (HINT64)pstInst->ullTotalSize - nOffset;
		break;

	case PAL_FS_POS_START:
	default:
		nAbsOffset = nOffset;
		break;
	}

	if (nAbsOffset < 0)
	{
		nAbsOffset = 0;
	}
	else if (nAbsOffset > pstInst->ullTotalSize)
	{
		nAbsOffset = pstInst->ullTotalSize;
	}

	pstInst->ullCurrPosition = (HUINT64)nAbsOffset;
	if (NULL != pnAbsOffset)					{ *pnAbsOffset = nAbsOffset; }

	return ERR_OK;
}

HERROR PAL_FS_CHUNK_Tell (Handle_t hChunk, HINT64 *pnAbsOffset)
{
	palfschunk_Inst_t	*pstInst;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	if (NULL != pnAbsOffset)					{ *pnAbsOffset = (HINT64)pstInst->ullCurrPosition; }
	return ERR_OK;
}

HBOOL PAL_FS_CHUNK_IsEof (Handle_t hChunk)
{
	palfschunk_Inst_t	*pstInst;

	CHECK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = pal_fs_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return FALSE;
	}

	return (pstInst->ullCurrPosition >= pstInst->ullTotalSize) ? TRUE : FALSE;
}


