/**************************************************************************************/
/**
 * @file pl_fs.c
 * defines APIs's body
 *
 * file operation
 *
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <htype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <linux/hdreg.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include <hlib.h>
#include "homma_interop.h"

#include "pl_fs.h"

#if defined(CONFIG_HOMMA_CDI)
#include "di_crypt.h"
#include "di_pvr.h"
#endif

#include <vkernel.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/* for ugly API */
#ifndef UNUSED
#define UNUSED(_x)		((void)(_x))
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef PATH_MAX
#define PATH_MAX			1024
#endif

/* see the linux/fs.h */
#define FILE_MODE		(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE		(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
#define ISSLASH(_c)		('/' == (_c))
#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

#define IS_DOT_HIDDEN(_x)		((_x[0]=='.') && (_x[1]!='\0'))
#define DOT_OR_DOTDOT_OR_HIDDEN(_basename)					\
	((_basename[0] == '.') &&					\
	 ((_basename[1] == '\0') || (_basename[1] == '.' && _basename[2] == '\0')|| (_basename[1] != '\0')))

#define MAX_SECURE_BUFFER 				(1024*1024*2)  // 2 MB
#define ALIGN_UNIT						(192)

#define CHECK_CRYPT_INITIALIZED 		do { if (TRUE != s_bFsCryptInited) __filesystem_crypt_Init(); } while (0)
#define CHECK_CHUNK_INITIALIZED 			do { if (TRUE != s_bFsChunkInited) __filesystem_chunk_Init(); } while (0)
#define ENTER_FSCHUNK				VK_SEM_Get (s_ullPlFsChunk_SemId)
#define LEAVE_FSCHUNK				VK_SEM_Release (s_ullPlFsChunk_SemId)

#if defined(CONFIG_HOMMA_CDI)
#define __USE_DI_CRYPT__	1
#define __USE_CHUNK_FS__	1
#else
#define __USE_DI_CRYPT__	0
#define __USE_CHUNK_FS__	1
#endif

#define	_FS_CRYPTO_KEY_LEN		16 /* DI_PVR_CRYPTO_KEY_LEN */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HOM_Handle_t		hChunk;
	HBOOL 				bSecure;
	HCHAR				szPath[PL_FS_MAX_NAME_LEN];

	HUINT32				ulChunkSize;
	HUINT32				ulChunkNum;
	HUINT64				ullTotalSize;
	HUINT64				ullCurrPosition;

	// File Open Cache용:
	HINT32				nOpenedChunkId;
	PL_FS_UINT32		ulOpenedChunkFile;
} PL_FS_CHUNK_Inst_t;

#if __USE_CHUNK_FS__
STATIC HBOOL			s_bFsChunkInited = FALSE;
STATIC HULONG			s_ullPlFsChunk_SemId;
STATIC HxList_t			*s_pstPlFsChunkInstList = NULL;
#endif

STATIC HBOOL s_bFsCryptInited = FALSE;
STATIC HUINT8 s_aucEncKey[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; /* _FS_CRYPTO_KEY_LEN */
#if __USE_DI_CRYPT__
STATIC HUINT8 s_aucEncIv[] = {0xad, 0xd6, 0x9e, 0xa3, 0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab, 0x3d, 0xbc, 0x7a, 0xf2}; /* _FS_CRYPTO_KEY_LEN */
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static char *	__filesystem_ConstStrdup (const char *str)
{
	HUINT32	len;
	char *	dup;

	len = HxSTD_StrLen(str);
	dup = (char *)HLIB_STD_MemAlloc(len + 1);
	if (dup)
	{
		if (len)
		{
			//HxSTD_MemSet(dup, str, len);
			HxSTD_MemSet(dup, 0x00, len);
		}

		dup[len] = '\0';
		HLIB_STD_StrRStrip(dup, "/");
	}
	return dup;
}

#ifdef CONFIG_DEBUG_FILEINFO
#include <sys/queue.h>

struct filedes_info {
	char *pathname;
	int filedes;
	int use_count;
	TAILQ_ENTRY(filedes_info) entries;
};
TAILQ_HEAD(listhead, filedes_info) head;

static pthread_mutex_t listlock = PTHREAD_MUTEX_INITIALIZER;
static volatile int opened;

static int __filesystem_debug_init(void)
{
	TAILQ_INIT(&head);
	return 0;
}

static int __filesystem_debug_add(const char *pathname, const int filedes)
{
	struct filedes_info *p;


	if (p = HLIB_STD_MemAlloc(sizeof(*p)), NULL == p)
	{
		HxLOG_Error("cannot HOMxFS_Malloc: %s\n", strerror(errno));
		return -1;
	}

	HxSTD_MemSet(p, 0, sizeof(*p));

	p->pathname = __filesystem_ConstStrdup(pathname);
	p->filedes = filedes;
	p->use_count++;

	pthread_mutex_lock(&listlock);
	TAILQ_INSERT_TAIL(&head, p, entries);
	opened++;
	pthread_mutex_unlock(&listlock);

	return 0;
}

static int __filesystem_debug_del(const int filedes)
{
	struct filedes_info *p;

	pthread_mutex_lock(&listlock);
	for (p = head.tqh_first; p != NULL; p = p->entries.tqe_next)
	{
		if (filedes == p->filedes)
		{
			TAILQ_REMOVE(&head, p, entries);
			if (p)
			{
				if (p->pathname)
				{
					HLIB_STD_MemFree(p->pathname);
					p->pathname = NULL;
				}

				HLIB_STD_MemFree(p);
				p = NULL;
			}
			opened--;
			pthread_mutex_unlock(&listlock);
			return 0;
		}
	}
	pthread_mutex_unlock(&listlock);
	HxLOG_Error("no entry for %i\n", filedes);

	return -1;
}
#else
#define __filesystem_debug_init(...)	((void)0)
#define __filesystem_debug_add(...)		((void)0)
#define __filesystem_debug_del(...)		((void)0)
#endif

/*
 * file operation
 */
int __filesystem_Open(const char *pathname, PL_FS_UINT32 flags, PL_FS_UINT32 *filedes)
{
	int fd = -1;
	int oflags = 0;
	mode_t mode = FILE_MODE;

	if (flags & PL_FS_MODE_READ)
	{
		oflags |= O_RDONLY;
	}
	else if (flags & PL_FS_MODE_WRITE)
	{
		oflags |= O_WRONLY;
		oflags |= O_CREAT;
	}
	else if (flags & PL_FS_MODE_RDWR)
	{
		oflags |= O_RDWR;
		oflags |= O_CREAT;
	}
	else
	{
		HxLOG_Error("invalid flags \"%s\": %li\n", pathname, flags);
		return ERR_FAIL;
	}

	if (flags & PL_FS_MODE_APPEND)		oflags |= O_APPEND;
	if (flags & PL_FS_MODE_TRUNC)		oflags |= O_TRUNC;

	if (flags & PL_FS_MODE_O_DIRECT)	oflags |= O_DIRECT;
	if (flags & PL_FS_MODE_O_LARGEFILE)	oflags |= O_LARGEFILE;

	if (flags & PL_FS_MODE_NONBLOCK)	oflags |= O_NONBLOCK;

	if (fd = open(pathname, oflags, mode), fd < 0)
	{
		*filedes = 0;
		HxLOG_Error("cannot open \"%s\": %s\n", pathname, strerror(errno));
		if (ENOENT == errno || ENOTDIR == errno || EACCES == errno)
		{
			return PL_FS_FOLDER_NOT_EXIST_ERR;
		}
		else if (EIO == errno)
		{
			return PL_FS_ERROR_IO;
		}
		else if (EROFS == errno)
		{
			return PL_FS_READ_ONLY_ERR;		// read-only filesystem
		}

		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*filedes = fd;
	__filesystem_debug_add(pathname, fd);

	return ERR_OK;
}

int __filesystem_Close(PL_FS_UINT32 filedes)
{
	__filesystem_debug_del((int)filedes);

	fsync((int)filedes);
	close((int)filedes);
	return ERR_OK;
}

int __filesystem_Read(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 size, PL_FS_UINT32 *sizep)
{
	ssize_t n;

	do {
		n = read((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n)
	{
		*sizep = 0;
		HxLOG_Error("cannot read: %s\n", strerror(errno));
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}

int __fileSystem_Write(PL_FS_UINT32 filedes, const PL_FS_UINT8 *buf, PL_FS_UINT32 size, PL_FS_UINT32 *sizep)
{
	ssize_t n;

	do {
		n = write((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n)
	{
		*sizep = 0;
		HxLOG_Error("cannot write: %s\n", strerror(errno));
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}

int __filesystem_LSeek(PL_FS_UINT32 filedes, PL_FS_INT64 offset, PL_FS_UINT32 pos, PL_FS_INT64 *offsetp)
{
	off_t	curr;
	int		whence;

	if (pos == PL_FS_POS_START)
	{
		whence = SEEK_SET;
	}
	else if (pos == PL_FS_POS_END)
	{
		whence = SEEK_END;
	}
	else
	{
		whence = SEEK_CUR;
	}

	if (curr = lseek((int)filedes, offset, whence), -1 == curr)
	{
		*offsetp = 0;
		HxLOG_Error("cannot seek: %s\n", strerror(errno));
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*offsetp = curr;
	return ERR_OK;
}

#if __USE_CHUNK_FS__
STATIC HERROR __filesystem_chunk_Init (void)
{
	HINT32			nRet;

	nRet = VK_SEM_Create ((HULONG *)&s_ullPlFsChunk_SemId, "ChunkSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != nRet)
	{
		HxLOG_Error ("VK_SEM_Create failed:\n");
		return ERR_FAIL;
	}

	s_bFsChunkInited = TRUE;
	return ERR_OK;
}

STATIC PL_FS_CHUNK_Inst_t *__filesystem_chunk_GetInstByChunkHandle (HOM_Handle_t hChunk)
{
	HxList_t				*pstListItem = NULL;

	if (0 != hChunk)
	{
		for (pstListItem = s_pstPlFsChunkInstList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			PL_FS_CHUNK_Inst_t *pstInst = (PL_FS_CHUNK_Inst_t *)HLIB_LIST_Data (pstListItem);
			if (hChunk == pstInst->hChunk)
			{
				return pstInst;
			}
		}
	}

	return NULL;
}

STATIC HOM_Handle_t __filesystem_chunk_AllocateUniqueChunkHandle (void)
{
	STATIC HOM_Handle_t		 s_hLastChunkHandle = 0x0001;
	HOM_Handle_t			 hChunk = s_hLastChunkHandle + 1;

	if (hChunk >= 0xFFFE)
	{
		hChunk = 0x0001;
	}

	while (hChunk != s_hLastChunkHandle)
	{
		PL_FS_CHUNK_Inst_t *pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
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

STATIC HBOOL __filesystem_chunk_CbCheckChunk (HxDIR_t *pstSelf, HCHAR *szPath, HCHAR *szName, HBOOL bIsDir, void *pvUserData)
{
	HBOOL				 bResult = FALSE;
	PL_FS_CHUNK_Inst_t	*pstInst = (PL_FS_CHUNK_Inst_t *)pvUserData;

	if ((TRUE != bIsDir) && (NULL != pstInst))
	{
		PL_FS_UINT32	 ulFileDes = 0;
		PL_FS_INT64	 nFileSize = 0;
		HINT32			 nRet;
		HCHAR			 szFullName[PL_FS_MAX_NAME_LEN];

		HxSTD_PrintToStrN (szFullName, PL_FS_MAX_NAME_LEN, "%s/%s", szPath, szName);

#if __USE_DI_CRYPT__
		if (pstInst->bSecure == TRUE)
		{
			nRet = PL_FS_Secure_Open ((const char *)szFullName, PL_FS_MODE_READ, &ulFileDes); /* needed secure_fs */
		}
		else
#endif
		{
			nRet = PL_FS_Open ((const char *)szFullName, PL_FS_MODE_READ, &ulFileDes);
		}

		if ((ERR_OK == nRet) && (0 != ulFileDes))
		{
#if __USE_DI_CRYPT__
			if (pstInst->bSecure == TRUE)
			{
				PL_FS_Secure_Seek (ulFileDes, 0L, PL_FS_POS_END);
				PL_FS_Secure_Tell (ulFileDes, &nFileSize);
			}
			else
#endif
			{
				PL_FS_Seek (ulFileDes, 0L, PL_FS_POS_END);
				PL_FS_Tell (ulFileDes, &nFileSize);
			}

			if (nFileSize > 16)
			{
				if (pstInst->ulChunkSize < (HUINT32)nFileSize)
				{
					pstInst->ulChunkSize = (HUINT32)nFileSize;
				}

				pstInst->ullTotalSize += (HUINT64)nFileSize;
				pstInst->ulChunkNum ++;
			}

#if __USE_DI_CRYPT__
			if (pstInst->bSecure == TRUE)
			{
				PL_FS_Secure_Close (ulFileDes);
			}
			else
#endif
			{
				PL_FS_Close (ulFileDes);
			}
		}

		bResult = (pstInst->ullTotalSize > 0) ? TRUE : FALSE;
	}

	return bResult;
}

STATIC HBOOL __filesystem_chunk_CbScanChunks (HxDIR_t *pstSelf, HCHAR *szPath, HCHAR *szName, HBOOL bIsDir, void *pvUserData)
{
	PL_FS_CHUNK_Inst_t	*pstInst = (PL_FS_CHUNK_Inst_t *)pvUserData;

	if ((TRUE != bIsDir) && (NULL != pstInst))
	{
		PL_FS_UINT32	 ulFileDes = 0;
		PL_FS_INT64	 nFileSize = 0;
		HINT32			 nRet;
		HCHAR			 szFullName[PL_FS_MAX_NAME_LEN];

		HxSTD_PrintToStrN (szFullName, PL_FS_MAX_NAME_LEN, "%s/%s", szPath, szName);
#if __USE_DI_CRYPT__
		if (pstInst->bSecure == TRUE)
		{
			nRet = PL_FS_Secure_Open ((const char *)szFullName, PL_FS_MODE_READ, &ulFileDes); /* needed secure_fs */
		}
		else
#endif
		{
			nRet = PL_FS_Open ((const char *)szFullName, PL_FS_MODE_READ, &ulFileDes);
		}

		if ((ERR_OK == nRet) && (0 != ulFileDes))
		{
#if __USE_DI_CRYPT__
			if (pstInst->bSecure == TRUE)
			{
				PL_FS_Secure_Seek (ulFileDes, 0L, PL_FS_POS_END);
				PL_FS_Secure_Tell (ulFileDes, &nFileSize);
			}
			else
#endif
			{
				PL_FS_Seek (ulFileDes, 0L, PL_FS_POS_END);
				PL_FS_Tell (ulFileDes, &nFileSize);
			}

			if (nFileSize > 16)
			{
				if (pstInst->ulChunkSize < (HUINT32)nFileSize)
				{
					pstInst->ulChunkSize = (HUINT32)nFileSize;
				}

				pstInst->ullTotalSize += (HUINT64)nFileSize;
				pstInst->ulChunkNum ++;
			}

#if __USE_DI_CRYPT__
			if (pstInst->bSecure == TRUE)
			{
				PL_FS_Secure_Close (ulFileDes);
			}
			else
#endif
			{
				PL_FS_Close (ulFileDes);
			}
		}
	}

	return FALSE;
}

STATIC HERROR __filesystem_chunk_ReadChunkBlocks (PL_FS_CHUNK_Inst_t *pstInst, HUINT32 ulChunkId, HUINT32 ulPos, HUINT32 ulReadSize, HUINT8 *pucBuf)
{
	PL_FS_UINT32		 ulSizeP;
	HINT32				nRet;
	PL_FS_UINT32		ulFileDes = 0;
	HCHAR				*szFileName = NULL;

	if ((pstInst->nOpenedChunkId >= 0) && (pstInst->nOpenedChunkId != ulChunkId))
	{
#if __USE_DI_CRYPT__
		if (pstInst->bSecure == TRUE)
		{
			PL_FS_Secure_Close (pstInst->ulOpenedChunkFile);
		}
		else
#endif
		{
			PL_FS_Close (pstInst->ulOpenedChunkFile);
		}

		pstInst->nOpenedChunkId = -1;
		pstInst->ulOpenedChunkFile = 0;
	}

	if (pstInst->nOpenedChunkId < 0)
	{
		szFileName = HLIB_STD_MemAlloc (PL_FS_MAX_NAME_LEN);
		if (NULL == szFileName)
		{
			HxLOG_Error ("HLIB_STD_MemAlloc failed\n");
			return ERR_FAIL;
		}

		HxSTD_PrintToStrN (szFileName, PL_FS_MAX_NAME_LEN, "%s/chunk.%d", pstInst->szPath, ulChunkId + 1);
#if __USE_DI_CRYPT__
		if (pstInst->bSecure == TRUE)
		{
			nRet = PL_FS_Secure_Open (szFileName, PL_FS_MODE_READ, &ulFileDes); /* needed secure_fs */
		}
		else
#endif
		{
			nRet = PL_FS_Open (szFileName, PL_FS_MODE_READ, &ulFileDes);
		}

		if ((ERR_OK != nRet) || (0 == ulFileDes))
		{
			HxLOG_Error ("Error> err:%d, desc:%d in PAL_FS_Open(%s)\n", nRet, ulFileDes, szFileName);
			if (szFileName != NULL)
			{
				HLIB_STD_MemFree(szFileName);
				szFileName = NULL;
			}
			return ERR_FAIL;
		}

		if (szFileName != NULL)
		{
			HLIB_STD_MemFree(szFileName);
			szFileName = NULL;
		}

		pstInst->nOpenedChunkId = (HINT32)ulChunkId;
		pstInst->ulOpenedChunkFile = ulFileDes;
	}

#if __USE_DI_CRYPT__
	if (pstInst->bSecure == TRUE)
	{
		PL_FS_Secure_Seek (pstInst->ulOpenedChunkFile, ulPos, PL_FS_POS_START);
		PL_FS_Secure_Read (pstInst->ulOpenedChunkFile, (PL_FS_UINT8 *)pucBuf, ulReadSize, &ulSizeP);
	}
	else
#endif
	{
		PL_FS_Seek (pstInst->ulOpenedChunkFile, ulPos, PL_FS_POS_START);
		PL_FS_Read (pstInst->ulOpenedChunkFile, (PL_FS_UINT8 *)pucBuf, ulReadSize, &ulSizeP);
	}

	return ERR_OK;
}

STATIC HBOOL __filesystem_chunk_IsChunkedFile (HCHAR *szPath)
{
	HCHAR				*szAllFileName = "*.*";
	PL_FS_CHUNK_Inst_t	*pstInst = NULL;

	pstInst = (PL_FS_CHUNK_Inst_t *)HLIB_STD_MemCalloc(sizeof(PL_FS_CHUNK_Inst_t));
	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance allocation failed:\n");
		return FALSE;
	}

	HLIB_DIR_Find (szPath, szAllFileName, "", __filesystem_chunk_CbCheckChunk, (void *)pstInst);

	if ((0 == pstInst->ulChunkNum) || (0 == pstInst->ulChunkSize) || (0L == pstInst->ullTotalSize))
	{
		HxLOG_Error ("Error> No proper chunked file inside:\n");
		HLIB_STD_MemFree (pstInst);
		pstInst = NULL;
		return FALSE;
	}

	HLIB_STD_MemFree (pstInst);
	pstInst = NULL;
	return TRUE;
}

STATIC HOM_Handle_t __filesystem_chunk_Open (HCHAR *szPath, HCHAR *szMode, HBOOL bSecure)
{
	HOM_Handle_t			 hChunk = __filesystem_chunk_AllocateUniqueChunkHandle();
	HCHAR				*szAllFileName = "*.*";
	PL_FS_CHUNK_Inst_t	*pstInst = NULL;

	if (HANDLE_NULL == hChunk)
	{
		HxLOG_Error ("Error> AllocateUniqueChunkHandle failed:\n");
		goto ERROR;
	}

	pstInst = (PL_FS_CHUNK_Inst_t *)HLIB_STD_MemCalloc (sizeof(PL_FS_CHUNK_Inst_t));
	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Instance allocation failed:\n");
		goto ERROR;
	}

	pstInst->hChunk			= hChunk;
	pstInst->bSecure		= bSecure;
	pstInst->ulChunkNum		= 0;
	pstInst->nOpenedChunkId	= -1;

	HxSTD_snprintf(pstInst->szPath, sizeof(pstInst->szPath), "%s", szPath);

	HLIB_DIR_Find (szPath, szAllFileName, "", __filesystem_chunk_CbScanChunks, (void *)pstInst);

	if ((0 == pstInst->ulChunkNum) || (0 == pstInst->ulChunkSize) || (0L == pstInst->ullTotalSize))
	{
		HxLOG_Error ("Error> No proper chunked file inside:\n");
		goto ERROR;
	}

	s_pstPlFsChunkInstList = HLIB_LIST_Append (s_pstPlFsChunkInstList, (void *)pstInst);
	return hChunk;

ERROR:
	if (NULL != pstInst)
	{
		HLIB_STD_MemFree (pstInst);
		pstInst = NULL;
	}

	return HANDLE_NULL;
}

STATIC HERROR __filesystem_chunk_Close (HOM_Handle_t hChunkFile)
{
	PL_FS_CHUNK_Inst_t	*pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunkFile);

	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	s_pstPlFsChunkInstList = HLIB_LIST_Remove (s_pstPlFsChunkInstList, (void *)pstInst);

	if (pstInst->nOpenedChunkId >= 0)
	{
#if __USE_DI_CRYPT__
		if (pstInst->bSecure)
		{
			PL_FS_Secure_Close (pstInst->ulOpenedChunkFile); /* needed secure_fs */
		}
		else
#endif
		{
			PL_FS_Close (pstInst->ulOpenedChunkFile);
		}

		pstInst->nOpenedChunkId = -1;
		pstInst->ulOpenedChunkFile = 0;
	}

	HLIB_STD_MemFree(pstInst);
	pstInst = NULL;
	return ERR_OK;
}

STATIC HERROR __filesystem_chunk_Read (PL_FS_CHUNK_Inst_t *pstInst, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead)
{
	HUINT32			ulReadSize;
	HUINT32			ulStartChunkId, ulStartChunkPos;
	HUINT32			ulEndChunkId, ulEndChunkPos;
	HINT64			nEndPos;
	HUINT32 		ulChunkId;

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
		__filesystem_chunk_ReadChunkBlocks (pstInst, ulStartChunkId, ulStartChunkPos, ulReadSize, pucBuf);
	}
	else
	{
		// First Chunk:
		ulReadSize = (pstInst->ulChunkSize - ulStartChunkPos);
		__filesystem_chunk_ReadChunkBlocks (pstInst, ulStartChunkId, ulStartChunkPos, ulReadSize, pucBuf);
		pucBuf += ulReadSize;

		// Middle Chunks:
		for (ulChunkId = ulStartChunkId + 1; ulChunkId < ulEndChunkId; ulChunkId++)
		{
			__filesystem_chunk_ReadChunkBlocks (pstInst, ulChunkId, 0, pstInst->ulChunkSize, pucBuf);
			pucBuf += pstInst->ulChunkSize;
			ulReadSize += pstInst->ulChunkSize;
		}

		// Last Chunk:
		__filesystem_chunk_ReadChunkBlocks (pstInst, ulEndChunkId, 0, ulEndChunkPos + 1, pucBuf);
		ulReadSize += (ulEndChunkPos + 1);
	}

	pstInst->ullCurrPosition += (HUINT64)ulReadSize;
	if (NULL != pulSizeRead)
	{
		*pulSizeRead = ulReadSize;
	}

	return ERR_OK;
}
#endif /* __USE_CHUNK_FS__ */


#ifdef CONFIG_DEBUG_FILEINFO
void PL_FS_OpenFileList(void)
{
	struct filedes_info *p;
	int num = 0;

	pthread_mutex_lock(&listlock);
	HxLOG_Print("+Total: %i\n", opened);
	if (opened > 0) {
		HxLOG_Print("index count filedes pathname\n");
		for (p = head.tqh_first; p != NULL; p = p->entries.tqe_next) {
			HxLOG_Print("%5i %5i %7i %s\n", num, p->use_count, p->filedes, p->pathname);
			num++;
		}
	}
	pthread_mutex_unlock(&listlock);
}
#else
void PL_FS_OpenFileList(void)
{
	HxLOG_Print("set the CONFIG_DEBUG_FILEINFO\n");
}
#endif

static inline char *PL_FS_Fullpath(const char *s1, const char *s2);

STATIC HUINT32	__filesystem_HexToInt(const HCHAR *s)
{
	HUINT32		val = 0;
	HINT32		x = 0;

	if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

	while(s[x]!='\0')
	{
		if((s[x] == 'X') || (s[x] == 'x'))
		{
			x++;
			continue;
		}
		else if(s[x] >= '0' && s[x] <='9')
		{
		  val = (val << 4) + s[x] - '0';
		}
		else if(s[x]>='A' && s[x] <='F')
		{
		  val = (val << 4) + s[x] - 'A' + 10;
		}
		else if(s[x]>='a' && s[x] <='f')
		{
		  val = (val << 4) + s[x] - 'a' + 10;
		}
		else return val;

	   x++;
	}
	return val;
}


STATIC HUINT32	__filesystem_StrToUINT32 (const HCHAR *s)
{
	HUINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return __filesystem_HexToInt(s);
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n;
}

/* __USE_DI_CRYPT__ 여부와 상관없이 key setting */
STATIC void __filesystem_crypt_Init(void)
{
	HERROR hErr = 0;
	HCHAR	szChipId[32]; /* OAPI_SYSINFO_NAME_LEN */
	HUINT32 uChipId = 0;

	/* DI_SYSTEM_GetChipId() */
	hErr = HOMMA_INTEROP_SYSINFO_STB_GetChipId(szChipId, 32);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error> Cant get chipID!\n");
		return;
	}

	/* uChipId = __filesystem_StrToUINT32(szChipId); */
	uChipId = __filesystem_HexToInt(szChipId);
	if (uChipId == 0x0)
	{
		HxLOG_Error("Error> invalid chipID(%s)\n", szChipId);
		return;
	}

	HxSTD_MemSet (s_aucEncKey, 0, _FS_CRYPTO_KEY_LEN);
	HxSTD_MemCopy (s_aucEncKey, (char *)&uChipId, 4);

#if defined(CONFIG_DEBUG)
	{
		int i;
		HxLOG_Info(" Key value: ");
		for( i=0; i<16; i++)
		{
			HxLOG_Info(" %02X", s_aucEncKey[i]);
		}
		HxLOG_Info("\n");
	}
#endif

	s_bFsCryptInited = TRUE;
	return;
}

/*
 * initialize opreation
 */
int PL_FS_Init(void)
{
	__filesystem_debug_init();
	__filesystem_crypt_Init();

	return ERR_OK;
}

/*
 * file operation
 */
int	PL_FS_Open(const char *pathname, PL_FS_UINT32 flags, PL_FS_UINT32 *filedes)
{
	int		nRet = ERR_OK;

	nRet = __filesystem_Open(pathname, flags, filedes);

	return nRet;
}

int PL_FS_Close(PL_FS_UINT32 filedes)
{
	int		nRet = ERR_OK;

	nRet = __filesystem_Close(filedes);

	return nRet;
}

int PL_FS_Read(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 size, PL_FS_UINT32 *sizep)
{
	int		nRet = ERR_OK;

	nRet = __filesystem_Read(filedes, buf, size, sizep);

	return nRet;
}

int PL_FS_Write(PL_FS_UINT32 filedes, const PL_FS_UINT8 *buf, PL_FS_UINT32 size, PL_FS_UINT32 *sizep)
{
	int		nRet = ERR_OK;

	nRet = __fileSystem_Write(filedes, buf, size, sizep);

	return nRet;
}

int PL_FS_LSeek(PL_FS_UINT32 filedes, PL_FS_INT64 offset, PL_FS_UINT32 pos, PL_FS_INT64 *offsetp)
{
	int		nRet = ERR_OK;

	nRet = __filesystem_LSeek(filedes, offset, pos, offsetp);

	return nRet;
}

int PL_FS_Seek(PL_FS_UINT32 filedes, PL_FS_INT64 offset, PL_FS_UINT32 whence)
{
	int		nRet = ERR_OK;
	PL_FS_INT64 curr;

	nRet = __filesystem_LSeek(filedes, offset, whence, &curr);

	return nRet;
}

int PL_FS_Tell (PL_FS_UINT32 filedes, PL_FS_INT64 *pnOffset)
{
	PL_FS_INT64	 nCurr;
	int				 nErr;

	if (pnOffset == NULL)
	{
		return ERR_FAIL;
	}

	nErr = __filesystem_LSeek(filedes, 0L, PL_FS_POS_CURRENT, &nCurr);
	if (nErr == ERR_OK)
	{
		*pnOffset = nCurr;
	}

	return nErr;
}

int PL_FS_IsEOF(PL_FS_UINT32  filedes)
{
	char buf[8];
	ssize_t n;

	n = read((int)filedes, buf, 1);
	if( n <= 0 )
		return 1;

	(void)lseek( filedes, -1, SEEK_CUR );

	return 0;
}

int PL_FS_GetFileSize(PL_FS_UINT32 filedes, PL_FS_INT64 *pulSize)
{
	struct stat st;

	if( pulSize == NULL )
		return ERR_FAIL;

	if (-1 == fstat(filedes, &st))
	{
		HxLOG_Error("cannot fstat: %s\n", strerror(errno));
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*pulSize = (PL_FS_INT64)st.st_size;

	return ERR_OK;
}

int PL_FS_GetFileInfo(const char *pathname, PL_FS_UINT32 type, PL_FS_FileStat_t *filestatp)
{
	struct stat st;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		filestatp->fileType = PL_FS_FILE_TYPE_UNKNOWN;
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	switch (st.st_mode & S_IFMT) {
	case S_IFREG:
		filestatp->fileType = PL_FS_FILE_TYPE_FILE;
		break;
	case S_IFDIR:
		filestatp->fileType = PL_FS_FILE_TYPE_DIR;
		break;
	case S_IFIFO:
		filestatp->fileType = PL_FS_FILE_TYPE_FIFO;
		break;
	case S_IFLNK:
		filestatp->fileType = PL_FS_FILE_TYPE_LINK;
		break;
	default:
		filestatp->fileType = PL_FS_FILE_TYPE_UNKNOWN;
		break;
	}

	/*
	 * when the named file is symbolic link,
	 * return size about the symbolic link,
	 * not the file referenced by the symbolic link.
	 */
	filestatp->ulFileSize = st.st_size;

	filestatp->aTime = st.st_atime;		/* time of last access */
	filestatp->mTime = st.st_mtime;		/* time of last modification */
	filestatp->cTime = st.st_ctime;		/* time of last change */

	/* FIXME PAL_FS_INFO_XXX and FM_FS_LISTTYPE_XXX are not value
	 * for bitwise operation */
	if (PL_FS_INFO_TIME == type) {
		struct timeval tv;
		struct timezone tz;
		int h, m, c;
		gettimeofday(&tv, &tz);
		h = tz.tz_minuteswest / 60;
		m = ((tz.tz_minuteswest / 15) % 4) * 15;
		c = ((60 * 60) * h) + (60 * m);

		filestatp->aTime = st.st_atime + c;
		filestatp->mTime = st.st_mtime + c;
		filestatp->cTime = st.st_ctime + c;
	}

	return ERR_OK;
}

int PL_FS_Rename(int pttype, const char *oldname, const char *newname)
{
	UNUSED(pttype);
	if (-1 == rename(oldname, newname))
	{
		HxLOG_Error("cannot rename \"%s\" to \"%s\": %s",
		    oldname, newname, strerror(errno));
		return ERR_FAIL;
	}
	return ERR_OK;
}

/*
 * directory operation
 */
int PL_FS_OpenDir(int pttype, const char *dirname, PL_FS_UINT32 *dirp)
{
	DIR *dp;

	UNUSED(pttype);

	if (dp = opendir(dirname), NULL == dp)
	{
		*dirp = 0;
		HxLOG_Error("cannot opendir \"%s\": %s\n", dirname, strerror(errno));
		return ERR_FAIL;
	}

	*dirp = (PL_FS_UINT32)dp;
	return ERR_OK;
}

int PL_FS_CloseDir(PL_FS_UINT32 dirp)
{
	closedir((DIR *)dirp);
	return ERR_OK;
}

int PL_FS_ReadDir(PL_FS_UINT32 dirp, PL_FS_DirEntry_t *entry)
{
	struct dirent const *dirent;

	if (dirent = readdir((DIR *)dirp), NULL != dirent)
	{
		if (!DOT_OR_DOTDOT(dirent->d_name))
		{
			HxSTD_StrNCpy((char *)entry->aucName, dirent->d_name, NAME_MAX);
			entry->aucName[NAME_MAX-1] = '\0';
			entry->bFlagDir = 0;
			entry->ulFileSize = 0;
		}
	}
	else
		return ERR_FAIL;

	return ERR_OK;
}

int PL_FS_ChangeDir(const char *dirname)
{
	if (chdir(dirname) < 0)
	{
		HxLOG_Error("cannot chdir \"%s\": %s\n", dirname, strerror(errno));
		return PL_FS_ERROR_INVALID_VALUE;
	}
	return ERR_OK;
}

int PL_FS_CountFile(const char *pathname, char *filterstrarray[], unsigned int *file_num)
{
	struct		stat st;
	int			i = 0;

	if(pathname == NULL)
		return ERR_FAIL;

	if ( -1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	if (S_ISDIR(st.st_mode))
	{
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)
			return 0;

		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			PL_FS_CountFile(fullpath, filterstrarray, file_num);

			HLIB_STD_MemFree(fullpath);
			fullpath = NULL;
		}

		closedir(dp);
	}
	else
	{
		// check file's extension, and count
		while (filterstrarray[i] != NULL)
		{
			if (filterstrarray[i] && NULL != strstr(pathname, filterstrarray[i]))
			{
				(*file_num)++;
				break;
			}
			i++;
		}
	}

	return ERR_OK;
}

int PL_FS_FindFile(const char *pathname, char *filterstrarray[], unsigned int *file_num, unsigned int *dir_num)
{
	struct stat st;
	int i = 0, num_file = 0, num_dir = 0;

	if(pathname == NULL)
		return ERR_FAIL;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	if (S_ISDIR(st.st_mode))
	{
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)		// empty directory
			return 0;

		while ((dirent = readdir(dp)) != NULL)
		{
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			i = 0;

			if (lstat(fullpath, &st) == 0)
			{

				/* count # of directories */
				if (S_ISDIR(st.st_mode))
					num_dir++;

				/* count # of files having specific extention */
				if (S_ISREG(st.st_mode))
				{
					while (filterstrarray[i] != NULL)
					{
						if (filterstrarray[i] && NULL != strstr(fullpath, filterstrarray[i]))
						{
							num_file++;
							break;
						}
						i++;
					}
				}
			}

			HLIB_STD_MemFree(fullpath);
			fullpath = NULL;
		}

		closedir(dp);
	}

	*file_num = num_file;
	*dir_num = num_dir;

	return ERR_OK;
}

int PL_FS_GetNumOfDirList(int pttype, const char *dirname, PL_FS_UINT32 *num)
{
	DIR *dp;
	struct dirent *dirent;
	int pos = 0;

	UNUSED(pttype);

	if (dp = opendir(dirname), NULL == dp) {
		*num = 0;
		return ERR_FAIL;
	}

	while ((dirent = readdir(dp)) != NULL)
	{
		if (!DOT_OR_DOTDOT(dirent->d_name))
			pos++;
	}

	closedir(dp);

	*num = pos;
	return ERR_OK;
}

/*
 * copy and remove operation
 */
static inline char *PL_FS_Fullpath(const char *s1, const char *s2)
{
	char *fullpath, *p;
	size_t len;

	len = strlen(s1) + strlen(s2) + 2;

	if (fullpath = HLIB_STD_MemAlloc(len), NULL == fullpath)
		return NULL;

	HxSTD_StrNCpy(fullpath, s1, strlen(s1));

	p = fullpath + HxSTD_StrLen(s1) - 1;
	while (ISSLASH(*p))
		p--;
	*++p = '/';
	*++p = '\0';

	HxSTD_StrNCpy(p, s2, strlen(s2));

	return fullpath;
}

#if __USE_DI_CRYPT__
/*
 * deprecated function
 */
int PL_FS_Secure_Open(const char *pathname, PL_FS_UINT32 flags, PL_FS_UINT32 *filedes)
{
	int nRet = 0;

	/* 부팅 시 __filesystem_crypt_Init()에서 에러 발생할 수 있으므로, 여기에서 재확인한다 */
	CHECK_CRYPT_INITIALIZED;

	nRet = __filesystem_Open(pathname, flags, filedes );

	return nRet;
}

int PL_FS_Secure_Close(PL_FS_UINT32 filedes)
{
	int nRet = 0;

	nRet = __filesystem_Close( filedes );

	return nRet;
}

int PL_FS_Secure_Read(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 readsize, PL_FS_UINT32 *sizep)
{
	int				nRet = 0;
	DI_ERR_CODE diError = DI_ERR_OK;
	PL_FS_UINT8		*pucSecureSrc=NULL, *pucSecureDst=NULL;
	stCrypt_t		stCrypt;
	HUINT32			ulBufSize = 0, ulHeadSize=0, ulTailSize = 0;
	PL_FS_UINT32	ulReadSize = 0;
	PL_FS_INT64		ulCurPosition = 0;
	PL_FS_INT64		nOffset = 0, nCur=0;

	if( (buf == NULL) || (sizep == NULL) )
	{
		return PL_FS_BAD_PARAMETER_ERR;
	}

	if( readsize > MAX_SECURE_BUFFER )
	{
		HxLOG_Error("MAX_SECURE_BUFFER size is over (%d, %d)\n", (int)readsize, (int)MAX_SECURE_BUFFER);
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	// make 192 byte align
	PL_FS_Secure_Tell( filedes, &ulCurPosition );
	ulHeadSize = ulCurPosition % ALIGN_UNIT;
	ulTailSize = (ulCurPosition+readsize) % ALIGN_UNIT;
	if( ulTailSize != 0 )
	{
		ulTailSize = (ALIGN_UNIT- ulTailSize);
	}

	ulBufSize = ulHeadSize + readsize + ulTailSize;

//	HxLOG_Print("[PAL_FS_Secure_Read] ulCur:%lld, ulHeadSize:%d, size:%d, ulTailSize:%d, ulBufSize:%d\r\n",
//			ulCurPosition, ulHeadSize, size, ulTailSize, ulBufSize);

	pucSecureSrc = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureSrc == NULL )
	{
		HxLOG_Error("Error> Out of memory (%d)\n", (int)ulBufSize);
		nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
		goto func_error;
	}

	pucSecureDst = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureDst == NULL )
	{
		HxLOG_Error("Error> Out of memory (%d)\n", (int)ulBufSize);
		nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
		goto func_error;
	}

	if( ulHeadSize != 0 )
	{
		nOffset = ulCurPosition - ulHeadSize;
		nRet = __filesystem_LSeek(filedes, nOffset, PL_FS_POS_START, &nCur);
		if( nRet != ERR_OK )
		{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulHeadSize);
			nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
			goto func_error;
		}

//		HxLOG_Print("  ulHeadSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulHeadSize, nOffset, nCur);
	}

	nRet = __filesystem_Read( filedes, pucSecureSrc, ulBufSize, &ulReadSize );
	if( (nRet != ERR_OK) || (ulReadSize == 0))
	{
		HxLOG_Error("error : PAL_FS_Read nRet:0x%X, size:%d, ulReadSize:%d\n", nRet, (int)ulBufSize, (int)ulReadSize);
		nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
		goto func_error;
	}

	if( ulTailSize != 0 )
	{
		nOffset= ulCurPosition+readsize;
		nRet = __filesystem_LSeek(filedes, nOffset, PL_FS_POS_START, &nCur);
		if( nRet != ERR_OK )
		{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulTailSize);
			nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
			goto func_error;
		}
//		HxLOG_Print("  ulTailSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulTailSize, nOffset, nCur);
	}

	HxSTD_MemSet( &stCrypt, 0, sizeof(stCrypt_t) );

	stCrypt.etCryptMode = DI_CRYPT_AES;	/* Changed PVR encryption with AES from TDES */
	stCrypt.etCryptKey = DI_CRYPT_USER_KEY;
	stCrypt.etCryptData = DI_CRYPT_STREAM;
	stCrypt.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;  // 16 byte
	stCrypt.pSrcBuffer =  pucSecureSrc;
	stCrypt.pDstBuffer = pucSecureDst;
	stCrypt.ulBuffSize = ulBufSize;
	stCrypt.pKey = s_aucEncKey;
	stCrypt.pIvKey = s_aucEncIv;  /* Added IV */

	diError = DI_CRYPT_Decrypt( &stCrypt );
	if( diError != DI_ERR_OK )
	{
		HxLOG_Error("error DI_CRYPT_Decrypt: 0x%X\n", diError);
		if (pucSecureSrc != NULL)
		{
			DI_CRYPT_MemFree( (void *)pucSecureSrc);
			pucSecureSrc = NULL;
		}

		if (pucSecureDst != NULL)
		{
			DI_CRYPT_MemFree( (void *)pucSecureDst);
			pucSecureDst = NULL;
		}
		return ERR_FAIL;
	}

	VK_memcpy32( buf, pucSecureDst+ulHeadSize, readsize );
	if( ulReadSize < readsize )
	{
		*sizep = ulReadSize;
	}
	else
	{
		*sizep= readsize;
	}

func_error:
	if (pucSecureSrc != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureSrc);
		pucSecureSrc = NULL;
	}

	if (pucSecureDst != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureDst);
		pucSecureDst = NULL;
	}

	return nRet;
}

int PL_FS_Secure_ReadWithKey(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 readsize, PL_FS_UINT32 *sizep, PL_FS_UINT8 *keydata, PL_FS_UINT32 keysize)
{
	int				nRet = 0;
	DI_ERR_CODE diError = DI_ERR_OK;
	PL_FS_UINT8		*pucSecureSrc=NULL, *pucSecureDst=NULL;
	stCrypt_t		stCrypt;
	HUINT32			ulBufSize = 0, ulHeadSize=0, ulTailSize = 0;
	PL_FS_UINT32	ulReadSize = 0;
	PL_FS_INT64		ulCurPosition = 0;
	PL_FS_INT64		nOffset = 0, nCur=0;

	if( (buf == NULL) || (sizep == NULL) )
	{
		return PL_FS_BAD_PARAMETER_ERR;
	}

	if( readsize > MAX_SECURE_BUFFER )
	{
		HxLOG_Error("MAX_SECURE_BUFFER size is over (%d, %d)\n", (int)readsize, (int)MAX_SECURE_BUFFER);
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	// make 192 byte align
	PL_FS_Secure_Tell( filedes, &ulCurPosition );
	ulHeadSize = ulCurPosition % ALIGN_UNIT;
	ulTailSize = (ulCurPosition+readsize) % ALIGN_UNIT;
	if( ulTailSize != 0 )
	{
		ulTailSize = (ALIGN_UNIT- ulTailSize);
	}

	ulBufSize = ulHeadSize + readsize + ulTailSize;

//	HxLOG_Print("[PAL_FS_Secure_Read] ulCur:%lld, ulHeadSize:%d, size:%d, ulTailSize:%d, ulBufSize:%d\r\n",
//			ulCurPosition, ulHeadSize, size, ulTailSize, ulBufSize);

	pucSecureSrc = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureSrc == NULL )
	{
		HxLOG_Error("Error> Out of memory (%d)\n", (int)ulBufSize);
		nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
		goto func_error;
	}

	pucSecureDst = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureDst == NULL )
	{
		HxLOG_Error("Error> Out of memory (%d)\n", (int)ulBufSize);
		nRet = PL_FS_ERROR_NO_MORE_RESOURCES;
		goto func_error;
	}

	if( ulHeadSize != 0 )
	{
		nOffset = ulCurPosition - ulHeadSize;
		nRet = __filesystem_LSeek(filedes, nOffset, PL_FS_POS_START, &nCur);
		if( nRet != ERR_OK )
		{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulHeadSize);
			return PL_FS_ERROR_NO_MORE_RESOURCES;
		}

//		HxLOG_Print("  ulHeadSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulHeadSize, nOffset, nCur);
	}

	nRet = __filesystem_Read( filedes, pucSecureSrc, ulBufSize, &ulReadSize );
	if( (nRet != ERR_OK) || (ulReadSize == 0))
	{
		HxLOG_Error("error : PAL_FS_Read nRet:0x%X, size:%d, ulReadSize:%d\n", nRet, (int)ulBufSize, (int)ulReadSize);
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	if( ulTailSize != 0 )
	{
		nOffset= ulCurPosition+readsize;
		nRet = __filesystem_LSeek(filedes, nOffset, PL_FS_POS_START, &nCur);
		if( nRet != ERR_OK )
		{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulTailSize);
			return PL_FS_ERROR_NO_MORE_RESOURCES;
		}
//		HxLOG_Print("  ulTailSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulTailSize, nOffset, nCur);
	}

	HxSTD_MemSet( &stCrypt, 0, sizeof(stCrypt_t) );

	stCrypt.etCryptMode = DI_CRYPT_AES;	/* Changed w/ AES, from TDES */
	stCrypt.etCryptKey = DI_CRYPT_USER_KEY;
	stCrypt.etCryptData = DI_CRYPT_STREAM;
	stCrypt.ulKeySize = keysize;  // 16 byte
	stCrypt.pSrcBuffer =  pucSecureSrc;
	stCrypt.pDstBuffer = pucSecureDst;
	stCrypt.ulBuffSize = ulBufSize;
	stCrypt.pKey = keydata;
	stCrypt.pIvKey = s_aucEncIv; /* Added IV */

	diError = DI_CRYPT_Decrypt( &stCrypt );
	if( diError != DI_ERR_OK )
	{
		HxLOG_Error("error DI_CRYPT_Decrypt: 0x%X\n", diError);
		if (pucSecureSrc != NULL)
		{
			DI_CRYPT_MemFree( (void *)pucSecureSrc);
			pucSecureSrc = NULL;
		}

		if (pucSecureDst != NULL)
		{
			DI_CRYPT_MemFree( (void *)pucSecureDst);
			pucSecureDst = NULL;
		}
		return ERR_FAIL;
	}

	VK_memcpy32( buf, pucSecureDst+ulHeadSize, readsize );
	if( ulReadSize < readsize )
	{
		*sizep = ulReadSize;
	}
	else
	{
		*sizep= readsize;
	}

func_error:
	if (pucSecureSrc != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureSrc);
		pucSecureSrc = NULL;
	}

	if (pucSecureDst != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureDst);
		pucSecureDst = NULL;
	}

	return nRet;
}

int PL_FS_Secure_Seek(PL_FS_UINT32 filedes, PL_FS_INT64 offset, PL_FS_UINT32 whence)
{
	int nRet = 0;
	PL_FS_INT64 curr;

	nRet = PL_FS_LSeek( filedes, offset, whence, &curr );
	if(nRet != ERR_OK)
	{
		curr = 0;
	}

	return (int)curr;
}

int PL_FS_Secure_Tell(PL_FS_UINT32 filedes, PL_FS_INT64 *poffset)
{
	if( poffset == NULL )
		return ERR_FAIL;

	*poffset = (PL_FS_INT64) lseek( filedes, 0, SEEK_CUR );

	return ERR_OK;
}

int PL_FS_Secure_IsEOF(PL_FS_UINT32  filedes)
{
	char buf[8];
	ssize_t n;

	n = read((int)filedes, buf, 1);
	if( n <= 0 )
		return 1;

	(void)lseek( filedes, -1, SEEK_CUR );

	return 0;
}

int PL_FS_Secure_GetFileSize(PL_FS_UINT32 filedes, PL_FS_INT64 *pulSize)
{
	struct stat st;

	if( pulSize == NULL )
		return ERR_FAIL;

	if (-1 == fstat(filedes, &st))
	{
		HxLOG_Error("cannot fstat: %s\n", strerror(errno));
		return PL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*pulSize = (PL_FS_INT64)st.st_size;

	return ERR_OK;
}
#endif /* __USE_DI_CRYPT__ */

PL_FS_BOOL PL_FS_IsFileExist(const char *pathname)
{
	struct stat st;
	return (0 == stat(pathname, &st) && S_ISREG(st.st_mode)) ? TRUE : FALSE;
}

PL_FS_BOOL PL_FS_IsDirExist(const char *dirname)
{
	struct stat st;
	return (0 == stat(dirname, &st) && S_ISDIR(st.st_mode)) ? TRUE : FALSE;
}

int PL_FS_Mkdir(const char *dirname)
{
	HCHAR *odirname, *p;

	odirname = p = __filesystem_ConstStrdup(dirname);

	if(odirname == NULL)
		return ERR_FAIL;

	do {
		p = strchr(p + 1, '/');
		if (p)
			*p = '\0';

			if (access(odirname, F_OK) == -1)
			{
				if (mkdir(odirname, 0755) == -1)
				{
					HxLOG_Error("fail to make a %s. %s\n", odirname, strerror(errno));
					HLIB_STD_MemFree(odirname);
					odirname = NULL;
					return ERR_FAIL;
				}
			}
			if (p)
					*p = '/';
	} while (p);
	HxLOG_Debug("mkdir:%s\n", odirname);


	HLIB_STD_MemFree(odirname);
	odirname = NULL;
	return ERR_OK;
}

#if __USE_CHUNK_FS__

HBOOL PL_FS_CHUNK_IsChunkedFile (HCHAR *szPath)
{
//	HBOOL				 bIsDir = FALSE;
//	HOM_Handle_t			 hChunk;
//	HxDIR_t				*pstDir = NULL;
//	struct 	stat			fstat;

	CHECK_CHUNK_INITIALIZED;

	if (PL_FS_IsDirExist(szPath) == FALSE)
	{
		return FALSE;
	}

	return __filesystem_chunk_IsChunkedFile(szPath);
}

HOM_Handle_t PL_FS_CHUNK_Open (HCHAR *szPath, HCHAR *szMode, HBOOL bSecure)
{
	HBOOL				 bIsDir = FALSE;
	HOM_Handle_t			 hChunk;
	HxDIR_t				*pstDir = NULL;
	struct 	stat			fstat;

	CHECK_CHUNK_INITIALIZED;

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
	hChunk = __filesystem_chunk_Open (szPath, szMode, bSecure);
	LEAVE_FSCHUNK;

	return hChunk;
}

HERROR PL_FS_CHUNK_Close (HOM_Handle_t hChunk)
{
	HERROR			 hErr;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	hErr = __filesystem_chunk_Close (hChunk);
	LEAVE_FSCHUNK;

	return hErr;
}

HERROR PL_FS_CHUNK_Read (HOM_Handle_t hChunk, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead)
{
	PL_FS_CHUNK_Inst_t	*pstInst;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	return __filesystem_chunk_Read (pstInst, pucBuf, ulSize, pulSizeRead);
}

HERROR PL_FS_CHUNK_GetFileSize (HOM_Handle_t hChunk, HINT64 *pnSize)
{
	PL_FS_CHUNK_Inst_t	*pstInst;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	if (NULL != pnSize) { *pnSize = (HINT64)pstInst->ullTotalSize; }
	return ERR_OK;
}

HERROR PL_FS_CHUNK_Seek (HOM_Handle_t hChunk, HINT64 nOffset, HULONG ulWhence, HINT64 *pnAbsOffset)
{
	HUINT64				 nAbsOffset;
	PL_FS_CHUNK_Inst_t	*pstInst;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	switch (ulWhence)
	{
	case PL_FS_POS_CURRENT:
		nAbsOffset = (HINT64)pstInst->ullCurrPosition + nOffset;
		break;

	case PL_FS_POS_END:
		nAbsOffset = (HINT64)pstInst->ullTotalSize - nOffset;
		break;

	case PL_FS_POS_START:
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
	if (NULL != pnAbsOffset) { *pnAbsOffset = nAbsOffset; }

	return ERR_OK;
}

HERROR PL_FS_CHUNK_Tell (HOM_Handle_t hChunk, HINT64 *pnAbsOffset)
{
	PL_FS_CHUNK_Inst_t	*pstInst;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return ERR_FAIL;
	}

	if (NULL != pnAbsOffset) { *pnAbsOffset = (HINT64)pstInst->ullCurrPosition; }
	return ERR_OK;
}

HBOOL PL_FS_CHUNK_IsEof (HOM_Handle_t hChunk)
{
	PL_FS_CHUNK_Inst_t	*pstInst;

	CHECK_CHUNK_INITIALIZED;

	ENTER_FSCHUNK;
	pstInst = __filesystem_chunk_GetInstByChunkHandle (hChunk);
	LEAVE_FSCHUNK;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Error> Invalid Chunk Handle(0x%08x): cannot find the instance\n", hChunk);
		return FALSE;
	}

	return (pstInst->ullCurrPosition >= pstInst->ullTotalSize) ? TRUE : FALSE;
}
#endif /* __USE_CHUNK_FS__ */

