
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>


#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef OS_WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#endif

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxFILE"

#define	LOG				HxLOG_Print
#define	ERR_LOG		HxLOG_Error

#define	FILE_MODE_READ_ONLY		"rb"
#define	FILE_MODE_WRITE_ONLY		"wb"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

#if defined(OS_LINUX)
STATIC	HOffset_t _hxfile_os_Seek(HxFILE_t hSelf, HOffset_t offset, HxFILE_Seek_e origin)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return fseeko(hSelf, offset, origin);
	#else
	return fseek(hSelf, offset, origin);
	#endif
}

STATIC	HOffset_t _hxfile_os_Tell(HxFILE_t hSelf)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return ftello(hSelf);
	#else
	return ftell(hSelf);
	#endif
}


#elif defined(OS_WIN32)

STATIC	HOffset_t _hxfile_os_Tell(HxFILE_t hSelf)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return _ftelli64(hSelf);
	#else
	return ftell(hSelf);
	#endif
}

STATIC	HOffset_t _hxfile_os_Seek(HxFILE_t hSelf, HOffset_t offset, HxFILE_Seek_e origin)
{
	HINT32		nRet = 0;

#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	nRet = _fseeki64(hSelf, offset, origin);
#else
	nRet = fseek(hSelf, offset, origin);
#endif
	if(nRet < 0)
	{
		return nRet;
	}

	return _hxfile_os_Tell(hSelf);
}

#else
#error	??
#endif


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

HBOOL HLIB_FILE_Exist(const HUINT8 *pName)
{

#if defined(OS_LINUX)
	if ( access(pName, F_OK) == -1 )
		return FALSE;
	else
		return TRUE;
#endif
#if defined(OS_WIN32)
	HINT32 fd;

	fd = fopen(pName, "r");
	if(0 < fd)
	{
		fclose(fd);
		return TRUE;
	}
	return FALSE;
#endif
}

/*
mode :
	   r      Open text file for reading.  The stream is positioned at the beginning of the file.
       r+     Open for reading and writing.  The stream is positioned at the beginning of the file.
       w      Truncate file to zero length or create text file for writing.  The stream is positioned at the beginning of the file.
       w+     Open for reading and writing.  The file is created if it does not exist, otherwise it is truncated.  The stream is positioned at the beginning of the file.
       a      Open for appending (writing at end of file).  The file is created if it does not exist.  The stream is positioned at the end of the file.
       a+     Open  for  reading and appending (writing at end of file).  The file is created if it does not exist.  The initial file position for reading is at the beginning of the file, but output is always appended to the end of
              the file.
*/
HxFILE_t 	HLIB_FILE_Open(const HUINT8 *filename, const HUINT8 *mode)
{
	return (HxFILE_t)fopen(filename, mode);
}

HxFILE_t 	HLIB_FILE_OpenByIndex(const HUINT8 *filename, const HUINT8 *mode)
{
	FILE 	*pFile = 0;
	FILE 	*pJumpFile = 0;
	HINT32	nIndex = 0;
	HCHAR	*pszDefName = NULL;
	HCHAR	*pExtName = NULL;
	HCHAR	*pToken = NULL;
	HCHAR	szIndexName[256] = {0, };

	pszDefName = strdup(filename);
	if(NULL == pszDefName)	return NULL;

	pToken = HxSTD_StrRChar(pszDefName, '.');
	if(NULL != pToken)
	{
		pExtName = strdup(pToken);
		*pToken = 0;
	}

	if(strlen(pszDefName) > 220) pszDefName[220] = 0;

	nIndex = 0;
	while(TRUE)
	{
		snprintf(szIndexName, 255, "%s_%06d%s", pszDefName, nIndex, (NULL==pExtName)? "" : pExtName);

		pFile = fopen(szIndexName, "r");
		if(NULL == pFile)
		{
			break;
		}
		else
		{
			snprintf(szIndexName, 255, "%s_%06d%s", pszDefName, (nIndex+10), (NULL==pExtName)? "" : pExtName);

			pJumpFile = fopen(szIndexName, "r");
			if(NULL != pJumpFile)
			{
				nIndex+=10;
				fclose(pJumpFile);
			}
			else
			{
				nIndex++;
			}
			fclose(pFile);
		}
	}

	if(NULL != pszDefName)
	{
		free(pszDefName);
	}

	if(NULL != pExtName)
	{
		free(pExtName);
	}

	return (HxFILE_t)fopen(szIndexName, mode);
}


HINT32 			HLIB_FILE_Close(HxFILE_t hSelf)
{
	return fclose(hSelf);
}

HUINT32		HLIB_FILE_Read(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	return fread(buffer, size, count, hSelf);
}

HUINT32		HLIB_FILE_Write(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	HUINT32 nWritten = 0;

	nWritten = fwrite(buffer, size, count, hSelf);
	fflush(hSelf);
	return nWritten;
}

HUINT32		HLIB_FILE_WriteWithoutFlush(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	HUINT32 nWritten = 0;

	nWritten = fwrite(buffer, size, count, hSelf);
	return nWritten;
}

#if 0 // 사용성이 떨어져서 수정
HERROR 			HLIB_FILE_Seek(HxFILE_t hSelf, HUINT32 hi_offset, HUINT32 lo_offset, HxFILE_Seek_e origin, HUINT32 *pHi, HUINT32 *pLo)
{
	HOffset_t		pos = 0;
	HOffset_t		offset = 0;

	offset = hi_offset;
	offset = (offset << 32) | lo_offset;
	pos = _hxfile_os_Seek(hSelf, offset, origin);
	if(pos == -1)
	{
		return ERR_FAIL;
	}

	if(pHi)	*pHi = (pos >> 32);
	if(pLo) *pLo = (pos & 0xFFFFFFFF);

	return ERR_OK;
}
#endif

HERROR 			HLIB_FILE_Seek(HxFILE_t hSelf, HOffset_t offset, HxFILE_Seek_e origin, HOffset_t *ret_pos)
{
	HOffset_t		pos = 0;

	pos = _hxfile_os_Seek(hSelf, offset, origin);
	if(pos == -1)
	{
		return ERR_FAIL;
	}

	if(ret_pos)
	{
		*ret_pos = pos;
	}

	return ERR_OK;
}

HERROR			HLIB_FILE_Tell(HxFILE_t hSelf, HOffset_t *ret_pos)
{
	HOffset_t	pos = 0;

	pos = _hxfile_os_Tell(hSelf);
	if(pos == -1)
	{
		return ERR_FAIL;
	}

	if(ret_pos)	*ret_pos = pos;

	return ERR_OK;
}

HERROR	HLIB_FILE_Size(HxFILE_t hSelf, HUINT64 *ret_size)
{
	HOffset_t 	pos = 0;
	HUINT64		size = 0;

	pos = _hxfile_os_Tell(hSelf);
	if(pos == -1)
	{
		return ERR_FAIL;
	}
	(void)_hxfile_os_Seek(hSelf, 0, eHxFILE_SEEK_END);

	size = _hxfile_os_Tell(hSelf);
	if(size == -1)
	{
		return ERR_FAIL;
	}
	(void)_hxfile_os_Seek(hSelf, pos,	eHxFILE_SEEK_SET);

	if(ret_size) *ret_size = size;

	return ERR_OK;
}

HERROR	HLIB_FILE_Size32(HxFILE_t hSelf, HUINT32 *ret_size)
{
	HOffset_t 	pos = 0;
	HUINT64		size = 0;

	pos = _hxfile_os_Tell(hSelf);
	if(pos == -1)
	{
		return ERR_FAIL;
	}
	(void)_hxfile_os_Seek(hSelf, 0, eHxFILE_SEEK_END);

	size = _hxfile_os_Tell(hSelf);
	if(size == -1)
	{
		return ERR_FAIL;
	}
	(void)_hxfile_os_Seek(hSelf, pos,	eHxFILE_SEEK_SET);

	if(ret_size) *ret_size = (HUINT32)(size & 0xFFFFFFFF);

	return ERR_OK;
}

HERROR HLIB_FILE_SizeFromPath(const HUINT8 *path, HUINT64 *ret_size)
{
#if defined(OS_WIN32)
	LARGE_INTEGER	nLargeInteger = { 0 } ;
	HANDLE			hFile;

	if(NULL == path || NULL == ret_size)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		if(GetFileSizeEx(hFile, &nLargeInteger))
		{
			*ret_size = (HUINT64)nLargeInteger.QuadPart;

			CloseHandle(hFile);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
#else
    struct stat fstat;

	if (stat(path, &fstat) != 0)
		return ERR_FAIL;
    *ret_size = fstat.st_size;
	return ERR_OK;
#endif
}

HINT32	HLIB_FILE_Flush(HxFILE_t hSelf)
{
	return fflush(hSelf);
}

HINT32	HLIB_FILE_EOF(HxFILE_t hSelf)
{
	return feof(hSelf);
}

HUINT32	HLIB_FILE_Error(HxFILE_t hSelf)
{
	return ferror(hSelf);
}

HINT32 	HLIB_FILE_PutChar(HxFILE_t hSelf, HUINT8 ch)
{
	return fputc(ch, hSelf);
}

HINT32	HLIB_FILE_GetChar(HxFILE_t hSelf)
{
	return getc(hSelf);
}

HUINT8*	HLIB_FILE_GetString(HxFILE_t hSelf, const HUINT8 *string, HINT32 len)
{
	return fgets((char *)string,  len, hSelf);
}

HERROR	HLIB_FILE_Make(const HUINT8 *pName)
{
	HxFILE_t	hFile = 0;

	hFile = HLIB_FILE_Open(pName, FILE_MODE_WRITE_ONLY);
	if(!hFile)
	{
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);
	return ERR_OK;
}

HERROR	HLIB_FILE_Move(const HUINT8 *pOldName, const HUINT8 *pNewName)
{
	HINT32 ret = 0;

	ret = rename(pOldName, pNewName);
	if(ret < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_FILE_Delete(const HUINT8 *pName)
{
	HINT32 ret = 0;

	ret = remove(pName);
	if(ret < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_FILE_WriteFile(const HUINT8 *pName, void *pvData, HINT32 len)
{
	HxFILE_t	file;

	file = HLIB_FILE_Open(pName, FILE_MODE_WRITE_ONLY);
	if (file == NULL)
		return ERR_FAIL;

	HLIB_FILE_Write(file, pvData, 1, len);
	HLIB_FILE_Close(file);

	return ERR_OK;
}

HERROR	HLIB_FILE_WriteFileWithLock(const HUINT8 *pName, void *pvData, HINT32 len)
{
#if	1
	return HLIB_FILE_WriteFile(pName, pvData, len);
#else
	HINT32		fd;
	HxFILE_t	file;
	struct flock	fileLock;

	file = HLIB_FILE_Open(pName, FILE_MODE_WRITE_ONLY);
	if (file == NULL)
		return ERR_FAIL;

	fileLock.l_type = F_WRLCK;
	fileLock.l_whence = SEEK_SET;
	fileLock.l_start = 0;
	fileLock.l_len = 0;
	fd = fileno(file);
	fcntl(fd, F_SETLKW, &fileLock);

	HLIB_FILE_Write(file, pvData, 1, len);
	HLIB_FILE_Close(file);

	return ERR_OK;
#endif
}

void	HLIB_FILE_FreeBuffer(void *p)
{
	HLIB_MEM_Free(p);
}

HERROR	HLIB_FILE_Touch(const HCHAR *pszFileName)
{
	HxFILE_t	file;

	file = HLIB_FILE_Open(pszFileName, FILE_MODE_WRITE_ONLY);
	if (file)
	{
		HLIB_FILE_Close(file);
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR	HLIB_FILE_ReadFile(const HUINT8 *pName, void **ppvData, HUINT32 *pulLen)
{
	HUINT8	*pucData = NULL;
	HUINT32	ulLen;
	HxFILE_t	file;
	HERROR	err = ERR_OK;

	file = HLIB_FILE_Open(pName, FILE_MODE_READ_ONLY);
	if (file == NULL)
		return ERR_FAIL;

	if (HLIB_FILE_Size32(file, &ulLen) == ERR_OK)
	{
		if (ulLen > 0)
		{
			pucData = (HUINT8*)HLIB_MEM_Calloc(ulLen);
			(void)HLIB_FILE_Read(file, pucData, 1, ulLen);
		} else
		{
			*ppvData = NULL;
			*pulLen  = 0;
			err = ERR_FAIL;
		}
	}

	HLIB_FILE_Close(file);

	*ppvData = pucData;
	*pulLen = ulLen;

	return err;
}

HERROR	HLIB_FILE_Copy(const HUINT8 *pSrcName, const HUINT8 *pDstName)
{
#if defined(OS_WIN32)
	HBOOL bResult;

	bResult = CopyFile(pSrcName, pDstName, FALSE);
	return (bResult) ? ERR_OK : ERR_FAIL;

#elif defined(OS_LINUX)
	HINT32	nSrcHandle, nDstHandle;
//	FILE	*in, *out;
	char	*buf;
	ssize_t	len;

	if (HxSTD_StrCmp(pSrcName, pDstName) == 0)
	{
		return ERR_FAIL;
	}

	if ((nSrcHandle = open(pSrcName, O_RDONLY)) < 0)
	{
		HxLOG_Error("fail to read... [%s]\n", pSrcName);
		return ERR_FAIL;
	}

	if ((nDstHandle = open(pDstName, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
	{
		HxLOG_Error("fail to read... [%s]\n", pDstName);
		close(nSrcHandle);
		return ERR_FAIL;
	}

	buf = (HUINT8*)HLIB_MEM_Malloc(16*1024);
	if (buf == NULL)
	{
		close(nSrcHandle);
		close(nDstHandle);
		return ERR_FAIL;
	}

	while ((len = read(nSrcHandle, buf,  16*1024)) > 0)
	{
		if (write(nDstHandle, buf, len) <= 0)
		{
			HxLOG_Error("fail to write... [%s]\n", pSrcName);
			close(nSrcHandle);
			close(nDstHandle);
			HLIB_MEM_Free(buf);
			unlink(pDstName);
			return ERR_FAIL;
		}
		fsync(nDstHandle);
	}

	close(nSrcHandle);
	close(nDstHandle);

	HLIB_MEM_Free(buf);

	return ERR_OK;
#endif
}

HERROR	HLIB_FILE_IsValid(const HCHAR *name)
{
#if defined(OS_WIN32)
	return ERR_OK;

#elif defined(OS_LINUX)
	struct stat fstat;

	if (stat(name, &fstat) == 0)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
#endif
}

HERROR	HLIB_FILE_CopyWithLock(const HCHAR *pName1, const HCHAR *pName2)
{
#if	1
	return HLIB_FILE_Copy(pName1, pName2);
#else
	FILE	*in, *out;
	HINT32	inFd, outFd;
	char	*buf;
	size_t	len;
	struct flock	readFileLock, writeFileLock;

	if (HxSTD_StrCmp(pName1, pName2) == 0)
		return ERR_FAIL;

	if ((in  = fopen(pName1, FILE_MODE_READ_ONLY)) == NULL)
		return ERR_FAIL;
	if ((out = fopen(pName2, FILE_MODE_WRITE_ONLY)) == NULL)
	{
		fclose(in);
		return ERR_FAIL;
	}

	readFileLock.l_type = F_RDLCK;
	readFileLock.l_whence = SEEK_SET;
	readFileLock.l_start = 0;
	readFileLock.l_len = 0;
	inFd = fileno(in);
	fcntl(inFd, F_SETLKW, &readFileLock);

	writeFileLock.l_type = F_WRLCK;
	writeFileLock.l_whence = SEEK_SET;
	writeFileLock.l_start = 0;
	writeFileLock.l_len = 0;
	outFd = fileno(out);
	fcntl(outFd, F_SETLKW, &writeFileLock);

	buf = (HUINT8*)HLIB_MEM_Malloc(1024);
	if (buf == NULL)
	{
		fclose(in);
		fclose(out);
		return ERR_FAIL;
	}

	while ((len = fread(buf, sizeof(HUINT8), 1024, in)) != 0)
	{
		if (fwrite(buf, sizeof(HUINT8), len, out) == 0)
		{
			fclose(in);
			fclose(out);
			HLIB_STD_MemFree(buf);
			unlink(pName2);
			return ERR_FAIL;
		}
		fflush(out);
	}

	fclose(in);
	fclose(out);
	HLIB_STD_MemFree(buf);

	return ERR_OK;
#endif
}

#if 0
void *	HLIB_FILE_Dump (const HCHAR *name, HUINT32 *size)
{
	struct stat	fstat;
	FILE		*fp;
	void		*dump;

	if (stat(name, &fstat) != 0)
		return NULL;

	dump = HLIB_MEM_Malloc(fstat.st_size);
	if (dump == NULL)
		return NULL;

	fp = fopen(name, "rb");
	if (fp == NULL)
	{
		HLIB_MEM_Free(dump);
		return NULL;
	}

	if (fread(dump, 1, fstat.st_size, fp) == fstat.st_size)
	{
		HLIB_MEM_Free(dump);
		dump = NULL;
	}
	fclose(fp);
}
#endif

