
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <rxlib.h>



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


/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#undef	RxLOG_DOMAIN
#define	RxLOG_DOMAIN	"RxFILE"

#define	LOG				RxLOG_Print
#define	ERR_LOG			RxLOG_Error






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
STATIC	HOffset_t _rxfile_os_Seek(RxFile_t hSelf, HOffset_t offset, RxFILE_Seek_e origin)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return fseeko(hSelf, offset, origin);
	#else
	return fseek(hSelf, offset, origin);
	#endif
}

STATIC	HOffset_t _rxfile_os_Tell(RxFile_t hSelf)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return ftello(hSelf);
	#else
	return ftell(hSelf);
	#endif
}


#elif defined(OS_WIN32)

STATIC	HOffset_t _rxfile_os_Tell(RxFile_t hSelf)
{
	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	return _ftelli64(hSelf);
	#else
	return ftell(hSelf);
	#endif
}

STATIC	HOffset_t _rxfile_os_Seek(RxFile_t hSelf, HOffset_t offset, RxFILE_Seek_e origin)
{
	HINT32		nRet = 0;
	HOffset_t	ullOffset = 0;

	#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
	nRet = _fseeki64(hSelf, offset, origin);
	#else
	nRet = fseek(hSelf, offset, origin);
	#endif
	if(nRet < 0)
	{
		return nRet;
	}

	return _rxfile_os_Tell(hSelf);
}


#else
#error	??
#endif



/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


RxFile_t 	RxFile_Open(const HUINT8 *filename, const HUINT8 *mode)
{
	return (RxFile_t)fopen(filename, mode);
}

HINT32 			RxFile_Close(RxFile_t hSelf)
{
	return fclose(hSelf);
}

HUINT32		RxFile_Read(RxFile_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	return fread(buffer, size, count, hSelf);
}

HUINT32		RxFile_Write(RxFile_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	return fwrite(buffer, size, count, hSelf);
}

HERROR 			RxFile_Seek(RxFile_t hSelf, HUINT32 hi_offset, HUINT32 lo_offset, RxFILE_Seek_e origin, HUINT32 *pHi, HUINT32 *pLo)
{
	HOffset_t		pos = 0;
	HOffset_t		offset = 0;

	offset = (((HOffset_t)hi_offset << 32) | lo_offset);
	pos = _rxfile_os_Seek(hSelf, offset, origin);
	if(pos == -1)
	{
		return ERR_FAIL;
	}

	if(pHi)	*pHi = (pos >> 32);
	if(pLo) *pLo = (pos & 0xFFFFFFFF);

	return ERR_OK;
}

HERROR			RxFile_Tell(RxFile_t hSelf, HUINT32 *pHi, HUINT32 *pLo)
{
	HOffset_t	size = 0;

	size = _rxfile_os_Tell(hSelf);
	if(size == -1)
	{
		return ERR_FAIL;
	}

	if(pHi)	*pHi = (size >> 32);
	if(pLo) *pLo = (size & 0xFFFFFFFF);

	return ERR_OK;
}

HERROR	RxFile_Size(RxFile_t hSelf, HUINT32 *pHi, HUINT32 *pLo)
{
	HOffset_t 	pos = 0;
	HOffset_t	size = 0;
	HINT32 test;

	pos = _rxfile_os_Tell(hSelf);
	if(pos == -1)
	{
		return ERR_FAIL;
	}
	_rxfile_os_Seek(hSelf, 0, eRxFILE_SEEK_END);

	size = _rxfile_os_Tell(hSelf);
	if(size == -1)
	{
		return ERR_FAIL;
	}
	_rxfile_os_Seek(hSelf, pos,	eRxFILE_SEEK_SET);

	if(pHi)	*pHi = (HUINT32)(size >> 32);
	if(pLo) *pLo = (HUINT32)(size & 0xFFFFFFFF);

	return ERR_OK;
}


HINT32	RxFile_Flush(RxFile_t hSelf)
{
	return fflush(hSelf);
}

HINT32	RxFile_EOF(RxFile_t hSelf)
{
	return feof(hSelf);
}

HUINT32	RxFile_Error(RxFile_t hSelf)
{
	return ferror(hSelf);
}

HINT32 	RxFile_PutChar(RxFile_t hSelf, HUINT8 ch)
{
	return fputc(ch, hSelf);
}

HINT32	RxFile_GetChar(RxFile_t hSelf)
{
	return getc(hSelf);
}

HUINT8*	RxFile_GetString(RxFile_t hSelf, HUINT8 *string, HINT32 len)
{
	return fgets(string,  len, hSelf);
}

HERROR	RxFile_Make(const HUINT8 *pName)
{
	RxFile_t	hFile = 0;

	hFile = RxFile_Open(pName,"wb");
	if(!hFile)
	{
		return ERR_FAIL;
	}

	RxFile_Close(hFile);
	return ERR_OK;
}


HERROR	RxFile_Move(const HUINT8 *pOldName, const HUINT8 *pNewName)
{
	HINT32 ret = 0;

	ret = rename(pOldName, pNewName);
	if(ret < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	RxFile_Delete(const HUINT8 *pName)
{
	HINT32 ret = 0;

	ret = unlink(pName);
	if(ret < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	RxFile_Copy(const HUINT8 *pName1, const HUINT8 *pName2)
{
	// TODO:: 만들어야 함.
	RxLOG_Assert(0);
	return ERR_OK;
}



