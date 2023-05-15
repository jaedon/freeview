/*************************************************************
 * @file		hxmem.h
 * @date		2011/08/25
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, fixed size blocks allocation memory
 * http://www.humaxdigital.com
 *************************************************************/

#define	____HxMEM_README___________________

/******************************************************************************
 [Example Code]

int main (int argc, char *argv[])
{


	return 0;
}


 [Expected Result]

 ************************************************************************************/

#ifndef __HxMEM_H__
#define __HxMEM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>

#include "htype.h"

enum
{
	HxMEM_GROUP_HLIB		= 0,
	HxMEM_GROUP_BASE,
	HxMEM_GROUP_MAX
};

// Do NOT use these function directly!!
HCHAR *	__HLIB_MEM_StrDup (const HCHAR *str);
void *	__HLIB_MEM_MemDup (const void *mem, HUINT32 size);
void *	__HLIB_MEM_Calloc (HUINT32 nmemb, HUINT32 size);

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/

#if defined(CONFIG_DEBUG) && !defined(WIN32)
	#include	<memory_debug.h>

	#define	HLIB_MEM_Malloc(size)			MEMDBG_Malloc(size, HxMEM_GROUP_HLIB, __FILE__, __LINE__)
	#define	HLIB_MEM_Calloc(size)			MEMDBG_Calloc(size, sizeof(char), HxMEM_GROUP_HLIB, __FILE__, __LINE__)
	#define	HLIB_MEM_Realloc(mem,size)		MEMDBG_Realloc(mem, size, HxMEM_GROUP_HLIB, __FILE__, __LINE__)
	#define	HLIB_MEM_StrDup(str)			MEMDBG_StrDup(str, HxMEM_GROUP_HLIB, __FILE__, __LINE__)
	#define	HLIB_MEM_MemDup(mem,size)		MEMDBG_MemDup(mem, size, HxMEM_GROUP_HLIB, __FILE__, __LINE__)
	#define	HLIB_MEM_Free(mem)				MEMDBG_Free(mem, __FILE__, __LINE__)
	static void*	HLIB_MEM_Malloc_CB(HUINT32 size)
	{
		return MEMDBG_Malloc(size, HxMEM_GROUP_HLIB, __BASE_FILE__, -1);
	}
	static void		HLIB_MEM_Free_CB(void *p)
	{
		MEMDBG_Free(p, __BASE_FILE__, -1);
	}
	static HCHAR*	HLIB_MEM_StrDup_CB(const HCHAR *str)
	{
		if (str)
		{
			return MEMDBG_StrDup(str, HxMEM_GROUP_HLIB, __BASE_FILE__, -1);
		}
		return NULL;
	}

	#define	HLIB_STD_MemAlloc(size)			MEMDBG_Malloc(size, HxMEM_GROUP_BASE, __FILE__, __LINE__)
	#define	HLIB_STD_MemCalloc(size)		MEMDBG_Calloc(size, sizeof(char), HxMEM_GROUP_BASE, __FILE__, __LINE__)
	#define	HLIB_STD_MemRealloc(mem,size)	MEMDBG_Realloc(mem, size, HxMEM_GROUP_BASE, __FILE__, __LINE__)
	#define	HLIB_STD_StrDup(str)			MEMDBG_StrDup(str, HxMEM_GROUP_BASE, __FILE__, __LINE__)
	#define	HLIB_STD_MemDup(mem,size)		MEMDBG_MemDup(mem, size, HxMEM_GROUP_BASE, __FILE__, __LINE__)
	#define	HLIB_STD_MemFree(mem)			MEMDBG_Free(mem, __FILE__, __LINE__)
	static void*	HLIB_STD_MemAlloc_CB(HUINT32 size)
	{
		return MEMDBG_Malloc(size, HxMEM_GROUP_BASE, __BASE_FILE__, -1);
	}
	static void		HLIB_STD_MemFree_CB(void *p)
	{
		MEMDBG_Free(p, __BASE_FILE__, -1);
	}
	static HCHAR*	HLIB_STD_StrDup_CB(const HCHAR *str)
	{
		if (str)
		{
			return MEMDBG_StrDup(str, HxMEM_GROUP_BASE, __BASE_FILE__, -1);
		}
		return NULL;
	}

	#define	DS_MEM_Malloc(size, handle, orgFile, orgLine)			MEMDBG_HLIB_Malloc(size, handle, orgFile, orgLine, __FILE__, __LINE__)
	#define	DS_MEM_Calloc(size, handle, orgFile, orgLine)			MEMDBG_HLIB_Calloc(size, sizeof(char), handle, orgFile, orgLine, __FILE__, __LINE__)
	#define	DS_MEM_Realloc(mem,size, handle, orgFile, orgLine)		MEMDBG_HLIB_Realloc(mem, size, handle, orgFile, orgLine, __FILE__, __LINE__)
	#define	DS_MEM_StrDup(str, handle, orgFile, orgLine)			MEMDBG_HLIB_StrDup(str, handle, orgFile, orgLine, __FILE__, __LINE__)
	#define	DS_MEM_MemDup(mem,size, handle, orgFile, orgLine)		MEMDBG_HLIB_MemDup(mem, size, handle, orgFile, orgLine, __FILE__, __LINE__)
	#define	DS_MEM_Free(mem, handle, orgFile, orgLine)				MEMDBG_HLIB_Free(mem, handle, orgFile, orgLine, __FILE__, __LINE__)
#else
	#define	HLIB_MEM_Malloc(size)			malloc(size)
	#define	HLIB_MEM_Calloc(size)			__HLIB_MEM_Calloc(size, sizeof(char))
	#define	HLIB_MEM_Realloc(mem,size)		realloc(mem, size)
	#define	HLIB_MEM_StrDup(str)			__HLIB_MEM_StrDup(str)
	#define	HLIB_MEM_MemDup(mem,size)		__HLIB_MEM_MemDup(mem, size)
	#define	HLIB_MEM_Free(mem)				free(mem)
	#define	HLIB_MEM_Malloc_CB				malloc
	#define	HLIB_MEM_Free_CB				free
	#define	HLIB_MEM_StrDup_CB				__HLIB_MEM_StrDup

	#define	HLIB_STD_MemAlloc(size)			malloc(size)
	#define	HLIB_STD_MemCalloc(size)		__HLIB_MEM_Calloc(size, sizeof(char))
	#define	HLIB_STD_MemRealloc(mem,size)	realloc(mem, size)
	#define	HLIB_STD_StrDup(str)			__HLIB_MEM_StrDup(str)
	#define	HLIB_STD_MemDup(mem,size)		__HLIB_MEM_MemDup(mem, size)
	#define	HLIB_STD_MemFree(mem)			free(mem)
	#define	HLIB_STD_MemAlloc_CB			malloc
	#define	HLIB_STD_MemFree_CB				free
	#define	HLIB_STD_StrDup_CB				__HLIB_MEM_StrDup

	#define DS_MEM_Malloc(size, handle, orgFile, orgLine)			malloc(size)
	#define DS_MEM_Calloc(size, handle, orgFile, orgLine)			__HLIB_MEM_Calloc(size, sizeof(char))
	#define DS_MEM_Realloc(mem,size, handle, orgFile, orgLine)	realloc(mem, size)
	#define DS_MEM_StrDup(str, handle, orgFile, orgLine)			__HLIB_MEM_StrDup(str)
	#define DS_MEM_MemDup(mem,size, handle, orgFile, orgLine)		__HLIB_MEM_MemDup(mem, size)
	#define DS_MEM_Free(mem, handle, orgFile, orgLine)				free(mem)


extern void		MEMDBG_Summary (HINT32 group, HUINT32 fromSec, HUINT32 durSec, HCHAR *szThreadName, HUINT32 ulMinSize, HUINT32 ulMaxSize, const HCHAR *filter, FILE *fp);
extern void		MEMDBG_GetBackTrace (HINT32 group, void *allocPtr, void *pvPrintCbFunc);
extern void		MEMDBG_LogTaskMemUsage (HCHAR *szTaskName, FILE *fp);
extern HUINT32	MEMDBG_GetUsedMemory(void);
extern void		MEMDBG_CheckMem(HINT32 group, FILE *fp);

extern	void	MEMDBG_HLIB_CONF(HINT32 nStartSec, HINT32 nDurationSec, HERROR(* memdbg_print)(const HCHAR *format, ...));
extern HERROR	MEMDBG_HLIB_Print(const HCHAR *opt, const HCHAR *opt2, void * num);

#endif

#endif /* __HxMEM_H__ */

